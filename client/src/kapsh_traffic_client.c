#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#include "networking.h"


#define MAX_NUMBERS 10
#define HOST "127.0.0.1"
#define PORT 8080

typedef struct factorizationResult
{
    char *number;
    char result[1024];
} FactorizationResult;

typedef struct threadArgs
{
    int threadProcessId;
    char *host;
    int port;
    char *strNumber;
} ThreadArgs;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

FactorizationResult results[MAX_NUMBERS];
int resultCount = 0;
int nextResultIndex = 0;

void print_usage(const char *prog_name)
{
    printf("Usage: %s [--host <host>] [--port <port>] [data_0, data_1, ..., data_n]\n", prog_name);
}

void *printResult(void *);
void *requestHandling(void *);


/**
 * Main function.
 * 
 * This function handles the command line arguments and
 * initializes the program. It then creates threads for each
 * number to be factored and waits for them to finish.
 * The user can also configure de server IP and port using
 * --host and --port options.
 
 * @param argc: The number of command line arguments.
 * @param argv: The array of command line arguments.

 * @return: int.
 * 
 */
int main(int argc, char **argv)
{
    char **numbers;
    int lenNums;
    char *host = NULL;
    int port = -1;

    static struct option long_options[] = {
        {"host", required_argument, 0, 'h'},
        {"port", required_argument, 0, 'p'},
        {"help", 0, 0, '?'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "h:p:", long_options, &option_index)) != -1)
    {
        switch (opt) {
            case 'h':
                host = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case '?':
                print_usage(argv[0]);
                return 1;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    if (host) 
    {
        printf("Host: %s\n", host);
    } else
    {
        printf("Host: not specified. Default host 127.0.0.1 will be used \n");
        host = HOST;
    }

    if (port >= 0) {
        printf("Port: %d\n", port);
    } else {
        printf("Port: not specified. Default port 8080 will be used \n");
        port = PORT;
    }

    if (optind < argc)
    {
        lenNums = argc - optind;
        if (lenNums > MAX_NUMBERS)
        {
            printf("You can send up to 10 numbers. The list will be truncated\n");
            lenNums = MAX_NUMBERS;
        }
        
        numbers = malloc(sizeof(char *) * lenNums);
        if (numbers == NULL)
        {
            printf("Failed to allocate memory for numbers\n");
            return 1;
        }

        for (int i = 0; i < lenNums; i++)
        {
            numbers[i] = strdup(argv[optind + i]);
            if (!numbers[i])
            {
                printf("Failed to allocate memory for number\n");
                for (int j = 0; j < i; j++)
                {
                    free(numbers[j]);
                }
                free(numbers);
                return 1;
            }
        }
    } else
    {
        print_usage(argv[0]);
        return 1;
    }

    pthread_t threadProcesses[lenNums], threadResult;
    ThreadArgs *threadArgsArray[lenNums];

    if (pthread_create(&threadResult, NULL, printResult, NULL) != 0)
    {
        printf("Error on creating print system\n");
        return 1;
    }

    for (int i = 0; i < lenNums; ++i)
    {
        threadArgsArray[i] = malloc(sizeof(ThreadArgs));
        if (!threadArgsArray[i])
        {
            printf("Request arguments couldn't be allocated\n");
            for (int j = 0; j < i; j++)
            {
                free(threadArgsArray[j]);
            }
            free(numbers);
            return 1;
        }
        else
        {
            threadArgsArray[i]->threadProcessId = i;
            threadArgsArray[i]->host = host;
            threadArgsArray[i]->port = port;
            threadArgsArray[i]->strNumber = numbers[i];
            if (pthread_create(&threadProcesses[i], NULL, requestHandling, (void *)threadArgsArray[i]) != 0)
            {
                printf("Error on creating factorization request for number %s\n", numbers[i]);
                free(threadArgsArray[i]);
                for (int j = 0; j < i; j++)
                {
                    free(threadArgsArray[j]);
                }
                free(numbers);
                return 1;
            }
        }
    }

    for (int i = 0; i < lenNums; ++i)
    {
        pthread_join(threadProcesses[i], NULL);
    }

    while (nextResultIndex != resultCount)
        sleep(.1);  // Reduce CPU usage on waiting...

    pthread_mutex_lock(&mutex);

    resultCount = -1;

    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    pthread_join(threadResult, NULL);

    for (int i = 0; i < lenNums; i++)
    {
        free(numbers[i]);
    }
    free(numbers);

    printf("Program finished!\n");

    return 0;
}


/**
 * Prints the factorization results.
 *
 * This function runs in a separate thread and continuously prints the factorization
 * results in order as FIFO. It uses a mutex and a condition variable to synchronize
 * access to the results array. The function terminates when the resultCount variable
 * is set to -1.
 *
 * @param args: a pointer to a void that is not used in the function

 * @return: void.
 */
void *printResult(void *args)
{
    do
    {
        pthread_mutex_lock(&mutex);

        while (resultCount == 0)
        {
            pthread_cond_wait(&cond, &mutex);
        }

        if (resultCount == -1)
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        if (strlen(results[nextResultIndex].result) != 0)
        {
            printf("Factors of %s: %s\n", results[nextResultIndex].number, results[nextResultIndex].result);
            nextResultIndex++;
        }
        pthread_mutex_unlock(&mutex);

    } while(1);

    pthread_exit(NULL);
}


/**
 * This function handles the request to the server.
 *
 * It receives the input arguments from the main function, connects to the server,
 * sends the number to factorize, receives the result, and saves it to the results
 * array. It uses a mutex and a condition variable to synchronize the access to the
 * results array. It terminates when the resultCount variable is set to -1.
 *
 * @param args: a pointer to a ThreadArgs structure containing the input arguments

 * @return: void.
 */
void *requestHandling(void *args)
{
    ThreadArgs *threadArgsData = (ThreadArgs *)args;

    int threadProcessId = threadArgsData->threadProcessId;
    char *host = threadArgsData->host;
    int port = threadArgsData->port;
    char *num = threadArgsData->strNumber;

    int socketFd = connect2server(host, port);
    if (socketFd < 0)
    {
        close(socketFd);
        free(threadArgsData);

        pthread_exit(NULL);
    }

    char messageBuffer[1024];

    memset(messageBuffer, 0, sizeof(messageBuffer));
    strcpy(messageBuffer, num);
    send(socketFd, messageBuffer, strlen(messageBuffer), 0);

    memset(messageBuffer, 0, sizeof(messageBuffer));
    read(socketFd, messageBuffer, sizeof(messageBuffer));

    pthread_mutex_lock(&mutex);

    results[threadProcessId].number = num;
    strcpy(results[threadProcessId].result, messageBuffer);
    resultCount++;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    close(socketFd);
    free(threadArgsData);

    pthread_exit(NULL);
}

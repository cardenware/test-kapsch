# TEST-KAPSCH

The task involves implementing a simple number factorization application using a **CLIENT-SERVER** model. The client requests the factorization of a number, while the server is responsible for calculating the factorization and returning the result. The task requires the creation of two console applications, client and server, which communicate via the TCP protocol.

## Objectives

* Implementation of CLIENT and SERVER using TCP connections.
* Optimize bandwidth usage and dynamic memory.
* The server must handle up to 10 simultaneous connections.
* Use of multithreading.
* The CLIENT must send a list of numbers to factorize.
* A connection will be opened for each number in the list.
* Each connection will be processed by a separate thread.
* Print data in "FIFO" order.
* The development must be done in C and run on Linux.

## Makefiles

- Each folder has its own Makefile.
  - `make` and `make clean` are available for each Makefile.
  - `make` creates a new `build` directory if it does not already exist.
  - Both `client` and `server` have a `make run` target to test each component.
  - To debug the `client`, run `make debug`: this command will automatically install Valgrind if it's not already installed, and then generate a detailed debug report in a file called client.log located in the root client directory.

## Executing by Terminal

**Set up the environment variable:**
- Export the `LD_LIBRARY_PATH` to include the path to the shared libraries.
  ```sh
   export LD_LIBRARY_PATH=path/to/shared/libs

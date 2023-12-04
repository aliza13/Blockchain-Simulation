# Blockchain-Simulation

## Project Description

This program simulates a basic blockchain structure, drawing inspiration from the Bitcoin and Ethereum blockchain. Each block contains essential information such as a hash, timestamp, and a reference to the previous block's hash.

**Cee Blockchain**

## Running the Blockchain Simulation Program

To compile and run the blockchain simulation program, follow these steps:

1. Open a terminal in the directory containing the source file `blockchain_simulation.c`.

2. Run the following command to compile the program:

    ```bash
    gcc -o blockchain_simulation .\blockchain_simulation.c .\blockchain_simulation_utility.c -lbcrypt
    ```

    This command uses the GNU Compiler Collection (`gcc`) to compile the source file `blockchain_simulation.c` and links it with the bcrypt library (`-lbcrypt`), a Window's specific hashing function. The resulting executable will be named `blockchain_simulation`.

3. After successfully compiling the program, you can run it with the following command:

    ```bash
    ./blockchain_simulation
    ```

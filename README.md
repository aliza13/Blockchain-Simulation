# Blockchain-Simulation

## Running the Blockchain Simulation Program

To compile and run the blockchain simulation program, follow these steps:

1. Open a terminal in the directory containing the source file `blockchain_simulation.c`.

2. Run the following command to compile the program:

    ```bash
    gcc -o blockchain_simulation blockchain_simulation.c -lbcrypt
    ```

    This command uses the GNU Compiler Collection (`gcc`) to compile the source file `blockchain_simulation.c` and links it with the bcrypt library (`-lbcrypt`). The resulting executable will be named `blockchain_simulation`.

3. After successfully compiling the program, you can run it with the following command:

    ```bash
    ./blockchain_simulation
    ```

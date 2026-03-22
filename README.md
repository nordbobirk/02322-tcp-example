# TCP example
This repository is an example of how you can set up a TCP connection between a C server and a Python client. It is intended for use in project 2 in the course 02322 Machine oriented programming at DTU.
During this project, students may choose to implement their GUI in C using SDL or with another language communicating with the C backend over TCP. If you choose to use another language for your frontend, this example shows how you can set up communication between the server and client.

### Running
1. first compile the server binary using a compiler of your choice. For instance with GCC: `gcc server/server.c -o server/bin/server`. The server has intentionally been implemented as a single file, so compilation should be simple no matter the device.
2. then run the server binary (example: `./server/bin/server`)
3. finally, open a second terminal and start the Python client (`python3 python-client/client.py`). Now you can send messages from the client, and the server will respond.

### OS
This has been tested on Linux and macOS. It should work on Windows as well, although it will probably be easier to just use WSL.

If you have any questions about the code or find any issues with it, please contact me via DTU email (s245427) or speak to me or one of the other TA's during the lab sessions.

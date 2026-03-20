from socket import *

serverName = "localhost"
serverPort = 12345
clientSocket = socket(AF_INET, SOCK_STREAM)
clientSocket.connect((serverName,serverPort))

print("Write 'disconnect' to disconnect from the server!")

running = True

while running:
    msg = input("Write a message for the server: ")
    if msg.lower() == "disconnect":
        running = False
        print("Disconnecting from server!")
    else:
        clientSocket.send(msg.encode())
        response = clientSocket.recv(1024)
        print("Response from the server: ", response.decode()) 

clientSocket.close()

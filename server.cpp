#include <iostream>
#include <thread>
#include <mutex>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> 
#include <cstdlib>  




void handleClient(int clientSocket) {
    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];

    // Receive the operation type (upload or download) with error handling & checking
    int operation;
    if (recv(clientSocket, &operation, sizeof(operation), 0) == -1) {
        perror("Error receiving operation type");
        close(clientSocket);
        return;
    }

    if (operation == 1) { // Upload 
        // Receive the filename
        char filename[BUFFER_SIZE];
        if (recv(clientSocket, filename, sizeof(filename), 0) == -1) {
            perror("Error receiving filename");
            close(clientSocket);
            return;
        }
        // Open the file for writing
        std::ofstream outputFile(filename, std::ios::binary);
        if (!outputFile.is_open()) {
            perror("Error opening file for upload");
            close(clientSocket);
            return;
        }

        // Receive and write the file content
        int bytesRead;
        while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
            outputFile.write(buffer, bytesRead);
        }
        // Check for errors in receiving file content
        if (bytesRead == -1) {
            perror("Error receiving file content");
            outputFile.close();
            close(clientSocket);
            return;
        }
        // Close the file after writing
        outputFile.close();
        // confirmation message
        std::cout << "File uploaded successfully: " << filename << std::endl;
        
    } else if (operation == 2) { // Download
        // Receive the filename
        char filename[BUFFER_SIZE];
        if (recv(clientSocket, filename, sizeof(filename), 0) == -1) {
            perror("Error receiving filename");
            close(clientSocket);
            return;
        }
        
        // Open the file for reading
        std::ifstream inputFile(filename, std::ios::binary);
        if (!inputFile.is_open()) {
            perror("Error opening file for download");
            close(clientSocket);
            return;
        }

        // Send the file content
        while (!inputFile.eof()) {
            inputFile.read(buffer, sizeof(buffer));
            send(clientSocket, buffer, inputFile.gcount(), 0);
            memset(buffer, 0, sizeof(buffer));  // Clear the buffer
        }

        // Close the file  after sending
        inputFile.close();
		//confirmation
        std::cout << "File downloaded successfully: " << filename << std::endl;
    } else {
        std::cerr << "Invalid operation type" << std::endl;
        close(clientSocket);
        return;
    }

    // Close the client socket
    close(clientSocket);
}



int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Setup server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080); // Choose a port
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    std::cout << "Server started at port 8080" << std::endl;

    

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding socket");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 10) == -1) {
        perror("Error listening for connections");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    while (true) {
        // Accept a connection
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            perror("Error accepting connection");
            continue;
        }

        // launch a new thread to handle the client
        std::thread(handleClient, clientSocket).detach();
    }
// 	close socket
    close(serverSocket);
    return 0;
}

#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> 
#include <arpa/inet.h> 
#include <thread>
#include <mutex>
#include <future> 

using namespace std;
std:: mutex m; // Global mutex for thread synchronization

//creating the upload function
void uploadFile(int socket, const char* filename) {
    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    
    // mutex before performing the upload file
    lock_guard<mutex> lock(m);

    // Send the operation type (1 for upload)
    int operation = 1;
    send(socket, &operation, sizeof(operation), 0);

    // Send the filename
    send(socket, filename, strlen(filename) + 1, 0);

//	open file to read 
    std::ifstream inputFile(filename, std::ios::binary); // read in binary
    if (!inputFile.is_open()) {
        perror("Error opening file for upload");
        close(socket);
        return;
    }

    // Send the file content
    while (inputFile.read(buffer, sizeof(buffer)) || inputFile.gcount() > 0) {
    int bytesSent = send(socket, buffer, inputFile.gcount(), 0);
    if (bytesSent < 0) {
        perror("Error sending file content");
        inputFile.close();
        close(socket);
        return;
    }
    std::cout << "Bytes read: " << inputFile.gcount() << ", Bytes sent: " << bytesSent << std::endl;
}

    // Close the file after sending
    inputFile.close();

//	confirmation
    std::cout << "File uploaded successfully: " << filename << std::endl;
//	unlock mutex
   lock_guard<mutex> unlock(m);

}

//function to download file from server
void downloadFile(int socket, const char* filename) {
    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    
    // mutex before downloading
   lock_guard<mutex> lock(m);

    // Send the operation type (2 for download)
    int operation = 2;
    send(socket, &operation, sizeof(operation), 0);

    // Send the filename
    send(socket, filename, strlen(filename) + 1, 0);

//	open file
    std::ofstream outputFile(filename, std::ios::binary);
    if (!outputFile.is_open()) {
        perror("Error opening file for download");
        close(socket);
        return;
    }

    // Receive and write the file content
    int bytesRead;
    do {
        bytesRead = recv(socket, buffer, sizeof(buffer), 0);
        if (bytesRead < 0) {
            perror("Error receiving file content");
            break;  // Exit the loop on error
        } else if (bytesRead > 0) {
            outputFile.write(buffer, bytesRead);
            std::cout << "Bytes received: " << bytesRead << std::endl;
        }
    } while (bytesRead > 0);

//	close after receiving
    outputFile.close();

    if (bytesRead < 0) {
        perror("Error receiving file content");
    } else {
        std::cout << "File downloaded successfully: " << filename << std::endl;
    }

//     unlock mutex
   lock_guard<mutex> unlock(m);
}



int main() {
    // Create a client socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;

    // Set up server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080); // can change this to Match the server port
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // can Replace with server IP address

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error connecting to server");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }
    
    //collect users choice from options
   std::cout<< "Select an option "<<std::endl;
    std::cout<< "1. Upload  "<<std::endl;
    std::cout<< "2. Download "<<std::endl;
    int choice;
    std::cin >> choice;
    
    if(choice==1){
       const char* uploadFilename = "upload_file.txt";
       std::thread uploadThread(uploadFile,clientSocket,uploadFilename);
       uploadThread.detach();
    }
    else if (choice ==2){
          const char* downloadFilename = "download_file.txt";
           std::thread downloadThread(downloadFile,clientSocket,downloadFilename);
           downloadThread.detach();
           
    }

    else{
    std::cerr <<"Invalid option " <<std::endl;
    }
//     wait & detach
    std::this_thread::sleep_for(chrono::seconds(3));
    


    close(clientSocket);
    return 0;
}

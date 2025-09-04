#include <iostream>  
#include <cstring>      // For handling C-style strings  
#include <unistd.h>     // For POSIX API, including socket functions  
#include <arpa/inet.h>  // For handling internet addresses  
#include <thread>       // For handling threads  
#include <vector>       // For using vector (not used in this example but included)  

// Function to encrypt/decrypt messages using XOR operation  
std::string xorEncryptDecrypt(const std::string input, char key) {  
    std::string output = input; // Initialize output string with the input  
    // Loop through each character of the input string  
    for (size_t i = 0; i < input.size(); i++) {  
        output[i] = input[i] ^ key; // Apply XOR operation with the key  
    }  
    return output; // Return the encrypted/decrypted output  
}  

// Function to handle communication with a client  
void handleClient(int clientSocket) {  
    char buffer[1024]; // Buffer to store incoming messages  
    char key = 'K';    // Encryption key for XOR operation  

    while (true) { // Loop to continuously handle client communication  
        // Clear the buffer to ensure no residual data  
        memset(buffer, 0, sizeof(buffer));  

        // Receive message from the client  
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);  
        if (bytesReceived <= 0) { // Check if the client has disconnected or if an error occurred  
            std::cerr << "Client disconnected or error in receiving.\n";  
            break; // Exit the loop if an error occurs  
        }  

        // Decrypt the received message using XOR  
        std::string decryptedMessage = xorEncryptDecrypt(std::string(buffer), key);  
        std::cout << "Client says: " << decryptedMessage << "\n"; // Print the decrypted message  

        // Prepare a response message  
        std::string response = "Message received: " + decryptedMessage;  
        // Encrypt the response before sending it back to the client  
        std::string encryptedResponse = xorEncryptDecrypt(response, key);  

        // Send the encrypted response back to the client  
        send(clientSocket, encryptedResponse.c_str(), encryptedResponse.size(), 0);  
    }  

    close(clientSocket); // Close the client socket after communication ends  
}  

int main() {  
    int serverSocket, clientSocket; // Socket descriptors for server and client  
    sockaddr_in serverAddr, clientAddr; // Structures for server and client addresses  
    socklen_t clientAddrLen = sizeof(clientAddr); // Length of client address structure  

    // Create a socket using IPv4 and TCP protocol  
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);  
    if (serverSocket < 0) { // Check if socket creation was successful  
        std::cerr << "Error creating socket.\n"; // Print error message  
        return 1; // Exit the program with an error code  
    }  

    // Set up the server address structure  
    serverAddr.sin_family = AF_INET; // Use IPv4 address family  
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Bind to any available IP address  
    serverAddr.sin_port = htons(8080); // Set port number to 8080 (converted to network byte order)  

    // Bind the socket to the server address  
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {  
        std::cerr << "Error binding socket.\n"; // Print error message  
        return 1; // Exit the program with an error code  
    }  

    // Listen for incoming connections with a backlog of 5  
    listen(serverSocket, 5);  
    std::cout << "Server listening on port 8080...\n"; // Notify that the server is ready  

    // Accept clients in a loop  
    while (true) {  
        // Accept a new client connection  
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);  
        if (clientSocket < 0) { // Check if the connection was successful  
            std::cerr << "Error accepting connection.\n"; // Print error message  
            continue; // Continue to the next iteration to accept another client  
        }  
        std::cout << "Client connected.\n"; // Notify that a client has connected  

        // Handle client communication in a separate thread to allow multiple clients  
        std::thread(handleClient, clientSocket).detach(); // Detach the thread to allow it to run independently  
    }  

    close(serverSocket); // Close the server socket when done  
    return 0; // Exit the program successfully  
}  

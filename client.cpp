#include <iostream>  
#include <cstring>    // For handling C-style strings  
#include <unistd.h>   // For POSIX API, including socket functions  
#include <arpa/inet.h> // For handling internet addresses  

// Function to encrypt/decrypt messages using XOR operation  
std::string xorEncryptDecrypt(const std::string input, char key) {  
    std::string output = input; // Initialize output string with the input  
    // Loop through each character of the input string  
    for (size_t i = 0; i < input.size(); i++) {  
        output[i] = input[i] ^ key; // Apply XOR operation with the key  
    }  
    return output; // Return the encrypted/decrypted output  
}  

int main() {  
    int sock; // Socket file descriptor  
    sockaddr_in serverAddr; // Structure to hold server address information  
    char buffer[1024]; // Buffer to store incoming data  
    char key = 'K'; // Encryption key for XOR operation  

    // Create a socket using IPv4 and TCP protocol  
    sock = socket(AF_INET, SOCK_STREAM, 0);  
    if (sock < 0) { // Check if socket creation was successful  
        std::cerr << "Error creating socket.\n"; // Print error message  
        return 1; // Exit the program with an error code  
    }  

    // Set up the server address structure  
    serverAddr.sin_family = AF_INET; // Use IPv4 address family  
    serverAddr.sin_port = htons(8080); // Set port number to 8080 (converted to network byte order)  
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); // Convert IP address from text to binary (Localhost)  

    // Attempt to connect to the server  
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {  
        std::cerr << "Error connecting to server.\n"; // Print error message  
        return 1; // Exit the program with an error code  
    }  

    std::string message; // Variable to hold user input message  
    while (true) { // Loop to continuously get user input  
        std::cout << "Enter message (type 'exit' to quit): "; // Prompt user for input  
        std::getline(std::cin, message); // Read the entire line of input  

        if (message == "exit") { // Check for exit command  
            break; // Exit the loop if user types 'exit'  
        }  

        // Encrypt the message before sending  
        std::string encryptedMessage = xorEncryptDecrypt(message, key);  

        // Send the encrypted message to the server  
        send(sock, encryptedMessage.c_str(), encryptedMessage.size(), 0);  

        // Clear the buffer before receiving response  
        memset(buffer, 0, sizeof(buffer));  

        // Receive response from the server  
        int bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);  
        if (bytesReceived > 0) { // Check if data was received  
            std::cout << "Server response: " << buffer << "\n"; // Print the server's response  
        }  
    }  

    close(sock); // Close the socket after finishing communication  
    return 0; // Exit the program successfully  
}  

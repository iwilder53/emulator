#include <iostream>
#include <chrono>
#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <curl/curl.h>
float temperature, humidity;


// Function to generate a random float within a given range
float generateRandomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

// Function to generate simulated temperature and humidity
void generateTemperatureAndHumidity(float &temperature, float &humidity) {
    temperature = generateRandomFloat(-10.0, 50.0);
    humidity = generateRandomFloat(0.0, 100.0);

}

// Function to send a POST request
void sendPostRequest() {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    
    if(curl) {
        
        generateTemperatureAndHumidity(temperature, humidity);
            // HTTP headers to send with request
        struct curl_slist* headers = nullptr;

            // Set Content-Type to application/json
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:7200/api/logger/log-data");
       char buffer[40];
       snprintf(buffer,40,"{\"temp\":%f,\"hum\":%f}",temperature, humidity);
       printf(buffer);
        
        
        // Specify the POST data
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer);
        
        // Perform the request
        res = curl_easy_perform(curl);
        
        // Check for errors
        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        
        // Cleanup
        curl_easy_cleanup(curl);
    }
    
    curl_global_cleanup();
}

// Function to handle a client request
void handleRequest(int client_socket) {
    char buffer[1024] = {0};
    read(client_socket, buffer, 1024);

    std::string response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: 13\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "Hello, World!";

    send(client_socket, response.c_str(), response.size(), 0);
    close(client_socket);
}
/* 
// Function to start the server
void startServer() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server started at http://localhost:8080" << std::endl;

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        std::thread([](int socket) {
            handleRequest(socket);
        }, new_socket).detach();
    }
} */

int main() {
    // Start server in a separate thread
   // std::thread server_thread(startServer);
        printf("Device started");
    // Send POST requests in the main thread
    while (true) {
        sendPostRequest();
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

  //  server_thread.join(); // This will never be reached in this example
    return 0;
}

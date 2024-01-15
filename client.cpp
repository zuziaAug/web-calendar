#include <iostream>
#include <cstring>
#include <vector>
#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

struct DateTime {
    int year;
    int month;
    int day;
    int hour;
};

struct CalendarEvent {
    int event_id;
    string event_title;
    string event_description;
    DateTime start_date;
    DateTime end_time;
};

struct Request {
    int client_id;
    string action;
    string event_title;  
    string event_description;  
    DateTime start_date;  
    DateTime end_time;  
    int event_id;  
};

void sendRequest(int sockfd, const Request& request) {
    string request_str = to_string(request.client_id) + " " + request.action + " " +
                         " " + request.event_title + " " + request.event_description + " " +
                         to_string(request.start_date.year) + " " + to_string(request.start_date.month) + " " +
                         to_string(request.start_date.day) + " " + to_string(request.start_date.hour) + " " +
                         to_string(request.end_time.year) + " " + to_string(request.end_time.month) + " " +
                         to_string(request.end_time.day) + " " + to_string(request.end_time.hour) + " " +
                         to_string(request.event_id);
    send(sockfd, request_str.c_str(), request_str.length(), 0);
}

void* receiveThread(void* arg) {
    int sockfd = *(int*)arg;
    char buffer[1024] = {0};
    ssize_t bytes_received;

    while (true) {
        bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received == -1) {
            perror("Error receiving data");
            close(sockfd);
            pthread_exit(NULL);
        }
        string server_response(buffer);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            cout << "Server response: " << buffer << endl;
            memset(buffer, 0, sizeof(buffer));
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <client_id>" << endl;
        return 1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error creating socket");
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8881);
    inet_pton(AF_INET, "127.0.0.1", &(server_addr.sin_addr));

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(sockfd);
        return 1;
    }

    int client_id = stoi(argv[1]);
    string action;
    char buffer[1024] = {0};

    // creating thread to listen for server commands
    pthread_t receive_thread;
    if (pthread_create(&receive_thread, NULL, receiveThread, (void*)&sockfd) != 0) {
        perror("Failed to create receive thread");
        close(sockfd);
        return 1;
    }

    Request request{client_id, "REGISTER"};
    sendRequest(sockfd, request);
    sleep(1);
    while (true) {
        
        cout << "\nCHOOSE ACTION:\n" <<
        "[1.] Add new event \n[2.] Delete event \n[3.] Show all events \n[4.] Show all clients\n";
        getline(cin, action);

        if (action == "exit") break;
        
        // ADD_NEW_EVENT
        if (action == "1") {
            string new_event_title;
            string new_event_description;
            string start_year_str, start_month_str, start_day_str, start_hour_str;
            string end_year_str, end_month_str, end_day_str, end_hour_str;

            cout << "Enter event title: ";
            getline(cin, new_event_title);
            cout << "Enter event description: ";
            getline(cin, new_event_description);

            cout << "Enter start date (YYYY MM DD HH): ";
            cin >> start_year_str >> start_month_str >> start_day_str >> start_hour_str;

            cout << "Enter end date (YYYY MM DD HH): ";
            cin >> end_year_str >> end_month_str >> end_day_str >> end_hour_str;

            try {
                action = "ADD_EVENT";
                DateTime start_date{stoi(start_year_str), stoi(start_month_str), stoi(start_day_str), stoi(start_hour_str)};
                DateTime end_date{stoi(end_year_str), stoi(end_month_str), stoi(end_day_str), stoi(end_hour_str)};
                Request request{client_id, action, new_event_title, new_event_description, start_date, end_date, -1};
                
                // Send the ADD_EVENT request to the server
                sendRequest(sockfd, request);

                // Receive the server's response
                char buffer[1024] = {0};
                ssize_t bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
                
                if (bytes_received > 0) {
                    buffer[bytes_received] = '\0';
                    cout << "Server response: " << buffer << endl;
                }
            } catch (const invalid_argument& e) {
                cerr << "Invalid input. Please enter valid integers for date and time." << endl;
            } catch (const out_of_range& e) {
                cerr << "Input out of range for date and time." << endl;
            }

        // DELETE_EVENT
        } else if (action == "2") {
            string event_id_to_delete;
            cout << "Enter event ID to delete: ";
            getline(cin, event_id_to_delete);

            try {
                action = "DELETE_EVENT";
                int event_id = stoi(event_id_to_delete);
                Request request{client_id, action, "", "", DateTime{}, DateTime{}, event_id};
                sendRequest(sockfd, request);
            } catch (const invalid_argument& e) {
                cerr << "Invalid input. Please enter a valid integer." << endl;
            } catch (const out_of_range& e) {
                cerr << "Input out of range for integer." << endl;
            }

        // SHOW_ALL_EVENTS
        } else if (action == "3") {
            try {
                action = "SHOW_ALL_EVENTS";
                Request request{client_id, action};
                sendRequest(sockfd, request);
            } catch (const invalid_argument& e) {
                cerr << "Invalid input. Please enter a valid integer." << endl;
            } catch (const out_of_range& e) {
                cerr << "Input out of range for integer." << endl;
            }

        // SHOW_ALL_CLIENTS    
        } else if (action == "4") {
            try {
                action = "SHOW_ALL_CLIENTS";
                Request request{client_id, action};
                sendRequest(sockfd, request);
            } catch (const invalid_argument& e) {
                cerr << "Invalid input. Please enter a valid integer." << endl;
            } catch (const out_of_range& e) {
                cerr << "Input out of range for integer." << endl;
            }
        sleep(1);
    }

    close(sockfd);
    return 0;
}}

#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <algorithm> 

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

struct ThreadData {
    std::mutex data_mutex;
    std::unordered_map<int, int>& client_id_sock_map;
    std::vector<int>& active_clients;
    int client_sock;
    std::vector<CalendarEvent> calendar;

    ThreadData(std::unordered_map<int, int>& client_map, std::vector<int>& all_clients, int sock)
        : client_id_sock_map(client_map), active_clients(all_clients), client_sock(sock) {}

    void removeClientIdFromLists(int client_id) {
        // Usuwanie z active_clients
        auto active_clients_it = std::find(active_clients.begin(), active_clients.end(), client_id);
        if (active_clients_it != active_clients.end()) {
            active_clients.erase(active_clients_it);
        }
    }
};

Request parseRequest(const char* buffer) {
    istringstream iss(buffer);
    Request request;
    iss >> request.client_id >> request.action >> ws; 
    getline(iss, request.event_title, ' ');
    getline(iss, request.event_description, ' ');
    iss >> request.start_date.year >> request.start_date.month >> request.start_date.day >> request.start_date.hour
        >> request.end_time.year >> request.end_time.month >> request.end_time.day >> request.end_time.hour
        >> request.event_id;
    return request;
}

// Function to handle adding events to the calendar
void addEvent(int client_sock, const Request& request, ThreadData& thread_data) {
    string response_message;

    lock_guard<mutex> lock(thread_data.data_mutex);

    CalendarEvent new_event;
    new_event.event_id = thread_data.calendar.size() + 1; // Assign a new ID
    new_event.event_title = request.event_title; // Use the title field
    new_event.event_description = request.event_description;
    new_event.start_date = request.start_date; // Use the start_date field
    new_event.end_time = request.end_time; // Use the end_time field

    thread_data.calendar.push_back(new_event);

    response_message = "Event added successfully!";
    send(client_sock, response_message.c_str(), response_message.length(), 0);
}


// Function to handle deleting events from the calendar
void deleteEvent(int client_sock, const Request& request, ThreadData& thread_data) {
    string response_message;
    
    lock_guard<mutex> lock(thread_data.data_mutex);
    
    // Find the event by ID
    auto it = find_if(thread_data.calendar.begin(), thread_data.calendar.end(),
                      [request](const CalendarEvent& event) { return event.event_id == request.event_id; });
    
    if (it != thread_data.calendar.end()) {
        // Event found, erase it from the calendar
        thread_data.calendar.erase(it);
        response_message = "Event deleted successfully!";
    } else {
        response_message = "Event not found!";
    }

    send(client_sock, response_message.c_str(), response_message.length(), 0);
}

// Function to handle showing all events in the calendar
void showEvents(int client_sock, const Request& request, ThreadData& thread_data) {
    string response_message = "Calendar events:\n";

    lock_guard<mutex> lock(thread_data.data_mutex);
    
    for (const CalendarEvent& event : thread_data.calendar) {
        response_message += "ID: " + to_string(event.event_id) + ", Description: " + event.event_description + "\n";
    }

    send(client_sock, response_message.c_str(), response_message.length(), 0);
}

void registerClient(int client_sock, const Request& request, ThreadData& thread_data) {
    string response_message = "Connected successfully!";
    unordered_map<int, int>& client_id_sock_map = thread_data.client_id_sock_map;
    cout << "Active client with id: " << to_string(request.client_id) << " registered" << endl;

    lock_guard<mutex> lock(thread_data.data_mutex);
    {
        thread_data.active_clients.push_back(request.client_id);
    }
        //we convert string to byte array and send to client
        send(client_sock, response_message.c_str(), response_message.length(), 0);
}

void showClients(int client_sock, const Request& request, ThreadData& thread_data) {
    string response_message = "Active client ids: [";
    cout << "Show all active clients for client with id: " << to_string(request.client_id) << endl;

    lock_guard<mutex> lock(thread_data.data_mutex);
    for(int id : thread_data.active_clients) {
        response_message = response_message + to_string(id) + ", ";
    }

    response_message = response_message.substr(0, response_message.size() - 2) + "]";
    send(client_sock, response_message.c_str(), response_message.length(), 0);
}

void* clientHandler(void* arg) {
    ThreadData* thread_data = (ThreadData*)arg;
    int client_sock = thread_data->client_sock;

    string response_message;
    char buffer[1024] = {0};

    bool client_id_added_to_map = false;
    while (true) {
        recv(client_sock, buffer, sizeof(buffer), 0);
        Request request = parseRequest(buffer);

        if (!client_id_added_to_map) {
            lock_guard<mutex> lock(thread_data->data_mutex);
            thread_data->client_id_sock_map[request.client_id] = client_sock;
            client_id_added_to_map = true;
        }

        if (request.action == "ADD_EVENT") {
            addEvent(client_sock, request, *thread_data);
        } else if (request.action == "DELETE_EVENT") {
            deleteEvent(client_sock, request, *thread_data);
        } else if (request.action == "SHOW_ALL_EVENTS") {
            showEvents(client_sock, request, *thread_data);
        } else if (request.action == "REGISTER") {
            registerClient(client_sock, request, *thread_data);
        } else if (request.action == "SHOW_ALL_CLIENTS") {
            showEvents(client_sock, request, *thread_data);
        }
        
        memset(buffer, 0, sizeof(buffer));
    }
}

int main() {
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Error creating socket");
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8881);

    if (::bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        return 1;
    }

    if (listen(server_sock, 10) == -1) {
        perror("Listen failed");
        return 1;
    }

    cout << "[MAIN] Server listening on port 8888..." << std::endl;

    vector<pthread_t> thread_ids;
    vector<int> active_clients;
    unordered_map<int, int> client_id_sock_map;
    vector<CalendarEvent> calendar;

    while (true) {
        
        int client_sock = accept(server_sock, nullptr, nullptr);
        cout << "[MAIN] New client connected, sock: " << client_sock << endl;
        if (client_sock == -1) {
            perror("Accept failed");
            continue;
        }

        ThreadData* thread_data = new ThreadData {
            client_id_sock_map, 
            active_clients, 
            client_sock
            };

        // creating client thread
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, clientHandler, (void*)thread_data) != 0) {
            perror("Failed to create thread");
            close(client_sock);
            delete thread_data;
            continue;
        }

        thread_ids.push_back(thread_id);
        pthread_detach(thread_id);
    }

    // waiting for all threads go down
    for (pthread_t thread_id : thread_ids) {
        pthread_join(thread_id, NULL);
    }

    close(server_sock);
    return 0;
}

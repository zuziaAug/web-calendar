#include <QtNetwork>

#include "clientthread.h"

ClientThread::ClientThread(QObject *parent)
    : QThread(parent), quit(false)
{
}

//! [0]
ClientThread::~ClientThread()
{
    mutex.lock();
    quit = true;
    cond.wakeOne();
    mutex.unlock();
    wait();
}

void ClientThread::connectClient(const QString &hostName, quint16 port, quint16 clientId)
{
    //! [1]
    QMutexLocker locker(&mutex);
    this->hostName = hostName;
    this->port = port;
    this->clientId = clientId;
    //! [3]
    if (!isRunning())
        start();
    else
        cond.wakeOne();
}
//! [0]
void ClientThread::run()
{
    mutex.lock();
    //! [4] //! [5]
    QString serverName = hostName;
    quint16 serverPort = port;
    mutex.unlock();
    //! [5]

    //! [6]
    while (!quit) {
        //! [7]
        // const int Timeout = 1000;
        using namespace std;
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            emit error(1, "Error creating socket");
        }

        sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(serverPort);
        inet_pton(AF_INET, serverName.toUtf8(), &(server_addr.sin_addr));

        if (::connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            emit error(1, "Connection failed");
            close(sockfd);

        }

        int client_id = 1;
        string action;
        //char buffer[1024] = {0};

        Request request{client_id, "REGISTER", "", "", DateTime{}, DateTime{},0};
        sendRequest(sockfd, request);
        sleep(1);

        QString event;
        while (true) {
            mutex.lock();
            emit newEvent(event);
            //! [7]

            emit newEvent("\nCHOOSE ACTION:\n"
                          "[1] Add new event \n[2] Delete event \n[3] Show all events \n[4] Show all clients\n[5] Exit\n");


            cond.wait(&mutex);
            mutex.unlock();
            // getline(cin, action);

            // if (action == "exit") break;

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

                //try {
                action = "ADD_EVENT";
                DateTime start_date{stoi(start_year_str), stoi(start_month_str), stoi(start_day_str), stoi(start_hour_str)};
                DateTime end_date{stoi(end_year_str), stoi(end_month_str), stoi(end_day_str), stoi(end_hour_str)};
                Request request{client_id, action, new_event_title, new_event_description, start_date, end_date, -1};

                // Send the ADD_EVENT request to the server
                sendRequest(sockfd, request);

                // Receive the server's response
                //     char buffer[1024] = {0};
                //     ssize_t bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);

                //     if (bytes_received > 0) {
                //         buffer[bytes_received] = '\0';
                //         cout << "Server response: " << buffer << endl;
                //     }
                // } catch (const invalid_argument& e) {
                //     cerr << "Invalid input. Please enter valid integers for date and time." << endl;
                // } catch (const out_of_range& e) {
                //     cerr << "Input out of range for date and time." << endl;
                // }

                // DELETE_EVENT
            } else if (action == "2") {
                string event_id_to_delete;
                qDebug() << "Enter event ID to delete: ";
                getline(cin, event_id_to_delete);

                try {
                    action = "DELETE_EVENT";
                    int event_id = stoi(event_id_to_delete);
                    Request request{client_id, action, "", "", DateTime{}, DateTime{}, event_id};
                    sendRequest(sockfd, request);
                } catch (const invalid_argument& e) {
                    qDebug() << "Invalid input. Please enter a valid integer.";
                } catch (const out_of_range& e) {
                    qDebug() << "Input out of range for integer.";
                }

                // SHOW_ALL_EVENTS
            } else if (action == "3") {
                // try {
                action = "SHOW_ALL_EVENTS";
                Request request{client_id, action, "", "", DateTime{}, DateTime{}, -1};
                sendRequest(sockfd, request);

                //     // Receive and display the server's response
                //     char buffer[1024] = {0};
                //     ssize_t bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);

                //     if (bytes_received > 0) {
                //         buffer[bytes_received] = '\0';
                //         cout << "Server response:\n" << buffer << endl;
                //     } else if (bytes_received == 0) {
                //         cout << "Server closed the connection." << endl;
                //     } else {
                //         perror("Error receiving data from server");
                //     }

                // } catch (const invalid_argument& e) {
                //     cerr << "Invalid input. Please enter a valid integer." << endl;
                // } catch (const out_of_range& e) {
                //     cerr << "Input out of range for integer." << endl;
                // }

                // SHOW_ALL_CLIENTS
            } else if (action == "4") {
                action = "SHOW_ALL_CLIENTS";
                Request request{ client_id, action, "", "", DateTime{}, DateTime{}, -1 };
                sendRequest(sockfd, request);

                // // Receive and display the server's response
                // char buffer[1024] = { 0 };
                // ssize_t bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);

                // if (bytes_received > 0) {
                //     buffer[bytes_received] = '\0';
                //     cout << "Server response:\n" << buffer << endl;
                // }
                // else if (bytes_received == 0) {
                //     cout << "Server closed the connection." << endl;
                // }
                // else {
                //     perror("Error receiving data from server");
                // }

            } else if (action == "5") {
                qDebug() << "Disconnecting from server.\n";
                break;
            }
        }
        /*
        // QTcpSocket socket;
        socket.connectToHost(serverName, serverPort);
        //! [6] //! [8]

        if (!socket.waitForConnected(Timeout)) {
            emit error(socket.error(), socket.errorString());
            return;
        }
        //! [8] //! [11]

        QDataStream in(&socket);
        in.setVersion(QDataStream::Qt_6_5);
        QString event;
        //! [11] //! [12]

        do {
            if (!socket.waitForReadyRead(Timeout)) {
                emit error(socket.error(), socket.errorString());
                return;
            }

            in.startTransaction();
            in >> event;
        } while (!in.commitTransaction());
        //! [12] //! [15]

        mutex.lock();
        emit newEvent(event);
        //! [7]

        cond.wait(&mutex);
        serverName = hostName;
        serverPort = port;
        mutex.unlock();
        */
    }
 }

void ClientThread::sendRequest(int sockfd, const Request& request) {
    using namespace std;
    string request_str = to_string(request.client_id) + "" + request.action + " " +
                         request.event_title + " " + request.event_description + " " +
                         to_string(request.start_date.year) + " " + to_string(request.start_date.month) + " " +
                         to_string(request.start_date.day) + " " + to_string(request.start_date.hour) + " " +
                         to_string(request.end_time.year) + " " + to_string(request.end_time.month) + " " +
                         to_string(request.end_time.day) + " " + to_string(request.end_time.hour) + " " +
                         to_string(request.event_id);

    lock_guard<std::mutex> lock(mtx);
    send(sockfd, request_str.c_str(), request_str.length(), 0);
}
    //! [15]
void* ClientThread::receiveThread(void* arg) {
    using namespace std;

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
        }
        memset(buffer, 0, sizeof(buffer));
    }
}

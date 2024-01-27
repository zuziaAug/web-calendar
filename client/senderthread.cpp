#include <QtNetwork>

#include "senderthread.h"

SenderThread::SenderThread(QObject *parent)
    : QThread(parent)
{

}

SenderThread::~SenderThread()
{

}

void SenderThread::connectClient(const QString &hostName, quint16 port, quint16 clientId)
{

    QMutexLocker locker(&mutex);
    using namespace std;
    this->clientId = clientId;
    this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->sockfd == -1) {
        emit error(1, "Error creating socket");
    }

    emit newSockfd(this->sockfd);

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, hostName.toUtf8(), &(server_addr.sin_addr));

    if (::connect(this->sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        emit error(1, "Connection failed");
        close(this->sockfd);
    }

    Request request{this->clientId, "REGISTER", "", "", DateTime{}, DateTime{},0};
    sendRequest(request);

    if (!isRunning())
        start();
    else
        cond.wakeOne();

}

void SenderThread::run()
{

}

void SenderThread::sendRequest(const Request& request) {
    using namespace std;
    string request_str = to_string(request.client_id) + "" + request.action + " " +
                         request.event_title + " " + request.event_description + " " +
                         to_string(request.start_date.year) + " " + to_string(request.start_date.month) + " " +
                         to_string(request.start_date.day) + " " + to_string(request.start_date.hour) + " " +
                         to_string(request.end_time.year) + " " + to_string(request.end_time.month) + " " +
                         to_string(request.end_time.day) + " " + to_string(request.end_time.hour) + " " +
                         to_string(request.event_id);

    send(this->sockfd, request_str.c_str(), request_str.length(), 0);
}

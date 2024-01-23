#include "receiverthread.h"
#include <QDebug>

ReceiverThread::ReceiverThread(QObject *parent)
    : QThread{parent}
{

}

ReceiverThread::~ReceiverThread()
{

}

void ReceiverThread::run()
{
    while(true) {
    bytes_received = recv(this->sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received == -1) {
        qDebug() << "Error receiving data";
    }
    std::string server_response(buffer);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        emit newEvent(buffer);
    }
    memset(buffer, 0, sizeof(buffer));
    sleep(2);
    }
}

void ReceiverThread::setSockfd(int sockfd)
{
    this->sockfd = sockfd;
}

#include <QtNetwork>

#include "senderthread.h"

SenderThread::SenderThread(QObject *parent)
    : QThread(parent), quit(false)
{

}

SenderThread::~SenderThread()
{
    mutex.lock();
    quit = true;
    cond.wakeOne();
    mutex.unlock();
    wait();
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

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
    this->clientId = clientId;    
    socket.connectToHost(hostName, port);
    if (!socket.waitForConnected(1000)) {
        emit error(socket.error(), socket.errorString());
        return;
    }
    emit sendSocket(&socket);

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

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_5);
    out << QString().fromStdString(request_str);
    mutex.lock();
    socket.write(block);
    mutex.unlock();
}

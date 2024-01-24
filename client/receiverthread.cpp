#include "receiverthread.h"

ReceiverThread::ReceiverThread(QObject *parent)
    : QThread{parent}
{

}

ReceiverThread::~ReceiverThread()
{

}

void ReceiverThread::run()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_5);
    QString response;

    do {
        if (!socket->waitForReadyRead(10000)) {
            emit error(socket->error(), socket->errorString());
            return;
        }

        in.startTransaction();
        in >> response;
    } while (!in.commitTransaction());

    emit newEvent(response);
}

void ReceiverThread::setSocket(QTcpSocket *socket)
{
    this->socket = socket;
}


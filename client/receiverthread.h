#ifndef RECEIVERTHREAD_H
#define RECEIVERTHREAD_H

#include <QThread>

#include <sys/socket.h>
#include <arpa/inet.h>

class ReceiverThread : public QThread
{
    Q_OBJECT
public:
    ReceiverThread(QObject *parent = nullptr);
    ~ReceiverThread();

    void run() override;

signals:
    void newEvent(const QString &event);
    void error(int socketError, const QString &message);

public slots:
    void setSockfd(int sockfd);

private:
    char buffer[1024] = {0};
    ssize_t bytes_received;
    int sockfd;
};

#endif // RECEIVERTHREAD_H

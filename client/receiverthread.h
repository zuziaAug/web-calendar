#ifndef RECEIVERTHREAD_H
#define RECEIVERTHREAD_H

#include <QThread>
#include <QMutex>
#include <QTcpSocket>

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
    void setSocket(QTcpSocket *socket);

private:
    QTcpSocket *socket;
    QMutex mutex;

};

#endif // RECEIVERTHREAD_H

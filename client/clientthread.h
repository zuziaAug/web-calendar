#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class ClientThread : public QThread
{
    Q_OBJECT
public:
    ClientThread(QObject *parent = nullptr);
    ~ClientThread();

    void requestNewEvent(const QString &hostName, quint16 port);
    void run() override;

signals:
    void newEvent(const QString &event);
    void error(int socketError, const QString &message);

private:
    QString hostName;
    quint16 port;
    QMutex mutex;
    QWaitCondition cond;
    bool quit;
};

#endif // CLIENTTHREAD_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

#include "clientthread.h"

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QAction;
QT_END_NAMESPACE

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void requestNewEvent();
    void showEvent(const QString &event);
    void displayError(int socketError, const QString &message);
    void enableGetEventButton();

private:
    QLabel *hostLabel;
    QLabel *portLabel;
    QLabel *clientIdLabel;
    QLineEdit *hostLineEdit;
    QLineEdit *portLineEdit;
    QLineEdit *clientIdLineEdit;
    QLabel *statusLabel;
    QPushButton *getEventButton;
    QPushButton *quitButton;
    QDialogButtonBox *buttonBox;

    ClientThread thread;
    QString currentEvent;
};
#endif // MAINWINDOW_H

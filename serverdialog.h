#ifndef SERVERDIALOG_H
#define SERVERDIALOG_H

#include <QWidget>
#include <QSslSocket>
#include <QCryptographicHash>

namespace Ui {
class ServerDialog;
}

class ServerDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ServerDialog(QWidget *parent = nullptr);
    ~ServerDialog();

private:
    Ui::ServerDialog *ui;
    QCryptographicHash hash;
    QSslSocket* socket;

private slots:
    void onConnectionAttempt();
    void onSocketConnected();

signals:
    void connected(QSslSocket socket);
    void cancelled();
};

#endif // SERVERDIALOG_H

#include "serverdialog.h"
#include "ui_serverdialog.h"

ServerDialog::ServerDialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ServerDialog)
    , hash(QCryptographicHash::Sha256)
    , socket(new QSslSocket(this))
{
    ui->setupUi(this);
    connect(ui->connectButton, &QAbstractButton::clicked, this, &ServerDialog::onConnectionAttempt);
}

void ServerDialog::onConnectionAttempt(){
    QString ip = ui->ipEdit->text();

    ui->outputLog->setTextColor(Qt::red);

    if(ip.isEmpty()) ui->outputLog->append("Ip must not be empty!");
    else
    {
        socket->connectToHost(ip, 12345);
    }

}

ServerDialog::~ServerDialog()
{
    delete ui;
}

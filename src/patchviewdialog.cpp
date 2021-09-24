#include "patchviewdialog.h"
#include "../ui/ui_patchviewdialog.h"

PatchViewDialog::PatchViewDialog(QWidget *parent) : QDialog(parent), ui(new Ui::PatchViewDialog) {
    ui->setupUi(this);

    hexView = new QHexView();
}

PatchViewDialog::~PatchViewDialog() {
    delete hexView;
    delete ui;
}

void PatchViewDialog::on_closeBtn_clicked() {
    PatchViewDialog::hide();
}

void PatchViewDialog::SetData(const QByteArray &data) {
    QHexView::DataStorageArray *dsa = new QHexView::DataStorageArray(data);

    hexView->setData(dsa);
    ui->verticalLayout->addWidget(hexView);
    ui->bytesCount->setText(QString("Bytes: %1").arg(data.size()));

}

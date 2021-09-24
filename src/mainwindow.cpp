#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QLabel>

#include "mainwindow.h"
#include "../ui/ui_mainwindow.h"
#include "rleRecord.h"
#include "record.h"

void MainWindow::onOpenIpsPatch() {
    QString ipsPath { QFileDialog::getOpenFileName(this, tr("Open IPS patch"), nullptr, tr("IPS patch (*.ips)")) };

    if (ipsPath.isNull())
        return;

    if (!ipsFile.isNull())
        disconnect(ipsFile.data(), &IPS::ipsLoaded, this, &MainWindow::onIpsFileRead);

    ipsFile = QSharedPointer<IPS>::create();

    clearTable();
    connect(ipsFile.data(), &IPS::ipsLoaded, this, &MainWindow::onIpsFileRead);
    ipsFile->loadIPS(ipsPath);

    if (ipsFile->hasError()) {
        QMessageBox::warning(this, tr("IPS viewer"), ipsFile->getError());
        return;
    }
}

void MainWindow::onIpsFileRead() {
    QTableWidget *table = ui->patchList;
    quint32 patchC = 0;
    quint32 rleC = 0;
    quint32 idx = 0;

    if (ipsFile->hasError()) {
        QMessageBox::warning(this, tr("invalid IPS patch"), ipsFile->getError());
        return;
    }

    for (const QSharedPointer<IPSRecord::ipsRecord> &record: ipsFile->getRecords()) {
        bool isRle = record->getSize() == 0;
        quint32 sz = isRle ? dynamic_cast<IPSRecord::rleRecord *>(record.data())->getRleSize() : record->getSize();
        QTableWidgetItem *off {new QTableWidgetItem("0x" + QString::number(record->getOffset(), 16))};
        QTableWidgetItem *roff {new QTableWidgetItem("0x" + QString::number(record->getRecordOffset(), 16))};
        QTableWidgetItem *size {new QTableWidgetItem(QString::number(sz))};
        QTableWidgetItem *type {new QTableWidgetItem(isRle ? "RLE" : "PATCH")};

        table->insertRow(idx);
        table->setItem(idx, 0, roff);
        table->setItem(idx, 1, off);
        table->setItem(idx, 2, size);
        table->setItem(idx, 3, type);

        ++idx;

        if (isRle) ++rleC;
        else        ++patchC;
    }

    patchCountTx->setText(QString::number(patchC));
    rleCountTx->setText(QString::number(rleC));
    totalCountTx->setText(QString::number(ipsFile->getRecords().size()));

    if (ipsFile->getTruncateOffset() != 0)
        truncOffTx->setText("0x" + QString::number(ipsFile->getTruncateOffset(), 16));
    else
        truncOffTx->setText("-");
}

void MainWindow::onTableItemDblClick(QTableWidgetItem *itm) {
    QByteArray data = ipsFile->getRecords().at(itm->row()).data()->getData();

    patchView->SetData(data);
    patchView->show();
}

void MainWindow::clearTable() {
    QTableWidget *tb = ui->patchList;

    tb->clearSelection();
    tb->clearContents();
    tb->setRowCount(0);

    truncOffTx->setText("-");
    patchCountTx->setText("0");
    rleCountTx->setText("0");
    totalCountTx->setText("0");
}

void MainWindow::onExitApp() {
    QApplication::exit();
}

void MainWindow::onAbout() {
    QMessageBox::information(this, tr("About"), tr("IPS Viewer v.1.0 @2021\n\nAuthor: kylon\n\nLicense: GPLv3"));
}

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QAction *menuOpenIps {ui->menuFile->actions().at(0)};
    QAction *menuExitApp {ui->menuFile->actions().at(1)};
    QAction *menuAbout {ui->menubar->addAction("About")};
    QLabel *truncateOffsetLbl {new QLabel("Truncate offset:")};
    QLabel *patchCountLbl {new QLabel("Patches:")};
    QLabel *rleCountLbl {new QLabel("| RLE:")};
    QLabel *totalCountLbl {new QLabel("| Total:")};

    truncOffTx = new QLabel("-");
    patchCountTx = new QLabel("0");
    rleCountTx = new QLabel("0");
    totalCountTx = new QLabel("0");

    ui->patchList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->statusbar->addPermanentWidget(truncateOffsetLbl);
    ui->statusbar->addPermanentWidget(truncOffTx, 1); // cheating here
    ui->statusbar->addPermanentWidget(patchCountLbl);
    ui->statusbar->addPermanentWidget(patchCountTx);
    ui->statusbar->addPermanentWidget(rleCountLbl);
    ui->statusbar->addPermanentWidget(rleCountTx);
    ui->statusbar->addPermanentWidget(totalCountLbl);
    ui->statusbar->addPermanentWidget(totalCountTx);

    patchView = new PatchViewDialog(this);

    connect(menuOpenIps, &QAction::triggered, this, &MainWindow::onOpenIpsPatch);
    connect(menuExitApp, &QAction::triggered, this, &MainWindow::onExitApp);
    connect(menuAbout, &QAction::triggered, this, &MainWindow::onAbout);
    connect(ui->patchList, &QTableWidget::itemDoubleClicked, this, &MainWindow::onTableItemDblClick);
}

MainWindow::~MainWindow() {
    delete patchView;
    delete ui;
}

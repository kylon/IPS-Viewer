/*
 * This file is part of IPSviewer.
 * Copyright (C) 2025 kylon
 *
 * IPSviewer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * IPSviewer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <QFileDialog>
#include <QMessageBox>
#include <QHexView/model/buffer/qmemoryrefbuffer.h>

#include "mainwindow.h"
#include "../ui/ui_mainwindow.h"
#include "RLERecord.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    const QAction *menuOpenIps {ui->menuFile->actions().at(0)};
    const QAction *menuAbout {ui->menubar->addAction("About")};

    patchView = new QHexView();
    truncOffTx = new QLabel("-");
    patchCountTx = new QLabel("0");
    rleCountTx = new QLabel("0");
    totalCountTx = new QLabel("0");

    patchView->setMinimumSize(630, 450);
    ui->patchList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->statusbar->addPermanentWidget(new QLabel("Truncate offset:"));
    ui->statusbar->addPermanentWidget(truncOffTx, 1); // cheating here
    ui->statusbar->addPermanentWidget(new QLabel("Patches:"));
    ui->statusbar->addPermanentWidget(patchCountTx);
    ui->statusbar->addPermanentWidget(new QLabel("| RLE:"));
    ui->statusbar->addPermanentWidget(rleCountTx);
    ui->statusbar->addPermanentWidget(new QLabel("| Total:"));
    ui->statusbar->addPermanentWidget(totalCountTx);

    QObject::connect(menuOpenIps, &QAction::triggered, this, &MainWindow::onOpenIpsPatch);
    QObject::connect(menuAbout, &QAction::triggered, this, &MainWindow::onAbout);
    QObject::connect(ui->patchList, &QTableWidget::itemDoubleClicked, this, &MainWindow::onTableItemDblClick);
}

MainWindow::~MainWindow() {
    delete patchView;
    delete ui;
}

void MainWindow::onOpenIpsPatch() {
    const QString ipsPath {QFileDialog::getOpenFileName(this, QStringLiteral("Open IPS patch"), nullptr, QStringLiteral("IPS patch (*.ips)"))};

    if (ipsPath.isNull())
        return;

    ipsFile.reset(new IPSV::IPS);
    clearTable();

    if (!ipsFile->loadIPS(ipsPath))
        QMessageBox::warning(this, QStringLiteral("IPS viewer"), ipsFile->getError());
    else
        showRecords();
}

void MainWindow::showRecords() const {
    const QList<QSharedPointer<IPSV::Record>> recordList = ipsFile->getRecords();
    QTableWidget *table = ui->patchList;
    quint32 patchC = 0;
    quint32 rleC = 0;
    quint32 idx = 0;

    for (const QSharedPointer<IPSV::Record> &record: recordList) {
        const bool isRle = record->size == 0;
        const quint32 sz = isRle ? record.dynamicCast<IPSV::RLERecord>()->rleSize : record->size;
        QTableWidgetItem *ipsOfft = new QTableWidgetItem(QString("0x%1").arg(QString::number(record->ipsOffset, 16)));
        QTableWidgetItem *offt = new QTableWidgetItem(QString("0x%1").arg(QString::number(record->offset, 16)));
        QTableWidgetItem *size = new QTableWidgetItem(QString::number(sz));
        QTableWidgetItem *type = new QTableWidgetItem(isRle ? "RLE" : "PATCH");

        table->insertRow(idx);
        table->setItem(idx, 0, ipsOfft);
        table->setItem(idx, 1, offt);
        table->setItem(idx, 2, size);
        table->setItem(idx, 3, type);

        ++idx;

        if (isRle)
            ++rleC;
        else
            ++patchC;
    }

    patchCountTx->setText(QString::number(patchC));
    rleCountTx->setText(QString::number(rleC));
    totalCountTx->setText(QString::number(recordList.size()));

    if (ipsFile->getTruncateOffset() != 0)
        truncOffTx->setText("0x" + QString::number(ipsFile->getTruncateOffset(), 16));
    else
        truncOffTx->setText("-");
}

void MainWindow::onTableItemDblClick(const QTableWidgetItem *itm) {
    const QByteArray data = ipsFile->getRecordData(itm->row());

    if (patchViewDoc != nullptr)
        patchViewDoc->deleteLater();

    patchViewDoc = QHexDocument::fromMemory<QMemoryRefBuffer>(data);
    patchView->setDocument(patchViewDoc);
    patchView->setWindowTitle(QString("%1, %2 bytes").arg(itm->text()).arg(data.size()));
    patchView->show();
}

void MainWindow::clearTable() const {
    QTableWidget *tb = ui->patchList;

    tb->clearSelection();
    tb->clearContents();
    tb->setRowCount(0);

    truncOffTx->setText("-");
    patchCountTx->setText("0");
    rleCountTx->setText("0");
    totalCountTx->setText("0");
}

void MainWindow::onAbout() {
    QMessageBox::information(this, QStringLiteral("About"), QStringLiteral("IPS Viewer 2.0 @2025 - kylon - GPLv3"));
}

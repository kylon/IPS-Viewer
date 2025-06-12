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
#pragma once

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QLabel>
#include <QHexView/qhexview.h>

#include "IPS.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow final: public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui = nullptr;
    QHexView *patchView = nullptr;
    QLabel *patchCountTx = nullptr;
    QLabel *rleCountTx = nullptr;
    QLabel *totalCountTx = nullptr;
    QLabel *truncOffTx = nullptr;
    QHexDocument *patchViewDoc = nullptr;
    QSharedPointer<IPSV::IPS> ipsFile;

    void clearTable() const;

private slots:
    void onOpenIpsPatch();
    void onAbout();
    void onIpsFileRead();
    void onTableItemDblClick(const QTableWidgetItem *itm);
};

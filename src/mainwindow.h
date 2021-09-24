#pragma once

#include <QMainWindow>
#include <QSharedPointer>
#include <QTableWidgetItem>
#include <QLabel>

#include "IPS.h"
#include "patchviewdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    PatchViewDialog *patchView;
    QLabel *patchCountTx, *rleCountTx, *totalCountTx, *truncOffTx;
    QSharedPointer<IPS> ipsFile;

    void clearTable();

private slots:
    void onOpenIpsPatch();
    void onExitApp();
    void onAbout();
    void onIpsFileRead();
    void onTableItemDblClick(QTableWidgetItem *itm);
};

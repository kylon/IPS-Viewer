#pragma once

#include <QDialog>

#include "QHexView.h"

namespace Ui {
    class PatchViewDialog;
}

class PatchViewDialog : public QDialog {
    Q_OBJECT

public:
    explicit PatchViewDialog(QWidget *parent = nullptr);
    ~PatchViewDialog();

    void SetData(const QByteArray &data);

private:
    Ui::PatchViewDialog *ui;
    QHexView *hexView;

private slots:
    void on_closeBtn_clicked();
};


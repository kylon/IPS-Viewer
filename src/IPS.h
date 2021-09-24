#pragma once

#include <QSharedPointer>
#include <QString>
#include <QObject>
#include <QList>

#include "ipsRecord.h"

class IPS: public QObject {
    Q_OBJECT

private:
    enum RECORD_READ_STATE {
        OFFSET,
        SIZE,
        RLE_SIZE,
        DATA,
        END
    };

    const QByteArray IPSHeader {"\x50\x41\x54\x43\x48"};
    const QByteArray IPSEof {"\x45\x4f\x46"};
    const qint64 maxFileSize {0x1000000};
    const quint32 maxRecordSize {0xFFFF};
    const qint32 truncateExtSize {3};
    quint32 truncateOffset {0};
    QList<QSharedPointer<IPSRecord::ipsRecord>> recordList;
    QString error;

    void readRecords(const QByteArray &file);
    void setError(const QString &msg);
    quint32 byte3ToUint(char b1, char b2, char b3);
    quint32 byte2ToUint(char b1, char b2);

public:
    IPS() {};
    ~IPS();

    void loadIPS(const QString &path);
    QString getError();

    bool hasError() const { return !error.isNull(); }
    void setTruncateOffset(quint32 off) { truncateOffset = off; }
    quint32 getTruncateOffset() const { return truncateOffset; }
    QList<QSharedPointer<IPSRecord::ipsRecord>> getRecords() const { return recordList; }

signals:
    void ipsLoaded();
};

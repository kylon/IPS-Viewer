#include <QFile>

#include "IPS.h"
#include "rleRecord.h"
#include "record.h"
#include "recordSize.h"

void IPS::loadIPS(const QString &path) {
    QFile ipsFile {path};
    QByteArray header, eof, truncExt;

    if (ipsFile.size() > maxFileSize) {
        setError(QString("Invalid IPS file!\n\nfile size: %1, max size: %2").arg(ipsFile.size(), maxFileSize));
        return;

    } else if (!ipsFile.open(QIODevice::ReadOnly)) {
        setError(ipsFile.errorString());
        return;
    }

    header = ipsFile.read(IPSHeader.size());
    if (header.size() < IPSHeader.size() || header.compare(IPSHeader) != 0) {
        setError("Invalid IPS file!\n\nWrong header.");
        return;
    }

    ipsFile.seek(ipsFile.size() - IPSEof.size());

    eof = ipsFile.read(IPSEof.size());
    if (eof.compare(IPSEof) != 0) {
        ipsFile.seek(ipsFile.size() - (IPSEof.size() + truncateExtSize));

        eof = ipsFile.read(IPSEof.size());
        if (eof.size() < IPSEof.size() || eof.compare(IPSEof) != 0) {
            setError("Invalid IPS file!\n\nWrong EOF.");
            return;
        }

        // if we are here, this has truncate extension
        truncExt = ipsFile.read(truncateExtSize);
        if (truncExt.size() < truncateExtSize) {
            setError("Invalid IPS file!\n\nInvalid extension size.");
            return;
        }

        setTruncateOffset(byte3ToUint(truncExt.at(0), truncExt.at(1), truncExt.at(2)));
    }

    ipsFile.seek(IPSHeader.size());
    readRecords(ipsFile.readAll());
    ipsFile.close();

    emit ipsLoaded();
}

void IPS::readRecords(const QByteArray &file) {
    quint32 len = file.size() - IPSEof.size() - (truncateOffset != 0 ? truncateExtSize:0);
    quint32 idx = 0;

    while (idx < len) {
        RECORD_READ_STATE state = RECORD_READ_STATE::OFFSET;
        qint32 nextReadSz = RECORD_SIZE::OFFSET;
        qint64 recordFileOffset = idx + IPSHeader.size();
        QSharedPointer<IPSRecord::ipsRecord> record;
        quint32 off, size, rleSize;

        while (true) {
            if (idx+nextReadSz > len) {
                setError(QString("invalid record at offset 0x%1!").arg(QString::number(recordFileOffset, 16)));
                return;
            }

            switch (state) {
                case OFFSET: {
                    off = byte3ToUint(file.at(idx), file.at(idx+1), file.at(idx+2));

                    idx += nextReadSz;
                    state = RECORD_READ_STATE::SIZE;
                    nextReadSz = RECORD_SIZE::SIZE;
                }
                    break;
                case SIZE: {
                    size = byte2ToUint(file.at(idx), file.at(idx+1));

                    idx += nextReadSz;
                    state = size == 0 ? RECORD_READ_STATE::RLE_SIZE : RECORD_READ_STATE::DATA;
                    nextReadSz = size == 0 ? RECORD_SIZE::RLE_SIZE : RECORD_SIZE::NONE;
                }
                    break;
                case RLE_SIZE: {
                     rleSize = byte2ToUint(file.at(idx), file.at(idx+1));

                     idx += nextReadSz;
                     state = RECORD_READ_STATE::DATA;
                     nextReadSz = RECORD_SIZE::RLE_VALUE;
                }
                    break;
                case DATA: {
                    if (size == 0) { // RLE
                        record = QSharedPointer<IPSRecord::rleRecord>::create(off, size, rleSize, file.mid(idx, RECORD_SIZE::RLE_VALUE));
                        idx += nextReadSz;

                    } else {
                        record = QSharedPointer<IPSRecord::record>::create(off, size, file.mid(idx, size));
                        idx += size;
                    }

                    state = RECORD_READ_STATE::END;
                }
                    break;
                default: {
                    setError(QString("invalid RECORD_READ_STATE: %1!").arg(QString::number(state)));
                    return;
                }
                    break;
            }

            if (state == RECORD_READ_STATE::END) {
                record->setRecordOffset(recordFileOffset);
                recordList.append(record);
                break;
            }
        }
    }
}

void IPS::setError(const QString &msg) {
    error = msg;
}

QString IPS::getError() {
    QString ret {error};

    error.clear();
    return ret;
}

// credits: https://zerosoft.zophar.net/ips.php
quint32 IPS::byte3ToUint(char b1, char b2, char b3) {
    return (((unsigned int) b1 << 16) & 0x00FF0000) |
            (((unsigned int) b2 << 8) & 0x0000FF00) |
            ((unsigned int) b3 & 0x000000FF);
}

// credits: https://zerosoft.zophar.net/ips.php
quint32 IPS::byte2ToUint(char b1, char b2) {
    return (((unsigned int) b1 << 8) & 0xFF00) |
            ((unsigned int) b2 & 0x00FF);
}

IPS::~IPS() {
    recordList.clear();
}

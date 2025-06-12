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
#include <QFile>

#include "IPS.h"
#include "RLERecord.h"
#include "Record.h"

namespace IPSV {
    void IPS::loadIPS(const QString &path) {
        QByteArray header, eof, truncExt;
        QFile ipsFile {path};

        if (ipsFile.size() > maxFileSize) {
            error = QString("Invalid IPS file!\n\nfile size: %1, max size: %2").arg(ipsFile.size(), maxFileSize);
            return;
        }

        if (!ipsFile.open(QIODevice::ReadOnly)) {
            error = ipsFile.errorString();
            return;
        }

        header = ipsFile.read(IPSHeader.size());
        if (header.size() < IPSHeader.size() || header.compare(IPSHeader) != 0) {
            error = "Invalid IPS file! Wrong header";
            return;
        }

        ipsFile.seek(ipsFile.size() - IPSEof.size());

        eof = ipsFile.read(IPSEof.size());
        if (eof.compare(IPSEof) != 0) {
            ipsFile.seek(ipsFile.size() - (IPSEof.size() + truncateExtSize));

            eof = ipsFile.read(IPSEof.size());
            if (eof.size() < IPSEof.size() || eof.compare(IPSEof) != 0) {
                error = "Invalid IPS file! Wrong EOF";
                return;
            }

            // if we are here, this has truncate extension
            truncExt = ipsFile.read(truncateExtSize);
            if (truncExt.size() < truncateExtSize) {
                error = "Invalid IPS file! Invalid extension size";
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
        const quint32 len = file.size() - IPSEof.size() - (truncateOffset != 0 ? truncateExtSize:0);
        quint32 idx = 0;

        while (idx < len) {
            const qint64 recordFileOffset = idx + IPSHeader.size();
            RECORD_READ_STATE state = RECORD_READ_STATE::OFFSET;
            int nextReadSz = recordOffsetSize;
            QSharedPointer<IPSV::IPSRecord> record;
            quint32 off, size, rleSize;

            while (true) {
                if (idx+nextReadSz > len) {
                    error = QString("invalid record at offset 0x%1!").arg(QString::number(recordFileOffset, 16));
                    return;
                }

                switch (state) {
                    case RECORD_READ_STATE::OFFSET: {
                        off = byte3ToUint(file.at(idx), file.at(idx+1), file.at(idx+2));

                        idx += nextReadSz;
                        state = RECORD_READ_STATE::SIZE;
                        nextReadSz = recordSize;
                    }
                        break;
                    case RECORD_READ_STATE::SIZE: {
                        size = byte2ToUint(file.at(idx), file.at(idx+1));

                        idx += nextReadSz;
                        state = size == 0 ? RECORD_READ_STATE::RLE_SIZE : RECORD_READ_STATE::DATA;
                        nextReadSz = size == 0 ? rleSize : 0;
                    }
                        break;
                    case RECORD_READ_STATE::RLE_SIZE: {
                         rleSize = byte2ToUint(file.at(idx), file.at(idx+1));

                         idx += nextReadSz;
                         state = RECORD_READ_STATE::DATA;
                         nextReadSz = rleValueSize;
                    }
                        break;
                    case RECORD_READ_STATE::DATA: {
                        if (size == 0) { // RLE
                            record = QSharedPointer<IPSV::RLERecord>::create(off, size, rleSize, file.mid(idx, rleValueSize));
                            idx += nextReadSz;

                        } else {
                            record = QSharedPointer<IPSV::Record>::create(off, size, file.mid(idx, size));
                            idx += size;
                        }

                        state = RECORD_READ_STATE::END;
                    }
                        break;
                    default: {
                        error = QString("invalid RECORD_READ_STATE: %1!").arg(QString::number(static_cast<int>(state)));
                        return;
                    }
                }

                if (state == RECORD_READ_STATE::END) {
                    record->setRecordOffset(recordFileOffset);
                    recordList.append(record);
                    break;
                }
            }
        }
    }

    QString IPS::getError() {
        QString ret {error};

        error.clear();
        return ret;
    }

    // credits: https://zerosoft.zophar.net/ips.php
    quint32 IPS::byte3ToUint(const char b1, const char b2, const char b3) const {
        return ((static_cast<uint>(b1) << 16) & 0x00FF0000) |
                ((static_cast<uint>(b2) << 8) & 0x0000FF00) |
                (static_cast<uint>(b3) & 0x000000FF);
    }

    // credits: https://zerosoft.zophar.net/ips.php
    quint32 IPS::byte2ToUint(const char b1, const char b2) const {
        return ((static_cast<uint>(b1) << 8) & 0xFF00) |
                (static_cast<uint>(b2) & 0x00FF);
    }
}

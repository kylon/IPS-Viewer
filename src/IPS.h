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

#include <QSharedPointer>
#include <QString>
#include <QObject>
#include <QList>

#include "IPSRecord.h"

namespace IPSV {
    class IPS final: public QObject {
        Q_OBJECT

    private:
        enum class RECORD_READ_STATE {
            OFFSET = 0,
            SIZE,
            RLE_SIZE,
            DATA,
            END
        };

        static constexpr quint64 maxFileSize = 0x1000000;
        static constexpr quint32 maxRecordSize = 0xFFFF;
        static constexpr int truncateExtSize = 3;
        static constexpr int recordSize = 2;
        static constexpr int recordOffsetSize = 3;
        static constexpr int rleSize = 2;
        static constexpr int rleValueSize = 1;
        const QByteArray IPSHeader {"\x50\x41\x54\x43\x48"};
        const QByteArray IPSEof {"\x45\x4f\x46"};
        quint32 truncateOffset = 0;
        QList<QSharedPointer<IPSRecord>> recordList;
        QString error;

        void readRecords(const QByteArray &file);
        quint32 byte3ToUint(char b1, char b2, char b3) const;
        quint32 byte2ToUint(char b1, char b2) const;

    public:
        void loadIPS(const QString &path);
        QString getError();

        bool hasError() const { return !error.isNull(); }
        void setTruncateOffset(const quint32 off) { truncateOffset = off; }
        quint32 getTruncateOffset() const { return truncateOffset; }
        QList<QSharedPointer<IPSRecord>> getRecords() const { return recordList; }

    signals:
        void ipsLoaded();
    };
}

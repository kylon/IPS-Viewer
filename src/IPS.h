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
#include <QList>
#include <QFile>

#include "Record.h"

namespace IPSV {
    class IPS final {
    private:
        static constexpr qint64 maxIPSSize = 134217728;
        quint32 truncateOffset = 0;
        QList<QSharedPointer<Record>> recordList;
        QString error;

        [[nodiscard]] quint32 getOffset(const QByteArray &offset) const;
        [[nodiscard]] quint16 getSize(const QByteArray &size) const;
        [[nodiscard]] bool isRealEof(const QFile &ips, const QByteArray &buf) const;

    public:
        [[nodiscard]] quint32 getTruncateOffset() const { return truncateOffset; }
        [[nodiscard]] QList<QSharedPointer<Record>> getRecords() const { return recordList; }
        [[nodiscard]] QByteArray getRecordData(const int idx) const { return recordList[idx]->data; }

        [[nodiscard]] bool loadIPS(const QString &path);
        [[nodiscard]] QString getError();
    };
}

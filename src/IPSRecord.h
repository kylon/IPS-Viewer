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

#include <QByteArray>

namespace IPSV {
    class IPSRecord {
    private:
        qint64 recordOffset = 0;

    public:
        virtual ~IPSRecord() = default;

        virtual quint32 getOffset() const = 0;
        virtual quint32 getSize() const = 0;
        virtual QByteArray getData() const = 0;
        virtual void setOffset(quint32) = 0;
        virtual void setSize(quint32) = 0;
        virtual void setData(const QByteArray &) = 0;

        void setRecordOffset(const qint64 roff) { recordOffset = roff; }
        qint64 getRecordOffset() const { return recordOffset; }
    };
}

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
#include "IPS.h"
#include "RLERecord.h"

namespace IPSV {
    // credits: https://zerosoft.zophar.net/ips.php
    quint32 IPS::getOffset(const QByteArray &offset) const {
        if (offset.size() < 3)
            return -1;

        return ((static_cast<unsigned>(offset[0]) << 16) & 0x00FF0000) |
                ((static_cast<unsigned>(offset[1]) << 8) & 0x0000FF00) |
                (static_cast<unsigned>(offset[2]) & 0x000000FF);
    }

    // credits: https://zerosoft.zophar.net/ips.php
    quint16 IPS::getSize(const QByteArray &size) const {
        if (size.size() < 2)
            return -1;

        return ((static_cast<unsigned>(size[0]) << 8) & 0xFF00) |
                (static_cast<unsigned>(size[1]) & 0x00FF);
    }

    bool IPS::isRealEof(const QFile &ips, const QByteArray &buf) const {
        if (buf != "EOF")
            return false;

        return (ips.size() - ips.pos() <= 3);
    }

    bool IPS::loadIPS(const QString &path) {
        QFile ips {path};

        if (ips.size() > maxIPSSize) {
            error = QString("Invalid IPS file!\n\nfile size: %1, max size: %2").arg(ips.size(), maxIPSSize);
            return false;
        }

        if (!ips.open(QIODevice::ReadOnly)) {
            error = ips.errorString();
            return false;
        }

        if (ips.read(5) != "PATCH") {
            error = "Invalid IPS file! wrong header";
            return false;
        }

        while (!ips.atEnd()) {
            const qint64 ipsOffset = ips.pos();
            const QByteArray offset = ips.read(3);
            QByteArray size;
            quint32 offsetI;
            quint16 sizeI;

            if (isRealEof(ips, offset)) {
                if (ips.size() - ips.pos() == 3) { // truncate extension
                    const QByteArray truncate = ips.read(3);

                    truncateOffset = getOffset(truncate);
                    if (truncateOffset == -1) {
                        error = QString("Failed to read truncate offset: read %1 of 3").arg(truncate.size());
                        return false;
                    }
                }

                break;
            }

            size = ips.read(2);

            offsetI = getOffset(offset);
            if (offsetI == -1) {
                error = QString("Failed to read record offset at offset %1").arg(ips.pos());
                return false;
            }

            sizeI = getSize(size);
            if (sizeI == -1) {
                error = QString("Failed to read record size at offset %1").arg(ips.pos());
                return false;
            }

            if (sizeI == 0) { // rle
                const QByteArray rleSize = ips.read(2);
                const quint16 rleSizeI = getSize(rleSize);
                QByteArray value;

                if (rleSizeI == -1) {
                    error = QString("Failed to read rle size at offset %1").arg(ips.pos());
                    return false;
                }

                value = ips.read(1);
                if (value.isEmpty()) {
                    error = QString("Failed to read rle value at offset %1").arg(ips.pos());
                    return false;
                }

                recordList.append(QSharedPointer<RLERecord>::create(ipsOffset, offsetI, rleSizeI, value));

            } else {
                const QByteArray data = ips.read(sizeI);

                if (data.size() != sizeI) {
                    error = QString("Failed to read patch data, read %1 of %2").arg(data.size()).arg(sizeI);
                    return false;
                }

                recordList.append(QSharedPointer<Record>::create(ipsOffset, offsetI, sizeI, data));
            }
        }

        return true;
    }

    QString IPS::getError() {
        QString ret {error};

        error.clear();
        return ret;
    }
}

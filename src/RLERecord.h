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

#include "IPSRecord.h"

namespace IPSV {
    class RLERecord final: public IPSRecord {
    private:
        quint32 _offset;
        quint32 _size;
        quint32 _rleSize;
        QByteArray _value;

    public:
        RLERecord(const quint32 off, const quint32 sz, const quint32 rleSz, const QByteArray &value) : _offset(off), _size(sz), _rleSize(rleSz), _value(value) {}

        quint32 getOffset() const override { return _offset; }
        quint32 getSize() const override { return _size; }
        quint32 getRleSize() const { return _rleSize; }
        QByteArray getData() const override { return _value; }
        void setOffset(const quint32 off) override { _offset = off; }
        void setSize(const quint32 sz) override { _size = sz; }
        void setRleSize(const quint32 rSz) { _rleSize = rSz; }
        void setData(const QByteArray &v) override { _value = v; }
    };
}

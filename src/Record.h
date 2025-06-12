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
    class Record final: public IPSRecord {
    private:
        quint32 offt;
        quint32 sz;
        QByteArray ba;

    public:
        Record(const quint32 offset, const quint32 size, const QByteArray &data) {
            offt = offset;
            sz = size;
            ba = data;
        }

        ~Record() override = default;

        quint32 getOffset() const override { return offt; }
        quint32 getSize() const override { return sz; }
        QByteArray getData() const override { return ba; }
        void setOffset(const quint32 offset) override { offt = offset; }
        void setSize(const quint32 size) override { sz = size; }
        void setData(const QByteArray &data) override { ba = data; }
    };
}

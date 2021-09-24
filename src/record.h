#pragma once

#include "ipsRecord.h"

namespace IPSRecord {
    class record: public ipsRecord {
    private:
        quint32 _offset;
        quint32 _size;
        QByteArray _data;

    public:
        record(quint32 off, quint32 sz, const QByteArray &data) : _offset(off), _size(sz), _data(data) {}

        quint32 getOffset() const { return _offset; }
        quint32 getSize() const { return _size; }
        QByteArray getData() const { return _data; }
        void setOffset(quint32 off) { _offset = off; }
        void setSize(quint32 sz) { _size = sz; }
        void setData(const QByteArray &d) { _data = d; }
    };
}


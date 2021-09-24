#pragma once

#include "ipsRecord.h"

namespace IPSRecord {
    class rleRecord: public ipsRecord {
    private:
        quint32 _offset;
        quint32 _size;
        quint32 _rleSize;
        QByteArray _value;

    public:
        rleRecord(quint32 off, quint32 sz, quint32 rleSz, const QByteArray &value) : _offset(off), _size(sz), _rleSize(rleSz), _value(value) {}

        quint32 getOffset() const { return _offset; }
        quint32 getSize() const { return _size; }
        quint32 getRleSize() const { return _rleSize; }
        QByteArray getData() const { return _value; }
        void setOffset(quint32 off) { _offset = off; }
        void setSize(quint32 sz) { _size = sz; }
        void setRleSize(quint32 rSz) { _rleSize = rSz; }
        void setData(const QByteArray &v) { _value = v; }
    };
}

#pragma once

#include <QByteArray>

namespace IPSRecord {
    class ipsRecord {
    protected:
        qint64 _recordOffset;

    public:
        virtual quint32 getOffset() const = 0;
        virtual quint32 getSize() const = 0;
        virtual QByteArray getData() const = 0;
        virtual void setOffset(quint32) = 0;
        virtual void setSize(quint32) = 0;
        virtual void setData(const QByteArray &) = 0;

        void setRecordOffset(qint64 roff) { _recordOffset = roff; }
        qint64 getRecordOffset() const { return _recordOffset; }
    };
}

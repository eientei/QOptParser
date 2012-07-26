#include "qoption.h"

QOption::QOption(const QChar &oshort, const QString &olong,
                 const QString &odescr, const QString &oargdescr)
    : QObject(0)
{
    m_short  = oshort;
    m_long   = olong;
    m_odescr = odescr;
    m_oargdescr = oargdescr;
    if (oargdescr.isNull()) {
        m_hasArg = false;
    } else {
        m_hasArg = true;
    }
}

QOption::QOption(const QChar &oshort, const QString &odescr, const QString &oargdescr)
    : QObject(0)
{
    m_short  = oshort;
    m_odescr = odescr;
    m_oargdescr = oargdescr;
    if (oargdescr.isNull()) {
        m_hasArg = false;
    } else {
        m_hasArg = true;
    }
}

QOption::QOption(const QString &olong, const QString &odescr, const QString &oargdescr)
    : QObject(0)
{
    m_long   = olong;
    m_odescr = odescr;
    m_oargdescr = oargdescr;
    if (oargdescr.isNull()) {
        m_hasArg = false;
    } else {
        m_hasArg = true;
    }
}

QOption::~QOption()
{
    // do nothing
}

const QChar &QOption::getShort() const
{
    return m_short;
}

const QString &QOption::getLong() const
{
    return m_long;
}

const QString &QOption::getDescr() const
{
    return m_odescr;
}

const QString QOption::getOArgDescr() const
{
    return m_oargdescr;
}

bool QOption::hasArg() const
{
    return m_hasArg;
}

void QOption::trigger(const QString &arg) const
{
    emit triggered(arg);
}

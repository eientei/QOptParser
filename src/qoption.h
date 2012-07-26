#ifndef QOPTION_H
#define QOPTION_H

#include "QOptParser_global.h"

#include <QObject>
#include <QString>
#include <QList>

class QOPTPARSERSHARED_EXPORT QOption : public QObject
{
    Q_OBJECT
public:
    QOption(const QChar &oshort, const QString &olong,
            const QString &odescr, const QString &oargdescr = QString());

    QOption(const QChar &oshort, const QString &odescr,
            const QString &oargdescr = QString());

    QOption(const QString &olong, const QString &odescr,
            const QString &oargdescr = QString());

    ~QOption();

    const QChar & getShort() const;
    const QString & getLong() const;
    const QString & getDescr() const;
    const QString getOArgDescr() const;
    bool hasArg() const;

    void trigger(const QString & arg = QString()) const;
    
signals:
    void triggered(const QString & arg = QString()) const;
    
private:
    QChar m_short;
    QString m_long;
    QString m_odescr;
    QString m_oargdescr;
    bool m_hasArg;
};

typedef QList<QOption*> QOptionList;

#endif // QOPTION_H

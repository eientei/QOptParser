#ifndef QOPTPARSER_H
#define QOPTPARSER_H

#include "qoption.h"
#include "QOptParser_global.h"

#include <QObject>
#include <QTextStream>
#include <QCoreApplication>
#include <QStringList>

class QOPTPARSERSHARED_EXPORT QOptParser : public QObject
{
    Q_OBJECT

    enum MatchType {
        ARGUMENT_IGNORED,
        ARGUMENT_CONSUMED,

    };

    struct Match {
        MatchType type;
        QObject * data;
    };

public:
    explicit QOptParser(QObject * parent = 0);
    QOptParser(const QCoreApplication & app, QObject * parent = 0);
    QOptParser(int argc, char ** argv, QObject * parent = 0);
    QOptParser(const QStringList & list, QObject * parent = 0);

    ~QOptParser();

    void addOption(QOption * option, const QString & group = QString());
    void setArguments(int argc, char ** argv);
    void setArguments(const QStringList & list);
    void setBanner(const QString & banner);

    void setBreakOnError(bool value);
    void setHelpOnEmpty(bool value);
    void setHelpOnError(bool value);
    void setStrictOptions(bool value);

    void setOutStream(QTextStream *stream);

    void produceHelp();

    bool parse();

    const QStringList & getLeftovers() const;


private:
    void fillSpaces(int n);
    QOption * matchOpt(const QString &str);
    QOption * matchOpt(const QChar &ch);
    void unknownOption(const QString &str);
    void unknownOption(const QChar &ch);
    void missingArg(const QString &str);
    void missingArg(const QChar &ch);
    void collideOptions(const QChar &ch, const QString &str);
    bool hasShortArgs(const QString &str);


    void initialize();

    QStringList m_arguments;
    QStringList m_leftovers;
    QMap<QString, QOptionList> m_options;
    QMap<QString, bool> m_unknownOptions;
    QString m_banner;
    QTextStream * m_outstream;
    bool m_helpOnEmpty;
    bool m_helpOnError;
    bool m_breakOnError;
    bool m_strictOptions;
};

#endif // QOPTPARSER_H

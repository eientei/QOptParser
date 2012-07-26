#include "qoptparser.h"

void QOptParser::initialize()
{
    m_breakOnError = false;
    m_helpOnEmpty = true;
    m_helpOnError = false;
    m_strictOptions = false;
    m_outstream = 0;
}

QOptParser::QOptParser(QObject *parent)
    : QObject(parent)
{
    initialize();
}

QOptParser::QOptParser(const QCoreApplication &app, QObject *parent)
    : QObject(parent)
{
    setArguments(app.arguments());
    initialize();
}

QOptParser::QOptParser(int argc, char **argv, QObject *parent)
    : QObject(parent)
{
    setArguments(argc,argv);
    initialize();
}

QOptParser::QOptParser(const QStringList &list, QObject *parent)
    : QObject(parent)
{
    setArguments(list);
    initialize();
}

QOptParser::~QOptParser()
{
}

void QOptParser::addOption(QOption *option, const QString &group)
{
    m_options[group].push_back(option);
}

void QOptParser::setArguments(int argc, char **argv)
{
    for (int i = 0; i < argc; i++) {
        m_arguments.append(argv[i]);
    }
}

void QOptParser::setArguments(const QStringList &list)
{
    m_arguments = list;
}

void QOptParser::setBanner(const QString &banner)
{
    m_banner = banner;
}

void QOptParser::setBreakOnError(bool value)
{
    m_breakOnError = value;
}

void QOptParser::setHelpOnEmpty(bool value)
{
    m_helpOnEmpty = value;
}

void QOptParser::setHelpOnError(bool value)
{
    m_helpOnError = value;
}

void QOptParser::setOutStream(QTextStream *stream)
{
    m_outstream = stream;
}

QStringList QOptParser::parse()
{
    bool wasStream = true;
    if (!m_outstream) {
        m_outstream = new QTextStream(stdout,QIODevice::WriteOnly);
        wasStream = false;
    }
    QStringList leftovers;
    int arglen = m_arguments.length();
    int i;
    for (i = 1; i < arglen; i++) {
        QString str = m_arguments[i];
        QString trimmed = str.trimmed();
        if (trimmed.startsWith("--")) {
            if (trimmed.length() == 2) {
                // end of options
                i++;
                break;
            }
            QString garg = trimmed.mid(2);
            if (QOption * opt = matchOpt(garg)) {
                if (opt->hasArg()) {
                    if ((i + 1) >= arglen) {
                        missingArg(opt->getLong());
                    } else {
                        i++;
                        opt->trigger(m_arguments[i]);
                    }
                } else {
                    opt->trigger();
                }
            } else {
                unknownOption(garg);
            }
        } else if (trimmed.startsWith("-")) {
            int optlen = trimmed.length();
            if (optlen == 1) {
                leftovers.append(str);
            }
            for (int n = 1; n < optlen; n++) {
                QChar ch = trimmed.at(n);
                if (QOption * opt = matchOpt(ch)) {
                    if (opt->hasArg()) {
                        if ((n+1) >= optlen) {
                            if ((i+1) >= arglen) {
                                missingArg(opt->getShort());
                            } else {
                                i++;
                                opt->trigger(m_arguments[i]);
                            }
                        } else {
                            QString rest = trimmed.mid(n+1);
                            if (hasShortArgs(rest) && m_strictOptions) {
                                collideOptions(opt->getShort(),rest);
                            } else {
                                opt->trigger(rest);
                                break; // breaks only inner cycle
                            }
                        }
                    } else {
                        opt->trigger();
                    }
                } else {
                    unknownOption(ch);
                }
            }
        } else {
            leftovers.append(str);
        }
    }

    for (; i < arglen; i++) {
        leftovers.append(m_arguments[i]);
    }

    if (!wasStream) {
        delete m_outstream;
    }

    return leftovers;
}

QOption *QOptParser::matchOpt(const QString &str)
{
    foreach (QOptionList group, m_options) {
        foreach (QOption *opt, group) {
            if (opt->getLong() == str) {
                return opt;
            }
        }
    }
    return 0;
}

QOption *QOptParser::matchOpt(const QChar &ch)
{
    foreach (QOptionList group, m_options) {
        foreach (QOption *opt, group) {
            if (opt->getShort() == ch) {
                return opt;
            }
        }
    }
    return 0;
}

void QOptParser::unknownOption(const QString &str)
{
    if (!m_unknownOptions.contains(str)) {
        (*m_outstream) << QString("Unknwon option `--%1'\n").arg(str);
        m_unknownOptions[str] = true;
    }

}

void QOptParser::unknownOption(const QChar &ch)
{
    if (!m_unknownOptions.contains(ch)) {
        (*m_outstream) << QString("Unknwon option `-%1'\n").arg(ch);
        m_unknownOptions[ch] = true;
    }
}

void QOptParser::missingArg(const QString &str)
{
    (*m_outstream) << QString("Missing argument for option `--%1'\n").arg(str);
}

void QOptParser::missingArg(const QChar &ch)
{
    (*m_outstream) << QString("Missing argument for option `-%1'\n").arg(ch);
}

void QOptParser::collideOptions(const QChar &ch, const QString &str)
{
    (*m_outstream) << QString("Wrong arg for option -%1: `%2'\n").arg(ch).arg(str);
}

bool QOptParser::hasShortArgs(const QString &str)
{
    foreach(QChar ch, str) {
        foreach (QOptionList group, m_options) {
            foreach (QOption *opt, group) {
                if (opt->getShort() == ch) {
                    return true;
                }
            }
        }
    }
    return false;
}

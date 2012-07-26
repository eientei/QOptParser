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

void QOptParser::produceHelp()
{
    bool wasStream = true;
    if (!m_outstream) {
        m_outstream = new QTextStream(stdout,QIODevice::WriteOnly);
        wasStream = false;
    }

    int maxLongLen = 0;
    int maxOptArgNameLen = 0;
    int maxDescrLen = 0;

    int leftMargin = 2;
    int rightMargin = 2;
    int termwidth = 72;

    int optSpace = 0;


    foreach (QOptionList group, m_options) {
        foreach (QOption *opt, group) {
            // TODO: probably inefficent
            if (opt->getLong().length() > maxLongLen) {
                maxLongLen = opt->getLong().length();
            }
            if (opt->getOArgDescr().length() > maxOptArgNameLen) {
                maxOptArgNameLen = opt->getOArgDescr().length();
            }
            if (opt->getDescr().length() > maxDescrLen) {
                maxDescrLen = opt->getDescr().length();
            }
        }
    }

    optSpace += leftMargin;       // obvious
    optSpace += 2;                // short option -s
    optSpace += 2;                // comma and space ", "
    optSpace += maxLongLen;       // obvious
    optSpace += 1;                // "=" or " "
    optSpace += maxOptArgNameLen; // obvious
    optSpace += rightMargin;      // obvious

    int descrLength = termwidth - optSpace;

    (*m_outstream) << m_banner;

    QMap<QString, QOptionList>::const_iterator it,end;
    end = m_options.end();
    for (it = m_options.begin(); it != end; it++) {
        (*m_outstream) << ">>> " << it.key() << endl;
        foreach(QOption * opt, it.value()) {
            fillSpaces(leftMargin);
            if (opt->getShort().isNull()) {
                fillSpaces(4); // "-o, "
            } else {
                (*m_outstream) << "-" << opt->getShort();
                if (opt->getLong().isEmpty()) {
                    fillSpaces(2);
                } else {
                    (*m_outstream) << ", ";
                }
            }
            if (opt->getLong().isEmpty()) {
                fillSpaces(maxLongLen + 2); // for "--"
            } else {
                (*m_outstream)  << "--" << opt->getLong();
                fillSpaces(maxLongLen - opt->getLong().length());
            }

            if (opt->hasArg()) {
                if (opt->getLong().isEmpty()) {
                    fillSpaces(1); // for "="
                    (*m_outstream) << opt->getOArgDescr();
                } else {
                    (*m_outstream) << "=" << opt->getOArgDescr();
                }
                fillSpaces(maxOptArgNameLen - opt->getOArgDescr().length() +1);
            } else {
                fillSpaces(maxOptArgNameLen+rightMargin); // +1 for "="
            }
            QStringList words = opt->getDescr().split(' ');
            int freeSpace = descrLength;
            bool first = true;
            foreach (QString word, words) {
                if ((freeSpace - word.length()) < 0) {
                    if (word.length() > descrLength) {
                        (*m_outstream) << word;
                    }
                    (*m_outstream) << endl;
                    fillSpaces(optSpace + 1);
                    first = true;
                    freeSpace = descrLength;
                } else {
                    freeSpace -= word.length();
                    if (first) {
                        first = false;
                    } else {
                        fillSpaces(1);
                    }
                    (*m_outstream) << word;
                }
            }

            (*m_outstream) << endl;
        }

    }

    if (!wasStream) {
        delete m_outstream;
    }
}

bool QOptParser::parse()
{
    bool success = true;
    bool wasStream = true;
    if (!m_outstream) {
        m_outstream = new QTextStream(stdout,QIODevice::WriteOnly);
        wasStream = false;
    }
    int arglen = m_arguments.length();
    int i;

    if (arglen <= 1 && m_helpOnEmpty) {
        produceHelp();
    }

    for (i = 1; i < arglen; i++) {
        QString str = m_arguments[i];
        QString trimmed = str.trimmed();
        if (trimmed.startsWith("--")) {
            if (trimmed.length() == 2) {
                // end of options
                i++;
                break;
            }
            QString garg = trimmed.mid(2,trimmed.indexOf('=')-2);
            if (QOption * opt = matchOpt(garg)) {
                if (opt->hasArg()) {
                    if (trimmed.contains('=')) {
                        opt->trigger(trimmed.mid(trimmed.indexOf('=')+1));
                    } else {
                        if ((i + 1) >= arglen) {
                            missingArg(opt->getLong());
                            success = false;
                        } else {
                            i++;
                        opt->trigger(m_arguments[i]);
                        }
                    }
                } else {
                    if (trimmed.contains('=')) {
                        m_leftovers.append(trimmed.mid(trimmed.indexOf('=')+1));
                    }
                    opt->trigger();
                }
            } else {
                unknownOption(garg);
                success = false;
            }
        } else if (trimmed.startsWith("-")) {
            int optlen = trimmed.length();
            if (optlen == 1) {
                m_leftovers.append(str);
            }
            for (int n = 1; n < optlen; n++) {
                QChar ch = trimmed.at(n);
                if (QOption * opt = matchOpt(ch)) {
                    if (opt->hasArg()) {
                        if ((n+1) >= optlen) {
                            if ((i+1) >= arglen) {
                                missingArg(opt->getShort());
                                success = false;
                            } else {
                                i++;
                                opt->trigger(m_arguments[i]);
                            }
                        } else {
                            QString rest = trimmed.mid(n+1);
                            if (hasShortArgs(rest) && m_strictOptions) {
                                collideOptions(opt->getShort(),rest);
                                success = false;
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
                    success = false;
                }
            }
        } else {
            m_leftovers.append(str);
        }
    }

    for (; i < arglen; i++) {
        m_leftovers.append(m_arguments[i]);
    }

    if (!wasStream) {
        delete m_outstream;
    }
    return success;
}

const QStringList &QOptParser::getLeftovers() const
{
    return m_leftovers;
}

void QOptParser::fillSpaces(int n)
{
    for (int i = 0; i < n; i++) {
        (*m_outstream) << ' ';
    }
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

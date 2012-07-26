#include "QOptParser"

#include <iostream>

int main(int argc, char ** argv) {
    QOptParser parser(argc,argv);

    QOption help('h',"help","help","abc");
    parser.addOption(&help);

    QStringList leftovers = parser.parse();
    std::cout << "Leftovers:" << std::endl;
    foreach (QString str, leftovers) {
        std::cout << "'" << str.toStdString() << "'" << std::endl;
    }


    return 0;
}

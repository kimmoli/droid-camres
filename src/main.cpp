#include <QtGui/QGuiApplication>
#include <QtGlobal>
#include <QFile>
#include <QTextStream>
#include <QScreen>

#include <sailfishapp.h>
#include "camres.h"

#define S(n) QString(" ").repeated(n)

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QRect screenGeometry = app.primaryScreen()->availableGeometry();

    QFile file;
    QTextStream *ts = NULL;
    bool toFile = false;

    printf("Camres version %s\n", APP_VERSION);
    printf("usage: camres {-o json-output-filename}\n\n");

    if (argc > 2)
    {
        if (QString(argv[1]).compare("-o") == 0 && QString(argv[2]).length() > 0)
        {
            file.setFileName(QString(argv[2]));

            if ( !file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
            {
                printf("Camres error: Could not create output file.\n");
                return EXIT_FAILURE;
            }
            toFile = true;
            ts = new QTextStream(&file);
            printf("Camres: Writing to file %s\n", qPrintable(file.fileName()));
        }
    }

    int i;
    Camres cr;

    QList<QPair<QString, int> > cameras = cr.getCameras();

    if (cameras.isEmpty())
    {
        printf("Camres error: No cameras found.\n");
        return EXIT_FAILURE;
    }

    if (toFile)
        *ts << "{" << endl;

    QStringList caps;
    caps << "image-capture-supported-caps";
    caps << "video-capture-supported-caps";
    caps << "viewfinder-supported-caps";

    for (i=0 ; i<cameras.size() ; i++)
    {
        int j,m;
        QList<QPair<QString, QStringList> > resolutions = cr.getResolutions(cameras.at(i).second, caps);

        if (resolutions.isEmpty())
        {
            printf("No resolutions found for %s (%d):\n", qPrintable(cameras.at(i).first), cameras.at(i).second);
            continue;
        }

        printf("Resolutions for %s:\n", qPrintable(cameras.at(i).first));

        if (toFile)
            *ts << S(4) << "\"" << cameras.at(i).first.split(" ").first().toLower() << "\":" << endl << S(4) << "{" << endl;

        for (j=0 ; j<resolutions.size() ; j++)
        {
            printf("%s resolutions:\n", qPrintable(resolutions.at(j).first.split("-").first()));

            QStringList res = resolutions.at(j).second;

            if (resolutions.at(j).first.startsWith("viewfinder"))
            {
                QString vfr43, vfr169;
                for (m=0 ; m<res.size() ; m++)
                {
                    if (qMin(screenGeometry.height(), screenGeometry.width()) ==
                        qMin(res.at(m).split("x").at(0).toInt(), res.at(m).split("x").at(1).toInt()) &&
                        qMax(screenGeometry.height(), screenGeometry.width()) >=
                        qMax(res.at(m).split("x").at(0).toInt(), res.at(m).split("x").at(1).toInt()))
                    {
                        if (Camres::aspectRatioForResolution(res.at(m)).compare("4:3") == 0 && vfr43.isEmpty())
                        {
                            vfr43 = res.at(m);
                        }
                        else if (Camres::aspectRatioForResolution(res.at(m)).compare("16:9") == 0 && vfr169.isEmpty())
                        {
                            vfr169 = res.at(m);
                        }
                    }

                    printf("%s (%s)\n", qPrintable(res.at(m)), qPrintable(Camres::aspectRatioForResolution(res.at(m))));
                }
                if (toFile)
                {
                    *ts << S(8) << "\"viewfinder\":" << endl << S(8) << "{" << endl
                        << S(12) << "\"viewfinderResolution_4_3\" : \"" << vfr43 << "\"," << endl
                        << S(12) << "\"viewfinderResolution_16_9\" : \"" << vfr169 << "\"" << endl
                        << S(8) << "}" << endl;
                }
            }
            else
            {
                if (toFile)
                    *ts << S(8) << "\"" << resolutions.at(j).first.split("-").first().toLower() << "\":" << endl << S(8) << "[" << endl;

                for (m=0 ; m<res.size() ; m++)
                {
                    printf("%s (%s)\n", qPrintable(res.at(m)), qPrintable(Camres::aspectRatioForResolution(res.at(m))));

                    if (toFile)
                        *ts << S(12) << "{ \"resolution\": \"" << res.at(m)
                           << "\", \"aspectRatio\": \"" << Camres::aspectRatioForResolution(res.at(m)) << "\" }"
                           << ((m == res.size()-1) ? "" : ",") << endl;
                }

                if (toFile)
                    *ts << S(8) << ((j == resolutions.size()-1) ? "]" : "],") << endl;
            }
        }
        if (toFile)
            *ts << S(4) << ((i == cameras.size()-1) ? "}" : "},") << endl;

    }
    if (toFile)
        *ts << "}" << endl;

    if (toFile)
        file.close();

    return EXIT_SUCCESS;
}

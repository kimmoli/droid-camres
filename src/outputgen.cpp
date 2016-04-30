#include <stdio.h>
#include <QFile>
#include <QTextStream>
#include <QRect>

#include "outputgen.h"
#include "camres.h"

#define S(n) QString(" ").repeated(n)

OutputGen::OutputGen(QObject *parent) :
    QObject(parent)
{
}

void OutputGen::dump(const QList<QPair<QString, int> > &cameras, const QList<QList<QPair<QString, QStringList> > > &resolutions)
{
    int i,j,m;

    for (i=0 ; i<cameras.size() ; i++)
    {
        if (resolutions.at(i).isEmpty())
        {
            fprintf(stderr, "Camres warning: No resolutions found for %s (%d):\n", qPrintable(cameras.at(i).first), cameras.at(i).second);
            continue;
        }

        printf("Resolutions for %s:\n", qPrintable(cameras.at(i).first));

        for (j=0 ; j<resolutions.at(i).size() ; j++)
        {
            printf("%s resolutions:\n", qPrintable(resolutions.at(i).at(j).first.split("-").first()));

            QStringList res = resolutions.at(i).at(j).second;

            for (m=0 ; m<res.size() ; m++)
            {
                printf("%s (%s)\n", qPrintable(res.at(m)), qPrintable(Camres::aspectRatioForResolution(res.at(m))));
            }
        }
    }
}

void OutputGen::makeJson(const QList<QPair<QString, int> > &cameras,
                         const QList<QList<QPair<QString, QStringList> > > &resolutions,
                         const QRect &screenGeometry,
                         const QString &filename)
{
    int i,j,m;
    QFile file;
    QTextStream *ts = NULL;
    QString vfr43, vfr169;

    file.setFileName(filename);

    if ( !file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
    {
        fprintf(stderr, "Camres error: Could not create output file.\n");
        return;
    }

    ts = new QTextStream(&file);

    fprintf(stderr, "Camres: Writing json to file %s\n", qPrintable(file.fileName()));

    for (i=0 ; i<cameras.size() ; i++)
    {
        if (resolutions.at(i).isEmpty())
        {
            continue;
        }

        *ts << S(4) << "\"" << cameras.at(i).first.split(" ").first().toLower() << "\":" << endl << S(4) << "{" << endl;

        for (j=0 ; j<resolutions.at(i).size() ; j++)
        {
            QStringList res = resolutions.at(i).at(j).second;

            if (resolutions.at(i).at(j).first.startsWith("viewfinder"))
            {
                for (m=0 ; m<res.size() ; m++)
                {
                    if (qMin(screenGeometry.height(), screenGeometry.width()) >=
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
                }
            }
            else
            {
                *ts << S(8) << "\"" << resolutions.at(i).at(j).first.split("-").first().toLower() << "\":" << endl << S(8) << "[" << endl;

                for (m=0 ; m<res.size() ; m++)
                {
                    *ts << S(12) << "{ \"resolution\": \"" << res.at(m)
                       << "\", \"aspectRatio\": \"" << Camres::aspectRatioForResolution(res.at(m)) << "\" }"
                       << ((m == res.size()-1) ? "" : ",") << endl;
                }

                *ts << S(8) << ((j == 0) ? "]," : "]") << endl;
            }
        }
        *ts << S(4) << "}," << endl;
    }

    *ts << S(4) << "\"viewfinder\":" << endl << S(4) << "{" << endl
        << S(8) << "\"viewfinderResolution_4_3\" : \"" << vfr43 << "\"," << endl
        << S(8) << "\"viewfinderResolution_16_9\" : \"" << vfr169 << "\"" << endl
        << S(4) << "}" << endl;
    *ts << "}" << endl;

    if (vfr43.isEmpty())
        fprintf(stderr, "Camres error: Not found suitable resolution for 4:3 viewfinder. Check output!\n");

    if (vfr169.isEmpty())
        fprintf(stderr, "Camres error: Not found suitable resolution for 16:9 viewfinder. Check output!\n");

    file.close();
}

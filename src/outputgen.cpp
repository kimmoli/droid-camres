#include <stdio.h>
#include <QFile>
#include <QTextStream>
#include <QMapIterator>
#include <QRect>

#include "outputgen.h"
#include "camres.h"

#include <QDebug>

#define S(n) QString(" ").repeated(n)

OutputGen::OutputGen(QObject *parent) :
    QObject(parent)
{
}

void OutputGen::dump(const QList<QPair<QString, int> > &cameras, const QList<QList<QPair<QString, QStringList> > > &resolutions)
{
    int i, j, m;

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
    int i, j, m;
    QFile file;
    QTextStream *ts = NULL;

    file.setFileName(filename);

    if ( !file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
    {
        fprintf(stderr, "Camres error: Could not create output file.\n");
        return;
    }

    ts = new QTextStream(&file);

    fprintf(stderr, "Camres: Writing json to file %s\n", qPrintable(file.fileName()));

    *ts << "{" << endl;

    for (i=0 ; i<cameras.size() ; i++)
    {
        if (resolutions.at(i).isEmpty())
        {
            continue;
        }

        if (i>0)
            *ts << "," << endl;

        *ts << S(4) << "\"" << cameras.at(i).first.split(" ").first().toLower() << "\":" << endl << S(4) << "{" << endl;

        for (j=0 ; j<resolutions.at(i).size() ; j++)
        {
            if (resolutions.at(i).at(j).first.startsWith("viewfinder"))
            {
                continue;
            }

            if (j>0)
                *ts << "," << endl;

            QStringList res = resolutions.at(i).at(j).second;

            *ts << S(8) << "\"" << resolutions.at(i).at(j).first.split("-").first().toLower() << "\":" << endl << S(8) << "[" << endl;

            for (m=0 ; m<res.size() ; m++)
            {
                *ts << S(12) << "{ \"resolution\": \"" << res.at(m) << "\", "
                   << "\"viewFinder\": \"" << Camres::findBestViewFinderForResolution(res.at(m), resolutions.at(i), screenGeometry) << "\", "
                   << "\"aspectRatio\": \"" << Camres::aspectRatioForResolution(res.at(m)) << "\" }"
                   << ((m == res.size()-1) ? "" : ",") << endl;
            }

            *ts << S(8) << "]";
        }

        *ts << endl;
        *ts << S(4) << "}";
    }

    *ts << endl;
    *ts << "}" << endl;

    file.close();
}

void OutputGen::makeCamhw(const QList<QPair<QString, int> > &cameras,
                          const QList<QList<QPair<QString, QStringList> > > &resolutions,
                          const QRect &screenGeometry,
                          const QString &filename)
{
    int i, j, m;

    QMap<QString, QString> map;

    QFile file;
    QTextStream *ts = NULL;
    file.setFileName(filename);

    if ( !file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
    {
        fprintf(stderr, "Camres error: Could not create output file.\n");
        return;
    }

    ts = new QTextStream(&file);

    fprintf(stderr, "Camres: Writing dconf settings to file %s\n", qPrintable(file.fileName()));

    QFile resfile("/usr/share/droid-camres/jolla-camera-hw-template.txt");
    QStringList camhwTemplate;

    if (!resfile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        fprintf(stderr, "Camres error: failed to open template\n");
        return;
    }

    QTextStream rests(&resfile);

    while(!rests.atEnd())
    {
        camhwTemplate.append(rests.readLine());
    }

    resfile.close();

    for (i=0 ; i<cameras.size() ; i++)
    {
        if (resolutions.at(i).isEmpty())
        {
            continue;
        }

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
                        if (Camres::aspectRatioForResolution(res.at(m)).compare("4:3") == 0 && map.value("@VF43RES@").isEmpty())
                        {
                            map.insert("@VF43RES@", res.at(m));
                        }
                        else if (Camres::aspectRatioForResolution(res.at(m)).compare("16:9") == 0 && map.value("@VF169RES@").isEmpty())
                        {
                            map.insert("@VF169RES@", res.at(m));
                        }
                    }
                }
            }
            else if (resolutions.at(i).at(j).first.startsWith("image"))
            {
                QString cam = cameras.at(i).first.left(3).toUpper();
                for (m=0 ; m<res.size() ; m++)
                {
                    if (Camres::aspectRatioForResolution(res.at(m)).compare("4:3") == 0 && map.value("@" + cam + "IMAGE43RES@").isEmpty())
                    {
                        map.insert("@" + cam + "IMAGE43RES@", res.at(m));
                    }
                    else if (Camres::aspectRatioForResolution(res.at(m)).compare("16:9") == 0 && map.value("@" + cam + "IMAGE169RES@").isEmpty())
                    {
                        map.insert("@" + cam + "IMAGE169RES@", res.at(m));
                    }
                }
            }
            else if (resolutions.at(i).at(j).first.startsWith("video"))
            {
                QString cam = cameras.at(i).first.left(3).toUpper();
                for (m=0 ; m<res.size() ; m++)
                {
                    if (Camres::aspectRatioForResolution(res.at(m)).compare("16:9") == 0 && map.value("@" + cam + "VIDEORES@").isEmpty())
                    {
                        map.insert("@" + cam + "VIDEORES@", res.at(m));
                    }
                }
            }
        }
    }

    QMapIterator<QString, QString> k(map);

    while (k.hasNext())
    {
        k.next();
        if (k.value().isEmpty())
            fprintf(stderr, "Camres error: Not found suitable resolution for %s. Check output!\n", qPrintable(k.key()));
        else
            camhwTemplate.replaceInStrings(k.key(), k.value());
    }

    for (i=0 ; i<camhwTemplate.size() ; i++)
        *ts << camhwTemplate.at(i) << endl;

    file.close();
}

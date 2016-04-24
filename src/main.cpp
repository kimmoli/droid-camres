#include <QtGui/QGuiApplication>
#include <QtQml>
#include <QQuickView>
#include <QQmlEngine>
#include <QQmlContext>
#include <QScopedPointer>
#include <QTimer>
#include <QResource>

#include <sailfishapp.h>

#include "camres.h"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    int i;
    Camres cr;

    QList<QPair<QString, int> > cameras = cr.getCameras();

    if (cameras.isEmpty())
    {
        printf("Camres error: No cameras found.\n");
        return EXIT_FAILURE;
    }

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

        for (j=0 ; j<resolutions.size() ; j++)
        {
            printf("%s\n", qPrintable(resolutions.at(j).first));
            QStringList res = resolutions.at(j).second;
            for (m=0 ; m<res.size() ; m++)
            {
                printf("%s\n", qPrintable(res.at(m)));
            }
        }
    }

    return EXIT_SUCCESS;
}

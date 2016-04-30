#include <stdio.h>
#include <QtGui/QGuiApplication>
#include <QtGlobal>
#include <QScreen>

#include <sailfishapp.h>
#include "camres.h"
#include "outputgen.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QString jsonFilename = QString();
    bool printUsage = true;

    fprintf(stderr, "Camres version %s\n", APP_VERSION);

    if (argc == 1)
        printUsage = false;

    if (argc > 2)
    {
        if (QString(argv[1]).compare("-o") == 0 && QString(argv[2]).length() > 0)
        {
            jsonFilename = QString(argv[2]);
            printUsage = false;
        }
    }

    if (printUsage)
    {
        fprintf(stderr, "Usage: camres [OPTION]\n\n");
        fprintf(stderr, "  -o [filename]       Write output in json to file, for camera-settings-plugin\n");

        return EXIT_FAILURE;
    }

    int i;
    Camres cr;

    fprintf(stderr, "Searching cameras...\n");

    QList<QPair<QString, int> > cameras = cr.getCameras();

    if (cameras.isEmpty())
    {
        fprintf(stderr, "Camres error: No cameras found.\n");
        return EXIT_FAILURE;
    }

    QStringList caps;
    caps << "image-capture-supported-caps";
    caps << "video-capture-supported-caps";
    caps << "viewfinder-supported-caps";

    QList<QList<QPair<QString, QStringList> > > resolutions;

    for (i=0 ; i<cameras.size() ; i++)
    {
        fprintf(stderr, "Searching resolutions for %s...\n", qPrintable(cameras.at(i).first));
        resolutions.append(cr.getResolutions(cameras.at(i).second, caps));
    }

    fprintf(stderr, "\n");

    OutputGen og;

    og.dump(cameras, resolutions);

    if (!jsonFilename.isEmpty())
        og.makeJson(cameras, resolutions, app.primaryScreen()->availableGeometry(), jsonFilename);

    return EXIT_SUCCESS;
}

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
    int cam = 0;
    if (argc > 1)
    {
        if (sscanf (argv[1], "%i", &cam)!=1)
        {
            cam = 0;
        }
    }

    Camres cr;
    cr.scan(cam);

    return 0;
}

#ifndef OUTPUTGEN_H
#define OUTPUTGEN_H

#include <QObject>

class OutputGen : public QObject
{
    Q_OBJECT
public:
    explicit OutputGen(QObject *parent = 0);

    void dump(const QList<QPair<QString, int> >& cameras,
              const QList<QList<QPair<QString, QStringList> > >& resolutions);

    void makeJson(const QList<QPair<QString, int> >& cameras,
                  const QList<QList<QPair<QString, QStringList> > >& resolutions,
                  const QRect& screenGeometry,
                  const QString& filename);
};

#endif // OUTPUTGEN_H

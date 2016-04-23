#ifndef CAMRES_H
#define CAMRES_H
#include <QObject>

#include <gst/gst.h>

class Q_DECL_EXPORT Camres : public QObject
{
    Q_OBJECT

public:
    explicit Camres(QObject *parent = 0);
    virtual ~Camres();
    void scan(int cam);
    QList<QSize> parse(GstCaps *caps);
};


#endif // CAMRES_H

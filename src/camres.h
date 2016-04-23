#ifndef CAMRES_H
#define CAMRES_H
#include <QObject>

class Q_DECL_EXPORT Camres : public QObject
{
    Q_OBJECT

public:
    explicit Camres(QObject *parent = 0);
    virtual ~Camres();
    void scan();

};


#endif // CAMRES_H

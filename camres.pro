TARGET = camres

CONFIG += sailfishapp
CONFIG += link_pkgconfig
PKGCONFIG += gstreamer-1.0

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

SOURCES += \
    src/camres.cpp \
    src/main.cpp

HEADERS += \
    src/camres.h

OTHER_FILES += \
    rpm/camres.spec

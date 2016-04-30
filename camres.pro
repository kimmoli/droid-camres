TARGET = camres

CONFIG += sailfishapp
CONFIG += link_pkgconfig
PKGCONFIG += gstreamer-1.0 gstreamer-pbutils-1.0

gep.files = video.gep
gep.path = /usr/share/camres

INSTALLS += gep

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

SOURCES += \
    src/camres.cpp \
    src/main.cpp \
    src/outputgen.cpp

HEADERS += \
    src/camres.h \
    src/outputgen.h

OTHER_FILES += \
    rpm/camres.spec \
    video.gep

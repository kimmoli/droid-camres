TARGET = droid-camres

CONFIG += sailfishapp
CONFIG += link_pkgconfig
PKGCONFIG += gstreamer-1.0 gstreamer-pbutils-1.0

other.files = video.gep jolla-camera-hw-template.txt
other.path = /usr/share/camres

INSTALLS += other

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

SOURCES += \
    src/camres.cpp \
    src/main.cpp \
    src/outputgen.cpp

HEADERS += \
    src/camres.h \
    src/outputgen.h

OTHER_FILES += \
    rpm/droid-camres.spec \
    video.gep \
    jolla-camera-hw-template.txt

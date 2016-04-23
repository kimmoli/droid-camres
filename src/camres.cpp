#include "camres.h"
#include <QDir>
#include <QDebug>
#include <QSize>

#include <gst/pbutils/encoding-profile.h>
#include <gst/pbutils/encoding-target.h>

Camres::Camres(QObject *parent) :
    QObject(parent)
{
    qDebug() << "hoplaa!";
    gst_init(0, 0);
}

Camres::~Camres()
{
}

void Camres::scan(int cam)
{
    QList<QPair<QString, QVariant> > devices;

    // Too bad there's no way to get the values of an enum without creating the element :(
    GstElement *elem = gst_element_factory_make("droidcamsrc", NULL);
    if (!elem)
    {
        qWarning() << "QtCamScanner: Failed to create an instance of droidcamsrc";
        return;
    }

    GParamSpec *spec = g_object_class_find_property(G_OBJECT_GET_CLASS(elem), "camera-device");
    if (!spec)
    {
        qWarning() << "QtCamScanner: Failed to get property camera-device";
        gst_object_unref(elem);
        return;
    }

    if (!G_IS_PARAM_SPEC_ENUM(spec))
    {
        qWarning() << "QtCamScanner: Property camera-device is not an enum";
        gst_object_unref(elem);
        return;
    }

    GParamSpecEnum *e = G_PARAM_SPEC_ENUM(spec);
    // First add the default:
    devices << qMakePair<QString, QVariant>(e->enum_class->values[e->default_value].value_name,
                        QByteArray::number(e->default_value));

    for (int x = e->enum_class->minimum; x <= e->enum_class->maximum; x++)
    {
        if (x != e->default_value)
        {
            devices << qMakePair<QString, QVariant>(e->enum_class->values[x].value_name, QByteArray::number(x));
        }
    }

    qDebug() << "Found" << devices.size() << "cameras:" << devices;

    if (cam >= devices.size())
    {
        qWarning() << "too large device number";
        gst_object_unref(elem);
        return;
    }

    GstElement *cameraBin = gst_element_factory_make("camerabin", NULL);
    if (!cameraBin)
    {
        qWarning() << "failed to create cameraBin";
        gst_object_unref(elem);
        return;
    }

    qDebug() << "creating videosource";
    GstElement *videoSource = gst_element_factory_make("droidcamsrc", NULL);
    if (!videoSource)
    {
        qWarning() << "failed to create videoSource";
        gst_object_unref(elem);
        gst_object_unref(cameraBin);
        return;
    }

    qDebug() << "setting camera source";
    g_object_set(cameraBin, "camera-source", videoSource, NULL);

    qDebug() << "using device" << devices.at(cam).second.toInt() << devices.at(cam).first;
    g_object_set(videoSource, "camera-device", devices.at(cam).second.toInt(), NULL);

    qDebug() << "create fakeviewfinder";
    GstElement *fakeviewfinder = gst_element_factory_make("fakesink", NULL);
    if (!fakeviewfinder)
    {
        {
            qWarning() << "failed to create fake viewfinder";
            gst_object_unref(elem);
            gst_object_unref(cameraBin);
            return;
        }
    }

    qDebug() << "setting viewfinder";
    g_object_set(cameraBin, "viewfinder-sink", fakeviewfinder, NULL);

    qDebug() << "setting video profile";

    GError *error = NULL;
    GstEncodingTarget *target = gst_encoding_target_load_from_file("/etc/video.gep", &error);

    if (!target)
    {
        qCritical() << "Failed to load encoding target" << error->message;
        g_error_free(error);
        return;
    }

    GstEncodingProfile *profile = gst_encoding_target_get_profile(target, "video-profile");
    if (!profile)
    {
        qCritical() << "Failed to load encoding profile";
        gst_encoding_target_unref(target);
        return;
    }

    gst_encoding_target_unref(target);

    g_object_set(cameraBin, "video-profile", profile, NULL);

    qDebug() << "playing";
    if (gst_element_set_state (GST_ELEMENT (cameraBin), GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE)
    {
        qWarning() << "failed to play";
    }

    qDebug() << "getting caps";
    GstCaps *caps = NULL;

    int i;

    g_object_get(cameraBin, "image-capture-supported-caps", &caps, NULL);
    QList<QSize> imageResolutions = parse(caps);
    qDebug() << "Supported image resolutions.";
    for (i=0; i<imageResolutions.size() ; i++)
        qDebug() << imageResolutions.at(i);

    g_object_get(cameraBin, "video-capture-supported-caps", &caps, NULL);
    QList<QSize> videoResolutions = parse(caps);
    qDebug() << "Supported video resolutions:";
    for (i=0; i<videoResolutions.size() ; i++)
        qDebug() << videoResolutions.at(i);

    g_object_get(cameraBin, "viewfinder-supported-caps", &caps, NULL);
    QList<QSize> viewfinderResolutions = parse(caps);
    qDebug() << "Supported viewfinder resolutions:";
    for (i=0; i<viewfinderResolutions.size() ; i++)
        qDebug() << viewfinderResolutions.at(i);

    gst_caps_unref(caps);
    gst_object_unref(elem);
}

QList<QSize> Camres::parse(GstCaps *caps)
{
    QList<QSize> res;

    if (!caps)
    {
        qWarning() << "no caps";
        return res;
    }

    for (guint x = 0; x < gst_caps_get_size(caps); x++)
    {
        const GstStructure *s = gst_caps_get_structure(caps, x);
        const GValue *width = gst_structure_get_value(s, "width");
        const GValue *height = gst_structure_get_value(s, "height");

        if (!width || !height)
        {
            qWarning() << "queryResolutions: no dimensions";
            continue;
        }

        bool width_is_list = GST_VALUE_HOLDS_LIST(width) ? true : false;
        bool height_is_list = GST_VALUE_HOLDS_LIST(height) ? true : false;

        if (!width_is_list && !height_is_list)
        {
            if (!res.contains(QSize(g_value_get_int(width), g_value_get_int(height))))
                res << QSize(g_value_get_int(width), g_value_get_int(height));
        }
        else if (width_is_list && height_is_list)
        {
            guint ws = gst_value_list_get_size(width);
            guint hs = gst_value_list_get_size(width);
            for (guint wc = 0; wc < ws; wc++)
            {
                int w = g_value_get_int(gst_value_list_get_value(width, wc));
                for (guint hc = 0; hc < hs; hc++)
                {
                    int h = g_value_get_int(gst_value_list_get_value(height, hc));
                    if (!res.contains(QSize(w, h)))
                        res << QSize(w, h);
                }
            }
        }
        else if (width_is_list)
        {
            int h = g_value_get_int(height);
            for (guint i = 0; i < gst_value_list_get_size(width); i++)
            {
                int w = g_value_get_int(gst_value_list_get_value(width, i));
                if (!res.contains(QSize(w, h)))
                    res << QSize(w, h);
            }
        }
        else if (height_is_list)
        {
            int w = g_value_get_int(width);
            for (guint i = 0; i < gst_value_list_get_size(height); i++)
            {
                int h = g_value_get_int(gst_value_list_get_value(height, i));
                if (!res.contains(QSize(w, h)))
                    res << QSize(w, h);
            }
        }
    }

    return res;
}


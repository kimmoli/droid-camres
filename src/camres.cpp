#include "camres.h"
#include <unistd.h>
#include <math.h>
#include <QDir>
#include <QDebug>
#include <QRect>

#include <gst/pbutils/encoding-profile.h>
#include <gst/pbutils/encoding-target.h>

Camres::Camres(QObject *parent) :
    QObject(parent)
{
    gst_init(0, 0);
}

Camres::~Camres()
{
}

QList<QPair<QString, int> > Camres::getCameras()
{
    QList<QPair<QString, int> > res;

    GstElement *elem = gst_element_factory_make("droidcamsrc", NULL);
    if (!elem)
    {
        fprintf(stderr, "Camres error: Failed to create an instance of droidcamsrc.\n");
        return res;
    }

    GParamSpec *spec = g_object_class_find_property(G_OBJECT_GET_CLASS(elem), "camera-device");
    if (!spec)
    {
        fprintf(stderr, "Camres error: Failed to get property camera-device\n");
        gst_object_unref(elem);
        return res;
    }

    if (!G_IS_PARAM_SPEC_ENUM(spec))
    {
        fprintf(stderr, "Camres error: Property camera-device is not an enum.\n");
        gst_object_unref(elem);
        return res;
    }

    GParamSpecEnum *e = G_PARAM_SPEC_ENUM(spec);

    res << qMakePair<QString, int>(e->enum_class->values[e->default_value].value_name, (int)e->default_value);

    for (int x = e->enum_class->minimum; x <= e->enum_class->maximum; x++)
    {
        if (x != e->default_value)
        {
            res << qMakePair<QString, int>(e->enum_class->values[x].value_name, x);
        }
    }

    gst_object_unref(elem);

    return res;
}


QList<QPair<QString, QStringList> > Camres::getResolutions(int cam, QStringList whichCaps)
{
    QList<QPair<QString, QStringList> > res;

    GstElement *cameraBin = gst_element_factory_make("camerabin", NULL);

    if (!cameraBin)
    {
        fprintf(stderr, "Camres error: Failed to create camerabin.\n");
        return res;
    }

    GstElement *videoSource = gst_element_factory_make("droidcamsrc", NULL);
    if (!videoSource)
    {
        fprintf(stderr, "Camres error: Failed to create videoSource.\n");
        gst_object_unref(cameraBin);
        return res;
    }

    g_object_set(cameraBin, "camera-source", videoSource, NULL);
    g_object_set(videoSource, "camera-device", cam, NULL);

    GstElement *fakeviewfinder = gst_element_factory_make("fakesink", NULL);
    if (!fakeviewfinder)
    {
        {
            fprintf(stderr, "Camres error: Failed to create fake viewfinder.\n");
            gst_object_unref(videoSource);
            gst_object_unref(cameraBin);
            return res;
        }
    }

    g_object_set(cameraBin, "viewfinder-sink", fakeviewfinder, NULL);

    GError *error = NULL;
    GstEncodingTarget *target = gst_encoding_target_load_from_file("/usr/share/camres/video.gep", &error);

    if (!target)
    {
        fprintf(stderr, "Camres error: Failed to load encoding target: %s\n", qPrintable(error->message));
        g_error_free(error);
        gst_object_unref(fakeviewfinder);
        gst_object_unref(videoSource);
        gst_object_unref(cameraBin);
        return res;
    }

    GstEncodingProfile *profile = gst_encoding_target_get_profile(target, "video-profile");
    if (!profile)
    {
        fprintf(stderr, "Camres error: Failed to load encoding profile.\n");
        gst_object_unref(fakeviewfinder);
        gst_object_unref(videoSource);
        gst_object_unref(cameraBin);
        gst_encoding_target_unref(target);
        return res;
    }

    gst_encoding_target_unref(target);

    g_object_set(cameraBin, "video-profile", profile, NULL);

    if (gst_element_set_state (GST_ELEMENT (cameraBin), GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE)
    {
        fprintf(stderr, "Camres error: Failed to start playback.\n");
        gst_object_unref(fakeviewfinder);
        gst_object_unref(videoSource);
        gst_object_unref(cameraBin);
        return res;
    }

    GstCaps *caps = NULL;

    int i;

    for (i=0 ; i<whichCaps.size() ; i++)
    {
        g_object_get(cameraBin, whichCaps.at(i).toLatin1().constData(), &caps, NULL);
        res.append(qMakePair<QString, QStringList>(whichCaps.at(i), parse(caps)));
    }

    gst_caps_unref(caps);
    gst_element_set_state (GST_ELEMENT (cameraBin), GST_STATE_NULL);
    gst_object_unref(fakeviewfinder);
    gst_object_unref(videoSource);
    gst_object_unref(cameraBin);

    return res;
}

QStringList Camres::parse(GstCaps *caps)
{
    QStringList res;
    QString tmp;

    if (!caps)
    {
        return res;
    }

    for (guint x = 0; x < gst_caps_get_size(caps); x++)
    {
        const GstStructure *s = gst_caps_get_structure(caps, x);
        const GValue *width = gst_structure_get_value(s, "width");
        const GValue *height = gst_structure_get_value(s, "height");

        if (!width || !height)
        {
            continue;
        }

        bool width_is_list = GST_VALUE_HOLDS_LIST(width) ? true : false;
        bool height_is_list = GST_VALUE_HOLDS_LIST(height) ? true : false;

        if (!width_is_list && !height_is_list)
        {
            tmp = QString("%1x%2").arg(g_value_get_int(width)).arg(g_value_get_int(height));
            if (!res.contains(tmp))
                res.append(tmp);
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
                    tmp = QString("%1x%2").arg(w).arg(h);
                    if (!res.contains(tmp))
                        res.append(tmp);
                }
            }
        }
        else if (width_is_list)
        {
            int h = g_value_get_int(height);
            for (guint i = 0; i < gst_value_list_get_size(width); i++)
            {
                int w = g_value_get_int(gst_value_list_get_value(width, i));
                tmp = QString("%1x%2").arg(w).arg(h);
                if (!res.contains(tmp))
                    res.append(tmp);
            }
        }
        else if (height_is_list)
        {
            int w = g_value_get_int(width);
            for (guint i = 0; i < gst_value_list_get_size(height); i++)
            {
                int h = g_value_get_int(gst_value_list_get_value(height, i));
                tmp = QString("%1x%2").arg(w).arg(h);
                if (!res.contains(tmp))
                    res.append(tmp);
            }
        }
    }

    return res;
}

QString Camres::aspectRatioForResolution(const QString& size)
{
    static QMap<float, QString> ratios;
    int width, height;

    width = size.split("x").at(0).toInt();
    height = size.split("x").at(1).toInt();

    if (ratios.isEmpty())
    {
        ratios[1.0] = "1:1";
        ratios[1.2] = "5:4";
        ratios[1.3] = "4:3";
        ratios[1.5] = "3:2";
        ratios[1.6] = "16:10";
        ratios[1.7] = "16:9";
        ratios[1.8] = "9:5";
    }

    float r = (width * 1.0) / height;
    r = floor(r * 10) / 10.0;

    for (QMap<float, QString>::const_iterator iter = ratios.constBegin(); iter != ratios.constEnd(); iter++)
    {
        if (qFuzzyCompare (r, iter.key()))
        {
            return iter.value();
        }
    }

    fprintf(stderr, "Camres error: Could not find aspect ratio for %dx%d\n", width, height);

    return QString("?:?");
}

QString Camres::findBestViewFinderForResolution(const QString& size, const QList<QPair<QString, QStringList> > &resolutions, const QRect &screenGeometry)
{
    int width, height;

    int j, m;

    for (j=0 ; j<resolutions.size(); j++)
    {
        if (resolutions.at(j).first.startsWith("viewfinder"))
        {
            for (m=0 ; m<resolutions.at(j).second.size(); m++)
            {
                width = resolutions.at(j).second.at(m).split("x").at(0).toInt();
                height = resolutions.at(j).second.at(m).split("x").at(1).toInt();

                if (qMin(screenGeometry.height(), screenGeometry.width()) >=
                    qMin(width, height) &&
                    qMax(screenGeometry.height(), screenGeometry.width()) >=
                    qMax(width, height))
                {
                    if (Camres::aspectRatioForResolution(resolutions.at(j).second.at(m)).compare(Camres::aspectRatioForResolution(size)) == 0)
                    {
                        return resolutions.at(j).second.at(m);
                    }
                }
            }
        }
    }

    fprintf(stderr, "Camres error: Could not find viewfinder for %s\n", qPrintable(size));

    return QString("?:?");
}

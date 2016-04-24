camres
======

Gets camera resolutions for droidcam

Optionally outputs json file for camera-settings-plugin

    [nemo@localhost ~]$ camres -o camera-resolutions.json
    Camres version 0.0.3
    usage: camres {-o json-output-filename}

    Camres: Writing to file camera-resolutions.json
    Resolutions for Primary camera:
    image resolutions:
    4160x3120 (4:3)
    4000x3000 (4:3)
    4096x2160 (9:5)
    3840x2160 (16:9)
    3264x2448 (4:3)
    3120x3120 (1:1)
    2592x1944 (4:3)
    2048x1536 (4:3)
    1920x1080 (16:9)
    1600x1200 (4:3)
    1280x960 (4:3)
    1280x768 (16:10)
    1280x720 (16:9)
    1088x1088 (1:1)
    1024x768 (4:3)
    800x600 (4:3)
    800x480 (16:10)
    720x480 (3:2)
    640x480 (4:3)
    352x288 (5:4)
    320x240 (4:3)
    video resolutions:
    1920x1080 (16:9)
    1280x720 (16:9)
    864x480 (9:5)
    800x480 (16:10)
    720x480 (3:2)
    640x480 (4:3)
    480x320 (3:2)
    352x288 (5:4)
    320x240 (4:3)
    176x144 (5:4)
    viewfinder resolutions:
    2048x1536 (4:3)
    1920x1080 (16:9)
    1440x1080 (4:3)
    1280x960 (4:3)
    1280x720 (16:9)
    1088x1088 (1:1)
    960x720 (4:3)
    864x480 (9:5)
    800x480 (16:10)
    768x432 (16:9)
    736x736 (1:1)
    720x480 (3:2)
    640x640 (1:1)
    640x480 (4:3)
    576x432 (4:3)
    480x320 (3:2)
    384x288 (4:3)
    352x288 (5:4)
    320x240 (4:3)
    240x160 (3:2)
    176x144 (5:4)
    Resolutions for Secondary camera:
    image resolutions:
    3264x2448 (4:3)
    2592x1944 (4:3)
    2048x1536 (4:3)
    1920x1080 (16:9)
    1600x1200 (4:3)
    1280x960 (4:3)
    1280x720 (16:9)
    1280x768 (16:10)
    1024x768 (4:3)
    1088x1088 (1:1)
    800x600 (4:3)
    800x480 (16:10)
    720x480 (3:2)
    640x480 (4:3)
    352x288 (5:4)
    320x240 (4:3)
    video resolutions:
    1920x1080 (16:9)
    1280x720 (16:9)
    864x480 (9:5)
    800x480 (16:10)
    720x480 (3:2)
    640x480 (4:3)
    480x320 (3:2)
    352x288 (5:4)
    320x240 (4:3)
    176x144 (5:4)
    viewfinder resolutions:
    2048x1536 (4:3)
    1920x1080 (16:9)
    1440x1080 (4:3)
    1280x960 (4:3)
    1280x720 (16:9)
    960x720 (4:3)
    1088x1088 (1:1)
    864x480 (9:5)
    800x480 (16:10)
    720x480 (3:2)
    768x432 (16:9)
    576x432 (4:3)
    736x736 (1:1)
    640x640 (1:1)
    640x480 (4:3)
    480x320 (3:2)
    384x288 (4:3)
    352x288 (5:4)
    320x240 (4:3)
    240x160 (3:2)
    176x144 (5:4)


camera-resolutions.json
=======================

    {
        "primary":
        {
            "image":
            [
                { "resolution": "4160x3120", "aspectRatio": "4:3" },
                { "resolution": "4000x3000", "aspectRatio": "4:3" },
                { "resolution": "4096x2160", "aspectRatio": "9:5" },
                { "resolution": "3840x2160", "aspectRatio": "16:9" },
                { "resolution": "3264x2448", "aspectRatio": "4:3" },
                { "resolution": "3120x3120", "aspectRatio": "1:1" },
                { "resolution": "2592x1944", "aspectRatio": "4:3" },
                { "resolution": "2048x1536", "aspectRatio": "4:3" },
                { "resolution": "1920x1080", "aspectRatio": "16:9" },
                { "resolution": "1600x1200", "aspectRatio": "4:3" },
                { "resolution": "1280x960", "aspectRatio": "4:3" },
                { "resolution": "1280x768", "aspectRatio": "16:10" },
                { "resolution": "1280x720", "aspectRatio": "16:9" },
                { "resolution": "1088x1088", "aspectRatio": "1:1" },
                { "resolution": "1024x768", "aspectRatio": "4:3" },
                { "resolution": "800x600", "aspectRatio": "4:3" },
                { "resolution": "800x480", "aspectRatio": "16:10" },
                { "resolution": "720x480", "aspectRatio": "3:2" },
                { "resolution": "640x480", "aspectRatio": "4:3" },
                { "resolution": "352x288", "aspectRatio": "5:4" },
                { "resolution": "320x240", "aspectRatio": "4:3" }
            ],
            "video":
            [
                { "resolution": "1920x1080", "aspectRatio": "16:9" },
                { "resolution": "1280x720", "aspectRatio": "16:9" },
                { "resolution": "864x480", "aspectRatio": "9:5" },
                { "resolution": "800x480", "aspectRatio": "16:10" },
                { "resolution": "720x480", "aspectRatio": "3:2" },
                { "resolution": "640x480", "aspectRatio": "4:3" },
                { "resolution": "480x320", "aspectRatio": "3:2" },
                { "resolution": "352x288", "aspectRatio": "5:4" },
                { "resolution": "320x240", "aspectRatio": "4:3" },
                { "resolution": "176x144", "aspectRatio": "5:4" }
            ]
        },
        "secondary":
        {
            "image":
            [
                { "resolution": "3264x2448", "aspectRatio": "4:3" },
                { "resolution": "2592x1944", "aspectRatio": "4:3" },
                { "resolution": "2048x1536", "aspectRatio": "4:3" },
                { "resolution": "1920x1080", "aspectRatio": "16:9" },
                { "resolution": "1600x1200", "aspectRatio": "4:3" },
                { "resolution": "1280x960", "aspectRatio": "4:3" },
                { "resolution": "1280x720", "aspectRatio": "16:9" },
                { "resolution": "1280x768", "aspectRatio": "16:10" },
                { "resolution": "1024x768", "aspectRatio": "4:3" },
                { "resolution": "1088x1088", "aspectRatio": "1:1" },
                { "resolution": "800x600", "aspectRatio": "4:3" },
                { "resolution": "800x480", "aspectRatio": "16:10" },
                { "resolution": "720x480", "aspectRatio": "3:2" },
                { "resolution": "640x480", "aspectRatio": "4:3" },
                { "resolution": "352x288", "aspectRatio": "5:4" },
                { "resolution": "320x240", "aspectRatio": "4:3" }
            ],
            "video":
            [
                { "resolution": "1920x1080", "aspectRatio": "16:9" },
                { "resolution": "1280x720", "aspectRatio": "16:9" },
                { "resolution": "864x480", "aspectRatio": "9:5" },
                { "resolution": "800x480", "aspectRatio": "16:10" },
                { "resolution": "720x480", "aspectRatio": "3:2" },
                { "resolution": "640x480", "aspectRatio": "4:3" },
                { "resolution": "480x320", "aspectRatio": "3:2" },
                { "resolution": "352x288", "aspectRatio": "5:4" },
                { "resolution": "320x240", "aspectRatio": "4:3" },
                { "resolution": "176x144", "aspectRatio": "5:4" }
            ]
        },
        "viewfinder":
        {
            "viewfinderResolution_4_3" : "1440x1080",
            "viewfinderResolution_16_9" : "1920x1080"
        }
    }

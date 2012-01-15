/*
    Copyright (C) 2010 George Kiagiadakis <kiagiadakis.george@gmail.com>
    Copyright (C) 2012 Collabora Ltd. <info@collabora.com>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GST_QWIDGET_VIDEO_SINK_H
#define GST_QWIDGET_VIDEO_SINK_H

#include "gstqtvideosinkbase.h"

#define GST_TYPE_QWIDGET_VIDEO_SINK \
  (GstQWidgetVideoSink::get_type())
#define GST_QWIDGET_VIDEO_SINK(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_QWIDGET_VIDEO_SINK, GstQWidgetVideoSink))
#define GST_QWIDGETVIDEOSINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_QWIDGET_VIDEO_SINK, GstQWidgetVideoSinkClass))

class WidgetProxy;

struct GstQWidgetVideoSink
{
public:
    GstQtVideoSinkBase parent;

    static GType get_type();

private:
    enum {
        PROP_0,
        PROP_WIDGET
    };

    static void base_init(gpointer g_class);
    static void class_init(gpointer g_class, gpointer class_data);
    static void init(GTypeInstance *instance, gpointer g_class);
    static void finalize(GObject *object);

    static void set_property(GObject *object, guint prop_id,
                             const GValue *value, GParamSpec *pspec);
    static void get_property(GObject *object, guint prop_id,
                             GValue *value, GParamSpec *pspec);

    static void update(GstQtVideoSinkBase *sink);


    WidgetProxy *proxy;
    static GstQtVideoSinkBaseClass *s_parent_class;
};

struct GstQWidgetVideoSinkClass
{
    GstQtVideoSinkBaseClass parent_class;
};

#endif
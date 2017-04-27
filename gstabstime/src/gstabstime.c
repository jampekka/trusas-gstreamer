/*
 * GStreamer
 * Copyright (C) 2006 Stefan Kost <ensonic@users.sf.net>
 * Copyright (C) 2017 Jami Pekkanen <jami.pekkanen@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-abstime
 *
 * FIXME:Describe abstime here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! abstime ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/base/base.h>
#include <gst/controller/controller.h>
#include <time.h>
#include <stdio.h>

#include "gstabstime.h"

GST_DEBUG_CATEGORY_STATIC (gst_abs_time_debug);
#define GST_CAT_DEFAULT gst_abs_time_debug

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_SILENT,
};

/* the capabilities of the inputs and outputs.
 *
 * FIXME:describe the real formats here.
 */
static GstStaticPadTemplate sink_template =
GST_STATIC_PAD_TEMPLATE (
  "sink",
  GST_PAD_SINK,
  GST_PAD_ALWAYS,
  GST_STATIC_CAPS ("ANY")
);

static GstStaticPadTemplate src_template =
GST_STATIC_PAD_TEMPLATE (
  "src",
  GST_PAD_SRC,
  GST_PAD_ALWAYS,
  GST_STATIC_CAPS ("text/x-raw,format=utf8")
);

#define gst_abs_time_parent_class parent_class
G_DEFINE_TYPE (GstAbsTime, gst_abs_time, GST_TYPE_BASE_TRANSFORM);

static void gst_abs_time_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_abs_time_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static GstFlowReturn gst_abs_time_transform (GstBaseTransform * base,
    GstBuffer * inbuf, GstBuffer * outbuf);
static GstCaps *
gst_abs_time_transform_caps (GstBaseTransform * trans, GstPadDirection direction,
    GstCaps * caps, GstCaps * filter);
static GstFlowReturn
gst_abs_time_prepare_output_buffer (GstBaseTransform * base,
    GstBuffer * inbuf, GstBuffer ** outbuf);

/* GObject vmethod implementations */

/* initialize the abstime's class */
static void
gst_abs_time_class_init (GstAbsTimeClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  gobject_class->set_property = gst_abs_time_set_property;
  gobject_class->get_property = gst_abs_time_get_property;

  g_object_class_install_property (gobject_class, PROP_SILENT,
    g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
          FALSE, G_PARAM_READWRITE | GST_PARAM_CONTROLLABLE));

  gst_element_class_set_details_simple (gstelement_class,
    "AbsTime",
    "Generic/Filter",
    "FIXME:Generic Template Filter",
    "Jami Pekkanen <<user@hostname.org>>");

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&src_template));
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&sink_template));

  //GST_BASE_TRANSFORM_CLASS (klass)->transform_ip =
  //    GST_DEBUG_FUNCPTR (gst_abs_time_transform_ip);
  GST_BASE_TRANSFORM_CLASS (klass)->transform =
      GST_DEBUG_FUNCPTR (gst_abs_time_transform);
  GST_BASE_TRANSFORM_CLASS (klass)->transform_caps =
      GST_DEBUG_FUNCPTR (gst_abs_time_transform_caps);
  GST_BASE_TRANSFORM_CLASS (klass)->prepare_output_buffer =
      GST_DEBUG_FUNCPTR (gst_abs_time_prepare_output_buffer);


  /* debug category for fltering log messages
   *
   * FIXME:exchange the string 'Template abstime' with your description
   */
  GST_DEBUG_CATEGORY_INIT (gst_abs_time_debug, "abstime", 0, "Template abstime");
}

/* initialize the new element
 * initialize instance structure
 */
static void
gst_abs_time_init (GstAbsTime *filter)
{
  filter->silent = FALSE;
  filter->clock = gst_system_clock_obtain();
  g_object_set(filter->clock, "clock-type", GST_CLOCK_TYPE_REALTIME, NULL);
  
  filter->buffer = gst_buffer_new_allocate(NULL, 512, NULL);
  gst_buffer_ref(filter->buffer);
}

static void
gst_abs_time_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstAbsTime *filter = GST_ABSTIME (object);

  switch (prop_id) {
    case PROP_SILENT:
      filter->silent = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_abs_time_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstAbsTime *filter = GST_ABSTIME (object);

  switch (prop_id) {
    case PROP_SILENT:
      g_value_set_boolean (value, filter->silent);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static GstFlowReturn
gst_abs_time_transform (GstBaseTransform * base, GstBuffer * inbuf, GstBuffer * outbuf)
{
  GstAbsTime *filter = GST_ABSTIME (base);

  //if (GST_CLOCK_TIME_IS_VALID (GST_BUFFER_TIMESTAMP (outbuf)))
  //  gst_object_sync_values (GST_OBJECT (filter), GST_BUFFER_TIMESTAMP (outbuf));

  GST_BUFFER_TIMESTAMP(outbuf) = GST_BUFFER_TIMESTAMP(inbuf);
  GST_BUFFER_DURATION(outbuf) = GST_BUFFER_DURATION(inbuf);
  // TODO: Push the timestamp
  GstClockTime streamtime = GST_BUFFER_TIMESTAMP(inbuf);
  GstClockTime ts = gst_clock_get_time(filter->clock);
  struct timespec tstruct;
  GST_TIME_TO_TIMESPEC(ts, tstruct);
  GstMapInfo info;
  gst_buffer_map(outbuf, &info, GST_MAP_WRITE);
  char *out = (char*)info.data;
  size_t i = strftime(out, 512, "%FT%T", gmtime(&tstruct.tv_sec));
  i += sprintf(out + i, ".%.09ld=", tstruct.tv_nsec);
  sprintf(out + i, "%" GST_TIME_FORMAT "\n", GST_TIME_ARGS(streamtime));
  //puts(out);
  
  gst_buffer_unmap(outbuf, &info);

  return GST_FLOW_OK;
}

static GstFlowReturn
gst_abs_time_prepare_output_buffer (GstBaseTransform * base,
    GstBuffer * inbuf, GstBuffer ** outbuf) {
  GstAbsTime *filter = GST_ABSTIME (base);
  // TODO: Don't allocate new one for every frame
  //*outbuf = gst_buffer_new_allocate(NULL, 512, NULL);
  //*outbuf = gst_buffer_make_writable (*outbuf);
  gst_buffer_ref(filter->buffer);
  *outbuf = gst_buffer_make_writable(filter->buffer);
  return GST_FLOW_OK;
}

static GstCaps *
gst_abs_time_transform_caps (GstBaseTransform * trans, GstPadDirection direction,
    GstCaps * caps, GstCaps * filter)
{
  GstAbsTime *abs_time = GST_ABSTIME (trans);
  GstCaps *othercaps;

  GST_DEBUG_OBJECT (abs_time, "transform_caps");

  //othercaps = gst_caps_copy (caps);

  /* Copy other caps and modify as appropriate */
  /* This works for the simplest cases, where the transform modifies one
   * or more fields in the caps structure.  It does not work correctly
   * if passthrough caps are preferred. */
  if (direction == GST_PAD_SRC) {
    /* transform caps going upstream */
    othercaps = gst_caps_new_any();
  } else {
    /* transform caps going downstream */
    othercaps = gst_caps_from_string("text/x-raw,format=utf8");
  }

  if (filter) {
    GstCaps *intersect;

    intersect = gst_caps_intersect (othercaps, filter);
    gst_caps_unref (othercaps);

    return intersect;
  } else {
    return othercaps;
  }
}




/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean
abstime_init (GstPlugin * abstime)
{
  return gst_element_register (abstime, "abstime", GST_RANK_NONE,
      GST_TYPE_ABSTIME);
}

/* gstreamer looks for this structure to register abstimes
 *
 * FIXME:exchange the string 'Template abstime' with you abstime description
 */
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    abstime,
    "Template abstime",
    abstime_init,
    VERSION,
    "LGPL",
    "GStreamer",
    "http://gstreamer.net/"
)

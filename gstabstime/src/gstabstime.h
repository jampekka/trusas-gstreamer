/* 
 * GStreamer
 * Copyright (C) 2006 Stefan Kost <ensonic@users.sf.net>
 * Copyright (C) 2017 Jami Pekkanen <<user@hostname.org>>
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
 
#ifndef __GST_ABSTIME_H__
#define __GST_ABSTIME_H__

#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>

G_BEGIN_DECLS

#define GST_TYPE_ABSTIME \
  (gst_abs_time_get_type())
#define GST_ABSTIME(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_ABSTIME,GstAbsTime))
#define GST_ABSTIME_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_ABSTIME,GstAbsTimeClass))
#define GST_IS_ABSTIME(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_ABSTIME))
#define GST_IS_ABSTIME_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_ABSTIME))

typedef struct _GstAbsTime      GstAbsTime;
typedef struct _GstAbsTimeClass GstAbsTimeClass;

struct _GstAbsTime {
  GstBaseTransform element;
  GstClock *clock;
  GstBuffer *buffer;
  gboolean silent;
};

struct _GstAbsTimeClass {
  GstBaseTransformClass parent_class;
};

GType gst_abs_time_get_type (void);

G_END_DECLS

#endif /* __GST_ABSTIME_H__ */

/*
 * Copyright (c) 2015, Ericsson AB. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or other
 * materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 */

/*/
\*\ OwrBus
/*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "owr_bus.h"

#include "owr_bus_private.h"

#include "owr_utils.h"

#include <gst/gst.h>

GST_DEBUG_CATEGORY_EXTERN(_owrbus_debug);
#define GST_CAT_DEFAULT _owrbus_debug

#define DEFAULT_MESSAGE_TYPE_MASK (OWR_MESSAGE_TYPE_ERROR | OWR_MESSAGE_TYPE_STATS | OWR_MESSAGE_TYPE_EVENT)

enum {
    PROP_0,
    PROP_MESSAGE_TYPE_MASK,
    N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL, };

#define OWR_BUS_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE((obj), OWR_TYPE_BUS, OwrBusPrivate))

G_DEFINE_TYPE(OwrBus, owr_bus, G_TYPE_OBJECT)

struct _OwrBusPrivate {
    OwrMessageType message_type_mask;
};

static void owr_bus_finalize(GObject *);
static void owr_bus_set_property(GObject *, guint property_id, const GValue *, GParamSpec *);
static void owr_bus_get_property(GObject *, guint property_id, GValue *, GParamSpec *);

static void owr_bus_class_init(OwrBusClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    g_type_class_add_private(klass, sizeof(OwrBusPrivate));

    obj_properties[PROP_MESSAGE_TYPE_MASK] = g_param_spec_flags("message-type-mask", "message-type-mask",
        "The message types that the bus should forward, other message types will be discarded"
        " (default: forward all messages)",
        OWR_TYPE_MESSAGE_TYPE, DEFAULT_MESSAGE_TYPE_MASK,
        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    gobject_class->set_property = owr_bus_set_property;
    gobject_class->get_property = owr_bus_get_property;

    gobject_class->finalize = owr_bus_finalize;

    g_object_class_install_properties(gobject_class, N_PROPERTIES, obj_properties);
}

static void owr_bus_init(OwrBus *bus)
{
    OwrBusPrivate *priv;
    bus->priv = priv = OWR_BUS_GET_PRIVATE(bus);

    priv->message_type_mask = DEFAULT_MESSAGE_TYPE_MASK;
}

static void owr_bus_finalize(GObject *object)
{
    OwrBus *bus = OWR_BUS(object);
    OwrBusPrivate *priv = bus->priv;

    OWR_UNUSED(priv);

    G_OBJECT_CLASS(owr_bus_parent_class)->finalize(object);
}

static void owr_bus_set_property(GObject *object, guint property_id,
    const GValue *value, GParamSpec *pspec)
{
    OwrBusPrivate *priv;

    g_return_if_fail(object);
    priv = OWR_BUS_GET_PRIVATE(object);

    switch (property_id) {
    case PROP_MESSAGE_TYPE_MASK:
        priv->message_type_mask = g_value_get_flags(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void owr_bus_get_property(GObject *object, guint property_id,
    GValue *value, GParamSpec *pspec)
{
    OwrBusPrivate *priv;

    g_return_if_fail(object);
    priv = OWR_BUS_GET_PRIVATE(object);

    switch (property_id) {
    case PROP_MESSAGE_TYPE_MASK:
        g_value_set_flags(value, priv->message_type_mask);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

OwrBus *owr_bus_new()
{
    return g_object_new(OWR_TYPE_BUS, NULL);
}

void owr_bus_add_message_origin(OwrBus *bus, OwrMessageOrigin *origin)
{
    OWR_UNUSED(bus);
    OWR_UNUSED(origin);
}

void owr_bus_remove_message_origin(OwrBus *bus, OwrMessageOrigin *origin)
{
    OWR_UNUSED(bus);
    OWR_UNUSED(origin);
}

GType owr_message_type_get_type(void)
{
    static const GFlagsValue types[] = {
        {OWR_MESSAGE_TYPE_ERROR, "Error", "error"},
        {OWR_MESSAGE_TYPE_STATS, "State", "state"},
        {OWR_MESSAGE_TYPE_EVENT, "Event", "event"},
        {0, NULL, NULL}
    };
    static volatile GType id = 0;

    if (g_once_init_enter((gsize *)&id)) {
        GType _id = g_flags_register_static("OwrMessageTypes", types);
        g_once_init_leave((gsize *)&id, _id);
    }

    return id;
}
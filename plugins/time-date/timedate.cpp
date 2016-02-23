/*
 * Copyright (C) 2013-2014 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Iain Lane <iain.lane@canonical.com>
 *
*/

#include "timedate.h"
#include <QDBusReply>
#include <QEvent>
#include <glib.h>
#include <glib-object.h>
#include <unistd.h>

#include <iostream>
TimeDate::TimeDate(QObject *parent) :
    QObject(parent),
    m_useNTP(false),
    m_systemBusConnection (QDBusConnection::systemBus()),
    m_serviceWatcher ("org.freedesktop.timedate1",
                      m_systemBusConnection,
                      QDBusServiceWatcher::WatchForOwnerChange),
    m_timeDateInterface ("org.freedesktop.timedate1",
                         "/org/freedesktop/timedate1",
                         "org.freedesktop.timedate1",
                          m_systemBusConnection),
    m_timeZoneModel()
{
    connect (&m_serviceWatcher,
             SIGNAL (serviceOwnerChanged (QString, QString, QString)),
             this,
             SLOT (slotNameOwnerChanged (QString, QString, QString)));

    connect (&m_timeZoneModel, SIGNAL (filterBegin ()),
             this, SIGNAL (listUpdatingChanged ()));
    connect (&m_timeZoneModel, SIGNAL (filterComplete ()),
             this, SIGNAL (listUpdatingChanged ()));

    m_useNTP = getUseNTP();

    setUpInterface();
}

void TimeDate::setUpInterface()
{
    m_timeDateInterface.connection().connect(
        m_timeDateInterface.service(),
        m_timeDateInterface.path(),
        "org.freedesktop.DBus.Properties",
        "PropertiesChanged",
        this,
        SLOT(slotChanged(QString, QVariantMap, QStringList)));
}

QString TimeDate::timeZone()
{
    if (m_currentTimeZone.isEmpty() || m_currentTimeZone.isNull())
        m_currentTimeZone = getTimeZone();

     return m_currentTimeZone;
}

QString TimeDate::getTimeZone()
{
    QVariant tz(m_timeDateInterface.property("Timezone"));

    if (tz.isValid())
        return tz.toString();

    return QString();
}

bool TimeDate::useNTP()
{
    return m_useNTP;
}

bool TimeDate::getUseNTP()
{
    QVariant useNTP(m_timeDateInterface.property("NTP"));

    if (useNTP.isValid())
        return useNTP.toBool();

    // Default to false
    return false;
}

void TimeDate::setUseNTP(bool enabled)
{
    m_timeDateInterface.call("SetNTP", enabled, false);
    m_useNTP = enabled;
}

void TimeDate::slotChanged(QString interface,
                           QVariantMap changed_properties,
                           QStringList invalidated_properties)
{
    Q_UNUSED (interface);
    Q_UNUSED (invalidated_properties);

    if (changed_properties.contains("Timezone")) {
        QString tz(changed_properties["Timezone"].toString());
        if (tz != m_currentTimeZone) {
            m_currentTimeZone = tz;
            Q_EMIT timeZoneChanged();
        }
    }

    if (changed_properties.contains("NTP")) {
        bool useNTP = changed_properties["NTP"].toBool();
        if (useNTP != m_useNTP) {
            m_useNTP = useNTP;
            Q_EMIT useNTPChanged();
        }
    }
}

void TimeDate::slotNameOwnerChanged(QString name,
                                    QString oldOwner,
                                    QString newOwner)
{
    Q_UNUSED (oldOwner);
    Q_UNUSED (newOwner);

    if (name != "org.freedesktop.timedate1")
        return;

    if (m_timeDateInterface.isValid())
        setUpInterface();
}

void TimeDate::setTimeZone(QString &time_zone)
{
    m_timeDateInterface.call("SetTimezone", time_zone, false);
}

QAbstractItemModel *TimeDate::getTimeZoneModel()
{
    return &m_timeZoneModel;
}

QString TimeDate::getFilter()
{
    return m_filter;
}

void TimeDate::setFilter(QString &new_filter)
{
    m_filter = new_filter;
    m_timeZoneModel.filter(m_filter);
}

void TimeDate::setTime(qlonglong new_time)
{
    m_timeDateInterface.call("SetTime", new_time, false, false);
}

bool TimeDate::getListUpdating()
{
    return m_timeZoneModel.modelUpdating;
}

TimeDate::~TimeDate() {
}

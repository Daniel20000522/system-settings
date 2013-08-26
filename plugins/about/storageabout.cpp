/*
 * Copyright (C) 2013 Canonical Ltd
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
 * Authors: Sebastien Bacher <sebastien.bacher@canonical.com>
 *
*/

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include "storageabout.h"
#include <hybris/properties/properties.h>

StorageAbout::StorageAbout(QObject *parent) :
    QObject(parent)
{

}

QByteArray StorageAbout::getClickList()
{
    QFile clickBinary("/usr/bin/click");
    if (!clickBinary.exists()) {
        return QByteArray();
    }

    QProcess *clickProcess = new QProcess();
    clickProcess->start("/usr/bin/click", QStringList() << "list" << "--manifest");
    clickProcess->waitForFinished(-1);
    return clickProcess->readAllStandardOutput();
}


QString StorageAbout::serialNumber()
{
    static char serialBuffer[PROP_NAME_MAX];

    if (m_serialNumber.isEmpty() || m_serialNumber.isNull())
    {
        property_get("ro.serialno", serialBuffer, "");
        m_serialNumber = QString(serialBuffer);
    }

    return m_serialNumber;
}

QString StorageAbout::vendorString()
{
    static char manufacturerBuffer[PROP_NAME_MAX];
    static char modelBuffer[PROP_NAME_MAX];

    if (m_vendorString.isEmpty() || m_vendorString.isNull())
    {
        property_get("ro.product.manufacturer", manufacturerBuffer, "");
        property_get("ro.product.model", modelBuffer, "");
        m_vendorString = QString("%1 %2").arg(manufacturerBuffer).arg(modelBuffer);
    }

    return m_vendorString;
}

QString StorageAbout::updateDate()
{
    if (m_updateDate.isEmpty() || m_updateDate.isNull())
    {
        QFile file("/userdata/.last_update");
        if (!file.exists())
            return "";
        m_updateDate = QFileInfo(file).created().toString("yyyy-MM-dd");
    }

    return m_updateDate;
}

QString StorageAbout::licenseInfo(const QString &subdir) const
{

    QString copyright = "/usr/share/doc/" + subdir + "/copyright";
    QString copyrightText;

    QFile file(copyright);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    copyrightText = QString(file.readAll());
    file.close();
    return copyrightText;
}

StorageAbout::~StorageAbout() {
}

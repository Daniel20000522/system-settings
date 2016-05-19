/*
 * Copyright (C) 2016 Canonical Ltd
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
*/

#ifndef CLICKAPPLICATIONSMODEL_H
#define CLICKAPPLICATIONSMODEL_H

// Qt
#include <QtCore/QAbstractListModel>
#include <QtCore/QUrl>

class ClickApplicationsModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ rowCount NOTIFY rowCountChanged)

    Q_ENUMS(Roles)

public:
    ClickApplicationsModel(QObject* parent=0);
    ~ClickApplicationsModel();

    enum Roles {
        DisplayName = Qt::UserRole + 1,
        Icon,
        SoundsNotify,
        VibrationsNotify,
        BubblesNotify,
        ListNotify
    };

    // reimplemented from QAbstractListModel
    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex& parent=QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;

    Q_INVOKABLE bool setNotifyEnabled(int role, int idx, bool enabled);

protected:
    struct ClickApplicationEntry {
        QString pkgName;
        QString appName;
        QString version;
        QString displayName;
        QUrl icon;
        bool soundsNotify = true;
        bool vibrationsNotify = true;
        bool bubblesNotify = true;
        bool listNotify = true;
 
    };
    QList<ClickApplicationEntry> m_entries;

Q_SIGNALS:
    void rowCountChanged();

private:
    void addClickApplicationEntry(const ClickApplicationEntry& entry);
    void getApplicationDataFromDesktopFile(ClickApplicationEntry& entry);
    void populateFromLegacyHelpersDir();
    bool clickManifestHasPushHelperHook(const QVariantMap& manifest);
    QString getApplicationNameFromDesktopHook(const QVariantMap& manifest);
    void populateFromClickDatabase();
};

#endif // CLICKAPPLICATIONSMODEL_H

/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.4
import Ubuntu.SystemSettings.Update 1.0

UpdateDelegate {
    id: update

    property string signedUrl

    // Makes the progress bar indeterminate when waiting
    // FIXME: Disabled until lp:1587176 is fixed.
    // Binding {
    //     target: update
    //     property: "progress"
    //     value: -1
    //     when: {
    //         var queued = updateState === Update.StateQueuedForDownload;
    //         var installing = updateState === Update.StateInstalling;
    //         return queued || installing;
    //     }
    // }
}

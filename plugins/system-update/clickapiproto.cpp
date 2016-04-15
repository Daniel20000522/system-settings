/*
 * Copyright (C) 2013-2016 Canonical Ltd
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
#include <assert.h>
#include "clickapiproto.h"

namespace UpdatePlugin {

ClickApiProto::ClickApiProto(QObject *parent):
    QObject(parent),
    m_errorString(""),
    m_token(UbuntuOne::Token())
{
    initializeNam();
}

ClickApiProto::~ClickApiProto()
{
    cancel();
}

void ClickApiProto::initializeNam()
{
    connect(&m_nam, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(requestFinished(QNetworkReply *)));
    connect(&m_nam, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError>&)),
            this, SLOT(requestSslFailed(QNetworkReply *, const QList<QSslError>&)));
}

void ClickApiProto::initializeReply(QNetworkReply *reply)
{
    qWarning() << "click proto: init reply" << reply;
    connect(this, SIGNAL(abortNetworking()),
            reply, SLOT(abort()));
}
void ClickApiProto::setToken(const UbuntuOne::Token &token)
{
    m_token = token;
}

void ClickApiProto::requestSslFailed(QNetworkReply *reply,
                                     const QList<QSslError> &errors)
{
    QString errorString = "SSL error:";
    foreach (const QSslError &err, errors) {
        errorString += err.errorString();
    }
    setErrorString(errorString);
    Q_EMIT serverError();
    reply->deleteLater();
}

void ClickApiProto::requestFinished(QNetworkReply *reply)
{
    qWarning() << "click proto: something finished" << reply;
    // check for http error status and emit all the required signals
    if (!validReply(reply)) {
        reply->deleteLater();
        return;
    }

    switch (reply->error()) {
    case QNetworkReply::NoError:
        requestSucceeded(reply);
        return;
    case QNetworkReply::TemporaryNetworkFailureError:
    case QNetworkReply::UnknownNetworkError:
    case QNetworkReply::UnknownProxyError:
    case QNetworkReply::UnknownServerError:
        Q_EMIT networkError();
        break;
    default:
        Q_EMIT serverError();
    }

    reply->deleteLater();
}

bool ClickApiProto::validReply(const QNetworkReply *reply)
{
    auto statusAttr = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (!statusAttr.isValid()) {
        Q_EMIT networkError();
        setErrorString("status attribute was invalid");
        return false;
    }

    int httpStatus = statusAttr.toInt();
    qWarning() << "HTTP Status: " << httpStatus;

    if (httpStatus == 401 || httpStatus == 403) {
        setErrorString("credential error");
        Q_EMIT credentialError();
        return false;
    }

    return true;
}

QString ClickApiProto::errorString() const
{
    return m_errorString;
}

void ClickApiProto::setErrorString(const QString &errorString)
{
    m_errorString = errorString;
    Q_EMIT errorStringChanged();
}

void ClickApiProto::cancel()
{
    // Tell each reply to abort. See initializeReply().
    Q_EMIT abortNetworking();
}

} // UpdatePlugin

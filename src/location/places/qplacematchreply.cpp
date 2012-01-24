/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: http://www.qt-project.org/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qplacematchreply.h"
#include "qplacereply_p.h"


QT_BEGIN_NAMESPACE
class QPlaceMatchReplyPrivate : public QPlaceReplyPrivate
{
public:
    QPlaceMatchReplyPrivate(){}
    QList<QPlace> places;
    QPlaceMatchRequest request;
};

QT_END_NAMESPACE

QT_USE_NAMESPACE

/*!
    \class QPlaceMatchReply
    \inmodule QtLocation
    \ingroup QtLocation-places
    \ingroup QtLocation-places-replies
    \since QtLocation 5.0

    \brief The QPlaceMatchReply class manages a place matching operation started by an
    instance of QPlaceManager.

    If the operation is successful, the number of places in the reply matches those
    in the request.  If a particular place in the request is not found, a default
    constructed place is used as a place holder in the reply. In this way, there
    is always a one is to one relationship between input places in the request,
    and output places in the reply.

    If the operation is not successful the number of places is always zero.

    See \l {Matching places between managers} for an example on how to use
    a match reply.

    \sa QPlaceMatchRequest, QPlaceManager
*/

/*!
    Constructs a match reply with a given \a parent.
*/
QPlaceMatchReply::QPlaceMatchReply(QObject *parent)
    : QPlaceReply(new QPlaceMatchReplyPrivate, parent)
{
}

/*!
    Destroys the match reply.
*/
QPlaceMatchReply::~QPlaceMatchReply()
{
}

/*!
    Returns the type of reply.
*/
QPlaceReply::Type QPlaceMatchReply::type() const
{
    return QPlaceReply::MatchReply;
}

 /*!
    Returns a list of matching places;
*/
QList<QPlace> QPlaceMatchReply::places() const
{
    Q_D(const QPlaceMatchReply);
    return d->places;
}

/*!
    Sets the list of matching \a places.
*/
void QPlaceMatchReply::setPlaces(const QList<QPlace> &places)
{
    Q_D(QPlaceMatchReply);
    d->places = places;
}

/*!
    Returns the match request that was used to generate this reply.
*/
QPlaceMatchRequest QPlaceMatchReply::request() const
{
    Q_D(const QPlaceMatchReply);
    return d->request;
}

/*!
    Sets the match \a request used to generate this reply.
*/
void QPlaceMatchReply::setRequest(const QPlaceMatchRequest &request)
{
    Q_D(QPlaceMatchReply);
    d->request = request;
}
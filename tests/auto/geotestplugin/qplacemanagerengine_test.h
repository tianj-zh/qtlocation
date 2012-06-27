/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the test suite of the Qt Toolkit.
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QPLACEMANAGERENGINE_TEST_H
#define QPLACEMANAGERENGINE_TEST_H

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QUuid>
#include <QtLocation/QGeoCoordinate>
#include <QtLocation/QGeoLocation>
#include <QtLocation/QPlaceManager>
#include <QtLocation/QPlaceManagerEngine>
#include <QtLocation/QPlaceReply>
#include <QtLocation/QPlaceDetailsReply>
#include <QtLocation/QPlaceIdReply>
#include <QtLocation/QPlaceSearchSuggestionReply>
#include <QtLocation/QPlaceSearchReply>
#include <QtLocation/QPlaceResult>
#include <QtLocation/QPlaceCategory>
#include <QtLocation/QPlace>
#include <QtTest/QTest>

QT_BEGIN_NAMESPACE

inline uint qHash(const QPlaceCategory &category)
{
    return qHash(QUuid(category.categoryId().toLatin1()));
}

QT_END_NAMESPACE

QT_USE_NAMESPACE

class PlaceReply : public QPlaceReply
{
    Q_OBJECT

    friend class QPlaceManagerEngineTest;

public:
    PlaceReply(QObject *parent = 0)
    :   QPlaceReply(parent)
    { }

    Q_INVOKABLE void emitFinished()
    {
        emit finished();
    }
};

class DetailsReply : public QPlaceDetailsReply
{
    Q_OBJECT

    friend class QPlaceManagerEngineTest;

public:
    DetailsReply(QObject *parent = 0)
    :   QPlaceDetailsReply(parent)
    { }

    Q_INVOKABLE void emitError()
    {
        emit error(error(), errorString());
    }

    Q_INVOKABLE void emitFinished()
    {
        emit finished();
    }
};

class IdReply : public QPlaceIdReply
{
    Q_OBJECT

    friend class QPlaceManagerEngineTest;

public:
    IdReply(QPlaceIdReply::OperationType type, QObject *parent = 0)
    :   QPlaceIdReply(type, parent)
    { }

    Q_INVOKABLE void emitError()
    {
        emit error(error(), errorString());
    }

    Q_INVOKABLE void emitFinished()
    {
        emit finished();
    }
};

class PlaceSearchReply : public QPlaceSearchReply
{
    Q_OBJECT

public:
    PlaceSearchReply(const QList<QPlaceSearchResult> &results, QObject *parent = 0)
    :   QPlaceSearchReply(parent)
    {
        setResults(results);
    }

    Q_INVOKABLE void emitError()
    {
        emit error(error(), errorString());
    }

    Q_INVOKABLE void emitFinished()
    {
        emit finished();
    }
};

class SuggestionReply : public QPlaceSearchSuggestionReply
{
    Q_OBJECT

public:
    SuggestionReply(const QStringList &suggestions, QObject *parent = 0)
    :   QPlaceSearchSuggestionReply(parent)
    {
        setSuggestions(suggestions);
    }

    Q_INVOKABLE void emitError()
    {
        emit error(error(), errorString());
    }

    Q_INVOKABLE void emitFinished()
    {
        emit finished();
    }
};

class QPlaceManagerEngineTest : public QPlaceManagerEngine
{
    Q_OBJECT
public:
    QPlaceManagerEngineTest(const QMap<QString, QVariant> &parameters)
        : QPlaceManagerEngine(parameters)
    {
        m_locales << QLocale();

        if (parameters.value(QStringLiteral("initializePlaceData"), false).toBool()) {
            QFile placeData(QFINDTESTDATA("place_data.json"));
            if (placeData.open(QIODevice::ReadOnly)) {
                QJsonDocument document = QJsonDocument::fromJson(placeData.readAll());

                if (document.isObject()) {
                    QJsonObject o = document.object();

                    if (o.contains(QStringLiteral("categories"))) {
                        QJsonArray categories = o.value(QStringLiteral("categories")).toArray();

                        for (int i = 0; i < categories.count(); ++i) {
                            QJsonObject c = categories.at(i).toObject();

                            QPlaceCategory category;

                            category.setName(c.value(QStringLiteral("name")).toString());
                            category.setCategoryId(c.value(QStringLiteral("id")).toString());

                            m_categories.insert(category.categoryId(), category);

                            const QString parentId = c.value(QStringLiteral("parentId")).toString();
                            m_childCategories[parentId].append(category.categoryId());
                        }
                    }

                    if (o.contains(QStringLiteral("places"))) {
                        QJsonArray places = o.value(QStringLiteral("places")).toArray();

                        for (int i = 0; i < places.count(); ++i) {
                            QJsonObject p = places.at(i).toObject();

                            QPlace place;

                            place.setName(p.value(QStringLiteral("name")).toString());
                            place.setPlaceId(p.value(QStringLiteral("id")).toString());

                            QList<QPlaceCategory> categories;
                            QJsonArray ca = p.value(QStringLiteral("categories")).toArray();
                            for (int j = 0; j < ca.count(); ++j) {
                                QPlaceCategory c = m_categories.value(ca.at(j).toString());
                                if (!c.isEmpty())
                                    categories.append(c);
                            }
                            place.setCategories(categories);

                            QGeoCoordinate coordinate;
                            QJsonObject lo = p.value(QStringLiteral("location")).toObject();
                            coordinate.setLatitude(lo.value(QStringLiteral("latitude")).toDouble());
                            coordinate.setLongitude(lo.value(QStringLiteral("longitude")).toDouble());

                            QGeoLocation location;
                            location.setCoordinate(coordinate);

                            place.setLocation(location);

                            m_places.insert(place.placeId(), place);
                        }
                    }
                }
            }
        }
    }

    QPlaceDetailsReply *getPlaceDetails(const QString &placeId)
    {
        DetailsReply *reply = new DetailsReply(this);

        if (placeId.isEmpty() || !m_places.contains(placeId)) {
            reply->setError(QPlaceReply::PlaceDoesNotExistError, tr("Place does not exist"));
            QMetaObject::invokeMethod(reply, "emitError", Qt::QueuedConnection);
        } else {
            reply->setPlace(m_places.value(placeId));
        }

        QMetaObject::invokeMethod(reply, "emitFinished", Qt::QueuedConnection);

        return reply;
    }

    QPlaceContentReply *getPlaceContent(const QString &placeId, const QPlaceContentRequest &query)
    {
        Q_UNUSED(placeId)
        Q_UNUSED(query)

        return 0;
    }

    QPlaceSearchReply *search(const QPlaceSearchRequest &query)
    {
        QList<QPlaceSearchResult> results;

        if (!query.searchTerm().isEmpty()) {
            foreach (const QPlace &place, m_places) {
                if (!place.name().contains(query.searchTerm(), Qt::CaseInsensitive))
                    continue;

                QPlaceResult r;
                r.setPlace(place);
                r.setTitle(place.name());

                results.append(r);
            }
        } else if (!query.categories().isEmpty()) {
            QSet<QPlaceCategory> categories = query.categories().toSet();
            foreach (const QPlace &place, m_places) {
                if (place.categories().toSet().intersect(categories).isEmpty())
                    continue;

                QPlaceResult r;
                r.setPlace(place);
                r.setTitle(place.name());

                results.append(r);
            }
        }

        PlaceSearchReply *reply = new PlaceSearchReply(results, this);

        QMetaObject::invokeMethod(reply, "emitFinished", Qt::QueuedConnection);

        return reply;
    }

    QPlaceSearchSuggestionReply *searchSuggestions(const QPlaceSearchRequest &query)
    {
        QStringList suggestions;
        if (query.searchTerm() == QLatin1String("test")) {
            suggestions << QStringLiteral("test1");
            suggestions << QStringLiteral("test2");
            suggestions << QStringLiteral("test3");
        }

        SuggestionReply *reply = new SuggestionReply(suggestions, this);

        QMetaObject::invokeMethod(reply, "emitFinished", Qt::QueuedConnection);

        return reply;
    }

    QPlaceIdReply *savePlace(const QPlace &place)
    {
        IdReply *reply = new IdReply(QPlaceIdReply::SavePlace, this);

        if (!place.placeId().isEmpty() && !m_places.contains(place.placeId())) {
            reply->setError(QPlaceReply::PlaceDoesNotExistError, tr("Place does not exist"));
            QMetaObject::invokeMethod(reply, "emitError", Qt::QueuedConnection);
        } else if (!place.placeId().isEmpty()) {
            m_places.insert(place.placeId(), place);
            reply->setId(place.placeId());
        } else {
            QPlace p = place;
            p.setPlaceId(QUuid::createUuid().toString());
            m_places.insert(p.placeId(), p);

            reply->setId(p.placeId());
        }

        QMetaObject::invokeMethod(reply, "emitFinished", Qt::QueuedConnection);

        return reply;
    }

    QPlaceIdReply *removePlace(const QString &placeId)
    {
        IdReply *reply = new IdReply(QPlaceIdReply::RemovePlace, this);
        reply->setId(placeId);

        if (!m_places.contains(placeId)) {
            reply->setError(QPlaceReply::PlaceDoesNotExistError, tr("Place does not exist"));
            QMetaObject::invokeMethod(reply, "emitError", Qt::QueuedConnection);
        } else {
            m_places.remove(placeId);
        }

        QMetaObject::invokeMethod(reply, "emitFinished", Qt::QueuedConnection);

        return reply;
    }

    QPlaceIdReply *saveCategory(const QPlaceCategory &category, const QString &parentId)
    {
        IdReply *reply = new IdReply(QPlaceIdReply::SaveCategory, this);

        if ((!category.categoryId().isEmpty() && !m_categories.contains(category.categoryId())) ||
            (!parentId.isEmpty() && !m_categories.contains(parentId))) {
            reply->setError(QPlaceReply::CategoryDoesNotExistError, tr("Category does not exist"));
            QMetaObject::invokeMethod(reply, "emitError", Qt::QueuedConnection);
        } else if (!category.categoryId().isEmpty()) {
            m_categories.insert(category.categoryId(), category);
            QStringList children = m_childCategories.value(parentId);

            QMutableHashIterator<QString, QStringList> i(m_childCategories);
            while (i.hasNext()) {
                i.next();
                i.value().removeAll(category.categoryId());
            }

            if (!children.contains(category.categoryId())) {
                children.append(category.categoryId());
                m_childCategories.insert(parentId, children);
            }
            reply->setId(category.categoryId());
        } else {
            QPlaceCategory c = category;
            c.setCategoryId(QUuid::createUuid().toString());
            m_categories.insert(c.categoryId(), c);
            QStringList children = m_childCategories.value(parentId);
            if (!children.contains(c.categoryId())) {
                children.append(c.categoryId());
                m_childCategories.insert(parentId, children);
            }

            reply->setId(c.categoryId());
        }

        QMetaObject::invokeMethod(reply, "emitFinished", Qt::QueuedConnection);

        return reply;
    }

    QPlaceIdReply *removeCategory(const QString &categoryId)
    {
        IdReply *reply = new IdReply(QPlaceIdReply::RemoveCategory, this);
        reply->setId(categoryId);

        if (!m_categories.contains(categoryId)) {
            reply->setError(QPlaceReply::CategoryDoesNotExistError, tr("Category does not exist"));
            QMetaObject::invokeMethod(reply, "emitError", Qt::QueuedConnection);
        } else {
            m_categories.remove(categoryId);

            QMutableHashIterator<QString, QStringList> i(m_childCategories);
            while (i.hasNext()) {
                i.next();
                i.value().removeAll(categoryId);
            }
        }

        QMetaObject::invokeMethod(reply, "emitFinished", Qt::QueuedConnection);

        return reply;
    }

    QPlaceReply *initializeCategories()
    {
        QPlaceReply *reply = new PlaceReply(this);

        QMetaObject::invokeMethod(reply, "emitFinished", Qt::QueuedConnection);

        return reply;
    }

    QString parentCategoryId(const QString &categoryId) const
    {
        QHashIterator<QString, QStringList> i(m_childCategories);
        while (i.hasNext()) {
            i.next();
            if (i.value().contains(categoryId))
                return i.key();
        }

        return QString();
    }

    virtual QStringList childCategoryIds(const QString &categoryId) const
    {
        return m_childCategories.value(categoryId);
    }

    virtual QPlaceCategory category(const QString &categoryId) const
    {
        return m_categories.value(categoryId);
    }

    QList<QPlaceCategory> childCategories(const QString &parentId) const
    {
        QList<QPlaceCategory> categories;

        foreach (const QString &id, m_childCategories.value(parentId))
            categories.append(m_categories.value(id));

        return categories;
    }

    QList<QLocale> locales() const
    {
        return m_locales;
    }

    void setLocales(const QList<QLocale> &locales)
    {
        m_locales = locales;
    }

    QUrl constructIconUrl(const QPlaceIcon &icon, const QSize &size) const {
        QList<QPair<int, QUrl> > candidates;

        QMap<QString, int> sizeDictionary;
        sizeDictionary.insert(QLatin1String("s"), 20);
        sizeDictionary.insert(QLatin1String("m"), 30);
        sizeDictionary.insert(QLatin1String("l"), 50);

        QStringList sizeKeys;
        sizeKeys << QLatin1String("s") << QLatin1String("m") << QLatin1String("l");

        foreach (const QString &sizeKey, sizeKeys)
        {
            if (icon.parameters().contains(sizeKey))
                candidates.append(QPair<int, QUrl>(sizeDictionary.value(sizeKey),
                                  icon.parameters().value(sizeKey).toUrl()));
        }

        if (candidates.isEmpty())
            return QUrl();
        else if (candidates.count() == 1) {
            return candidates.first().second;
        } else {
            //we assume icons are squarish so we can use height to
            //determine which particular icon to return
            int requestedHeight = size.height();

            for (int i = 0; i < candidates.count() - 1; ++i) {
                int thresholdHeight = (candidates.at(i).first + candidates.at(i+1).first) / 2;
                if (requestedHeight < thresholdHeight)
                    return candidates.at(i).second;
            }
            return candidates.last().second;
        }
    }

    QPlace compatiblePlace(const QPlace &original) const {
        QPlace place;
        place.setName(original.name());
        return place;
    }

private:
    QList<QLocale> m_locales;
    QHash<QString, QPlace> m_places;
    QHash<QString, QPlaceCategory> m_categories;
    QHash<QString, QStringList> m_childCategories;
};

#endif

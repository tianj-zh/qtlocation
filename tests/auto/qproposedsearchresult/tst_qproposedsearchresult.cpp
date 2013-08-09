/****************************************************************************
**
** Copyright (C) 2013 Aaron McCarthy <mccarthy.aaron@gmail.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/QString>
#include <QtLocation/QProposedSearchResult>
#include <QtLocation/QPlaceIcon>
#include <QtLocation/QPlaceSearchRequest>
#include <QtTest/QtTest>

#include "../utils/qlocationtestutils_p.h"

QT_USE_NAMESPACE

class tst_QProposedSearchResult : public QObject
{
    Q_OBJECT

public:
    QProposedSearchResult initialSubObject();
    bool checkType(const QPlaceSearchResult &result);
    void detach(QPlaceSearchResult *result);
    void setSubClassProperty(QProposedSearchResult *result);

private Q_SLOTS:
    void constructorTest();
    void title();
    void icon();
    void searchRequest();
    void conversion();
};

QProposedSearchResult tst_QProposedSearchResult::initialSubObject()
{
    QProposedSearchResult proposedSearchResult;
    proposedSearchResult.setTitle(QStringLiteral("title"));

    QPlaceIcon icon;
    QVariantMap parameters;
    parameters.insert(QPlaceIcon::SingleUrl,
                      QUrl(QStringLiteral("file:///opt/icons/icon.png")));
    icon.setParameters(parameters);
    proposedSearchResult.setIcon(icon);

    QPlaceSearchRequest searchRequest;
    searchRequest.setSearchContext(QUrl(QStringLiteral("http://www.example.com/")));
    proposedSearchResult.setSearchRequest(searchRequest);

    return proposedSearchResult;
}

bool tst_QProposedSearchResult::checkType(const QPlaceSearchResult &result)
{
    return result.type() == QPlaceSearchResult::ProposedSearchResult;
}

void tst_QProposedSearchResult::detach(QPlaceSearchResult *result)
{
    result->setTitle(QStringLiteral("title"));
}

void tst_QProposedSearchResult::setSubClassProperty(QProposedSearchResult *result)
{
    QPlaceSearchRequest request;
    request.setSearchContext(QUrl(QStringLiteral("http://www.example.com/place-search")));
    result->setSearchRequest(request);
}

void tst_QProposedSearchResult::constructorTest()
{
    QProposedSearchResult result;
    QCOMPARE(result.type(), QPlaceSearchResult::ProposedSearchResult);

    result.setTitle(QStringLiteral("title"));

    QPlaceIcon icon;
    QVariantMap parameters;
    parameters.insert(QLatin1String("paramKey"), QLatin1String("paramValue"));
    icon.setParameters(parameters);
    result.setIcon(icon);

    QPlaceSearchRequest searchRequest;
    searchRequest.setSearchContext(QUrl(QStringLiteral("http://www.example.com/place-search")));
    result.setSearchRequest(searchRequest);

    //check copy constructor
    QProposedSearchResult result2(result);
    QCOMPARE(result2.title(), QStringLiteral("title"));
    QCOMPARE(result2.icon(), icon);
    QCOMPARE(result2.searchRequest(), searchRequest);

    QVERIFY(QLocationTestUtils::compareEquality(result, result2));

    //check results are the same after detachment of underlying shared data pointer
    result2.setTitle(QStringLiteral("title"));
    QVERIFY(QLocationTestUtils::compareEquality(result, result2));

    //check construction of SearchResult using a ProposedSearchResult
    QPlaceSearchResult searchResult(result);
    QCOMPARE(searchResult.title(), QStringLiteral("title"));
    QCOMPARE(searchResult.icon(), icon);
    QVERIFY(QLocationTestUtils::compareEquality(searchResult, result));
    QVERIFY(searchResult.type() == QPlaceSearchResult::ProposedSearchResult);
    result2 = searchResult;
    QVERIFY(QLocationTestUtils::compareEquality(result, result2));

    //check construction of a SearchResult using a SearchResult
    //that is actually a PlaceResult underneath
    QPlaceSearchResult searchResult2(searchResult);
    QCOMPARE(searchResult2.title(), QStringLiteral("title"));
    QCOMPARE(searchResult2.icon(), icon);
    QVERIFY(QLocationTestUtils::compareEquality(searchResult2, result));
    QVERIFY(QLocationTestUtils::compareEquality(searchResult, searchResult2));
    QVERIFY(searchResult2.type() == QPlaceSearchResult::ProposedSearchResult);
    result2 = searchResult2;
    QVERIFY(QLocationTestUtils::compareEquality(result, result2));
}

void tst_QProposedSearchResult::title()
{
    QProposedSearchResult result;
    QVERIFY(result.title().isEmpty());

    result.setTitle(QStringLiteral("title"));
    QCOMPARE(result.title(), QStringLiteral("title"));

    result.setTitle(QString());
    QVERIFY(result.title().isEmpty());

    QProposedSearchResult result2;
    QVERIFY(QLocationTestUtils::compareEquality(result, result2));

    result2.setTitle("title");
    QVERIFY(QLocationTestUtils::compareInequality(result, result2));

    result.setTitle("title");
    QVERIFY(QLocationTestUtils::compareEquality(result, result2));
}

void tst_QProposedSearchResult::icon()
{
    QProposedSearchResult result;
    QVERIFY(result.icon().isEmpty());

    QPlaceIcon icon;
    QVariantMap iconParams;
    iconParams.insert(QLatin1String("paramKey"), QLatin1String("paramValue"));
    icon.setParameters(iconParams);
    result.setIcon(icon);
    QCOMPARE(result.icon(), icon);

    result.setIcon(QPlaceIcon());
    QVERIFY(result.icon().isEmpty());

    QProposedSearchResult result2;
    QVERIFY(QLocationTestUtils::compareEquality(result, result2));

    result2.setIcon(icon);
    QVERIFY(QLocationTestUtils::compareInequality(result, result2));

    result.setIcon(icon);
    QVERIFY(QLocationTestUtils::compareEquality(result, result2));
}

void tst_QProposedSearchResult::searchRequest()
{
    QProposedSearchResult result;
    QCOMPARE(result.searchRequest(), QPlaceSearchRequest());

    QPlaceSearchRequest placeSearchRequest;
    placeSearchRequest.setSearchContext(QUrl(QStringLiteral("http://www.example.com/")));
    result.setSearchRequest(placeSearchRequest);
    QCOMPARE(result.searchRequest(), placeSearchRequest);

    result.setSearchRequest(QPlaceSearchRequest());
    QCOMPARE(result.searchRequest(), QPlaceSearchRequest());

    QProposedSearchResult result2;
    QVERIFY(QLocationTestUtils::compareEquality(result, result2));

    result2.setSearchRequest(placeSearchRequest);
    QVERIFY(QLocationTestUtils::compareInequality(result, result2));

    result.setSearchRequest(placeSearchRequest);
    QVERIFY(QLocationTestUtils::compareEquality(result, result2));
}

void tst_QProposedSearchResult::conversion()
{
    QLocationTestUtils::testConversion<tst_QProposedSearchResult,
                                       QPlaceSearchResult,
                                       QProposedSearchResult>(this);
}

QTEST_APPLESS_MAIN(tst_QProposedSearchResult)

#include "tst_qproposedsearchresult.moc"
/***************************************************************************
    Copyright (C) 2010 Robby Stephenson <robby@periapsis.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of        *
 *   the License or (at your option) version 3 or any later version        *
 *   accepted by the membership of KDE e.V. (or its successor approved     *
 *   by the membership of KDE e.V.), which shall act as a proxy            *
 *   defined in Section 14 of version 3 of the license.                    *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 ***************************************************************************/

#undef QT_NO_CAST_FROM_ASCII

#include "qtest_kde.h"
#include "freebasefetchertest.h"
#include "freebasefetchertest.moc"

#include "../fetch/fetcherjob.h"
#include "../fetch/freebasefetcher.h"
#include "../collections/bookcollection.h"
#include "../collections/videocollection.h"
#include "../collections/gamecollection.h"
#include "../collections/boardgamecollection.h"
#include "../collectionfactory.h"
#include "../entry.h"
#include "../images/imagefactory.h"

QTEST_KDEMAIN( FreebaseFetcherTest, GUI )

FreebaseFetcherTest::FreebaseFetcherTest() : m_loop(this) {
}

void FreebaseFetcherTest::initTestCase() {
  Tellico::RegisterCollection<Tellico::Data::BookCollection> registerBook(Tellico::Data::Collection::Book, "book");
  Tellico::RegisterCollection<Tellico::Data::GameCollection> registerGame(Tellico::Data::Collection::Game, "game");
  Tellico::RegisterCollection<Tellico::Data::BoardGameCollection> registerBoard(Tellico::Data::Collection::BoardGame, "boardgame");
  Tellico::RegisterCollection<Tellico::Data::VideoCollection> registerVideo(Tellico::Data::Collection::Video, "video");
  Tellico::ImageFactory::init();

  QHash<QString, QString> coding;
  coding.insert(QLatin1String("title"), QLatin1String("c++ coding standards: 101 rules, guidelines, and best practices"));
  coding.insert(QLatin1String("publisher"), QLatin1String("Addison-Wesley"));
  coding.insert(QLatin1String("pub_year"), QLatin1String("2004"));
  coding.insert(QLatin1String("isbn"), QLatin1String("9780321113580"));
  coding.insert(QLatin1String("author"), QLatin1String("Herb Sutter"));
  coding.insert(QLatin1String("series"), QLatin1String("C++ In-Depth Series"));
  coding.insert(QLatin1String("lccn"), QLatin1String("2004022605"));
  coding.insert(QLatin1String("binding"), QLatin1String("Trade Paperback"));
  coding.insert(QLatin1String("pages"), QLatin1String("220"));

  m_fieldValues.insert(QLatin1String("coding"), coding);
}

void FreebaseFetcherTest::testBookTitle() {
  Tellico::Fetch::FetchRequest request(Tellico::Data::Collection::Book, Tellico::Fetch::Title,
                                       QLatin1String("c++ coding standards"));
  Tellico::Fetch::Fetcher::Ptr fetcher(new Tellico::Fetch::FreebaseFetcher(this));

  // don't use 'this' as job parent, it crashes
  Tellico::Fetch::FetcherJob* job = new Tellico::Fetch::FetcherJob(0, fetcher, request);
  connect(job, SIGNAL(result(KJob*)), this, SLOT(slotResult(KJob*)));
  job->setMaximumResults(1);

  job->start();
  m_loop.exec();

  QCOMPARE(m_results.size(), 1);

  Tellico::Data::EntryPtr entry = m_results.at(0);
  QHashIterator<QString, QString> i(m_fieldValues.value(QLatin1String("coding")));
  while(i.hasNext()) {
    i.next();
    QString result = entry->field(i.key()).toLower();
    QCOMPARE(result, i.value().toLower());
  }
  QVERIFY(!entry->field(QLatin1String("cover")).isEmpty());
}

void FreebaseFetcherTest::testBookAuthor() {
  Tellico::Fetch::FetchRequest request(Tellico::Data::Collection::Book, Tellico::Fetch::Person,
                                       QLatin1String("herb sutter"));
  Tellico::Fetch::Fetcher::Ptr fetcher(new Tellico::Fetch::FreebaseFetcher(this));

  // don't use 'this' as job parent, it crashes
  Tellico::Fetch::FetcherJob* job = new Tellico::Fetch::FetcherJob(0, fetcher, request);
  connect(job, SIGNAL(result(KJob*)), this, SLOT(slotResult(KJob*)));

  job->start();
  m_loop.exec();

  QCOMPARE(m_results.size(), 4);

  Tellico::Data::EntryPtr entry = m_results.at(0);
  QHashIterator<QString, QString> i(m_fieldValues.value(QLatin1String("coding")));
  while(i.hasNext()) {
    i.next();
    QString result = entry->field(i.key()).toLower();
    QCOMPARE(result, i.value().toLower());
  }
  QVERIFY(!entry->field(QLatin1String("cover")).isEmpty());
}

void FreebaseFetcherTest::testISBN() {
  Tellico::Fetch::FetchRequest request(Tellico::Data::Collection::Book, Tellico::Fetch::ISBN,
                                       QLatin1String("9780321113580"));
  Tellico::Fetch::Fetcher::Ptr fetcher(new Tellico::Fetch::FreebaseFetcher(this));

  // don't use 'this' as job parent, it crashes
  Tellico::Fetch::FetcherJob* job = new Tellico::Fetch::FetcherJob(0, fetcher, request);
  connect(job, SIGNAL(result(KJob*)), this, SLOT(slotResult(KJob*)));
  job->setMaximumResults(1);

  job->start();
  m_loop.exec();

  QCOMPARE(m_results.size(), 1);

  Tellico::Data::EntryPtr entry = m_results.at(0);
  QHashIterator<QString, QString> i(m_fieldValues.value(QLatin1String("coding")));
  while(i.hasNext()) {
    i.next();
    QString result = entry->field(i.key()).toLower();
    QCOMPARE(result, i.value().toLower());
  }
  QVERIFY(!entry->field(QLatin1String("cover")).isEmpty());
}

void FreebaseFetcherTest::testLCCN() {
  Tellico::Fetch::FetchRequest request(Tellico::Data::Collection::Book, Tellico::Fetch::LCCN,
                                       QLatin1String("2004022605"));
  Tellico::Fetch::Fetcher::Ptr fetcher(new Tellico::Fetch::FreebaseFetcher(this));

  // don't use 'this' as job parent, it crashes
  Tellico::Fetch::FetcherJob* job = new Tellico::Fetch::FetcherJob(0, fetcher, request);
  connect(job, SIGNAL(result(KJob*)), this, SLOT(slotResult(KJob*)));
  job->setMaximumResults(1);

  job->start();
  m_loop.exec();

  QCOMPARE(m_results.size(), 1);

  Tellico::Data::EntryPtr entry = m_results.at(0);
  QHashIterator<QString, QString> i(m_fieldValues.value(QLatin1String("coding")));
  while(i.hasNext()) {
    i.next();
    QString result = entry->field(i.key()).toLower();
    QCOMPARE(result, i.value().toLower());
  }
  QVERIFY(!entry->field(QLatin1String("cover")).isEmpty());
}

void FreebaseFetcherTest::testMovieTitle() {
  Tellico::Fetch::FetchRequest request(Tellico::Data::Collection::Video, Tellico::Fetch::Title,
                                       QLatin1String("man from snowy river"));
  Tellico::Fetch::Fetcher::Ptr fetcher(new Tellico::Fetch::FreebaseFetcher(this));

  // don't use 'this' as job parent, it crashes
  Tellico::Fetch::FetcherJob* job = new Tellico::Fetch::FetcherJob(0, fetcher, request);
  connect(job, SIGNAL(result(KJob*)), this, SLOT(slotResult(KJob*)));
  job->setMaximumResults(1);

  job->start();
  m_loop.exec();

  QCOMPARE(m_results.size(), 1);

  Tellico::Data::EntryPtr entry = m_results.at(0);
  QCOMPARE(entry->field(QLatin1String("title")), QLatin1String("The Man From Snowy River"));
  QCOMPARE(entry->field(QLatin1String("director")), QLatin1String("George T. Miller; George Miller"));
  QCOMPARE(entry->field(QLatin1String("producer")), QLatin1String("Simon Wincer"));
  QCOMPARE(entry->field(QLatin1String("writer")), QLatin1String("Banjo Paterson"));
  QCOMPARE(entry->field(QLatin1String("composer")), QLatin1String("Bruce Rowland"));
  QCOMPARE(entry->field(QLatin1String("certification")), QLatin1String("PG (USA)"));
  QCOMPARE(entry->field(QLatin1String("running-time")), QLatin1String("102"));
  QCOMPARE(entry->field(QLatin1String("year")), QLatin1String("1982"));
  QCOMPARE(entry->field(QLatin1String("genre")), QLatin1String("Western; Drama"));
  QCOMPARE(entry->field(QLatin1String("nationality")), QLatin1String("Australia"));
  QVERIFY(!entry->field(QLatin1String("cover")).isEmpty());
  QVERIFY(!entry->field(QLatin1String("plot")).isEmpty());
  QStringList castList = Tellico::FieldFormat::splitTable(entry->field("cast"));
  QCOMPARE(castList.at(0), QLatin1String("Tom Burlinson::Jim Craig"));
  QCOMPARE(castList.size(), 4);
}

void FreebaseFetcherTest::testGameTitle() {
  Tellico::Fetch::FetchRequest request(Tellico::Data::Collection::Game, Tellico::Fetch::Title,
                                       QLatin1String("halo 3:odst"));
  Tellico::Fetch::Fetcher::Ptr fetcher(new Tellico::Fetch::FreebaseFetcher(this));

  // don't use 'this' as job parent, it crashes
  Tellico::Fetch::FetcherJob* job = new Tellico::Fetch::FetcherJob(0, fetcher, request);
  connect(job, SIGNAL(result(KJob*)), this, SLOT(slotResult(KJob*)));
  job->setMaximumResults(1);

  job->start();
  m_loop.exec();

  QCOMPARE(m_results.size(), 1);

  Tellico::Data::EntryPtr entry = m_results.at(0);
  QCOMPARE(entry->field(QLatin1String("title")).toLower(), QLatin1String("halo 3: odst"));
  QCOMPARE(entry->field(QLatin1String("developer")), QLatin1String("Bungie Studios"));
  QCOMPARE(entry->field(QLatin1String("publisher")), QLatin1String("Microsoft Game Studios"));
  QCOMPARE(entry->field(QLatin1String("year")), QLatin1String("2009"));
  QCOMPARE(entry->field(QLatin1String("genre")), QLatin1String("First-person Shooter"));
  QVERIFY(!entry->field(QLatin1String("cover")).isEmpty());
  QVERIFY(!entry->field(QLatin1String("description")).isEmpty());
}

void FreebaseFetcherTest::testBoardGameTitle() {
  Tellico::Fetch::FetchRequest request(Tellico::Data::Collection::BoardGame, Tellico::Fetch::Title,
                                       QLatin1String("settlers of catan"));
  Tellico::Fetch::Fetcher::Ptr fetcher(new Tellico::Fetch::FreebaseFetcher(this));

  // don't use 'this' as job parent, it crashes
  Tellico::Fetch::FetcherJob* job = new Tellico::Fetch::FetcherJob(0, fetcher, request);
  connect(job, SIGNAL(result(KJob*)), this, SLOT(slotResult(KJob*)));
  job->setMaximumResults(1);

  job->start();
  m_loop.exec();

  QCOMPARE(m_results.size(), 1);

  Tellico::Data::EntryPtr entry = m_results.at(0);
  QCOMPARE(entry->field(QLatin1String("title")).toLower(), QLatin1String("settlers of catan"));
  QCOMPARE(entry->field(QLatin1String("designer")), QLatin1String("Klaus Teuber"));
  QCOMPARE(entry->field(QLatin1String("publisher")), QLatin1String("999 Games; Mayfair Games; Kosmos; Capcom"));
  QCOMPARE(entry->field(QLatin1String("year")), QLatin1String("1995"));
  QCOMPARE(entry->field(QLatin1String("genre")), QLatin1String("German-style"));
  QCOMPARE(entry->field(QLatin1String("num-player")), QLatin1String("3; 4"));
  QVERIFY(!entry->field(QLatin1String("cover")).isEmpty());
  QVERIFY(!entry->field(QLatin1String("description")).isEmpty());
}

void FreebaseFetcherTest::slotResult(KJob* job_) {
  m_results = static_cast<Tellico::Fetch::FetcherJob*>(job_)->entries();
  m_loop.quit();
}

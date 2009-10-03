/***************************************************************************
    Copyright (C) 2009 Robby Stephenson <robby@periapsis.org>
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

#include "fieldformat.h"
#include "core/tellico_config.h"

using Tellico::FieldFormat;

//these get overwritten, but are here since they're static
// calling Config::articleList() has to run QString::split() every time, so save a copy
QStringList FieldFormat::articles;
QStringList FieldFormat::articlesApos;
QRegExp FieldFormat::delimiterRx = QRegExp(QLatin1String("\\s*;\\s*"));
QRegExp FieldFormat::commaSplitRx = QRegExp(QLatin1String("\\s*,\\s*"));

QString FieldFormat::delimiterString() {
  return QLatin1String("; ");
}

QRegExp FieldFormat::delimiterRegExp() {
  return delimiterRx;
}

QString FieldFormat::fixupValue(const QString& value_) {
  QString value = value_;
  value.replace(delimiterRegExp(), delimiterString());
  return value;
}

QString FieldFormat::columnDelimiterString() {
  return QLatin1String("::");
}

QString FieldFormat::rowDelimiterString() {
  return QChar(0x2028);
}

QStringList FieldFormat::splitValue(const QString& string_, SplitParsing parsing_, QString::SplitBehavior behavior_) {
  return parsing_ == StringSplit ? string_.split(delimiterString(), behavior_)
                                 : string_.split(delimiterRegExp(), behavior_);
}

QStringList FieldFormat::splitRow(const QString& string_, QString::SplitBehavior behavior_) {
  return string_.split(columnDelimiterString(), behavior_);
}

QStringList FieldFormat::splitTable(const QString& string_, QString::SplitBehavior behavior_) {
  return string_.split(rowDelimiterString(), behavior_);
}

QString FieldFormat::title(const QString& title_) {
  QString newTitle = title_;
  // special case for multi-column tables, assume user never has column delimiter in a value
  QString tail;
  if(newTitle.indexOf(columnDelimiterString()) > -1) {
    tail = columnDelimiterString() + newTitle.section(columnDelimiterString(), 1);
    newTitle = newTitle.section(columnDelimiterString(), 0, 0);
  }

  if(Config::autoFormat()) {
    // arbitrarily impose rule that a space must follow every comma
    // has to come before the capitalization since the space is significant
    newTitle.replace(commaSplitRx, QLatin1String(", "));
  }

  if(Config::autoCapitalization()) {
    newTitle = capitalize(newTitle);
  }

  if(Config::autoFormat()) {
    const QString lower = newTitle.toLower();
    // TODO if the title has ",the" at the end, put it at the front
    foreach(const QString& article, articles) {
      // assume white space is already stripped
      // the articles are already in lower-case
      if(lower.startsWith(article + QLatin1Char(' '))) {
        QRegExp regexp(QLatin1Char('^') + QRegExp::escape(article) + QLatin1String("\\s*"), Qt::CaseInsensitive);
        // can't just use article since it's in lower-case
        QString titleArticle = newTitle.left(article.length());
        newTitle = newTitle.remove(regexp)
                           .append(QLatin1String(", "))
                           .append(titleArticle);
        break;
      }
    }
  }

  return newTitle + tail;
}

QString FieldFormat::name(const QString& name_, bool multiple_/*=true*/) {
  static const QRegExp spaceComma(QLatin1String("[\\s,]"));
  // the ending look-ahead is so that a space is not added at the end
  static const QRegExp periodSpace(QLatin1String("\\.\\s*(?=.)"));

  QStringList entries;
  if(multiple_) {
    // split by semi-colon, optionally preceded or followed by white spacee
    entries = name_.split(delimiterRegExp(), QString::SkipEmptyParts);
  } else {
    entries << name_;
  }

  QRegExp lastWord;
  lastWord.setCaseSensitivity(Qt::CaseInsensitive);

  QStringList names;
  for(QStringList::ConstIterator it = entries.constBegin(); it != entries.constEnd(); ++it) {
    QString name = *it;
    // special case for 2-column tables, assume user never has '::' in a value
    QString tail;
    if(name.indexOf(columnDelimiterString()) > -1) {
      tail = columnDelimiterString() + name.section(columnDelimiterString(), 1);
      name = name.section(columnDelimiterString(), 0, 0);
    }
    name.replace(periodSpace, QLatin1String(". "));
    if(Config::autoCapitalization()) {
      name = capitalize(name);
    }

    // split the name by white space and commas
    QStringList words = name.split(spaceComma, QString::SkipEmptyParts);
    lastWord.setPattern(QLatin1Char('^') + QRegExp::escape(words.last()) + QLatin1Char('$'));

    // if it contains a comma already and the last word is not a suffix, don't format it
    if(!Config::autoFormat() || (name.indexOf(QLatin1Char(',')) > -1 && Config::nameSuffixList().filter(lastWord).isEmpty())) {
      // arbitrarily impose rule that no spaces before a comma and
      // a single space after every comma
      name.replace(commaSplitRx, QLatin1String(", "));
      names << name + tail;
      continue;
    }
    // otherwise split it by white space, move the last word to the front
    // but only if there is more than one word
    if(words.count() > 1) {
      // if the last word is a suffix, it has to be kept with last name
      if(Config::nameSuffixList().filter(lastWord).count() > 0) {
        words.prepend(words.last().append(QLatin1Char(',')));
        words.removeLast();
      }

      // now move the word
      // adding comma here when there had been a suffix is because it was originally split with space or comma
      words.prepend(words.last().append(QLatin1Char(',')));
      words.removeLast();

      // update last word regexp
      lastWord.setPattern(QLatin1Char('^') + QRegExp::escape(words.last()) + QLatin1Char('$'));

      // this is probably just something for me, limited to english
      while(Config::surnamePrefixList().filter(lastWord).count() > 0) {
        words.prepend(words.last());
        words.removeLast();
        lastWord.setPattern(QLatin1Char('^') + QRegExp::escape(words.last()) + QLatin1Char('$'));
      }

      names << words.join(QLatin1String(" ")) + tail;
    } else {
      names << name + tail;
    }
  }

  return names.join(QLatin1String("; "));
}

QString FieldFormat::date(const QString& date_) {
  // internally, this is "year-month-day"
  // any of the three may be empty
  // if they're not digits, return the original string
  bool empty = true;
  // for empty year, use current
  // for empty month or date, use 1
  QStringList s = date_.split(QLatin1Char('-'));
  bool ok = true;
  int y = s.count() > 0 ? s[0].toInt(&ok) : QDate::currentDate().year();
  if(ok) {
    empty = false;
  } else {
    y = QDate::currentDate().year();
  }
  int m = s.count() > 1 ? s[1].toInt(&ok) : 1;
  if(ok) {
    empty = false;
  } else {
    m = 1;
  }
  int d = s.count() > 2 ? s[2].toInt(&ok) : 1;
  if(ok) {
    empty = false;
  } else {
    d = 1;
  }
  // rather use ISO date formatting than locale formatting for now. Primarily, it makes sorting just work.
  return empty ? date_ : QDate(y, m, d).toString(Qt::ISODate);
  // use short form
//  return KGlobal::locale()->formatDate(date, true);
}

QString FieldFormat::capitalize(QString str_, bool checkConfig_) {
  if(str_.isEmpty()) {
    return str_;
  }

  if(checkConfig_ && !Config::autoCapitalization()) {
    return str_;
  }

  // regexp to split words
  const QRegExp rx(QLatin1String("[-\\s,.;]"));

  // first letter is always capitalized
  str_.replace(0, 1, str_.at(0).toUpper());

  // special case for french words like l'espace

  int pos = rx.indexIn(str_, 1);
  int nextPos;

  QRegExp wordRx;
  wordRx.setCaseSensitivity(Qt::CaseInsensitive);

  QStringList notCap = Config::noCapitalizationList();
  // don't capitalize the surname prefixes
  // does this hold true everywhere other than english?
  notCap += Config::surnamePrefixList();

  QString word = str_.mid(0, pos);
  // now check to see if words starts with apostrophe list
  for(QStringList::ConstIterator it = articlesApos.constBegin(); it != articlesApos.constEnd(); ++it) {
    if(word.startsWith(*it, Qt::CaseInsensitive)) {
      uint l = (*it).length();
      str_.replace(l, 1, str_.at(l).toUpper());
      break;
    }
  }

  while(pos > -1) {
    // also need to compare against list of non-capitalized words
    nextPos = rx.indexIn(str_, pos+1);
    if(nextPos == -1) {
      nextPos = str_.length();
    }
    word = str_.mid(pos+1, nextPos-pos-1);
    bool aposMatch = false;
    // now check to see if words starts with apostrophe list
    for(QStringList::ConstIterator it = articlesApos.constBegin(); it != articlesApos.constEnd(); ++it) {
      if(word.startsWith(*it, Qt::CaseInsensitive)) {
        uint l = (*it).length();
        str_.replace(pos+l+1, 1, str_.at(pos+l+1).toUpper());
        aposMatch = true;
        break;
      }
    }

    if(!aposMatch) {
      wordRx.setPattern(QLatin1Char('^') + QRegExp::escape(word) + QLatin1Char('$'));
      if(notCap.filter(wordRx).isEmpty() && nextPos-pos > 1) {
        str_.replace(pos+1, 1, str_.at(pos+1).toUpper());
      }
    }

    pos = rx.indexIn(str_, pos+1);
  }
  return str_;
}

QString FieldFormat::sortKeyTitle(const QString& title_) {
  const QString lower = title_.toLower();
  foreach(const QString& article, articles) {
    // assume white space is already stripped
    // the articles are already in lower-case
    if(lower.startsWith(article + QLatin1Char(' '))) {
      return title_.mid(article.length() + 1);
    }
  }
  // check apostrophes, too
  foreach(const QString& article, articlesApos) {
    if(lower.startsWith(article)) {
      return title_.mid(article.length());
    }
  }
  return title_;
}

// articles should all be in lower-case
void FieldFormat::articlesUpdated() {
  articles = Config::articleList();
  articlesApos.clear();
  foreach(const QString& article, articles) {
    if(article.endsWith(QLatin1Char('\''))) {
      articlesApos += article;
    }
  }
}

void FieldFormat::stripArticles(QString& value) {
  if(Config::articleList().isEmpty()) {
    return;
  }
  foreach(const QString& article, articles) {
    QRegExp rx(QLatin1String("\\b") + article + QLatin1String("\\b"));
    value.remove(rx);
  }
  value = value.trimmed();
  value.remove(QRegExp(QLatin1String(",$")));
}

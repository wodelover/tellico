/***************************************************************************
    copyright            : (C) 2003-2008 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#include "textimporter.h"
#include "../core/filehandler.h"

using Tellico::Import::TextImporter;

TextImporter::TextImporter(const KUrl& url_, bool useUTF8_)
    : Import::Importer(url_) {
  if(url_.isValid()) {
    setText(FileHandler::readTextFile(url_, false, useUTF8_));
  }
}

TextImporter::TextImporter(const QString& text_) : Import::Importer(text_) {
}

#include "textimporter.moc"

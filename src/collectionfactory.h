/***************************************************************************
    copyright            : (C) 2003-2006 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#ifndef COLLECTIONFACTORY_H
#define COLLECTIONFACTORY_H

#include "collection.h"

#include <qmap.h>

namespace Tellico {

typedef QMap<Data::Collection::Type, QString> CollectionNameMap;

/**
 * A factory class for dealing with the different types of collections.
 *
 * @author Robby Stephenson
 */
class CollectionFactory {
public:
  static Data::CollPtr collection(Data::Collection::Type type, bool addFields,
                                      const QString& entryTitle=QString::null);
  static Data::CollPtr collection(const QString& typeName, bool addFields);
  static CollectionNameMap nameMap();
  static QString typeName(Data::Collection::Type type);
  static bool isDefaultField(Data::Collection::Type type, const QString& name);
};

} // end namespace
#endif

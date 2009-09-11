/***************************************************************************
    Copyright (C) 2003-2009 Robby Stephenson <robby@periapsis.org>
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

#include "groupiterator.h"
#include "entrygroup.h"
#include "models/models.h"

#include <QAbstractItemModel>

using Tellico::GroupIterator;

GroupIterator::GroupIterator() : m_model(0), m_row(0) {
}

GroupIterator::GroupIterator(QAbstractItemModel* model_) : m_model(model_), m_row(0) {
}

bool GroupIterator::isValid() const {
  return m_model != 0;
}

GroupIterator& GroupIterator::operator++() {
  ++m_row;
  return *this;
}

Tellico::Data::EntryGroup* GroupIterator::group() {
  if(!m_model || m_row >= m_model->rowCount()) {
    return 0;
  }

  return m_model->data(m_model->index(m_row, 0), GroupPtrRole).value<Data::EntryGroup*>();
}

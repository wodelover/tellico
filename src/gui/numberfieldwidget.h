/***************************************************************************
    copyright            : (C) 2005-2006 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#ifndef NUMBERFIELDWIDGET_H
#define NUMBERFIELDWIDGET_H

class KLineEdit;

#include "fieldwidget.h"
#include "../datavectors.h"

namespace Tellico {
  namespace GUI {
    class SpinBox;

/**
 * @author Robby Stephenson
 */
class NumberFieldWidget : public FieldWidget {
Q_OBJECT

public:
  NumberFieldWidget(Data::FieldPtr field, QWidget* parent, const char* name=0);
  virtual ~NumberFieldWidget() {}

  virtual QString text() const;
  virtual void setText(const QString& text);

public slots:
  virtual void clear();

protected:
  virtual QWidget* widget();
  virtual void updateFieldHook(Data::FieldPtr oldField, Data::FieldPtr newField);

private:
  bool isSpinBox() const { return (m_spinBox != 0); }
  void initLineEdit();
  void initSpinBox();

  KLineEdit* m_lineEdit;
  SpinBox* m_spinBox;
};

  } // end GUI namespace
} // end namespace
#endif

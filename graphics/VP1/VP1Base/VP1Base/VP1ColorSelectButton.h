/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1ColorSelectButton             //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: June 2007                             //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1COLORSELECTBUTTON_H
#define VP1COLORSELECTBUTTON_H

#include <QPushButton>

class VP1ColorSelectButton : public QPushButton {

  Q_OBJECT

public:

  VP1ColorSelectButton(QWidget*parent=0, const QColor& initialcolor = Qt::black, int dim = 24 );//dim<=0 => Won't change sizepolicy
  virtual ~VP1ColorSelectButton();

  QColor color() const;
  void setText ( const QString &  );//Forbidden!! Only here since Designer generated code needs it in public.
  static void setColButtonProperties(QPushButton*,const QColor& col,int dim);

public slots:
  void launchColorChooser();
  void setColor(const QColor&);
  void setDimension(int dim);
signals:
  void colorChanged(const QColor&);
  void aboutToShowColorDialog();

protected slots:
  void updateButton();
private:
  class Imp;
  Imp * m_d;
};

#endif

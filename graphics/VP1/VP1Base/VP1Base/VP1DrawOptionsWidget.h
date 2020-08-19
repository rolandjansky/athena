/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1DrawOptionsWidget                //
//                                                            //
//  Description: Widget for setting complexity, line width,   //
//               point sizes and base colouring.              //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: August 2008                              //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1DRAWOPTIONSWIDGET_H
#define VP1DRAWOPTIONSWIDGET_H

// FIXME: need to include this before any QT header, because QT defines
// a macro `foreach' that collides with an identifier in AthenaKernel.
#include "xAODCore/BaseInfo.h"

#include <QWidget>

class SoGroup;

class VP1DrawOptionsWidget : public QWidget {

  Q_OBJECT

public:

  VP1DrawOptionsWidget(QWidget * parent = 0);
  virtual ~VP1DrawOptionsWidget();

  //Put this in the scene graph:
  SoGroup * drawOptionsGroup() const;

  //Use these to set defaults (ASAP after construction)
  //NB: Due to constraints in granularities, the values might not end
  //up exactly the same as the arguments.
  void setComplexity(const double&);//default 0.5
  void setLineWidths(const double&);//default 1.0
  void setPointSizes(const double&);//default 1.0
  void setUseBaseLighting(bool);//default is false

  //And to fine tune options:
  void setComplexityDisabled(bool b=true);//In case you have no curved surfaces you don't need complexity controls
  void setLineWidthsDisabled(bool b=true);//In case you have no lines surfaces you don't need line width controls
  void setPointSizesDisabled(bool b=true);//In case you have no points you don't need point size controls
  void setBaseLightingDisabled(bool b=true);//In case you don't need this

  QByteArray state() const;
  void applyState(QByteArray);

  //We might need the complexity elsewhere:
  double complexity() const;
signals:
  void complexityChanged(const double&);
private:
  class Imp;
  Imp * m_d;
private slots:
  void updateNodes();
};

#endif

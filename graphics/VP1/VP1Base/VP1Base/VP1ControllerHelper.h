/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1ControllerHelper              //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1CONTROLLERHELPER_H
#define VP1CONTROLLERHELPER_H

// FIXME: need to include this before any QT header, because QT defines
// a macro `foreach' that collides with an identifier in AthenaKernel.
#include "xAODCore/BaseInfo.h"

#include <QMap>
#include <QPair>

class QWidget;
class IVP1System;
class QCheckBox;
class VP1ColorSelectButton;
class VP1TabWidget;

class VP1ControllerHelper {
public:

  static QWidget * compositionController( const QList<IVP1System*>& systemsWithControllersAllowed,
					  QMap<IVP1System*,QWidget*>& sys2tabpage,
					  VP1TabWidget*&tabwidget,
					  const QList<QPair<IVP1System*,bool> >& system2switchable,
					  QMap<QCheckBox*,IVP1System*>& checkbox2system,
					  VP1ColorSelectButton* & colorselectbutton,
					  QWidget * extrawidget = 0, bool nobgdcolorsel = false );
  //Fixme: Add option to put in an extra custom list of tabs

private:
  VP1ControllerHelper();
  ~VP1ControllerHelper();

  VP1ControllerHelper(const VP1ControllerHelper & );
  VP1ControllerHelper & operator= (const VP1ControllerHelper & );

};

#endif

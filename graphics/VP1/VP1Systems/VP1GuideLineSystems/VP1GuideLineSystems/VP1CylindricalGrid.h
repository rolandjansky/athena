/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1CylindricalGrid                  //
//                                                            //
//  Description: Helper class providing a cylindrical grid.   //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1CYLINDRICALGRID_H
#define VP1CYLINDRICALGRID_H

#include "VP1Base/VP1HelperClassBase.h"
#include <QObject>
#include <Inventor/C/errors/debugerror.h>
#include <Inventor/SbColor4f.h>
class SoSeparator;

class VP1CylindricalGrid : public QObject, public VP1HelperClassBase {

  Q_OBJECT

public:

  VP1CylindricalGrid( SoSeparator * attachsep,//where the grid separator will attach itself when visible
		    IVP1System * sys,QObject * parent = 0);
  virtual ~VP1CylindricalGrid();

public slots:

  void setShown(bool);//will attach/detach itself from attachsep depending on this
  void setColourAndTransp(const SbColor4f&);
  void setExtent(const double&);//A negative value means that negative parts will be shown as well
  void setSpacing(const double&);

private:
  class Imp;
  Imp * m_d;

};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1CoordinateAxes                   //
//                                                            //
//  Description: Helper class providing coordinate axes       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1COORDINATEAXES_H
#define VP1COORDINATEAXES_H

#include "VP1Base/VP1HelperClassBase.h"
#include <QObject>

class SoMaterial;
class SoSeparator;

class VP1CoordinateAxes : public QObject, public VP1HelperClassBase {

  Q_OBJECT

public:

  VP1CoordinateAxes( SoMaterial * xmat,SoMaterial * ymat,SoMaterial * zmat,
		     SoSeparator * attachsep,//where the axis
					     //separator will attach
					     //itself when visible
		     IVP1System * sys,QObject * parent = 0);
  virtual ~VP1CoordinateAxes();

public slots:
  void setShown(bool);//will attach/detach itself from attachsep depending on this
  void setLength(const double&);//A negative value means that negative parts will be shown as well
  void setPosition(const SbVec3f&);
  void setRelativeAxisThickness(const double&);
private:
  class Imp;
  Imp * m_d;
};

#endif

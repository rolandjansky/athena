/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1Floor                            //
//                                                            //
//  Description: Helper class providing a floor.              //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1FLOOR_H
#define VP1FLOOR_H

#include "VP1Base/VP1HelperClassBase.h"
#include <QObject>
#include <Inventor/C/errors/debugerror.h>
#include <Inventor/SbColor4f.h>
class SoSeparator;

class VP1Floor : public QObject, public VP1HelperClassBase {

  Q_OBJECT

public:

  //Static methods used in various places. Put it here to avoid copying code:
  static int nMax() { return 400; }
  static bool calcParsFromExtentAndSpacing( VP1HelperClassBase*,const double& extent, const double& spacing,
					    const int& nmaxlimit, int& nmax, double& distmax );

  VP1Floor( SoSeparator * attachsep,//where the floor separator will attach itself when visible
	    IVP1System * sys,QObject * parent = 0);
  virtual ~VP1Floor();

public slots:

  void setShown(bool);//will attach/detach itself from attachsep depending on this
  void setColourAndTransp(const SbColor4f&);
  void setExtent(const double&);//A negative value means that negative parts will be shown as well
  void setSpacing(const double&);
  void setVerticalPosition(const double&);

private:
  class Imp;
  Imp * d;

};

#endif

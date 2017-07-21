/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1EtaCone                          //
//                                                            //
//  Description: Helper class providing an eta cone.          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1ETACONE_H
#define VP1ETACONE_H

#include "VP1Base/VP1HelperClassBase.h"
#include <QObject>

class SoMaterial;
class SoSeparator;

class VP1EtaCone : public QObject, public VP1HelperClassBase {

  Q_OBJECT

public:

  //Used in various places. Put it here to avoid copying code:
  static bool calcParsFromExtentAndSpacing( VP1HelperClassBase*,const double& extent, const double& spacing,
					    const int& nmaxlimit, int& nmax, double& distmax );

  VP1EtaCone( SoMaterial * mat,
	      SoSeparator * attachsep,//where the cone separator will attach itself when visible
	      IVP1System * sys,QObject * parent = 0);
  virtual ~VP1EtaCone();

public slots:

  void setShown(bool);//will attach/detach itself from attachsep depending on this
  void setExtent(const double&);//>0: extent means to a given radius, <0: extent means to a given z.
  void setEtaValue(const double&);

private:
  class Imp;
  Imp * d;

};

#endif

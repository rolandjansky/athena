/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1TrackingVolumes                   //
//                                                            //
//  Description: Helper class providing coordinate axes       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1TRACKINGVOLUMES_H
#define VP1TRACKINGVOLUMES_H

#include "VP1Base/VP1HelperClassBase.h"
#include <QObject>

class SoMaterial;
class SoSeparator;

class VP1TrackingVolumes : public QObject, public VP1HelperClassBase {

  Q_OBJECT

public:

  VP1TrackingVolumes( SoMaterial * materialID,SoMaterial * materialCalo,SoMaterial * materialMS,
                     bool showID, bool showCalo, bool showMS,
                     SoSeparator * attachsep,//where the vol
					     //separator will attach
					     //itself when visible
		     IVP1System * sys,QObject * parent = 0);
  virtual ~VP1TrackingVolumes();

public slots:
  void setShown(bool);//!< will attach/detach itself from attachsep depending on this
  void setShownID(bool);//!< will attach/detach ID from sep depending on this
  void setShownCalo(bool);//!< will attach/detach Calo from sep depending on this
  void setShownMS(bool);//!< will attach/detach MS from sep depending on this

private:
  class Imp;
  Imp * m_d;
};

#endif

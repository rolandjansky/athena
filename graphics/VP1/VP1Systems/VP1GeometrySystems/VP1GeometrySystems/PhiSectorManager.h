/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHISECTORMANAGER_H
#define PHISECTORMANAGER_H

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Header file for class PhiSectorManager                             //
//                                                                     //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Initial VP1 version: September 2007                                //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include <QObject>
#include "VP1GeometrySystems/VP1GeoFlags.h"

class PhiSectionWidget;

class SoSwitch;
class SoSeparator;
class SoNode;
class SbMatrix;
class VP1ExtraSepLayerHelper;
class IVP1System;

class PhiSectorManager : public QObject {

  Q_OBJECT

public:

  PhiSectorManager(PhiSectionWidget *, IVP1System *, QObject*parent=0);
  virtual ~PhiSectorManager();

  //Used by the geometry system to let the manager know under separator stuff for a given subsystem goes:
  void registerSubSystemSeparator(VP1GeoFlags::SubSystemFlag flag,SoSeparator * subsystemsep);

  //Volume handles first call this method to figure out what kind of volume they are:
  int getVolumeType(const SbMatrix& transform, SoNode * shape) const;
  //The return value, iphi, indicates the type:
  // 0...(nphi-1): Volume with a bounding box not around the Z axis.
  //  -1: Volume has bounding box around Z-axis, but can not be dynamically altered  (probably boolean)
  //  -2: Volume is around Z-axis AND can be dynamically subdivided (SoTubs, SoCons, SoPcons, SoLAr).
  //if return value was >=-1, attach it via (iphi is the return value from getVolumeType):
  VP1ExtraSepLayerHelper * getSepHelperForNode(VP1GeoFlags::SubSystemFlag flag, int iphi);
  
  /** Addition to handle label separators, in exactly the same way as getSepHelperForNode(...)*/
  VP1ExtraSepLayerHelper * getLabelSepHelperForNode(VP1GeoFlags::SubSystemFlag flag, int iphi);
  //if return value was -2, a call to the following is required
  //instead (again the returned helper is to be used to attach/detach
  //the nodesep to the scenegraph:
  VP1ExtraSepLayerHelper *  registerVolumeAroundZAxis( VP1GeoFlags::SubSystemFlag flag, SoSwitch* sw, const SbMatrix& tranformation );
  //NB: The switch should have your regular shape as its only child (other stuff for the dynamical volumes will be added dynamically):
  //NB: "tranformation" must be the complete transformation up to the shape.

  //For updating the phi-slicings:
  void updateRepresentationsOfVolsAroundZAxis();

  //If changing the states of a lot of volume handles, call these before and after.
  void largeChangesBegin();
  void largeChangesEnd();

protected slots:
  void enabledPhiSectorsChanged();
  void updateEnabledPhiSections(int);

private:

  class Imp;
  Imp * d;

};

#endif

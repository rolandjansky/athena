/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VOLUMEHANDLESHAREDDATA_H
#define VOLUMEHANDLESHAREDDATA_H

//This reference counted class keeps data (and related methods) which are common
//for all volume handle nodes under a given top-level handle.

#include "GeoModelKernel/GeoPVConstLink.h"
#include "VP1GeometrySystems/VP1GeoFlags.h"
#include <map>
class SoNode;
class SoGroup;
class SoSeparator;
class VolumeHandle;
class SoMaterial;
class PhiSectorManager;
class ZappedVolumeListModel;
class MatVisAttributes;
class VolVisAttributes;
class VP1GeoTreeView;
class GeoSysController;

class VolumeHandleSharedData {
public:
  VolumeHandleSharedData(GeoSysController * controller,VP1GeoFlags::SubSystemFlag,std::map<SoSeparator*,VolumeHandle*>* sonodesep2volhandle,
			 const GeoPVConstLink& motherpV,PhiSectorManager*,
			 SoMaterial * topMaterial,MatVisAttributes *,VolVisAttributes *,
			 ZappedVolumeListModel *, VP1GeoTreeView *, SoSeparator*);
  ~VolumeHandleSharedData();
  void ref();
  void unref();

  //Fixme: Inline?
  GeoPVConstLink geoPVConstLinkOfTreeTopsMother() const;
  PhiSectorManager* phiSectorManager() const;
  VP1GeoFlags::SubSystemFlag subSystemFlag() const;
  SoMaterial * fallBackTopLevelMaterial() const;
  VP1GeoTreeView * volumeBrowser() const { return m_volumebrowser; }
  MatVisAttributes * matVisAttributes() const;
  VolVisAttributes * volVisAttributes() const;
  inline SoSeparator* textSep() const {return m_textSep;} 
  GeoSysController* controller() const;
  void addZappedVolumeToGui(VolumeHandle*);
  void removeZappedVolumesFromGui(VolumeHandle*);

  SoNode * toShapeNode(const GeoPVConstLink& pV);//Returns shape of pV->getLogVol() (uses shared instancing as appropriate)
  SoNode * getSoCylinderOrientedLikeGeoTube(const double& radius, const double& halfLength);//(uses shared instancing as appropriate)

  void registerNodeSepForVolumeHandle(SoSeparator*,VolumeHandle*);

  static void setShowVolumeOutlines(SoGroup*nodesep,bool showvol);

  
private:

  // It is illegal to copy/assign a VolumeHandleSharedData:
  VolumeHandleSharedData(const VolumeHandleSharedData & );
  VolumeHandleSharedData & operator= (const VolumeHandleSharedData & );

  class Imp;
  Imp * d;

  //For inlines:
  VP1GeoTreeView * m_volumebrowser;
  SoSeparator* m_textSep; //!< base text sep

};

#endif

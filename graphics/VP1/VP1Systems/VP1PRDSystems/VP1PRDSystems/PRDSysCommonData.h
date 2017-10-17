/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class PRDSysCommonData                    //
//                                                            //
//  Description: Common pointers, data, node-to-object maps   //
//               etc. for the prd system                      //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef PRDSYSCOMMONDATA_H
#define PRDSYSCOMMONDATA_H

#include "VP1Base/VP1HelperClassBase.h"
class IVP13DSystem;
class HitsSoNodeManager;
class PRDSystemController;
class VP1SoMaterialMixer;
class MuonChamberProjectionHelper;
class InDetProjHelper;
class TouchedMuonChamberHelper;
class PRDTrackSegmentHelper;
class SoTransform;
class SoPath;
class PRDHandleBase;
namespace Trk { class PrepRawData; }

class PRDSysCommonData : public VP1HelperClassBase {
public:

  PRDSysCommonData(IVP13DSystem *, PRDSystemController*);
  virtual ~PRDSysCommonData();

  IVP13DSystem * system() const;
  HitsSoNodeManager * nodeManager() const;
  PRDSystemController * controller() const;
  VP1SoMaterialMixer * materialMixer() const;
  MuonChamberProjectionHelper * muonChamberProjectionHelper() const;
  InDetProjHelper * indetProjHelper_Pixel() const;
  InDetProjHelper * indetProjHelper_SCT() const;
  InDetProjHelper * indetProjHelper_TRT() const;
  TouchedMuonChamberHelper * touchedMuonChamberHelper() const;
  PRDTrackSegmentHelper * trackAndSegmentHelper() const;

  void registerTransform2Handle(SoTransform*transform,PRDHandleBase*handle);
  void registerPRD2Handle(const Trk::PrepRawData*prd,PRDHandleBase*handle);
  //Access pick->handle association (and pop path):
  PRDHandleBase * pickedPathToHandle( SoPath*pickedPath );

  void clearEventData(); //Deletes AscObjSelectionManager

private:

  class Imp;
  Imp * m_d;

  IVP13DSystem * m_3dsystem;
  HitsSoNodeManager * m_nodeManager;
  PRDSystemController * m_controller;
  VP1SoMaterialMixer * m_materialMixer;
  MuonChamberProjectionHelper * m_muonChamberProjectionHelper;
  InDetProjHelper * m_indetProjHelper_Pixel;
  InDetProjHelper * m_indetProjHelper_SCT;
  InDetProjHelper * m_indetProjHelper_TRT;
  TouchedMuonChamberHelper * m_touchedMuonChamberHelper;

  PRDTrackSegmentHelper * m_trackAndSegmentHelper;

};

inline IVP13DSystem* PRDSysCommonData::system() const { return m_3dsystem; }
inline HitsSoNodeManager* PRDSysCommonData::nodeManager() const { return m_nodeManager; }
inline PRDSystemController * PRDSysCommonData::controller() const { return m_controller; }
inline VP1SoMaterialMixer * PRDSysCommonData::materialMixer() const { return m_materialMixer; }
inline MuonChamberProjectionHelper * PRDSysCommonData::muonChamberProjectionHelper() const { return m_muonChamberProjectionHelper; }
inline InDetProjHelper * PRDSysCommonData::indetProjHelper_Pixel() const { return m_indetProjHelper_Pixel; }
inline InDetProjHelper * PRDSysCommonData::indetProjHelper_SCT() const { return m_indetProjHelper_SCT; }
inline InDetProjHelper * PRDSysCommonData::indetProjHelper_TRT() const { return m_indetProjHelper_TRT; }
inline TouchedMuonChamberHelper * PRDSysCommonData::touchedMuonChamberHelper() const { return m_touchedMuonChamberHelper; }
inline PRDTrackSegmentHelper * PRDSysCommonData::trackAndSegmentHelper() const { return m_trackAndSegmentHelper; }

#endif

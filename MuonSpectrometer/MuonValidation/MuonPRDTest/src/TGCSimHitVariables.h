/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSimHitVariables_H
#define TGCSimHitVariables_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <vector>

class TGCSimHitVariables : public ValAlgVariables
{
 public:
  TGCSimHitVariables(StoreGateSvc* evtStore,
                     const MuonGM::MuonDetectorManager* detManager,
                     const MuonIdHelper* idhelper,
                     TTree* tree,
                     std::string containername,
              MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl),
    m_TGC_Sim_stationName(nullptr),
    m_TGC_stationName(nullptr),
    m_TGC_stationEta(nullptr),
    m_TGC_stationPhi(nullptr),
    m_TGC_isEndcap(nullptr),
    m_TGC_Sim_stationEta(nullptr),
    m_TGC_Sim_stationPhi(nullptr),   
    m_TGC_Sim_gasGap(nullptr),
    m_TGC_gasGap(nullptr),
    m_TGC_isStrip(nullptr),
    m_TGC_measuresPhi(nullptr),
    m_TGC_channel(nullptr),   
    m_TGC_globalTime(nullptr),
    m_TGC_hitLocalPositionX(nullptr),
    m_TGC_hitLocalPositionY(nullptr),
    m_TGC_hitLocalPositionZ(nullptr),
    m_TGC_hitGlobalPositionX(nullptr),
    m_TGC_hitGlobalPositionY(nullptr),
    m_TGC_hitGlobalPositionZ(nullptr),
    m_TGC_hitGlobalPositionR(nullptr),
    m_TGC_hitGlobalPositionP(nullptr),
    m_TGC_detector_globalPositionX(nullptr),
    m_TGC_detector_globalPositionY(nullptr),
    m_TGC_detector_globalPositionZ(nullptr),
    m_TGC_detector_globalPositionR(nullptr),
    m_TGC_detector_globalPositionP(nullptr),
    m_TGC_particleEncoding(nullptr),
    m_TGC_kineticEnergy(nullptr),
    m_TGC_depositEnergy(nullptr),
    m_TGC_StepLength(nullptr),
    m_TGC_trackId(nullptr),
    m_TGC_truthEl(nullptr),
    m_TGC_nSimHits(0)
  {
    setHelper(idhelper);
  }

  ~TGCSimHitVariables()
  {
    this->deleteVariables();
  }

  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_TgcIdHelper = dynamic_cast<const TgcIdHelper*>(idhelper);
    if(m_TgcIdHelper == 0) {
       ATH_MSG_ERROR("casting IdHelper to TgcIdhelper failed");
       throw;
    }
  }

  void deleteVariables();
  StatusCode clearVariables();

  const TgcIdHelper* m_TgcIdHelper;

  std::vector<std::string>  *m_TGC_Sim_stationName;
  std::vector<int>  *m_TGC_stationName;
  std::vector<int>  *m_TGC_stationEta;
  std::vector<int>  *m_TGC_stationPhi;
  std::vector<bool> *m_TGC_isEndcap;
  std::vector<int>  *m_TGC_Sim_stationEta;
  std::vector<int>  *m_TGC_Sim_stationPhi;
  std::vector<int>  *m_TGC_Sim_gasGap;
  std::vector<int>  *m_TGC_gasGap;
  std::vector<int>  *m_TGC_isStrip;
  std::vector<int>  *m_TGC_measuresPhi;
  std::vector<int>  *m_TGC_channel;
  std::vector<double>  *m_TGC_globalTime;
  std::vector<double>  *m_TGC_hitLocalPositionX;
  std::vector<double>  *m_TGC_hitLocalPositionY;
  std::vector<double>  *m_TGC_hitLocalPositionZ;
  std::vector<double>  *m_TGC_hitGlobalPositionX;
  std::vector<double>  *m_TGC_hitGlobalPositionY;
  std::vector<double>  *m_TGC_hitGlobalPositionZ;
  std::vector<double>  *m_TGC_hitGlobalPositionR;
  std::vector<double>  *m_TGC_hitGlobalPositionP;
  std::vector<double>  *m_TGC_detector_globalPositionX;
  std::vector<double>  *m_TGC_detector_globalPositionY;
  std::vector<double>  *m_TGC_detector_globalPositionZ;
  std::vector<double>  *m_TGC_detector_globalPositionR;
  std::vector<double>  *m_TGC_detector_globalPositionP;
  std::vector<int>  *m_TGC_particleEncoding;
  std::vector<double>  *m_TGC_kineticEnergy;
  std::vector<double>  *m_TGC_depositEnergy;
  std::vector<double>  *m_TGC_StepLength;
  std::vector<int>  *m_TGC_trackId;
  std::vector<int>  *m_TGC_truthEl;
  int m_TGC_nSimHits;

};

#endif // TGCSimHitVariables_H

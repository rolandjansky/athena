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
                     const std::string & containername,
              MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl),
    m_TgcIdHelper(nullptr),
    m_TGC_Sim_stationName(),
    m_TGC_stationName(),
    m_TGC_stationEta(),
    m_TGC_stationPhi(),
    m_TGC_isEndcap(),  
    m_TGC_Sim_gasGap(),
    m_TGC_gasGap(),
    m_TGC_isStrip(),
    m_TGC_measuresPhi(),
    m_TGC_channel(),   
    m_TGC_globalTime(),
    m_TGC_hitLocalPositionX(),
    m_TGC_hitLocalPositionY(),
    m_TGC_hitLocalPositionZ(),
    m_TGC_hitGlobalPositionX(),
    m_TGC_hitGlobalPositionY(),
    m_TGC_hitGlobalPositionZ(),
    m_TGC_hitGlobalPositionR(),
    m_TGC_hitGlobalPositionP(),
    m_TGC_detector_globalPositionX(),
    m_TGC_detector_globalPositionY(),
    m_TGC_detector_globalPositionZ(),
    m_TGC_detector_globalPositionR(),
    m_TGC_detector_globalPositionP(),
    m_TGC_particleEncoding(),
    m_TGC_kineticEnergy(),
    m_TGC_depositEnergy(),
    m_TGC_StepLength(),
    m_TGC_trackId(),
    m_TGC_truthEl(),
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
    if(!m_TgcIdHelper) {
       ATH_MSG_ERROR("casting IdHelper to TgcIdhelper failed");
       throw std::runtime_error("Casting error in TGCSimHitVariables::setHelper");
    }
  }

  void deleteVariables();
  StatusCode clearVariables();

  const TgcIdHelper* m_TgcIdHelper{};

  std::vector<std::string> m_TGC_Sim_stationName;
  std::vector<int>  m_TGC_stationName;
  std::vector<int>  m_TGC_stationEta;
  std::vector<int>  m_TGC_stationPhi;
  std::vector<bool> m_TGC_isEndcap;
  std::vector<int>  m_TGC_Sim_gasGap;
  std::vector<int>  m_TGC_gasGap;
  std::vector<int>  m_TGC_isStrip;
  std::vector<int>  m_TGC_measuresPhi;
  std::vector<int>  m_TGC_channel;
  std::vector<double> m_TGC_globalTime;
  std::vector<double> m_TGC_hitLocalPositionX;
  std::vector<double> m_TGC_hitLocalPositionY;
  std::vector<double> m_TGC_hitLocalPositionZ;
  std::vector<double> m_TGC_hitGlobalPositionX;
  std::vector<double> m_TGC_hitGlobalPositionY;
  std::vector<double> m_TGC_hitGlobalPositionZ;
  std::vector<double> m_TGC_hitGlobalPositionR;
  std::vector<double> m_TGC_hitGlobalPositionP;
  std::vector<double> m_TGC_detector_globalPositionX;
  std::vector<double> m_TGC_detector_globalPositionY;
  std::vector<double> m_TGC_detector_globalPositionZ;
  std::vector<double> m_TGC_detector_globalPositionR;
  std::vector<double> m_TGC_detector_globalPositionP;
  std::vector<int> m_TGC_particleEncoding;
  std::vector<double> m_TGC_kineticEnergy;
  std::vector<double> m_TGC_depositEnergy;
  std::vector<double> m_TGC_StepLength;
  std::vector<int> m_TGC_trackId;
  std::vector<int> m_TGC_truthEl;
  int m_TGC_nSimHits{};

};

#endif // TGCSimHitVariables_H

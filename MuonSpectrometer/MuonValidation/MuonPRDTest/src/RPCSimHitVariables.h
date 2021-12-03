/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCSimHitVariables_H
#define RPCSimHitVariables_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <vector>

class RPCSimHitVariables : public ValAlgVariables
{
 public:
  RPCSimHitVariables(StoreGateSvc* evtStore,
                     const MuonGM::MuonDetectorManager* detManager,
                     const MuonIdHelper* idhelper,
                     TTree* tree,
                     const std::string & containername,
              MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl),
    m_RpcIdHelper(nullptr),
    m_RPC_Sim_stationName(),
    m_RPC_stationName(),
    m_RPC_stationEta(),
    m_RPC_stationPhi(),
    m_RPC_isEndcap(),
    m_RPC_strip(),
    m_RPC_Sim_stationEta(),
    m_RPC_Sim_stationPhi(),
    m_RPC_Sim_doubletR(),
    m_RPC_Sim_doubletZ(),
    m_RPC_Sim_doubletPhi(),
    m_RPC_Sim_gasGapLayer(),
    m_RPC_Sim_measuresPhi(),
    m_RPC_globalTime(),
    m_RPC_hitLocalPositionX(),
    m_RPC_hitLocalPositionY(),
    m_RPC_hitLocalPositionZ(),
    m_RPC_hitGlobalPositionX(),
    m_RPC_hitGlobalPositionY(),
    m_RPC_hitGlobalPositionZ(),
    m_RPC_hitGlobalPositionR(),
    m_RPC_hitGlobalPositionP(),
    m_RPC_detector_globalPositionX(),
    m_RPC_detector_globalPositionY(),
    m_RPC_detector_globalPositionZ(),
    m_RPC_detector_globalPositionR(),
    m_RPC_detector_globalPositionP(),
    m_RPC_particleEncoding(),
    m_RPC_kineticEnergy(),
    m_RPC_depositEnergy(),
    m_RPC_StepLength(),
    m_RPC_trackId(),
    m_RPC_truthEl(),
    m_RPC_nSimHits(0)
  {
    setHelper(idhelper);
  }

  ~RPCSimHitVariables()
  {
    this->deleteVariables();
  }

  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_RpcIdHelper = dynamic_cast<const RpcIdHelper*>(idhelper);
    if(!m_RpcIdHelper) {
       ATH_MSG_ERROR("casting IdHelper to RpcIdhelper failed");
       throw std::runtime_error("Casting error in RPCSimHitVariables::setHelper");
    }
  }

  void deleteVariables();
  StatusCode clearVariables();

  const RpcIdHelper* m_RpcIdHelper{};

  std::vector<std::string> m_RPC_Sim_stationName;
  std::vector<int>  m_RPC_stationName;
  std::vector<int>  m_RPC_stationEta;
  std::vector<int>  m_RPC_stationPhi;
  std::vector<bool> m_RPC_isEndcap;
  std::vector<int>  m_RPC_strip;
  std::vector<int>  m_RPC_Sim_stationEta;
  std::vector<int>  m_RPC_Sim_stationPhi;
  std::vector<int>  m_RPC_Sim_doubletR;
  std::vector<int>  m_RPC_Sim_doubletZ;
  std::vector<int>  m_RPC_Sim_doubletPhi;
  std::vector<int>  m_RPC_Sim_gasGapLayer;
  std::vector<int>  m_RPC_Sim_measuresPhi;
  std::vector<double> m_RPC_globalTime;
  std::vector<double> m_RPC_hitLocalPositionX;
  std::vector<double> m_RPC_hitLocalPositionY;
  std::vector<double> m_RPC_hitLocalPositionZ;
  std::vector<double> m_RPC_hitGlobalPositionX;
  std::vector<double> m_RPC_hitGlobalPositionY;
  std::vector<double> m_RPC_hitGlobalPositionZ;
  std::vector<double> m_RPC_hitGlobalPositionR;
  std::vector<double> m_RPC_hitGlobalPositionP;
  std::vector<double> m_RPC_detector_globalPositionX;
  std::vector<double> m_RPC_detector_globalPositionY;
  std::vector<double> m_RPC_detector_globalPositionZ;
  std::vector<double> m_RPC_detector_globalPositionR;
  std::vector<double> m_RPC_detector_globalPositionP;

  std::vector<int> m_RPC_particleEncoding;
  std::vector<double> m_RPC_kineticEnergy;
  std::vector<double> m_RPC_depositEnergy;
  std::vector<double> m_RPC_StepLength;
  std::vector<int> m_RPC_trackId;
  std::vector<int> m_RPC_truthEl;
  int m_RPC_nSimHits{};

};

#endif // RPCSimHitVariables_H

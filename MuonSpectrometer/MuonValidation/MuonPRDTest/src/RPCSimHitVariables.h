/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
                     std::string containername,
              MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl),
    m_RPC_Sim_stationName(nullptr),
    m_RPC_stationName(nullptr),
    m_RPC_stationEta(nullptr),
    m_RPC_stationPhi(nullptr),
    m_RPC_isEndcap(nullptr),
    m_RPC_strip(nullptr),
    m_RPC_Sim_stationEta(nullptr),
    m_RPC_Sim_stationPhi(nullptr),
    m_RPC_Sim_doubletR(nullptr),
    m_RPC_Sim_doubletZ(nullptr),
    m_RPC_Sim_doubletPhi(nullptr),
    m_RPC_Sim_gasGapLayer(nullptr),
    m_RPC_Sim_measuresPhi(nullptr),
    m_RPC_globalTime(nullptr),
    m_RPC_hitLocalPositionX(nullptr),
    m_RPC_hitLocalPositionY(nullptr),
    m_RPC_hitLocalPositionZ(nullptr),
    m_RPC_hitGlobalPositionX(nullptr),
    m_RPC_hitGlobalPositionY(nullptr),
    m_RPC_hitGlobalPositionZ(nullptr),
    m_RPC_hitGlobalPositionR(nullptr),
    m_RPC_hitGlobalPositionP(nullptr),
    m_RPC_detector_globalPositionX(nullptr),
    m_RPC_detector_globalPositionY(nullptr),
    m_RPC_detector_globalPositionZ(nullptr),
    m_RPC_detector_globalPositionR(nullptr),
    m_RPC_detector_globalPositionP(nullptr),
    m_RPC_particleEncoding(nullptr),
    m_RPC_kineticEnergy(nullptr),
    m_RPC_depositEnergy(nullptr),
    m_RPC_StepLength(nullptr),
    m_RPC_trackId(nullptr),
    m_RPC_truthEl(nullptr),
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
    if(m_RpcIdHelper == 0) {
       ATH_MSG_ERROR("casting IdHelper to RpcIdhelper failed");
       throw;
    }
  }

  void deleteVariables();
  StatusCode clearVariables();

  const RpcIdHelper* m_RpcIdHelper;

  std::vector<std::string>  *m_RPC_Sim_stationName;
  std::vector<int>  *m_RPC_stationName;
  std::vector<int>  *m_RPC_stationEta;
  std::vector<int>  *m_RPC_stationPhi;
  std::vector<bool> *m_RPC_isEndcap;
  std::vector<int>  *m_RPC_strip;
  std::vector<int>  *m_RPC_Sim_stationEta;
  std::vector<int>  *m_RPC_Sim_stationPhi;
  std::vector<int>  *m_RPC_Sim_doubletR;
  std::vector<int>  *m_RPC_Sim_doubletZ;
  std::vector<int>  *m_RPC_Sim_doubletPhi;
  std::vector<int>  *m_RPC_Sim_gasGapLayer;
  std::vector<int>  *m_RPC_Sim_measuresPhi;
  std::vector<double>  *m_RPC_globalTime;
  std::vector<double>  *m_RPC_hitLocalPositionX;
  std::vector<double>  *m_RPC_hitLocalPositionY;
  std::vector<double>  *m_RPC_hitLocalPositionZ;
  std::vector<double>  *m_RPC_hitGlobalPositionX;
  std::vector<double>  *m_RPC_hitGlobalPositionY;
  std::vector<double>  *m_RPC_hitGlobalPositionZ;
  std::vector<double>  *m_RPC_hitGlobalPositionR;
  std::vector<double>  *m_RPC_hitGlobalPositionP;
  std::vector<double>  *m_RPC_detector_globalPositionX;
  std::vector<double>  *m_RPC_detector_globalPositionY;
  std::vector<double>  *m_RPC_detector_globalPositionZ;
  std::vector<double>  *m_RPC_detector_globalPositionR;
  std::vector<double>  *m_RPC_detector_globalPositionP;

  std::vector<int>  *m_RPC_particleEncoding;
  std::vector<double>  *m_RPC_kineticEnergy;
  std::vector<double>  *m_RPC_depositEnergy;
  std::vector<double>  *m_RPC_StepLength;
  std::vector<int>  *m_RPC_trackId;
  std::vector<int>  *m_RPC_truthEl;
  int m_RPC_nSimHits;

};

#endif // RPCSimHitVariables_H

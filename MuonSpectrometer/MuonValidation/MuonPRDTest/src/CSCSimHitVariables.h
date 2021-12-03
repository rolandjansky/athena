/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSimHitVariables_H
#define CSCSimHitVariables_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <vector>

class CSCSimHitVariables : public ValAlgVariables
{
 public:
  CSCSimHitVariables(StoreGateSvc* evtStore,
                     const MuonGM::MuonDetectorManager* detManager,
                     const MuonIdHelper* idhelper,
                     TTree* tree,
                     const std::string & containername,
              MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl),
    m_CscIdHelper(nullptr),
    m_CSC_Sim_stationName(),
    m_CSC_stationName(),
    m_CSC_stationEta(),
    m_CSC_stationPhi(),
    m_CSC_isEndcap(),
    m_CSC_strip(),
    m_CSC_Sim_stationEta(),
    m_CSC_Sim_stationPhi(),
    m_CSC_Sim_chamberLayer(),
    m_CSC_Sim_wireLayer(),
    m_CSC_globalTime(),
    m_CSC_hitGlobalPositionX(),
    m_CSC_hitGlobalPositionY(),
    m_CSC_hitGlobalPositionZ(),
    m_CSC_hitGlobalPositionR(),
    m_CSC_hitGlobalPositionP(),
    m_CSC_detector_globalPositionX(),
    m_CSC_detector_globalPositionY(),
    m_CSC_detector_globalPositionZ(),
    m_CSC_detector_globalPositionR(),
    m_CSC_detector_globalPositionP(),
    m_CSC_kineticEnergy(),
    m_CSC_depositEnergy(),
    m_CSC_trackId(),
    m_CSC_truthEl(),
    m_CSC_nSimHits(0)
    
  {
    setHelper(idhelper);
  }

  ~CSCSimHitVariables()
  {
    this->deleteVariables();
  }

  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_CscIdHelper = dynamic_cast<const CscIdHelper*>(idhelper);
    if(!m_CscIdHelper) {
       ATH_MSG_ERROR("casting IdHelper to CscIdhelper failed");
       throw std::runtime_error("Bad cast in CSCSimHitVariables::setHelper");
    }
  }

  void deleteVariables();
  StatusCode clearVariables();

  const CscIdHelper* m_CscIdHelper{};
  
	std::vector<std::string> m_CSC_Sim_stationName;
	std::vector<int> m_CSC_stationName;
	std::vector<int> m_CSC_stationEta;
	std::vector<int> m_CSC_stationPhi;
	std::vector<bool> m_CSC_isEndcap;
	std::vector<int> m_CSC_strip;
	std::vector<int> m_CSC_Sim_stationEta;
	std::vector<int> m_CSC_Sim_stationPhi;
	std::vector<int> m_CSC_Sim_chamberLayer;
	std::vector<int> m_CSC_Sim_wireLayer;
	std::vector<double> m_CSC_globalTime;
	std::vector<double> m_CSC_hitGlobalPositionX;
	std::vector<double> m_CSC_hitGlobalPositionY;
	std::vector<double> m_CSC_hitGlobalPositionZ;
	std::vector<double> m_CSC_hitGlobalPositionR;
	std::vector<double> m_CSC_hitGlobalPositionP;
	std::vector<double> m_CSC_detector_globalPositionX;
	std::vector<double> m_CSC_detector_globalPositionY;
	std::vector<double> m_CSC_detector_globalPositionZ;
	std::vector<double> m_CSC_detector_globalPositionR;
	std::vector<double> m_CSC_detector_globalPositionP;
	std::vector<double> m_CSC_kineticEnergy;
	std::vector<double> m_CSC_depositEnergy;
	std::vector<int> m_CSC_trackId;
	std::vector<int> m_CSC_truthEl;
  int m_CSC_nSimHits{};

};

#endif // CscSimHitVariables_H

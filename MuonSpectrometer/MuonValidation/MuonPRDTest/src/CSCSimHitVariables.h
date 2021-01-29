/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSimHitVariables_H
#define CSCSimHitVariables_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <vector>
#include <memory>

class CSCSimHitVariables : public ValAlgVariables
{
 public:
  CSCSimHitVariables(StoreGateSvc* evtStore,
                     const MuonGM::MuonDetectorManager* detManager,
                     const MuonIdHelper* idhelper,
                     TTree* tree,
                     std::string containername,
              MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl),
    m_CSC_Sim_stationName(nullptr),
    m_CSC_stationName(nullptr),
    m_CSC_stationEta(nullptr),
    m_CSC_stationPhi(nullptr),
    m_CSC_isEndcap(nullptr),
    m_CSC_strip(nullptr),
    m_CSC_Sim_stationEta(nullptr),
    m_CSC_Sim_stationPhi(nullptr),
    m_CSC_Sim_chamberLayer(nullptr),
    m_CSC_Sim_wireLayer(nullptr),
    m_CSC_globalTime(nullptr),
    m_CSC_hitGlobalPositionX(nullptr),
    m_CSC_hitGlobalPositionY(nullptr),
    m_CSC_hitGlobalPositionZ(nullptr),
    m_CSC_hitGlobalPositionR(nullptr),
    m_CSC_hitGlobalPositionP(nullptr),
    m_CSC_detector_globalPositionX(nullptr),
    m_CSC_detector_globalPositionY(nullptr),
    m_CSC_detector_globalPositionZ(nullptr),
    m_CSC_detector_globalPositionR(nullptr),
    m_CSC_detector_globalPositionP(nullptr),
    m_CSC_kineticEnergy(nullptr),
    m_CSC_depositEnergy(nullptr),
    m_CSC_trackId(nullptr),
    m_CSC_truthEl(nullptr),
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
    if(m_CscIdHelper == 0) {
       ATH_MSG_ERROR("casting IdHelper to CscIdhelper failed");
       throw;
    }
  }

  void deleteVariables();
  StatusCode clearVariables();

  const CscIdHelper* m_CscIdHelper;
  
	std::unique_ptr<std::vector<std::string>> m_CSC_Sim_stationName;
	std::unique_ptr<std::vector<int>> m_CSC_stationName;
	std::unique_ptr<std::vector<int>> m_CSC_stationEta;
	std::unique_ptr<std::vector<int>> m_CSC_stationPhi;
	std::unique_ptr<std::vector<bool>> m_CSC_isEndcap;
	std::unique_ptr<std::vector<int>> m_CSC_strip;
	std::unique_ptr<std::vector<int>> m_CSC_Sim_stationEta;
	std::unique_ptr<std::vector<int>> m_CSC_Sim_stationPhi;
	std::unique_ptr<std::vector<int>> m_CSC_Sim_chamberLayer;
	std::unique_ptr<std::vector<int>> m_CSC_Sim_wireLayer;
	std::unique_ptr<std::vector<double>> m_CSC_globalTime;
	std::unique_ptr<std::vector<double>> m_CSC_hitGlobalPositionX;
	std::unique_ptr<std::vector<double>> m_CSC_hitGlobalPositionY;
	std::unique_ptr<std::vector<double>> m_CSC_hitGlobalPositionZ;
	std::unique_ptr<std::vector<double>> m_CSC_hitGlobalPositionR;
	std::unique_ptr<std::vector<double>> m_CSC_hitGlobalPositionP;
	std::unique_ptr<std::vector<double>> m_CSC_detector_globalPositionX;
	std::unique_ptr<std::vector<double>> m_CSC_detector_globalPositionY;
	std::unique_ptr<std::vector<double>> m_CSC_detector_globalPositionZ;
	std::unique_ptr<std::vector<double>> m_CSC_detector_globalPositionR;
	std::unique_ptr<std::vector<double>> m_CSC_detector_globalPositionP;
	std::unique_ptr<std::vector<double>> m_CSC_kineticEnergy;
	std::unique_ptr<std::vector<double>> m_CSC_depositEnergy;
	std::unique_ptr<std::vector<int>> m_CSC_trackId;
	std::unique_ptr<std::vector<int>> m_CSC_truthEl;
  int m_CSC_nSimHits;

};

#endif // CscSimHitVariables_H

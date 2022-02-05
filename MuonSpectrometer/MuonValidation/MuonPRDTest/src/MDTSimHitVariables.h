/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTSimHitVariables_H
#define MDTSimHitVariables_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <vector>

class MDTSimHitVariables : public ValAlgVariables
{
 public:
  MDTSimHitVariables(StoreGateSvc* evtStore,
                     const MuonGM::MuonDetectorManager* detManager,
                     const MuonIdHelper* idhelper,
                     TTree* tree,
                     const std::string & containername,
              MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl),
    m_MDT_Sim_stationName(),
    m_MDT_stationName(),
    m_MDT_stationEta(),
    m_MDT_stationPhi(),
    m_MDT_isEndcap(),
    m_MDT_Sim_stationEta(),
    m_MDT_Sim_stationPhi(),
    m_MDT_Sim_multilayer(),
    m_MDT_Sim_tubelayer(),
    m_MDT_Sim_tube(),
    m_MDT_globalTime(),
    m_MDT_hitLocalPositionX(),
    m_MDT_hitLocalPositionY(),
    m_MDT_hitLocalPositionZ(),
    m_MDT_hitGlobalPositionX(),
    m_MDT_hitGlobalPositionY(),
    m_MDT_hitGlobalPositionZ(),
    m_MDT_hitGlobalPositionR(),
    m_MDT_hitGlobalPositionP(),
    m_MDT_detector_globalPositionX(),
    m_MDT_detector_globalPositionY(),
    m_MDT_detector_globalPositionZ(),
    m_MDT_detector_globalPositionR(),
    m_MDT_detector_globalPositionP(),
    m_MDT_driftRadius(),
    m_MDT_particleEncoding(),
    m_MDT_kineticEnergy(),
    m_MDT_depositEnergy(),
    m_MDT_StepLength(),
    m_MDT_trackId(),
    m_MDT_truthEl(),
    m_MDT_nSimHits(0)
  {
    setHelper(idhelper);
  }

  ~MDTSimHitVariables()
  {
    this->deleteVariables();
  }

  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_MdtIdHelper = dynamic_cast<const MdtIdHelper*>(idhelper);
    if(!m_MdtIdHelper) {
       ATH_MSG_ERROR("casting IdHelper to MdtIdhelper failed");
       throw std::runtime_error("Bad cast in MDTSimHitVariables::setHelper");
    }
  }

  void deleteVariables();
  StatusCode clearVariables();

  const MdtIdHelper* m_MdtIdHelper{};

  std::vector<std::string> m_MDT_Sim_stationName;
  std::vector<int>  m_MDT_stationName;
  std::vector<int>  m_MDT_stationEta;
  std::vector<int>  m_MDT_stationPhi;
  std::vector<bool> m_MDT_isEndcap;
  std::vector<int>  m_MDT_Sim_stationEta;
  std::vector<int>  m_MDT_Sim_stationPhi;
  std::vector<int>  m_MDT_Sim_multilayer;
  std::vector<int>  m_MDT_Sim_tubelayer;
  std::vector<int>  m_MDT_Sim_tube;
  std::vector<double> m_MDT_globalTime;
  std::vector<double> m_MDT_hitLocalPositionX;
  std::vector<double> m_MDT_hitLocalPositionY;
  std::vector<double> m_MDT_hitLocalPositionZ;
  std::vector<double> m_MDT_hitGlobalPositionX;
  std::vector<double> m_MDT_hitGlobalPositionY;
  std::vector<double> m_MDT_hitGlobalPositionZ;
  std::vector<double> m_MDT_hitGlobalPositionR;
  std::vector<double> m_MDT_hitGlobalPositionP;
  std::vector<double> m_MDT_detector_globalPositionX;
  std::vector<double> m_MDT_detector_globalPositionY;
  std::vector<double> m_MDT_detector_globalPositionZ;
  std::vector<double> m_MDT_detector_globalPositionR;
  std::vector<double> m_MDT_detector_globalPositionP;
  std::vector<double> m_MDT_driftRadius;

  std::vector<int> m_MDT_particleEncoding;
  std::vector<double> m_MDT_kineticEnergy;
  std::vector<double> m_MDT_depositEnergy;
  std::vector<double> m_MDT_StepLength;
  std::vector<int> m_MDT_trackId;
  std::vector<int> m_MDT_truthEl;
  int m_MDT_nSimHits{};

};

#endif // MDTSimHitVariables_H

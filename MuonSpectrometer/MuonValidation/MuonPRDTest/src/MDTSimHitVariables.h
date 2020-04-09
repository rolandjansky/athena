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
                     std::string containername,
              MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl),
    m_MDT_Sim_stationName(nullptr),
    m_MDT_stationName(nullptr),
    m_MDT_stationEta(nullptr),
    m_MDT_stationPhi(nullptr),
    m_MDT_isEndcap(nullptr),
    m_MDT_Sim_stationEta(nullptr),
    m_MDT_Sim_stationPhi(nullptr),
    m_MDT_Sim_multilayer(nullptr),
    m_MDT_Sim_tubelayer(nullptr),
    m_MDT_Sim_tube(nullptr),
    m_MDT_globalTime(nullptr),
    m_MDT_hitLocalPositionX(nullptr),
    m_MDT_hitLocalPositionY(nullptr),
    m_MDT_hitLocalPositionZ(nullptr),
    m_MDT_hitGlobalPositionX(nullptr),
    m_MDT_hitGlobalPositionY(nullptr),
    m_MDT_hitGlobalPositionZ(nullptr),
    m_MDT_hitGlobalPositionR(nullptr),
    m_MDT_hitGlobalPositionP(nullptr),
    m_MDT_detector_globalPositionX(nullptr),
    m_MDT_detector_globalPositionY(nullptr),
    m_MDT_detector_globalPositionZ(nullptr),
    m_MDT_detector_globalPositionR(nullptr),
    m_MDT_detector_globalPositionP(nullptr),
    m_MDT_driftRadius(nullptr),
    m_MDT_particleEncoding(nullptr),
    m_MDT_kineticEnergy(nullptr),
    m_MDT_depositEnergy(nullptr),
    m_MDT_StepLength(nullptr),
    m_MDT_trackId(nullptr),
    m_MDT_truthEl(nullptr),
    m_MDT_nSimHits(0)
  {
    setHelper(idhelper);
  }

  ~MDTSimHitVariables()
  {
    this->deleteVariables();
  }

  StatusCode initializeVariables();
  StatusCode fillVariables();

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_MdtIdHelper = dynamic_cast<const MdtIdHelper*>(idhelper);
    if(m_MdtIdHelper == 0) {
       ATH_MSG_ERROR("casting IdHelper to MdtIdhelper failed");
       throw;
    }
  }

  void deleteVariables();
  StatusCode clearVariables();

  const MdtIdHelper* m_MdtIdHelper;

  std::vector<std::string>  *m_MDT_Sim_stationName;
  std::vector<int>  *m_MDT_stationName;
  std::vector<int>  *m_MDT_stationEta;
  std::vector<int>  *m_MDT_stationPhi;
  std::vector<bool> *m_MDT_isEndcap;
  std::vector<int>  *m_MDT_Sim_stationEta;
  std::vector<int>  *m_MDT_Sim_stationPhi;
  std::vector<int>  *m_MDT_Sim_multilayer;
  std::vector<int>  *m_MDT_Sim_tubelayer;
  std::vector<int>  *m_MDT_Sim_tube;
  std::vector<double>  *m_MDT_globalTime;
  std::vector<double>  *m_MDT_hitLocalPositionX;
  std::vector<double>  *m_MDT_hitLocalPositionY;
  std::vector<double>  *m_MDT_hitLocalPositionZ;
  std::vector<double>  *m_MDT_hitGlobalPositionX;
  std::vector<double>  *m_MDT_hitGlobalPositionY;
  std::vector<double>  *m_MDT_hitGlobalPositionZ;
  std::vector<double>  *m_MDT_hitGlobalPositionR;
  std::vector<double>  *m_MDT_hitGlobalPositionP;
  std::vector<double>  *m_MDT_detector_globalPositionX;
  std::vector<double>  *m_MDT_detector_globalPositionY;
  std::vector<double>  *m_MDT_detector_globalPositionZ;
  std::vector<double>  *m_MDT_detector_globalPositionR;
  std::vector<double>  *m_MDT_detector_globalPositionP;
  std::vector<double>  *m_MDT_driftRadius;

  std::vector<int>  *m_MDT_particleEncoding;
  std::vector<double>  *m_MDT_kineticEnergy;
  std::vector<double>  *m_MDT_depositEnergy;
  std::vector<double>  *m_MDT_StepLength;
  std::vector<int>  *m_MDT_trackId;
  std::vector<int>  *m_MDT_truthEl;
  int m_MDT_nSimHits;

};

#endif // MDTSimHitVariables_H

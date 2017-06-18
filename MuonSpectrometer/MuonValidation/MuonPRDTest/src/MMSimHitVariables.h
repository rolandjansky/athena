/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMSIMHITVARIABLES_H
#define MMSIMHITVARIABLES_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <vector>

class MMSimHitVariables : public ValAlgVariables
{
 public:
  MMSimHitVariables(StoreGateSvc* evtStore,
                    const MuonGM::MuonDetectorManager* detManager,
                    const MuonIdHelper* idhelper,
                    TTree* tree,
                    std::string containername) :
    ValAlgVariables(evtStore, detManager, tree, containername, "MMSimHitVariables"),
    m_MmIdHelper(0),
    m_NSWMM_nSimHits(0), 
    m_NSWMM_trackId(0),
    m_NSWMM_truthEl(0),
    m_NSWMM_globalTime(0), 
    m_NSWMM_hitGlobalPositionX(0), 
    m_NSWMM_hitGlobalPositionY(0), 
    m_NSWMM_hitGlobalPositionZ(0), 
    m_NSWMM_hitGlobalPositionR(0),
    m_NSWMM_hitGlobalPositionP(0),
    m_NSWMM_hitGlobalDirectionX(0), 
    m_NSWMM_hitGlobalDirectionY(0), 
    m_NSWMM_hitGlobalDirectionZ(0), 
    m_NSWMM_hitLocalPositionX(0), 
    m_NSWMM_hitLocalPositionY(0), 
    m_NSWMM_hitLocalPositionZ(0), 
    m_NSWMM_detector_globalPositionX(0),
    m_NSWMM_detector_globalPositionY(0),
    m_NSWMM_detector_globalPositionZ(0),
    m_NSWMM_detector_globalPositionR(0),
    m_NSWMM_detector_globalPositionP(0),
    m_NSWMM_hitToDsurfacePositionX(0),
    m_NSWMM_hitToDsurfacePositionY(0),
    m_NSWMM_hitToDsurfacePositionZ(0),
    m_NSWMM_hitToRsurfacePositionX(0),
    m_NSWMM_hitToRsurfacePositionY(0),
    m_NSWMM_hitToRsurfacePositionZ(0),
    m_NSWMM_FastDigitRsurfacePositionX(0),
    m_NSWMM_FastDigitRsurfacePositionY(0),
    m_NSWMM_particleEncoding(0), 
    m_NSWMM_kineticEnergy(0), 
    m_NSWMM_depositEnergy(0), 
    m_NSWMM_StepLength(0),
    m_NSWMM_sim_stationName(0),
    m_NSWMM_sim_stationEta(0),
    m_NSWMM_sim_stationPhi(0),
    m_NSWMM_sim_multilayer(0),
    m_NSWMM_sim_layer(0),
    m_NSWMM_sim_side(0),
    m_NSWMM_off_stationName(0),
    m_NSWMM_off_stationEta(0),
    m_NSWMM_off_stationPhi(0),
    m_NSWMM_off_multiplet(0),
    m_NSWMM_off_gas_gap(0),
    m_NSWMM_off_channel(0)
  {
    setHelper(idhelper);
  }

  ~MMSimHitVariables()
  {
    if( (this->deleteVariables()).isFailure() ){
      ATH_MSG_ERROR("Failing to delete MMSimHitVariables.");
      std::terminate();
    }
  }

  StatusCode initializeVariables();
  StatusCode fillVariables();

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_MmIdHelper = dynamic_cast<const MmIdHelper*>(idhelper);
    if(m_MmIdHelper == 0) {
       ATH_MSG_ERROR("casting IdHelper to MmIdhelper failed");
       throw;
    }
  }

  StatusCode deleteVariables();
  StatusCode clearVariables();

  const MmIdHelper* m_MmIdHelper;

  int m_NSWMM_nSimHits;
  std::vector<int>    *m_NSWMM_trackId;
  std::vector<int>    *m_NSWMM_truthEl;
  std::vector<double> *m_NSWMM_globalTime;
  std::vector<double> *m_NSWMM_hitGlobalPositionX;
  std::vector<double> *m_NSWMM_hitGlobalPositionY;
  std::vector<double> *m_NSWMM_hitGlobalPositionZ;
  std::vector<double> *m_NSWMM_hitGlobalPositionR;
  std::vector<double> *m_NSWMM_hitGlobalPositionP;
  std::vector<double> *m_NSWMM_hitGlobalDirectionX;
  std::vector<double> *m_NSWMM_hitGlobalDirectionY;
  std::vector<double> *m_NSWMM_hitGlobalDirectionZ;

  std::vector<double> *m_NSWMM_hitLocalPositionX;
  std::vector<double> *m_NSWMM_hitLocalPositionY;
  std::vector<double> *m_NSWMM_hitLocalPositionZ;

  std::vector<double> *m_NSWMM_detector_globalPositionX;
  std::vector<double> *m_NSWMM_detector_globalPositionY;
  std::vector<double> *m_NSWMM_detector_globalPositionZ;
  std::vector<double> *m_NSWMM_detector_globalPositionR;
  std::vector<double> *m_NSWMM_detector_globalPositionP;

  std::vector<double> *m_NSWMM_hitToDsurfacePositionX;
  std::vector<double> *m_NSWMM_hitToDsurfacePositionY;
  std::vector<double> *m_NSWMM_hitToDsurfacePositionZ;
  
  std::vector<double> *m_NSWMM_hitToRsurfacePositionX;
  std::vector<double> *m_NSWMM_hitToRsurfacePositionY;
  std::vector<double> *m_NSWMM_hitToRsurfacePositionZ;

  std::vector<double> *m_NSWMM_FastDigitRsurfacePositionX;
  std::vector<double> *m_NSWMM_FastDigitRsurfacePositionY;

  std::vector<int> *m_NSWMM_particleEncoding;
  std::vector<double> *m_NSWMM_kineticEnergy;
  std::vector<double> *m_NSWMM_depositEnergy;
  std::vector<double> *m_NSWMM_StepLength;

  std::vector<std::string> *m_NSWMM_sim_stationName;
  std::vector<int> *m_NSWMM_sim_stationEta;
  std::vector<int> *m_NSWMM_sim_stationPhi;
  std::vector<int> *m_NSWMM_sim_multilayer;
  std::vector<int> *m_NSWMM_sim_layer; 
  std::vector<int> *m_NSWMM_sim_side;

  std::vector<std::string> *m_NSWMM_off_stationName;
  std::vector<int> *m_NSWMM_off_stationEta;
  std::vector<int> *m_NSWMM_off_stationPhi;
  std::vector<int> *m_NSWMM_off_multiplet;
  std::vector<int> *m_NSWMM_off_gas_gap;
  std::vector<int> *m_NSWMM_off_channel;

};

#endif // MMSIMHITVARIABLES_H

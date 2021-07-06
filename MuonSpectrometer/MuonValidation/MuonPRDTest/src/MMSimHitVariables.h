/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
						  std::string containername,
						  MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl),
    m_MmIdHelper(nullptr),
    m_NSWMM_nSimHits(0), 
    m_NSWMM_trackId(),
    m_NSWMM_isInsideBounds(),
    m_NSWMM_globalTime(), 
    m_NSWMM_hitGlobalPositionX(), 
    m_NSWMM_hitGlobalPositionY(), 
    m_NSWMM_hitGlobalPositionZ(), 
    m_NSWMM_hitGlobalPositionR(),
    m_NSWMM_hitGlobalPositionP(),
    m_NSWMM_hitGlobalDirectionX(), 
    m_NSWMM_hitGlobalDirectionY(), 
    m_NSWMM_hitGlobalDirectionZ(), 
    m_NSWMM_detector_globalPositionX(),
    m_NSWMM_detector_globalPositionY(),
    m_NSWMM_detector_globalPositionZ(),
    m_NSWMM_detector_globalPositionR(),
    m_NSWMM_detector_globalPositionP(),
    m_NSWMM_hitToDsurfacePositionX(),
    m_NSWMM_hitToDsurfacePositionY(),
    m_NSWMM_hitToDsurfacePositionZ(),
    m_NSWMM_hitToRsurfacePositionX(),
    m_NSWMM_hitToRsurfacePositionY(),
    m_NSWMM_hitToRsurfacePositionZ(),
    m_NSWMM_FastDigitRsurfacePositionX(),
    m_NSWMM_FastDigitRsurfacePositionY(),
    m_NSWMM_particleEncoding(), 
    m_NSWMM_kineticEnergy(), 
    m_NSWMM_depositEnergy(), 
    m_NSWMM_sim_stationName(),
    m_NSWMM_sim_stationEta(),
    m_NSWMM_sim_stationPhi(),
    m_NSWMM_sim_multilayer(),
    m_NSWMM_sim_layer(),
    m_NSWMM_sim_side(),
    m_NSWMM_off_stationName(),
    m_NSWMM_off_stationEta(),
    m_NSWMM_off_stationPhi(),
    m_NSWMM_off_multiplet(),
    m_NSWMM_off_gas_gap(),
    m_NSWMM_off_channel()
  {
    setHelper(idhelper);
  }

  ~MMSimHitVariables()
  {
    deleteVariables();
  }

  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_MmIdHelper = dynamic_cast<const MmIdHelper*>(idhelper);
    if(!m_MmIdHelper) {
       ATH_MSG_ERROR("casting IdHelper to MmIdhelper failed");
       throw;
    }
  }

  void deleteVariables();
  StatusCode clearVariables();

  const MmIdHelper* m_MmIdHelper;

  int m_NSWMM_nSimHits;
  std::vector<int>    m_NSWMM_trackId;
  std::vector<bool>   m_NSWMM_isInsideBounds;
  std::vector<double> m_NSWMM_globalTime;
  std::vector<double> m_NSWMM_hitGlobalPositionX;
  std::vector<double> m_NSWMM_hitGlobalPositionY;
  std::vector<double> m_NSWMM_hitGlobalPositionZ;
  std::vector<double> m_NSWMM_hitGlobalPositionR;
  std::vector<double> m_NSWMM_hitGlobalPositionP;
  std::vector<double> m_NSWMM_hitGlobalDirectionX;
  std::vector<double> m_NSWMM_hitGlobalDirectionY;
  std::vector<double> m_NSWMM_hitGlobalDirectionZ;

  std::vector<double> m_NSWMM_detector_globalPositionX;
  std::vector<double> m_NSWMM_detector_globalPositionY;
  std::vector<double> m_NSWMM_detector_globalPositionZ;
  std::vector<double> m_NSWMM_detector_globalPositionR;
  std::vector<double> m_NSWMM_detector_globalPositionP;

  std::vector<double> m_NSWMM_hitToDsurfacePositionX;
  std::vector<double> m_NSWMM_hitToDsurfacePositionY;
  std::vector<double> m_NSWMM_hitToDsurfacePositionZ;
  
  std::vector<double> m_NSWMM_hitToRsurfacePositionX;
  std::vector<double> m_NSWMM_hitToRsurfacePositionY;
  std::vector<double> m_NSWMM_hitToRsurfacePositionZ;

  std::vector<double> m_NSWMM_FastDigitRsurfacePositionX;
  std::vector<double> m_NSWMM_FastDigitRsurfacePositionY;

  std::vector<int> m_NSWMM_particleEncoding;
  std::vector<double> m_NSWMM_kineticEnergy;
  std::vector<double> m_NSWMM_depositEnergy;

  std::vector<std::string> m_NSWMM_sim_stationName;
  std::vector<int> m_NSWMM_sim_stationEta;
  std::vector<int> m_NSWMM_sim_stationPhi;
  std::vector<int> m_NSWMM_sim_multilayer;
  std::vector<int> m_NSWMM_sim_layer; 
  std::vector<int> m_NSWMM_sim_side;

  std::vector<std::string> m_NSWMM_off_stationName;
  std::vector<int> m_NSWMM_off_stationEta;
  std::vector<int> m_NSWMM_off_stationPhi;
  std::vector<int> m_NSWMM_off_multiplet;
  std::vector<int> m_NSWMM_off_gas_gap;
  std::vector<int> m_NSWMM_off_channel;

};

#endif // MMSIMHITVARIABLES_H

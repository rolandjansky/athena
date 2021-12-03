/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGCSIMHITVARIABLES_H
#define STGCSIMHITVARIABLES_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <vector>

class sTGCSimHitVariables : public ValAlgVariables
{
 public:
  sTGCSimHitVariables(StoreGateSvc* evtStore,
                    const MuonGM::MuonDetectorManager* detManager,
                    const MuonIdHelper* idhelper,
                    TTree* tree,
						  const std::string & containername,
						  MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl),
    m_sTgcIdHelper(nullptr),
    m_NSWsTGC_nSimHits(0), 
    m_NSWsTGC_trackId(),
    m_NSWsTGC_isInsideBounds(),
    m_NSWsTGC_globalTime(), 
    m_NSWsTGC_hitGlobalPositionX(), 
    m_NSWsTGC_hitGlobalPositionY(), 
    m_NSWsTGC_hitGlobalPositionZ(), 
    m_NSWsTGC_hitGlobalPositionR(),
    m_NSWsTGC_hitGlobalPositionP(),
    m_NSWsTGC_hitGlobalDirectionX(), 
    m_NSWsTGC_hitGlobalDirectionY(), 
    m_NSWsTGC_hitGlobalDirectionZ(), 
    m_NSWsTGC_detector_globalPositionX(),
    m_NSWsTGC_detector_globalPositionY(),
    m_NSWsTGC_detector_globalPositionZ(),
    m_NSWsTGC_detector_globalPositionR(),
    m_NSWsTGC_detector_globalPositionP(),
    m_NSWsTGC_hitToDsurfacePositionX(),
    m_NSWsTGC_hitToDsurfacePositionY(),
    m_NSWsTGC_hitToDsurfacePositionZ(),
    m_NSWsTGC_hitToRsurfacePositionX(),
    m_NSWsTGC_hitToRsurfacePositionY(),
    m_NSWsTGC_hitToRsurfacePositionZ(),
    m_NSWsTGC_FastDigitRsurfacePositionX(),
    m_NSWsTGC_FastDigitRsurfacePositionY(),
    m_NSWsTGC_particleEncoding(), 
    m_NSWsTGC_depositEnergy(), 
    m_NSWsTGC_sim_stationName(),
    m_NSWsTGC_wedgeId(),
    m_NSWsTGC_wedgeType(),
    m_NSWsTGC_detectorNumber(),
    m_NSWsTGC_sim_stationEta(),
    m_NSWsTGC_sim_stationPhi(),
    m_NSWsTGC_sim_multilayer(),
    m_NSWsTGC_sim_layer(),
    m_NSWsTGC_sim_side(),
    m_NSWsTGC_stripNumber(),
    m_NSWsTGC_wireNumber(),
    m_NSWsTGC_off_stationName(),
    m_NSWsTGC_off_stationEta(),                                         
    m_NSWsTGC_off_stationPhi(),
    m_NSWsTGC_off_multiplet(),
    m_NSWsTGC_off_gas_gap(),
    m_NSWsTGC_off_channel_type(),
    m_NSWsTGC_off_channel()
  {
    setHelper(idhelper);
  }

  ~sTGCSimHitVariables()
  {
    this->deleteVariables();
  }


  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_sTgcIdHelper = dynamic_cast<const sTgcIdHelper*>(idhelper);
    if(!m_sTgcIdHelper) {
       ATH_MSG_ERROR("casting IdHelper to sTgcIdhelper failed");
       throw std::runtime_error("Casting error in sTGCSimHitVariables::setHelper");
    }
  }

  void deleteVariables();
  StatusCode clearVariables();

  const sTgcIdHelper* m_sTgcIdHelper{};

  int m_NSWsTGC_nSimHits{};
  std::vector<int> m_NSWsTGC_trackId;

  std::vector<bool> m_NSWsTGC_isInsideBounds;

  std::vector<double> m_NSWsTGC_globalTime;
  std::vector<double> m_NSWsTGC_hitGlobalPositionX;
  std::vector<double> m_NSWsTGC_hitGlobalPositionY;
  std::vector<double> m_NSWsTGC_hitGlobalPositionZ;
  std::vector<double> m_NSWsTGC_hitGlobalPositionR;
  std::vector<double> m_NSWsTGC_hitGlobalPositionP;
  std::vector<double> m_NSWsTGC_hitGlobalDirectionX;
  std::vector<double> m_NSWsTGC_hitGlobalDirectionY;
  std::vector<double> m_NSWsTGC_hitGlobalDirectionZ;

  std::vector<double> m_NSWsTGC_detector_globalPositionX;
  std::vector<double> m_NSWsTGC_detector_globalPositionY;
  std::vector<double> m_NSWsTGC_detector_globalPositionZ;
  std::vector<double> m_NSWsTGC_detector_globalPositionR;
  std::vector<double> m_NSWsTGC_detector_globalPositionP;

  std::vector<double> m_NSWsTGC_hitToDsurfacePositionX;
  std::vector<double> m_NSWsTGC_hitToDsurfacePositionY;
  std::vector<double> m_NSWsTGC_hitToDsurfacePositionZ;

  std::vector<double> m_NSWsTGC_hitToRsurfacePositionX;
  std::vector<double> m_NSWsTGC_hitToRsurfacePositionY;
  std::vector<double> m_NSWsTGC_hitToRsurfacePositionZ;

  std::vector<double> m_NSWsTGC_FastDigitRsurfacePositionX;
  std::vector<double> m_NSWsTGC_FastDigitRsurfacePositionY;

  std::vector<int> m_NSWsTGC_particleEncoding;
  std::vector<double> m_NSWsTGC_depositEnergy;

  std::vector<std::string> m_NSWsTGC_sim_stationName;
  std::vector<int> m_NSWsTGC_wedgeId;        // large=0, small=1
  std::vector<int> m_NSWsTGC_wedgeType;      // pivot 0, confirmation 1
  std::vector<int> m_NSWsTGC_detectorNumber; // [0-3]
  std::vector<int> m_NSWsTGC_sim_stationEta;
  std::vector<int> m_NSWsTGC_sim_stationPhi; // [1-16]
  std::vector<int> m_NSWsTGC_sim_multilayer; // [1-2]
  std::vector<int> m_NSWsTGC_sim_layer;      // [1-4]
  std::vector<int> m_NSWsTGC_sim_side;       // [1-4]
  std::vector<int> m_NSWsTGC_stripNumber;
  std::vector<int> m_NSWsTGC_wireNumber;     // wire groups 0-31 in increasing phi

  std::vector<std::string> m_NSWsTGC_off_stationName;
  std::vector<int> m_NSWsTGC_off_stationEta; 
  std::vector<int> m_NSWsTGC_off_stationPhi;
  std::vector<int> m_NSWsTGC_off_multiplet;
  std::vector<int> m_NSWsTGC_off_gas_gap;
  std::vector<int> m_NSWsTGC_off_channel_type;
  std::vector<int> m_NSWsTGC_off_channel;


};

#endif // STGCSIMHITVARIABLES_H

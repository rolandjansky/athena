/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
                    std::string containername) :
    ValAlgVariables(evtStore, detManager, tree, containername, "sTGCSimHitVariables"),
    m_sTgcIdHelper(0),
    m_NSWsTGC_nSimHits(0), 
    m_NSWsTGC_trackId(0),
    m_NSWsTGC_truthEl(0),
    m_NSWsTGC_globalTime(0), 
    m_NSWsTGC_hitGlobalPositionX(0), 
    m_NSWsTGC_hitGlobalPositionY(0), 
    m_NSWsTGC_hitGlobalPositionZ(0), 
    m_NSWsTGC_hitGlobalPositionR(0),
    m_NSWsTGC_hitGlobalPositionP(0),
    m_NSWsTGC_hitGlobalDirectionX(0), 
    m_NSWsTGC_hitGlobalDirectionY(0), 
    m_NSWsTGC_hitGlobalDirectionZ(0), 
    m_NSWsTGC_hitLocalPositionX(0), 
    m_NSWsTGC_hitLocalPositionY(0), 
    m_NSWsTGC_hitLocalPositionZ(0), 
    m_NSWsTGC_detector_globalPositionX(0),
    m_NSWsTGC_detector_globalPositionY(0),
    m_NSWsTGC_detector_globalPositionZ(0),
    m_NSWsTGC_detector_globalPositionR(0),
    m_NSWsTGC_detector_globalPositionP(0),
    m_NSWsTGC_hitToDsurfacePositionX(0),
    m_NSWsTGC_hitToDsurfacePositionY(0),
    m_NSWsTGC_hitToDsurfacePositionZ(0),
    m_NSWsTGC_hitToRsurfacePositionX(0),
    m_NSWsTGC_hitToRsurfacePositionY(0),
    m_NSWsTGC_hitToRsurfacePositionZ(0),
    m_NSWsTGC_FastDigitRsurfacePositionX(0),
    m_NSWsTGC_FastDigitRsurfacePositionY(0),
    m_NSWsTGC_particleEncoding(0), 
    m_NSWsTGC_kineticEnergy(0), 
    m_NSWsTGC_depositEnergy(0), 
    m_NSWsTGC_StepLength(0),
    m_NSWsTGC_sim_stationName(0),
    m_NSWsTGC_wedgeId(0),
    m_NSWsTGC_wedgeType(0),
    m_NSWsTGC_detectorNumber(0),
    m_NSWsTGC_sim_stationEta(0),
    m_NSWsTGC_sim_stationPhi(0),
    m_NSWsTGC_sim_multilayer(0),
    m_NSWsTGC_sim_layer(0),
    m_NSWsTGC_sim_side(0),
    m_NSWsTGC_stripNumber(0),
    m_NSWsTGC_wireNumber(0),
    m_NSWsTGC_off_stationName(0),
    m_NSWsTGC_off_stationEta(0),                                         
    m_NSWsTGC_off_stationPhi(0),
    m_NSWsTGC_off_multiplet(0),
    m_NSWsTGC_off_gas_gap(0),
    m_NSWsTGC_off_channel_type(0),
    m_NSWsTGC_off_channel(0)
  {
    setHelper(idhelper);
  }

  ~sTGCSimHitVariables()
  {
    if( (this->deleteVariables()).isFailure() ){
      ATH_MSG_ERROR("Failing to delete sTGCSimHitVariables.");
      std::terminate();
    }
  }


  StatusCode initializeVariables();
  StatusCode fillVariables();

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_sTgcIdHelper = dynamic_cast<const sTgcIdHelper*>(idhelper);
    if(m_sTgcIdHelper == 0) {
       ATH_MSG_ERROR("casting IdHelper to sTgcIdhelper failed");
       throw;
    }
  }

  StatusCode deleteVariables();
  StatusCode clearVariables();

  const sTgcIdHelper* m_sTgcIdHelper;

  int m_NSWsTGC_nSimHits;
  std::vector<int> *m_NSWsTGC_trackId;
  std::vector<int> *m_NSWsTGC_truthEl;

  std::vector<double> *m_NSWsTGC_globalTime;
  std::vector<double> *m_NSWsTGC_hitGlobalPositionX;
  std::vector<double> *m_NSWsTGC_hitGlobalPositionY;
  std::vector<double> *m_NSWsTGC_hitGlobalPositionZ;
  std::vector<double> *m_NSWsTGC_hitGlobalPositionR;
  std::vector<double> *m_NSWsTGC_hitGlobalPositionP;
  std::vector<double> *m_NSWsTGC_hitGlobalDirectionX;
  std::vector<double> *m_NSWsTGC_hitGlobalDirectionY;
  std::vector<double> *m_NSWsTGC_hitGlobalDirectionZ;

  std::vector<double> *m_NSWsTGC_hitLocalPositionX;
  std::vector<double> *m_NSWsTGC_hitLocalPositionY;
  std::vector<double> *m_NSWsTGC_hitLocalPositionZ;

  std::vector<double> *m_NSWsTGC_detector_globalPositionX;
  std::vector<double> *m_NSWsTGC_detector_globalPositionY;
  std::vector<double> *m_NSWsTGC_detector_globalPositionZ;
  std::vector<double> *m_NSWsTGC_detector_globalPositionR;
  std::vector<double> *m_NSWsTGC_detector_globalPositionP;

  std::vector<double> *m_NSWsTGC_hitToDsurfacePositionX;
  std::vector<double> *m_NSWsTGC_hitToDsurfacePositionY;
  std::vector<double> *m_NSWsTGC_hitToDsurfacePositionZ;

  std::vector<double> *m_NSWsTGC_hitToRsurfacePositionX;
  std::vector<double> *m_NSWsTGC_hitToRsurfacePositionY;
  std::vector<double> *m_NSWsTGC_hitToRsurfacePositionZ;

  std::vector<double> *m_NSWsTGC_FastDigitRsurfacePositionX;
  std::vector<double> *m_NSWsTGC_FastDigitRsurfacePositionY;

  std::vector<int> *m_NSWsTGC_particleEncoding;
  std::vector<double> *m_NSWsTGC_kineticEnergy;
  std::vector<double> *m_NSWsTGC_depositEnergy;
  std::vector<double> *m_NSWsTGC_StepLength;

  std::vector<std::string> *m_NSWsTGC_sim_stationName;
  std::vector<int> *m_NSWsTGC_wedgeId;        // large=0, small=1
  std::vector<int> *m_NSWsTGC_wedgeType;      // pivot 0, confirmation 1
  std::vector<int> *m_NSWsTGC_detectorNumber; // [0-3]
  std::vector<int> *m_NSWsTGC_sim_stationEta;
  std::vector<int> *m_NSWsTGC_sim_stationPhi; // [1-16]
  std::vector<int> *m_NSWsTGC_sim_multilayer; // [1-2]
  std::vector<int> *m_NSWsTGC_sim_layer;      // [1-4]
  std::vector<int> *m_NSWsTGC_sim_side;       // [1-4]
  std::vector<int> *m_NSWsTGC_stripNumber;
  std::vector<int> *m_NSWsTGC_wireNumber;     // wire groups 0-31 in increasing phi

  std::vector<std::string> *m_NSWsTGC_off_stationName;
  std::vector<int> *m_NSWsTGC_off_stationEta; 
  std::vector<int> *m_NSWsTGC_off_stationPhi;
  std::vector<int> *m_NSWsTGC_off_multiplet;
  std::vector<int> *m_NSWsTGC_off_gas_gap;
  std::vector<int> *m_NSWsTGC_off_channel_type;
  std::vector<int> *m_NSWsTGC_off_channel;


};

#endif // STGCSIMHITVARIABLES_H

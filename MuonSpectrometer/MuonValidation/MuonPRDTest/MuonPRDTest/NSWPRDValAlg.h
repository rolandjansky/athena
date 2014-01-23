/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NSWPRDVALALG_H // Include guard
#define NSWPRDVALALG_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include <vector>

class ITHistSvc;
class TTree;
class MmIdHelper;
class sTgcIdHelper;

namespace MuonGM {
  class MuonDetectorManager;
}

class NSWPRDValAlg:public AthAlgorithm
{
 public:
  NSWPRDValAlg(const std::string& name, ISvcLocator* pSvcLocator); // Constructor

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();

 private:
  void initializeTruthVariables(); 
  void clearTruthVariables(); 
  StatusCode fillTruthVariables(); 
  void deleteTruthVariables(); 

  void initializeMuEntryVariables(); 
  void clearMuEntryVariables(); 
  StatusCode fillMuEntryVariables(); 
  void deleteMuEntryVariables(); 

  // sTGC hit validation
  void initializeNSWsTGCHitVariables(); 
  void clearNSWsTGCHitVariables(); 
  StatusCode fillNSWsTGCHitVariables(); 
  void deleteNSWsTGCHitVariables(); 

  // sTGC fast digitization validation
  void initializeNSWsTGCFastDigitVariables();
  void clearNSWsTGCFastDigitVariables();
  StatusCode fillNSWsTGCFastDigitVariables();
  void deleteNSWsTGCFastDigitVariables();

  // sTGC digitization validation
  void initializeNSWsTGCDigitVariables();
  void clearNSWsTGCDigitVariables();
  StatusCode fillNSWsTGCDigitVariables();
  void deleteNSWsTGCDigitVariables();


  // MicroMegas hit validation
  void initializeNSWMMHitVariables(); 
  void clearNSWMMHitVariables(); 
  StatusCode fillNSWMMHitVariables();
  void deleteNSWMMHitVariables(); 

  // MicroMegas fast digitization validation
  void initializeNSWMMFastDigitVariables();
  void clearNSWMMFastDigitVariables();
  StatusCode fillNSWMMFastDigitVariables();
  void deleteNSWMMFastDigitVariables();

  // MicroMegas digitization validation 
  void initializeNSWMMDigitVariables();
  void clearNSWMMDigitVariables();
  StatusCode fillNSWMMDigitVariables();
  void deleteNSWMMDigitVariables();


  ITHistSvc *m_thistSvc;
  TTree *m_tree;

  const MuonGM::MuonDetectorManager* m_detManager;
  const MmIdHelper*   m_MmIdHelper;
  const sTgcIdHelper* m_sTgcIdHelper;

  BooleanProperty  m_doTruth;            // switch on the output of the MC truth
  BooleanProperty  m_doMuEntry;          // switch on the output of the Muon Entry Layer
  BooleanProperty  m_doSTGCHit;          // switch on the output of the Small TGC data
  BooleanProperty  m_doSTGCFastDigit;    // switch on the output of the Small TGC fast digitization
  BooleanProperty  m_doSTGCDigit;        // swicth on the output of the Small TGC digit
  BooleanProperty  m_doMMHit;            // switch on the output of the MicroMegas data
  BooleanProperty  m_doMMFastDigit;      // switch on the output of the MicroMegas fast digitization
  BooleanProperty  m_doMMDigit;          // switch on the output of the MicroMegas digitization

  unsigned int m_runNumber;
  unsigned int m_eventNumber;

  std::string m_Truth_ContainerName;
  int m_Truth_nVertices;
  std::vector<double> *m_Truth_vertexX;
  std::vector<double> *m_Truth_vertexY;
  std::vector<double> *m_Truth_vertexZ;
  std::vector<double> *m_Truth_vertexT;
  std::vector<int> *m_Truth_vertexId;
  
  int m_Truth_nParticles;  
  std::vector<double> *m_Truth_particlePt;
  std::vector<double> *m_Truth_particleEta;
  std::vector<double> *m_Truth_particlePhi;
  std::vector<double> *m_Truth_particleE;
  std::vector<double> *m_Truth_particleM;
  std::vector<int> *m_Truth_particlePdg_id;
  std::vector<int> *m_Truth_particleStatus;
  std::vector<int> *m_Truth_particleBarcode;
  std::vector<int> *m_Truth_particleProduction_vertex_id;
  std::vector<int> *m_Truth_particleEnd_vertex_id;  
  
  std::string m_MuEntry_ContainerName;
  int m_MuEntry_nParticles;  
  std::vector<double> *m_MuEntry_particlePt;
  std::vector<double> *m_MuEntry_particleEta;
  std::vector<double> *m_MuEntry_particlePhi;
  std::vector<int> *m_MuEntry_particlePdg_id;
  std::vector<int> *m_MuEntry_particleBarcode;
  std::vector<double> *m_MuEntry_positionEta;
  std::vector<double> *m_MuEntry_positionPhi;
  std::vector<double> *m_MuEntry_positionX;
  std::vector<double> *m_MuEntry_positionY;
  std::vector<double> *m_MuEntry_positionZ;


  // sTGC variables for hit validation
  std::string m_NSWsTGC_ContainerName;
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
  std::vector<int> *m_NSWsTGC_stripNumber;    // 0 (small R to n-1
  std::vector<int> *m_NSWsTGC_wireNumber;     // wire groups 0-31 in increasing phi

  std::vector<std::string> *m_NSWsTGC_off_stationName;
  std::vector<int> *m_NSWsTGC_off_stationEta; 
  std::vector<int> *m_NSWsTGC_off_stationPhi;
  std::vector<int> *m_NSWsTGC_off_multiplet;
  std::vector<int> *m_NSWsTGC_off_gas_gap;
  std::vector<int> *m_NSWsTGC_off_channel_type;
  std::vector<int> *m_NSWsTGC_off_channel;


  // sTGC variables for fast digitization validation
  std::string m_NSWsTGC_FastDigitContainerName;
  int m_NSWsTGC_nFastDigits;
  std::vector<std::string> *m_NSWsTGC_fdg_stationName;
  std::vector<int> *m_NSWsTGC_fdg_stationEta;
  std::vector<int> *m_NSWsTGC_fdg_stationPhi;
  std::vector<int> *m_NSWsTGC_fdg_multiplet;
  std::vector<int> *m_NSWsTGC_fdg_gas_gap;
  std::vector<int> *m_NSWsTGC_fdg_channel_type;
  std::vector<int> *m_NSWsTGC_fdg_channel;

  std::vector<double> *m_NSWsTGC_fdg_globalPosX;
  std::vector<double> *m_NSWsTGC_fdg_globalPosY;
  std::vector<double> *m_NSWsTGC_fdg_globalPosZ;

  std::vector<double> *m_NSWsTGC_fdg_localPosX;
  std::vector<double> *m_NSWsTGC_fdg_localPosY;
  std::vector<double> *m_NSWsTGC_fdg_covMatrix_1_1;
  std::vector<double> *m_NSWsTGC_fdg_covMatrix_2_2;


  std::vector<int>    *m_NSWsTGC_fdg_truth_barcode;
  std::vector<double> *m_NSWsTGC_fdg_truth_hitOnSurfaceX;
  std::vector<double> *m_NSWsTGC_fdg_truth_hitOnSurfaceY;


  // sTGC variables for digitization validation
  std::string m_NSWsTGC_DigitContainerName;
  int m_NSWsTGC_nDigits;
  std::vector<std::string> *m_NSWsTGC_dig_stationName;
  std::vector<int> *m_NSWsTGC_dig_stationEta;
  std::vector<int> *m_NSWsTGC_dig_stationPhi;
  std::vector<int> *m_NSWsTGC_dig_multiplet;
  std::vector<int> *m_NSWsTGC_dig_gas_gap;
  std::vector<int> *m_NSWsTGC_dig_channel_type;
  std::vector<int> *m_NSWsTGC_dig_channel;



  // MicroMegas variables for hit validation
  std::string m_NSWMM_ContainerName;
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


  // MicroMegas variables for fast digitization validation
  std::string m_NSWMM_FastDigitContainerName;
  int m_NSWMM_nFastDigits;
  std::vector<std::string> *m_NSWMM_fdg_stationName;
  std::vector<int> *m_NSWMM_fdg_stationEta;
  std::vector<int> *m_NSWMM_fdg_stationPhi;
  std::vector<int> *m_NSWMM_fdg_multiplet;
  std::vector<int> *m_NSWMM_fdg_gas_gap;
  std::vector<int> *m_NSWMM_fdg_channel;

  std::vector<double> *m_NSWMM_fdg_globalPosX;
  std::vector<double> *m_NSWMM_fdg_globalPosY;
  std::vector<double> *m_NSWMM_fdg_globalPosZ;

  std::vector<double> *m_NSWMM_fdg_localPosX;
  std::vector<double> *m_NSWMM_fdg_localPosY;
  std::vector<double> *m_NSWMM_fdg_covMatrix_1_1;

  std::vector<int>    *m_NSWMM_fdg_truth_barcode;
  std::vector<double> *m_NSWMM_fdg_truth_hitOnSurfaceX;
  std::vector<double> *m_NSWMM_fdg_truth_hitOnSurfaceY;



  // MicroMegas variables for digitization validation
  std::string m_NSWMM_DigitContainerName;
  int m_NSWMM_nDigits;
  std::vector<std::string> *m_NSWMM_dig_stationName;
  std::vector<int> *m_NSWMM_dig_stationEta;
  std::vector<int> *m_NSWMM_dig_stationPhi;
  std::vector<int> *m_NSWMM_dig_multiplet;
  std::vector<int> *m_NSWMM_dig_gas_gap;
  std::vector<int> *m_NSWMM_dig_channel;

  std::vector< std::vector<float> >  *m_NSWMM_dig_time;
  std::vector< std::vector<float> >  *m_NSWMM_dig_charge;
  std::vector< std::vector<int> >    *m_NSWMM_dig_stripPosition;
  std::vector< std::vector<double> > *m_NSWMM_dig_stripLposX;
  std::vector< std::vector<double> > *m_NSWMM_dig_stripLposY;
  std::vector< std::vector<double> > *m_NSWMM_dig_stripGposX;
  std::vector< std::vector<double> > *m_NSWMM_dig_stripGposY;
  std::vector< std::vector<double> > *m_NSWMM_dig_stripGposZ;
  std::vector< std::vector<float> >  *m_NSWMM_dig_sr_time;
  std::vector< std::vector<float> >  *m_NSWMM_dig_sr_charge;
  std::vector< std::vector<int> >    *m_NSWMM_dig_sr_stripPosition;
  std::vector< std::vector<double> > *m_NSWMM_dig_sr_stripLposX;
  std::vector< std::vector<double> > *m_NSWMM_dig_sr_stripLposY;
  std::vector< std::vector<double> > *m_NSWMM_dig_sr_stripGposX;
  std::vector< std::vector<double> > *m_NSWMM_dig_sr_stripGposY;
  std::vector< std::vector<double> > *m_NSWMM_dig_sr_stripGposZ;


  std::vector< int    > *m_NSWMM_dig_truth_barcode;
  std::vector< double > *m_NSWMM_dig_truth_localPosX;
  std::vector< double > *m_NSWMM_dig_truth_localPosY;
  std::vector< double > *m_NSWMM_dig_truth_globalPosX;
  std::vector< double > *m_NSWMM_dig_truth_globalPosY;
  std::vector< double > *m_NSWMM_dig_truth_globalPosZ;

  std::vector< float  > *m_NSWMM_dig_truth_XZ_angle;

  std::vector<int>   *m_NSWMM_dig_stripForTrigger;
  std::vector<float> *m_NSWMM_dig_stripTimeForTrigger;

  double m_energyThreshold;
};

#endif // NSWHITSDUMPALG_H

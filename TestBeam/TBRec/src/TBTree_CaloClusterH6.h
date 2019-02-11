/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBTREE_CALOCLUSTERH6_H
#define TBREC_TBTREE_CALOCLUSTERH6_H

// 
// TBTree_CaloClusterH6:
// Make ROOT TTree for CaloClusters for H6 CBT
//
#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArCabling/LArOnOffIdMapping.h"

#include <TRandom.h>

class StoreGateSvc;
class TFile;
class TTree;
class CaloCell_ID;
class CaloDetDescrManager;
class CaloCluster;
class IToolSvc;
class ICaloNoiseTool;
class LArDigitContainer;
class ILArPedestal;
class ILArADC2MeVTool;

class TBTree_CaloClusterH6: public AthAlgorithm {
 public:    
  
  TBTree_CaloClusterH6(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TBTree_CaloClusterH6();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private: 
  void clear();
  /** Get Xcryo and Ytable from a text file */
  StatusCode getXcryoYtable(float &x, float &y, float &eBeam);
  StatusCode getNoise(CaloCluster const * const cluster);
  
  std::string m_suffix;
  int m_nEvent;                     // counter
  int m_nEventRejected;             // counter
  int m_nEventAccepted;             // counter
  int m_nEventRandomTrigger;        // counter
  bool m_addNoise;
  bool m_addMoments;
  bool m_addGain;
  bool m_addTime;
  bool m_addQuality;
  bool m_addBeamTrack;
  bool m_addWTC;
  bool m_useEMTBCluster;
  bool m_first;             // First event flag

  // Run header
  int m_nRun;               /** Run number */
  float m_beamMom;          /** Beam momentum */
  float m_xCryo;            /** CryoX */
  float m_yTable;           /** TableY */
  float m_zCalo;            /** z-coordinate of the calorimeter surface
                                at which beam coordinates calculated */

  // Variables to be in the TTree
  //--------------------------
  // Event type
  unsigned short m_evType;
  // Parameters of the sum of clusters (TB particle)
  float m_eTotal;
  float m_etaTotal;
  float m_phiTotal;
  float m_eEME2;
  float m_eEME3;
  float m_eHEC0;
  float m_eHEC1;
  float m_eHEC2;
  float m_eFCAL0;
  float m_eFCAL1;
  float m_eFCAL2;
  // Claster parameters
  int m_nClusters;
  std::vector<int>* m_nCellCluster;
  std::vector<float>* m_eCluster;
  std::vector<float>* m_etaCluster; 
  std::vector<float>* m_phiCluster; 
  // Cluster moments
  std::vector<float>* m_m1_eta;
  std::vector<float>* m_m1_phi;
  std::vector<float>* m_m2_r;
  std::vector<float>* m_m2_lambda;
  std::vector<float>* m_delta_phi;
  std::vector<float>* m_delta_theta;
  std::vector<float>* m_delta_alpha;
  std::vector<float>* m_center_x;
  std::vector<float>* m_center_y;
  std::vector<float>* m_center_z;
  std::vector<float>* m_center_lambda;
  std::vector<float>* m_lateral;
  std::vector<float>* m_longitudinal;
  std::vector<float>* m_eng_frac_em;
  std::vector<float>* m_eng_frac_max;
  std::vector<float>* m_eng_frac_core;
  std::vector<float>* m_m1_dens;
  std::vector<float>* m_m2_dens;
  // Cell parameters
  int m_nCells;
  std::vector<int>* m_cell_id;
  std::vector<int>* m_cell_ind_cluster;
  std::vector<float>* m_cell_energy;
  std::vector<float>* m_cell_noise;
  std::vector<int>* m_cell_gain;
  std::vector<float>* m_cell_time;
  std::vector<float>* m_cell_quality;
  
  // Beam track parameters
  float m_beam_coor_x;
  float m_beam_coor_y;
  float m_beam_chi2_x;
  float m_beam_chi2_y;
  float m_beam_intercept_x;
  float m_beam_intercept_y;
  float m_beam_slope_x;
  float m_beam_slope_y;

  // Warm TailCatcher data: one word per layer (6 layers at all)
  std::vector<short>* m_wtcNOverflow;
  std::vector<float>* m_wtcSignal;

  // Names and pointers
  std::string m_digitContainerName;
  std::string m_caloCellContainerName;    // Cell container name
  std::string m_clusterContainerName;     // Cluster container
  std::string m_WTCContainerName;         // Warm TailCatcher
  std::string m_noiseToolName;            // Noise tool name
  std::string m_OFNoiseSuppToolName;      // Noise tool name for OFNoiseSupp
  std::string m_TBTreeName;               // TBTree name
  std::string m_rootfile_name;            // name of the ROOT file with TBTree
  TFile* m_rootfile;                      // and its pointer
  TTree* m_tree;                          // TBTree pointer

  const CaloCell_ID* m_calo_id;
  const CaloDetDescrManager* m_calo_dd_man; 
  ICaloNoiseTool* m_noiseTool;
  ICaloNoiseTool* m_OFNoiseSupp;
  const ILArADC2MeVTool* m_adc2mevTool;
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

  /** Text file containing xCryo and yTable */
  std::string m_txtFileWithXY;
  // Random generator
  TRandom m_rndm;

};

#endif

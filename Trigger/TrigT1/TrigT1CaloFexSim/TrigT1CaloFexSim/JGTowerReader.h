/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXSIM_JGTOWERREADER_H
#define TRIGT1CALOFEXSIM_JGTOWERREADER_H 

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/WriteHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloTriggerTool/JTowerSCMap.h"
#include "CaloTriggerTool/GTowerSCMap.h"
#include "CaloIdentifier/TTOnlineID.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/JTower_ID.h"
#include "CaloIdentifier/GTower_ID.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODTrigL1Calo/JGTower.h"
#include "xAODTrigL1Calo/JGTowerContainer.h"
#include "xAODTrigL1Calo/JGTowerAuxContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetInterface/IJetUpdateJvt.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"
#include "Identifier/IdentifierHash.h"
#include "TH1.h"
#include "TH2.h"
#include "TrigT1CaloFexSim/JetAlg.h"
#include "TrigT1CaloFexSim/METAlg.h"
class JGTowerReader: public ::AthAlgorithm { 
 public: 
  JGTowerReader( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~JGTowerReader(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
  virtual StatusCode  beginInputFile();
  virtual StatusCode  ProcessObject();
 private: 
  bool m_outputNoise;
  float m_jJet_thr;
  float m_jSeed_size;
  float m_jMax_r;
  float m_jJet_r;
  float m_gJet_thr;
  float m_gSeed_size;
  float m_gMax_r;
  float m_gJet_r;
  std::string m_noise_file;
 
  const CaloCell_SuperCell_ID* m_scid;
  const JTower_ID* m_jTowerId;
  const GTower_ID* m_gTowerId;
 
  virtual StatusCode JFexAlg(const xAOD::JGTowerContainer*jTs);
  virtual StatusCode GFexAlg(const xAOD::JGTowerContainer*gTs); 

  std::vector<float> jT_noise;
  std::vector<float> jJet_thr;
  std::vector<float> gT_noise;
  std::vector<float> gJet_thr;


  JetAlg::Seed*   jSeeds=new JetAlg::Seed;
  JetAlg::Seed*   gSeeds=new JetAlg::Seed;
  METAlg::MET*    jMET=new  METAlg::MET;
  METAlg::MET*    gMET=new  METAlg::MET;
  std::vector<JetAlg::L1Jet>  jL1Jets;
  std::vector<JetAlg::L1Jet>  gL1Jets;
  std::map<TString, TH1*> hName;
  std::map<TString, TH2*> h2Name;
  int m_jTowerHashMax;
  int m_gTowerHashMax;

}; 

#endif //> !TRIGT1CALOFEXSIM_JGTOWERREADER_H

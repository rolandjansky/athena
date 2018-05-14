#ifndef TRIGT1CALOFEXSIM_JGTOWERREADER_H
#define TRIGT1CALOFEXSIM_JGTOWERREADER_H 1

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

class JGTowerReader: public ::AthAlgorithm { 
 public: 
  JGTowerReader( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~JGTowerReader(); 



  struct L1Jet{

    float eta;
    float phi;
    float et;
  // constructor for convenience
    L1Jet(float eta_j, float phi_j, float et_j){
      eta = eta_j;
      phi = phi_j;
      et  = et_j;
    };
  };

  struct Seed{

    std::vector<float> eta;
    std::vector<std::vector<float>> phi;
    std::vector<std::vector<float>> et;
    std::vector<std::vector<bool>> local_max;
  };


  struct MET{
    float phi;
    float et;

  };

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
  virtual StatusCode  beginInputFile();

   private: 
  bool m_outputNoise;
  float m_jSeed_size;
  float m_jJet_r;
  float m_gSeed_size;
  float m_gJet_r;
  std::string m_noise_file;
 
  const CaloCell_SuperCell_ID* m_scid;
  const JTower_ID* m_jTowerId;
  const GTower_ID* m_gTowerId;
 
  virtual StatusCode JFexAlg(const xAOD::JGTowerContainer*jTs);
  virtual StatusCode GFexAlg(const xAOD::JGTowerContainer*gTs); 

  virtual StatusCode SeedGrid(const xAOD::JGTowerContainer*towers, JGTowerReader::Seed*seeds);
  virtual StatusCode SeedFinding(const xAOD::JGTowerContainer*towers, JGTowerReader::Seed*seeds, float seed_size,float range, std::vector<float> noise);

  virtual StatusCode BuildJet(const xAOD::JGTowerContainer*towers,std::vector<float> noise, JGTowerReader::Seed*seeds, std::vector<JGTowerReader::L1Jet> &js, float jet_size);
  virtual StatusCode BuildMET(const xAOD::JGTowerContainer*towers,std::vector<float> noise,JGTowerReader::MET* met);

  virtual StatusCode ProcessObject();

  std::vector<float> jT_noise;
  std::vector<float> gT_noise;
  JGTowerReader::Seed*   jSeeds=new JGTowerReader::Seed;
  JGTowerReader::Seed*   gSeeds=new JGTowerReader::Seed;
  JGTowerReader::MET*    jMET=new  JGTowerReader::MET;
  JGTowerReader::MET*    gMET=new  JGTowerReader::MET;
  std::vector<JGTowerReader::L1Jet>  jL1Jets;
  std::vector<JGTowerReader::L1Jet>  gL1Jets;
  std::map<TString, TH1*> hName;
  std::map<TString, TH2*> h2Name;
  int m_jTowerHashMax;
  int m_gTowerHashMax;

}; 

#endif //> !TRIGT1CALOFEXSIM_JGTOWERREADER_H

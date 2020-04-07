#ifndef TRIGT1CALOFEXSIM_JFEXELETAU_H
#define TRIGT1CALOFEXSIM_JFEXELETAU_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#include "xAODTrigL1Calo/JGTowerContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"

class JFexEleTau: public ::AthAnalysisAlgorithm { 
 public: 
  JFexEleTau( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~JFexEleTau(); 

  ///uncomment and implement methods as required
  virtual StatusCode  initialize();     //once, before any input is loaded
  virtual StatusCode  execute();        //per event
  virtual StatusCode  finalize();       //once, after all events processed
  virtual StatusCode analyzeSeedRun2(float eta, float phi, const xAOD::JGTowerContainer* jTowers, xAOD::EmTauRoIContainer* clustersForTau); // Use a given seed to produce a candidate
  virtual StatusCode analyzeSeedRun3(float eta, float phi, const xAOD::JGTowerContainer* jTowers, xAOD::EmTauRoIContainer* clustersForTau); // Use a given seed to produce a candidate
  virtual StatusCode analyzeSeedEle(float eta, float phi, const xAOD::JGTowerContainer* jTowers, xAOD::EmTauRoIContainer* clustersForEle); // Use a given seed to produce a candidate

 private: 

  bool m_regenerateSeeds = false;
  bool m_applyNoise = false;
  bool m_checkMax = false;
  int m_noiseStrategy = 0;
  bool m_useRun2 = false;
  bool m_singleTowerSeed = false;

}; 

#endif //> !TRIGT1CALOFEXSIM_JFEXELETAU_H

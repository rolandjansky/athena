#ifndef L1CALOCONDALGREADER_H
#define L1CALOCONDALGREADER_H




#include <vector>
#include "TString.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"


#include "TrigT1CaloCalibConditions/L1CaloDisabledTowersContainer.h" 
#include "TrigT1CaloCalibConditions/L1CaloDerivedRunParsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanCalibContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanDefaultsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPpmFineTimeRefsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloRunParametersContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanStrategyContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPpmDeadChannelsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditionsContainerRun2.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditionsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprDisabledChannelContainerRun2.h"
#include "TrigT1CaloCalibConditions/L1CaloReadoutConfigContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloReadoutConfigContainerJSON.h"





class L1CaloCondAlgReader: public ::AthAlgorithm { 
 public: 
    // this is a standard algorithm constructor

  L1CaloCondAlgReader( const std::string& name, ISvcLocator* pSvcLocator );


  // these are the functions inherited from Algorithm
  virtual StatusCode initialize () override;
  virtual StatusCode execute () override;


 private: 
  
  SG::ReadHandleKey<xAOD::TriggerTowerContainer> m_legacy {this, "TriggerTowerContainer", "xAODTriggerTowers", "Trigger Tower container"};


  
  SG::ReadCondHandleKey<L1CaloDisabledTowersContainer> m_disabledTowersContainer{this,"InputKeyDisChan","L1CaloDisabledTowersContainer"}; 
  SG::ReadCondHandleKey<L1CaloDerivedRunParsContainer> m_derivedRunParsContainer{this, "InputKeyDerRunsPars", "L1CaloDerivedRunParsContainer"};
  SG::ReadCondHandleKey<L1CaloPprChanCalibContainer>  m_pprChanCalibContainer{ this, "InputKeyPPr", "L1CaloPprChanCalibContainer"}; 
  SG::ReadCondHandleKey<L1CaloPprChanDefaultsContainer>  m_pprChanDefaultsContainer{ this, "InputKeyDef", "L1CaloPprChanDefaultsContainer"}; 
  SG::ReadCondHandleKey<L1CaloPpmFineTimeRefsContainer>  m_ppmFineTimeRefsContainer{ this, "InputKeyTimeRefs", "L1CaloPpmFineTimeRefsContainer"}; 
  SG::ReadCondHandleKey<L1CaloRunParametersContainer>  m_runParametersContainer{ this, "InputKeyRunParameters", "L1CaloRunParametersContainer"};
  SG::ReadCondHandleKey<L1CaloPprChanStrategyContainer>  m_pprChanStrategyContainer{ this, "InputKeyPprChanStrategy", "L1CaloPprChanStrategyContainer"};
  SG::ReadCondHandleKey<L1CaloPpmDeadChannelsContainer> m_ppmDeadChannelsContainer{ this, "InputKeyPpmDeadChannels", "L1CaloPpmDeadChannelsContainer"};
  SG::ReadCondHandleKey<L1CaloPprConditionsContainerRun2> m_pprConditionsContainerRun2{ this, "InputKeyPprConditionsRun2", "L1CaloPprConditionsContainerRun2"};
  SG::ReadCondHandleKey<L1CaloPprDisabledChannelContainerRun2> m_pprDisabledChannelContainer{ this, "InputKeyDisabledChannel", "L1CaloPprDisabledChannelContainerRun2"};
  SG::ReadCondHandleKey<L1CaloReadoutConfigContainer>  m_readoutConfigContainer{ this, "InputKeyReadoutConfig", "L1CaloReadoutConfigContainer"};
  
  SG::ReadCondHandleKey<L1CaloReadoutConfigContainerJSON>  m_readoutConfigContainerJSON{ this, "InputKeyReadoutConfigJSON", "L1CaloReadoutConfigContainerJSON"};




}; 

#endif //> !MYPACKAGE_MYPACKAGEALG_H

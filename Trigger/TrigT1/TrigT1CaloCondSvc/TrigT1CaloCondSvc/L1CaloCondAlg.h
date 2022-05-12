#ifndef L1CALOCONDALG_H
#define L1CALOCONDALG_H



#include "TString.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaKernel/IOVInfiniteRange.h"

#include "TrigT1CaloCalibConditions/L1CaloDisabledTowersContainer.h" 
#include "TrigT1CaloCalibConditions/L1CaloDerivedRunParsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanCalibContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanDefaultsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPpmFineTimeRefsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloRunParametersContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanStrategyContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPpmDeadChannelsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPpmDeadChannelsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditionsContainerRun2.h"
#include "TrigT1CaloCalibConditions/L1CaloPprDisabledChannelContainerRun2.h" 
#include "TrigT1CaloCalibConditions/L1CaloReadoutConfigContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloReadoutConfigContainerJSON.h"


class L1CaloCondAlg: public ::AthAlgorithm { 
 public: 
    // this is a standard algorithm constructor

  L1CaloCondAlg( const std::string& name, ISvcLocator* pSvcLocator );


  // these are the functions inherited from Algorithm
  virtual StatusCode initialize () override;
  virtual StatusCode execute () override;


 private: 


  Gaudi::Property<std::string> m_timingRegime{this,"timingRegime","","String-> Cali1, Calib2 or Physics"};  
  Gaudi::Property<std::string> m_strategy{this,"strategy","","String-> empty, PprChanHighMu or PprChanLowMu"}; 


  // Folders only located at V1
  SG::ReadCondHandleKey<CondAttrListCollection> m_disabledTowers{ this, "DisabledTowers", "/TRIGGER/L1Calo/V1/Conditions/DisabledTowers", "DisabledTowers" };
  SG::ReadCondHandleKey<CondAttrListCollection> m_derivedRunPars{ this, "DerivedRunPars", "/TRIGGER/L1Calo/V1/Conditions/DerivedRunPars",  "DerivedRunPars"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_ppmDeadChannels{ this, "PpmDeadChannels", "/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels","PpmDeadChannels" };
  SG::ReadCondHandleKey<CondAttrListCollection> m_ppmFineTimeRefs{ this, "PpmFineTimeRefs" ,"/TRIGGER/L1Calo/V1/References/FineTimeReferences",  "PpmFineTimeRefs"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_runParameters{ this, "RunParameters" ,"/TRIGGER/L1Calo/V1/Conditions/RunParameters",  "RunParameters"};



  // Folders with configuration information
  SG::ReadCondHandleKey<CondAttrListCollection> m_pprChanDefaults{ this, "PprChanDefaults" ,"/TRIGGER/L1Calo/V2/Configuration/PprChanDefaults",  "PprChanDefaults"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_pprChanStrategy{ this, "PprChanStrategy", "/TRIGGER/L1Calo/V2/Configuration/PprChanStrategy",  "PprChanStrategy"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_readoutConfig{ this, "ReadoutConfig",  "/TRIGGER/L1Calo/V2/Configuration/ReadoutConfig",  "ReadoutConfig"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_readoutConfigJSON{ this, "ReadoutConfigJSON",  "/TRIGGER/L1Calo/V2/Configuration/ReadoutConfigJSON",  "ReadoutConfigJSON"};


  // Defining WriteCondHandleKey for saving condition containers 
  SG::WriteCondHandleKey<L1CaloDisabledTowersContainer> m_disabledTowersContainer{this,"OutputKeyDisTowers","L1CaloDisabledTowersContainer"}; 
  SG::WriteCondHandleKey<L1CaloDerivedRunParsContainer> m_derivedRunParsContainer{this, "OutputKeyDerRunsPars", "L1CaloDerivedRunParsContainer"};
  SG::WriteCondHandleKey<L1CaloPprChanCalibContainer>  m_pprChanCalibContainer{ this, "OutputKeyPPr", "L1CaloPprChanCalibContainer"}; 
  SG::WriteCondHandleKey<L1CaloPprChanDefaultsContainer>  m_pprChanDefaultsContainer{ this, "OutputKeyDef", "L1CaloPprChanDefaultsContainer"}; 
  SG::WriteCondHandleKey<L1CaloPpmFineTimeRefsContainer>  m_ppmFineTimeRefsContainer{ this, "OutputKeyTimeRefs", "L1CaloPpmFineTimeRefsContainer"}; 
  SG::WriteCondHandleKey<L1CaloRunParametersContainer>  m_runParametersContainer{ this, "OutputKeyRunParameters", "L1CaloRunParametersContainer"};
  SG::WriteCondHandleKey<L1CaloPprChanStrategyContainer>  m_pprChanStrategyContainer{ this, "OutputKeyPprChanStrategy", "L1CaloPprChanStrategyContainer"};
  SG::WriteCondHandleKey<L1CaloPpmDeadChannelsContainer> m_ppmDeadChannelsContainer{ this, "OutputKeyPpmDeadChannels", "L1CaloPpmDeadChannelsContainer"};
  SG::WriteCondHandleKey<L1CaloPprConditionsContainerRun2> m_pprConditionsContainer{ this, "OutputKeyPprConditionsRun2", "L1CaloPprConditionsContainerRun2"};
  SG::WriteCondHandleKey<L1CaloPprDisabledChannelContainerRun2> m_pprDisabledChannelContainer{ this, "OutputKeyDisabledChannelRun2", "L1CaloPprDisabledChannelContainerRun2"};
  SG::WriteCondHandleKey<L1CaloReadoutConfigContainer>  m_readoutConfigContainer{ this, "OutputKeyReadoutConfig", "L1CaloReadoutConfigContainer"};
  SG::WriteCondHandleKey<L1CaloReadoutConfigContainerJSON>  m_readoutConfigContainerJSON{ this, "OutputKeyReadoutConfigJSON", "L1CaloReadoutConfigContainerJSON"};



}; 

#endif //> !MYPACKAGE_MYPACKAGEALG_H

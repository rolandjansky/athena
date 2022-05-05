// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "TrigT1CaloCondSvc/L1CaloCondAlgReader.h"
#include "StoreGate/StoreGateSvc.h"


#include <iomanip>
#include "TH1F.h"
#include "TrigT1CaloCalibConditions/IL1CaloPersistenceCapable.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h" 
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"


L1CaloCondAlgReader :: L1CaloCondAlgReader ( const std::string &name,ISvcLocator *pSvcLocator): AthAlgorithm( name, pSvcLocator )
{

}





StatusCode  L1CaloCondAlgReader:: initialize ()
{
  
  ATH_MSG_INFO( "initialize " << name() );                
  
  ATH_CHECK( m_legacy.initialize() );

  ATH_CHECK( m_disabledTowersContainer.initialize() );
  ATH_CHECK( m_derivedRunParsContainer.initialize() );
  ATH_CHECK( m_pprChanCalibContainer.initialize() );
  ATH_CHECK( m_pprChanDefaultsContainer.initialize() );
  ATH_CHECK( m_ppmFineTimeRefsContainer.initialize() );
  ATH_CHECK( m_runParametersContainer.initialize() );
  ATH_CHECK( m_pprChanStrategyContainer.initialize() );
  ATH_CHECK( m_ppmDeadChannelsContainer.initialize() );
  ATH_CHECK( m_pprConditionsContainerRun2.initialize() );
  ATH_CHECK( m_pprDisabledChannelContainer.initialize() );
  ATH_CHECK( m_readoutConfigContainer.initialize() );
  ATH_CHECK( m_readoutConfigContainerJSON.initialize() );


  return StatusCode::SUCCESS;
}


StatusCode  L1CaloCondAlgReader:: execute ()
{
  
  ATH_MSG_INFO( "start execute " << name() ); 



  SG::ReadHandle<xAOD::TriggerTowerContainer>tts(m_legacy);   
  ATH_MSG_INFO("size Trigger Tower Container " << tts->size());
  
  SG::ReadCondHandle<L1CaloDisabledTowersContainer> disTowers( m_disabledTowersContainer);
  SG::ReadCondHandle<L1CaloDerivedRunParsContainer> derRunPars( m_derivedRunParsContainer);
  SG::ReadCondHandle<L1CaloPprChanCalibContainer> pprChanCalib( m_pprChanCalibContainer);
  SG::ReadCondHandle<L1CaloPprChanDefaultsContainer> pprChanDefaults( m_pprChanDefaultsContainer);
  SG::ReadCondHandle<L1CaloPpmFineTimeRefsContainer> ppmFineTimeRefs( m_ppmFineTimeRefsContainer);
  SG::ReadCondHandle<L1CaloRunParametersContainer> runParameters( m_runParametersContainer);
  SG::ReadCondHandle<L1CaloPprChanStrategyContainer> pprChanStrategy(  m_pprChanStrategyContainer);
  SG::ReadCondHandle<L1CaloPpmDeadChannelsContainer> ppmDeadChannels ( m_ppmDeadChannelsContainer);
  SG::ReadCondHandle<L1CaloPprConditionsContainerRun2>  pprConditionsRun2( m_pprConditionsContainerRun2);
  SG::ReadCondHandle<L1CaloPprDisabledChannelContainerRun2>  pprDisabledChannel(m_pprDisabledChannelContainer);
  SG::ReadCondHandle<L1CaloReadoutConfigContainer> readoutConfig(m_readoutConfigContainer);
  SG::ReadCondHandle<L1CaloReadoutConfigContainerJSON> readoutConfigJSON(m_readoutConfigContainerJSON);


  

  ATH_MSG_INFO("************************************************************************");
  
  L1CaloDerivedRunParsContainer::const_iterator it = derRunPars->begin();   
  for(;it!=derRunPars->end();++it) {     
    ATH_MSG_INFO("DerRunPars:: " << *it);
    ATH_MSG_INFO("DerRunPars:: channelId: " << it->channelId());
    ATH_MSG_INFO("DerRunPars:: timingRegime: " << it->timingRegime());
    ATH_MSG_INFO("DerRunPars:: tierZeroTag: " << it->tierZeroTag());
  }

  
  
  ATH_MSG_INFO("************************************************************************");
  
  
  L1CaloRunParametersContainer::const_iterator id = runParameters->begin();
  for(;id!=runParameters->end();++id) {     
    ATH_MSG_INFO("RunParameters:: " << *id);
      ATH_MSG_INFO("RunParameters:: runType: " << id->runType());
      ATH_MSG_INFO("RunParameters:: runActionName " << id->runActionName());
      ATH_MSG_INFO("RunParameters:: readoutConfig " << id->readoutConfig());
  }
  
  
  ATH_MSG_INFO("************************************************************************");
  
  
  L1CaloPprChanStrategyContainer::const_iterator ir = pprChanStrategy->begin();
  for(;ir!=pprChanStrategy->end();++ir) {     
    ATH_MSG_INFO("PprChanStrategy:: " << *ir);
    ATH_MSG_INFO("PprChanStrategy:: strategy " << ir->strategy());
    ATH_MSG_INFO("PprChanStrategy:: code " << ir->code());
    ATH_MSG_INFO("PprChanStrategy::  timingRegime" << ir->timingRegime());
  }
  
  
  
   
  
  //pprDisabledChannel->dump();
  
  ATH_MSG_INFO("************************************************************************");
  
  ATH_MSG_INFO("ReadoutConfig(1) --> " << readoutConfig->readoutConfig(1));
  ATH_MSG_INFO("ReadoutConfig(1)::description --> " <<  readoutConfig->readoutConfig(1)->description());
  ATH_MSG_INFO("ReadoutConfig(1)::baselinePointer --> " <<  readoutConfig->readoutConfig(1)->baselinePointer());
  ATH_MSG_INFO("ReadoutConfig(1)::numFadcSlices --> " <<  readoutConfig->readoutConfig(1)->numFadcSlices());
  
  ATH_MSG_INFO("ReadoutConfig(2) --> " << readoutConfig->readoutConfig(2));
  ATH_MSG_INFO("ReadoutConfig(2)::description --> " <<  readoutConfig->readoutConfig(2)->description());
  ATH_MSG_INFO("ReadoutConfig(2)::baselinePointer --> " <<  readoutConfig->readoutConfig(2)->baselinePointer());
  ATH_MSG_INFO("ReadoutConfig(2)::numFadcSlices --> " <<  readoutConfig->readoutConfig(2)->numFadcSlices());
  
  ATH_MSG_INFO("ReadoutConfig(5) --> " << readoutConfig->readoutConfig(5));
  ATH_MSG_INFO("ReadoutConfig(5)::description --> " <<  readoutConfig->readoutConfig(5)->description());
  ATH_MSG_INFO("ReadoutConfig(5)::baselinePointer --> " <<  readoutConfig->readoutConfig(5)->baselinePointer());
  ATH_MSG_INFO("ReadoutConfig(5)::numFadcSlices --> " <<  readoutConfig->readoutConfig(5)->numFadcSlices());
  
  ATH_MSG_INFO("************************************************************************");
  
  if ( readoutConfigJSON->readoutConfigJSON(1) ) {
    
    ATH_MSG_INFO("ReadoutConfigJSON(1)::baselinePointer --> " << readoutConfigJSON->readoutConfigJSON(1)->baselinePointer());
    ATH_MSG_INFO("ReadoutConfigJSON(1)::numFadcSlices --> " << readoutConfigJSON->readoutConfigJSON(1)->numFadcSlices());
    ATH_MSG_INFO("ReadoutConfigJSON(1)::numFexSlices --> " << readoutConfigJSON->readoutConfigJSON(1)->numFexSlices());
    ATH_MSG_INFO("ReadoutConfigJSON(1)::l1aFexSlice --> " << readoutConfigJSON->readoutConfigJSON(1)->l1aFexSlice());
    ATH_MSG_INFO("ReadoutConfigJSON(1)::inputReadoutModeFex --> " << readoutConfigJSON->readoutConfigJSON(1)->inputReadoutModeFex());
    ATH_MSG_INFO("ReadoutConfigJSON(1)::readoutOffsetEfex --> " << readoutConfigJSON->readoutConfigJSON(1)->readoutOffsetEfex());
    ATH_MSG_INFO("ReadoutConfigJSON(1)::readoutOffsetGfex --> " << readoutConfigJSON->readoutConfigJSON(1)->readoutOffsetGfex());
    ATH_MSG_INFO("ReadoutConfigJSON(1)::readoutOffsetJfex --> " << readoutConfigJSON->readoutConfigJSON(1)->readoutOffsetJfex());
    ATH_MSG_INFO("ReadoutConfigJSON(1)::readoutOffsetPh1Topo --> " << readoutConfigJSON->readoutConfigJSON(1)->readoutOffsetPh1Topo());
    
    ATH_MSG_INFO("ReadoutConfigJSON(2)::baselinePointer --> " << readoutConfigJSON->readoutConfigJSON(2)->baselinePointer());
    ATH_MSG_INFO("ReadoutConfigJSON(2)::numFadcSlices --> " << readoutConfigJSON->readoutConfigJSON(2)->numFadcSlices());
    ATH_MSG_INFO("ReadoutConfigJSON(2)::numFexSlices --> " << readoutConfigJSON->readoutConfigJSON(2)->numFexSlices());
    ATH_MSG_INFO("ReadoutConfigJSON(2)::l1aFexSlice --> " << readoutConfigJSON->readoutConfigJSON(2)->l1aFexSlice());
    ATH_MSG_INFO("ReadoutConfigJSON(2)::inputReadoutModeFex --> " << readoutConfigJSON->readoutConfigJSON(2)->inputReadoutModeFex());
    ATH_MSG_INFO("ReadoutConfigJSON(2)::readoutOffsetEfex --> " << readoutConfigJSON->readoutConfigJSON(2)->readoutOffsetEfex());
    ATH_MSG_INFO("ReadoutConfigJSON(2)::readoutOffsetGfex --> " << readoutConfigJSON->readoutConfigJSON(2)->readoutOffsetGfex());
    ATH_MSG_INFO("ReadoutConfigJSON(2)::readoutOffsetJfex --> " << readoutConfigJSON->readoutConfigJSON(2)->readoutOffsetJfex());
    ATH_MSG_INFO("ReadoutConfigJSON(2)::readoutOffsetPh1Topo --> " << readoutConfigJSON->readoutConfigJSON(2)->readoutOffsetPh1Topo());

  }
  
  ATH_MSG_INFO("************************************************************************");

  ATH_MSG_INFO("PprChanDefaults: " <<   pprChanDefaults->pprChanDefaults(0)  );
  ATH_MSG_INFO("PprChanDefaults::channelId " <<   pprChanDefaults->pprChanDefaults(0)->channelId()  );
  ATH_MSG_INFO("PprChanDefaults::peakFinderCond " <<   pprChanDefaults->pprChanDefaults(0)->peakFinderCond()  );
  ATH_MSG_INFO("PprChanDefaults::decisionSource " <<   pprChanDefaults->pprChanDefaults(0)->decisionSource()  );
  ATH_MSG_INFO("PprChanDefaults::satOverride1 " <<   pprChanDefaults->pprChanDefaults(0)->satOverride1() );
  ATH_MSG_INFO("PprChanDefaults::sat80High " <<   pprChanDefaults->pprChanDefaults(0)->sat80High() );
  ATH_MSG_INFO("PprChanDefaults::sat80Low " <<   pprChanDefaults->pprChanDefaults(0)->sat80Low() );

  



  for (auto tt : * tts){

    ATH_MSG_INFO("************************************************************************");
    ATH_MSG_INFO("************************************************************************");
    
    ATH_MSG_INFO("Channel " << tt->coolId());

    ATH_MSG_INFO("************************************************************************");

    
    ATH_MSG_INFO("Disabled Towers --> " << disTowers->disabledTowers(tt->coolId()));
    ATH_MSG_INFO("Disabled Towers --> " << disTowers->disabledTowers(tt->coolId())->disabledBits());
    ATH_MSG_INFO("Dead Channel --> " << ppmDeadChannels->ppmDeadChannels(tt->coolId()));
    
    if  (ppmDeadChannels->ppmDeadChannels(tt->coolId())) {
      ATH_MSG_INFO("PpmDeadChannels::errorCode --> " <<   ppmDeadChannels-> ppmDeadChannels(tt->coolId())->errorCode()  );
      ATH_MSG_INFO("PpmDeadChannels::noiseCut --> " <<   ppmDeadChannels-> ppmDeadChannels(tt->coolId())->noiseCut()  );
      
    }
    
    
    if ( pprDisabledChannel->pprDisabledChannel(tt->coolId())){
      
      ATH_MSG_INFO("Disabled Channel::disabledBits --> " << pprDisabledChannel->pprDisabledChannel(tt->coolId())->disabledBits());
      ATH_MSG_INFO("Disabled Channel::deadErrorCode--> " << pprDisabledChannel->pprDisabledChannel(tt->coolId())->deadErrorCode());
      ATH_MSG_INFO("Disabled Channel::calibErrorCode--> " << pprDisabledChannel->pprDisabledChannel(tt->coolId())->calibErrorCode());
      ATH_MSG_INFO("Disabled Channel::noiseCut--> " << pprDisabledChannel->pprDisabledChannel(tt->coolId())->noiseCut());
      
      
    }
    
    
    ATH_MSG_INFO("************************************************************************");
    
      
      
    ATH_MSG_INFO("PprChanCalib::ErrorCode --> " <<   pprChanCalib->pprChanCalib(tt->coolId())->errorCode()  );
    ATH_MSG_INFO("PprChanCalib::PedMean --> " <<  pprChanCalib->pprChanCalib(tt->coolId())->pedMean()  );
    ATH_MSG_INFO("PprChanCalib::PedValue --> " <<  pprChanCalib->pprChanCalib(tt->coolId())->pedValue()  );
    ATH_MSG_INFO("PprChanCalib::extBcidThreshold--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->extBcidThreshold()  );
    ATH_MSG_INFO("PprChanCalib::satBcidThreshLow--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->satBcidThreshLow()  );
    ATH_MSG_INFO("PprChanCalib::satBcidThreshHigh--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->satBcidThreshHigh()  );
    ATH_MSG_INFO("PprChanCalib::satBcidLevel--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->satBcidLevel()  );
    ATH_MSG_INFO("PprChanCalib::bcidEnergyRangeLow--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->bcidEnergyRangeLow()  );
    ATH_MSG_INFO("PprChanCalib::bcidEnergyRangeHigh--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->bcidEnergyRangeHigh()  );
    ATH_MSG_INFO("PprChanCalib::firStartBit--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->firStartBit()  );
    ATH_MSG_INFO("PprChanCalib::firCoeff1--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->firCoeff1()  );
    ATH_MSG_INFO("PprChanCalib::firCoeff2--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->firCoeff2()  );
    ATH_MSG_INFO("PprChanCalib::firCoeff3--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->firCoeff3()  );
    ATH_MSG_INFO("PprChanCalib::firCoeff4--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->firCoeff4()  );
    ATH_MSG_INFO("PprChanCalib::firCoeff5--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->firCoeff5()  );
    ATH_MSG_INFO("PprChanCalib::lutCpStrategy--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->lutCpStrategy()  );
    ATH_MSG_INFO("PprChanCalib::lutCpOffset--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->lutCpOffset()  );
    ATH_MSG_INFO("PprChanCalib::lutCpNoiseCut--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->lutCpNoiseCut()  );
    ATH_MSG_INFO("PprChanCalib::lutCpSlope--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->lutCpSlope()  );
    ATH_MSG_INFO("PprChanCalib::lutCpPar1--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->lutCpPar1()  );
    ATH_MSG_INFO("PprChanCalib::lutCpPar2--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->lutCpPar2()  );
    ATH_MSG_INFO("PprChanCalib::lutCpPar3--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->lutCpPar3()  );
    ATH_MSG_INFO("PprChanCalib::lutCpPar4--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->lutCpPar4()  );
    ATH_MSG_INFO("PprChanCalib::lutCpScale--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->lutCpScale()  );
    ATH_MSG_INFO("PprChanCalib::lutJepStrategy--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->lutJepStrategy()  );
    ATH_MSG_INFO("PprChanCalib::lutJepOffset--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->lutJepOffset()  );
    ATH_MSG_INFO("PprChanCalib::lutJepNoiseCut--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->lutJepNoiseCut()  );
    ATH_MSG_INFO("PprChanCalib::lutJepSlope--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->lutJepSlope()  );
    ATH_MSG_INFO("PprChanCalib::lutJepPar1--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->lutJepPar1()  );
    ATH_MSG_INFO("PprChanCalib::lutJepPar2--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->lutJepPar2()  );
    ATH_MSG_INFO("PprChanCalib::lutJepPar3--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->lutJepPar3()  );
    ATH_MSG_INFO("PprChanCalib::lutJepPar4--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->lutJepPar4()  );
    ATH_MSG_INFO("PprChanCalib::lutJepScale--> " <<  pprChanCalib->pprChanCalib(tt->coolId())->lutJepScale()  );
    
    ATH_MSG_INFO("************************************************************************");
    

    ATH_MSG_INFO("PprCondRun2::PedMean --> " <<  pprConditionsRun2->pprConditions(tt->coolId())->pedMean()  );
    ATH_MSG_INFO("PprCondRun2::PedValue --> " <<  pprConditionsRun2->pprConditions(tt->coolId())->pedValue()  );
    ATH_MSG_INFO("PprCondRun2::extBcidThreshold--> " <<   pprConditionsRun2->pprConditions(tt->coolId())->extBcidThreshold()  );
    ATH_MSG_INFO("PprCondRun2::satBcidThreshLow--> " <<  pprConditionsRun2->pprConditions(tt->coolId())->satBcidThreshLow()  );
    ATH_MSG_INFO("PprCondRun2::satBcidThreshHigh--> " <<  pprConditionsRun2->pprConditions(tt->coolId())->satBcidThreshHigh()  );
    ATH_MSG_INFO("PprCondRun2::satBcidLevel--> " <<   pprConditionsRun2->pprConditions(tt->coolId())->satBcidLevel()  );
    ATH_MSG_INFO("PprCondRun2::bcidEnergyRangeLow--> " <<   pprConditionsRun2->pprConditions(tt->coolId())->bcidEnergyRangeLow()  );
    ATH_MSG_INFO("PprCondRun2::bcidEnergyRangeHigh--> " <<   pprConditionsRun2->pprConditions(tt->coolId())->bcidEnergyRangeHigh()  );
    ATH_MSG_INFO("PprCondRun2::firStartBit--> " <<   pprConditionsRun2->pprConditions(tt->coolId())->firStartBit()  );
    ATH_MSG_INFO("PprCondRun2::firCoefficients--> " <<  pprConditionsRun2->pprConditions(tt->coolId())->firCoefficients()  );
    ATH_MSG_INFO("PprCondRun2::lutCpStrategy--> " <<   pprConditionsRun2->pprConditions(tt->coolId())->lutCpStrategy()  );
    ATH_MSG_INFO("PprCondRun2::lutCpOffset--> " <<   pprConditionsRun2->pprConditions(tt->coolId())->lutCpOffset()  );
    ATH_MSG_INFO("PprCondRun2::lutCpNoiseCut--> " <<   pprConditionsRun2->pprConditions(tt->coolId())->lutCpNoiseCut()  );
    ATH_MSG_INFO("PprCondRun2::lutCpSlope--> " <<   pprConditionsRun2->pprConditions(tt->coolId())->lutCpSlope()  );
    ATH_MSG_INFO("PprCondRun2::lutCpPar1--> " <<  pprConditionsRun2->pprConditions(tt->coolId())->lutCpPar1()  );
    ATH_MSG_INFO("PprCondRun2::lutCpPar2--> " <<  pprConditionsRun2->pprConditions(tt->coolId())->lutCpPar2()  );
    ATH_MSG_INFO("PprCondRun2::lutCpPar3--> " <<  pprConditionsRun2->pprConditions(tt->coolId())->lutCpPar3()  );
    ATH_MSG_INFO("PprCondRun2::lutCpPar4--> " <<  pprConditionsRun2->pprConditions(tt->coolId())->lutCpPar4()  );
    ATH_MSG_INFO("PprCondRun2::lutJepStrategy--> " <<   pprConditionsRun2->pprConditions(tt->coolId())->lutJepStrategy()  );
    ATH_MSG_INFO("PprCondRun2::lutJepOffset--> " <<   pprConditionsRun2->pprConditions(tt->coolId())->lutJepOffset()  );
    ATH_MSG_INFO("PprCondRun2::lutJepNoiseCut--> " <<   pprConditionsRun2->pprConditions(tt->coolId())->lutJepNoiseCut()  );
    ATH_MSG_INFO("PprCondRun2::lutJepSlope--> " <<   pprConditionsRun2->pprConditions(tt->coolId())->lutJepSlope()  );
    ATH_MSG_INFO("PprCondRun2::lutJepPar1--> " <<  pprConditionsRun2->pprConditions(tt->coolId())->lutJepPar1()  );
    ATH_MSG_INFO("PprCondRun2::lutJepPar2--> " <<  pprConditionsRun2->pprConditions(tt->coolId())->lutJepPar2()  );
    ATH_MSG_INFO("PprCondRun2::lutJepPar3--> " <<  pprConditionsRun2->pprConditions(tt->coolId())->lutJepPar3()  );
    ATH_MSG_INFO("PprCondRun2::lutJepPar4--> " <<  pprConditionsRun2->pprConditions(tt->coolId())->lutJepPar4()  );
    
    
    
    
    ATH_MSG_INFO("************************************************************************");
    

    ATH_MSG_INFO("PpmFineTimeRefs::refValue --> " <<  ppmFineTimeRefs->ppmFineTimeRefs(tt->coolId())->refValue()  );
    ATH_MSG_INFO("PpmFineTimeRefs::criterion --> " <<   ppmFineTimeRefs->ppmFineTimeRefs(tt->coolId())->criterion()  );
    ATH_MSG_INFO("PpmFineTimeRefs::calibValue --> " <<   ppmFineTimeRefs->ppmFineTimeRefs(tt->coolId())->calibValue()  );
    

      

    
  


  }
  
  
  return StatusCode::SUCCESS;

  

}

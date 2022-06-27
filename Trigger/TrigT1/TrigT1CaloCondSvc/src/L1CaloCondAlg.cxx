// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "TrigT1CaloCondSvc/L1CaloCondAlg.h"


#include <iomanip>
#include "TrigT1CaloCalibConditions/IL1CaloPersistenceCapable.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"


L1CaloCondAlg :: L1CaloCondAlg ( const std::string &name,ISvcLocator *pSvcLocator): AthAlgorithm( name, pSvcLocator )    
										    
										    

{
 
}





StatusCode  L1CaloCondAlg:: initialize ()
{
  
  ATH_MSG_DEBUG( "initialize " << name() );                
  
 

  // Disabled Towers 
  ATH_CHECK( m_disabledTowers.initialize() );
  ATH_CHECK( m_disabledTowersContainer.initialize() );


  // Derived Run Pars
  ATH_CHECK(m_derivedRunPars.initialize() );
  ATH_CHECK(m_derivedRunParsContainer.initialize() );

  // Ppr Channel Calib 
  ATH_CHECK(m_pprChanCalibContainer.initialize() );

  // Cond Container
  ATH_CHECK(m_pprConditionsContainer.initialize() );


  // Ppr Defaults Calib
  ATH_CHECK(m_pprChanDefaults.initialize() );
  ATH_CHECK(m_pprChanDefaultsContainer.initialize() );

  // Ppr FineTime Refs
  ATH_CHECK(m_ppmFineTimeRefs.initialize() );
  ATH_CHECK(m_ppmFineTimeRefsContainer.initialize() );
  
  // Run Parameters
  ATH_CHECK(m_runParameters.initialize() );
  ATH_CHECK(m_runParametersContainer.initialize() );

  // PprChanStrategy
  ATH_CHECK(m_pprChanStrategy.initialize() );
  ATH_CHECK(m_pprChanStrategyContainer.initialize() );


  // Ppm Dead Channels 
  ATH_CHECK(m_ppmDeadChannels.initialize() );
  ATH_CHECK(m_ppmDeadChannelsContainer.initialize() );


  // Disabled Channel
  ATH_CHECK(m_pprDisabledChannelContainer.initialize() );

  // Readout Config
  ATH_CHECK( m_readoutConfig.initialize() );
  ATH_CHECK( m_readoutConfigContainer.initialize() );
  
  // Readout Config JSON (Run 3)
  ATH_CHECK( m_readoutConfigJSON.initialize() );
  ATH_CHECK( m_readoutConfigContainerJSON.initialize() );
 

  return StatusCode::SUCCESS;

}


StatusCode  L1CaloCondAlg:: execute ()
{
  
  ATH_MSG_DEBUG( "start execute " << name() ); 
  ATH_MSG_DEBUG("readConditions() ");

  


  // Defining condition container to be saved 
   
  SG::WriteCondHandle<L1CaloDisabledTowersContainer> writeHandleDisTowers{m_disabledTowersContainer};
  
  SG::WriteCondHandle<L1CaloDerivedRunParsContainer> writeHandleDerRunPars{m_derivedRunParsContainer};
  
  SG::WriteCondHandle<L1CaloPprChanCalibContainer> writeHandlePprChanCalib{m_pprChanCalibContainer};
  
  SG::WriteCondHandle<L1CaloPprChanDefaultsContainer> writeHandlePprChanDefaults{m_pprChanDefaultsContainer};
 
  SG::WriteCondHandle<L1CaloPpmFineTimeRefsContainer> writeHandlePpmFineTimeRefs{m_ppmFineTimeRefsContainer};
  
  SG::WriteCondHandle<L1CaloRunParametersContainer> writeHandleRunParameters{m_runParametersContainer};
  
  SG::WriteCondHandle<L1CaloPprChanStrategyContainer> writeHandlePprChanStrategy{m_pprChanStrategyContainer};
 
  SG::WriteCondHandle<L1CaloPpmDeadChannelsContainer> writeHandlePpmDeadChannels{m_ppmDeadChannelsContainer};
  
  SG::WriteCondHandle<L1CaloPprConditionsContainerRun2> writeHandlePprConditions{m_pprConditionsContainer};
  
  SG::WriteCondHandle<L1CaloPprDisabledChannelContainerRun2> writeHandlePprDisabledChannel{m_pprDisabledChannelContainer};
  
  SG::WriteCondHandle<L1CaloReadoutConfigContainer> writeHandleReadoutConfig{m_readoutConfigContainer};

  SG::WriteCondHandle<L1CaloReadoutConfigContainerJSON> writeHandleReadoutConfigJSON{m_readoutConfigContainerJSON};

 

  
  // Reading timing regime from database, and defining cool database folder for reading FIR coefficients 
  
  SG::ReadCondHandle<CondAttrListCollection> readHandleDerRunPars(m_derivedRunPars);
  const CondAttrListCollection* readCdo_DerRunPars(*readHandleDerRunPars);
  if (readCdo_DerRunPars == 0) {
     ATH_MSG_ERROR("Null pointer to the read conditions object");
     return StatusCode::FAILURE; 
   } 

  auto writeCdoDerRunPars = std::make_unique<L1CaloDerivedRunParsContainer>();
  
  std::map<std::string, const CondAttrListCollection*> condAttrListCollectionMapDerRunPars;
  if (readCdo_DerRunPars) {
     ATH_MSG_DEBUG("AttrListCollection for Derivated Run Parameters= "<<readCdo_DerRunPars);
     condAttrListCollectionMapDerRunPars[ m_derivedRunPars.key() ] = readCdo_DerRunPars;
     writeCdoDerRunPars->makeTransient(condAttrListCollectionMapDerRunPars);

   }

   std::string timingRegime ;
   if (m_timingRegime.empty()) { 
     timingRegime = std::cbegin(*writeCdoDerRunPars)->timingRegime();
   }
   else {
     timingRegime = m_timingRegime;
   }
   
   ATH_MSG_DEBUG("timing regime --> "<< timingRegime );

   
   // Strategy 
  
  SG::ReadCondHandle<CondAttrListCollection> readHandlePprChanStrategy(m_pprChanStrategy);
  const CondAttrListCollection* readCdo_PprChanStrategy(*readHandlePprChanStrategy);
  
  if (readCdo_PprChanStrategy == 0) {
     ATH_MSG_ERROR("Null pointer to the read conditions object");
     return StatusCode::FAILURE; 
  }
  auto writeCdoPprChanStrategy = std::make_unique<L1CaloPprChanStrategyContainer>();

  std::map<std::string, const CondAttrListCollection*> condAttrListCollectionMapPprChanStrategy;
  if (readCdo_PprChanStrategy) {
    ATH_MSG_DEBUG("AttrListCollection for Ppr Chan Strategy= "<<readCdo_PprChanStrategy);
    condAttrListCollectionMapPprChanStrategy[ m_pprChanStrategy.key() ] = readCdo_PprChanStrategy;
    writeCdoPprChanStrategy->makeTransient(condAttrListCollectionMapPprChanStrategy);
    
  }
   


  std::string strategy;


  if (m_strategy.empty()) {
    
    for(const auto& it: *writeCdoPprChanStrategy){
      if (it.timingRegime() == timingRegime){
	strategy = it.strategy();
	
      }
    }}

  else{ 
    strategy = m_strategy;
  } 
  
  ATH_MSG_DEBUG("strategy selected --> " << strategy);
  



  
    
  std::string keyPprChanCalib= "/TRIGGER/L1Calo/V2/Calibration/" + timingRegime  +"/PprChanCalib";
  SG::ReadCondHandleKey<CondAttrListCollection> pprChanCalib (keyPprChanCalib);
  
  ATH_CHECK(pprChanCalib.initialize() );

  


  SG::ReadCondHandle<CondAttrListCollection> readHandleDisTowers(m_disabledTowers);
  const CondAttrListCollection* readCdo_DisTowers(*readHandleDisTowers);
  
  SG::ReadCondHandle<CondAttrListCollection> readHandlePprChanCalib(pprChanCalib);
  const CondAttrListCollection* readCdo_PprChanCalib(*readHandlePprChanCalib);

  SG::ReadCondHandle<CondAttrListCollection> readHandlePprChanDefaults(m_pprChanDefaults);
  const CondAttrListCollection* readCdo_PprChanDefaults(*readHandlePprChanDefaults);

  SG::ReadCondHandle<CondAttrListCollection> readHandlePpmFineTimeRefs(m_ppmFineTimeRefs);
  const CondAttrListCollection* readCdo_PpmFineTimeRefs(*readHandlePpmFineTimeRefs);

  SG::ReadCondHandle<CondAttrListCollection> readHandleRunParameters(m_runParameters);
  const CondAttrListCollection* readCdo_RunParameters(*readHandleRunParameters);
 
  SG::ReadCondHandle<CondAttrListCollection> readHandlePpmDeadChannels(m_ppmDeadChannels);
  const CondAttrListCollection* readCdo_PpmDeadChannels(*readHandlePpmDeadChannels);

  SG::ReadCondHandle<CondAttrListCollection> readHandleReadoutConfig(m_readoutConfig);
  const CondAttrListCollection* readCdo_ReadoutConfig(*readHandleReadoutConfig);


  SG::ReadCondHandle<CondAttrListCollection> readHandleReadoutConfigJSON(m_readoutConfigJSON);
  const CondAttrListCollection* readCdo_ReadoutConfigJSON(*readHandleReadoutConfigJSON);
  CondAttrListCollection::const_iterator itr;
  

   if (readCdo_DisTowers == 0) {
     ATH_MSG_ERROR("Null pointer to the read conditions object");
     return StatusCode::FAILURE; 
   } 
   
     
   if (readCdo_PprChanCalib == 0) {
     ATH_MSG_ERROR("Null pointer to the read conditions object");
     return StatusCode::FAILURE; 
   } 

   if (readCdo_PprChanDefaults == 0) {
     ATH_MSG_ERROR("Null pointer to the read conditions object");
     return StatusCode::FAILURE; 
   } 

   if (readCdo_PpmFineTimeRefs == 0) {
     ATH_MSG_ERROR("Null pointer to the read conditions object");
     return StatusCode::FAILURE; 
   } 


   if (readCdo_RunParameters == 0) {
     ATH_MSG_ERROR("Null pointer to the read conditions object");
     return StatusCode::FAILURE; 
   } 

   if (readCdo_PpmDeadChannels == 0) {
     ATH_MSG_ERROR("Null pointer to the read conditions object");
     return StatusCode::FAILURE; 
   }

   
   if (readCdo_ReadoutConfig == 0) {
     ATH_MSG_ERROR("Null pointer to the read conditions object");
     return StatusCode::FAILURE; 
   }

  




   ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandleDisTowers.fullKey() << " readCdo_DisTower->size()= " << readCdo_DisTowers->size());
   ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandleDerRunPars.fullKey() << " readCdo_DerRunPars->size()= " <<  readCdo_DerRunPars->size());
   ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandlePprChanCalib.fullKey() << " readCdo_PprChanCalib->size()= " <<  readCdo_PprChanCalib->size());
   ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandlePprChanDefaults.fullKey() << " readCdo_PprChanDefaults->size()= " <<  readCdo_PprChanDefaults->size());
   ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandlePpmFineTimeRefs.fullKey() << " readCdo_PpmFineTimeRefs->size()= " <<  readCdo_PpmFineTimeRefs->size());
   ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandleRunParameters.fullKey() << " readCdo_RunParameters->size()= " <<  readCdo_RunParameters->size());
   ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandlePprChanStrategy.fullKey() << " readCdo_PprChanStrategy->size()= " <<  readCdo_PprChanStrategy->size());
   ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandlePpmDeadChannels.fullKey() << " readCdo_PpmDeadChannels->size()= " <<  readCdo_PpmDeadChannels->size());
   ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandleReadoutConfig.fullKey() << " readCdo_ReadoutConfig->size()= " <<  readCdo_ReadoutConfig->size());
   ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandleReadoutConfigJSON.fullKey() << " readCdo_ReadoutConfigJSON->size()= " <<  readCdo_ReadoutConfigJSON->size());

   


   auto writeCdoDisTowers = std::make_unique<L1CaloDisabledTowersContainer>();
   auto writeCdoPprChanCalib = std::make_unique<L1CaloPprChanCalibContainer>();
   auto writeCdoPprChanDefaults = std::make_unique<L1CaloPprChanDefaultsContainer>();
   auto writeCdoPpmFineTimeRefs = std::make_unique<L1CaloPpmFineTimeRefsContainer>();
   auto writeCdoRunParameters = std::make_unique<L1CaloRunParametersContainer>();
   auto writeCdoPpmDeadChannels = std::make_unique<L1CaloPpmDeadChannelsContainer>();
   auto writeCdoPprConditions = std::make_unique<L1CaloPprConditionsContainerRun2>();
   auto writeCdoPprDisabledChannel = std::make_unique<L1CaloPprDisabledChannelContainerRun2>();
   auto writeCdoReadoutConfig = std::make_unique<L1CaloReadoutConfigContainer>();
   auto writeCdoReadoutConfigJSON = std::make_unique<L1CaloReadoutConfigContainerJSON>();


  

   // Filling the condition container
   std::map<std::string, const CondAttrListCollection*> condAttrListCollectionMapDisTowers;
   if (readCdo_DisTowers) {
     ATH_MSG_DEBUG("AttrListCollection for Disabled Towers= "<<readCdo_DisTowers);
     condAttrListCollectionMapDisTowers[ m_disabledTowers.key() ] = readCdo_DisTowers;
     writeCdoDisTowers->makeTransient(condAttrListCollectionMapDisTowers);
   }

   
   std::map<std::string, const CondAttrListCollection*> condAttrListCollectionMapPprChanCalib;
   if (readCdo_PprChanCalib) {
     ATH_MSG_DEBUG("AttrListCollection for Ppr Channel Calib = "<<readCdo_PprChanCalib);
     condAttrListCollectionMapPprChanCalib[keyPprChanCalib] = readCdo_PprChanCalib;
     writeCdoPprChanCalib->makeTransient(condAttrListCollectionMapPprChanCalib);
     
   }




   std::map<std::string, const CondAttrListCollection*> condAttrListCollectionMapPprChanDefaults;
   if (readCdo_PprChanDefaults) {
     ATH_MSG_DEBUG("AttrListCollection for Ppr Channel Defaults = "<<readCdo_PprChanDefaults);
     condAttrListCollectionMapPprChanDefaults[ m_pprChanDefaults.key() ] = readCdo_PprChanDefaults;
     writeCdoPprChanDefaults->makeTransient(condAttrListCollectionMapPprChanDefaults);

   }


   std::map<std::string, const CondAttrListCollection*> condAttrListCollectionMapPpmFineTimeRefs;
   if (readCdo_PpmFineTimeRefs) {
     ATH_MSG_DEBUG("AttrListCollection for Ppm FineTime Refs = "<<readCdo_PpmFineTimeRefs);
     condAttrListCollectionMapPpmFineTimeRefs[ m_ppmFineTimeRefs.key() ] = readCdo_PpmFineTimeRefs;
     writeCdoPpmFineTimeRefs->makeTransient(condAttrListCollectionMapPpmFineTimeRefs);

   }

   
  std::map<std::string, const CondAttrListCollection*> condAttrListCollectionMapRunParameters;
  if (readCdo_RunParameters) {
     ATH_MSG_DEBUG("AttrListCollection for Run Parameters = "<<readCdo_RunParameters);
     condAttrListCollectionMapRunParameters[ m_runParameters.key() ] = readCdo_RunParameters;
     writeCdoRunParameters->makeTransient(condAttrListCollectionMapRunParameters);

   }

   
   std::map<std::string, const CondAttrListCollection*> condAttrListCollectionMapPpmDeadChannels;
   if (readCdo_PpmDeadChannels) {
     ATH_MSG_DEBUG("AttrListCollection for Ppm Dead Channels= "<<readCdo_PpmDeadChannels);
     condAttrListCollectionMapPpmDeadChannels[ m_ppmDeadChannels.key() ] = readCdo_PpmDeadChannels;
     writeCdoPpmDeadChannels->makeTransient(condAttrListCollectionMapPpmDeadChannels);

   }

   std::map<std::string, const CondAttrListCollection*> condAttrListCollectionMapReadoutConfig;
   if (readCdo_ReadoutConfig) {
     ATH_MSG_DEBUG("AttrListCollection for ReadoutConfig= "<<readCdo_ReadoutConfig);
     condAttrListCollectionMapReadoutConfig[  m_readoutConfig.key() ] = readCdo_ReadoutConfig;
     writeCdoReadoutConfig->makeTransient(condAttrListCollectionMapReadoutConfig);

   }

   std::map<std::string, const CondAttrListCollection*> condAttrListCollectionMapReadoutConfigJSON;
   if (readCdo_ReadoutConfigJSON) {
     ATH_MSG_DEBUG("AttrListCollection for ReadoutConfigJSON= "<<readCdo_ReadoutConfigJSON);
     condAttrListCollectionMapReadoutConfigJSON[ m_readoutConfigJSON.key() ] = readCdo_ReadoutConfigJSON;
     writeCdoReadoutConfigJSON->makeTransient(condAttrListCollectionMapReadoutConfigJSON);

   }
   
   // Filling DisableChannels Contianer 
   std::map<std::string, const CondAttrListCollection*> condAttrListCollectionMapDisableChannels;
   condAttrListCollectionMapDisableChannels[keyPprChanCalib] = readCdo_PprChanCalib;
   condAttrListCollectionMapDisableChannels[ m_disabledTowers.key() ] = readCdo_DisTowers;
   condAttrListCollectionMapDisableChannels[ m_ppmDeadChannels.key() ] = readCdo_PpmDeadChannels;
   writeCdoPprDisabledChannel->makeTransient(condAttrListCollectionMapDisableChannels);


   // Event range definition 
   
   EventIDRange range_DisTowers;
   if ( !readHandleDisTowers.range(range_DisTowers) ) {
     ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandleDisTowers.key());
     return StatusCode::FAILURE;
   } 
   
   ATH_MSG_DEBUG("Range of input is Disable Towers" << range_DisTowers);
   
   EventIDRange range_DerRunPars;
   if ( !readHandleDerRunPars.range(range_DerRunPars) ) {
     ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandleDerRunPars.key());
     return StatusCode::FAILURE;
   } 
   
   ATH_MSG_DEBUG("Range of input is DerRunPars" << range_DerRunPars);
   
   
   EventIDRange range_PprChanCalib;
   if ( !readHandlePprChanCalib.range(range_PprChanCalib) ) {
     ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandlePprChanCalib.key());
     return StatusCode::FAILURE;
   } 
   
   ATH_MSG_DEBUG("Range of input is PprChanCalib" << range_PprChanCalib);


   

   EventIDRange range_PprChanDefaults;
   if ( !readHandlePprChanDefaults.range(range_PprChanDefaults) ) {
     ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandlePprChanDefaults.key());
     return StatusCode::FAILURE;
   } 
   
   ATH_MSG_DEBUG("Range of input is PprChanDefaults" << range_PprChanDefaults);
   writeHandlePprConditions.addDependency( range_PprChanDefaults );
   

   EventIDRange range_PpmFineTimeRefs;
   if ( !readHandlePpmFineTimeRefs.range(range_PpmFineTimeRefs) ) {
     ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandlePpmFineTimeRefs.key());
     return StatusCode::FAILURE;
   } 
   
   ATH_MSG_DEBUG("Range of input is PpmFineTimeRefs" << range_PpmFineTimeRefs);
   


   EventIDRange range_RunParameters;
   if ( !readHandleRunParameters.range(range_RunParameters) ) {
     ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandleRunParameters.key());
     return StatusCode::FAILURE;
   } 
   
   ATH_MSG_DEBUG("Range of input is RunParameters" << range_RunParameters);
   

   EventIDRange range_PprChanStrategy;
   if ( !readHandlePprChanStrategy.range(range_PprChanStrategy) ) {
     ATH_MSG_DEBUG("Failed to retrieve validity range for " << readHandlePprChanStrategy.key());
     return StatusCode::FAILURE;
   } 
   
   ATH_MSG_DEBUG("Range of input is PprChanStrategy" << range_PprChanStrategy);
   

   EventIDRange range_PpmDeadChannels;
   if ( !readHandlePpmDeadChannels.range(range_PpmDeadChannels) ) {
     ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandlePpmDeadChannels.key());
     return StatusCode::FAILURE;
   } 
   ATH_MSG_DEBUG("Range of input is PpmDeadChannels" << range_PpmDeadChannels);



   EventIDRange range_ReadoutConfig;
   if ( !readHandleReadoutConfig.range(range_ReadoutConfig) ) {
     ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandleReadoutConfig.key());
     return StatusCode::FAILURE;
   } 
   ATH_MSG_DEBUG("Range of input is Readout Config" << range_ReadoutConfig);


   EventIDRange range_ReadoutConfigJSON;
   if ( !readHandleReadoutConfigJSON.range(range_ReadoutConfigJSON) ) {
     ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandleReadoutConfigJSON.key());
     return StatusCode::FAILURE;
   } 
   ATH_MSG_DEBUG("Range of input is Readout Config JSON" << range_ReadoutConfigJSON);



   // Adding PprDisabledChannel dependencies
   writeHandlePprDisabledChannel.addDependency(readHandlePpmDeadChannels);
   writeHandlePprDisabledChannel.addDependency(readHandlePprChanCalib);
   writeHandlePprDisabledChannel.addDependency(readHandleDisTowers);
   
  

   // Saving condition containers   

   
   if (writeHandleDisTowers.record(range_DisTowers, std::move(writeCdoDisTowers)).isFailure()) {
     ATH_MSG_FATAL("Could not record " << writeHandleDisTowers.key() 
		   << " with EventRange " << range_DisTowers
                   << " into Conditions Store");
     return StatusCode::FAILURE;
   }         

   
   if (writeHandleDerRunPars.record(range_DerRunPars, std::move(writeCdoDerRunPars)).isFailure()) {
     ATH_MSG_FATAL("Could not record " << writeHandleDerRunPars.key() 
		   << " with EventRange " << range_DerRunPars
                   << " into Conditions Store");
     return StatusCode::FAILURE;
   }         

   
   if (writeHandlePprChanCalib.record(range_PprChanCalib, std::move(writeCdoPprChanCalib)).isFailure()) {
     ATH_MSG_FATAL("Could not record " << writeHandlePprChanCalib.key() 
		   << " with EventRange " << range_PprChanCalib
                   << " into Conditions Store");
     return StatusCode::FAILURE;
   }         




   if (writeHandlePprChanDefaults.record(range_PprChanDefaults, std::move(writeCdoPprChanDefaults)).isFailure()) {
     ATH_MSG_FATAL("Could not record " << writeHandlePprChanDefaults.key() 
		   << " with EventRange " << range_PprChanDefaults
                   << " into Conditions Store");
     return StatusCode::FAILURE;
   }         


   if (writeHandlePpmFineTimeRefs.record(range_PpmFineTimeRefs, std::move(writeCdoPpmFineTimeRefs)).isFailure()) {
     ATH_MSG_FATAL("Could not record " << writeHandlePpmFineTimeRefs.key() 
		   << " with EventRange " << range_PpmFineTimeRefs
                   << " into Conditions Store");
     return StatusCode::FAILURE;
   }         



   if (writeHandleRunParameters.record(range_RunParameters, std::move(writeCdoRunParameters)).isFailure()) {
     ATH_MSG_FATAL("Could not record " << writeHandleRunParameters.key() 
		   << " with EventRange " << range_RunParameters
                   << " into Conditions Store");
     return StatusCode::FAILURE;
   }         

   
   
   if (writeHandlePprChanStrategy.record(range_PprChanStrategy, std::move(writeCdoPprChanStrategy)).isFailure()) {
       ATH_MSG_FATAL("Could not record " << writeHandlePprChanStrategy.key() 
		     << " with EventRange " << range_PprChanStrategy
		     << " into Conditions Store");
       return StatusCode::FAILURE;
     }         
     
  

   if (writeHandlePpmDeadChannels.record(range_PpmDeadChannels, std::move(writeCdoPpmDeadChannels)).isFailure()) {
       ATH_MSG_FATAL("Could not record " << writeHandlePpmDeadChannels.key() 
		     << " with EventRange " << range_PpmDeadChannels
		     << " into Conditions Store");
       return StatusCode::FAILURE;
     }         
     

   if (writeHandleReadoutConfig.record(range_ReadoutConfig, std::move(writeCdoReadoutConfig)).isFailure()) {
       ATH_MSG_FATAL("Could not record " << writeHandleReadoutConfig.key() 
		     << " with EventRange " << range_ReadoutConfig
		     << " into Conditions Store");
       return StatusCode::FAILURE;
     }         
     


   if (writeHandleReadoutConfigJSON.record(range_ReadoutConfigJSON, std::move(writeCdoReadoutConfigJSON)).isFailure()) {
       ATH_MSG_FATAL("Could not record " << writeHandleReadoutConfigJSON.key() 
		     << " with EventRange " << range_ReadoutConfigJSON
		     << " into Conditions Store");
       return StatusCode::FAILURE;
     }         
     



   if (writeHandlePprDisabledChannel.record(std::move(writeCdoPprDisabledChannel)).isFailure()) {
     ATH_MSG_FATAL("Could not record " << writeHandlePprDisabledChannel.key() 
		   << " into Conditions Store");
     return StatusCode::FAILURE;
   }         
     

   // Saving L1CaloConditionContianerRun2 depending on the strategy selected (to read FIR coefficients)
   
   
   std::map<std::string, const CondAttrListCollection*> condAttrListCollectionMapCond;
   if (!strategy.empty()){
     

      std::string keyPprChanCommon = "/TRIGGER/L1Calo/V2/Calibration/" + timingRegime  +"/PprChanCommon";
      SG::ReadCondHandleKey<CondAttrListCollection> pprChanCommon (keyPprChanCommon);
      
      ATH_CHECK(pprChanCommon.initialize() );
  
      
      std::string keyPprChanStrategy =  "/TRIGGER/L1Calo/V2/Calibration/" + timingRegime + "/PprChan" + strategy;
      SG::ReadCondHandleKey<CondAttrListCollection> pprChanStrategy (keyPprChanStrategy);
  
      ATH_CHECK(pprChanStrategy.initialize() );
  
      ATH_MSG_DEBUG("Entering to !strategy.empty() option --> strategy " << strategy);
      ATH_MSG_DEBUG("PprChanCommon folder " << keyPprChanCommon );
      ATH_MSG_DEBUG("PprChanStrategy folder " << keyPprChanStrategy);



      SG::ReadCondHandle<CondAttrListCollection> readHandlePprChanCommon(pprChanCommon);
      const CondAttrListCollection* readCdo_PprChanCommon(*readHandlePprChanCommon);
      
      SG::ReadCondHandle<CondAttrListCollection> readHandlePprChanStrategy(pprChanStrategy);
      const CondAttrListCollection* readCdo_PprChanStrategy(*readHandlePprChanStrategy);
      
      if (readCdo_PprChanCommon == 0) {
	ATH_MSG_ERROR("Null pointer to the read conditions object");
	return StatusCode::FAILURE; 
      }
      
      if (readCdo_PprChanStrategy == 0) {
	ATH_MSG_ERROR("Null pointer to the read conditions object");
	return StatusCode::FAILURE; 
      }
      
      
      ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandlePprChanCommon.fullKey() << " readCdo_PprChanCommon->size()= " <<  readCdo_PprChanCommon->size());
      ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandlePprChanStrategy.fullKey() << " readCdo_PprChanMu->size()= " <<  readCdo_PprChanStrategy->size());

      condAttrListCollectionMapCond[ keyPprChanCommon ] = readCdo_PprChanCommon;
      condAttrListCollectionMapCond[ keyPprChanStrategy ] = readCdo_PprChanStrategy;
      condAttrListCollectionMapCond[ m_pprChanDefaults.key() ] = readCdo_PprChanDefaults;
      
      writeCdoPprConditions->makeTransient(condAttrListCollectionMapCond);
      
      
      EventIDRange range_PprChanCommon;
      if ( !readHandlePprChanCommon.range(range_PprChanCommon) ) {
	ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandlePprChanCommon.key());
	return StatusCode::FAILURE;
      } 
      
      ATH_MSG_DEBUG("Range of input is PprChanCommon" << range_PprChanCommon);
      writeHandlePprConditions.addDependency(range_PprChanCommon);


      EventIDRange range_PprChanStrategy;
      if ( !readHandlePprChanStrategy.range(range_PprChanStrategy) ) {
	ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandlePprChanStrategy.key());
	return StatusCode::FAILURE;
      } 
      
      ATH_MSG_DEBUG("Range of input is PprChanMu" << range_PprChanStrategy);
      writeHandlePprConditions.addDependency(range_PprChanStrategy);

      
     
    }
    else {
      
      
    

      condAttrListCollectionMapCond[ keyPprChanCalib ] = readCdo_PprChanCalib;
      condAttrListCollectionMapCond[ m_pprChanDefaults.key() ] = readCdo_PprChanDefaults;
      writeCdoPprConditions->makeTransient(condAttrListCollectionMapCond);
      writeHandlePprConditions.addDependency(range_PprChanCalib);
      
      ATH_MSG_DEBUG("Entering to strategy.empty() option");
      ATH_MSG_DEBUG("PprChanCalib folder " << keyPprChanCalib );
      

    }
    



   
   if (writeHandlePprConditions.record(std::move(writeCdoPprConditions)).isFailure()) {
     ATH_MSG_FATAL("Could not record " << writeHandlePprConditions.key() 
		   << " into Conditions Store");
     return StatusCode::FAILURE;
   }         
      





  
   ATH_MSG_DEBUG("recorded new " <<  writeHandleDisTowers.key()  << " with range " << writeHandleDisTowers.getRange() << " into Conditions Store");
   ATH_MSG_DEBUG("recorded new " <<  writeHandleDerRunPars.key()  << " with range " << writeHandleDerRunPars.getRange() << " into Conditions Store");
   ATH_MSG_DEBUG("recorded new " <<  writeHandlePprChanCalib.key()  << " with range " << writeHandlePprChanCalib.getRange() << " into Conditions Store");
   ATH_MSG_DEBUG("recorded new " <<  writeHandlePprChanDefaults.key()  << " with range " << writeHandlePprChanDefaults.getRange() << " into Conditions Store");
   ATH_MSG_DEBUG("recorded new " <<  writeHandlePpmFineTimeRefs.key()  << " with range " << writeHandlePpmFineTimeRefs.getRange() << " into Conditions Store");
   ATH_MSG_DEBUG("recorded new " <<  writeHandleRunParameters.key()  << " with range " << writeHandleRunParameters.getRange() << " into Conditions Store");
   ATH_MSG_DEBUG("recorded new " <<  writeHandlePprChanStrategy.key()  << " with range " << writeHandlePprChanStrategy.getRange() << " into Conditions Store");
   ATH_MSG_DEBUG("recorded new " <<  writeHandlePpmDeadChannels.key()  << " with range " << writeHandlePpmDeadChannels.getRange() << " into Conditions Store");
   ATH_MSG_DEBUG("recorded new " <<  writeHandlePprConditions.key()  << " with range " << writeHandlePprConditions.getRange() << " into Conditions Store");
   ATH_MSG_DEBUG("recorded new " <<  writeHandleReadoutConfig.key()  << " with range " << writeHandleReadoutConfig.getRange() << " into Conditions Store");
   ATH_MSG_DEBUG("recorded new " <<  writeHandleReadoutConfigJSON.key()  << " with range " << writeHandleReadoutConfigJSON.getRange() << " into Conditions Store");
   ATH_MSG_DEBUG("recorded new " <<  writeHandlePprDisabledChannel.key()  << " with range " << writeHandlePprDisabledChannel.getRange() << " into Conditions Store");



   return StatusCode::SUCCESS;

}

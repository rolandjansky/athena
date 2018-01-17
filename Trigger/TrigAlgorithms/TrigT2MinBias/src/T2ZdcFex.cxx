/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT2MinBias/T2ZdcFex.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrigT2CaloCommon/ITrigDataAccess.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "ZdcIdentifier/ZdcID.h"
#include "ZdcConditions/ZdcCablingService.h"
#include "xAODTrigMinBias/TrigT2ZdcSignals.h"


T2ZdcFex::T2ZdcFex(const std::string &name, ISvcLocator* pSvcLocator): 
  HLT::AllTEAlgo(name, pSvcLocator),
  m_timerLoadColl(nullptr), m_timerAlg(nullptr), m_timerSave(nullptr),
  m_data("TrigDataAccess/TrigDataAccess"),
  m_zdcID(nullptr),
  m_triggerEnergies(xAOD::TrigT2ZdcSignals::NUM_ZDC,0.),
  m_triggerTimes(xAOD::TrigT2ZdcSignals::NUM_ZDC,0.),
  m_triggerEntries(xAOD::TrigT2ZdcSignals::NUM_ZDC,0.),
  m_useCachedResult(false),
  m_zdcSignals(nullptr),
  m_cachedTE(nullptr) {
  declareProperty("TrigDataAccess",m_data,"Data Access for LVL2 Calo Algorithms");
  declareProperty("ZdcEnRecoOption",  m_ZdcEnRecoOpt=0);
  declareProperty("ZdcTimeRecoOption",  m_ZdcTimeRecoOpt=0);
  declareProperty("ZcdEnThreshold" , m_en_threshold=0);
  declareProperty("ZcdTimeThreshold", m_time_threshold=0);
  declareProperty("ZcdChiThreshold" , m_chi_threshold=0);
  declareMonitoredStdContainer("TriggerEnergies", m_triggerEnergies); // Energies deposited in each counter.
  declareMonitoredStdContainer("TriggerTimes", m_triggerTimes); // Relative times of each of the triggers.
}

//--------------------------------------------------------------------------------------

HLT::ErrorCode T2ZdcFex::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
                                    unsigned int type_out) {
  // Caching.
  // First check whether we executed this instance before:
  if(m_useCachedResult) {
    ATH_MSG_DEBUG("Executing " << name() << " in cached mode");
    
    // Get all input TEs (for seeding relation of navigation structure)
    HLT::TEVec allTEs;
    std::vector<HLT::TEVec>::const_iterator itEnd = tes_in.end();
    for( std::vector<HLT::TEVec>::const_iterator it = tes_in.begin(); it != itEnd; ++it) {
      
      HLT::TEVec::const_iterator inner_itEnd = (*it).end();
      for (HLT::TEVec::const_iterator inner_it = (*it).begin();
           inner_it != inner_itEnd; ++inner_it) {
        allTEs.push_back(*inner_it);
      }
    }
    
    // Create an output TE seeded by the inputs
    HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);
    outputTE->setActiveState(true);
    
    // Save (cached) feature to output TE:
    m_config->getNavigation()->copyAllFeatures( m_cachedTE, outputTE );
    
    return HLT::OK;
  }
  
  ATH_MSG_DEBUG("Executing this T2ZdcFex as " << name());
  
  // start monitoring
  beforeExecMonitors().ignore();

  if(timerSvc()) 
    m_timerLoadColl->start();
  
  // Reset Iterators
  m_zBegin=m_zEnd;
  
  if( m_data->LoadZdcCollection(m_zBegin,m_zEnd).isFailure() ){
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::BAD_JOB_SETUP);
    //    return StatusCode::FAILURE;
  }
  
  if(timerSvc()){
    m_timerLoadColl->stop();
    m_timerAlg->start();
  }

  // Clear the variables which hold the processed signals.
  m_triggerEnergies.clear();
  m_triggerEnergies.resize(xAOD::TrigT2ZdcSignals::NUM_ZDC,0);
  m_triggerTimes.clear();
  m_triggerTimes.resize(xAOD::TrigT2ZdcSignals::NUM_ZDC,0);
  m_triggerEntries.clear();
  m_triggerEntries.resize(xAOD::TrigT2ZdcSignals::NUM_ZDC,0.);

  for( m_zt = m_zBegin; m_zt!=m_zEnd; ++m_zt){ // loop on ZdcRawChannelCollection iterators
    const ZdcRawChannel* zdc = (*m_zt);
    const Identifier& id = zdc->identify();
    // type = 0 (1 is pixel)
    // module = 0,1,2,3
    // side = -1 (C), 1 (A)
    // channel = 0
    int type = m_zdcID->type(id);
    if(type!=0) continue; // dont consider pixel
    int side =  m_zdcID->side(id);
    int module =  m_zdcID->module(id);
    //int m_channel =  m_zdcID->channel(id); //Not currently used
    
    int my_module = ((1.-side)/2)*4 + module;
    
    float energy = zdc->getEnergy(m_ZdcEnRecoOpt);
    float time = zdc->getTime(m_ZdcTimeRecoOpt);
    float chi = zdc->getChi(m_ZdcEnRecoOpt);
    if(energy > m_en_threshold && (chi < m_chi_threshold || m_chi_threshold<0 ) ){
      ++m_triggerEntries[my_module];
      m_triggerEnergies[my_module] += energy;
      m_triggerTimes[my_module] = time/m_triggerEntries[my_module] + 
	      m_triggerTimes[my_module]*(1.-(1./m_triggerEntries[my_module]));
    }    
  }

  if( timerSvc() ){
    m_timerAlg->stop();
    m_timerSave->start();
  }
  

  // Create the T2ZdcSignals output object.
  m_zdcSignals = new xAOD::TrigT2ZdcSignals;
  m_zdcSignals->makePrivateStore();
  m_zdcSignals->setTriggerEnergies(m_triggerEnergies);
  m_zdcSignals->setTriggerTimes(m_triggerTimes);

  // A vector of trigger elements is passed into this function, where
  // for each trigger element type provided there is a vector of
  // trigger elements.  For example, L1_MBTS_1 L2_MBTS_2 L3_MBTS_3 are
  // each a vector of one element.  Therefore start by concatenating
  // the trigger elements.
  HLT::TEVec allTEs;
  std::vector<HLT::TEVec>::const_iterator itEnd = tes_in.end();

  for(std::vector<HLT::TEVec>::const_iterator it = tes_in.begin(); it != itEnd; ++it) { 

    HLT::TEVec::const_iterator inner_itEnd = (*it).end();
    for(HLT::TEVec::const_iterator inner_it = (*it).begin(); inner_it != inner_itEnd; ++inner_it ){
      ATH_MSG_DEBUG("Creating TE seeded from input TE " << (*inner_it)->getId());
      allTEs.push_back(*inner_it);
    }
  }
  
  // Create an output TE seeded by the inputs
  HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);
  outputTE->setActiveState(true);

  HLT::ErrorCode hltStatus = attachFeature(outputTE, m_zdcSignals, "zdcsignals");
  if(hltStatus != HLT::OK) {
    ATH_MSG_ERROR("Write of TrigEMCluster into outputTE failed");
    return hltStatus;
  }
  
  if( timerSvc() ) 
    m_timerSave->stop();

  // Cache the TE in case this Fex is called again in this event.
  m_useCachedResult = true;
  m_cachedTE = outputTE;
  
  // Stop monitoring
  afterExecMonitors().ignore();
  
  return HLT::OK;
}

//--------------------------------------------------------------------------------------

HLT::ErrorCode T2ZdcFex::hltInitialize() {
  
  ATH_MSG_INFO("in T2ZdcFex::initialize()");
  
  if(m_data.retrieve().isFailure()) {
    ATH_MSG_ERROR("Could not get m_data");
    return HLT::BAD_JOB_SETUP;
  }

  StoreGateSvc* detStore(0);
  if ( service("DetectorStore",detStore).isFailure() ) {
    ATH_MSG_ERROR("Could not get detStore");
    return HLT::BAD_JOB_SETUP;
  }
  
  const ZdcID* zdcID = 0;
  if ( detStore->retrieve( zdcID ).isFailure() )  {
    //if ( detSvc()->retrieve( zdcID ).isFailure() )  {
    ATH_MSG_ERROR("Could not get ZdcIDs");
    return HLT::BAD_JOB_SETUP;
  }
  
  m_zdcID = zdcID;
  ZdcCablingService::getInstance()->setZdcID(m_zdcID);
  
  // Create timers
  if(timerSvc()) {
    m_timerLoadColl  = addTimer("LoadColl");
    m_timerAlg       = addTimer("Algorithm");
    m_timerSave      = addTimer("Saving");
  }
  
  return HLT::OK;
}

//--------------------------------------------------------------------------------------

HLT::ErrorCode T2ZdcFex::hltFinalize() {
  return HLT::OK;
}

//--------------------------------------------------------------------------------------
HLT::ErrorCode T2ZdcFex::hltEndEvent() {
    m_useCachedResult = false;
    m_zdcSignals = 0;
    m_cachedTE=0;
    return HLT::OK;
}

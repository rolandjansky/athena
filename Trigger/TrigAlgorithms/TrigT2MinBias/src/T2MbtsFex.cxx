/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT2MinBias/T2MbtsFex.h"
#include "xAODTrigMinBias/TrigT2MbtsBits.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsAuxContainer.h"
#include "TileIdentifier/TileTBID.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include <cstdlib>
#include <bitset>

T2MbtsFex::T2MbtsFex(const std::string &name, ISvcLocator* pSvcLocator): HLT::AllTEAlgo(name, pSvcLocator),
									 T2MbtsUtils(),
									 m_timerLoadColl(0),
									 m_timerAlg(0),
									 m_timerSave(0),
									 m_data("TrigDataAccess/TrigDataAccess"),
									 m_detStore("DetectorStore", name),
									 m_tileTBID(0),
									 m_error(0),
									 m_triggerEnergies(xAOD::TrigT2MbtsBits::NUM_MBTS,0.),
									 m_triggerTimes(xAOD::TrigT2MbtsBits::NUM_MBTS,0.),
                                                                         m_triggerID(xAOD::TrigT2MbtsBits::NUM_MBTS,0.),
									 m_useCachedResult(false), 
									 m_t2MbtsBits(0),
									 m_cachedTE(0) {

  declareProperty("TrigDataAccess",m_data,"Data Access for LVL2 Calo Algorithms");

  // These properties are used for monitoring only.
  declareProperty("Threshold",                          m_threshold = 40.0/222.0 );  // Value in pC
  declareProperty("TimeCut",                            m_timeCut=-1.0);
  declareProperty("GlobalTimeOffset",                   m_globalTimeOffset=0.0);
  declareProperty("TimeOffsetA0",                       m_timeOffsets[0]=0.0);
  declareProperty("TimeOffsetA1",                       m_timeOffsets[1]=0.0);
  declareProperty("TimeOffsetA2",                       m_timeOffsets[2]=0.0);
  declareProperty("TimeOffsetA3",                       m_timeOffsets[3]=0.0);
  declareProperty("TimeOffsetA4",                       m_timeOffsets[4]=0.0);
  declareProperty("TimeOffsetA5",                       m_timeOffsets[5]=0.0);
  declareProperty("TimeOffsetA6",                       m_timeOffsets[6]=0.0);
  declareProperty("TimeOffsetA7",                       m_timeOffsets[7]=0.0);
  declareProperty("TimeOffsetA8",                       m_timeOffsets[8]=0.0);
  declareProperty("TimeOffsetA9",                       m_timeOffsets[9]=0.0);
  declareProperty("TimeOffsetA10",                      m_timeOffsets[10]=0.0);
  declareProperty("TimeOffsetA11",                      m_timeOffsets[11]=0.0);
  declareProperty("TimeOffsetA12",                      m_timeOffsets[12]=0.0);
  declareProperty("TimeOffsetA13",                      m_timeOffsets[13]=0.0);
  declareProperty("TimeOffsetA14",                      m_timeOffsets[14]=0.0);
  declareProperty("TimeOffsetA15",                      m_timeOffsets[15]=0.0);
  declareProperty("TimeOffsetC0",                       m_timeOffsets[16]=0.0);
  declareProperty("TimeOffsetC1",                       m_timeOffsets[17]=0.0);
  declareProperty("TimeOffsetC2",                       m_timeOffsets[18]=0.0);
  declareProperty("TimeOffsetC3",                       m_timeOffsets[19]=0.0);
  declareProperty("TimeOffsetC4",                       m_timeOffsets[20]=0.0);
  declareProperty("TimeOffsetC5",                       m_timeOffsets[21]=0.0);
  declareProperty("TimeOffsetC6",                       m_timeOffsets[22]=0.0);
  declareProperty("TimeOffsetC7",                       m_timeOffsets[23]=0.0);
  declareProperty("TimeOffsetC8",                       m_timeOffsets[24]=0.0);
  declareProperty("TimeOffsetC9",                       m_timeOffsets[25]=0.0);
  declareProperty("TimeOffsetC10",                      m_timeOffsets[26]=0.0);
  declareProperty("TimeOffsetC11",                      m_timeOffsets[27]=0.0);
  declareProperty("TimeOffsetC12",                      m_timeOffsets[28]=0.0);
  declareProperty("TimeOffsetC13",                      m_timeOffsets[29]=0.0);
  declareProperty("TimeOffsetC14",                      m_timeOffsets[30]=0.0);
  declareProperty("TimeOffsetC15",                      m_timeOffsets[31]=0.0);

  declareMonitoredVariable("MultiplicityEBA",           m_mult.first); // Number of counters within cuts, side A
  declareMonitoredVariable("MultiplicityEBC",           m_mult.second); // Number of counters within cuts, side C
  declareMonitoredVariable("TimeDiff_A_C",              m_timeDiff_A_C);  // Time difference A - C
  declareMonitoredVariable("BCID",                      m_BCID);  
  declareMonitoredStdContainer("TriggerEnergies",       m_triggerEnergies); // Energies deposited in each counter
  declareMonitoredStdContainer("TriggerTimes",          m_triggerTimes); // Relative times of each of the triggers.
  declareMonitoredStdContainer("TriggerID",             m_triggerID); // ID of MBTS detectors
                                                                             
                                                            
}
//--------------------------------------------------------------------------------------

HLT::ErrorCode T2MbtsFex::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
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
  
  ATH_MSG_DEBUG("Executing this T2MbtsFex as " << name());
  
  // start monitoring
  beforeExecMonitors().ignore();
 
  // monitor
  const xAOD::EventInfo* evinfo(0);
  StatusCode sc = store()->retrieve(evinfo);
  if( sc.isFailure() ){
    ATH_MSG_ERROR("Cannot retrieve xAOD::EventInfo from SG.");
    return HLT::SG_ERROR;
  }
  else {
    m_BCID = evinfo->bcid();
    ATH_MSG_DEBUG("BCID is " << m_BCID);
  }

  // Reset errors
  m_error=0x0;
  
  if(timerSvc()) 
    m_timerLoadColl->start();
  
  // Reset Iterators
  m_itBegin=m_itEnd;
  
  if(m_data->LoadMBTS(m_itBegin,m_itEnd).isFailure()){
    m_error|=m_data->report_error();
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::BAD_JOB_SETUP);
  }
  
  if(m_itBegin == m_itEnd) {
    ATH_MSG_WARNING("The MBTS TileCellCollection has no elements!");
  }
  
  if(timerSvc()){
    m_timerLoadColl->stop();
    m_timerAlg->start();
  }

  uint32_t bit_pos = 0;

  // Clear the monitoring variables.
  m_triggerID.clear();
  m_triggerID.resize(xAOD::TrigT2MbtsBits::NUM_MBTS,0);
  m_triggerEnergies.clear();
  m_triggerEnergies.resize(xAOD::TrigT2MbtsBits::NUM_MBTS,0);
  m_triggerTimes.clear();
  m_triggerTimes.resize(xAOD::TrigT2MbtsBits::NUM_MBTS,0);
  
  // Pack values into vectors.
  for( m_itt=m_itBegin; m_itt != m_itEnd; ++m_itt ){
    Identifier id=(*m_itt)->ID();
    
    // MBTS Id type is  "side"  +/- 1
    int type_id = m_tileTBID->type(id);
   
    // MBTS Id channel is "eta"  0-1   zero is closer to beam pipe
    unsigned int channel_id = m_tileTBID->channel(id);
     
    // MBTS Id module is "phi"  0-7
    unsigned int module_id = m_tileTBID->module(id);
    
    ATH_MSG_DEBUG("type_id = " << type_id);
    ATH_MSG_DEBUG("channel_id = " << channel_id);
    ATH_MSG_DEBUG("module_id = " << module_id);

    // Catch errors
    if(abs(type_id) != 1) {
      ATH_MSG_WARNING("MBTS identifier type is out of range");
      continue;
    }
    if( channel_id > 1 ){
	    ATH_MSG_WARNING("MBTS identifier channel is out of range");
      continue;
    }
    if( module_id > 7 ){
	    ATH_MSG_WARNING("MBTS identifier module is out of range");
      continue;
    }      
    
    bit_pos = 0; // The position of the bit
      
    if(type_id == -1) { // C-side
      bit_pos += 16;
    }
    
    bit_pos += channel_id*8;
    bit_pos += module_id;
    
    unsigned int nTriggerEnergies = m_triggerEnergies.size();
    if(bit_pos > (nTriggerEnergies-1)) {
      ATH_MSG_ERROR("Bit pos " << bit_pos << " is greater than the size of the energy vector " << nTriggerEnergies);
    }
    else {
      m_triggerEnergies[bit_pos] = (*m_itt)->energy();
      m_triggerID[bit_pos] = bit_pos;
      ATH_MSG_DEBUG("Counter id = " << bit_pos << ", energy = " << m_triggerEnergies[bit_pos] << " pC");
    }
      
    unsigned int nTriggerTimes = m_triggerTimes.size();
    if(bit_pos > (nTriggerTimes-1)) {
      ATH_MSG_ERROR("Bit pos " << bit_pos << " is greater than the size of the time vector " << nTriggerTimes);
    }
    else {
      m_triggerTimes[bit_pos] = (*m_itt)->time();
	    ATH_MSG_DEBUG("Counter id = " << bit_pos << ", time = " << m_triggerTimes[bit_pos] << " ns"); 
    }
  }
  
  if( timerSvc() ){
    m_timerAlg->stop();
    m_timerSave->start();
  }

  m_t2MbtsBits = new xAOD::TrigT2MbtsBits();
  m_t2MbtsBits->makePrivateStore();
  m_t2MbtsBits->setTriggerEnergies(m_triggerEnergies);
  m_t2MbtsBits->setTriggerTimes(m_triggerTimes);

  // Calculate the multiplicities to fill the monitoring variables
  if(!calculateMultiplicities(m_t2MbtsBits,0,msg(),msgLvl())) {
    ATH_MSG_DEBUG("calculateMultiplicities failed");
    return HLT::OK;
  } 

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

  HLT::ErrorCode hltStatus = attachFeature(outputTE, m_t2MbtsBits, "T2Mbts");
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

HLT::ErrorCode T2MbtsFex::hltInitialize() {

  ATH_MSG_INFO("in T2MbtsFex::initialize()");

  if(m_data.retrieve().isFailure()) {
    ATH_MSG_ERROR("Could not get m_data");
    return HLT::BAD_JOB_SETUP;
  }
  
  // Connect to the Detector Store to retrieve TileTBID.
  if(m_detStore.retrieve().isFailure()) {
    ATH_MSG_ERROR("Couldn't connect to " << m_detStore.typeAndName());
    return HLT::BAD_JOB_SETUP;
  }
 
  // Retrieve TileTBID helper from det store
  // (The MBTS was added to the Test Beam (TB) list.)
  if(m_detStore->retrieve(m_tileTBID).isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve TileTBID helper from DetectorStore");
    return HLT::BAD_JOB_SETUP;
  }

  // Create timers
  if(timerSvc()) {
    m_timerLoadColl  = addTimer("LoadColl");
    m_timerAlg       = addTimer("Algorithm");
    m_timerSave      = addTimer("Saving");
  }
  
  return HLT::OK;
}

//--------------------------------------------------------------------------------------

HLT::ErrorCode T2MbtsFex::hltFinalize() {
  return HLT::OK;
}

//--------------------------------------------------------------------------------------
HLT::ErrorCode T2MbtsFex::hltEndEvent() {
     m_useCachedResult = false;
     m_t2MbtsBits = 0;
     m_cachedTE=0;
     return HLT::OK;
}

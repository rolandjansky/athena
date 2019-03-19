/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <string>

// Framework
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

// Reconstruction and MC
#include "GeneratorObjects/McEventCollection.h"
#include "TruthHelper/IsGenStable.h"
#include "TruthHelper/PileUpType.h"

// Local
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"
#include "TrigCostMonitor/TrigNtVarsTool.h"

//---------------------------------------------------------------------------------------
Trig::TrigNtVarsTool::TrigNtVarsTool(const std::string &name,
             const std::string &type,
             const IInterface  *parent)
  :AthAlgTool(name, type, parent),
   m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
   m_config(0)
{
  declareInterface<Trig::ITrigNtTool>(this);

  declareProperty("collectMC",     m_collectMC  = true);
  declareProperty("collectPU",     m_collectPU  = true);
  declareProperty("collectTD",     m_collectTD  = false);
  declareProperty("keyMCEvent",    m_keyMCEvent = "TruthEvent");
  declareProperty("keyPileUp",     m_keyPileUp  = "OverlayEvent");
  declareProperty("trigDec",       m_trigDec);
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtVarsTool::initialize()
{    
  //
  // Get services
  //

  // Get TrigDecisionTool
  if(m_collectTD) CHECK(m_trigDec.retrieve());

  return StatusCode::SUCCESS;
}
//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtVarsTool::finalize()
{
  //
  // Clean up internal state
  //
  ATH_MSG_DEBUG("finalize()" );

  m_config = 0;

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtVarsTool::Fill(TrigMonConfig *confg)
{
  //
  // Copy configuration
  //
  m_config = 0;

  if(confg) {
    m_config = confg;
  }
  else {
    ATH_MSG_WARNING("Null TrigMonConfig pointer" );
    return false;
  }

  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtVarsTool::Fill(TrigMonEvent &event)
{
  //
  // Process current event
  //
  if(m_collectMC) CollectMC(event);
  if(m_collectPU) CollectPU(event);
  if(m_collectTD) CollectTD(event);

  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtVarsTool::CollectTD(TrigMonEvent& /*event*/)
{

  ATH_MSG_ERROR("This tool is depricated. It needs migrating if it is to function again." );

 //  //
 //  // Collect trigger deciions
 //  //
 //  if(!m_config) {
 //    ATH_MSG_WARNING("Missing internal trigger configuration" );
 //    return false;
 //  }
  
 //  //
 //  // Get configured chains for this level
 //  //
 //  Trig::TrigDecisionTool &t = *m_trigDec;
 //  const std::vector<std::string> trig_list = t.getListOfTriggers();    

 //  for(unsigned i = 0; i < m_config->size<TrigConfChain>(); ++i) {
 //    const TrigConfChain &chain = m_config->at<TrigConfChain>(i);
 //    const std::string &n = chain.getName();
    
 //    //
 //    // Match chain name against list of triggers
 //    //
 //    const unsigned ntrig = std::count(trig_list.begin(), trig_list.end(), chain.getName());
 //    if(ntrig != 1) {
 //      ATH_MSG_WARNING("Incorrect count " << chain.getName() << ": " << ntrig );
 //    }

 //    //
 //    // Collect all decision bits
 //    //
 //    std::vector<TrigMonL1Item::Decision> lv1;
 //    std::vector<TrigMonChain::Decision> hlt;
    
 //    if(chain.getLevelId() == 1) {
 //      if(t.isPassed(n, TrigDefs::Physics))                        lv1.push_back(TrigMonL1Item::kL1AfterVeto);
 //      if(t.isPassedBits(n) & TrigDefs::L1_isPassedAfterVeto)      lv1.push_back(TrigMonL1Item::kL1AfterVeto);
 //      if(t.isPassedBits(n) & TrigDefs::L1_isPassedAfterPrescale)  lv1.push_back(TrigMonL1Item::kL1AfterPrescale);
 //      if(t.isPassedBits(n) & TrigDefs::L1_isPassedBeforePrescale) lv1.push_back(TrigMonL1Item::kL1BeforePrescale);
 //    }
 //    else if(chain.getLevelId() == 2) {
 //      if(t.isPassed(n, TrigDefs::Physics))             hlt.push_back(TrigMonChain::kPassed);
 //      if(t.isPassedBits(n) & TrigDefs::L2_passedRaw)   hlt.push_back(TrigMonChain::kPassedRaw);
 //      if(t.isPassedBits(n) & TrigDefs::L2_prescaled)   hlt.push_back(TrigMonChain::kPrescaled);
 //      if(t.isPassedBits(n) & TrigDefs::L2_passThrough) hlt.push_back(TrigMonChain::kPassedThrough);
 //      if(t.isPassedBits(n) & TrigDefs::L2_resurrected) hlt.push_back(TrigMonChain::kResurrected);
 //    }
 //    else if(chain.getLevelId() == 3) {
 //      if(t.isPassed(n, TrigDefs::Physics))             hlt.push_back(TrigMonChain::kPassed);
 //      if(t.isPassedBits(n) & TrigDefs::EF_passedRaw)   hlt.push_back(TrigMonChain::kPassedRaw);
 //      if(t.isPassedBits(n) & TrigDefs::EF_prescaled)   hlt.push_back(TrigMonChain::kPrescaled);
 //      if(t.isPassedBits(n) & TrigDefs::EF_passThrough) hlt.push_back(TrigMonChain::kPassedThrough);
 //      if(t.isPassedBits(n) & TrigDefs::EF_resurrected) hlt.push_back(TrigMonChain::kResurrected);
 //    }
    
 //    //
 //    // Save triggers which have at least one trigger bit set
 //    //
 //    if(!lv1.empty()) {  
 //      TrigMonL1Item trig;
 //      trig.setCtpId(chain.getCounter());
      
 //      for(unsigned j = 0; j < lv1.size(); ++j) {
  // trig.addDecision(lv1.at(j));
 //      }
      
 //      event.add<TrigMonL1Item>(trig);
 //    }

 //    if(!hlt.empty()) {
 //      TrigMonChain trig(chain.getLevelId(), chain.getCounter());    
      
 //      for(unsigned j = 0; j < hlt.size(); ++j) {
  // trig.addDecision(hlt.at(j));
 //      }
      
 //      event.add<TrigMonChain>(trig);
 //    }
 //  }

  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtVarsTool::CollectMC(TrigMonEvent& /*event*/)
{  

  ATH_MSG_ERROR("This tool is depricated. It needs migrating if it is to function again." );

  // //
  // // Collect MC truth informartion
  // //

  // if(!m_storeGate->contains<McEventCollection>(m_keyMCEvent)) {
  //   ATH_MSG_DEBUG("McEventCollection not found" );
  //   return false;
  // }

  // const McEventCollection* mcCollptr = 0;
  // if(m_storeGate->retrieve(mcCollptr, m_keyMCEvent).isFailure()) {
  //   ATH_MSG_WARNING("Could not retrieve McEventCollection" );
  //   return false;
  // }

  // /** initialize a pileup type helper object */ 
  // PileUpType pileupType( mcCollptr );

  // // signal
  // const HepMC::GenEvent * signal = pileupType.signal_mc_event();
  // const HepMC::GenVertex* prodVtx = signal->signal_process_vertex();
  // if ( prodVtx ) {
  //    float xi = (prodVtx->position()).x();
  //    float yi = (prodVtx->position()).y();
  //    float zi = (prodVtx->position()).z();

  //    log() << MSG::DEBUG
   //   << "Signal vertex is " << xi << " " << yi << " " << zi
   //   << " process ID = " << signal->signal_process_id()
   //   << " McEvent index = " << signal->event_number() );
  // }

  // // in-time minbias
  // McEventCollection::const_iterator ibeg_minb = pileupType.in_time_minimum_bias_event_begin();
  // McEventCollection::const_iterator iend_minb = pileupType.in_time_minimum_bias_event_end();
  // unsigned int icount_pileup = 0;

  // for (; ibeg_minb!=iend_minb; ++ibeg_minb ) {
  //   const HepMC::GenVertex* prodVtx = (*ibeg_minb)->signal_process_vertex();
  //   if ( prodVtx ) {
  //     ++icount_pileup;
  //      float xi = (prodVtx->position()).x();
  //      float yi = (prodVtx->position()).y();
  //      float zi = (prodVtx->position()).z();
       
  //      log() << MSG::DEBUG
   //     << "Pileup vertex is " << xi << " " << yi << " " << zi
   //     << " process ID = " << (*ibeg_minb)->signal_process_id()
   //     << " McEvent index = " << (*ibeg_minb)->event_number() );
  //   }
  // }

  // // in-time minbias and signal
  // McEventCollection::const_iterator ibeg_evt = pileupType.in_time_event_begin();
  // McEventCollection::const_iterator iend_evt = pileupType.in_time_event_end();
  // unsigned int icount_event = 0;

  // for (; ibeg_evt != iend_evt; ++ibeg_evt ) {
  //   const HepMC::GenVertex* prodVtx = (*ibeg_evt)->signal_process_vertex();
  //   if ( prodVtx ) {
  //     ++icount_event;
  //      float xi = (prodVtx->position()).x();
  //      float yi = (prodVtx->position()).y();
  //      float zi = (prodVtx->position()).z();
       
  //      log() << MSG::DEBUG
   //     << "Pileup/signal vertex is " << xi << " " << yi << " " << zi
   //     << " process ID = " << (*ibeg_evt)->signal_process_id()
   //     << " McEvent index = " << (*ibeg_evt)->event_number() );
  //   }
  // }

  // // in-time minbias and signal
  // HepMC::GenEvent::particle_const_iterator ibeg_sig = pileupType.signal_particle_begin();
  // HepMC::GenEvent::particle_const_iterator iend_sig = pileupType.signal_particle_end();
  // unsigned int icount_signal = 0;

  // for (; ibeg_sig != iend_sig; ++ibeg_sig ) {
  //   const HepMC::GenParticle* prodEvent = (*ibeg_sig);
  //   if ( prodEvent ) {
  //     ++icount_signal;
  //   }
  // }

  // // retrieve the list of in-time generator stable particles - signal and in-time minimum bias 
  // // in-time meas particles in Bunch crossing = 0
  // TruthHelper::IsGenStable isStable;
  // std::vector<const HepMC::GenParticle*> particleList;
  // pileupType.in_time_particles( particleList, isStable );

  // // Add data
  // event.addVar(201, static_cast<float>(icount_signal));
  // event.addVar(202, static_cast<float>(icount_pileup));
  // event.addVar(203, static_cast<float>(icount_event));
  // event.addVar(204, static_cast<float>(particleList.size()));
  
  // ATH_MSG_DEBUG("Number of stable in-time pileup particles = " << particleList.size() );

  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtVarsTool::CollectPU(TrigMonEvent& /*event*/)
{

  ATH_MSG_ERROR("This tool is depricated. It needs migrating if it is to function again." );

  //
  // Collect MC truth from PileupEventInfo
  //

  //
  // Counters for pileup events
  //
 //  unsigned nevent = 0, nminbias = 0, ncavern = 0, nbeamgas = 0, nzerobias = 0;
 //  unsigned nevent_intime = 0, nsignal_intime = 0, nminbias_intime = 0;
 //  unsigned ncavern_intime = 0, nbeamgas_intime = 0;

 //  const PileUpEventInfo* pevt = 0;
 //  if(m_storeGate->contains<PileUpEventInfo>(m_keyPileUp) && 
 //     m_storeGate->retrieve(pevt, m_keyPileUp).isSuccess()) {

 //    ATH_MSG_DEBUG("Found PileUpEventInfo with key: " << m_keyPileUp );      
    
 //    PileUpEventInfo::SubEvent::const_iterator it  = pevt->beginSubEvt();
 //    PileUpEventInfo::SubEvent::const_iterator end = pevt->endSubEvt();
    
 //    for (; it != end; ++it) {
 //      const PileUpEventInfo::SubEvent &sube = *it;
 //      const EventInfo *sevt = sube.pSubEvt;
 //      if (sevt) {
  // ATH_MSG_DEBUG("PileUp SubEvent Info:"
  //       << " Time="     << sube.time()
  //       << " Index="    << sube.index()
  //       << " UserType=" << sevt->event_type()->user_type() );
  
  // if (sube.time() == 0) {
  //   nevent_intime++;
    
  //   if     (sube.type() == 0) ++nsignal_intime;
  //   else if(sube.type() == 1) ++nminbias_intime;
  //   else if(sube.type() == 2) ++ncavern_intime;
  //   else if(sube.type() == 3) ++nbeamgas_intime;
  // }
  
  // ++nevent;
  
  // if      ( sube.type()==1 ) ++nminbias;  // MIN BIAS
  // else if ( sube.type()==2 ) ++ncavern;   // CAVERN BACKGROUND
  // else if ( sube.type()==3 ) ++nbeamgas;  // BEAM GAS / HALO
  // else if ( sube.type()==4 ) ++nzerobias; // ZERO BIAS 
 //      }
 //      else { 
  // ATH_MSG_DEBUG("Subevent is null ptr " );
 //      }
 //    }
 //  }
 //  else {
 //    ATH_MSG_DEBUG("Failed to get PileUpEventInfo with key=" << m_keyPileUp );
 //    return false;
 //  }
  
 //  log() << MSG::DEBUG 
  // << "Number of minbias events  = " << nminbias        << endmsg
  // << "Number of cavern  events  = " << ncavern         << endmsg
  // << "Number of intime  events  = " << nevent_intime   << endmsg
  // << "Number of intime  signal  = " << nsignal_intime  << endmsg
  // << "Number of intime  minbias = " << nminbias_intime << endmsg  
  // << "Number of intime  cavern  = " << ncavern_intime  << endmsg
  // << "Number of intime beamgas  = " << nbeamgas_intime );

 //  // Add event counts for all pileup events
 //  event.addVar(210, static_cast<float>(nevent));     // N of all pileup events
 //  event.addVar(211, static_cast<float>(nminbias));   // N of all minbias events
 //  event.addVar(212, static_cast<float>(ncavern));    // N of all cavern events
 //  event.addVar(213, static_cast<float>(nbeamgas));   // N of all beamgas events
 //  event.addVar(214, static_cast<float>(nzerobias));  // N of all zero bias events

 //  // Add event counts for intime (BC=0) pileup events
 //  event.addVar(215, static_cast<float>(nevent_intime));   // N of all intime events
 //  event.addVar(216, static_cast<float>(nsignal_intime));  // N of intime signal events
 //  event.addVar(217, static_cast<float>(nminbias_intime)); // N of intime minbias events 
 //  event.addVar(218, static_cast<float>(ncavern_intime));  // N of intime cavern events 
 //  event.addVar(219, static_cast<float>(nbeamgas_intime)); // N of intime beamgas events 

  return true;
}

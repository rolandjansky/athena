/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : LvlConverter
 *
 * @brief  Base class for HLT Lvl1Converter, Lvl2Converter, ...
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: LvlConverter.cxx,v 1.34 2009-03-24 20:41:45 tbold Exp $
 **********************************************************************************/
#include "TrigSteering/LvlConverter.h"
#include "TrigMonitorBase/TrigMonitorToolBase.h"
#include "TrigSteering/SteeringChain.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigNavigation/Navigation.h"
#include "TrigSteering/HLTResultAccessTool.h"



using namespace HLT;


LvlConverter::LvlConverter(const std::string& name, const std::string& type,
			   const IInterface* parent)
  : AthAlgTool(name, type, parent),
    m_config(0),
    m_includePrescaledChains(false),
    m_ignorePrescales(false),
    m_hltTool("HLT::HLTResultAccessTool/HLTResultAccessTool",this),
    m_configuredChains(0)
{
  declareInterface<HLT::ILvlConverter>( this );

  declareProperty( "HLTResultAccessTool", m_hltTool, "tool to access the HLTResult");  
  declareProperty( "ignorePrescales", m_ignorePrescales=false, "Ignore prescales from the configuration & and preovious level.");
  declareProperty( "includePrescaledChains", m_includePrescaledChains=true, "Include chains originating from prescaled lower-level chains.");

}


StatusCode LvlConverter::initialize()
{
  // get the hlt tool
  CHECK(m_hltTool.retrieve());

  if (hltInitialize() != HLT::OK ) return StatusCode::FAILURE;
  
  ATH_MSG_INFO("m_includePrescaledChains = " << m_includePrescaledChains);

  return StatusCode::SUCCESS;
}


StatusCode LvlConverter::finalize()
{
  HLT::ErrorCode ec = hltFinalize();

  if (ec == HLT::ERROR) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

ErrorCode LvlConverter::setConfiguredChains(const std::vector<HLT::SteeringChain*>& configuredChains)
{
   // save local pointer to all configured chains:
   m_configuredChains = &configuredChains;

   // clear search map of chains
   m_chainIdMap.clear();
   m_chainCounterMap.clear();

   // fill search map with all configured chains
   // (this will speed-up the activateChain(..) method where Chains have to be matched)
   for(std::vector<HLT::SteeringChain*>::const_iterator chain = configuredChains.begin();
       chain != configuredChains.end(); ++chain) {
      ATH_MSG_DEBUG("adding chain  " << " " << (*chain)->getConfigChain()->chain_name() << " " << (*chain)->getConfigChain()->lower_chain_name());

      // for fast activation of all chain w/o a lower_chain name, remember them already :
      if ( (*chain)->getConfigChain()->lower_chain_name() == "" ) {
         m_chainsAlwaysActive.push_back( (*chain) );
      } else if ( (*chain)->getConfigChain()->hasMultipleLowerChains() ) {
         std::vector<unsigned int> hashes = (*chain)->getConfigChain()->lower_chain_hash_ids();

         for ( unsigned i = hashes.size(); i--; ) {
            //	m_chainCounterMap[ (*chain)->getConfigChain()->lower_chain_counter() ].push_back(*chain);
         
            m_chainIdMap[ hashes[i] ].push_back(*chain);
            ATH_MSG_DEBUG(" adding Or seeding  " << hashes[i] << " " << (*chain)->getConfigChain()->chain_name() << " " << (*chain)->getConfigChain()->lower_chain_name());
         }
      } else { // for all other chains, create the lookup map via lower_chain
         m_chainCounterMap[ (*chain)->getConfigChain()->lower_chain_counter() ].push_back(*chain);
         m_chainIdMap[      (*chain)->getConfigChain()->lower_chain_hash_id() ].push_back(*chain);
      
         ATH_MSG_DEBUG(" adding plain seeding  " << (*chain)->getConfigChain()->lower_chain_hash_id()  << " " << (*chain)->getConfigChain()->chain_name() << " " << (*chain)->getConfigChain()->lower_chain_name());

      }

   }

   return HLT::OK;
}


ErrorCode LvlConverter::deserializeNavigation(HLT::Navigation& nav, const HLTResult& result)
{
  const std::vector<uint32_t>& navData = result.getNavigationResult();
  if (navData.size() == 0) {
     ATH_MSG_ERROR("navigation data in HLTResult has size 0!");
     return HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::NAV_ERROR, 
                           HLT::SteeringInternalReason::NO_HLT_RESULT);
  }

  if ( !nav.deserialize( navData ) ) {
     ATH_MSG_ERROR("navigation data in HLTResult corrupted or truncated");
     return HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::NAV_ERROR, 
                           HLT::SteeringInternalReason::WRONG_HLT_RESULT);
  }

  // some navigation debug output:
  ATH_MSG_VERBOSE("\n" << nav);

  return HLT::OK;
}




ErrorCode LvlConverter::activateChains(std::vector<SteeringChain*>& chains,
				       const std::vector<Chain>& previousChains, bool /*stickyPassThrough*/)
{
   // Step 1: activate all chains which have a lower chain with state chainPassed()
   // Step 2: activate all chains without lower chain

   //========= Step 1 ========================
   // loop over chains from previous level
   for (std::vector<Chain>::const_iterator preChain = previousChains.begin();
        preChain != previousChains.end(); ++preChain) {

      // Include chains into this level, only if previous chain passed. This might be because the chain
      // was normally [A]ccepted, or passedThrough. Note, passThrough overwrites preScaled!
      bool l2AfterPrescale = (preChain->chainPassedRaw() && !preChain->isResurrected()) || preChain->isPassedThrough();
      bool l2BeforePrescale = preChain->chainPassedRaw() && preChain->isResurrected();
    
      //std::vector<HLT::SteeringChain*>::iterator chainBegin = m_chainCounterMap[ (*preChain).getChainCounter() ].begin();
      //    std::vector<HLT::SteeringChain*>::iterator chainEnd   = m_chainCounterMap[ (*preChain).getChainCounter() ].end();
      //    for(std::vector<HLT::SteeringChain*>::iterator chain = chainBegin;
      //	chain != chainEnd; ++chain) {
      for(HLT::SteeringChain* chain : m_chainCounterMap[ (*preChain).getChainCounter() ] ) {
         addChain(chains, chain, l2AfterPrescale, l2BeforePrescale, preChain->isPassedThrough());
      }
   }

   //========= Step 2 ========================
   // loop over chains and activate (in addition) all those that have no lower_chain specified !
   for ( HLT::SteeringChain* chain : m_chainsAlwaysActive) {
      ATH_MSG_DEBUG("Activating chain: " << chain->getChainCounter());
      addChain(chains, chain, true, true);
   }
   return HLT::OK;
}



//bool LvlConverter::addChain(std::vector<HLT::SteeringChain*>& chains, HLT::SteeringChain* ch,
//			    bool isAfterPrescale, bool isBeforePrescale ) {
// at this point we have the same info as usually get from L1


bool LvlConverter::addChain(std::vector<HLT::SteeringChain*>& chains, HLT::SteeringChain* ch,
                            bool isAfterPrescale, bool isBeforePrescale, bool stickyPassThrough ) {
   // at this point we have the same info as usually get from L1
   bool added=false;
   
   // do not add chains which have negative prescale
   if (ch->prescaleFactor() < -0.1) {
      return added;
   }

   if ( isAfterPrescale ) {
      // chains passed by previous level
      ch->setActive();
      // we apply prescales if ignore is not set
      // we always apply prescales if PS factor indicates it is spacial one (0 - infinte, -1 - chain off)
      if ( ! m_ignorePrescales || ch->prescaleFactor() < 0.9 )
         ch->setPrescaleState();

      // set PT given this chain factors
      ch->setPassThroughState();

      // but set it always when it comes from previous level
      if ( stickyPassThrough ) 
         ch->setPassThrough();



      // check if chain is to be rerun
      if ( m_includePrescaledChains && (ch->isPrescaled() && !ch->isPassedThrough() ) ) 
         ch->setResurrectedState();
    
      if ( ch->runInFirstPass() || ch->runInSecondPass() || ch->isPrescaled()) {
         chains.push_back(ch);
         added=true;
      }
   } 
  
   if ( ! added && isBeforePrescale) {    
      ch->setActive();
      if ( m_includePrescaledChains )
         ch->setResurrectedState();
      if ( ch->runInSecondPass() ) {
         added=true;
         chains.push_back(ch);
      }
   }

   ATH_MSG_DEBUG("Previous level:" << " BP: " << isBeforePrescale << " AP: " << isAfterPrescale 
                 << " added: " << added << " 1st: " << ch->runInFirstPass() <<  " 2nd: " << ch->runInSecondPass() << " " << *ch);             
   return added;
}

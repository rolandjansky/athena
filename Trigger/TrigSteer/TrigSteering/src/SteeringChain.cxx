/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "boost/foreach.hpp"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTStreamTag.h"

//#include "TrigSteering/TrigSteer.h"
#include "TrigSteering/Signature.h"
#include "TrigInterfaces/AlgoConfig.h"

#include "TrigSteering/Scaler.h"
#include "TrigSteering/ScalerSvc.h"

#include "TrigSteering/StreamTag.h"

#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Algorithm.h"

#include "TrigNavigation/Navigation.h"
#include "TrigTimeAlgs/ITrigTimerSvc.h"
#include "TrigTimeAlgs/TrigTimer.h"

#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigSteeringEvent/PartialEventBuildingInfo.h"

#include "TrigSteeringEvent/ScoutingInfo.h"

#include "TrigSteering/SteeringChain.h"

#include <iostream>

using namespace HLT;
using namespace std;

// normal constructor:
SteeringChain::SteeringChain( const TrigConf::HLTChain* configChain, ISequenceProvider* seqs,
              AlgoConfig* config, IScalerSvc* scalerSvc )
  : Chain(configChain),
    m_config(config),
    m_preScaler(0),
    m_passScaler(0),
    m_active(false), 
    m_timer(0),
    m_calibrationROBs(0),
    m_scouting(0),
    m_prescaleMaster(0), 
    m_prescalingWasRun(false)
{

  // make sure we have all the arguements:
  if (!config) {
    std::cerr << "Chain::Chain AlgoConfig is 0!" << std::endl;
    return;
  }

  if (!configChain) {
    m_config->getMsgStream() << MSG::ERROR << "Chain::Chain HLTChain is 0!" << endmsg;
    return;
  }

  if (!seqs) {
    m_config->getMsgStream() << MSG::ERROR << "Chain::Chain ISequenceProvider is 0!" << endmsg;
    return;
  }


  if (m_config->getMsgLvl() <=MSG::DEBUG) {
    m_config->getMsgStream() << MSG::DEBUG << "Chain "
                             << configChain->chain_name() << ": constructor" << endmsg;
  }

  if (!fillFromConfigChain(seqs, scalerSvc)) return;


  /* -------------------------------------------------------
   * do some range checks with the numbers,
   *  so that we don't run into problems when serializing...
   * ------------------------------------------------------- */

  // make sure there is at least one stream associated to this Chain:
  if (m_configChain->groups().size() < 1) {
    m_config->getMsgStream() << MSG::WARNING << "Chain "
                             << configChain->chain_name()
                             << ": has no group associated with it." << endmsg;
  }
  if (m_streamTags.size() < 1) {
    m_config->getMsgStream() << MSG::ERROR << "Chain "
                             << configChain->chain_name()
                             << ": no stream associated to this Chain => abort!" << endmsg;
    return;
  }
  // make sure the maximum number of steps is fitting into our 8 bits:
  if (m_signatures.size() > 64 ) {
    m_config->getMsgStream() << MSG::ERROR << "Chain "
                             << configChain->chain_name()
                             << ": has maximum step (="<< m_signatures.size() <<") does not fit into our 8 bits for serialization => abort!"
                             << endmsg;
    return;
  }

  // make sure chain_counter fits into 13 bits:
  if (m_chain_counter > 8191 ) {
    m_config->getMsgStream() << MSG::ERROR << "Chain "
                             << configChain->chain_name()
                             << ": chain_counter (="<< m_chain_counter <<") does not fit into our 13 bits for serialization => abort!"
                             << endmsg;
    return;
  }


  

  // add the timer
 if ( config->getDoTiming() ) {
   ServiceHandle<ITrigTimerSvc> timerSvc("TrigTimerSvc/TrigTimerSvc", "tr");
   if ( timerSvc.retrieve().isFailure() ) {
     m_config->getMsgStream() << MSG::WARNING << "Requested timing measurements but can't retrieve TrigTimerSvc for chain" << endmsg;
   }
   AlgTool *ntool = dynamic_cast<AlgTool*>(m_config->getNavigation());
   if ( !ntool ) {
     m_config->getMsgStream() << MSG::WARNING << "Navigation tool not available" << endmsg;
     return;
   }
   const Algorithm* parentAlg = dynamic_cast<const Algorithm*>(ntool->parent()); // this is the way to get parent alg name (TrigSteer_L2/EF) ugh we need better way.
   if ( !parentAlg ) {
     m_config->getMsgStream() << MSG::WARNING << "Parent alg not available" << endmsg;
     return;
   }
   
   std::string name = parentAlg->name()+":Chain_"+configChain->chain_name();
   m_timer = timerSvc->addItem(name); 
   m_config->getMsgStream() << MSG::DEBUG << "Booked timer: " << name << endmsg;
 }
 setStatus(HLT::ChainOK);

}


SteeringChain::~SteeringChain() {
  // delete all created signatures:
  for (std::vector<Signature*>::iterator sig = m_signatures.begin();
       sig != m_signatures.end(); ++sig) {
    delete (*sig); (*sig) = 0;
  }
  m_signatures.clear();
  m_streamTags.clear();
}





bool
SteeringChain::fillFromConfigChain( ISequenceProvider* seqs,
                                    IScalerSvc* scalerSvc ) {

   // we need at least one signature, otherwise we can not call the first signatures execute()
   // in case there is no signature, we must never activate this Chain! -> see setActive()
   if (getConfigChain()->signatureList().size() == 0) {
      m_config->getMsgStream() << MSG::DEBUG << "Chain: no signatures found for this chain!! "
                               << endmsg;
   }

   // We assume here that the HLTSignatures have reasonable signature_counters (e.g. not all the same)!
   // otherwise, we will print an ERROR message (in addSignature)

   // create our own HLT::Signature from the TrigConf::HLTSignature for all the configured signatures
   for(TrigConf::HLTSignature* sig : getConfigChain()->signatureList()) {

      if (m_config->getMsgLvl() <=MSG::DEBUG) {
         m_config->getMsgStream() << MSG::DEBUG << "Chain: creating new Signature: "
                                  << sig->label() << endmsg;
      }

      unsigned int pos = sig->signature_counter() - 1; // - 1 : signature_counters start with 1, vector pos with 0 !!
      //unsigned int pos = sig->signature_counter(); // if we want to use step 0 for topo emulation

      if (canAddSignature(pos) )
         m_signatures[pos] = new Signature(const_cast<const TrigConf::HLTSignature*>(sig), seqs, m_config);

   }

   // if we don't have the ScalerSvc (because this Chain is created from a TrigConf::HLTChain
   if ( !scalerSvc ) {
      if (m_config->getMsgLvl() <=MSG::DEBUG) {
         m_config->getMsgStream() << MSG::DEBUG << "ScalerSvc is NULL!" << endmsg;
      }
   }

   // For now use default -- at some point Chain should get "ScalerType" attributes.
   if (scalerSvc) m_preScaler  = scalerSvc->get("");
   if (scalerSvc) m_passScaler = scalerSvc->get("");

   // look for StreamTags ..
   const std::vector<TrigConf::HLTStreamTag*>& streams = getConfigChain()->streams();

   for ( std::vector<TrigConf::HLTStreamTag*>::const_iterator it = streams.begin();
         it != streams.end(); ++it) {
      // make sure the TrigConf streamTag pointer is not NULL:
      if ( !(*it) ) {
         m_config->getMsgStream() << MSG::ERROR << "Chain: TrigConf::HLTStreamTag pointer NULL!" << endmsg;
         return false;
      }
      m_streamTags.push_back( StreamTag( *it, scalerSvc ) );

      // if the stream type is "calibration" prepare ROBs storage
      if ((*it)->type() == "calibration" ) {
         m_calibrationROBs = new PartialEventBuildingInfo();
         m_scouting = new ScoutingInfo();
	 m_config->getMsgStream() << MSG::VERBOSE << "Chain " << getChainName() << " has the storage for the ROBs for PEB and data scouting" << endmsg;
      }

      // if the stream type is "monitoring" prepare ROBs storage
      if ((*it)->type() == "monitoring" ) {
	m_calibrationROBs = new PartialEventBuildingInfo();
	m_config->getMsgStream() << MSG::VERBOSE << "Chain " << getChainName() << " has the storage for the ROBs for PEB" << endmsg;
      }
   }

   // find out prescale master
   // assuming only one per chain (conf checkers must catch it otherwise)
   const std::set<std::string>& groups = m_configChain->groups();
   for ( std::set<std::string>::const_iterator git = groups.begin(); git != groups.end(); ++git ) {
      if ( git->find("CPS:") != std::string::npos ) {
         m_prescaleGroup = *git;
         break;
      }
   }

   return true;
}



HLT::ErrorCode SteeringChain::executeStep() {

   // if chain already inactive, stop
   if (!m_active) return HLT::OK;

   ScopeResumePauseTimer scopeTimer(m_timer, m_currentStep+1 >= int(m_signatures.size()));
   //ScopeResumePauseTimer scopeTimer(m_timer);
   // Debug output
   if (m_config->getMsgLvl() <=MSG::DEBUG) {
      m_config->getMsgStream() << MSG::DEBUG  << "Executing chain #"
                               << getChainCounter() << " (id "
                               << m_configChain->chain_name() << ") "
                               << " step " << m_currentStep << " out of " << m_signatures.size()
                               << endmsg;
   }
   

   // collect operational information: create new TrigOperationalInfo 
   if(m_config -> getSteeringOPILevel() > 0 && m_config -> getSteeringOPI()) {
      TrigOperationalInfo *steer_opi = m_config -> getSteeringOPI();
      steer_opi -> set("CHAIN:"+getChainName(), m_currentStep);
   }
  
   m_config->setPEBI(m_calibrationROBs); // this sets the context of PEB (i.e. al algs executing within this chain will insert their ROB demends into this object) 
   m_config->setScoutingInfo(m_scouting); // this sets the context of PEB (i.e. al algs executing within this chain will insert their ROB demends into this object) 


   // check if there is anything to do at this step
   if ( m_signatures.size() > unsigned(m_currentStep) ) {
      if ( m_signatures[m_currentStep] == 0) {
         if (m_config->getMsgLvl() <=MSG::DEBUG) {
            m_config->getMsgStream() << MSG::DEBUG << "skipping this step (no signature to satisfy)"
                                     << endmsg;
         }
      } else {
        
         // execute the current step; deactivate the chain if it fails
        
         HLT::ErrorCode ec = m_signatures[m_currentStep]->execute( m_active );

         //     m_config->getMsgStream() << MSG::DEBUG << "Chain " << m_configChain->chain_name()
         // 			     << " EC from signature execution =  " << HLT::strErrorCode(ec) << " at step "
         // 			     << m_currentStep << endmsg;
         m_errorCode = m_errorCode > ec ? m_errorCode : ec;
         // If some error occurred, end here:
         if (ec.action() > HLT::Action::CONTINUE ) {
            // some debug output
            if (m_config->getMsgLvl() <= MSG::WARNING) {
               m_config->getMsgStream() << MSG::WARNING << "Chain " << getChainName()
                                        << " aborting with error code " << strErrorCode(ec) << " at step "
                                        << m_currentStep << endmsg;
            }
            //m_errorCode = m_errorCode > ec ? m_errorCode : ec;
           
            m_active = false;
            return m_errorCode;
         } else {
            if (m_config->getMsgLvl() <= MSG::DEBUG) {
               m_config->getMsgStream() << MSG::DEBUG << "Chain " << getChainName()
                                        << " continues with error code " << strErrorCode(ec) << " at step "
                                        << m_currentStep << endmsg;
            }
         }
        
         // Chain failed without error: also stop
         if ( !m_active ) {
            if (m_config->getMsgLvl() <= MSG::DEBUG)
               m_config->getMsgStream() << MSG::DEBUG << "Chain " << getChainName()
                                        << " failed at step " << m_currentStep << endmsg;
            return m_errorCode;
         }
      }       
      // We passed: go on to the next step
      m_currentStep++;
   }

   // If there are no more steps, we passed (see chainPassed()). Nothing left to
   // do => de-activate the chain and wait for the others to finish.
   if (m_currentStep >= static_cast<int>(m_signatures.size()) ) {
      setPassedRaw();
      m_active = false;
        
      // some debug output
      if (m_config->getMsgLvl() <= MSG::DEBUG) {
         m_config->getMsgStream()   << MSG::DEBUG << "Chain " << getChainName()
                                    << " finished successfully" << endmsg;
      }
      return m_errorCode;
   }
   return m_errorCode;
}

HLT::ErrorCode SteeringChain::prepareStepRobRequests() {
  // if chain already inactive, stop
  if (!m_active) return HLT::OK;

  // if we are past the last chain step, also stop
  if ( m_signatures.size() <= unsigned(m_currentStep) ) return HLT::OK;

  // check if there is anything to do at this step
  if ( m_signatures[m_currentStep] == 0) {

    // nothing to do, only debug output
    if (m_config->getMsgLvl() <= MSG::DEBUG) {
      m_config->getMsgStream() << MSG::DEBUG << "skipping step " << m_currentStep << " (no signature to satisfy)"
                               << endmsg;
    }

  } else {
    // execute the prepareStepRobRequests of the current step
    HLT::ErrorCode ec = m_signatures[m_currentStep]->prepareRobRequests();
    
    m_errorCode = m_errorCode > ec ? m_errorCode : ec;

    // check the error code, if some error occurred, deactivate chain and end here
    if (ec.action() > HLT::Action::CONTINUE ) {

      // some debug output
      if (m_config->getMsgLvl() <= MSG::WARNING) {
        m_config->getMsgStream() << MSG::WARNING << "Chain " << getChainName()
                                 << " aborting with error code " << strErrorCode(ec) << " at step "
                                 << m_currentStep << endmsg;
      }
      m_active = false;
      return m_errorCode;
    }


    // if success then only some debug output
    if (m_config->getMsgLvl() <= MSG::DEBUG)
      m_config->getMsgStream() << MSG::DEBUG << "Chain " << getChainName()
                               << " continues with error code " << strErrorCode(ec) << " at step "
                               << m_currentStep << endmsg;
    
  }

  //  m_currentStep++;

  return m_errorCode;
}

bool SteeringChain::resetChain() {
  Chain::reset();
  m_active = false;
  if ( m_timer ) m_timer->reset();
  // reset streamtags

  BOOST_FOREACH(HLT::StreamTag& st, m_streamTags){
    st.reset();
  }
  // reset all signatures:
  for (std::vector<Signature*>::const_iterator sigIt =  m_signatures.begin();
       sigIt != m_signatures.end(); ++sigIt) {
    if ((*sigIt) != 0) {
      if (!(*sigIt)->reset()) return false;
    }
  }
  m_prescalingWasRun = false;

  if ( m_calibrationROBs ) m_calibrationROBs->clear();
  if ( m_scouting ) m_scouting->clear();
  
  return true;
}


bool SteeringChain::resetROBRequestPreparation() {
  // resetROBRequestPreparation for all signatures:
  for (std::vector<Signature*>::const_iterator sigIt =  m_signatures.begin();
       sigIt != m_signatures.end(); ++sigIt) {
    if ((*sigIt) != 0) {
      if (!(*sigIt)->resetROBRequestPreparation()) return false;
    }
  }
  return true;
}



void SteeringChain::setPassThroughState() {
  // passThrough  < 1 -> do nothing (pass though is not set) 0, -1, -17 work the same way then
  // passThrough == 1 -> always accept (pass through) this chain (for all events)
  // passThrough == N -> always accept (pass through) 1 event out of N

  // also the PT is applied only in events which are not prescales
  m_passThrough = false;
  if ( isPrescaled() )
    return;

  if (passThroughFactor() == 1.)
    m_passThrough = true;
  else if (passThroughFactor() > 1.)
    m_passThrough = m_passScaler->decision( passThroughFactor() );

}



void SteeringChain::setPrescaleState() {
  // prescale == < 0 - unsupported
  // prescale == 0 -> infinity
  // prescale == 1 -> do nothing
  // prescale == N -> reject (N-1) events out of N
  if ( m_prescalingWasRun )
    return;
  
  m_prescalingWasRun = true;

  if ( prescaleFactor() < -0.1 )
    m_config->getMsgStream() << MSG::ERROR << "Chain: : " << getChainName()
			     << " arrived to scaler service with prescale value < 0, this is not suported anymore" << endmsg;
  
  if ( prescaleFactor() < 0.1  && prescaleFactor() > -0.1 ) { // treat zero
    m_prescaled = true;
  } else {
    if ( m_prescaleMaster ) {
      m_prescaled = true;
      m_prescaleMaster->setPrescaleState();
      if ( ! m_prescaleMaster->isPrescaled() ) {
	m_prescaled = ! m_preScaler->decision( prescaleFactor()/m_prescaleMaster->prescaleFactor() );
      }
    } else {
      m_prescaled = ! m_preScaler->decision( prescaleFactor() );
    }
  }
  // if chain m_prescaled is already set to true, there is nothing left to do
  //if (m_prescaled) return;

  // Deal with StreamTag prescales
  // idea: if all streamTag prescales of this Chain are true => chain is also prescaled !

  // special case: there is no stream associated to this chain ...
  // not going to happen => checked in constructor!
  bool streamsPS = true;
  for ( std::vector<StreamTag>::iterator it = m_streamTags.begin();
        it !=  m_streamTags.end(); ++it) {
    (*it).setPrescaleState();
    streamsPS = streamsPS && (*it).isPrescaled();
  }

  // set m_prescale to true if _all_ streams are prescaled:
  // we do not want to do it anymore, this limits comissioning 
  // capabilities and in normal operations it also never the case
  //  if (streamsPS) m_prescaled = true;

}

void SteeringChain::setResurrectedState() {
  float rerun = m_configChain->rerun_prescale("1").second;
  m_resurrected = false;
  if ( rerun == 1.)
    m_resurrected = true;
  else if ( rerun > 1.)
    m_resurrected = m_preScaler->decision( rerun );    
}


bool SteeringChain::runInFirstPass() const {
  return (!isPrescaled() || isPassedThrough()) && !isResurrected();
}

bool SteeringChain::runInSecondPass() const {
  return isResurrected() && !isPassedThrough();
}


const std::vector<const Signature*> SteeringChain::getSignatures() const {

  std::vector<const Signature*> constVec;
  for (std::vector<Signature*>::const_iterator it = m_signatures.begin();
       it != m_signatures.end(); ++it) {
    constVec.push_back(*it);
  }

  return constVec;
}


bool SteeringChain::isValid() const {
  
  //if (getStatus() != HLT::ChainOK ) return false;

  for (std::vector<Signature*>::const_iterator it = m_signatures.begin();
       it != m_signatures.end(); ++it) {
    if (*it == 0) continue; // this is the case where one step has no Signature !
    if (!(*it)->isValid()) {
      return false;
    }
  }
  return true;
}


bool SteeringChain::canAddSignature(unsigned int pos) {
  // make sure, vector is big enough
  while(m_signatures.size() <= pos) m_signatures.push_back(0);

  if (m_signatures[pos] != 0) {
      m_config->getMsgStream() << MSG::ERROR << "Chain: 2 or more signatures have the same signature_counter: "
			       << pos << " --> ignore this Signature! " << endmsg;
    return false;
  }

  return true;
}

void SteeringChain::setPrescaleMaster(SteeringChain* master) {
  m_prescaleMaster = master;
}

std::string& SteeringChain::prescaleGroup() {
  return m_prescaleGroup;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : Sequence
 *
 * @brief  one sequence node, i.e.
 *         input TE types -> N x Fex algorithm, [Hypo algorithm] -> output TE type
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: Sequence.cxx,v 1.31 2009-02-26 12:52:52 tbold Exp $
 **********************************************************************************/

#include "TrigSteering/Sequence.h"

#include <algorithm>
#include "TrigInterfaces/Algo.h"
#include "TrigInterfaces/TECreateAlgo.h"
#include "TrigInterfaces/AlgoConfig.h"
#include "TrigInterfaces/ExceptionHandler.h"
#include "TrigNavigation/TriggerElement.h"

#include "AthenaKernel/Timeout.h"
#include "GaudiKernel/ThreadGaudi.h"
#include "GaudiKernel/GaudiException.h"
#include "eformat/Issue.h"

#include "GaudiKernel/ServiceHandle.h"
#include "TrigTimeAlgs/ITrigTimerSvc.h"
#include "TrigTimeAlgs/TrigTimer.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "GaudiKernel/AlgTool.h"

using namespace HLT;


Sequence::Sequence(std::vector<unsigned int> inputTypes,
		   unsigned int outputType, HLT::Algo* firstAlgo,
		   const std::vector<HLT::Algo*>& algos, HLT::AlgoConfig* config,
		   const std::vector<unsigned int>& topoTypes) :
   m_alreadyExecuted(false),
   m_execErrorCode(OK),
   m_firstAlgo(firstAlgo),
   m_nextAlgos(algos),
   m_inputTeTypes(inputTypes),
   m_outputTeType(outputType),
   m_config(config),
   m_topoStartFrom(topoTypes),
   m_prepRobReqAlreadyExecuted(false),
   m_prepRobReqErrorCode(HLT::OK),
   m_topoStartFromSequence(0),
   m_timer(0),
   m_robDataProvider("ROBDataProviderSvc/ROBDataProviderSvc","TrigSteer_HLT")
{
   // add the timer
   if ( config->getDoTiming() ) {
      ServiceHandle<ITrigTimerSvc> timerSvc("TrigTimerSvc/TrigTimerSvc", "tr");
      if ( timerSvc.retrieve().isFailure() ) {
         m_config->getMsgStream() << MSG::WARNING << "Requested timing measurements but can't retrieve TrigTimerSvc for sequence" << endmsg;
      }
      std::string telabel;
      if ( TrigConf::HLTTriggerElement::getLabel(outputType, telabel) ) {

         const AlgTool *ntool = dynamic_cast<AlgTool*>(m_config->getNavigation());
         if ( !ntool ) {
            m_config->getMsgStream() << MSG::WARNING << "Navigation tool not available" << endmsg;
            return;
         }
      
         const Algorithm* parentAlg = dynamic_cast<const Algorithm*>(ntool->parent()); // this is the way to get parent alg name (TrigSteer_L2/EF) ugh we need better way.
         if ( !parentAlg ) {
            m_config->getMsgStream() << MSG::WARNING << "No parent algorithm for the sequence" << endmsg;
            return;
         }

         std::string name( parentAlg->name()+":sequence_" + telabel );
         m_timer = timerSvc->addItem(name); // TrigSteer is hardcoded but we can find it via alg->parentAlg()->name() which should be steering instance name;
         m_config->getMsgStream() << MSG::DEBUG << "Booked timer: " << name << endmsg;

      }
      else
         m_config->getMsgStream() << MSG::WARNING << "Can not find TE label for id: " << outputType << endmsg;
   }

   // Get TrigROBDataProvider_RTT for data pre-fetch test 
   if( m_robDataProvider.retrieve().isFailure() ) {
      m_config->getMsgStream() << MSG::ERROR << "can't get ROBDataProviderSvc" << endmsg;
   } else{
      m_trigROBDataProviderRTT = SmartIF<ITrigROBDataProviderSvc_RTT>( &*m_robDataProvider );
      if (m_trigROBDataProviderRTT.isValid()) {
         if (m_config && m_config->getMsgLvl() <= MSG::DEBUG)
            m_config->getMsgStream() << MSG::DEBUG << "A ROBDataProviderSvc implementing the trig interface ITrigROBDataProviderSvc_RTT was found."<< endmsg;
      }
   }
   return;
}


bool Sequence::reset() {

   m_alreadyExecuted = false;
   m_outputTEs.clear();
   m_execErrorCode = HLT::OK;
   if ( m_timer) m_timer->reset();

   if ( m_firstAlgo ) {
      if (m_config->getMsgLvl() <= MSG::VERBOSE)
         m_config->getMsgStream() << MSG::VERBOSE << "Sequence about to do reset() of 1st algo: " << m_firstAlgo->name()<< endmsg;
      if ( !m_firstAlgo->reset() ) {
	m_config->getMsgStream() << MSG::WARNING << "Sequence reset() of 1st algo problematic " << m_firstAlgo->name()<< endmsg;
	return false;
      }
   }

   for (unsigned int i = 0; i < m_nextAlgos.size(); i++) {
      if (m_config->getMsgLvl() <=MSG::VERBOSE)
         m_config->getMsgStream() << MSG::VERBOSE << "Sequence about to do reset() of next algo: " << m_nextAlgos[i]->name() << endmsg;
      if ( !m_nextAlgos[i]->reset() ) {
	m_config->getMsgStream() << MSG::WARNING << "Sequence reset() of next algo problematic " << m_nextAlgos[i]->name() << endmsg;
	return false;
      }
   }

   // check previous Sequences:
   // in some situations, no other chain uses the recursively executed sequences,
   // so we need to make sure they are reset !!!
   bool recursiveResetStatus = true;

   for ( std::vector< HLT::Sequence* >::const_iterator seq = m_previousSequences.begin();
         seq != m_previousSequences.end(); ++seq) {
      // if in "already executed" state, reset it !
      if ( (*seq)->m_alreadyExecuted ) {
         recursiveResetStatus = recursiveResetStatus && (*seq)->reset();
      }
   }
   
   if(m_topoStartFromSequence!=0  &&  m_topoStartFromSequence->m_alreadyExecuted) {
      recursiveResetStatus = recursiveResetStatus && m_topoStartFromSequence->reset();
   }

   return recursiveResetStatus;
}


bool Sequence::resetROBRequestPreparation()
{
  m_prepRobReqAlreadyExecuted = false;
  m_prepRobReqErrorCode = HLT::OK;

  // check previous Sequences:
  // in some situations, no other chain uses the recursively executed sequences,
  // so we need to make sure they are reset !!!
  bool recursiveResetStatus = true;
  for ( std::vector< HLT::Sequence* >::const_iterator seq = m_previousSequences.begin();
        seq != m_previousSequences.end(); ++seq) {
     // if in "already executed" state, reset it !
     if ( (*seq)->m_prepRobReqAlreadyExecuted ) {
        recursiveResetStatus = recursiveResetStatus && (*seq)->resetROBRequestPreparation();
    }
  }
  return recursiveResetStatus;
}


int Sequence::producedOutputTEs() const
{
  if (!m_alreadyExecuted) return -1;
  return m_outputTEs.size();
}


int Sequence::activeOutputTEs() const
{
  if (!m_alreadyExecuted) return -1;

  return activeOutputTEsInternal();
}

class OPITimerHelper {
public:
  OPITimerHelper(TrigTimer* timer, TrigOperationalInfo* opi, const std::string& label) 
    : m_timer(timer), m_opi(opi), m_label(label) {
    if (m_timer) m_timer->start();
    if (m_opi) m_opi->set(m_label+":Start", 1.0);
  }
  
  ~OPITimerHelper() {
    if(m_timer) { 
      m_timer -> stop();
      if(m_opi) m_opi -> set(m_label+":ElapsedTime", m_timer -> elapsed());
    }
  }

private:
  TrigTimer *m_timer;
  TrigOperationalInfo *m_opi;
  const std::string m_label;
};


HLT::ErrorCode Sequence::execute()
{
   // in case this sequence was executed before, just return the result:

   // collect operational information: create new TrigOperationalInfo
   std::string te_label;
   TrigOperationalInfo *steer_opi = 0;
   if(m_config -> getSteeringOPILevel() > 0 && m_config -> getSteeringOPI()) {
      steer_opi = m_config -> getSteeringOPI();
      TrigConf::HLTTriggerElement::getLabel(m_outputTeType, te_label);
      steer_opi -> set("SEQUENCE:"+te_label, (m_alreadyExecuted ? 1. : 0. ));
   }

   if (m_alreadyExecuted) {
      if (m_config->getMsgLvl() <=MSG::DEBUG) {
         m_config->getMsgStream() << MSG::DEBUG << "Sequence already executed! -> doing nothing"
                                  << endmsg;
         m_config->getMsgStream() << MSG::DEBUG << "Sequence: inputTEs = ";
         for (std::vector< unsigned int >::const_iterator it = m_inputTeTypes.begin();
              it != m_inputTeTypes.end(); ++it) {
            m_config->getMsgStream() << MSG::DEBUG << (*it) << ", ";
         }
         m_config->getMsgStream() << MSG::DEBUG << " outputTE = " << m_outputTeType << " firstAlgo = " << m_firstAlgo->name() << endmsg;
      }
      return m_execErrorCode;
   }

   // Whether the execution fails or not we don't want to re-run, so set in the beginning
   m_alreadyExecuted = true;
   m_execErrorCode = OK;

   // if it exists, EXECUTE SEQUENCE TOPO_START_FROM if not already done so
   if(m_topoStartFromSequence!=0  &&  ! m_topoStartFromSequence->m_alreadyExecuted) {
      ErrorCode ec = m_topoStartFromSequence->execute();
      m_execErrorCode = m_execErrorCode > ec ? m_execErrorCode : ec;
      if ( m_execErrorCode.action() > HLT::Action::CONTINUE ) return m_execErrorCode;
   }


   // execute input sequences if not already done so
   for ( HLT::Sequence* seq : m_previousSequences) {
      // if not executed already, do it now!
      // TB we need to care about errors here
      if ( ! seq->m_alreadyExecuted ) {
         if(steer_opi) {
            std::string rte_label;
            TrigConf::HLTTriggerElement::getLabel( seq->outputTEType(), rte_label);
            steer_opi -> set("SEQUENCE:"+rte_label+":Previous", 1.0);
         }
         ErrorCode ec = seq->execute();
         m_execErrorCode = m_execErrorCode > ec ? m_execErrorCode : ec;
         if ( m_execErrorCode.action() > HLT::Action::CONTINUE ) return m_execErrorCode;
      }
   }


   //ScopeTimer scopetimer(m_timer); // that is all what we need for timing, it will be stoped on first return or at end
   OPITimerHelper scopetimer(m_timer, steer_opi, "SEQUENCE:"+te_label);

   if (m_config->getMsgLvl() <=MSG::DEBUG)
      m_config->getMsgStream() << MSG::DEBUG << "Sequence executing, producing output type = "
                               << m_outputTeType << endmsg;


   // Execute the algorithms.
   m_outputTEs.clear();

   // The first one creates the TEs:
   try {
      if (m_config->getMsgLvl() <=MSG::DEBUG)
         m_config->getMsgStream() << MSG::DEBUG << "Executing first algo "
                                  <<  m_firstAlgo->name() << endmsg;

      ErrorCode ec =  m_firstAlgo->createAndProcessTEs( m_inputTeTypes,  m_outputTeType,
                                                        m_outputTEs, m_topoStartFrom);
      m_execErrorCode = m_execErrorCode > ec ? m_execErrorCode : ec;

      if (m_config->getMsgLvl() <=MSG::DEBUG)
         m_config->getMsgStream() << MSG::DEBUG << "Sequence '" << te_label
                                  << "': EC from firstalgo->createAndProcessTEs(..) =  " << HLT::strErrorCode( m_execErrorCode ) <<  endmsg;

      if ( m_execErrorCode.action() > HLT::Action::CONTINUE ) {
         m_config->getMsgStream() << MSG::WARNING 
                                  << "Sequence '" << te_label << "' got error back while executing first algorithm '"
                                  << m_firstAlgo->name() << "':" 
                                  << HLT::strErrorCode( m_execErrorCode ) <<  endmsg;
      }

      // Check if event timeout was reached
      if ( Athena::Timeout::instance().reached() ) {
         if (m_config->getMsgLvl() <= MSG::DEBUG)
            m_config->getMsgStream() << MSG::DEBUG << "Timeout reached executing algo " << m_firstAlgo->name() << endmsg;
      
         return m_execErrorCode = HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::TIMEOUT);
      }

      if (m_execErrorCode.action() > HLT::Action::CONTINUE || m_outputTEs.size() == 0) return m_execErrorCode;
        

      // execute following Algorithms of this Sequence, as long as there is at least
      // 1 active output TE AND the previous Algorithm's status was < STOP_THIS_CHAIN EC
      for (unsigned int j = 0; j <  m_nextAlgos.size(); ++j) {

         if (activeOutputTEsInternal() == 0) {
            if (m_config->getMsgLvl() <=MSG::DEBUG)
               m_config->getMsgStream() << MSG::DEBUG << "No more valid TEs ==> exiting sequence before "
                                        <<  m_nextAlgos[j]->name() << endmsg;
            return m_execErrorCode;
         }

         if (m_config->getMsgLvl() <= MSG::DEBUG)
            m_config->getMsgStream() << MSG::DEBUG << "Executing next algo "
                                     << m_nextAlgos[j]->name() << endmsg;
     
         ErrorCode ec = m_nextAlgos[j]->processTEs(m_outputTEs);
         m_execErrorCode = m_execErrorCode > ec ? m_execErrorCode : ec;

         if (m_config->getMsgLvl() <=MSG::DEBUG)
            m_config->getMsgStream() << MSG::DEBUG << "Sequence "
                                     << " EC from algo->processTEs(..) =  " << HLT::strErrorCode( m_execErrorCode ) <<  endmsg;

         if ( ec.action() > HLT::Action::CONTINUE ) {
            m_config->getMsgStream() << MSG::WARNING 
                                     << "Sequence got error back while executing algorithm: " 
                                     << m_nextAlgos[j]->name() << " " << HLT::strErrorCode( m_execErrorCode ) <<  endmsg;
         }

         // Check if event timeout was reached
         if ( Athena::Timeout::instance().reached() ) {
            if (m_config->getMsgLvl() <= MSG::DEBUG)
               m_config->getMsgStream() << MSG::DEBUG << "Timeout reached executing algo " << m_nextAlgos[j]->name() << endmsg;
	
            return m_execErrorCode = HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::TIMEOUT);
         }

         if ( m_execErrorCode.action() > HLT::Action::CONTINUE )
            return m_execErrorCode;

      }
   } catch (...) {
      m_execErrorCode = trigExceptionHandler(&m_config->getMsgStream());
   }    

   return m_execErrorCode;
}


/**
   calls
 */
HLT::ErrorCode Sequence::prepareRobRequests()
{
  // this variable enables the monitor of the pre-fetching with the trigROBDataProviderSvc
  bool do_prefetching_test = m_trigROBDataProviderRTT.isValid() && m_trigROBDataProviderRTT->isPrefetchingAtAlgoLevel();

  // in case this sequence was executed before
  if (m_prepRobReqAlreadyExecuted) {
    // print some debug info
    if (m_config->getMsgLvl() <=MSG::DEBUG) {
      std::string telabel;
      TrigConf::HLTTriggerElement::getLabel(m_outputTeType, telabel);

      m_config->getMsgStream() << MSG::DEBUG << "Sequence: prepareRobRequests for '" << telabel << "' already executed! -> doing nothing"
                               << endmsg;
      m_config->getMsgStream() << MSG::DEBUG << "Sequence: inputTEs = ";
      for (std::vector< unsigned int >::const_iterator it = m_inputTeTypes.begin();
           it != m_inputTeTypes.end(); ++it) {
        m_config->getMsgStream() << MSG::DEBUG << (*it) << ", ";
      }
      m_config->getMsgStream() << MSG::DEBUG << " outputTE = " << m_outputTeType << " firstAlgo = " << m_firstAlgo->name() << endmsg;
    }
    // and just return the result
    return m_prepRobReqErrorCode;
  }

  // Whether the execution fails or not we don't want to re-run, so set in the beginning
  // this gets reset in resetPreparation()
  m_prepRobReqAlreadyExecuted = true;
  m_prepRobReqErrorCode = OK;

  // In case the sequence has a predecessor in this level, we execute
  // the prepareRobRequests only for the predecessor and not for the
  // sequence itself. If a sequence has multiple input TE's then all
  // those are executed. Only if there is no predecessor, the sequence
  // calls prepareRobRequest for its own algorithms

  //FPP: latter sentence is not true anymore for Run2: we want now to execute all the sequences, not only the predecesors

  if( m_previousSequences.size()>0 ) {
    if (m_config->getMsgLvl() <=MSG::DEBUG) {
      std::string telabel;
      TrigConf::HLTTriggerElement::getLabel(m_outputTeType, telabel);
      m_config->getMsgStream() << MSG::DEBUG << "Sequence " << telabel << " has input sequence at this step, calling prepareRobRequest for those" << endmsg;
    }
    // call prepareRobRequests() for all input Sequences
    for ( std::vector< HLT::Sequence* >::const_iterator seq = m_previousSequences.begin();
	  seq != m_previousSequences.end(); ++seq) {      

      // If the previous sequence was executed in the same step, the output TE is not created yet, so prefecthing is useless      
      if( (*seq)->outputTEs().size() == 0) continue;

      // ignore, if already executed
      if ( (*seq)->m_alreadyExecuted ) continue;
      
      HLT::ErrorCode ec = (*seq)->prepareRobRequests();
      m_prepRobReqErrorCode = m_prepRobReqErrorCode > ec ? m_prepRobReqErrorCode : ec;
      
      // abandon in case of failure
      if ( m_prepRobReqErrorCode.action() > HLT::Action::CONTINUE ) 
	return m_prepRobReqErrorCode;
    }
    //FPP: comment this to run the current sequence (run on all the sequences, not only the first)
    //return m_prepRobReqErrorCode;
  }

  // debug output
  if (m_config->getMsgLvl() <=MSG::DEBUG) {
    std::string telabel;
    TrigConf::HLTTriggerElement::getLabel(m_outputTeType, telabel);
    m_config->getMsgStream() << MSG::DEBUG << "Sequence " << telabel << ": preparing ROB requests for all fex-type of the " << m_nextAlgos.size()+1 << " algos of this sequence" << endmsg;
  }


  try {

     // execute ROB preparation of all algorithms of this Sequence

    // a few comments:

    // This code runs before any hlt algorithms are executed, hence
    // before any HLT output TEs are created. That means the
    // preparation can only be done for sequences that have L1
    // thresholds as input

    // This does not run on HYPO algorithms

    // The continues as long as the algorithm's does not return an
    // error above CONTINUE or a time-out is reached

    for (unsigned int j = 0; j <  m_nextAlgos.size()+1; ++j) {
      
      // FPP: comment this in order to run on all the algo of the sequence (needed in the merged system)
      // if (j>0) break; // only for the first algo the processRobRequests can be run 
     

      HLT::Algo* alg = ( j==0 ? m_firstAlgo : m_nextAlgos[j-1] );

      HLT::TECreateAlgo * te_create_alg = dynamic_cast<HLT::TECreateAlgo*>(alg);

      if(te_create_alg == 0) continue; // a hypo algo

      if (m_config->getMsgLvl() <=MSG::DEBUG){
	std::string teName;
	TrigConf::HLTTriggerElement::getLabel(m_outputTeType, teName);
        m_config->getMsgStream() << MSG::DEBUG << "  calling processRobRequests of TECreateAlgo '" <<  te_create_alg->name() << "' with outputTE = " << teName << endmsg;
      }

      //test of the pre-fetching: clear the pre-fetching list
      if (do_prefetching_test){
	m_config->robRequestInfo()->clearRequestScheduledRobIDs();
      }

      ErrorCode ec =  te_create_alg->processRobRequests( m_inputTeTypes );
      m_prepRobReqErrorCode = std::max(m_prepRobReqErrorCode, ec);     

      if (m_config->getMsgLvl() <=MSG::DEBUG)
        m_config->getMsgStream() << MSG::DEBUG
                                 << "EC from algo->processRobRequests(..) =  " 
                                 << HLT::strErrorCode( m_prepRobReqErrorCode ) <<  endmsg;
      

      if ( m_prepRobReqErrorCode.action() > HLT::Action::CONTINUE )
        m_config->getMsgStream() << MSG::WARNING 
                                 << "Sequence got error back while executing algo->processRobRequests(..): "
                                 << te_create_alg->name() << " " << HLT::strErrorCode( m_prepRobReqErrorCode ) <<  endmsg;

      //test of the pretching: fill the pre-fetching list 
      if (do_prefetching_test && m_robDataProvider.isValid()){
	std::string pref_name = alg->name() + "_pref";
	if (m_config->getMsgLvl() <=MSG::INFO) m_config->getMsgStream() << MSG::INFO <<"Forcing trigROBDataProvider_RTT.addROBData: Algorithm "<< te_create_alg->name() <<" scheduled "<<m_config->robRequestInfo()->requestScheduledRobIDs().size() <<" ROBs"<<endmsg;
	m_robDataProvider->addROBData(m_config->robRequestInfo()->requestScheduledRobIDs(),pref_name);
      }

      // Check if event timeout was reached
      if ( Athena::Timeout::instance().reached() ) {
        if (m_config->getMsgLvl() <= MSG::DEBUG)
          m_config->getMsgStream() << MSG::DEBUG << "Timeout reached executing algo->processRobRequests(..) " << m_nextAlgos[j]->name() << endmsg;
        
        return m_prepRobReqErrorCode = HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::TIMEOUT);
      }

      // return if there is any error
      if ( m_prepRobReqErrorCode.action() > HLT::Action::CONTINUE )
        return m_prepRobReqErrorCode;
    }
  } catch (...) {
    m_execErrorCode = trigExceptionHandler(&m_config->getMsgStream());
  }
  
  return m_execErrorCode;
}


unsigned int Sequence::activeOutputTEsInternal() const
{
  unsigned int activeOutputTEs = 0;

  for (std::vector<TriggerElement*>::const_iterator iter = m_outputTEs.begin();
       iter != m_outputTEs.end(); iter++)
    if ((*iter)->getActiveState()) activeOutputTEs++;

  return activeOutputTEs;
}

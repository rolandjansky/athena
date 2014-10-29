/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EvgenProdTools/CountHepMC.h"
#include "GaudiKernel/IEventProcessor.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ClassID.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/errorcheck.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include <cmath>
#include <cassert>

using namespace std;


CountHepMC::CountHepMC(const std::string& name, ISvcLocator* pSvcLocator) :
  GenBase(name, pSvcLocator),
  m_nPass(0),
  m_wtpass(0),
  m_wthepmc(0),
  m_wtall(0),
  /// @todo Why 'new' these?
  m_streamAlgs(new vector<Algorithm*>),
  m_requireAlgs(new vector<Algorithm*>),
  m_vetoAlgs(new vector<Algorithm*>),
  m_acceptAlgs(new vector<Algorithm*>)
{
  declareProperty("RequestedOutput", m_nCount=5000);
  declareProperty("CheckStream",     m_streamNames);
  declareProperty("FirstEvent",      m_firstEv=1);
  declareProperty("CorrectHepMC",    m_corHepMC=false);
  declareProperty("CorrectEventID",  m_corEvtID=false);
  declareProperty("UseEventWeight",  m_useEventWeight=true);

  m_streamNames.declareUpdateHandler(&CountHepMC::streamAlgsHandler, this);
}


CountHepMC::~CountHepMC() {
  /// @todo Memory leak on m_streamAlgs?
  //delete m_streamAlgs;
  delete m_requireAlgs;
  delete m_vetoAlgs;
  delete m_acceptAlgs;
}


StatusCode CountHepMC::initialize() {
  CHECK(GenBase::initialize());

  m_nPass = 0;
  m_wtpass = 0.;
  m_wthepmc = 0.;
  m_wtall = 0.;

  isStreamRechecked = false;

  // Decode the required Algorithms.
  // The logic is the following:
  //  a. The event is accepted if the list is empty.
  //  b. The event is rejected unless all Algorithms in the required list have
  //     been executed and have indicated that their filter passed.
  StatusCode status = decodeStreamAlgs( );
  if ( !status.isSuccess() ) ATH_MSG_FATAL("Unable to decode filter Algs.");
  return status;
}


StatusCode CountHepMC::execute() {
  if (!isStreamRechecked) {
    isStreamRechecked = true;
    if (decodeAlgRequires().isFailure() ) {
      ATH_MSG_ERROR("StatusCode Failure in decodeAlgRequires");
      return StatusCode::FAILURE;
    }
    if (decodeAlgVetos().isFailure() ) {
      ATH_MSG_ERROR("StatusCode Failure in decodeAlgVetos");
      return StatusCode::FAILURE;
    }
    if (decodeAlgAccepts().isFailure() ) {
      ATH_MSG_ERROR("StatusCode Failure in decodeAlgAccepts");
      return StatusCode::FAILURE;
    }
  }

  // Collect event weights for summation
  double weight = 1;
  if (event_const()->weights().size() > 0 && m_useEventWeight) {
    weight = event_const()->weights()[0];
  } else if (m_useEventWeight) {
    ATH_MSG_DEBUG("No weight to retrieve ");
  }
  ATH_MSG_DEBUG("Current weight = " << weight);
  if (weight == 0.) weight=1;
  m_wtall += weight;
  if (isTestHepMCPassed()) m_wthepmc += weight;

  /// @todo Replace the old event ?
  if (isEventAccepted()) {
    m_wtpass += weight;
    m_nPass++;
    ATH_MSG_DEBUG("Current count = " << m_nPass);

    // Fix the event number
    int newnum = m_nPass + m_firstEv - 1;
    if (m_corHepMC) {
      // Replace the old event
      HepMC::GenEvent* new_evt = new HepMC::GenEvent(*event());
      new_evt->set_event_number(newnum);
      events()->pop_back();
      events()->push_back(new_evt);
    }

    if (m_corEvtID) {
      // Change the EventID in the eventinfo header
      const EventInfo* pInputEvt(0);
      if (evtStore()->retrieve(pInputEvt).isSuccess()) {
        assert(pInputEvt);
        EventID* eventID = const_cast<EventID*>(pInputEvt->event_ID());
        eventID->set_event_number(newnum);
        ATH_MSG_DEBUG("Set new event number in event_ID");
      } else {
        ATH_MSG_ERROR("No EventInfo object found");
        return StatusCode::SUCCESS;
      }
    }
  }

  if (m_nPass == m_nCount) {
    ATH_MSG_INFO("Stopping the event processing...." << m_nPass << "/" << m_nCount);
    ATH_MSG_INFO("Weight count...." << m_wtpass << "/" << m_wthepmc << "/" << m_wtall);
    ATH_MSG_INFO("Resulting filtering efficiency = " << m_wtpass/m_wthepmc);
    cout << "MetaData: GenFiltEff = " << scientific << m_wtpass/m_wthepmc << fixed << endl;
    IEventProcessor* apm = 0;
    IService* ssvc = 0;
    static const bool CRE(false);
    StatusCode sc = serviceLocator()->getService("AthenaEventLoopMgr", ssvc, CRE);
    if (sc.isSuccess()) {
      sc = ssvc->queryInterface(IEventProcessor::interfaceID(), pp_cast<void>(&apm));
    } else {
      ATH_MSG_WARNING("No EventLoop Manager found ");
    }
    if (sc.isSuccess()) {
      if ((apm->stopRun()).isFailure()) {
        ATH_MSG_ERROR("StatusCode Failure in stopRun()");
        return StatusCode::FAILURE;
      }
      apm->release();
    } else {
      ATH_MSG_WARNING("No EventLoop Manager specified ");
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode CountHepMC::finalize() {
  ATH_MSG_INFO("Events passing all checks and written = " << m_nPass);
  return StatusCode::SUCCESS;
}


StatusCode CountHepMC::decodeAlgorithms( StringArrayProperty& theNames, vector<Algorithm*>* theAlgs ) {
  // Reset the list of Algorithms
  theAlgs->clear( );

  IAlgManager* theAlgMgr;
  StatusCode result = serviceLocator( )->getService( "ApplicationMgr", IAlgManager::interfaceID(), *pp_cast<IInterface>(&theAlgMgr) );
  if ( result.isSuccess( ) ) {

    // Build the list of Algorithms from the names list
    const vector<string> nameList = theNames.value( );
    vector<string>::const_iterator it;
    vector<string>::const_iterator itend = nameList.end( );
    for (it = nameList.begin(); it != itend; it++) {

      // Check whether the suppied name corresponds to an existing
      // Algorithm object.
      string theName = (*it);
      IAlgorithm* theIAlg;
      Algorithm*  theAlgorithm;
      result = theAlgMgr->getAlgorithm( theName, theIAlg );
      if ( result.isSuccess( ) ) {
        try{
          theAlgorithm = dynamic_cast<Algorithm*>(theIAlg);
        } catch(...){
          result = StatusCode::FAILURE;
        }
      }

      if (result.isSuccess( )) {
        // Check that the specified algorithm doesn't already exist in the list
        vector<Algorithm*>::iterator ita;
        vector<Algorithm*>::iterator itaend = theAlgs->end( );
        for (ita = theAlgs->begin(); ita != itaend; ita++) {
          Algorithm* existAlgorithm = (*ita);
          if ( theAlgorithm == existAlgorithm ) {
            result = StatusCode::FAILURE;
            break;
          }
        }
        if ( result.isSuccess( ) ) {
          theAlgs->push_back( theAlgorithm );
        }
      } else {
        ATH_MSG_INFO(theName << " doesn't exist - ignored");
      }
    }
    result = StatusCode::SUCCESS;
  } else {
    ATH_MSG_FATAL("Can't locate ApplicationMgr!!!");
  }
  return result;
}


StatusCode CountHepMC::decodeStreamAlgs( ) {
  StatusCode result = decodeAlgorithms( m_streamNames, m_streamAlgs );
  if (m_streamNames.value().size()) {
    ATH_MSG_DEBUG("Found Stream names: ");
    vector<string>::const_iterator first = m_streamNames.value().begin();
    vector<string>::const_iterator last  = m_streamNames.value().end();
    for (; first != last; ++first) ATH_MSG_DEBUG(*first);
  } else {
    ATH_MSG_DEBUG("No Stream hook found ");
  }

  if (m_streamAlgs->size()) {
    ATH_MSG_DEBUG("Found Stream  algs: ");
    vector<Algorithm*>::const_iterator first = m_streamAlgs->begin();
    vector<Algorithm*>::const_iterator last  = m_streamAlgs->end();
    for (; first != last; ++first) ATH_MSG_DEBUG((*first)->name());
  } else {
    ATH_MSG_DEBUG("No Stream Alg found ");
  }

  return result;
}


void CountHepMC::streamAlgsHandler( Property& /* theProp */ ) {
  StatusCode sc = decodeAlgorithms( m_streamNames, m_streamAlgs );
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL("Unable to decode Stream list");
    throw GaudiException("Unable to decode Stream list", name(), sc);
  }
}


StatusCode CountHepMC::decodeAlgRequires( ) {
  StatusCode result =  StatusCode::SUCCESS;
  StringArrayProperty sprop;

  if (  ! m_streamAlgs->empty( ) ) {
    vector<Algorithm*>::iterator it;
    vector<Algorithm*>::iterator itend = m_streamAlgs->end( );
    for (it = m_streamAlgs->begin(); it != itend; it++) {
      Algorithm* theAlgorithm = (*it);
      try {
        const Property &prop = theAlgorithm->getProperty("RequireAlgs");
        const StringArrayProperty &saprop = dynamic_cast<const StringArrayProperty&>( prop ); ;
        if (saprop.value().size()) {
          sprop=saprop;
          ATH_MSG_DEBUG("Found Stream " << theAlgorithm->name() << " RequireNames: ");
          vector<string>::const_iterator first = saprop.value().begin();
          vector<string>::const_iterator last  = saprop.value().end();
          for (; first != last; ++first) {
            ATH_MSG_DEBUG(*first << " ");
          }
          result = decodeAlgorithms( sprop, m_requireAlgs );
        }
        else {
          ATH_MSG_DEBUG("No RequireAlgs found in stream found ");
        }
      } catch (...) {
        ATH_MSG_DEBUG(theAlgorithm->name() << " stream has no RequireAlgs fields");
      }
    }
  }
  return result;
}


StatusCode CountHepMC::decodeAlgVetos( ) {
  StatusCode result =  StatusCode::SUCCESS;
  StringArrayProperty sprop;

  if (  ! m_streamAlgs->empty( ) ) {
    vector<Algorithm*>::iterator it;
    vector<Algorithm*>::iterator itend = m_streamAlgs->end( );
    for (it = m_streamAlgs->begin(); it != itend; it++) {
      Algorithm* theAlgorithm = (*it);
      try {
        const Property &prop = theAlgorithm->getProperty("VetoAlgs");
        const StringArrayProperty &saprop = dynamic_cast<const StringArrayProperty&>( prop ); ;
        if (saprop.value().size()) {
          sprop=saprop;
          ATH_MSG_DEBUG("Found Stream " << theAlgorithm->name() << " VetoNames: ");
          vector<string>::const_iterator first = saprop.value().begin();
          vector<string>::const_iterator last  = saprop.value().end();
          for (; first != last; ++first) {
            ATH_MSG_DEBUG(*first << " ");
          }
          result = decodeAlgorithms( sprop, m_vetoAlgs );
        }
        else {
          ATH_MSG_DEBUG("No VetoAlgs found in stream found ");
        }
      } catch (...) {
        ATH_MSG_DEBUG(theAlgorithm->name() << " stream has no VetoAlgs fields");
      }
    }
  }
  return result;
}



StatusCode CountHepMC::decodeAlgAccepts( ) {
  StatusCode result =  StatusCode::SUCCESS;
  StringArrayProperty sprop;

  if (  ! m_streamAlgs->empty( ) ) {
    vector<Algorithm*>::iterator it;
    vector<Algorithm*>::iterator itend = m_streamAlgs->end( );
    for (it = m_streamAlgs->begin(); it != itend; it++) {
      Algorithm* theAlgorithm = (*it);
      try {
        const Property &prop = theAlgorithm->getProperty("AcceptAlgs");
        const StringArrayProperty &saprop = dynamic_cast<const StringArrayProperty&>( prop ); ;
        if (saprop.value().size()) {
          sprop=saprop;
          ATH_MSG_DEBUG("Found Stream " << theAlgorithm->name() << " AcceptNames: ");
          vector<string>::const_iterator first = saprop.value().begin();
          vector<string>::const_iterator last  = saprop.value().end();
          for (; first != last; ++first) {
            ATH_MSG_DEBUG(*first << " ");
          }
          result = decodeAlgorithms( sprop, m_acceptAlgs );
        }
        else {
          ATH_MSG_DEBUG("No AcceptAlgs found in stream found ");
        }
      } catch (...) {
        ATH_MSG_DEBUG(theAlgorithm->name() << " stream has no AcceptAlgs fields");
      }
    }
  }
  return result;
}


bool CountHepMC::isEventAccepted( ) const {
  bool result = true;

  // Loop over all Algorithms in the required list to see whether all have been
  // executed and have their filter passed flag set. Any mismatch causes the
  // event to be rejected.
  if (result) {
    for (vector<Algorithm*>::iterator it = m_requireAlgs->begin(); it != m_requireAlgs->end(); it++) {
      Algorithm* theAlgorithm = (*it);
      if ( ! theAlgorithm->isExecuted( ) || ! theAlgorithm->filterPassed( ) ) {
        result = false;
        ATH_MSG_DEBUG("Require Algs not passed! Failure in " << theAlgorithm->name() );
        break;
      }
    }
  }

  // Loop over all Algorithms in the accept list to see whether any have been
  // executed and have their filter passed flag set. Any match causes the event
  // to be provisionally accepted.
  if (result && !m_acceptAlgs->empty()) {
    result = false;
    for (std::vector<Algorithm*>::iterator it = m_acceptAlgs->begin(); it != m_acceptAlgs->end(); it++) {
      Algorithm* theAlgorithm = (*it);
      if ( theAlgorithm->isExecuted( ) && theAlgorithm->filterPassed( ) ) {
        result = true;
        break;
      }
    }
  }

  // Loop over all Algorithms in the veto list to see whether any have been
  // executed and have their filter passed flag set. Any match causes the event
  // to be rejected.
  if (result) {
    for (std::vector<Algorithm*>::iterator it = m_vetoAlgs->begin(); it != m_vetoAlgs->end(); it++) {
      Algorithm* theAlgorithm = (*it);
      if ( theAlgorithm->isExecuted( ) && theAlgorithm->filterPassed( ) ) {
        ATH_MSG_DEBUG("Veto Algs not passed! " << theAlgorithm->name() );
        result = false;
        break;
      }
    }
  }

  return result;
}


bool CountHepMC::isTestHepMCPassed( ) const {
  bool result = true;
  if ( result && ! m_requireAlgs->empty( ) ) {
    vector<Algorithm*>::iterator it;
    vector<Algorithm*>::iterator itend = m_requireAlgs->end( );
    for (it = m_requireAlgs->begin(); it != itend; it++) {
      Algorithm* theAlgorithm = (*it);
      if ( theAlgorithm->name() == "TestHepMC" ) {
        if ( ! theAlgorithm->isExecuted( ) || ! theAlgorithm->filterPassed( ) ) {
          result = false;
          break;
        }
      }
    }
  }
  return result;
}

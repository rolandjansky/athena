///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// AthFilterAlgorithm.cxx
// Implementation file for class AthFilterAlgorithm
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////

// AthenaBaseComps includes
#include "AthenaBaseComps/AthFilterAlgorithm.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadHandle.h"

// STL includes

// Framework includes
#include "GaudiKernel/Property.h"


///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
AthFilterAlgorithm::AthFilterAlgorithm( const std::string& name,
                                        ISvcLocator* pSvcLocator ) :
  ::AthAlgorithm( name, pSvcLocator ),
  m_cutID ( 0 ),
  m_cutFlowSvc("CutFlowSvc/CutFlowSvc", name)
{
  //
  // Property declaration
  //
  //declareProperty( "Property", m_nProperty );

  declareProperty("CutFlowSvc", m_cutFlowSvc,
                  "handle to the ICutFlowSvc instance this filtering algorithm"
                  " will use for building the flow of cuts.");

  declareProperty("FilterDescription", m_filterDescr = "N/A",
                  "describe to the cutflowsvc what this filter does.")
    ->declareUpdateHandler(&AthFilterAlgorithm::doNotResetSelfDescription, this);
  m_resetSelfDescription=true;
}

// Destructor
///////////////
AthFilterAlgorithm::~AthFilterAlgorithm()
{}

///////////////////////////////////////////////////////////////////
// Const methods:
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// Non-const methods:
///////////////////////////////////////////////////////////////////

/** Initialization method invoked by the framework. This method is responsible
 *  for any bookkeeping of initialization required by the framework itself.
 *  It will in turn invoke the initialize() method of the derived algorithm,
 * and of any sub-algorithms which it creates.
 */
StatusCode
AthFilterAlgorithm::sysInitialize()
{
  // ---- stolen from GaudiKernel/Algorithm::sysInitialize -------
  // Bypass the initialization if the algorithm
  // has already been initialized.
  if ( Gaudi::StateMachine::INITIALIZED <= FSMState() ) return StatusCode::SUCCESS;

  // Set the Algorithm's properties
  ATH_CHECK(setProperties());

  // Bypass the initialization if the algorithm is disabled.
  // Need to do this after setProperties.
  if ( !isEnabled( ) ) return StatusCode::SUCCESS;

  // ---- stolen from GaudiKernel/Algorithm::sysInitialize ------- END ---

  // register ourselves with the cutFlowSvc
  if ( cutFlowSvc().retrieve().isSuccess()) {
    m_cutID = cutFlowSvc()->registerFilter(this->name(), m_filterDescr);
    if (0 == m_cutID) {
      ATH_MSG_INFO("problem registering myself with cutflow-svc");
    } else {
      ATH_MSG_VERBOSE("registered with cutflow-svc");
    }
  }

  ATH_CHECK( m_eventInfoKey.initialize() );

  // re-direct to base class...
  return AthAlgorithm::sysInitialize();
}

/// Set the filter passed flag to the specified state
void
AthFilterAlgorithm::setFilterPassed( bool state ) const
{
  AthAlgorithm::setFilterPassed(state);

  if (state) {
    double evtWeight=1.0;

    const EventContext& ctx = Gaudi::Hive::currentContext();
    SG::ReadHandle<xAOD::EventInfo> evtInfo (m_eventInfoKey, ctx);
    // Only try to access the mcEventWeight if we are running on Monte Carlo, duhhh!
    if ( evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION) ) {
      evtWeight = evtInfo->mcEventWeight();
    }
    m_cutFlowSvc->addEvent(m_cutID,evtWeight);
  }
}

/// @brief helper function to ease the setting of this filter's description
/// in derived classes
void
AthFilterAlgorithm::setFilterDescription(const std::string& descr)
{
  if( cutFlowSvc()==0 ){
    m_filterDescr = descr;
  }
  else if( m_resetSelfDescription and cutID() ){
    cutFlowSvc()->setFilterDescription(cutID(),descr);
  }
  else{
    ATH_MSG_INFO("problem setting filter description with cutflow-svc");
  }

  return;
}

const SG::ReadHandleKey<xAOD::EventInfo>&
AthFilterAlgorithm::eventInfoKey() const
{
  return m_eventInfoKey;
}


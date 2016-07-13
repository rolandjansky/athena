/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HistorySimSelector.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class include
#include "HistorySimSelector.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"

/** Constructor **/
ISF::HistorySimSelector::HistorySimSelector(const std::string& t, const std::string& n, const IInterface* p)
  : ISimulationSelector(t,n,p)
  , m_prevSimSvcHandle("UnspecifiedSimulationService", n)
  , m_prevSimSvcID(ISF::fUndefinedSimID)
  , m_checkSameGeoID(false)
{
  declareInterface<ISF::ISimulationSelector>(this);

  declareProperty( "PrevSimSvc",
                   m_prevSimSvcHandle,
                   "Check if particle was recently simulated by the given SimSvc." );

  declareProperty( "RequiresUnchangedGeoID",
                   m_checkSameGeoID,
                   "Check if GeoID of particle did not change." );

}

/** Destructor **/
ISF::HistorySimSelector::~HistorySimSelector()
{
}

// Athena algtool's Hooks
StatusCode  ISF::HistorySimSelector::initialize()
{
  ATH_MSG_VERBOSE("Initializing ...");

  // retrieve SimulationService
  ATH_CHECK( m_prevSimSvcHandle.retrieve() );
  ATH_MSG_INFO( m_prevSimSvcHandle.propertyName() << ": Retrieved service " << m_prevSimSvcHandle.type());

  ATH_MSG_VERBOSE("Initialize successful");
  return StatusCode::SUCCESS;
}


StatusCode  ISF::HistorySimSelector::finalize()
{
  ATH_MSG_VERBOSE("Finalizing ...");

  ATH_MSG_VERBOSE("Finalize successful");
  return StatusCode::SUCCESS;
}

void ISF::HistorySimSelector::beginEvent()
{
  // get the simSvcID from the simulation service
  if (m_prevSimSvcID==ISF::fUndefinedSimID)
    {
      m_prevSimSvcID = m_prevSimSvcHandle->simSvcID();
      if (m_prevSimSvcID==ISF::fUndefinedSimID)
        {
          ATH_MSG_ERROR( m_prevSimSvcHandle.propertyName() <<
                         " does not return a proper SimSvcID! Unable to make valid routing decisions" );
        }
    }
}

bool  ISF::HistorySimSelector::passSelectorCuts(const ISFParticle& particle) const
{
  // pass selector cuts if particle was previously processed by the given SimSvcID
  bool pass = (particle.prevSimID() == m_prevSimSvcID);
  if (pass && m_checkSameGeoID)
    {
      pass = particle.prevGeoID() == particle.nextGeoID();
    }
  return pass;
}

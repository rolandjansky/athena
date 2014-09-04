/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/Incident.h"

#include "EventKernel/SignalStateHelper.h"

#include "EventShapeTools/EventEtDensityProviderBase.h"

/////////////////////////////////
// Static Parameters/Constants //
/////////////////////////////////

std::string EventEtDensityProviderBase::m_keyDefault = "NO_KEY";

EventEtDensityProviderBase::statemap_t 
EventEtDensityProviderBase::m_stateMap = statemap_t();

EventEtDensityProviderBase::statekey_t
EventEtDensityProviderBase::m_stateDefaultKey = "CALIBRATED";
EventEtDensityProviderBase::statekey_t
EventEtDensityProviderBase::m_stateUnknownKey = "UNKNOWN";
EventEtDensityProviderBase::statekey_t
EventEtDensityProviderBase::m_stateNoReqKey   = "NOT_REQUESTED";

EventEtDensityProviderBase::state_t 
EventEtDensityProviderBase::m_stateDefault = P4SignalState::CALIBRATED;
EventEtDensityProviderBase::state_t
EventEtDensityProviderBase::m_stateUnknown = P4SignalState::UNKNOWN;

///////////////////////////////
// Static Accessor Functions //
///////////////////////////////

EventEtDensityProviderBase::state_t
EventEtDensityProviderBase::findState(const statekey_t& stateKey)
{
  const statemap_t& map = getMap(); 
  statemap_t::const_iterator findKey(map.find(stateKey));
  return ( findKey == map.end() ) 
    ? map.find(stateKeyUnknown())->second : findKey->second;
}

const EventEtDensityProviderBase::statekey_t& 
EventEtDensityProviderBase::findStateKey(state_t state)
{
  const statemap_t& map = getMap();
  statemap_t::const_iterator fMap(map.begin());
  statemap_t::const_iterator lMap(map.end());
  while ( fMap != lMap && fMap->second != state ) { ++fMap; }
  return fMap != lMap ? fMap->first : stateKeyUnknown();
}

const EventEtDensityProviderBase::statemap_t& 
EventEtDensityProviderBase::getMap()
{
  if ( m_stateMap.empty() )
    {
      m_stateMap["CALIBRATED"]          = P4SignalState::CALIBRATED;
      m_stateMap["UNCALIBRATED"]        = P4SignalState::UNCALIBRATED;
      m_stateMap["ALTCALIBRATED"]       = P4SignalState::ALTCALIBRATED;
      m_stateMap["JETEMSCALE"]          = P4SignalState::JETEMSCALE;
      m_stateMap["JETFINAL"]            = P4SignalState::JETFINAL;
      m_stateMap["JETCONSTITUENTSCALE"] = P4SignalState::JETCONSTITUENTSCALE;
      m_stateMap["UNKNOWN"]             = P4SignalState::UNKNOWN;
      // check defaults
      if ( m_stateMap.find(stateKeyDefault()) == m_stateMap.end() )
	m_stateMap[stateKeyDefault()] = stateDefault();
      if ( m_stateMap.find(stateKeyUnknown()) == m_stateMap.end() )
	m_stateMap[stateKeyUnknown()] = stateUnknown();
    }
  return m_stateMap;
}

//////////////////////////////
// Constructor & Destructor //
//////////////////////////////

EventEtDensityProviderBase::EventEtDensityProviderBase(const std::string& type,
						       const std::string& name,
						       const IInterface* 
						       pParent)
  : AthAlgTool(type,name,pParent)
  , m_fastJetTool(0)
  , m_haveFastJetTool(false)
  , p_IIncidentSvc("IncidentSvc",name)
  , m_resetFlag(true)
  , m_inputCollection(m_keyDefault)
  , m_rhoValue(EventEtDensity())
  , m_rhoNullValue(EventEtDensity())
  , m_etaMin(-5.)
  , m_etaMax(5.)
  , m_signalStateKey(stateNoReq())
  , m_signalState(stateUnknown())
  , m_haveSignalState(false)
{
  declareInterface<IEventEtDensityProvider>(this);
  declareProperty("InputCollectionKey",m_inputCollection);
  declareProperty("FastJetTool",m_fastJetTool);
  declareProperty("EtaMin",m_etaMin);
  declareProperty("EtaMax",m_etaMax);
  declareProperty("SignalState",m_signalStateKey);
}

EventEtDensityProviderBase::~EventEtDensityProviderBase()
{ }

////////////////
// Initialize //
////////////////

StatusCode EventEtDensityProviderBase::initialize()
{
  m_rhoNullValue.reset();
  // allocate FastJet tool
  if ( !m_fastJetTool.empty() )
    {
      m_haveFastJetTool = m_fastJetTool.retrieve().isSuccess();
      if ( !m_haveFastJetTool )
	{
	  ATH_MSG_ERROR("Requested FastJetInterfaceTool but cannot allocate");
	  return StatusCode::FAILURE;
	}
      ATH_MSG_INFO("Allocated FastJetInterfaceTool <" 
		   << m_fastJetTool->name() <<  ">");
    }

  // allocate IncidentService and register
  if ( p_IIncidentSvc.retrieve().isFailure() )
    {
      ATH_MSG_ERROR("Cannot allocate IncidentSvc <" 
		    << p_IIncidentSvc << ">" );
      return StatusCode::FAILURE;
    }
  p_IIncidentSvc->addListener(this,IncidentType::BeginEvent);
  ATH_MSG_INFO("Retrieved IncidentSvc <" << p_IIncidentSvc << ">");

  // signal state
  m_haveSignalState = m_signalStateKey != stateNoReq(); 
  if ( m_haveSignalState )
    {
      m_signalState = findState(m_signalStateKey); 
      if (  m_signalState == stateUnknown() )
	{
      
	  ATH_MSG_ERROR("SignalState key \042" << m_signalStateKey 
			<< "\042 unknown" );
	  ATH_MSG_INFO("Known SignalState keys are:");
	  const statemap_t& map = getMap();
	  statemap_t::const_iterator fMap(map.begin());
	  statemap_t::const_iterator lMap(map.end());
	  for ( ; fMap != lMap; ++fMap )
	    { ATH_MSG_INFO("\042" << fMap->first << "\042"); }
	  return StatusCode::FAILURE;
	}
      ATH_MSG_INFO("Requested SignalState \042" 
		   << m_signalStateKey << "\042 known");
    }
  return StatusCode::SUCCESS;
}

///////////////////////////////
// BeginRun Incident Handler //
///////////////////////////////

void EventEtDensityProviderBase::handle(const Incident& incident)
{
  if ( incident.type() != IncidentType::BeginEvent ) return;
  if ( this->eventAction().isFailure() )
    ATH_MSG_WARNING("Problems executing eventAction()"); 
}

/////////////////////
// BeginRun Action //
/////////////////////

StatusCode EventEtDensityProviderBase::eventAction()
{
  this->eventReset();
  return isReset() ? StatusCode::SUCCESS : StatusCode::FAILURE; 
}

//////////////////////
// Helper Functions //
//////////////////////

size_t EventEtDensityProviderBase::convertToPJ(const momcontainer_t& navMoms,
					       jetcontainer_t& pseudoJets,
					       double pTthresh)
{
  pseudoJets.clear();
  if ( navMoms.empty() ) return 0;

  momcontainer_t::const_iterator fMom(navMoms.begin());
  momcontainer_t::const_iterator lMom(navMoms.end());
  size_t i(0);
  for ( ; fMom != lMom; ++fMom,++i )
    {
      if ( (*fMom)->pt() > pTthresh ) 
	{
	  pseudoJets.push_back(jet_t((*fMom)->px(),(*fMom)->py(),(*fMom)->pz(),
				     (*fMom)->e()));
	  pseudoJets.back().set_user_index(i);
	}
    }
  return pseudoJets.size();
}

size_t EventEtDensityProviderBase::convertToPJ(const momcontainer_t& navMoms,
					       jetcontainer_t& pseudoJets,
					       state_t signalState,
					       double pTthresh)
{
  pseudoJets.clear();
  if ( navMoms.empty() ) return 0;

  momcontainer_t::const_iterator fMom(navMoms.begin());
  momcontainer_t::const_iterator lMom(navMoms.end());
  size_t i(0);
  for ( ; fMom != lMom; ++fMom,++i )
    {
      if ( (*fMom)->pt() > pTthresh )
	{
	  SignalStateHelper s(dynamic_cast<const ISignalState*>(*fMom),
			      signalState);
	  pseudoJets.push_back(jet_t((*fMom)->px(),(*fMom)->py(),(*fMom)->pz(),
				     (*fMom)->e()));
	  pseudoJets.back().set_user_index(i);
	}
    }
  return pseudoJets.size();
}

/////////////////////////////
// Data Access: Rho Median //
/////////////////////////////

// -- overall
double EventEtDensityProviderBase::rho()
{
  if ( this->isReset() ) 
    {
      if ( this->executeAction().isFailure() )
	{
	  ATH_MSG_WARNING("Cannot update event Et density");
	  this->eventReset();
	}
      else
	{
	  this->setResetFlag(false);
	}
    }
  return this->valueServer().rhoValue;
}
// -- at given direction
double EventEtDensityProviderBase::rho(double eta)
{
  if ( this->isReset() )
    {
      if ( this->executeAction(eta).isFailure() )
	{
	  ATH_MSG_WARNING("Cannot update event Et density function");
	  this->eventReset();
	}
      else
	{
	  this->setResetFlag(false);
	}
    }
  return this->valueServer(eta).rhoValue;
}
// -- in given range
double EventEtDensityProviderBase::rho(double etaMin,double etaMax)
{
  if ( this->isReset() )
    {
      if ( this->executeAction(etaMin,etaMax).isFailure() )
	{
	  ATH_MSG_WARNING("Cannot update event Et density in range");
	  this->eventReset();
	}
      else
	{
	  this->setResetFlag(false);
	}
    }
  return this->valueServer(etaMin,etaMax).rhoValue;
}

////////////////////////////
// Data Access: Rho Sigma //
////////////////////////////

// -- overall
double EventEtDensityProviderBase::sigma()
{
  if ( this->isReset() )
    {
      if ( this->executeAction().isFailure() )
	{
	  ATH_MSG_WARNING("Cannot update event Et density fluctuations");
	  this->eventReset();
	}
      else
	{
	  this->setResetFlag(false);
	}
    }
  return this->valueServer().rhoSigma;  
}
// -- at given direction
double EventEtDensityProviderBase::sigma(double eta)
{
  if ( this->isReset() )
    {
      if ( this->executeAction(eta).isFailure() )
	{
	  ATH_MSG_WARNING("Cannot update event Et density fluctuations"
			  << " function");
	  this->eventReset();
	}
      else
	{
	  this->setResetFlag(false);
	}
    }
  return this->valueServer(eta).rhoSigma;
}
// -- in given range
double EventEtDensityProviderBase::sigma(double etaMin,double etaMax)
{
  if ( this->isReset() )
    {
      if ( this->executeAction(etaMin,etaMax).isFailure() )
	{
	  ATH_MSG_WARNING("Cannot update event Et density in range");
	  this->eventReset();
	}
      else
	{
	  this->setResetFlag(false);
	}
    }
  return this->valueServer(etaMin,etaMax).rhoSigma;  
}

///////////////////////
// Data Access: Area //
///////////////////////

// -- overall
double EventEtDensityProviderBase::area()
{
  if ( this->isReset() )
    {
      if ( this->executeAction().isFailure() )
	{
	  ATH_MSG_WARNING("Cannot update event Et density fluctuations");
	  this->eventReset();
	}
      else
	{
	  this->setResetFlag(false);
	}
    }
  return this->valueServer().areaValue;  
}
// -- at given direction
double EventEtDensityProviderBase::area(double eta)
{
  if ( this->isReset() )
    {
      if ( this->executeAction(eta).isFailure() )
	{
	  ATH_MSG_WARNING("Cannot update event Et density fluctuations"
			  << " function");
	  this->eventReset();
	}
      else
	{
	  this->setResetFlag(false);
	}
    }
  return this->valueServer(eta).areaValue;
}
// -- in given range
double EventEtDensityProviderBase::area(double etaMin,double etaMax)
{
  if ( this->isReset() )
    {
      if ( this->executeAction(etaMin,etaMax).isFailure() )
	{
	  ATH_MSG_WARNING("Cannot update event Et density in range");
	  this->eventReset();
	}
      else
	{
	  this->setResetFlag(false);
	}
    }
  return this->valueServer(etaMin,etaMax).areaValue;  
}

/////////////////////////////////
// Data Access: EventEtDensity //
/////////////////////////////////

// -- overall
const EventEtDensity& EventEtDensityProviderBase::eventEtDensity()
{
  if ( this->isReset() )
    {
      if ( this->executeAction().isFailure() )
	{
	  ATH_MSG_WARNING("Cannot update event Et density cache");
	  this->eventReset();
	}
      else
	{
	  this->setResetFlag(false);
	}
    }
  return this->valueServer();  
}
// -- at given direction
const EventEtDensity& EventEtDensityProviderBase::eventEtDensity(double eta)
{
  if ( this->isReset() )
    {
      if ( this->executeAction(eta).isFailure() )
	{
	  ATH_MSG_WARNING("Cannot update event Et density cache"
			  << " function");
	  this->eventReset();
	}
      else
	{
	  this->setResetFlag(false);
	}
    }
  //
  return this->valueServer(eta);
}
// -- in given range
const EventEtDensity& EventEtDensityProviderBase::eventEtDensity(double etaMin,
								 double etaMax)
{
  if ( this->isReset() )
    {
      if ( this->executeAction(etaMin,etaMax).isFailure() )
	{
	  ATH_MSG_WARNING("Cannot update event Et density cache in range");
	  this->eventReset();
	}
      else
	{
	  this->setResetFlag(false);
	}
    }
  return this->valueServer(etaMin,etaMax);  
}

/////////////////////////
// Set Data (protcted) //
/////////////////////////

// -- from list of numbers
void EventEtDensityProviderBase::setValue(double rhoMedian,
					  double rhoRMS,
					  double area,
					  double etaMin,
					  double etaMax)
{ 
  m_rhoValue.reset();
  m_rhoValue.rhoValue     = rhoMedian;
  m_rhoValue.rhoSigma     = rhoRMS;
  m_rhoValue.areaValue    = area;
  m_rhoValue.etaRangeLow  = etaMin;
  m_rhoValue.etaRangeHigh = etaMax;
}
// -- from EventEtDensity object
void EventEtDensityProviderBase::setValue(const EventEtDensity& density)
{ m_rhoValue = density; }

////////////////////////////////////
// Generic Data Acces (protected) //
////////////////////////////////////

// -- get reference to data value server for modifications
EventEtDensity& EventEtDensityProviderBase::valueServer()
{ return m_rhoValue; }
// -- get read-only reference to data value server
const EventEtDensity& EventEtDensityProviderBase::valueServer() const
{ return m_rhoValue; }
// -- get read-only reference to data value server function
const EventEtDensity& 
EventEtDensityProviderBase::valueServer(double eta) const
{ return m_rhoValue.inRange(eta) ? m_rhoValue : m_rhoNullValue; }
// -- get read-only reference to data value in range
const EventEtDensity& 
EventEtDensityProviderBase::valueServer(double etaMin,double etaMax) const
{ 
  return m_rhoValue.inRange(etaMin) && m_rhoValue.inRange(etaMax) 
    ? m_rhoValue : m_rhoNullValue;
}


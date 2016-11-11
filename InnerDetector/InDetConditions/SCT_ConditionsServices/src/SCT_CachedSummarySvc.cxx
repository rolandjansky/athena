/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_CachedSummarySvc.cxx
 * implementation file for SCT_CachedSummarySvc class
 * @author shaun.roe@cern.ch
 **/

#include "SCT_CachedSummarySvc.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "EventInfo/EventIncident.h"



using namespace std;

// Constructor
SCT_CachedSummarySvc::SCT_CachedSummarySvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator),m_noReports(true){
  declareProperty("ConditionsServices",m_reportingServices);
}
void 
SCT_CachedSummarySvc::handle(const Incident& /*beginEventIncident*/){
  m_cache.reset();
}

// Destructor
SCT_CachedSummarySvc::~SCT_CachedSummarySvc(){
//nop
}

//Initialize
StatusCode 
SCT_CachedSummarySvc::initialize(){
  StatusCode sc(StatusCode::FAILURE);
  m_noReports = m_reportingServices.value().empty();
  if (m_noReports){
    sc=StatusCode::SUCCESS;
    msg(MSG::INFO)<<"No services were selected for the SCT summary"<<endmsg;
  } else {
    vector<string>::const_iterator pSvcName= m_reportingServices.value().begin();
    vector<string>::const_iterator pLastName= m_reportingServices.value().end();
    bool successfulRetrieve(true);
    for (;pSvcName not_eq pLastName;++pSvcName){
      ServiceHandle<ISCT_ConditionsSvc> thisSvc(*pSvcName,name());
      sc=thisSvc.retrieve();
      if (sc.isSuccess()){
        msg(MSG::INFO)<<"Using "<<*pSvcName<<endmsg;
        m_svcHandles.push_back(thisSvc);
      } else {
        msg(MSG::ERROR)<<"The service "<<*pSvcName<<" is not available."<<endmsg;
        successfulRetrieve=false;
      }
    }
    sc=successfulRetrieve?(StatusCode::SUCCESS):(StatusCode::FAILURE);
  }
  ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc",name());
  sc = incidentSvc.retrieve();
  if (sc.isFailure() ) {
    msg(MSG::ERROR)<<"Failed to retrieve the IncidentSvc"<<endmsg;
    return StatusCode::FAILURE;
  } else {
    incidentSvc->addListener( this , IncidentType::BeginEvent);
  }
  return sc;
}

//Finalize
StatusCode
SCT_CachedSummarySvc::finalize(){
  msg(MSG::INFO)<<"Thank-you for using the SCT_CachedSummarySvc, version "<<PACKAGE_VERSION<<endmsg;
  //Code
  return StatusCode::SUCCESS;
}

// From s.binet
// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode 
SCT_CachedSummarySvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IInDetConditionsSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IInDetConditionsSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}


bool
SCT_CachedSummarySvc::isActive(const Identifier & elementId, const InDetConditions::Hierarchy h){
  return isGood(elementId, h);
}

bool 
SCT_CachedSummarySvc::isActive(const IdentifierHash & elementHash){
  return isGood(elementHash);
}

bool 
SCT_CachedSummarySvc::isActive(const IdentifierHash & elementHash, const Identifier & elementId){
  return isGood(elementHash, elementId);
}

double 
SCT_CachedSummarySvc::activeFraction(const IdentifierHash & elementHash, const Identifier & idStart, const Identifier & idEnd){
  return goodFraction(elementHash, idStart, idEnd);
}


bool 
SCT_CachedSummarySvc::isGood(const Identifier & elementId, const InDetConditions::Hierarchy h){
  bool result(true);
  if (not m_noReports){
    SvcVector_t::const_iterator svc(m_svcHandles.begin());
    SvcVector_t::const_iterator lastSvc(m_svcHandles.end());
    for (;svc not_eq lastSvc;++svc){
      if ((*svc)->canReportAbout(h) and (not (*svc)->isGood(elementId, h))) result=false;      
    }    
  }
  return result;
}

bool 
SCT_CachedSummarySvc::isGood(const IdentifierHash & elementHash){
  bool result(true);
  std::pair<bool,bool> status=m_cache.retrieve(elementHash);
  if (status.second) {
    result=status.first;
  } else {
    if (not m_noReports){
      SvcVector_t::const_iterator svc(m_svcHandles.begin());
      SvcVector_t::const_iterator lastSvc(m_svcHandles.end());
      for (;svc not_eq lastSvc;++svc){
        if ((*svc)->canReportAbout(InDetConditions::SCT_SIDE) and (not (*svc)->isGood(elementHash))) result=false;
      }    
    }
    m_cache.store(elementHash, result);
  }
  return result;
}

bool 
SCT_CachedSummarySvc::isGood(const IdentifierHash & /*elementHash*/, const Identifier & elementId){
  //could cache here as well, but should beware of 32 vs 64 bit identifiers in the Identifier class... might need to change the cache itself
  return isGood(elementId, InDetConditions::SCT_STRIP);
}

double 
SCT_CachedSummarySvc::goodFraction(const IdentifierHash & /*elementHash*/, const Identifier & /*idStart*/, const Identifier & /*idEnd*/){
  double result(1.0);
  msg(MSG::WARNING)<<"goodFraction is a deprecated function always returning 1.0 "<<endmsg;
  return result;
}

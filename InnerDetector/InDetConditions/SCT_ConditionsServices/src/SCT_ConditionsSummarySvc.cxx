/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConditionsSummarySvc.cxx
 * implementation file for SCT_ConditionsSummarySvc class
 * @author shaun.roe@cern.ch
 **/

#include "SCT_ConditionsSummarySvc.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"



using namespace std;

// Constructor
SCT_ConditionsSummarySvc::SCT_ConditionsSummarySvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator),m_noReports(true){
  declareProperty("ConditionsServices",m_reportingServices);
}

// Destructor
SCT_ConditionsSummarySvc::~SCT_ConditionsSummarySvc(){
//nop
}

//Initialize
StatusCode 
SCT_ConditionsSummarySvc::initialize(){
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
  return sc;
}

//Finalize
StatusCode
SCT_ConditionsSummarySvc::finalize(){
  msg(MSG::INFO)<<"Thank-you for using the SCT_ConditionsSummarySvc, version "<<PACKAGE_VERSION<<endmsg;
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
SCT_ConditionsSummarySvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
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
SCT_ConditionsSummarySvc::isActive(const Identifier & elementId, const InDetConditions::Hierarchy h){
  return isGood(elementId, h);
}

bool 
SCT_ConditionsSummarySvc::isActive(const IdentifierHash & elementHash){
  return isGood(elementHash);
}

bool 
SCT_ConditionsSummarySvc::isActive(const IdentifierHash & elementHash, const Identifier & elementId){
  return isGood(elementHash, elementId);
}

double 
SCT_ConditionsSummarySvc::activeFraction(const IdentifierHash & elementHash, const Identifier & idStart, const Identifier & idEnd){
  return goodFraction(elementHash, idStart, idEnd);
}


bool 
SCT_ConditionsSummarySvc::isGood(const Identifier & elementId, const InDetConditions::Hierarchy h){
  if (not m_noReports){
    SvcVector_t::const_iterator svc(m_svcHandles.begin());
    SvcVector_t::const_iterator lastSvc(m_svcHandles.end());
    for (;svc not_eq lastSvc;++svc){
      if ((*svc)->canReportAbout(h) and (not (*svc)->isGood(elementId, h))) return false;      
    }    
  }
  return true;
}

bool 
SCT_ConditionsSummarySvc::isGood(const IdentifierHash & elementHash){
  if (not m_noReports){
    SvcVector_t::const_iterator svc(m_svcHandles.begin());
    SvcVector_t::const_iterator lastSvc(m_svcHandles.end());
    for (;svc not_eq lastSvc;++svc){
      if ((*svc)->canReportAbout(InDetConditions::SCT_SIDE) and (not (*svc)->isGood(elementHash))) return false;
    }    
  }
  return true;
}

bool 
SCT_ConditionsSummarySvc::isGood(const IdentifierHash & /*elementHash*/, const Identifier & elementId){
  if (not m_noReports){
    SvcVector_t::const_iterator svc(m_svcHandles.begin());
    SvcVector_t::const_iterator lastSvc(m_svcHandles.end());
    for (;svc not_eq lastSvc;++svc){
      if ((*svc)->canReportAbout(InDetConditions::SCT_STRIP) and (not (*svc)->isGood(elementId))) return false;;
    } 
  }
  return true;
}

double 
SCT_ConditionsSummarySvc::goodFraction(const IdentifierHash & /*elementHash*/, const Identifier & /*idStart*/, const Identifier & /*idEnd*/){
  double result(1.0);
  msg(MSG::WARNING)<<"goodFraction is a deprecated function always returning 1.0 "<<endmsg;
  return result;
}

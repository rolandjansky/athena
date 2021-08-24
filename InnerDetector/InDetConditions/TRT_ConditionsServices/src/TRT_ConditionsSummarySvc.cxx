/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TRT_ConditionsSummarySvc.cxx
 * implementation file for TRT_ConditionsSummarySvc class
 * @author shaun.roe@cern.ch
 **/

#include "TRT_ConditionsSummarySvc.h"
#include "TRT_ReadoutGeometry/TRT_BaseElement.h"
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TRT_ReadoutGeometry/TRT_DetectorManager.h"

//Gaudi includes
#include "StoreGate/StoreGateSvc.h"

// Constructor
TRT_ConditionsSummarySvc::TRT_ConditionsSummarySvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator),
  m_svcCollection(name),
  m_manager(nullptr),
  m_trtid(nullptr)
{
  // Get properties from job options
  declareProperty( "ServiceList", m_svcCollection );
}

// Destructor
TRT_ConditionsSummarySvc::~TRT_ConditionsSummarySvc(){
  
}

//Initialize
StatusCode 
TRT_ConditionsSummarySvc::initialize(){
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TRT_ConditionsSummarySvc::initialize." << endmsg;
  StatusCode sc(StatusCode::SUCCESS);

  // Retrieve the services to be called.
  if ( m_svcCollection.empty() ) {
    msg(MSG::WARNING) << "No services to be called!" << endmsg;
  } else {
    sc = m_svcCollection.retrieve();
    if ( sc.isFailure() ) {
      msg(MSG::ERROR) << "Couldn't retrieve services: "
	   << m_svcCollection << endmsg;
      return sc;
    } else {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Successfully retrieved services: "
	   << m_svcCollection << endmsg;
    }
  }

  // Get DetectorStore service
  StoreGateSvc* detStore;
  sc = service("DetectorStore", detStore);
  if (sc.isFailure()){
    msg(MSG::FATAL) << "DetectorStore service not found!" << endmsg;
    return sc;
  }
  
  // Retrieve TRT_DetectorManager
  std::string managerName;
  sc=detStore->retrieve(m_manager, managerName);
  if (sc.isFailure() || !m_manager)
    {
      msg(MSG::FATAL) << "Could not find the Manager: "
	    << managerName << " !" << endmsg;
      return sc;
    }

  // Retrieve TRT ID Helper
  sc = detStore->retrieve(m_trtid,"TRT_ID");
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL("Could not retrieve TRT ID Helper.");
    return sc;
  }

  return sc;
}

//Finalize
StatusCode
TRT_ConditionsSummarySvc::finalize(){
  return StatusCode::SUCCESS;
}

bool
TRT_ConditionsSummarySvc::isActive(const Identifier & elementId, const InDetConditions::Hierarchy h){
  bool result(false);
  
  if(h==InDetConditions::TRT_STRAW || h==InDetConditions::DEFAULT){
    InDet::TRT_CondFlag flag=condSummaryStatus(elementId);
    
    result = flag==InDet::TRT_COND_GOOD || flag==InDet::TRT_COND_CAUTION || flag==InDet::TRT_COND_NOINFO;
  }else if(h==InDetConditions::TRT_STRAW_LAYER){
    //need to loop over all straws at this hierarchy level
    result=true;
    const InDetDD::TRT_BaseElement *element=m_manager->getElement(elementId);

    //loop over all straws
    int nelements=element->nStraws();
    for(int i=0;i<nelements;i++){
      InDet::TRT_CondFlag flag=condSummaryStatus(m_trtid->straw_id(elementId,i));
      if(!(flag==InDet::TRT_COND_GOOD || flag==InDet::TRT_COND_CAUTION || flag==InDet::TRT_COND_NOINFO)){
	result=false;
	break;
      }
    }
  }else{
    msg(MSG::FATAL)<<"Hierarchy Level "<<h<<" not yet implemented!!!"<<endmsg;
    result=false;
  }

  return result;
}

bool 
TRT_ConditionsSummarySvc::isActive(const IdentifierHash & elementHash){
  bool result(true);

  const InDetDD::TRT_BaseElement *element=m_manager->getElement(elementHash);

  Identifier elementId=m_trtid->layer_id(elementHash);

  //loop over all straws
  int nelements=element->nStraws();
  for(int i=0;i<nelements;i++){
    InDet::TRT_CondFlag flag=condSummaryStatus(m_trtid->straw_id(elementId,i));
    if(!(flag==InDet::TRT_COND_GOOD || flag==InDet::TRT_COND_CAUTION || flag==InDet::TRT_COND_NOINFO)){
      result=false;
      break;
    }
  }
  
  return result;
}

bool 
TRT_ConditionsSummarySvc::isActive(const IdentifierHash&, const Identifier & elementId){
  bool result(false);
  
  result=isActive(elementId,InDetConditions::TRT_STRAW);

  return result;
}

double 
TRT_ConditionsSummarySvc::activeFraction(const IdentifierHash & elementHash, const Identifier & idStart, const Identifier & idEnd){
  double result(0.0);
  
  Identifier layerId=m_trtid->layer_id(elementHash);

  int start=m_trtid->straw(idStart);
  int end=m_trtid->straw(idEnd);

  double count=1.0;
  for(int i=start;i<=end;i++){
    Identifier elementId=m_trtid->straw_id(layerId,i);
    if(isActive(elementId,InDetConditions::TRT_STRAW))
      result+=1.0;
  }
  if(count>0){
    result/=count;
  }

  return result;
}

bool 
TRT_ConditionsSummarySvc::isGood(const Identifier & elementId, const InDetConditions::Hierarchy h){
  bool result(false);

  if(h==InDetConditions::TRT_STRAW || h==InDetConditions::DEFAULT){
    InDet::TRT_CondFlag flag=condSummaryStatus(elementId);
    
    result = flag==InDet::TRT_COND_GOOD;
  }else if(h==InDetConditions::TRT_STRAW_LAYER){
    //need to loop over all straws at this hierarchy level
    result=true;
    const InDetDD::TRT_BaseElement *element=m_manager->getElement(elementId);

    //loop over all straws
    int nelements=element->nStraws();
    for(int i=0;i<nelements;i++){
      InDet::TRT_CondFlag flag=condSummaryStatus(m_trtid->straw_id(elementId,i));
      if(!(flag==InDet::TRT_COND_GOOD)){
	result=false;
	break;
      }
    }
  }else{
    msg(MSG::FATAL)<<"Hierarchy Level "<<h<<" not yet implemented!!!"<<endmsg;
    result=false;
  }

  return result;
}

bool 
TRT_ConditionsSummarySvc::isGood(const IdentifierHash & elementHash){
  bool result(false);

  const InDetDD::TRT_BaseElement *element=m_manager->getElement(elementHash);

  Identifier elementId=m_trtid->layer_id(elementHash);

  //loop over all straws
  int nelements=element->nStraws();
  for(int i=0;i<nelements;i++){
    InDet::TRT_CondFlag flag=condSummaryStatus(m_trtid->straw_id(elementId,i));
    if(!(flag==InDet::TRT_COND_GOOD)){
      result=false;
      break;
    }
  }

  return result;
}

bool 
TRT_ConditionsSummarySvc::isGood(const IdentifierHash&, const Identifier & elementId){
  bool result(false);
  
  result=isGood(elementId,InDetConditions::TRT_STRAW);

  return result;
}

double 
TRT_ConditionsSummarySvc::goodFraction(const IdentifierHash & elementHash, const Identifier & idStart, const Identifier & idEnd){
  double result(0.0);
  
  Identifier layerId=m_trtid->layer_id(elementHash);

  int start=m_trtid->straw(idStart);
  int end=m_trtid->straw(idEnd);

  double count=1.0;
  for(int i=start;i<=end;i++){
    Identifier elementId=m_trtid->straw_id(layerId,i);
    if(isGood(elementId,InDetConditions::TRT_STRAW))
      result+=1.0;
  }
  if(count>0){
    result/=count;
  }

  return result;
}

InDet::TRT_CondFlag TRT_ConditionsSummarySvc::condSummaryStatus(
					         const Identifier &ident ) {

  //  Identifier id=ident;
  InDet::TRT_CondFlag status = InDet::TRT_COND_GOOD;

  // Call condSummaryStatus for each service in m_svcCollection.
  ServiceHandleArray<ITRT_ConditionsSvc>::const_iterator svcItr;
  for ( svcItr = m_svcCollection.begin();
	svcItr != m_svcCollection.end(); ++svcItr ) {
    InDet::TRT_CondFlag thisSvcFlag;
    thisSvcFlag = (*svcItr)->condSummaryStatus( ident );
    if ( thisSvcFlag == InDet::TRT_COND_SKIPME ) continue;
    if ( thisSvcFlag == InDet::TRT_COND_NOINFO ) continue;
    //  If any tool reports CAUTION, return CAUTION.
    //  If any tool reports BAD, return BAD.
    if ( thisSvcFlag != InDet::TRT_COND_GOOD ) {
      if ( status != InDet::TRT_COND_BAD ) status = InDet::TRT_COND_CAUTION;
      if ( thisSvcFlag == InDet::TRT_COND_BAD ) status = InDet::TRT_COND_BAD;
    }
  }

  return status;
}

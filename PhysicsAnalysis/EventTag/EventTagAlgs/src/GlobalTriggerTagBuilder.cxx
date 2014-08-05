/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GlobalTriggerTagBuilder.h"

#include "TagEvent/GlobalTriggerAttributeNames.h"

#include "TagEvent/TagFragmentCollection.h"

#include "AthenaPoolUtilities/AthenaAttributeListSpecification.h"
#include "AthenaPoolUtilities/TagAthenaAttributeList.h"
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/MsgStream.h"

#include <sstream>

GlobalTriggerTagBuilder::GlobalTriggerTagBuilder( const std::string& name, ISvcLocator* pSvcLocator ) 
  : Algorithm(name, pSvcLocator),
    TagBuilderBase(),
    m_globalTriggerTagTool("GlobalTriggerTagTool/GlobalTriggerTagTool",this) {
  declareProperty("GlobalTriggerTagTool", m_globalTriggerTagTool);
  declareProperty("AttributeList", m_attributeListName);
  declareProperty("MaxNumber",     m_MAX_NUMBER);
  declareProperty("CheckAttribute", m_check = false);
}


GlobalTriggerTagBuilder::~GlobalTriggerTagBuilder() 
{}


StatusCode GlobalTriggerTagBuilder::initialize() {
  StatusCode sc;

  MsgStream mLog( messageService(), name() );
  mLog << MSG::DEBUG << "Initializing " << name() << endreq;

  /** get StoreGate service */
  sc = service( "StoreGateSvc", m_storeGateSvc );
  if (sc.isFailure()) {
    mLog << MSG::ERROR << "Unable to get StoreGate service" << endreq;
    return sc;
  }

  /** get the Global Trigger Tag Tools */
  sc = m_globalTriggerTagTool.retrieve(); 
  if ( sc.isFailure() ) {
     mLog << MSG::ERROR<< "Error retrieving the GlobalTriggerTagTool" << endreq;
     return sc;
  }

  /** define attributes */ 
  mLog << MSG::DEBUG << "Defining the attribute list specification." << endreq;

  std::map<std::string,AthenaAttributeType> attrMap;
  sc = m_globalTriggerTagTool->attributeSpecification(attrMap, m_MAX_NUMBER);
  if ( sc.isFailure() ) {
     mLog << MSG::ERROR << "Fail to build Attribute List Specification " << endreq;
     return sc;
  }

  std::map<std::string,AthenaAttributeType>::iterator bMap = attrMap.begin();
  std::map<std::string,AthenaAttributeType>::iterator eMap = attrMap.end();

  /** remove un-wanted attributes from the map and fill the attribute list */
  for (; bMap != eMap; ++bMap) {
    bool check = checkAttribute((*bMap).first, TriggerAttributeNames, Trg::Ntrg,  Trg::NumTrig);
    if (check) {
       //m_attrMap[(*bMap).first] = ((*bMap).second).typeName();
       addAttribute( (*bMap).first, (*bMap).second );
    } else {
      mLog << MSG::WARNING << "Removing " << (*bMap).first << " from the attribute List: not in TAG EDM"
           << endreq;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode GlobalTriggerTagBuilder::execute() {
  StatusCode sc;

  MsgStream mLog( messageService(), name() );

  mLog << MSG::DEBUG << "Executing " << name() << endreq;

  /** retrieve TagAthenaAttributeList */
  TagAthenaAttributeList* attribList;  
  sc = m_storeGateSvc->retrieve( attribList, m_attributeListName);
  if (sc.isFailure()) {
    mLog << MSG::ERROR << "No attribute list in SG" << endreq; 
    return sc;
  }
  
  /** create a GlobalTrigger Tag and ask the tool to fill it */ 
  TagFragmentCollection globalTriggerTag;
  sc = m_globalTriggerTagTool->execute( globalTriggerTag, m_MAX_NUMBER );
  if (sc.isFailure()) {
    mLog << MSG::WARNING << "Cannot Execute GlobalTriggerTagTool - for Trigger" << endreq; 
  } else fillAttribute(attribList, globalTriggerTag);

  /** decrease number of builders */
  TagBuilderBase::decNumOfBuilder();

  /** if this is the last builder, lock the Attribute List */
  if (TagBuilderBase::lastBuilder())
    sc = m_storeGateSvc->setConst(attribList);

  if (sc.isFailure())
    {
      mLog << MSG::WARNING << "Could not set const to attribList" << endreq; 
    }

  mLog << MSG::DEBUG << "Finished " << name() << endreq;

  return StatusCode::SUCCESS;
}


StatusCode GlobalTriggerTagBuilder::finalize() 
{
  return StatusCode::SUCCESS;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MissingETTagBuilder.h"

#include "TagEvent/MissingETAttributeNames.h"

#include "TagEvent/TagFragmentCollection.h"

#include "AthenaPoolUtilities/AthenaAttributeListSpecification.h"
#include "AthenaPoolUtilities/TagAthenaAttributeList.h"
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/MsgStream.h"

#include <sstream>

MissingETTagBuilder::MissingETTagBuilder( const std::string& name, ISvcLocator* pSvcLocator ) 
  : Algorithm(name, pSvcLocator),
    TagBuilderBase(),
    m_jetMissingEtTagTool("JetMissingEtTagTool", this) {
  declareProperty("JetMissingEtTagTool", m_jetMissingEtTagTool);
  declareProperty("AttributeList", m_attributeListName);
  declareProperty("CheckAttribute", m_check = true);
}


MissingETTagBuilder::~MissingETTagBuilder() 
{}


StatusCode MissingETTagBuilder::initialize() 
{
  StatusCode sc;

  MsgStream mLog( messageService(), name() );
  mLog << MSG::DEBUG << "Initializing " << name() << endreq;

  /** get StoreGate service */
  sc = service( "StoreGateSvc", m_storeGateSvc );
  if (sc.isFailure()) {
    mLog << MSG::ERROR << "Unable to get StoreGate service" << endreq;
    return sc;
  }

  /** get the JetMissingEt Tag Tools */
  sc = m_jetMissingEtTagTool.retrieve();
  if ( sc.isFailure() ) {
     mLog << MSG::ERROR<< "Error retrieving the JetMissingEtTagTool" << endreq;
     return sc;
  }

  /** define attributes */ 
  mLog << MSG::DEBUG << "Defining the attribute list specification." << endreq;

  std::map<std::string,AthenaAttributeType> attrMap;
  sc = m_jetMissingEtTagTool->attributeSpecification(attrMap);
  if ( sc.isFailure() ) {
     mLog << MSG::ERROR << "Fail to build Attribute List Specification " << endreq;
     return sc;
  }

  std::map<std::string,AthenaAttributeType>::iterator bMap = attrMap.begin();
  std::map<std::string,AthenaAttributeType>::iterator eMap = attrMap.end();

  /** remove un-wanted attributes from the map and fill the attribute list */
  for (; bMap != eMap; ++bMap) {
    bool check = checkAttribute((*bMap).first, MissingEtAttributeNames, EtMiss::NetAttr, EtMiss::NumMissingEt);
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

StatusCode MissingETTagBuilder::execute() {
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
  
  /** create a MissingEt Tag  and ask the tool to fill it */ 
  TagFragmentCollection missingEtTag;
  sc = m_jetMissingEtTagTool->execute( missingEtTag );
  if (sc.isFailure()) {
    mLog << MSG::WARNING << "Cannot Execute JetMissingEtTagTool" << endreq; 
  } else fillAttribute(attribList, missingEtTag);

  /** decrease number of builders */
  TagBuilderBase::decNumOfBuilder();

  /** if this is the last builder, lock the Attribute List */
  if (TagBuilderBase::lastBuilder())
    sc = m_storeGateSvc->setConst(attribList);

  if (sc.isFailure())
    {
      mLog << MSG::WARNING << "Could not set const to attribList" << endreq; 
    }

  mLog << MSG::DEBUG << "Finished" << name() << endreq;

  return StatusCode::SUCCESS;
}


StatusCode MissingETTagBuilder::finalize() 
{
  return StatusCode::SUCCESS;
}

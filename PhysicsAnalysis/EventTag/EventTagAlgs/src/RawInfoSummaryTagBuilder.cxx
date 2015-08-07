/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RawInfoSummaryTagBuilder.h"

#include "TagEvent/RawInfoSummaryForTagNames.h"

#include "TagEvent/TagFragmentCollection.h"

#include "AthenaPoolUtilities/AthenaAttributeListSpecification.h"
#include "AthenaPoolUtilities/TagAthenaAttributeList.h"
#include "StoreGate/StoreGateSvc.h"

#include <sstream>

RawInfoSummaryTagBuilder::RawInfoSummaryTagBuilder( const std::string& name, ISvcLocator* pSvcLocator ) 
  : AthAlgorithm(name, pSvcLocator),
    TagBuilderBase(),
    m_tagTool("RawInfoSummaryForTagTool", this) {
  declareProperty("RawInfoSummaryForTagTool", m_tagTool);
  declareProperty("AttributeList", m_attributeListName);
  declareProperty("CheckAttribute", m_check = true);
}


RawInfoSummaryTagBuilder::~RawInfoSummaryTagBuilder() 
{}


StatusCode RawInfoSummaryTagBuilder::initialize() {
  StatusCode sc;

  MsgStream mLog( messageService(), name() );
  mLog << MSG::DEBUG << "Initializing " << name() << endreq;

  /** get StoreGate service */
  sc = service( "StoreGateSvc", m_storeGateSvc );
  if (sc.isFailure()) {
    mLog << MSG::ERROR << "Unable to get StoreGate service" << endreq;
    return sc;
  }

  /** get the Tag Tool */
  sc = m_tagTool.retrieve();
  if ( sc.isFailure() ) {
     mLog << MSG::ERROR<< "Error retrieving the RawInfoSummaryForTagTool" << endreq;
     return sc;
  }

  /** define attributes */ 
  mLog << MSG::DEBUG << "Defining the attribute list specification." << endreq;

  /** define the attributes */
  //std::map<std::string,AthenaAttributeType> attrMap;
  std::vector<AthenaAttributeSpecification> attrMap;
  sc = m_tagTool->attributeSpecification(attrMap);
  if ( sc.isFailure() ) {
     mLog << MSG::ERROR << "Fail to build Attribute List Specification " << endreq;
     return sc;
  }

  //std::map<std::string,AthenaAttributeType>::iterator bMap = attrMap.begin();
  //std::map<std::string,AthenaAttributeType>::iterator eMap = attrMap.end();
  std::vector<AthenaAttributeSpecification>::iterator bMap = attrMap.begin();
  std::vector<AthenaAttributeSpecification>::iterator eMap = attrMap.end();

  /** remove un-wanted attributes from the map and fill the attribute list */
  for (; bMap != eMap; ++bMap) {
    bool check = checkAttribute((*bMap).name(), RawSummaryAttributeSpecs, Raw::NAttr);
    if (check) {
       //addAttribute( (*bMap).name(), (*bMap).attributeType() );
       const std::string name(bMap->name());
       AthenaAttributeType type(bMap->attributeType());
       //addAttribute( bMap->name(), bMap->attributeType() );
       addAttribute( name, type );
    } else {
      mLog << MSG::WARNING << "Removing " << (*bMap).name() << " from the attribute List: not in TAG EDM"
           << endreq;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode RawInfoSummaryTagBuilder::execute() {
  StatusCode sc;

  MsgStream mLog( messageService(), name() );

  mLog << MSG::DEBUG << "Executing " << name() << endreq;

  /** retrieve TagAthenaAttributeList */
  TagAthenaAttributeList* attribList;  
  sc = m_storeGateSvc->retrieve( attribList, m_attributeListName);
  if (sc.isFailure()) {
    mLog << MSG::WARNING << "No attribute list in SG" << endreq; 
    return sc;
  }
  
  /** create a DataQuality Tag  and ask the tool to fill it */ 
  TagFragmentCollection rawfragment;
  sc = m_tagTool->execute( rawfragment );
  if (sc.isFailure()) {
    mLog << MSG::WARNING << "Cannot Execute RawInfoSummaryForTagTool" << endreq; 
  } else fillAttribute(attribList, rawfragment);

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


StatusCode RawInfoSummaryTagBuilder::finalize() 
{
  return StatusCode::SUCCESS;
}

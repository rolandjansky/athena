/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TagEvent/GlobalEventAttributeNames.h"
#include "TagEvent/TagFragmentCollection.h"

#include "AthenaPoolUtilities/AthenaAttributeListSpecification.h"
#include "AthenaPoolUtilities/TagAthenaAttributeList.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "GlobalEventTagBuilder.h"

#include "POOLCore/Exception.h"

#include "CLHEP/Vector/ThreeVector.h"
#include <vector>

GlobalEventTagBuilder::GlobalEventTagBuilder( const std::string& name, ISvcLocator* pSvcLocator ) 
  : Algorithm(name, pSvcLocator),
    TagBuilderBase(),
    m_globalEventTagTool("GlobalEventTagTool/GlobalEventTagTool",this) {
  declareProperty("GlobalEventTagTool", m_globalEventTagTool);
  declareProperty("AttributeList", m_attributeListName);
  declareProperty("CheckAttribute", m_check = true);
}

GlobalEventTagBuilder::~GlobalEventTagBuilder() 
{}

StatusCode GlobalEventTagBuilder::initialize() {
  MsgStream mLog( messageService(), name() );
  mLog << MSG::DEBUG << "Initializing " << name() << endreq;

  /** get StoreGate service */
  StatusCode sc = service( "StoreGateSvc", m_storeGateSvc );
  if (sc.isFailure()) {
    mLog << MSG::ERROR << "Unable to get StoreGate service" << endreq;
    return sc;
  }

  /** get the Global Event Tag Tools */
  sc = m_globalEventTagTool.retrieve();
  if ( sc.isFailure() ) {
     mLog << MSG::ERROR<< "Error retrieving " << m_globalEventTagTool.typeAndName() << endreq;
     return sc;
  }

  /** initialize attribute list spec pointer */
  m_attribListSpec  = new AthenaAttributeListSpecification;
  
  /** define attributes */
  mLog << MSG::DEBUG << "Defining the attribute list specification." << endreq;

  std::vector<std::pair<std::string,AthenaAttributeType> > attrMap;
  sc = m_globalEventTagTool->attributeSpecification(attrMap);
  if ( sc.isFailure() ) {
     mLog << MSG::ERROR << "Fail to build Attribute List Specification " << endreq;
     return sc;
  }

  std::vector<std::pair<std::string,AthenaAttributeType> >::iterator bMap = attrMap.begin();
  std::vector<std::pair<std::string,AthenaAttributeType> >::iterator eMap = attrMap.end();

  /** remove un-wanted attributes from the map and fill the attribute list */
  for (; bMap != eMap; ++bMap) {
    bool check = checkAttribute((*bMap).first, EventAttributeSpecs, Evt::NevtAttr);
    if (!check) {
      mLog << MSG::WARNING << "Removing " << (*bMap).first << " from the attribute List: not in TAG EDM"
           << endreq;
    }
    else {
      addAttribute( (*bMap).first, (*bMap).second );
/*
      bool check2 = (m_attrMap.find(bMap->first)==m_attrMap.end());
      if (check2) {
        //m_attrMap[(*bMap).first] = ((*bMap).second).typeName();
        addAttribute( (*bMap).first, (*bMap).second );
      } else {
        mLog << MSG::ERROR << "Attribute " << (*bMap).first 
                           << " already exists, unresolved ambiguity likely" << endreq;
      }
*/
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode GlobalEventTagBuilder::execute() {
  MsgStream mLog( messageService(), name() );

  mLog << MSG::DEBUG << "Executing " << name() << endreq;

  /** reset number of builders */
  TagBuilderBase::setNumOfBuilderToMax();

  /** create TagAthenaAttributeList */
  TagAthenaAttributeList* attribList = 0;
  try {
    attribList = new TagAthenaAttributeList( *m_attribListSpec );
  } 
  catch (pool::Exception& e) {
    mLog << MSG::ERROR
	 << "Caught exception during creation of TagAthenaAttributeList object. "
	 << "Message: " << e.what() << endreq;
    return StatusCode::FAILURE;
  }

  /** create a GlobalEvent Tag and ask the tool to fill it */ 
  TagFragmentCollection globalEventTag;
  StatusCode sc = m_globalEventTagTool->execute( globalEventTag );
  if (sc.isFailure()) {
    mLog << MSG::WARNING << "Cannot Execute GlobalEventTagTool" << endreq; 
  } else fillAttribute(attribList, globalEventTag);

  /** record attribute list to SG */
  sc = m_storeGateSvc->record( attribList, m_attributeListName );
  if (sc.isFailure()) {
    mLog << MSG::ERROR << "Could not record TagAthenaAttributeList" << endreq;
    return sc;
  }

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


StatusCode GlobalEventTagBuilder::finalize() 
{
  return StatusCode::SUCCESS;
}

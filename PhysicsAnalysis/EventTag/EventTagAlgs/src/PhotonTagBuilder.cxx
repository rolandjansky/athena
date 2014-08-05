/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PhotonTagBuilder.h"

#include "TagEvent/TagFragmentCollection.h"
#include "TagEvent/PhotonAttributeNames.h"

#include "AthenaPoolUtilities/AthenaAttributeListSpecification.h"
#include "AthenaPoolUtilities/TagAthenaAttributeList.h"
#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <sstream>

PhotonTagBuilder::PhotonTagBuilder( const std::string& name, ISvcLocator* pSvcLocator ) 
  : Algorithm(name, pSvcLocator),
    TagBuilderBase(), 
  m_photonTagTool("PhotonTagTool", this) {
  declareProperty("PhotonTagTool", m_photonTagTool);
  declareProperty("AttributeList", m_attributeListName);
  declareProperty("MaxNumber",     m_MAX_NUMBER = 4);
  declareProperty("CheckAttribute", m_check = true);
}

PhotonTagBuilder::~PhotonTagBuilder() 
{}

StatusCode PhotonTagBuilder::initialize() {
  StatusCode sc;

  MsgStream mLog( messageService(), name() );
  mLog << MSG::DEBUG << "Initializing " << name() << endreq;

  /** get StoreGate service */
  sc = service( "StoreGateSvc", m_storeGateSvc );
  if (sc.isFailure()) {
    mLog << MSG::ERROR << "Unable to get StoreGate service" << endreq;
    return sc;
  }

  /** get the Photon Tag Tools */
  sc = m_photonTagTool.retrieve();
  if ( sc.isFailure() ) {
     mLog << MSG::ERROR<< "Error retrieving the PhotonTagTool" << endreq;
     return sc;
  }

  /** define attributes */
  mLog << MSG::DEBUG << "Defining the attribute list specification." << endreq;

  std::map<std::string,AthenaAttributeType> attrMap;
  sc = m_photonTagTool->attributeSpecification(attrMap, m_MAX_NUMBER);
  if ( sc.isFailure() ) {
     mLog << MSG::ERROR << "Fail to build Attribute List Specification " << endreq;
     return sc; 
  }

  std::map<std::string,AthenaAttributeType>::iterator bMap = attrMap.begin();
  std::map<std::string,AthenaAttributeType>::iterator eMap = attrMap.end();

  /** remove un-wanted attributes from the map and fill the attribute list */
  for (; bMap != eMap; ++bMap) {
    bool check = checkAttribute((*bMap).first, PhotonAttributeNames, PhotonID::NphotonAttr, PhotonID::NumPhoton);
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

StatusCode PhotonTagBuilder::execute() {
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

  /** ask the tool to fill the photon tag fragment */
  TagFragmentCollection photonTagColl; 
  sc = m_photonTagTool->execute( photonTagColl, m_MAX_NUMBER );
  if (sc.isFailure()) {
    mLog << MSG::WARNING << "Cannot Execute PhotonTagToool" << endreq; 
  } else fillAttribute(attribList, photonTagColl);

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


StatusCode PhotonTagBuilder::finalize() 
{
  return StatusCode::SUCCESS;
}

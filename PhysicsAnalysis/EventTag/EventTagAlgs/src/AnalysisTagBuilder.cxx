/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisTagBuilder.h"

#include "TagEvent/EgammaAttributeNames.h"
#include "TagEvent/JetMissingEtIdentificationAttributeNames.h"

#include "TagEvent/TagFragmentCollection.h"

#include "AthenaPoolUtilities/AthenaAttributeListSpecification.h"
#include "AthenaPoolUtilities/TagAthenaAttributeList.h"
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/MsgStream.h"

#include <sstream>

AnalysisTagBuilder::AnalysisTagBuilder( const std::string& name, ISvcLocator* pSvcLocator ) 
  : AthAlgorithm(name, pSvcLocator),
    TagBuilderBase(), 
    m_jetMissingEtTagTool("JetMissingEtIdentificationTagTool", this){

  declareProperty("JetMissingEtIdentificationTagTool", m_jetMissingEtTagTool);
  declareProperty("AttributeList", m_attributeListName);
  declareProperty("NumJetMissinEt", m_nJetMissingEt = 1);
  declareProperty("CheckAttribute", m_check = true);
}


AnalysisTagBuilder::~AnalysisTagBuilder() 
{}


StatusCode AnalysisTagBuilder::initialize() {
  StatusCode sc;

  MsgStream mLog( messageService(), name() );
  mLog << MSG::DEBUG << "Initializing " << name() << endreq;

  /** get StoreGate service */
  sc = service( "StoreGateSvc", m_storeGateSvc );
  if (sc.isFailure()) {
    mLog << MSG::ERROR << "Unable to get StoreGate service" << endreq;
    return sc;
  }

  /** get the Analysis Tag Tools */
  sc = m_jetMissingEtTagTool.retrieve();
  if ( sc.isFailure() ) {
     mLog << MSG::ERROR<< "Error retrieving the JetMissingEtIdentificationTagTool" << endreq;
     return sc;
  }

  /** define attributes */ 
  mLog << MSG::DEBUG << "Defining the attribute list specification." << endreq;

  /** define the attributes for the analysis tags */

  std::map<std::string,AthenaAttributeType> attrMap;

  sc = m_jetMissingEtTagTool->attributeSpecification(attrMap, m_nJetMissingEt);
  if ( sc.isFailure() ) {
     mLog << MSG::ERROR << "Fail to build Attribute List Specification " << endreq;
     return sc;
  }


  std::map<std::string,AthenaAttributeType>::iterator bMap = attrMap.begin();
  std::map<std::string,AthenaAttributeType>::iterator eMap = attrMap.end();

  /** remove un-wanted attributes from the map and fill the attribute list */
  for (; bMap != eMap; ++bMap) {
    bool c04 = checkAttribute((*bMap).first, JetMissingEtAttributeNames, JetMissingEt::NjetMetAttr, JetMissingEt::NumJetMissingEtID);

    bool check = c04;

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

StatusCode AnalysisTagBuilder::execute() {
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
  
  /** create an Analysis Tag  and ask the tool to fill it */


  /** Jet and MissingEt Word */ 
  TagFragmentCollection jetMissingEtTag;
  sc = m_jetMissingEtTagTool->execute( jetMissingEtTag, m_nJetMissingEt );
  if (sc.isFailure()) {
    mLog << MSG::WARNING << "Cannot Execute JetMissingEtIdentificationTagTool" << endreq; 
  } else fillAttribute(attribList, jetMissingEtTag );


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


StatusCode AnalysisTagBuilder::finalize() 
{
  return StatusCode::SUCCESS;
}

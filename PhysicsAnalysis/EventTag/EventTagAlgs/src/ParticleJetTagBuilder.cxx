/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleJetTagBuilder.h"

#include "TagEvent/ParticleJetAttributeNames.h"

#include "TagEvent/TagFragmentCollection.h"

#include "AthenaPoolUtilities/AthenaAttributeListSpecification.h"
#include "AthenaPoolUtilities/TagAthenaAttributeList.h"
#include "StoreGate/StoreGateSvc.h"

#include <sstream>

ParticleJetTagBuilder::ParticleJetTagBuilder( const std::string& name, ISvcLocator* pSvcLocator ) 
  : AthAlgorithm(name, pSvcLocator),
    TagBuilderBase(),
    m_jetMissingEtTagTool("JetMissingEtTagTool", this) {
  declareProperty("JetMissingEtTagTool", m_jetMissingEtTagTool);
  declareProperty("AttributeList", m_attributeListName);
  declareProperty("MaxNumber",     m_MAX_NUMBER = 6);
  declareProperty("CheckAttribute", m_check = true);
}

ParticleJetTagBuilder::~ParticleJetTagBuilder() 
{}

StatusCode ParticleJetTagBuilder::initialize() 
{
  ATH_MSG_DEBUG( "Initializing " << name() );

  ATH_CHECK( m_jetMissingEtTagTool.retrieve() );

  /** define attributes */ 
  ATH_MSG_DEBUG( "Defining the attribute list specification." );

  std::map<std::string,AthenaAttributeType> attrMap;
  ATH_CHECK(  m_jetMissingEtTagTool->attributeSpecification(attrMap, m_MAX_NUMBER) );

  std::map<std::string,AthenaAttributeType>::iterator bMap = attrMap.begin();
  std::map<std::string,AthenaAttributeType>::iterator eMap = attrMap.end();

  /** remove un-wanted attributes from the map and fill the attribute list */
  for (; bMap != eMap; ++bMap) {
    bool check = checkAttribute((*bMap).first, JetAttributeNames, PJet::NjetAttr, PJet::NumJet);
    if (check) {
       //m_attrMap[(*bMap).first] = ((*bMap).second).typeName();
       addAttribute( (*bMap).first, (*bMap).second );
    } else {
      ATH_MSG_WARNING( "Removing " << (*bMap).first << " from the attribute List: not in TAG EDM" );
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode ParticleJetTagBuilder::execute() {
  ATH_MSG_DEBUG( "Executing " << name() );

  // retrieve TagAthenaAttributeList
  TagAthenaAttributeList* attribList = nullptr;
  ATH_CHECK( evtStore()->retrieve( attribList, m_attributeListName) );
  
  /** ask the tool to fill particle jet tag collection */ 
  TagFragmentCollection particleJetTagColl;
  StatusCode sc = m_jetMissingEtTagTool->execute( particleJetTagColl, m_MAX_NUMBER );
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Cannot Execute JetMissingEtTagToool" );
  } else fillAttribute(attribList, particleJetTagColl);

  /** decrease number of builders */
  TagBuilderBase::decNumOfBuilder();

  /** if this is the last builder, lock the Attribute List */
  if (TagBuilderBase::lastBuilder())
    sc = evtStore()->setConst(attribList);

  if (sc.isFailure())
    {
      ATH_MSG_WARNING( "Could not set const to attribList" );
    }

  ATH_MSG_DEBUG( "Finished" << name() );

  return StatusCode::SUCCESS;
}


StatusCode ParticleJetTagBuilder::finalize() 
{
  return StatusCode::SUCCESS;
}

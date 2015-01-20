/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MissingETTagBuilder.h"

#include "TagEvent/MissingETAttributeNames.h"

#include "TagEvent/TagFragmentCollection.h"

#include "AthenaPoolUtilities/AthenaAttributeListSpecification.h"
#include "AthenaPoolUtilities/TagAthenaAttributeList.h"
#include "StoreGate/StoreGateSvc.h"

#include <sstream>

MissingETTagBuilder::MissingETTagBuilder( const std::string& name, ISvcLocator* pSvcLocator ) 
  : AthAlgorithm(name, pSvcLocator),
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
  ATH_MSG_DEBUG( "Initializing " << name() );

  ATH_CHECK( m_jetMissingEtTagTool.retrieve() );

  /** define attributes */ 
  ATH_MSG_DEBUG( "Defining the attribute list specification." );

  std::map<std::string,AthenaAttributeType> attrMap;
  ATH_CHECK(  m_jetMissingEtTagTool->attributeSpecification(attrMap) );

  std::map<std::string,AthenaAttributeType>::iterator bMap = attrMap.begin();
  std::map<std::string,AthenaAttributeType>::iterator eMap = attrMap.end();

  /** remove un-wanted attributes from the map and fill the attribute list */
  for (; bMap != eMap; ++bMap) {
    bool check = checkAttribute((*bMap).first, MissingEtAttributeNames, EtMiss::NetAttr, EtMiss::NumMissingEt);
    if (check) {
       //m_attrMap[(*bMap).first] = ((*bMap).second).typeName();
       addAttribute( (*bMap).first, (*bMap).second );
    } else {
      ATH_MSG_WARNING( "Removing " << (*bMap).first << " from the attribute List: not in TAG EDM" );
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode MissingETTagBuilder::execute() {
  ATH_MSG_DEBUG( "Executing " << name() );

  /** retrieve TagAthenaAttributeList */
  TagAthenaAttributeList* attribList = nullptr;
  ATH_CHECK( evtStore()->retrieve( attribList, m_attributeListName) );
  
  /** create a MissingEt Tag  and ask the tool to fill it */ 
  TagFragmentCollection missingEtTag;
  StatusCode sc = m_jetMissingEtTagTool->execute( missingEtTag );
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Cannot Execute JetMissingEtTagTool" );
  } else fillAttribute(attribList, missingEtTag);

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


StatusCode MissingETTagBuilder::finalize() 
{
  return StatusCode::SUCCESS;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GlobalTriggerTagBuilder.h"

#include "TagEvent/GlobalTriggerAttributeNames.h"

#include "TagEvent/TagFragmentCollection.h"

#include "AthenaPoolUtilities/AthenaAttributeListSpecification.h"
#include "AthenaPoolUtilities/TagAthenaAttributeList.h"
#include "StoreGate/StoreGateSvc.h"

#include <sstream>

GlobalTriggerTagBuilder::GlobalTriggerTagBuilder( const std::string& name, ISvcLocator* pSvcLocator ) 
  : AthAlgorithm(name, pSvcLocator),
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
  ATH_MSG_DEBUG( "Initializing " << name() );

  ATH_CHECK( m_globalTriggerTagTool.retrieve() );

  /** define attributes */ 
  ATH_MSG_DEBUG( "Defining the attribute list specification." );

  std::map<std::string,AthenaAttributeType> attrMap;
  ATH_CHECK( m_globalTriggerTagTool->attributeSpecification(attrMap, m_MAX_NUMBER) );

  std::map<std::string,AthenaAttributeType>::iterator bMap = attrMap.begin();
  std::map<std::string,AthenaAttributeType>::iterator eMap = attrMap.end();

  /** remove un-wanted attributes from the map and fill the attribute list */
  for (; bMap != eMap; ++bMap) {
    bool check = checkAttribute((*bMap).first, TriggerAttributeNames, Trg::Ntrg,  Trg::NumTrig);
    if (check) {
       //m_attrMap[(*bMap).first] = ((*bMap).second).typeName();
       addAttribute( (*bMap).first, (*bMap).second );
    } else {
      ATH_MSG_WARNING( "Removing " << (*bMap).first << " from the attribute List: not in TAG EDM" );
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode GlobalTriggerTagBuilder::execute() {
  ATH_MSG_DEBUG( "Executing " << name() );

  /** retrieve TagAthenaAttributeList */
  TagAthenaAttributeList* attribList = nullptr;
  ATH_CHECK( evtStore()->retrieve( attribList, m_attributeListName) );
  
  /** create a GlobalTrigger Tag and ask the tool to fill it */ 
  TagFragmentCollection globalTriggerTag;
  StatusCode sc = m_globalTriggerTagTool->execute( globalTriggerTag, m_MAX_NUMBER );
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Cannot Execute GlobalTriggerTagTool - for Trigger" );
  } else fillAttribute(attribList, globalTriggerTag);

  /** decrease number of builders */
  TagBuilderBase::decNumOfBuilder();

  /** if this is the last builder, lock the Attribute List */
  if (TagBuilderBase::lastBuilder())
    sc = evtStore()->setConst(attribList);

  if (sc.isFailure())
    {
      ATH_MSG_WARNING( "Could not set const to attribList" );
    }

  ATH_MSG_DEBUG( "Finished " << name() );
  return StatusCode::SUCCESS;
}


StatusCode GlobalTriggerTagBuilder::finalize() 
{
  return StatusCode::SUCCESS;
}

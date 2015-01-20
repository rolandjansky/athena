/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTagBuilder.h"

#include "TagEvent/TagFragmentCollection.h"
#include "TagEvent/MuonAttributeNames.h"

#include "AthenaPoolUtilities/AthenaAttributeListSpecification.h"
#include "AthenaPoolUtilities/TagAthenaAttributeList.h"
#include "StoreGate/StoreGateSvc.h"

#include <sstream>

MuonTagBuilder::MuonTagBuilder( const std::string& name, ISvcLocator* pSvcLocator ) 
  : AthAlgorithm(name, pSvcLocator),
    TagBuilderBase(),
    m_muonTagTool("MuonTagTool",this) {
  declareProperty("MuonTagTool", m_muonTagTool);
  declareProperty("AttributeList", m_attributeListName);
  declareProperty("MaxNumber",     m_MAX_NUMBER = 4);
  declareProperty("CheckAttribute", m_check = true);
}

MuonTagBuilder::~MuonTagBuilder() 
{}


StatusCode MuonTagBuilder::initialize() {
  ATH_MSG_DEBUG( "Initializing " << name() );

  ATH_CHECK( m_muonTagTool.retrieve() );

  /** define attributes */
  ATH_MSG_DEBUG( "Defining the attribute list specification." );

  std::map<std::string,AthenaAttributeType> attrMap;
  ATH_CHECK( m_muonTagTool->attributeSpecification(attrMap, m_MAX_NUMBER) );

  std::map<std::string,AthenaAttributeType>::iterator bMap = attrMap.begin();
  std::map<std::string,AthenaAttributeType>::iterator eMap = attrMap.end();

  /** remove un-wanted attributes from the map and fill the attribute list */
  for (; bMap != eMap; ++bMap) {
    bool check = checkAttribute((*bMap).first, MuonAttributeNames, MuonTAG::NmuonAttr, MuonTAG::NumMuon);
    if (check) {
       //m_attrMap[(*bMap).first] = ((*bMap).second).typeName();
       addAttribute( (*bMap).first, (*bMap).second ) ;
    } else {
      ATH_MSG_WARNING( "Removing " << (*bMap).first << " from the attribute List: not in TAG EDM" );
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode MuonTagBuilder::execute() {
  ATH_MSG_DEBUG( "Executing " << name() );

  /** retrieve TagAthenaAttributeList */
  TagAthenaAttributeList* attribList;  
  ATH_CHECK( evtStore()->retrieve( attribList, m_attributeListName) );
  
  /** clear the MuonTagCollection and ask the tool to fill it */ 
  TagFragmentCollection muonTagColl;
  StatusCode sc = m_muonTagTool->execute( muonTagColl, m_MAX_NUMBER );
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Cannot Execute MuonTagToool" );
  } else fillAttribute(attribList, muonTagColl);

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


StatusCode MuonTagBuilder::finalize() 
{
  return StatusCode::SUCCESS;
}

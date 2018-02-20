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
  ATH_MSG_DEBUG( "Initializing " << name()  );

  /** get the Tag Tool */
  ATH_CHECK( m_tagTool.retrieve() );

  /** define attributes */ 
  ATH_MSG_DEBUG( "Defining the attribute list specification."  );

  /** define the attributes */
  //std::map<std::string,AthenaAttributeType> attrMap;
  std::vector<AthenaAttributeSpecification> attrMap;
  ATH_CHECK( m_tagTool->attributeSpecification(attrMap) );

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
      ATH_MSG_WARNING( "Removing " << (*bMap).name() << " from the attribute List: not in TAG EDM" );
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode RawInfoSummaryTagBuilder::execute() {
  ATH_MSG_DEBUG( "Executing " << name()  );

  /** retrieve TagAthenaAttributeList */
  TagAthenaAttributeList* attribList;  
  StatusCode sc = evtStore()->retrieve( attribList, m_attributeListName);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "No attribute list in SG"  );
    return sc;
  }
  
  /** create a DataQuality Tag  and ask the tool to fill it */ 
  TagFragmentCollection rawfragment;
  sc = m_tagTool->execute( rawfragment );
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Cannot Execute RawInfoSummaryForTagTool"  );
  } else fillAttribute(attribList, rawfragment);

  /** decrease number of builders */
  TagBuilderBase::decNumOfBuilder();

  /** if this is the last builder, lock the Attribute List */
  if (TagBuilderBase::lastBuilder())
    sc = evtStore()->setConst(attribList);

  if (sc.isFailure())
    {
      ATH_MSG_WARNING( "Could not set const to attribList"  );
    }

  ATH_MSG_DEBUG( "Finished" << name()  );

  return StatusCode::SUCCESS;
}


StatusCode RawInfoSummaryTagBuilder::finalize() 
{
  return StatusCode::SUCCESS;
}

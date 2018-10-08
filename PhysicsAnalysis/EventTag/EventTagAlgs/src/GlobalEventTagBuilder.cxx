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
  : AthAlgorithm(name, pSvcLocator),
    TagBuilderBase(),
    m_globalEventTagTool("GlobalEventTagTool/GlobalEventTagTool",this) {
  declareProperty("GlobalEventTagTool", m_globalEventTagTool);
  declareProperty("AttributeList", m_attributeListName);
  declareProperty("CheckAttribute", m_check = true);
}

GlobalEventTagBuilder::~GlobalEventTagBuilder() 
{}

StatusCode GlobalEventTagBuilder::initialize() {
  ATH_MSG_DEBUG( "Initializing " << name() );

  ATH_CHECK( m_globalEventTagTool.retrieve() );

  /** initialize attribute list spec pointer */
  m_attribListSpec  = new AthenaAttributeListSpecification;
  
  /** define attributes */
  ATH_MSG_DEBUG( "Defining the attribute list specification." );

  std::vector<std::pair<std::string,AthenaAttributeType> > attrMap;
  ATH_CHECK( m_globalEventTagTool->attributeSpecification(attrMap) );

  std::vector<std::pair<std::string,AthenaAttributeType> >::iterator bMap = attrMap.begin();
  std::vector<std::pair<std::string,AthenaAttributeType> >::iterator eMap = attrMap.end();

  /** remove un-wanted attributes from the map and fill the attribute list */
  for (; bMap != eMap; ++bMap) {
    bool check = checkAttribute((*bMap).first, EventAttributeSpecs, Evt::NevtAttr);
    if (!check) {
      ATH_MSG_WARNING( "Removing " << (*bMap).first << " from the attribute List: not in TAG EDM" );
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
                           << " already exists, unresolved ambiguity likely" << endmsg;
      }
*/
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode GlobalEventTagBuilder::execute() {
  ATH_MSG_DEBUG( "Executing " << name() );

  /** reset number of builders */
  TagBuilderBase::setNumOfBuilderToMax();

  /** create TagAthenaAttributeList */
  TagAthenaAttributeList* attribList = 0;
  try {
    attribList = new TagAthenaAttributeList( *m_attribListSpec );
  } 
  catch (pool::Exception& e) {
    ATH_MSG_ERROR( "Caught exception during creation of TagAthenaAttributeList object. "
                   << "Message: " << e.what() );
    return StatusCode::FAILURE;
  }

  /** create a GlobalEvent Tag and ask the tool to fill it */ 
  TagFragmentCollection globalEventTag;
  StatusCode sc = m_globalEventTagTool->execute( globalEventTag );
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Cannot Execute GlobalEventTagTool" );
  } else fillAttribute(attribList, globalEventTag);

  /** record attribute list to SG */
  ATH_CHECK( evtStore()->record( attribList, m_attributeListName ) );

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


StatusCode GlobalEventTagBuilder::finalize() 
{
  return StatusCode::SUCCESS;
}

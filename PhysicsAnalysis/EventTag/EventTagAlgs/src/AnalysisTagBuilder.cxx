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
  ATH_MSG_DEBUG( "Initializing " << name()  );

  /** get the Analysis Tag Tools */
  ATH_CHECK( m_jetMissingEtTagTool.retrieve() );

  /** define attributes */ 
  ATH_MSG_DEBUG( "Defining the attribute list specification."  );

  /** define the attributes for the analysis tags */

  std::map<std::string,AthenaAttributeType> attrMap;

  ATH_CHECK( m_jetMissingEtTagTool->attributeSpecification(attrMap, m_nJetMissingEt) );


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
      ATH_MSG_WARNING( "Removing " << (*bMap).first << " from the attribute List: not in TAG EDM" 
                       );
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode AnalysisTagBuilder::execute() {
  ATH_MSG_DEBUG( "Executing " << name()  );

  /** retrieve TagAthenaAttributeList */
  TagAthenaAttributeList* attribList;  
  StatusCode sc = evtStore()->retrieve( attribList, m_attributeListName);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "No attribute list in SG"  );
    return sc;
  }
  
  /** create an Analysis Tag  and ask the tool to fill it */


  /** Jet and MissingEt Word */ 
  TagFragmentCollection jetMissingEtTag;
  sc = m_jetMissingEtTagTool->execute( jetMissingEtTag, m_nJetMissingEt );
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Cannot Execute JetMissingEtIdentificationTagTool"  );
  } else fillAttribute(attribList, jetMissingEtTag );


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


StatusCode AnalysisTagBuilder::finalize() 
{
  return StatusCode::SUCCESS;
}

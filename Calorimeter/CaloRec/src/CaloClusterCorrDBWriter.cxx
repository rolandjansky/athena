/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
//
// Description: see CaloClusterCorrDBWriter.h
//-----------------------------------------------------------------------


#include "CaloClusterCorrDBWriter.h"
#include "GaudiKernel/ThreadLocalContext.h"

#include "CaloConditions/ToolConstants.h"
#include "AthenaKernel/errorcheck.h"


using namespace CaloRec;

//#############################################################################

StatusCode CaloClusterCorrDBWriter::initialize()
{
  ATH_CHECK( m_blobTool.retrieve());
  ATH_CHECK( m_tools.retrieve());
  REPORT_MESSAGE(MSG::INFO) << m_key << ": "
                            << "Found " << m_tools.size() <<
    " tools.";
  return StatusCode::SUCCESS;
}

//#############################################################################

StatusCode CaloClusterCorrDBWriter::finalize()
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  if (!m_inlineFolder.empty()) {
    CaloRec::ToolConstants tc;
    std::string toolnames;
    unsigned coolChannelNbr=m_blobTool->nameToChannelNumber(m_key);
    CondAttrListCollection* attrColl=nullptr;
    if (detStore()->contains<CondAttrListCollection>(m_inlineFolder)) {
      CHECK(detStore()->retrieve(attrColl,m_inlineFolder));
    }
    else {
      attrColl=new CondAttrListCollection(true);
      CHECK(detStore()->record(attrColl,m_inlineFolder));
    }

    for (size_t i = 0; i < m_tools.size(); i++) {
      CHECK( m_tools[i]->mergeConstants (tc, ctx) );
      toolnames += m_tools[i]->name() + " ";	
    }
    coral::AttributeList* attrList=m_blobTool->ToolConstantsToAttrList(&tc);
    if (!attrList)
      return StatusCode::FAILURE;
    
    const std::string& tName=m_key;
    attrColl->add(coolChannelNbr,tName);
    attrColl->add(coolChannelNbr,*attrList);
    
    delete attrList;
    

    REPORT_MESSAGE(MSG::INFO) << "Recorded CondAttributeListCollection with key " << m_inlineFolder << " channel name " << m_key  
			      << " to DetStore.\n  Tools: " << toolnames;
  }
  else {
    auto tc = std::make_unique<CaloRec::ToolConstants>();
    std::string toolnames;
    for (size_t i = 0; i < m_tools.size(); i++) {
      CHECK( m_tools[i]->mergeConstants (*tc, ctx) );
      toolnames += m_tools[i]->name() + " ";
    }

    CHECK( detStore()->record (std::move(tc), m_key) );
    REPORT_MESSAGE(MSG::INFO) << "Recorded constants for key " << m_key
			      << " to DetStore.\n  Tools: " << toolnames;
  }//end else m_inline


  

  //std::cout << detStore()->dump() << std::endl;


  return StatusCode::SUCCESS;
}

//#############################################################################

StatusCode CaloClusterCorrDBWriter::execute (const EventContext& /*ctx*/) const
{
  return StatusCode::SUCCESS;
}


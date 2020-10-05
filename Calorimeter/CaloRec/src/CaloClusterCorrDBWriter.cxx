/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
//
// Description: see CaloClusterCorrDBWriter.h
//-----------------------------------------------------------------------


#include "CaloClusterCorrDBWriter.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"

#include "CaloRec/ToolWithConstantsMixin.h"
#include "CaloConditions/ToolConstants.h"
#include "AthenaKernel/errorcheck.h"

//#include "ToolConstants2Blob.h"


using namespace CaloRec;

//#############################################################################
CaloClusterCorrDBWriter::CaloClusterCorrDBWriter(const std::string& name, 
						 ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_blobTool("Blob2ToolConstants")
{
  // Name(s) of Cluster Correction Tools
  declareProperty("ClusterCorrectionTools", m_correctionToolNames);
  declareProperty("key",m_key="");
  declareProperty("COOLInlineFolder",m_inlineFolder);
}

//#############################################################################

CaloClusterCorrDBWriter::~CaloClusterCorrDBWriter()
{ }

//#############################################################################

StatusCode CaloClusterCorrDBWriter::initialize()
{
  //Get ToolSvc  
  IToolSvc*     p_toolSvc;
  CHECK( service("ToolSvc", p_toolSvc) );

  CHECK( m_blobTool.retrieve());

  
  // allocate tools derived from ToolsWithConstants
  std::vector<std::string>::const_iterator firstTool=m_correctionToolNames.begin();
  std::vector<std::string>::const_iterator lastTool =m_correctionToolNames.end();
  for ( ; firstTool != lastTool; ++firstTool ) {
    IAlgTool* algToolPtr;
    ListItem  clusAlgoTool(*firstTool);
    StatusCode scTool = p_toolSvc->retrieveTool(clusAlgoTool.type(),
						clusAlgoTool.name(),
						algToolPtr,
						this);
    if ( scTool.isFailure() ) {
      REPORT_MESSAGE(MSG::ERROR) << "Cannot find tool for " << *firstTool;
    }
    else {
      REPORT_MESSAGE(MSG::INFO) << m_key << ": "
                                << "Found tool for " << *firstTool;
      
      // check for tool type
      CaloRec::ToolWithConstantsMixin* theTool = 
	dynamic_cast<CaloRec::ToolWithConstantsMixin*>(algToolPtr);
      if ( theTool != nullptr ) { 
	m_correctionTools.push_back(theTool);
      }
    }
  }
  REPORT_MESSAGE(MSG::INFO) << m_key << ": "
                            << "Found " << m_correctionTools.size() <<
    " tools.";
  return StatusCode::SUCCESS;
}

//#############################################################################

StatusCode CaloClusterCorrDBWriter::finalize()
{
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

    for (size_t i = 0; i < m_correctionTools.size(); i++) {
      CHECK( m_correctionTools[i]->mergeConstants (tc) );
      toolnames += m_correctionTools[i]->name() + " ";	
    }
    coral::AttributeList* attrList=m_blobTool->ToolConstantsToAttrList(&tc);
    if (!attrList)
      return StatusCode::FAILURE;
    
    const std::string& tName=m_key;//m_correctionTools[i]->name();
    attrColl->add(coolChannelNbr,tName);
    attrColl->add(coolChannelNbr,*attrList);
    
    delete attrList;
    

    REPORT_MESSAGE(MSG::INFO) << "Recorded CondAttributeListCollection with key " << m_inlineFolder << " channel name " << m_key  
			      << " to DetStore.\n  Tools: " << toolnames;
  }
  else {
    auto tc = std::make_unique<CaloRec::ToolConstants>();
    std::string toolnames;
    for (size_t i = 0; i < m_correctionTools.size(); i++) {
      CHECK( m_correctionTools[i]->mergeConstants (*tc) );
      toolnames += m_correctionTools[i]->name() + " ";
    }

    CHECK( detStore()->record (std::move(tc), m_key) );
    REPORT_MESSAGE(MSG::INFO) << "Recorded constants for key " << m_key
			      << " to DetStore.\n  Tools: " << toolnames;
  }//end else m_inline


  

  //std::cout << detStore()->dump() << std::endl;


  return StatusCode::SUCCESS;
}

//#############################################################################

StatusCode CaloClusterCorrDBWriter::execute()
{
  return StatusCode::SUCCESS;
}


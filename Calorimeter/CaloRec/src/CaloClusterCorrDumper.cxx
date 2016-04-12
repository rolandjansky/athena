/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
//
// Description: see CaloClusterCorrDumper.h
//-----------------------------------------------------------------------


#include "CaloRec/CaloClusterCorrDumper.h"
//#include "GaudiKernel/ISvcLocator.h"
//#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"

#include "StoreGate/StoreGateSvc.h" 
//#include "CaloRec/ToolWithConstantsMixin.h"
#include "CaloConditions/ToolConstants.h"

#include <fstream>


//###############################################################################
CaloClusterCorrDumper::CaloClusterCorrDumper(const std::string& name, 
                                             ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator)
{
  // Name(s) of Cluster Correction Tools
  declareProperty("ClusterCorrectionTools", m_correctionToolNames,
		  "List of Tools whose constants should be dumped");
  declareProperty("FileName",m_fileName="","Name of output text file");
}

//###############################################################################

CaloClusterCorrDumper::~CaloClusterCorrDumper()
{ }

//###############################################################################


StatusCode CaloClusterCorrDumper::execute() {
  return StatusCode::SUCCESS;
}


StatusCode CaloClusterCorrDumper::initialize()
{
  MsgStream log(messageService(), name());

  //Get ToolSvc  
  IToolSvc*     p_toolSvc;
  CHECK( service("ToolSvc", p_toolSvc) );
  
  // allocate tools derived from ToolsWithConstants
  std::vector<std::string>::const_iterator firstTool=m_correctionToolNames.begin();
  std::vector<std::string>::const_iterator lastTool =m_correctionToolNames.end();
  for ( ; firstTool != lastTool; firstTool++ ) {
    IAlgTool* algToolPtr;
    ListItem  clusAlgoTool(*firstTool);
    StatusCode scTool = p_toolSvc->retrieveTool(clusAlgoTool.type(),
						clusAlgoTool.name(),
						algToolPtr,
						this
						);
    if ( scTool.isFailure() ) {
      REPORT_MESSAGE(MSG::ERROR) << "Cannot find tool for " << *firstTool;
    }
    else {
      REPORT_MESSAGE(MSG::INFO) /*<< m_key << ": "*/
                                << "Found tool for " << *firstTool;
      
      // check for tool type
      CaloRec::ToolWithConstantsMixin* theTool = 
	dynamic_cast<CaloRec::ToolWithConstantsMixin*>(algToolPtr);
      if ( theTool != 0 ) { 
	m_correctionTools.push_back(theTool);
      }
    }
  }
  REPORT_MESSAGE(MSG::INFO) /*<< m_key << ": "*/
                            << "Found " << m_correctionTools.size() <<
    " tools.";

  return StatusCode::SUCCESS;
}

StatusCode CaloClusterCorrDumper::finalize() {
  MsgStream log(messageService(), name());
  std::ofstream file;
  file.open(m_fileName.c_str(),std::ios::app);
  if (!file.is_open()) {
    log << MSG::FATAL << "Failed to open file named " << m_fileName << endreq;
    return StatusCode::FAILURE;
  }



  std::vector<CaloRec::ToolWithConstantsMixin*>::const_iterator it=m_correctionTools.begin();
  std::vector<CaloRec::ToolWithConstantsMixin*>::const_iterator it_e=m_correctionTools.end();
  for (;it!=it_e;++it) {
    file << "Dump of constants:" << std::endl;
    (*it)->writeConstants(file,"dumper");
  }


  file.close();
  return StatusCode::SUCCESS;
}




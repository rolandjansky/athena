/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonJiveXML/CSCClusterRetriever.h"
#include "MuonJiveXML/MuonFullIDHelper.h"

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  CSCClusterRetriever::CSCClusterRetriever(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_typeName("CSC")
  {
    
    //Declare the interface
    declareInterface<IDataRetriever>(this);
 
    //And the properties
    declareProperty("StoreGateKey", m_sgKey = "CSC_Clusters", "Name of the CSC_ClusterCollection");
  }


  StatusCode CSCClusterRetriever::initialize(){
    
    //be verbose
    if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Initializing retriever for " << dataTypeName()); 
    
    return StatusCode::SUCCESS;
  }        

  /**
   * For the CSC Cluster collection retrieve all data
   * - loop over all clusters in the collection
   * - get all the basic parameters
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode CSCClusterRetriever::retrieve(ToolHandle<IFormatTool> &/*FormatTool*/) {
    return StatusCode::SUCCESS;
  }
}

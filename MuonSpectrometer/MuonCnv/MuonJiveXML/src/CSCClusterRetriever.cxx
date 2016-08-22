/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonJiveXML/CSCClusterRetriever.h"
#include "MuonJiveXML/MuonFullIDHelper.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/CscIdHelper.h"

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

  /**
   * Initialize before event loop
   * - get the MuonDetectorManager
   */
  StatusCode CSCClusterRetriever::initialize(){
    
    //be verbose
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initializing retriever for " << dataTypeName() << endmsg; 
    
    //Retrieve the CSC ID helper
    const MuonGM::MuonDetectorManager* muon_mgr;
    if ( detStore()->retrieve(muon_mgr).isFailure()){
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not retrieve MuonDetectorManager!" << endmsg;
      return StatusCode::FAILURE;
    } 

    return StatusCode::SUCCESS;
  }        

  /**
   * For the CSC Cluster collection retrieve all data
   * - loop over all clusters in the collection
   * - get all the basic parameters
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode CSCClusterRetriever::retrieve(ToolHandle<IFormatTool> &/*FormatTool*/) {
   
    // //be verbose
    // if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieving " << dataTypeName() << endmsg; 
    
    // //retrieve the cluster collection
    // const CscClusterCollection* container;
    // if( evtStore()->retrieve(container,m_sgKey).isFailure() ) {
    //   if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Could not find CscClusterCollection " << m_sgKey << endmsg;
    //   return StatusCode::SUCCESS;
    // }

    // //Abort if the container is empty
    // if (container->empty()){
    //   if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "CscClusterCollection with name " << m_sgKey << " is empty!" << endmsg;
    //   return StatusCode::SUCCESS;
    // }

    // //Make the vectors to contain the information      
    // //and reserve enough space
    // int nClusters = container->size();
    // DataVect x; x.reserve(nClusters);
    // DataVect y; y.reserve(nClusters);
    // DataVect z; z.reserve(nClusters);
    // DataVect sigma; sigma.reserve(nClusters);
    // DataVect id; id.reserve(nClusters);
    // DataVect identifier; identifier.reserve(nClusters);
    // DataVect numHits; numHits.reserve(nClusters);
    // DataVect barcode; barcode.reserve(nClusters);
    // DataVect hitsFull; 

    // //Get the number of full hits, and reserve enough space
    // int hitsCount = 0;
    // //Get an iterator over the clusters
    // //and calculate number of hits
    // CscClusterCollection::const_iterator itr = container->begin();
    // for(; itr != container->end() ; ++itr)
    //   hitsCount += ((*itr)->digit_iterator_end() - (*itr)->digit_iterator_begin());
   
    // //Reserve enough space for hits
    // std::string noHitsFull = DataType( hitsCount/((double)(container->size())) ).toString();
    // hitsFull.reserve(hitsCount);

    // //Now loop again to retrieve the information
    // for(itr = container->begin(); itr != container->end() ; ++itr){
    //   x.push_back( DataType( (*itr)->position().x()/CLHEP::cm ) ); 
    //   y.push_back( DataType( (*itr)->position().y()/CLHEP::cm ) ); 
    //   z.push_back( DataType( (*itr)->position().z()/CLHEP::cm ) );  
    //   sigma.push_back( DataType( (*itr)->sigma() ) );  
    //   id.push_back( DataType( (*itr)->identify().get_compact() ) ); 
    //   identifier.push_back( DataType( MuonFullIDHelper::getFullID((*itr)->identify(),m_cscHelper) ) );  
    //   //Barcode is always zero
    //   barcode.push_back(DataType((int)0));  

    //   //Now loop over CSC digits
    //   std::vector<CscDigit*>::const_iterator dig = (*itr)->digit_iterator_begin();
    //   //store all their ids
    //   for(; dig != (*itr)->digit_iterator_end(); ++dig)
    //     hitsFull.push_back( DataType( MuonFullIDHelper::getFullID( (*dig)->identify(),m_cscHelper ) ) );
    //   //and the number of them
    //   numHits.push_back( DataType(((*itr)->digit_iterator_end() - (*itr)->digit_iterator_begin())));
    // }
    
    // //Finally assemble the DataMap
    // DataMap myDataMap;
    // myDataMap["x"] = x;
    // myDataMap["y"] = y;
    // myDataMap["z"] = z;
    // myDataMap["sigma"] = sigma;
    // myDataMap["id"] = id;
    // myDataMap["identifier"] = identifier;
    // myDataMap["numHits"] = numHits;
    // myDataMap["barcode"] = barcode;
    // myDataMap["hitsFull multiple=\"" + noHitsFull + "\""] = hitsFull;

    // //Be verbose
    // if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": "<< x.size() << endmsg;

    // //forward data to formating tool
    // return FormatTool->AddToEvent(dataTypeName(), m_sgKey, &myDataMap);
    return StatusCode::SUCCESS;
  }
}

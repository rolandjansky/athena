/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloJiveXML/CaloClusterRetriever.h"
#include "AthenaKernel/Units.h"

using Athena::Units::GeV;

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  CaloClusterRetriever::CaloClusterRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_typeName("Cluster"){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);
    
    m_sgKeyFavourite = "LArClusterEM";
    declareProperty("FavouriteClusterCollection" ,m_sgKeyFavourite,
        "Collection to be first in output, shown in Atlantis without switching");
    declareProperty("OtherClusterCollections" ,m_otherKeys,
        "Other collections to be retrieved. If list left empty, all available retrieved");
    declareProperty("DoWriteHLT", m_doWriteHLT = false,"Ignore HLTAutokey object by default."); // ignore HLTAutoKey objects
  }
   
  /**
   * For each cluster collections retrieve basic parameters.
   * 'Favourite' cluster collection first, then 'Other' collections.
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode CaloClusterRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieveAll()" << endreq;
    
    const DataHandle<xAOD::CaloClusterContainer> iterator, end;
    const xAOD::CaloClusterContainer* ccc;

    //obtain the default collection first
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve " << dataTypeName() << " (" << m_sgKeyFavourite << ")" << endreq;

    if ( evtStore()->retrieve(ccc, m_sgKeyFavourite).isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Favourite Collection " << m_sgKeyFavourite << " not found in SG " << endreq; 
    }else{
      DataMap data = getData(ccc);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKeyFavourite+"_ESD", &data).isFailure()){
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Favourite Collection " << m_sgKeyFavourite << " not found in SG " << endreq;
      }else{
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << dataTypeName() << " (" << m_sgKeyFavourite << ") CaloCluster retrieved" << endreq;
      }
    }

    if ( m_otherKeys.empty() ) {
      //obtain all other collections from StoreGate
      if (( evtStore()->retrieve(iterator, end)).isFailure()){
         if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << 
	 "Unable to retrieve iterator for CaloCluster collection" << endreq;
//        return false;
      }
      
      for (; iterator!=end; iterator++) {

        std::string::size_type position = iterator.key().find("HLTAutoKey",0);
        if ( m_doWriteHLT ){ position = 99; } // override SG key find

//      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " CaloCluster: HLTAutoKey in " << iterator.key() << " at position " 
//	    << position << endreq;
        if ( position != 0 ){  // SG key doesn't contain HLTAutoKey         
	  if (iterator.key()!=m_sgKeyFavourite) {
             if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve all " << dataTypeName() << " (" << iterator.key() << ")" << endreq;
             DataMap data = getData(iterator);
             if ( FormatTool->AddToEvent(dataTypeName(), iterator.key()+"_ESD", &data).isFailure()){
	       if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << iterator.key() << " not found in SG " << endreq;
	    }else{
	      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << iterator.key() << ") CaloCluster retrieved" << endreq;
	    }
          }
	}
      }
    }else {
      //obtain all collections with keys provided by user: m_otherKeys
      std::vector<std::string>::const_iterator keyIter,endIter;
      for ( keyIter=m_otherKeys.begin(); keyIter!=m_otherKeys.end(); ++keyIter ){
       if ( evtStore()->contains<xAOD::CaloClusterContainer>(*keyIter) ){ // to avoid some SG dumps
	if ( !evtStore()->retrieve( ccc, (*keyIter) ).isFailure()) {
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve selected " << dataTypeName() << " (" << (*keyIter) << ")" << endreq;
          DataMap data = getData(ccc);
          if ( FormatTool->AddToEvent(dataTypeName(), (*keyIter)+"_ESD", &data).isFailure()){
	    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << (*keyIter) << " not found in SG " << endreq;
	  }else{
	     if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << (*keyIter) << ") retrieved" << endreq;
	  }
	}
       }
      }
    }
    //All collections retrieved okay
    return StatusCode::SUCCESS;
  }


  /**
   * Retrieve basic parameters, mainly four-vectors.
   *  Clusters have no cells (trying to access them without
   * back-navigation causes Athena crash).
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  const DataMap CaloClusterRetriever::getData(const xAOD::CaloClusterContainer* ccc) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "getData()" << endreq;

    DataMap DataMap;

    DataVect phi; phi.reserve(ccc->size());
    DataVect eta; eta.reserve(ccc->size());
    DataVect et; et.reserve(ccc->size());
    DataVect idVec; idVec.reserve(ccc->size());
    DataVect numCellsVec; numCellsVec.reserve(ccc->size());

    std::string tagCells;
    xAOD::CaloClusterContainer::const_iterator itr = ccc->begin();  
    int noClu = ccc->size();
    int noCells = 0;
    for (; itr != ccc->end(); ++itr){ 
      for(xAOD::CaloCluster::const_cell_iterator it = (*itr)->cell_begin(); 
              it != (*itr)->cell_end() ; ++it){
         ++noCells;
      }
    }
    if(noClu){
	tagCells = "cells multiple=\"" +DataType(noCells/(noClu*1.0)).toString()+"\"";
    }else{
	tagCells = "cells multiple=\"1.0\"";
    }
    DataVect cells; cells.reserve( ccc->size() );
    
    itr = ccc->begin(); // reset iterator
    int id = 0;
    for (; itr != ccc->end(); ++itr) {
      phi.push_back(DataType((*itr)->phi()));
      eta.push_back(DataType((*itr)->eta()));
      et.push_back(DataType((*itr)->et()*(1./GeV)));
      idVec.push_back(DataType( ++id ));

      int numCells = 0;
      xAOD::CaloCluster::const_cell_iterator cell = (*itr)->cell_begin();
      for(; cell != (*itr)->cell_end() ; ++cell){
	cells.push_back(DataType((*cell)->ID().get_compact()));
	++numCells;
      }
      numCellsVec.push_back(DataType( numCells ));
    }

    // Start with mandatory entries
    DataMap["phi"] = phi;
    DataMap["eta"] = eta;
    DataMap["et"] = et;
    DataMap[tagCells] = cells;
    DataMap["numCells"] = numCellsVec;
    DataMap["id"] = idVec;

    //Be verbose
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " , collection: " << dataTypeName();
      msg(MSG::DEBUG) << " retrieved with " << phi.size() << " entries"<< endreq;
    }

    //All collections retrieved okay
    return DataMap;

  } // retrieve

  //--------------------------------------------------------------------------
  
} // JiveXML namespace

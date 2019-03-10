/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloJiveXML/CaloClusterRetriever.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCell.h"

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
    
    ATH_MSG_DEBUG( "in retrieveAll()"  );
    
    const DataHandle<CaloClusterContainer> iterator, end;
    const CaloClusterContainer* ccc;

    //obtain the default collection first
    ATH_MSG_DEBUG( "Trying to retrieve " << dataTypeName() << " (" << m_sgKeyFavourite << ")"  );

    if ( evtStore()->retrieve(ccc, m_sgKeyFavourite).isFailure() ) {
      ATH_MSG_WARNING( "Favourite Collection " << m_sgKeyFavourite << " not found in SG "  );
    }else{
      DataMap data = getData(ccc);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKeyFavourite+"_ESD", &data).isFailure()){
	ATH_MSG_WARNING( "Favourite Collection " << m_sgKeyFavourite << " not found in SG "  );
      }else{
        ATH_MSG_DEBUG( dataTypeName() << " (" << m_sgKeyFavourite << ") CaloCluster retrieved"  );
      }
    }

    if ( m_otherKeys.empty() ) {
      //obtain all other collections from StoreGate
      if (( evtStore()->retrieve(iterator, end)).isFailure()){
        ATH_MSG_WARNING( "Unable to retrieve iterator for CaloCluster collection"  );
//        return false;
      }
      
      for (; iterator!=end; iterator++) {

        std::string::size_type position = iterator.key().find("HLTAutoKey",0);
        if ( m_doWriteHLT ){ position = 99; } // override SG key find

        if ( position != 0 ){  // SG key doesn't contain HLTAutoKey         
	  if (iterator.key()!=m_sgKeyFavourite) {
             ATH_MSG_DEBUG( "Trying to retrieve all " << dataTypeName() << " (" << iterator.key() << ")"  );
             DataMap data = getData(iterator);
             if ( FormatTool->AddToEvent(dataTypeName(), iterator.key()+"_ESD", &data).isFailure()){
	       ATH_MSG_WARNING( "Collection " << iterator.key() << " not found in SG "  );
	    }else{
               ATH_MSG_DEBUG( dataTypeName() << " (" << iterator.key() << ") CaloCluster retrieved"  );
	    }
          }
	}
      }
    }else {
      //obtain all collections with keys provided by user: m_otherKeys
      std::vector<std::string>::const_iterator keyIter;
      for ( keyIter=m_otherKeys.begin(); keyIter!=m_otherKeys.end(); ++keyIter ){
       if ( evtStore()->contains<CaloClusterContainer>(*keyIter) ){ // to avoid some SG dumps
	if ( !evtStore()->retrieve( ccc, (*keyIter) ).isFailure()) {
          ATH_MSG_DEBUG( "Trying to retrieve selected " << dataTypeName() << " (" << (*keyIter) << ")"  );
          DataMap data = getData(ccc);
          if ( FormatTool->AddToEvent(dataTypeName(), (*keyIter)+"_ESD", &data).isFailure()){
	    ATH_MSG_WARNING( "Collection " << (*keyIter) << " not found in SG "  );
	  }else{
            ATH_MSG_DEBUG( dataTypeName() << " (" << (*keyIter) << ") retrieved"  );
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
  const DataMap CaloClusterRetriever::getData(const CaloClusterContainer* ccc) {
    
    ATH_MSG_DEBUG( "getData()"  );

    DataMap DataMap;

    DataVect phi; phi.reserve(ccc->size());
    DataVect eta; eta.reserve(ccc->size());
    DataVect et; et.reserve(ccc->size());
    DataVect idVec; idVec.reserve(ccc->size());
    DataVect numCellsVec; numCellsVec.reserve(ccc->size());

    std::string tagCells;
    CaloClusterContainer::const_iterator itr = ccc->begin();  
    int noClu = ccc->size();
    int noCells = 0;
    for (; itr != ccc->end(); ++itr){ 
      for(CaloCluster::cell_iterator it = (*itr)->cell_begin(); 
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
      CaloCluster::cell_iterator cell = (*itr)->cell_begin();
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
    ATH_MSG_DEBUG( dataTypeName() << " , collection: " << dataTypeName()
                   << " retrieved with " << phi.size() << " entries" );

    //All collections retrieved okay
    return DataMap;

  } // retrieve

  //--------------------------------------------------------------------------
  
} // JiveXML namespace

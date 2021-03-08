/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODJiveXML/xAODCaloClusterRetriever.h"

#include "CaloGeoHelpers/CaloSampling.h"

#include "AthenaKernel/Units.h"
using Athena::Units::GeV;

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  xAODCaloClusterRetriever::xAODCaloClusterRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_typeName("Cluster"){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);
    
    m_sgKeyFavourite = "egammaClusters"; // new SGKey in rel.20
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
  StatusCode xAODCaloClusterRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieveAll()" << endmsg;
    
    const DataHandle<xAOD::CaloClusterContainer> iterator, end;
    const xAOD::CaloClusterContainer* ccc;

    //obtain the default collection first
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve " << dataTypeName() << " (" << m_sgKeyFavourite << ")" << endmsg;
    StatusCode sc = evtStore()->retrieve(ccc, m_sgKeyFavourite);
    if (sc.isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKeyFavourite << " not found in SG " << endmsg; 
    }else{
      DataMap data = getData(ccc);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKeyFavourite+"_xAOD", &data).isFailure()){
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKeyFavourite << " not found in SG " << endmsg;
      }else{
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << dataTypeName() << " (" << m_sgKeyFavourite << ") AODCaloCluster retrieved" << endmsg;
      }
    }

    if ( m_otherKeys.empty() ) {
      //obtain all other collections from StoreGate
      if (( evtStore()->retrieve(iterator, end)).isFailure()){
         if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << 
	 "Unable to retrieve iterator for AODCaloCluster collection" << endmsg;
//        return false;
      }
      
      for (; iterator!=end; iterator++) {
       	     if ((iterator.key().find("HLT",0) != std::string::npos) && (!m_doWriteHLT)){
	          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Ignoring HLT collection: " << iterator.key() << endmsg;
	         continue;  }
	  if (iterator.key()!=m_sgKeyFavourite) {
             if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve all. Current collection: " << dataTypeName() << " (" << iterator.key() << ")" << endmsg;
             DataMap data = getData(iterator);
             if ( FormatTool->AddToEvent(dataTypeName(), iterator.key()+"_xAOD", &data).isFailure()){
	       if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << iterator.key() << " not found in SG " << endmsg;
	    }else{
	      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << iterator.key() << ") AODCaloCluster retrieved" << endmsg;
	    }
          }
      }
    }else {
      //obtain all collections with the given keys
      std::vector<std::string>::const_iterator keyIter;
      for ( keyIter=m_otherKeys.begin(); keyIter!=m_otherKeys.end(); ++keyIter ){
	StatusCode sc = evtStore()->retrieve( ccc, (*keyIter) );
	if (!sc.isFailure()) {
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve selected " << dataTypeName() << " (" << (*keyIter) << ")" << endmsg;
          DataMap data = getData(ccc);
          if ( FormatTool->AddToEvent(dataTypeName(), (*keyIter), &data).isFailure()){
	    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << (*keyIter) << " not found in SG " << endmsg;
	  }else{
	     if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << (*keyIter) << ") retrieved" << endmsg;
	  }
	}
      }
    }
    //All collections retrieved okay
    return StatusCode::SUCCESS;
  }


  /**
   * Retrieve basic parameters, mainly four-vectors.
   * AOD Clusters have no cells (trying to access them without
   * back-navigation causes Athena crash).
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  const DataMap xAODCaloClusterRetriever::getData(const xAOD::CaloClusterContainer* ccc) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "retrieve()" << endmsg;

    DataMap DataMap;

    DataVect phi; phi.reserve(ccc->size());
    DataVect eta; eta.reserve(ccc->size());
    DataVect et; et.reserve(ccc->size());
    DataVect cells; cells.reserve(ccc->size());
    DataVect numCells; numCells.reserve(ccc->size());
    DataVect idVec; idVec.reserve(ccc->size());
    DataVect emfracVec; emfracVec.reserve(ccc->size());
    DataVect labelVec; labelVec.reserve(ccc->size());

    std::string label="";
    int id = 0;
    int s = 0;
    float eInSample = 0.; 
    float eInSampleFull = 0.; 
    float emfrac = 0.; 
    float rawemfrac = 0.; 

// cells n/a in AOD, but keep this for compatibility
// with 'full' clusters in AtlantisJava
    std::string tagCells;
    tagCells = "cells multiple=\"1.0\"";

    xAOD::CaloClusterContainer::const_iterator itr = ccc->begin();
    for (; itr != ccc->end(); ++itr) {

// sum over samplings to get EMfraction: 
//// works from AOD ! Info from Sven Menke 5Aug10
// full sum:
      for (s=0;s<CaloSampling::Unknown; s++){
	eInSampleFull += (*itr)->eSample(CaloSampling::CaloSample(s));
      }
// Now only EMB1-3, EME1-3 and FCAL1:
	eInSample += (*itr)->eSample(CaloSampling::EMB1);
	eInSample += (*itr)->eSample(CaloSampling::EMB2);
	eInSample += (*itr)->eSample(CaloSampling::EMB3);
	eInSample += (*itr)->eSample(CaloSampling::EME1);
	eInSample += (*itr)->eSample(CaloSampling::EME2);
	eInSample += (*itr)->eSample(CaloSampling::EME3);
	eInSample += (*itr)->eSample(CaloSampling::FCAL1);

      emfrac  = eInSample/eInSampleFull;
      rawemfrac = emfrac;
// sanity cut: emfrac should be within [0,1]
      if ( emfrac > 1.0 ) emfrac = 1.;
      if ( emfrac < 0.0 ) emfrac = 0.;
      emfracVec.push_back(  DataType(emfrac).toString() );

      if ( DataType( eInSample ).toString() != "0." ){
       label = "AllMeV_SumEMSampl=" + DataType( eInSample ).toString() +
  	 "_SumAllSampl=" + DataType( eInSampleFull ).toString() +
  	 "_calcEMFrac=" + DataType( rawemfrac ).toString()+
  	 "_outEMFrac=" + DataType( emfrac ).toString();
      }else{ label = "n_a"; }
      eInSample = 0.;
      eInSampleFull = 0.;

      labelVec.push_back( label );
      if (msgLvl(MSG::VERBOSE)) {
	msg(MSG::VERBOSE) << "label is " << label << endmsg;
      }

// now the standard variables
// getBasicEnergy n/a in xAOD !

      phi.push_back(DataType((*itr)->phi()));
      eta.push_back(DataType((*itr)->eta()));
      et.push_back(DataType((*itr)->et()/GeV));
      numCells.push_back(DataType( "0" ));
      cells.push_back(DataType( "0" ));
      idVec.push_back(DataType( ++id ));

      if (msgLvl(MSG::VERBOSE)) {
        msg(MSG::VERBOSE) << dataTypeName() << " cluster #" << id  
                          << " ,e=" <<  (*itr)->e()/GeV  << ", et=";
        msg(MSG::VERBOSE) << (*itr)->et()/GeV << ", eta=" << (*itr)->eta() 
		<< ", phi=" << (*itr)->phi() << endmsg;
      }

    }
    // Start with mandatory entries
    DataMap["phi"] = phi;
    DataMap["eta"] = eta;
    DataMap["et"] = et;
    DataMap[tagCells] = cells;
    DataMap["numCells"] = numCells;
    DataMap["id"] = idVec;
    DataMap["emfrac"] = emfracVec; // not in Atlantis yet ! Could be used in legoplot
    DataMap["label"] = labelVec; // not in Atlantis yet ! 

    //Be verbose
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " (AOD, no cells), collection: " << dataTypeName();
      msg(MSG::DEBUG) << " retrieved with " << phi.size() << " entries"<< endmsg;
    }

    //All collections retrieved okay
    return DataMap;

  } // retrieve

  //--------------------------------------------------------------------------
  
} // JiveXML namespace

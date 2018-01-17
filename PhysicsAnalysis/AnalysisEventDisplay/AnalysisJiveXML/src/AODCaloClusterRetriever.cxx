/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisJiveXML/AODCaloClusterRetriever.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloClusterMoment.h"
#include "CaloEvent/CaloVariableType.h"
#include "CaloGeoHelpers/CaloSampling.h"

#include "CLHEP/Units/SystemOfUnits.h"

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  AODCaloClusterRetriever::AODCaloClusterRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_typeName("Cluster"){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);
    
    m_sgKeyFavourite = "egClusterCollection";
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
  StatusCode AODCaloClusterRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieveAll()" << endmsg;
    
    const DataHandle<CaloClusterContainer> iterator, end;
    const CaloClusterContainer* ccc;

    //obtain the default collection first
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve " << dataTypeName() << " (" << m_sgKeyFavourite << ")" << endmsg;
    StatusCode sc = evtStore()->retrieve(ccc, m_sgKeyFavourite);
    if (sc.isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKeyFavourite << " not found in SG " << endmsg; 
    }else{
      DataMap data = getData(ccc, false);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKeyFavourite, &data).isFailure()){
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKeyFavourite << " not found in SG " << endmsg;
      }else{
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << dataTypeName() << " (" << m_sgKeyFavourite << ") AODCaloCluster retrieved" << endmsg;
      }
    }

    // uncalibrated topo clusters: calibFlag=true
    //obtain the default collection first
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve CaloCalTopoCluster (for non-calib)" << endmsg;
    StatusCode sc3 = evtStore()->retrieve(ccc, "CaloCalTopoCluster");
    if (sc3.isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection CaloCalTopoCluster (for non-calib) not found in SG " << endmsg; 
    }else{
      DataMap data = getData(ccc, true); // calibFlag: If true, use getBasicEnergy() instead of et()
      if ( FormatTool->AddToEvent(dataTypeName(), "BasicEnergyCaloCalTopoCluster", &data).isFailure()){
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection CaloCalTopoCluster (for non-calib) not found in SG " << endmsg;
      }else{
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << dataTypeName() << " CaloCalTopoCluster (for non-calib) AODCaloCluster retrieved" << endmsg;
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

        std::string::size_type position = iterator.key().find("HLTAutoKey",0);
        if ( m_doWriteHLT ){ position = 99; } // override SG key find

//      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " AODCaloCluster: HLTAutoKey in " << iterator.key() << " at position " 
//	    << position << endmsg;
        if ( position != 0 ){  // SG key doesn't contain HLTAutoKey         
	  if (iterator.key()!=m_sgKeyFavourite) {
             if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve all " << dataTypeName() << " (" << iterator.key() << ")" << endmsg;
             DataMap data = getData(iterator, false);
             if ( FormatTool->AddToEvent(dataTypeName(), iterator.key(), &data).isFailure()){
	       if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << iterator.key() << " not found in SG " << endmsg;
	    }else{
	      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << iterator.key() << ") AODCaloCluster retrieved" << endmsg;
	    }
          }
	}
      }
    }else {
      //obtain all collections with the given keys
      std::vector<std::string>::const_iterator keyIter,endIter;
      for ( keyIter=m_otherKeys.begin(); keyIter!=m_otherKeys.end(); ++keyIter ){
	StatusCode sc = evtStore()->retrieve( ccc, (*keyIter) );
	if (!sc.isFailure()) {
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve selected " << dataTypeName() << " (" << (*keyIter) << ")" << endmsg;
          DataMap data = getData(ccc, false);
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
  const DataMap AODCaloClusterRetriever::getData(const CaloClusterContainer* ccc, bool calibFlag) {
    
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

    CaloClusterContainer::const_iterator itr = ccc->begin();
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
      emfracVec.push_back( emfrac );

      label = "AllMeV_SumEMSampl=" + DataType( eInSample ).toString() +
  	"_SumAllSampl=" + DataType( eInSampleFull ).toString() +
  	"_basicEnergy=" + DataType( (*itr)->getBasicEnergy()).toString() +
  	"_calcEMFrac=" + DataType( rawemfrac ).toString()+
  	"_outEMFrac=" + DataType( emfrac ).toString();
      eInSample = 0.;
      eInSampleFull = 0.;

      labelVec.push_back( label );
      if (msgLvl(MSG::DEBUG)) {
	msg(MSG::DEBUG) << "label is " << label << endmsg;
      }

// now the standard variables

      phi.push_back(DataType((*itr)->phi()));
      eta.push_back(DataType((*itr)->eta()));
      if (!calibFlag){ // default: just take et
        et.push_back(DataType((*itr)->et()/CLHEP::GeV));
      }else{ // non-calib energies: need to convert to et by hand
        et.push_back(DataType( (((*itr)->getBasicEnergy()/CLHEP::GeV)*((*itr)->sinTh())) ));
//// this would work also for egClusterCollection, but is it necessary ?
////        et.push_back(DataType( (  eInSampleFull *((*itr)->sinTh())) ));
      }
      // emfrac.push_back(DataType( (*itr)->getMomentValue(CaloClusterMoment::ENG_FRAC_EM) )); // doesn't work for AOD
      numCells.push_back(DataType( "0" ));
      cells.push_back(DataType( "0" ));
      idVec.push_back(DataType( ++id ));

//      if (msgLvl(MSG::DEBUG)) {
//        msg(MSG::DEBUG) << dataTypeName() << " cluster basic=" << (*itr)->getBasicEnergy()/CLHEP::GeV << " ,e=" <<  (*itr)->e()/CLHEP::GeV  << ", et=";
//        msg(MSG::DEBUG) << (*itr)->et()/CLHEP::GeV << ", eta=" << (*itr)->eta() << ", phi=" << (*itr)->phi() << ", sinTheta=" <<
//	   (*itr)->sinTh() << ",calc=" << ((*itr)->getBasicEnergy()/CLHEP::GeV)*((*itr)->sinTh()) << endmsg;
//      }

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

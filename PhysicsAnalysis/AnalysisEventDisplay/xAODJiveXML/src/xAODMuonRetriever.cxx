/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODJiveXML/xAODMuonRetriever.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODMuon/MuonContainer.h" 

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   *
   * code reference for xAOD:     jpt6Feb14
   *  https://svnweb.cern.ch/trac/atlasgroups/browser/PAT/AODUpgrade/xAODReaderAlgs
   *
   * This is afirst 'skeleton' try for many xAOD retrievers to be done:
   *  xAOD::Muon, xAOD::Vertex, xAOD::Photon, xAOD::CaloCluster, xAOD::Jet
   *  xAOD::TrackParticle, xAOD::TauJet, xAOD::Muon
   *
   * class defs:
   *    https://svnweb.cern.ch/trac/atlasoff/browser/Event/xAOD
   *
   **/
  xAODMuonRetriever::xAODMuonRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent), typeName("Muon"){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);

    m_sgKey = "Muons"; // is xAOD name
    declareProperty("StoreGateKey", m_sgKey, 
        "Collection to be first in output, shown in Atlantis without switching");
    declareProperty("OtherCollections" ,m_otherKeys,
        "Other collections to be retrieved. If list left empty, all available retrieved");
  }
  
  /**
   * For each jet collections retrieve basic parameters.
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode xAODMuonRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieveAll()" << endreq;
    
    const DataHandle<xAOD::MuonContainer> iterator, end;
    const xAOD::MuonContainer* muons;
    
    //obtain the default collection first
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve " << dataTypeName() << " (" << m_sgKey << ")" << endreq;
    StatusCode sc = evtStore()->retrieve(muons, m_sgKey);
    if (sc.isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endreq; 
    }else{
      DataMap data = getData(muons);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKey+"_xAOD", &data).isFailure()){ //suffix can be removed later
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endreq;
      }else{
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << dataTypeName() << " (" << m_sgKey << ") Muon retrieved" << endreq;
      }
    }
 

    if ( m_otherKeys.empty() ) {
      //obtain all other collections from StoreGate
      if (( evtStore()->retrieve(iterator, end)).isFailure()){
         if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << 
	 "Unable to retrieve iterator for xAOD Muon collection" << endreq;
//        return false;
      }
      
      for (; iterator!=end; iterator++) {
	  if (iterator.key()!=m_sgKey) {
             if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve all. Current collection: " << dataTypeName() << " (" << iterator.key() << ")" << endreq;
             DataMap data = getData(iterator);
             if ( FormatTool->AddToEvent(dataTypeName(), iterator.key()+"_xAOD", &data).isFailure()){
	       if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << iterator.key() << " not found in SG " << endreq;
	    }else{
	      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << iterator.key() << ") xAOD Muon retrieved" << endreq;
	    }
          }
      }
    }else {
      //obtain all collections with the given keys
      std::vector<std::string>::const_iterator keyIter,endIter;
      for ( keyIter=m_otherKeys.begin(); keyIter!=m_otherKeys.end(); ++keyIter ){
	StatusCode sc = evtStore()->retrieve( muons, (*keyIter) );
	if (!sc.isFailure()) {
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve selected " << dataTypeName() << " (" << (*keyIter) << ")" << endreq;
          DataMap data = getData(muons);
          if ( FormatTool->AddToEvent(dataTypeName(), (*keyIter), &data).isFailure()){
	    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << (*keyIter) << " not found in SG " << endreq;
	  }else{
	     if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << (*keyIter) << ") retrieved" << endreq;
	  }
	}
      }
    }

    //All collections retrieved okay
    return StatusCode::SUCCESS;
  }


  /**
   * Retrieve basic parameters, mainly four-vectors, for each collection.
   * Also association with clusters and tracks (ElementLink).
   */
  const DataMap xAODMuonRetriever::getData(const xAOD::MuonContainer* muCont) {

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in getData()" << endreq;

    DataMap m_DataMap;

    DataVect pt; pt.reserve(muCont->size());
    DataVect phi; phi.reserve(muCont->size());
    DataVect eta; eta.reserve(muCont->size());
    DataVect mass; mass.reserve(muCont->size());
    DataVect energy; energy.reserve(muCont->size());
    DataVect chi2; chi2.reserve(muCont->size());
    DataVect pdgId; pdgId.reserve(muCont->size());

    xAOD::MuonContainer::const_iterator muItr  = muCont->begin();
    xAOD::MuonContainer::const_iterator muItrE = muCont->end();

    int counter = 0;

    for (; muItr != muItrE; ++muItr) {

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "  Muon #" << counter++ << " : eta = "  << (*muItr)->eta() 
		      << ", phi = "  << (*muItr)->phi() << ", pt = " <<  (*muItr)->pt() 
		      << ", pdgId = " << -13.*(*muItr)->primaryTrackParticle()->charge() 
		      <<  endreq;
    }

      phi.push_back(DataType((*muItr)->phi()));
      eta.push_back(DataType((*muItr)->eta()));
      pt.push_back(DataType((*muItr)->pt()/CLHEP::GeV));

      mass.push_back(DataType((*muItr)->m()/CLHEP::GeV));
      energy.push_back( DataType((*muItr)->e()/CLHEP::GeV ) );
      chi2.push_back( 1.0 ); //placeholder
      pdgId.push_back(DataType( -13.*(*muItr)->primaryTrackParticle()->charge() )); // pdgId not available anymore in xAOD
    } // end MuonIterator 

    // four-vectors
    m_DataMap["phi"] = phi;
    m_DataMap["eta"] = eta;
    m_DataMap["pt"] = pt;
    m_DataMap["energy"] = energy;
    m_DataMap["mass"] = mass;
    m_DataMap["chi2"] = chi2;
    m_DataMap["pdgId"] = pdgId;

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " retrieved with " << phi.size() << " entries"<< endreq;
    }

    //All collections retrieved okay
    return m_DataMap;

  } // retrieve

  //--------------------------------------------------------------------------
  
} // JiveXML namespace

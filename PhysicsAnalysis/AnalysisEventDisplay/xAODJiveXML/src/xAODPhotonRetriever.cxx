/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODJiveXML/xAODPhotonRetriever.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODEgamma/PhotonContainer.h" 

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
   *  xAOD::Photon, xAOD::Vertex, xAOD::Photon, xAOD::CaloCluster, xAOD::Jet
   *  xAOD::TrackParticle, xAOD::TauJet, xAOD::Muon
   *
   **/
  xAODPhotonRetriever::xAODPhotonRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent), typeName("Photon"){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);

    m_sgKey = "Photons"; // is xAOD name
    declareProperty("StoreGateKey", m_sgKey, 
        "Collection to be first in output, shown in Atlantis without switching");
    declareProperty("OtherCollections" ,m_otherKeys,
        "Other collections to be retrieved. If list left empty, all available retrieved");
    declareProperty("DoWriteHLT"              , m_doWriteHLT = false, "Wether to write HLTAutoKey objects");
  }
  
  /**
   * For each jet collections retrieve basic parameters.
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode xAODPhotonRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieveAll()" << endreq;
    
    const DataHandle<xAOD::PhotonContainer> iterator, end;
    const xAOD::PhotonContainer* photons;
    
    //obtain the default collection first
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve " << dataTypeName() << " (" << m_sgKey << ")" << endreq;
    StatusCode sc = evtStore()->retrieve(photons, m_sgKey);
    if (sc.isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endreq; 
    }else{
      DataMap data = getData(photons);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKey+"_xAOD", &data).isFailure()){ //suffix can be removed later
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endreq;
      }else{
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << dataTypeName() << " (" << m_sgKey << ") Photon retrieved" << endreq;
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
       	     if ((iterator.key().find("HLT",0) != std::string::npos) && (!m_doWriteHLT)){
	          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Ignoring HLT-AutoKey collection " << iterator.key() << endreq;
	         continue;  }
             if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve all. Current collection: " << dataTypeName() << " (" << iterator.key() << ")" << endreq;
             DataMap data = getData(iterator);
             if ( FormatTool->AddToEvent(dataTypeName(), iterator.key()+"_xAOD", &data).isFailure()){
	       if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << iterator.key() << " not found in SG " << endreq;
	    }else{
	      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << iterator.key() << ") xAOD Photon retrieved" << endreq;
	    }
          }
      }
    }else {
      //obtain all collections with the given keys
      std::vector<std::string>::const_iterator keyIter,endIter;
      for ( keyIter=m_otherKeys.begin(); keyIter!=m_otherKeys.end(); ++keyIter ){
	StatusCode sc = evtStore()->retrieve( photons, (*keyIter) );
	if (!sc.isFailure()) {
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve selected " << dataTypeName() << " (" << (*keyIter) << ")" << endreq;
          DataMap data = getData(photons);
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
  const DataMap xAODPhotonRetriever::getData(const xAOD::PhotonContainer* phCont) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in getData()" << endreq;

    DataMap m_DataMap;

    DataVect pt; pt.reserve(phCont->size());
    DataVect phi; phi.reserve(phCont->size());
    DataVect eta; eta.reserve(phCont->size());
    DataVect mass; mass.reserve(phCont->size());
    DataVect energy; energy.reserve(phCont->size());

    DataVect isEMString; isEMString.reserve(phCont->size());
    DataVect author; author.reserve(phCont->size());
    DataVect label; label.reserve(phCont->size());

    xAOD::PhotonContainer::const_iterator phItr  = phCont->begin();
    xAOD::PhotonContainer::const_iterator phItrE = phCont->end();

    int counter = 0;

    for (; phItr != phItrE; ++phItr) {

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "  Photon #" << counter++ << " : eta = "  << (*phItr)->eta() << ", phi = " 
          << (*phItr)->phi() << endreq;
    }

    std::string photonAuthor = "";
    std::string photonIsEMString = "none";
    std::string photonLabel = "";

      phi.push_back(DataType((*phItr)->phi()));
      eta.push_back(DataType((*phItr)->eta()));
      pt.push_back(DataType((*phItr)->pt()/CLHEP::GeV));

      bool passesTight(false);
      bool passesMedium(false);
      bool passesLoose(false);
      const bool tightSelectionExists = (*phItr)->passSelection(passesTight, "Tight");
       msg(MSG::VERBOSE) << "tight exists " << tightSelectionExists 
	 << " and passes? " << passesTight << endreq;
      const bool mediumSelectionExists = (*phItr)->passSelection(passesMedium, "Medium");
       msg(MSG::VERBOSE) << "medium exists " << mediumSelectionExists 
	 << " and passes? " << passesMedium << endreq;
      const bool looseSelectionExists = (*phItr)->passSelection(passesLoose, "Loose");
       msg(MSG::VERBOSE) << "loose exists " << looseSelectionExists 
	<< " and passes? " << passesLoose << endreq;

      photonAuthor = "author"+DataType( (*phItr)->author() ).toString(); // for odd ones eg FWD
      photonLabel = photonAuthor;
      if (( (*phItr)->author()) == 0){ photonAuthor = "unknown"; photonLabel += "_unknown"; }
      if (( (*phItr)->author()) == 8){ photonAuthor = "forward"; photonLabel += "_forward"; }
      if (( (*phItr)->author()) == 2){ photonAuthor = "softe"; photonLabel += "_softe"; }
      if (( (*phItr)->author()) == 1){ photonAuthor = "egamma"; photonLabel += "_egamma"; }

      if ( passesLoose ){  
            photonLabel += "_Loose"; 
            photonIsEMString = "Loose"; // assume that hierarchy is obeyed !
      } 
      if ( passesMedium ){ 
            photonLabel += "_Medium"; 
            photonIsEMString = "Medium"; // assume that hierarchy is obeyed !
      }   
      if ( passesTight ){ 
            photonLabel += "_Tight"; 
            photonIsEMString = "Tight"; // assume that hierarchy is obeyed !
      }     
      author.push_back( DataType( photonAuthor ) );
      label.push_back( DataType( photonLabel ) );
      isEMString.push_back( DataType( photonIsEMString ) );

      mass.push_back(DataType((*phItr)->m()/CLHEP::GeV));
      energy.push_back( DataType((*phItr)->e()/CLHEP::GeV ) );
    } // end PhotonIterator 

    // four-vectors
    m_DataMap["phi"] = phi;
    m_DataMap["eta"] = eta;
    m_DataMap["pt"] = pt;
    m_DataMap["energy"] = energy;
    m_DataMap["mass"] = mass;
    m_DataMap["isEMString"] = isEMString;
    m_DataMap["label"] = label;
    m_DataMap["author"] = author;

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " retrieved with " << phi.size() << " entries"<< endreq;
    }

    //All collections retrieved okay
    return m_DataMap;

  } // retrieve

  //--------------------------------------------------------------------------
  
} // JiveXML namespace

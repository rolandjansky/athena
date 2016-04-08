/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODJiveXML/xAODMissingETRetriever.h"

#include "CLHEP/Units/SystemOfUnits.h"

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
   *  xAOD::MissingET, xAOD::Vertex, xAOD::MissingET, xAOD::CaloCluster, xAOD::Jet
   *  xAOD::TrackParticle, xAOD::TauJet, xAOD::Muon
   *
   **/
  xAODMissingETRetriever::xAODMissingETRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent), typeName("ETMis"){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);

    m_sgKey = "MET_Reference_AntiKt4EMTopo"; // New xAOD SG key in rel.20
    declareProperty("StoreGateKey", m_sgKey, // backwards compatibility only !
        "Collection to be first in output, shown in Atlantis without switching");
    declareProperty("FavouriteMETCollection" ,m_sgKeyFavourite = "MET_RefFinal" ,
	"Collection to be first in output, shown in Atlantis without switching");
    declareProperty("OtherMETCollections" ,m_otherKeys,
	"Other collections to be retrieved. If list left empty, all available retrieved");
  }
  
  /**
   * For each jet collections retrieve basic parameters.
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode xAODMissingETRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieveAll()" << endreq;
    
    const DataHandle<xAOD::MissingETContainer> iterator, end;
    const xAOD::MissingETContainer* MissingETs;

    //obtain the default collection first
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve " << dataTypeName() << " (" << m_sgKey << ")" << endreq;
    StatusCode sc = evtStore()->retrieve(MissingETs, m_sgKeyFavourite);
    if (sc.isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endreq; 
    }else{
      DataMap data = getData(MissingETs);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKey+"_xAOD", &data).isFailure()){ //suffix can be removed later
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endreq;
      }else{
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << dataTypeName() << " (" << m_sgKey << ") MissingET retrieved" << endreq;
      }
    }
 
    if ( m_otherKeys.empty() ) {
      //obtain all other collections from StoreGate
      if (( evtStore()->retrieve(iterator, end)).isFailure()){
         if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "Unable to retrieve iterator for MET collection" << endreq;
//        return false;
      }
      
      for (; iterator!=end; iterator++) {
	  if (iterator.key()!=m_sgKeyFavourite) {
             if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve all " << dataTypeName() << " (" << iterator.key() << ")" << endreq;
             DataMap data = getData(iterator);
             if ( FormatTool->AddToEvent(dataTypeName(), iterator.key()+"_xAOD", &data).isFailure()){
	       if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << iterator.key() << " not found in SG " << endreq;
	    }else{
	      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << iterator.key() << ") xAOD_MET retrieved" << endreq;
	    }
	}
      }
    }else {
      //obtain all collections with the given keys
      std::vector<std::string>::const_iterator keyIter,endIter;
      for ( keyIter=m_otherKeys.begin(); keyIter!=m_otherKeys.end(); ++keyIter ){
        if ( !evtStore()->contains<xAOD::MissingETContainer>( (*keyIter) ) ){ continue; } // skip if not in SG
	StatusCode sc = evtStore()->retrieve( MissingETs, (*keyIter) );
	if (!sc.isFailure()) {
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve selected " << dataTypeName() << " (" << (*keyIter) << ")" << endreq;
          DataMap data = getData(MissingETs);
          if ( FormatTool->AddToEvent(dataTypeName(), (*keyIter)+"_xAOD", &data).isFailure()){
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
  const DataMap xAODMissingETRetriever::getData(const xAOD::MissingETContainer* metCont) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in getData()" << endreq;

    DataMap m_DataMap;

    DataVect etx; etx.reserve(metCont->size());
    DataVect ety; ety.reserve(metCont->size());
    DataVect et; et.reserve(metCont->size());

    float m_mpx = 0.;
    float m_mpy = 0.;
    float m_sumet = 0.;

    xAOD::MissingETContainer::const_iterator metItr  = metCont->begin();
    xAOD::MissingETContainer::const_iterator metItrE = metCont->end();

    // current understanding is that we only need the final value
    // out of the ~9 values within each MET container ('final')

    for (; metItr != metItrE; ++metItr) {
	m_sumet = (*metItr)->sumet()/CLHEP::GeV;
    	m_mpx = (*metItr)->mpx()/CLHEP::GeV;
       	m_mpy = (*metItr)->mpy()/CLHEP::GeV;
 
	if (msgLvl(MSG::DEBUG)) {
	   msg(MSG::DEBUG) << "  Components: MissingET [GeV] mpx= "  << (*metItr)->mpx()/CLHEP::GeV
		<< ", mpy= " << (*metItr)->mpy()/CLHEP::GeV
		<< ", sumet= " << (*metItr)->sumet()/CLHEP::GeV << endreq;

	}
    } // end MissingETIterator 

    if (msgLvl(MSG::DEBUG)) {
	   msg(MSG::DEBUG) << "  FINAL: MissingET [GeV] mpx= "  << m_mpx
		<< ", mpy= " << m_mpy << ", sumet= " << m_sumet << endreq;
    }

    etx.push_back(DataType( m_mpx ));
    ety.push_back(DataType( m_mpy ));
    et.push_back(DataType( m_sumet ));

    // four-vectors
    m_DataMap["et"] = et;
    m_DataMap["etx"] = etx;
    m_DataMap["ety"] = ety;

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " retrieved with " << et.size() << " entries"<< endreq;
    }

    //All collections retrieved okay
    return m_DataMap;

  } // retrieve

  //--------------------------------------------------------------------------
  
} // JiveXML namespace

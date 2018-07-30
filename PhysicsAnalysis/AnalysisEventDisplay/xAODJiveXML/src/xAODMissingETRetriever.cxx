/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODJiveXML/xAODMissingETRetriever.h"

#include "AthenaKernel/Units.h"
using Athena::Units::GeV;

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
    AthAlgTool(type,name,parent), m_typeName("ETMis"){

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
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieveAll()" << endmsg;
    
    const DataHandle<xAOD::MissingETContainer> iterator, end;
    const xAOD::MissingETContainer* MissingETs;

    //obtain the default collection first
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve " << dataTypeName() << " (" << m_sgKey << ")" << endmsg;
    StatusCode sc = evtStore()->retrieve(MissingETs, m_sgKeyFavourite);
    if (sc.isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endmsg; 
    }else{
      DataMap data = getData(MissingETs);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKey+"_xAOD", &data).isFailure()){ //suffix can be removed later
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endmsg;
      }else{
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << dataTypeName() << " (" << m_sgKey << ") MissingET retrieved" << endmsg;
      }
    }
 
    if ( m_otherKeys.empty() ) {
      //obtain all other collections from StoreGate
      if (( evtStore()->retrieve(iterator, end)).isFailure()){
         if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "Unable to retrieve iterator for MET collection" << endmsg;
//        return false;
      }
      
      for (; iterator!=end; iterator++) {
	  if (iterator.key()!=m_sgKeyFavourite) {
             if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve all " << dataTypeName() << " (" << iterator.key() << ")" << endmsg;
             DataMap data = getData(iterator);
             if ( FormatTool->AddToEvent(dataTypeName(), iterator.key()+"_xAOD", &data).isFailure()){
	       if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << iterator.key() << " not found in SG " << endmsg;
	    }else{
	      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << iterator.key() << ") xAOD_MET retrieved" << endmsg;
	    }
	}
      }
    }else {
      //obtain all collections with the given keys
      std::vector<std::string>::const_iterator keyIter;
      for ( keyIter=m_otherKeys.begin(); keyIter!=m_otherKeys.end(); ++keyIter ){
        if ( !evtStore()->contains<xAOD::MissingETContainer>( (*keyIter) ) ){ continue; } // skip if not in SG
	StatusCode sc = evtStore()->retrieve( MissingETs, (*keyIter) );
	if (!sc.isFailure()) {
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve selected " << dataTypeName() << " (" << (*keyIter) << ")" << endmsg;
          DataMap data = getData(MissingETs);
          if ( FormatTool->AddToEvent(dataTypeName(), (*keyIter)+"_xAOD", &data).isFailure()){
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
   * Retrieve basic parameters, mainly four-vectors, for each collection.
   * Also association with clusters and tracks (ElementLink).
   */
  const DataMap xAODMissingETRetriever::getData(const xAOD::MissingETContainer* metCont) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in getData()" << endmsg;

    DataMap DataMap;

    DataVect etx; etx.reserve(metCont->size());
    DataVect ety; ety.reserve(metCont->size());
    DataVect et; et.reserve(metCont->size());

    float mpx = 0.;
    float mpy = 0.;
    float sumet = 0.;

    xAOD::MissingETContainer::const_iterator metItr  = metCont->begin();
    xAOD::MissingETContainer::const_iterator metItrE = metCont->end();

    // current understanding is that we only need the final value
    // out of the ~9 values within each MET container ('final')

    for (; metItr != metItrE; ++metItr) {
	sumet = (*metItr)->sumet()/GeV;
    	mpx = (*metItr)->mpx()/GeV;
       	mpy = (*metItr)->mpy()/GeV;
 
	if (msgLvl(MSG::DEBUG)) {
	   msg(MSG::DEBUG) << "  Components: MissingET [GeV] mpx= "  << (*metItr)->mpx()/GeV
		<< ", mpy= " << (*metItr)->mpy()/GeV
		<< ", sumet= " << (*metItr)->sumet()/GeV << endmsg;

	}
    } // end MissingETIterator 

    if (msgLvl(MSG::DEBUG)) {
	   msg(MSG::DEBUG) << "  FINAL: MissingET [GeV] mpx= "  << mpx
		<< ", mpy= " << mpy << ", sumet= " << sumet << endmsg;
    }

    etx.push_back(DataType( mpx ));
    ety.push_back(DataType( mpy ));
    et.push_back(DataType( sumet ));

    // four-vectors
    DataMap["et"] = et;
    DataMap["etx"] = etx;
    DataMap["ety"] = ety;

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " retrieved with " << et.size() << " entries"<< endmsg;
    }

    //All collections retrieved okay
    return DataMap;

  } // retrieve

  //--------------------------------------------------------------------------
  
} // JiveXML namespace

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODJiveXML/xAODTrackParticleRetriever.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODTracking/TrackParticleContainer.h" 

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   *
   * code reference for xAOD:     jpt6Feb14
   *  https://svnweb.cern.ch/trac/atlasgroups/browser/PAT/AODUpgrade/xAODReaderAlgs
   * classes:
   *  https://svnweb.cern.ch/trac/atlasoff/browser/Event/xAOD
   *
   **/
  xAODTrackParticleRetriever::xAODTrackParticleRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent), typeName("Track"){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);

    //In xAOD: GSFTrackParticles, CombinedFitMuonParticles, InDetTrackParticlesForward, ExtrapolatedMuonSpectrometerParticles
    declareProperty("StoreGateKey", m_sgKey = "InDetTrackParticles", 
        "Collection to be first in output, shown in Atlantis without switching");
    declareProperty("PriorityTrackCollection", m_sgKey = "TrackParticleCandidate", 
        "Track collections to retrieve first, shown as default in Atlantis");
    declareProperty("OtherTrackCollections"   , m_otherKeys , "Track collections to retrieve, all if empty");
  }
  
  /**
   * For each TrackParticle collections retrieve basic parameters.
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode xAODTrackParticleRetriever::retrieve(ToolHandle<IFormatTool> FormatTool) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieveAll()" << endreq;
    
    const DataHandle<xAOD::TrackParticleContainer> iterator, end;
    const xAOD::TrackParticleContainer* TrackParticles;
    
    //obtain the default collection first
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve " << dataTypeName() << " (" << m_sgKey << ")" << endreq;
    StatusCode sc = evtStore()->retrieve(TrackParticles, m_sgKey);
    if (sc.isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endreq; 
    }else{
      DataMap data = getData(TrackParticles);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKey+"_xAOD", &data).isFailure()){
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endreq;
      }else{
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << dataTypeName() << " (" << m_sgKey << ") TrackParticle retrieved" << endreq;
      }
    }

    if ( m_otherKeys.empty() ) {
      //obtain all other collections from StoreGate
      if (( evtStore()->retrieve(iterator, end)).isFailure()){
         if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "Unable to retrieve iterator for Jet collection" << endreq;
//        return false;
      }
      
      for (; iterator!=end; iterator++) {
	  if (iterator.key()!=m_sgKey) {
             if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve all " << dataTypeName() << " (" << iterator.key() << ")" << endreq;
             DataMap data = getData(iterator);
             if ( FormatTool->AddToEvent(dataTypeName(), iterator.key()+"_xAOD", &data).isFailure()){
	       if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << iterator.key() << " not found in SG " << endreq;
	    }else{
	      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << iterator.key() << ") AODJet retrieved" << endreq;
	    }
	}
      }
    }else {
      //obtain all collections with the given keys
      std::vector<std::string>::const_iterator keyIter,endIter;
      for ( keyIter=m_otherKeys.begin(); keyIter!=m_otherKeys.end(); ++keyIter ){
        if ( !evtStore()->contains<xAOD::TrackParticleContainer>( (*keyIter) ) ){ continue; } // skip if not in SG
	StatusCode sc = evtStore()->retrieve( TrackParticles, (*keyIter) );
	if (!sc.isFailure()) {
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve selected " << dataTypeName() << " (" << (*keyIter) << ")" << endreq;
          DataMap data = getData(TrackParticles);
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
  const DataMap xAODTrackParticleRetriever::getData(const xAOD::TrackParticleContainer* TrackParticleCont) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in getData()" << endreq;

    DataMap m_DataMap;

    DataVect d0; d0.reserve(TrackParticleCont->size());
    DataVect z0; z0.reserve(TrackParticleCont->size());
    DataVect pt; pt.reserve(TrackParticleCont->size());
    DataVect phi0; phi0.reserve(TrackParticleCont->size());
    DataVect cotTheta; cotTheta.reserve(TrackParticleCont->size());

    xAOD::TrackParticleContainer::const_iterator TrackParticleItr  = TrackParticleCont->begin();
    xAOD::TrackParticleContainer::const_iterator TrackParticleItrE = TrackParticleCont->end();

    int counter = 0;
    float charge = 0.;
    float myQOverP = 0.;

    for (; TrackParticleItr != TrackParticleItrE; ++TrackParticleItr) {

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "  TrackParticle #" << counter++ << " : d0 = "  << (*TrackParticleItr)->d0() << ", z0 = " 
          << (*TrackParticleItr)->z0() << ", pt[GeV] = "  << (*TrackParticleItr)->pt()/CLHEP::GeV  
          << ", phi = "  << (*TrackParticleItr)->phi() 
          << ", qOverP = "  << (*TrackParticleItr)->qOverP() 
          << ", abs(qOverP) = "  << fabs((*TrackParticleItr)->qOverP()) 
	  << endreq;
    }

      d0.push_back(DataType((*TrackParticleItr)->d0()/CLHEP::cm));
      z0.push_back(DataType((*TrackParticleItr)->z0()/CLHEP::cm));
      phi0.push_back(DataType((*TrackParticleItr)->phi()));

      // pt is always positive, get charge from qOverP
      myQOverP = (*TrackParticleItr)->qOverP() ;
      if (fabs(myQOverP) != myQOverP){  
         charge = -1.;
      }else{
         charge = +1.;
      }
      pt.push_back(DataType(( charge*(*TrackParticleItr)->pt() )/CLHEP::GeV));

     if ( (*TrackParticleItr)->theta()  == 0.) {
         cotTheta.push_back(DataType(9999.));
     } else {
         cotTheta.push_back(DataType(1./tan((*TrackParticleItr)->theta())));
     }
    } // end TrackParticleIterator 

    // four-vectors
    m_DataMap["d0"] = d0;
    m_DataMap["z0"] = z0;
    m_DataMap["pt"] = pt;
    m_DataMap["phi0"] = phi0;
    m_DataMap["cotTheta"] = cotTheta;

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " retrieved with " << d0.size() << " entries"<< endreq;
    }

    //All collections retrieved okay
    return m_DataMap;

  } // retrieve

  //--------------------------------------------------------------------------
  
} // JiveXML namespace

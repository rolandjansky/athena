/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODJiveXML/xAODTauRetriever.h"

#include "xAODTau/TauJetContainer.h" 
#include "xAODTau/TauxAODHelpers.h"

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
   *  xAOD::Tau, xAOD::Vertex, xAOD::Photon, xAOD::CaloCluster, xAOD::Tau
   *  xAOD::TrackParticle, xAOD::TauTau, xAOD::Muon
   *
   * Class references:
   *     https://svnweb.cern.ch/trac/atlasoff/browser/Event/xAOD
   **/
  xAODTauRetriever::xAODTauRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent), m_typeName("TauJet"){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);

    //In xAOD: AntiKt6TopoEMTaus, AntiKt6LCTopoTaus,  AntiKt4TopoEMTaus, AntiKt4LCTopoTaus
    m_sgKey = "TauJets"; // 
    declareProperty("StoreGateKey", m_sgKey, 
        "Collection to be first in output, shown in Atlantis without switching");
    declareProperty ( "TracksName",  m_tracksName = "InDetTrackParticles_xAOD" );
  }
  
  /**
   * For each Tau collections retrieve basic parameters.
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode xAODTauRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieveAll()" << endmsg;
    
    const DataHandle<xAOD::TauJetContainer> iterator, end;
    const xAOD::TauJetContainer* Taus;
    
    //obtain the default collection first
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve " << dataTypeName() << " (" << m_sgKey << ")" << endmsg;
    StatusCode sc = evtStore()->retrieve(Taus, m_sgKey);
    if (sc.isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endmsg; 
    }else{
      DataMap data = getData(Taus);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKey+"_xAOD", &data).isFailure()){
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endmsg;
      }else{
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << dataTypeName() << " (" << m_sgKey << ") Tau retrieved" << endmsg;
      }
    }
 
    //All collections retrieved okay
    return StatusCode::SUCCESS;
  }

  // code reference:
  //   Event/xAOD/xAODTau/trunk/xAODTau/versions/TauJet_v1.h

  /**
   * Retrieve basic parameters, mainly four-vectors, for each collection.
   * Also association with clusters and tracks (ElementLink).
   */
  const DataMap xAODTauRetriever::getData(const xAOD::TauJetContainer* tauCont) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in getData()" << endmsg;

    DataMap DataMap;

    DataVect pt; pt.reserve(tauCont->size());
    DataVect phi; phi.reserve(tauCont->size());
    DataVect eta; eta.reserve(tauCont->size());
    DataVect charge; charge.reserve(tauCont->size());

    DataVect numTracks; numTracks.reserve(tauCont->size());
    DataVect isolFrac; isolFrac.reserve(tauCont->size());
    DataVect logLhRatio; logLhRatio.reserve(tauCont->size());
    DataVect label; label.reserve(tauCont->size());
    DataVect tracks; tracks.reserve(tauCont->size());
    DataVect sgKey; sgKey.reserve(tauCont->size());
    DataVect trackLinkCount; trackLinkCount.reserve(tauCont->size());
    DataVect isTauString; isTauString.reserve(tauCont->size());

    DataVect mass; mass.reserve(tauCont->size());
    DataVect energy; energy.reserve(tauCont->size());

    xAOD::TauJetContainer::const_iterator tauItr  = tauCont->begin();
    xAOD::TauJetContainer::const_iterator tauItrE = tauCont->end();

    int counter = 0;

    for (; tauItr != tauItrE; ++tauItr) {

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "  Tau #" << counter++ << " : eta = "  << (*tauItr)->eta() << ", phi = " 
          << (*tauItr)->phi() << endmsg;
    }

      phi.push_back(DataType((*tauItr)->phi()));
      eta.push_back(DataType((*tauItr)->eta()));
      pt.push_back(DataType((*tauItr)->pt()/GeV));

      isolFrac.push_back(DataType( 1. ));
      logLhRatio.push_back(DataType( 1. ));
      label.push_back(DataType( "xAOD_tauJet_withoutQuality" ));
      charge.push_back(DataType( (*tauItr)->charge() ));
      isTauString.push_back(DataType( "xAOD_tauJet_withoutQuality" ));

      mass.push_back(DataType((*tauItr)->m()/GeV));
      energy.push_back( DataType((*tauItr)->e()/GeV ) );

      // track-vertex association code in xAOD from Nick Styles, Apr14:
      // InnerDetector/InDetRecAlgs/InDetPriVxFinder/InDetVxLinksToTrackParticles

      int trkCnt = 0;
#ifndef XAODTAU_VERSIONS_TAUTRACK_V1_H      
      const std::vector< ElementLink< xAOD::TrackParticleContainer > > tpLinks =  (*tauItr)->trackLinks();
#else
      const std::vector< ElementLink< xAOD::TrackParticleContainer > > tpLinks =  xAOD::TauHelpers::trackParticleLinks(*tauItr);
#endif
      
      //iterating over the links
      unsigned int tp_size = tpLinks.size();
      numTracks.push_back(DataType( tp_size )); // same as:  (*tauItr)->nTracks()
      trackLinkCount.push_back(DataType( tp_size ));
      if(tp_size){ // links exist
	for(unsigned int tp = 0; tp<tp_size; ++tp)
	  {     
	    ElementLink< xAOD::TrackParticleContainer >  tpl = tpLinks.at(tp);

	    //checking a container name consitency         
	    //       if(tpl.key() == m_tracksName) // doesn't work. tpl.key is a number ?

	    if (msgLvl(MSG::DEBUG)) {
	      msg(MSG::DEBUG) << "  tau #" << counter << " track association index: " << tpl.index() 
			      << ", collection : "  << tpl.key() 
			      << ", Tracks : " << tp  << " out of " << tp_size << ", own count: " << trkCnt++ << endmsg;
	    }
	    tracks.push_back(DataType( tpl.index() ));
	    sgKey.push_back( m_tracksName );
	  } //links exist
      }

    } // end TauIterator 

    // four-vectors
    DataMap["phi"] = phi;
    DataMap["eta"] = eta;
    DataMap["pt"] = pt;
    DataMap["mass"] = mass;
    DataMap["energy"] = energy;

    DataMap["numTracks"] = numTracks;
    DataMap["isolFrac"] = isolFrac;
    DataMap["logLhRatio"] = logLhRatio;
    DataMap["label"] = label;
    DataMap["charge"] = charge;
    DataMap["trackLinkCount"] = trackLinkCount;
    DataMap["isTauString"] = isTauString;

    //This is needed once we know numTracks and associations:
    //If there had been any tracks, add a tag
    if ((numTracks.size()) != 0){
      //Calculate average number of tracks per vertex
      double NTracksPerVertex = tracks.size()*1./numTracks.size();
      std::string tag = "trackIndex multiple=\"" +DataType(NTracksPerVertex).toString()+"\"";
      DataMap[tag] = tracks;
      tag = "trackKey multiple=\"" +DataType(NTracksPerVertex).toString()+"\"";
      DataMap[tag] = sgKey;
    } 

//    DataMap["energy"] = energy;
//    DataMap["mass"] = mass;

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " retrieved with " << phi.size() << " entries"<< endmsg;
    }

    //All collections retrieved okay
    return DataMap;

  } // retrieve

  //--------------------------------------------------------------------------
  
} // JiveXML namespace

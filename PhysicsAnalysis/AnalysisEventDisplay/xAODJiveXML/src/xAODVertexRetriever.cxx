/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODJiveXML/xAODVertexRetriever.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   *
   * code reference for xAOD:     jpt 6Feb14
   *  https://svnweb.cern.ch/trac/atlasgroups/browser/PAT/AODUpgrade/xAODReaderAlgs
   * classes:
   *  https://svnweb.cern.ch/trac/atlasoff/browser/Event/xAOD
   *
   **/
  xAODVertexRetriever::xAODVertexRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent), typeName("RVx"){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);

    //In xAOD: PrimaryVertices, AllPhotonsVxCandidates
    m_sgKey = "PrimaryVertices"; // 
    declareProperty("StoreGateKey", m_sgKey, 
        "Collection to be first in output, shown in Atlantis without switching");
    declareProperty ( "TracksName",  m_tracksName = "InDetTrackParticles_xAOD" );
  }
  
  /**
   * For each Vertex collections retrieve basic parameters.
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode xAODVertexRetriever::retrieve(ToolHandle<IFormatTool> FormatTool) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieveAll()" << endreq;
    
    const DataHandle<xAOD::VertexContainer> iterator, end;
    const xAOD::VertexContainer* Vertizes;
    
    //obtain the default collection first
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve " << dataTypeName() << " (" << m_sgKey << ")" << endreq;
    StatusCode sc = evtStore()->retrieve(Vertizes, m_sgKey);
    if (sc.isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endreq; 
    }else{
      DataMap data = getData(Vertizes);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKey+"_xAOD", &data).isFailure()){
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endreq;
      }else{
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << dataTypeName() << " (" << m_sgKey << ") Vertex retrieved" << endreq;
      }
    }
 
    //All collections retrieved okay
    return StatusCode::SUCCESS;
  }


  /**
   * Retrieve basic parameters, mainly four-vectors, for each collection.
   * Also association with clusters and tracks (ElementLink).
   */
  const DataMap xAODVertexRetriever::getData(const xAOD::VertexContainer* VertexCont) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in getData()" << endreq;

    DataMap m_DataMap;

    DataVect x; x.reserve(VertexCont->size());
    DataVect y; y.reserve(VertexCont->size());
    DataVect z; z.reserve(VertexCont->size());
    DataVect chi2; chi2.reserve(VertexCont->size());
    DataVect vertexType; vertexType.reserve(VertexCont->size());
    DataVect primVxCand; primVxCand.reserve(VertexCont->size());
    DataVect covMatrix; covMatrix.reserve(VertexCont->size());
    DataVect numTracks; numTracks.reserve(VertexCont->size());
    DataVect tracks; tracks.reserve(VertexCont->size());
    DataVect sgkey; sgkey.reserve(VertexCont->size());

    float m_chi2 = 0.;

    xAOD::VertexContainer::const_iterator VertexItr  = VertexCont->begin();
    xAOD::VertexContainer::const_iterator VertexItrE = VertexCont->end();

    int counter = 0;

    for (; VertexItr != VertexItrE; ++VertexItr) {

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "  Vertex #" << counter++ << " : x = "  << (*VertexItr)->x()/CLHEP::cm << ", y = " 
          << (*VertexItr)->y()/CLHEP::cm << ", z[GeV] = "  << (*VertexItr)->z()/CLHEP::cm
          << ", vertexType = "  << (*VertexItr)->vertexType()
          << ", chiSquared = "  << (*VertexItr)->chiSquared() 
          << ", numberDoF = "  << (*VertexItr)->numberDoF() << endreq;
    }
	
      x.push_back(DataType((*VertexItr)->x()/CLHEP::cm));
      y.push_back(DataType((*VertexItr)->y()/CLHEP::cm));
      z.push_back(DataType((*VertexItr)->z()/CLHEP::cm));
      vertexType.push_back(DataType((*VertexItr)->vertexType()));

    if ((*VertexItr)->vertexType() == 1 ){ 
	primVxCand.push_back( 1 );
    }else{
	primVxCand.push_back( 0 );
    }
    sgkey.push_back (m_tracksName); 

    //// placeholder ! From old V0CandidateRetriever.
    covMatrix.push_back(DataType("2 -.1 .5 -.01 0.002 .01")); 

    //degrees of freedom might be zero - beware
      if ( (*VertexItr)->numberDoF()  != 0 ){
          m_chi2 = (*VertexItr)->chiSquared()/(*VertexItr)->numberDoF() ;
      }else{
          m_chi2 = -1.;
      }
      chi2.push_back(DataType( m_chi2 ));

    // track-vertex association code in xAOD from Nick Styles, Apr14:
    // InnerDetector/InDetRecAlgs/InDetPriVxFinder/InDetVxLinksToTrackParticles

      int trkCnt = 0;
      const std::vector< ElementLink< xAOD::TrackParticleContainer > > tpLinks =  (*VertexItr)->trackParticleLinks();
   
 //iterating over the links
      unsigned int tp_size = tpLinks.size();
      numTracks.push_back(DataType( tp_size ));
      if(tp_size){ // links exist
      for(unsigned int tp = 0; tp<tp_size; ++tp)
      {     
       ElementLink< xAOD::TrackParticleContainer >  tpl = tpLinks.at(tp);

//checking a container name consitency         
//       if(tpl.key() == m_tracksName) // doesn't work. tpl.key is a number ?

      if (msgLvl(MSG::DEBUG)) {
       msg(MSG::DEBUG) << "  Vertex #" << counter << " track association index: " << tpl.index() 
         << ", collection : "  << tpl.key() 
		       << ", Tracks : " << tp  << " out of " << tp_size << ", own count: " << trkCnt++ << endreq;
       }
      tracks.push_back(DataType( tpl.index() ));
      } //links exist
    }//end of track particle collection size check

    //if (msgLvl(MSG::DEBUG)) {
    //  msg(MSG::DEBUG) << "  Vertex #" << counter << ", numTracks : " << tpLinks.size() << endreq;
    //}

    } // end VertexIterator 

    // four-vectors
    m_DataMap["x"] = x;
    m_DataMap["y"] = y;
    m_DataMap["z"] = z;
    m_DataMap["chi2"] = chi2;
    m_DataMap["vertexType"] = vertexType;
    m_DataMap["primVxCand"] = primVxCand;
    m_DataMap["covMatrix multiple=\"6\""] = covMatrix;
    m_DataMap["numTracks"] = numTracks;
//    m_DataMap["tracks multiple=\"0\""];
    m_DataMap["sgkey"] = sgkey;

    //This is needed once we know numTracks and associations:
    //If there had been any tracks, add a tag
    if ((numTracks.size()) != 0){
      //Calculate average number of tracks per vertex
      double NTracksPerVertex = tracks.size()*1./numTracks.size();
      std::string tag = "tracks multiple=\"" +DataType(NTracksPerVertex).toString()+"\"";
      m_DataMap[tag] = tracks;
    } 

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " retrieved with " << x.size() << " entries"<< endreq;
    }

    //All collections retrieved okay
    return m_DataMap;

  } // retrieve

  //--------------------------------------------------------------------------
  
} // JiveXML namespace

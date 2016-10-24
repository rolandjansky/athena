/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODJiveXML/xAODVertexRetriever.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

#include "AthenaKernel/Units.h"
using Athena::Units::cm;

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
    AthAlgTool(type,name,parent), m_typeName("RVx"){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);

    //In xAOD: PrimaryVertices, AllPhotonsVxCandidates
    declareProperty("PrimaryVertexCollection",  m_primaryVertexKey = "PrimaryVertices", 
        "Collection to be first in output, shown in Atlantis without switching");
    declareProperty("SecondaryVertexCollection", m_secondaryVertexKey = "SecVertices", "Vertices to use as secondary vertex");  
    declareProperty ( "TracksName",  m_tracksName = "InDetTrackParticles_xAOD" );
  }
  
  /**
   * For each Vertex collections retrieve basic parameters.
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode xAODVertexRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    //Get an iterator over all vertex collections,
    //return if there are none
    const DataHandle<xAOD::VertexContainer> vtxCollectionItr, vtxCollectionsEnd;
    if (evtStore()->retrieve(vtxCollectionItr,vtxCollectionsEnd).isFailure()) {
      if (msgLvl(MSG::DEBUG )) msg(MSG::DEBUG ) << "No xAODVertexContainer containers found in this event" << endmsg;
      return StatusCode::SUCCESS;
    }

    //See if we can find the requested secondary vertex collection
    const xAOD::VertexContainer* secondaryVtxCollection;
    if (evtStore()->retrieve(secondaryVtxCollection,m_secondaryVertexKey).isFailure()) {
      if (msgLvl(MSG::DEBUG )) msg(MSG::DEBUG ) << "No Secondary vertex container found at SecVertices" << endmsg;
    }else{
      if (msgLvl(MSG::DEBUG )) msg(MSG::DEBUG ) << "Secondary vertex container size: " << secondaryVtxCollection->size() << endmsg;
    }
	
    //See if we can find the requested primary vertex collection
    const xAOD::VertexContainer* primaryVtxCollection;
    if ( evtStore()->retrieve(primaryVtxCollection,m_primaryVertexKey).isFailure()) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Primary vertex container "
          << m_primaryVertexKey << " not found" << endmsg;
    }
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieve" << endmsg;

  /**
   * Retrieve basic parameters, mainly four-vectors, for each collection.
   * Also association with clusters and tracks (ElementLink).
   */
    float chi2val = 0.;
    DataVect x; 
    DataVect y; 
    DataVect z; 
    DataVect chi2; 
    DataVect vertexType; 
    DataVect primVxCand; 
    DataVect covMatrix; 
    DataVect numTracks; 
    DataVect tracks; 
    DataVect sgkey; 

    //Loop over all vertex containers
    for ( ; vtxCollectionItr != vtxCollectionsEnd; ++vtxCollectionItr ) {

      if ( ( vtxCollectionItr.key().find("HLT") != std::string::npos)){ // ignore all HLT for now
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Ignoring HLT collection " << vtxCollectionItr.key() << endmsg;
        continue;
      }
      if ( ( vtxCollectionItr.key().find("V0") != std::string::npos)){ // ignore all HLT for now                                                 
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Ignoring V0 collection " << vtxCollectionItr.key() << endmsg;
        continue;
      }

      //Get size of current container
      xAOD::VertexContainer::size_type NVtx = vtxCollectionItr->size();

      //Be a bit verbose
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Reading vertex container " << vtxCollectionItr.key()
                                              << " with " << NVtx << " entries" << endmsg;

    x.reserve(x.size()+NVtx);
    y.reserve(y.size()+NVtx);
    z.reserve(z.size()+NVtx);
    chi2.reserve(chi2.size()+NVtx);
    vertexType.reserve(vertexType.size()+NVtx);
    primVxCand.reserve(primVxCand.size()+NVtx);
    covMatrix.reserve(covMatrix.size()+NVtx);
    numTracks.reserve(numTracks.size()+NVtx);
    tracks.reserve(tracks.size()+NVtx);
    sgkey.reserve(sgkey.size()+NVtx);

    int counter = 0;

    //Loop over vertices
    xAOD::VertexContainer::const_iterator VertexItr = vtxCollectionItr->begin();
    for ( ; VertexItr != vtxCollectionItr->end(); ++VertexItr) {

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "  Vertex #" << counter++ << " : x = "  << (*VertexItr)->x()/cm << ", y = " 
          << (*VertexItr)->y()/cm << ", z[GeV] = "  << (*VertexItr)->z()/cm
          << ", vertexType = "  << (*VertexItr)->vertexType()
          << ", chiSquared = "  << (*VertexItr)->chiSquared() 
          << ", numberDoF = "  << (*VertexItr)->numberDoF() << endmsg;
    }
	
      x.push_back(DataType((*VertexItr)->x()/cm));
      y.push_back(DataType((*VertexItr)->y()/cm));
      z.push_back(DataType((*VertexItr)->z()/cm));

     if ( vtxCollectionItr.key() == m_secondaryVertexKey){ 
      vertexType.push_back( 2 );
     }else{    
      vertexType.push_back( DataType((*VertexItr)->vertexType()));
     }

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
          chi2val = (*VertexItr)->chiSquared()/(*VertexItr)->numberDoF() ;
      }else{
          chi2val = -1.;
      }
      chi2.push_back(DataType( chi2val ));

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
		       << ", Tracks : " << tp  << " out of " << tp_size << ", own count: " << trkCnt++ << endmsg;
       }
        if ( tpl.index() < 1000 ){ // sanity check, this can be huge number
          tracks.push_back(DataType( tpl.index() ));
        }else{
          tracks.push_back(DataType( 0 ));
        }
      } //links exist
    }//end of track particle collection size check

    //if (msgLvl(MSG::DEBUG)) {
    //  msg(MSG::DEBUG) << "  Vertex #" << counter << ", numTracks : " << tpLinks.size() << endmsg;
    //}
    
    } // end VertexIterator 
    } // end collectionIterator    

    // four-vectors
    DataMap DataMap;
    DataMap["x"] = x;
    DataMap["y"] = y;
    DataMap["z"] = z;
    DataMap["chi2"] = chi2;
    DataMap["vertexType"] = vertexType;
    DataMap["primVxCand"] = primVxCand;
    DataMap["covMatrix multiple=\"6\""] = covMatrix;
    DataMap["numTracks"] = numTracks;
//    DataMap["tracks multiple=\"0\""];
    DataMap["sgkey"] = sgkey;
  
    //This is needed once we know numTracks and associations:
    //If there had been any tracks, add a tag
    if ((numTracks.size()) != 0){
      //Calculate average number of tracks per vertex
      double NTracksPerVertex = tracks.size()*1./numTracks.size();
      std::string tag = "tracks multiple=\"" +DataType(NTracksPerVertex).toString()+"\"";
      DataMap[tag] = tracks;
    } 

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " retrieved with " << x.size() << " entries"<< endmsg;
    }
    //All collections retrieved okay
    return FormatTool->AddToEvent(dataTypeName(), "Vertices_xAOD", &DataMap);
  
  } // retrieve

  //--------------------------------------------------------------------------
  
} // JiveXML namespace

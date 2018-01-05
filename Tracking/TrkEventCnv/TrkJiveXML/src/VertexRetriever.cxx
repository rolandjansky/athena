/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkJiveXML/VertexRetriever.h"
#include <string>
#include <vector>
#include <map>
#include <cmath>

#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/RecVertex.h"
#include "VxVertex/VxTrackAtVertex.h"

#include "TrkTrack/TrackInfo.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkTrackLink/ITrackLink.h"

//#include "TrkVertexAnalysisUtils/V0Tools.h"

#include "Particle/TrackParticleContainer.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkParameters/TrackParameters.h"

#include "StoreGate/DataHandle.h"

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  VertexRetriever::VertexRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    typeName("RVx"){

    //Declare the interface
    declareInterface<IDataRetriever>(this);
  
    //Declare AlgTool properties
    declareProperty("PrimaryVertexCollection", m_primaryVertexKey = "VxPrimaryCandidate", "Vertices to use as primary vertex");  
    declareProperty("SecondaryVertexCollection", m_secondaryVertexKey = "SecVertices", "Vertices to use as secondary vertex");  
    declareProperty("ConversionVertexCollection", m_conversionVertexKey = "ConversionCandidate", "Vertices to use as conversion vertex");  
    declareProperty("DoWritePrimAndSecVertexOnly", m_doWritePrimAndSecVertexOnly = false,
                        "if true only write primary and secondary vertex, placeholder to be removed"); 
    declareProperty("DoWriteHLT", m_doWriteHLT = false, "whether to write HLTAutoKey objects");
    declareProperty("TrackCollection", m_trackCollection = "Tracks");
    declareProperty("Chi2OverDOFCut", m_chi2Cut = 10.); // <10 is 'tight', <100 is 'loose'
  } 


  /**
   * Manually match measured perigee d0,z0,phi0 of
   * Trk::Track and Rec::TrackParticle, some problem with direct
   * match depite identical values (maybe worth trying again).
  **/

  void manualPerigeeMatch(const Trk::Perigee* per1, 
                          const Trk::Perigee* per2, bool& matched){

    double diff1 = ( per1->parameters()[Trk::d0] -
                     per2->parameters()[Trk::d0] );
    double diff2 = ( per1->parameters()[Trk::z0] -
                     per2->parameters()[Trk::z0] );
    double diff3 = ( per1->parameters()[Trk::phi0] -
                     per2->parameters()[Trk::phi0] );
    double diff4 = ( per1->charge() -
                     per2->charge() );
    double diff5 = ( per1->pT() -
                     per2->pT() );

    if (diff1+diff2+diff3+diff4+diff5 == 0.){
       matched = true;
    }else{
       matched = false;
    }
  }
 
  /**
   * Retrieve measured perigee, automatically switch between
   * Trk::Track and Rec::TrackParticle depending on selected input collection
   * for tracks
  **/

  StatusCode VertexRetriever::fillPerigeeList() {

/// read TrackParticleCollection to compare Perigee and ordering
// from:
//   http://atlas-sw.cern.ch/cgi-bin/viewcvs-all.cgi/users/jsvirzi/PhysicsAnalysis/Atlas_ESD_Analysis/src/Atlas_ESD_Analysis.cxx?root=atlas&view=co
///// example:
//  InnerDetector/InDetMonitoring/InDetAlignmentMonitoring/TrackSplitterTool.cxx
/////
//    const Trk::Perigee* trackPerigee = inputTrack->perigeeParameters();
//    const Trk::perigee* perigee = dynamic_cast<const Trk::perigee*>( trackPerigee );
//    double m_d0 = perigee->parameters()[Trk::d0];

    const Rec::TrackParticleContainer* tracks = NULL ;
    const TrackCollection* trktracks = NULL ;
 
    size_t found;    
    std::string searchStr = "TrackParticle";
    found=m_trackCollection.find(searchStr);

    perigeeVector.clear(); // need to clear, otherwise accumulates over events
    if (found!=std::string::npos){ // User selected a Rec::TrackParticle Collection
      if (evtStore()->retrieve(tracks, m_trackCollection).isFailure()){
        if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Unable to retrieve track collection"
                                                  << m_trackCollection << " for association "<< endmsg;
      } else {
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved " << 
	     m_trackCollection << endmsg;

         Rec::TrackParticleContainer::const_iterator track;
         for(track=tracks->begin();track!=tracks->end();++track) {
	    const Trk::Perigee *perigee = (*track)->perigee();
//          if(perigee == 0) continue; // not skip ! need to keep order for index !
            perigeeVector.push_back( perigee ); // this perigee match works !
         }
      }
    }else{ // it's a Trk::Tracks collection
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " User selected a Trk::Track collection ! " << endmsg; 

      if (evtStore()->retrieve(trktracks, m_trackCollection).isFailure()){
        if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Unable to retrieve track collection"
                                                  << m_trackCollection << " for association "<< endmsg;
      } else {
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved " << 
	    m_trackCollection << endmsg;
        TrackCollection::const_iterator track;
        for(track=trktracks->begin();track!=trktracks->end();++track) {
          const Trk::Perigee* trackPerigee = (*track)->perigeeParameters();
          //const Trk::Perigee *perigee = dynamic_cast<const Trk::Perigee*>( trackPerigee );
//          if(perigee == 0) continue; // not skip ! need to keep order for index !
//          perigeeVector.push_back( perigee ); 
          perigeeVector.push_back( trackPerigee ); 
        }
      }
    }
    return StatusCode::SUCCESS;
  }

  /**
   * For all vertex collections retrieve all data
   * - loop over vertices in all collections
   * - for each vertex get basic coordinates and fit parameters
   * - find index in track collection for each track associated with this vertex
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode VertexRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool){

    //Get an iterator over all vertex collections,
    //return if there are none
    const DataHandle<VxContainer> vtxCollectionItr, vtxCollectionsEnd;
    if (evtStore()->retrieve(vtxCollectionItr,vtxCollectionsEnd).isFailure()) {
      if (msgLvl(MSG::DEBUG )) msg(MSG::DEBUG ) << "No VxContainer containers found in this event" << endmsg;
      return StatusCode::SUCCESS;
    }

    //See if we can find the requested secondary vertex collection
    const VxContainer* secondaryVtxCollection;
    if (evtStore()->retrieve(secondaryVtxCollection,m_secondaryVertexKey).isFailure()) {
      if (msgLvl(MSG::DEBUG )) msg(MSG::DEBUG ) << "No Secondary vertex container found at SecVertices" << endmsg;
    }else{
      if (msgLvl(MSG::DEBUG )) msg(MSG::DEBUG ) << "Secondary vertex container size: " << secondaryVtxCollection->size() << endmsg;
    } 

    //See if we can find the requested primary vertex collection
    const VxContainer* primaryVtxCollection;
    if ( evtStore()->retrieve(primaryVtxCollection,m_primaryVertexKey).isFailure()) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Primary vertex container "
          << m_primaryVertexKey << " not found" << endmsg;
    }

    //Declare all the data vectors we want to retrieve
    DataVect x;
    DataVect y;
    DataVect z;
    DataVect primVxCand;
    DataVect chi2;
    DataVect sgkey;
    DataVect covMatrix;
    DataVect numTracks;
    DataVect tracks;
    DataVect vertexType; 

    float m_chi2 = 0.;

    //Loop over all vertex containers
    for ( ; vtxCollectionItr != vtxCollectionsEnd; ++vtxCollectionItr ) {

      //Check whether we should ignore HLTAutoKey collections
      if ( (!m_doWriteHLT) && ( vtxCollectionItr.key().find("HLT") != std::string::npos)){
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Ignoring HLT collection " << vtxCollectionItr.key() << endmsg;
        continue;
      }

      //Get size of current container
      VxContainer::size_type NVtx = vtxCollectionItr->size();

      //Be a bit verbose
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Reading vertex container " << vtxCollectionItr.key() 
                                              << " with " << NVtx << " entries" << endmsg;
      
      //Declare all the data vectors we want to retrieve and reserve space
      x.reserve(x.size()+NVtx);
      y.reserve(y.size()+NVtx);
      z.reserve(z.size()+NVtx);
      primVxCand.reserve(primVxCand.size()+NVtx);
      chi2.reserve(chi2.size()+NVtx);
      sgkey.reserve(sgkey.size()+NVtx);
      covMatrix.reserve(covMatrix.size()+NVtx);
      numTracks.reserve(numTracks.size()+NVtx);
      vertexType.reserve(numTracks.size()+NVtx);

      StatusCode sc = fillPerigeeList();
      if (!sc.isFailure()) {
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Perigee list filled with " << perigeeVector.size()
                          << " entries " << endmsg;
      }    

      //Loop over vertices
      VxContainer::const_iterator vertexItr = vtxCollectionItr->begin();
      for ( ; vertexItr != vtxCollectionItr->end(); ++vertexItr) {
///////// read all vertex collections again, but make chi2 cut on ConversionCandidate
//       if ( m_doWritePrimAndSecVertexOnly && vtxCollectionItr.key() == m_conversionVertexKey ){
//         if (msgLvl(MSG::DEBUG)){ msg(MSG::DEBUG) << " DoWritePrimAndSecVertexOnly switch true - skipping " 
//	     << vtxCollectionItr.key() << endmsg; }
//          continue; } 
/////////
        /**
         * Step 1:
         * Fill in all the basic parameters that should be always there
         */
        //Get fit quality object
        Trk::FitQuality fitQuality = (*vertexItr)->recVertex().fitQuality();

        //degrees of freedom might be zero - beware
        if ( fitQuality.doubleNumberDoF() != 0 ){
          m_chi2 = fitQuality.chiSquared()/fitQuality.doubleNumberDoF();
        }else{
          m_chi2 = -1.;
        }
        //Cut: on Chi^2 over NumberOfDegreesOfFreedom only for ConversionCandidate 
        if ( m_chi2 > m_chi2Cut && ( vtxCollectionItr.key() == m_conversionVertexKey  )) continue;

        float m_x =  (*vertexItr)->recVertex().position().x()/10.; //Atlantis units are cm
        float m_y =  (*vertexItr)->recVertex().position().y()/10.;
        float m_z =  (*vertexItr)->recVertex().position().z()/10.;
        float R = sqrt( pow(m_x,2) + pow(m_y,2) ); // distance from beamline

        if (msgLvl(MSG::DEBUG)){ msg(MSG::DEBUG) << " Collection: " << vtxCollectionItr.key() 
            << ", m_chi2: " << m_chi2 << " - chi2: " << fitQuality.chiSquared() 
//            << " ," << (*vertexItr)->recVertex().position().x()/10. << " ," << (*vertexItr)->recVertex().position().x()*CLHEP::cm 
            << ", R: " << R << endmsg; }

        chi2.push_back(DataType( m_chi2 ));
        x.push_back(DataType( m_x ));
        y.push_back(DataType( m_y ));
        z.push_back(DataType( m_z ));

        // from: Tracking/TrkEvent/TrkEventPrimitives/VertexType.h
	const Trk::VertexType vtx_type = (*vertexItr)->vertexType();
        vertexType.push_back(DataType( vtx_type )); 
	if (msgLvl(MSG::DEBUG)){ msg(MSG::DEBUG) << " collection " << vtxCollectionItr.key() << ": VertexType: " << vtx_type << endmsg; }

        //Store primary vertex candidate flag
        if ( vtxCollectionItr == primaryVtxCollection ){
	  if ( Trk::PriVtx == vtx_type ){ primVxCand.push_back(DataType( 1 )); // type 1 'real' primary vertex
          }else{ primVxCand.push_back(DataType( 0 )); } // hack ! 'type 3 pileup' Should properly use 'vertexType'
	}else if ( vtxCollectionItr == secondaryVtxCollection ){ 
	   primVxCand.push_back(DataType( 2 )); // normally those are 'type 9 Kshort'
        }else{
           primVxCand.push_back(DataType( 0 ));
        }    

// !!! This doesn't work after Migration. 'errorposition' not known anymore ? jpt Dec'13 !!!
        ////Fill the half-sided (symmetric) covariance matrix
        //double scale = (CLHEP::cm*CLHEP::cm)/(CLHEP::mm*CLHEP::mm); // in order to convert the covMatrix elements from (mm)^2 to (cm)^2 divide them by 100
        //double precision = 10000; //// otherwise you sometimes loose precision by putting only 6 decimal points into the xml
        //for (int i = 0;i<3; i++)
          //for (int j = 0; j<i+1; j++ )
            //covMatrix.push_back (DataType((*vertexItr)->recVertex().errorPosition().covariance()[i][j]/scale*precision ));
//-----------
        //// placeholder ! From old V0CandidateRetriever.
        //// Purely experimentally found !
        //// these values of covMatrix make Atlantis
        //// not to reject the 'vertex'  
	covMatrix.push_back(DataType("2 -.1 .5 -.01 0.002 .01")); 

        /**
         * Step 2:
         * Get track associations
         */

//---- association from:
//---- from http://alxr.usatlas.bnl.gov/lxr/source/atlas/Reconstruction/tauRec/src/PhotonConversionPID.cxx

    const std::vector<Trk::VxTrackAtVertex*>* trklist = (*vertexItr)->vxTrackAtVertex();

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Tracks at vertex: " << trklist->size() << endmsg;

    numTracks.push_back(DataType( trklist->size() ) );
    sgkey.push_back(DataType( m_trackCollection )); // sgkey in current scheme is _not_ a multiple !

    tracks.push_back(DataType( -1 ));
      }
    } // loop over vertex containers

    //Finally add all retrieved data to the data map
    DataMap dataMap;
    dataMap["x"] = x;
    dataMap["y"] = y;
    dataMap["z"] = z;
    dataMap["primVxCand"] = primVxCand;
    dataMap["chi2"] = chi2;
    dataMap["covMatrix multiple=\"6\""] = covMatrix;
    dataMap["numTracks"] = numTracks;
    dataMap["sgkey"] = sgkey;
    dataMap["vertexType"] = vertexType;
    
    //If there had been any tracks, add a tag
    if ((numTracks.size()) != 0){
      //Calculate average number of tracks per vertex
      double NTracksPerVertex = tracks.size()*1./numTracks.size();
      std::string tag = "tracks multiple=\"" +DataType(NTracksPerVertex).toString()+"\"";
      dataMap[tag] = tracks;
///// sgkey in current scheme is _not_ a multiple !
//      std::string tag2 = "sgkey multiple=\"" +DataType(NTracksPerVertex).toString()+"\"";
//      dataMap[tag2] = sgkey;
    } 

    ////forward data to formating tool and return
    ////   putting sgkey here can cause problem ! Causing twisted V's 
    ////   reverting to no-sgKey until Atlantis code modified. jpt 2Jul10
    //return FormatTool->AddToEvent(dataTypeName(), "PrimSecConvRecVx", &dataMap);
    return FormatTool->AddToEvent(dataTypeName(), "", &dataMap);

  } // end retrieve

} // namepspace JiveXML

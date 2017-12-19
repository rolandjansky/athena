/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisJiveXML/TrackParticleRetriever.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"

#include "Particle/TrackParticleContainer.h"

#include "TrkEventPrimitives/JacobianThetaPToCotThetaPt.h"

#include "AthLinks/ElementLink.h"
#include "AthContainers/DataVector.h"

namespace JiveXML {
  
  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  TrackParticleRetriever::TrackParticleRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_typeName("Track"){

      //Declare the interface
      declareInterface<IDataRetriever>(this);

      //Properties
      declareProperty("PriorityTrackCollection", m_PriorityTrackCollection = "TrackParticleCandidate", 
            "Track collections to retrieve first, shown as default in Atlantis");
      declareProperty("OtherTrackCollections"   , m_OtherTrackCollections , "Track collections to retrieve, all if empty");
      declareProperty("DoWriteHLT"              , m_doWriteHLT = false, "Wether to write HLTAutoKey objects");
  }


  namespace TrackParticleRetrieverHelpers {

     /**
      * Get polyline hits if available. Modified from TrackRetriever
      */
// vectors with measurement, maximum 3 for TrackParticle (perigee, calo entry, muon entry)
// info from Ed Moyse  Oct09
     void getPolylineFromTrackParticle(  const Rec::TrackParticle* track,
          DataVect& polylineX, DataVect& polylineY, DataVect& polylineZ, DataVect& numPolyline){

     DataVect myPositionX;
     DataVect myPositionY;
     DataVect myPositionZ;

     int numPoly = 0 ;

// Migration Dec'13: Not yet done here ! Need equivalent code
// to access spacepoints. jpt 13Dec13

     unsigned int i = 0;
     const std::vector<const Trk::TrackParameters*>&  trackpars =track->trackParameters();
     
     if (!trackpars.empty() && trackpars.size() == 3 ) { //polylines with less than 3 entries not useful 

       bool needresorting = trackpars.at(0)!=track->perigee();//Needed since TrackParticles are (at the moment)
                                                              //created with the first parameter put last
       for ( i=0; i<trackpars.size(); i++){
	 const Amg::Vector3D& pos = trackpars.at(i)->position();
             myPositionX.push_back(DataType(pos.x()/10.));
             myPositionY.push_back(DataType(pos.y()/10.));
             myPositionZ.push_back(DataType(pos.z()/10.));
	   //std::cout << " Polyline # " << i << " x: " << pos.x() << std::endl;
	   ++numPoly;
       } 
       if (needresorting){ // swapping vector around. Is this too naive ? 
	 polylineX.push_back( myPositionX.at( trackpars.size()-1 ) );
         polylineY.push_back( myPositionY.at( trackpars.size()-1 ) );
	 polylineZ.push_back( myPositionZ.at( trackpars.size()-1 ) );
         polylineX.push_back( myPositionX.at( trackpars.size()-3 ) );
         polylineY.push_back( myPositionY.at( trackpars.size()-3 ) );
         polylineZ.push_back( myPositionZ.at( trackpars.size()-3 ) );
         polylineX.push_back( myPositionX.at( trackpars.size()-2 ) );
         polylineY.push_back( myPositionY.at( trackpars.size()-2 ) );
         polylineZ.push_back( myPositionZ.at( trackpars.size()-2 ) );
       }
     }

     // std::cout << " numPolyline" << numPoly << std::endl;

     //Store counter as well
     numPolyline.push_back(DataType(numPoly)); 
  }
} // end TrackRetrieverHelpers namespace

  /**
   * This is largely a chopped copy of TrkJiveXML/TrackRetriever.
   * For each track collection retrieve all data
   * - loop over tracks in all collections
   * - for each track get basic parameters
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode TrackParticleRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
    //be verbose
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieving AOD TrackParticle" << endmsg; 
  
    //Generate a list of requested track collections
    typedef std::pair< Rec::TrackParticleContainer , std::string > tracksNamePair;
    std::vector< tracksNamePair > requestedTrackColls;
     
    //First try to get hold of the priority track collection
    const Rec::TrackParticleContainer* tracks = NULL ;
    if (evtStore()->retrieve(tracks, m_PriorityTrackCollection).isFailure()){
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Unable to retrieve requested priority track collection "
                                                  << m_PriorityTrackCollection << endmsg;
    } else {
      //Add this to the list of requested collections
      requestedTrackColls.push_back(tracksNamePair(*tracks,m_PriorityTrackCollection));
    }

    //If we have been given an explicit list, try to retrieve these in the order
    //they were given

    std::vector<std::string>::iterator CollNameItr = m_OtherTrackCollections.begin();
    for ( ; CollNameItr != m_OtherTrackCollections.end(); ++CollNameItr){
      const Rec::TrackParticleContainer* tracks = NULL ;
      if (evtStore()->retrieve(tracks, (*CollNameItr)).isFailure()){
        if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Unable to retrieve requested track collection " << (*CollNameItr) << endmsg;
        continue ;
      }

// This fails for me, compilation error. Leave it out for now.
// But this means some collections are written out twice ! jpt 24Jul09 
// This is copy/paste from TrackRetriever, works with TrackCollection,
// but _not_ with Rec::TrackParticleContainer in completely identical code. 
      //Check if this collection is not already in the list
//      if (std::find(requestedTrackColls.begin(), requestedTrackColls.end(),
//         tracksNamePair(*tracks,*CollNameItr) ) == requestedTrackColls.end()){
      //Add this to the list of requested collections
        requestedTrackColls.push_back( tracksNamePair(*tracks,(*CollNameItr)) );
//      }
    }      

    //If no collections had been requested explicitly, loop over all of them
    if (m_OtherTrackCollections.empty()) {
      
      //Get an iterator over all other track collections
      const DataHandle<Rec::TrackParticleContainer> trackCollIter, trackCollEnd;
      if ((evtStore()->retrieve(trackCollIter, trackCollEnd)).isFailure()){
        if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Unable to retrieve track collection iterator" << endmsg;
        return StatusCode::SUCCESS; 
      }

      //Next loop over all collections
      for (; trackCollIter!=trackCollEnd; trackCollIter++) {

	//hack to avoid double, as 'find' (further below) fails.
	if ( trackCollIter.key() == m_PriorityTrackCollection ){ continue; }

        //Check if this is an HLT-AutoKey collection
        if ((trackCollIter.key().find("HLT",0) != std::string::npos) && (!m_doWriteHLT)){
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Ignoring HLT-AutoKey collection " << trackCollIter.key() << endmsg;
          continue ;
        }

	// Veto AtlfastTrackParticles as they have different parameter access method. 
	// Retrieving them will lead to runtime crash currently.  jpt 04Aug07
        if ( (trackCollIter.key() =="AtlfastTrackParticles")) {
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Always ignoring collection " << trackCollIter.key() << endmsg;
          continue ;
        }

        //Next try to retrieve the actual collection
        if (evtStore()->retrieve(tracks,trackCollIter.key()).isFailure()){
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unable to retrieve collection " << trackCollIter.key() << endmsg;
          continue ;
        }

// this fails for me, leave it out. jpt 24Jul09
        //Check if this collection is not already in the list
//        if (std::find(requestedTrackColls.begin(), requestedTrackColls.end(),tracksNamePair(*tracks,trackCollIter.key())) != requestedTrackColls.end())
//          continue ;
        //Add this to the list of requested collections 
        requestedTrackColls.push_back(tracksNamePair(*tracks,trackCollIter.key()));
      } //loop over all track collections
    }

    /**
     * Second step:
     * Now loop over all collections and retrieve the actual values
     **/
    
    //Loop over the collection list we have assembled above
    std::vector<tracksNamePair>::iterator tracksNamePairItr = requestedTrackColls.begin();
    for ( ; tracksNamePairItr != requestedTrackColls.end(); ++tracksNamePairItr){
 
      //save some typing by getting a handle on the collection pointer
      const Rec::TrackParticleContainer* tpc =&((*tracksNamePairItr).first);
      std::string collectionName = (*tracksNamePairItr).second;
      
      //Some sanity checks
      if ( tpc->size() == 0){
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Ignoring empty track collection " << collectionName << endmsg;
      } else {
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieving data for track collection " << collectionName << endmsg;
      }
 
      // Make a list of track-wise entries and reserve enough space
      DataVect id; id.reserve(tpc->size());
      DataVect chi2; chi2.reserve(tpc->size());
      DataVect numDoF; numDoF.reserve(tpc->size());
      DataVect trackAuthor; trackAuthor.reserve(tpc->size());
      DataVect label; label.reserve(tpc->size());

      //Store wether this collection has perigee parameters
      DataVect pt; pt.reserve(tpc->size());
      DataVect d0; d0.reserve(tpc->size());
      DataVect z0; z0.reserve(tpc->size());
      DataVect phi0; phi0.reserve(tpc->size());
      DataVect cotTheta; cotTheta.reserve(tpc->size()); 
      DataVect covMatrix; covMatrix.reserve(tpc->size() * 15 );

      DataVect numPolyline; numPolyline.reserve(tpc->size());
      DataVect polylineX;
      DataVect polylineY;
      DataVect polylineZ;

      std::string labelStr = "unknownHits";
    
      // Now loop over all tracks in the collection
      Rec::TrackParticleContainer::const_iterator tpcItr;
      for (tpcItr=tpc->begin(); tpcItr!=tpc->end(); ++tpcItr) {
        
        /**
         * General track fit info
         */
        id.push_back(DataType(id.size())); //<! simple counter starting from 0
        chi2.push_back(DataType((*tpcItr)->fitQuality()->chiSquared()));
        numDoF.push_back(DataType((*tpcItr)->fitQuality()->numberDoF()));
        trackAuthor.push_back( (*tpcItr)->info().trackFitter() );

        const Trk::TrackSummary* tSum = (*tpcItr)->trackSummary();
        if(tSum){
          int nPixelHits = tSum->get(Trk::numberOfPixelHits);
          int nSCTHits   = tSum->get(Trk::numberOfSCTHits);
          int nBLayerHits = tSum->get(Trk::numberOfInnermostPixelLayerHits);
          int nTRTHits   = tSum->get(Trk::numberOfTRTHits);
          labelStr = "_PixelHits"+DataType( nPixelHits ).toString() + "_SCTHits"+DataType( nSCTHits ).toString() + 
                     "_BLayerHits"+DataType( nBLayerHits ).toString() + "_TRTHits"+DataType( nTRTHits ).toString() ;
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Author: " << (*tpcItr)->info().trackFitter() << " Pixel hits: " << nPixelHits
						  << ", SCT hits: " << nSCTHits << " BLayer hits: " << nBLayerHits
						  << ", TRT hits: " << nTRTHits << ", pT[GeV]= " << (*tpcItr)->perigee()->pT()/1000. << endmsg;
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Label: " << labelStr << endmsg;
        }

        if (((*tpcItr)->perigee()->parameters())[Trk::qOverP]==0) {
           pt.push_back(DataType(9999.));
	} else {
	   pt.push_back(DataType((*tpcItr)->perigee()->charge() * (*tpcItr)->perigee()->pT()/1000.));
	}

	d0.push_back(DataType(((*tpcItr)->perigee()->parameters())[Trk::d0]/CLHEP::cm));
	z0.push_back(DataType((*tpcItr)->perigee()->parameters()[Trk::z0]/CLHEP::cm));
	phi0.push_back(DataType((*tpcItr)->perigee()->parameters()[Trk::phi0]));

	if ((*tpcItr)->perigee()->parameters()[Trk::theta] == 0.) {
	  cotTheta.push_back(DataType(9999.));
	} else {
	  cotTheta.push_back(DataType(1./tan((*tpcItr)->perigee()->parameters()[Trk::theta])));
	}
	
	// CLHEP->Eigen migration. jpt Dec'13
        // https://twiki.cern.ch/twiki/bin/viewauth/Atlas/MigrationCLHEPtoEigen
	// https://twiki.cern.ch/twiki/bin/viewauth/Atlas/MigrationToUpdatedEDM#Changes_to_TrkParameters

        /// get transformed covariance matrix
        AmgSymMatrix(5) covVert;
        //covVert.clear();
	const Trk::Perigee* per = 
	         dynamic_cast<const Trk::Perigee*>((*tpcItr)->perigee());
	const AmgSymMatrix(5)* covariance = per ? per->covariance() : NULL;
	if (per && covariance) {
	     // do trafo to old format
	     double measuredTheta = (*tpcItr)->perigee()->parameters()[Trk::theta];
             double measuredQoverp = (*tpcItr)->perigee()->parameters()[Trk::qOverP];
	     const Trk::JacobianThetaPToCotThetaPt theJac( measuredTheta, measuredQoverp );
	     covVert = covariance->similarity(theJac);
        }else{	
	  for ( int ii=0; ii<20; ii++){ // placeholder. Do this nicer.
             covVert(ii) = 0.;
	  }
	}
	//Scale covariance matrix values to get good resolution with fixed
	//precision in JiveXML data

	const long scale = 10000;
// Migration: Now only has diagonal elements from covariance matrix ?
	covMatrix.push_back(DataType(covVert(0)*scale/100.)); // 5 elements 
	covMatrix.push_back(DataType(covVert(1)*scale/100.));
	covMatrix.push_back(DataType(covVert(2)*scale/100.));
	covMatrix.push_back(DataType(covVert(3)*scale/100.));
	covMatrix.push_back(DataType(covVert(4)*scale/100.));

// Used to be 15 elements before migration, so need to put 10 placeholders
	for ( int i=0; i<10; i++){
	   covMatrix.push_back(DataType( 0. ));
	}
/*
	covMatrix.push_back(DataType(covVert[0][0]*scale/100.)); // 15 elements (diagonal +1half)
        covMatrix.push_back(DataType(covVert[1][0]*scale/100.));
        covMatrix.push_back(DataType(covVert[1][1]*scale/100.));
        covMatrix.push_back(DataType(covVert[2][0]*scale/10.));
        covMatrix.push_back(DataType(covVert[2][1]*scale/10.));
        covMatrix.push_back(DataType(covVert[2][2]*scale/1.));
        covMatrix.push_back(DataType(covVert[3][0]*scale/10.));
        covMatrix.push_back(DataType(covVert[3][1]*scale/10.));
        covMatrix.push_back(DataType(covVert[3][2]*scale/1.));
        covMatrix.push_back(DataType(covVert[3][3]*scale/1.));
        covMatrix.push_back(DataType(covVert[4][0]*scale/0.01));
        covMatrix.push_back(DataType(covVert[4][1]*scale/0.01));
        covMatrix.push_back(DataType(covVert[4][2]*scale/0.001));
        covMatrix.push_back(DataType(covVert[4][3]*scale/0.001));
        covMatrix.push_back(DataType(covVert[4][4]*scale/0.000001));
*/
      TrackParticleRetrieverHelpers::getPolylineFromTrackParticle( (*tpcItr),polylineX,polylineY,polylineZ,numPolyline);

      } // end loop over tracks in collection

      //Now fill everything in a datamap
      DataMap DataMap;
      // Start with mandatory entries
      DataMap["id"] = id;
      DataMap["chi2"] = chi2;
      DataMap["numDoF"] = numDoF;
      DataMap["trackAuthor"] = trackAuthor;
      DataMap["numPolyline"] = numPolyline;
      // DataMap["label"] = labelStr; // need to add into event.dtd first
     
      // if perigee parameters are not available, leave the corresponding subtags empty.
      // This way atlantis knows that such tracks can only be displayed as polylines.
      if (pt.size() > 0){
        DataMap["pt"] = pt;
        DataMap["d0"] = d0;
        DataMap["z0"] = z0;
        DataMap["phi0"] = phi0;
        DataMap["cotTheta"] = cotTheta;
        DataMap["covMatrix multiple=\"15\""] = covMatrix;
      }

      // vectors with measurement, maximum 3 for TrackParticle (perigee, calo entry, muon entry)
      if ( polylineX.size() > 0){
        std::string numPolyPerTrack = DataType(polylineX.size()/((double)id.size())).toString();
        DataMap["polylineX multiple=\"" + numPolyPerTrack + "\""] = polylineX;
        DataMap["polylineY multiple=\"" + numPolyPerTrack + "\""] = polylineY;
        DataMap["polylineZ multiple=\"" + numPolyPerTrack + "\""] = polylineZ;
      }

      //forward data to formating tool
      if ( FormatTool->AddToEvent(dataTypeName(), collectionName, &DataMap).isFailure())
            return StatusCode::RECOVERABLE;
      
      //Be verbose
       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " collection " << collectionName << " retrieved with " << id.size() << " entries"<< endmsg; 
    } //loop over track collections

    //All collections retrieved okay
    return StatusCode::SUCCESS;

  } //retrieve
} //namespace JiveXML

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./TrackRoIAssocTool.h"


TrackRoIAssocTool::TrackRoIAssocTool(const std::string& type, const std::string& name,  const IInterface* parent) : 
  AthAlgTool(type, name, parent),
  m_dR_size(0.4) {  
 declareInterface<TrackRoIAssocTool>( this );
 declareProperty("dRSize", m_dR_size,"Size of dR cone");     
}

StatusCode TrackRoIAssocTool::initialize() {
  return StatusCode::SUCCESS;
}



StatusCode TrackRoIAssocTool::associate(SG::ReadHandle< TrackCollection > tracks , 
                                        SG::ReadHandle< TrigRoiDescriptorCollection> RoIs, 
                                        SG::WriteHandle< xAOD::TrigCompositeContainer > &  outputTCs) {

  std::string trk_key_string  = tracks.key();
  int trk_key  = ElementLink<TrackCollection>(tracks.name(),0).key();
  int trk_clid = tracks.clid();//ClassID_traits< TrackCollection >::ID();
  
  ATH_MSG_DEBUG("tracks: key="<<trk_key<<" "<<trk_key_string<<" clid="<< trk_clid);
  ATH_MSG_DEBUG("n.tracks="<<tracks->size()<<" n.rois="<<RoIs->size());
  
  size_t nRoI = 0;
  for ( auto roi : *RoIs.cptr() ) {
    xAOD::TrigComposite * decision  = new xAOD::TrigComposite();          
    outputTCs->push_back(decision);   
    //set roi link
    //    ATH_MSG_DEBUG("Set RoI link into TC");
    decision->setObjectLink("initialRoI", ElementLink<TrigRoiDescriptorCollection>(RoIs.name(), nRoI));


    std::vector<int> trackLinks;
    unsigned itrack=0;
    //const Trk::Perigee* aMeasPer = 0;
    for (auto track: *tracks.cptr()){
      //ATH_MSG_DEBUG("RoI "<<nRoI<<" -> track "<<(*track));

      auto tSOS = track->trackStateOnSurfaces()->begin();
      if (tSOS!=track->trackStateOnSurfaces()->end()){//get the first only
        const Trk::TrackParameters* trackPars = (*tSOS)->trackParameters();
        if (trackPars){
          float trackPerigeeTheta = trackPars->parameters()[Trk::theta];
          float trackPerigeeEta = -log(tan(0.5*trackPerigeeTheta));
          double deta = trackPerigeeEta - roi->eta(); 
          double dphi = trackPars->parameters()[Trk::phi] - roi->phi();
          double dzed = trackPars->parameters()[Trk::z0] - roi->zed();
          if ( dphi<-M_PI ) dphi +=2*M_PI;
          if ( dphi>M_PI )  dphi -=2*M_PI;
          float dR = std::sqrt(dphi*dphi + deta*deta);

          if (dR<m_dR_size) {
            ATH_MSG_DEBUG("RoI "<<nRoI<<" -> track "<<itrack<<" at surface has dR="<<dR);
            //      ATH_MSG_DEBUG("RoI "<<nRoI<<" -> track at surface "<<itrack<<" deta="<<deta<<" dphi="<<dphi<<" dzed="<<dzed<<" dR="<<dR);
            //track is in RoI
            trackLinks.push_back(itrack);
          }
        }
      }

      // aMeasPer = track->perigeeParameters();
      // if( aMeasPer != 0 ){
      //  float trackPerigeeTheta = aMeasPer->parameters()[Trk::theta];
      //  float trackPerigeeEta = -log(tan(0.5*trackPerigeeTheta));
      //  double deta = trackPerigeeEta - roi->eta(); //track->trackParameters()->eta()
      //  double dphi = aMeasPer->parameters()[Trk::phi] - roi->phi();
      //  double dzed = aMeasPer->parameters()[Trk::z0] - roi->zed();
      //  if ( dphi<-M_PI ) dphi +=2*M_PI;
      //  if ( dphi>M_PI )  dphi -=2*M_PI;
      //  float dR = std::sqrt(dphi*dphi + deta*deta);
      //  ATH_MSG_DEBUG("RoI "<<nRoI<<" -> track "<<itrack<<" eta="<<deta<<" dphi="<<dphi<<" dzed="<<dzed);
      //  if (dR<0.2) {
      //    //track is in RoI
      //    trackLinks.push_back(itrack);
      //  }
      // }
      itrack++;
    }

    ATH_MSG_DEBUG("Found "<<trackLinks.size()<<" tracks in this roi "<<nRoI);
    if (trackLinks.size() > 0){
      decision->setDetail( "TrackKey", trk_key ) ;
      decision->setDetail( "TrackCLID", trk_clid ) ;
      decision->setDetail( "TrackIndices", trackLinks ) ;
    }
    nRoI++;
  }

  ATH_MSG_DEBUG("TC output collection has "<<outputTCs->size()<<" elements");


  return StatusCode::SUCCESS;
}



StatusCode TrackRoIAssocTool::getTrackLinks(const xAOD::TrigComposite* inputTC,  ElementLinkVector<TrackCollection>& tracks){
  int trk_key = 0;
  int trk_clid = 0;
  bool hasTracks = inputTC->getDetail("TrackKey",trk_key) &&  inputTC->getDetail("TrackCLID",trk_clid);
  if (hasTracks){
    std::vector<int> indices;
    if (! ( inputTC->getDetail( "TrackIndices", indices))){
      ATH_MSG_ERROR("TC has no tracks, but are expected");
      return StatusCode::FAILURE;
    }
    // fill vectors of links to tracks
    for (unsigned int index=0;index<indices.size();index++){
      tracks.push_back(ElementLink<TrackCollection>(trk_key, index));
    }
  }

  ATH_MSG_DEBUG("TC has "<<tracks.size()<<" tracks");

  return StatusCode::SUCCESS;
}

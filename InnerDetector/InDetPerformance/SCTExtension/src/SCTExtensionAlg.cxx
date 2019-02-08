/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTExtension/SCTExtensionAlg.h"
#include "TrkTrack/Track.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"




/////////////////////////////////////////////////////////////////////////////
SCTExtensionAlg::SCTExtensionAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
  // Properties go here
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode SCTExtensionAlg::initialize(){

  ATH_MSG_INFO ("initialize()");

  ATH_CHECK(m_pixelTracklets.initialize());
  ATH_CHECK(m_tracks.initialize());

  return StatusCode::SUCCESS;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode SCTExtensionAlg::execute(const EventContext& ctx) const {

  SG::ReadHandle<xAOD::TrackParticleContainer> tracklets(m_pixelTracklets, ctx);
  if (not tracklets.isValid())
    {
      ATH_MSG_FATAL("Could not retrieve " << m_pixelTracklets.key());
      return StatusCode::FAILURE;
    }

  SG::ReadHandle<xAOD::TrackParticleContainer> trks(m_tracks, ctx);
  if (not trks.isValid())
    {
      ATH_MSG_FATAL("Could not retrieve " << m_tracks.key());
      return StatusCode::FAILURE;
    }

  xAOD::TrackParticleContainer::const_iterator mytracklet;

  int n_tracks=0;
  int n_tracklets=0;

  for (mytracklet = tracklets->begin(); mytracklet != tracklets->end(); mytracklet++) // Loop over tracklets
    { 
      n_tracklets++;
      int nSharedHits=0;
      const xAOD::TrackParticle* trackletParticle = *mytracklet;
      const Trk::Track* tracklet =  trackletParticle->track();
      if(!tracklet)
        return StatusCode::FAILURE;
    
      std::vector<const InDet::PixelCluster*> tracklet_pixelClusters;
  
      const DataVector<const Trk::MeasurementBase>* tracklet_measurements = tracklet->measurementsOnTrack();
      for ( auto  tracklet_measurement : *tracklet_measurements )
        { 
          //  find out if it is a pixel hit
          const InDet::PixelClusterOnTrack* pixelClusterOnTracklet = dynamic_cast<const InDet::PixelClusterOnTrack*>(tracklet_measurement);
          if( pixelClusterOnTracklet )
            {
              //Go back to the original pixel cluster
              const InDet::PixelCluster* tracklet_pixelCluster = dynamic_cast<const InDet::PixelCluster*>( pixelClusterOnTracklet->prepRawData() );
       
              if( tracklet_pixelCluster )
                {
                  // This is the pixel cluster that you would like to check if it contained in the other tracklet.
                  // to avoid doing this loop multiple time then would store these is a vector
                  tracklet_pixelClusters.push_back(tracklet_pixelCluster);
                }
            } // End: tracklet_PixelCluster
        } // End: tracklet_Measurements
  
      xAOD::TrackParticleContainer::const_iterator mytrack;
      for (mytrack = trks->begin(); mytrack != trks->end(); mytrack++) // Loop over tracks for each tracklet
        { 
          n_tracks++;
          const xAOD::TrackParticle* trackParticle = *mytrack;
          const Trk::Track* track =  trackParticle->track();
          if(!track)
            return StatusCode::FAILURE;
    
          std::vector<const InDet::PixelCluster*> pixelClusters;
  
          const DataVector<const Trk::MeasurementBase>* measurements = track->measurementsOnTrack();
          for ( auto  measurement : *measurements )
            { 
              //  find out if it is a pixel hit
              const InDet::PixelClusterOnTrack* pixelClusterOnTrack = dynamic_cast<const InDet::PixelClusterOnTrack*>(measurement);
              if( pixelClusterOnTrack )
                {
                  //Go back to the original pixel cluster
                  const InDet::PixelCluster* pixelCluster = dynamic_cast<const InDet::PixelCluster*>( pixelClusterOnTrack->prepRawData() );
       
                  if( pixelCluster )
                    {
                      // This is the pixel cluster that you would like to check if it contained in the other track.
                      // to avoid doing this loop multiple time then would store these is a vector
                      pixelClusters.push_back(pixelCluster);
                    }
                } // End: PixelCluster
            } // End: Measurements
    
          int n_corr=0;
          for (unsigned int i = 0; i < pixelClusters.size(); i++)
            {
              if ( std::find(tracklet_pixelClusters.begin(),tracklet_pixelClusters.end(),pixelClusters[i]) != tracklet_pixelClusters.end() )
                n_corr++;
            } 
          if (n_corr>0)
            {
              nSharedHits=n_corr;
              ElementLink< xAOD:: TrackParticleContainer > eLink( *trks, trackParticle->index());
              eLink.toPersistent();
              trackletParticle->auxdecor<  ElementLink< xAOD:: TrackParticleContainer >  >("MatchedTrackLink") = eLink;
	
              break; //??? how many n_corr?

            }  
        } // End: Loop over tracks for each tracklet
  
      trackletParticle->auxdecor<int>("MatchedTrackSharedHits") = nSharedHits;
  

  
    } // End: Loop over tracklets

  if (n_tracks==0 || n_tracklets==0) ATH_MSG_INFO("=========>>>>>>>>n_tracklets=" << n_tracklets << " =========>>>>>>>>n_tracks="<< n_tracks);


  ATH_MSG_DEBUG ("Your new package and algorithm are successfully executing"); 

  return StatusCode::SUCCESS;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode SCTExtensionAlg::finalize() {
    
  ATH_MSG_INFO ("finalize()");
    
  return StatusCode::SUCCESS;
}

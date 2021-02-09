/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class RoISeedTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// AlgTool used for define RoI for TrkSeeded seeding algoirthm
///////////////////////////////////////////////////////////////////


#include "TrkTrack/TrackCollection.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "SiSpacePointsSeedTool_xk/RoISeedTool.h"
#include "TVector2.h"
#include <map>


///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::RoISeedTool::RoISeedTool
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p),
    m_RoiSeedTracks("ResolvedPixelThreeLayerTracks"),
    m_tracksForIsolation("ResolvedTracks")
{

  m_doTrkMethod          = false;
  m_doVtxMethod          = false;
  m_RoISeedTrackD0       = 10.0;
  m_RoISeedTrackPt       = 20000.0;
  m_RoISeedTrackSCTHits  = 0;
  m_RoISeedTrackPixHits  = 3;
  m_RoISeedTrackIso      = 0.1;
  m_IsoTrackPtThr        = 400.0;
  m_IsoTrackConeSize     = 0.4;
  m_RoISeedTrackD0Sort   = true;

  //
  declareInterface<IRoISeedTool>(this);

  //
  declareProperty("RoISeedTrackContainer" ,m_RoiSeedTracks         );
  declareProperty("TracksForIsolation"    ,m_tracksForIsolation    );
  //
  declareProperty("RoISeedRTrackD0"       ,m_RoISeedTrackD0        );
  declareProperty("RoISeedTrackPt"        ,m_RoISeedTrackPt        );
  declareProperty("RoISeedTrackSCTHits"   ,m_RoISeedTrackSCTHits   );
  declareProperty("RoISeedTrackPixHits"   ,m_RoISeedTrackPixHits   );
  declareProperty("RoISeedTrackIso"       ,m_RoISeedTrackIso       );
  declareProperty("IsoTrackPtThr"         ,m_IsoTrackPtThr         );
  declareProperty("IsoTrackConeSize"      ,m_IsoTrackConeSize      );
  declareProperty("DoRoITrackD0Sort"      ,m_RoISeedTrackD0Sort    );

}

///////////////////////////////////////////////////////////////////
// Destructor
///////////////////////////////////////////////////////////////////

InDet::RoISeedTool::~RoISeedTool()
{
}

///////////////////////////////////////////////////////////////////
// Initialization
///////////////////////////////////////////////////////////////////

StatusCode InDet::RoISeedTool::initialize()
{
  StatusCode sc = AlgTool::initialize(); 

  m_doTrkMethod = false;
  m_doVtxMethod   = false;
  

  if( m_RoiSeedTracks!="" ){
    m_doTrkMethod = true;
  }

  // TODO Vtx
  
  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::RoISeedTool::finalize()
{
   StatusCode sc = AlgTool::finalize(); return sc;
}

/////////////////////////////////////////////////////////////////////
// Grab the measurements that will be used to generate seeds
/////////////////////////////////////////////////////////////////////

std::vector<Amg::Vector3D> InDet::RoISeedTool::getRoIs()
{

  //
  std::vector<Amg::Vector3D> ROISeeds;
  ROISeeds.clear();

  /*
    Track seeds
  */
  if( m_doTrkMethod ){
    getTrkMeasSeeds(ROISeeds);
  }

  /*
    Vertex seeds
  */
  if( m_doVtxMethod ){
    getVtxSeeds(ROISeeds);
  }
  
  return ROISeeds;

}

void InDet::RoISeedTool::getTrkMeasSeeds(std::vector<Amg::Vector3D>& trkseeds)
{

  // For sorting entries
  std::vector<std::pair<float,Amg::Vector3D>> trackSeedMap;
  trackSeedMap.clear();

  const TrackCollection* tracks = 0;
  if( evtStore()->retrieve ( tracks, m_RoiSeedTracks ).isFailure() ){
    ATH_MSG_WARNING( "Could not find " << m_RoiSeedTracks << " in StoreGate!!");
    return;
  }
  ATH_MSG_DEBUG("Starting with " << tracks->size() << "... Going to try and reduce this...");

  const TrackCollection* std_tracks = 0;
  if( evtStore()->retrieve ( std_tracks, m_tracksForIsolation ).isFailure() ){
    ATH_MSG_WARNING( "Could not find " << m_tracksForIsolation << " in StoreGate" );
    return;
  }

  /*
     Loop over all RoI tracks
  */
  for( const auto& sTrack : *tracks ){

    /*
      Hit requirements
        - Maximum number of SCT hits
        - Minumum number of Pixel hits
    */
    const Trk::TrackSummary* trkSum = sTrack->trackSummary();
    if( !trkSum ){
      ATH_MSG_WARNING("TrackSummary doesn't exist! Track will be ignored from seeding algorithm!" );
      continue;
    }
    if( trkSum->get( Trk::SummaryType::numberOfSCTHits   )>m_RoISeedTrackSCTHits   ) continue;
    if( trkSum->get( Trk::SummaryType::numberOfPixelHits )<m_RoISeedTrackPixHits   ) continue;
    
    /*
      Perigee requirments
        - Transverse momentum,
        - Track d0
    */
    const Trk::Perigee* perigee = sTrack->perigeeParameters();
    if( !perigee ){
      ATH_MSG_WARNING("No valid perigee. Track will be ignored from seeding algorithm!" );
      continue;
    }
    if( perigee->momentum().perp()<m_RoISeedTrackPt            ) continue;
    if( fabs(perigee->parameters()[Trk::d0])>m_RoISeedTrackD0  ) continue;

    /*
      Isolation requirements
        - Relative isolation cut
    */

    float ptcone = getTrackPtCone(std_tracks,perigee);
    if( (ptcone/perigee->momentum().perp()) > m_RoISeedTrackIso ) continue;

    /*
      Cache last MeasurementOnTrack, with its ranking variable
        - Track d0
    */
    const DataVector<const Trk::MeasurementBase>* MoTs = sTrack->measurementsOnTrack();
    if( MoTs->size()==0 ){
      ATH_MSG_WARNING("NO TRACK PARAMETERS FOUND........." );
      continue;
    }

    const Trk::MeasurementBase* MoT = 0;
    for( auto& m : *MoTs ){
      /*
        Ignore pseudo measurements
      */
      if( dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(m) ) continue;
      /*
        Choose last measurement along the track
      */
      if( !MoT || m->globalPosition().perp()>MoT->globalPosition().perp() ){
        MoT = m;
      }
    }
    if( !MoT ) continue;
    trackSeedMap.push_back( std::pair<float,Amg::Vector3D>(perigee->parameters()[Trk::d0],MoT->globalPosition()) );

  } // Lopp over RoI tracks

  /*
   Rank MeasurementsOnTrack by their d0 measured at the perigee
  */
  if( m_RoISeedTrackD0Sort ){
    std::sort(trackSeedMap.begin(),trackSeedMap.end(), rankD0() );
  }

  for(auto& m : trackSeedMap ){
    trkseeds.push_back(m.second);
  }


}
///////////////////////////////////////////////////////////////////
// Get all RoI measurements from a list of vertices
/////////////////////////////////////////////////////////////////////

void InDet::RoISeedTool::getVtxSeeds(std::vector<Amg::Vector3D>& /*vtxseeds*/)
{

  // Undeveloped

}

///////////////////////////////////////////////////////////////////
// Compute the ptcone for a given tracklet
/////////////////////////////////////////////////////////////////////

float InDet::RoISeedTool::getTrackPtCone(const TrackCollection* std_tracks,
					 const Trk::Perigee* perigee)
{
  float ptcone = 0.0;
  for( const auto& stdTrack : *std_tracks ){
    const Trk::Perigee* std_perigee = stdTrack->perigeeParameters();
    if( !std_perigee ){
      ATH_MSG_WARNING("NO VALID perigee for standard resolved tracks. Ignoring this track...." );
      continue;
    }
    if( std_perigee->momentum().perp()<m_IsoTrackPtThr ) continue;
    if( abs( std_perigee->eta() - perigee->eta() ) > m_IsoTrackConeSize ) continue;
    if( abs( TVector2::Phi_mpi_pi( std_perigee->momentum().phi() - perigee->momentum().phi()  ) ) > m_IsoTrackConeSize ) continue;
    ptcone += std_perigee->momentum().perp();
  }
  return ptcone;
}

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class ZWindowRoISeedTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#include "TrkTrack/TrackCollection.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "SiSpacePointsSeedTool_xk/ZWindowRoISeedTool.h"
#include "TVector2.h"
#include <map>


///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::ZWindowRoISeedTool::ZWindowRoISeedTool
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p),
    m_input_tracks_collection("Tracks")
{

  //
  declareInterface<IZWindowRoISeedTool>(this);

  //
  declareProperty("InputTracksCollection", m_input_tracks_collection );  
  declareProperty("LeadingMinTrackPt", m_trk_leading_pt = 27000.); 
  declareProperty("SubleadingMinTrackPt", m_trk_subleading_pt = 20000.); 
  declareProperty("TracksMaxEta", m_trk_eta_max = 2.5);
  declareProperty("TracksMaxD0", m_trk_d0_max = 9999.);
  declareProperty("MaxDeltaZTracksPair", m_max_delta_z = 1.0);
  declareProperty("TrackZ0Window", m_z0_window = 1.0);

}

///////////////////////////////////////////////////////////////////
// Destructor
///////////////////////////////////////////////////////////////////

InDet::ZWindowRoISeedTool::~ZWindowRoISeedTool()
{
}

///////////////////////////////////////////////////////////////////
// Initialization
///////////////////////////////////////////////////////////////////

StatusCode InDet::ZWindowRoISeedTool::initialize()
{
  StatusCode sc = AlgTool::initialize();   
  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::ZWindowRoISeedTool::finalize()
{
   StatusCode sc = AlgTool::finalize(); 
   return sc;
}

/////////////////////////////////////////////////////////////////////
// Compute RoI
/////////////////////////////////////////////////////////////////////

std::vector<InDet::IZWindowRoISeedTool::ZWindow> InDet::ZWindowRoISeedTool::getRoIs()
{

  // prepare output
  std::vector<InDet::IZWindowRoISeedTool::ZWindow> listRoIs;
  InDet::IZWindowRoISeedTool::ZWindow RoI;
  listRoIs.clear();

  //select tracks, then order by pT
  const TrackCollection* tracks = 0;
  std::vector<Trk::Track*> selectedTracks;
  if ( evtStore()->retrieve(tracks, m_input_tracks_collection).isFailure() ) {
    ATH_MSG_DEBUG("Could not find TrackCollection " << m_input_tracks_collection << " in StoreGate.");
    return listRoIs;    
  }
  ATH_MSG_DEBUG("Input track collection size "<<tracks->size());
  for ( Trk::Track* trk : tracks->stdcont() ) {
    float theta = trk->perigeeParameters()->parameters()[Trk::theta];
    float ptinv = fabs(trk->perigeeParameters()->parameters()[Trk::qOverP]) / sin(theta);    
    if (ptinv < 0.001) //1 GeV tracks
      ATH_MSG_VERBOSE("Examining track");
    if (ptinv != 0) {
      float pt = 1. / ptinv;      
      if (pt > 1000.) //1 GeV tracks for printout
	ATH_MSG_VERBOSE("- pT = " << pt << " MeV");
      if ( pt < m_trk_subleading_pt ) continue;
    }
    float eta = -log( tan( theta/2 ) );
    ATH_MSG_VERBOSE("- eta = " << eta);
    if ( fabs(eta) > m_trk_eta_max ) continue;
    float d0 = trk->perigeeParameters()->parameters()[Trk::d0];
    ATH_MSG_VERBOSE("- d0 = " << d0 << "mm");
    if ( fabs(d0) > m_trk_d0_max ) continue;
    ATH_MSG_VERBOSE("- Passed all selections");
    selectedTracks.push_back(trk);
  }
  std::sort(selectedTracks.begin(), selectedTracks.end(), tracks_pt_greater_than);
  ATH_MSG_DEBUG("Selected track collection size "<<selectedTracks.size());
  //create all pairs that satisfy leading pT and delta z0 requirements
  typedef std::vector<Trk::Track*>::iterator iterator_tracks;
  for ( iterator_tracks trk_itr_leading = selectedTracks.begin(); trk_itr_leading != selectedTracks.end(); ++trk_itr_leading ) {
    Trk::Track *trk_leading = *trk_itr_leading;
    //kinematic requirements
    float theta_leading = trk_leading->perigeeParameters()->parameters()[Trk::theta];
    float ptinv_leading = fabs(trk_leading->perigeeParameters()->parameters()[Trk::qOverP]) / sin(theta_leading);
    ATH_MSG_VERBOSE("Examining selected track pairs");
    if (ptinv_leading != 0) {
      float pt = 1. / ptinv_leading;
      ATH_MSG_VERBOSE("- pT_leading = " << pt << " MeV");
      if ( pt < m_trk_leading_pt ) break; //tracks ordered by pT
    }
    //loop over sub-leading track
    for ( iterator_tracks trk_itr = (trk_itr_leading + 1); trk_itr != selectedTracks.end(); ++trk_itr ) {
      Trk::Track *trk = *trk_itr;
      //kinematic requirements
      float z0_leading = trk_leading->perigeeParameters()->parameters()[Trk::z0];
      float z0 = trk->perigeeParameters()->parameters()[Trk::z0];
      ATH_MSG_VERBOSE("- z0_leading = " << z0_leading << " mm");
      ATH_MSG_VERBOSE("- z0_sublead = " << z0 << " mm");
      if ( fabs(z0_leading - z0) > m_max_delta_z ) continue;
      //create the pair in global coordinates 
      float z0_trk_reference = trk->perigeeParameters()->associatedSurface().center().z();
      float z0_trk_leading_reference = trk_leading->perigeeParameters()->associatedSurface().center().z();
      RoI.z_reference = (z0 + z0_trk_reference + z0_leading + z0_trk_leading_reference) / 2;
      RoI.z_window[0] = RoI.z_reference - m_z0_window; 
      RoI.z_window[1] = RoI.z_reference + m_z0_window; 
      ATH_MSG_INFO("New RoI created [mm]: " << RoI.z_window[0] << " - " << RoI.z_window[1] << " (z-ref: " << RoI.z_reference << ")");
      listRoIs.push_back(RoI);
    }
  }

  return listRoIs;
  
}


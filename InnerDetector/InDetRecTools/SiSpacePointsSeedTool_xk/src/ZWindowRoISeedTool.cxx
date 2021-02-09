/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "ITrackToVertex/ITrackToVertex.h"
#include "TVector2.h"
#include <map>
#include <cmath>


///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::ZWindowRoISeedTool::ZWindowRoISeedTool
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p),
    m_trackToVertex       ("Reco::TrackToVertex/TrackToVertex"),
    m_inputTracksCollection("Tracks")
{

  //
  declareInterface<IZWindowRoISeedTool>(this);

  //
  declareProperty("InputTracksCollection", m_inputTracksCollection );  
  declareProperty("LeadingMinTrackPt", m_trkLeadingPt = 18000.); 
  declareProperty("SubleadingMinTrackPt", m_trkSubLeadingPt = 12500.); 
  declareProperty("TracksMaxEta", m_trkEtaMax = 2.5);
  declareProperty("TracksMaxD0", m_trkD0Max = 9999.);
  declareProperty("MaxDeltaZTracksPair", m_maxDeltaZ = 2.0);
  declareProperty("TrackZ0Window", m_z0Window = 1.0);
  declareProperty("TrackToVertex",            m_trackToVertex );

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
  
  ATH_CHECK( m_trackToVertex.retrieve() );
  ATH_MSG_DEBUG( "Retrieved tool " << m_trackToVertex );
  
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

  static const float nonZeroInvP = 1e-9;
  
  // prepare output
  std::vector<InDet::IZWindowRoISeedTool::ZWindow> listRoIs;
  InDet::IZWindowRoISeedTool::ZWindow RoI;
  listRoIs.clear();

  //select tracks, then order by pT
  const TrackCollection* tracks = 0;
  std::vector<Trk::Track*> selectedTracks;
  if ( evtStore()->retrieve(tracks, m_inputTracksCollection).isFailure() ) {
    ATH_MSG_DEBUG("Could not find TrackCollection " << m_inputTracksCollection << " in StoreGate.");
    return listRoIs;    
  }
  ATH_MSG_DEBUG("Input track collection size "<<tracks->size());
  for ( Trk::Track* trk : tracks->stdcont() ) {
    float theta = trk->perigeeParameters()->parameters()[Trk::theta];
    float ptinv = std::abs(trk->perigeeParameters()->parameters()[Trk::qOverP]) / std::sin(theta);    
    if (ptinv < 0.001) //1 GeV tracks
      ATH_MSG_VERBOSE("Examining track");
    if ( std::abs(ptinv) > nonZeroInvP ) {
      float pt = 1. / ptinv;      
      if (pt > 1000.) //1 GeV tracks for printout
	ATH_MSG_VERBOSE("- pT = " << pt << " MeV");
      if ( pt < m_trkSubLeadingPt ) continue;
    }
    float eta = -std::log( std::tan( 0.5*theta ) );
    ATH_MSG_VERBOSE("- eta = " << eta);
    if ( std::abs(eta) > m_trkEtaMax ) continue;
    float d0 = trk->perigeeParameters()->parameters()[Trk::d0];
    ATH_MSG_VERBOSE("- d0 = " << d0 << "mm");
    if ( std::abs(d0) > m_trkD0Max ) continue;
    ATH_MSG_VERBOSE("- Passed all selections");
    selectedTracks.push_back(trk);
  }
  std::sort(selectedTracks.begin(), selectedTracks.end(), tracksPtGreaterThan);
  ATH_MSG_DEBUG("Selected track collection size "<<selectedTracks.size());
  //create all pairs that satisfy leading pT and delta z0 requirements
  typedef std::vector<Trk::Track*>::iterator iteratorTracks;
  for ( iteratorTracks trkItrLeading = selectedTracks.begin(); trkItrLeading != selectedTracks.end(); ++trkItrLeading ) {
    Trk::Track *trkLeading = *trkItrLeading;
    //kinematic requirements
    float thetaLeading = trkLeading->perigeeParameters()->parameters()[Trk::theta];
    float ptInvLeading = std::abs(trkLeading->perigeeParameters()->parameters()[Trk::qOverP]) / std::sin(thetaLeading);
    ATH_MSG_VERBOSE("Examining selected track pairs");
    if (std::abs(ptInvLeading) > nonZeroInvP) {
      float pt = 1. / ptInvLeading;
      ATH_MSG_VERBOSE("- pT_leading = " << pt << " MeV");
      if ( pt < m_trkLeadingPt ) break; //tracks ordered by pT
    }
    //loop over sub-leading track
    for ( iteratorTracks trkItr = (trkItrLeading + 1); trkItr != selectedTracks.end(); ++trkItr ) {
      Trk::Track *trk = *trkItr;
      //kinematic requirements
      float z0Leading = trkLeading->perigeeParameters()->parameters()[Trk::z0];
      float z0 = trk->perigeeParameters()->parameters()[Trk::z0];
      ATH_MSG_VERBOSE("- z0Leading = " << z0Leading << " mm");
      ATH_MSG_VERBOSE("- z0_sublead = " << z0 << " mm");

      const Trk::Perigee* leadAtBeam = m_trackToVertex->perigeeAtBeamline(*trkLeading);
      const Trk::Perigee* subleadAtBeam = m_trackToVertex->perigeeAtBeamline(*trk);
      float z0LeadingBeam = leadAtBeam->parameters()[Trk::z0];
      float z0Beam = subleadAtBeam->parameters()[Trk::z0];

      if ( std::abs(z0LeadingBeam - z0Beam) > m_maxDeltaZ ) continue;
      //create the pair in global coordinates 
      float z0TrkReference = subleadAtBeam->associatedSurface().center().z();
      float z0TrkLeadingReference = leadAtBeam->associatedSurface().center().z();
      RoI.zReference = (z0Beam + z0TrkReference + z0LeadingBeam + z0TrkLeadingReference) / 2;
      RoI.zWindow[0] = RoI.zReference - m_z0Window; 
      RoI.zWindow[1] = RoI.zReference + m_z0Window; 
      RoI.zPerigeePos[0] = z0LeadingBeam; 
      RoI.zPerigeePos[1] = z0Beam; 
      ATH_MSG_DEBUG("New RoI created [mm]: " << RoI.zWindow[0] << " - " << RoI.zWindow[1] << " (z-ref: " << RoI.zReference << ")");
      listRoIs.push_back(RoI);
    }
  }


  if( listRoIs.empty() ){
    for( Trk::Track* trkItrLeading2 : selectedTracks ){
      Trk::Track *trkLeading = trkItrLeading2;
      //kinematic requirements
      float thetaLeading = trkLeading->perigeeParameters()->parameters()[Trk::theta];
      float ptInvLeading = std::abs(trkLeading->perigeeParameters()->parameters()[Trk::qOverP]) / std::sin(thetaLeading);
      ATH_MSG_VERBOSE("Examining selected track pairs");
      if (std::abs(ptInvLeading) > nonZeroInvP) {
	float pt = 1. / ptInvLeading;
	ATH_MSG_VERBOSE("- pT_leading = " << pt << " MeV");
	if ( pt < m_trkLeadingPt ) break; //tracks ordered by pT
	
	const Trk::Perigee* leadAtBeam = m_trackToVertex->perigeeAtBeamline(*trkLeading);
	float z0LeadingBeam = leadAtBeam->parameters()[Trk::z0];
	
	//create the pair in global coordinates 
	float z0TrkLeadingReference = leadAtBeam->associatedSurface().center().z();
	RoI.zReference = z0LeadingBeam + z0TrkLeadingReference;
	RoI.zWindow[0] = RoI.zReference - m_z0Window; 
	RoI.zWindow[1] = RoI.zReference + m_z0Window; 
	RoI.zPerigeePos[0] = z0LeadingBeam; 
	ATH_MSG_DEBUG("New RoI created [mm]: " << RoI.zWindow[0] << " - " << RoI.zWindow[1] << " (z-ref: " << RoI.zReference << ")");
	listRoIs.push_back(RoI);
	
      }
      

    }
  }

  return listRoIs;
  
}


// emacs: this is -*- c++ -*-

#ifndef TRIGINDETANALYSISUTILS_TRIGTRACKSELECTOR_H
#define TRIGINDETANALYSISUTILS_TRIGTRACKSELECTOR_H

#include "TrigInDetAnalysisUtils/TIDA_newtracking.h"

#ifndef  TIDA_NEWTRACKING_H
#include "TrigInDetAnalysisUtils/TrigTrackSelector_old.h"
#else

/// L2 tracks
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
///TruthMap
#include "TrigInDetTruthEvent/TrigInDetTrackTruthMap.h"

/// offline and EF
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"

/// MC truth
#include "McParticleEvent/TruthParticleContainer.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

///// stuff *merely* to get the particle charge!!!
/// #include "HepPDT/ParticleData.hh"
/// #include "HepMC/ParticleDataTable.h"
/// #include "McParticleEvent/TruthParticle.h"
/// #include "McParticleUtils/McUtils.h" // for chargeFromPdgId
///
///// FrameWork includes
/// #include "GaudiKernel/ServiceHandle.h"
/// #include "GaudiKernel/IPartPropSvc.h"
/// absolutely pathetic!! write my own class

#include "TrigInDetAnalysisUtils/particleType.h"

/// TrigInDetAnalysis classes
#include "TrigInDetAnalysis/TrackSelector.h"
#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/TrackFilter.h"

#include "TrkParameters/TrackParameters.h"
// #include "TrkParameters/Perigee.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/InDetTrackSummary.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "GaudiKernel/ToolHandle.h" 
#include "TMath.h"


// xAOD
// not actual XAOD, but this file safely includes 
// the xAOD::TrackParticle header if it exists
#include "TrkParticleCreator/TrackParticleCreatorTool.h"


// namspace TrigInDetAnalysis {

// class TrigTrackSelector : public TIDA::TrackSelector<TIDA::Track> { 

class TrigTrackSelector : public TrackSelector { 

public:

  //  TrigTrackSelector( bool (*selector)(const TIDA::Track*)=NULL ) : TrackSelector(selector) {  } 
  TrigTrackSelector( TrackFilter* selector );

  ~TrigTrackSelector() { clear(); }


  void setBeamline( double x, double y, double z=0) { m_xBeam = x; m_yBeam = y; m_zBeam=z; }

  double getBeamX() const { return m_xBeam; } 
  double getBeamY() const { return m_yBeam; } 
  double getBeamZ() const { return m_zBeam; } 

  void correctTracks(bool b=true) { m_correctTrkTracks = b; } 

  virtual void clear() { for ( unsigned i=mtracks.size() ; i-- ; ) delete mtracks[i]; mtracks.clear(); }   


  void selectTrack( const TrigInDetTrack* track, const TrigInDetTrackTruthMap* truthMap=0 );


  // extract all the tracks from a TrigInDetTrack collection and associated TruthMap and convert them
  void selectTracks( const TrigInDetTrackCollection* trigtracks, const TrigInDetTrackTruthMap* truthMap=0 );

  // add a TrackParticle 
  void selectTrack( const Rec::TrackParticle* track );
  

  // extract all the tracks from a TrackParticle collection and add them
  void selectTracks( const Rec::TrackParticleContainer* trigtracks );


  // extract all the tracks from a TrackParticle collection and add them
  void selectTracks( const TruthParticleContainer* truthtracks );


  // add a TruthParticle from a GenParticle - easy, bet it doesn't work 
  void selectTrack( const HepMC::GenParticle* track );


  // add a TruthParticle 
  void selectTrack( const TruthParticle& track );

  // add a TruthParticle 
  void selectTrack( const TruthParticle* track );


  // make a TIDA::Track from a GenParticle 
  TIDA::Track* makeTrack( const HepMC::GenParticle* track );

  // make a TIDA::Track from a TruthParticle 
  TIDA::Track* makeTrack( const TruthParticle* track, unsigned long tid=0 );


  // add a Trk::Track
  void selectTrack( const Trk::Track* track );

  
  // extract all the tracks from a TrackCollection and add them
  void selectTracks( const TrackCollection* trigtracks );


#ifdef XAODTRACKING_TRACKPARTICLE_H

  void selectTrack( const xAOD::TrackParticle* track, void* =0);


  void selectTracks( const xAOD::TrackParticleContainer* tracks, void* =0);

#endif



  void correctToBeamline( double& z0,    double& dz0, 
			  double& d0,    double& dd0, 
			  double  theta, double  phi );

  

private:

  unsigned long m_id;

  double m_xBeam;
  double m_yBeam;
  double m_zBeam;

  //bool m_first;

  bool m_correctTrkTracks;

};


// }

#endif //   TIDA_NEWTRACKING_H
#endif // TRIGINDETANALYSISUTILS_TRIGTRACKSELECTOR_H

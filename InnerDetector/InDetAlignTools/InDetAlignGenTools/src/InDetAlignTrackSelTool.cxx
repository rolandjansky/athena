/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// InDetAlignTrackSelTool.h

// Sergio Gonzalez Sevilla, started 04/7/05
// Miguel Olivo Gomez, extended 07/6/06

// AlgTool to select high quality tracks for the inner detector 
// (Pixel+SCT) alignment algorithms.
// This AlgTool provides a track selection based on the following cut variables:
// Momentum, pt, number of shared hits, number of holes and chi2 probability. 
// Returns 0 in case a track is not accepted, otherwise 1

#include "InDetAlignGenTools/InDetAlignTrackSelTool.h"

#include "TrkTrackSummary/TrackSummary.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"

InDetAlignTrackSelTool::InDetAlignTrackSelTool( const std::string& type
						, const std::string& name
						, const IInterface* parent
						)
  : AthAlgTool(type,name,parent),
    m_trackSumTool("Trk::TrackSummaryTool", this),
    m_minMomentum(0),
    m_minPt(2),
    m_maxShared(0),
    m_maxHoles(1),
    m_minChi2Prob(0.2)
{
  declareInterface<IInDetAlignTrackSelTool>(this);
  declareProperty("MinMomentum"        , m_minMomentum);
  declareProperty("MinPt"              , m_minPt);
  declareProperty("MaxShared"          , m_maxShared);
  declareProperty("MaxHoles"           , m_maxHoles);
  declareProperty("MinChi2Prob"        , m_minChi2Prob);

  // Tools
  declareProperty("TrackSummaryTool"   , m_trackSumTool,
		  "tool to extract track info");
}

InDetAlignTrackSelTool::~InDetAlignTrackSelTool()
{}

////////////////////////////////////////////////////////////////////////////////////////
StatusCode InDetAlignTrackSelTool::initialize(){
  ////////////////////////////////////////////////////////////////////////////////////////

  // get TrackSummaryTool
  if ( m_trackSumTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackSumTool ) ;
    return StatusCode::FAILURE;
  } else ATH_MSG_DEBUG( "Retrieved tool " << m_trackSumTool ) ;
  
  ATH_MSG_DEBUG( "Cuts selected : min_Momentum(CLHEP::GeV)=" <<  m_minMomentum
		<< " min_pt(CLHEP::GeV)=" <<  m_minPt
		<< " max_shared=" << m_maxShared
		<< " max_holes=" << m_maxHoles
		<< " min_chi2Prob=" << m_minChi2Prob ) ;

  ATH_MSG_DEBUG( "InDetAlignTrackSelTool initialize() successful" ) ;
  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////
StatusCode InDetAlignTrackSelTool::finalize(){
  ////////////////////////////////////////////////////////////////////////////////////////
  ATH_MSG_DEBUG( "InDetAlignTrackSelTool finalize method called" ) ;
  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////
double InDetAlignTrackSelTool::Momentum(const Trk::Track& track) const {
  ////////////////////////////////////////////////////////////////////////////////////////
  ATH_MSG_DEBUG( "in Momentum() " ) ;
  double mom=0.;

  // get measured perigee and momentum of track
  const Trk::Perigee* perigee = track.perigeeParameters();

  if ( !perigee->covariance()) {
    ATH_MSG_ERROR( "No measured perigee parameters assigned to the track" ) ; 
    mom = -1e12; // return big value
  }
  else{
    Amg::VectorX perigeeParams = perigee->parameters();
    mom = fabs(1./perigeeParams[Trk::qOverP]);
    mom /= 1000.; //mom in GeV
  }

  return mom;
}

////////////////////////////////////////////////////////////////////////////////////////
double InDetAlignTrackSelTool::Pt(const Trk::Track& track) const {
  ////////////////////////////////////////////////////////////////////////////////////////
  ATH_MSG_DEBUG( "in Pt() " ) ;
  double pt=0.;

  // get measured perigee and pt of track
  const Trk::Perigee* perigee = track.perigeeParameters();

  if (!perigee->covariance()) {
    ATH_MSG_ERROR( "No measured perigee parameters assigned to the track" ) ; 
    pt = -1e12; // return big value
  }
  else{  
    Amg::VectorX perigeeParams = perigee->parameters();
    pt = fabs(sin(perigeeParams[Trk::theta])/perigeeParams[Trk::qOverP]);
    pt /= 1000.; // pt in GeV
  }

  return pt;
}

////////////////////////////////////////////////////////////////////////////////////////
int InDetAlignTrackSelTool::nShared(const Trk::Track& track) const {
  ////////////////////////////////////////////////////////////////////////////////////////
  ATH_MSG_DEBUG( "in nShared()" ) ;
  int nshared=0, nshpix, nshsct;

  std::unique_ptr<Trk::TrackSummary> summary = m_trackSumTool->summary(track);
  if(!summary){
    ATH_MSG_ERROR( "Could not create TrackSummary" ) ;
    nshared = 1000;
  }
  else{
    nshpix = summary->get(Trk::numberOfPixelSharedHits);
    nshsct = summary->get(Trk::numberOfSCTSharedHits);

    if(nshpix==-1) 
      nshpix=0;

    if(nshsct==-1) 
      nshsct=0;

    nshared = nshpix + nshsct;
  }
  return nshared;
}

////////////////////////////////////////////////////////////////////////////////////////
int InDetAlignTrackSelTool::nHoles(const Trk::Track& track) const {
  ////////////////////////////////////////////////////////////////////////////////////////
  ATH_MSG_DEBUG( "in nHoles() " ) ;
  int nholes=0, nhpix, nhsct;

  std::unique_ptr<Trk::TrackSummary> summary = m_trackSumTool->summary(track);
  if(summary==0){
    ATH_MSG_ERROR( "Could not create TrackSummary" ) ;
    nholes = 1000;
  }
  else{
    ATH_MSG_VERBOSE( "TrackSummary created. Dumping it..." ) ;
    ATH_MSG_VERBOSE( (*summary) ) ;

    nhpix = summary->get(Trk::numberOfPixelHoles);
    nhsct = summary->get(Trk::numberOfSCTHoles);

    if(nhpix==-1)
      nhpix = 0;

    if(nhsct==-1)
      nhsct = 0;

    nholes =  nhpix + nhsct;
  }
  return nholes;
}

////////////////////////////////////////////////////////////////////////////////////////
double InDetAlignTrackSelTool::chi2Prob(const Trk::Track& track) const {
  ////////////////////////////////////////////////////////////////////////////////////////
  ATH_MSG_DEBUG( "in chi2Prob()" ) ;
  double chi2Prob=0.;

  // get fit quality and chi2 probability of track
  // chi2Prob = TMath::Prob(chi2,DoF) ROOT function
  const Trk::FitQuality* fitQual = track.fitQuality();

  if (fitQual==0) {
    ATH_MSG_ERROR( "No fit quality assigned to the track" ) ; 
    chi2Prob = -1e12; // return big value
  }
  else {
    if (fitQual->chiSquared() > 0. && fitQual->numberDoF() > 0) {
      Genfun::CumulativeChiSquare probabilityFunction( fitQual->numberDoF() );
      chi2Prob = 1 - probabilityFunction( fitQual->chiSquared() );
    }
  }

  return chi2Prob;
}

////////////////////////////////////////////////////////////////////////////////////////
int InDetAlignTrackSelTool::getStatus(const Trk::Track& track) const {
  ////////////////////////////////////////////////////////////////////////////////////////
  ATH_MSG_DEBUG( "in getStatus()" ) ;
  int stat=1, nholes, nshared;
  double mom, pt, chi2prob;

  // momentum
  mom = Momentum(track);
  if (mom < m_minMomentum) {
    stat=0;
  }

  // transverse momentum
  pt = Pt(track);
  if (pt < m_minPt) {
    stat=0; 
  }

  // number of holes
  nholes = nHoles(track);
  if (nholes > m_maxHoles) {
    stat=0; 
  }

  // number of shared hits
  nshared = nShared(track);
  if (nshared > m_maxShared) {
    stat=0; 
  }

  // chi2 Probability
  chi2prob = chi2Prob(track);
  if (chi2prob < m_minChi2Prob) {
    stat=0;
  }

  ATH_MSG_DEBUG( " momentum(CLHEP::GeV)=" << mom 
		 << " pt (CLHEP::GeV)=" << pt 
		 << " nshared=" << nshared
		 << " nholes=" <<  nholes 
		 << " chi2Prob=" << chi2prob
		 ) ;

  if(!stat)
    ATH_MSG_DEBUG( "Track not accepted" ) ;
  else
    ATH_MSG_DEBUG( "Track accepted" ) ;

  return stat;
}

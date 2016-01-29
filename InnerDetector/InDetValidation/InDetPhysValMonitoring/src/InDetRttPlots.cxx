/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetRttPlots.cxx
 * @author shaun roe
**/

#include "InDetRttPlots.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h" 
#include "xAODTruth/TruthVertex.h" 
#include <cmath> //std::isnan()
#include <limits>

using std::cout;
using std::endl;
using std::string;

namespace { //utility functions used in this area - stolen from InDetPhysValMonitoringTool and other places - not so nice as should be in a common header somewhere
  //get truth particle associated with a track particle
  const xAOD::TruthParticle * getTruthPtr(const xAOD::TrackParticle & trackParticle){
    typedef ElementLink<xAOD::TruthParticleContainer> ElementTruthLink_t;
    const xAOD::TruthParticle * result(nullptr);
		//0. is there any truth?
		if (trackParticle.isAvailable<ElementTruthLink_t>("truthParticleLink")) {
			//1. ..then get link
			const ElementTruthLink_t ptruthContainer= trackParticle.auxdata<ElementTruthLink_t>("truthParticleLink" );
			if (ptruthContainer.isValid()){
				result= *ptruthContainer;
			}
		}
    return result;
  }
  
  //get truth/track matching probability
  float getMatchingProbability(const xAOD::TrackParticle & trackParticle){
  	float result(std::numeric_limits<float>::quiet_NaN());
  	if (trackParticle.isAvailable<float>("truthMatchProbability")){
  		result = trackParticle.auxdata<float>("truthMatchProbability" );
  	}
  	return result;
  }
  
// this has no business here - should be in a common header.  Soon to be replaced w/ TrackTruthSelectionTool
// also not a nice way of configuring the eta cut..
  bool isPrimary( const xAOD::TruthParticle* tp, float max_eta )
  {
    /*Logically, the way to make this configurable between Forward/Backward/Min_Bias tracking is
      with an appropriate if-statement setup.  For simplicity, place the common conditions, such as 
      eta or Pt cuts, in front of the if-block, to minimize repetition.    
    */

    //old: how it was originally set-up
    //new: how I set it up.  In principle we can define a whole set of these to cover all the bases (Min_Bias, Backtracking, etc.)
    
    string Version = "old";

    if(Version == "new"){
      int minPt = 400;
      int maxBarcode = 200e3;
      int pixelrange = 110;
      
      if((tp->pt() < minPt) || (fabs(tp->eta()) > max_eta)) return false; //Particle in fiducial region?
      if(tp->status() != 1) return false;                                 //Particle is stable?
      if(tp->isNeutral()) return false;                                   //Particle is not neutral?
      if((tp->barcode() == 0) || (tp->barcode() >= maxBarcode)) return false;  //Particle has associated Truth & is not a designated secondary?
      if((!tp->hasProdVtx()) || (tp->prodVtx()->perp() > pixelrange)) return false;  //Particle has a vertex within the acceptable radius?

    }else if(Version == "old"){
      if( tp->status() != 1 )  //Check to see if it's a stable particle
	return false;
      //Note this const cast will not be required in the future -- there is a bug in the truth edm
      if( const_cast<xAOD::TruthParticle*>(tp)->isNeutral() )        //Clearly for tracking we don't care about Neutrals
	return false;

      if (tp->barcode() == 0 || tp->barcode() >= 200e3)
	return false;
      
      if( tp->pt() < 400.   ||  fabs(tp->eta()) > max_eta ) // Check the particle is within acceptance
	return false;
     
      // make sure particle decays before the last pixel layer
      if( !tp->hasProdVtx() || tp->prodVtx()->perp() > 110)
        return false;
    }

    return true;
  }

  bool isPrimary( const xAOD::TrackParticle* tp, float max_eta )
  {
    const xAOD::TruthParticle* truth = getTruthPtr( *tp );
    if( !truth  ){
      return false;
    }
    return isPrimary( truth, max_eta );
  }
  
}//namespace


InDetRttPlots::InDetRttPlots(InDetPlotBase* pParent, const std::string & sDir):InDetPlotBase(pParent, sDir),
    m_ptPlot(this,"Tracks/SelectedGoodTracks"),
    m_basicPlot(this, "Tracks/SelectedGoodTracks"),
    m_PtEtaPlots(this,"Tracks/SelectedGoodTracks","TrackParticle"),
    m_IPPlots(this,"Tracks/SelectedGoodTracks"),
    m_TrackRecoInfoPlots(this,"Tracks/SelectedGoodTracks"),
    m_TrackTruthInfoPlots(this,"Truth"),
    m_nTracks(this,"Tracks/SelectedGoodTracks"), 
    m_resPlots(this,"Tracks/SelectedGoodTracks"),
    m_hitResidualPlot(this, "Tracks/SelectedGoodTracks"),
									       /*m_pullPlots(this, "Tracks/SelectedGoodTracks"),*/		
    m_fakePlots(this,"Tracks/SelectedFakeTracks"),
    m_hitsPlots(this,"Tracks/SelectedGoodTracks"),
    m_hitsMatchedTracksPlots(this,"Tracks/SelectedMatchedTracks"),
    m_hitsFakeTracksPlots(this,"Tracks/SelectedFakeTracks"),
    m_hitsDetailedPlots(this,"Tracks/SelectedGoodTracks"),                   
    m_effPlots(this, "Tracks/SelectedGoodTracks","Selected Good Tracks"),
    m_BadMatchRate(this, "Tracks/SelectedFakeTracks"),
    m_verticesPlots(this, "Vertices/AllPrimaryVertices"),
    m_vertexPlots(this,"Vertices/AllPrimaryVertices"),
    m_hardScatterVertexPlots(this,"Vertices/HardScatteringVertex"),
									  //m_DuplicateTruth(this, "Tracks/SelectedGoodTrack"),
    m_duplicatePlots(this,"Tracks/SelectedMatchedTracks"),

    m_trkInJetPlot(this,"Tracks/SelectedGoodJetTracks"),
    m_trkInJetPlot_highPt(this,"Tracks/SelectedGoodHighPtJetTracks"),
    m_trkInJetPtPlot(this,"Tracks/SelectedGoodJetTracks"),
    m_trkInJetPtEtaPlots(this,"Tracks/SelectedGoodJetTracks","TrackParticle"),
    m_trkInJetIPPlots(this,"Tracks/SelectedGoodJetTracks"),
    m_trkInJetTrackRecoInfoPlots(this,"Tracks/SelectedGoodJetTracks"),
    m_trkInJetHitsPlots(this,"Tracks/SelectedGoodJetTracks"),
    m_trkInJetHitsDetailedPlots(this,"Tracks/SelectedGoodJetTracks"),                   
    m_trkInJetFakePlots(this,"Tracks/SelectedFakeJetTracks"),
    m_trkInJetResPlots(this,"Tracks/SelectedGoodJetTracks"),
									  //m_trkInJetPullPlots(this, "Tracks/SelectedGoodJetTracks"),
    m_trkInJetHighPtResPlots(this,"Tracks/SelectedGoodJetHighPtTracks"),
									  //m_trkInJetHighPtPullPlots(this, "Tracks/SelectedGoodJetHighPtTracks"),
    m_trkInJetHitsFakeTracksPlots(this,"Tracks/SelectedFakeJetTracks"),
    m_trkInJetHitsMatchedTracksPlots(this,"Tracks/SelectedMatchedJetTracks"),
    m_trkInJetEffPlots(this, "Tracks/SelectedGoodJetTracks","Selected Good Tracks in Jets"),
    m_trkInJetTrackTruthInfoPlots(this,"TruthInJet")
{
  m_moreJetPlots = false; // changed with setter function

  //These settings are probably all redundant & can be removed from this script
  m_trackParticleTruthProbKey = "truthMatchProbability";
  m_truthProbThreshold = 0.8;
  m_truthProbLowThreshold = 0.5;
  m_truthPrimaryEtaCut = 2.5;

  if(m_moreJetPlots) {
    m_trkInJetResPlotsDr0010 = new InDetPerfPlot_res(this,"Tracks/SelectedGoodJetDr0010Tracks");
    m_trkInJetResPlotsDr1020 = new InDetPerfPlot_res(this,"Tracks/SelectedGoodJetDr1020Tracks");
    m_trkInJetResPlotsDr2030 = new InDetPerfPlot_res(this,"Tracks/SelectedGoodJetDr2030Tracks");
  }
  //m_trkInJetPlot_highPt.setJetPtRange( 450, 900 );
}


void 
InDetRttPlots::fill(const xAOD::TrackParticle& particle,const xAOD::TruthParticle& truthParticle){
  //fill measurement bias, resolution, and pull plots
  m_resPlots.fill(particle, truthParticle); 
  m_basicPlot.fill(truthParticle);
  //Not sure that the following hitsMatchedTracksPlots does anything...
  float barcode = truthParticle.barcode();
  if (barcode < 100000 && barcode != 0) { //Not sure why the barcode limit is 100k instead of 200k...
    m_hitsMatchedTracksPlots.fill(particle);
  }
}

void
InDetRttPlots::fillSingleMatch(const xAOD::TrackParticle & trackParticle){
  m_duplicatePlots.fillSingleMatch(trackParticle);
}

void
InDetRttPlots::fillTwoMatchDuplicate(Float_t prob1, Float_t prob2, const xAOD::TrackParticle & trackParticle,const xAOD::TrackParticle & particle, const xAOD::TruthParticle&  tp){
  m_duplicatePlots.fillTwoMatchDuplicate(prob1,prob2,trackParticle,particle,tp);
}




void 
InDetRttPlots::fill(const xAOD::TrackParticle& particle){
  m_hitResidualPlot.fill(particle); 
  //fill pt plots
  m_ptPlot.fill(particle);
  m_basicPlot.fill(particle);
  m_PtEtaPlots.fill(particle);
  m_IPPlots.fill(particle);
  m_TrackRecoInfoPlots.fill(particle);
  m_hitsPlots.fill(particle);
  m_hitsDetailedPlots.fill(particle); 
}

void 
InDetRttPlots::fillTruth(const xAOD::TruthParticle& truth){
  m_effPlots.fillDenominator(truth);

  //m_hitsDetailedPlots.fillDenom(truth);

}

void 
InDetRttPlots::fill(const xAOD::TruthParticle& truthParticle){
  //fill truth plots
  m_TrackTruthInfoPlots.fill(truthParticle);
  // this gets called for the truth particles associated to tracks - exactly what we need!
  m_effPlots.fillNumerator(truthParticle);
}

/*
void
InDetRttPlots::fillDupTrack(const xAOD::TrackParticle& track){
  //fill any duplicate plots requiring tracks only
  m_DuplicateTrack.fill(track);
}
*/

void InDetRttPlots::fillBMR_Denom(const xAOD::TrackParticle& trackParticle){
  //fill the plot requiring prob < 0.8
  m_BadMatchRate.fillDenominator(trackParticle);
}

void InDetRttPlots::fillBMR_Num(const xAOD::TrackParticle& trackParticle){                
  //fill the plot requiring prob < 0.8                  
  m_BadMatchRate.fillNumBMR(trackParticle);                                                                                                                     
}    

void InDetRttPlots::fillRF_Num(const xAOD::TrackParticle& trackParticle){
  //fill the plot with prob < 0.2
  m_BadMatchRate.fillNumRF(trackParticle);
}

void 
InDetRttPlots::fill(const xAOD::VertexContainer& vertexContainer){
  //fill vertex container general properties
  //m_verticesPlots.fill(vertexContainer); //if ever needed
  //fill vertex-specific properties, for all vertices and for hard-scattering vertex
  for (const auto& vtx : vertexContainer.stdcont()){
    if (vtx->vertexType() == xAOD::VxType::NoVtx) continue; //skip dummy vertex
    m_vertexPlots.fill(*vtx);
    if (vtx->vertexType() == xAOD::VxType::PriVtx) m_hardScatterVertexPlots.fill(*vtx);
  }
}

void 
InDetRttPlots::fill(const xAOD::VertexContainer& vertexContainer, const xAOD::EventInfo& ei){  
  m_verticesPlots.fill(vertexContainer, ei);
}

void 
InDetRttPlots::fillCounter(const unsigned int freq, const InDetPerfPlot_nTracks::CounterCategory  counter){
  m_nTracks.fill(freq,counter);
}

void 
InDetRttPlots::fillFakeRate(const xAOD::TrackParticle& particle, const bool match, const InDetPerfPlot_fakes::Category c){
  m_fakePlots.fill(particle,match, c);
}


void 
InDetRttPlots::fillJetPlot(const xAOD::TrackParticle& particle, const xAOD::Jet& jet){
    const float minProbEffLow(0.5); //if the probability of match is less than this, we call it a fake

    bool pass = m_trkInJetPlot.fill(particle,jet);    
    if(!pass) { return; }
    m_trkInJetPlot_highPt.fill(particle,jet);

    // the full suit of track plots
    m_trkInJetPtPlot.fill(particle);
    m_trkInJetPtEtaPlots.fill(particle);
    m_trkInJetIPPlots.fill(particle);
    m_trkInJetTrackRecoInfoPlots.fill(particle);
    m_trkInJetHitsPlots.fill(particle);
    m_trkInJetHitsDetailedPlots.fill(particle);    

    // the rest requires truth information
    // how can we check that the decorrators have been applied previously?
    const xAOD::TruthParticle * associatedTruth = getTruthPtr(particle); //get the associated truth      
    if (!associatedTruth){ return; }

    //Fake-Finder, will be made irrelevant by changes in Monitoring Tool
    float prob(1); // does this run on data?
    if (particle.isAvailable<float>(m_trackParticleTruthProbKey)) {
      prob = getMatchingProbability(particle);
    }
    if ( std::isnan(prob) ) { return; }
    const bool isFake=(prob<minProbEffLow);
    m_trkInJetFakePlots.fill(particle, isFake, InDetPerfPlot_fakes::ALL);

    // fill pull and resolution plots
    m_trkInJetResPlots.fill(particle, *associatedTruth); 
    //m_trkInJetPullPlots.fill(particle, *associatedTruth);
    if(particle.pt() > 10e3) { // 10 GeV
      m_trkInJetHighPtResPlots.fill(particle, *associatedTruth); 
      //m_trkInJetHighPtPullPlots.fill(particle, *associatedTruth);
    }
    if(m_moreJetPlots) {
      float dR( jet.p4().DeltaR( particle.p4() ) );
      if( dR < 0.1 ) { 
        m_trkInJetResPlotsDr0010->fill(particle, *associatedTruth); 
      } else if( dR < 0.2 ) {
        m_trkInJetResPlotsDr1020->fill(particle, *associatedTruth); 
      } else if( dR < 0.3 ) {
        m_trkInJetResPlotsDr2030->fill(particle, *associatedTruth); 
      }
    } // m_moreJetPlots

    //Essentially a Fake-Finder, will be eliminated by changes in Monitoring Tool
    float barcode = associatedTruth->barcode();
    if (prob < m_truthProbLowThreshold) {
      m_trkInJetHitsFakeTracksPlots.fill(particle);
    } else if (barcode < 100000 && barcode != 0) {
      m_trkInJetHitsMatchedTracksPlots.fill(particle);
    }
    m_trkInJetTrackTruthInfoPlots.fill(*associatedTruth);

    // if does not have truth particle - fails our primary definition
    //This if-statement will be made irrelevant by TTST
    if( !isPrimary( &particle, m_truthPrimaryEtaCut  ) ) { return; }
    //m_trkInJetEffPlots.fillNumerator(*associatedTruth);       // pT and eta efficiencies
    m_trkInJetPlot.BookEffReco(*associatedTruth,jet);         // fill hists with truth info!
    m_trkInJetPlot_highPt.BookEffReco(*associatedTruth,jet);  // fill hists with truth info!
}


bool
InDetRttPlots::filltrkInJetPlot(const xAOD::TrackParticle& particle, const xAOD::Jet& jet){
  //const float minProbEffLow(0.5); //if the probability of match is less than this, we call it a fake
  bool pass = m_trkInJetPlot.fill(particle, jet);
  m_trkInJetPlot_highPt.fill(particle, jet);
  return pass;
}

void
InDetRttPlots::fillSimpleJetPlots(const xAOD::TrackParticle& particle){
  //The mysterious "pass" goes here...      
  //bool pass = m_trkInJetPlot.fill(particle, jet);
  //if(!pass) return;
  //m_trkInJetPlot_highPt.fill(particle, jet);

  // the full suit of track plots                                                                                              
  m_trkInJetPtPlot.fill(particle);
  m_trkInJetPtEtaPlots.fill(particle);
  m_trkInJetIPPlots.fill(particle);
  m_trkInJetTrackRecoInfoPlots.fill(particle);
  m_trkInJetHitsPlots.fill(particle);
  m_trkInJetHitsDetailedPlots.fill(particle);

  float minProbEffLow(0.5);
  float prob = getMatchingProbability(particle);

  if ( std::isnan(prob) ) { return; }
  const bool isFake=(prob<minProbEffLow);
  m_trkInJetFakePlots.fill(particle, isFake, InDetPerfPlot_fakes::ALL);

  const xAOD::TruthParticle * associatedTruth = getTruthPtr(particle);
  float barcode = associatedTruth->barcode();
  if (prob < m_truthProbLowThreshold) {
    m_trkInJetHitsFakeTracksPlots.fill(particle);
  } else if (barcode < 100000 && barcode != 0) {
    m_trkInJetHitsMatchedTracksPlots.fill(particle);
  }

}




void
InDetRttPlots::fillJetResPlots(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truth, const xAOD::Jet& jet){
  //The mysterious "pass" goes here...
  //bool pass = m_trkInJetPlot.fill(particle, jet);
  //  if(!pass) return;
  //m_trkInJetPlot_highPt.fill(particle, jet);
  
  // fill pull and resolution plots                                                               
  m_trkInJetResPlots.fill(particle, truth);
  //m_trkInJetPullPlots.fill(particle, truth);
  if(particle.pt() > 10e3) { // 10 GeV       
    m_trkInJetHighPtResPlots.fill(particle, truth);
    //m_trkInJetHighPtPullPlots.fill(particle, truth);
  }
  if(m_moreJetPlots) {
    float dR( jet.p4().DeltaR( particle.p4() ) );
    if( dR < 0.1 ) {
      m_trkInJetResPlotsDr0010->fill(particle, truth);
    } else if( dR < 0.2 ) {
      m_trkInJetResPlotsDr1020->fill(particle, truth);
    } else if( dR < 0.3 ) {
      m_trkInJetResPlotsDr2030->fill(particle, truth);
    }
  } // m_moreJetPlots  
}


void 
InDetRttPlots::fillJetFakes(const xAOD::TrackParticle& particle){
  const bool isFake = true;
  m_trkInJetFakePlots.fill(particle, isFake, InDetPerfPlot_fakes::ALL);
  m_trkInJetHitsFakeTracksPlots.fill(particle);                        //How is this one different from the above one?!
}


void
InDetRttPlots::fillJetEffPlots(const xAOD::TruthParticle& truth, const xAOD::Jet& jet){
  //m_trkInJetEffPlots.fillNumerator(truth);       // pT and eta efficiencies                
  m_trkInJetPlot.BookEffReco(truth,jet);         // fill hists with truth info!           
  m_trkInJetPlot_highPt.BookEffReco(truth,jet);  // fill hists with truth info! 
}


void
InDetRttPlots::fillJetTrkTruth(const xAOD::TruthParticle& truth, const xAOD::Jet& jet){
  //isPrimary will be replaced by TTST implementation in Monitoring Tool
  //if( !isPrimary( &truth, m_truthPrimaryEtaCut  ) ) { return; }
  m_trkInJetPlot.BookEffTruth(truth,jet);
  m_trkInJetPlot_highPt.BookEffTruth(truth,jet);
  // m_trkInJetEffPlots.fillDenominator(truth);
}

void 
InDetRttPlots::fillJetPlotCounter(const xAOD::Jet& jet){
  m_trkInJetPlot.fillCounter(jet);    
  m_trkInJetPlot_highPt.fillCounter(jet);    
}

void 
InDetRttPlots::fillJetTrkTruthCounter(const xAOD::Jet& jet){
    m_trkInJetPlot.fillEff(jet);
    m_trkInJetPlot_highPt.fillEff(jet);
}

void InDetRttPlots::SetPrimaryEtaCut( float eta/*, float pt*/ ) { 
  m_truthPrimaryEtaCut = eta; 
  m_resPlots.SetEtaBinning( int(2*eta/0.25), -1*eta, eta ); //Isn't this hard-coded into res.cxx anyway?

  //  m_truthPrimaryPtCut = pt;
  //m_resPlots.SetPtBinning( 20, pt, 25*pt );
}

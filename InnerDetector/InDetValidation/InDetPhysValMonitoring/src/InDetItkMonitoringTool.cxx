/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetItkMonitoringTool.cxx
 * @author shaun roe
**/

#include "InDetPhysValMonitoring/InDetItkMonitoringTool.h"

#include <vector>
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODEventInfo/EventInfo.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "InDetRttPlots.h"
#include "InDetPerfPlot_nTracks.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODJet/JetContainer.h" 
#include <limits>
#include <cmath> //to get std::isnan()



namespace { //utility functions used here
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
  
  bool passJetCuts( const xAOD::Jet& jet ) {
		float etaMin = -2.5;
		float etaMax = 2.5;
		float jetPtMin = 100;  // in GeV
		float jetPtMax = 1000; // in GeV
		float jetPt = jet.pt()/1e3; // GeV
		if( jetPt < jetPtMin ) { return false; }
		if( jetPt > jetPtMax ) { return false; }
		float eta = jet.eta();
		if( eta < etaMin ) { return false; }
		if( eta > etaMax ) { return false; }

		return true;
	}

}//namespace

///Parametrized constructor
InDetItkMonitoringTool::InDetItkMonitoringTool(const std::string & type, const std::string & name, const IInterface* parent):
    ManagedMonitorToolBase(type, name, parent),
    m_useTrackSelection(false),
    m_onlyInsideOutTracks(false),
    m_fillTIDEPlots(true),
    m_fillExtraTIDEPlots(false)
{
  declareProperty("DirName",    m_dirName="IDPerformanceMon/");
  declareProperty("TrackParticleContainerName", m_trkParticleName="InDetTrackParticles"); 
  declareProperty("TruthParticleContainerName", m_truthParticleName="TruthParticles"); 
  declareProperty("VertexContainerName", m_vertexContainerName="PrimaryVertices");
  declareProperty("EventInfoContainerName", m_eventInfoContainerName="EventInfo");
  declareProperty("useTrackSelection"       , m_useTrackSelection); //redundant?
  declareProperty("onlyInsideOutTracks"     , m_onlyInsideOutTracks);
  declareProperty("TrackSelectionTool"      , m_trackSelectionTool);      
  declareProperty("FillTrackInJetPlots"     , m_fillTIDEPlots);
  declareProperty("FillExtraTrackInJetPlots"     , m_fillExtraTIDEPlots);
  declareProperty("jetContainerName", m_jetContainerName="AntiKt4TruthJets");
  declareProperty("maxTrkJetDR", m_maxTrkJetDR=0.4);
  declareProperty("truthMinPt", m_truthMinPt=400);
  declareProperty("truthMaxPt", m_truthMaxPt=-1);
  declareProperty("truthMaxEta", m_truthMaxEta=2.5);
  declareProperty("truthPdgId", m_truthPdgId=-1);
}

InDetItkMonitoringTool::~InDetItkMonitoringTool(){
}

StatusCode 
InDetItkMonitoringTool::initialize(){
    ATH_MSG_DEBUG ("Initializing " << name() << "...");
    m_monPlots = std::unique_ptr<InDetRttPlots> (new InDetRttPlots(0,m_dirName));
    m_monPlots->SetFillExtraTIDEPlots( m_fillExtraTIDEPlots );
    ATH_MSG_INFO ("Eta cut " << m_truthMaxEta );
    m_monPlots->SetPrimaryEtaCut(m_truthMaxEta);
    ATH_CHECK(ManagedMonitorToolBase::initialize());
    //Get the track selector tool only if m_useTrackSelection is true
    if (m_useTrackSelection) ATH_CHECK(m_trackSelectionTool.retrieve());
    
    return StatusCode::SUCCESS;
}

StatusCode 
InDetItkMonitoringTool::fillHistograms(){
    ATH_MSG_DEBUG ("Filling hists " << name() << "...");
    //retrieve trackParticle container
    auto ptracks = getContainer<xAOD::TrackParticleContainer>(m_trkParticleName);
    if ((!ptracks) ) return StatusCode::FAILURE;
    //retrieve truthParticle container
    auto ptruth = (!m_truthParticleName.empty() ? getContainer<xAOD::TruthParticleContainer>(m_truthParticleName) : nullptr);
    // if ((!ptruth) ) return StatusCode::FAILURE;
    //
    //Loop over reconstructed tracks
    const unsigned int nTracks(ptracks->size());
    const unsigned int nTruth((ptruth ? ptruth->size() : 0 ) );
    unsigned int nSelectedTracks(0), num_truthmatch_match	(0), num_truthmatch_sel (0);
    const float minProbEffLow(0.5); //if the probability of match is less than this, we call it a fake
    for (const auto & thisTrack: *ptracks){
      //Select only good tracks, if selection is turned on
      if (m_useTrackSelection ) {
        // 0 means z0, d0 cut is wrt beam spot - put a PV in to change this
        if( ! (m_trackSelectionTool->accept(*thisTrack, 0)) ) { continue; }
      }
      ++nSelectedTracks; //increment number of selected tracks
      const xAOD::TruthParticle * associatedTruth = getTruthPtr(*thisTrack); //get the associated truth      
      //
      m_monPlots->fill(*thisTrack);//Make all the plots requiring only trackParticle (kinematics, hits etc)
      if (associatedTruth) {
        ++num_truthmatch_match;
        float prob=getMatchingProbability(*thisTrack);
        if (not std::isnan(prob)) {
        	const bool isFake=(prob<minProbEffLow);
        	m_monPlots->fillFakeRate(*thisTrack, isFake);
        }
        if ( truthSelector (*associatedTruth)) {
        ++num_truthmatch_sel;
      ATH_MSG_DEBUG("Filling efficiency plots");
          //should explicitly decide whether to continue or not in the case of unsuccessful addition of information...
          m_monPlots->fill(*thisTrack, *associatedTruth); //Make all the plots requiring both truth and track 
                                                          // pulls, resolutions, but also hits plots for matched/unmatched split by prob
          // following line causes crash because there is no truthType or truthOrigin decoration
          m_monPlots->fill(*associatedTruth);//Make all the plots requiring  truth only (if any) (numerator for efficiency, truth info plots)
        }
      }
    }
    if (num_truthmatch_match == 0){
      ATH_MSG_DEBUG("NO TRACKS had associated truth.");
    } else {
      ATH_MSG_DEBUG(num_truthmatch_match <<" tracks out of "<<nTracks<<" had associated truth.");
      ATH_MSG_DEBUG(num_truthmatch_sel <<" tracks out of "<<nTracks<<" had associated truth passing truth selection.");
    }
    m_monPlots->fillCounter(nSelectedTracks, InDetPerfPlot_nTracks::SELECTED);
    m_monPlots->fillCounter(nTracks, InDetPerfPlot_nTracks::ALL);
    m_monPlots->fillCounter(nTruth, InDetPerfPlot_nTracks::TRUTH);
    m_monPlots->fillCounter(num_truthmatch_match, InDetPerfPlot_nTracks::TRUTH_MATCHED);   

    ATH_MSG_DEBUG("Getting Truth Container");
    std::string m_truthContainerName = "TruthParticles";
    const xAOD::TruthParticleContainer* truthParticles = getContainer<xAOD::TruthParticleContainer>(m_truthContainerName);
    for (const auto thisTruth: *truthParticles){
      if (truthSelector (*thisTruth)) m_monPlots->fillTruth(*thisTruth); // Deonimator for efficiency plot
    } // loop over truth

    ATH_MSG_DEBUG("Filling vertex plots");
    const xAOD::VertexContainer* pvertex = getContainer<xAOD::VertexContainer>(m_vertexContainerName);
    if (pvertex) {
      m_monPlots->fill(*pvertex);
    } else {
      ATH_MSG_WARNING("Cannot open " << m_vertexContainerName << " vertex container. Skipping vertexing plots.");
    }
		ATH_MSG_DEBUG("Filling vertex/event info monitoring plots");
    const xAOD::EventInfo* pei = getContainer<xAOD::EventInfo>(m_eventInfoContainerName);
    if (pei) {
      m_monPlots->fill(*pvertex, *pei);
    } else {
      ATH_MSG_WARNING("Cannot open " << m_eventInfoContainerName << " EventInfo container. Skipping vertexing plots using EventInfo.");      
    }

    // do all jet stuff here - easier to loop over jets first
    // some duplication of code below - "cleanest" way of adding this information
    // and only calling this tool 1x
    // G. Facini
    if(m_fillTIDEPlots) { 
      ATH_MSG_DEBUG("Getting Jet Container");
      const xAOD::JetContainer* jets = getContainer<xAOD::JetContainer>(m_jetContainerName);
      ATH_MSG_DEBUG("Getting Truth Container");
      std::string m_truthContainerName = "TruthParticles";
      const xAOD::TruthParticleContainer* truthParticles = getContainer<xAOD::TruthParticleContainer>(m_truthContainerName);
      if( !jets || !truthParticles) {
        ATH_MSG_WARNING("Cannot open " << m_jetContainerName << " jet container. Skipping jet plots.");
        ATH_MSG_WARNING("Cannot open " << m_truthContainerName << " jet container. Skipping jet plots.");
      } else {
        for (const auto & thisJet: *jets){
          if (not passJetCuts(*thisJet))  continue; 
          for (const auto thisTrack: *ptracks){
            if (m_useTrackSelection ) {
              // 0 means z0, d0 cut is wrt beam spot - put a PV in to change this
              if( ! (m_trackSelectionTool->accept(*thisTrack, 0)) ) { continue; }
            }
            if( thisJet->p4().DeltaR( thisTrack->p4() ) > m_maxTrkJetDR ) { continue; }
            m_monPlots->fillJetPlot(*thisTrack,*thisJet);
          } // loop over tracks
          // fill in things like sum jet pT in dR bins - need all tracks in the jet first
          m_monPlots->fillJetPlotCounter(*thisJet);
          for (const auto & thisTruth: *truthParticles){
            // for primary tracks want an efficiency as a function of track jet dR
            if (not truthSelector(*thisTruth)) continue;
            m_monPlots->fillJetTrkTruth(*thisTruth,*thisJet); // Track in jet plots
          } // loop over truth
          m_monPlots->fillJetTrkTruthCounter(*thisJet);
        } // loop over jets
      } // if have collections needed
    } // if TIDE


    return StatusCode::SUCCESS;
}


StatusCode
InDetItkMonitoringTool::bookHistograms(){
  ATH_MSG_INFO ("Booking hists " << name() << "...");
  m_monPlots->setDetailLevel(100); //DEBUG, enable expert histograms
  m_monPlots->initialize();
  std::vector<HistData> hists = m_monPlots->retrieveBookedHistograms();
  for (auto hist : hists){
    ATH_CHECK(regHist(hist.first,hist.second,all)); //??
  }
  return StatusCode::SUCCESS;
}


StatusCode 
InDetItkMonitoringTool::procHistograms() {
    ATH_MSG_INFO ("Finalising hists " << name() << "...");
    if (endOfRun){
      m_monPlots->finalize();
    }
    return StatusCode::SUCCESS;
  }
  
  
bool InDetItkMonitoringTool::truthSelector(const xAOD::TruthParticle &truth){
  const bool truthMinPtCut = ( truth.pt() > m_truthMinPt );
  const bool truthMaxPtCut ( (m_truthMaxPt<0) or ( truth.pt() < m_truthMaxPt ) );
  const bool stabilityCut = (truth.status() == 1);
  const bool noNeutrals = not (truth.isNeutral());
  const bool pdgCut = (m_truthPdgId<0 or fabs(truth.pdgId()==m_truthPdgId));
  return (stabilityCut and noNeutrals and truthMinPtCut and truthMaxPtCut and pdgCut);
}

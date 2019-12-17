/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPhysValMonitoringTool.cxx
 * @author shaun roe
 **/

#include "InDetPhysValMonitoring/InDetPhysValMonitoringTool.h"
#include "InDetPhysHitDecoratorAlg.h"
#include "InDetRttPlots.h"
#include "InDetPerfPlot_nTracks.h"
#include "AthTruthSelectionTool.h"
#include "CachedGetAssocTruth.h"

#include "safeDecorator.h"
// #include "TrackTruthLookup.h"
//
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackStateValidationContainer.h"
#include "xAODTracking/TrackStateValidation.h"
#include "xAODTracking/TrackStateValidationContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthPileupEvent.h"
#include "xAODTruth/TruthPileupEventAuxContainer.h"

#include "TrkTrack/TrackCollection.h"
//
#include <algorithm>
#include <limits>
#include <cmath> // to get std::isnan(), std::abs etc.
// #include <functional> // to get std::plus
#include <utility>
#include <cstdlib> // to getenv
#include <vector>



namespace { // utility functions used here
  // sort method for prospects
  bool
  sortProspects(std::pair<float, const xAOD::TrackParticle*> pair1,
                std::pair<float, const xAOD::TrackParticle*> pair2) {
    return(pair1.first < pair2.first);
  }

  // get truth/track matching probability
  float
  getMatchingProbability(const xAOD::TrackParticle& trackParticle) {
    float result(std::numeric_limits<float>::quiet_NaN());

    if (trackParticle.isAvailable<float>("truthMatchProbability")) {
      result = trackParticle.auxdata<float>("truthMatchProbability");
    }
    return result;
  }

  bool
  isInsideOut(const xAOD::TrackParticle& track) {
    std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo>  patternInfo = track.patternRecoInfo();
    return patternInfo.test(0);
  }

  template <class T>
  inline float
  safelyGetEta(const T& pTrk, const float safePtThreshold = 0.1) {
    return (pTrk->pt() > safePtThreshold) ? (pTrk->eta()) : std::nan("");
  }

  constexpr float
  operator "" _GeV (unsigned long long energy) {
    return energy * 0.001;
  }

  constexpr float twoPi = 2 * M_PI;

  // general utility function to check value is in range
  template <class T>
  inline bool
  inRange(const T& value, const T& minVal, const T& maxVal) {
    return not ((value < minVal)or(value > maxVal));
  }

  template<class T>
  inline bool
  inRange(const T& value, const T& absoluteMax) {
    return not (std::abs(value) > absoluteMax);
  }

  // Cuts on various objects
  bool
  passJetCuts(const xAOD::Jet& jet) {
    const float absEtaMax = 2.5;
    const float jetPtMin = 100;  // in GeV
    const float jetPtMax = 1000; // in GeV
    const float jetPt = jet.pt() * 1_GeV; // GeV
    const float jetEta = jet.eta();

    return inRange(jetPt, jetPtMin, jetPtMax) and inRange(jetEta, absEtaMax);
  }

  // general utility function to return bin index given a value and the upper endpoints of each bin
  template <class T>
  unsigned int
  binIndex(const T& val, const std::vector<T>& partitions) {// signature should allow other containers
    unsigned int i(0);
    bool nf = true;

    while (nf and i != partitions.size()) {
      nf = (val > partitions[i++]);
    }
    return nf ? i : i - 1;
  }

  bool
  acceptVertex(const xAOD::Vertex* vtx) {
    return(vtx->vertexType() != xAOD::VxType::NoVtx);
  }

  bool
  acceptTruthVertex(const xAOD::TruthVertex* vtx) {
    const float x(vtx->x()), y(vtx->y()), z(vtx->z());
    const float vrtR2 = (x * x + y * y); // radial distance squared

    return inRange(z, 500.f) and not (vrtR2 > 1); // units?
  }

  const std::vector<float> ETA_PARTITIONS = {
    2.7, 3.5, std::numeric_limits<float>::infinity()
  };
}// namespace

///Parametrized constructor
InDetPhysValMonitoringTool::InDetPhysValMonitoringTool(const std::string& type, const std::string& name,
                                                       const IInterface* parent) :
  ManagedMonitorToolBase(type, name, parent),
  m_useTrackSelection(false),
  m_onlyInsideOutTracks(false),
  m_TrkSelectPV(false),
  m_trackSelectionTool("InDet::InDetTrackSelectionTool/TrackSelectionTool"),
  m_truthSelectionTool("AthTruthSelectionTool", this),
  m_prospectsMatched(6, 0),
  m_twoMatchedEProb(0),
  m_threeMatchedEProb(0),
  m_fourMatchedEProb(0),
  m_truthCounter(0),
  m_fillTIDEPlots(false),
  m_fillExtraTIDEPlots(false) {
  declareProperty("useTrackSelection", m_useTrackSelection);
  declareProperty("useTrkSelectPV", m_TrkSelectPV);
  declareProperty("onlyInsideOutTracks", m_onlyInsideOutTracks);
  declareProperty("TrackSelectionTool", m_trackSelectionTool);
  declareProperty("TruthSelectionTool", m_truthSelectionTool);
  declareProperty("FillTrackInJetPlots", m_fillTIDEPlots);
  declareProperty("FillExtraTrackInJetPlots", m_fillExtraTIDEPlots);
  declareProperty("maxTrkJetDR", m_maxTrkJetDR = 0.4);
  declareProperty("DirName", m_dirName = "IDPerformanceMon/");
  declareProperty("SubFolder", m_folder);
  declareProperty("PileupSwitch", m_pileupSwitch = "All");
}

InDetPhysValMonitoringTool::~InDetPhysValMonitoringTool() {
}

StatusCode
InDetPhysValMonitoringTool::initialize() {
  ATH_MSG_DEBUG("Initializing " << name() << "...");
  ATH_CHECK(ManagedMonitorToolBase::initialize());
  // Get the track selector tool only if m_useTrackSelection is true;
  // first check for consistency i.e. that there is a trackSelectionTool if you ask
  // for trackSelection
  ATH_CHECK(m_trackSelectionTool.retrieve(EnableTool {m_useTrackSelection} ));
  ATH_CHECK(m_truthSelectionTool.retrieve(EnableTool {not m_truthParticleName.key().empty()} ));
  if (m_truthSelectionTool.get() ) {
    m_truthCutFlow = CutFlow(m_truthSelectionTool->nCuts());
  }
  m_monPlots = std::make_unique<InDetRttPlots> (nullptr, m_dirName + m_folder);
  m_monPlots->SetFillExtraTIDEPlots(m_fillExtraTIDEPlots);

  ATH_CHECK( m_trkParticleName.initialize() );
  ATH_CHECK( m_truthParticleName.initialize(m_pileupSwitch == "All" and not m_truthParticleName.key().empty() ) );
  ATH_CHECK( m_vertexContainerName.initialize() );
  ATH_CHECK( m_truthVertexContainerName.initialize( not m_truthVertexContainerName.key().empty() ) );
  ATH_CHECK( m_eventInfoContainerName.initialize() );

  ATH_CHECK( m_truthEventKey.initialize( m_pileupSwitch == "HardScatter" and not m_truthEventKey.key().empty()) );
  ATH_CHECK( m_truthPileUpeEventKey.initialize( m_pileupSwitch == "PileUp" and not m_truthPileUpeEventKey.key().empty() ) );
  ATH_CHECK( m_jetContainerName.initialize( not m_jetContainerName.key().empty()) );

  std::vector<std::string> required_float_track_decorations {"d0","hitResiduals_residualLocX","d0err"};
  std::vector<std::string> required_int_track_decorations {};
  std::vector<std::string> required_float_truth_decorations {"d0"};
  std::vector<std::string> required_int_truth_decorations {};

  std::string empty_prefix;
  IDPVM::addReadDecoratorHandleKeys(*this, m_trkParticleName, empty_prefix, required_float_track_decorations, m_floatTrkDecor);
  IDPVM::addReadDecoratorHandleKeys(*this, m_trkParticleName, empty_prefix, required_int_truth_decorations,   m_intTrkDecor);
  if (!m_truthParticleName.key().empty()) {
    IDPVM::addReadDecoratorHandleKeys(*this, m_truthParticleName, empty_prefix, required_float_truth_decorations, m_floatTruthDecor);
    IDPVM::addReadDecoratorHandleKeys(*this, m_truthParticleName, empty_prefix, required_int_truth_decorations,   m_intTruthDecor);
  }
  return StatusCode::SUCCESS;
}

StatusCode
InDetPhysValMonitoringTool::fillHistograms() {
  ATH_MSG_DEBUG("Filling hists " << name() << "...");
  // function object could be used to retrieve truth: IDPVM::CachedGetAssocTruth getTruth;
  const char* debugBacktracking = std::getenv("BACKTRACKDEBUG");

  // retrieve trackParticle container
  SG::ReadHandle<xAOD::TrackParticleContainer> ptracks(m_trkParticleName);
  if (not ptracks.isValid()) {
    return StatusCode::FAILURE;
  }
  if(debugBacktracking){
    std::cout<<"Rey: Start of new event \n";
    std::cout<<"Finn: Number of particles in container: "<<ptracks->size()<<"\n";
  }
  //
  // retrieve truthParticle container
  std::vector<const xAOD::TruthParticle*> truthParticlesVec = getTruthParticles();
  // IDPVM::TrackTruthLookup getAsTruth(ptracks, &truthParticlesVec); //caches everything upon construction
  IDPVM::CachedGetAssocTruth getAsTruth; // only cache one way, track->truth, not truth->tracks
  //
  bool incFake = false;
  int nMuEvents = 0;
  SG::ReadHandle<xAOD::TruthPileupEventContainer> truthPileupEventContainer;
  if (incFake) {
    ATH_MSG_VERBOSE("getting TruthPileupEvents container");
    truthPileupEventContainer=SG::ReadHandle<xAOD::TruthPileupEventContainer> (m_truthPileUpeEventKey);
    nMuEvents = truthPileupEventContainer.isValid() ?  static_cast<int>( truthPileupEventContainer->size() ) : 0;
  }
  ATH_MSG_DEBUG("Filling vertex plots");
  SG::ReadHandle<xAOD::VertexContainer>  pvertex(m_vertexContainerName);
  const xAOD::Vertex* pvtx = nullptr;

  if (pvertex.isValid() and not pvertex->empty()) {
    ATH_MSG_DEBUG("Number of vertices retrieved for this event " << pvertex->size());
    const auto& stdVertexContainer = pvertex->stdcont();
    // find last non-dummy vertex; note *usually* there is only one (or maybe zero)
    auto findVtx = std::find_if(stdVertexContainer.rbegin(), stdVertexContainer.rend(), acceptVertex);
    pvtx = (findVtx == stdVertexContainer.rend()) ? nullptr : *findVtx;
    m_monPlots->fill(*pvertex);
  } else {
    ATH_MSG_WARNING("Skipping vertexing plots.");
  }
  ATH_MSG_DEBUG("Filling vertex/event info monitoring plots");
  SG::ReadHandle<xAOD::EventInfo> pei;
  if (not m_eventInfoContainerName.key().empty()) {
    pei=SG::ReadHandle<xAOD::EventInfo>(m_eventInfoContainerName);
  }
  if (pei.isValid() and pvertex.isValid()) {
    m_monPlots->fill(*pvertex, *pei);
  } else {
    ATH_MSG_WARNING("Skipping vertexing plots using EventInfo.");
  }

  // get truth vertex container name - m_truthVertexContainerName
  SG::ReadHandle<xAOD::TruthVertexContainer> truthVrt;
  const xAOD::TruthVertex* truthVertex = 0;
  if (not m_truthVertexContainerName.key().empty()) {
    truthVrt = SG::ReadHandle<xAOD::TruthVertexContainer>( m_truthVertexContainerName );
    if (truthVrt.isValid()) {
      if (!m_TrkSelectPV) {
        ATH_MSG_VERBOSE("size of TruthVertex container = " << truthVrt->size());
      }
      const auto& stdVertexContainer = truthVrt->stdcont();
      auto findVtx = std::find_if(stdVertexContainer.rbegin(), stdVertexContainer.rend(), acceptTruthVertex);
      truthVertex = (findVtx == stdVertexContainer.rend()) ? nullptr : *findVtx;
    } else {
      ATH_MSG_WARNING("Cannot open " << m_truthVertexContainerName.key() << " truth vertex container");
    }
    if (not truthVertex) ATH_MSG_INFO ("Truth vertex did not pass cuts");
  }
  unsigned int num_truth_selected(0), nSelectedTracks(0), num_truthmatch_match(0);
  // the truth matching probability must not be <= 0., otherwise the tool will seg fault in case of missing truth (e.g.
  // data):
  const float minProbEffLow(0.50); // if the probability of a match is less than this, we call it a fake
  //    const float minProbEffHigh(0.80); //if the probability of a match is higher than this, it either feeds the NUM
  // or is a duplicate
  // VJ - Mar 14, 2016 - even for effs, use 0.51
  const float minProbEffHigh(0.5); // if the probability of a match is higher than this, it either feeds the NUM or is
                                   // a duplicate

  // check if we are doing track selection relative to PV (or the default, which is the Beam Spot)
  if (!m_TrkSelectPV) {
    pvtx = nullptr;
  }

  bool fillVtx = true; // fill PV plots in fillSpectrum only once

  // Main track loop, filling Track-only, Track 'n' Truth with good matching probability (meas, res, & pull), and Fakes
  std::vector<int> incTrkNum = {
    0, 0, 0
  };
  CutFlow tmp_truth_cutflow( m_truthSelectionTool.get() ?  m_truthSelectionTool->nCuts() : 0 );

  // dummy variables
  int base(0), hasTruth(0), hashighprob(0), passtruthsel(0);

  for (const auto& thisTrack: *ptracks) {
    m_monPlots->fillSpectrum(*thisTrack); // This one needs prob anyway, why not rearrange & eliminate
                                          // getMatchingProbability from RttPlots? 5-17-16
    const xAOD::TruthParticle* associatedTruth = getAsTruth.getTruth(thisTrack);
    float prob = getMatchingProbability(*thisTrack);
    if (!m_TrkSelectPV && truthVertex) {
      m_monPlots->fillSpectrum(*thisTrack, *truthVertex);
    }
    if (m_TrkSelectPV && pvtx) {
      m_monPlots->fillSpectrum(*thisTrack, *pvtx, fillVtx);
    }
    fillVtx = false;

    if (m_useTrackSelection) {
      if (!(m_trackSelectionTool->accept(*thisTrack, pvtx))) {
        continue;
      }
    }
    if (m_onlyInsideOutTracks and(not isInsideOut(*thisTrack))) {
      continue; // not an inside-out track
    }
    ++nSelectedTracks;                                                    // increment number of selected tracks
    m_monPlots->fill(*thisTrack);                                         // Make all the plots requiring only
                                                                          // trackParticle
    // const float absTrackEta = (thisTrack->pt() >1e-7) ? std::abs(thisTrack->eta()) : std::nan("");
    const float absTrackEta = std::abs(safelyGetEta(thisTrack));
    const unsigned int idx = binIndex(absTrackEta, ETA_PARTITIONS);
    if (idx < incTrkNum.size()) {
      ++incTrkNum[idx];
    }

    // This is where the BMR, Fake, and Really Fake fillers need to go.
    float BMR_w(0), RF_w(0); // weights for filling the Bad Match & Fake Rate plots
    float Prim_w(0), Sec_w(0), Unlinked_w(0);  // weights for the fake plots

    if (associatedTruth) m_monPlots->track_vs_truth(*thisTrack, *associatedTruth, prob);

    if (prob < minProbEffHigh) {
      BMR_w = 1;
    }
    if (prob < minProbEffLow) {
      RF_w = 1;
    }
    m_monPlots->fillBMR(*thisTrack, BMR_w);
    m_monPlots->fillRF(*thisTrack, RF_w);

    if (!associatedTruth) {
      m_monPlots->fillSpectrumUnlinked2(*thisTrack);
      Unlinked_w = 1; // Unlinked, set weight to 1
    }
    base += 1;
    if (associatedTruth) {
      hasTruth += 1;
      if (not std::isnan(prob)) {
        // Fixing double counting of fake rates --> fill fake rates only once within track loop
        const bool isFake = (prob < minProbEffLow);
        m_monPlots->fillFakeRate(*thisTrack, isFake);
      }
      if (prob < minProbEffLow) {
        if ((associatedTruth->barcode() < 200e3)and(associatedTruth->barcode() != 0)) {
          Prim_w = 1; // Fake Primary, set weight to 1
        }
        if (associatedTruth->barcode() >= 200e3) {
          Sec_w = 1;                                         // Fake Secondary, set weight to 1
        }
      }
      if (prob > minProbEffLow) {
        hashighprob += 1;
      }
      // if there is associated truth also a truth selection tool was retrieved.
      IAthSelectionTool::CutResult passed( m_truthSelectionTool ?  m_truthSelectionTool->accept(associatedTruth) : IAthSelectionTool::CutResult(0) );
      if (m_truthSelectionTool) {
        tmp_truth_cutflow.update( passed.missingCuts() );
        passtruthsel += (bool) passed;
      }
      if ((prob > minProbEffLow) and passed) {
        m_monPlots->fill(*thisTrack, *associatedTruth); // Make all plots requiring both truth & track (meas, res, &
                                                        // pull)
      }
    }

    m_monPlots->fillLinkedandUnlinked(*thisTrack, Prim_w, Sec_w, Unlinked_w, nMuEvents);
  }
  if (m_truthSelectionTool.get()) {
    ATH_MSG_DEBUG( CutFlow(tmp_truth_cutflow).report(m_truthSelectionTool->names()) );
    std::lock_guard<std::mutex> lock(m_mutex);
    m_truthCutFlow.merge(std::move(tmp_truth_cutflow));
  }
  int nTruths(0), nInsideOut(0), nOutsideIn(0);
  std::vector<int> incTrkDenom = {0, 0, 0};

  // This is the beginning of the Nested Loop, built mainly for the Efficiency Plots
  if (debugBacktracking) {
    std::cout << "Start of new nested loop event ------------------------------------------------ \n";
  }
  // preselect tracks to do efficiency calculation
  std::vector<const xAOD::TrackParticle*> selectedTracks {};
  selectedTracks.reserve(ptracks->size());
  for (const auto& thisTrack: *ptracks) {
    if (m_useTrackSelection and not (m_trackSelectionTool->accept(*thisTrack, pvtx))) {
      continue;
    }
    const bool insideOut = isInsideOut(*thisTrack);
    if (insideOut) {
      nInsideOut += 1;
    } else {
      nOutsideIn += 1;
    }
    if (m_onlyInsideOutTracks and(not insideOut)) {
      continue;
    }
    selectedTracks.push_back(thisTrack);
  }

  ATH_MSG_DEBUG("Starting nested loop efficiency calculation");
  for (int itruth = 0; itruth < (int) truthParticlesVec.size(); itruth++) {  // Outer loop over all truth particles
    nTruths += 1;
    const xAOD::TruthParticle* thisTruth = truthParticlesVec[itruth];
    m_monPlots->fillSpectrum(*thisTruth);

    if(debugBacktracking){
      if (thisTruth->pdgId() == 22) {
	if (thisTruth->nParents() == 0) {
	  m_monPlots->prim_photon_fill(*thisTruth);
	} else {
	  m_monPlots->brem_photon_fill(*thisTruth);
	}
      }
    }

    // if the vector of truth particles is not empty also a truthSelectionTool was retrieved
    const IAthSelectionTool::CutResult accept = m_truthSelectionTool->accept(thisTruth);
    if (accept) {
      ++m_truthCounter;     // total number of truth tracks which pass cuts
      ++num_truth_selected; // total number of truth which pass cuts per event
      const float absTruthEta = std::abs(safelyGetEta(thisTruth));
      const unsigned int idx = binIndex(absTruthEta, ETA_PARTITIONS);
      if (idx < incTrkNum.size()) {
        ++incTrkNum[idx];
      }

      // LMTODO add this Jain/Swift
      bool addsToEfficiency(true); // weight for the trackeff histos

      if (debugBacktracking) {
        float lepton_w(0);
        double Px = thisTruth->px();
        double Py = thisTruth->py();
        constexpr int electronId = 11;
        if (thisTruth->absPdgId() == electronId) {
          double PtSquared = (Px * Px) + (Py * Py);
          if (PtSquared < 9000000) {
            lepton_w = 1;
          }
        }
        m_monPlots->lepton_fill(*thisTruth, lepton_w);
	if(thisTruth->hasProdVtx()){
	  const xAOD::TruthVertex* vtx = thisTruth->prodVtx();
	  double prod_rad = vtx->perp();
	  if(prod_rad < 600){
	    int truth_id = thisTruth->pdgId();
	    double truth_charge = thisTruth->charge();
	    double truth_pt = thisTruth->pt() * 0.001;
	    double min_dR = 10;
	    float bestmatch = 0;
	    double best_inverse_delta_pt(10);
	    double inverse_delta_pt(10);
	    
	    std::cout<<"Han: this Truth has an ID of: "<<truth_id<<"\n";
	    for(const auto& thisTrack: selectedTracks){
	      double track_charge = thisTrack->charge();
	      double track_pt = thisTrack->pt() * 0.001;
	      float prob = getMatchingProbability(*thisTrack);
	      double charge_product = truth_charge * track_charge;

	      if(charge_product == 1){
		//fill the plots we want
		if(prob > bestmatch){
		  bestmatch = prob;
		  double track_theta = thisTrack->theta();
		  double truth_eta = thisTruth->eta();                                                                                                                           
		  double track_eta = -std::log(std::tan(track_theta/2));                                                                                                               
		  double track_phi = thisTrack->phi();                                                                                                                              
		  double truth_phi = thisTruth->auxdata< float >("phi");
		  if(track_phi < 0) track_phi += twoPi;
		  if(truth_phi < 0) truth_phi += twoPi;

		  if((track_pt != 0) and (truth_pt != 0))  inverse_delta_pt = ((1./track_pt) - (1./truth_pt));

		  double delta_phi = track_phi - truth_phi;                                                                                                                            
		  double delta_eta = track_eta - truth_eta;                                                                                                                       
		  double delta_R = sqrt(delta_eta * delta_eta + delta_phi * delta_phi);
		  if(delta_R < min_dR){
		    min_dR = delta_R;
		    best_inverse_delta_pt = inverse_delta_pt;
		  }
		}
	      }
	    }// End of TrackParticle loop
	    if(bestmatch < 0.50){
	      if(prod_rad < 300){
		for(const auto& thisTrack: selectedTracks){
		  double charge_product = truth_charge * (thisTrack->charge());
		  std::cout<<"(Sub-three) Charge_Product: "<<charge_product<<" TMP: "<<getMatchingProbability(*thisTrack)<<" \n";
		}
	      }else{
		for(const auto& thisTrack: selectedTracks){
                  double charge_product = truth_charge * (thisTrack->charge());
		  std::cout<<"(Sub-six) Charge_Product: "<<charge_product<<" TMP: "<<getMatchingProbability(*thisTrack)<<" \n";
                }
	      }
	    }
	    m_monPlots->minDR(min_dR, prod_rad, bestmatch, best_inverse_delta_pt, truth_pt);
	  }
	}
      } // End of debugBackTracking section

      std::vector <std::pair<float, const xAOD::TrackParticle*> > matches; // Vector of pairs:
                                                                           // <truth_matching_probability, track> if
                                                                           // prob > minProbEffLow (0.5)
      float bestMatch = 0;
      float truth_charge = thisTruth->charge();
      for (const auto& thisTrack: selectedTracks) { // Inner loop over selected track particles
        const xAOD::TruthParticle* associatedTruth = getAsTruth.getTruth(thisTrack);
        if (associatedTruth && associatedTruth == thisTruth) {
          float prob = getMatchingProbability(*thisTrack);
	  float track_charge = thisTrack->charge();
	  float charge_product = truth_charge * track_charge;
          if (not std::isnan(prob)) {
            bestMatch = std::max(prob, bestMatch);
            if (prob > minProbEffLow) {
	      if(debugBacktracking){
		if(charge_product != 1) std::cout<<"Nested_Loop CP is: "<<charge_product<<" \n";
	      }
              matches.push_back(std::make_pair(prob, thisTrack));
            }
          }
        }
      }

      // count number of prospects and increment entry in vector for this event
      int deg_count = matches.size();
      if (deg_count <= 4) {
        ++m_prospectsMatched[deg_count];
      } else {
        ++m_prospectsMatched[5];
      }

      // special histograms for 1 or 2 matched tracks
      if (deg_count == 1) {
        m_monPlots->fillSingleMatch(*matches.at(0).second);
      } else if (deg_count == 2) {
        m_monPlots->fillTwoMatchDuplicate(matches[1].first, matches[0].first,
                                          *matches[1].second, *matches[0].second,
                                          *thisTruth);
      }
      // determine how many duplicate match probabilities
      // first create vector of probabilities and sort
      if (deg_count > 1) {
        std::vector<float> probs;
        for (int i = 0; i < deg_count; i++) {
          probs.push_back(matches[i].first);
        }
        std::sort(matches.begin(), matches.end(), sortProspects);
        // count duplicates
        float prev = matches[0].first;
        int nduplicates = 0;
        const std::array<int*, 5> matchedEProbs = {
          nullptr, nullptr, &m_twoMatchedEProb, &m_threeMatchedEProb, &m_fourMatchedEProb
        };
        for (int i = 1; i < deg_count; i++) {
          bool duplicate = std::fabs(matches[i].first - prev) < 1.e-9;
          if (duplicate) {
            ++nduplicates;
          }
          if (!duplicate || i == deg_count - 1) {
            if (nduplicates > 1) {
              (*(matchedEProbs[deg_count]))++;
            }
            nduplicates = 0;
            prev = matches[i].first;
          }
        }
      }

      // fill truth-only plots
      if (bestMatch >= minProbEffHigh) {
        ++num_truthmatch_match;
        const xAOD::TruthParticle* associatedTruth = matches.empty() ? nullptr : getAsTruth.getTruth(
          matches.back().second);
        if (!associatedTruth) {
          continue;
        }
        m_monPlots->fill(*associatedTruth); // This is filling truth-only plots:  m_TrackTruthInfoPlots
      } else {
        addsToEfficiency = false;
      }

      m_monPlots->fillEfficiency(*thisTruth, addsToEfficiency,nMuEvents);
    } // end of the "if(accept)" loop
  }// End of Big truthParticle loop
  ATH_MSG_DEBUG("End of efficiency calculation");

  if (m_useTrackSelection) {
    for (const auto& thisTrack: *ptracks) { // Inner loop over all track particle
      if (m_useTrackSelection) {
        fillTrackCutFlow(m_trackSelectionTool->accept(*thisTrack, pvtx));
      }
    }
  }

  if (num_truthmatch_match == 0) {
    if (not m_truthParticleName.key().empty()) {
      ATH_MSG_INFO("NO TRACKS had associated truth.");
    }
  } else {
    ATH_MSG_DEBUG(num_truthmatch_match << " tracks out of " << ptracks->size() << " had associated truth.");
  }
  m_monPlots->fillCounter(nSelectedTracks, InDetPerfPlot_nTracks::SELECTED);
  m_monPlots->fillCounter(ptracks->size(), InDetPerfPlot_nTracks::ALL);
  m_monPlots->fillCounter(truthParticlesVec.size(), InDetPerfPlot_nTracks::TRUTH);
  m_monPlots->fillCounter(num_truthmatch_match, InDetPerfPlot_nTracks::TRUTH_MATCHED);
  // Tracking In Dense Environment
  if (m_fillTIDEPlots && !m_jetContainerName.key().empty()) {
    return doJetPlots(ptracks.cptr(), getAsTruth, pvtx, truthParticlesVec);
  }     // if TIDE
  // ATH_MSG_INFO(getTruth.report());
  return StatusCode::SUCCESS;
}

StatusCode
InDetPhysValMonitoringTool::bookHistograms() {
  ATH_MSG_INFO("Booking hists " << name() << "...");
  m_monPlots->setDetailLevel(100); // DEBUG, enable expert histograms
  m_monPlots->initialize();
  std::vector<HistData> hists = m_monPlots->retrieveBookedHistograms();
  for (auto hist : hists) {
    ATH_CHECK(regHist(hist.first, hist.second, all)); // ??
  }
  // do the same for Efficiencies, but there's a twist:
  std::vector<EfficiencyData> effs = m_monPlots->retrieveBookedEfficiencies();
  for (auto& eff : effs) {
    // reg**** in the monitoring baseclass doesnt have a TEff version, but TGraph *
    // pointers just get passed through, so we use that method after an ugly cast
    ATH_CHECK(regGraph(reinterpret_cast<TGraph*>(eff.first), eff.second, all)); // ??
  }
  return StatusCode::SUCCESS;
}

StatusCode
InDetPhysValMonitoringTool::procHistograms() {
  ATH_MSG_INFO("Finalising hists " << name() << "...");
  ATH_MSG_INFO(" number with one matched reco = " << m_prospectsMatched[1]);
  ATH_MSG_INFO(" number with two matched reco = " << m_prospectsMatched[2]);
  ATH_MSG_INFO(" number with two matched reco where there is a duplicate mc_probability = " << m_twoMatchedEProb);
  ATH_MSG_INFO(" number with three matched reco = " << m_prospectsMatched[3]);
  ATH_MSG_INFO(" number with three matched reco where there is a duplicate mc_probability = " << m_threeMatchedEProb);
  ATH_MSG_INFO(" number with four matched reco = " << m_prospectsMatched[4]);
  ATH_MSG_INFO(" number with four matched reco where there is a duplicate mc_probability = " << m_fourMatchedEProb);
  ATH_MSG_INFO(" number with five+ matched reco = " << m_prospectsMatched[5]);
  ATH_MSG_INFO(" total number of truth particles which pass cuts = " << m_truthCounter);
  ATH_MSG_INFO(
    " total number of truth particles which pass and match a reco track (including duplicates) = " <<
      m_prospectsMatched[1] +
      m_prospectsMatched[2] + m_prospectsMatched[3] + m_prospectsMatched[4] + m_prospectsMatched[5]);
  ATH_MSG_INFO(" number zero matched reco = " << m_prospectsMatched[0]);
  ATH_MSG_INFO(
    " total number of truth tracks, which have more than one matching reco track = " << m_prospectsMatched[2] +
      m_prospectsMatched[3] + m_prospectsMatched[4] + m_prospectsMatched[5]);
  ATH_MSG_INFO(
    " total number of truth tracks, which have more than one matching reco and atleast one duplicate mc_probability = " << m_twoMatchedEProb + m_threeMatchedEProb +
      m_fourMatchedEProb);
  if (m_useTrackSelection) {
    ATH_MSG_INFO("");
    ATH_MSG_INFO("Now Cutflow for track cuts:");
    ATH_MSG_INFO("");
    for (int i = 0; i < (int) m_trackCutflow.size(); ++i) {
      ATH_MSG_INFO("number after " << m_trackCutflowNames[i] << ": " << m_trackCutflow[i]);
    }
  }

  ATH_MSG_INFO("");
  ATH_MSG_INFO("Cutflow for truth tracks:");
  if (m_truthSelectionTool.get()) {
    ATH_MSG_INFO("Truth selection report: " << m_truthCutFlow.report( m_truthSelectionTool->names()) );
  }
  if (endOfRunFlag()) {
    m_monPlots->finalize();
  }
  ATH_MSG_INFO("Successfully finalized hists");
  return StatusCode::SUCCESS;
}

const std::vector<const xAOD::TruthParticle*>
InDetPhysValMonitoringTool::getTruthParticles() {
  // truthParticles.clear();
  std::vector<const xAOD::TruthParticle*> tempVec {};
  if (m_truthParticleName.key().empty()) {
    return tempVec;
  }
  if (m_pileupSwitch == "All") {
    SG::ReadHandle<xAOD::TruthParticleContainer> truthParticleContainer( m_truthParticleName);
    if (not truthParticleContainer.isValid()) {
      return tempVec;
    }
    tempVec.insert(tempVec.begin(), truthParticleContainer->begin(), truthParticleContainer->end());
  } else {
    if (m_pileupSwitch == "HardScatter") {
      // get truthevent container to separate out pileup and hardscatter truth particles
      if (not m_truthEventKey.key().empty()) {
      SG::ReadHandle<xAOD::TruthEventContainer> truthEventContainer( m_truthEventKey);
      const xAOD::TruthEvent* event = (truthEventContainer.isValid()) ? truthEventContainer->at(0) : nullptr;
      if (not event) {
        return tempVec;
      }
      const auto& links = event->truthParticleLinks();
      tempVec.reserve(event->nTruthParticles());
      for (const auto& link : links) {
        tempVec.push_back(*link);
      }
      }
    } else if (m_pileupSwitch == "PileUp") {
      if (not m_truthPileUpeEventKey.key().empty()) {
      ATH_MSG_VERBOSE("getting TruthPileupEvents container");
      // get truth particles from all pileup events
      SG::ReadHandle<xAOD::TruthPileupEventContainer> truthPileupEventContainer(m_truthPileUpeEventKey);
      if (truthPileupEventContainer.isValid()) {
        const unsigned int nPileup = truthPileupEventContainer->size();
        tempVec.reserve(nPileup * 200); // quick initial guess, will still save some time
        for (unsigned int i(0); i != nPileup; ++i) {
          auto eventPileup = truthPileupEventContainer->at(i);
          // get truth particles from each pileup event
          int ntruth = eventPileup->nTruthParticles();
          ATH_MSG_VERBOSE("Adding " << ntruth << " truth particles from TruthPileupEvents container");
          const auto& links = eventPileup->truthParticleLinks();
          for (const auto& link : links) {
            tempVec.push_back(*link);
          }
        }
      } else {
        ATH_MSG_ERROR("no entries in TruthPileupEvents container!");
      }
      }
    } else {
      ATH_MSG_ERROR("bad value for PileUpSwitch");
    }
  }
  return tempVec;
}

void
InDetPhysValMonitoringTool::fillTrackCutFlow(const asg::AcceptData& accept) {
  fillCutFlow(accept, m_trackCutflowNames, m_trackCutflow);
  return;
}

void
InDetPhysValMonitoringTool::fillCutFlow(const asg::AcceptData& accept, std::vector<std::string>& names,
                                        std::vector<int>& cutFlow) {
  // initialise cutflows
  if (cutFlow.empty()) {
    names.push_back("preCut");
    cutFlow.push_back(0);
    for (unsigned int i = 0; i != accept.getNCuts(); ++i) {
      cutFlow.push_back(0);
      names.push_back((std::string) accept.getCutName(i));
    }
  }
  // get cutflow
  cutFlow[0] += 1;
  bool cutPositive = true;
  for (unsigned int i = 0; i != (accept.getNCuts() + 1); ++i) {
    if (!cutPositive) {
      continue;
    }
    if (accept.getCutResult(i)) {
      cutFlow[i + 1] += 1;
    } else {
      cutPositive = false;
    }
  }
  return;
}

StatusCode
InDetPhysValMonitoringTool::doJetPlots(const xAOD::TrackParticleContainer* ptracks,
                                       IDPVM::CachedGetAssocTruth& getAsTruth,
                                       const xAOD::Vertex* primaryVtx, 
                                       const std::vector<const xAOD::TruthParticle*> &truthParticles) {
  SG::ReadHandle<xAOD::JetContainer> jets(m_jetContainerName);
  const float minProbEffHigh = 0.5;

  if (not jets.isValid() or not truthParticles.empty()) {
    ATH_MSG_WARNING(
      "Cannot open " << m_jetContainerName <<
        " jet container or TruthParticles truth particle container. Skipping jet plots.");
  } else {
    for (const auto& thisJet: *jets) {         // The big jets loop
      if (not passJetCuts(*thisJet)) {
        continue;
      }
      for (auto thisTrack: *ptracks) {    // The beginning of the track loop
        if (m_useTrackSelection and not (m_trackSelectionTool->accept(*thisTrack, primaryVtx))) {
          continue;
        }
        if (m_onlyInsideOutTracks and !(isInsideOut(*thisTrack))) {
          continue; // not an inside out track
        }
        if (thisJet->p4().DeltaR(thisTrack->p4()) > m_maxTrkJetDR) {
          continue;
        }
        //
        const bool safelyFilled = m_monPlots->filltrkInJetPlot(*thisTrack, *thisJet);
        if (safelyFilled) {
          float trkInJet_w(0), trkInJet_BMR(1);
          float prob = getMatchingProbability(*thisTrack);
          if (prob > minProbEffHigh) {
            trkInJet_w = 1;
            trkInJet_BMR = 0;
          }
          m_monPlots->jet_fill(*thisTrack, *thisJet, trkInJet_w);                          // fill trkinjeteff plots
          m_monPlots->jetBMR(*thisTrack, *thisJet, trkInJet_BMR);                          // fin in track in jet bad
          m_monPlots->fillSimpleJetPlots(*thisTrack, prob);                                // Fill all the
                                                                                          
          const xAOD::TruthParticle* associatedTruth = getAsTruth.getTruth(thisTrack);     //
                                                                                         
          if (associatedTruth) {
            m_monPlots->fillJetResPlots(*thisTrack, *associatedTruth, *thisJet);          // Fill jet pull &
                                                                                          // resolution plots
            int barcode = associatedTruth->barcode();
            m_monPlots->fillJetHitsPlots(*thisTrack, prob, barcode);                      // Fill the two extra
                                                                                          // plots
            if (m_truthSelectionTool->accept(associatedTruth)) {                          // Fill the Jet plots with
                                                                                          // "Eff" in the name
              m_monPlots->fillJetEffPlots(*associatedTruth, *thisJet);
            }
          }
        }
      } // end of track loop
        // fill in things like sum jet pT in dR bins - need all tracks in the jet first
      m_monPlots->fillJetPlotCounter(*thisJet);
      for (const auto& thisTruth: truthParticles) {
        // for primary tracks we want an efficiency as a function of track jet dR
        if ((m_truthSelectionTool->accept(thisTruth) and(thisJet->p4().DeltaR(thisTruth->p4()) < m_maxTrkJetDR))) {
          m_monPlots->fillJetTrkTruth(*thisTruth, *thisJet);
        }
      }
      m_monPlots->fillJetTrkTruthCounter(*thisJet);
    } // loop over jets
  }
  return StatusCode::SUCCESS;
}

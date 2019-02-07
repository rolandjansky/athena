/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPhysValMonitoringTool.cxx
 * @author shaun roe
 **/

#include "InDetPhysValMonitoring/InDetPhysValMonitoringTool.h"
#include "InDetPhysHitDecoratorTool.h"
#include "InDetRttPlots.h"
#include "InDetPerfPlot_nTracks.h"
#include "AthTruthSelectionTool.h"
#include "CachedGetAssocTruth.h"
// #include "TrackTruthLookup.h"
//
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackStateValidationContainer.h"
#include "xAODTracking/TrackStateValidation.h"
#include "xAODTracking/TrackStateValidationContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthPileupEvent.h"
#include "xAODTruth/TruthPileupEventContainer.h"
#include "xAODTruth/TruthPileupEventAuxContainer.h"

#include "TrkTrack/TrackCollection.h"
#include "xAODJet/JetContainer.h"
#include "PATCore/TAccept.h"
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

  // Convert to GeV from the default MeV
  constexpr float
  operator "" _GeV (long double energy) {
    return energy * 0.001;
  }

  constexpr float
  operator "" _GeV (unsigned long long energy) {
    return energy * 0.001;
  }

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
  m_truthCutCounters{},
  m_fillTIDEPlots(false),
  m_fillExtraTIDEPlots(false),
  m_fillITkResolutionPlots(false),
  m_fillAdditionalITkPlots(false)
  {
  declareProperty("TrackParticleContainerName", m_trkParticleName = "InDetTrackParticles"); // Aug 8th: switch
                                                                                            // "InDetTrackParticles"
                                                                                            // with
                                                                                            // "TrackCollection_tlp5_CombinedInDetTracks"
                                                                                            // (v1)...no tracks appeared
  declareProperty("TruthParticleContainerName", m_truthParticleName = "TruthParticles");
  declareProperty("VertexContainerName", m_vertexContainerName = "PrimaryVertices");
  declareProperty("TruthVertexContainerName", m_truthVertexContainerName = "TruthVertices");
  declareProperty("EventInfoContainerName", m_eventInfoContainerName = "EventInfo");
  declareProperty("useTrackSelection", m_useTrackSelection);
  declareProperty("useTrkSelectPV", m_TrkSelectPV);
  declareProperty("onlyInsideOutTracks", m_onlyInsideOutTracks);
  declareProperty("TrackSelectionTool", m_trackSelectionTool);
  declareProperty("TruthSelectionTool", m_truthSelectionTool);
  declareProperty("FillTrackInJetPlots", m_fillTIDEPlots);
  declareProperty("FillExtraTrackInJetPlots", m_fillExtraTIDEPlots);
  declareProperty("jetContainerName", m_jetContainerName = "AntiKt4TruthJets");
  declareProperty("maxTrkJetDR", m_maxTrkJetDR = 0.4);
  declareProperty("DirName", m_dirName = "IDPerformanceMon/");
  declareProperty("SubFolder", m_folder);
  declareProperty("PileupSwitch", m_pileupSwitch = "All");
  declareProperty("FillITkResolutionPlots", m_fillITkResolutionPlots = false);
  declareProperty("FillAdditionalITkPlots", m_fillAdditionalITkPlots=false);
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
  if (m_useTrackSelection) {
    ATH_CHECK(m_trackSelectionTool.retrieve());
    if (not m_trackSelectionTool) {
      ATH_MSG_ERROR(
        "\033[1;31mYou have chosen to use track selection, but no track selection tool was configured\033[0m\n");
      return StatusCode::FAILURE;
    }
  }
  ATH_CHECK(m_truthSelectionTool.retrieve());
  m_truthCutCounters = m_truthSelectionTool->counters();
  m_monPlots = std::move(std::unique_ptr<InDetRttPlots> (new InDetRttPlots(0, m_dirName + m_folder)));
  m_monPlots->SetFillExtraTIDEPlots(m_fillExtraTIDEPlots);
  m_monPlots->SetFillITkResolutionPlots(m_fillITkResolutionPlots);
  m_monPlots->SetFillAdditionalITkPlots(m_fillAdditionalITkPlots);
  return StatusCode::SUCCESS;
}

StatusCode
InDetPhysValMonitoringTool::fillHistograms() {
  ATH_MSG_DEBUG("Filling hists " << name() << "...");
  // function object could be used to retrieve truth: IDPVM::CachedGetAssocTruth getTruth;

  // retrieve trackParticle container
  const auto ptracks = getContainer<xAOD::TrackParticleContainer>(m_trkParticleName);
  if (not ptracks) {
    return StatusCode::FAILURE;
  }
  
  //
  // retrieve truthParticle container
  std::vector<const xAOD::TruthParticle*> truthParticlesVec = getTruthParticles();
  // IDPVM::TrackTruthLookup getAsTruth(ptracks, &truthParticlesVec); //caches everything upon construction
  IDPVM::CachedGetAssocTruth getAsTruth; // only cache one way, track->truth, not truth->tracks
  //

  unsigned int nMuEvents = 0;
  const xAOD::TruthPileupEventContainer* truthPileupEventContainer = 0;
  std::string truthPUEventCollName = "TruthPileupEvents";
  if (evtStore()->contains<xAOD::TruthPileupEventContainer>(truthPUEventCollName)) {
    evtStore()->retrieve(truthPileupEventContainer, truthPUEventCollName);
  }
  else {
    truthPUEventCollName = "TruthPileupEvent";
    if (evtStore()->contains<xAOD::TruthPileupEventContainer>(truthPUEventCollName)) {
      evtStore()->retrieve(truthPileupEventContainer, truthPUEventCollName);
    }
  }
  if (NULL!=truthPileupEventContainer) {
    nMuEvents = (int) truthPileupEventContainer->size();
  }

  ATH_MSG_DEBUG("Filling vertex plots");
  const xAOD::VertexContainer* pvertex = getContainer<xAOD::VertexContainer>(m_vertexContainerName);
  const xAOD::Vertex* pvtx = nullptr;
  
  if (pvertex and not pvertex->empty()) {
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
  const xAOD::EventInfo* pei = getContainer<xAOD::EventInfo>(m_eventInfoContainerName);
  if (pei and pvertex) {
    m_monPlots->fill(*pvertex, *pei);
  } else {
    ATH_MSG_WARNING("Skipping vertexing plots using EventInfo.");
  }

  // get truth vertex container name - m_truthVertexContainerName
  const xAOD::TruthVertexContainer* truthVrt = getContainer<xAOD::TruthVertexContainer>(m_truthVertexContainerName);
  const xAOD::TruthVertex* truthVertex = 0;
  if (truthVrt) {
    if (!m_TrkSelectPV) {
      ATH_MSG_VERBOSE("size of TruthVertex container = " << truthVrt->size());
    }
    const auto& stdVertexContainer = truthVrt->stdcont();
    auto findVtx = std::find_if(stdVertexContainer.rbegin(), stdVertexContainer.rend(), acceptTruthVertex);
    truthVertex = (findVtx == stdVertexContainer.rend()) ? nullptr : *findVtx;
  } else {
    ATH_MSG_WARNING("Cannot open " << m_truthVertexContainerName << " truth vertex container");
  }
  if (not truthVertex) ATH_MSG_INFO ("Truth vertex did not pass cuts");
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
  m_truthSelectionTool->clearCounters();

  // dummy variables
  int hasTruth(0), hashighprob(0), passtruthsel(0);

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
    
    if (associatedTruth) {
      hasTruth += 1;
      if (prob < minProbEffLow) { // nan will also fail this test
        const bool isFake = (prob < minProbEffLow);
        m_monPlots->fillFakeRate(*thisTrack, isFake);
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
      if (m_truthSelectionTool->accept(associatedTruth)) {
        passtruthsel += 1;
      }
      if ((prob > minProbEffLow) and m_truthSelectionTool->accept(associatedTruth)) {
        m_monPlots->fill(*thisTrack, *associatedTruth); // Make all plots requiring both truth & track (meas, res, &
                                                        // pull)
      }
    }

    m_monPlots->fillLinkedandUnlinked(*thisTrack, Prim_w, Sec_w, Unlinked_w, nMuEvents);
  }
  ATH_MSG_DEBUG(m_truthSelectionTool->str());
  const auto& tmp = m_truthSelectionTool->counters(); // get array of counters for the cuts

  unsigned idx(0);
  for (auto& i:m_truthCutCounters) {
    i += tmp[idx++]; // i=sum of all the individual counters on each cut.
  }
  int nTruths(0), nInsideOut(0), nOutsideIn(0);
  std::vector<int> incTrkDenom = {
    0, 0, 0
  };

  // This is the beginning of the Nested Loop, built mainly for the Efficiency Plots
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
  const int SiSPSF = 0;
  const int TRTSeededTrackFinder = 4;
  const int TRTStandalone = 20;

  int truth_count(0);
  for (int itruth = 0; itruth < (int) truthParticlesVec.size(); itruth++) {  // Outer loop over all truth particles
    nTruths += 1;
    const xAOD::TruthParticle* thisTruth = truthParticlesVec[itruth];
    m_monPlots->fillSpectrum(*thisTruth);

    if (thisTruth->pdgId() == 22) {
      if (thisTruth->nParents() == 0) {
        m_monPlots->prim_photon_fill(*thisTruth);
      } else {
        m_monPlots->brem_photon_fill(*thisTruth);
      }
    }

    const bool accept = m_truthSelectionTool->accept(thisTruth);
    if (accept) {
      ++m_truthCounter; // total number of truth tracks which pass cuts
      ++num_truth_selected; // total number of truth which pass cuts per event
      const float absTruthEta = std::abs(safelyGetEta(thisTruth));
      const unsigned int idx = binIndex(absTruthEta, ETA_PARTITIONS);
      if (idx < incTrkNum.size()) {
        ++incTrkNum[idx];
      }

      // LMTODO add this Jain/Swift
      bool addsToEfficiency(true); // weight for the trackeff histos
      
      if(truth_count <= 1){
        std::bitset<52> base_bits;
        std::vector< const xAOD::TrackParticle* > bestTrack;
        int SiSPweight(0), TRTSeededweight(0), TRTStandaloneweight(0), other_weight(0);
        double truth_charge = thisTruth->charge();
        double prod_rad = 0.;
        const xAOD::TruthVertex* vtx = thisTruth->prodVtx();
        if (vtx)
          prod_rad = vtx->perp();
        double best_match = -1;
        for(const auto& thisTrack: selectedTracks){
          double track_charge = thisTrack->charge();
          double charge_product = truth_charge * track_charge;
          if(charge_product == 1){
            double tmp = getMatchingProbability(*thisTrack);
            if(tmp > best_match){
              best_match = tmp;
              bestTrack.push_back(thisTrack);
            }
          }
        }
        if(best_match > 0.50){
          std::bitset< xAOD::NumberOfTrackRecoInfo > author_bitset = bestTrack.back()->patternRecoInfo(); //grab the best of the possible TrackParticles
          if(author_bitset.test(SiSPSF)) SiSPweight = 1;
          if(author_bitset.test(TRTSeededTrackFinder)) TRTSeededweight = 1;
          if(author_bitset.test(TRTStandalone)) TRTStandaloneweight = 1;
          int bitlength = author_bitset.size();
          for(int j=1; j<bitlength; j++){
            if((author_bitset.test(j)) && (j != 4) && (j != 20)) other_weight = 1;
          }
          base_bits |= author_bitset;
        }
        m_monPlots->algoEfficiency(prod_rad, SiSPweight, TRTSeededweight, TRTStandaloneweight, other_weight);
        m_monPlots->track_author(base_bits);
      }
      truth_count += 1;

      std::vector <std::pair<float, const xAOD::TrackParticle*> > matches; // Vector of pairs:
                                                                           // <truth_matching_probability, track> if
                                                                           // prob > minProbEffLow (0.5)
      float bestMatch = 0;
      for (const auto& thisTrack: selectedTracks) { // Inner loop over selected track particles
        const xAOD::TruthParticle* associatedTruth = getAsTruth.getTruth(thisTrack);
        if (associatedTruth && associatedTruth == thisTruth) {
          float prob = getMatchingProbability(*thisTrack);
          if (not std::isnan(prob)) {
            bestMatch = std::max(prob, bestMatch);
            if (prob > minProbEffLow) {
              matches.push_back(std::make_pair(prob, thisTrack));
            }
            const bool isFake = (prob < minProbEffLow);
            m_monPlots->fillFakeRate(*thisTrack, isFake);
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
        Root::TAccept trackAccept = m_trackSelectionTool->accept(*thisTrack, pvtx);
        fillTrackCutFlow(trackAccept);
      }
    }
  }

  if (num_truthmatch_match == 0) {
    ATH_MSG_INFO("NO TRACKS had associated truth.");
  } else {
    ATH_MSG_DEBUG(num_truthmatch_match << " tracks out of " << ptracks->size() << " had associated truth.");
  }
  m_monPlots->fillCounter(nSelectedTracks, InDetPerfPlot_nTracks::SELECTED);
  m_monPlots->fillCounter(ptracks->size(), InDetPerfPlot_nTracks::ALL);
  m_monPlots->fillCounter(truthParticlesVec.size(), InDetPerfPlot_nTracks::TRUTH);
  m_monPlots->fillCounter(num_truthmatch_match, InDetPerfPlot_nTracks::TRUTH_MATCHED);
  // Tracking In Dense Environment
  if (m_fillTIDEPlots && !m_jetContainerName.empty()) {
    return doJetPlots(ptracks, getAsTruth, pvtx);
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
  unsigned int idx(0);
  for (const auto& cutName:m_truthSelectionTool->names()) {
    ATH_MSG_INFO("number after " << cutName << ": " << m_truthCutCounters[idx++]);
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
  if (m_truthParticleName.empty()) {
    return tempVec;
  }
  if (m_pileupSwitch == "All") {
    const xAOD::TruthParticleContainer* truthParticleContainer =
      (!m_truthParticleName.empty() ? getContainer<xAOD::TruthParticleContainer>(m_truthParticleName) : nullptr);
    if (not truthParticleContainer) {
      return tempVec;
    }
    tempVec.insert(tempVec.begin(), truthParticleContainer->begin(), truthParticleContainer->end());
  } else {
    if (m_pileupSwitch == "HardScatter") {
      // get truthevent container to separate out pileup and hardscatter truth particles
      const xAOD::TruthEventContainer* truthEventContainer = nullptr;
      const std::string truthEventCollName =
        evtStore()->contains<xAOD::TruthEventContainer>("TruthEvents") ? "TruthEvents" : "TruthEvent";
      evtStore()->retrieve(truthEventContainer, truthEventCollName);
      const xAOD::TruthEvent* event = (truthEventContainer) ? truthEventContainer->at(0) : nullptr;
      if (not event) {
        return tempVec;
      }
      const auto& links = event->truthParticleLinks();
      tempVec.reserve(event->nTruthParticles());
      for (const auto& link : links) {
        tempVec.push_back(*link);
      }
    } else if (m_pileupSwitch == "PileUp") {
      ATH_MSG_VERBOSE("getting TruthPileupEvents container");
      // get truth particles from all pileup events
      const xAOD::TruthPileupEventContainer* truthPileupEventContainer = nullptr;
      const std::string truthPUEventCollName =
        evtStore()->contains<xAOD::TruthPileupEventContainer>("TruthPileupEvents") ? "TruthPileupEvents" :
        "TruthPileupEvent";
      evtStore()->retrieve(truthPileupEventContainer, truthPUEventCollName);
      if (truthPileupEventContainer) {
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
    } else {
      ATH_MSG_ERROR("bad value for PileUpSwitch");
    }
  }
  return tempVec;
}

void
InDetPhysValMonitoringTool::fillTrackCutFlow(Root::TAccept& accept) {
  fillCutFlow(accept, m_trackCutflowNames, m_trackCutflow);
  return;
}

void
InDetPhysValMonitoringTool::fillCutFlow(Root::TAccept& accept, std::vector<std::string>& names,
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
                                       const xAOD::Vertex* primaryVtx) {
  const xAOD::JetContainer* jets = getContainer<xAOD::JetContainer>(m_jetContainerName);
  const xAOD::TruthParticleContainer* truthParticles = getContainer<xAOD::TruthParticleContainer>("TruthParticles");
  const float minProbEffHigh = 0.5;

  if (!jets || !truthParticles) {
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
      for (const auto& thisTruth: *truthParticles) {
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

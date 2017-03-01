/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPhysValLargeD0Tool.cxx
 * @author siin che
 **/

#include "InDetPhysValMonitoring/InDetPhysValLargeD0Tool.h"

#include <vector>
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODEventInfo/EventInfo.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "InDetRttLargeD0Plots.h"
#include "InDetPerfPlot_nTracks.h"
#include "TrkTrack/TrackCollection.h"
#include <limits>
#include <cmath> // to get std::isnan()

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "xAODTracking/TrackParticle.h"
#include "InDetPhysValMonitoring/IInDetPhysValDecoratorTool.h"

#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"

// #include <unordered_map>
#include <utility>

using std::pair;
using std::make_pair;

namespace { // utility functions used here
  // get truth particle associated with a track particle
  const xAOD::TruthParticle*
  getTruthPtr(const xAOD::TrackParticle& trackParticle) {
    typedef ElementLink<xAOD::TruthParticleContainer> ElementTruthLink_t;
    const xAOD::TruthParticle* result(nullptr);
    // 0. is there any truth?
    if (trackParticle.isAvailable<ElementTruthLink_t>("truthParticleLink")) {
      // 1. ..then get link
      const ElementTruthLink_t ptruthContainer = trackParticle.auxdata<ElementTruthLink_t>("truthParticleLink");
      if (ptruthContainer.isValid()) {
        result = *ptruthContainer;
      }
    }
    return result;
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

//  bool isInsideOut(const xAOD::TrackParticle &track){
//    std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo>  patternInfo = track.patternRecoInfo();
//    return patternInfo.test(0);
//  }
/**
   unused here.
   bool truthSelector(const xAOD::TruthParticle &truth){
    const bool pCut = ( truth.pt() >= 500 ); //ORIGINAL SETTING: ( truth.pt() >= 400 );
    const bool stabilityCut = (truth.status() == 1);
    const bool noNeutrals = not (truth.isNeutral());
    return (pCut and stabilityCut and noNeutrals);
   }
 **/
}// namespace

///Parametrized constructor
InDetPhysValLargeD0Tool::InDetPhysValLargeD0Tool(const std::string& type, const std::string& name,
                                                 const IInterface* parent) :
  ManagedMonitorToolBase(type, name, parent),
  m_useTrackSelection(true),
  m_onlyInsideOutTracks(false),
  m_trackSelectionTool("TrackSelectionTool/TrackSelectionTool"),
  m_truthSelectionTool("TrackTruthSelectionTool/TruthSelectionTool"),
  m_maxTrkJetDR{},
  m_folder{},
  m_fillTIDEPlots{},
  m_fillExtraTIDEPlots{} {
  declareProperty("TrackParticleContainerName", m_trkParticleName = "InDetTrackParticles");
  declareProperty("TruthParticleContainerName", m_truthParticleName = "TruthParticles");
  declareProperty("VertexContainerName", m_vertexContainerName = "PrimaryVertices");
  declareProperty("EventInfoContainerName", m_eventInfoContainerName = "EventInfo");
  declareProperty("useTrackSelection", m_useTrackSelection); // redundant?
  declareProperty("onlyInsideOutTracks", m_onlyInsideOutTracks);
  declareProperty("TrackSelectionTool", m_trackSelectionTool);
  declareProperty("TruthSelectionTool", m_truthSelectionTool);
  declareProperty("SubFolder", m_folder);
  declareProperty("SignalIds", m_signalIds); // @asogaard
}

InDetPhysValLargeD0Tool::~InDetPhysValLargeD0Tool() {
}

StatusCode
InDetPhysValLargeD0Tool::initialize() {
  ATH_MSG_DEBUG("Initializing " << name() << "...");
  ATH_CHECK(ManagedMonitorToolBase::initialize());
  // Get the track selector tool only if m_useTrackSelection is true;
  // first check for consistency i.e. that there is a trackSelectionTool if you ask
  // for trackSelection
  if (m_useTrackSelection) {
    if (not m_trackSelectionTool) {
      ATH_MSG_ERROR(
        "\033[1;31mYou have chosen to use track selection, but no track selection tool was configured\033[0m\n");
      return StatusCode::FAILURE;
    }
    ATH_CHECK(m_trackSelectionTool.retrieve());
  }
  ATH_CHECK(m_truthSelectionTool.retrieve());
  m_LargeD0Plots =
    std::move(std::unique_ptr<InDetRttLargeD0Plots> (new InDetRttLargeD0Plots(0, "IDPerformanceMon/" + m_folder)));

  if (this->msgLvl(MSG::VERBOSE)) {
    std::string _sid;
    for (auto pdgid: m_signalIds) {
      _sid.append(std::to_string(pdgid) + std::string(" "));
    }
    ATH_MSG_DEBUG("Signal PDGID to be checked: " << _sid);
  }

  return StatusCode::SUCCESS;
}

StatusCode
InDetPhysValLargeD0Tool::fillHistograms() {
  ATH_MSG_DEBUG("Filling hists " << name() << "...");

  // Retrieve trackParticle container.
  auto ptracks = getContainer<xAOD::TrackParticleContainer>(m_trkParticleName);
  if (!ptracks) {
    return StatusCode::FAILURE;
  }
  // Retrieve EventInfo container
  auto eventinfo = getContainer<xAOD::EventInfo>(m_eventInfoContainerName);
  // Retrieve vertex container
  auto privert = getContainer<xAOD::VertexContainer>(m_vertexContainerName);

  // Retrieve truthParticle container.
  const xAOD::TruthParticleContainer* truthParticles =
    (!m_truthParticleName.empty() ? getContainer<xAOD::TruthParticleContainer>(m_truthParticleName) : nullptr);

  // Counters etc.
  const unsigned int nTracks(ptracks->size());
  const unsigned int nTruth(truthParticles  ? truthParticles->size() : 0);
  unsigned int nSelectedTracks(0), num_truthmatch_match(0);

  // Probabilities.
  /**
   * The truth matching probability must not be <= 0., otherwise the tool will seg fault in case of missing truth (e.g.
   * data).
   */
  const float minProbEffReallyLow(0.20); // Temporary fake limit, for comparing with PRTT.
  const float minProbEffLow(0.50); // If the probability of a match is less than this, we call it a fake.
  const float minProbEffHigh(0.80); // If the probability of a match is higher than this, it either feeds the NUM or is
                                    // a duplicate.

  // =======================================================================
  // Main track loop.
  // -------------------------------------------------------------------------
  /**
   * This is for filling Track-only, Track 'n' Truth with good matching probability (meas, res, & pull), and Fakes.
   */

  for (const auto& thisTrack : *ptracks) {
    // apply minimum track selection to all tracks to match with Truth Selection cuts
    if (!MinTrackSelection(thisTrack)) {
      continue;
    }
    // * 0 means z0, d0 cut is wrt. beam spot - put in a PV to change this.
    if (m_useTrackSelection &&
        isLargeD0Track(thisTrack) && !(m_trackSelectionTool->accept(thisTrack))) { // @asogaard
      continue;
    }

    // * Not an inside-out track. NOT RELEVANT!
    // if (m_onlyInsideOutTracks and (not isInsideOut(*thisTrack))) { continue; }
    // * Increment number of selected tracks.
    ++nSelectedTracks;

    // * Get associated truth particle and match probability.
    const xAOD::TruthParticle* associatedTruth = getTruthPtr(*thisTrack);
    float prob = getMatchingProbability(*thisTrack);

    bool trtHit(false), trtOut(false);
    uint8_t iTrtHits(0), iTrtOutliers(0), iTrtTubeHits(0);
    int nTrtHits(0), nTrtTubeHits(0);

    if (thisTrack->summaryValue(iTrtHits, xAOD::numberOfTRTHits)) {
      if (iTrtHits > 0) {
        trtHit = true;
        nTrtHits = (int) iTrtHits;
      }
    }
    if (thisTrack->summaryValue(iTrtOutliers, xAOD::numberOfTRTOutliers)) {
      if (iTrtOutliers > 0) {
        trtOut = true;
      }
    }
    if (thisTrack->summaryValue(iTrtTubeHits, xAOD::numberOfTRTTubeHits)) {
      if (iTrtTubeHits > 0) {
        nTrtTubeHits = (int) iTrtTubeHits;
      }
    }

    // This is where the BMR, Fake, and Really Fake fillers need to go.
    float BMR_w(0), RF_w(0);
    if (prob < minProbEffHigh) {
      BMR_w = 1.0;
    }
    if (prob < minProbEffReallyLow) {
      RF_w = 1.0;
    }
    m_LargeD0Plots->fillBMR(*thisTrack, BMR_w);
    m_LargeD0Plots->fillRF(*thisTrack, RF_w);

    bool isFake = (prob < minProbEffLow);
    int barcode = (associatedTruth ? associatedTruth->barcode() : -1);

    // * Fake rate plots, using 'fake' flag.
    m_LargeD0Plots->fillFakeRate(*thisTrack, isFake, barcode, trtHit, trtOut, nTrtHits, nTrtTubeHits);

    if (isFake) {
      m_LargeD0Plots->fillFake(*thisTrack, barcode, *eventinfo, *privert);
    } else {
      // * Distributions for non-fake tracks.
      m_LargeD0Plots->fill(*thisTrack, barcode, *eventinfo, *privert);
    }
  } // END: Main track loop.


  // =======================================================================
  // Main truth loop.
  // -------------------------------------------------------------------------
  /**
   * This is the beginning of the nested Loop, built mainly for the Efficiency Plots.
   */
  if (truthParticles) {
    // Outer loop: All truth particles.
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    for (const auto& thisTruth : *truthParticles) {
      if (thisTruth->pt() < 0.001) {
        continue;
      }
      if (!m_truthSelectionTool->accept(thisTruth)) {
        continue;
      }

      bool reconstructed(false), largeD0Track(false), trtHit(false), trtOut(false);
      float bestMatch(0.0);
      bool addsToEfficiency(true); // weight for the trackeff histos
      uint8_t iTrtHits(0), iTrtOutliers(0), iTrtTubeHits(0);
      int nTrtHits(0), nTrtTubeHits(0);

      m_LargeD0Plots->fillTruth(*thisTruth);

      // * Vector of pairs: <truth_matching_probability, track> if prob > minProbEffLow (0.5).
      std::vector <pair<float, const xAOD::TrackParticle*> > prospects;

      // Inner loop: All track particles.
      for (const auto& thisTrack : *ptracks) {
        if (m_useTrackSelection) {
          // * 0 means z0, d0 cut is wrt. beam spot - put in a PV to change this.
          // if ( !(m_trackSelectionTool->accept(*thisTrack, 0)) ) { continue; }
          if (isLargeD0Track(thisTrack) && !(m_trackSelectionTool->accept(thisTrack))) {
            continue;
          } // @asogaard
        }
        // * Not an inside-out track.
        // if (m_onlyInsideOutTracks and (not isInsideOut(*thisTrack))) { continue; }

        // * Get associated truth particle and probability.
        const xAOD::TruthParticle* associatedTruth = getTruthPtr(*thisTrack);
        float prob = getMatchingProbability(*thisTrack);

        // * If the associated truth particle matches the current particle in the truth particle loop...
        if (associatedTruth && associatedTruth == thisTruth) {
          // * Determine TRT hits, outliers, and tube hits for truth associated tracks
          if (thisTrack->summaryValue(iTrtHits, xAOD::numberOfTRTHits)) {
            if (iTrtHits > 0) {
              trtHit = true;
              nTrtHits = (int) iTrtHits;
            }
          }
          if (thisTrack->summaryValue(iTrtOutliers, xAOD::numberOfTRTOutliers)) {
            if (iTrtOutliers > 0) {
              trtOut = true;
            }
          }
          if (thisTrack->summaryValue(iTrtTubeHits, xAOD::numberOfTRTTubeHits)) {
            if (iTrtTubeHits > 0) {
              nTrtTubeHits = (int) iTrtTubeHits;
            }
          }
          // * ... as a standard or largeD0 track.
          largeD0Track = isLargeD0Track(thisTrack);
          bestMatch = std::max(prob, bestMatch);
          if (prob > minProbEffLow) {
            prospects.push_back(make_pair(prob, thisTrack));
            // * Current truth particle has been reconstructed...
            reconstructed = true;
          }
        }
      }

      // @asogaard: Not touched.
      // int deg_count = prospects.size();//unused
      if (bestMatch >= minProbEffHigh) {
        ++num_truthmatch_match;
        const xAOD::TruthParticle* assoc_Truth = getTruthPtr(*prospects.at(0).second);
        if (!assoc_Truth) {
          continue;
        }
        m_LargeD0Plots->fill(*assoc_Truth); // This is filling truth-only plots: m_TrackTruthInfoPlots
      } else {
        addsToEfficiency = false;
      }
      m_LargeD0Plots->fillEfficiency(*thisTruth, addsToEfficiency);
      // end of bestMatch >= minProbEffHigh

      m_LargeD0Plots->fillEfficiency(*thisTruth, reconstructed, largeD0Track, trtHit, trtOut, nTrtHits, nTrtTubeHits, isSignal(
                                       thisTruth));
    } // END: loop truthParticles
  } // END: if truthParticles

  if (num_truthmatch_match == 0) {
    ATH_MSG_DEBUG("NO TRACKS had associated truth.");
  } else {
    ATH_MSG_DEBUG(num_truthmatch_match << " tracks out of " << nTracks << " had associated truth.");
  }

  m_LargeD0Plots->fillCounter(nSelectedTracks, InDetPerfPlot_nTracks::SELECTED);
  m_LargeD0Plots->fillCounter(nTracks, InDetPerfPlot_nTracks::ALL);
  m_LargeD0Plots->fillCounter(nTruth, InDetPerfPlot_nTracks::TRUTH);
  m_LargeD0Plots->fillCounter(num_truthmatch_match, InDetPerfPlot_nTracks::TRUTH_MATCHED);

  return StatusCode::SUCCESS;
}

StatusCode
InDetPhysValLargeD0Tool::bookHistograms() {
  ATH_MSG_INFO("Booking hists " << name() << "...");
  m_LargeD0Plots->setDetailLevel(100); // DEBUG, enable expert histograms
  m_LargeD0Plots->initialize();
  std::vector<HistData> hists = m_LargeD0Plots->retrieveBookedHistograms();
  for (auto hist : hists) {
    ATH_CHECK(regHist(hist.first, hist.second, all)); // ??
  }
  return StatusCode::SUCCESS;
}

StatusCode
InDetPhysValLargeD0Tool::procHistograms() {
  ATH_MSG_INFO("Finalising hists " << name() << "...");
  if (endOfRunFlag()) {
    m_LargeD0Plots->finalize();
  }
  ATH_MSG_INFO("Successfully finalized hists");
  return StatusCode::SUCCESS;
}

// LRT specific functions.
// -------------------------------------------------------------------

bool
InDetPhysValLargeD0Tool::isLargeD0Track(const xAOD::TrackParticle* tp) {
  const std::bitset<xAOD::NumberOfTrackRecoInfo> patternReco = tp->patternRecoInfo();

  if (patternReco.test(49)) {
    return true;
  }
  return false;
}

bool
InDetPhysValLargeD0Tool::isSignal(const xAOD::TruthParticle* p) {
  if (m_signalIds.empty()) {
    return false;
  }
  if ((p != NULL) &&         \
      (p->status() == 1) &&  \
      (p->hasProdVtx()) &&   \
      (!p->hasDecayVtx()) && \
      //     (p->nParents() > 0) &&		\ //taken out because not all signatures have tracks that stop decaying the way
      // muons do
      (p->isCharged())) {
    const xAOD::TruthParticle* parent = p->parent();
    if (parent == NULL) {
      return false;
    }
    do {
      if ((std::find(m_signalIds.begin(),          \
                     m_signalIds.end(),                                \
                     parent->absPdgId()) != m_signalIds.end())) {
        return true;
      }
      parent = parent->parent();
    } while ((parent != NULL) &&                  \
             (parent->hasProdVtx()) &&            \
             (parent->nParents() > 0));
  }
  return false;
}

bool
InDetPhysValLargeD0Tool::MinTrackSelection(const xAOD::TrackParticle* tp) {
  float maxEta = 2.5;
  float minPt = /*1000.*/ 0; // Putting in this minPt cut actually increases the fake rate, so I am putting back to

  // zero.

  if ((tp->pt() > 1e-7 ? (fabs(tp->eta()) < maxEta) : false) &&  \
      (tp->pt() > minPt)) {
    return true;
  } else {
    return false;
  }
}

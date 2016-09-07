/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPhysValMonitoringTool.cxx
 * @author shaun roe
 **/

#include "InDetPhysValMonitoring/InDetPhysValMonitoringTool.h"

#include <vector>
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
#include "InDetRttPlots.h"
#include "InDetPerfPlot_nTracks.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODJet/JetContainer.h"
#include "PATCore/TAccept.h"
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#include "TrackTruthSelectionTool.h"
#include <limits>
#include <cmath> // to get std::isnan()
#include <utility>


namespace { // utility functions used here
  // sort method for prospects
  bool
  sortProspects(std::pair<float, const xAOD::TrackParticle *> pair1,
                std::pair<float, const xAOD::TrackParticle *> pair2) {
    return(pair1.first < pair2.first);
  }

  // get truth particle associated with a track particle
  const xAOD::TruthParticle *
  getTruthPtr(const xAOD::TrackParticle &trackParticle) {
    typedef ElementLink<xAOD::TruthParticleContainer> ElementTruthLink_t;
    const xAOD::TruthParticle *result(nullptr);
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
  getMatchingProbability(const xAOD::TrackParticle &trackParticle) {
    float result(std::numeric_limits<float>::quiet_NaN());

    if (trackParticle.isAvailable<float>("truthMatchProbability")) {
      result = trackParticle.auxdata<float>("truthMatchProbability");
    }
    return result;
  }

  bool
  isInsideOut(const xAOD::TrackParticle &track) {
    std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo>  patternInfo = track.patternRecoInfo();

    return patternInfo.test(0);
  }

  bool
  passJetCuts(const xAOD::Jet &jet) {
    float etaMin = -2.5;
    float etaMax = 2.5;
    float jetPtMin = 100;  // in GeV
    float jetPtMax = 1000; // in GeV
    float jetPt = jet.pt() / 1e3; // GeV

    if (jetPt < jetPtMin) {
      return false;
    }
    if (jetPt > jetPtMax) {
      return false;
    }
    float eta = jet.eta();
    if (eta < etaMin) {
      return false;
    }
    if (eta > etaMax) {
      return false;
    }

    return true;
  }
}// namespace

///Parametrized constructor
InDetPhysValMonitoringTool::InDetPhysValMonitoringTool(const std::string &type, const std::string &name,
                                                       const IInterface *parent) :
  ManagedMonitorToolBase(type, name, parent),
  m_useTrackSelection(false),
  m_onlyInsideOutTracks(false),
  m_trackSelectionTool("InDet::InDetTrackSelectionTool/TrackSelectionTool"),
  m_truthSelectionTool("TrackTruthSelectionTool/TruthSelectionTool"),
  //m_geoUtilsTool("GeoUtilsTool/GeoTool"),
  m_prospectsMatched(6, 0),
  m_twoMatchedEProb(0),
  m_threeMatchedEProb(0),
  m_fourMatchedEProb(0),
  m_truthCounter(0),
  m_fillTIDEPlots(false),
  m_fillExtraTIDEPlots(false) {
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
}

InDetPhysValMonitoringTool::~InDetPhysValMonitoringTool() {
}

StatusCode
InDetPhysValMonitoringTool::initialize() {

  ATH_MSG_DEBUG("Initializing " << name() << "...");
  ATH_CHECK(ManagedMonitorToolBase::initialize());

    // get geometry from input file metadata
  std::string _DetDescrVersion="";
  ATH_CHECK(AthAnalysisHelper::retrieveMetadata("/Digitization/Parameters","DetDescrVersion",_DetDescrVersion));
  if (""==_DetDescrVersion) {
    ATH_MSG_ERROR("\033[1;31mCannot infer Detector Geometry from Metadata\033[0m\n");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Detector geometry from Metadata : "<< _DetDescrVersion);

  /*
  ATH_CHECK(m_geoUtilsTool.retrieve());
  if (not m_geoUtilsTool) {
    ATH_MSG_ERROR(
		  "\033[1;31mCould not retrieve the tool needed to set geometry info.\033[0m\n");
    return StatusCode::FAILURE;
  }
  std::string _DetDescrVersion="";
  // the folder in the xAOD metadata where the key is:
  std::string _geoMetaFold=m_geoUtilsTool->getMetaFolder();
  // metadata key needed for extracting geometry information:
  std::string _geoMetaKey=m_geoUtilsTool->getMetaKey();
  ATH_CHECK(AthAnalysisHelper::retrieveMetadata(_geoMetaFold,_geoMetaKey,_DetDescrVersion));
  if (""==_DetDescrVersion) {
    ATH_MSG_ERROR("\033[1;31mCannot infer Detector Geometry from Metadata\033[0m\n");
    return StatusCode::FAILURE;
  }
  m_geoUtilsTool->setDetDesc(_DetDescrVersion);
  m_geoUtilsTool->initialize();
  ATH_MSG_DEBUG("Detector geometry from Metadata : "<< _DetDescrVersion); 
  std::cout << "LM _DetDescrVersion " << _DetDescrVersion << std::endl;
  */

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
  if (not m_truthSelectionTool) {
    ATH_MSG_ERROR("\033[1;31mFailed to retrieve truthSelectionTool\033[0m\n");
    return StatusCode::FAILURE;
  }
  // set geometry-specific truth track selection
  std::string _geoITk="ATLAS-P2";
  if (_DetDescrVersion.find(_geoITk) != std::string::npos ) {
    ATH_MSG_DEBUG("Got ITk geometry. Setting track selection accordingly.");
    m_truthSelectionTool->setGeoTrackTruthSelection( 4.0 , 260. );    
  }

  m_monPlots = std::move(std::unique_ptr<InDetRttPlots> (new InDetRttPlots(0, m_dirName + m_folder)));
  m_monPlots->SetFillExtraTIDEPlots(m_fillExtraTIDEPlots);

  return StatusCode::SUCCESS;
}

StatusCode
InDetPhysValMonitoringTool::fillHistograms() {
  ATH_MSG_DEBUG("Filling hists " << name() << "...");
  // retrieve trackParticle container
  auto ptracks = getContainer<xAOD::TrackParticleContainer>(m_trkParticleName);
  // const xAOD::TruthParticleContainer* truthParticles = (!m_truthParticleName.empty() ?
  // getContainer<xAOD::TruthParticleContainer>(m_truthParticleName) : nullptr);
  if ((!ptracks)) {
    return StatusCode::FAILURE;
  }
  // retrieve truthParticle container
  std::vector<const xAOD::TruthParticle *> truthParticlesVec;
  getTruthParticles(truthParticlesVec);

  ATH_MSG_DEBUG("Filling vertex plots");
  const xAOD::VertexContainer *pvertex = getContainer<xAOD::VertexContainer>(m_vertexContainerName);
  const xAOD::Vertex *pvtx = 0;
  if (pvertex) {
    for (const auto &vtx : pvertex->stdcont()) {
      if (vtx->vertexType() == xAOD::VxType::NoVtx) {
        continue; // skip dummy vertex
      }
      //   if (vtx->vertexType() == xAOD::VxType::PriVtx) ATH_MSG_INFO("PV x/y/z " << vtx->x() << "," <<vtx->y() << ","
      // << vtx->z() );
      pvtx = vtx;
    }

    m_monPlots->fill(*pvertex);
  } else {
    ATH_MSG_WARNING("Cannot open " << m_vertexContainerName << " vertex container. Skipping vertexing plots.");
  }
  ATH_MSG_DEBUG("Filling vertex/event info monitoring plots");
  const xAOD::EventInfo *pei = getContainer<xAOD::EventInfo>(m_eventInfoContainerName);
  if (pei) {
    if (pvertex) {
      m_monPlots->fill(*pvertex, *pei);
    }
  } else {
    ATH_MSG_WARNING(
      "Cannot open " << m_eventInfoContainerName << " EventInfo container. Skipping vertexing plots using EventInfo.");
  }

  // get truth vertex container name - m_truthVertexContainerName
  const xAOD::TruthVertexContainer *truthVrt = getContainer<xAOD::TruthVertexContainer>(m_truthVertexContainerName);
  const xAOD::TruthVertex *truthVertex = 0;
  if (truthVrt) {
    if (!m_TrkSelectPV) {
      ATH_MSG_INFO("size of TruthVertex container = " << truthVrt->size());
    }

    for (const auto &vtx : truthVrt->stdcont()) {
      if (!m_TrkSelectPV) {
        ATH_MSG_INFO("TruthVertex XYZ = " << vtx->x() << "," << vtx->y() << "," << vtx->z());
      }
      float vrtR = sqrt(vtx->x() * vtx->x() + vtx->y() * vtx->y());

      if (vrtR > 1 || abs(vtx->z()) > 500) {
        continue;
      }
      truthVertex = vtx;
    }
  }else {
    ATH_MSG_WARNING("Cannot open " << m_truthVertexContainerName << " truth vertex container");
  }

  unsigned int num_truth_selected(0), nSelectedTracks(0), num_truthmatch_match(0);
  // const unsigned int nTracks(ptracks->size()); //unused
  // const unsigned int nTruth(truthParticles  ? truthParticles->size() : 0);

  // the truth matching probability must not be <= 0., otherwise the tool will seg fault in case of missing truth (e.g.
  // data):
  const float minProbEffLow(0.50); // if the probability of a match is less than this, we call it a fake
  //    const float minProbEffHigh(0.80); //if the probability of a match is higher than this, it either feeds the NUM
  // or is a duplicate
  // VJ - Mar 14, 2016 - even for effs, use 0.51
  const float minProbEffHigh(0.501); // if the probability of a match is higher than this, it either feeds the NUM or is
                                     // a duplicate

  // check if we are doing track selection relative to PV (or the default, which is the BS)
  if (!m_TrkSelectPV) {
    pvtx = 0;
  }
  if (pvtx && pvtx->vertexType() == xAOD::VxType::PriVtx) {
    ATH_MSG_DEBUG("PV x/y/z " << pvtx->x() << "," << pvtx->y() << "," << pvtx->z());
  }

  bool fillVtx = true; // fill PV plots in fillSpectrum only once

  // Main track loop, filling Track-only, Track 'n' Truth with good matching probability (meas, res, & pull), and Fakes
  for (const auto &thisTrack: *ptracks) {
    m_monPlots->fillSpectrum(*thisTrack); // This one needs prob anyway, why not rearrange & eliminate
                                          // getMatchingProbability from RttPlots? 5-17-16
    const xAOD::TruthParticle *associatedTruth = getTruthPtr(*thisTrack);
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

    // This is where the BMR, Fake, and Really Fake fillers need to go.
    float BMR_w(0), RF_w(0); // weights for filling the Bad Match & Fake Rate plots
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
    }

    if (associatedTruth) {
      m_monPlots->fillSpectrumLinked(*thisTrack, *associatedTruth);
      if ((prob < minProbEffLow)and(not std::isnan(prob))) {
        const bool isFake = (prob < minProbEffLow);
        m_monPlots->fillFakeRate(*thisTrack, isFake);
      }
      if ((prob > minProbEffLow) and m_truthSelectionTool->accept(associatedTruth)) {
        m_monPlots->fill(*thisTrack, *associatedTruth); // Make all plots requiring both truth & track (meas, res, &
                                                        // pull)
      }
    }
  }
  // Set of temp counters, to be removed in the near future (7-27-16)
  int nTruths(0), nParents(0), nKshorts(0), nConversions(0), nPairs(0), nGoodMC(0);

  // This is the beginning of the Nested Loop, built mainly for the Efficiency Plots
  for (int itruth = 0; itruth < (int) truthParticlesVec.size(); itruth++) {  // Outer loop over all truth particles
    nTruths += 1;
    const xAOD::TruthParticle *thisTruth = truthParticlesVec[itruth];
    bool isParent(false), isKshort(false), isConversion(false), isPair(false), isGoodMC(false), goodTruthMatch(false);
    std::vector<int> barcodes;                           // set of barcodes for parent & daughters
    if (thisTruth->nChildren() > 0) {
      isParent = true;      // is it a parent?
    }
    if (isParent) {
      nParents += 1;
      if (thisTruth->pdgId() == 310) {
        isKshort = true;     // is it a kshort?
        nKshorts += 1;
      }
      if (thisTruth->pdgId() == 22) {
        isConversion = true; // is it a photon?
        nConversions += 1;
      }

      std::vector<const xAOD::TruthParticle *> children;
      if (isKshort xor isConversion) {                     // either way, does it pair-produce (e+e- for photon, pi+pi-
                                                           // for kshort)?
        bool dual(false), type(false);       // bools for number of children, antagonist children, and
        // right particle species
        for (unsigned int ich = 0; ich < thisTruth->nChildren(); ich++) {
          const xAOD::TruthParticle *child = thisTruth->child(ich);
          children.push_back(child);
        }
        if (children.size() == 2) {
          int child_ID1 = children.at(0)->pdgId();
          int child_ID2 = children.at(1)->pdgId();
          if ((child_ID1 + child_ID2 == 0)) {
            dual = true;
          }
          if ((fabs(child_ID1 == 211) or(fabs(child_ID1) == 11))) {
            type = true;
          }
        }
        if (dual and type) {
          isPair = true;
        }
      }

      if (isPair) {
        nPairs += 1;

        float px1 = children.at(0)->px();
        float px2 = children.at(1)->px();
        float px_t = px1 + px2;

        float py1 = children.at(0)->py();
        float py2 = children.at(1)->py();
        float py_t = py1 + py2;

        float pt_t = std::sqrt((px_t * px_t) + (py_t * py_t));

        float base(0);
        if (isKshort) {
          base = 10000.0;
        }else if (isConversion) {
          base = 20000.0;
        }

        float difference = fabs(pt_t - base);
        if (difference < 20.0) {
          isGoodMC = true;  // The truth part is good!
          int parent_bcode = thisTruth->barcode();
          int daughter_bcode1 = children.at(0)->barcode();
          int daughter_bcode2 = children.at(1)->barcode();
          barcodes.push_back(parent_bcode);
          barcodes.push_back(daughter_bcode1);
          barcodes.push_back(daughter_bcode2);
        }
      }
    }// End of isParent if-statement
     /**/
    if (isGoodMC) {
      nGoodMC += 1;
      float daughter_w(0);                                 // weight for the "of_daughters" efficiency plots
      for (const auto &thisTrack: *ptracks) {
        if (m_onlyInsideOutTracks)  continue;                // If inside out only then we clearly aren't doing backtracking!        
        const xAOD::TruthParticle *associatedTruth = getTruthPtr(*thisTrack);
        if (associatedTruth) {
          int ast_barcode = associatedTruth->barcode();
          if ((ast_barcode != barcodes.at(0))and((ast_barcode == barcodes.at(1)) xor (ast_barcode == barcodes.at(2)))) {
            goodTruthMatch = true;
          }
          if (goodTruthMatch) {
            daughter_w = 1;
          }
        }
      }
      m_monPlots->BT_fill(*thisTruth, daughter_w);
    }
    /**/
    m_monPlots->fillSpectrum(*thisTruth);
    Root::TAccept accept = m_truthSelectionTool->accept(thisTruth);
    fillTruthCutFlow(accept);
    if (accept) {
      ++m_truthCounter; // total number of truth tracks which pass cuts
      ++num_truth_selected; // total number of truth which pass cuts per event
      // LMTODO add this Jain/Swift

      float PF_w(1); // weight for the trackeff histos

      float bestMatch = 0;
      std::vector <std::pair<float, const xAOD::TrackParticle *> > prospects; // Vector of pairs:
                                                                              // <truth_matching_probability, track> if
                                                                              // prob > minProbEffLow (0.5)
      for (const auto &thisTrack: *ptracks) { // Inner loop over all track particle
        if (m_useTrackSelection) {
          if (!(m_trackSelectionTool->accept(*thisTrack, pvtx))) {
            continue;
          }
        }
        if (m_onlyInsideOutTracks and(not isInsideOut(*thisTrack))) {
          continue;  // not an inside-out track
        }
        const xAOD::TruthParticle *associatedTruth = getTruthPtr(*thisTrack); // get the associated truth for this track
        if (associatedTruth && associatedTruth == thisTruth) {
          float prob = getMatchingProbability(*thisTrack);
          if (not std::isnan(prob)) {
            bestMatch = std::max(prob, bestMatch);
            if (prob > minProbEffLow) {
              prospects.push_back(std::make_pair(prob, thisTrack));
            }
            const bool isFake = (prob < minProbEffLow);
            m_monPlots->fillFakeRate(*thisTrack, isFake);
          }
        }
      }
      // count number of prospects and increment entry in vector for this event
      int deg_count = prospects.size();
      if (deg_count <= 4) {
        ++m_prospectsMatched[deg_count];
      } else {
        ++m_prospectsMatched[5];
      }

      // special histograms for 1 or 2 matched tracks
      if (deg_count == 1) {
        m_monPlots->fillSingleMatch(*prospects.at(0).second);
      } else if (deg_count == 2) {
        m_monPlots->fillTwoMatchDuplicate(prospects[1].first, prospects[0].first,
                                          *prospects[1].second, *prospects[0].second,
                                          *thisTruth);
      }
      // determine how many duplicate match probabilities
      // first create vector of probabilities and sort
      if (deg_count > 1) {
        std::vector<float> probs;
        for (int i = 0; i < deg_count; i++) {
          probs.push_back(prospects.at(i).first);
        }
        sort(prospects.begin(), prospects.end(), sortProspects);

        // count duplicates
        float prev = prospects[0].first;
        int nduplicates = 0;
        for (int i = 1; i < deg_count; i++) {
          bool duplicate = std::fabs(prospects[i].first - prev) < 1.e-9;
          if (duplicate) {
            ++nduplicates;
          }
          if (!duplicate || i == deg_count - 1) {
            if (nduplicates >= 1) {
              if (deg_count == 2) {
                ++m_twoMatchedEProb;
              } else if (deg_count == 3) {
                ++m_threeMatchedEProb;
              } else if (deg_count == 4) {
                ++m_fourMatchedEProb;
              }
            }
            nduplicates = 0;
            prev = prospects[i].first;
          }
        }
      }

      // fill truth-only plots
      if (bestMatch >= minProbEffHigh) {
        ++num_truthmatch_match;
        const xAOD::TruthParticle *assoc_Truth = getTruthPtr(*prospects.back().second);
        if (!assoc_Truth) {
          continue;
        }
        m_monPlots->fill(*assoc_Truth); // This is filling truth-only plots:  m_TrackTruthInfoPlots
      }else {
        PF_w = 0;
      }
      m_monPlots->pro_fill(*thisTruth, PF_w);                                               
    } // end of the "if(accept)" loop?
  }// End of Big truthParticle loop

  ATH_MSG_INFO("Rey: the set of counters is here");
  ATH_MSG_INFO("Number of TruthParticles: " << nTruths);
  ATH_MSG_INFO("Number of Parents: " << nParents);
  ATH_MSG_INFO("Number of Kshorts: " << nKshorts);
  ATH_MSG_INFO("Number of Conversions: " << nConversions);
  ATH_MSG_INFO("Number of Pairs: " << nPairs);
  ATH_MSG_INFO("Number of GoodMCs: " << nGoodMC);
  // This is the end of the Nested Loop approach section

  if (m_useTrackSelection) {
    for (const auto &thisTrack: *ptracks) { // Inner loop over all track particle
      if (m_useTrackSelection) {
        Root::TAccept trackAccept = m_trackSelectionTool->accept(*thisTrack, pvtx);
        fillTrackCutFlow(trackAccept);
      }
    }
  }

  if (num_truthmatch_match == 0) {
    ATH_MSG_DEBUG("NO TRACKS had associated truth.");
  } else {
    ATH_MSG_DEBUG(num_truthmatch_match << " tracks out of " << ptracks->size() << " had associated truth.");
  }
  double ifr = (double) nSelectedTracks / (double) num_truth_selected;
  m_monPlots->fillCounter(nSelectedTracks, InDetPerfPlot_nTracks::SELECTED);
  m_monPlots->fillCounter(ptracks->size(), InDetPerfPlot_nTracks::ALL);
  m_monPlots->fillCounter(truthParticlesVec.size(), InDetPerfPlot_nTracks::TRUTH);
  m_monPlots->fillCounter(num_truthmatch_match, InDetPerfPlot_nTracks::TRUTH_MATCHED);

  m_monPlots->fillIncFake(ptracks->size(), ifr, nSelectedTracks);

  if (m_fillTIDEPlots && !m_jetContainerName.empty()) {
    ATH_MSG_DEBUG("Getting jet Container");
    const xAOD::JetContainer *jets = getContainer<xAOD::JetContainer>(m_jetContainerName);
    ATH_MSG_DEBUG("Getting Truth Container");
    const xAOD::TruthParticleContainer *truthParticles = getContainer<xAOD::TruthParticleContainer>("TruthParticles");
    ATH_MSG_VERBOSE("Truth Container obtained");
    if (!jets || !truthParticles) {
      ATH_MSG_WARNING(
        "Cannot open " << m_jetContainerName <<
        " jet container or TruthParticles truth particle container. Skipping jet plots.");
    } else {
      for (const auto &thisJet: *jets) {         // The big jets loop
        if (not passJetCuts(*thisJet)) {
          continue;
        }
        ATH_MSG_VERBOSE("Got a jet");
        for (const auto thisTrack: *ptracks) {    // The beginning of the track loop
          ATH_MSG_VERBOSE("looping over the tracks now");
          if (m_useTrackSelection and not (m_trackSelectionTool->accept(*thisTrack, pvtx))) {
            continue;
          }
          ATH_MSG_VERBOSE("track in acceptance");
          if (m_onlyInsideOutTracks and !(isInsideOut(*thisTrack))) {
            continue; // not an inside out track
          }
          ATH_MSG_VERBOSE("track insideout accepted");
          if (thisJet->p4().DeltaR(thisTrack->p4()) > m_maxTrkJetDR) {
            continue;
          }
          //
          ATH_MSG_VERBOSE("Filling trkInJet");
          const bool safelyFilled = m_monPlots->filltrkInJetPlot(*thisTrack, *thisJet);
          ATH_MSG_VERBOSE("Is it safely filled? 0 means no, 1 means yes " << safelyFilled);
          if (safelyFilled) {
            float trkInJet_w(0), trkInJet_BMR(1);
            float prob = getMatchingProbability(*thisTrack);
            ATH_MSG_VERBOSE("got matching");
            if (prob > minProbEffHigh) {
              trkInJet_w = 1;
              trkInJet_BMR = 0;
            }
            ATH_MSG_VERBOSE("Filling in three jet plots");
            m_monPlots->jet_fill(*thisTrack, *thisJet, trkInJet_w);                          // fill trkinjeteff plots
            m_monPlots->jetBMR(*thisTrack, *thisJet, trkInJet_BMR);                          // fin in track in jet bad
                                                                                             // match rate plots
            m_monPlots->fillSimpleJetPlots(*thisTrack, prob);                                      // Fill all the
                                                                                                   // simple jet plots
            const xAOD::TruthParticle *associatedTruth = getTruthPtr(*thisTrack);           // Get the associated truth
                                                                                            // for this track
            if (associatedTruth) {
              ATH_MSG_VERBOSE("Found associated truth");
              m_monPlots->fillJetResPlots(*thisTrack, *associatedTruth, *thisJet);             // Fill jet pull &
                                                                                               // resolution plots
              int barcode = associatedTruth->barcode();
              m_monPlots->fillJetHitsPlots(*thisTrack, prob, barcode);                         // Fill the two extra
                                                                                               // plots
              if (m_truthSelectionTool->accept(associatedTruth)) {                            // Fill the Jet plots with
                                                                                              // "Eff" in the name
                m_monPlots->fillJetEffPlots(*associatedTruth, *thisJet);
              }
            } else {
              ATH_MSG_VERBOSE("No associated truth");
            }
          }
        } // end of track loop
        ATH_MSG_DEBUG("Finished loop over Jets");
        // fill in things like sum jet pT in dR bins - need all tracks in the jet first
        m_monPlots->fillJetPlotCounter(*thisJet);
        for (const auto &thisTruth: *truthParticles) {
          // for primary tracks we want an efficiency as a function of track jet dR
          if ((m_truthSelectionTool->accept(thisTruth) and(thisJet->p4().DeltaR(thisTruth->p4()) < m_maxTrkJetDR))) {
            m_monPlots->fillJetTrkTruth(*thisTruth, *thisJet);
          }
        }
        m_monPlots->fillJetTrkTruthCounter(*thisJet);
      } // loop over jets
    }   // if have collections needed
  }     // if TIDE

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
    " total number of truth particles which match a reco track (including duplicates) = " << m_prospectsMatched[1] +
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
  ATH_MSG_INFO("");
  for (int i = 0; i < (int) m_truthCutflow.size(); i++) {
    ATH_MSG_INFO("number after " << m_truthCutflowNames[i] << ": " << m_truthCutflow[i]);
  }
  if (endOfRunFlag()) {
    m_monPlots->finalize();
  }
  ATH_MSG_INFO("Successfully finalized hists");
  return StatusCode::SUCCESS;
}

void
InDetPhysValMonitoringTool::getTruthParticles(std::vector<const xAOD::TruthParticle *> &truthParticles) {
  truthParticles.clear();
  if (m_pileupSwitch == "All") {
    std::string m_truthContainerName = "TruthParticles";
    const xAOD::TruthParticleContainer *truthParticleContainer =
      (!m_truthParticleName.empty() ? getContainer<xAOD::TruthParticleContainer>(m_truthParticleName) : nullptr);
    if (!truthParticleContainer) {
      return;
    }
    for (const auto &thisTruth: *truthParticleContainer) {
      truthParticles.push_back(thisTruth);
    }
  }else {
    if (m_pileupSwitch == "HardScatter") {
      // get truthevent container to separate out pileup and hardscatter truth particles
      const xAOD::TruthEventContainer *truthEventContainer = 0;
      const char *truthEventCollName =
        evtStore()->contains<xAOD::TruthEventContainer>("TruthEvents") ? "TruthEvents" : "TruthEvent";
      evtStore()->retrieve(truthEventContainer, truthEventCollName);
      auto event = truthEventContainer->at(0);

      // get truth particles from first event only
      int ntruth = event->nTruthParticles();
      ATH_MSG_VERBOSE("looping over " << ntruth << " truth particles in TruthEvents container");
      for (int j = 0; j < ntruth; j++) {
        const xAOD::TruthParticle *thisTruth = event->truthParticle(j);
        truthParticles.push_back(thisTruth);
      }
    }else if (m_pileupSwitch == "PileUp") {
      ATH_MSG_VERBOSE("getting TruthPileupEvents container");
      // get truth particles from all pileup events
      const xAOD::TruthPileupEventContainer *truthPileupEventContainer = 0;
      const char *truthPUEventCollName =
        evtStore()->contains<xAOD::TruthPileupEventContainer>("TruthPileupEvents") ? "TruthPileupEvents" :
        "TruthPileupEvent";
      evtStore()->retrieve(truthPileupEventContainer, truthPUEventCollName);

      if (truthPileupEventContainer != NULL) {
        for (int i = 0; i < (int) truthPileupEventContainer->size(); ++i) {
          auto eventPileup = truthPileupEventContainer->at(i);
          // get truth particles from each pileup event
          int ntruth = eventPileup->nTruthParticles();
          ATH_MSG_VERBOSE("looping over " << ntruth << " truth particles in TruthPileupEvents container");
          for (int j = 0; j < ntruth; ++j) {
            const xAOD::TruthParticle *thisTruth = eventPileup->truthParticle(j);
            truthParticles.push_back(thisTruth);
          }
        }
      } else {
        ATH_MSG_ERROR("no entries in TruthPileupEvents container!");
      }
    } else {
      ATH_MSG_ERROR("bad value for PileUpSwitch");
    }
  }

  ATH_MSG_VERBOSE("returning " << truthParticles.size() << " truth particles");
  return;
}

void
InDetPhysValMonitoringTool::fillTrackCutFlow(Root::TAccept &accept) {
  fillCutFlow(accept, m_trackCutflowNames, m_trackCutflow);
  return;
}

void
InDetPhysValMonitoringTool::fillTruthCutFlow(Root::TAccept &accept) {
  fillCutFlow(accept, m_truthCutflowNames, m_truthCutflow);
  return;
}

void
InDetPhysValMonitoringTool::fillCutFlow(Root::TAccept &accept, std::vector<std::string> &names,
                                        std::vector<int> &cutFlow) {
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
  cutFlow[0] = cutFlow[0] + 1;
  bool cutPositive = true;
  for (unsigned int i = 0; i != (accept.getNCuts() + 1); ++i) {
    if (!cutPositive) {
      continue;
    }
    if (accept.getCutResult(i)) {
      cutFlow[i + 1] = cutFlow[i + 1] + 1;
    } else {
      cutPositive = false;
    }
  }
  return;
}

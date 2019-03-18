/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**    @file SCTLorentzMonTool.cxx
 *
 *    @author Elias Coniavitis based on code from Luca Fiorini,
 *    Shaun Roe, Manuel Diaz, Rob McPherson & Richard Batley
 *    Modified by Yuta
 */
#include "SCT_Monitoring/SCTLorentzMonTool.h"

#include "SCT_NameFormatter.h"

#include "AthContainers/DataVector.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetPrepRawData/SiCluster.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/ReadHandle.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrackSummary/TrackSummary.h" // for sct residuals

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IToolSvc.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TProfile2D.h"
#include "TF1.h"

#include <cmath>
#include <type_traits>

using namespace std;
using namespace Rec;
using namespace SCT_Monitoring;

// ====================================================================================================
/** Constructor, calls base class constructor with parameters
 *
 *  several properties are "declared" here, allowing selection
 *  of the filepath for histograms etc
 */
// ====================================================================================================
SCTLorentzMonTool::SCTLorentzMonTool(const string& type, const string& name,
                                     const IInterface* parent) : SCTMotherTrigMonTool(type, name, parent),
  m_trackToVertexTool("Reco::TrackToVertex", this), // for TrackToVertexTool
  m_phiVsNstrips{},
  m_phiVsNstrips_100{},
  m_phiVsNstrips_111{},
  m_phiVsNstrips_Side{},
  m_phiVsNstrips_Side_100{},
  m_phiVsNstrips_Side_111{},
  m_pSCTHelper(nullptr) {
  /** sroe 3 Sept 2015:
     histoPathBase is declared as a property in the base class, assigned to m_path
     with default as empty string.
     Declaring it here as well gives rise to compilation warning
     WARNING duplicated property name 'histoPathBase', see https://its.cern.ch/jira/browse/GAUDI-1023

     declareProperty("histoPathBase", m_stream = "/stat"); **/
  m_stream = "/stat";
  declareProperty("tracksName", m_tracksName = std::string("CombinedInDetTracks")); // this recommended
  declareProperty("TrackToVertexTool", m_trackToVertexTool); // for TrackToVertexTool
  m_numberOfEvents = 0;
}

// ====================================================================================================
// ====================================================================================================
StatusCode SCTLorentzMonTool::initialize() {
  ATH_CHECK( SCTMotherTrigMonTool::initialize() );

  ATH_CHECK(detStore()->retrieve(m_pSCTHelper, "SCT_ID"));

  ATH_CHECK( m_tracksName.initialize() );
  ATH_CHECK(m_SCTDetEleCollKey.initialize());

  ATH_CHECK(m_trackToVertexTool.retrieve());

  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                       SCTLorentzMonTool :: bookHistograms
// ====================================================================================================
StatusCode
SCTLorentzMonTool::bookHistogramsRecurrent( ) {
  m_path = "";
  if (newRunFlag()) {
    m_numberOfEvents = 0;
  }
  ATH_MSG_DEBUG("initialize being called");
  // Booking  Track related Histograms
  if (bookLorentzHistos().isFailure()) {
    ATH_MSG_WARNING("Error in bookLorentzHistos()");
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                       SCTLorentzMonTool :: bookHistograms
// ====================================================================================================
StatusCode
SCTLorentzMonTool::bookHistograms( ) {
  m_path = "";
  m_numberOfEvents = 0;
  ATH_MSG_DEBUG("initialize being called");
  /* Retrieve TrackToVertex extrapolator tool */
  ATH_CHECK(m_trackToVertexTool.retrieve());
  // Booking  Track related Histograms
  if (bookLorentzHistos().isFailure()) {
    ATH_MSG_WARNING("Error in bookLorentzHistos()");
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                        SCTLorentzMonTool :: fillHistograms
/// This is the real workhorse, called for each event. It retrieves the data each time
// ====================================================================================================
StatusCode
SCTLorentzMonTool::fillHistograms() {
  // should use database for this!
  constexpr int layer100[] = {
    2, 2, 3, 2, 2, 2, 0, 2, 3, 2, 0, 2, 3, 2, 3, 2, 0, 2, 3, 0, 2, 0, 2, 3, 2, 2, 2, 0, 0, 0, 0, 0, 0, 3, 0, 3, 2, 0, 2,
    2, 0, 3, 3, 3, 0, 2, 2, 2, 2, 2, 2, 2, 3, 2, 2, 3, 3, 2, 2, 2, 2, 2, 3, 3, 2, 3, 2, 2, 2, 3, 3, 3, 2, 2, 2, 2, 3, 3,
    2, 3, 2, 3, 3, 2, 3, 2, 2, 2, 2, 2, 2, 2
  };
  constexpr int phi100[] = {
    29, 29, 6, 13, 23, 13, 14, 29, 9, 29, 14, 29, 9, 29, 39, 32, 21, 32, 13, 22, 32, 22, 32, 13, 32, 32, 32, 20, 20, 20,
    20, 20, 20, 13, 21, 17, 33, 5, 33, 33, 31, 6, 19, 47, 21, 37, 37, 37, 37, 33, 37, 37, 24, 33, 33, 47, 19, 33, 33,
    37, 37, 37, 55, 9, 38, 24, 37, 38, 8, 9, 9, 26, 38, 38, 38, 38, 39, 39, 38, 11, 45, 54, 54, 24, 31, 14, 47, 45, 47,
    47, 47, 47
  };
  constexpr int eta100[] = {
    3, -4, -6, 2, 6, 3, -5, -1, 6, -2, -6, -5, 5, -3, 2, 6, -3, 5, 5, 3, 4, 2, 2, 2, -1, -3, -4, 1, -1, -2, -3, -4, 4,
    -1, -5, 6, 2, 4, 3, 1, 6, -2, 6, 3, -6, -1, 2, 1, 3, -5, 4, 5, -3, -4, -3, -5, -2, -1, -2, -3, -2, -4, -3, 2, 3, -6,
    -5, 4, 6, 1, -6, 1, 1, -5, -4, -3, -3, -5, -2, 1, 5, 5, 4, 4, 5, 4, -1, -5, 3, 4, 1, -5
  };
  constexpr unsigned int layer100_n = sizeof(layer100) / sizeof(*layer100);
  constexpr unsigned int phi100_n = sizeof(phi100) / sizeof(*phi100);
  constexpr unsigned int eta100_n = sizeof(eta100) / sizeof(*eta100);
  constexpr bool theseArraysAreEqualInLength = ((layer100_n == phi100_n)and(phi100_n == eta100_n));

  static_assert(theseArraysAreEqualInLength, "Coordinate arrays for <100> wafers are not of equal length");

  ATH_MSG_DEBUG("enters fillHistograms");

  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEle(m_SCTDetEleCollKey);
  const InDetDD::SiDetectorElementCollection* elements(sctDetEle.retrieve());
  if (elements==nullptr) {
    ATH_MSG_WARNING(m_SCTDetEleCollKey.fullKey() << " could not be retrieved");
    return StatusCode::SUCCESS;
  }

  SG::ReadHandle<TrackCollection> tracks(m_tracksName);
  if (not tracks.isValid()) {
    ATH_MSG_WARNING(" TrackCollection not found: Exit SCTLorentzTool" << m_tracksName.key());
    return StatusCode::SUCCESS;
  }
  TrackCollection::const_iterator trkitr = tracks->begin();
  TrackCollection::const_iterator trkend = tracks->end();

  for (; trkitr != trkend; ++trkitr) {
    // Get track
    const Trk::Track* track = (*trkitr);
    if (not track) {
      ATH_MSG_ERROR("no pointer to track!!!");
      continue;
    }

    // Get pointer to track state on surfaces
    const DataVector<const Trk::TrackStateOnSurface>* trackStates = track->trackStateOnSurfaces();
    if (not trackStates) {
      ATH_MSG_WARNING("for current track, TrackStateOnSurfaces == Null, no data will be written for this track");
      continue;
    }

    const Trk::TrackSummary* summary = track->trackSummary();
    if (not summary) {
      ATH_MSG_WARNING(" null trackSummary");
      continue;
    }

    DataVector<const Trk::TrackStateOnSurface>::const_iterator endit = trackStates->end();
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator it = trackStates->begin(); it != endit; ++it) {
      if ((*it)->type(Trk::TrackStateOnSurface::Measurement)) {
        const InDet::SiClusterOnTrack* clus =
          dynamic_cast<const InDet::SiClusterOnTrack*>((*it)->measurementOnTrack());
        if (clus) { // Is it a SiCluster? If yes...
          const InDet::SiCluster* RawDataClus = dynamic_cast<const InDet::SiCluster*>(clus->prepRawData());
          if (not RawDataClus) {
            continue; // Continue if dynamic_cast returns null
          }
          if (RawDataClus->detectorElement()->isSCT()) {
            const Identifier sct_id = clus->identify();
            const int bec(m_pSCTHelper->barrel_ec(sct_id));
            const int layer(m_pSCTHelper->layer_disk(sct_id));
            const int side(m_pSCTHelper->side(sct_id));
            const int eta(m_pSCTHelper->eta_module(sct_id));
            const int phi(m_pSCTHelper->phi_module(sct_id));

            bool in100 = false;
            if (bec != 0) {
              continue; // We only care about the barrel
            }
            // wtf is this?
            for (unsigned int i = 0; i < layer100_n; i++) {
              if (layer100[i] == layer && eta100[i] == eta && phi100[i] == phi) {
                in100 = true;
                break;
              }
            }
            // find cluster size
            const std::vector<Identifier>& rdoList = RawDataClus->rdoList();
            int nStrip = rdoList.size();
            const Trk::TrackParameters* trkp = dynamic_cast<const Trk::TrackParameters*>((*it)->trackParameters());
            if (not trkp) {
              ATH_MSG_WARNING(" Null pointer to MeasuredTrackParameters");
              continue;
            }

            const Trk::Perigee* perigee = track->perigeeParameters();

            if (perigee) {
              // Get angle to wafer surface
              float phiToWafer(90.), thetaToWafer(90.);
              float sinAlpha = 0.; // for barrel, which is the only thing considered here
              float pTrack[3];
              pTrack[0] = trkp->momentum().x();
              pTrack[1] = trkp->momentum().y();
              pTrack[2] = trkp->momentum().z();
              int iflag = findAnglesToWaferSurface(pTrack, sinAlpha, clus->identify(), elements, thetaToWafer, phiToWafer);
              if (iflag < 0) {
                ATH_MSG_WARNING("Error in finding track angles to wafer surface");
                continue; // Let's think about this (later)... continue, break or return?
              }

              bool passesCuts = true;

              if ((AthenaMonManager::dataType() == AthenaMonManager::cosmics) &&
                  (trkp->momentum().mag() > 500.) &&  // Pt > 500MeV
                  (summary->get(Trk::numberOfSCTHits) > 6)// && // #SCTHits >6
                  ) {
                passesCuts = true;
              }
              else if ((track->perigeeParameters()->parameters()[Trk::qOverP] < 0.) && // use negative track only
                       (fabs(perigee->parameters()[Trk::d0]) < 1.) && // d0 < 1mm
                       (fabs(perigee->parameters()[Trk::z0] * sin(perigee->parameters()[Trk::theta])) < 1.) && // d0 <
                                                                                                               // 1mm
                       (trkp->momentum().mag() > 500.) &&  // Pt > 500MeV
                       (summary->get(Trk::numberOfSCTHits) > 6)// && // #SCTHits >6
                       ) {
                passesCuts = true;
              }else {
                passesCuts = false;
              }

              if (passesCuts) {
                // Fill profile
                m_phiVsNstrips[layer]->Fill(phiToWafer, nStrip, 1.);
                m_phiVsNstrips_Side[layer][side]->Fill(phiToWafer, nStrip, 1.);

                if (in100) {
                  // cout << "This event is going to 100" << endl;
                  m_phiVsNstrips_100[layer]->Fill(phiToWafer, nStrip, 1.);
                  m_phiVsNstrips_Side_100[layer][side]->Fill(phiToWafer, nStrip, 1.);
                }else {
                  m_phiVsNstrips_111[layer]->Fill(phiToWafer, nStrip, 1.);
                  m_phiVsNstrips_Side_111[layer][side]->Fill(phiToWafer, nStrip, 1.);
                }
              }// end if passesCuts
            }// end if mtrkp
            //            delete perigee;perigee = 0;
          } // end if SCT..
        } // end if (clus)
      } // if ((*it)->type(Trk::TrackStateOnSurface::Measurement)){
    }// end of loop on TrackStatesonSurface (they can be SiClusters, TRTHits,..)
  } // end of loop on tracks

  m_numberOfEvents++;
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                             SCTLorentzMonTool :: procHistograms
// ====================================================================================================
StatusCode
SCTLorentzMonTool::procHistograms() {
  if (endOfRunFlag()) {
    ATH_MSG_DEBUG("finalHists()");
    ATH_MSG_DEBUG("Total Rec Event Number: " << m_numberOfEvents);
    ATH_MSG_DEBUG("Calling checkHists(true); true := end of run");
    if (checkHists(true).isFailure()) {
      ATH_MSG_WARNING("Error in checkHists(true)");
    }
  }
  ATH_MSG_DEBUG("Exiting finalHists");
  return StatusCode::SUCCESS;
}

StatusCode
SCTLorentzMonTool::checkHists(bool /*fromFinalize*/) {
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                              SCTLorentzMonTool :: bookLorentzHistos
// ====================================================================================================
StatusCode
SCTLorentzMonTool::bookLorentzHistos() {
  const int nLayers(4);
  const int nSides(2);
  string stem = m_path + "/SCT/GENERAL/lorentz/";
  MonGroup Lorentz(this, m_path + "SCT/GENERAL/lorentz", run, ATTRIB_UNMANAGED);

  string hNum[nLayers] = {
    "0", "1", "2", "3"
  };
  string hNumS[nSides] = {
    "0", "1"
  };
  int nProfileBins = 360;

  int success = 1;

  for (int l = 0; l != nLayers; ++l) {
    // granularity set to one profile/layer for now
    int iflag = 0;
    m_phiVsNstrips_100[l] = pFactory("h_phiVsNstrips_100" + hNum[l], "100 - Inc. Angle vs nStrips for Layer " + hNum[l],
                                     nProfileBins, -90., 90., Lorentz, iflag);
    m_phiVsNstrips_111[l] = pFactory("h_phiVsNstrips_111" + hNum[l], "111 - Inc. Angle vs nStrips for Layer " + hNum[l],
                                     nProfileBins, -90., 90., Lorentz, iflag);

    m_phiVsNstrips[l] = pFactory("h_phiVsNstrips" + hNum[l], "Inc. Angle vs nStrips for Layer" + hNum[l], nProfileBins,
                                 -90., 90., Lorentz, iflag);
    m_phiVsNstrips[l]->GetXaxis()->SetTitle("#phi to Wafer");
    m_phiVsNstrips[l]->GetYaxis()->SetTitle("Num of Strips");

    m_phiVsNstrips_100[l]->GetXaxis()->SetTitle("#phi to Wafer");
    m_phiVsNstrips_100[l]->GetYaxis()->SetTitle("Num of Strips");

    m_phiVsNstrips_111[l]->GetXaxis()->SetTitle("#phi to Wafer");
    m_phiVsNstrips_111[l]->GetYaxis()->SetTitle("Num of Strips");

    for (int side = 0; side < nSides; ++side) {
      m_phiVsNstrips_Side_100[l][side] = pFactory("h_phiVsNstrips_100_" + hNum[l] + "Side" + hNumS[side],
                                                  "100 - Inc. Angle vs nStrips for Layer Side " + hNum[l] + hNumS[side],
                                                  nProfileBins, -90., 90., Lorentz, iflag);
      m_phiVsNstrips_Side_111[l][side] = pFactory("h_phiVsNstrips_111_" + hNum[l] + "Side" + hNumS[side],
                                                  "111 - Inc. Angle vs nStrips for Layer Side " + hNum[l] + hNumS[side],
                                                  nProfileBins, -90., 90., Lorentz, iflag);
      m_phiVsNstrips_Side[l][side] = pFactory("h_phiVsNstrips" + hNum[l] + "Side" + hNumS[side],
                                              "Inc. Angle vs nStrips for Layer Side" + hNum[l] + hNumS[side],
                                              nProfileBins, -90., 90., Lorentz, iflag);

      m_phiVsNstrips_Side[l][side]->GetXaxis()->SetTitle("#phi to Wafer");
      m_phiVsNstrips_Side[l][side]->GetYaxis()->SetTitle("Num of Strips");

      m_phiVsNstrips_Side_100[l][side]->GetXaxis()->SetTitle("#phi to Wafer");
      m_phiVsNstrips_Side_100[l][side]->GetYaxis()->SetTitle("Num of Strips");

      m_phiVsNstrips_Side_111[l][side]->GetXaxis()->SetTitle("#phi to Wafer");
      m_phiVsNstrips_Side_111[l][side]->GetYaxis()->SetTitle("Num of Strips");
    }
    success *= iflag;
  }
  if (success == 0) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

TProfile*
SCTLorentzMonTool::pFactory(const std::string& name, const std::string& title, int nbinsx, float xlow, float xhigh,
                            MonGroup& registry, int& iflag) {
  Prof_t tmp = new TProfile(TString(name), TString(title), nbinsx, xlow, xhigh);
  bool success(registry.regHist(tmp).isSuccess());

  if (not success) {
    ATH_MSG_ERROR("Cannot book SCT histogram: " << name);
    iflag = 0;
  }else {
    iflag = 1;
  }

  return tmp;
}

bool
SCTLorentzMonTool::h1Factory(const std::string& name, const std::string& title, const float extent, MonGroup& registry,
                             VecH1_t& storageVector) {
  const unsigned int nbins(100);
  const float lo(-extent), hi(extent);
  H1_t tmp = new TH1F(TString(name), TString(title), nbins, lo, hi);
  bool success(registry.regHist(tmp).isSuccess());

  if (not success) {
    ATH_MSG_ERROR("Cannot book SCT histogram: " << name);
  }
  storageVector.push_back(tmp);
  return success;
}

int
SCTLorentzMonTool::findAnglesToWaferSurface(const float (&vec)[3], const float& sinAlpha, const Identifier& id,
                                            const InDetDD::SiDetectorElementCollection* elements,
                                            float& theta, float& phi) {
  int iflag(-1);

  phi = 90.;
  theta = 90.;

  const Identifier waferId = m_pSCTHelper->wafer_id(id);
  const IdentifierHash waferHash = m_pSCTHelper->wafer_hash(waferId);
  const InDetDD::SiDetectorElement* element = elements->getDetectorElement(waferHash);
  if (!element) {
    ATH_MSG_ERROR("findAnglesToWaferSurface:  failed to find detector element for id=" <<
                  m_pSCTHelper->show_to_string(id));
    return iflag;
  }

  float cosAlpha = sqrt(1. - sinAlpha * sinAlpha);
  float phix = cosAlpha * element->phiAxis().x() + sinAlpha * element->phiAxis().y();
  float phiy = -sinAlpha *element->phiAxis().x() + cosAlpha * element->phiAxis().y();

  float pNormal = vec[0] * element->normal().x() + vec[1] * element->normal().y() + vec[2] * element->normal().z();
  float pEta = vec[0] * element->etaAxis().x() + vec[1] * element->etaAxis().y() + vec[2] * element->etaAxis().z();
  float pPhi = vec[0] * phix + vec[1] * phiy + vec[2] * element->phiAxis().z();

  if (pPhi < 0.) {
    phi = -90.;
  }
  if (pEta < 0.) {
    theta = -90.;
  }
  if (pNormal != 0.) {
    phi = atan(pPhi / pNormal) / CLHEP::deg;
    theta = atan(pEta / pNormal) / CLHEP::deg;
  }
  iflag = 1;
  return iflag;
}

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**    @file SCTLorentzMonTool.cxx
 *
 *    @author Elias Coniavitis based on code from Luca Fiorini,
 *    Shaun Roe, Manuel Diaz, Rob McPherson & Richard Batley
 *    Modified by Yuta
 */
#include "SCTLorentzMonTool.h"

#include "SCT_NameFormatter.h"

#include "AthContainers/DataVector.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetPrepRawData/SiCluster.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/ReadHandle.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrackSummary/TrackSummary.h"

#include "GaudiKernel/StatusCode.h"

#include "TProfile.h"

#include <cmath>
#include <type_traits>

using namespace SCT_Monitoring;

// ====================================================================================================
/** Constructor, calls base class constructor with parameters
 *
 *  several properties are "declared" here, allowing selection
 *  of the filepath for histograms etc
 */
// ====================================================================================================
SCTLorentzMonTool::SCTLorentzMonTool(const std::string& type, const std::string& name, const IInterface* parent):
  ManagedMonitorToolBase(type, name, parent) {
}

// ====================================================================================================
// ====================================================================================================
StatusCode SCTLorentzMonTool::initialize() {
  ATH_CHECK(detStore()->retrieve(m_pSCTHelper, "SCT_ID"));

  ATH_CHECK(m_tracksName.initialize());
  ATH_CHECK(m_SCTDetEleCollKey.initialize());

  if (m_rejectSharedHits) {
    ATH_CHECK(m_assoTool.retrieve());
  } else {
    m_assoTool.disable();
  }

  return ManagedMonitorToolBase::initialize();
}

// ====================================================================================================
//                       SCTLorentzMonTool :: bookHistograms
// ====================================================================================================
StatusCode
SCTLorentzMonTool::bookHistogramsRecurrent( ) {
  m_path = "";
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
  ATH_MSG_DEBUG("initialize being called");
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
  static const int layer100[] = {
    2, 2, 3, 2, 2, 2, 0, 2, 3, 2, 0, 2, 3, 2, 3, 2, 0, 2, 3, 0, 2, 0, 2, 3, 2, 2, 2, 0, 0, 0, 0, 0, 0, 3, 0, 3, 2, 0, 2,
    2, 0, 3, 3, 3, 0, 2, 2, 2, 2, 2, 2, 2, 3, 2, 2, 3, 3, 2, 2, 2, 2, 2, 3, 3, 2, 3, 2, 2, 2, 3, 3, 3, 2, 2, 2, 2, 3, 3,
    2, 3, 2, 3, 3, 2, 3, 2, 2, 2, 2, 2, 2, 2
  };
  static const int phi100[] = {
    29, 29, 6, 13, 23, 13, 14, 29, 9, 29, 14, 29, 9, 29, 39, 32, 21, 32, 13, 22, 32, 22, 32, 13, 32, 32, 32, 20, 20, 20,
    20, 20, 20, 13, 21, 17, 33, 5, 33, 33, 31, 6, 19, 47, 21, 37, 37, 37, 37, 33, 37, 37, 24, 33, 33, 47, 19, 33, 33,
    37, 37, 37, 55, 9, 38, 24, 37, 38, 8, 9, 9, 26, 38, 38, 38, 38, 39, 39, 38, 11, 45, 54, 54, 24, 31, 14, 47, 45, 47,
    47, 47, 47
  };
  static const int eta100[] = {
    3, -4, -6, 2, 6, 3, -5, -1, 6, -2, -6, -5, 5, -3, 2, 6, -3, 5, 5, 3, 4, 2, 2, 2, -1, -3, -4, 1, -1, -2, -3, -4, 4,
    -1, -5, 6, 2, 4, 3, 1, 6, -2, 6, 3, -6, -1, 2, 1, 3, -5, 4, 5, -3, -4, -3, -5, -2, -1, -2, -3, -2, -4, -3, 2, 3, -6,
    -5, 4, 6, 1, -6, 1, 1, -5, -4, -3, -3, -5, -2, 1, 5, 5, 4, 4, 5, 4, -1, -5, 3, 4, 1, -5
  };
  constexpr unsigned int layer100_n{sizeof(layer100) / sizeof(*layer100)};
  constexpr unsigned int phi100_n{sizeof(phi100) / sizeof(*phi100)};
  constexpr unsigned int eta100_n{sizeof(eta100) / sizeof(*eta100)};
  constexpr bool theseArraysAreEqualInLength{(layer100_n == phi100_n) and (phi100_n == eta100_n)};

  static_assert(theseArraysAreEqualInLength, "Coordinate arrays for <100> wafers are not of equal length");

  ATH_MSG_DEBUG("enters fillHistograms");

  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEle{m_SCTDetEleCollKey};
  const InDetDD::SiDetectorElementCollection* elements{sctDetEle.retrieve()};
  if (elements==nullptr) {
    ATH_MSG_WARNING(m_SCTDetEleCollKey.fullKey() << " could not be retrieved");
    return StatusCode::SUCCESS;
  }

  SG::ReadHandle<TrackCollection> tracks{m_tracksName};
  if (not tracks.isValid()) {
    ATH_MSG_WARNING(" TrackCollection not found: Exit SCTLorentzTool" << m_tracksName.key());
    return StatusCode::SUCCESS;
  }

  // Prepare AssociationTool
  Trk::IPRD_AssociationTool::Maps maps;
  for (const Trk::Track* track : *tracks) {
    ATH_CHECK(m_assoTool->addPRDs(maps, *track));
  }

  for (const Trk::Track* track: *tracks) {
    if (track==nullptr) {
      ATH_MSG_ERROR("no pointer to track!!!");
      continue;
    }

    // Get pointer to track state on surfaces
    const DataVector<const Trk::TrackStateOnSurface>* trackStates{track->trackStateOnSurfaces()};
    if (trackStates==nullptr) {
      ATH_MSG_WARNING("for current track, TrackStateOnSurfaces == Null, no data will be written for this track");
      continue;
    }

    const Trk::TrackSummary* summary{track->trackSummary()};
    if (summary==nullptr) {
      ATH_MSG_WARNING(" null trackSummary");
      continue;
    }

    for (const Trk::TrackStateOnSurface* tsos: *trackStates) {
      if (tsos->type(Trk::TrackStateOnSurface::Measurement)) {
        const InDet::SiClusterOnTrack* clus{dynamic_cast<const InDet::SiClusterOnTrack*>(tsos->measurementOnTrack())};
        if (clus) { // Is it a SiCluster? If yes...
          // Reject shared hits if you want
          if (m_rejectSharedHits and m_assoTool->isShared(maps, *(clus->prepRawData()))) {
            continue;
          }

          const InDet::SiCluster* RawDataClus{dynamic_cast<const InDet::SiCluster*>(clus->prepRawData())};
          if (RawDataClus==nullptr) {
            continue; // Continue if dynamic_cast returns null
          }
          if (RawDataClus->detectorElement()->isSCT()) {
            const Identifier sct_id{clus->identify()};
            const int bec{m_pSCTHelper->barrel_ec(sct_id)};
            const int layer{m_pSCTHelper->layer_disk(sct_id)};
            const int side{m_pSCTHelper->side(sct_id)};
            const int eta{m_pSCTHelper->eta_module(sct_id)};
            const int phi{m_pSCTHelper->phi_module(sct_id)};

            if (bec != 0) {
              continue; // We only care about the barrel
            }
            // Check if the silicon surface is 100.
            SiliconSurface surface{surface111};
            for (unsigned int i{0}; i < layer100_n; i++) {
              if ((layer100[i] == layer) and (eta100[i] == eta) and (phi100[i] == phi)) {
                surface = surface100;
                break;
              }
            }
            // find cluster size
            const std::vector<Identifier>& rdoList{RawDataClus->rdoList()};
            int nStrip{static_cast<int>(rdoList.size())};
            const Trk::TrackParameters* trkp{dynamic_cast<const Trk::TrackParameters*>(tsos->trackParameters())};
            if (trkp==nullptr) {
              ATH_MSG_WARNING(" Null pointer to MeasuredTrackParameters");
              continue;
            }

            const Trk::Perigee* perigee{track->perigeeParameters()};

            if (perigee) {
              // Get angle to wafer surface
              float phiToWafer{90.f};
              float thetaToWafer(90.f);
              float sinAlpha{0.f}; // for barrel, which is the only thing considered here
              float pTrack[3]; // 3 is for x, y, z.
              pTrack[0] = trkp->momentum().x();
              pTrack[1] = trkp->momentum().y();
              pTrack[2] = trkp->momentum().z();
              int iflag{findAnglesToWaferSurface(pTrack, sinAlpha, clus->identify(), elements, thetaToWafer, phiToWafer)};
              if (iflag < 0) {
                ATH_MSG_WARNING("Error in finding track angles to wafer surface");
                continue; // Let's think about this (later)... continue, break or return?
              }

              bool passesCuts{true};
              if ((AthenaMonManager::dataType() == AthenaMonManager::cosmics) and
                  (trkp->momentum().mag() > 500.) and  // Pt > 500MeV
                  (summary->get(Trk::numberOfSCTHits) > 6)// and // #SCTHits >6
                  ) {
                passesCuts = true;
              } else if ((track->perigeeParameters()->parameters()[Trk::qOverP] < 0.) and // use negative track only
                         (std::abs(perigee->parameters()[Trk::d0]) < 1.) and // d0 < 1mm
                         (std::abs(perigee->parameters()[Trk::z0] * sin(perigee->parameters()[Trk::theta])) < 1.) and // d0 < 1mm
                         (trkp->momentum().mag() > 500.) and  // Pt > 500MeV
                         (summary->get(Trk::numberOfSCTHits) > 6)// and // #SCTHits >6
                         ) {
                passesCuts = true;
              } else {
                passesCuts = false;
              }

              if (passesCuts) {
                // Fill profile
                m_phiVsNstrips[layer][     side][    surface]->Fill(phiToWafer, nStrip, 1.);
              }// end if passesCuts
            }// end if mtrkp
          } // end if SCT..
        } // end if (clus)
      } // if (tsos->type(Trk::TrackStateOnSurface::Measurement)) {
    }// end of loop on TrackStatesonSurface (they can be SiClusters, TRTHits,..)
  } // end of loop on tracks
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                             SCTLorentzMonTool :: procHistograms
// ====================================================================================================
StatusCode
SCTLorentzMonTool::procHistograms() {
  if (endOfRunFlag()) {
    ATH_MSG_DEBUG("finalHists()");
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
  const std::string stem{m_path + "/SCT/GENERAL/lorentz/"};
  MonGroup Lorentz{this, m_path + "SCT/GENERAL/lorentz", run, ATTRIB_UNMANAGED};

  static const std::string hNum[N_BARRELS] = {
    "0", "1", "2", "3"
  };
  static const std::string hNumS[N_SIDES] = {
    "0", "1"
  };
  static const int nProfileBins{360};
 
  int success{1};
  static const std::string histNames1[nSurfaces]{"_100",   "_111"}; 
  static const std::string histNames2[nSurfaces]{"_100_",  "_111_"}; 
  static const std::string histTitles[nSurfaces]{"100 - ", "111 - "};
  for (int l{0}; l < N_BARRELS; ++l) {
    // granularity set to one profile/layer for now
    for (int side{0}; side < nSides; ++side) {
      for (unsigned int iSurface{0}; iSurface<nSurfaces; iSurface++) {
        std::string histName;
        histName = "h_phiVsNstrips" + histNames2[iSurface] + hNum[l] + "Side" + hNumS[side];
        std::string histTitle;
        histTitle = histTitles[iSurface] + "Inc. Angle vs nStrips for Layer Side" + hNum[l] + hNumS[side];
        int iflag{0};
        m_phiVsNstrips[l][side][iSurface] = pFactory(histName, histTitle, nProfileBins, -90., 90., Lorentz, iflag);
        m_phiVsNstrips[l][side][iSurface]->GetXaxis()->SetTitle("#phi to Wafer");
        m_phiVsNstrips[l][side][iSurface]->GetYaxis()->SetTitle("Num of Strips");
        success *= iflag;
      }
    }
  }
  if (success == 0) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

TProfile*
SCTLorentzMonTool::pFactory(const std::string& name, const std::string& title, int nbinsx, float xlow, float xhigh,
                            MonGroup& registry, int& iflag) const {
  TProfile* tmp{new TProfile{name.c_str(), title.c_str(), nbinsx, xlow, xhigh}};
  bool success{registry.regHist(tmp).isSuccess()};

  if (not success) {
    ATH_MSG_ERROR("Cannot book SCT histogram: " << name);
    iflag = 0;
  } else {
    iflag = 1;
  }

  return tmp;
}

int
SCTLorentzMonTool::findAnglesToWaferSurface(const float (&vec)[3], // 3 is for x, y, z.
                                            const float& sinAlpha, const Identifier& id,
                                            const InDetDD::SiDetectorElementCollection* elements,
                                            float& theta, float& phi) const {
  int iflag{-1};

  phi = 90.;
  theta = 90.;

  const Identifier waferId{m_pSCTHelper->wafer_id(id)};
  const IdentifierHash waferHash{m_pSCTHelper->wafer_hash(waferId)};
  const InDetDD::SiDetectorElement* element{elements->getDetectorElement(waferHash)};
  if (element==nullptr) {
    ATH_MSG_ERROR("findAnglesToWaferSurface:  failed to find detector element for id=" <<
                  m_pSCTHelper->show_to_string(id));
    return iflag;
  }

  float cosAlpha{std::sqrt(1.0f - sinAlpha * sinAlpha)};
  double phix{ cosAlpha * element->phiAxis().x() + sinAlpha * element->phiAxis().y()};
  double phiy{-sinAlpha * element->phiAxis().x() + cosAlpha * element->phiAxis().y()};

  double pNormal{vec[0] * element->normal().x()  + vec[1] * element->normal().y()  + vec[2] * element->normal().z()};
  double pEta{   vec[0] * element->etaAxis().x() + vec[1] * element->etaAxis().y() + vec[2] * element->etaAxis().z()};
  double pPhi{   vec[0] * phix                   + vec[1] * phiy                   + vec[2] * element->phiAxis().z()};

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

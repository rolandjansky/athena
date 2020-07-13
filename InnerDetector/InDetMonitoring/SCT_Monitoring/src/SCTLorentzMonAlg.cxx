/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTLorentzMonAlg.h"

#include "AthenaMonitoringKernel/Monitored.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetPrepRawData/SiCluster.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "TrkTrackSummary/TrackSummary.h"

#include <cmath>
#include <memory>
#include <vector>

SCTLorentzMonAlg::SCTLorentzMonAlg(const std::string& name, ISvcLocator* pSvcLocator)
  :AthMonitorAlgorithm(name,pSvcLocator) {
}

StatusCode SCTLorentzMonAlg::initialize() {
  ATH_CHECK(detStore()->retrieve(m_pSCTHelper, "SCT_ID"));
  ATH_CHECK(m_trackSummaryTool.retrieve());
  ATH_CHECK(m_tracksName.initialize());
  ATH_CHECK(m_SCTDetEleCollKey.initialize());

  if (m_rejectSharedHits) {
    ATH_CHECK(m_assoTool.retrieve());
  } else {
    m_assoTool.disable();
  }

  return AthMonitorAlgorithm::initialize();
}

StatusCode SCTLorentzMonAlg::fillHistograms(const EventContext& ctx) const {
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

  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEle{m_SCTDetEleCollKey, ctx};
  const InDetDD::SiDetectorElementCollection* elements{sctDetEle.retrieve()};
  if (elements==nullptr) {
    ATH_MSG_WARNING(m_SCTDetEleCollKey.fullKey() << " could not be retrieved");
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<TrackCollection> tracks{m_tracksName, ctx};
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
    std::unique_ptr<Trk::TrackSummary> mySummary;
    if (summary==nullptr) {
      mySummary = m_trackSummaryTool->summary(*track);
      summary = mySummary.get();
      if (summary==nullptr) {
        ATH_MSG_WARNING("Trk::TrackSummary is null and cannot be created by " << m_trackSummaryTool.name());
        continue;
      }
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
              if ((dataType() == AthMonitorAlgorithm::DataType_t::cosmics) and
                  (trkp->momentum().mag() > 500.) and  // Pt > 500MeV
                  (summary->get(Trk::numberOfSCTHits) > 6) // #SCTHits >6
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
                std::string xVar{"phiToWafer_"+std::to_string(layer)};
                std::string yVar{"nStrip_"+std::to_string(layer)};
                if(surface == surface100){
                    xVar += "_100";
                    yVar += "_100";
                }
                if(surface == surface111){
                    xVar += "_111";
                    yVar += "_111";
                }
                if(side == side0){
                    xVar += "_0";
                    yVar += "_0";
                }
                if(side == side1){
                    xVar += "_1";
                    yVar += "_1";
                }
                auto phiToWaferAcc{Monitored::Scalar<float>(xVar, phiToWafer)};
                auto nStripAcc{Monitored::Scalar<int>(yVar, nStrip)};
                fill("SCTLorentzMonitor", phiToWaferAcc, nStripAcc);
              }// end if passesCuts
            }// end if mtrkp
          } // end if SCT..
        } // end if (clus)
      } // if (tsos->type(Trk::TrackStateOnSurface::Measurement)) {
    }// end of loop on TrackStatesonSurface (they can be SiClusters, TRTHits,..)
  } // end of loop on tracks

    
  return StatusCode::SUCCESS;
}

int
SCTLorentzMonAlg::findAnglesToWaferSurface(const float (&vec)[3], // 3 is for x, y, z.
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

  float cosAlpha{sqrt(1.0f - sinAlpha * sinAlpha)};
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

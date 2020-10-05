/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTTracksMonAlg.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "SCT_NameFormatter.h"

#include "AthContainers/DataVector.h"
#include "AthenaKernel/errorcheck.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetPrepRawData/SiCluster.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "StoreGate/ReadHandle.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrackSummary/TrackSummary.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ThreadLocalContext.h"

#include <cmath>

using SCT_Monitoring::N_REGIONS;
using SCT_Monitoring::bec2Index;

namespace {
  // some possible parameter key values
  enum ParameterKey {
    ONE_D_LOCATION=1, TWO_D_LOCATION=3, TRACK_SEGMENT=11, OLD_PARAMETERS_VECTOR=31
  };

  // segregate the eta regions
  float
  etaRegion(const float eta) {
    // eta cut when segregating the tracks per region histogram
    static const float etaBoundary(1.0);

    if (eta < -etaBoundary) {
      return 0.0; // EC C
    }
    if (eta > etaBoundary) {
      return 2.0; // EC A
    }
    return 1.0; // Barrel
  }
}

SCTTracksMonAlg::SCTTracksMonAlg(const std::string& name, ISvcLocator* pSvcLocator) :AthMonitorAlgorithm(name, pSvcLocator) {
 
}

StatusCode SCTTracksMonAlg::initialize(){
  ATH_CHECK( m_tracksName.initialize() );
  ATH_CHECK(m_trackSummaryTool.retrieve());
  ATH_CHECK(m_residualPullCalculator.retrieve());
  if (m_doUnbiasedCalc) {
    ATH_CHECK(m_updator.retrieve());
  } else {
    m_updator.disable();
  }
  ATH_CHECK(detStore()->retrieve(m_pSCTHelper, "SCT_ID"));


  return AthMonitorAlgorithm::initialize();
}

StatusCode SCTTracksMonAlg::fillHistograms(const EventContext& ctx) const{
	
ATH_MSG_DEBUG("SCTTracksMonAlg::fillHistograms()"); 

  const bool doThisSubsystem[N_REGIONS] = {
    m_doNegativeEndcap, true, m_doPositiveEndcap
  };

  std::bitset<N_TRIGGER_TYPES> firedTriggers{0};
  if (m_doTrigger and (not checkTriggers(firedTriggers).isSuccess())) {
    ATH_MSG_WARNING("Triggers not found!");
  }
  SG::ReadHandle<TrackCollection> tracks{m_tracksName,ctx}; 
  if (not tracks.isValid()) {
    ATH_MSG_WARNING("No collection named " << m_tracksName.key() << " in StoreGate");
    return StatusCode::SUCCESS;
  }


  ATH_MSG_DEBUG("Begin loop over " << tracks->size() << " tracks"); 
  int goodTrks_N{0};
  int local_tot_trkhits{0};
  for (const Trk::Track* track: *tracks) {
    if (track==nullptr) {
      ATH_MSG_ERROR("No pointer to track");
      break;
    }

    int local_scthits{0};
    int scthits_on_trk{0}; // Breaks out of loop if track has less than 3 sct hits
    std::unique_ptr<const Trk::TrackSummary> trkSum = m_trackSummaryTool->summary (*track);
    if (trkSum==nullptr) {
      ATH_MSG_WARNING("Trk::TrackSummary is null and cannot be created by " << m_trackSummaryTool.name());
    }

    if (trkSum) {
      scthits_on_trk = trkSum->get(Trk::numberOfSCTHits);
    } else {
      ATH_MSG_WARNING("TrackSummary not found not using track!"); 
    }
    if (scthits_on_trk < m_trackHitCut) {
      ATH_MSG_DEBUG("track fails minimum SCT hit requirement"); 
      break;
    }
    goodTrks_N++;
    if (track->fitQuality()->numberDoF() > 0.) { // Fill Track Chi2/ndf histogram
        auto trk_chi2Acc{Monitored::Scalar<float>("trk_chi2", track->fitQuality()->chiSquared() / track->fitQuality()->numberDoF())};
        fill("SCTTracksMonitor", trk_chi2Acc);
    }
    if (track->perigeeParameters() == 0) {
      continue;
    }
    double trackPerigeeTheta{track->perigeeParameters()->parameters()[Trk::theta]};
    double trackPerigeeEta{-log(tan(0.5 * trackPerigeeTheta))};
    auto tracksPerRegionAcc{Monitored::Scalar<float>("tracksPerRegion", etaRegion(trackPerigeeEta))}; 

    fill("SCTTracksMonitor", tracksPerRegionAcc);

    auto trk_etaAcc{Monitored::Scalar<float>("trk_eta", trackPerigeeEta)};
    fill("SCTTracksMonitor", trk_etaAcc); 

    if (track->perigeeParameters()->parameters()[Trk::qOverP] != 0.) {
        auto trk_ptAcc{Monitored::Scalar<float>("trk_pt", std::abs(1. / (track->perigeeParameters()->parameters()[Trk::qOverP] * 1000.)))};
        fill("SCTTracksMonitor", trk_ptAcc);
    }
    auto trk_d0Acc{Monitored::Scalar<float>("trk_d0", track->perigeeParameters()->parameters()[Trk::d0])};
    fill("SCTTracksMonitor", trk_d0Acc);
    auto trk_z0Acc{Monitored::Scalar<float>("trk_z0", track->perigeeParameters()->parameters()[Trk::z0])};
    fill("SCTTracksMonitor", trk_z0Acc);
    auto trk_phiAcc{Monitored::Scalar<float>("trk_phi", track->perigeeParameters()->parameters()[Trk::phi])};
    fill("SCTTracksMonitor", trk_phiAcc);

    if (m_doTrigger) { 
      for (int trig{0}; trig < N_TRIGGER_TYPES; ++trig) {
        if (hasTriggerFired(trig, firedTriggers)) {
            auto trackTriggerAcc{Monitored::Scalar<int>("trackTriggers", trig)};
            fill("SCTTracksMonitor", trackTriggerAcc);
        }
      }
    }
    bool hasHits[N_REGIONS] = {
      false, false, false
    }; // Define bools to check whether the track has barrel, EA/C hits
    // Get pointer to track state on surfaces
    const DataVector<const Trk::TrackStateOnSurface>* trackStates{track->trackStateOnSurfaces()};
    if (not trackStates) {
      ATH_MSG_ERROR("for current track, TrackStateOnSurfaces == Null, no data will be written for this track");
      break;
    }
    for (const Trk::TrackStateOnSurface* tsos: *trackStates) {
      if (tsos->type(Trk::TrackStateOnSurface::Measurement)) {
        const InDet::SiClusterOnTrack* clus{dynamic_cast<const InDet::SiClusterOnTrack*>(tsos->measurementOnTrack())};
        if (clus) { // Is it a SiCluster? If yes...
          const InDet::SiCluster* RawDataClus{dynamic_cast<const InDet::SiCluster*>(clus->prepRawData())};
          if (RawDataClus==nullptr) {
            continue; // Continue if dynamic_cast returns null
          }
          if (RawDataClus->detectorElement()->isSCT()) {
            const Identifier sct_id{clus->identify()};
            const int bec{m_pSCTHelper->barrel_ec(sct_id)};
            const unsigned int subsystemIndex{bec2Index(bec)};
            const bool doThisDetector{doThisSubsystem[subsystemIndex]};
            hasHits[subsystemIndex] = true;
            std::unique_ptr<const Trk::TrackParameters> trkParameters(nullptr);
            const Trk::TrackParameters* trkParam{tsos->trackParameters()};
            const Trk::RIO_OnTrack* rio{dynamic_cast<const Trk::RIO_OnTrack*>(tsos->measurementOnTrack())};
            if (rio) {
#ifndef NDEBUG
              ATH_MSG_DEBUG("if rio");
#endif
              if (m_doUnbiasedCalc) {
                if (trkParam) {
                  trkParameters.reset(m_updator->removeFromState(*trkParam, rio->localParameters(), rio->localCovariance())); //need to take ownership of the returned pointer
                  if (trkParameters) {
                    trkParam = trkParameters.get();
                  }
                }
              }
            } else {
              ATH_MSG_DEBUG("not rio");
            }
            if (trkParam) {
              const AmgVector(5) LocalTrackParameters{trkParam->parameters()};
#ifndef NDEBUG
              ATH_MSG_DEBUG("Track Position Phi= " << LocalTrackParameters[Trk::locX]);
              ATH_MSG_DEBUG("Cluster Position Phi= " << clus->localParameters()[Trk::locX]);
#endif
              if (not m_residualPullCalculator.empty()) {
                std::unique_ptr<const Trk::ResidualPull> residualPull{m_residualPullCalculator->residualPull(rio, trkParam,
                                                                      m_doUnbiasedCalc ? Trk::ResidualPull::Unbiased : Trk::ResidualPull::Biased)};
                if (not residualPull) {
                  ATH_MSG_WARNING("Residual Pull Calculator did not succeed!");
                  return StatusCode::SUCCESS;
                } else {
                  double local_residual{residualPull->residual()[Trk::locX]};
                  double local_pull{residualPull->pull()[Trk::locX]};
                  if (doThisDetector) {
                    auto residualAcc{Monitored::Scalar<float>("total"+m_regionNames[bec2Index(bec)]+"Residual", local_residual)};
                    fill("SCTTracksMonitor", residualAcc);

                    auto pullAcc{Monitored::Scalar<float>("total"+m_regionNames[bec2Index(bec)]+"Pull", local_pull)};
                    fill("SCTTracksMonitor", pullAcc);

                  }
                }
              }
            } else { // no measured local parameters, pull won't be calculated
              ATH_MSG_WARNING("No measured local parameters, pull won't be calculated");
            }
            ++local_scthits; // TODO This is not correct, change it
            ++local_tot_trkhits;
          } // end if SCT..
        } // end if (clus)
      } // if (tsos->type(Trk::TrackStateOnSurface::Measurement))
    }// end of loop on TrackStatesonSurface (they can be SiClusters, TRTHits,..)
    auto local_hitsAcc{Monitored::Scalar<float>("trk_sct_hits", static_cast<float>(local_scthits))};
    fill("SCTTracksMonitor", local_hitsAcc);

    // We now know whether this particular track had hits in the barrel or endcaps- update the profile histogram
    for (unsigned int region{0}; region < N_REGIONS; ++region) {
        auto regionAcc{Monitored::Scalar<int>("region", static_cast<int>(region))};
        auto hitsAcc{Monitored::Scalar<float>("hitsRegion", static_cast<float>(hasHits[region]))};
        fill("SCTTracksMonitor", regionAcc,hitsAcc);
      // barrel, Eca, Ecb)
    }
  } // end of loop on tracks
  auto goodTrks_NAcc{Monitored::Scalar<int>("trk_N", goodTrks_N)};
  fill("SCTTracksMonitor", goodTrks_NAcc);
  return StatusCode::SUCCESS;
}


// ====================================================================================================
//                           SCTTracksMonTool :: calculatePull
// ====================================================================================================
float
SCTTracksMonAlg::calculatePull(const float residual, const float trkErr, const float hitErr) const {
  float ErrorSum{sqrt(trkErr * trkErr + hitErr * hitErr)};

  if (ErrorSum > 1.0e-20) { // as floats are rarely exactly zero
    return residual / ErrorSum;
  } else {
    ATH_MSG_DEBUG("Error on Track and Cluster are 0. Returning Pull value 0.");
    return 0.;
  }
}

StatusCode
SCTTracksMonAlg::checkTriggers(std::bitset<N_TRIGGER_TYPES>& firedTriggers) const {

  const EventContext& ctx = Gaudi::Hive::currentContext();
  SG::ReadHandle<xAOD::EventInfo> evtInfo = GetEventInfo (ctx);
  if (evtInfo.isValid()) {
    firedTriggers = evtInfo->level1TriggerType();

    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

bool
SCTTracksMonAlg::hasTriggerFired(const unsigned int trigger, const std::bitset<N_TRIGGER_TYPES>& firedTriggers) const {
  return ((trigger < N_TRIGGER_TYPES) ? firedTriggers.test(trigger) : false);
}

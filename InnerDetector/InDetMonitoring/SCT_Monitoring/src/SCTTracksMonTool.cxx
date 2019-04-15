// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**    @file SCTTracksMonTool.cxx
 *
 *    @author Luca Fiorini, based on code from Shaun Roe, Manuel Diaz & Rob McPherson
 *    Also uses code from InDet::SCT_ResidualPullCalculator
 */
#include "SCTTracksMonTool.h"

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

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/StatusCode.h"

#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile2D.h"

#include <cmath>

using namespace std;
using namespace SCT_Monitoring;

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

const string SCTTracksMonTool::s_triggerNames[] = {
  "RNDM", "BPTX", "L1CAL", "TGC", "RPC", "MBTS", "COSM", "Calib"
};

// ====================================================================================================
/** Constructor, calls base class constructor with parameters
 *
 *  several properties are "declared" here, allowing selection
 *  of the filepath for histograms, the first and second plane
 *  numbers to be used, and the timebin.
 */
// ====================================================================================================
SCTTracksMonTool::SCTTracksMonTool(const string& type,
                                   const string& name,
                                   const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent) {
}

// ====================================================================================================
// ====================================================================================================
StatusCode SCTTracksMonTool::initialize() {
  ATH_CHECK( m_tracksName.initialize() );
  ATH_CHECK( m_eventInfoKey.initialize() );
  ATH_CHECK(m_residualPullCalculator.retrieve());
  if (m_doUnbiasedCalc) {
    ATH_CHECK(m_updator.retrieve());
  } else {
    m_updator.disable();
  }
  ATH_CHECK(detStore()->retrieve(m_pSCTHelper, "SCT_ID"));
  return ManagedMonitorToolBase::initialize();
}

// ====================================================================================================
//                       SCTTracksMonTool :: bookHistograms
// ====================================================================================================
StatusCode
SCTTracksMonTool::bookHistogramsRecurrent() {
  ATH_MSG_DEBUG("SCTTracksMonTool::bookHistograms");
  m_path = (m_useIDGlobal) ? ("/InDetGlobal/") : ("");
  if (newRunFlag()) {
    m_numberOfEvents = 0;
  }
  // Booking  Track related Histograms
  ATH_CHECK(bookGeneralHistos());
  const bool doThisSubsystem[N_REGIONS] = {
    m_doNegativeEndcap, true, m_doPositiveEndcap
  };
  static const string names[N_REGIONS] = {
    "endcap C", "barrel", "endcap A"
  };
  for (unsigned int sys{0}; sys < N_REGIONS; ++sys) {
    if (doThisSubsystem[sys] and bookTrackHistos(index2Bec(sys)).isFailure()) {
      ATH_MSG_WARNING("Error in booking track histograms for " << names[sys]);
    }
  }

  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                       SCTTracksMonTool :: bookHistograms
// ====================================================================================================
StatusCode
SCTTracksMonTool::bookHistograms() {
  ATH_MSG_DEBUG("SCTTracksMonTool::bookHistograms");
  m_path = (m_useIDGlobal) ? ("/InDetGlobal/") : ("");
  if (newRunFlag()) {
    m_numberOfEvents = 0;
  }
  // Booking  Track related Histograms
  ATH_CHECK(bookGeneralHistos());
  const bool doThisSubsystem[N_REGIONS] = {
    m_doNegativeEndcap, true, m_doPositiveEndcap
  };
  static const string names[N_REGIONS] = {
    "endcap C", "barrel", "endcap A"
  };
  for (unsigned int sys{0}; sys < N_REGIONS; ++sys) {
    if (doThisSubsystem[sys] and bookTrackHistos(index2Bec(sys)).isFailure()) {
      ATH_MSG_WARNING("Error in booking track histograms for " << names[sys]);
    }
  }

  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                        SCTTracksMonTool :: fillHistograms
/// This is the real workhorse, called for each event. It retrieves the data each time
// ====================================================================================================
StatusCode
SCTTracksMonTool::fillHistograms() {
  if (newRunFlag()) {
    for (unsigned int iReg{0}; iReg<N_REGIONS; iReg++) {
      for (int ld{0}; ld < ((iReg==BARREL_INDEX) ? N_BARRELSx2 : N_DISKSx2); ld++) {
        m_psctresiduals_summaryHistoVector[iReg][ld]->GetXaxis()->SetTitle("Residuals [mm]");
        m_psctpulls_summaryHistoVector[iReg][ld]->GetXaxis()->SetTitle("Pull");
        if (m_environment != AthenaMonManager::online) {
          m_psctresidualsHistoVector[iReg][ld]->GetXaxis()->SetTitle("Index in the direction of #eta");
          m_psctresidualsHistoVector[iReg][ld]->GetYaxis()->SetTitle("Index in the direction of #phi");
          m_psctresidualsRMSHistoVector[iReg][ld]->GetXaxis()->SetTitle("Index in the direction of #eta");
          m_psctresidualsRMSHistoVector[iReg][ld]->GetYaxis()->SetTitle("Index in the direction of #phi");
          m_psctpullsHistoVector[iReg][ld]->GetXaxis()->SetTitle("Index in the direction of #eta");
          m_psctpullsHistoVector[iReg][ld]->GetYaxis()->SetTitle("Index in the direction of #phi");
          m_psctpullsRMSHistoVector[iReg][ld]->GetXaxis()->SetTitle("Index in the direction of #eta");
          m_psctpullsRMSHistoVector[iReg][ld]->GetYaxis()->SetTitle("Index in the direction of #phi");
        }
      }
    }
  }

  ATH_MSG_DEBUG("SCTTracksMonTool::fillHistograms()");

  const bool doThisSubsystem[N_REGIONS] = {
    m_doNegativeEndcap, true, m_doPositiveEndcap
  };

  bitset<N_TRIGGER_TYPES> firedTriggers{0};
  if (m_doTrigger and (not checkTriggers(firedTriggers).isSuccess())) {
    ATH_MSG_WARNING("Triggers not found!");
  }

  SG::ReadHandle<TrackCollection> tracks{m_tracksName};
  if (not tracks.isValid()) {
    ATH_MSG_WARNING("No collection named " << m_tracksName.key() << " in StoreGate");
    return StatusCode::SUCCESS;
  }

  // Fill trigger info once if no tracks are reconstructed.
  if (tracks->size()==0) {
    if (m_doTrigger) {
      for (int trig{0}; trig < N_TRIGGER_TYPES; ++trig) {
        m_trackTriggerRate->Fill(trig, 0);
      }
    }
    for (int i{0}; i < N_REGIONS; ++i) {
      m_trackRate->Fill(i, 0);
    }
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
    const Trk::TrackSummary* trkSum{track->trackSummary()};
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
      m_trk_chi2->Fill(track->fitQuality()->chiSquared() / track->fitQuality()->numberDoF());
    }
    if (track->perigeeParameters() == 0) {
      continue;
    }
    double trackPerigeeTheta{track->perigeeParameters()->parameters()[Trk::theta]};
    double trackPerigeeEta{-log(tan(0.5 * trackPerigeeTheta))};
    m_tracksPerRegion->Fill(etaRegion(trackPerigeeEta));
    m_trk_eta->Fill(trackPerigeeEta);
    if (track->perigeeParameters()->parameters()[Trk::qOverP] != 0.) {
      m_trk_pt->Fill(fabs(1. / (track->perigeeParameters()->parameters()[Trk::qOverP] * 1000.)));
    }
    m_trk_d0->Fill(track->perigeeParameters()->parameters()[Trk::d0]);
    m_trk_z0->Fill(track->perigeeParameters()->parameters()[Trk::z0]);
    m_trk_phi->Fill(track->perigeeParameters()->parameters()[Trk::phi]);
    //
    if (m_doTrigger) {
      for (int trig{0}; trig < N_TRIGGER_TYPES; ++trig) {
        if (hasTriggerFired(trig, firedTriggers)) {
          m_trackTrigger->Fill(trig);
          m_trackTriggerRate->Fill(trig, 1);
        } else {
          m_trackTriggerRate->Fill(trig, 0);
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
    TProfile2D* residualsHistogram{nullptr};
    TProfile2D* pullsHistogram{nullptr};
    TH1F* residualsSummaryHistogram{nullptr};
    TH1F* pullsSummaryHistogram{nullptr};
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
            const int eta{m_pSCTHelper->eta_module(sct_id)};
            const int phi{m_pSCTHelper->phi_module(sct_id)};
            const int bec{m_pSCTHelper->barrel_ec(sct_id)};
            const int layer{m_pSCTHelper->layer_disk(sct_id)};
            const int element{N_SIDES * layer + m_pSCTHelper->side(sct_id)};
            const unsigned int subsystemIndex{bec2Index(bec)};
            const bool doThisDetector{doThisSubsystem[subsystemIndex]};
            hasHits[subsystemIndex] = true;
            const Trk::TrackParameters* trkParameters{nullptr};
            const Trk::RIO_OnTrack* rio{dynamic_cast<const Trk::RIO_OnTrack*>(tsos->measurementOnTrack())};
            bool updateSucceeds{true};
            if (rio) {
#ifndef NDEBUG
              ATH_MSG_DEBUG("if rio");
#endif
              if (m_doUnbiasedCalc) {
                const Trk::TrackParameters* trkParam{tsos->trackParameters()};
                if (trkParam) {
                  trkParameters = m_updator->removeFromState(*trkParam, rio->localParameters(), rio->localCovariance());
                  updateSucceeds = (trkParameters);
                }
              }
            } else {
              ATH_MSG_DEBUG("not rio");
            }
            if (trkParameters==nullptr) {
              trkParameters = tsos->trackParameters();
            }
            if (trkParameters) {
              const AmgVector(5) LocalTrackParameters{trkParameters->parameters()};
#ifndef NDEBUG
              ATH_MSG_DEBUG("Track Position Phi= " << LocalTrackParameters[Trk::locX]);
              ATH_MSG_DEBUG("Cluster Position Phi= " << clus->localParameters()[Trk::locX]);
#endif
              if (not m_residualPullCalculator.empty()) {
                unique_ptr<const Trk::ResidualPull> residualPull{m_residualPullCalculator->residualPull(rio, trkParameters,
                                                                      m_doUnbiasedCalc ? Trk::ResidualPull::Unbiased : Trk::ResidualPull::Biased)};
                if (not residualPull) {
                  ATH_MSG_WARNING("Residual Pull Calculator did not succeed!");
                  return StatusCode::SUCCESS;
                } else {
                  double local_residual{residualPull->residual()[Trk::locX]};
                  double local_pull{residualPull->pull()[Trk::locX]};
                  bool bigPull{(local_pull < 10.) and (local_pull > -10.)};
                  if (doThisDetector) {
                    residualsSummaryHistogram = m_psctresiduals_summaryHistoVector[subsystemIndex][element];
                    pullsSummaryHistogram = m_psctpulls_summaryHistoVector[subsystemIndex][element];
                    if (m_environment != AthenaMonManager::online) {
                      residualsHistogram = m_psctresidualsHistoVector[subsystemIndex][element];
                      pullsHistogram = m_psctpullsHistoVector[subsystemIndex][element];
                      if (residualsHistogram) {
                        residualsHistogram->Fill(eta, phi, local_residual);
                      }
                      if (bigPull and pullsHistogram) {
                        pullsHistogram->Fill(eta, phi, local_pull);
                      }
                    }
                    if (residualsSummaryHistogram) {
                      residualsSummaryHistogram->Fill(local_residual, 1.);
                    }
                    m_totalResidual[bec2Index(bec)]->Fill(local_residual, 1.);
                    if (bigPull and pullsSummaryHistogram) {
                      pullsSummaryHistogram->Fill(local_pull, 1.);
                    }
                    m_totalPull[bec2Index(bec)]->Fill(local_pull, 1.);
                  }
                }
              }
            } else { // no measured local parameters, pull won't be calculated
              ATH_MSG_WARNING("No measured local parameters, pull won't be calculated");
            }
            ++local_scthits; // TODO This is not correct, change it
            ++local_tot_trkhits;
            if (m_doUnbiasedCalc and rio and updateSucceeds) {
              if (trkParameters) {
                delete trkParameters;
                trkParameters = nullptr;
              }
            }
          } // end if SCT..
        } // end if (clus)
      } // if (tsos->type(Trk::TrackStateOnSurface::Measurement))
    }// end of loop on TrackStatesonSurface (they can be SiClusters, TRTHits,..)
    m_trk_ncluHisto->Fill(local_scthits, 1.);
    // We now know whether this particular track had hits in the barrel or endcaps- update the profile histogram
    for (unsigned int region{0}; region < N_REGIONS; ++region) {
      m_trackRate->Fill(static_cast<float>(region), static_cast<float>(hasHits[region])); // note: ordering was different in original code (was
      // barrel, Eca, Ecb)
    }
  } // end of loop on tracks
  m_trk_N->Fill(goodTrks_N);
  m_trk_nclu_totHisto->Fill(local_tot_trkhits, 1.);

  if (m_environment == AthenaMonManager::online) {
    if ((m_numberOfEvents == 1) or ((m_numberOfEvents > 1) and (m_numberOfEvents % m_checkrate == 0))) {
      ATH_MSG_DEBUG("Calling checkHists(false); false := during run");
      if (checkHists(false).isFailure()) {
        ATH_MSG_WARNING("Error in checkHists(false)");
      }
    }
    // Time Dependent SP plots only online
    m_nTracks_buf[m_nTracks_pos] = static_cast<int>(m_trk_chi2->GetEntries());
    m_nTracks_pos++;
    if (m_nTracks_pos == m_evtsbins) {
      m_nTracks_pos = 0;
    }
    if (m_numberOfEvents % m_checkrate == 0) {
      m_nTracks->Reset();
      int latest_nTracks_pos{m_nTracks_pos};
      for (int i{1}; i < m_evtsbins; ++i) {
        if (latest_nTracks_pos == m_evtsbins) {
          latest_nTracks_pos = 0;
        }
        if (m_numberOfEvents < m_evtsbins) {
          if (i < m_nTracks_pos) {
            m_nTracks->SetBinContent(i, m_nTracks_buf[i]);
          } else {
            m_nTracks->SetBinContent(i, 0);
          }
        } else {
          m_nTracks->SetBinContent(i, m_nTracks_buf[latest_nTracks_pos]);
          m_nTracks->GetXaxis()->Set(m_evtsbins, m_numberOfEvents - m_evtsbins, m_numberOfEvents);
        }
        latest_nTracks_pos++;
        if (latest_nTracks_pos == m_evtsbins) {
          latest_nTracks_pos = 0;
        }
      }
    }
  }

  m_numberOfEvents++;

  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                             SCTTracksMonTool :: procHistograms
// ====================================================================================================
StatusCode
SCTTracksMonTool::procHistograms() {
  if (endOfRunFlag()) {
    ATH_MSG_DEBUG("SCTTracksMonTool::procHistograms");
    ATH_MSG_DEBUG("Total Rec Event Number: " << m_numberOfEvents);
    ATH_MSG_DEBUG("Calling checkHists(true); true := end of run");
    if (checkHists(true).isFailure()) {
      ATH_MSG_WARNING("Error in checkHists(true)");
    }
  }
  ATH_MSG_DEBUG("Exiting finalHists");
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                           SCTTracksMonTool :: checkHists
// LF 13/01/2006
// Here we have the check of the histograms and we can also build
// efficiency, occupancy, rms histograms, etc from the basic ones

// LF: A little explanation is needed to understand the reason the following odd C++ coding:
// We registered the RMS histograms in bookHists and we really nead to register them before
// finalize to have  them available online on OH during the run.
// Unfortunaltely THistSvc doesn't (still) allow to unregister or update an histogram and
// ProjectionXY creates a new histogram by a 'new' call. Hence, we cannot change the pointer
//  in THistSvc memory and we cannot pass that pointer in input to ProjectionXY.
// Therefore the following lines pass the information of a tmp hist created by ProjectionXY
//  to the hist which pointer is in THistSvc memory
// ===================================================================================================
StatusCode
SCTTracksMonTool::checkHists(bool /*fromFinalize*/) {
  const bool doDetector[N_REGIONS] = {
    m_doNegativeEndcap, true, m_doPositiveEndcap
  };
  const int numberOfHistograms[N_REGIONS] = {
    N_DISKSx2, N_BARRELSx2, N_DISKSx2
  };

  const int negativeEndCap{0};

  if (m_environment != AthenaMonManager::online) {
    for (int thisDetector{negativeEndCap}; thisDetector < N_REGIONS; ++thisDetector) {
      if (doDetector[thisDetector]) {
        const int lastElement{numberOfHistograms[thisDetector]};
        if (not m_psctresidualsHistoVector[thisDetector].empty()) { /// added protection if vectors are not filled
          for (int element{0}; element < lastElement; ++element) {
            const int nXbins{m_psctresidualsHistoVector[thisDetector][element]->GetNbinsX()};
            const int nYbins{m_psctresidualsHistoVector[thisDetector][element]->GetNbinsY()};
            for (int xbin{1}; xbin <= nXbins; ++xbin) {
              for (int ybin{1}; ybin <= nYbins; ++ybin) {
                m_psctresidualsRMSHistoVector[thisDetector][element]->SetBinContent(xbin, ybin,
                                                                                    m_psctresidualsHistoVector[thisDetector][element]->GetBinError(xbin, ybin));
                m_psctpullsRMSHistoVector[thisDetector][element]->SetBinContent(xbin, ybin,
                                                                                m_psctpullsHistoVector[thisDetector][element]->GetBinError(xbin, ybin));
              }
            }
          }
        }
      }
    }
  }
  // Now checking RMS and Means of Pulls 1D Histos
  TF1 pullgaus{"pullgaus", "gaus"};
  pullgaus.SetParameter(1, 0.);
  pullgaus.SetParameter(2, 1.);
  if (not m_psctpulls_summaryHistoVector[BARREL_INDEX].empty()) {
    for (TH1F* h1: m_psctpulls_summaryHistoVector[BARREL_INDEX]) {
      if (h1->GetEntries() > 2) {// only fit if #entries > 1, otherwise root crashes
        ATH_MSG_DEBUG("GetEntries = " << h1->GetEntries());
        h1->Fit("pullgaus", "Q", "", -2., 2.); // Fit Pulls with a gaussian, Quiet mode ; adding 'N' would aslo do no
        // drawing of the fitted function: the increase in speed is small,
        // though.
        double par[3], epar[3]; // 3 is the number of parameters of Gaussian.
        pullgaus.GetParameters(par);          // gaus params : norm, mean, rms
        for (int i{0}; i < 3; ++i) {
          epar[i] = pullgaus.GetParError(i);    // error on the params
        }
        if (abs(par[1]) > 5. * epar[1]) {
          h1->SetLineColor(2);
        }
        if ((par[2] - 5. * epar[2]) > 2.) {
          h1->SetLineColor(2);
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                           SCTTracksMonTool :: calculatePull
// ====================================================================================================
float
SCTTracksMonTool::calculatePull(const float residual, const float trkErr, const float hitErr) const {
  float ErrorSum{sqrt(trkErr * trkErr + hitErr * hitErr)};

  if (ErrorSum > 1.0e-20) { // as floats are rarely exactly zero
    return residual / ErrorSum;
  } else {
    ATH_MSG_DEBUG("Error on Track and Cluster are 0. Returning Pull value 0.");
    return 0.;
  }
}

// ====================================================================================================
//                              SCTTracksMonTool :: bookGeneralHistos
// ====================================================================================================
StatusCode
SCTTracksMonTool::bookGeneralHistos() {
  if (newRunFlag()) {
    string stem{m_path + "/SCT/GENERAL/tracks/"};
    MonGroup Tracks{this, m_path + "SCT/GENERAL/tracks", run, ATTRIB_UNMANAGED};

    const string regionNames[N_REGIONS]{"EndCapC", "Barrel", "EndCapA"};

    // Book histogram of number of tracks per region
    m_tracksPerRegion = new TH1F("tracksPerRegion", "Number of tracks in eta regions", N_REGIONS, 0, N_REGIONS);
    for (unsigned int iReg{0}; iReg<N_REGIONS; iReg++) {
      m_tracksPerRegion->GetXaxis()->SetBinLabel(iReg+1, regionNames[iReg].c_str());
    }
    ATH_CHECK(Tracks.regHist(m_tracksPerRegion));
    for(unsigned int iReg{0}; iReg<N_REGIONS; iReg++) {
      m_totalResidual[iReg] = new TH1F(("total"+regionNames[iReg]+"Residual").c_str(),
                                       ("Overall Residual Distribution for the "+regionNames[iReg]).c_str(),
                                       100, -0.5, 0.5);
      m_totalResidual[iReg]->GetXaxis()->SetTitle("Residual [mm]");
      ATH_CHECK(Tracks.regHist(m_totalResidual[iReg])); 

      m_totalPull[iReg] = new TH1F(("total"+regionNames[iReg]+"Pull").c_str(),
                                   ("Overall Pull Distribution for the "+regionNames[iReg]).c_str(),
                                   100, -5, 5);
      m_totalPull[iReg]->GetXaxis()->SetTitle("Pull");
      ATH_CHECK(Tracks.regHist(m_totalPull[iReg]));
   }

    if (m_doTrigger) {
      m_trackTrigger = new TH1I("trackTriggers", "Tracks for different trigger types", N_TRIGGER_TYPES, -0.5, 7.5);
      // first bin, last bin
      m_trackTriggerRate = new TProfile("trackTriggersRate", "Track per event for different trigger types",
                                        N_TRIGGER_TYPES, -0.5, 7.5);            // first bin, last bin
      for (int trig{0}; trig < N_TRIGGER_TYPES; ++trig) {
        m_trackTrigger->GetXaxis()->SetBinLabel(trig + 1, s_triggerNames[trig].c_str());
        m_trackTriggerRate->GetXaxis()->SetBinLabel(trig + 1, s_triggerNames[trig].c_str());
      }
      ATH_CHECK(Tracks.regHist(m_trackTrigger));
      ATH_CHECK(Tracks.regHist(m_trackTriggerRate));
    }
    // Book histogram of track rate for different regions of the detector
    m_trackRate = new TProfile("SCTTrackRate", "Track per event for SCT regions", 3, 0.0, 3.0);
    for(unsigned int iReg{0}; iReg<N_REGIONS; iReg++) {
      m_trackRate->GetXaxis()->SetBinLabel(iReg+1, regionNames[iReg].c_str());
    }

    ATH_CHECK(Tracks.regHist(m_trackRate));
    //
    m_trk_ncluHisto = new TH1F("trk_sct_hits", "SCT HITS per single Track", N_HIT_BINS, FIRST_HIT_BIN, LAST_HIT_BIN);
    m_trk_ncluHisto->GetXaxis()->SetTitle("Num of Hits");
    ATH_CHECK(Tracks.regHist(m_trk_ncluHisto));

    m_trk_nclu_totHisto = new TH1F("sct_hits_onall_tracks", "SCT HITS on all event Tracks", N_HIT_BINS, FIRST_HIT_BIN,
                                   LAST_HIT_BIN);
    m_trk_nclu_totHisto->GetXaxis()->SetTitle("Num of SCT Hits");
    m_trk_nclu_totHisto->GetYaxis()->SetTitle("Num of Events");
    ATH_CHECK(Tracks.regHist(m_trk_nclu_totHisto));

    m_trk_chi2 = new TH1F("trk_chi2", "Track #chi^{2} div ndf", 150, 0., 150.);
    m_trk_chi2->GetXaxis()->SetTitle("Number of track #chi^{2}/NDF");
    ATH_CHECK(Tracks.regHist(m_trk_chi2));

    m_trk_N = new TH1F("trk_N", "Number of tracks", 400, 0, 4000);
    m_trk_N->GetXaxis()->SetTitle("Number of tracks");
    ATH_CHECK(Tracks.regHist(m_trk_N));


    m_trk_pt = new TH1F("trk_pt", "Track P_{T}", 150, 0., 150.);
    m_trk_pt->GetXaxis()->SetTitle("P_{T} [GeV]");
    ATH_CHECK(Tracks.regHist(m_trk_pt));

    m_trk_d0 = new TH1F("trk_d0", "Track d0", 160, -40., 40.);
    m_trk_d0->GetXaxis()->SetTitle("d0 [mm]");
    ATH_CHECK(Tracks.regHist(m_trk_d0));

    m_trk_z0 = new TH1F("trk_z0", "Track z0", 200, -200., 200.);
    m_trk_z0->GetXaxis()->SetTitle("z0 [mm]");
    ATH_CHECK(Tracks.regHist(m_trk_z0));

    m_trk_phi = new TH1F("trk_phi", "Track Phi", 160, -4, 4.);
    m_trk_phi->GetXaxis()->SetTitle("#phi [rad]");
    ATH_CHECK(Tracks.regHist(m_trk_phi));

    m_trk_eta = new TH1F("trk_eta", "Track Eta", 160, -4., 4.);
    m_trk_eta->GetXaxis()->SetTitle("#eta");
    ATH_CHECK(Tracks.regHist(m_trk_eta));

    if (m_environment == AthenaMonManager::online) {
      m_nTracks = new TH1I("sct_tracks_vs_en", "Number of Tracks vs Event Number", m_evtsbins, 1, m_evtsbins + 1);
      m_nTracks->GetXaxis()->SetTitle("Event Number");
      m_nTracks->GetYaxis()->SetTitle("Num of Tracks");
      m_nTracks_buf.reserve(m_evtsbins);
      m_nTracks_pos = 0;
      ATH_CHECK(Tracks.regHist(m_nTracks));
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode
SCTTracksMonTool::bookTrackHistos(const Bec becVal) {
  if (not newRunFlag()) {
    return StatusCode::SUCCESS;
  }
  static const string pathDelimiter{"/"};
  static const string streamDelimiter{"_"};
  static const string regionNames[N_REGIONS] = {
    "Negative Endcap", "Barrel", "Positive Endcap"
  };
  static const string abbreviations[N_REGIONS] = {
    "ECm", "", "ECp"
  };
  static const string localPaths[N_REGIONS] = {
    "SCT/SCTEC/tracks", "SCT/SCTB/tracks", "SCT/SCTEA/tracks"
  };
  static const string localPathsResi[N_REGIONS] = {
    "SCT/SCTEC/tracks", "SCT/SCTB/tracks", "SCT/SCTEA/tracks"
  };
  static const string localPathsPull[N_REGIONS] = {
    "SCT/SCTEC/tracks", "SCT/SCTB/tracks", "SCT/SCTEA/tracks"
  };
  static const unsigned int limits[N_REGIONS] = {
    N_DISKSx2, N_BARRELSx2, N_DISKSx2
  };
  const unsigned int systemIndex{bec2Index(becVal)};

  const string polarityString{regionNames[systemIndex]};
  const string abbreviation{abbreviations[systemIndex]};
  const string localPath{localPaths[systemIndex]};
  const string localPathResi{localPathsResi[systemIndex]};
  const string localPathPull{localPathsPull[systemIndex]};
  const unsigned int limit{limits[systemIndex]};
  vector<TH1F*>* p_residualsSummary{&m_psctresiduals_summaryHistoVector[systemIndex]};
  vector<TH1F*>* p_pullsSummary{&m_psctpulls_summaryHistoVector[systemIndex]};

  MonGroup endCapTracks{this, m_path + localPath, run, ATTRIB_UNMANAGED};
  MonGroup endCapTracksResi{this, m_path + localPathResi, run, ATTRIB_UNMANAGED};
  MonGroup endCapTracksPull{this, m_path + localPathPull, run, ATTRIB_UNMANAGED};

  p_residualsSummary->clear();
  p_pullsSummary->clear();

  string stem{m_stream + pathDelimiter + localPath + pathDelimiter};
  for (unsigned int i{0}; i < limit; ++i) {
    LayerSideFormatter layerSide{i, systemIndex};
    string streamResidual{string{"residuals"} + abbreviation + streamDelimiter + layerSide.name()};
    string streamPull{string{"pulls"} + abbreviation + streamDelimiter + layerSide.name()};
    string titleResidual{string{"SCT Residuals for "} + polarityString + ": " + layerSide.title()};
    string titlePull{string{"SCT Pulls: "} + layerSide.title()};
    ATH_CHECK(h1Factory(streamResidual + "_summary", "Summary " + titleResidual, 0.5, endCapTracksResi, *p_residualsSummary));
    ATH_CHECK(h1Factory(streamPull + "_summary", "Summary " + titlePull, 5., endCapTracksPull, *p_pullsSummary));
  }
  
  
  if (m_environment != AthenaMonManager::online) {
    vector<TProfile2D*>* p_residuals{&m_psctresidualsHistoVector[systemIndex]};
    vector<TProfile2D*>* p_pulls{&m_psctpullsHistoVector[systemIndex]};
    vector<TH2F*>* p_residualsRms{&m_psctresidualsRMSHistoVector[systemIndex]};
    vector<TH2F*>* p_pullsRms{&m_psctpullsRMSHistoVector[systemIndex]};

    p_residuals->clear();
    p_pulls->clear();
    p_residualsRms->clear();
    p_pullsRms->clear();
    
    for (unsigned int i{0}; i < limit; ++i) {
      LayerSideFormatter layerSide{i, systemIndex};
      string streamResidual{string{"residuals"} + abbreviation + streamDelimiter + layerSide.name()};
      string streamPull{string{"pulls"} + abbreviation + streamDelimiter + layerSide.name()};
      string titleResidual{string{"SCT Residuals for "} + polarityString + ": " + layerSide.title()};
      string titlePull{string{"SCT Pulls: "} + layerSide.title()};
      
      ATH_CHECK(p2Factory(streamResidual, titleResidual, becVal, endCapTracksResi, *p_residuals));
      ATH_CHECK(p2Factory(streamPull, titlePull, becVal, endCapTracksPull, *p_pulls));
    }
    
    for (unsigned int i{0}; i < limit; ++i) {
      LayerSideFormatter layerSide{i, systemIndex};
      const string layerString{to_string(i / 2)};
      const string sideString{to_string(i % 2)};
      string streamResidual{"residualsRMS" + abbreviation + streamDelimiter + layerString + streamDelimiter
                            + sideString};
      string streamPull{"pullsRMS" + abbreviation + streamDelimiter + layerString + streamDelimiter + sideString};
      string titleResidual{"SCT Residuals RMS for " + polarityString + ": " + layerSide.title()};
      string titlePull{"SCT Pulls RMS for " + polarityString + ": " + layerSide.title()};
      ATH_CHECK(h2Factory(streamResidual, titleResidual, becVal, endCapTracksResi, *p_residualsRms));
      ATH_CHECK(h2Factory(streamPull, titlePull, becVal, endCapTracksPull, *p_pullsRms));
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode
SCTTracksMonTool::h2Factory(const string& name, const string& title,
                            const Bec bec, MonGroup& registry, vector<TH2F*>& storageVector) const {
  int firstEta{FIRST_ETA_BIN}, lastEta{LAST_ETA_BIN}, 
      firstPhi{FIRST_PHI_BIN}, lastPhi{LAST_PHI_BIN},
      nEta{N_ETA_BINS}, nPhi{N_PHI_BINS};

  if (bec != BARREL) {
    firstEta = FIRST_ETA_BIN_EC;
    lastEta = LAST_ETA_BIN_EC;
    firstPhi = FIRST_PHI_BIN_EC;
    lastPhi = LAST_PHI_BIN_EC;
    nEta = N_ETA_BINS_EC;
    nPhi = N_PHI_BINS_EC;
  }
  TH2F* tmp{new TH2F{name.c_str(), title.c_str(), nEta, firstEta - 0.5, lastEta + 0.5, nPhi, firstPhi - 0.5, lastPhi + 0.5}};
  ATH_CHECK(registry.regHist(tmp));
  storageVector.push_back(tmp);
  return StatusCode::SUCCESS;
}

StatusCode
SCTTracksMonTool::p2Factory(const string& name, const string& title,
                            const Bec bec, MonGroup& registry, vector<TProfile2D*>& storageVector) const {
  int firstEta{FIRST_ETA_BIN}, lastEta{LAST_ETA_BIN}, 
      firstPhi{FIRST_PHI_BIN}, lastPhi{LAST_PHI_BIN},
      nEta{N_ETA_BINS}, nPhi{N_PHI_BINS};

  if (bec != BARREL) {
    firstEta = FIRST_ETA_BIN_EC;
    lastEta = LAST_ETA_BIN_EC;
    firstPhi = FIRST_PHI_BIN_EC;
    lastPhi = LAST_PHI_BIN_EC;
    nEta = N_ETA_BINS_EC;
    nPhi = N_PHI_BINS_EC;
  }
  TProfile2D* tmp{new TProfile2D{name.c_str(), title.c_str(),
                             nEta, firstEta - 0.5, lastEta + 0.5,
                             nPhi, firstPhi - 0.5, lastPhi + 0.5,
                             "s"}};
  ATH_CHECK(registry.regHist(tmp));
  storageVector.push_back(tmp);
  return StatusCode::SUCCESS;
}

StatusCode
SCTTracksMonTool::h1Factory(const string& name, const string& title, const float extent, MonGroup& registry,
                            vector<TH1F*>& storageVector) const {
  static const unsigned int nbins{100};
  const float lo{-extent};
  const float hi{extent};
  TH1F* tmp{new TH1F{name.c_str(), title.c_str(), nbins, lo, hi}};

  ATH_CHECK(registry.regHist(tmp));
  storageVector.push_back(tmp);
  return StatusCode::SUCCESS;
}

StatusCode
SCTTracksMonTool::checkTriggers(bitset<N_TRIGGER_TYPES>& firedTriggers) const {
  SG::ReadHandle<xAOD::EventInfo> evtInfo{m_eventInfoKey};
  if (evtInfo.isValid()) {
    firedTriggers = evtInfo->level1TriggerType();

    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

bool
SCTTracksMonTool::hasTriggerFired(const unsigned int trigger, const bitset<N_TRIGGER_TYPES>& firedTriggers) const {
  return ((trigger < N_TRIGGER_TYPES) ? firedTriggers.test(trigger) : false);
}

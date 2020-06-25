/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TrkEventPrimitives/FitQuality.h"
//Eigen stuff
#include "EventPrimitives/EventPrimitivesHelpers.h"
//
#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkEventPrimitives/TrackStateDefs.h"
#include "TrkTrackSummary/TrackSummary.h"

#include "xAODEventInfo/EventInfo.h"

#include "InDetIdentifier/TRT_ID.h"

#include "TrkToolInterfaces/ITrackSummaryTool.h"

#include "TrkAlignEvent/AlignModule.h"
#include "TrkAlignEvent/AlignTSOS.h"
#include "TrkAlignEvent/AlignTrack.h"
#include "TrkAlignEvent/AlignPar.h"
#include "TrkAlignEvent/Residual.h"

#include "TrkAlignInterfaces/IAlignModuleTool.h"

#include "InDetAlignNtupleTools/SimpleIDNtupleTool.h"

#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include <cmath>


namespace InDet {
//________________________________________________________________________
  SimpleIDNtupleTool::SimpleIDNtupleTool(const std::string& type, const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent)
    , m_trackSumTool("Trk::TrackSummaryTool/TrackSummaryTool", this)
    , m_alignModuleTool{}
    , m_idHelper{}
    , m_file{}
    , m_tree(nullptr)
    , m_runNumber{}
    , m_evtNumber{}
    , m_xvtx{}
    , m_yvtx{}
    , m_zvtx{}
    , m_d0{}
    , m_z0{}
    , m_phi0{}
    , m_theta{}
    , m_qoverp{}
    , m_pt{}
    , m_eta{}
    , m_chi2{}
    , m_ndof{}
    , m_chi2prob{}
    , m_nhits{}
    , m_nhitstrt{}
    , m_nhitssct{}
    , m_nhitspix{}
    , m_nshared{}
    , m_nshsct{}
    , m_nshpix{}
    , m_nholes{}
    , m_nhsct{}
    , m_nhpix{}
    , m_hit_rho{}
    , m_hit_phi{}
    , m_hit_z{}
    , m_hit_cotth{}
    , m_hit_eta{}
    , m_hit_xloc{}
    , m_hit_yloc{}
    , m_hit_zloc{}
    , m_hit_drd{}
    , m_hit_sig{}
    , m_hit_resx{}
    , m_hit_resx_err{}
    , m_hit_resy{}
    , m_hit_resy_err{}
    , m_hit_tpar_x{}
    , m_hit_tpar_y{}
    , m_hit_det{}
    , m_hit_type{}
    , m_hit_derivx_transx{}
    , m_hit_derivx_transy{}
    , m_hit_derivx_transz{}
    , m_hit_derivx_rotx{}
    , m_hit_derivx_roty{}
    , m_hit_derivx_rotz{}
    , m_hit_derivx_bowx{}
    , m_hit_derivx_bowy{}
    , m_hit_derivx_bowz{}
    , m_hit_derivy_transx{}
    , m_hit_derivy_transy{}
    , m_hit_derivy_transz{}
    , m_hit_derivy_rotx{}
    , m_hit_derivy_roty{}
    , m_hit_derivy_rotz{}
    , m_hit_derivy_bowx{}
    , m_hit_derivy_bowy{}
    , m_hit_derivy_bowz{}
    , m_max_hits{150}
    , m_storeDerivatives{false} {
    declareInterface<Trk::IFillNtupleTool>(this);
    declareProperty("TrackSummaryTool", m_trackSumTool, "tool to extract track info");
    declareProperty("AlignModuleTool", m_alignModuleTool);
    declareProperty("MaxHits", m_max_hits);
    declareProperty("StoreDerivatives", m_storeDerivatives);
  }

//________________________________________________________________________
  SimpleIDNtupleTool::~SimpleIDNtupleTool() {
    if (m_hit_rho) {
      delete [] m_hit_rho;
      delete [] m_hit_phi;
      delete [] m_hit_z;
      delete [] m_hit_cotth;
      delete [] m_hit_eta;
      delete [] m_hit_xloc;
      delete [] m_hit_yloc;
      delete [] m_hit_zloc;
      delete [] m_hit_drd;
      delete [] m_hit_sig;
      delete [] m_hit_tpar_x;
      delete [] m_hit_tpar_y;
      delete [] m_hit_resx;
      delete [] m_hit_resx_err;
      delete [] m_hit_resy;
      delete [] m_hit_resy_err;
      delete [] m_hit_det;
      delete [] m_hit_type;

      if (m_hit_derivx_transx) {
        delete [] m_hit_derivx_transx;
        delete [] m_hit_derivx_transy;
        delete [] m_hit_derivx_transz;
        delete [] m_hit_derivx_rotx;
        delete [] m_hit_derivx_roty;
        delete [] m_hit_derivx_rotz;
        delete [] m_hit_derivx_bowx;
        delete [] m_hit_derivx_bowy;
        delete [] m_hit_derivx_bowz;
        delete [] m_hit_derivy_transx;
        delete [] m_hit_derivy_transy;
        delete [] m_hit_derivy_transz;
        delete [] m_hit_derivy_rotx;
        delete [] m_hit_derivy_roty;
        delete [] m_hit_derivy_rotz;
        delete [] m_hit_derivy_bowx;
        delete [] m_hit_derivy_bowy;
        delete [] m_hit_derivy_bowz;
      }
    }
  }

//________________________________________________________________________
  StatusCode SimpleIDNtupleTool::initialize() {
    ATH_MSG_DEBUG("initialize() of SimpleIDNtupleTool");

    // retrieve TRT ID helper
    ATH_CHECK(detStore()->retrieve(m_idHelper, "TRT_ID"));

    // get TrackSummaryTool
    ATH_CHECK(m_trackSumTool.retrieve());

    // get AlignModuleTool
    if (m_alignModuleTool.empty() || m_alignModuleTool.retrieve().isFailure()) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_alignModuleTool);
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Retrieved tool " << m_alignModuleTool);

    return StatusCode::SUCCESS;
  }

//________________________________________________________________________
  StatusCode SimpleIDNtupleTool::fillNtuple() {
    if (m_file && m_file->IsOpen()) {
      m_file->cd();
      if (m_tree) m_tree->Write();
      else ATH_MSG_WARNING("Variable m_tree not set, nothing to write.");
    } else ATH_MSG_WARNING("Variable m_file not set or not pointing to open output file.");
    return StatusCode::SUCCESS;
  }

//________________________________________________________________________
  StatusCode SimpleIDNtupleTool::finalize() {
    ATH_MSG_DEBUG("finalize() of SimpleIDNtupleTool");

    return StatusCode::SUCCESS;
  }

//________________________________________________________________________
  void SimpleIDNtupleTool::dumpTrack(int itrk, const Trk::AlignTrack* alignTrack) {
    ATH_MSG_DEBUG("In dumpTrack()");

    if (!m_tree) initializeNtuple();

    // get run and event numbers
    ATH_MSG_DEBUG("Retrieving event info.");
    const xAOD::EventInfo* eventInfo;
    if (evtStore()->retrieve(eventInfo).isFailure()) ATH_MSG_ERROR("Could not retrieve event info.");
    else {
      m_runNumber = eventInfo->runNumber();
      m_evtNumber = eventInfo->eventNumber();
    }

    // initialize variables
    constexpr double invalidParameterValue{-999.};
    m_d0 = invalidParameterValue;
    m_z0 = invalidParameterValue;
    m_phi0 = invalidParameterValue;
    m_theta = invalidParameterValue;
    m_qoverp = invalidParameterValue;;
    m_pt = invalidParameterValue;
    m_eta = invalidParameterValue;
    m_xvtx = invalidParameterValue;
    m_yvtx = invalidParameterValue;
    m_zvtx = invalidParameterValue;;

    constexpr int initialiseNHits{0};
    m_nhitspix = initialiseNHits;
    m_nhitssct = initialiseNHits;
    m_nhitstrt = initialiseNHits;
    m_nshared = initialiseNHits;
    m_nshpix = initialiseNHits;
    m_nshsct = initialiseNHits;
    m_nholes = initialiseNHits;
    m_nhpix = initialiseNHits;
    m_nhsct = initialiseNHits;
    
    constexpr double invalidChiSq(-1e12);
    constexpr int invalidDegreesOfFreedom{-999};
    m_chi2 = invalidChiSq;
    m_chi2prob = invalidChiSq;
    m_ndof = invalidDegreesOfFreedom;

    const Trk::Perigee* aMeasPer = (alignTrack->perigeeParameters());
    if (not aMeasPer) ATH_MSG_ERROR("Could not get Trk::MeasuredPerigee");
    else {
      m_d0 = aMeasPer->parameters()[Trk::d0];
      m_z0 = aMeasPer->parameters()[Trk::z0];
      m_phi0 = aMeasPer->parameters()[Trk::phi0];
      m_theta = aMeasPer->parameters()[Trk::theta];
      m_qoverp = aMeasPer->parameters()[Trk::qOverP];
      ATH_MSG_DEBUG(itrk << ". " << " Track Parameters (d0, z0, phi0, theta, q/p)");
      ATH_MSG_DEBUG(" " << m_d0 << ", " << m_z0 << ", " << m_phi0 << ", " << m_theta << ", " << m_qoverp);
      m_pt = std::sqrt((aMeasPer->momentum().x()) * (aMeasPer->momentum().x())
                       + (aMeasPer->momentum().y()) * (aMeasPer->momentum().y()));
      m_eta = aMeasPer->eta();
      m_xvtx = aMeasPer->position().x();
      m_yvtx = aMeasPer->position().y();
      m_zvtx = aMeasPer->position().z();
      std::unique_ptr<const Trk::TrackSummary> summary = 
        m_trackSumTool->summary((*alignTrack));
      if (not summary) ATH_MSG_ERROR("Could not get Trk::TrackSummary");
      else {
        // hits
        m_nhitspix = summary->get(Trk::numberOfPixelHits);
        m_nhitssct = summary->get(Trk::numberOfSCTHits);
        m_nhitstrt = summary->get(Trk::numberOfTRTHits);

        ATH_MSG_DEBUG("   -- number of Pixel hits : " << m_nhitspix);
        ATH_MSG_DEBUG("   -- number of SCT hits   : " << m_nhitssct);
        ATH_MSG_DEBUG("   -- number of TRT hits   : " << m_nhitstrt);

        // shared hits
        m_nshpix = summary->get(Trk::numberOfPixelSharedHits);
        m_nshsct = summary->get(Trk::numberOfSCTSharedHits);
        m_nshared = m_nshpix + m_nshsct;

        if (m_nshpix < 0) m_nshpix = 0;
        if (m_nshsct < 0) m_nshsct = 0;
        if (m_nshared < 0) m_nshared = 0;

        ATH_MSG_DEBUG(" - number of shared hits         : " << m_nshared);
        ATH_MSG_DEBUG("  -- number of Pixel shared hits : " << m_nshpix);
        ATH_MSG_DEBUG("  -- number of SCT shared hits   : " << m_nshsct);

        // holes
        m_nhpix = summary->get(Trk::numberOfPixelHoles);
        m_nhsct = summary->get(Trk::numberOfSCTHoles);
        m_nholes = m_nhpix + m_nhsct;

        if (m_nhpix < 0) m_nhpix = 0;
        if (m_nhsct < 0) m_nhsct = 0;
        if (m_nholes < 0) m_nholes = 0;

        ATH_MSG_DEBUG(" - number of Pixel holes : " << m_nhpix);
        ATH_MSG_DEBUG("  -- number of SCT holes : " << m_nhsct);
        ATH_MSG_DEBUG("  -- number of holes     : " << m_nholes);
      }

      // get fit quality and chi2 probability of track
      const Trk::FitQuality* fitQual = alignTrack->fitQuality();
      if (not fitQual) ATH_MSG_ERROR("No fit quality assigned to the track");
      else {
        if (fitQual->chiSquared() > 0. && fitQual->numberDoF() > 0) {
          m_chi2 = fitQual->chiSquared();
          m_ndof = fitQual->numberDoF();
          m_chi2prob = TMath::Prob(m_chi2, m_ndof);

          ATH_MSG_DEBUG("  - chi2             : " << m_chi2);
          ATH_MSG_DEBUG("  - ndof             : " << m_ndof);
          ATH_MSG_DEBUG("  - chi2/ndof        : " << m_chi2 / (double) m_ndof);
          ATH_MSG_DEBUG("  - chi2 propability : " << m_chi2prob);
        }
      }
    }

    // store information for all hits on this track, including
    // scattering centers
    const Trk::AlignTSOSCollection* alignTSOSCollection = alignTrack->alignTSOSCollection();
    int nhitsTotal = alignTSOSCollection->size();
    if (nhitsTotal > m_max_hits) nhitsTotal = m_max_hits;


    fillHits(alignTrack);


    m_file->cd();
    m_tree->Fill();
    ATH_MSG_DEBUG("tree filled");

    ++itrk;
    return;
  }

//________________________________________________________________________
  void SimpleIDNtupleTool::fillHits(const Trk::AlignTrack* alignTrack) {
    ATH_MSG_DEBUG("In fillHits()");

    // store information for all  hits on this track
    // initialize ntuple variables
    for (int i = 0; i < m_max_hits; i++) {
      m_hit_rho[i] = -999.;
      m_hit_phi[i] = -999.;
      m_hit_z[i] = -999.;
      m_hit_cotth[i] = -999.;
      m_hit_eta[i] = -999.;
      m_hit_xloc[i] = -999.;
      m_hit_yloc[i] = -999.;
      m_hit_zloc[i] = -999.;
      m_hit_drd[i] = -999.;
      m_hit_sig[i] = -999.;

      m_hit_tpar_x[i] = -999.;
      m_hit_tpar_y[i] = -999.;

      m_hit_resx[i] = -999.;
      m_hit_resx_err[i] = -999.;
      m_hit_resy[i] = -999.;
      m_hit_resy_err[i] = -999.;

      m_hit_det[i] = -999;
      m_hit_type[i] = -999;

      if (m_storeDerivatives) {
        m_hit_derivx_transx[i] = -999.;
        m_hit_derivx_transy[i] = -999.;
        m_hit_derivx_transz[i] = -999.;
        m_hit_derivx_rotx[i] = -999.;
        m_hit_derivx_roty[i] = -999.;
        m_hit_derivx_rotz[i] = -999.;
        m_hit_derivx_bowx[i] = -999.;
        m_hit_derivx_bowy[i] = -999.;
        m_hit_derivx_bowz[i] = -999.;

        m_hit_derivy_transx[i] = -999.;
        m_hit_derivy_transy[i] = -999.;
        m_hit_derivy_transz[i] = -999.;
        m_hit_derivy_rotx[i] = -999.;
        m_hit_derivy_roty[i] = -999.;
        m_hit_derivy_rotz[i] = -999.;
        m_hit_derivy_bowx[i] = -999.;
        m_hit_derivy_bowy[i] = -999.;
        m_hit_derivy_bowz[i] = -999.;
      }
    }

    const Trk::AlignTSOSCollection* alignTSOSCollection = alignTrack->alignTSOSCollection();
    m_nhits = alignTSOSCollection->size();
    ATH_MSG_DEBUG("have " << m_nhits << " hits on alignTrack in total");

    int nhits = 0;

    DataVector<Trk::AlignTSOS>::const_iterator atsit = alignTSOSCollection->begin();
    DataVector<Trk::AlignTSOS>::const_iterator atsit_end = alignTSOSCollection->end();
    for (; atsit != atsit_end; ++atsit) {
      const Trk::AlignTSOS* atsos = *atsit;
      ATH_MSG_DEBUG("in loop over AlignTSOS, nhits=" << nhits);
      if (nhits == m_max_hits) break;

      if (!atsos->type(Trk::TrackStateOnSurface::Measurement)) continue;

      m_hit_det[nhits] = atsos->measType();
      ATH_MSG_DEBUG("measurement type: " << atsos->dumpMeasType());


      // get RIO
      const Trk::RIO_OnTrack* rio = atsos->rio();
      ATH_MSG_DEBUG("have RIO");
      Amg::Vector3D pos = rio->globalPosition();

      double locPosX(0.);
      if (rio->localParameters().contains(Trk::locX)) locPosX = rio->localParameters()[Trk::locX];
      double locPosY(0.);
      if (rio->localParameters().contains(Trk::locY)) locPosY = rio->localParameters()[Trk::locY];
      double locPosZ(0.);
      if (rio->localParameters().contains(Trk::locZ)) locPosZ = rio->localParameters()[Trk::locZ];

      double error = Amg::error(rio->localCovariance(), Trk::locX);
      ATH_MSG_DEBUG("error: " << error);

      Identifier id = rio->identify();


      m_hit_xloc[nhits] = locPosX;
      m_hit_yloc[nhits] = locPosY;
      m_hit_zloc[nhits] = locPosZ;

      m_hit_rho[nhits] = pos.perp();
      m_hit_phi[nhits] = pos.phi();
      m_hit_z[nhits] = pos.z();
      m_hit_sig[nhits] = error;

      // get residual x
      std::vector<Trk::Residual>::const_iterator ires = atsos->firstResidual();

      m_hit_resx[nhits] = ires->residual();
      m_hit_resx_err[nhits] = ires->error();

      // get residual y if applicable
      if (atsos->nResDim() > 1) {
        ++ires;
        m_hit_resy[nhits] = ires->residual();
        m_hit_resy_err[nhits] = ires->error();
      }

      // get local track parameters
      const Trk::TrackParameters* trackPars = 0;
      if (atsos->firstResidual()->residualType() == Trk::HitOnly) trackPars = atsos->trackParameters();
      else trackPars = atsos->unbiasedTrackPars();

      m_hit_tpar_x[nhits] = trackPars->parameters()[Trk::locX];
      m_hit_tpar_y[nhits] = trackPars->parameters()[Trk::locY];


      if (m_idHelper->is_barrel(id)) {
        m_hit_cotth[nhits] = 0.;
        m_hit_eta[nhits] = 0.;
      } else {
        m_hit_cotth[nhits] = 1. / std::tan(pos.theta());
        double etaval = -std::log(std::tan(std::atan(1. / std::fabs(m_hit_cotth[nhits])) / 2.));
        if (m_hit_cotth[nhits] < 0.) etaval *= -1.;
        m_hit_eta[nhits] = etaval;
      }
      if (atsos->measType() != Trk::TrackState::TRT) m_hit_drd[nhits] = locPosX;

      // fill derivatives
      if (m_storeDerivatives) {
        //const std::vector<CLHEP::HepVector> * derivs = atsos->derivatives();
        const std::vector<Amg::VectorX>* derivs = atsos->derivatives();
        if (derivs) {
          int npar = derivs->at(0).rows();

          const DataVector<Trk::AlignPar>* alignPars = m_alignModuleTool->getAlignPars(atsos->module());
          int naPar = alignPars->size();

          if (npar != naPar) ATH_MSG_WARNING("number of derivatives on AlignTSOS doesn't match the one on AlignModule");

          else {
            for (int i = 0; i < npar; i++) {
              double derivative = (*derivs)[0][i];
              Trk::AlignModule::TransformParameters ptype = alignPars->at(i)->paramType();
              switch (ptype) {
              case Trk::AlignModule::TransX:
                m_hit_derivx_transx[nhits] = derivative;
                break;

              case Trk::AlignModule::TransY:
                m_hit_derivx_transy[nhits] = derivative;
                break;

              case Trk::AlignModule::TransZ:
                m_hit_derivx_transz[nhits] = derivative;
                break;

              case Trk::AlignModule::RotX:
                m_hit_derivx_rotx[nhits] = derivative;
                break;

              case Trk::AlignModule::RotY:
                m_hit_derivx_roty[nhits] = derivative;
                break;

              case Trk::AlignModule::RotZ:
                m_hit_derivx_rotz[nhits] = derivative;
                break;

              case Trk::AlignModule::BowX:
                m_hit_derivx_bowx[nhits] = derivative;
                break;

              case Trk::AlignModule::BowY:
                m_hit_derivx_bowy[nhits] = derivative;
                break;

              case Trk::AlignModule::BowZ:
                m_hit_derivx_bowz[nhits] = derivative;
                break;

              default:
                ATH_MSG_WARNING("Unknown parameter type " << ptype);
              }
            }
            if (atsos->nResDim() > 1) {
              for (int i = 0; i < npar; i++) {
                double derivative = (*derivs)[1][i];
                Trk::AlignModule::TransformParameters ptype = alignPars->at(i)->paramType();
                switch (ptype) {
                case Trk::AlignModule::TransX:
                  m_hit_derivy_transx[nhits] = derivative;
                  break;

                case Trk::AlignModule::TransY:
                  m_hit_derivy_transy[nhits] = derivative;
                  break;

                case Trk::AlignModule::TransZ:
                  m_hit_derivy_transz[nhits] = derivative;
                  break;

                case Trk::AlignModule::RotX:
                  m_hit_derivy_rotx[nhits] = derivative;
                  break;

                case Trk::AlignModule::RotY:
                  m_hit_derivy_roty[nhits] = derivative;
                  break;

                case Trk::AlignModule::RotZ:
                  m_hit_derivy_rotz[nhits] = derivative;
                  break;

                case Trk::AlignModule::BowX:
                  m_hit_derivy_bowx[nhits] = derivative;
                  break;

                case Trk::AlignModule::BowY:
                  m_hit_derivy_bowy[nhits] = derivative;
                  break;

                case Trk::AlignModule::BowZ:
                  m_hit_derivy_bowz[nhits] = derivative;
                  break;

                default:
                  ATH_MSG_WARNING("Unknown parameter type " << ptype);
                }
              }
            }
          }
        }
      } // end of storing derivatives

      ++nhits;

      if (nhits >= m_max_hits) {
        ATH_MSG_WARNING("Number of hits on track more that maximum: " << m_nhits << " > " << m_max_hits << " "
                                                                      << "Some hits will not be stored.");
        break;
      }
    } //atsos

    ATH_MSG_DEBUG("done with loop over TSOS");
  }

//________________________________________________________________________
  void SimpleIDNtupleTool::storeHitmap() {
  }

//________________________________________________________________________
  void SimpleIDNtupleTool::fillHitmap() {
  }

//________________________________________________________________________
  void SimpleIDNtupleTool::fillSummary() {
  }

//________________________________________________________________________
  void SimpleIDNtupleTool::showStatistics() {
  }

//________________________________________________________________________
  void SimpleIDNtupleTool::initializeNtuple() {
    m_hit_rho = new double[m_max_hits];
    m_hit_phi = new double[m_max_hits];
    m_hit_z = new double[m_max_hits];
    m_hit_cotth = new double[m_max_hits];
    m_hit_eta = new double[m_max_hits];
    m_hit_xloc = new double[m_max_hits];
    m_hit_yloc = new double[m_max_hits];
    m_hit_zloc = new double[m_max_hits];
    m_hit_drd = new double[m_max_hits];
    m_hit_sig = new double[m_max_hits];

    m_hit_tpar_x = new double[m_max_hits];
    m_hit_tpar_y = new double[m_max_hits];

    m_hit_resx = new double[m_max_hits];
    m_hit_resx_err = new double[m_max_hits];
    m_hit_resy = new double[m_max_hits];
    m_hit_resy_err = new double[m_max_hits];

    m_hit_det = new int[m_max_hits];
    m_hit_type = new int[m_max_hits];

    m_file->cd();
    m_tree = new TTree("IDAlign", "Inner Detector Alignment Ntuple");

    m_tree->Branch("run", &m_runNumber, "run/I");
    m_tree->Branch("evt", &m_evtNumber, "evt/I");

    m_tree->Branch("xvtx", &m_xvtx, "xvtx/D");
    m_tree->Branch("yvtx", &m_yvtx, "yvtx/D");
    m_tree->Branch("zvtx", &m_zvtx, "zvtx/D");
    m_tree->Branch("d0", &m_d0, "d0/D");
    m_tree->Branch("z0", &m_z0, "z0/D");
    m_tree->Branch("phi0", &m_phi0, "phi0/D");
    m_tree->Branch("theta", &m_theta, "theta/D");
    m_tree->Branch("qoverp", &m_qoverp, "qoverp/D");
    m_tree->Branch("pt", &m_pt, "pt/D");
    m_tree->Branch("eta", &m_eta, "eta/D");
    m_tree->Branch("chi2", &m_chi2, "chi2/D");
    m_tree->Branch("ndof", &m_ndof, "ndof/I");
    m_tree->Branch("chi2prob", &m_chi2prob, "chi2prob/D");

    m_tree->Branch("nhits", &m_nhits, "nhits/I");
    m_tree->Branch("nhitstrt", &m_nhitstrt, "nhitstrt/I");
    m_tree->Branch("nhitssct", &m_nhitssct, "nhitssct/I");
    m_tree->Branch("nhitspix", &m_nhitspix, "nhitspix/I");
    m_tree->Branch("nshared", &m_nshared, "nshared/I");
    m_tree->Branch("nshsct", &m_nshsct, "nshsct/I");
    m_tree->Branch("nshpix", &m_nshpix, "nshpix/I");
    m_tree->Branch("nholes", &m_nholes, "nholes/I");
    m_tree->Branch("nhsct", &m_nhsct, "nhsct/I");
    m_tree->Branch("nhpix", &m_nhpix, "nhpix/I");

    m_tree->Branch("hit_rho", m_hit_rho, "hit_rho[nhits]/D");
    m_tree->Branch("hit_phi", m_hit_phi, "hit_phi[nhits]/D");
    m_tree->Branch("hit_z", m_hit_z, "hit_z[nhits]/D");
    m_tree->Branch("hit_cotth", m_hit_cotth, "hit_cotth[nhits]/D");
    m_tree->Branch("hit_eta", m_hit_eta, "hit_eta[nhits]/D");
    m_tree->Branch("hit_xloc", m_hit_xloc, "hit_xloc[nhits]/D");
    m_tree->Branch("hit_yloc", m_hit_yloc, "hit_yloc[nhits]/D");
    m_tree->Branch("hit_zloc", m_hit_zloc, "hit_zloc[nhits]/D");
    m_tree->Branch("hit_drd", m_hit_drd, "hit_drd[nhits]/D");
    m_tree->Branch("hit_sig", m_hit_sig, "hit_sig[nhits]/D");

    m_tree->Branch("hit_tpar_x", m_hit_tpar_x, "hit_tpar_x[nhits]/D");
    m_tree->Branch("hit_tpar_y", m_hit_tpar_y, "hit_tpar_y[nhits]/D");

    m_tree->Branch("hit_resx", m_hit_resx, "hit_resx[nhits]/D");
    m_tree->Branch("hit_resx_err", m_hit_resx_err, "hit_resx_err[nhits]/D");
    m_tree->Branch("hit_resy", m_hit_resy, "hit_resy[nhits]/D");
    m_tree->Branch("hit_resy_err", m_hit_resy_err, "hit_resy_err[nhits]/D");

    m_tree->Branch("hit_det", m_hit_det, "hit_det[nhits]/I");
    m_tree->Branch("hit_type", m_hit_type, "hit_type[nhits]/I");

    if (m_storeDerivatives) {
      m_hit_derivx_transx = new double[m_max_hits];
      m_hit_derivx_transy = new double[m_max_hits];
      m_hit_derivx_transz = new double[m_max_hits];
      m_hit_derivx_rotx = new double[m_max_hits];
      m_hit_derivx_roty = new double[m_max_hits];
      m_hit_derivx_rotz = new double[m_max_hits];
      m_hit_derivx_bowx = new double[m_max_hits];
      m_hit_derivx_bowy = new double[m_max_hits];
      m_hit_derivx_bowz = new double[m_max_hits];
      m_hit_derivy_transx = new double[m_max_hits];
      m_hit_derivy_transy = new double[m_max_hits];
      m_hit_derivy_transz = new double[m_max_hits];
      m_hit_derivy_rotx = new double[m_max_hits];
      m_hit_derivy_roty = new double[m_max_hits];
      m_hit_derivy_rotz = new double[m_max_hits];
      m_hit_derivy_bowx = new double[m_max_hits];
      m_hit_derivy_bowy = new double[m_max_hits];
      m_hit_derivy_bowz = new double[m_max_hits];

      m_tree->Branch("derivx_transx", m_hit_derivx_transx, "derivx_transx[nhits]/D");
      m_tree->Branch("derivx_transy", m_hit_derivx_transy, "derivx_transy[nhits]/D");
      m_tree->Branch("derivx_transz", m_hit_derivx_transz, "derivx_transz[nhits]/D");
      m_tree->Branch("derivx_rotx", m_hit_derivx_rotx, "derivx_rotx[nhits]/D");
      m_tree->Branch("derivx_roty", m_hit_derivx_roty, "derivx_roty[nhits]/D");
      m_tree->Branch("derivx_rotz", m_hit_derivx_rotz, "derivx_rotz[nhits]/D");
      m_tree->Branch("derivx_bowx", m_hit_derivx_bowx, "derivx_bowx[nhits]/D");
      m_tree->Branch("derivx_bowy", m_hit_derivx_bowy, "derivx_bowy[nhits]/D");
      m_tree->Branch("derivx_bowz", m_hit_derivx_bowz, "derivx_bowz[nhits]/D");
      m_tree->Branch("derivy_transx", m_hit_derivy_transx, "derivy_transx[nhits]/D");
      m_tree->Branch("derivy_transy", m_hit_derivy_transy, "derivy_transy[nhits]/D");
      m_tree->Branch("derivy_transz", m_hit_derivy_transz, "derivy_transz[nhits]/D");
      m_tree->Branch("derivy_rotx", m_hit_derivy_rotx, "derivy_rotx[nhits]/D");
      m_tree->Branch("derivy_roty", m_hit_derivy_roty, "derivy_roty[nhits]/D");
      m_tree->Branch("derivy_rotz", m_hit_derivy_rotz, "derivy_rotz[nhits]/D");
      m_tree->Branch("derivy_bowx", m_hit_derivy_bowx, "derivy_bowx[nhits]/D");
      m_tree->Branch("derivy_bowy", m_hit_derivy_bowy, "derivy_bowy[nhits]/D");
      m_tree->Branch("derivy_bowz", m_hit_derivy_bowz, "derivy_bowz[nhits]/D");
    }
    return;
  }
} // end namespace

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     muComb.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigmuComb
// VERSION:  V3
//
// AUTHOR:   S.Giagu <stefano.giagu@cern.ch>
//
// ********************************************************************
#include <sstream>
#include <math.h>

#include "TrigmuComb/muComb.h"
#include "TrigmuComb/muCombUtil.h"
#include "PathResolver/PathResolver.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "xAODTrigMuon/TrigMuonDefs.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigMuon/L2StandAloneMuonAuxContainer.h"
//#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuonAuxContainer.h"
//#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TrigSiSpacePointTool/ISpacePointProvider.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "AthenaKernel/Timeout.h"

// temporary
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "CLHEP/Units/SystemOfUnits.h"

ATLAS_NO_CHECK_FILE_THREAD_SAFETY;  // legacy trigger code

muComb::muComb(const std::string& name, ISvcLocator* pSvcLocator):
   HLT::FexAlgo(name, pSvcLocator),
   m_pStoreGate(NULL),
   m_backExtrapolatorG4("Trk::Extrapolator/AtlasExtrapolator"),
   m_MagFieldSvc(0),
   m_pTimerService(0)
{

   // backextrapolators
   declareProperty("UseBackExtrapolatorG4", m_useBackExtrapolatorG4 = true);
   declareProperty("BackExtrapolatorG4",    m_backExtrapolatorG4, "public tool for back extrapolating the muon tracks to the IV G4 based");

   // magnetic field
   declareProperty("UseAthenaFieldService",     m_useAthenaFieldService = true);
   declareProperty("AssumeToroidOff",           m_assumeToroidOff   = false);
   declareProperty("AssumeSolenoidOff",         m_assumeSolenoidOff = false);

   // Algorithm Strategy
   //0: auto select best option
   //1: simple angular match:
   declareProperty("MuCombStrategy", m_AlgoStrategy = 0);
   m_AlgoStrategy.verifier().setBounds(0, 1);

   // Charge assignment startegy
   //0: use muFast
   //1: use ID
   //2: use resolution model
   declareProperty("ChargeStrategy",         m_ChargeStrategy = 0);
   m_ChargeStrategy.verifier().setBounds(0, 2);

   // ID trk xAOD collection to use
   declareProperty("IDalgo", m_ID_algo_to_use = "InDetTrigTrackingxAODCnv_Muon_FTF");

   //ID tracks quality parameters
   declareProperty("MinPtTRK",      m_PtMinTrk = 1.); //GeV/c
   declareProperty("MaxAbsEtaTRK",  m_EtaMaxTrk = 2.5);

   // Matching Parameters
   //Common
   declareProperty("NdofMin",                m_NdofMin        = 9999);
   declareProperty("UseAbsPt",               m_UseAbsPt       = false);

   // LUT based backextrapolator
   declareProperty("WinEtaSigma",            m_WinEta         = 4.0);
   declareProperty("WinPhiSigma",            m_WinPhi         = 4.0);
   declareProperty("Chi2Max",                m_Chi2Max        = 1.e33);
   declareProperty("WinEtaSigmaEndCaps",     m_WinEta_EC      = 4.0);
   declareProperty("WinPhiSigmaEndCaps",     m_WinPhi_EC      = 10.);
   declareProperty("Chi2MaxEndCaps",         m_Chi2Max_EC     = 1.e33);
   declareProperty("WinEtaSigmaTS",          m_WinEta_TS      = 5.0);
   declareProperty("WinPhiSigmaTS",          m_WinPhi_TS      = 5.0);
   declareProperty("Chi2MaxTS",              m_Chi2Max_TS     = 1.e33);
   declareProperty("WinEtaSigmaFE",          m_WinEta_FE      = 10.0);
   declareProperty("WinPhiSigmaFE",          m_WinPhi_FE      = 10.0);
   declareProperty("Chi2MaxFE",              m_Chi2Max_FE     = 1.e33);
   declareProperty("WeightEta",              m_WeightEta      = 2.0);
   declareProperty("WeightPhi",              m_WeightPhi      = 2.0);

   // g4 based backextrapolator
   declareProperty("WinEtaSigma_g4",         m_WinEta_g4      = 7.0);
   declareProperty("WinPhiSigma_g4",         m_WinPhi_g4      = 7.0);
   declareProperty("Chi2Max_g4",             m_Chi2Max_g4     = 1.e33);
   declareProperty("WinEtaSigmaEndCaps_g4",  m_WinEta_EC_g4   = 7.0);
   declareProperty("WinPhiSigmaEndCaps_g4",  m_WinPhi_EC_g4   = 7.0);
   declareProperty("Chi2MaxEndCaps_g4",      m_Chi2Max_EC_g4  = 1.e33);
   declareProperty("WeightEta_g4",           m_WeightEta_g4   = 2.0);
   declareProperty("WeightPhi_g4",           m_WeightPhi_g4   = 2.0);
   declareProperty("Chi2Weight_g4",          m_Chi2Weight_g4  = 2.0);

   // Simplified DeltaR(/Pt) based match
   declareProperty("WinPt",       m_winPt = -1.0); //in GeV (disabled if < 0)
   declareProperty("WinDelta",    m_winDR = 0.2);  //strategy dependent

   // Configure the string to be used for the regular test
   declareProperty("TestString", m_test_string = "");

   // Monitored variables
   //Input
   declareMonitoredVariable("PtMS",        m_ptMS);
   declareMonitoredVariable("EtaMS",       m_etaMS);
   declareMonitoredVariable("PhiMS",       m_phiMS);
   declareMonitoredVariable("ZetaMS",      m_zetaMS);
   //ID
   declareMonitoredVariable("PtID",        m_ptID);
   declareMonitoredVariable("EtaID",       m_etaID);
   declareMonitoredVariable("PhiID",       m_phiID);
   declareMonitoredVariable("ZetaID",      m_zetaID);
   //Combined
   declareMonitoredVariable("PtMC",        m_ptMC);
   declareMonitoredVariable("DEta",        m_dEta);
   declareMonitoredVariable("DPhi",        m_dPhi);
   declareMonitoredVariable("DZeta",       m_dZeta);
   declareMonitoredVariable("DeltaR",      m_dR);
   //Failed
   declareMonitoredVariable("PtFL",        m_ptFL);
   declareMonitoredVariable("EtaFL",       m_etaFL);
   declareMonitoredVariable("PhiFL",       m_phiFL);
   //Info
   declareMonitoredVariable("Efficiency",  m_efficiency);
   declareMonitoredVariable("StrategyMC",  m_StrategyMC);
   declareMonitoredVariable("ErrorFlagMC", m_ErrorFlagMC);
   declareMonitoredVariable("MatchFlagMC", m_MatchFlagMC);

   // ID parametrized rsolution for matching
   std::vector<double> default_IDSCANRes_barrel;
   muCombUtil::setIDSCANRes(default_IDSCANRes_barrel, 0.017, 0.000000418);

   std::vector<double> default_IDSCANRes_endcap1;
   muCombUtil::setIDSCANRes(default_IDSCANRes_endcap1, 0.025, 0.0000002);

   std::vector<double> default_IDSCANRes_endcap2;
   muCombUtil::setIDSCANRes(default_IDSCANRes_endcap2, 0.030, 0.0000002);

   std::vector<double> default_IDSCANRes_endcap3;
   muCombUtil::setIDSCANRes(default_IDSCANRes_endcap3, 0.036, 0.0000004);

   std::vector<double> default_IDSCANRes_endcap4;
   muCombUtil::setIDSCANRes(default_IDSCANRes_endcap4, 0.046, 0.0000002);

   declareProperty("IDSCANBarrelRes",  m_IDSCANRes_barrel = default_IDSCANRes_barrel);
   declareProperty("IDSCANEndcap1Res", m_IDSCANRes_endcap1 = default_IDSCANRes_endcap1);
   declareProperty("IDSCANEndcap2Res", m_IDSCANRes_endcap2 = default_IDSCANRes_endcap2);
   declareProperty("IDSCANEndcap3Res", m_IDSCANRes_endcap3 = default_IDSCANRes_endcap3);
   declareProperty("IDSCANEndcap4Res", m_IDSCANRes_endcap4 = default_IDSCANRes_endcap4);
}

HLT::ErrorCode muComb::hltInitialize()
{
  ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   //   msg() << MSG::INFO << ">>>>>>> MB DEVEL - LOCAL COPY <<<<<<<" << PACKAGE_VERSION << endreq;
   m_pStoreGate = store();

   // Timer Service
   m_pTimerService = 0;

   // BackExtrapolator services
   if (m_useBackExtrapolatorG4) {
      StatusCode sc = m_backExtrapolatorG4.retrieve();
      if (sc.isFailure()) {
	ATH_MSG_ERROR("Could not retrieve " << m_backExtrapolatorG4);
         return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      } else {
	ATH_MSG_INFO("Retrieved tool " << m_backExtrapolatorG4);
      }
   } else {
     m_backExtrapolatorG4.disable();
   }

   if (m_useAthenaFieldService) {
      if (!m_MagFieldSvc) service("AtlasFieldSvc", m_MagFieldSvc, /*createIf=*/ false).ignore();
      if (m_MagFieldSvc) {
	ATH_MSG_INFO("Retrieved AtlasFieldSvc ");
      } else {
	ATH_MSG_ERROR("Could not retrieve AtlasFieldSvc");
         return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      }
   }

   ATH_MSG_INFO("Initialization completed successfully");
   return HLT::OK;
}

HLT::ErrorCode muComb::hltFinalize()
{
   return HLT::OK;
}

// muon-trk match based on angular distance
// return 0 --> match,  1 --> no match
int muComb::drptMatch(const xAOD::L2StandAloneMuon* feature, double id_pt, double id_eta, double id_phi, int algo,
                      double& combPtInv, double& combPtRes, double& deta, double& dphi, double& dr)
{
   double pt     = feature->pt() * CLHEP::GeV;
   double phi    = feature->phiMS();
   double eta    = feature->etaMS();
   return muComb::drptMatch(pt, eta, phi, id_pt, id_eta, id_phi, algo, combPtInv, combPtRes, deta, dphi, dr);
}
int muComb::drptMatch(double pt, double eta, double phi, double id_pt, double id_eta, double id_phi, int algo,
                      double& combPtInv, double& combPtRes, double& deta, double& dphi, double& dr)
{

   // algo: 1 --> R (+ pt), combined pt
   // algo: 2 --> R match, Id pt
   // algo: 3 --> R match, MS pt
   // algo: 4 --> R match, infinite pt
   if (algo < 1 || algo > 4) {
     ATH_MSG_DEBUG(" muComb::drptMatch wrong algo parameter, it is: " << algo
		   << " while must be in the range [1,4], match failed!!!");
      return 0;
   }

   double winDR = m_winDR;
   double winPt = m_winPt;

   //muFast parameters (in MeV!)
   //double phi    = feature->phiMS();
   //double eta    = feature->etaMS();
   //double pt = feature->pt() * CLHEP::GeV;

   combPtRes = 0.0;
   if (algo == 1) combPtInv = ((1. / pt) + (1. / id_pt)) * 0.5;
   if (algo == 2) combPtInv = 1. / id_pt;
   if (algo == 3) combPtInv = 1. / pt;
   if (algo == 4) combPtInv = 1.e-33;

   double tmp_deta = std::max(eta, id_eta) - std::min(eta, id_eta);
   double tmp_dphi = std::max(phi, id_phi) - std::min(phi, id_phi);
   if (tmp_dphi >= M_PI) tmp_dphi = 2 * M_PI - tmp_dphi;
   double tmp_dr = sqrt(tmp_deta * tmp_deta + tmp_dphi * tmp_dphi);

   dphi      = tmp_dphi;
   deta      = tmp_deta;
   dr        = tmp_dr;

   bool passDR = true;
   bool passPt = true;

   if (tmp_dr > winDR)  passDR = false;

   ATH_MSG_DEBUG(m_test_string
		 << " REGTEST Angular MU-ID match / dR / threshold / result:"
		 << " / " << tmp_dr
		 << " / " << winDR
		 << " / " << (passDR ? "true" : "false"));

   if (algo == 1 && winPt > 0) {
      double tmp_dpt = fabs(fabs(pt) - fabs(id_pt)) / CLHEP::GeV; //don't use charge info
      if (tmp_dpt > winPt) passPt = false;
      ATH_MSG_DEBUG(m_test_string
		    << " REGTEST MU-ID match / dpt (GeV) / threshold (GeV) / result:"
		    << " / " << tmp_dpt
		    << " / " << winPt
		    << " / " << (passPt ? "true" : "false"));
   }

   if (passDR && passPt) return 0;
   else                  return 1;
}

// muon-trk match based on Geant4 backextrapolated SA Muon matched with ID track
// return 0 --> match,  1 --> no match
int muComb::g4Match(const xAOD::L2StandAloneMuon* feature,
                    double id_eta, double id_phi, double id_pt, double id_charge, double id_eeta, double id_ephi, double id_eipt,
                    double& combPtInv, double& combPtRes, double& deta, double& dphi, double& chi2, int& ndof)
{

   chi2 = 1.0e30;
   ndof = 0;

   //muFast parameters (in MeV!)
   double phi    = feature->phiMS();
   //double eta    = feature->etaMS();
   double theta  = 2.*atan(exp(-feature->etaMS()));
   double p      = 0.0;
   if (sin(theta) != 0) {
      p = (feature->pt() * CLHEP::GeV) / sin(theta);
   } else {
      return 1; //No match if muon angle is zero
   }
   double charge = 1.0;
   double q_over_p = 0.;
   if (p != 0.)  {
      q_over_p = 1. / p;
      charge = p / fabs(p);
   } else {
      return 1; //No match if muon Pt is zero
   }
   double pt = feature->pt() * CLHEP::GeV;
   //double ptinv  = 1/pt;
   double eptinv = feature->deltaPt() * CLHEP::GeV / pt / pt;

   bool   isBarrel = ((feature->sAddress() != -1) ? true : false);
   double etaShift = (isBarrel ? 0 : charge * 0.01);
   bool   doFix = kFALSE;

   //Superpoints
   int inner  = (feature->sAddress() == -1) ? xAOD::L2MuonParameters::Chamber::EndcapInner : xAOD::L2MuonParameters::Chamber::BarrelInner;
   int middle = (feature->sAddress() == -1) ? xAOD::L2MuonParameters::Chamber::EndcapMiddle : xAOD::L2MuonParameters::Chamber::BarrelMiddle;
   //int outer  = (feature->sAddress() == -1) ? xAOD::L2MuonParameters::Chamber::EndcapOuter : xAOD::L2MuonParameters::Chamber::BarrelOuter;

   double sp1_z = feature->superPointZ(inner);
   double sp1_R = feature->superPointR(inner);
   double sp2_z = feature->superPointZ(middle);
   double sp2_R = feature->superPointR(middle);
   //double sp3_z = feature->superPointZ(outer);
   //double sp3_R = feature->superPointR(outer);

   if ((fabs(sp1_R) < 1000.)) {
      sp1_z *= 10.;
      sp1_R *= 10.;
   }
   if ((fabs(sp2_R) < 1300.)) {
      sp2_z *= 10.;
   }

   double R = sp1_R;
   double z = sp1_z;
   //msg() << MSG::DEBUG << m_test_string
   //      << " Before Extrapolator: sp1_z / spi_R / spi2_z / sp2_R  / z / R "
   //      << " / " << std::setw(11) << sp1_z
   //      << " / " << std::setw(11) << sp1_R
   //      << " / " << std::setw(11) << sp2_z
   //      << " / " << std::setw(11) << sp2_R
   //      << " / " << std::setw(11) << z
   //      << " / " << std::setw(11) << R << endmsg;

   if (R == 0. && z == 0.) { //treat patological endcap cases
      doFix = kTRUE;
      if (fabs(sp2_R) > 1300.) {
         z = sp2_z;
         if (z == 0.) z = 7600.;
         R = z * tan(theta);
         theta  = 2.*atan(exp(-(feature->etaMS() - etaShift)));
      }
   }

   double x = R * cos(phi);
   double y = R * sin(phi);

   Amg::Vector3D vertex(x, y, z);
   //Trk::GlobalPosition vertex(x, y, z);
   Trk::PerigeeSurface beamSurface;
   Trk::PerigeeSurface pgsf(vertex);
   Trk::Perigee perigeeMS(0., 0., phi, theta, q_over_p, pgsf);

   //ID parameters
   double id_eptinv_OLD = muCombUtil::getIDSCANRes(m_IDSCANRes_barrel,  m_IDSCANRes_endcap1, m_IDSCANRes_endcap2,
                                               m_IDSCANRes_endcap3, m_IDSCANRes_endcap4, id_pt, id_eta);
   double id_ptinv  = 1.0e33;
   if (id_pt != 0) {
      id_ptinv  = 1. / id_pt;
   } else {
      return 3; //no match if ID track Pt zero
   }

   double id_eptinv = id_eipt; //now taken from Track itself ...

   const Trk::Perigee* muonPerigee = (Trk::Perigee*) m_backExtrapolatorG4->extrapolate(perigeeMS, beamSurface, Trk::oppositeMomentum, true, Trk::muon);

   //Protection against failing extrapolation
   double extr_eta;
   double extr_phi;
   double extr_pt;
   if (!muonPerigee) { //G4 probably failed, getting LUT extrapolated values
      extr_eta    = feature->eta();
      extr_phi    = feature->phi();
      extr_pt = pt;
   } else {
      double extr_theta    = muonPerigee -> parameters()[Trk::theta];
      extr_phi      = muonPerigee -> parameters()[Trk::phi0];
      double extr_q_over_p = muonPerigee -> parameters()[Trk::qOverP];
      extr_eta      = -log(tan(extr_theta / 2.));
      if (doFix) extr_eta = -log(tan(theta / 2.));
      int    extr_q        = (extr_q_over_p > 0 ? 1 : -1);
      extr_pt       = extr_q * 1.0e33;
      if (extr_q_over_p != 0.) {
         extr_pt = (1. / extr_q_over_p) * sin(extr_theta);
         if (doFix) extr_pt = (1. / extr_q_over_p) * sin(theta);
      }
   }

   //int isuc = 1;
   //if (!muonPerigee) isuc = 0;
   //msg() << MSG::DEBUG << m_test_string
   //      << " Extrapolator called: Success / doFix / pt / eta / phi"
   //      << " / " << std::setw(11) << isuc
   //      << " / " << std::setw(11) << doFix
   //      << " / " << std::setw(11) << extr_pt
   //      << " / " << std::setw(11) << extr_eta
   //      << " / " << std::setw(11) << extr_phi << endmsg;

   double extr_eeta = muCombUtil::getG4ExtEtaRes(feature->pt(), feature->etaMS());
   double extr_ephi = muCombUtil::getG4ExtPhiRes(feature->pt(), feature->etaMS());
   double extr_ptinv  = 1.0e33;
   if (extr_pt != 0) extr_ptinv = 1. / extr_pt;
   double extr_eptinv = eptinv;

   if (isBarrel) {//retuned ptinv resolution (only for barrel)
      extr_eptinv *= m_Chi2Weight_g4;
      id_eptinv   *= m_Chi2Weight_g4;
   }

   //avoid memory leak
   delete muonPerigee;

   //Combined muon parameters
   combPtInv = muCombUtil::getCombinedAverage(extr_ptinv, extr_eptinv, id_ptinv, id_eptinv);
   combPtRes = muCombUtil::getCombinedAverageSigma(extr_eptinv, id_eptinv);
   double q_tmp = charge;
   if (m_ChargeStrategy == 1) q_tmp = id_charge;
   else if (m_ChargeStrategy == 2) {
      if (1. / combPtInv > 50000.) q_tmp = charge;
      else                         q_tmp = id_charge;
   }
   combPtInv *= q_tmp;

   //Masaki/Kunihiro treatment of TGC/RPC readout problems
   if (msgLvl() <= MSG::DEBUG)
     ATH_MSG_DEBUG(" Enlarge phi matching error in case TGC/RPC readout failed. : " << feature->isRpcFailure() << " / " << feature->isTgcFailure());

   if (feature->isTgcFailure() || feature->isRpcFailure()) extr_ephi *= 2.0;

   //Match
   deta = muCombUtil::getDeltaEta(extr_eta, id_eta);
   dphi = muCombUtil::getDeltaPhi(extr_phi, id_phi);
   int ndof_OLD = 0;
   double chi2_OLD = muCombUtil::getChi2(ndof_OLD, combPtInv, 
                                         extr_eta, extr_eeta, extr_phi, extr_ephi, extr_ptinv, extr_eptinv,
                                         id_eta, 0.0, id_phi, 0.0, id_ptinv, id_eptinv, true);
   chi2 = muCombUtil::getStdChi2(ndof, extr_eta, extr_eeta, extr_phi, extr_ephi, extr_ptinv, extr_eptinv,
                              id_eta, id_eeta, id_phi, id_ephi, id_ptinv, id_eptinv, m_UseAbsPt);


   ATH_MSG_DEBUG(m_test_string
		 << " REGTEST Resolution / OLDIdRes / IdRes / muFastRes / combRes:"
		 << " / " << std::setw(11) << id_eptinv_OLD / CLHEP::GeV
		 << " / " << std::setw(11) << id_eptinv / CLHEP::GeV
		 << " / " << std::setw(11) << extr_eptinv / CLHEP::GeV
		 << " / " << std::setw(11) << combPtRes / CLHEP::GeV);

   ATH_MSG_DEBUG(m_test_string
		 << " REGTEST Momentum / IdPt / muFastPt  / CombPt :"
		 << " / " << std::setw(11) << 1. / id_ptinv / CLHEP::GeV
		 << " / " << std::setw(11) << 1. / extr_ptinv / CLHEP::GeV
		 << " / " << std::setw(11) << 1. / combPtInv / CLHEP::GeV);

   ATH_MSG_DEBUG(m_test_string
		 << " REGTEST Chi2 / ndof // Chi2OLD / ndofOLD :"
		 << " / " << std::setw(11) << chi2
		 << " / " << std::setw(11) << ndof
		 << " // " << std::setw(11) << chi2_OLD
		 << " / " << std::setw(11) << ndof_OLD);

   //Cuts
   double winEtaSigma = m_WinEta_g4;
   double winPhiSigma = m_WinPhi_g4;
   double maxChi2     = m_Chi2Max_g4;
   if (!isBarrel) {//EC
      winEtaSigma = m_WinEta_EC_g4;
      winPhiSigma = m_WinPhi_EC_g4;
      maxChi2     = m_Chi2Max_EC_g4;
   }

   ATH_MSG_DEBUG(m_test_string
		 << " REGTEST DeltaEta / DeltaPhi / WinEta / WinPhi:"
		 << " / " << std::setw(11) << fabs(deta)
		 << " / " << std::setw(11) << fabs(dphi)
		 << " / " << std::setw(11) << m_WeightEta_g4*winEtaSigma*sqrt(extr_eeta * extr_eeta)
		 << " / " << std::setw(11) << m_WeightPhi_g4*winPhiSigma*sqrt(extr_ephi * extr_ephi));

   if (fabs(deta) > m_WeightEta_g4 * winEtaSigma * sqrt(extr_eeta * extr_eeta)) {
      return 4;
   }
   if (fabs(dphi) > m_WeightPhi_g4 * winPhiSigma * sqrt(extr_ephi * extr_ephi)) {
      return 5;
   }
   if (ndof >= m_NdofMin) {
      if (chi2 > maxChi2) return 6;
   }

   return 0; //match OK
}

// muon-trk match based on LUT backextrapolated SA Muon matched with ID track
// return 0 --> match,  1 --> no match
int muComb::mfMatch(const xAOD::L2StandAloneMuon* feature,
                    double id_eta, double id_phi, double id_pt, double id_charge,
                    double& combPtInv, double& combPtRes, double& deta, double& dphi, double& chi2, int& ndof)
{

   chi2 = 1.0e30;
   ndof = 0;
   //muFast parameters

   double    pt = feature->pt() * CLHEP::GeV;
   if (pt == 0.)  {
      return 1; //No match if muFast Pt is zero
   }

   bool   isTS     = ((feature->rMS() <= 10.) ? true : false);
   bool   isBarrel = ((feature->sAddress() != -1) ? true : false);

   double charge = pt / fabs(pt);
   double ptinv  = 1. / pt;
   double eptinv = feature->deltaPt() * CLHEP::GeV / pt / pt;

   //ID parameters
   double id_eptinv    = muCombUtil::getIDSCANRes(m_IDSCANRes_barrel, m_IDSCANRes_endcap1, m_IDSCANRes_endcap2,
                                                  m_IDSCANRes_endcap3, m_IDSCANRes_endcap4, id_pt, id_eta);
   double id_ptinv     = 1.0e33;
   //double id_ept       = 1.0e33;
   if (id_pt != 0) {
      id_ptinv  = 1. / id_pt;
      //id_ept    = id_eptinv * id_pt * id_pt;
   } else {
      return 3; //no match if ID track Pt zero
   }

   //Combined muon parameters
   combPtInv = muCombUtil::getCombinedAverage(ptinv, eptinv, id_ptinv, id_eptinv);
   combPtRes = muCombUtil::getCombinedAverageSigma(eptinv, id_eptinv);
   double q_tmp = charge;
   if (m_ChargeStrategy == 1) q_tmp = id_charge;
   else if (m_ChargeStrategy == 2) {
      if (1. / combPtInv > 50000.) q_tmp = charge;
      else                         q_tmp = id_charge;
   }
   combPtInv *= q_tmp;

   // Extrapolated (LUT) quantities (now stored in the xAOD::L2StandAloneMuon container)
   double extr_eta    = feature->eta();
   double extr_eeta   = feature->deltaEta();
   double extr_phi    = feature->phi();
   double extr_ephi   = feature->deltaPhi();
   double extr_ptinv  = ptinv;
   double extr_eptinv = eptinv;

   //Masaki/Kunihiro treatment of TGC/RPC readout problems
   ATH_MSG_DEBUG(" Enlarge phi matching error in case TGC/RPC readout failed. : " << feature->isRpcFailure() << " / " << feature->isTgcFailure());

   if (feature->isTgcFailure() || feature->isRpcFailure()) extr_ephi *= 2.0;

   //Match
   deta = muCombUtil::getDeltaEta(extr_eta, id_eta);
   dphi = muCombUtil::getDeltaPhi(extr_phi, id_phi);
   chi2 = muCombUtil::getChi2(ndof, combPtInv, extr_eta, extr_eeta, extr_phi, extr_ephi, extr_ptinv, extr_eptinv,
                              id_eta, 0.0, id_phi, 0.0, id_ptinv, id_eptinv, m_UseAbsPt);


   ATH_MSG_DEBUG(m_test_string
		 << " REGTEST Resolution / IdRes / muFastRes / combRes:"
		 << " / " << std::setw(11) << id_eptinv / CLHEP::GeV
		 << " / " << std::setw(11) << extr_eptinv / CLHEP::GeV
		 << " / " << std::setw(11) << combPtRes / CLHEP::GeV);

   ATH_MSG_DEBUG(m_test_string
		 << " REGTEST Momentum / IdPt / muFastPt  / CombPt :"
		 << " / " << std::setw(11) << 1. / id_ptinv / CLHEP::GeV
		 << " / " << std::setw(11) << 1. / ptinv / CLHEP::GeV
		 << " / " << std::setw(11) << 1. / combPtInv / CLHEP::GeV);

   ATH_MSG_DEBUG(m_test_string
		 << " REGTEST Chi2 / ndof :"
		 << " / " << std::setw(11) << chi2
		 << " / " << std::setw(11) << ndof);
		 
   //Cuts
   double winEtaSigma = m_WinEta;
   double winPhiSigma = m_WinPhi;
   double maxChi2     = m_Chi2Max;
   if (isTS) { //Trigger Station (barrel/EC)
      winEtaSigma = m_WinEta_TS;
      winPhiSigma = m_WinPhi_TS;
      maxChi2     = m_Chi2Max_TS;
   } else { //if endcaps and not TS
      if (!isBarrel) {
         winEtaSigma = m_WinEta_EC;
         winPhiSigma = m_WinPhi_EC;
         maxChi2     = m_Chi2Max_EC;
      }
   }
   bool isFT = false;
   if (isBarrel)
      if ((id_phi >= -2.60 && id_phi <= -2.10) || (id_phi >= -1.10 && id_phi <= -0.50)) isFT = true;
   if (isFT) { //if MS-feet region
      winEtaSigma = m_WinEta_FE;
      winPhiSigma = m_WinPhi_FE;
      maxChi2     = m_Chi2Max_FE;
   }

   ATH_MSG_DEBUG(m_test_string
		 << " REGTEST DeltaEta / DeltaPhi / WinEta / WinPhi:"
		 << " / " << std::setw(11) << fabs(deta)
		 << " / " << std::setw(11) << fabs(dphi)
		 << " / " << std::setw(11) << m_WeightEta*winEtaSigma*sqrt(extr_eeta * extr_eeta)
		 << " / " << std::setw(11) << m_WeightPhi*winPhiSigma*sqrt(extr_ephi * extr_ephi));

   if (fabs(deta) > m_WeightEta * winEtaSigma * sqrt(extr_eeta * extr_eeta)) {
      return 4;
   }
   if (fabs(dphi) > m_WeightPhi * winPhiSigma * sqrt(extr_ephi * extr_ephi)) {
      return 5;
   }
   if (ndof >= m_NdofMin) {
      if (chi2 > maxChi2) return 6;
   }

   return 0; //match OK
}


/** @fn bool muComb::execHLTAlgorithm(TriggerElement* outputTE)
 * @brief Execute(TE) method.
 * @return bool
 */
//HLT::ErrorCode muComb::hltExecute(const HLT::TriggerElement* /*inputTE*/,
HLT::ErrorCode muComb::hltExecute(const HLT::TriggerElement* inputTE,
                                  HLT::TriggerElement* outputTE)
{

   // init monitoring variables
   m_ptMS        = -9999.;
   m_etaMS       = -9999.;
   m_phiMS       = -9999.;
   m_zetaMS      = -9999.;
   m_ptID        = -9999.;
   m_etaID       = -9999.;
   m_phiID       = -9999.;
   m_zetaID      = -9999.;
   m_ptMC        = -9999.;
   m_dEta        = -9999.;
   m_dPhi        = -9999.;
   m_dZeta       = -9999.;
   m_dR          = -9999.;
   m_ptFL        = -9999.;
   m_etaFL       = -9999.;
   m_phiFL       = -9999.;
   m_efficiency  = -1.;
   m_ErrorFlagMC = 0;
   m_MatchFlagMC = 0;
   m_StrategyMC  = m_AlgoStrategy;

   //Magnetic field status
   bool toroidOn   = !m_assumeToroidOff;
   bool solenoidOn = !m_assumeSolenoidOff;
   if (m_useAthenaFieldService) {
      if (m_MagFieldSvc) {
         toroidOn  = m_MagFieldSvc->toroidOn() && !m_assumeToroidOff;
         solenoidOn = m_MagFieldSvc->solenoidOn() && !m_assumeSolenoidOff;
      }
   }
   ATH_MSG_DEBUG("=========== Magnetic Field Status ========== ");
   ATH_MSG_DEBUG(" B Fields read from AthenaFieldService:   " << (m_useAthenaFieldService ? "TRUE" : "FALSE"));
   ATH_MSG_DEBUG(" Assuming Toroid OFF is:                  " << (m_assumeToroidOff ? "TRUE" : "FALSE"));
   ATH_MSG_DEBUG(" Assuming Solenoid OFF is:                " << (m_assumeSolenoidOff ? "TRUE" : "FALSE"));
   ATH_MSG_DEBUG(" ---> Solenoid : " << ((solenoidOn) ? "ON" : "OFF"));
   ATH_MSG_DEBUG(" ---> Toroid   : " << ((toroidOn) ?   "ON" : "OFF"));

   // Algorithm strategy
   // select best matchinig strategy
   int usealgo = 0;                           //extrapolated muon - ID match (default)
   if (solenoidOn) {
      if (toroidOn) {
         if (m_AlgoStrategy == 0)  usealgo = 0;           //extrapolated muon - ID match
         else                      usealgo = 1;           //simple R-match w/o extrapolation (pt from combined MS-ID)
      } else {
         usealgo = 2;   //simple R-match w/o extrapolation (pt from ID) (match ID-L1 muon if ptMS=0 or MS muon not available)
      }
   } else {
      if (toroidOn) usealgo = 3;           //simple R-match w/o extrapolation (pt from MS)
      else          usealgo = 4;           //simple R-match w/o extrapolation (pt inf)
   }

   ATH_MSG_DEBUG("MuCombStrategy: " << usealgo);

   ATH_MSG_DEBUG("=========== Matching windows g4 ========== ");
   ATH_MSG_DEBUG(" WinEtaSigma g4: " << m_WinEta_g4);
   ATH_MSG_DEBUG(" WinPhiSigma g4: " << m_WinPhi_g4);
   ATH_MSG_DEBUG(" WinEtaSigma g4 EC: " << m_WinEta_EC_g4);
   ATH_MSG_DEBUG(" WinPhiSigma g4 EC: " << m_WinPhi_EC_g4);
   ATH_MSG_DEBUG(" WeightEta g4: " << m_WeightEta_g4);
   ATH_MSG_DEBUG(" WeightPhi g4: " << m_WeightPhi_g4);
   ATH_MSG_DEBUG(" Chi2Weight g4: " << m_Chi2Weight_g4);
   ATH_MSG_DEBUG(" ");

   // Decode TE
   ATH_MSG_DEBUG("outputTE->getId(): " << outputTE->getId());
   ATH_MSG_DEBUG("inputTE->getId(): " << inputTE->getId());

   // Get input for seeding
   const xAOD::L2StandAloneMuonContainer* const_muonColl(0);
   Bool_t useL1 = false;

   // retrieve L2StandAloneMuonContainer
   HLT::ErrorCode status = getFeature(outputTE, const_muonColl);
   if (status != HLT::OK) {
      if (usealgo == 2) {
         useL1 = true;
      } else {
	ATH_MSG_ERROR(" L2StandAloneMuonContainer not found --> ABORT");
         return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
      }
   }
   xAOD::L2StandAloneMuonContainer* muonColl = const_cast<xAOD::L2StandAloneMuonContainer*>(const_muonColl);

   // retrieve L2StandAloneMuon (assumed to be the first element)
   const xAOD::L2StandAloneMuon* muonSA = muonColl->front();

   // Create Combined muon
   xAOD::L2CombinedMuonContainer *muonCBColl = new xAOD::L2CombinedMuonContainer();
   xAOD::L2CombinedMuonAuxContainer caux;
   muonCBColl->setStore(&caux);

   xAOD::L2CombinedMuon* muonCB = new xAOD::L2CombinedMuon();
   muonCB->makePrivateStore();
   muonCB->setPt(0.0);
   muonCB->setStrategy(usealgo);

   // check muonSA pt != 0
   if (muonSA->pt() == 0.) {
      m_ErrorFlagMC = 1;
      if (usealgo == 2 || usealgo == 4) {
	ATH_MSG_DEBUG(" L2StandAloneMuon pt = 0 --> using angular match");
         if (usealgo == 2) useL1 = true;
      } else {
	ATH_MSG_DEBUG(" L2StandAloneMuon pt = 0 --> stop processing RoI");
         muonCB->setErrorFlag(m_ErrorFlagMC);
         muonCBColl->push_back(muonCB);
         return muCombSeed(outputTE, muonCBColl);
      }
   }

   double ptL1    = -999.;
   double etaL1   = -999.;
   double phiL1   = -999.;
   double pt      = -999.;
   double eta     = -999.;
   double phi     = -999.;
   double eta_ms  = -999.;
   double phi_ms  = -999.;
   double zeta_ms = -999.;
   if (useL1) {
      const LVL1::RecMuonRoI* muonRoI;
      if (HLT::OK != getFeature(inputTE, muonRoI, "")) {
         ATH_MSG_DEBUG("Could not find the LVL1 roi");
         ATH_MSG_DEBUG("L2StandAloneMuon pt == 0. && no L1 && torid=OFF --> no match");
         m_ErrorFlagMC = 1;
         muonCB->setErrorFlag(m_ErrorFlagMC);
         return muCombSeed(outputTE, muonCBColl);
      } else {
         ptL1  = muonRoI->getThresholdValue();
         etaL1 = muonRoI->eta();
         phiL1 = muonRoI->phi();
      }
      ATH_MSG_DEBUG("Input L1 muon pt (GeV) = " << ptL1
		    << " / eta = "                 << etaL1
		    << " / phi = "                 << phiL1);
   } else {
      // Save SA muon EL into CB muon
      ElementLink<xAOD::L2StandAloneMuonContainer> muonSAEL(*muonColl, 0);
      muonCB->setMuSATrackLink(muonSAEL);

      pt       = muonSA->pt();
      eta      = muonSA->eta();
      phi      = muonSA->phi();
      eta_ms   = muonSA->etaMS();
      phi_ms   = muonSA->phiMS();
      zeta_ms  = muonSA->zMS();

      ATH_MSG_DEBUG("Input L2StandaloneMuon pt (GeV) = " << pt
		    << " / eta = "                    << eta
		    << " / phi = "                    << phi
		    << " / etaMS = "                  << eta_ms
		    << " / phiMS = "                  << phi_ms
		    << " / zMS = "                    << zeta_ms);
   }

   // ID tracks Decoding

   std::string algoId = m_ID_algo_to_use;
   const xAOD::TrackParticleContainer* idTrackParticles = 0;
   status = getFeature(outputTE, idTrackParticles, algoId);

   if (status != HLT::OK) {
     ATH_MSG_DEBUG(" Failed to get " << algoId << " xAOD::TrackParticleContainer --> ABORT");
     m_ErrorFlagMC = 2;
     return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
   }
   if (!idTrackParticles) {
     ATH_MSG_DEBUG("Pointer to xAOD::TrackParticleContainer[" << algoId << "] = 0 --> no match");
      m_ErrorFlagMC = 2;
      muonCB->setErrorFlag(m_ErrorFlagMC);
      muonCBColl->push_back(muonCB);
      return muCombSeed(outputTE, muonCBColl);
   }

   ATH_MSG_DEBUG(" Got xAOD::TrackParticleContainer with size: " << idTrackParticles->size());

   // matching
   double ptinv_comb = 0.;
   double ptres_comb = 0.001;
   double chi2_comb  = 1.0e33;
   int    ndof_comb  = -1;
   double best_dr    = 1.0e33;
   double best_deta  = 1.0e33;
   double best_dphi  = 1.0e33;
   bool   has_match  = false;
   int    imatch     = -1;
   int    matched_trk_idx = -1;
   int    matched_trk_idx_tmp = -1;


   for (xAOD::TrackParticleContainer::const_iterator trkit = idTrackParticles->begin(); trkit != idTrackParticles->end(); ++trkit) {

      matched_trk_idx_tmp++;
      double ptinv_tmp = 0.;
      double ptres_tmp = 0.001;
      double deta_tmp  = 0.;
      double dphi_tmp  = 0.;
      double dr_tmp    = 0.;
      double chi2_tmp  = 0.;
      int    ndof_tmp  = 0;
      bool   has_match_tmp = false;
      int    imatch_tmp    = -1;

      // Check if event timeout was reached
      if (Athena::Timeout::instance().reached()) {
	ATH_MSG_DEBUG("Timeout reached. Trk loop, Aborting sequence.");
	return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
      }

      //Select tracks
      double phi_id    = (*trkit)->phi();
      double eta_id    = (*trkit)->eta();
      double qoverp_id = (*trkit)->qOverP();
      double q_id      = ((*trkit)->qOverP() > 0 ? 1.0 : -1.0);
      double pt_id     = (*trkit)->pt() * q_id;

      double e_qoverp_id = std::sqrt((*trkit)->definingParametersCovMatrix()(Trk::qOverP,Trk::qOverP));
      double e_phi_id = std::sqrt((*trkit)->definingParametersCovMatrix()(Trk::phi0,Trk::phi0));
      double e_theta_id = std::sqrt((*trkit)->definingParametersCovMatrix()(Trk::theta,Trk::theta));
      double tanthetaov2 = fabs(std::exp(-eta));
      double e_eta_id = fabs(0.5 * (1./tanthetaov2 + tanthetaov2) * e_theta_id);

      double e_qoverpt_id = e_qoverp_id;
      double theta_id = (*trkit)->theta();
      if (sin(theta_id) != 0) e_qoverpt_id /= fabs(sin(theta_id)); //approximate

      ATH_MSG_DEBUG("Found track: "
		    << "  with pt (GeV) = " << pt_id / CLHEP::GeV
		    << ", q    = " << q_id
		    << ", eta  = " << eta_id
		    << ", phi  = " << phi_id
		    << ", th   = " << theta_id
		    << ", ephi = " << e_phi_id
		    << ", eth  = " << e_theta_id
		    << ", eeta = " << e_eta_id
		    << ", ip   = " << qoverp_id
		    << ", eip  = " << e_qoverp_id
		    << ", eipt = " << e_qoverpt_id);

      if (usealgo != 3) {
         if ((fabs(pt_id) / CLHEP::GeV) < m_PtMinTrk)       continue;
      }
      if (fabs(eta_id)  > m_EtaMaxTrk)      continue;

      ATH_MSG_DEBUG("Track selected ");

      if (usealgo > 0) {//DeltaR match
         if (useL1) {
            imatch_tmp = drptMatch(ptL1, etaL1, phiL1, pt_id, eta_id, phi_id, usealgo, ptinv_tmp, ptres_tmp, deta_tmp, dphi_tmp, chi2_tmp);
         } else {
            imatch_tmp = drptMatch(muonSA, pt_id, eta_id, phi_id, usealgo, ptinv_tmp, ptres_tmp, deta_tmp, dphi_tmp, chi2_tmp);
         }
         if (imatch_tmp == 0) has_match_tmp = true;
      } else { //Std match
         if (!m_useBackExtrapolatorG4) {
            imatch_tmp = mfMatch(muonSA, eta_id, phi_id, pt_id, q_id, ptinv_tmp, ptres_tmp, deta_tmp, dphi_tmp, chi2_tmp, ndof_tmp);
            if (imatch_tmp == 0) has_match_tmp = true;
            if (Athena::Timeout::instance().reached()) {
	      ATH_MSG_DEBUG("Timeout reached. mfMatch backextrapolation, Aborting sequence.");
               return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
            }
         } else { //G4 match
            imatch_tmp = g4Match(muonSA, eta_id, phi_id, pt_id, q_id, e_eta_id, e_phi_id, e_qoverpt_id, ptinv_tmp, ptres_tmp, deta_tmp, dphi_tmp, chi2_tmp, ndof_tmp);
            if (imatch_tmp == 0) has_match_tmp = true;
            if (Athena::Timeout::instance().reached()) {
	      ATH_MSG_DEBUG("Timeout reached. g4Match backextrapolation, Aborting sequence.");
	      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
            }
         }
      }

      imatch = imatch_tmp;
      if (!has_match_tmp) continue;

      // select nearest track
      dr_tmp = sqrt(deta_tmp * deta_tmp + dphi_tmp * dphi_tmp);

      if (chi2_tmp <= chi2_comb) {//Select nearest track
         has_match  = true;
         chi2_comb  = chi2_tmp;
         ndof_comb  = ndof_tmp;
         ptinv_comb = ptinv_tmp;
         ptres_comb = ptres_tmp;
         best_deta  = deta_tmp;
         best_dphi  = dphi_tmp;
         best_dr    = dr_tmp;
         imatch     = imatch_tmp;
         matched_trk_idx  = matched_trk_idx_tmp;
      }

   }//Tracks loop

   //Set monitored quantities (monitor only pt>6 GeV/c && standard matching)
   if (usealgo == 0 && fabs(pt) >= 6.) {
      m_ptMS       = pt;
      m_etaMS      = eta;
      m_phiMS      = phi;
      m_zetaMS     = zeta_ms;
      m_ptFL       = pt;
      m_etaFL      = eta;
      m_phiFL      = phi;
      if (m_ptMS >  100.) m_ptMS =  101.5;
      if (m_ptMS < -100.) m_ptMS = -101.5;
      if (m_ptFL >  100.) m_ptFL =  101.5;
      if (m_ptFL < -100.) m_ptFL = -101.5;
   }

   if (!has_match) {
      m_ErrorFlagMC = 3;
      m_MatchFlagMC = imatch;
      if (usealgo == 0 && fabs(pt) >= 6.)  m_efficiency = 0; //monitor only efficiency for mu6 && standard matching
      ATH_MSG_DEBUG(" No matched ID tracks --> no match");
      muonCB->setErrorFlag(m_ErrorFlagMC);
      muonCB->setMatchFlag(m_MatchFlagMC);
      muonCBColl->push_back(muonCB);
      return muCombSeed(outputTE, muonCBColl);
   }

   //Save EL to ID trk into CB muon
   ElementLink<xAOD::TrackParticleContainer> idtrkEL(*idTrackParticles, matched_trk_idx);
   muonCB->setIdTrackLink(idtrkEL);

   double pt_id        = muonCB->idTrack()->pt();
   double q_id         = ((muonCB->idTrack()->qOverP()) > 0 ? 1.0 : -1.0);
   double phi_id       = muonCB->idTrack()->phi();
   double eta_id       = muonCB->idTrack()->eta();
   //const Trk::Perigee& idtrk_perigee = muonCB->idTrack()->perigeeParameters();
   double zPos_id      = muonCB->idTrack()->z0(); //idtrk_perigee.parameters()[Trk::z0];

   ATH_MSG_DEBUG(" SA muon macthed to ID track ...");

   //Update monitored vars
   m_MatchFlagMC = imatch;
   m_ptFL        = -9999.;
   m_etaFL       = -9999.;
   m_phiFL       = -9999.;
   if (usealgo == 0 && fabs(pt) >= 6.) {
      m_efficiency = 1;
      m_ptID       = pt_id / CLHEP::GeV; //in GeV/c
      m_etaID      = eta_id;
      m_phiID      = phi_id;
      m_zetaID     = zPos_id;
      m_ptMC       = 1. / (ptinv_comb * CLHEP::GeV); //in GeV/c
      m_dZeta      = zeta_ms - zPos_id;
      m_dPhi       = best_dphi;
      m_dEta       = best_deta;
      m_dR         = best_dr;

      if (m_ptMC >  100.) m_ptMC =  101.5;
      if (m_ptMC < -100.) m_ptMC = -101.5;
      if (m_ptID >  100.) m_ptID =  101.5;
      if (m_ptID < -100.) m_ptID = -101.5;
   }

   double prt_pt = pt;
   if (useL1) prt_pt = ptL1;
   ATH_MSG_DEBUG(m_test_string << " REGTEST Combination chosen: "
		 << " usealgo / IdPt (GeV) / muonPt (GeV) / CombPt (GeV) / chi2 / ndof: "
		 << " / " << usealgo << " / " << pt_id*q_id / CLHEP::GeV << " / " << prt_pt << " / " << 1. / ptinv_comb / CLHEP::GeV << " / " << chi2_comb << " / " << ndof_comb);

   muonCB->setPt(fabs(1. / ptinv_comb));
   muonCB->setEta(eta_id);
   muonCB->setPhi(phi_id);

   float mcq = -1.0;
   if (ptinv_comb > 0) mcq = 1.0;

   muonCB->setCharge(mcq);

   float mcresu = fabs(ptres_comb / (ptinv_comb * ptinv_comb));
   ATH_MSG_DEBUG(" SigmaPt (GeV) is: " << mcresu / CLHEP::GeV);
   muonCB->setSigmaPt(mcresu);

   muonCB->setErrorFlag(m_ErrorFlagMC);
   muonCB->setMatchFlag(m_MatchFlagMC);

   muonCBColl->push_back(muonCB);
   return muCombSeed(outputTE, muonCBColl);
}

HLT::ErrorCode
muComb::muCombSeed(HLT::TriggerElement* outputTE, xAOD::L2CombinedMuonContainer* muon_cont)
{
   //seeding the next Algorithm
   std::string muonCBCollKey = "MuonL2CBInfo";
   HLT::ErrorCode status = attachFeature(outputTE, muon_cont, muonCBCollKey);
   if (status != HLT::OK) {
      outputTE->setActiveState(false);
      delete muon_cont;
      ATH_MSG_ERROR(" Record of xAOD::L2CombinedMuonContainer in TriggerElement failed");
      return status;
   } else {
     ATH_MSG_DEBUG(" xAOD::L2CombinedMuonContainer attached to the TriggerElement");
   }
   // Temporary
   ElementLink<MuonFeatureContainer> pMuon(0, 0);
   ElementLink<TrigInDetTrackCollection> pTrack(0, 0);
   CombinedMuonFeature* muon_feature = new CombinedMuonFeature(0., -1.0, 1.0, 0, 0, 0, pMuon, pTrack);
   status = attachFeature(outputTE, muon_feature);
   if (status != HLT::OK) {
     ATH_MSG_ERROR(" Record of fake/empty CombinedMuonFeature in TriggerElement failed");
   } else {
     ATH_MSG_DEBUG(" fake/empty CombinedMuonFeature attached to the TriggerElement");
   }
   outputTE->setActiveState(true);
   return HLT::OK;
}

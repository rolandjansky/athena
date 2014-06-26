/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     muComb.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigmuComb
// VERSION:  V2
//
// AUTHOR:   S.Giagu <stefano.giagu@cern.ch>
//
// ********************************************************************
#include <sstream>
#include <math.h>

#include "TrigmuComb/muComb.h"
#include "TrigmuComb/muCombUtil.h"
#include "PathResolver/PathResolver.h"
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigSiSpacePointTool/ISpacePointProvider.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "AthenaKernel/Timeout.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkParameters/TrackParameters.h"
//#include "TrkParameters/Perigee.h"

muComb::muComb(const std::string& name, ISvcLocator* pSvcLocator):
   HLT::FexAlgo(name, pSvcLocator),
   m_backExtrapolatorLUT("TrigMuonBackExtrapolator"),
   m_backExtrapolatorG4("ExtrapolateMuonToIPTool"),
   m_MagFieldSvc(0)
{

   // backextrapolators
   declareProperty("UseBackExtrapolatorG4", m_useBackExtrapolatorG4 = false);
   declareProperty("BackExtrapolatorLUT", m_backExtrapolatorLUT, "public tool for back extrapolating the muon tracks to the IV LUT based");
   declareProperty("BackExtrapolatorG4",  m_backExtrapolatorG4, "public tool for back extrapolating the muon tracks to the IV G4 based");

   // magnetic field
   declareProperty("UseAthenaFieldService",     m_useAthenaFieldService = true);
   declareProperty("AssumeToroidOff",           m_assumeToroidOff   = false);
   declareProperty("AssumeSolenoidOff",         m_assumeSolenoidOff = false);

   //Charge assignment startegy
   //0: use muFast
   //1: use ID
   //2: use resolution model
   declareProperty("ChargeStrategy",         m_ChargeStrategy = 0);
   m_ChargeStrategy.verifier().setBounds(0, 2);

   // standard configuration
   declareProperty("IDalgo", m_ID_algo_to_use = "TrigFastTrackFinder_Muon");
   //0: auto select best option
   //1: Std: muFast-backextrapolated-ID match
   //2: Toroid OFF - Solenoid ON  muFast-ID match
   //3: Toroid ON  - Solenoid OFF muFast-ID match
   //4: Toroid OFF - Solenoid OFF muFast-ID match
   //5: Toroid ON  - Solenoid ON  L1-ID match
   //6: Toroid OFF - Solenoid ON  L1-ID match
   //7: Toroid ON  - Solenoid OFF L1-ID match
   //8: Toroid OFF - Solenoid OFF L1-ID match
   declareProperty("MuCombStrategy", m_AlgoStrategy = 1);
   m_AlgoStrategy.verifier().setBounds(0, 8);

   //ID tracks quality parameters
   declareProperty("MinPtTRK",      m_PtMinTrk = 1.); //GeV/c
   declareProperty("MaxAbsEtaTRK",  m_EtaMaxTrk = 2.5);
   declareProperty("MaxAbsZetaTRK", m_ZMaxTrk = 999999999.);
   declareProperty("MaxChi2TRK",    m_Chi2MaxTrk = 999999999.);
   declareProperty("MinNPIXHitTRK", m_NPIXhitMinTrk = 0);
   declareProperty("MinNSCTHitTRK", m_NSCThitMinTrk = 0);

   // Matching Parameters
   //Common
   declareProperty("NdofMin",                m_NdofMin        = 9999);
   declareProperty("UseAbsPt",               m_UseAbsPt       = true);

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
   declareProperty("WinEtaSigma_g4",         m_WinEta_g4      = 3.0);
   declareProperty("WinPhiSigma_g4",         m_WinPhi_g4      = 3.0);
   declareProperty("Chi2Max_g4",             m_Chi2Max_g4     = 1.e33);
   declareProperty("WinEtaSigmaEndCaps_g4",  m_WinEta_EC_g4   = 3.0);
   declareProperty("WinPhiSigmaEndCaps_g4",  m_WinPhi_EC_g4   = 3.0);
   declareProperty("Chi2MaxEndCaps_g4",      m_Chi2Max_EC_g4  = 1.e33);
   declareProperty("WeightEta_g4",           m_WeightEta_g4   = 1.7);
   declareProperty("WeightPhi_g4",           m_WeightPhi_g4   = 1.7);

   // DeltaR based strategies
   declareProperty("WinPt",       m_winPt); //in GeV
   declareProperty("WinDelta",    m_winDR); //strategy dependent

   std::vector<double> def_winPt;
   def_winPt.push_back(4.0);   //strategy 1 (ad hoc)
   def_winPt.push_back(-1.0);  //strategy 2 (in GeV) (disabled if < 0)
   def_winPt.push_back(-1.0);  //strategy 3 (in GeV) (disabled if < 0)
   def_winPt.push_back(-1.0);  //strategy 4 (in GeV) (disabled if < 0)
   def_winPt.push_back(5.0);   //strategy 5 (in GeV) (disabled if < 0)
   def_winPt.push_back(-1.0);  //strategy 6 (in GeV) (disabled if < 0)
   def_winPt.push_back(-1.0);  //strategy 7 (in GeV) (disabled if < 0)
   def_winPt.push_back(-1.0);  //strategy 8 (in GeV) (disabled if < 0)
   m_winPt.set(def_winPt);

   std::vector<double> def_winDR;
   def_winDR.push_back(1.7);   //strategy 1 (ad hoc)
   def_winDR.push_back(0.2);   //strategy 2
   def_winDR.push_back(0.1);   //strategy 3
   def_winDR.push_back(0.1);   //strategy 4
   def_winDR.push_back(0.1);   //strategy 5
   def_winDR.push_back(0.2);   //strategy 6
   def_winDR.push_back(0.1);   //strategy 7
   def_winDR.push_back(0.1);   //strategy 8
   m_winDR.set(def_winDR);

   // Configure the string to be used for the regular test
   declareProperty("TestString", m_test_string = "");

   // Monitored variables
   //Input
   declareMonitoredVariable("PtMS",        m_ptMS);
   declareMonitoredVariable("EtaMS",       m_etaMS);
   declareMonitoredVariable("PhiMS",       m_phiMS);
   declareMonitoredVariable("ZetaMS",      m_zetaMS);
   //Matched
   declareMonitoredVariable("PtID",        m_ptID);
   declareMonitoredVariable("EtaID",       m_etaID);
   declareMonitoredVariable("PhiID",       m_phiID);
   declareMonitoredVariable("ZetaID",      m_zetaID);
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

   std::vector<double> default_muFastRes_barrel;
   muCombUtil::setMuFastRes(default_muFastRes_barrel, 0.042, -0.00046, 3.5, -1.8, 0.35, -0.017);

   std::vector<double> default_muFastRes_endcap1;
   muCombUtil::setMuFastRes(default_muFastRes_endcap1, 0.098, -0.000097, 77.0, -47.0, 9.8, -0.67);

   std::vector<double> default_muFastRes_endcap2;
   muCombUtil::setMuFastRes(default_muFastRes_endcap2, 0.19, -0.00043, 10.4, -5.21, 1.14, -0.056);

   std::vector<double> default_muFastRes_endcap3;
   muCombUtil::setMuFastRes(default_muFastRes_endcap3, 0.087, -0.0013, 98.0, -60.0, 12.0, -0.80);

   std::vector<double> default_muFastRes_endcap4;
   muCombUtil::setMuFastRes(default_muFastRes_endcap4, 0.060, -0.0014, 101.0, -61.0, 12.0, -0.80);

   declareProperty("MuFastBarrelRes",  m_muFastRes_barrel = default_muFastRes_barrel);
   declareProperty("MuFastEndcap1Res", m_muFastRes_endcap1 = default_muFastRes_endcap1);
   declareProperty("MuFastEndcap2Res", m_muFastRes_endcap2 = default_muFastRes_endcap2);
   declareProperty("MuFastEndcap3Res", m_muFastRes_endcap3 = default_muFastRes_endcap3);
   declareProperty("MuFastEndcap4Res", m_muFastRes_endcap4 = default_muFastRes_endcap4);

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
   msg() << MSG::INFO << "Initializing " << name() << " - package version "
         << PACKAGE_VERSION << endreq;

   m_pStoreGate = store();

   // ID Tracks Selections
   std::string algoId = m_ID_algo_to_use;

   if (algoId == "IDSCAN")           m_algoId = TrigInDetTrack::IDSCANID;
   else if (algoId == "SITRACK")    m_algoId = TrigInDetTrack::SITRACKID;
   else if (algoId == "STRATEGY_A") m_algoId = TrigInDetTrack::STRATEGY_A_ID;
   else if (algoId == "STRATEGY_B") m_algoId = TrigInDetTrack::STRATEGY_B_ID;
   else if (algoId == "STRATEGY_C") m_algoId = TrigInDetTrack::STRATEGY_C_ID;
   else if (algoId == "STRATEGY_F") m_algoId = TrigInDetTrack::STRATEGY_F_ID;
   else if (algoId == "TRTXK")      m_algoId = TrigInDetTrack::TRTXKID;
   else if (algoId == "TRTSEG")     m_algoId = TrigInDetTrack::TRTLUTID;
   else                             m_algoId = TrigInDetTrack::IDSCANID;

   // Timer Service
   m_pTimerService = 0;

   StatusCode sc;

   // BackExtrapolator services
   sc = m_backExtrapolatorLUT.retrieve();
   if (sc.isFailure()) {
      msg() << MSG::ERROR << "Could not retrieve " << m_backExtrapolatorLUT << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
   } else {
      msg() << MSG::INFO << "Retrieved tool " << m_backExtrapolatorLUT << endreq;
   }

   if (m_useBackExtrapolatorG4) {
      sc = m_backExtrapolatorG4.retrieve();
      if (sc.isFailure()) {
         msg() << MSG::ERROR << "Could not retrieve " << m_backExtrapolatorG4 << endreq;
         return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      } else {
         msg() << MSG::INFO << "Retrieved tool " << m_backExtrapolatorG4 << endreq;
      }
      msg() << MSG::INFO << "Required G4 BackExtrapolator Tool currently disabled " << endreq;
   }

   if (m_useAthenaFieldService) {
      if (!m_MagFieldSvc) service("AtlasFieldSvc", m_MagFieldSvc, /*createIf=*/ false).ignore();
      if (m_MagFieldSvc) {
         msg() << MSG::INFO << "Retrieved AtlasFieldSvc " << endreq;
      } else {
         msg() << MSG::ERROR << "Could not retrieve AtlasFieldSvc" << endreq;
         return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      }
   }

   msg() << MSG::INFO
         << "Initialization completed successfully"
         << endreq;

   return HLT::OK;
}

HLT::ErrorCode muComb::hltFinalize()
{
   return HLT::OK;
}

int muComb::drptMatch(double pt, double eta, double phi, const TrigInDetTrack* idtrack, int algo,
                      double& combPtInv, double& combPtRes, double& deta, double& dphi, double& dr)
{

   if (algo < 2 || algo > 8) {
      msg() << MSG::DEBUG << " muComb::drptMatch algo parameter out of range, it is: " << algo
            << " while must be in [2,8], match failed!!!" << endreq;
      return 0;
   }
   double winDR = m_winDR.value()[algo - 1];
   double winPt = m_winPt.value()[algo - 1];

   bool idtrpt = false;
   if (algo != 3 || algo != 4 || algo != 7 || algo != 8) idtrpt = true; //actual ID pt when solenoid ON

   //ID parameters
   double id_eta       = idtrack->param()->eta();
   double id_phi       = idtrack->param()->phi0();
   double id_pt        = 0.;
   double id_ept       = 0.;
   if (idtrpt) id_pt   = idtrack->param()->pT();
   if (idtrpt) id_ept  = idtrack->param()->epT();

   double tmp_deta = std::max(eta, id_eta) - std::min(eta, id_eta);
   double tmp_dphi = std::max(phi, id_phi) - std::min(phi, id_phi);
   if (tmp_dphi >= M_PI) tmp_dphi = 2 * M_PI - tmp_dphi;
   double tmp_dr = sqrt(tmp_deta * tmp_deta + tmp_dphi * tmp_dphi);

   dphi      = tmp_dphi;
   deta      = tmp_deta;
   dr        = tmp_dr;

   bool passDR = true;
   bool passPt = true;

   if (pt == 0.) {
      if (id_pt == 0.) {
         combPtInv = 1.0e33;
         combPtRes = 0.0;
      } else {
         combPtInv = 1. / id_pt;
         combPtRes = id_ept / id_pt / id_pt;
      }
   } else {
      if (id_pt == 0.) {
         combPtInv = 1. / (pt * 1000.);
         combPtRes = 0.0;
      } else {
         combPtInv = 1. / id_pt;
         combPtRes = id_ept / id_pt / id_pt;
      }
   }

   if (tmp_dr > winDR)  passDR = false;

   msg() << MSG::DEBUG << m_test_string
         << " REGTEST L1-ID match / dR / threshold / result:"
         << " / " << tmp_dr
         << " / " << winDR
         << " / " << (passDR ? "true" : "false")
         << endreq;


   if (pt != 0. && id_pt != 0. && winPt > 0) {
      double tmp_dpt = fabs(fabs(pt) - fabs(id_pt) / 1000.); //L1 pt has no charge info encoded
      if (tmp_dpt > winPt) passPt = false;
      msg() << MSG::DEBUG << m_test_string
            << " REGTEST L1-ID match / dpt / threshold / result:"
            << " / " << tmp_dpt
            << " / " << winPt
            << " / " << (passPt ? "true" : "false")
            << endreq;
   }

   if (passDR && passPt) return 0;
   else                  return 1;
}

int muComb::g4Match(const MuonFeature* feature, const TrigInDetTrack* idtrack,
                    double& combPtInv, double& combPtRes, double& deta, double& dphi, double& chi2, int& ndof)
{
//return mfMatch(feature, idtrack, combPtInv, combPtRes, deta, dphi, chi2, ndof);

   chi2 = 1.0e30;
   ndof = 0;

   //muFast parameters
   double phi    = feature->phi();
   double theta  = 2.*atan(exp(-feature->eta()));
   double p      = (feature->pt() * 1000.) / sin(theta);
   double charge = 1.0;
   double q_over_p = 0.;
   if (p != 0.)  {
      q_over_p = 1. / p;
      charge = p / fabs(p);
   } else {
      return 1; //No match if muFast Pt is zero
   }

   bool   isBarrel = ((feature->saddress() != -1) ? true : false);

   double sp1_z = feature->sp1_z();
   double sp1_R = feature->sp1_r();
   double sp2_z = feature->sp2_z();
   //double sp2_R = feature->sp2_r();
   //double sp3_z = feature->sp3_z();
   //double sp3_R = feature->sp3_r();

   if ((fabs(feature->sp1_r()) < 1000.)) {
      sp1_z = feature->sp1_z() * 10.;
      sp1_R = feature->sp1_r() * 10.;
   }
   if ((fabs(feature->sp2_r()) < 1300.)) {
      sp2_z = feature->sp2_z() * 10.;
      //   sp2_R = feature->sp2_r() * 10.;
   }
   //if ((fabs(feature->sp3_r()) < 1300.)) {
   //   sp3_z = feature->sp3_z() * 10.;
   //   sp3_R = feature->sp3_r() * 10.;
   //}

   double R = sp1_R;
   double z = sp1_z;

   if (R == 0. && z == 0.) { //treat patological endcap cases
      if (fabs(feature->sp2_r()) > 1300.) {
         z = sp2_z;
         if (z == 0.) z = 7600.;
         R = z * tan(theta);
      }
   }

   double x = R * cos(phi);
   double y = R * sin(phi);

   Amg::Vector3D vertex(x, y, z);
   //Trk::GlobalPosition vertex(x, y, z);
   DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>();
   Trk::PerigeeSurface pgsf(vertex);
   Trk::TrackParameters* trkparameters = new Trk::Perigee(0., 0., phi, theta, q_over_p, pgsf);
   trackStateOnSurfaces->push_back(new Trk::TrackStateOnSurface(0, trkparameters, 0, 0));

   Trk::TrackInfo info(Trk::TrackInfo::Unknown, Trk::muon);
   Trk::Track* newtrk = new Trk::Track(info, trackStateOnSurfaces, 0);

   //ID parameters
   double id_eta       = idtrack->param()->eta();
   double id_phi       = idtrack->param()->phi0();
   double id_pt        = idtrack->param()->pT();
   double id_eeta      = idtrack->param()->eeta();
   double id_ephi      = idtrack->param()->ephi0();
   double id_ept       = idtrack->param()->epT();
   double id_ptinv     = 1.0e33;
   double id_eptinv    = 1.0e33;
   double id_charge    = 1.0;
   if (id_pt != 0) {
      id_charge = id_pt / fabs(id_pt);
      id_ptinv  = 1. / id_pt;
      id_eptinv = id_ept / id_pt / id_pt;
   } else {
      return 3; //no match if ID track Pt zero
   }

   //Backextrapolation
   Trk::Track* extrnewtrk = m_backExtrapolatorG4->extrapolate(*newtrk);
   if (extrnewtrk == NULL) {
      delete newtrk;
      return 2; //no match in case of failed backextrapolation
   }

   const Trk::Perigee* muonPerigee = extrnewtrk->perigeeParameters();
   double extr_theta    = muonPerigee -> parameters()[Trk::theta];
   double extr_phi      = muonPerigee -> parameters()[Trk::phi0];
   double extr_q_over_p = muonPerigee -> parameters()[Trk::qOverP];
   double extr_eta      = -log(tan(extr_theta / 2.));
   int    extr_q        = (extr_q_over_p > 0 ? 1 : -1);
   double extr_pt       = extr_q * 1.0e33;
   if (extr_q_over_p != 0.) extr_pt = (1. / extr_q_over_p) * sin(extr_theta);
   double extr_eeta = muCombUtil::getMuFastEtaRes(feature);
   double extr_ephi = muCombUtil::getMuFastPhiRes(feature);
   double extr_ptinv    = 1.0e33;
   if (extr_pt != 0) extr_ptinv = 1. / extr_pt;
   double extr_eptinv   = muCombUtil::getMuFastRes(m_muFastRes_barrel, feature);

   //Combined muon parameters
   combPtInv = muCombUtil::getCombinedAverage(extr_ptinv, extr_eptinv, id_ptinv, id_eptinv);
   combPtRes = muCombUtil::getCombinedAverageSigma(extr_eptinv, id_eptinv);
   double q_tmp = charge;
   if (m_ChargeStrategy == 1) q_tmp = id_charge;
   else if (m_ChargeStrategy == 2) {
      if (1. / combPtInv > 50000.) q_tmp = charge;
      else                       q_tmp = id_charge;
   }
   combPtInv *= q_tmp;

   //Masaki/Kunihiro treatment of TGC/RPC readout problems
   if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << " Enlarge phi matching error in case TGC/RPC readout failed. dq_var2:" << feature->dq_var2() << endreq;

   if (feature->dq_var2() > 0) extr_ephi *= 2.0;

   //Match
   deta = muCombUtil::getDeltaEta(extr_eta, id_eta);
   dphi = muCombUtil::getDeltaPhi(extr_phi, id_phi);
   chi2 = muCombUtil::getChi2(ndof, combPtInv, extr_eta, extr_eeta, extr_phi, extr_ephi, extr_ptinv, extr_eptinv,
                              id_eta, id_eeta, id_phi, id_ephi, id_ptinv, id_eptinv, m_UseAbsPt);

   msg() << MSG::DEBUG << m_test_string
         << " REGTEST Resolution / IdRes / muFastRes / combRes:"
         << " / " << std::setw(11) << id_eptinv
         << " / " << std::setw(11) << extr_eptinv
         << " / " << std::setw(11) << combPtRes
         << endreq;

   msg() << MSG::DEBUG << m_test_string
         << " REGTEST Momentum / IdPt / muFastPt  / CombPt :"
         << " / " << std::setw(11) << 1. / id_ptinv
         << " / " << std::setw(11) << 1. / extr_ptinv
         << " / " << std::setw(11) << 1. / combPtInv << endreq;

   msg() << MSG::DEBUG << m_test_string
         << " REGTEST Chi2 / ndof :"
         << " / " << std::setw(11) << chi2
         << " / " << std::setw(11) << ndof << endreq;

   //Cuts
   double winEtaSigma = m_WinEta_g4;
   double winPhiSigma = m_WinPhi_g4;
   double maxChi2     = m_Chi2Max_g4;
   if (!isBarrel) {//EC
      winEtaSigma = m_WinEta_EC_g4;
      winPhiSigma = m_WinPhi_EC_g4;
      maxChi2     = m_Chi2Max_EC_g4;
   }

   msg() << MSG::DEBUG << m_test_string
         << " REGTEST DeltaEta / DeltaPhi / WinEta / WinPhi:"
         << " / " << std::setw(11) << fabs(deta)
         << " / " << std::setw(11) << fabs(dphi)
         << " / " << std::setw(11) << m_WeightEta_g4*winEtaSigma*sqrt(extr_eeta * extr_eeta + id_eeta * id_eeta)
         << " / " << std::setw(11) << m_WeightPhi_g4*winPhiSigma*sqrt(extr_ephi * extr_ephi + id_ephi * id_ephi)
         << endreq;

   if (fabs(deta) > m_WeightEta_g4 * winEtaSigma * sqrt(extr_eeta * extr_eeta + id_eeta * id_eeta)) {
      return 4;
   }
   if (fabs(dphi) > m_WeightPhi_g4 * winPhiSigma * sqrt(extr_ephi * extr_ephi + id_ephi * id_ephi)) {
      return 5;
   }
   if (ndof >= m_NdofMin) {
      if (chi2 > maxChi2) return 6;
   }

   return 0; //match OK
}

int muComb::mfMatch(const MuonFeature* feature, const TrigInDetTrack* idtrack,
                    double& combPtInv, double& combPtRes, double& deta, double& dphi, double& chi2, int& ndof)
{

   chi2 = 1.0e30;
   ndof = 0;
   //muFast parameters

   double    pt = feature->pt() * 1000.;
   if (pt == 0.)  {
      return 1; //No match if muFast Pt is zero
   }

   bool   isTS     = ((feature->radius() <= 10.) ? true : false);
   bool   isBarrel = ((feature->saddress() != -1) ? true : false);

   double charge = pt / fabs(pt);
   double ptinv  = 1. / pt;
   double eptinv = muCombUtil::getMuFastRes(m_muFastRes_barrel, feature);
   //ID parameters
   double id_z         = idtrack->param()->z0();
   double id_eta       = idtrack->param()->eta();
   double id_phi       = idtrack->param()->phi0();
   double id_pt        = idtrack->param()->pT();
   double id_eeta      = idtrack->param()->eeta();
   double id_ephi      = idtrack->param()->ephi0();
   double id_eptinv    = muCombUtil::getIDSCANRes(m_IDSCANRes_barrel, m_IDSCANRes_endcap1, m_IDSCANRes_endcap2,
                                                  m_IDSCANRes_endcap3, m_IDSCANRes_endcap4, idtrack);
   double id_ptinv     = 1.0e33;
   //double id_ept       = 1.0e33;
   double id_charge    = 1.0;
   if (id_pt != 0) {
      id_charge = id_pt / fabs(id_pt);
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
      else                       q_tmp = id_charge;
   }
   combPtInv *= q_tmp;

   //Backextrapolation
   double extr_eta_tmp  = 0.;
   double extr_eeta_tmp = 0.;
   double extr_phi_tmp  = 0.;
   double extr_ephi_tmp = 0.;
   double winPt = m_winPt.value()[0];
   if (winPt <  4.0) winPt =  4.0;
   if (winPt > 40.0) winPt = 40.0;

   if (!((m_backExtrapolatorLUT->give_eta_phi_at_vertex((1. / ptinv) / 1000., feature, extr_eta_tmp,
                                                        extr_eeta_tmp, extr_phi_tmp, extr_ephi_tmp, winPt)).isSuccess())) {
      return 2; //no match in case of failed backextrapolation
   }
   double extr_eta    = extr_eta_tmp;
   double extr_eeta   = extr_eeta_tmp;
   double extr_phi    = extr_phi_tmp;
   double extr_ephi   = extr_ephi_tmp;
   double extr_ptinv  = ptinv;
   double extr_eptinv = eptinv;

   //Refined extrapolation
   if (((m_backExtrapolatorLUT->give_eta_phi_at_vertex((1. / combPtInv) / 1000., feature, id_z, extr_eta_tmp,
                                                       extr_eeta_tmp, extr_phi_tmp, extr_ephi_tmp, 0.0)).isSuccess())) {
      extr_eta    = extr_eta_tmp;
      extr_eeta   = extr_eeta_tmp;
      extr_phi    = extr_phi_tmp;
      extr_ephi   = extr_ephi_tmp;
   }

   //Masaki/Kunihiro treatment of TGC/RPC readout problems
   if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << " Enlarge phi matching error in case TGC/RPC readout failed. dq_var2:" << feature->dq_var2() << endreq;

   if (feature->dq_var2() > 0) extr_ephi *= 2.0;

   //Match
   deta = muCombUtil::getDeltaEta(extr_eta, id_eta);
   dphi = muCombUtil::getDeltaPhi(extr_phi, id_phi);
   chi2 = muCombUtil::getChi2(ndof, combPtInv, extr_eta, extr_eeta, extr_phi, extr_ephi, extr_ptinv, extr_eptinv,
                              id_eta, id_eeta, id_phi, id_ephi, id_ptinv, id_eptinv, m_UseAbsPt);


   msg() << MSG::DEBUG << m_test_string
         << " REGTEST Resolution / IdRes / muFastRes / combRes:"
         << " / " << std::setw(11) << id_eptinv
         << " / " << std::setw(11) << extr_eptinv
         << " / " << std::setw(11) << combPtRes
         << endreq;

   msg() << MSG::DEBUG << m_test_string
         << " REGTEST Momentum / IdPt / muFastPt  / CombPt :"
         << " / " << std::setw(11) << 1. / id_ptinv
         << " / " << std::setw(11) << 1. / ptinv
         << " / " << std::setw(11) << 1. / combPtInv << endreq;

   msg() << MSG::DEBUG << m_test_string
         << " REGTEST Chi2 / ndof :"
         << " / " << std::setw(11) << chi2
         << " / " << std::setw(11) << ndof << endreq;

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

   msg() << MSG::DEBUG << m_test_string
         << " REGTEST DeltaEta / DeltaPhi / WinEta / WinPhi:"
         << " / " << std::setw(11) << fabs(deta)
         << " / " << std::setw(11) << fabs(dphi)
         << " / " << std::setw(11) << m_WeightEta*winEtaSigma*sqrt(extr_eeta * extr_eeta + id_eeta * id_eeta)
         << " / " << std::setw(11) << m_WeightPhi*winPhiSigma*sqrt(extr_ephi * extr_ephi + id_ephi * id_ephi)
         << endreq;

   if (fabs(deta) > m_WeightEta * winEtaSigma * sqrt(extr_eeta * extr_eeta + id_eeta * id_eeta)) {
      return 4;
   }
   if (fabs(dphi) > m_WeightPhi * winPhiSigma * sqrt(extr_ephi * extr_ephi + id_ephi * id_ephi)) {
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


   //Algorithm Strategy printout
   if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "MuCombStrategy: " << m_AlgoStrategy << endreq;

   //Decode TE
   if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "outputTE->ID(): " << outputTE->getId() << endreq;

   // Get input for seeding
   ElementLink<MuonFeatureContainer> muonFeatureEL;
   const LVL1::RecMuonRoI* muonRoI = 0;
   const MuonFeature*  pMuonFeature = 0;
   bool  muFastPtOK = true;

   if (m_AlgoStrategy < 5)  { //Auto strategy or muFast seeding explicitely requested
      if (HLT::OK == getFeatureLink<MuonFeatureContainer, MuonFeature> (outputTE, muonFeatureEL)) {
         if (muonFeatureEL.isValid()) {
            pMuonFeature = *muonFeatureEL;
            if (pMuonFeature->pt() == 0.) {
               m_ErrorFlagMC = 1;
               muFastPtOK = false;
               msg() << MSG::DEBUG << "muFast pt==0.!" << endreq;
               if (m_AlgoStrategy != 0) {//if a muFast based strategy is explicitely requested return an empty feature and exit
                  //demoted to DEBUG during commissioning phase
                  //msg() << MSG::WARNING << "muFast pt == 0. --> noMatch" << endreq;
                  msg() << MSG::DEBUG << "muFast pt == 0. --> no match" << endreq;
                  ElementLink<TrigInDetTrackCollection> pTrack(0, 0);
                  CombinedMuonFeature* muon_feature = new CombinedMuonFeature(0., -1.0, 1.0, m_AlgoStrategy, m_ErrorFlagMC, 0, muonFeatureEL, pTrack);

                  return muCombSeed(outputTE, muon_feature);
               } else {
                  muFastPtOK = false;
               }
            }
         } else  {
            if (m_AlgoStrategy != 0) {
               msg() << MSG::ERROR << " getFeatureLink finds no TrigMuonFeature (EL invalid)" << endreq;
               m_ErrorFlagMC = 0;
               return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
            } else {
               if (msgLvl() <= MSG::DEBUG)
                  msg() << MSG::DEBUG << " getFeatureLink finds no TrigMuonFeature (EL invalid) --> use L1 ROI" << endreq;
               muFastPtOK = false;
            }
         }
      } else {
         if (m_AlgoStrategy != 0) {
            msg() << MSG::ERROR << " getFeatureLink fails to get MuonFeature " << endreq;
            m_ErrorFlagMC = 0;
            return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
         } else {
            if (msgLvl() <= MSG::DEBUG)
               msg() << MSG::DEBUG << " getFeatureLink fails to get MuonFeature --> use L1 ROI" << endreq;
            muFastPtOK = false;
         }
      }
   } else {
      muFastPtOK = false;
   }

   // if muFastPt=0. or if explicitely requested use L1 ROI as seed to search for the ID track
   if (!muFastPtOK) {
      // Get latest L1 RoI
      if (HLT::OK != getFeature(inputTE, muonRoI, "")) {
         m_ErrorFlagMC = 2;
         //msg() << MSG::WARNING  << "Could not find the LVL1 roi" << endreq;
         msg() << MSG::DEBUG << "Could not find the LVL1 roi" << endreq;
         msg() << MSG::DEBUG << "muFast pt == 0. && no L1 --> no match" << endreq;
         ElementLink<TrigInDetTrackCollection> pTrack(0, 0);
         CombinedMuonFeature* muon_feature = new CombinedMuonFeature(0., -1.0, 1.0, m_AlgoStrategy, m_ErrorFlagMC, 0, muonFeatureEL, pTrack);
         return muCombSeed(outputTE, muon_feature);
      }
   }

   double pt    = -9999.;
   double eta   = -9999.;
   double phi   = -9999.;
   double zeta  = -9999.;

   if (muFastPtOK) {
      pt   = pMuonFeature->pt();
      eta  = pMuonFeature->eta();
      phi  = pMuonFeature->phi();
      zeta = pMuonFeature->zeta();

      if (msgLvl() <= MSG::DEBUG)
         msg() << MSG::DEBUG
               << "Input MuonFeature saddress = " << pMuonFeature->saddress()
               << " / pt = "                      << pMuonFeature->pt()
               << " / radius = "                  << pMuonFeature->radius()
               << " / eta = "                     << pMuonFeature->eta()
               << " / phi = "                     << pMuonFeature->phi()
               << " / dir_phi = "                 << pMuonFeature->dir_phi()
               << " / zeta = "                    << pMuonFeature->zeta()
               << " / dir_zeta = "                << pMuonFeature->dir_zeta()
               << endreq;
   } else {
      pt   = muonRoI->getThresholdValue(); //in GeV
      eta  = muonRoI->eta();
      phi  = muonRoI->phi();
      zeta = -9999.;

      if (msgLvl() <= MSG::DEBUG)
         msg() << MSG::DEBUG
               << "Used L1 Muon ROI pt is: "      << pt
               << " / eta = "                     << eta
               << " / phi = "                     << phi
               << endreq;
   }

   // ID tracks Decoding
   std::vector<std::pair<const TrigInDetTrackCollection*, int> > mySelectedTracks;
   std::vector <const TrigInDetTrackCollection*> vectorOfTrackCollections;
   std::string algoId = m_ID_algo_to_use;
   HLT::ErrorCode status = getFeatures(outputTE, vectorOfTrackCollections, "");

   if (status != HLT::OK) {
      m_ErrorFlagMC = 3;
      msg() << MSG::DEBUG << " Failed to get " << algoId << " InDetTrackCollections --> no match" << endreq;
      ElementLink<TrigInDetTrackCollection> pTrack(0, 0);
      CombinedMuonFeature* muon_feature = new CombinedMuonFeature(0., -1.0, 1.0, m_AlgoStrategy, m_ErrorFlagMC, 0, muonFeatureEL, pTrack);
      return muCombSeed(outputTE, muon_feature);
   }


   if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << " Got " << vectorOfTrackCollections.size() << " InDetTrackCollections, now fill InDet Block " << endreq;

   std::vector<const TrigInDetTrackCollection*>::iterator
   theTrackColl = vectorOfTrackCollections.begin(),
   endTrackColl = vectorOfTrackCollections.end();

   for (; theTrackColl != endTrackColl;  theTrackColl++) { //Tracks Collections Loop

      TrigInDetTrackCollection::const_iterator
      track     = (*theTrackColl)->begin(),
      lasttrack = (*theTrackColl)->end();

      ATH_MSG_DEBUG( "InDetTrackCollection n(tracks) = " << (*theTrackColl)->size());

      for (int trackIndex = 0; track != lasttrack; track++, trackIndex++) { //Tracks Loop

         // Check if event timeout was reached
         if (Athena::Timeout::instance().reached()) {
            if (msgLvl() <= MSG::DEBUG)
               msg() << MSG::DEBUG << "Timeout reached. Trk loop, Aborting sequence." << endreq;
            return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
         }

         //Select tracks
         bool isRightAlgo = false;
         if (((*track)->algorithmId() == m_algoId)) isRightAlgo = true;
         if (algoId == "TrigFastTrackFinder_Muon")  isRightAlgo = true; //tmporary fix

         if (!isRightAlgo) continue;

         if (!((*track)->param())) continue;   //Must have helix parameters
         double pt_id    = (*track)->param()->pT() / 1000.; //in GeV/c
         double phi_id   = (*track)->param()->phi0();
         double eta_id   = (*track)->param()->eta();
         double zPos_id  = (*track)->param()->z0();
         double chi2_id  = (*track)->chi2();
         int    npixh_id = (*track)->NPixelSpacePoints();
         int    nscth_id = (*track)->NSCT_SpacePoints();

         if (msgLvl() <= MSG::DEBUG)
            msg() << MSG::DEBUG << "Found track: "
                  << m_ID_algo_to_use
                  << "  with pt= " << pt_id
                  << ", eta=" << eta_id
                  << ", phi=" << phi_id
                  << ", Zid=" << zPos_id
                  << ", Chi2=" << chi2_id
                  << ", NPix=" << npixh_id
                  << ", NSCT=" << nscth_id
                  << endreq;

         if (fabs(pt_id)   < m_PtMinTrk)       continue;
         if (fabs(eta_id)  > m_EtaMaxTrk)      continue;
         if (fabs(zPos_id) > m_ZMaxTrk)        continue;
         //if (chi2_id       > m_Chi2MaxTrk)     continue;
         //if (npixh_id      < m_NPIXhitMinTrk)  continue;
         //if (nscth_id      < m_NSCThitMinTrk)  continue;

         if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Track selected " << endreq;
         std::pair<const TrigInDetTrackCollection*, int> selTrack(*theTrackColl, trackIndex);
         mySelectedTracks.push_back(selTrack);

      }//Tracks loop
   }//Track collections loop

   if (!mySelectedTracks.size()) {
      m_ErrorFlagMC = 4;
      msg() << MSG::DEBUG << " No selected ID tracks --> no match" << endreq;
      ElementLink<TrigInDetTrackCollection> pTrack(0, 0);
      CombinedMuonFeature* muon_feature = new CombinedMuonFeature(0., -1.0, 1.0, m_AlgoStrategy, m_ErrorFlagMC, 0, muonFeatureEL, pTrack);
      return muCombSeed(outputTE, muon_feature);
   }

   //Magnetic field status
   bool toroidOn   = !m_assumeToroidOff;
   bool solenoidOn = !m_assumeSolenoidOff;
   if (m_useAthenaFieldService) {
      //if (!m_MagFieldSvc) service("AtlasFieldSvc", m_MagFieldSvc, /*createIf=*/ false).ignore();
      if (m_MagFieldSvc) {
         toroidOn  = m_MagFieldSvc->toroidOn() && !m_assumeToroidOff;
         solenoidOn = m_MagFieldSvc->solenoidOn() && !m_assumeSolenoidOff;
      }
   }
   if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "=========== Magnetic Field Status ========== " << endreq;
      msg() << MSG::DEBUG << " B Fields read from AthenaFieldService:   " << (m_useAthenaFieldService ? "TRUE" : "FALSE") << endreq;
      msg() << MSG::DEBUG << " Assuming Toroid OFF is:                  " << (m_assumeToroidOff ? "TRUE" : "FALSE") << endreq;
      msg() << MSG::DEBUG << " Assuming Solenoid OFF is:                " << (m_assumeSolenoidOff ? "TRUE" : "FALSE") << endreq;
      msg() << MSG::DEBUG << " ---> Solenoid : " << ((solenoidOn) ? "ON" : "OFF") << endreq;
      msg() << MSG::DEBUG << " ---> Toroid   : " << ((toroidOn) ?   "ON" : "OFF") << endreq;
   }

   //muComb combination strategies
   int usealgo = 1;
   if (m_AlgoStrategy > 0) {//Explicit strategy selection
      if (m_AlgoStrategy <= 8) {
         usealgo = m_AlgoStrategy;
      } else {
         msg() << MSG::DEBUG << " m_AlgoStrategy out of range, is: " << m_AlgoStrategy << " while must be in [0,8], value 1 used" << endreq;
         m_ErrorFlagMC = 5;
      }
   } else { //Auto-strategy selection
      if (muFastPtOK) {//muFast seeded
         if (solenoidOn) {
            if (toroidOn)  usealgo = 1; //solenoid ON - Toroid ON algo: i.e. Std muFast back_extrapolation & matching algo.
            else           usealgo = 2; //solenoid ON - Toroid OFF algo: i.e. MS alignment algo
         } else {
            if (toroidOn)  usealgo = 3; //solenoid OFF - Toroid ON algo: for commissioning studies
            else           usealgo = 4; //solenoid OFF - Toroid OFF algo: straight tracks
         }
      } else { //L1 seeded
         if (solenoidOn) {
            if (toroidOn)  usealgo = 5; //solenoid ON - Toroid ON algo: i.e. L1 seeded
            else           usealgo = 6; //solenoid ON - Toroid OFF algo: i.e. L1 seeded
         } else {
            if (toroidOn)  usealgo = 7; //solenoid OFF - Toroid ON algo: L1 seeded
            else           usealgo = 8; //solenoid OFF - Toroid OFF algo: L1 seeded
         }
      }
   }
   m_StrategyMC = usealgo;
   if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << " matching strategy selected is: " << usealgo << endreq;

   //Matching step

   double ptinv_comb = 0.;
   double ptres_comb = 0.001;
   double chi2_comb  = 1.0e33;
   int    ndof_comb  = -1;
   double best_dr    = 1.0e33;
   double best_deta  = 1.0e33;
   double best_dphi  = 1.0e33;

   bool   has_match  = false;
   int    imatch     = -1;

   std::pair<const TrigInDetTrackCollection*, int> selTrack(0, 0);

   std::vector<std::pair<const TrigInDetTrackCollection*, int> >::iterator
   selTrackColl    = mySelectedTracks.begin(),
   endselTrackColl = mySelectedTracks.end();

   for (; selTrackColl != endselTrackColl;  selTrackColl++) {

      // Check if event timeout was reached
      if (Athena::Timeout::instance().reached()) {
         if (msgLvl() <= MSG::DEBUG)
            msg() << MSG::DEBUG << "Timeout reached. selTrkColl loop, Aborting sequence." << endreq;
         return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
      }

      const TrigInDetTrackCollection* trackColl = (*selTrackColl).first;
      int   trackIndex = (*selTrackColl).second;
      const TrigInDetTrack* track = (*trackColl)[trackIndex];

      double pt_ms = 0.;
      if (usealgo != 2 || usealgo != 4 || usealgo != 6 || usealgo != 8) pt_ms = pt;                     //actual MS pt when toroid ON

      double ptinv_tmp = 0.;
      double ptres_tmp = 0.001;
      double deta_tmp  = 0.;
      double dphi_tmp  = 0.;
      double dr_tmp    = 0.;
      double chi2_tmp  = 0.;
      int    ndof_tmp  = 0;
      bool   has_match_tmp = false;
      int    imatch_tmp    = -1;

      if (usealgo > 1) {//DeltaR match
         imatch_tmp = drptMatch(pt_ms, eta, phi, track, usealgo, ptinv_tmp, ptres_tmp, deta_tmp, dphi_tmp, chi2_tmp);
         if (imatch_tmp == 0) has_match_tmp = true;
      } else { //Std match
         if (!m_useBackExtrapolatorG4) {
            imatch_tmp = mfMatch(pMuonFeature, track, ptinv_tmp, ptres_tmp, deta_tmp, dphi_tmp, chi2_tmp, ndof_tmp);
            if (imatch_tmp == 0) has_match_tmp = true;
            if (Athena::Timeout::instance().reached()) {
               if (msgLvl() <= MSG::DEBUG)
                  msg() << MSG::DEBUG << "Timeout reached. mfMatch backextrapolation, Aborting sequence." << endreq;
               return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
            }
         } else { //G4 match
            imatch_tmp = g4Match(pMuonFeature, track, ptinv_tmp, ptres_tmp, deta_tmp, dphi_tmp, chi2_tmp, ndof_tmp);
            if (imatch_tmp == 0) has_match_tmp = true;
            if (Athena::Timeout::instance().reached()) {
               if (msgLvl() <= MSG::DEBUG)
                  msg() << MSG::DEBUG << "Timeout reached. g4Match backextrapolation, Aborting sequence." << endreq;
               return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
            }
         }
      }

      dr_tmp = sqrt(deta_tmp * deta_tmp + dphi_tmp * dphi_tmp);
      imatch = imatch_tmp;

      if (!has_match_tmp) continue;

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
         selTrack.first = trackColl;
         selTrack.second = trackIndex;
      }
   }

   //Set monitored quantities (monitor only pt>6 GeV/ c&& mf seeded combined muons)
   if (usealgo == 1 && fabs(pt) >= 6.) {
      m_ptMS       = pt;
      m_etaMS      = eta;
      m_phiMS      = phi;
      m_zetaMS     = zeta;
      m_ptFL       = pt;
      m_etaFL      = eta;
      m_phiFL      = phi;
      if (m_ptMS >  100.) m_ptMS =  101.5;
      if (m_ptMS < -100.) m_ptMS = -101.5;
      if (m_ptFL >  100.) m_ptFL =  101.5;
      if (m_ptFL < -100.) m_ptFL = -101.5;
   }

   if (!has_match) {
      m_ErrorFlagMC = 6;
      m_MatchFlagMC = imatch;
      if (usealgo == 1 && fabs(pt) >= 6.)  m_efficiency = 0; //monitor only efficiency for mu6 && mf seeded combined muons
      msg() << MSG::DEBUG << " No matched ID tracks --> no match" << endreq;
      ElementLink<TrigInDetTrackCollection> pTrack(0, 0);
      CombinedMuonFeature* muon_feature = new CombinedMuonFeature(0., -1.0, 1.0, usealgo, m_ErrorFlagMC, 0, muonFeatureEL, pTrack);
      return muCombSeed(outputTE, muon_feature);
   }

   const TrigInDetTrack* bestTrack = (*selTrack.first)[selTrack.second];
   double pt_id        = bestTrack->param()->pT();
   double phi_id       = bestTrack->param()->phi0();
   double eta_id       = bestTrack->param()->eta();
   double zPos_id      = bestTrack->param()->z0();

   //Update monitored vars
   m_MatchFlagMC = imatch;
   m_ptFL        = -9999.;
   m_etaFL       = -9999.;
   m_phiFL       = -9999.;
   if (usealgo == 1 && fabs(pt) >= 6.) {
      m_efficiency  = 1;
      m_ptID        = pt_id / 1000.; //in GeV/c
      m_etaID       = eta_id;
      m_phiID       = phi_id;
      m_zetaID      = zPos_id;
      m_ptMC        = 1. / (ptinv_comb * 1000.); //in GeV/c
      m_dZeta       = zeta - zPos_id;
      m_dPhi        = best_dphi;
      m_dEta        = best_deta;
      m_dR          = best_dr;

      if (m_ptMC >  100.) m_ptMC =  101.5;
      if (m_ptMC < -100.) m_ptMC = -101.5;
      if (m_ptID >  100.) m_ptID =  101.5;
      if (m_ptID < -100.) m_ptID = -101.5;
   }

   if (usealgo < 5) {
      if (usealgo == 1) {
         msg() << MSG::DEBUG << m_test_string << " REGTEST Combination chosen: "
               << "Combined Pt / IdPt / muFastPt / CombPt / chi2 / ndof: "
               << " / " << pt_id << " / " << pt * 1000. << " / " << 1. / ptinv_comb << " / " << chi2_comb << " / " << ndof_comb << endreq;
      } else {
         msg() << MSG::DEBUG << m_test_string << " REGTEST Combination chosen: "
               << "Combined Pt / IdPt / muFastPt / CombPt: "
               << " / " << pt_id << " / " << pt * 1000. << " / " << 1. / ptinv_comb << endreq;
      }
   } else {
      msg() << MSG::DEBUG << m_test_string << " REGTEST Combination chosen: "
            << "Combined Pt / IdPt / L1MUPt / CombPt: "
            << " / " << pt_id << " / " << pt * 1000. << " / " << 1. / ptinv_comb << endreq;
   }


   float mcresu = fabs(ptres_comb / (ptinv_comb * ptinv_comb));
   if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << " SigmaPt is: " << mcresu << endreq;

   //Pack usedalgo info and muComb resolution in one variable (tmp fix to avoid changes in the muon combined feature edm)
   //float resu = fabs(ptres_comb/(ptinv_comb*ptinv_comb))/100.;
   //if (resu > 5000.) resu = 5000.0;
   //if (resu < 0.002) resu = 0.002;
   //float packedinfo = static_cast<float>(10000*usealgo) + resu;

   // fill the Combined feature with the best match found

   ElementLinkVector<TrigInDetTrackCollection> elv;
   if (HLT::OK != getFeaturesLinks<TrigInDetTrackCollection, TrigInDetTrackCollection>(outputTE, elv, "")) {
      msg() << MSG::DEBUG << " Failed to get InDetTrackCollections element links --> no match" << endreq;
      m_ErrorFlagMC = 7;
      ElementLink<TrigInDetTrackCollection> pTrack(0, 0);
      CombinedMuonFeature* muon_feature = new CombinedMuonFeature(0., -1.0, 1.0, usealgo, m_ErrorFlagMC, 0, muonFeatureEL, pTrack);
      return muCombSeed(outputTE, muon_feature);
   }

   ElementLinkVector<TrigInDetTrackCollection>::const_iterator it = elv.begin(), itEnd = elv.end();
   for (int itrk = 0; it != itEnd; ++it, ++itrk) {
      if ((*(*it)) == bestTrack) {
         float mcq = -1.0;
         if (ptinv_comb > 0) mcq = 1.0;
         CombinedMuonFeature* muon_feature = new CombinedMuonFeature(fabs(1. / ptinv_comb), mcresu, mcq, usealgo, m_ErrorFlagMC,
                                                                     m_MatchFlagMC, muonFeatureEL, *it);
         return muCombSeed(outputTE, muon_feature);
      }
   }

   //IF everthing's fine Should never reach this step ...
   msg() << MSG::DEBUG << " Failed to fill the Combined fature with the ID track" << endreq;
   m_ErrorFlagMC = 8;
   ElementLink<TrigInDetTrackCollection> pTrack(0, 0);
   CombinedMuonFeature* muon_feature = new CombinedMuonFeature(0., -1.0, 1.0, usealgo, m_ErrorFlagMC, 0, muonFeatureEL, pTrack);
   return muCombSeed(outputTE, muon_feature);
}

HLT::ErrorCode
muComb::muCombSeed(HLT::TriggerElement* outputTE, CombinedMuonFeature* muon_feature)
{

   //seeding the next Algorithm
   std::string Key = "";
   HLT::ErrorCode status;
   status = attachFeature(outputTE, muon_feature);
   if (status != HLT::OK) {
      msg() << MSG::ERROR
            << "Record of Combined Muon Feature in TriggerElement failed"
            << endreq;
      return status;
   } else {
      if (msgLvl() <= MSG::DEBUG)
         msg() << MSG::DEBUG
               << "Combined Muon Feature attached to the TriggerElement"
               << endreq;
   }

   outputTE->setActiveState(true);
   return HLT::OK;
}

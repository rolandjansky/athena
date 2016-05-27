/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: tabstop=2:shiftwidth=2:expandtab
// -*- C++ -*-//
/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBPhysHypo/TrigEFBMuMuXFex.cxx
 **
 **   Description: EF hypothesis algorithms for
 **                B^0_{s,d},\Lambda_b \to X \mu^+ \mu^-
 **
 **
 **   Author: J.Kirk
 **   Author: Semen Turchikhin <Semen.Turchikhin@cern.ch>
 **
 **   Created:   12.09.2007
 **   Modified:  08.04.2012
 **
 **   Modified:  26.09.2012 : "B_c -> D_s(*) (->Phi(->K+K-)) mu+mu-" added (Leonid Gladilin <gladilin@mail.cern.ch>)
 **
 **
 ***************************************************************************/

//#include "TrigMuonEvent/CombinedMuonFeature.h"

#include "TrigEFBMuMuXFex.h"

//#include "TrigParticle/TrigEFBphysContainer.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include <math.h>
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "AthenaKernel/Timeout.h"

//#include "MuidEvent/MuidTrackContainer.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"
//#include "Particle/TrackParticleContainer.h"
#include "VxVertex/VxCandidate.h"
//#include "TrigVKalFitter/TrigVKalFitter.h"
//#include "TrigVKalFitter/VKalVrtAtlas.h"

//#include "TrigNavigation/Navigation.h"

//class ISvcLocator;

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"
#include <memory>

/*----------------------------------------------------------------------------*/
TrigEFBMuMuXFex::TrigEFBMuMuXFex(const std::string & name, ISvcLocator* pSvcLocator):
HLT::ComboAlgo(name, pSvcLocator)
,m_fitterSvc("Trk::TrkVKalVrtFitter/VertexFitterTool",this)
,m_bphysHelperTool("TrigBphysHelperUtilsTool")
//,m_trigBphysColl_b(NULL)
//,m_trigBphysColl_X(NULL)
,mTrigBphysColl_b(NULL)
,mTrigBphysColl_X(NULL)

,m_TotTimer(0)
,m_VtxFitTimer(0)
,m_VKVFitter(0)

// counters
,m_lastEvent(-1)
,m_lastEventPassed(-1)
,m_countTotalEvents(0)
,m_countTotalRoI(0)
,m_countPassedEvents(0)
,m_countPassedRoIs(0)

,m_lastEventPassedBplus(-1)
,m_lastEventPassedBd(-1)
,m_lastEventPassedBs(-1)
,m_lastEventPassedLb(-1)
,m_lastEventPassedBc(-1)
,m_countPassedEventsBplus(0)
,m_countPassedEventsBs(0)
,m_countPassedEventsBd(0)
,m_countPassedEventsLb(0)
,m_countPassedEventsBc(0)

,m_countPassedMuMuID(0)
,m_countPassedMuMuOS(0)
,m_countPassedMuMuMass(0)
,m_countPassedMuMuVtx(0)
,m_countPassedMuMuVtxChi2(0)

,m_countPassedBplusMass(0)
,m_countPassedBplusVtx(0)
,m_countPassedBplusVtxChi2(0)
,m_countBpToStore(0)

,m_countPassedKstarMass(0)
,m_countPassedBdMass(0)
,m_countPassedKstarVtx(0)
,m_countPassedKstarVtxChi2(0)
,m_countPassedBdVtx(0)
,m_countPassedBdVtxChi2(0)
,m_countBdToStore(0)

,m_countPassedPhi1020Mass(0)
,m_countPassedBsMass(0)
,m_countPassedPhi1020Vtx(0)
,m_countPassedPhi1020VtxChi2(0)
,m_countPassedBsVtx(0)
,m_countPassedBsVtxChi2(0)
,m_countBsToStore(0)

,m_countPassedLambdaMass(0)
,m_countPassedLbMass(0)
,m_countPassedLambdaVtx(0)
,m_countPassedLambdaVtxChi2(0)
,m_countPassedLbVtx(0)
,m_countPassedLbVtxChi2(0)
,m_countLbToStore(0)

,m_countPassedPhiDsMass(0)
,m_countPassedDsMass(0)
,m_countPassedBcMass(0)
,m_countPassedDsVtx(0)
,m_countPassedDsVtxChi2(0)
,m_countPassedBcVtx(0)
,m_countPassedBcVtxChi2(0)
,m_countBcToStore(0)
{
    declareProperty("AcceptAll",    m_acceptAll=true); // Should we just accept all events
    // sign & mass cuts
    //   declareProperty("CutMuonTightness", m_cutMuon = 0.01);                               // default = 0.01
    declareProperty("OppositeSign", m_oppositeCharge = true);                            // deafult = true
    declareProperty("LowerMuMuMassCut", m_lowerMuMuMassCut = 100.);                      // default = 100.0 MeV
    declareProperty("UpperMuMuMassCut", m_upperMuMuMassCut = 5500.);                      // default = 5500.0 MeV
    //   declareProperty("LowerMuVtxMassCut", m_lowerMuVtxMassCut =100.);                    // default = 100.0 MeV
    //   declareProperty("UpperMuVtxMassCut", m_upperMuVtxMassCut = 5500.);                    // default = 5500.0 MeV
    declareProperty("MuVtxChi2Cut", m_muVtxChi2Cut = 40.);                              // default = 40.0
    
    // Maximum number of decay candidates (2 mu + 1 or 2 tracks) to test - protection against TimeOuts
    declareProperty("MaxNcombinations", m_maxNcombinations = 50000);
    
    // B+->K+MuMu cuts
    declareProperty("DoB_KMuMuDecay", m_doB_KMuMuDecay = true);                          //Proceed B->KMuMu part of algorithm
    declareProperty("LowerKMuMuMassCut", m_lowerKMuMuMassCut = 4500.);                    // default = 4500.0 MeV
    declareProperty("UpperKMuMuMassCut", m_upperKMuMuMassCut = 5900.);                    // default = 5900.0 MeV
    //   declareProperty("LowerB_KMuMuMassCutVtxOff", m_lowerB_KMuMuMassCutVtxOff = 4500.);  // default = 4500.0 MeV
    //   declareProperty("UpperB_KMuMuMassCutVtxOff", m_upperB_KMuMuMassCutVtxOff= 5900.);  // default = 5900.0 MeV
    //   declareProperty("LowerBVtxMassCut", m_lowerBVtxMassCut = 4500.);                      // default = 4500.0 MeV
    //   declareProperty("UpperBVtxMassCut", m_upperBVtxMassCut = 5900.);                      // default = 5900.0 MeV
    declareProperty("DoB_KMuMuVertexing", m_doB_KMuMuVertexing = true);
    declareProperty("BVtxChi2Cut", m_bVtxChi2Cut = 300.);                                // default = 300.0
    declareProperty("MaxBpToStore", m_maxBpToStore = -1);
    
    // Bd->K*MuMu cuts
    declareProperty("DoBd_KstarMuMuDecay", m_doBd_KstarMuMuDecay = true);                //Proceed Bd->K*MuMu part of algorithm
    declareProperty("LowerKstar_KaonMassCut", m_lowerKstar_KaonMassCut = 600.);          // default = 600.0 MeV
    declareProperty("UpperKstar_KaonMassCut", m_upperKstar_KaonMassCut = 1500.);          // default = 1500.0 MeV
    declareProperty("LowerBd_KstarMuMuMassCut", m_lowerBd_KstarMuMuMassCut= 4300.);      // default = 4600.0 MeV
    declareProperty("UpperBd_KstarMuMuMassCut", m_upperBd_KstarMuMuMassCut =6300.);      // default = 6300.0 MeV
    //   declareProperty("LowerBd_KstarMuMuMassCutVtxOff", m_lowerBd_KstarMuMuMassCutVtxOff = 4600.);  // default = 4600.0 MeV
    //   declareProperty("UpperBd_KstarMuMuMassCutVtxOff", m_upperBd_KstarMuMuMassCutVtxOff= 6300.);  // default = 6300.0 MeV
    //   declareProperty("LowerKstarVtxMassCut", m_lowerKstarVtxMassCut = 600.);              // default = 600.0 MeV
    //   declareProperty("UpperKstarVtxMassCut", m_upperKstarVtxMassCut= 1200.);              // default = 1200.0 MeV
    //   declareProperty("LowerBdVtxMassCut", m_lowerBdVtxMassCut = 4600.);                    // default = 4600.0 MeV
    //   declareProperty("UpperBdVtxMassCut", m_upperBdVtxMassCut = 6000.);                    // default = 6000.0 MeV
    declareProperty("DoKstar_KPiVertexing", m_doKstar_KPiVertexing = true);
    declareProperty("DoBd_KstarMuMuVertexing", m_doBd_KstarMuMuVertexing = true);
    declareProperty("KstarVtxChi2Cut", m_kStarVtxChi2Cut = 60.);                        // default = 60.0
    declareProperty("BdVtxChi2Cut", m_bDVtxChi2Cut = 60.);                              // default = 60.0
    declareProperty("MaxBdToStore", m_maxBdToStore = -1);
    
    // Bs->PhiMuMu cuts
    declareProperty("DoBs_Phi1020MuMuDecay", m_doBs_Phi1020MuMuDecay = true);            //Proceed Bs->PhiMuMu part of algorithm
    declareProperty("LowerPhi1020_KaonMassCut", m_lowerPhi1020_KaonMassCut  = 990.);      // default = 990.0 MeV
    declareProperty("UpperPhi1020_KaonMassCut", m_upperPhi1020_KaonMassCut = 1050.);      // default = 1050.0 MeV
    declareProperty("LowerBs_Phi1020MuMuMassCut", m_lowerBs_Phi1020MuMuMassCut = 5000.);  // default = 5000.0 MeV
    declareProperty("UpperBs_Phi1020MuMuMassCut", m_upperBs_Phi1020MuMuMassCut = 5800.);  // default = 5800.0 MeV
    //   declareProperty("LowerBs_Phi1020MuMuMassCutVtxOff", m_lowerBs_Phi1020MuMuMassCutVtxOff = 5000.);  // default = 5000.0 MeV
    //   declareProperty("UpperBs_Phi1020MuMuMassCutVtxOff", m_upperBs_Phi1020MuMuMassCutVtxOff = 5800.);  // default = 5800.0 MeV
    //   declareProperty("LowerPhi1020VtxMassCut", m_lowerPhi1020VtxMassCut = 990.);          // default = 990.0 MeV
    //   declareProperty("UpperPhi1020VtxMassCut", m_upperPhi1020VtxMassCut = 1060.);          // default = 1060.0 MeV
    //   declareProperty("LowerBsVtxMassCut", m_lowerBsVtxMassCut = 5000.);                    // default = 5000.0 MeV
    //   declareProperty("UpperBsVtxMassCut", m_upperBsVtxMassCut = 5800.);                    // default = 5800.0 MeV
    declareProperty("DoPhi1020_KKVertexing", m_doPhi1020_KKVertexing = true);
    declareProperty("DoBs_Phi1020MuMuVertexing", m_doBs_Phi1020MuMuVertexing = true);
    declareProperty("Phi1020VtxChi2Cut", m_phi1020VtxChi2Cut = 60.);                    // default = 60.0
    declareProperty("BsVtxChi2Cut", m_bSVtxChi2Cut = 60.);                              // default = 60.0
    declareProperty("MaxBsToStore", m_maxBsToStore = -1);
    
    // Lb->LMuMu cuts // cuts optimalization in progress
    declareProperty("DoLb_LambdaMuMuDecay", m_doLb_LambdaMuMuDecay = true);              //Proceed Lb->LMuMu part of algorithm
    declareProperty("LowerLambda_PrPiMassCut", m_lowerLambda_PrPiMassCut = 1040.);           // optimal = 1080.0 MeV
    declareProperty("UpperLambda_PrPiMassCut", m_upperLambda_PrPiMassCut = 1220.);       // default = 2 500.0 MeV
    declareProperty("LowerLb_LambdaMuMuMassCut", m_lowerLb_LambdaMuMuMassCut = 5090.);    // default = 1 000.0 MeV
    declareProperty("UpperLb_LambdaMuMuMassCut", m_upperLb_LambdaMuMuMassCut = 6270.);    // default = 10 000.0 MeV
    //   declareProperty("LowerLb_LambdaMuMuMassCutVtxOff", m_lowerLb_LambdaMuMuMassCutVtxOff = 1080.); // default = 2 000.0 MeV
    //   declareProperty("UpperLb_LambdaMuMuMassCutVtxOff", m_upperLb_LambdaMuMuMassCutVtxOff = 1180.); // default = 8 000.0 MeV
    //   declareProperty("LowerLambdaVtxMassCut", m_lowerLambdaVtxMassCut = 1070.);            // default = 200.0 MeV
    //   declareProperty("UpperLambdaVtxMassCut", m_upperLambdaVtxMassCut = 1160.);            // default = 2 000.0 MeV
    declareProperty("DoLambda_PPiVertexing", m_doLambda_PPiVertexing = true);
    declareProperty("DoLb_LambdaMuMuVertexing", m_doLb_LambdaMuMuVertexing = true);
    declareProperty("LambdaVtxChi2Cut", m_lambdaVtxChi2Cut = 100.);                      // default = 500.0 MeV
    //   declareProperty("LowerLbVtxMassCut", m_lowerLbVtxMassCut = 4800.);                    // default = 1 000.0 MeV
    //   declareProperty("UpperLbVtxMassCut", m_upperLbVtxMassCut = 6100.);                    // default = 10 000.0 MeV
    declareProperty("LbVtxChi2Cut", m_lBVtxChi2Cut = 100.);                              // default = 500.0
    //   declareProperty("LbVtxDistanceCut", m_lBVtxDistanceCut = 0.);                      // default = 0.0
    //   declareProperty("PiImpactCut", m_piImpactCut = 0.);                                // default = 0.0
    //   declareProperty("PrImpactCut", m_prImpactCut = 0.);                                // default = 0.0
    declareProperty("MaxLbToStore", m_maxLbToStore = -1);
    
    // Bc->DsMuMu cuts
    declareProperty("DoBc_DsMuMuDecay", m_doBc_DsMuMuDecay = true);             // Proceed Bc->DsMuMu part of algorithm
    declareProperty("LowerPhiDs_MassCut", m_lowerPhiDs_MassCut  = 980.);        // default =  980.0 MeV
    declareProperty("UpperPhiDs_MassCut", m_upperPhiDs_MassCut = 1080.);        // default = 1080.0 MeV
    declareProperty("LowerDs_MassCut", m_lowerDs_MassCut = 1600.);              // default = 1600.0 MeV
    declareProperty("UpperDs_MassCut", m_upperDs_MassCut = 2400.);              // default = 2400.0 MeV
    declareProperty("LowerBc_DsMuMuMassCut", m_lowerBc_DsMuMuMassCut = 1800.);  // default = 1800.0 MeV
    declareProperty("UpperBc_DsMuMuMassCut", m_upperBc_DsMuMuMassCut = 7050.);  // default = 7050.0 MeV
    declareProperty("DoDs_Vertexing", m_doDs_Vertexing = true);
    declareProperty("DoBc_DsMuMuVertexing", m_doBc_DsMuMuVertexing = true);
    declareProperty("DsVtxChi2Cut", m_DsVtxChi2Cut =  90.);                    // default =  90.0
    declareProperty("BcVtxChi2Cut", m_bCVtxChi2Cut = 120.);                    // default = 120.0
    declareProperty("MaxBcToStore", m_maxBcToStore = -1);

    // FTK Flag
    declareProperty("DoFTK",    m_FTK=false); // Are we using FTK??

    
    // Monitoring variables
    //   General
    declareMonitoredStdContainer("Errors",     mon_Errors,     AutoClear);
    declareMonitoredStdContainer("Acceptance", mon_Acceptance, AutoClear);
    declareMonitoredVariable("nTriedCombinations", mon_nTriedCombinations );
    //   Timing
    declareMonitoredVariable("TotalRunTime",  mon_TotalRunTime);
    declareMonitoredVariable("VertexingTime", mon_VertexingTime);
    //   RoIs
    declareMonitoredStdContainer("RoI_RoI1Eta", mon_RoI_RoI1Eta, AutoClear);
    declareMonitoredStdContainer("RoI_RoI2Eta", mon_RoI_RoI2Eta, AutoClear);
    declareMonitoredStdContainer("RoI_RoI1Phi", mon_RoI_RoI1Phi, AutoClear);
    declareMonitoredStdContainer("RoI_RoI2Phi", mon_RoI_RoI2Phi, AutoClear);
    declareMonitoredStdContainer("RoI_dEtaRoI", mon_RoI_dEtaRoI, AutoClear);
    declareMonitoredStdContainer("RoI_dPhiRoI", mon_RoI_dPhiRoI, AutoClear);
    //   DiMuon
    declareMonitoredVariable("DiMu_n", mon_DiMu_n);
    declareMonitoredStdContainer("DiMu_Pt_Mu1",       mon_DiMu_Pt_Mu1,       AutoClear);
    declareMonitoredStdContainer("DiMu_Pt_Mu2",       mon_DiMu_Pt_Mu2,       AutoClear);
    declareMonitoredStdContainer("DiMu_Eta_Mu1",      mon_DiMu_Eta_Mu1,      AutoClear);
    declareMonitoredStdContainer("DiMu_Eta_Mu2",      mon_DiMu_Eta_Mu2,      AutoClear);
    declareMonitoredStdContainer("DiMu_Phi_Mu1",      mon_DiMu_Phi_Mu1,      AutoClear);
    declareMonitoredStdContainer("DiMu_Phi_Mu2",      mon_DiMu_Phi_Mu2,      AutoClear);
    declareMonitoredStdContainer("DiMu_dEtaMuMu",     mon_DiMu_dEtaMuMu,     AutoClear);
    declareMonitoredStdContainer("DiMu_dPhiMuMu",     mon_DiMu_dPhiMuMu,     AutoClear);
    declareMonitoredStdContainer("DiMu_pTsumMuMu",    mon_DiMu_pTsumMuMu,    AutoClear);
    declareMonitoredStdContainer("DiMu_InvMassMuMu",  mon_DiMu_InvMassMuMu, AutoClear);
    declareMonitoredStdContainer("DiMu_VtxMassMuMu",  mon_DiMu_VtxMassMuMu, AutoClear);
    declareMonitoredStdContainer("DiMu_Chi2MuMu",     mon_DiMu_Chi2MuMu,    AutoClear);
    //   Tracks
    declareMonitoredVariable("Tracks_n", mon_Tracks_n);
    declareMonitoredStdContainer("Tracks_Eta", mon_Tracks_Eta, AutoClear);
    declareMonitoredStdContainer("Tracks_Pt",  mon_Tracks_Pt,  AutoClear);
    declareMonitoredStdContainer("Tracks_Phi", mon_Tracks_Phi, AutoClear);
    //   B+
    declareMonitoredVariable("BMuMuK_n", mon_BMuMuK_n);
    declareMonitoredStdContainer("BMuMuK_Pt_K",      mon_BMuMuK_Pt_K,      AutoClear);
    declareMonitoredStdContainer("BMuMuK_Eta_K",     mon_BMuMuK_Eta_K,     AutoClear);
    declareMonitoredStdContainer("BMuMuK_Phi_K",     mon_BMuMuK_Phi_K,     AutoClear);
    declareMonitoredStdContainer("BMuMuK_InvMass_B", mon_BMuMuK_InvMass_B, AutoClear);
    declareMonitoredStdContainer("BMuMuK_VtxMass_B", mon_BMuMuK_VtxMass_B, AutoClear);
    declareMonitoredStdContainer("BMuMuK_Chi2_B",    mon_BMuMuK_Chi2_B,    AutoClear);
    //   Bd
    declareMonitoredVariable("BdMuMuKs_n", mon_BdMuMuKs_n);
    declareMonitoredStdContainer("BdMuMuKs_Pt_K",          mon_BdMuMuKs_Pt_K,          AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_Eta_K",         mon_BdMuMuKs_Eta_K,         AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_Phi_K",         mon_BdMuMuKs_Phi_K,         AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_Pt_Pi",         mon_BdMuMuKs_Pt_Pi,         AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_Eta_Pi",        mon_BdMuMuKs_Eta_Pi,        AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_Phi_Pi",        mon_BdMuMuKs_Phi_Pi,        AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_InvMass_Kstar", mon_BdMuMuKs_InvMass_Kstar, AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_VtxMass_Kstar", mon_BdMuMuKs_VtxMass_Kstar, AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_Chi2_Kstar",    mon_BdMuMuKs_Chi2_Kstar,    AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_InvMass_Bd",    mon_BdMuMuKs_InvMass_Bd,    AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_VtxMass_Bd",    mon_BdMuMuKs_VtxMass_Bd,    AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_Chi2_Bd",       mon_BdMuMuKs_Chi2_Bd,       AutoClear);
    //   Bs
    declareMonitoredVariable("BsMuMuPhi_n", mon_BsMuMuPhi_n);
    declareMonitoredStdContainer("BsMuMuPhi_Pt_K1",           mon_BsMuMuPhi_Pt_K1,           AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_Eta_K1",          mon_BsMuMuPhi_Eta_K1,          AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_Phi_K1",          mon_BsMuMuPhi_Phi_K1,          AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_Pt_K2",           mon_BsMuMuPhi_Pt_K2,           AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_Eta_K2",          mon_BsMuMuPhi_Eta_K2,          AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_Phi_K2",          mon_BsMuMuPhi_Phi_K2,          AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_InvMass_Phi1020", mon_BsMuMuPhi_InvMass_Phi1020, AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_VtxMass_Phi1020", mon_BsMuMuPhi_VtxMass_Phi1020, AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_Chi2_Phi1020",    mon_BsMuMuPhi_Chi2_Phi1020,    AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_InvMass_Bs",      mon_BsMuMuPhi_InvMass_Bs,      AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_VtxMass_Bs",      mon_BsMuMuPhi_VtxMass_Bs,      AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_Chi2_Bs",         mon_BsMuMuPhi_Chi2_Bs,         AutoClear);
    //   Lambda_b
    declareMonitoredVariable("LbMuMuLambda_n", mon_LbMuMuLambda_n);
    declareMonitoredStdContainer("LbMuMuLambda_Pt_P",           mon_LbMuMuLambda_Pt_P,           AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_Eta_P",          mon_LbMuMuLambda_Eta_P,          AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_Phi_P",          mon_LbMuMuLambda_Phi_P,          AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_Pt_Pi",          mon_LbMuMuLambda_Pt_Pi,          AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_Eta_Pi",         mon_LbMuMuLambda_Eta_Pi,         AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_Phi_Pi",         mon_LbMuMuLambda_Phi_Pi,         AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_InvMass_Lambda", mon_LbMuMuLambda_InvMass_Lambda, AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_VtxMass_Lambda", mon_LbMuMuLambda_VtxMass_Lambda, AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_Chi2_Lambda",    mon_LbMuMuLambda_Chi2_Lambda,    AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_InvMass_Lb",     mon_LbMuMuLambda_InvMass_Lb,     AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_VtxMass_Lb",     mon_LbMuMuLambda_VtxMass_Lb,     AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_Chi2_Lb",        mon_LbMuMuLambda_Chi2_Lb,        AutoClear);
    //   Bc
    declareMonitoredVariable("BcMuMuDs_n", mon_BcMuMuDs_n);
    declareMonitoredStdContainer("BcMuMuDs_Pt_K1",           mon_BcMuMuDs_Pt_K1,           AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Eta_K1",          mon_BcMuMuDs_Eta_K1,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Phi_K1",          mon_BcMuMuDs_Phi_K1,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Pt_K2",           mon_BcMuMuDs_Pt_K2,           AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Eta_K2",          mon_BcMuMuDs_Eta_K2,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Phi_K2",          mon_BcMuMuDs_Phi_K2,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Pt_pi",           mon_BcMuMuDs_Pt_pi,           AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Eta_pi",          mon_BcMuMuDs_Eta_pi,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Phi_pi",          mon_BcMuMuDs_Phi_pi,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_InvMass_PhiDs",   mon_BcMuMuDs_InvMass_PhiDs,   AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_InvMass_Ds",      mon_BcMuMuDs_InvMass_Ds,      AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_VtxMass_Ds",      mon_BcMuMuDs_VtxMass_Ds,      AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Chi2_Ds",         mon_BcMuMuDs_Chi2_Ds,         AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_InvMass_Bc",      mon_BcMuMuDs_InvMass_Bc,      AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_VtxMass_Bc",      mon_BcMuMuDs_VtxMass_Bc,      AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Chi2_Bc",         mon_BcMuMuDs_Chi2_Bc,         AutoClear);
    
    
}


/*----------------------------------------------------------------------------*/
TrigEFBMuMuXFex::~TrigEFBMuMuXFex()
{ }


/*----------------------------------------------------------------------------*/
HLT::ErrorCode TrigEFBMuMuXFex::hltInitialize()
{
    msg() << MSG::INFO << "Running TrigEFBMuMuXFex::hltInitialize" << endreq;
    
    if(msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "Initialization completed successfully:" << endreq;
        msg() << MSG::DEBUG << "AcceptAll          = "
        << (m_acceptAll==true ? "True" : "False") << endreq;
        msg() << MSG::DEBUG << "DoFTK         = "
        << (m_FTK==true ? "True" : "False") << endreq;
        msg() << MSG::INFO << "MaxNcombinations            = " << m_maxNcombinations << endreq;
        
        msg() << MSG::DEBUG << "Activated decays:" << endreq;
        msg() << MSG::DEBUG << "    B+ -> mu mu K+ : " << (m_doB_KMuMuDecay==true ? "True" : "False") << endreq;
        msg() << MSG::DEBUG << "    Bd -> mu mu K*(K+ Pi-) : " << (m_doBd_KstarMuMuDecay==true ? "True" : "False") << endreq;
        msg() << MSG::DEBUG << "    Bs -> mu mu Phi(K+ K-) : " << (m_doBs_Phi1020MuMuDecay==true ? "True" : "False") << endreq;
        msg() << MSG::DEBUG << "    Lambda_b -> mu mu Lambda(P Pi) : " << (m_doLb_LambdaMuMuDecay==true ? "True" : "False") << endreq;
        msg() << MSG::DEBUG << "    Bc -> mu mu Ds(Phi pi) : " << (m_doBc_DsMuMuDecay==true ? "True" : "False") << endreq;
        
        msg() << MSG::DEBUG << "OppositeCharge     = "        << (m_oppositeCharge==true ? "True" : "False") << endreq;
        
    }
    
    // setting up timers
    if ( timerSvc() ) {
        m_TotTimer    = addTimer("EFBMuMuXFexTot");
        m_VtxFitTimer = addTimer("EFBMuMuXFexVtxFit");
    }
    
    // retrieving the vertex fitting tool
    if (m_fitterSvc.retrieve().isFailure()) {
        msg() << MSG::ERROR << "Can't find Trk::TrkVKalVrtFitter" << endreq;
        return StatusCode::SUCCESS;
    } else {
        if (msgLvl() <= MSG::DEBUG) {
            msg() << MSG::DEBUG << "Trk::TrkVKalVrtFitter found" << endreq;
        }
        m_VKVFitter = dynamic_cast< Trk::TrkVKalVrtFitter* > (&(*m_fitterSvc));
    }
    
    // retrieving BphysHelperUtilsTool
    if (m_bphysHelperTool.retrieve().isFailure()) {
        msg() << MSG::ERROR << "Can't find TrigBphysHelperUtilsTool" << endreq;
        return StatusCode::SUCCESS;
    } else {
            if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "TrigBphysHelperUtilsTool found" << endreq;
    }
    
    // set counters
    m_lastEvent=-1;
    m_lastEventPassed=-1;
    m_countTotalEvents=0;
    m_countTotalRoI=0;
    m_countPassedEvents=0;
    m_countPassedRoIs=0;
    
    m_lastEventPassedBplus=-1;
    m_lastEventPassedBd=-1;
    m_lastEventPassedBs=-1;
    m_lastEventPassedLb=-1;
    
    m_countPassedEventsBplus=0;
    m_countPassedEventsBs=0;
    m_countPassedEventsBd=0;
    m_countPassedEventsLb=0;
    m_countPassedEventsBc=0;
    
    m_countPassedMuMuID=0;
    m_countPassedMuMuOS=0;
    m_countPassedMuMuMass=0;
    m_countPassedMuMuVtx=0;
    m_countPassedMuMuVtxChi2=0;
    
    m_countPassedBplusMass=0;
    m_countPassedBplusVtx=0;
    m_countPassedBplusVtxChi2=0;
    
    m_countPassedKstarMass=0;
    m_countPassedBdMass=0;
    m_countPassedKstarVtx=0;
    m_countPassedKstarVtxChi2=0;
    m_countPassedBdVtx=0;
    m_countPassedBdVtxChi2=0;
    
    m_countPassedPhi1020Mass=0;
    m_countPassedBsMass=0;
    m_countPassedPhi1020Vtx=0;
    m_countPassedPhi1020VtxChi2=0;
    m_countPassedBsVtx=0;
    m_countPassedBsVtxChi2=0;
    
    m_countPassedLambdaMass=0;
    m_countPassedLbMass=0;
    m_countPassedLambdaVtx=0;
    m_countPassedLambdaVtxChi2=0;
    m_countPassedLbVtx=0;
    m_countPassedLbVtxChi2=0;
    
    m_countPassedPhiDsMass=0;
    m_countPassedDsMass=0;
    m_countPassedBcMass=0;
    m_countPassedDsVtx=0;
    m_countPassedDsVtxChi2=0;
    m_countPassedBcVtx=0;
    m_countPassedBcVtxChi2=0;
    
    
    return HLT::OK;
}


/*----------------------------------------------------------------------------*/
HLT::ErrorCode TrigEFBMuMuXFex::hltFinalize()
{
    
    msg() << MSG::INFO << "Running TrigEFBMuMuXFex::hltFinalize" << endreq;
    
    msg() << MSG::INFO << "|----------------------- SUMMARY FROM TrigEFBMuMuXFex -------------|" << endreq;
    msg() << MSG::INFO << "Run on events/RoIs      " << m_countTotalEvents << "/" << m_countTotalRoI <<  endreq;
    msg() << MSG::INFO << "Passed events/RoIs      " << m_countPassedEvents << "/" << m_countPassedRoIs <<  endreq;
    msg() << MSG::INFO << "Evts Passed B+:         " << m_countPassedEventsBplus << endreq;
    msg() << MSG::INFO << "Evts Passed Bd:         " << m_countPassedEventsBd << endreq;
    msg() << MSG::INFO << "Evts Passed Bs:         " << m_countPassedEventsBs << endreq;
    msg() << MSG::INFO << "Evts Passed Lambda_b:   " << m_countPassedEventsLb << endreq;
    msg() << MSG::INFO << "Evts Passed Bc:         " << m_countPassedEventsBc << endreq;
    msg() << MSG::INFO << std::endl << endreq;
    msg() << MSG::INFO << "PassedMuMuID:           " << m_countPassedMuMuID << endreq;
    msg() << MSG::INFO << "PassedMuMuOS:           " << m_countPassedMuMuOS << endreq;
    msg() << MSG::INFO << "PassedMuMuMass:         " << m_countPassedMuMuMass << endreq;
    msg() << MSG::INFO << "PassedMuMuVtx:          " << m_countPassedMuMuVtx << endreq;
    msg() << MSG::INFO << "PassedMuMuVtxChi2:      " << m_countPassedMuMuVtxChi2 << endreq;
    msg() << MSG::INFO << "PassedBplusMass:        " << m_countPassedBplusMass << endreq;
    msg() << MSG::INFO << "PassedBplusVtx:         " << m_countPassedBplusVtx << endreq;
    msg() << MSG::INFO << "PassedBplusVtxChi2:     " << m_countPassedBplusVtxChi2 << endreq;
    msg() << MSG::INFO << "PassedKstarMass:        " << m_countPassedKstarMass << endreq;
    msg() << MSG::INFO << "PassedKstarVtx:         " << m_countPassedKstarVtx << endreq;
    msg() << MSG::INFO << "PassedKstarVtxChi2:     " << m_countPassedKstarVtxChi2 << endreq;
    msg() << MSG::INFO << "PassedBdMass:           " << m_countPassedBdMass << endreq;
    msg() << MSG::INFO << "PassedBdVtx:            " << m_countPassedBdVtx << endreq;
    msg() << MSG::INFO << "PassedBdVtxChi2:        " << m_countPassedBdVtxChi2 << endreq;
    msg() << MSG::INFO << "PassedPhi1020Mass:      " << m_countPassedPhi1020Mass << endreq;
    msg() << MSG::INFO << "PassedPhi1020Vtx:       " << m_countPassedPhi1020Vtx << endreq;
    msg() << MSG::INFO << "PassedPhi1020VtxChi2:   " << m_countPassedPhi1020VtxChi2 << endreq;
    msg() << MSG::INFO << "PassedBsMass:           " << m_countPassedBsMass << endreq;
    msg() << MSG::INFO << "PassedBsVtx:            " << m_countPassedBsVtx << endreq;
    msg() << MSG::INFO << "PassedBsVtxChi2:        " << m_countPassedBsVtxChi2 << endreq;
    msg() << MSG::INFO << "PassedLambdaMass:       " << m_countPassedLambdaMass << endreq;
    msg() << MSG::INFO << "PassedLambdaVtx:        " << m_countPassedLambdaVtx << endreq;
    msg() << MSG::INFO << "PassedLambdaVtxChi2:    " << m_countPassedLambdaVtxChi2 << endreq;
    msg() << MSG::INFO << "PassedLbMass:           " << m_countPassedLbMass << endreq;
    msg() << MSG::INFO << "PassedLbVtx:            " << m_countPassedLbVtx << endreq;
    msg() << MSG::INFO << "PassedLbVtxChi2:        " << m_countPassedLbVtxChi2 << endreq;
    msg() << MSG::INFO << "PassedPhiDsMass:        " << m_countPassedPhiDsMass << endreq;
    msg() << MSG::INFO << "PassedDsMass:           " << m_countPassedDsMass << endreq;
    msg() << MSG::INFO << "PassedDsVtx:            " << m_countPassedDsVtx << endreq;
    msg() << MSG::INFO << "PassedDsVtxChi2:        " << m_countPassedDsVtxChi2 << endreq;
    msg() << MSG::INFO << "PassedBcMass:           " << m_countPassedBcMass << endreq;
    msg() << MSG::INFO << "PassedBcVtx:            " << m_countPassedBcVtx << endreq;
    msg() << MSG::INFO << "PassedBcVtxChi2:        " << m_countPassedBcVtxChi2 << endreq;
    
    return HLT::OK;
}


/*----------------------------------------------------------------------------*/
HLT::ErrorCode TrigEFBMuMuXFex::acceptInputs(HLT::TEConstVec& , bool& pass)
{
    if ( msgLvl() <= MSG::DEBUG )
        msg() << MSG::DEBUG << "Running TrigEFBMuMuXFex::acceptInputs" << endreq;
    
    pass = true;
    
    return HLT::OK;
}


/*----------------------------------------------------------------------------*/
HLT::ErrorCode TrigEFBMuMuXFex::hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement*  outputTE )
{
    if ( msgLvl() <= MSG::DEBUG )
        msg() << MSG::DEBUG << "Running TrigEFBMuMuXFex::hltExecute" << endreq;
    
    if ( timerSvc() ) m_TotTimer->start();
    
    if ( timerSvc() ) {
        m_VtxFitTimer->start();
        m_VtxFitTimer->pause();
    }
    
    bool result(false);
    
    // Set monitoring counters to zeroes
    mon_nTriedCombinations = 0;
    mon_DiMu_n         = 0;
    mon_Tracks_n       = 0;
    mon_BMuMuK_n       = 0;
    mon_BdMuMuKs_n     = 0;
    mon_BsMuMuPhi_n    = 0;
    mon_LbMuMuLambda_n = 0;
    mon_BcMuMuDs_n     = 0;
    mon_TotalRunTime   = 0.;
    mon_VertexingTime  = 0.;
    
    if(msgLvl() <= MSG::VERBOSE) {
        if (m_acceptAll) {
            msg() << MSG::VERBOSE << "AcceptAll property is set: taking all events" << endreq;
            result = true;
        } else {
            msg() << MSG::VERBOSE << "AcceptAll property not set: applying selection" << endreq;
        }
    }
    
    // Retrieve event info
    int IdRun   = 0;
    int IdEvent = 0;
    
    // JW - Try to get the xAOD event info
    const EventInfo* pEventInfo(0);
    const xAOD::EventInfo *evtInfo(0);
    if ( store()->retrieve(evtInfo).isFailure() ) {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Failed to get xAOD::EventInfo " << endreq;
        // now try the old event ifo
        if ( store()->retrieve(pEventInfo).isFailure() ) {
            if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Failed to get EventInfo " << endreq;
            mon_Errors.push_back( ERROR_No_EventInfo );
        } else {
            IdRun   = pEventInfo->event_ID()->run_number();
            IdEvent = pEventInfo->event_ID()->event_number();
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << IdRun << " Event " << IdEvent << " using algo " << "m_muonAlgo"<<  endreq;
        }// found old event info
    }else { // found the xAOD event info
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << evtInfo->runNumber()
            << " Event " << evtInfo->eventNumber() << " using algo m_muonAlgo" << endreq;
        IdRun   = evtInfo->runNumber();
        IdEvent = evtInfo->eventNumber();
    } // get event info
    
    
    //Check that we got 2 input TEs
    int mu1_TE=-1;
    int mu2_TE=-1;
    if (!m_FTK) {
      if ( inputTE.size() != 2 ) {
        msg() << MSG::ERROR << "Got different than 2 number of input TEs: " << inputTE.size() << endreq;
        mon_Errors.push_back(ERROR_Not_2_InputTEs);
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::BAD_JOB_SETUP;
      }
      mu1_TE=0;
      mu2_TE=1;
    } else {
      if ( inputTE.size() != 3 ) {
        msg() << MSG::ERROR << "FTK mode expect 3 input TEs, got : " << inputTE.size() << endreq;
        mon_Errors.push_back(ERROR_Not_2_InputTEs);
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::BAD_JOB_SETUP;
      }
      mu1_TE=1;
      mu2_TE=2;
    }


    if(IdEvent!=m_lastEvent) {
        m_countTotalEvents++;
        m_lastEvent = IdEvent;
        m_countBpToStore = 0;
        m_countBdToStore = 0;
        m_countBsToStore = 0;
        m_countLbToStore = 0;
        m_countBcToStore = 0;
    }
    m_countTotalRoI++;
    
    
    // JK DEBUG input TEs

    for (unsigned int iTE=0; iTE<inputTE.size(); ++iTE) {
      msg() << MSG::DEBUG << "Input TE " << iTE << "  " << inputTE[iTE] << " ID " << inputTE[iTE]->getId() << endreq;
    }


    //Retrieve ROIs
    const TrigRoiDescriptor *roiDescriptor1(0);
    const TrigRoiDescriptor *roiDescriptor2(0);
    const TrigRoiDescriptor *roiDescriptorTrk(0); // for FTK chain
    
    // get them from the navigation

     if ( getFeature(inputTE[mu1_TE], roiDescriptor1) != HLT::OK ) {
        msg() << MSG::ERROR << "Navigation error while getting RoI descriptor 1" << endreq;
        mon_Errors.push_back(ERROR_No_RoIs);
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::NAV_ERROR;
     }
     if ( !roiDescriptor1 ) {
       msg() << MSG::ERROR << "roiDescriptor1 is NULL" << endreq;
       return HLT::NAV_ERROR;
     }
    
     if ( getFeature(inputTE[mu2_TE], roiDescriptor2) != HLT::OK ) {
        msg() << MSG::ERROR << "Navigation error while getting RoI descriptor 2" << endreq;
        mon_Errors.push_back(ERROR_No_RoIs);
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::NAV_ERROR;
     }
     if ( !roiDescriptor2 ) {
       msg() << MSG::ERROR << "roiDescriptor2 is NULL" << endreq;
       return HLT::NAV_ERROR;
     }

     if (m_FTK) {
      if ( getFeature(inputTE[0], roiDescriptorTrk) != HLT::OK ) {
        msg() << MSG::ERROR << "Navigation error while getting RoI descriptor Trk" << endreq;
        mon_Errors.push_back(ERROR_No_RoIs);
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::NAV_ERROR;
      }
     }


    if ( msgLvl() <= MSG::DEBUG ){
        msg() << MSG::DEBUG
        << "Using inputTEs: "<< inputTE[mu1_TE] <<  " and "  << inputTE[mu2_TE] << " with Ids " << inputTE[mu1_TE]->getId()<< " AND "<< inputTE[mu2_TE]->getId() << std::endl
        << "; RoI IDs = "   << roiDescriptor1->roiId()<< " AND   " <<roiDescriptor2->roiId() << std::endl
        << ": Eta1 =    "   << roiDescriptor1->eta() << " Eta2= " <<roiDescriptor2->eta() << std::endl
        << ", Phi1 =    "   << roiDescriptor1->phi() << " Phi2= " <<roiDescriptor2->phi()
        << endreq;
	if (m_FTK) {
	  msg() << MSG::DEBUG << "Using inputTE for tracks: "<< inputTE[0] << " " << inputTE[0]->getId()<< std::endl
        << "; RoI IDs = "   << roiDescriptorTrk->roiId() << std::endl
        << ": EtaTrk =    "   << roiDescriptorTrk->eta() << std::endl
        << ", PhiTrk =    "   << roiDescriptorTrk->phi() 
        << endreq;
	}
    }
    
    // Fill RoIs monitoring containers
    mon_RoI_RoI1Eta.push_back(roiDescriptor1->eta());
    mon_RoI_RoI1Phi.push_back(roiDescriptor1->phi());
    mon_RoI_RoI2Eta.push_back(roiDescriptor2->eta());
    mon_RoI_RoI2Phi.push_back(roiDescriptor2->phi());
    mon_RoI_dEtaRoI.push_back( fabs(roiDescriptor1->eta() - roiDescriptor2->eta()) );
    float tmp_RoI_dPhiRoI = roiDescriptor1->phi() - roiDescriptor2->phi();
    while (tmp_RoI_dPhiRoI >  M_PI) tmp_RoI_dPhiRoI -= 2*M_PI;
    while (tmp_RoI_dPhiRoI < -M_PI) tmp_RoI_dPhiRoI += 2*M_PI;
    mon_RoI_dPhiRoI.push_back( fabs(tmp_RoI_dPhiRoI) );
    
    // Retrieve muons
    //std::vector<const Trk::Track*> muidIDtracks1;
    //std::vector<const Trk::Track*> muidIDtracks2;
    //JW
    std::vector<ElementLink<xAOD::TrackParticleContainer> > muonTPELtracks1;
    std::vector<ElementLink<xAOD::TrackParticleContainer> > muonTPELtracks2;
    std::vector<ElementLink<xAOD::TrackParticleContainer> > muonTPELtracksMerged; // combined unique tracks from roi 1 and 2

    //   const DataHandle<MuidTrackContainer> MuidTracksEF1, MuidTracksEF2;
    //const MuidTrackContainer* MuEFTracksEF1;
    //const MuidTrackContainer* MuEFTracksEF2;
    
    // Get the muon container from the outputTE
    //std::vector<const xAOD::MuonContainer*> muonContainerEF1;
    ElementLinkVector<xAOD::MuonContainer> muonContainerEF1;
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Try to retrieve EFInfo container of muon 1" << endreq;
    //if(getFeatures(inputTE[mu1_TE], muonContainerEF1)!=HLT::OK ) {
    if(getFeaturesLinks<xAOD::MuonContainer,xAOD::MuonContainer>(inputTE[mu1_TE], muonContainerEF1)!=HLT::OK ) {
        if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Failed to get EFInfo feature of muon 1, exiting" << endreq;
        mon_Errors.push_back(ERROR_No_MuonEFInfoContainer);
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::OK; // FIXME should be HLT::MISSING_FEATURE ??
    }
    if(msgLvl() <= MSG::DEBUG) ATH_MSG_DEBUG("Found MuonContainer, Got MuonEF (1) Feature, size = " << muonContainerEF1.size());
    
    for ( const auto muel : muonContainerEF1 ) {
      if ( (*muel)->muonType() != xAOD::Muon::Combined && (*muel)->muonType() != xAOD::Muon::SegmentTagged) {
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Muon from roi1 is neither Combined or SegmentTagged - reject" << endreq;
        continue;
      }
      const xAOD::Muon* mu = *muel;
      if(!mu) continue;
      const xAOD::TrackParticle* idtp(0);
        const ElementLink<xAOD::TrackParticleContainer> & idtpEl = mu->inDetTrackParticleLink();
      if( idtpEl.isValid() ) idtp = *idtpEl;
      if (!idtp) {
          if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "No innerdetector muon1 TrackParticle found" << endreq;
          continue;
      }
        //const Trk::Track* indetTrack = idtp->track();
        //if ( !indetTrack ) {
        //  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "No id muon1 id Trk::Track found" << endreq;
        //  continue;
        //}
        //addUnique(muidIDtracks1, indetTrack);
        addUnique(muonTPELtracks1,idtpEl);
        addUnique(muonTPELtracksMerged,idtpEl);
        
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG
            << " Comb muon 1 pt/eta/phi " << idtp->pt()
            << " / " << idtp->eta() << " / " << idtp->phi()
            << endreq;

        
//         std::vector<const Trk::Track*> idTrks;
//         HLT::ErrorCode status = GetxAODMuonTracks(muonContainerEF1[i_mu], idTrks, msg());
//         if ( status != HLT::OK ) {
//             if ( timerSvc() ) m_TotTimer->stop();
//             return status;
//         } // if bad
//         for (unsigned int i_trk=0; i_trk<idTrks.size(); i_trk++ ) {
//             addUnique(muidIDtracks1, idTrks[i_trk]);
//             const Trk::Perigee* perigee = idTrks[i_trk]->perigeeParameters();
//             if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG
//                 << " Comb muon 1 pt/eta/phi " << perigee->pT()
//                 << " / " << perigee->eta() << " / " << perigee->parameters()[Trk::phi]
//                 << endreq;
//         } // loop over any tracks
    } // for
    
    
    //  std::vector<const TrigMuonEFInfoContainer*> MuEFTracksEF1;
    //  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Try to retrieve EFInfo container of muon 1" << endreq;
    //  HLT::ErrorCode status = getFeatures(inputTE.front(), MuEFTracksEF1);
    //  if ( status != HLT::OK ) {
    //    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Failed to get EFInfo feature of muon 1, exiting" << endreq;
    //    mon_Errors.push_back(ERROR_No_MuonEFInfoContainer);
    //    if ( timerSvc() ) m_TotTimer->stop();
    //    return HLT::OK;
    //  }
    //  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Got MuonEF (1) Feature, size = " << MuEFTracksEF1.size() << endreq;
    //  for ( unsigned int i_mu=0; i_mu<MuEFTracksEF1.size(); i_mu++ ) {
    //    std::vector<const Trk::Track*> idTrks;
    //    status = GetTrigMuonEFInfoTracks(MuEFTracksEF1[i_mu], idTrks, msg());
    //    if ( status != HLT::OK ) {
    //      if ( timerSvc() ) m_TotTimer->stop();
    //      return status;
    //    }
    //    for ( unsigned int i_trk=0; i_trk<idTrks.size(); i_trk++ ) {
    //      addUnique(muidIDtracks1, idTrks[i_trk]);
    //      const Trk::Perigee* perigee = idTrks[i_trk]->perigeeParameters();
    //      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG
    //                                          << " Comb muon 1 " << idTrks[i_trk] << " pt/eta/phi " << perigee->pT()
    //                                          << " / " << perigee->eta() << " / " << perigee->parameters()[Trk::phi]
    //                                          << endreq;
    //    }
    //  }
    
    // second
    
    //std::vector<const xAOD::MuonContainer*> muonContainerEF2;
    ElementLinkVector<xAOD::MuonContainer> muonContainerEF2;
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Try to retrieve EFInfo container of muon 2" << endreq;
    //if(getFeatures(inputTE[mu2_TE], muonContainerEF2)!=HLT::OK ) {
    if(getFeaturesLinks<xAOD::MuonContainer,xAOD::MuonContainer>(inputTE[mu2_TE], muonContainerEF2)!=HLT::OK ) {
        if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Failed to get EFInfo feature of muon 2, exiting" << endreq;
        mon_Errors.push_back(ERROR_No_MuonEFInfoContainer);
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::OK; // FIXME - should be
        // return HLT::MISSING_FEATURE; // was HLT::OK
    }
    if(msgLvl() <= MSG::DEBUG) ATH_MSG_DEBUG("Found MuonContainer, Got MuonEF (2) Feature, size = " << muonContainerEF2.size());
    
    for ( const auto muel : muonContainerEF2 ) {
      if ( (*muel)->muonType() != xAOD::Muon::Combined && (*muel)->muonType() != xAOD::Muon::SegmentTagged) {
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Muon from roi2 is neither Combined or SegmentTagged - reject" << endreq;
        continue;
      }
      const xAOD::Muon* mu = *muel;
      if(!mu) continue;
      const xAOD::TrackParticle* idtp(0);
        const ElementLink<xAOD::TrackParticleContainer> & idtpEl = mu->inDetTrackParticleLink();
        if( idtpEl.isValid() ) idtp = *idtpEl;
      if (!idtp) {
          if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "No innerdetector muon2 TrackParticle found" << endreq;
          continue;
      }
        //      const Trk::Track* indetTrack = idtp->track();
        //      if ( !indetTrack ) {
        //          if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "No id muon2 id Trk::Track found" << endreq;
        //          continue;
        //      }
        //addUnique(muidIDtracks2, indetTrack);
        addUnique(muonTPELtracks2,idtpEl);
        addUnique(muonTPELtracksMerged,idtpEl);
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG
            << " Comb muon 1 pt/eta/phi " << idtp->pt()
            << " / " << idtp->eta() << " / " << idtp->phi()
            << endreq;

//      const Trk::Perigee* perigee = indetTrack->perigeeParameters();
//      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG
//          << " Comb muon 2 pt/eta/phi " << perigee->pT()
//          << " / " << perigee->eta() << " / " << perigee->parameters()[Trk::phi]
//          << endreq;
      
//         std::vector<const Trk::Track*> idTrks;
//         HLT::ErrorCode status = GetxAODMuonTracks(muonContainerEF2[i_mu], idTrks, msg());
//         if ( status != HLT::OK ) return status;
//         for (unsigned int i_trk=0; i_trk<idTrks.size(); i_trk++ ) {
//             addUnique(muidIDtracks2, idTrks[i_trk]);
//             const Trk::Perigee* perigee = idTrks[i_trk]->perigeeParameters();
//             if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG
//                 << " Comb muon 2 pt/eta/phi " << perigee->pT()
//                 << " / " << perigee->eta() << " / " << perigee->parameters()[Trk::phi]
//                 << endreq;
//         } // loop over any tracks
    } // for loop over muons
    
    // build a map of the tracks and corresponding muons
    std::map<const xAOD::TrackParticle*, ElementLink<xAOD::MuonContainer> > mapTrkToMuons;
    //for (auto& muCont: muonContainerEF1) {
        for (const auto mu : muonContainerEF1) {
            auto idtp  = (*mu)->inDetTrackParticleLink();
            if (!idtp.isValid()) continue;
            if (!*idtp) continue;
            //if (!(*idtp)->track()) continue;
            mapTrkToMuons[(*idtp)] = mu;
        } // muCont
    //} // muonContainerEF1
    //for (auto& muCont: muonContainerEF2) {
        for (const auto mu : muonContainerEF2) {
            auto idtp  = (*mu)->inDetTrackParticleLink();
            if (!idtp.isValid()) continue;
            if (!*idtp) continue;
            //if (!(*idtp)->track()) continue;
            mapTrkToMuons[(*idtp)] = mu;
        } // muCont
    //} // muonContainerEF2

    //    std::vector<const TrigMuonEFInfoContainer*> MuEFTracksEF2;
    //  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Try to retrieve EFInfo container of muon 2" << endreq;
    //  status = getFeatures(inputTE.back(), MuEFTracksEF2);
    //  if ( status != HLT::OK ) {
    //    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Failed to get EFInfo feature of muon 2, exiting" << endreq;
    //    mon_Errors.push_back(ERROR_No_MuonEFInfoContainer);
    //    if ( timerSvc() ) m_TotTimer->stop();
    //    return HLT::OK;
    //  }
    //  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Got MuonEF (2) Feature, size = " << MuEFTracksEF2.size() << endreq;
    //  for ( unsigned int i_mu=0; i_mu<MuEFTracksEF2.size(); i_mu++ ) {
    //    std::vector<const Trk::Track*> idTrks;
    //    HLT::ErrorCode status = GetTrigMuonEFInfoTracks(MuEFTracksEF2[i_mu], idTrks, msg());
    //    if ( status != HLT::OK ) {
    //      if ( timerSvc() ) m_TotTimer->stop();
    //      return status;
    //    }
    //    for ( unsigned int i_trk=0; i_trk<idTrks.size(); i_trk++ ) {
    //      addUnique(muidIDtracks2, idTrks[i_trk]);
    //      const Trk::Perigee* perigee = idTrks[i_trk]->perigeeParameters();
    //      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG
    //                                          << " Comb muon 2 " << idTrks[i_trk] << " pt/eta/phi " << perigee->pT()
    //                                          << " / " << perigee->eta() << " / " << perigee->parameters()[Trk::phi]
    //                                          << endreq;
    //    }
    //  }
    
    if(msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "muonContainerEF1.size()= " << muonContainerEF1.size()<<endreq;
        msg() << MSG::DEBUG << "muonContainerEF2.size()= " << muonContainerEF2.size()<<endreq;
        msg() << MSG::DEBUG << "muonTPELtracks1.size()= "  << muonTPELtracks1.size() <<endreq;
        msg() << MSG::DEBUG << "muonTPELtracks2.size()= "  << muonTPELtracks2.size() <<endreq;
        msg() << MSG::DEBUG << "muonTPELtracksMerged.size()= "  << muonTPELtracksMerged.size() <<endreq;
        
        for (auto muel: muonTPELtracks1) {
            msg() << MSG::DEBUG << "muonTPELtracks1: " << *muel << " " <<
            (*muel)->pt() << " , " << (*muel)->eta() << " , " << (*muel)->phi() << " , " << (*muel)->charge() << endreq;
        }
        for (auto muel: muonTPELtracks2) {
            msg() << MSG::DEBUG << "muonTPELtracks2: " << *muel << " " <<
            (*muel)->pt() << " , " << (*muel)->eta() << " , " << (*muel)->phi() << " , " << (*muel)->charge() << endreq;
        }
        for (auto muel: muonTPELtracksMerged) {
            msg() << MSG::DEBUG << "muonTPELtracksMerged: " << *muel << " " <<
            (*muel)->pt() << " , " << (*muel)->eta() << " , " << (*muel)->phi() << " , " << (*muel)->charge() << endreq;
        }
    } // if debug
    
    Amg::Vector3D vtx ( 0.,0.,0. );
    
    //   TrigEFBphys* trigPartBMuMuX (NULL);
    // FIXME - remove these 'new's
    //m_trigBphysColl_b = new TrigEFBphysContainer();
    //m_trigBphysColl_X = new TrigEFBphysContainer();
    
    mTrigBphysColl_b = new xAOD::TrigBphysContainer();
    xAOD::TrigBphysAuxContainer xAODTrigBphysAuxColl_b;
    mTrigBphysColl_b->setStore(&xAODTrigBphysAuxColl_b);
    
    mTrigBphysColl_X = new xAOD::TrigBphysContainer();
    xAOD::TrigBphysAuxContainer xAODTrigBphysAuxColl_X;
    mTrigBphysColl_X->setStore(&xAODTrigBphysAuxColl_X);
    
    // Tried combinations counter - protection against timeouts
    int nTriedCombinations(0);
    
    if( !(muonTPELtracks1.size()> 0 && muonTPELtracks2.size() > 0) )
    {
        if ( msgLvl() <= MSG::DEBUG )
            msg() << MSG::DEBUG << "No muon candidate found for one or both TEs" << endreq;
        mon_Errors.push_back(ERROR_No_MuonCandidate);
        //delete m_trigBphysColl_b;
        //delete m_trigBphysColl_X;
        delete mTrigBphysColl_b;
        delete mTrigBphysColl_X;
        mTrigBphysColl_b = 0;
        mTrigBphysColl_X = 0;
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::OK;
    } else {
        
        // Loop over muons
        //std::vector<ElementLink<xAOD::TrackParticleContainer> >::iterator pElItr=muonTPELtracks1.begin();
        //std::vector<ElementLink<xAOD::TrackParticleContainer> >::iterator mElItr=muonTPELtracks2.begin();
        std::vector<ElementLink<xAOD::TrackParticleContainer> >::iterator pElItr=muonTPELtracksMerged.begin();
        std::vector<ElementLink<xAOD::TrackParticleContainer> >::iterator mElItr=muonTPELtracksMerged.begin();

        //        for(pElItr=muonTPELtracks1.begin(); pElItr != muonTPELtracks1.end(); pElItr++) {
        //            for(mElItr=muonTPELtracks2.begin(); mElItr != muonTPELtracks2.end(); mElItr++) {
        
        for(pElItr=muonTPELtracksMerged.begin(); pElItr != muonTPELtracksMerged.end(); ++pElItr) {
            for(mElItr=pElItr+1; mElItr != muonTPELtracksMerged.end(); ++mElItr) {
                auto pTp = **pElItr;
                auto mTp = **mElItr;
                ATH_MSG(DEBUG) << "Try to build muon pair from mu1 " << *(*pElItr) << ", mu2 " << *(*mElItr) << endreq;
                
                // check that we have two different muon tracks
                if (pTp==mTp) {
                    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Muon tracks are same" << endreq;
                    mon_Errors.push_back(ERROR_SameMuon);
                    continue;
                }
                m_countPassedMuMuID++;
                
                // check if muons have opposite signs
                if( m_oppositeCharge && (mTp->charge())*(pTp->charge()) > 0) {
                    ATH_MSG(DEBUG) << "Muon pair rejected by opposite change check: mu1 " << pTp->charge() << ", mu2 " << mTp->charge() << endreq;
                    continue;
                }
                m_countPassedMuMuOS++;
                
                // check if dimuon mass within the range
                //float massMuMu = XMass(*pItr,*mItr,di_to_muons);
                float massMuMu = XMass(pTp,mTp,di_to_muons);
                if( massMuMu < m_lowerMuMuMassCut || massMuMu > m_upperMuMuMassCut ) {
                    ATH_MSG(DEBUG) << "Muon pair rejected by mass cut: m = " << massMuMu << endreq;
                    continue;
                }
                m_countPassedMuMuMass++;
                
                if ( timerSvc() ) m_VtxFitTimer->resume();
                //xAOD::TrigBphys * trigMuMuObj = new xAOD::TrigBphys;
                std::unique_ptr<xAOD::TrigBphys> trigMuMuUniquePtr(new xAOD::TrigBphys);
                trigMuMuUniquePtr->makePrivateStore();
                trigMuMuUniquePtr->initialise(0,0.,0.,0.,
                                              xAOD::TrigBphys::BMUMUX, massMuMu, xAOD::TrigBphys::EF);
                std::vector<ElementLink<xAOD::TrackParticleContainer> > vec_mumu_tracks = {*pElItr,*mElItr};
                std::vector<double> masses_mumu = {MUMASS,MUMASS};
                if (!m_bphysHelperTool->vertexFit(trigMuMuUniquePtr.get(),
                                                  vec_mumu_tracks,masses_mumu).isSuccess()) {
                    ATH_MSG(DEBUG) << " Vertex fitting failed." << endreq;
                    if ( timerSvc() ) m_VtxFitTimer->pause();
                    continue;
                }
                if ( timerSvc() ) m_VtxFitTimer->pause();
                
                
                m_countPassedMuMuVtx++;
                
                float chi2MuMu = trigMuMuUniquePtr->fitchi2();
                if(chi2MuMu > m_muVtxChi2Cut || chi2MuMu < 0) {
                    ATH_MSG(DEBUG) << "Muon pair rejected by chi2 cut: chi2 = " << chi2MuMu << endreq;
                    continue;
                }
                m_countPassedMuMuVtxChi2++;
                
                
                // Fill muons monitoring containers
                mon_DiMu_Pt_Mu1.push_back (pTp->pt()/1000.);
                mon_DiMu_Pt_Mu2.push_back (mTp->pt()/1000.);
                mon_DiMu_Eta_Mu1.push_back(pTp->eta());
                mon_DiMu_Eta_Mu2.push_back(mTp->eta());
                mon_DiMu_Phi_Mu1.push_back(pTp->phi());
                mon_DiMu_Phi_Mu2.push_back(mTp->phi());
                
                mon_DiMu_dEtaMuMu.push_back( fabs(pTp->eta() - mTp->eta()) );
                float tmp_DiMu_dPhiMuMu = roiDescriptor1->phi() - roiDescriptor2->phi();
                while (tmp_DiMu_dPhiMuMu >  M_PI) tmp_DiMu_dPhiMuMu -= 2*M_PI;
                while (tmp_DiMu_dPhiMuMu < -M_PI) tmp_DiMu_dPhiMuMu += 2*M_PI;
                mon_DiMu_dPhiMuMu.push_back( fabs(tmp_DiMu_dPhiMuMu) );
                mon_DiMu_pTsumMuMu.push_back( (pTp->pt() + mTp->pt())/1000. );
                mon_DiMu_InvMassMuMu.push_back(massMuMu/1000.);
                mon_DiMu_VtxMassMuMu.push_back(trigMuMuUniquePtr->fitmass()/1000.);
                mon_DiMu_Chi2MuMu.push_back(chi2MuMu);
                
                mon_DiMu_n++;
                
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Dimuon Sec Vtx at ("
                    << trigMuMuUniquePtr->fitx() << " , "
                    << trigMuMuUniquePtr->fity() << " , "
                    << trigMuMuUniquePtr->fitz()  <<  ") with chi2 = "
                    << trigMuMuUniquePtr->fitchi2() << " ("
                    << trigMuMuUniquePtr->fitndof() << "  dof)"
                    << " Mass= "<< massMuMu << endreq;
                
                // JK look for tracks in TrackParticle containers and add to TrigEFBphys
                //
                // TODO
                // Doesn't work now: track pointers *pItr, *mItr are not the same as in vectorTrkParticleCollections elements
                //
                bool Found1Track=false;
                bool Found2Track=false;
                
                // test for new method of track retrieval
                //const xAOD::TrackParticleContainer *tracksRoiI1(nullptr);
                //const xAOD::TrackParticleContainer *tracksRoiI2(nullptr);
                // Sergey S.: Retrieve tracks as ELs
                ElementLinkVector<xAOD::TrackParticleContainer> tracksRoiI1;
                ElementLinkVector<xAOD::TrackParticleContainer> tracksRoiI2;
                //get the tracks for each roi
                //HLT::ErrorCode status = getFeature(inputTE.front(), tracksRoiI1); // Need to insert the correct label
                HLT::ErrorCode status = getFeaturesLinks< xAOD::TrackParticleContainer, xAOD::TrackParticleContainer > (inputTE.front(), tracksRoiI1);
                if(status != HLT::OK) {
                    msg() << MSG::WARNING << "Failed to get xAOD::TrackParticleCollection from the 1st trigger element" << endreq;
                    mon_Errors.push_back(ERROR_No_TrackColl);
                    delete mTrigBphysColl_b;
                    delete mTrigBphysColl_X;
                    mTrigBphysColl_b = NULL;
                    mTrigBphysColl_X = NULL;
                    return HLT::MISSING_FEATURE;
                }
                //status = getFeature(inputTE.back() , tracksRoiI2); // Need to insert the correct label
                status = getFeaturesLinks< xAOD::TrackParticleContainer, xAOD::TrackParticleContainer > (inputTE.back(), tracksRoiI2);
                if(status != HLT::OK) {
                    msg() << MSG::WARNING << "Failed to get xAOD::TrackParticleCollection from the 2nd trigger element" << endreq;
                    mon_Errors.push_back(ERROR_No_TrackColl);
                    delete mTrigBphysColl_b;
                    delete mTrigBphysColl_X;
                    mTrigBphysColl_b = NULL;
                    mTrigBphysColl_X = NULL;
                    if ( timerSvc() ) m_TotTimer->stop();
                    return HLT::MISSING_FEATURE;
                }
                ATH_MSG(DEBUG) << "Ntracks RoI1: " << ( tracksRoiI1.empty() ? -1 : tracksRoiI1.size() ) << endreq;
                ATH_MSG(DEBUG) << "Ntracks RoI2: " << ( tracksRoiI2.empty() ? -1 : tracksRoiI2.size() ) << endreq;
                // JW if containers are empty - it's not necessarily an issue; right?
                //                if ( tracksRoiI1.empty() || tracksRoiI2.empty()) {
                //                    msg() << MSG::WARNING << "Null track pointer" << endreq;
                //                    mon_Errors.push_back(ERROR_No_TrackColl);
                //                    delete mTrigBphysColl_b;
                //                    delete mTrigBphysColl_X;
                //                    mTrigBphysColl_b = NULL;
                //                    mTrigBphysColl_X = NULL;
                //                    if ( timerSvc() ) m_TotTimer->stop();
                //                    return HLT::MISSING_FEATURE;
                //                }
                
                std::vector<const xAOD::TrackParticle*> merged_tracks;
                merged_tracks.reserve(tracksRoiI1.size() + tracksRoiI2.size());
                
                // in order to use the element links, create a map of the particle pointer, and the corresponding container, and index
                struct ElIndex{
                    int roi;
                    int index;
                    ElementLink<xAOD::TrackParticleContainer> elLink;
                };
                std::map<const xAOD::TrackParticle*, ElIndex> mapTrackToIndex;
                std::map<const xAOD::TrackParticle*, ElementLink<xAOD::TrackParticleContainer> > mapTrackToEL;
                
                int idCounter(0);
                for (auto trk: tracksRoiI1) {
                    // merged_tracks.push_back(trk);
                    addUnique(merged_tracks,*trk);
                    ElIndex tmp;
                    tmp.roi = 1;
                    tmp.index = idCounter;
                    tmp.elLink = trk;
                    mapTrackToIndex.insert( std::make_pair(*trk,tmp));
                    ++idCounter;
                }
                idCounter = 0;
                for (auto trk: tracksRoiI2) {
                    // merged_tracks.push_back(trk);
                    addUnique(merged_tracks,*trk);
                    ElIndex tmp;
                    tmp.roi = 2;
                    tmp.index = idCounter;
                    tmp.elLink = trk;
                    mapTrackToIndex.insert(std::make_pair(*trk,tmp));
                    ++idCounter;
                }
                
                if(msgLvl() <= MSG::DEBUG) {
                    msg() << MSG::DEBUG << "Merged Counts: " << merged_tracks.size() << "With merged tracks:\n";
                    for (auto trk: merged_tracks) {
                        msg() << MSG::DEBUG << trk << " TrackParticle: pt/eta/phi/q/d0/z0: "
                        << trk->pt() << " , " << trk->eta() << " , " << trk->phi() << " , " << trk->charge() << " , "
                        << trk->d0() << " , " << trk->z0()<<  "\n";
                        if (trk->track()) {
                            int qsign = (trk->track()->perigeeParameters()->parameters()[Trk::qOverP] > 0 ? +1 : -1);
                            msg() << MSG::DEBUG << trk->track() << " and Trk::Track: pt/eta/phi/q/d0/z0: "
                            << trk->track()->perigeeParameters()->pT()  << " , "
                            << trk->track()->perigeeParameters()->eta() << " , "
                            << trk->track()->perigeeParameters()->parameters()[Trk::phi0] << " , "
                            << qsign << " , "
                            << trk->track()->perigeeParameters()->parameters()[Trk::d0] << " , "
                            << trk->track()->perigeeParameters()->parameters()[Trk::z0] << "\n";
                        } else {
                            msg() << MSG::DEBUG <<  "   no Trk::Track\n" ;
                        }
                    }
                    msg() << MSG::DEBUG<< endreq;
                } // if debug
                
                
                // loop to find the muon associated withthe trk::track. can be optimised
                //                     int piTrk(-1),miTrk(-1);
                // JW just make a reference to the El-link, for simplicity of code updation.
                ElementLink<xAOD::TrackParticleContainer> &trackELmu1 = *pElItr;
                ElementLink<xAOD::TrackParticleContainer> &trackELmu2 = *mElItr;
                ElementLink<xAOD::IParticleContainer> ELmu1;
                ElementLink<xAOD::IParticleContainer> ELmu2;
                auto trkmuit = mapTrkToMuons.find(*trackELmu1);
                if (trkmuit != mapTrkToMuons.end()) {
                    //trackELmu1 = (*(trkmuit->second))->inDetTrackParticleLink();
                    ELmu1.resetWithKeyAndIndex(trkmuit->second.dataID(),trkmuit->second.index());
                    Found1Track = true;
                } // if
                trkmuit = mapTrkToMuons.find(*trackELmu2);
                if (trkmuit != mapTrkToMuons.end()) {
                    //trackELmu2 = (*(trkmuit->second))->inDetTrackParticleLink();
                    ELmu2.resetWithKeyAndIndex(trkmuit->second.dataID(),trkmuit->second.index());
                    Found2Track = true;
                } // if
                  //                ELmu1.resetWithKeyAndIndex(pTp->dataID(),pTp->index());
                  //                Found1Track = true;
                  //                ELmu2.resetWithKeyAndIndex(mTp->dataID(),mTp->index());
                  //                Found2Track = true;
                
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Matching summary: " << Found1Track<<Found2Track << endreq;
                
                
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Now loop over TrackParticles to find tracks " << endreq;
                //                    xAOD::TrackParticleContainer::const_iterator trkIt =  merged_tracks.begin();
                //                    xAOD::TrackParticleContainer::const_iterator lastTrkIt = merged_tracks.end();
                //std::vector<const xAOD::TrackParticle*>::const_iterator trkIt     = merged_tracks.begin();
                std::vector<const xAOD::TrackParticle*>::const_iterator lastTrkIt = merged_tracks.end();
                
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Found tracks, ntrack= " << merged_tracks.size() << endreq;
                
                
                // Main outer loop over tracks
                //xAOD::TrackParticleContainer::const_iterator trkIt1 =  merged_tracks.begin();
                std::vector<const xAOD::TrackParticle*>::const_iterator trkIt1     = merged_tracks.begin();
                
                for (int itrk1=0 ; trkIt1 != lastTrkIt; itrk1++, trkIt1++)
                {
                    //const Trk::Track* track1 = (*trkIt1)->track();
                    const xAOD::TrackParticle* track1 = (*trkIt1);
                    
//                    ElementLink<xAOD::TrackParticleContainer> trackEL3( mapTrackToIndex[*trkIt1].roi == 1 ? tracksRoiI1[0].dataID() : tracksRoiI2[0].dataID(), mapTrackToIndex[*trkIt1].index);
//                    ElementLink<xAOD::IParticleContainer> ItrackEL3;
//                    ItrackEL3.resetWithKeyAndIndex(mapTrackToIndex[*trkIt1].roi == 1 ? tracksRoiI1[0].dataID() : tracksRoiI2[0].dataID(), mapTrackToIndex[*trkIt1].index);
                    
                    ElementLink<xAOD::TrackParticleContainer> & trackEL3 = mapTrackToIndex[*trkIt1].elLink;
                    ElementLink<xAOD::IParticleContainer> ItrackEL3;
                    ItrackEL3.resetWithKeyAndIndex(trackEL3.dataID(), trackEL3.index());

                    if (*trackEL3 != track1) {
                        ATH_MSG(FATAL) << "Track 1 doesn't match dereferenced elementlink: " << track1 << " " << *trackEL3 << endreq;
                        if ( timerSvc() ) m_TotTimer->stop();
                        return HLT::MISSING_FEATURE;
                    }
                    
                    // Check that it is not muon track
                    //                         if(itrk1==piTrk || itrk1==miTrk) {
                    //                             ATH_MSG(DEBUG) << "Track " << track1 << " was matched to a muon, skip it" << endreq;
                    //                             continue;
                    //                         }
                    
                    // ST: EL comparison does not work -- they appear to be always different
                    //                         if(trackEL3 == trackELmu1 || trackEL3 == trackELmu2 ) {
                    //                             ATH_MSG(DEBUG) << "Track " << track1 << " was matched to a muon, skip it" << endreq;
                    //                             continue;
                    //                         }
                    if( !(Found1Track && isUnique(*trkIt1,*trackELmu1)) || !(Found2Track &&isUnique(*trkIt1,*trackELmu2)) ) {
                        ATH_MSG(DEBUG) << "Track " << track1 << " was matched to a muon, skip it" << endreq;
                        continue;
                    }
                    
                    // Fill tracks monitoring containers
                    mon_Tracks_Pt.push_back (track1->pt()/1000.);
                    mon_Tracks_Eta.push_back(track1->eta());
                    mon_Tracks_Phi.push_back(track1->phi());
                    
                    mon_Tracks_n++;
                    
                    //
                    // B+ -> mu mu K+ part
                    //
                    if(m_doB_KMuMuDecay) {
                        xAOD::TrigBphys* trigPartBplusMuMuKplus = checkBplusMuMuKplus(trackEL3,trackELmu1,trackELmu2);
                        ++nTriedCombinations;
                        if (trigPartBplusMuMuKplus) {
                            m_bphysHelperTool->setBeamlineDisplacement(trigPartBplusMuMuKplus,{*trackEL3,*trackELmu1,*trackELmu2});
                            if(m_maxBpToStore >= 0 && m_countBpToStore >= m_maxBpToStore) {
                              if(m_countBpToStore == m_maxBpToStore) {
                                ATH_MSG(WARNING) << "Reached maximum number of B+ candidates to store " << m_maxBpToStore << "; following candidates won't be written out" << endreq;
                                mon_Errors.push_back(ERROR_MaxNumBpReached);
                              }
                              else
                                ATH_MSG(DEBUG) << "Do not write out " << m_countBpToStore+1 << "th B+ candidate" << endreq;
                            }
                            else {
                              mTrigBphysColl_b->push_back(trigPartBplusMuMuKplus);
                            }
                            result=true;
                            mon_BMuMuK_n++;
                            m_countBpToStore++;
                            if(IdEvent!=m_lastEventPassedBplus) {
                                m_countPassedEventsBplus++;
                                m_lastEventPassedBplus = IdEvent;
                            }
                            // trackParticleLinks are set by the vertexing method
                            trigPartBplusMuMuKplus->addParticleLink(ItrackEL3);				// Sergey S.
                            if(Found1Track) trigPartBplusMuMuKplus->addParticleLink(ELmu1);
                            if(Found2Track) trigPartBplusMuMuKplus->addParticleLink(ELmu2);
                        }
                    } //  end if(m_doB_KMuMuDecay)
                    
                    //                        if(m_doB_KMuMuDecay) {
                    //
                    //                            TrigEFBphys* trigPartBplusMuMuKplus = checkBplusMuMuKplus(track1,*mItr,*pItr);
                    //                            nTriedCombinations++;
                    //                            if (trigPartBplusMuMuKplus) {
                    //                                //now make the xaod object
                    //                                xAOD::TrigBphys* xaodObj = new xAOD::TrigBphys();
                    //                                mTrigBphysColl_b->push_back( xaodObj );
                    //                                xaodObj->initialise(0, 0., 0., xAOD::TrigBphys::BKMUMU, trigPartBplusMuMuKplus->mass(), xAOD::TrigBphys::EF );
                    //
                    //                                //xaodObj->setEta         (trigPartBplusMuMuKplus->eta());
                    //                                //xaodObj->setPhi         (trigPartBplusMuMuKplus->phi());
                    //                                //xaodObj->setMass        (trigPartBplusMuMuKplus->mass());
                    //                                //xaodObj->setRoiId       (trigPartBplusMuMuKplus->roiId());
                    //                                //xaodObj->setParticleType((xAOD::TrigBphys::pType)aod->particleType());
                    //                                //xaodObj->setLevel       (xAOD::TrigBphys::levelType::EF);
                    //                                xaodObj->setFitmass     (trigPartBplusMuMuKplus->fitmass());
                    //                                xaodObj->setFitchi2     (trigPartBplusMuMuKplus->fitchi2());
                    //                                xaodObj->setFitndof     (trigPartBplusMuMuKplus->fitndof());
                    //                                xaodObj->setFitx        (trigPartBplusMuMuKplus->fitx());
                    //                                xaodObj->setFity        (trigPartBplusMuMuKplus->fity());
                    //                                xaodObj->setFitz        (trigPartBplusMuMuKplus->fitz());
                    //
                    //                                m_bphysHelperTool->setBeamlineDisplacement(xaodObj,{*trkIt1,*trackELmu1,*trackELmu2});
                    //
                    //                                delete trigPartBplusMuMuKplus; // done with the old object now.
                    //                                trigPartBplusMuMuKplus =0;
                    //
                    //
                    //                                xaodObj->addTrackParticleLink(trackEL3);
                    //                                xaodObj->addParticleLink(ItrackEL3);				// Sergey S.
                    //                                if(Found1Track) xaodObj->addTrackParticleLink(trackELmu1);
                    //                                if(Found2Track) xaodObj->addTrackParticleLink(trackELmu2);
                    //                                if(Found1Track) xaodObj->addParticleLink(ELmu1);
                    //                                if(Found2Track) xaodObj->addParticleLink(ELmu2);
                    //                                //m_trigBphysColl_b->push_back(trigPartBplusMuMuKplus);
                    //                                
                    //                                result=true;
                    //                                
                    //                                mon_BMuMuK_n++;
                    //                                
                    //                                if(IdEvent!=m_lastEventPassedBplus) {
                    //                                    m_countPassedEventsBplus++;
                    //                                    m_lastEventPassedBplus = IdEvent;
                    //                                }
                    //                                
                    //                                //                 delete trigPartBplusMuMuKplus;
                    //                            }
                    //                        } // end if(m_doB_KMuMuDecay)
                    
                    // Protection
                    if( nTriedCombinations > m_maxNcombinations ) {
                        ATH_MSG(DEBUG) << "Too many track combinations: " << endreq;
                        ATH_MSG(DEBUG) << "  nTriedCombinations = " << nTriedCombinations
                        << ", while MaxNcombinations = " << m_maxNcombinations << endreq;
                        ATH_MSG(DEBUG) << "Terminate the loop" << endreq;
                        ATH_MSG(DEBUG) << "  Bphys Collection size is " << mTrigBphysColl_b->size() << endreq;
                        if(mTrigBphysColl_b->size() > 0)
                            mon_Errors.push_back(ERROR_TooManyComb_Acc);
                        else
                            mon_Errors.push_back(ERROR_TooManyComb_Rej);
                        break;
                    }
                    
                    // Inner loop over tracks
                    //xAOD::TrackParticleContainer::const_iterator trkIt2 = trkIt1 + 1;
                    std::vector<const xAOD::TrackParticle*>::const_iterator trkIt2 = trkIt1 + 1;
                    for (int itrk2=itrk1+1 ; trkIt2 != lastTrkIt; itrk2++, trkIt2++)
                    {
                        //If timeout has happened in spite of the protection
                        if ( Athena::Timeout::instance().reached() ) {
                            return HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::TIMEOUT);
                        }
                        
                        if(m_doBd_KstarMuMuDecay || m_doBs_Phi1020MuMuDecay || m_doLb_LambdaMuMuDecay || m_doBc_DsMuMuDecay) {
                                //const Trk::Track* track2 = (*trkIt2)->track();
                                const xAOD::TrackParticle* track2 = (*trkIt2);

                            // Sergey S.
                            //                                ElementLink<xAOD::TrackParticleContainer> trackEL4( mapTrackToIndex[*trkIt2].roi == 1 ? tracksRoiI1[0].dataID() : tracksRoiI2[0].dataID(), mapTrackToIndex[*trkIt2].index);
                            //                                ElementLink<xAOD::IParticleContainer> ItrackEL4;
                            //                                ItrackEL4.resetWithKeyAndIndex(mapTrackToIndex[*trkIt2].roi == 1 ? tracksRoiI1[0].dataID() : tracksRoiI2[0].dataID(), mapTrackToIndex[*trkIt2].index);
                            
                            ElementLink<xAOD::TrackParticleContainer> & trackEL4 = mapTrackToIndex[*trkIt2].elLink;
                            ElementLink<xAOD::IParticleContainer> ItrackEL4;
                            ItrackEL4.resetWithKeyAndIndex(trackEL4.dataID(), trackEL4.index());

                            
                            if (*trackEL4 != track2) {
                                ATH_MSG(FATAL) << "Track 2 doesn't match dereferenced elementlink: " << track2 << " " << *trackEL4 << endreq;
                                if ( timerSvc() ) m_TotTimer->stop();
                                return HLT::MISSING_FEATURE;
                            }

                            
                                // Check that it is not muon track
//                                 if(itrk2==piTrk || itrk2==miTrk) {
//                                     ATH_MSG(DEBUG) << "Track " << track2 << " was matched to a muon, skip it" << endreq;
//                                     continue;
//                                 }
                                if( !(Found1Track && isUnique(*trkIt2,*trackELmu1)) || !(Found2Track &&isUnique(*trkIt2,*trackELmu2)) ) {
                                    ATH_MSG(DEBUG) << "Track " << track2 << " was matched to a muon, skip it" << endreq;
                                    continue;
                                }
                                
                                // check that track1 and track2 have opposite charges
                                if( (track1->charge())*(track2->charge()) > 0) {
                                    ATH_MSG(DEBUG) << "Track pair rejected by opposite charge check: "
                                    << track1 << " = " << track1->charge() << ",  "
                                    << track2 << " = " << track2->charge() << endreq;
                                    continue;
                                }
                                
                                //
                                // Bd -> mu mu K0* (K+ pi-) part
                                //
                                if(m_doBd_KstarMuMuDecay) {
                                    // Mass hypothesis 1: track1 = kaon, track2 = pion
                                    xAOD::TrigBphys* xaod_trigPartKstar(nullptr);
                                    xAOD::TrigBphys* xaod_trigPartBdMuMuKstar = checkBMuMu2X(trackEL3,trackEL4,trackELmu1,trackELmu2,
                                                                                             bD_to_Kstar,xaod_trigPartKstar);
                                    ++nTriedCombinations;
                                    if (xaod_trigPartBdMuMuKstar) {
                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartBdMuMuKstar,{*trackEL3,*trackEL4,*trackELmu1,*trackELmu2});
                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartKstar,      {*trackEL3,*trackEL4});
                                        
                                        if(m_maxBdToStore >= 0 && m_countBdToStore >= m_maxBdToStore) {
                                          if(m_countBdToStore == m_maxBdToStore) {
                                            ATH_MSG(WARNING) << "Reached maximum number of Bd candidates to store " << m_maxBdToStore << "; following candidates won't be written out" << endreq;
                                            mon_Errors.push_back(ERROR_MaxNumBdReached);
                                          }
                                          else
                                            ATH_MSG(DEBUG) << "Do not write out " << m_countBdToStore+1 << "th Bd candidate" << endreq;
                                        }
                                        else {
                                          mTrigBphysColl_b->push_back(xaod_trigPartBdMuMuKstar);
                                          mTrigBphysColl_X->push_back(xaod_trigPartKstar );
                                        }
                                        result=true;
                                        mon_BdMuMuKs_n++;
                                        m_countBdToStore++;
                                        if(IdEvent!=m_lastEventPassedBplus) {
                                            m_countPassedEventsBplus++;
                                            m_lastEventPassedBplus = IdEvent;
                                        }
                                        // trackParticleLinks are set by the vertexing method
                                        xaod_trigPartBdMuMuKstar->addParticleLink(ItrackEL3);				// Sergey S.
                                        xaod_trigPartBdMuMuKstar->addParticleLink(ItrackEL4);
                                        if(Found1Track) xaod_trigPartBdMuMuKstar->addParticleLink(ELmu1);
                                        if(Found2Track) xaod_trigPartBdMuMuKstar->addParticleLink(ELmu2);
                                        
                                        xaod_trigPartKstar->addParticleLink(ItrackEL3);				// Sergey S.
                                        xaod_trigPartKstar->addParticleLink(ItrackEL4);

                                        // set the secondary link; note, does not set correctly for persistified data.
                                        // see code further down for the re-setting of these links
                                        int iKstar = mTrigBphysColl_X->size() - 1;
                                        ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*mTrigBphysColl_X,iKstar);
                                        xaod_trigPartBdMuMuKstar->setSecondaryDecayLink(trigPartXEL);
                                        

                                    } // if hypo1
                                    xaod_trigPartKstar = nullptr; //hypo 1 added to SG(if exists), so reuse the pointer
                                    
                                    // Mass hypothesis 2: track1 = pion, track2 = kaon
                                    xaod_trigPartBdMuMuKstar = checkBMuMu2X(trackEL4,trackEL3,trackELmu1,trackELmu2,
                                                                            bD_to_Kstar,xaod_trigPartKstar);
                                    ++nTriedCombinations;
                                    if (xaod_trigPartBdMuMuKstar) {
                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartBdMuMuKstar,{*trackEL4,*trackEL3,*trackELmu1,*trackELmu2});
                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartKstar,      {*trackEL4,*trackEL3});
                                        if(m_maxBdToStore >= 0 && m_countBdToStore >= m_maxBdToStore) {
                                          if(m_countBdToStore == m_maxBdToStore) {
                                            ATH_MSG(WARNING) << "Reached maximum number of Bd candidates to store " << m_maxBdToStore << "; following candidates won't be written out" << endreq;
                                            mon_Errors.push_back(ERROR_MaxNumBdReached);
                                          }
                                          else
                                            ATH_MSG(DEBUG) << "Do not write out " << m_countBdToStore+1 << "th Bd candidate" << endreq;
                                        }
                                        else {
                                          mTrigBphysColl_b->push_back(xaod_trigPartBdMuMuKstar);
                                          mTrigBphysColl_X->push_back(xaod_trigPartKstar );
                                        }
                                        result=true;
                                        mon_BdMuMuKs_n++;
                                        m_countBdToStore++;
                                        if(IdEvent!=m_lastEventPassedBplus) {
                                            m_countPassedEventsBplus++;
                                            m_lastEventPassedBplus = IdEvent;
                                        }
                                        // trackParticleLinks are set by the vertexing method
                                        xaod_trigPartBdMuMuKstar->addParticleLink(ItrackEL4);				// Sergey S.
                                        xaod_trigPartBdMuMuKstar->addParticleLink(ItrackEL3);
                                        if(Found1Track) xaod_trigPartBdMuMuKstar->addParticleLink(ELmu1);
                                        if(Found2Track) xaod_trigPartBdMuMuKstar->addParticleLink(ELmu2);
                                        
                                        xaod_trigPartKstar->addParticleLink(ItrackEL4);				// Sergey S.
                                        xaod_trigPartKstar->addParticleLink(ItrackEL3);
                                        
                                        // set the secondary link; note, does not set correctly for persistified data.
                                        // see code further down for the re-setting of these links
                                        int iKstar = mTrigBphysColl_X->size() - 1;
                                        ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*mTrigBphysColl_X,iKstar);
                                        xaod_trigPartBdMuMuKstar->setSecondaryDecayLink(trigPartXEL);
                                    } // if hypo2
                                } // if m_doBd_KstarMuMuDecay
                                
//                                if(m_doBd_KstarMuMuDecay) {
//                                    TrigEFBphys* trigPartKstar(0);
//                                    
//                                    // Mass hypothesis 1: track1 = kaon, track2 = pion
//                                    TrigEFBphys* trigPartBdMuMuKstar = checkBMuMu2X(*mItr,*pItr,track1,track2,bD_to_Kstar,trigPartKstar);
//                                    nTriedCombinations++;
//                                    if (trigPartBdMuMuKstar) {
//                                        //                     trigPartBdMuMuKstar->addTrack(trackELmu1);
//                                        //                     trigPartBdMuMuKstar->addTrack(trackELmu2);
//                                        //now make the xaod object
//                                        xAOD::TrigBphys* xaod_trigPartBdMuMuKstar = new xAOD::TrigBphys();
//                                        mTrigBphysColl_b->push_back( xaod_trigPartBdMuMuKstar );
//                                        xaod_trigPartBdMuMuKstar->initialise(0, 0., 0.,
//                                                                             (xAOD::TrigBphys::pType)trigPartBdMuMuKstar->particleType(),
//                                                                             trigPartBdMuMuKstar->mass(),
//                                                                             xAOD::TrigBphys::EF );
//                                        
//                                        xAOD::TrigBphys* xaod_trigPartKstar = new xAOD::TrigBphys();
//                                        mTrigBphysColl_X->push_back( xaod_trigPartKstar );
//                                        xaod_trigPartKstar->initialise(0, 0., 0.,
//                                                                       (xAOD::TrigBphys::pType)trigPartKstar->particleType(),
//                                                                       trigPartKstar->mass(),
//                                                                       xAOD::TrigBphys::EF );
//                                        
//                                        
//                                        xaod_trigPartBdMuMuKstar->setFitmass     (trigPartBdMuMuKstar->fitmass());
//                                        xaod_trigPartBdMuMuKstar->setFitchi2     (trigPartBdMuMuKstar->fitchi2());
//                                        xaod_trigPartBdMuMuKstar->setFitndof     (trigPartBdMuMuKstar->fitndof());
//                                        xaod_trigPartBdMuMuKstar->setFitx        (trigPartBdMuMuKstar->fitx());
//                                        xaod_trigPartBdMuMuKstar->setFity        (trigPartBdMuMuKstar->fity());
//                                        xaod_trigPartBdMuMuKstar->setFitz        (trigPartBdMuMuKstar->fitz());
//                                        
//                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartBdMuMuKstar,{*trkIt1,*trkIt2,*trackELmu1,*trackELmu2});
//                                        
//                                        delete trigPartBdMuMuKstar; // done with the old object now.
//                                        trigPartBdMuMuKstar =0;
//                                        
//                                        xaod_trigPartKstar->setFitmass     (trigPartKstar->fitmass());
//                                        xaod_trigPartKstar->setFitchi2     (trigPartKstar->fitchi2());
//                                        xaod_trigPartKstar->setFitndof     (trigPartKstar->fitndof());
//                                        xaod_trigPartKstar->setFitx        (trigPartKstar->fitx());
//                                        xaod_trigPartKstar->setFity        (trigPartKstar->fity());
//                                        xaod_trigPartKstar->setFitz        (trigPartKstar->fitz());
//                                        
//                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartKstar,{*trkIt1,*trkIt2});
//                                        
//                                        delete trigPartKstar; // done with the old object now.
//                                        trigPartKstar =0;
//                                        
//                                        xaod_trigPartKstar->addTrackParticleLink(trackEL3);
//                                        xaod_trigPartKstar->addTrackParticleLink(trackEL4);
//                                        xaod_trigPartKstar->addParticleLink(ItrackEL3);
//                                        xaod_trigPartKstar->addParticleLink(ItrackEL4);
//                                        //m_trigBphysColl_X->push_back(trigPartKstar);
//                                        int iKstar = mTrigBphysColl_X->size() - 1;
//                                        ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*mTrigBphysColl_X,iKstar);
//                                        xaod_trigPartBdMuMuKstar->addTrackParticleLink(trackEL3);
//                                        xaod_trigPartBdMuMuKstar->addTrackParticleLink(trackEL4);
//                                        xaod_trigPartBdMuMuKstar->addParticleLink(ItrackEL3);
//                                        xaod_trigPartBdMuMuKstar->addParticleLink(ItrackEL4);
//                                        if(Found1Track) xaod_trigPartBdMuMuKstar->addTrackParticleLink(trackELmu1);
//                                        if(Found2Track) xaod_trigPartBdMuMuKstar->addTrackParticleLink(trackELmu2);
//                                        if(Found1Track) xaod_trigPartBdMuMuKstar->addParticleLink(ELmu1);
//                                        if(Found2Track) xaod_trigPartBdMuMuKstar->addParticleLink(ELmu2);
//                                        xaod_trigPartBdMuMuKstar->setSecondaryDecayLink(trigPartXEL);
//                                        //m_trigBphysColl_b->push_back(trigPartBdMuMuKstar);
//                                        
//                                        result=true;
//                                        
//                                        mon_BdMuMuKs_n++;
//                                        
//                                        if(IdEvent!=m_lastEventPassedBd) {
//                                            m_countPassedEventsBd++;
//                                            m_lastEventPassedBd = IdEvent;
//                                        }
//                                        
//                                        //                     delete trigPartBdMuMuKstar;
//                                    }
//                                    
//                                    // Mass hypothesis 2: track1 = pion, track2 = kaon
//                                    trigPartBdMuMuKstar = checkBMuMu2X(*mItr,*pItr,track2,track1,bD_to_Kstar,trigPartKstar);
//                                    nTriedCombinations++;
//                                    if (trigPartBdMuMuKstar) {
//                                        //now make the xaod object
//                                        xAOD::TrigBphys* xaod_trigPartBdMuMuKstar = new xAOD::TrigBphys();
//                                        mTrigBphysColl_b->push_back( xaod_trigPartBdMuMuKstar );
//                                        xaod_trigPartBdMuMuKstar->initialise(0, 0., 0.,
//                                                                             (xAOD::TrigBphys::pType)trigPartBdMuMuKstar->particleType(),
//                                                                             trigPartBdMuMuKstar->mass(),
//                                                                             xAOD::TrigBphys::EF );
//                                        
//                                        xAOD::TrigBphys* xaod_trigPartKstar = new xAOD::TrigBphys();
//                                        mTrigBphysColl_X->push_back( xaod_trigPartKstar );
//                                        xaod_trigPartKstar->initialise(0, 0., 0.,
//                                                                       (xAOD::TrigBphys::pType)trigPartKstar->particleType(),
//                                                                       trigPartKstar->mass(),
//                                                                       xAOD::TrigBphys::EF );
//                                        
//                                        
//                                        xaod_trigPartBdMuMuKstar->setFitmass     (trigPartBdMuMuKstar->fitmass());
//                                        xaod_trigPartBdMuMuKstar->setFitchi2     (trigPartBdMuMuKstar->fitchi2());
//                                        xaod_trigPartBdMuMuKstar->setFitndof     (trigPartBdMuMuKstar->fitndof());
//                                        xaod_trigPartBdMuMuKstar->setFitx        (trigPartBdMuMuKstar->fitx());
//                                        xaod_trigPartBdMuMuKstar->setFity        (trigPartBdMuMuKstar->fity());
//                                        xaod_trigPartBdMuMuKstar->setFitz        (trigPartBdMuMuKstar->fitz());
//                                        
//                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartBdMuMuKstar,{*trkIt1,*trkIt2,*trackELmu1,*trackELmu2});
//                                        
//                                        delete trigPartBdMuMuKstar; // done with the old object now.
//                                        trigPartBdMuMuKstar =0;
//                                        
//                                        xaod_trigPartKstar->setFitmass     (trigPartKstar->fitmass());
//                                        xaod_trigPartKstar->setFitchi2     (trigPartKstar->fitchi2());
//                                        xaod_trigPartKstar->setFitndof     (trigPartKstar->fitndof());
//                                        xaod_trigPartKstar->setFitx        (trigPartKstar->fitx());
//                                        xaod_trigPartKstar->setFity        (trigPartKstar->fity());
//                                        xaod_trigPartKstar->setFitz        (trigPartKstar->fitz());
//                                        
//                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartKstar,{*trkIt1,*trkIt2});
//                                        
//                                        delete trigPartKstar; // done with the old object now.
//                                        trigPartKstar =0;
//
//                                        xaod_trigPartKstar->addTrackParticleLink(trackEL3);
//                                        xaod_trigPartKstar->addTrackParticleLink(trackEL4);
//                                        xaod_trigPartKstar->addParticleLink(ItrackEL3);
//                                        xaod_trigPartKstar->addParticleLink(ItrackEL4);
//                                        //m_trigBphysColl_X->push_back(trigPartKstar);
//                                        int iKstar = mTrigBphysColl_X->size() - 1;
//                                        ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*mTrigBphysColl_X,iKstar);
//                                        xaod_trigPartBdMuMuKstar->addTrackParticleLink(trackEL3);
//                                        xaod_trigPartBdMuMuKstar->addTrackParticleLink(trackEL4);
//                                        xaod_trigPartBdMuMuKstar->addParticleLink(ItrackEL3);
//                                        xaod_trigPartBdMuMuKstar->addParticleLink(ItrackEL4);
//                                        if(Found1Track) xaod_trigPartBdMuMuKstar->addTrackParticleLink(trackELmu1);
//                                        if(Found2Track) xaod_trigPartBdMuMuKstar->addTrackParticleLink(trackELmu2);
//                                        if(Found1Track) xaod_trigPartBdMuMuKstar->addParticleLink(ELmu1);
//                                        if(Found2Track) xaod_trigPartBdMuMuKstar->addParticleLink(ELmu2);
//                                        xaod_trigPartBdMuMuKstar->setSecondaryDecayLink(trigPartXEL);
//                                        //m_trigBphysColl_b->push_back(trigPartBdMuMuKstar);
//                                        
//                                        result=true;
//                                        
//                                        mon_BdMuMuKs_n++;
//                                        
//                                        if(IdEvent!=m_lastEventPassedBd) {
//                                            m_countPassedEventsBd++;
//                                            m_lastEventPassedBd = IdEvent;
//                                        }
//                                        
//                                        //                     delete trigPartBdMuMuKstar;
//                                    }
//                                } // end if(m_doBd_KstarMuMuDecay)
                                
                                //
                                // Bs -> mu mu Phi (K K) part
                                //
                                if(m_doBs_Phi1020MuMuDecay) {
                                    xAOD::TrigBphys* xaod_trigPartPhi(nullptr);
                                    xAOD::TrigBphys* xaod_trigPartBsMuMuPhi = checkBMuMu2X(trackEL3,trackEL4,trackELmu1,trackELmu2,
                                                                                           bS_to_Phi,xaod_trigPartPhi);

                                    ++nTriedCombinations;
                                    if (xaod_trigPartBsMuMuPhi) {
                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartBsMuMuPhi,{*trackEL3,*trackEL4,*trackELmu1,*trackELmu2});
                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartPhi,        {*trackEL3,*trackEL4});
                                        
                                        if(m_maxBsToStore >= 0 && m_countBsToStore >= m_maxBsToStore) {
                                          if(m_countBsToStore == m_maxBsToStore) {
                                            ATH_MSG(WARNING) << "Reached maximum number of Bs candidates to store " << m_maxBsToStore << "; following candidates won't be written out" << endreq;
                                            mon_Errors.push_back(ERROR_MaxNumBsReached);
                                          }
                                          else
                                            ATH_MSG(DEBUG) << "Do not write out " << m_countBsToStore+1 << "th Bs candidate" << endreq;
                                        }
                                        else {
                                          mTrigBphysColl_b->push_back(xaod_trigPartBsMuMuPhi);
                                          mTrigBphysColl_X->push_back(xaod_trigPartPhi );
                                        }
                                        result=true;
                                        mon_BsMuMuPhi_n++;
                                        m_countBsToStore++;
                                        if(IdEvent!=m_lastEventPassedBplus) {
                                            m_countPassedEventsBplus++;
                                            m_lastEventPassedBplus = IdEvent;
                                        }
                                        // trackParticleLinks are set by the vertexing method
                                        xaod_trigPartBsMuMuPhi->addParticleLink(ItrackEL3);				// Sergey S.
                                        xaod_trigPartBsMuMuPhi->addParticleLink(ItrackEL4);
                                        if(Found1Track) xaod_trigPartBsMuMuPhi->addParticleLink(ELmu1);
                                        if(Found2Track) xaod_trigPartBsMuMuPhi->addParticleLink(ELmu2);
                                        
                                        xaod_trigPartPhi->addParticleLink(ItrackEL3);				// Sergey S.
                                        xaod_trigPartPhi->addParticleLink(ItrackEL4);
                                        
                                        // set the secondary link; note, does not set correctly for persistified data.
                                        // see code further down for the re-setting of these links
                                        int iPhi = mTrigBphysColl_X->size() - 1;
                                        ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*mTrigBphysColl_X,iPhi);
                                        xaod_trigPartBsMuMuPhi->setSecondaryDecayLink(trigPartXEL);
                                        
                                        
                                    }
                                } // m_doBs_Phi1020MuMuDecay
                                
//                                if(m_doBs_Phi1020MuMuDecay) {
//                                    TrigEFBphys* trigPartPhi(0);
//                                    
//                                    TrigEFBphys* trigPartBsMuMuPhi = checkBMuMu2X(*mItr,*pItr,track1,track2,bS_to_Phi,trigPartPhi);
//                                    nTriedCombinations++;
//                                    if (trigPartBsMuMuPhi) {
//                                        //now make the xaod object
//                                        xAOD::TrigBphys* xaod_trigPartBsMuMuPhi = new xAOD::TrigBphys();
//                                        mTrigBphysColl_b->push_back( xaod_trigPartBsMuMuPhi );
//                                        xaod_trigPartBsMuMuPhi->initialise(0, 0., 0.,
//                                                                             (xAOD::TrigBphys::pType)trigPartBsMuMuPhi->particleType(),
//                                                                             trigPartBsMuMuPhi->mass(),
//                                                                             xAOD::TrigBphys::EF );
//                                        
//                                        xAOD::TrigBphys* xaod_trigPartPhi = new xAOD::TrigBphys();
//                                        mTrigBphysColl_X->push_back( xaod_trigPartPhi );
//                                        xaod_trigPartPhi->initialise(0, 0., 0.,
//                                                                       (xAOD::TrigBphys::pType)trigPartPhi->particleType(),
//                                                                       trigPartPhi->mass(),
//                                                                       xAOD::TrigBphys::EF );
//                                        
//                                        
//                                        xaod_trigPartBsMuMuPhi->setFitmass     (trigPartBsMuMuPhi->fitmass());
//                                        xaod_trigPartBsMuMuPhi->setFitchi2     (trigPartBsMuMuPhi->fitchi2());
//                                        xaod_trigPartBsMuMuPhi->setFitndof     (trigPartBsMuMuPhi->fitndof());
//                                        xaod_trigPartBsMuMuPhi->setFitx        (trigPartBsMuMuPhi->fitx());
//                                        xaod_trigPartBsMuMuPhi->setFity        (trigPartBsMuMuPhi->fity());
//                                        xaod_trigPartBsMuMuPhi->setFitz        (trigPartBsMuMuPhi->fitz());
//                                        
//                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartBsMuMuPhi,{*trkIt1,*trkIt2,*trackELmu1,*trackELmu2});
//                                        
//                                        delete trigPartBsMuMuPhi; // done with the old object now.
//                                        trigPartBsMuMuPhi =0;
//                                        
//                                        xaod_trigPartPhi->setFitmass     (trigPartPhi->fitmass());
//                                        xaod_trigPartPhi->setFitchi2     (trigPartPhi->fitchi2());
//                                        xaod_trigPartPhi->setFitndof     (trigPartPhi->fitndof());
//                                        xaod_trigPartPhi->setFitx        (trigPartPhi->fitx());
//                                        xaod_trigPartPhi->setFity        (trigPartPhi->fity());
//                                        xaod_trigPartPhi->setFitz        (trigPartPhi->fitz());
//                                        
//                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartPhi,{*trkIt1,*trkIt2});
//                                        
//                                        delete trigPartPhi; // done with the old object now.
//                                        trigPartPhi =0;
//                                        
//                                        xaod_trigPartPhi->addTrackParticleLink(trackEL3);
//                                        xaod_trigPartPhi->addTrackParticleLink(trackEL4);
//                                        //m_trigBphysColl_X->push_back(trigPartPhi);
//                                        int iPhi = mTrigBphysColl_X->size() - 1;
//                                        ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*mTrigBphysColl_X,iPhi);
//                                        xaod_trigPartBsMuMuPhi->addTrackParticleLink(trackEL3);
//                                        xaod_trigPartBsMuMuPhi->addTrackParticleLink(trackEL4);
//                                        if(Found1Track) xaod_trigPartBsMuMuPhi->addTrackParticleLink(trackELmu1);
//                                        if(Found2Track) xaod_trigPartBsMuMuPhi->addTrackParticleLink(trackELmu2);
//                                        if(Found1Track) xaod_trigPartBsMuMuPhi->addParticleLink(ELmu1);
//                                        if(Found2Track) xaod_trigPartBsMuMuPhi->addParticleLink(ELmu2);
//                                        xaod_trigPartBsMuMuPhi->setSecondaryDecayLink(trigPartXEL);
//                                        //m_trigBphysColl_b->push_back(trigPartBsMuMuPhi);
//                                        
//                                        result=true;
//                                        
//                                        mon_BsMuMuPhi_n++;
//                                        
//                                        if(IdEvent!=m_lastEventPassedBs) {
//                                            m_countPassedEventsBs++;
//                                            m_lastEventPassedBs = IdEvent;
//                                        }
//                                        
//                                        //                     delete trigPartBsMuMuPhi;
//                                    }
//                                } // end if(m_doBs_Phi1020MuMuDecay)
                                
                                //
                                // Lambda_b -> mu mu Lambda part
                                //
                                
                                if(m_doLb_LambdaMuMuDecay) {
                                    // Now L_b uses simple 4-prong vertex fit instead of cascade vertexing

                                    // Mass hypothesis 1: track1 = proton, track2 = pion
                                    xAOD::TrigBphys* xaod_trigPartLambda(nullptr);
                                    xAOD::TrigBphys* xaod_trigPartLbMuMuLambda = checkBMuMu2X(trackEL3,trackEL4,trackELmu1,trackELmu2,
                                                                                              lB_to_L,xaod_trigPartLambda);
                                    ++nTriedCombinations;
                                    if (xaod_trigPartLbMuMuLambda) {
                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartLbMuMuLambda,{*trackEL3,*trackEL4,*trackELmu1,*trackELmu2});
                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartLambda,      {*trackEL3,*trackEL4});
                                        
                                        if(m_maxLbToStore >= 0 && m_countLbToStore >= m_maxLbToStore) {
                                          if(m_countLbToStore == m_maxLbToStore) {
                                            ATH_MSG(WARNING) << "Reached maximum number of Lb candidates to store " << m_maxLbToStore << "; following candidates won't be written out" << endreq;
                                            mon_Errors.push_back(ERROR_MaxNumLbReached);
                                          }
                                          else
                                            ATH_MSG(DEBUG) << "Do not write out " << m_countLbToStore+1 << "th Lb candidate" << endreq;
                                        }
                                        else {
                                          mTrigBphysColl_b->push_back(xaod_trigPartLbMuMuLambda);
                                          mTrigBphysColl_X->push_back(xaod_trigPartLambda );
                                        }
                                        result=true;
                                        mon_LbMuMuLambda_n++;
                                        m_countLbToStore++;
                                        if(IdEvent!=m_lastEventPassedBplus) {
                                            m_countPassedEventsBplus++;
                                            m_lastEventPassedBplus = IdEvent;
                                        }
                                        // trackParticleLinks are set by the vertexing method
                                        xaod_trigPartLbMuMuLambda->addParticleLink(ItrackEL3);				// Sergey S.
                                        xaod_trigPartLbMuMuLambda->addParticleLink(ItrackEL4);
                                        if(Found1Track) xaod_trigPartLbMuMuLambda->addParticleLink(ELmu1);
                                        if(Found2Track) xaod_trigPartLbMuMuLambda->addParticleLink(ELmu2);
                                        
                                        xaod_trigPartLambda->addParticleLink(ItrackEL3);				// Sergey S.
                                        xaod_trigPartLambda->addParticleLink(ItrackEL4);
                                        
                                        // set the secondary link; note, does not set correctly for persistified data.
                                        // see code further down for the re-setting of these links
                                        int iLambda = mTrigBphysColl_X->size() - 1;
                                        ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*mTrigBphysColl_X,iLambda);
                                        xaod_trigPartLbMuMuLambda->setSecondaryDecayLink(trigPartXEL);
                                    } // if hypo1
                                    xaod_trigPartLambda = nullptr; //hypo 1 added to SG(if exists), so reuse the pointer

                                    // Mass hypothesis 2: track1 = pion, track2 = proton
                                    xaod_trigPartLbMuMuLambda = checkBMuMu2X(trackEL4,trackEL3,trackELmu1,trackELmu2,
                                                                             lB_to_L,xaod_trigPartLambda);
                                    ++nTriedCombinations;
                                    if (xaod_trigPartLbMuMuLambda) {
                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartLbMuMuLambda,{*trackEL4,*trackEL3,*trackELmu1,*trackELmu2});
                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartLambda,      {*trackEL4,*trackEL3});
                                        
                                        if(m_maxLbToStore >= 0 && m_countLbToStore >= m_maxLbToStore) {
                                          if(m_countLbToStore == m_maxLbToStore) {
                                            ATH_MSG(WARNING) << "Reached maximum number of Lb candidates to store " << m_maxLbToStore << "; following candidates won't be written out" << endreq;
                                            mon_Errors.push_back(ERROR_MaxNumLbReached);
                                          }
                                          else
                                            ATH_MSG(DEBUG) << "Do not write out " << m_countLbToStore+1 << "th Lb candidate" << endreq;
                                        }
                                        else {
                                          mTrigBphysColl_b->push_back(xaod_trigPartLbMuMuLambda);
                                          mTrigBphysColl_X->push_back(xaod_trigPartLambda );
                                        }
                                        result=true;
                                        mon_LbMuMuLambda_n++;
                                        m_countLbToStore++;
                                        if(IdEvent!=m_lastEventPassedBplus) {
                                            m_countPassedEventsBplus++;
                                            m_lastEventPassedBplus = IdEvent;
                                        }
                                        // trackParticleLinks are set by the vertexing method
                                        xaod_trigPartLbMuMuLambda->addParticleLink(ItrackEL4);				// Sergey S.
                                        xaod_trigPartLbMuMuLambda->addParticleLink(ItrackEL3);
                                        if(Found1Track) xaod_trigPartLbMuMuLambda->addParticleLink(ELmu1);
                                        if(Found2Track) xaod_trigPartLbMuMuLambda->addParticleLink(ELmu2);
                                        
                                        xaod_trigPartLambda->addParticleLink(ItrackEL4);				// Sergey S.
                                        xaod_trigPartLambda->addParticleLink(ItrackEL3);
                                        
                                        // set the secondary link; note, does not set correctly for persistified data.
                                        // see code further down for the re-setting of these links
                                        int iLambda = mTrigBphysColl_X->size() - 1;
                                        ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*mTrigBphysColl_X,iLambda);
                                        xaod_trigPartLbMuMuLambda->setSecondaryDecayLink(trigPartXEL);
                                    } // if hypo2

                                    
                                } // m_doLb_LambdaMuMuDecay
                                
//                                if(m_doLb_LambdaMuMuDecay) {
//                                    // TODO
//                                    // Now L_b uses simple 4-prong vertex fit instead of cascade vertexing
//                                    TrigEFBphys* trigPartLambda(0);
//                                    
//                                    // Mass hypothesis 1: track1 = proton, track2 = pion
//                                    TrigEFBphys* trigPartLbMuMuLambda = checkBMuMu2X(*mItr,*pItr,track1,track2,lB_to_L,trigPartLambda);
//                                    nTriedCombinations++;
//                                    if (trigPartLbMuMuLambda) {
//                                        //now make the xaod object
//                                        xAOD::TrigBphys* xaod_trigPartLbMuMuLambda = new xAOD::TrigBphys();
//                                        mTrigBphysColl_b->push_back( xaod_trigPartLbMuMuLambda );
//                                        xaod_trigPartLbMuMuLambda->initialise(0, 0., 0.,
//                                                                           (xAOD::TrigBphys::pType)trigPartLbMuMuLambda->particleType(),
//                                                                           trigPartLbMuMuLambda->mass(),
//                                                                           xAOD::TrigBphys::EF );
//                                        
//                                        xAOD::TrigBphys* xaod_trigPartLambda = new xAOD::TrigBphys();
//                                        mTrigBphysColl_X->push_back( xaod_trigPartLambda );
//                                        xaod_trigPartLambda->initialise(0, 0., 0.,
//                                                                     (xAOD::TrigBphys::pType)trigPartLambda->particleType(),
//                                                                     trigPartLambda->mass(),
//                                                                     xAOD::TrigBphys::EF );
//                                        
//                                        
//                                        xaod_trigPartLbMuMuLambda->setFitmass     (trigPartLbMuMuLambda->fitmass());
//                                        xaod_trigPartLbMuMuLambda->setFitchi2     (trigPartLbMuMuLambda->fitchi2());
//                                        xaod_trigPartLbMuMuLambda->setFitndof     (trigPartLbMuMuLambda->fitndof());
//                                        xaod_trigPartLbMuMuLambda->setFitx        (trigPartLbMuMuLambda->fitx());
//                                        xaod_trigPartLbMuMuLambda->setFity        (trigPartLbMuMuLambda->fity());
//                                        xaod_trigPartLbMuMuLambda->setFitz        (trigPartLbMuMuLambda->fitz());
//                                        
//                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartLbMuMuLambda,{*trkIt1,*trkIt2,*trackELmu1,*trackELmu2});
//                                        
//                                        delete trigPartLbMuMuLambda; // done with the old object now.
//                                        trigPartLbMuMuLambda =0;
//                                        
//                                        xaod_trigPartLambda->setFitmass     (trigPartLambda->fitmass());
//                                        xaod_trigPartLambda->setFitchi2     (trigPartLambda->fitchi2());
//                                        xaod_trigPartLambda->setFitndof     (trigPartLambda->fitndof());
//                                        xaod_trigPartLambda->setFitx        (trigPartLambda->fitx());
//                                        xaod_trigPartLambda->setFity        (trigPartLambda->fity());
//                                        xaod_trigPartLambda->setFitz        (trigPartLambda->fitz());
//                                        
//                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartLambda,{*trkIt1,*trkIt2});
//                                        
//                                        delete trigPartLambda; // done with the old object now.
//                                        trigPartLambda =0;
//
//                                        xaod_trigPartLambda->addTrackParticleLink(trackEL3);
//                                        xaod_trigPartLambda->addTrackParticleLink(trackEL4);
//                                        xaod_trigPartLambda->addParticleLink(ItrackEL3);
//                                        xaod_trigPartLambda->addParticleLink(ItrackEL4);
//                                        // m_trigBphysColl_X->push_back(trigPartLambda);
//                                        int iLambda = mTrigBphysColl_X->size() - 1;
//                                        ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*mTrigBphysColl_X,iLambda);
//                                        xaod_trigPartLbMuMuLambda->addTrackParticleLink(trackEL3);
//                                        xaod_trigPartLbMuMuLambda->addTrackParticleLink(trackEL4);
//                                        xaod_trigPartLbMuMuLambda->addParticleLink(ItrackEL3);
//                                        xaod_trigPartLbMuMuLambda->addParticleLink(ItrackEL4);
//                                        if(Found1Track) xaod_trigPartLbMuMuLambda->addTrackParticleLink(trackELmu1);
//                                        if(Found2Track) xaod_trigPartLbMuMuLambda->addTrackParticleLink(trackELmu2);
//                                        if(Found1Track) xaod_trigPartLbMuMuLambda->addParticleLink(ELmu1);
//                                        if(Found2Track) xaod_trigPartLbMuMuLambda->addParticleLink(ELmu2);
//                                        xaod_trigPartLbMuMuLambda->setSecondaryDecayLink(trigPartXEL);
//                                        // m_trigBphysColl_b->push_back(trigPartLbMuMuLambda);
//                                        
//                                        result = true;
//                                        
//                                        mon_LbMuMuLambda_n++;
//                                        
//                                        if(IdEvent!=m_lastEventPassedLb) {
//                                            m_countPassedEventsLb++;
//                                            m_lastEventPassedLb = IdEvent;
//                                        }
//                                        
//                                        //                     delete trigPartLbMuMuLambda;
//                                    }
//                                    
//                                    // Mass hypothesis 2: track1 = pion, track2 = proton
//                                    trigPartLbMuMuLambda = checkBMuMu2X(*mItr,*pItr,track2,track1,lB_to_L,trigPartLambda);
//                                    nTriedCombinations++;
//                                    if (trigPartLbMuMuLambda) {
//                                        //now make the xaod object
//                                        xAOD::TrigBphys* xaod_trigPartLbMuMuLambda = new xAOD::TrigBphys();
//                                        mTrigBphysColl_b->push_back( xaod_trigPartLbMuMuLambda );
//                                        xaod_trigPartLbMuMuLambda->initialise(0, 0., 0.,
//                                                                              (xAOD::TrigBphys::pType)trigPartLbMuMuLambda->particleType(),
//                                                                              trigPartLbMuMuLambda->mass(),
//                                                                              xAOD::TrigBphys::EF );
//                                        
//                                        xAOD::TrigBphys* xaod_trigPartLambda = new xAOD::TrigBphys();
//                                        mTrigBphysColl_X->push_back( xaod_trigPartLambda );
//                                        xaod_trigPartLambda->initialise(0, 0., 0.,
//                                                                        (xAOD::TrigBphys::pType)trigPartLambda->particleType(),
//                                                                        trigPartLambda->mass(),
//                                                                        xAOD::TrigBphys::EF );
//                                        
//                                        
//                                        xaod_trigPartLbMuMuLambda->setFitmass     (trigPartLbMuMuLambda->fitmass());
//                                        xaod_trigPartLbMuMuLambda->setFitchi2     (trigPartLbMuMuLambda->fitchi2());
//                                        xaod_trigPartLbMuMuLambda->setFitndof     (trigPartLbMuMuLambda->fitndof());
//                                        xaod_trigPartLbMuMuLambda->setFitx        (trigPartLbMuMuLambda->fitx());
//                                        xaod_trigPartLbMuMuLambda->setFity        (trigPartLbMuMuLambda->fity());
//                                        xaod_trigPartLbMuMuLambda->setFitz        (trigPartLbMuMuLambda->fitz());
//                                        
//                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartLbMuMuLambda,{*trkIt1,*trkIt2,*trackELmu1,*trackELmu2});
//                                        
//                                        delete trigPartLbMuMuLambda; // done with the old object now.
//                                        trigPartLbMuMuLambda =0;
//                                        
//                                        xaod_trigPartLambda->setFitmass     (trigPartLambda->fitmass());
//                                        xaod_trigPartLambda->setFitchi2     (trigPartLambda->fitchi2());
//                                        xaod_trigPartLambda->setFitndof     (trigPartLambda->fitndof());
//                                        xaod_trigPartLambda->setFitx        (trigPartLambda->fitx());
//                                        xaod_trigPartLambda->setFity        (trigPartLambda->fity());
//                                        xaod_trigPartLambda->setFitz        (trigPartLambda->fitz());
//                                        
//                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartLambda,{*trkIt1,*trkIt2});
//                                        
//                                        delete trigPartLambda; // done with the old object now.
//                                        trigPartLambda =0;
//
//                                        
////                                         //ElementLink<xAOD::TrackParticleContainer> trackEL3(*xAODTrkParticleCont,itrk2);
////                                         //ElementLink<xAOD::TrackParticleContainer> trackEL4(*xAODTrkParticleCont,itrk1);
////                                         //ElementLink<xAOD::TrackParticleContainer> trackEL3( mapTrackToIndex[*trkIt1].roi == 1 ? *tracksRoiI1 : *tracksRoiI2, mapTrackToIndex[*trkIt1].index);
////                                         //ElementLink<xAOD::TrackParticleContainer> trackEL4( mapTrackToIndex[*trkIt2].roi == 1 ? *tracksRoiI1 : *tracksRoiI2, mapTrackToIndex[*trkIt2].index);
//// 	                                ElementLink<xAOD::TrackParticleContainer> trackEL3( mapTrackToIndex[*trkIt1].roi == 1 ? tracksRoiI1[0].dataID() : tracksRoiI2[0].dataID(), mapTrackToIndex[*trkIt1].index);
//// 					ElementLink<xAOD::IParticleContainer> ItrackEL3;
////                                         ItrackEL3.resetWithKeyAndIndex(mapTrackToIndex[*trkIt1].roi == 1 ? tracksRoiI1[0].dataID() : tracksRoiI2[0].dataID(), mapTrackToIndex[*trkIt1].index);
//// 
//// 	                                ElementLink<xAOD::TrackParticleContainer> trackEL4( mapTrackToIndex[*trkIt2].roi == 1 ? tracksRoiI1[0].dataID() : tracksRoiI2[0].dataID(), mapTrackToIndex[*trkIt2].index);
//// 					ElementLink<xAOD::IParticleContainer> ItrackEL4;
//// 					ItrackEL4.resetWithKeyAndIndex(mapTrackToIndex[*trkIt2].roi == 1 ? tracksRoiI1[0].dataID() : tracksRoiI2[0].dataID(), mapTrackToIndex[*trkIt2].index);
//
//                                        xaod_trigPartLambda->addTrackParticleLink(trackEL3);
//                                        xaod_trigPartLambda->addTrackParticleLink(trackEL4);
//                                        xaod_trigPartLambda->addParticleLink(ItrackEL3);
//                                        xaod_trigPartLambda->addParticleLink(ItrackEL4);
//                                        // m_trigBphysColl_X->push_back(trigPartLambda);
//                                        int iLambda = mTrigBphysColl_X->size() - 1;
//                                        ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*mTrigBphysColl_X,iLambda);
//                                        xaod_trigPartLbMuMuLambda->addTrackParticleLink(trackEL3);
//                                        xaod_trigPartLbMuMuLambda->addTrackParticleLink(trackEL4);
//                                        xaod_trigPartLbMuMuLambda->addParticleLink(ItrackEL3);
//                                        xaod_trigPartLbMuMuLambda->addParticleLink(ItrackEL4);
//                                        if(Found1Track) xaod_trigPartLbMuMuLambda->addTrackParticleLink(trackELmu1);
//                                        if(Found2Track) xaod_trigPartLbMuMuLambda->addTrackParticleLink(trackELmu2);
//                                        if(Found1Track) xaod_trigPartLbMuMuLambda->addParticleLink(ELmu1);
//                                        if(Found2Track) xaod_trigPartLbMuMuLambda->addParticleLink(ELmu2);
//                                        xaod_trigPartLbMuMuLambda->setSecondaryDecayLink(trigPartXEL);
//                                        // m_trigBphysColl_b->push_back(trigPartLbMuMuLambda);
//                                        
//                                        result = true;
//                                        
//                                        mon_LbMuMuLambda_n++;
//                                        
//                                        if(IdEvent!=m_lastEventPassedLb) {
//                                            m_countPassedEventsLb++;
//                                            m_lastEventPassedLb = IdEvent;
//                                        }
//                                        
//                                        //                     delete trigPartLbMuMuLambda;
//                                    }
//                                } // end if(m_doLb_LambdaMuMuDecay)
                                
                                //
                                // Bc -> mu mu Ds (Phi pi) part
                                //
                                if(m_doBc_DsMuMuDecay) {
                                    
                                    double xPhiMass = XMass(track1,track2,bS_to_Phi); // decay = bS_to_Phi to actually calculate Phi->KK mass
                                    
                                    if ( xPhiMass > m_lowerPhiDs_MassCut && xPhiMass < m_upperPhiDs_MassCut ) {
                                        
                                        double xPhiMuMuMass = XMuMuMass (*trackELmu1,*trackELmu2,track1,track2,bS_to_Phi);
                                        if ( xPhiMuMuMass < m_upperBc_DsMuMuMassCut - PIMASS ) {
                                            
                                            // 3rd loop over tracks
                                            //xAOD::TrackParticleContainer::const_iterator trkIt3 =  xAODTrkParticleCont->begin();
                                            std::vector<const xAOD::TrackParticle*>::const_iterator trkIt3 = merged_tracks.begin();

                                            for (int itrk3=0 ; trkIt3 != lastTrkIt; itrk3++, trkIt3++)
                                            {
                                                //const Trk::Track* track3 = (*trkIt3)->track();
                                                const xAOD::TrackParticle* track3 = *trkIt3;

                                                // looping over all tracks, so expect to same tracks, but should skip those combinations
                                                if(itrk3==itrk1 || itrk3==itrk2) {
                                                    ATH_MSG(DEBUG) << "Track " << track3 << " is the same as another" << endreq;
                                                    continue;
                                                }

                                                
                                                //ElementLink<xAOD::TrackParticleContainer> trackEL5(*xAODTrkParticleCont,itrk3);
                                                //ElementLink<xAOD::TrackParticleContainer> trackEL5( mapTrackToIndex[*trkIt3].roi == 1 ? *tracksRoiI1 : *tracksRoiI2, mapTrackToIndex[*trkIt3].index);
//                                                ElementLink<xAOD::TrackParticleContainer> trackEL5( mapTrackToIndex[*trkIt3].roi == 1 ? tracksRoiI1[0].dataID() : tracksRoiI2[0].dataID(), mapTrackToIndex[*trkIt3].index);
//                                                ElementLink<xAOD::IParticleContainer> ItrackEL5;
//                                                ItrackEL5.resetWithKeyAndIndex(mapTrackToIndex[*trkIt3].roi == 1 ? tracksRoiI1[0].dataID() : tracksRoiI2[0].dataID(), mapTrackToIndex[*trkIt3].index);
                                                
                                                ElementLink<xAOD::TrackParticleContainer> & trackEL5 = mapTrackToIndex[*trkIt3].elLink;
                                                ElementLink<xAOD::IParticleContainer> ItrackEL5;
                                                ItrackEL5.resetWithKeyAndIndex(trackEL5.dataID(), trackEL5.index());

                                                
                                                if (*trackEL5 != track3) {
                                                    ATH_MSG(FATAL) << "Track 3 doesn't match dereferenced elementlink: " << track3 << " " << *trackEL5 << endreq;
                                                    if ( timerSvc() ) m_TotTimer->stop();
                                                    return HLT::MISSING_FEATURE;
                                                }

                                                
                                                
                                                // Check that it is not muon track
//                                                 if(itrk3==piTrk || itrk3==miTrk) {
//                                                     ATH_MSG(DEBUG) << "Track " << track3 << " was matched to a muon, skip it" << endreq;
//                                                     continue;
//                                                 }
                                                if( !(Found1Track && isUnique(*trkIt3,*trackELmu1)) || !(Found2Track &&isUnique(*trkIt3,*trackELmu2)) ) {
                                                    ATH_MSG(DEBUG) << "Track " << track3 << " was matched to a muon, skip it" << endreq;
                                                    continue;
                                                }
                                                
                                                xAOD::TrigBphys* trigPartDs(nullptr);
                                                xAOD::TrigBphys* trigPartBcMuMuDs = checkBcMuMuDs(trackEL3,trackEL4,trackEL5,
                                                                                                  trackELmu1,trackELmu2,
                                                                                                  xPhiMass,trigPartDs);
                                                nTriedCombinations++;
                                                if (trigPartBcMuMuDs) {
                                                    if(m_maxBcToStore >= 0 && m_countBcToStore >= m_maxBcToStore) {
                                                      if(m_countBcToStore == m_maxBcToStore) {
                                                        ATH_MSG(WARNING) << "Reached maximum number of Bc candidates to store " << m_maxBcToStore << "; following candidates won't be written out" << endreq;
                                                        mon_Errors.push_back(ERROR_MaxNumBcReached);
                                                      }
                                                      else
                                                        ATH_MSG(DEBUG) << "Do not write out " << m_countBcToStore+1 << "th Bc candidate" << endreq;
                                                    }
                                                    else {
                                                      mTrigBphysColl_b->push_back( trigPartBcMuMuDs );
                                                      mTrigBphysColl_X->push_back( trigPartDs );
                                                    }

                                                    m_bphysHelperTool->setBeamlineDisplacement(trigPartBcMuMuDs,
                                                                                               {*trkIt1,*trkIt2,*trkIt3,*trackELmu1,*trackELmu2});
                                                    m_bphysHelperTool->setBeamlineDisplacement(trigPartDs,
                                                                                               {*trkIt1,*trkIt2,*trkIt3});
                                                    trigPartDs->addParticleLink(ItrackEL3);
                                                    trigPartDs->addParticleLink(ItrackEL4);
                                                    trigPartDs->addParticleLink(ItrackEL5);
                                                   
                                                    int iDs = mTrigBphysColl_X->size() - 1;
                                                    ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*mTrigBphysColl_X,iDs);
                                                    
                                                    trigPartBcMuMuDs->addParticleLink(ItrackEL3);
                                                    trigPartBcMuMuDs->addParticleLink(ItrackEL4);
                                                    trigPartBcMuMuDs->addParticleLink(ItrackEL5);
                                                    if(Found1Track) trigPartBcMuMuDs->addParticleLink(ELmu1);
                                                    if(Found2Track) trigPartBcMuMuDs->addParticleLink(ELmu2);
                                                    trigPartBcMuMuDs->setSecondaryDecayLink(trigPartXEL);
                                                    
                                                    result=true;
                                                    mon_BcMuMuDs_n++;
                                                    m_countBcToStore++;
                                                    if(IdEvent!=m_lastEventPassedBc) {
                                                        m_countPassedEventsBc++;
                                                        m_lastEventPassedBc = IdEvent;
                                                    }

                                                } // if trigPartBcMuMuDs
                                            } // for loop over track3
                                        } // in mass range
                                    } // in phi mass range
                                } // if m_doBc_DsMuMuDecay
                                
                                
                                                
//                                                TrigEFBphys* trigPartDs;
//                                                
//                                                TrigEFBphys* trigPartBcMuMuDs = checkBcMuMuDs(*mItr,*pItr,track1,track2, xPhiMass, track3,trigPartDs);
//                                                nTriedCombinations++;
//                                                if (trigPartBcMuMuDs) {
//                                                    
//                                                    //now make the xaod object
//                                                    xAOD::TrigBphys* xaod_trigPartBcMuMuDs = new xAOD::TrigBphys();
//                                                    mTrigBphysColl_b->push_back( xaod_trigPartBcMuMuDs );
//                                                    xaod_trigPartBcMuMuDs->initialise(0, 0., 0.,
//                                                                                          (xAOD::TrigBphys::pType)trigPartBcMuMuDs->particleType(),
//                                                                                          trigPartBcMuMuDs->mass(),
//                                                                                          xAOD::TrigBphys::EF );
//                                                    
//                                                    xAOD::TrigBphys* xaod_trigPartDs = new xAOD::TrigBphys();
//                                                    mTrigBphysColl_X->push_back( xaod_trigPartDs );
//                                                    xaod_trigPartDs->initialise(0, 0., 0.,
//                                                                                    (xAOD::TrigBphys::pType)trigPartDs->particleType(),
//                                                                                    trigPartDs->mass(),
//                                                                                    xAOD::TrigBphys::EF );
//                                                    
//                                                    
//                                                    xaod_trigPartBcMuMuDs->setFitmass     (trigPartBcMuMuDs->fitmass());
//                                                    xaod_trigPartBcMuMuDs->setFitchi2     (trigPartBcMuMuDs->fitchi2());
//                                                    xaod_trigPartBcMuMuDs->setFitndof     (trigPartBcMuMuDs->fitndof());
//                                                    xaod_trigPartBcMuMuDs->setFitx        (trigPartBcMuMuDs->fitx());
//                                                    xaod_trigPartBcMuMuDs->setFity        (trigPartBcMuMuDs->fity());
//                                                    xaod_trigPartBcMuMuDs->setFitz        (trigPartBcMuMuDs->fitz());
//                                                    
//                                                    m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartBcMuMuDs,{*trkIt1,*trkIt2,*trkIt3,*trackELmu1,*trackELmu2});
//                                                    
//                                                    delete trigPartBcMuMuDs; // done with the old object now.
//                                                    trigPartBcMuMuDs =0;
//                                                    
//                                                    xaod_trigPartDs->setFitmass     (trigPartDs->fitmass());
//                                                    xaod_trigPartDs->setFitchi2     (trigPartDs->fitchi2());
//                                                    xaod_trigPartDs->setFitndof     (trigPartDs->fitndof());
//                                                    xaod_trigPartDs->setFitx        (trigPartDs->fitx());
//                                                    xaod_trigPartDs->setFity        (trigPartDs->fity());
//                                                    xaod_trigPartDs->setFitz        (trigPartDs->fitz());
//                                                    
//                                                    m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartDs,{*trkIt1,*trkIt2,*trkIt3});
//                                                    
//                                                    delete trigPartDs; // done with the old object now.
//                                                    trigPartDs =0;
//
//                                                    xaod_trigPartDs->addTrackParticleLink(trackEL3);
//                                                    xaod_trigPartDs->addTrackParticleLink(trackEL4);
//                                                    xaod_trigPartDs->addTrackParticleLink(trackEL5);
//                                                    xaod_trigPartDs->addParticleLink(ItrackEL3);
//                                                    xaod_trigPartDs->addParticleLink(ItrackEL4);
//                                                    xaod_trigPartDs->addParticleLink(ItrackEL5);
//                                                    //m_trigBphysColl_X->push_back(trigPartDs);
//                                                    int iDs = mTrigBphysColl_X->size() - 1;
//                                                    ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*mTrigBphysColl_X,iDs);
//                                                    xaod_trigPartBcMuMuDs->addTrackParticleLink(trackEL3);
//                                                    xaod_trigPartBcMuMuDs->addTrackParticleLink(trackEL4);
//                                                    xaod_trigPartBcMuMuDs->addTrackParticleLink(trackEL5);
//                                                    xaod_trigPartBcMuMuDs->addParticleLink(ItrackEL3);
//                                                    xaod_trigPartBcMuMuDs->addParticleLink(ItrackEL4);
//                                                    xaod_trigPartBcMuMuDs->addParticleLink(ItrackEL5);
//                                                    if(Found1Track) xaod_trigPartBcMuMuDs->addTrackParticleLink(trackELmu1);
//                                                    if(Found2Track) xaod_trigPartBcMuMuDs->addTrackParticleLink(trackELmu2);
//                                                    if(Found1Track) xaod_trigPartBcMuMuDs->addParticleLink(ELmu1);
//                                                    if(Found2Track) xaod_trigPartBcMuMuDs->addParticleLink(ELmu2);
//                                                    xaod_trigPartBcMuMuDs->setSecondaryDecayLink(trigPartXEL);
//                                                    //m_trigBphysColl_b->push_back(trigPartBcMuMuDs);
//                                                    
//                                                    result=true;
//                                                    
//                                                    mon_BcMuMuDs_n++;
//                                                    
//                                                    if(IdEvent!=m_lastEventPassedBc) {
//                                                        m_countPassedEventsBc++;
//                                                        m_lastEventPassedBc = IdEvent;
//                                                    }
//                                                }
//                                            }// end of 3rd track loop
//                                        } // end if ( xPhiMuMuMass < m_upperBc_DsMuMuMassCut - PIMASS )
//                                    } // end if ( xPhiMass > m_lowerPhiDs_MassCut && xPhiMass < m_upperPhiDs_MassCut )
//                                } // end if(m_doBc_DsMuMuDecay)
                            } // end if(m_doBd_KstarMuMuDecay || m_doBs_Phi1020MuMuDecay || m_doLb_LambdaMuMuDecay)
                    } // end of inner loop over tracks 2
                    
                } // end of outer loop over tracks 1
            } // end of inner muon loop
        } // end of outer muon loop
    } // end if muidIDtracks1,2 sizes >= 0
    
    if (msgLvl() <= MSG::DEBUG) msg()  << MSG::DEBUG << "Totally tried to build " << nTriedCombinations << " tracks cobinations" << endreq;
    mon_nTriedCombinations = nTriedCombinations;
    
//     std::string  KEY; //sivokl
    
    if(mTrigBphysColl_b!=0 && mTrigBphysColl_b->size()>0) {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: Store Bphys Collection size: " << mTrigBphysColl_b->size() << endreq;
        HLT::ErrorCode sc = attachFeature(outputTE, mTrigBphysColl_X, "EFBMuMuXFex_X" );
        if(sc != HLT::OK) {
            msg() << MSG::WARNING << "Failed to store trigBphys_X Collection in outputTE" << endreq;
            mon_Errors.push_back(ERROR_BphysCollStore_Fails);
            delete mTrigBphysColl_b;
            delete mTrigBphysColl_X;
            if ( timerSvc() ) m_TotTimer->stop();
            return HLT::ERROR;
        }
        // Sergey S.: get persistified TrigBphysContainer of X decays
        ElementLinkVector<xAOD::TrigBphysContainer> ELvecTBPh;
        sc = getFeaturesLinks< xAOD::TrigBphysContainer, xAOD::TrigBphysContainer > (outputTE, ELvecTBPh, "EFBMuMuXFex_X");
//         const xAOD::TrigBphysContainer * TrigBphysColl_X_persist(0);
//         sc = getFeature(outputTE, TrigBphysColl_X_persist, "EFBMuMuXFex_X");
        if(sc != HLT::OK) {
        msg() << MSG::WARNING << "Failed to getFeaturesLinks trigBphys_X Collection in outputTE" << endreq;
        } else {
            for ( const auto eltp: ELvecTBPh) {
              if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "  ===== TrigBphys Container ElementLinks : " 
              << " index: "  << eltp.index()
              << " sgkey: "  << eltp.dataID()
              << " hashkey: "<< eltp.key()
              << " valid: "  << eltp.isValid()
              << " ptr: "    << (eltp.isValid() ? *eltp : nullptr)
              << endreq;
//               KEY = eltp.dataID();
            }
        }
        // Sergey S.: reset the secondary decay links to persistified container
        for( xAOD::TrigBphysContainer::const_iterator BPobj = mTrigBphysColl_b->begin(); BPobj != mTrigBphysColl_b->end(); ++BPobj) {
            if ((*BPobj)->secondaryDecayLink().isValid() ) {
              ElementLink<xAOD::TrigBphysContainer> secEL;
//               secEL.resetWithKeyAndIndex(KEY,(*BPobj)->secondaryDecayLink().index());
              // match transient secondary decay ELs with those from persistified container
              for(auto persistentSecEL : ELvecTBPh ) {
                if(*persistentSecEL == *(*BPobj)->secondaryDecayLink())
                  secEL = persistentSecEL;
              }
              if(!secEL.isValid())
                msg() << MSG::WARNING << "Failed to find persistified secondary decay EL" << endreq;
              else
                (*BPobj)->setSecondaryDecayLink(secEL);
              
              if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "  ===== TrigBphys secondaryDecay ElementLinks AFTER reset: "
              << "S link isValid = " << (*BPobj)->secondaryDecayLink().isValid() << std::endl
              << "S link  dataID = " << (*BPobj)->secondaryDecayLink().dataID() << std::endl
              << "S link   index = " << (*BPobj)->secondaryDecayLink().index() << std::endl
              << "S link     key = " << (*BPobj)->secondaryDecayLink().key() << std::endl
              << "S link    cptr = " << (*BPobj)->secondaryDecayLink().cptr() << std::endl
              << endreq;
            }
        }



	        
        sc = attachFeature(outputTE, mTrigBphysColl_b, "EFBMuMuXFex" );
        if(sc != HLT::OK) {
            msg() << MSG::WARNING << "Failed to store trigBphys Collection in outputTE" << endreq;
            mon_Errors.push_back(ERROR_BphysCollStore_Fails);
            delete mTrigBphysColl_b;
            delete mTrigBphysColl_X;
            if ( timerSvc() ) m_TotTimer->stop();
            return HLT::ERROR;
        }
    } else {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: no bphys collection to store "  << endreq;
        delete mTrigBphysColl_b;
        delete mTrigBphysColl_X;
    }
    
    if(result) {
        if(IdEvent!=m_lastEventPassed) {
            m_countPassedEvents++;
            m_lastEventPassed = IdEvent;
        }
        m_countPassedRoIs++;
    }
    
    if ( timerSvc() )  {
        m_TotTimer->stop();
        m_VtxFitTimer->resume();
        m_VtxFitTimer->stop();
        mon_TotalRunTime   = m_TotTimer->elapsed();
        mon_VertexingTime  = m_VtxFitTimer->elapsed();
    }
    
    if ( timerSvc() ) {
        ATH_MSG(DEBUG) << "m_TotTimer->elapsed()    = " << m_TotTimer->elapsed() << endreq;
        ATH_MSG(DEBUG) << "m_VtxFitTimer->elapsed() = " << m_VtxFitTimer->elapsed() << endreq;
    }
    
    return HLT::OK;
    
}

/*----------------------------------------------------------------------------*/
TrigEFBphys* TrigEFBMuMuXFex::checkBMuMu2X(const Trk::Track* mu1, const Trk::Track* mu2, const Trk::Track* track1, const Trk::Track* track2, const int decay, TrigEFBphys* & trigPartX)
{
    TrigEFBphys* trigPartXMuMu(0);
    
    double lowerXMassCut, upperXMassCut;
    double lowerXMuMuMassCut, upperXMuMuMassCut;
    double chi2XCut;
    double chi2XMuMuCut;
    std::string decayName;
    TrigEFBphys::pType decayType;
    unsigned int *countPassedXMass;
    unsigned int *countPassedXMuMuMass;
    unsigned int *countPassedXVtx;
    unsigned int *countPassedXVtxChi2;
    unsigned int *countPassedXMuMuVtx;
    unsigned int *countPassedXMuMuVtxChi2;
    
    bool do2XVertexing;
    bool doBMuMu2XVertexing;
    
    std::vector<float> *pMon_BMuMu2X_Pt_X1;
    std::vector<float> *pMon_BMuMu2X_Eta_X1;
    std::vector<float> *pMon_BMuMu2X_Phi_X1;
    std::vector<float> *pMon_BMuMu2X_Pt_X2;
    std::vector<float> *pMon_BMuMu2X_Eta_X2;
    std::vector<float> *pMon_BMuMu2X_Phi_X2;
    std::vector<float> *pMon_BMuMu2X_InvMass_2X;
    std::vector<float> *pMon_BMuMu2X_VtxMass_2X;
    std::vector<float> *pMon_BMuMu2X_Chi2_2X;
    std::vector<float> *pMon_BMuMu2X_InvMass_B;
    std::vector<float> *pMon_BMuMu2X_VtxMass_B;
    std::vector<float> *pMon_BMuMu2X_Chi2_B;
    
    // Fix silly warning messages
    (void)pMon_BMuMu2X_VtxMass_2X;
    (void)pMon_BMuMu2X_VtxMass_B;
    
    if(decay == bD_to_Kstar) {
        lowerXMassCut = m_lowerKstar_KaonMassCut;
        upperXMassCut = m_upperKstar_KaonMassCut;
        chi2XCut = m_kStarVtxChi2Cut;
        lowerXMuMuMassCut = m_lowerBd_KstarMuMuMassCut;
        upperXMuMuMassCut = m_upperBd_KstarMuMuMassCut;
        chi2XMuMuCut = m_bDVtxChi2Cut;
        decayName = "Bd -> mu mu K*";
        decayType = TrigEFBphys::BDKSTMUMU;
        countPassedXMass        = &m_countPassedKstarMass;
        countPassedXMuMuMass    = &m_countPassedBdMass;
        countPassedXVtx         = &m_countPassedKstarVtx;
        countPassedXVtxChi2     = &m_countPassedKstarVtxChi2;
        countPassedXMuMuVtx     = &m_countPassedBdVtx;
        countPassedXMuMuVtxChi2 = &m_countPassedBdVtxChi2;
        do2XVertexing      = m_doKstar_KPiVertexing;
        doBMuMu2XVertexing = m_doBd_KstarMuMuVertexing;
        pMon_BMuMu2X_Pt_X1      = &mon_BdMuMuKs_Pt_K;
        pMon_BMuMu2X_Eta_X1     = &mon_BdMuMuKs_Eta_K;
        pMon_BMuMu2X_Phi_X1     = &mon_BdMuMuKs_Phi_K;
        pMon_BMuMu2X_Pt_X2      = &mon_BdMuMuKs_Pt_Pi;
        pMon_BMuMu2X_Eta_X2     = &mon_BdMuMuKs_Eta_Pi;
        pMon_BMuMu2X_Phi_X2     = &mon_BdMuMuKs_Phi_Pi;
        pMon_BMuMu2X_InvMass_2X = &mon_BdMuMuKs_InvMass_Kstar;
        pMon_BMuMu2X_VtxMass_2X = &mon_BdMuMuKs_VtxMass_Kstar;
        pMon_BMuMu2X_Chi2_2X    = &mon_BdMuMuKs_Chi2_Kstar;
        pMon_BMuMu2X_InvMass_B  = &mon_BdMuMuKs_InvMass_Bd;
        pMon_BMuMu2X_VtxMass_B  = &mon_BdMuMuKs_VtxMass_Bd;
        pMon_BMuMu2X_Chi2_B     = &mon_BdMuMuKs_Chi2_Bd;
    } else if(decay == bS_to_Phi) {
        lowerXMassCut = m_lowerPhi1020_KaonMassCut;
        upperXMassCut = m_upperPhi1020_KaonMassCut;
        chi2XCut = m_phi1020VtxChi2Cut;
        lowerXMuMuMassCut = m_lowerBs_Phi1020MuMuMassCut;
        upperXMuMuMassCut = m_upperBs_Phi1020MuMuMassCut;
        chi2XMuMuCut = m_bSVtxChi2Cut;
        decayName = "Bs -> mu mu phi";
        decayType = TrigEFBphys::BSPHIMUMU;
        countPassedXMass        = &m_countPassedPhi1020Mass;
        countPassedXMuMuMass    = &m_countPassedBsMass;
        countPassedXVtx         = &m_countPassedPhi1020Vtx;
        countPassedXVtxChi2     = &m_countPassedPhi1020VtxChi2;
        countPassedXMuMuVtx     = &m_countPassedBsVtx;
        countPassedXMuMuVtxChi2 = &m_countPassedBsVtxChi2;
        do2XVertexing      = m_doPhi1020_KKVertexing;
        doBMuMu2XVertexing = m_doBs_Phi1020MuMuVertexing;
        pMon_BMuMu2X_Pt_X1      = &mon_BsMuMuPhi_Pt_K1;
        pMon_BMuMu2X_Eta_X1     = &mon_BsMuMuPhi_Eta_K1;
        pMon_BMuMu2X_Phi_X1     = &mon_BsMuMuPhi_Phi_K1;
        pMon_BMuMu2X_Pt_X2      = &mon_BsMuMuPhi_Pt_K2;
        pMon_BMuMu2X_Eta_X2     = &mon_BsMuMuPhi_Eta_K2;
        pMon_BMuMu2X_Phi_X2     = &mon_BsMuMuPhi_Phi_K2;
        pMon_BMuMu2X_InvMass_2X = &mon_BsMuMuPhi_InvMass_Phi1020;
        pMon_BMuMu2X_VtxMass_2X = &mon_BsMuMuPhi_VtxMass_Phi1020;
        pMon_BMuMu2X_Chi2_2X    = &mon_BsMuMuPhi_Chi2_Phi1020;
        pMon_BMuMu2X_InvMass_B  = &mon_BsMuMuPhi_InvMass_Bs;
        pMon_BMuMu2X_VtxMass_B  = &mon_BsMuMuPhi_VtxMass_Bs;
        pMon_BMuMu2X_Chi2_B     = &mon_BsMuMuPhi_Chi2_Bs;
    } else if(decay == lB_to_L) {
        lowerXMassCut = m_lowerLambda_PrPiMassCut;
        upperXMassCut = m_upperLambda_PrPiMassCut;
        chi2XCut = m_lambdaVtxChi2Cut;
        lowerXMuMuMassCut = m_lowerLb_LambdaMuMuMassCut;
        upperXMuMuMassCut = m_upperLb_LambdaMuMuMassCut;
        chi2XMuMuCut = m_lBVtxChi2Cut;
        decayName = "Lambda_b -> mu mu Lambda";
        decayType = TrigEFBphys::LBLMUMU;
        countPassedXMass        = &m_countPassedLambdaMass;
        countPassedXMuMuMass    = &m_countPassedLbMass;
        countPassedXVtx         = &m_countPassedLambdaVtx;
        countPassedXVtxChi2     = &m_countPassedLambdaVtxChi2;
        countPassedXMuMuVtx     = &m_countPassedLbVtx;
        countPassedXMuMuVtxChi2 = &m_countPassedLbVtxChi2;
        do2XVertexing      = m_doLambda_PPiVertexing;
        doBMuMu2XVertexing = m_doLb_LambdaMuMuVertexing;
        pMon_BMuMu2X_Pt_X1      = &mon_LbMuMuLambda_Pt_P;
        pMon_BMuMu2X_Eta_X1     = &mon_LbMuMuLambda_Eta_P;
        pMon_BMuMu2X_Phi_X1     = &mon_LbMuMuLambda_Phi_P;
        pMon_BMuMu2X_Pt_X2      = &mon_LbMuMuLambda_Pt_Pi;
        pMon_BMuMu2X_Eta_X2     = &mon_LbMuMuLambda_Eta_Pi;
        pMon_BMuMu2X_Phi_X2     = &mon_LbMuMuLambda_Phi_Pi;
        pMon_BMuMu2X_InvMass_2X = &mon_LbMuMuLambda_InvMass_Lambda;
        pMon_BMuMu2X_VtxMass_2X = &mon_LbMuMuLambda_VtxMass_Lambda;
        pMon_BMuMu2X_Chi2_2X    = &mon_LbMuMuLambda_Chi2_Lambda;
        pMon_BMuMu2X_InvMass_B  = &mon_LbMuMuLambda_InvMass_Lb;
        pMon_BMuMu2X_VtxMass_B  = &mon_LbMuMuLambda_VtxMass_Lb;
        pMon_BMuMu2X_Chi2_B     = &mon_LbMuMuLambda_Chi2_Lb;
    } else {
        ATH_MSG(DEBUG) << "Wrong decay identifier passed to checkBMuMu2X: decay = " << decay << endreq;
        mon_Errors.push_back(ERROR_WrongDecayID);
        return trigPartXMuMu;
    }
    
    ATH_MSG(DEBUG) << "Try to build " << decayName << " with tracks " << track1 << ", " << track2 << endreq;
    
    float massX = XMass(track1, track2, decay);
    if( !(massX > lowerXMassCut && massX < upperXMassCut) ) {
        ATH_MSG(DEBUG) << " " << decayName << " candidate rejected by X mass cut: m = " << massX << endreq;
    } else {
        (*countPassedXMass)++;
        
        float massXMuMu = XMuMuMass(mu1, mu2, track1, track2, decay);
        if( !(massXMuMu > lowerXMuMuMassCut && massXMuMu < upperXMuMuMassCut) ) {
            ATH_MSG(DEBUG) << " " << decayName << " candidate rejected by XMuMu mass cut: m = " << massXMuMu << endreq;
        } else {
            (*countPassedXMuMuMass)++;
            
            if(do2XVertexing) {
                // Try to fit X -> track1 track2 vertex
                if ( timerSvc() ) m_VtxFitTimer->resume();
                Amg::Vector3D vtx ( 0.,0.,0. );
                Trk::Vertex vertex ( vtx );
                std::vector<const Trk::Track*> trackPair;
                trackPair.push_back(track1);
                trackPair.push_back(track2);
                Trk::VxCandidate * XVxCandidate ( 0 );
                XVxCandidate = m_fitterSvc->fit(trackPair,vertex);
                if ( timerSvc() ) m_VtxFitTimer->pause();
                
                if( !XVxCandidate ) {
                    ATH_MSG(DEBUG) << " Failed to fit X vertex for " << decayName << endreq;
                    // mon_Errors.push_back(ERROR_XVtxFit_Fails);
                } else {
                    (*countPassedXVtx)++;
                    
                    float chi2X = XVxCandidate->recVertex().fitQuality().chiSquared();
                    if( !(chi2X < chi2XCut) ) {
                        ATH_MSG(DEBUG) << " " << decayName << " candidate rejected by X vertex chi2 cut: chi2 = " << chi2X << endreq;
                    } else {
                        (*countPassedXVtxChi2)++;
                        
                        // calculate X mass from vertexing
                        double vtxMassX(-1.), vtxMassErrorX(-1.);
                        std::vector<int> trkIndicesX;
                        for (int i=0;i<(int)trackPair.size();++i) {trkIndicesX.push_back(1);}
                        if( !(m_VKVFitter->VKalGetMassError(trkIndicesX,vtxMassX,vtxMassErrorX).isSuccess()) ) {
                            ATH_MSG(DEBUG) << "Warning from VKalVrt - cannot calculate fitmass and error for X in " << decayName << "!" << endreq;
                            mon_Errors.push_back(ERROR_XVtxMass_Fails);
                        }
                        
                        if(doBMuMu2XVertexing) {
                            // Try to fit XMuMu -> mu mu X vertex
                            if ( timerSvc() ) m_VtxFitTimer->resume();
                            std::vector<const Trk::Track*> quartet;
                            quartet.push_back(mu1);
                            quartet.push_back(mu2);
                            quartet.push_back(track1);
                            quartet.push_back(track2);
                            Trk::VxCandidate * XMuMuVxCandidate ( 0 );
                            XMuMuVxCandidate = m_fitterSvc->fit(quartet,vertex);
                            if ( timerSvc() ) m_VtxFitTimer->pause();
                            
                            if( !XMuMuVxCandidate ) {
                                ATH_MSG(DEBUG) << " Failed to fit XMuMu vertex for " << decayName << endreq;
                                // mon_Errors.push_back(ERROR_XMuMuVtxFit_Fails);
                            } else {
                                (*countPassedXMuMuVtx)++;
                                
                                float chi2XMuMu = XMuMuVxCandidate->recVertex().fitQuality().chiSquared();
                                if( !(chi2XMuMu < chi2XMuMuCut) ) {
                                    ATH_MSG(DEBUG) << " " << decayName << " candidate rejected by XMuMu vertex chi2 cut: chi2 = " << chi2XMuMu << endreq;
                                } else {
                                    (*countPassedXMuMuVtxChi2)++;
                                    
                                    //                   // calculate XMuMu mass from vertexing
                                    //                   double vtxMassXMuMu(-1.), vtxMassErrorXMuMu(-1.);
                                    //                   std::vector<int> trkIndicesXMuMu;
                                    //                   for (int i=0;i<(int)quartet.size();++i) {trkIndicesXMuMu.push_back(1);}
                                    //                   if( !(m_VKVFitter->VKalGetMassError(trkIndicesXMuMu,vtxMassXMuMu,vtxMassErrorXMuMu).isSuccess()) ) {
                                    //                     ATH_MSG(DEBUG) << "Warning from VKalVrt - cannot calculate fitmass and error for XMuMu in " << decayName << "!" << endreq;
                                    //                     mon_Errors.push_back(ERROR_XMuMuVtxMass_Fails);
                                    //                   }
                                    
                                    trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::PHIKK, massX);
                                    trigPartX->fitchi2(XVxCandidate->recVertex().fitQuality().chiSquared());
                                    //                   trigPartX->fitmass(vtxMassX);
                                    trigPartX->fitx(XVxCandidate->recVertex().position() [0]);
                                    trigPartX->fity(XVxCandidate->recVertex().position() [1]);
                                    trigPartX->fitz(XVxCandidate->recVertex().position() [2]);
                                    
                                    //                   trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BMUMUX, massXMuMu);
                                    trigPartXMuMu = new TrigEFBphys( 0, 0., 0., decayType, massXMuMu);
                                    trigPartXMuMu->fitchi2(XMuMuVxCandidate->recVertex().fitQuality().chiSquared());
                                    //                   trigPartXMuMu->fitmass(vtxMassXMuMu);
                                    trigPartXMuMu->fitx(XMuMuVxCandidate->recVertex().position() [0]);
                                    trigPartXMuMu->fity(XMuMuVxCandidate->recVertex().position() [1]);
                                    trigPartXMuMu->fitz(XMuMuVxCandidate->recVertex().position() [2]);
                                    
                                    // Fill BMuMu2X monitoring containers
                                    pMon_BMuMu2X_Pt_X1->push_back(track1->perigeeParameters()->pT()/1000.);
                                    pMon_BMuMu2X_Eta_X1->push_back(track1->perigeeParameters()->eta());
                                    pMon_BMuMu2X_Phi_X1->push_back(track1->perigeeParameters()->parameters()[Trk::phi0]);
                                    pMon_BMuMu2X_Pt_X2->push_back(track2->perigeeParameters()->pT()/1000.);
                                    pMon_BMuMu2X_Eta_X2->push_back(track2->perigeeParameters()->eta());
                                    pMon_BMuMu2X_Phi_X2->push_back(track2->perigeeParameters()->parameters()[Trk::phi0]);
                                    pMon_BMuMu2X_InvMass_2X->push_back(massX/1000.);
                                    //                   pMon_BMuMu2X_VtxMass_2X->push_back(vtxMassX/1000.);
                                    pMon_BMuMu2X_Chi2_2X->push_back(chi2X);
                                    pMon_BMuMu2X_InvMass_B->push_back(massXMuMu/1000.);
                                    //                   pMon_BMuMu2X_VtxMass_B->push_back(vtxMassXMuMu/1000.);
                                    pMon_BMuMu2X_Chi2_B->push_back(chi2XMuMu);
                                    
                                    if(msgLvl() <= MSG::DEBUG)
                                        msg() << MSG::DEBUG << " Good " << decayName << " found!" << std::endl
                                        << "  m = " << trigPartXMuMu->mass() << ", "
                                        << "chi2 = " << trigPartXMuMu->fitchi2() << ", vertex (" << trigPartXMuMu->fitx() << ", "
                                        << trigPartXMuMu->fity() << ", " << trigPartXMuMu->fitz() << ")" << endreq;
                                } // end XMuMu chi2 cut
                            } // end if(XMuMuVxCandidate)
                            delete XMuMuVxCandidate;
                        } // end if(doBMuMu2XVertexing), do2XVertexing
                        else {
                            trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::PHIKK, massX);
                            trigPartX->fitchi2(XVxCandidate->recVertex().fitQuality().chiSquared());
                            //               trigPartX->fitmass(vtxMassX);
                            trigPartX->fitx(XVxCandidate->recVertex().position() [0]);
                            trigPartX->fity(XVxCandidate->recVertex().position() [1]);
                            trigPartX->fitz(XVxCandidate->recVertex().position() [2]);
                            
                            //               trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BMUMUX, massXMuMu);
                            trigPartXMuMu = new TrigEFBphys( 0, 0., 0., decayType, massXMuMu);
                            //               trigPartXMuMu->fitchi2(XMuMuVxCandidate->recVertex().fitQuality().chiSquared());
                            //               trigPartXMuMu->fitmass(vtxMassXMuMu);
                            //               trigPartXMuMu->fitx(XMuMuVxCandidate->recVertex().position() [0]);
                            //               trigPartXMuMu->fity(XMuMuVxCandidate->recVertex().position() [1]);
                            //               trigPartXMuMu->fitz(XMuMuVxCandidate->recVertex().position() [2]);
                            
                            // Fill BMuMu2X monitoring containers
                            pMon_BMuMu2X_Pt_X1->push_back(track1->perigeeParameters()->pT()/1000.);
                            pMon_BMuMu2X_Eta_X1->push_back(track1->perigeeParameters()->eta());
                            pMon_BMuMu2X_Phi_X1->push_back(track1->perigeeParameters()->parameters()[Trk::phi0]);
                            pMon_BMuMu2X_Pt_X2->push_back(track2->perigeeParameters()->pT()/1000.);
                            pMon_BMuMu2X_Eta_X2->push_back(track2->perigeeParameters()->eta());
                            pMon_BMuMu2X_Phi_X2->push_back(track2->perigeeParameters()->parameters()[Trk::phi0]);
                            pMon_BMuMu2X_InvMass_2X->push_back(massX/1000.);
                            //               pMon_BMuMu2X_VtxMass_2X->push_back(vtxMassX/1000.);
                            pMon_BMuMu2X_Chi2_2X->push_back(chi2X);
                            pMon_BMuMu2X_InvMass_B->push_back(massXMuMu/1000.);
                            //               pMon_BMuMu2X_VtxMass_B->push_back(vtxMassXMuMu/1000.);
                            //               pMon_BMuMu2X_Chi2_B->push_back(chi2XMuMu);
                            
                            if(msgLvl() <= MSG::DEBUG)
                                msg() << MSG::DEBUG << " Good " << decayName << " found (no BMuMu2X vertexing)!" << std::endl
                                << "  m = " << trigPartXMuMu->mass() << endreq;
                        } // end if(!doBMuMu2XVertexing), do2XVertexing
                        
                    } // end X chi2 cut
                } // end if(XVxCandidate)
                delete XVxCandidate;
            } // end if(do2XVertexing)
            else {
                if(doBMuMu2XVertexing) { // !do2XVertexing, STUPID CONFIGURATION
                    // Try to fit XMuMu -> mu mu X vertex
                    Amg::Vector3D vtx ( 0.,0.,0. );
                    Trk::Vertex vertex ( vtx );
                    if ( timerSvc() ) m_VtxFitTimer->resume();
                    std::vector<const Trk::Track*> quartet;
                    quartet.push_back(mu1);
                    quartet.push_back(mu2);
                    quartet.push_back(track1);
                    quartet.push_back(track2);
                    Trk::VxCandidate * XMuMuVxCandidate ( 0 );
                    XMuMuVxCandidate = m_fitterSvc->fit(quartet,vertex);
                    if ( timerSvc() ) m_VtxFitTimer->pause();
                    
                    if( !XMuMuVxCandidate ) {
                        ATH_MSG(DEBUG) << " Failed to fit XMuMu vertex for " << decayName << endreq;
                        // mon_Errors.push_back(ERROR_XMuMuVtxFit_Fails);
                    } else {
                        (*countPassedXMuMuVtx)++;
                        
                        float chi2XMuMu = XMuMuVxCandidate->recVertex().fitQuality().chiSquared();
                        if( !(chi2XMuMu < chi2XMuMuCut) ) {
                            ATH_MSG(DEBUG) << " " << decayName << " candidate rejected by XMuMu vertex chi2 cut: chi2 = " << chi2XMuMu << endreq;
                        } else {
                            (*countPassedXMuMuVtxChi2)++;
                            
                            //               // calculate XMuMu mass from vertexing
                            //               double vtxMassXMuMu(-1.), vtxMassErrorXMuMu(-1.);
                            //               std::vector<int> trkIndicesXMuMu;
                            //               for (int i=0;i<(int)quartet.size();++i) {trkIndicesXMuMu.push_back(1);}
                            //               if( !(m_VKVFitter->VKalGetMassError(trkIndicesXMuMu,vtxMassXMuMu,vtxMassErrorXMuMu).isSuccess()) ) {
                            //                 ATH_MSG(DEBUG) << "Warning from VKalVrt - cannot calculate fitmass and error for XMuMu in " << decayName << "!" << endreq;
                            //                 mon_Errors.push_back(ERROR_XMuMuVtxMass_Fails);
                            //               }
                            
                            trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::PHIKK, massX);
                            
                            //               trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BMUMUX, massXMuMu);
                            trigPartXMuMu = new TrigEFBphys( 0, 0., 0., decayType, massXMuMu);
                            trigPartXMuMu->fitchi2(XMuMuVxCandidate->recVertex().fitQuality().chiSquared());
                            //               trigPartXMuMu->fitmass(vtxMassXMuMu);
                            trigPartXMuMu->fitx(XMuMuVxCandidate->recVertex().position() [0]);
                            trigPartXMuMu->fity(XMuMuVxCandidate->recVertex().position() [1]);
                            trigPartXMuMu->fitz(XMuMuVxCandidate->recVertex().position() [2]);
                            
                            // Fill BMuMu2X monitoring containers
                            pMon_BMuMu2X_Pt_X1->push_back(track1->perigeeParameters()->pT()/1000.);
                            pMon_BMuMu2X_Eta_X1->push_back(track1->perigeeParameters()->eta());
                            pMon_BMuMu2X_Phi_X1->push_back(track1->perigeeParameters()->parameters()[Trk::phi0]);
                            pMon_BMuMu2X_Pt_X2->push_back(track2->perigeeParameters()->pT()/1000.);
                            pMon_BMuMu2X_Eta_X2->push_back(track2->perigeeParameters()->eta());
                            pMon_BMuMu2X_Phi_X2->push_back(track2->perigeeParameters()->parameters()[Trk::phi0]);
                            pMon_BMuMu2X_InvMass_2X->push_back(massX/1000.);
                            //               pMon_BMuMu2X_VtxMass_2X->push_back(vtxMassX/1000.);
                            //               pMon_BMuMu2X_Chi2_2X->push_back(chi2X);
                            pMon_BMuMu2X_InvMass_B->push_back(massXMuMu/1000.);
                            //               pMon_BMuMu2X_VtxMass_B->push_back(vtxMassXMuMu/1000.);
                            pMon_BMuMu2X_Chi2_B->push_back(chi2XMuMu);
                            
                            if(msgLvl() <= MSG::DEBUG)
                                msg() << MSG::DEBUG << " Good " << decayName << " found (no 2X vertexing)!" << std::endl
                                << "  m = " << trigPartXMuMu->mass() << ", "
                                << "chi2 = " << trigPartXMuMu->fitchi2()
                                << ", vertex ("
                                << trigPartXMuMu->fitx() << ", "
                                << trigPartXMuMu->fity() << ", "
                                << trigPartXMuMu->fitz() << ")" << endreq;
                        } // end XMuMu chi2 cut
                    } // end if(XMuMuVxCandidate)
                    delete XMuMuVxCandidate;
                } // end if(doBMuMu2XVertexing), !do2XVertexing
                else {
                    trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::PHIKK, massX);
                    
                    //           trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BMUMUX, massXMuMu);
                    trigPartXMuMu = new TrigEFBphys( 0, 0., 0., decayType, massXMuMu);
                    
                    // Fill BMuMu2X monitoring containers
                    pMon_BMuMu2X_Pt_X1->push_back(track1->perigeeParameters()->pT()/1000.);
                    pMon_BMuMu2X_Eta_X1->push_back(track1->perigeeParameters()->eta());
                    pMon_BMuMu2X_Phi_X1->push_back(track1->perigeeParameters()->parameters()[Trk::phi0]);
                    pMon_BMuMu2X_Pt_X2->push_back(track2->perigeeParameters()->pT()/1000.);
                    pMon_BMuMu2X_Eta_X2->push_back(track2->perigeeParameters()->eta());
                    pMon_BMuMu2X_Phi_X2->push_back(track2->perigeeParameters()->parameters()[Trk::phi0]);
                    pMon_BMuMu2X_InvMass_2X->push_back(massX/1000.);
                    //               pMon_BMuMu2X_VtxMass_2X->push_back(vtxMassX/1000.);
                    //               pMon_BMuMu2X_Chi2_2X->push_back(chi2X);
                    pMon_BMuMu2X_InvMass_B->push_back(massXMuMu/1000.);
                    //           pMon_BMuMu2X_VtxMass_B->push_back(vtxMassXMuMu/1000.);
                    //           pMon_BMuMu2X_Chi2_B->push_back(chi2XMuMu);
                    
                    if(msgLvl() <= MSG::DEBUG)
                        msg() << MSG::DEBUG << " Good " << decayName << " found (no 2X vertexing, no BMuMu2X vertexing)!" << std::endl
                        << "  m = " << trigPartXMuMu->mass() << endreq;
                }// end if(!doBMuMu2XVertexing), !do2XVertexing
                
            } // end if(!do2XVertexing)
            
        } // end if XMuMu mass cut
    } // end if X mass cut
    
    return trigPartXMuMu;
    
}

/*----------------------------------------------------------------------------*/
TrigEFBphys* TrigEFBMuMuXFex::checkBcMuMuDs(const Trk::Track* mu1, const Trk::Track* mu2, const Trk::Track* track1, const Trk::Track* track2, double xPhiMass, const Trk::Track* track3, TrigEFBphys* & trigPartX)
{
    TrigEFBphys* trigPartXMuMu(0);
    
    m_countPassedPhiDsMass++;
    
    ATH_MSG(DEBUG) << "Try to build " << "D_s" << " with tracks " << track1 << ", " << track2 << ", " << track3 << endreq;
    
    float massX = X3Mass(track1, track2, track3);
    if( !(massX > m_lowerDs_MassCut && massX < m_upperDs_MassCut) ) {
        ATH_MSG(DEBUG) << " " << "D_s" << " candidate rejected by the mass cut: m = " << massX << endreq;
    } else {
        m_countPassedDsMass++;
        
        float massXMuMu = X3MuMuMass(mu1, mu2, track1, track2, track3);
        if( !(massXMuMu > m_lowerBc_DsMuMuMassCut && massXMuMu < m_upperBc_DsMuMuMassCut) ) {
            ATH_MSG(DEBUG) << " " << "B_c" << " candidate rejected by the mass cut: m = " << massXMuMu << endreq;
        } else {
            m_countPassedBcMass++;
            
            if(m_doDs_Vertexing) {
                // Try to fit X -> track1 track2 track3 vertex
                if ( timerSvc() ) m_VtxFitTimer->resume();
                Amg::Vector3D vtx ( 0.,0.,0. );
                Trk::Vertex vertex ( vtx );
                std::vector<const Trk::Track*> trackTroika;
                trackTroika.push_back(track1);
                trackTroika.push_back(track2);
                trackTroika.push_back(track3);
                Trk::VxCandidate * XVxCandidate ( 0 );
                XVxCandidate = m_fitterSvc->fit(trackTroika,vertex);
                if ( timerSvc() ) m_VtxFitTimer->pause();
                
                if( !XVxCandidate ) {
                    ATH_MSG(DEBUG) << " Failed to fit X vertex for " << "D_s" << endreq;
                    // mon_Errors.push_back(ERROR_XVtxFit_Fails);
                } else {
                    m_countPassedDsVtx++;
                    
                    float chi2X = XVxCandidate->recVertex().fitQuality().chiSquared();
                    if( !(chi2X < m_DsVtxChi2Cut) ) {
                        ATH_MSG(DEBUG) << " " << "D_s" << " candidate rejected by X vertex chi2 cut: chi2 = " << chi2X << endreq;
                    } else {
                        m_countPassedDsVtxChi2++;
                        
                        // calculate X mass from vertexing
                        double vtxMassX(-1.), vtxMassErrorX(-1.);
                        std::vector<int> trkIndicesX;
                        for (int i=0;i<(int)trackTroika.size();++i) {trkIndicesX.push_back(1);}
                        if( !(m_VKVFitter->VKalGetMassError(trkIndicesX,vtxMassX,vtxMassErrorX).isSuccess()) ) {
                            ATH_MSG(DEBUG) << "Warning from VKalVrt - cannot calculate fitmass and error for X in " << "D_s" << "!" << endreq;
                            mon_Errors.push_back(ERROR_XVtxMass_Fails);
                        }
                        
                        if(m_doBc_DsMuMuVertexing) {
                            // Try to fit XMuMu -> mu mu X vertex
                            if ( timerSvc() ) m_VtxFitTimer->resume();
                            std::vector<const Trk::Track*> quintet;
                            quintet.push_back(mu1);
                            quintet.push_back(mu2);
                            quintet.push_back(track1);
                            quintet.push_back(track2);
                            quintet.push_back(track3);
                            Trk::VxCandidate * XMuMuVxCandidate ( 0 );
                            XMuMuVxCandidate = m_fitterSvc->fit(quintet,vertex);
                            if ( timerSvc() ) m_VtxFitTimer->pause();
                            
                            if( !XMuMuVxCandidate ) {
                                ATH_MSG(DEBUG) << " Failed to fit XMuMu vertex for " << "B_c" << endreq;
                                // mon_Errors.push_back(ERROR_XMuMuVtxFit_Fails);
                            } else {
                                m_countPassedBcVtx++;
                                
                                float chi2XMuMu = XMuMuVxCandidate->recVertex().fitQuality().chiSquared();
                                if( !(chi2XMuMu < m_bCVtxChi2Cut) ) {
                                    ATH_MSG(DEBUG) << " " << "B_c" << " candidate rejected by XMuMu vertex chi2 cut: chi2 = " << chi2XMuMu << endreq;
                                } else {
                                    m_countPassedBcVtxChi2++;
                                    
                                    //                  trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::PHIKK, xPhiMass);
                                    trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::DSPHIPI, massX);
                                    trigPartX->fitchi2(XVxCandidate->recVertex().fitQuality().chiSquared());
                                    //                   trigPartX->fitmass(vtxMassX);
                                    trigPartX->fitx(XVxCandidate->recVertex().position() [0]);
                                    trigPartX->fity(XVxCandidate->recVertex().position() [1]);
                                    trigPartX->fitz(XVxCandidate->recVertex().position() [2]);
                                    
                                    trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BCDSMUMU, massXMuMu);
                                    trigPartXMuMu->fitchi2(XMuMuVxCandidate->recVertex().fitQuality().chiSquared());
                                    //                   trigPartXMuMu->fitmass(vtxMassXMuMu);
                                    trigPartXMuMu->fitx(XMuMuVxCandidate->recVertex().position() [0]);
                                    trigPartXMuMu->fity(XMuMuVxCandidate->recVertex().position() [1]);
                                    trigPartXMuMu->fitz(XMuMuVxCandidate->recVertex().position() [2]);
                                    
                                    // Fill BMuMu2X monitoring containers
                                    mon_BcMuMuDs_Pt_K1.push_back(track1->perigeeParameters()->pT()/1000.);
                                    mon_BcMuMuDs_Eta_K1.push_back(track1->perigeeParameters()->eta());
                                    mon_BcMuMuDs_Phi_K1.push_back(track1->perigeeParameters()->parameters()[Trk::phi0]);
                                    mon_BcMuMuDs_Pt_K2.push_back(track2->perigeeParameters()->pT()/1000.);
                                    mon_BcMuMuDs_Eta_K2.push_back(track2->perigeeParameters()->eta());
                                    mon_BcMuMuDs_Phi_K2.push_back(track2->perigeeParameters()->parameters()[Trk::phi0]);
                                    mon_BcMuMuDs_Pt_pi.push_back(track3->perigeeParameters()->pT()/1000.);
                                    mon_BcMuMuDs_Eta_pi.push_back(track3->perigeeParameters()->eta());
                                    mon_BcMuMuDs_Phi_pi.push_back(track3->perigeeParameters()->parameters()[Trk::phi0]);
                                    mon_BcMuMuDs_InvMass_PhiDs.push_back(xPhiMass/1000.);
                                    mon_BcMuMuDs_InvMass_Ds.push_back(massX/1000.);
                                    //                   mon_BcMuMuDs_VtxMass_Ds.push_back(vtxMassX/1000.);
                                    mon_BcMuMuDs_Chi2_Ds.push_back(chi2X);
                                    mon_BcMuMuDs_InvMass_Bc.push_back(massXMuMu/1000.);
                                    //                   mon_BcMuMuDs_VtxMass_Bc.push_back(vtxMassXMuMu/1000.);
                                    mon_BcMuMuDs_Chi2_Bc.push_back(chi2XMuMu);
                                    
                                    if(msgLvl() <= MSG::DEBUG)
                                        msg() << MSG::DEBUG << " Good " << "B_c" << " found!" << std::endl
                                        << "  m = " << trigPartXMuMu->mass() << ", "
                                        << "chi2 = " << trigPartXMuMu->fitchi2()
                                        << ", vertex ("
                                        << trigPartXMuMu->fitx() << ", "
                                        << trigPartXMuMu->fity() << ", "
                                        << trigPartXMuMu->fitz() << ")" << endreq;
                                } // end XMuMu chi2 cut
                            } // end if(XMuMuVxCandidate)
                            delete XMuMuVxCandidate;
                        } // end if(doBMuMu2XVertexing), do2XVertexing
                        else {
                            //              trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::PHIKK, xPhiMass);
                            trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::DSPHIPI, massX);
                            trigPartX->fitchi2(XVxCandidate->recVertex().fitQuality().chiSquared());
                            //               trigPartX->fitmass(vtxMassX);
                            trigPartX->fitx(XVxCandidate->recVertex().position() [0]);
                            trigPartX->fity(XVxCandidate->recVertex().position() [1]);
                            trigPartX->fitz(XVxCandidate->recVertex().position() [2]);
                            
                            //               trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BCDSMUMU, massXMuMu);
                            trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BCDSMUMU, massXMuMu);
                            //               trigPartXMuMu->fitchi2(XMuMuVxCandidate->recVertex().fitQuality().chiSquared());
                            //               trigPartXMuMu->fitmass(vtxMassXMuMu);
                            //               trigPartXMuMu->fitx(XMuMuVxCandidate->recVertex().position() [0]);
                            //               trigPartXMuMu->fity(XMuMuVxCandidate->recVertex().position() [1]);
                            //               trigPartXMuMu->fitz(XMuMuVxCandidate->recVertex().position() [2]);
                            
                            // Fill BMuMu2X monitoring containers
                            mon_BcMuMuDs_Pt_K1.push_back(track1->perigeeParameters()->pT()/1000.);
                            mon_BcMuMuDs_Eta_K1.push_back(track1->perigeeParameters()->eta());
                            mon_BcMuMuDs_Phi_K1.push_back(track1->perigeeParameters()->parameters()[Trk::phi0]);
                            mon_BcMuMuDs_Pt_K2.push_back(track2->perigeeParameters()->pT()/1000.);
                            mon_BcMuMuDs_Eta_K2.push_back(track2->perigeeParameters()->eta());
                            mon_BcMuMuDs_Phi_K2.push_back(track2->perigeeParameters()->parameters()[Trk::phi0]);
                            mon_BcMuMuDs_Pt_pi.push_back(track3->perigeeParameters()->pT()/1000.);
                            mon_BcMuMuDs_Eta_pi.push_back(track3->perigeeParameters()->eta());
                            mon_BcMuMuDs_Phi_pi.push_back(track3->perigeeParameters()->parameters()[Trk::phi0]);
                            mon_BcMuMuDs_InvMass_PhiDs.push_back(xPhiMass/1000.);
                            mon_BcMuMuDs_InvMass_Ds.push_back(massX/1000.);
                            //               mon_BcMuMuDs_VtxMass_Ds.push_back(vtxMassX/1000.);
                            mon_BcMuMuDs_Chi2_Ds.push_back(chi2X);
                            mon_BcMuMuDs_InvMass_Bc.push_back(massXMuMu/1000.);
                            //               mon_BcMuMuDs_VtxMass_Bc.push_back(vtxMassXMuMu/1000.);
                            //               mon_BcMuMuDs_Chi2_Bc.push_back(chi2XMuMu);
                            
                            if(msgLvl() <= MSG::DEBUG) 
                                msg() << MSG::DEBUG << " Good " << "B_c" << " found (no BMuMu2X vertexing)!" << std::endl
                                << "  m = " << trigPartXMuMu->mass() << endreq;
                        } // end if(!doBMuMu2XVertexing), do2XVertexing
                        
                    } // end X chi2 cut
                } // end if(XVxCandidate)
                delete XVxCandidate;
            } // end if(do2XVertexing)
            else {
                if(m_doBc_DsMuMuVertexing) { // !do2XVertexing, STUPID CONFIGURATION
                    // Try to fit XMuMu -> mu mu X vertex
                    Amg::Vector3D vtx ( 0.,0.,0. );
                    Trk::Vertex vertex ( vtx );
                    if ( timerSvc() ) m_VtxFitTimer->resume();
                    std::vector<const Trk::Track*> quintet;
                    quintet.push_back(mu1);
                    quintet.push_back(mu2);
                    quintet.push_back(track1);
                    quintet.push_back(track2);
                    quintet.push_back(track3);
                    Trk::VxCandidate * XMuMuVxCandidate ( 0 );
                    XMuMuVxCandidate = m_fitterSvc->fit(quintet,vertex);
                    if ( timerSvc() ) m_VtxFitTimer->pause();
                    
                    if( !XMuMuVxCandidate ) {
                        ATH_MSG(DEBUG) << " Failed to fit XMuMu vertex for " << "B_c" << endreq;
                        // mon_Errors.push_back(ERROR_XMuMuVtxFit_Fails);
                    } else {
                        m_countPassedBcVtx++;
                        
                        float chi2XMuMu = XMuMuVxCandidate->recVertex().fitQuality().chiSquared();
                        if( !(chi2XMuMu < m_bCVtxChi2Cut) ) {
                            ATH_MSG(DEBUG) << " " << "B_c" << " candidate rejected by XMuMu vertex chi2 cut: chi2 = " << chi2XMuMu << endreq;
                        } else {
                            m_countPassedBcVtxChi2++;
                            
                            //               // calculate XMuMu mass from vertexing
                            //               double vtxMassXMuMu(-1.), vtxMassErrorXMuMu(-1.);
                            //               std::vector<int> trkIndicesXMuMu;
                            //               for (int i=0;i<(int)quartet.size();++i) {trkIndicesXMuMu.push_back(1);} 
                            //               if( !(m_VKVFitter->VKalGetMassError(trkIndicesXMuMu,vtxMassXMuMu,vtxMassErrorXMuMu).isSuccess()) ) {
                            //                 ATH_MSG(DEBUG) << "Warning from VKalVrt - cannot calculate fitmass and error for XMuMu in " << decayName << "!" << endreq;
                            //                 mon_Errors.push_back(ERROR_XMuMuVtxMass_Fails);
                            //               }
                            
                            //              trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::PHIKK, xPhiMass);
                            trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::DSPHIPI, massX);
                            
                            //               trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BMUMUX, massXMuMu);
                            trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BCDSMUMU, massXMuMu);
                            trigPartXMuMu->fitchi2(XMuMuVxCandidate->recVertex().fitQuality().chiSquared());
                            //               trigPartXMuMu->fitmass(vtxMassXMuMu);
                            trigPartXMuMu->fitx(XMuMuVxCandidate->recVertex().position() [0]);
                            trigPartXMuMu->fity(XMuMuVxCandidate->recVertex().position() [1]);
                            trigPartXMuMu->fitz(XMuMuVxCandidate->recVertex().position() [2]);
                            
                            // Fill BMuMu2X monitoring containers
                            mon_BcMuMuDs_Pt_K1.push_back(track1->perigeeParameters()->pT()/1000.);
                            mon_BcMuMuDs_Eta_K1.push_back(track1->perigeeParameters()->eta());
                            mon_BcMuMuDs_Phi_K1.push_back(track1->perigeeParameters()->parameters()[Trk::phi0]);
                            mon_BcMuMuDs_Pt_K2.push_back(track2->perigeeParameters()->pT()/1000.);
                            mon_BcMuMuDs_Eta_K2.push_back(track2->perigeeParameters()->eta());
                            mon_BcMuMuDs_Phi_K2.push_back(track2->perigeeParameters()->parameters()[Trk::phi0]);
                            mon_BcMuMuDs_Pt_pi.push_back(track3->perigeeParameters()->pT()/1000.);
                            mon_BcMuMuDs_Eta_pi.push_back(track3->perigeeParameters()->eta());
                            mon_BcMuMuDs_Phi_pi.push_back(track3->perigeeParameters()->parameters()[Trk::phi0]);
                            mon_BcMuMuDs_InvMass_PhiDs.push_back(xPhiMass/1000.);
                            mon_BcMuMuDs_InvMass_Ds.push_back(massX/1000.);
                            //               mon_BcMuMuDs_VtxMass_Ds.push_back(vtxMassX/1000.);
                            //               mon_BcMuMuDs_Chi2_Ds.push_back(chi2X);
                            mon_BcMuMuDs_InvMass_Bc.push_back(massXMuMu/1000.);
                            //               mon_BcMuMuDs_VtxMass_Bc.push_back(vtxMassXMuMu/1000.);
                            mon_BcMuMuDs_Chi2_Bc.push_back(chi2XMuMu);
                            
                            if(msgLvl() <= MSG::DEBUG) 
                                msg() << MSG::DEBUG << " Good " << "B_c" << " found (no 2X vertexing)!" << std::endl
                                << "  m = " << trigPartXMuMu->mass() << ", "
                                << "chi2 = " << trigPartXMuMu->fitchi2() << ", vertex (" << trigPartXMuMu->fitx() << ", " 
                                << trigPartXMuMu->fity() << ", " << trigPartXMuMu->fitz() << ")" << endreq;
                        } // end XMuMu chi2 cut
                    } // end if(XMuMuVxCandidate)
                    delete XMuMuVxCandidate;
                } // end if(doBMuMu2XVertexing), !do2XVertexing
                else {
                    //              trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::PHIKK, xPhiMass);
                    trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::DSPHIPI, massX);
                    
                    //           trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BMUMUX, massXMuMu);
                    trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BCDSMUMU, massXMuMu);
                    
                    // Fill BMuMu2X monitoring containers
                    mon_BcMuMuDs_Pt_K1.push_back(track1->perigeeParameters()->pT()/1000.);
                    mon_BcMuMuDs_Eta_K1.push_back(track1->perigeeParameters()->eta());
                    mon_BcMuMuDs_Phi_K1.push_back(track1->perigeeParameters()->parameters()[Trk::phi0]);
                    mon_BcMuMuDs_Pt_K2.push_back(track2->perigeeParameters()->pT()/1000.);
                    mon_BcMuMuDs_Eta_K2.push_back(track2->perigeeParameters()->eta());
                    mon_BcMuMuDs_Phi_K2.push_back(track2->perigeeParameters()->parameters()[Trk::phi0]);
                    mon_BcMuMuDs_Pt_pi.push_back(track3->perigeeParameters()->pT()/1000.);
                    mon_BcMuMuDs_Eta_pi.push_back(track3->perigeeParameters()->eta());
                    mon_BcMuMuDs_Phi_pi.push_back(track3->perigeeParameters()->parameters()[Trk::phi0]);
                    mon_BcMuMuDs_InvMass_PhiDs.push_back(xPhiMass/1000.);
                    mon_BcMuMuDs_InvMass_Ds.push_back(massX/1000.);
                    //           mon_BcMuMuDs_VtxMass_Ds.push_back(vtxMassX/1000.);
                    //           mon_BcMuMuDs_Chi2_Ds.push_back(chi2X);
                    mon_BcMuMuDs_InvMass_Bc.push_back(massXMuMu/1000.);
                    //           mon_BcMuMuDs_VtxMass_Bc.push_back(vtxMassXMuMu/1000.);
                    //           mon_BcMuMuDs_Chi2_Bc.push_back(chi2XMuMu);
                    
                    if(msgLvl() <= MSG::DEBUG) 
                        msg() << MSG::DEBUG << " Good " << "B_c" << " found (no 2X vertexing, no BMuMu2X vertexing)!" << std::endl
                        << "  m = " << trigPartXMuMu->mass() << endreq;
                }// end if(!doBMuMu2XVertexing), !do2XVertexing
                
            } // end if(!do2XVertexing)
            
        } // end if XMuMu mass cut
    } // end if X mass cut
    
    return trigPartXMuMu;
    
}



/*----------------------------------------------------------------------------*/
TrigEFBphys* TrigEFBMuMuXFex::checkBplusMuMuKplus(const Trk::Track* mu1, const Trk::Track* mu2, const Trk::Track* track1)
{
    TrigEFBphys* trigPartXMuMu(0);
    
    ATH_MSG(DEBUG) << "Try to build B+ -> mu mu K+ with track " << track1 << endreq;
    
    float massKMuMu = KMuMuMass(mu1,mu2,track1);
    if( !(massKMuMu > m_lowerKMuMuMassCut && massKMuMu < m_upperKMuMuMassCut) ) {
        ATH_MSG(DEBUG) << " B+ -> mu mu K+ candidate rejected by mass cut: m = " << massKMuMu << endreq;
    } else {
        m_countPassedBplusMass++;
        
        if(m_doB_KMuMuVertexing) {
            // Try to fit B+ -> mu mu K+ vertex
            if ( timerSvc() ) m_VtxFitTimer->resume();
            Amg::Vector3D vtx ( 0.,0.,0. );
            Trk::Vertex vertex ( vtx );
            std::vector<const Trk::Track*> trio;
            trio.push_back(mu1);
            trio.push_back(mu2);
            trio.push_back(track1);
            Trk::VxCandidate * bPlusVxCandidate ( 0 );
            bPlusVxCandidate = m_fitterSvc->fit(trio,vertex);
            if ( timerSvc() ) m_VtxFitTimer->pause();
            
            if( !bPlusVxCandidate ) {
                ATH_MSG(DEBUG) << " Failed to fit B+ -> mu mu K+ vertex" << endreq;
                // mon_Errors.push_back(ERROR_BplusVtxFit_Fails);
            } else {
                m_countPassedBplusVtx++;
                
                float chi2KMuMu = bPlusVxCandidate->recVertex().fitQuality().chiSquared();
                if( !(chi2KMuMu < m_bVtxChi2Cut) ) {
                    ATH_MSG(DEBUG) << " B+ -> mu mu K+ candidate rejected by chi2 cut: chi2 = " << chi2KMuMu << endreq;
                } else {
                    m_countPassedBplusVtxChi2++;
                    
                    //           // calculate KMuMu mass from vertexing
                    //           double vtxMassKMuMu(-1.), vtxMassErrorKMuMu(-1.);
                    //           std::vector<int> trkIndices;
                    //           for (int i=0;i<(int)trio.size();++i) {trkIndices.push_back(1);} 
                    //           if( !(m_VKVFitter->VKalGetMassError(trkIndices,vtxMassKMuMu,vtxMassErrorKMuMu).isSuccess()) ) {
                    //             ATH_MSG(DEBUG) << "Warning from VKalVrt - cannot calculate fitmass and error for B+ -> mu mu K+!" << endreq;
                    //             mon_Errors.push_back(ERROR_BplusVtxMass_Fails);
                    //           }
                    
                    //           trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BMUMUX, massKMuMu);
                    trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BKMUMU, massKMuMu);
                    trigPartXMuMu->fitchi2(bPlusVxCandidate->recVertex().fitQuality().chiSquared());
                    //           trigPartXMuMu->fitmass(vtxMassKMuMu);
                    trigPartXMuMu->fitx(bPlusVxCandidate->recVertex().position() [0]);
                    trigPartXMuMu->fity(bPlusVxCandidate->recVertex().position() [1]);
                    trigPartXMuMu->fitz(bPlusVxCandidate->recVertex().position() [2]);
                    
                    // Fill BMuMuK monitoring containers
                    mon_BMuMuK_Pt_K.push_back(track1->perigeeParameters()->pT()/1000.);
                    mon_BMuMuK_Eta_K.push_back(track1->perigeeParameters()->eta());
                    mon_BMuMuK_Phi_K.push_back(track1->perigeeParameters()->parameters()[Trk::phi0]);
                    mon_BMuMuK_InvMass_B.push_back(massKMuMu/1000.);
                    //           mon_BMuMuK_VtxMass_B.push_back(vtxMassKMuMu/1000.);
                    mon_BMuMuK_Chi2_B.push_back(chi2KMuMu);
                    
                    if(msgLvl() <= MSG::DEBUG) 
                        msg() << MSG::DEBUG << " Good B+ -> mu mu K+ found!" << std::endl
                        << "  m = " << trigPartXMuMu->mass() << ", "
                        << "chi2 = " << trigPartXMuMu->fitchi2() << ", vertex (" << trigPartXMuMu->fitx() << ", " 
                        << trigPartXMuMu->fity() << ", " << trigPartXMuMu->fitz() << ")" << endreq;
                } // end KMuMu chi2 cut
            } // end if(bPlusVxCandidate)
            delete bPlusVxCandidate;
        }
        else { // no vertexing
            //       trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BMUMUX, massKMuMu);
            trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BKMUMU, massKMuMu);
            
            // Fill BMuMuK monitoring containers
            mon_BMuMuK_Pt_K.push_back(track1->perigeeParameters()->pT()/1000.);
            mon_BMuMuK_Eta_K.push_back(track1->perigeeParameters()->eta());
            mon_BMuMuK_Phi_K.push_back(track1->perigeeParameters()->parameters()[Trk::phi0]);
            mon_BMuMuK_InvMass_B.push_back(massKMuMu/1000.);
            //       mon_BMuMuK_VtxMass_B.push_back(vtxMassKMuMu/1000.);
            //       mon_BMuMuK_Chi2_B.push_back(chi2KMuMu);
            
            if(msgLvl() <= MSG::DEBUG) 
                msg() << MSG::DEBUG << " Good B+ -> mu mu K+ found (no vertexing)!" << std::endl
                << "  m = " << trigPartXMuMu->mass() << endreq;
        }
        
    } // end if KMuMu mass cut
    
    return trigPartXMuMu;
}


xAOD::TrigBphys* TrigEFBMuMuXFex::checkBplusMuMuKplus(const ElementLink<xAOD::TrackParticleContainer> & eltrack1,
                                                      const ElementLink<xAOD::TrackParticleContainer> & elmu1,
                                                      const ElementLink<xAOD::TrackParticleContainer> & elmu2) {
    
    if(msgLvl() <= MSG::DEBUG) {
        ATH_MSG(DEBUG) << "Try to build B+ -> mu mu K+ with track " << *eltrack1 << endreq;
    }
    float massKMuMu = KMuMuMass(*elmu1,*elmu2,*eltrack1);
    
    static std::vector<double> masses = {KPLUSMASS,MUMASS,MUMASS};
    
    if( !(massKMuMu > m_lowerKMuMuMassCut && massKMuMu < m_upperKMuMuMassCut) ) {
        ATH_MSG(DEBUG) << " B+ -> mu mu K+ candidate rejected by mass cut: m = " << massKMuMu << endreq;
        return nullptr;
    }
    ++m_countPassedBplusMass;
    auto track1 = *eltrack1;
    
    // Fill BMuMuK monitoring containers
    mon_BMuMuK_Pt_K.push_back(track1->pt()*0.001);
    mon_BMuMuK_Eta_K.push_back(track1->eta());
    mon_BMuMuK_Phi_K.push_back(track1->phi());
    mon_BMuMuK_InvMass_B.push_back(massKMuMu*0.001);
    
    // create initial object
    xAOD::TrigBphys * result = new xAOD::TrigBphys;
    result->makePrivateStore();
    result->initialise(0,0.,0.,0.,
                       xAOD::TrigBphys::BKMUMU, massKMuMu, xAOD::TrigBphys::EF);
    
    std::vector<ElementLink<xAOD::TrackParticleContainer> > vec_tracks = {eltrack1,elmu1, elmu2};
    
    if (m_doB_KMuMuVertexing) {
        
        if ( timerSvc() ) m_VtxFitTimer->resume();
        if (!m_bphysHelperTool->vertexFit(result,vec_tracks,masses).isSuccess()) {
            ATH_MSG(DEBUG) << " Vertex fitting failed." << endreq;
            if ( timerSvc() ) m_VtxFitTimer->pause();
            delete result;
            return nullptr;
        }
        m_countPassedBplusVtx++;
        
        double chi2val(result->fitchi2());
        
        if (chi2val > m_bVtxChi2Cut || chi2val < 0) {
            ATH_MSG(DEBUG) << " B+ -> mu mu K+ candidate rejected by chi2 cut: chi2 = " << chi2val << endreq;
            delete result;
            return nullptr;
        }
        mon_BMuMuK_Chi2_B.push_back(chi2val);
        ++m_countPassedBplusVtxChi2;
        
        
        if(msgLvl() <= MSG::DEBUG) {
            msg() << MSG::DEBUG << " Good B+ -> mu mu K+ found!" << std::endl
            << "  m = " << result->mass() << ", "
            << "chi2 = " << result->fitchi2() << ", vertex (" << result->fitx() << ", "
            << result->fity() << ", " << result->fitz() << ")" << endreq;
        } // if debug
        
        
    } // if m_doB_KMuMuVertexing
    else {
      // if vertexting is not done - just attach trackParticle links
      for(auto tpel : vec_tracks) {
        result->addTrackParticleLink(tpel);
      }
    }
    
    return result;
} // checkBplusMuMuKplus

xAOD::TrigBphys* TrigEFBMuMuXFex::checkBcMuMuDs(const ElementLink<xAOD::TrackParticleContainer> & eltrack1,
                                                const ElementLink<xAOD::TrackParticleContainer> & eltrack2,
                                                const ElementLink<xAOD::TrackParticleContainer> & eltrack3,
                                                const ElementLink<xAOD::TrackParticleContainer> & elmu1,
                                                const ElementLink<xAOD::TrackParticleContainer> & elmu2,
                                                double xPhiMass,
                                                xAOD::TrigBphys* & trigPartX) {
    
    trigPartX = nullptr;
    xAOD::TrigBphys * fitVtx_X(nullptr);
    xAOD::TrigBphys * fitVtx(nullptr);
    
    std::vector<double> massHypoX = {KPLUSMASS,KPLUSMASS,PIMASS};
    std::vector<double> massHypo  = {KPLUSMASS,KPLUSMASS,PIMASS,MUMASS,MUMASS};
    
    m_countPassedPhiDsMass++;
    
    ATH_MSG(DEBUG) << "Try to build " << "D_s" << " with tracks " << *eltrack1 << ", " << *eltrack2 << ", " << *eltrack3 << endreq;
    
    
    float massX = X3Mass(*eltrack1, *eltrack2, *eltrack3);
    if( !(massX > m_lowerDs_MassCut && massX < m_upperDs_MassCut) ) {
        ATH_MSG(DEBUG) << " " << "D_s" << " candidate rejected by the mass cut: m = " << massX << endreq;
        return nullptr;
    }
    m_countPassedDsMass++;
    
    float massXMuMu = X3MuMuMass(*elmu1, *elmu2, *eltrack1, *eltrack2, *eltrack3);
    if( !(massXMuMu > m_lowerBc_DsMuMuMassCut && massXMuMu < m_upperBc_DsMuMuMassCut) ) {
        ATH_MSG(DEBUG) << " " << "B_c" << " candidate rejected by the mass cut: m = " << massXMuMu << endreq;
        return nullptr;
    }
    m_countPassedBcMass++;
    
    // create initial object
    fitVtx_X = new xAOD::TrigBphys;
    fitVtx_X->makePrivateStore();
    fitVtx_X->initialise(0,0.,0.,0.,
                         xAOD::TrigBphys::DSPHIPI, massX, xAOD::TrigBphys::EF);
    fitVtx = new xAOD::TrigBphys;
    fitVtx->makePrivateStore();
    fitVtx->initialise(0,0.,0.,0.,
                       xAOD::TrigBphys::BCDSMUMU, massXMuMu, xAOD::TrigBphys::EF);
    
    std::vector<ElementLink<xAOD::TrackParticleContainer> > vec_tracksX = {eltrack1,eltrack2,eltrack3};
    std::vector<ElementLink<xAOD::TrackParticleContainer> > vec_tracks  = {elmu1,elmu2,eltrack1,eltrack2,eltrack3};
    
    mon_BcMuMuDs_Pt_K1.push_back ((*eltrack1)->pt()*0.001);
    mon_BcMuMuDs_Eta_K1.push_back((*eltrack1)->eta());
    mon_BcMuMuDs_Phi_K1.push_back((*eltrack1)->phi());
    mon_BcMuMuDs_Pt_K2.push_back( (*eltrack1)->pt()*0.001);
    mon_BcMuMuDs_Eta_K2.push_back((*eltrack2)->eta());
    mon_BcMuMuDs_Phi_K2.push_back((*eltrack2)->phi());
    mon_BcMuMuDs_Pt_pi.push_back ((*eltrack3)->pt()*0.001);
    mon_BcMuMuDs_Eta_pi.push_back((*eltrack3)->eta());
    mon_BcMuMuDs_Phi_pi.push_back((*eltrack3)->phi());
    
    
    mon_BcMuMuDs_InvMass_PhiDs.push_back(xPhiMass/1000.);
    
    
    if(m_doDs_Vertexing) {
        if ( timerSvc() ) m_VtxFitTimer->resume();
        if (!m_bphysHelperTool->vertexFit(fitVtx_X,vec_tracksX,massHypoX).isSuccess()) {
            ATH_MSG(DEBUG) << " Failed to fit X vertex for " << "D_s" << endreq;
            if ( timerSvc() ) m_VtxFitTimer->pause();
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        if ( timerSvc() ) m_VtxFitTimer->pause();
        
        
        float chi2X = fitVtx_X->fitchi2();
        if( chi2X > m_DsVtxChi2Cut || chi2X < 0  ) {
            ATH_MSG(DEBUG) << " " << "D_s" << " candidate rejected by X vertex chi2 cut: chi2 = " << chi2X << endreq;
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        m_countPassedDsVtxChi2++;
        mon_BcMuMuDs_InvMass_Ds.push_back(massX/1000.);
        //                   mon_BcMuMuDs_VtxMass_Ds.push_back(vtxMassX/1000.);
        mon_BcMuMuDs_Chi2_Ds.push_back(chi2X);
        
    } // m_doDs_Vertexing
    else {
      // if vertexting is not done - just attach trackParticle links
      for(auto tpel : vec_tracksX) {
        fitVtx_X->addTrackParticleLink(tpel);
      }
    }
    
    if(m_doBc_DsMuMuVertexing) {
        // Try to fit XMuMu -> mu mu X vertex
        if ( timerSvc() ) m_VtxFitTimer->resume();
        if (!m_bphysHelperTool->vertexFit(fitVtx,vec_tracks,massHypo).isSuccess()) {
            ATH_MSG(DEBUG) << " Failed to fit XMuMu vertex for " << "B_c" << endreq;
            if ( timerSvc() ) m_VtxFitTimer->pause();
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        if ( timerSvc() ) m_VtxFitTimer->pause();
        m_countPassedBcVtx++;
        float chi2XMuMu = fitVtx->fitchi2();
        if( chi2XMuMu > m_bCVtxChi2Cut ||  chi2XMuMu < 0 ) {
            ATH_MSG(DEBUG) << " " << "B_c" << " candidate rejected by XMuMu vertex chi2 cut: chi2 = " << chi2XMuMu << endreq;
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        m_countPassedBcVtxChi2++;
        
        mon_BcMuMuDs_InvMass_Bc.push_back(massXMuMu/1000.);
        //                   mon_BcMuMuDs_VtxMass_Bc.push_back(vtxMassXMuMu/1000.);
        mon_BcMuMuDs_Chi2_Bc.push_back(chi2XMuMu);
        
    } //m_doBc_DsMuMuVertexing
    else {
      // if vertexting is not done - just attach trackParticle links
      for(auto tpel : vec_tracks) {
        fitVtx->addTrackParticleLink(tpel);
      }
    }
    
    if(msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << " Good " << "B_c" << " found!" << std::endl
        << " doDsVtx: " << (m_doDs_Vertexing       ? "Yes" : "No")
        << " doBcVtx: " << (m_doBc_DsMuMuVertexing ? "Yes" : "No")
        << "  m = " << fitVtx->mass() << ", "
        << "chi2 = " << fitVtx->fitchi2() << ", vertex (" << fitVtx->fitx() << ", "
        << fitVtx->fity() << ", " << fitVtx->fitz() << ")" << endreq;
    }
    
    trigPartX = fitVtx_X;
    return fitVtx;
} //checkBcMuMuDs

xAOD::TrigBphys* TrigEFBMuMuXFex::checkBMuMu2X(const ElementLink<xAOD::TrackParticleContainer> & eltrack1,
                                               const ElementLink<xAOD::TrackParticleContainer> & eltrack2,
                                               const ElementLink<xAOD::TrackParticleContainer> & elmu1,
                                               const ElementLink<xAOD::TrackParticleContainer> & elmu2,
                                               int decay, xAOD::TrigBphys* & trigPartX) {
    trigPartX = nullptr;
    xAOD::TrigBphys * fitVtx_X(nullptr);
    xAOD::TrigBphys * fitVtx(nullptr);
    
    std::vector<double> massHypoX;
    std::vector<double> massHypo;
    double lowerXMassCut, upperXMassCut;
    double lowerXMuMuMassCut, upperXMuMuMassCut;
    double chi2XCut;
    double chi2XMuMuCut;
    std::string decayName;
    xAOD::TrigBphys::pType decayType(xAOD::TrigBphys::UNKNOWNPTYPE);
    unsigned int *countPassedXMass;
    unsigned int *countPassedXMuMuMass;
    unsigned int *countPassedXVtx;
    unsigned int *countPassedXVtxChi2;
    unsigned int *countPassedXMuMuVtx;
    unsigned int *countPassedXMuMuVtxChi2;
    
    bool do2XVertexing;
    bool doBMuMu2XVertexing;
    
    std::vector<float> *pMon_BMuMu2X_Pt_X1;
    std::vector<float> *pMon_BMuMu2X_Eta_X1;
    std::vector<float> *pMon_BMuMu2X_Phi_X1;
    std::vector<float> *pMon_BMuMu2X_Pt_X2;
    std::vector<float> *pMon_BMuMu2X_Eta_X2;
    std::vector<float> *pMon_BMuMu2X_Phi_X2;
    std::vector<float> *pMon_BMuMu2X_InvMass_2X;
    std::vector<float> *pMon_BMuMu2X_VtxMass_2X;
    std::vector<float> *pMon_BMuMu2X_Chi2_2X;
    std::vector<float> *pMon_BMuMu2X_InvMass_B;
    std::vector<float> *pMon_BMuMu2X_VtxMass_B;
    std::vector<float> *pMon_BMuMu2X_Chi2_B;
    
    // Fix silly warning messages
    //    (void)pMon_BMuMu2X_VtxMass_2X;
    //    (void)pMon_BMuMu2X_VtxMass_B;
    
    if(decay == bD_to_Kstar) {
        massHypoX = {KPLUSMASS,PIMASS};
        massHypo  = {KPLUSMASS,PIMASS,MUMASS,MUMASS};
        lowerXMassCut = m_lowerKstar_KaonMassCut;
        upperXMassCut = m_upperKstar_KaonMassCut;
        chi2XCut = m_kStarVtxChi2Cut;
        lowerXMuMuMassCut = m_lowerBd_KstarMuMuMassCut;
        upperXMuMuMassCut = m_upperBd_KstarMuMuMassCut;
        chi2XMuMuCut = m_bDVtxChi2Cut;
        decayName = "Bd -> mu mu K*";
        decayType = xAOD::TrigBphys::BDKSTMUMU;
        countPassedXMass        = &m_countPassedKstarMass;
        countPassedXMuMuMass    = &m_countPassedBdMass;
        countPassedXVtx         = &m_countPassedKstarVtx;
        countPassedXVtxChi2     = &m_countPassedKstarVtxChi2;
        countPassedXMuMuVtx     = &m_countPassedBdVtx;
        countPassedXMuMuVtxChi2 = &m_countPassedBdVtxChi2;
        do2XVertexing      = m_doKstar_KPiVertexing;
        doBMuMu2XVertexing = m_doBd_KstarMuMuVertexing;
        pMon_BMuMu2X_Pt_X1      = &mon_BdMuMuKs_Pt_K;
        pMon_BMuMu2X_Eta_X1     = &mon_BdMuMuKs_Eta_K;
        pMon_BMuMu2X_Phi_X1     = &mon_BdMuMuKs_Phi_K;
        pMon_BMuMu2X_Pt_X2      = &mon_BdMuMuKs_Pt_Pi;
        pMon_BMuMu2X_Eta_X2     = &mon_BdMuMuKs_Eta_Pi;
        pMon_BMuMu2X_Phi_X2     = &mon_BdMuMuKs_Phi_Pi;
        pMon_BMuMu2X_InvMass_2X = &mon_BdMuMuKs_InvMass_Kstar;
        pMon_BMuMu2X_VtxMass_2X = &mon_BdMuMuKs_VtxMass_Kstar;
        pMon_BMuMu2X_Chi2_2X    = &mon_BdMuMuKs_Chi2_Kstar;
        pMon_BMuMu2X_InvMass_B  = &mon_BdMuMuKs_InvMass_Bd;
        pMon_BMuMu2X_VtxMass_B  = &mon_BdMuMuKs_VtxMass_Bd;
        pMon_BMuMu2X_Chi2_B     = &mon_BdMuMuKs_Chi2_Bd;
    } else if(decay == bS_to_Phi) {
        massHypoX = {KPLUSMASS,KPLUSMASS};
        massHypo  = {KPLUSMASS,KPLUSMASS,MUMASS,MUMASS};
        lowerXMassCut = m_lowerPhi1020_KaonMassCut;
        upperXMassCut = m_upperPhi1020_KaonMassCut;
        chi2XCut = m_phi1020VtxChi2Cut;
        lowerXMuMuMassCut = m_lowerBs_Phi1020MuMuMassCut;
        upperXMuMuMassCut = m_upperBs_Phi1020MuMuMassCut;
        chi2XMuMuCut = m_bSVtxChi2Cut;
        decayName = "Bs -> mu mu phi";
        decayType = xAOD::TrigBphys::BSPHIMUMU;
        countPassedXMass        = &m_countPassedPhi1020Mass;
        countPassedXMuMuMass    = &m_countPassedBsMass;
        countPassedXVtx         = &m_countPassedPhi1020Vtx;
        countPassedXVtxChi2     = &m_countPassedPhi1020VtxChi2;
        countPassedXMuMuVtx     = &m_countPassedBsVtx;
        countPassedXMuMuVtxChi2 = &m_countPassedBsVtxChi2;
        do2XVertexing      = m_doPhi1020_KKVertexing;
        doBMuMu2XVertexing = m_doBs_Phi1020MuMuVertexing;
        pMon_BMuMu2X_Pt_X1      = &mon_BsMuMuPhi_Pt_K1;
        pMon_BMuMu2X_Eta_X1     = &mon_BsMuMuPhi_Eta_K1;
        pMon_BMuMu2X_Phi_X1     = &mon_BsMuMuPhi_Phi_K1;
        pMon_BMuMu2X_Pt_X2      = &mon_BsMuMuPhi_Pt_K2;
        pMon_BMuMu2X_Eta_X2     = &mon_BsMuMuPhi_Eta_K2;
        pMon_BMuMu2X_Phi_X2     = &mon_BsMuMuPhi_Phi_K2;
        pMon_BMuMu2X_InvMass_2X = &mon_BsMuMuPhi_InvMass_Phi1020;
        pMon_BMuMu2X_VtxMass_2X = &mon_BsMuMuPhi_VtxMass_Phi1020;
        pMon_BMuMu2X_Chi2_2X    = &mon_BsMuMuPhi_Chi2_Phi1020;
        pMon_BMuMu2X_InvMass_B  = &mon_BsMuMuPhi_InvMass_Bs;
        pMon_BMuMu2X_VtxMass_B  = &mon_BsMuMuPhi_VtxMass_Bs;
        pMon_BMuMu2X_Chi2_B     = &mon_BsMuMuPhi_Chi2_Bs;
    } else if(decay == lB_to_L) {
        massHypoX = {PROTONMASS,PIMASS};
        massHypo  = {PROTONMASS,PIMASS,MUMASS,MUMASS};
        lowerXMassCut = m_lowerLambda_PrPiMassCut;
        upperXMassCut = m_upperLambda_PrPiMassCut;
        chi2XCut = m_lambdaVtxChi2Cut;
        lowerXMuMuMassCut = m_lowerLb_LambdaMuMuMassCut;
        upperXMuMuMassCut = m_upperLb_LambdaMuMuMassCut;
        chi2XMuMuCut = m_lBVtxChi2Cut;
        decayName = "Lambda_b -> mu mu Lambda";
        decayType = xAOD::TrigBphys::LBLMUMU;
        countPassedXMass        = &m_countPassedLambdaMass;
        countPassedXMuMuMass    = &m_countPassedLbMass;
        countPassedXVtx         = &m_countPassedLambdaVtx;
        countPassedXVtxChi2     = &m_countPassedLambdaVtxChi2;
        countPassedXMuMuVtx     = &m_countPassedLbVtx;
        countPassedXMuMuVtxChi2 = &m_countPassedLbVtxChi2;
        do2XVertexing      = m_doLambda_PPiVertexing;
        doBMuMu2XVertexing = m_doLb_LambdaMuMuVertexing;
        pMon_BMuMu2X_Pt_X1      = &mon_LbMuMuLambda_Pt_P;
        pMon_BMuMu2X_Eta_X1     = &mon_LbMuMuLambda_Eta_P;
        pMon_BMuMu2X_Phi_X1     = &mon_LbMuMuLambda_Phi_P;
        pMon_BMuMu2X_Pt_X2      = &mon_LbMuMuLambda_Pt_Pi;
        pMon_BMuMu2X_Eta_X2     = &mon_LbMuMuLambda_Eta_Pi;
        pMon_BMuMu2X_Phi_X2     = &mon_LbMuMuLambda_Phi_Pi;
        pMon_BMuMu2X_InvMass_2X = &mon_LbMuMuLambda_InvMass_Lambda;
        pMon_BMuMu2X_VtxMass_2X = &mon_LbMuMuLambda_VtxMass_Lambda;
        pMon_BMuMu2X_Chi2_2X    = &mon_LbMuMuLambda_Chi2_Lambda;
        pMon_BMuMu2X_InvMass_B  = &mon_LbMuMuLambda_InvMass_Lb;
        pMon_BMuMu2X_VtxMass_B  = &mon_LbMuMuLambda_VtxMass_Lb;
        pMon_BMuMu2X_Chi2_B     = &mon_LbMuMuLambda_Chi2_Lb;
    } else {
        ATH_MSG(DEBUG) << "Wrong decay identifier passed to checkBMuMu2X: decay = " << decay << endreq;
        mon_Errors.push_back(ERROR_WrongDecayID);
        return nullptr;
    }
    
    ATH_MSG(DEBUG) << "Try to build " << decayName << " with tracks " << *eltrack1 << ", " << *eltrack2 << endreq;
    
    float massX = XMass(*eltrack1, *eltrack2, decay);
    if( !(massX > lowerXMassCut && massX < upperXMassCut) ) {
        ATH_MSG(DEBUG) << " " << decayName << " candidate rejected by X mass cut: m = " << massX << endreq;
        trigPartX = nullptr;
        return nullptr;
    }
    
    ++(*countPassedXMass);
    
    float massXMuMu = XMuMuMass(*elmu1, *elmu2, *eltrack1, *eltrack2, decay);
    if( !(massXMuMu > lowerXMuMuMassCut && massXMuMu < upperXMuMuMassCut) ) {
        ATH_MSG(DEBUG) << " " << decayName << " candidate rejected by XMuMu mass cut: m = " << massXMuMu << endreq;
        trigPartX = nullptr;
        return nullptr;
    }
    
    ++(*countPassedXMuMuMass);
    
    // create initial object
    fitVtx_X = new xAOD::TrigBphys;
    fitVtx_X->makePrivateStore();
    fitVtx_X->initialise(0,0.,0.,0.,
                         decayType, massX, xAOD::TrigBphys::EF);
    fitVtx = new xAOD::TrigBphys;
    fitVtx->makePrivateStore();
    fitVtx->initialise(0,0.,0.,0.,
                       decayType, massXMuMu, xAOD::TrigBphys::EF);
    
    
    std::vector<ElementLink<xAOD::TrackParticleContainer> > vec_tracksX = {eltrack1,eltrack2};
    std::vector<ElementLink<xAOD::TrackParticleContainer> > vec_tracks  = {eltrack1,eltrack2,elmu1,elmu2};
    
    
    pMon_BMuMu2X_Pt_X1->push_back ((*eltrack1)->pt()*0.001);
    pMon_BMuMu2X_Eta_X1->push_back((*eltrack1)->eta());
    pMon_BMuMu2X_Phi_X1->push_back((*eltrack1)->phi());
    pMon_BMuMu2X_Pt_X2->push_back ((*eltrack2)->pt()*0.001);
    pMon_BMuMu2X_Eta_X2->push_back((*eltrack2)->eta());
    pMon_BMuMu2X_Phi_X2->push_back((*eltrack2)->phi());
    pMon_BMuMu2X_InvMass_2X->push_back(massX*0.001);
    pMon_BMuMu2X_InvMass_B->push_back(massXMuMu/1000.);
    
    
    
    if(do2XVertexing) {
        if ( timerSvc() ) m_VtxFitTimer->resume();
        if (!m_bphysHelperTool->vertexFit(fitVtx_X,vec_tracksX,massHypoX).isSuccess()) {
            ATH_MSG(DEBUG) << " Vertex fitting of X failed." << decayName << endreq;
        }
        if ( timerSvc() ) m_VtxFitTimer->pause();
        
        ++(*countPassedXVtx);
        double chi2X(fitVtx_X->fitchi2());
        if( chi2X > chi2XCut || chi2X < 0 ) {
            ATH_MSG(DEBUG) << " " << decayName << " candidate rejected by X vertex chi2 cut: chi2 = " << chi2X << endreq;
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        (*countPassedXVtxChi2)++;
        pMon_BMuMu2X_VtxMass_2X->push_back(fitVtx_X->fitmass()*0.001);
        pMon_BMuMu2X_Chi2_2X->push_back(chi2X);
    } // do2XVertexing
    else {
      // if vertexting is not done - just attach trackParticle links
      for(auto tpel : vec_tracksX) {
        fitVtx_X->addTrackParticleLink(tpel);
      }
    }
    
    if(doBMuMu2XVertexing) {
        if ( timerSvc() ) m_VtxFitTimer->resume();
        if (!m_bphysHelperTool->vertexFit(fitVtx,vec_tracks,massHypo).isSuccess()) {
            ATH_MSG(DEBUG) << " Vertex fitting of MuMuX failed for " << decayName << endreq;
        }
        if ( timerSvc() ) m_VtxFitTimer->pause();
        (*countPassedXMuMuVtx)++;
        double chi2MuMuX(fitVtx->fitchi2());
        if( chi2MuMuX > chi2XMuMuCut || chi2MuMuX < 0 ) {
            ATH_MSG(DEBUG) << " " << decayName << " candidate rejected by XMuMu vertex chi2 cut: chi2 = " << chi2MuMuX << endreq;
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        (*countPassedXMuMuVtxChi2)++;
        pMon_BMuMu2X_VtxMass_B->push_back(fitVtx->fitmass()*0.001);
        pMon_BMuMu2X_Chi2_B->push_back(chi2MuMuX);
        
    } // doBMuMu2XVertexing
    else {
      // if vertexting is not done - just attach trackParticle links
      for(auto tpel : vec_tracks) {
        fitVtx->addTrackParticleLink(tpel);
      }
    }
    
    trigPartX = fitVtx_X;
    return fitVtx;
    
} // checkBMuMu2X



/*----------------------------------------------------------------------------*/
void TrigEFBMuMuXFex::addUnique(std::vector<const Trk::Track*>& tracks, const Trk::Track* trkIn)
{
    //  std::cout<<" in addUnique : trkIn pT= "<<trkIn->perigeeParameters()->pT()<<std::endl;
    std::vector<const Trk::Track*>::iterator tItr;
    for( tItr = tracks.begin(); tItr != tracks.end(); tItr++)
    {
        double dPhi=fabs((*tItr)->perigeeParameters()->parameters()[Trk::phi] -
                         trkIn->perigeeParameters()->parameters()[Trk::phi]);
        if (dPhi > M_PI) dPhi = 2.*M_PI - dPhi;
        
        if( fabs(dPhi) < 0.02 &&
           fabs((*tItr)->perigeeParameters()->eta() -
                trkIn->perigeeParameters()->eta()) < 0.02 ) 
        { //std::cout<<" TrigEFBMuMuFex addUnique: the SAME tracks! pT= "<<
            //trkIn->perigeeParameters()->pT()<<" and "<<
            //(*tItr)->perigeeParameters()->pT()<<std::endl;
            return;
        }
    } 
    tracks.push_back(trkIn);       
}

/*----------------------------------------------------------------------------*/
void TrigEFBMuMuXFex::addUnique(std::vector<const xAOD::TrackParticle*>& tps, const xAOD::TrackParticle* tpIn)
{
    //  std::cout<<" in addUnique : tpIn pT= "<<tpIn->pt()<<std::endl;
    std::vector<const xAOD::TrackParticle*>::iterator tpItr;
    for( tpItr = tps.begin(); tpItr != tps.end(); tpItr++)
    {
        double dPhi=fabs((*tpItr)->phi() -
                         tpIn->phi());
        if (dPhi > M_PI) dPhi = 2.*M_PI - dPhi;
        
        if( fabs(dPhi) < 0.02 &&
           fabs((*tpItr)->eta() -
                tpIn->eta()) < 0.02  &&
                    tpIn->charge() * (*tpItr)->charge() > 0) 
        { //std::cout<<" TrigEFBMuMuFex addUnique: the SAME tps! pT= "<<
            //tpIn->pt()<<" and "<<
            //(*tpItr)->pt()<<std::endl;
            return;
        }
    } 
    tps.push_back(tpIn);       
}

void TrigEFBMuMuXFex::addUnique(std::vector<ElementLink<xAOD::TrackParticleContainer> >& tps, const ElementLink<xAOD::TrackParticleContainer>&tpIn) {
    if (!tpIn.isValid()) {
        return;
    }
    for (auto itr = tps.begin(); itr != tps.end(); ++itr) {
        auto tpel = *itr;
        double dPhi = fabs( (*tpel)->phi() - (*tpIn)->phi() );
        if (dPhi > M_PI) dPhi = 2.*M_PI - dPhi;
 
        if( fabs(dPhi) < 0.02 &&
           fabs((*tpel)->eta() -
                (*tpIn)->eta()) < 0.02  &&
           (*tpIn)->charge() * (*tpel)->charge() > 0) {
            // same parameters
            return;
        }
    } // for
    
    // if here then its unique
    tps.push_back(tpIn);
} // addUnique


/*----------------------------------------------------------------------------*/
bool TrigEFBMuMuXFex::isUnique(const  xAOD::TrackParticle* id1, const  xAOD::TrackParticle* id2) const {
    if (!id1 || !id2) return false;
    float dEta = fabs( id1->eta() - id2->eta() );
    float dPhi = id1->phi() - id2->phi();
    while  (dPhi >  M_PI) dPhi -= 2*M_PI;
    while  (dPhi < -M_PI) dPhi += 2*M_PI;
    
    if( dEta < 0.02 && fabs(dPhi) < 0.02 && id1->charge() * id2->charge() > 0 ) return false;
    else return true;
    
}






double TrigEFBMuMuXFex::XMass(const xAOD::TrackParticle* particle1, const xAOD::TrackParticle* particle2, int decay) { /// checking the mass
    
    std::vector<double> massHypo;
    massHypo.clear();
    if(decay == di_to_muons){
        massHypo.push_back(MUMASS);
        massHypo.push_back(MUMASS);
    }
    if(decay == bD_to_Kstar){
        massHypo.push_back(KPLUSMASS);
        massHypo.push_back(PIMASS);
    }
    if(decay == bS_to_Phi){
        massHypo.push_back(KPLUSMASS);
        massHypo.push_back(KPLUSMASS);
    }
    if(decay == lB_to_L){
        massHypo.push_back(PROTONMASS);
        massHypo.push_back(PIMASS);
    }
    std::vector<const xAOD::TrackParticle*> bTracks;
    bTracks.clear();
    bTracks.push_back(particle1);
    bTracks.push_back(particle2);
    return m_bphysHelperTool->invariantMass(bTracks, massHypo);
    
}

double TrigEFBMuMuXFex::X3Mass(const xAOD::TrackParticle* particle1, const xAOD::TrackParticle* particle2, const xAOD::TrackParticle* particle3 ) {
    
    std::vector<double> massHypo;
    massHypo.clear();
    massHypo.push_back(KPLUSMASS);
    massHypo.push_back(KPLUSMASS);
    massHypo.push_back(PIMASS);
    std::vector<const xAOD::TrackParticle*> bTracks;
    bTracks.clear();
    bTracks.push_back(particle1);
    bTracks.push_back(particle2);
    bTracks.push_back(particle3);
    return m_bphysHelperTool->invariantMass(bTracks, massHypo);
    
}

double TrigEFBMuMuXFex::KMuMuMass( const xAOD::TrackParticle* mu1, const xAOD::TrackParticle* mu2, const xAOD::TrackParticle* kaon) {
    std::vector<double> massHypo;
    massHypo.clear();
    massHypo.push_back(MUMASS);
    massHypo.push_back(MUMASS);
    massHypo.push_back(KPLUSMASS);  //K
    std::vector<const xAOD::TrackParticle*> bTracks;
    bTracks.clear();
    bTracks.push_back(mu1);
    bTracks.push_back(mu2);
    bTracks.push_back(kaon);
    return m_bphysHelperTool->invariantMass(bTracks, massHypo);
}

double TrigEFBMuMuXFex::XMuMuMass(const xAOD::TrackParticle* mu1, const xAOD::TrackParticle* mu2, const xAOD::TrackParticle* particle1,
                                  const xAOD::TrackParticle* particle2, int decay){
    std::vector<double> massHypo;
    massHypo.clear();
    massHypo.push_back(MUMASS);
    massHypo.push_back(MUMASS);
    if(decay == bD_to_Kstar){
        massHypo.push_back(KPLUSMASS);
        massHypo.push_back(PIMASS);
    }
    if(decay == bS_to_Phi){
        massHypo.push_back(KPLUSMASS);
        massHypo.push_back(KPLUSMASS);
    }
    if(decay == lB_to_L){
        massHypo.push_back(PROTONMASS);
        massHypo.push_back(PIMASS);
    }
    std::vector<const xAOD::TrackParticle*> bTracks;
    bTracks.clear();
    bTracks.push_back(mu1);
    bTracks.push_back(mu2);
    bTracks.push_back(particle1);
    bTracks.push_back(particle2);
    return m_bphysHelperTool->invariantMass(bTracks, massHypo);
}

double TrigEFBMuMuXFex::X3MuMuMass(const xAOD::TrackParticle* mu1, const xAOD::TrackParticle* mu2, const xAOD::TrackParticle* particle1,
                                   const xAOD::TrackParticle* particle2, const xAOD::TrackParticle* particle3 ) {
    std::vector<double> massHypo;
    massHypo.clear();
    massHypo.push_back(MUMASS);
    massHypo.push_back(MUMASS);
    massHypo.push_back(KPLUSMASS);
    massHypo.push_back(KPLUSMASS);
    massHypo.push_back(PIMASS);
    std::vector<const xAOD::TrackParticle*> bTracks;
    bTracks.clear();
    bTracks.push_back(mu1);
    bTracks.push_back(mu2);
    bTracks.push_back(particle1);
    bTracks.push_back(particle2);
    bTracks.push_back(particle3);
    return m_bphysHelperTool->invariantMass(bTracks, massHypo);
    
}





/*----------------------------------------------------------------------------*/
double TrigEFBMuMuXFex::XMass(const Trk::Track* particle1, const Trk::Track* particle2, int decay)
{
    std::vector<double> massHypo;
    massHypo.clear();
    if(decay == di_to_muons){
        massHypo.push_back(MUMASS);
        massHypo.push_back(MUMASS);
    }
    if(decay == bD_to_Kstar){  
        massHypo.push_back(KPLUSMASS);
        massHypo.push_back(PIMASS);
    }
    if(decay == bS_to_Phi){  
        massHypo.push_back(KPLUSMASS);
        massHypo.push_back(KPLUSMASS);    
    }
    if(decay == lB_to_L){  
        massHypo.push_back(PROTONMASS);
        massHypo.push_back(PIMASS);    
    }
    std::vector<const Trk::Track*> bTracks;
    bTracks.clear();
    bTracks.push_back(particle1);
    bTracks.push_back(particle2);
    return InvMass(bTracks, massHypo);                                            
}


/*----------------------------------------------------------------------------*/
double TrigEFBMuMuXFex::X3Mass(const Trk::Track* particle1, const Trk::Track* particle2, const Trk::Track* particle3)
{
    std::vector<double> massHypo;
    massHypo.clear();
    massHypo.push_back(KPLUSMASS);
    massHypo.push_back(KPLUSMASS);    
    massHypo.push_back(PIMASS);    
    std::vector<const Trk::Track*> bTracks;
    bTracks.clear();
    bTracks.push_back(particle1);
    bTracks.push_back(particle2);
    bTracks.push_back(particle3);
    return InvMass(bTracks, massHypo);                                            
}


/*----------------------------------------------------------------------------*/
double TrigEFBMuMuXFex::KMuMuMass(const Trk::Track* mu1, const Trk::Track* mu2,
                                       const Trk::Track* kaon)
{
    std::vector<double> massHypo;
    massHypo.clear();
    massHypo.push_back(MUMASS);
    massHypo.push_back(MUMASS);
    massHypo.push_back(KPLUSMASS);  //K
    std::vector<const Trk::Track*> bTracks;
    bTracks.clear();
    bTracks.push_back(mu1);
    bTracks.push_back(mu2);
    bTracks.push_back(kaon);
    return InvMass(bTracks, massHypo);                                            
}


/*----------------------------------------------------------------------------*/
double TrigEFBMuMuXFex::XMuMuMass(const Trk::Track* mu1, const Trk::Track* mu2,
                                       const Trk::Track* particle1, const Trk::Track* particle2, int decay)
{
    std::vector<double> massHypo;
    massHypo.clear();
    massHypo.push_back(MUMASS);
    massHypo.push_back(MUMASS);
    if(decay == bD_to_Kstar){  
        massHypo.push_back(KPLUSMASS);
        massHypo.push_back(PIMASS);
    }
    if(decay == bS_to_Phi){  
        massHypo.push_back(KPLUSMASS);
        massHypo.push_back(KPLUSMASS);
    }
    if(decay == lB_to_L){  
        massHypo.push_back(PROTONMASS);
        massHypo.push_back(PIMASS);    
    }  
    std::vector<const Trk::Track*> bTracks;
    bTracks.clear();
    bTracks.push_back(mu1);
    bTracks.push_back(mu2);
    bTracks.push_back(particle1);
    bTracks.push_back(particle2);  
    return InvMass(bTracks, massHypo);
}

/*----------------------------------------------------------------------------*/
double TrigEFBMuMuXFex::X3MuMuMass(const Trk::Track* mu1, const Trk::Track* mu2,
                                        const Trk::Track* particle1, const Trk::Track* particle2, const Trk::Track* particle3)
{
    std::vector<double> massHypo;
    massHypo.clear();
    massHypo.push_back(MUMASS);
    massHypo.push_back(MUMASS);
    massHypo.push_back(KPLUSMASS);
    massHypo.push_back(KPLUSMASS);
    massHypo.push_back(PIMASS);    
    std::vector<const Trk::Track*> bTracks;
    bTracks.clear();
    bTracks.push_back(mu1);
    bTracks.push_back(mu2);
    bTracks.push_back(particle1);
    bTracks.push_back(particle2);  
    bTracks.push_back(particle3);  
    return InvMass(bTracks, massHypo);
}

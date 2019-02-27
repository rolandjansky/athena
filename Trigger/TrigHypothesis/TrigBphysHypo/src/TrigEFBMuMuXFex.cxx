/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
 **   Modified:  26.09.2012 : "B_c -> D_s(*) (->Phi(->K+K-) pi) mu+mu-" added   (Leonid Gladilin <gladilin@mail.cern.ch>)
 **
 **              30.06.2016 : "B_c -> Dplus  (->K pi pi)        mu+mu-" added   (Leonid Gladilin <gladilin@mail.cern.ch>)
 **                         : "B_c -> Dstar  (->D0 (->K pi) pi) mu+mu-" added
 **                         : "B_c -> D0 (->K pi) mu+mu-" (pi_s lost)"  added
 **
 **              30.06.2016 : Cascade fit added to 4 B_c and Lambda_b modes     (Leonid Gladilin <gladilin@mail.cern.ch>)
 **
 **
 ***************************************************************************/

#include "TrigEFBMuMuXFex.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include <math.h>
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "AthenaKernel/Timeout.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "VxVertex/VxCandidate.h"

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
,m_TrigBphysColl_b(NULL)
,m_TrigBphysColl_X(NULL)

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
,m_lastEventPassedBcDplus(-1)
,m_lastEventPassedBcDstar(-1)
,m_lastEventPassedBcD0(-1)
,m_countPassedEventsBplus(0)
,m_countPassedEventsBs(0)
,m_countPassedEventsBd(0)
,m_countPassedEventsLb(0)
,m_countPassedEventsBc(0)
,m_countPassedEventsBcDplus(0)
,m_countPassedEventsBcDstar(0)
,m_countPassedEventsBcD0(0)

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
,m_countPassedLambdaVtxMass(0)
,m_countPassedLbMass(0)
,m_countPassedLbVtxMass(0)
,m_countPassedLambdaVtx(0)
,m_countPassedLambdaVtxChi2(0)
,m_countPassedLbVtx(0)
,m_countPassedLbVtxChi2(0)
,m_countPassedLbLambdaLxy(0)
,m_countLbToStore(0)

,m_countPassedPhiDsMass(0)
,m_countPassedDsMass(0)
,m_countPassedBcMass(0)
,m_countPassedDsVtx(0)
,m_countPassedDsVtxChi2(0)
,m_countPassedBcVtx(0)
,m_countPassedBcVtxChi2(0)
,m_countPassedBcDsLxy(0)
,m_countBcToStore(0)

,m_countPassedDplusMass(0)
,m_countPassedBcDplusMass(0)
,m_countPassedDplusVtx(0)
,m_countPassedDplusVtxChi2(0)
,m_countPassedBcDplusVtx(0)
,m_countPassedBcDplusVtxChi2(0)
,m_countPassedBcDplusLxy(0)
,m_countBcDplusToStore(0)

,m_countPassedD0DstarMass(0)
,m_countPassedDstarMass(0)
,m_countPassedBcDstarMass(0)
,m_countPassedD0DstarVtx(0)
,m_countPassedD0DstarVtxChi2(0)
,m_countPassedBcDstarVtx(0)
,m_countPassedBcDstarVtxChi2(0)
,m_countPassedBcD0DstarLxy(0)
,m_countBcDstarToStore(0)

,m_countPassedD0Mass(0)
,m_countPassedBcD0Mass(0)
,m_countPassedD0Vtx(0)
,m_countPassedD0VtxChi2(0)
,m_countPassedBcD0Vtx(0)
,m_countPassedBcD0VtxChi2(0)
,m_countPassedBcD0Lxy(0)
,m_countBcD0ToStore(0)
{
    declareProperty("TrigBphysHelperTool", m_bphysHelperTool);
    declareProperty("VertexFitterTool", m_fitterSvc);
    
    declareProperty("bphysCollectionKey", m_bphysCollectionKey  = "EFBMuMuXFex" );
    declareProperty("AcceptAll",    m_acceptAll=true); // Should we just accept all events
    
    declareProperty("TrackCollection",m_input_trackCollectionKey="InDetTrigTrackingxAODCnv_Bphysics_IDTrig");
    
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
    declareProperty("UpperLambda_PrPiMassCut", m_upperLambda_PrPiMassCut = 1250.);       // default = 2 500.0 MeV
    declareProperty("LowerLambda_PtCut", m_lowerLambda_PtCut = 1200.);
    declareProperty("LowerLb_PtCut", m_lowerLb_PtCut = 8000.);
    declareProperty("LowerLb_LambdaMuMuMassCut", m_lowerLb_LambdaMuMuMassCut = 3800.);    // default = 1 000.0 MeV
    declareProperty("UpperLb_LambdaMuMuMassCut", m_upperLb_LambdaMuMuMassCut = 6270.);    // default = 10 000.0 MeV
    //   declareProperty("LowerLb_LambdaMuMuMassCutVtxOff", m_lowerLb_LambdaMuMuMassCutVtxOff = 1080.); // default = 2 000.0 MeV
    //   declareProperty("UpperLb_LambdaMuMuMassCutVtxOff", m_upperLb_LambdaMuMuMassCutVtxOff = 1180.); // default = 8 000.0 MeV
    //   declareProperty("LowerLambdaVtxMassCut", m_lowerLambdaVtxMassCut = 1070.);            // default = 200.0 MeV
    declareProperty("UpperLambdaVtxMassCut", m_upperLambdaVtxMassCut = 1200.);            // default = 2 000.0 MeV
    declareProperty("LowerLambda_LxyCut", m_lowerLambda_LxyCut = 0.5);             // default = 0.5 mm
    declareProperty("DoLambda_PPiVertexing", m_doLambda_PPiVertexing = true);
    declareProperty("DoLb_LambdaMuMuVertexing", m_doLb_LambdaMuMuVertexing = true);
    declareProperty("DoLb_LambdaMuMuCascade", m_doLb_LambdaMuMuCascade = true);
    declareProperty("LambdaVtxChi2Cut", m_lambdaVtxChi2Cut = 90.);                      // default = 500.0 MeV
    declareProperty("LowerLbVtxMassCut", m_lowerLb_LambdaMuMuVtxMassCut = 3950.);                    // default = 1 000.0 MeV
    declareProperty("UpperLbVtxMassCut", m_upperLb_LambdaMuMuVtxMassCut = 6100.);                    // default = 10 000.0 MeV
    declareProperty("LbVtxChi2Cut", m_lBVtxChi2Cut = 180.);                              // default = 500.0
    //   declareProperty("LbVtxDistanceCut", m_lBVtxDistanceCut = 0.);                      // default = 0.0
    //   declareProperty("PiImpactCut", m_piImpactCut = 0.);                                // default = 0.0
    //   declareProperty("PrImpactCut", m_prImpactCut = 0.);                                // default = 0.0
    declareProperty("MaxLbToStore", m_maxLbToStore = -1);
    
    // Bc->DsMuMu cuts
    declareProperty("DoBc_DsMuMuDecay", m_doBc_DsMuMuDecay = false);             // Proceed Bc->DsMuMu part of algorithm
    declareProperty("LowerPhiDs_MassCut", m_lowerPhiDs_MassCut  = 980.);        // default =  980.0 MeV
    declareProperty("UpperPhiDs_MassCut", m_upperPhiDs_MassCut = 1080.);        // default = 1080.0 MeV
    declareProperty("LowerDs_MassCut", m_lowerDs_MassCut = 1600.);              // default = 1600.0 MeV
    declareProperty("UpperDs_MassCut", m_upperDs_MassCut = 2400.);              // default = 2400.0 MeV
    declareProperty("LowerDs_PtCut", m_lowerDs_PtCut = 2000.);                  // default = 2000.0 MeV
    declareProperty("LowerDs_LxyCut", m_lowerDs_LxyCut = -999.3);               // default = 0.3 mm
    declareProperty("LowerBc_DsMuMuMassCut", m_lowerBc_DsMuMuMassCut = 5450.);  // default = 1800.0 MeV
    declareProperty("UpperBc_DsMuMuMassCut", m_upperBc_DsMuMuMassCut = 7050.);  // default = 7050.0 MeV
    declareProperty("LowerBcDs_PtCut", m_lowerBcDs_PtCut = 8000.);              // default = 8000.0 MeV
    declareProperty("DoDs_Vertexing", m_doDs_Vertexing = true);
    declareProperty("DoBc_DsMuMuVertexing", m_doBc_DsMuMuVertexing = true);
    declareProperty("DoBc_DsMuMuCascade", m_doBc_DsMuMuCascade = true);
    declareProperty("DsVtxChi2Cut", m_DsVtxChi2Cut =  90.);                    // default =  90.0
    declareProperty("BcVtxChi2Cut", m_bCVtxChi2Cut = 120.);                    // default = 120.0
    declareProperty("MaxBcToStore", m_maxBcToStore = -1);
    
    // Bc->DplusMuMu cuts
    declareProperty("DoBc_DplusMuMuDecay", m_doBc_DplusMuMuDecay = false);       // Proceed Bc->DplusMuMu part of algorithm
    declareProperty("LowerDplus_MassCut", m_lowerDplus_MassCut = 1500.);        // default = 1500.0 MeV
    declareProperty("UpperDplus_MassCut", m_upperDplus_MassCut = 2300.);        // default = 2300.0 MeV
    declareProperty("LowerDplus_PtCut", m_lowerDplus_PtCut = 2000.);            // default = 2000.0 MeV
    declareProperty("LowerDplus_LxyCut", m_lowerDplus_LxyCut = -990.3);            // default = 0.3 mm
    declareProperty("LowerBc_DplusMuMuMassCut", m_lowerBc_DplusMuMuMassCut = 5450.);  // default = 5450.0 MeV
    declareProperty("UpperBc_DplusMuMuMassCut", m_upperBc_DplusMuMuMassCut = 7050.);  // default = 7050.0 MeV
    declareProperty("LowerBcDplus_PtCut", m_lowerBcDplus_PtCut = 8000.);        // default = 8000.0 MeV
    declareProperty("DoDplus_Vertexing", m_doDplus_Vertexing = true);
    declareProperty("DoBc_DplusMuMuVertexing", m_doBc_DplusMuMuVertexing = true);
    declareProperty("DoBc_DplusMuMuCascade", m_doBc_DplusMuMuCascade = true);
    declareProperty("DplusVtxChi2Cut", m_DplusVtxChi2Cut =  90.);               // default =  90.0
    //    declareProperty("BcDplusVtxChi2Cut", m_bCDplusVtxChi2Cut = 120.);           // default = 120.0
    declareProperty("BcDplusVtxChi2Cut", m_bCDplusVtxChi2Cut = 180.);           // default = 180.0
    declareProperty("MaxBcDplusToStore", m_maxBcDplusToStore = -1);
        
    // Bc->DstarMuMu cuts
    declareProperty("DoBc_DstarMuMuDecay", m_doBc_DstarMuMuDecay = false);       // Proceed Bc->DstarMuMu part of algorithm
    declareProperty("LowerD0Dstar_MassCut", m_lowerD0Dstar_MassCut = 1500.);     // default = 1500.0 MeV
    declareProperty("UpperD0Dstar_MassCut", m_upperD0Dstar_MassCut = 2300.);     // default = 2300.0 MeV
    declareProperty("UpperDstar_DMCut", m_upperDstar_DMCut = 180.);              // default = 180.0 MeV
    declareProperty("LowerDstar_PtCut", m_lowerDstar_PtCut = 2000.);             // default = 2000.0 MeV
    declareProperty("LowerDstarKpi_PtCut", m_lowerDstarKpi_PtCut = 1800.);       // default = 1800.0 MeV
    declareProperty("LowerD0Dstar_LxyCut", m_lowerD0Dstar_LxyCut = -9999999.);   // default = -9999999. mm
    declareProperty("LowerBc_DstarMuMuMassCut", m_lowerBc_DstarMuMuMassCut = 5450.);  // default = 5450.0 MeV
    declareProperty("UpperBc_DstarMuMuMassCut", m_upperBc_DstarMuMuMassCut = 7050.);  // default = 7050.0 MeV
    declareProperty("LowerBcDstar_PtCut", m_lowerBcDstar_PtCut = 8000.);        // default = 8000.0 MeV
    declareProperty("DoD0Dstar_Vertexing", m_doD0Dstar_Vertexing = true);
    declareProperty("DoBc_DstarMuMuVertexing", m_doBc_DstarMuMuVertexing = true);
    declareProperty("DoBc_DstarMuMuCascade", m_doBc_DstarMuMuCascade = true);
    declareProperty("D0DstarVtxChi2Cut", m_D0DstarVtxChi2Cut =  90.);               // default =  90.0
    declareProperty("BcDstarVtxChi2Cut", m_bCDstarVtxChi2Cut = 120.);           // default = 120.0
    declareProperty("MaxBcDstarToStore", m_maxBcDstarToStore = -1);
        
    // Bc->D0MuMu cuts
    declareProperty("DoBc_D0MuMuDecay", m_doBc_D0MuMuDecay = false);       // Proceed Bc->D0MuMu part of algorithm
    declareProperty("LowerD0_MassCut", m_lowerD0_MassCut = 1500.);        // default = 1500.0 MeV
    declareProperty("UpperD0_MassCut", m_upperD0_MassCut = 2300.);        // default = 2300.0 MeV
    declareProperty("LowerD0_PtCut", m_lowerD0_PtCut = 2000.);            // default = 2000.0 MeV
    declareProperty("LowerD0_LxyCut", m_lowerD0_LxyCut = 0.01);             // default = 0.01 mm
    declareProperty("LowerBc_D0MuMuMassCut", m_lowerBc_D0MuMuMassCut = 5250.);  // default = 5250.0 MeV
    declareProperty("UpperBc_D0MuMuMassCut", m_upperBc_D0MuMuMassCut = 6911.);  // default = 6911.0 MeV
    declareProperty("LowerBcD0_PtCut", m_lowerBcD0_PtCut = 8000.);        // default = 8000.0 MeV
    declareProperty("DoD0_Vertexing", m_doD0_Vertexing = true);
    declareProperty("DoBc_D0MuMuVertexing", m_doBc_D0MuMuVertexing = true);
    declareProperty("DoBc_D0MuMuCascade", m_doBc_D0MuMuCascade = true);
    declareProperty("D0VtxChi2Cut", m_D0VtxChi2Cut =  90.);               // default =  90.0
    declareProperty("BcD0VtxChi2Cut", m_bCD0VtxChi2Cut = 120.);           // default = 120.0
    declareProperty("MaxBcD0ToStore", m_maxBcD0ToStore = -1);

    // FTK Flag
    declareProperty("DoFTK",    m_FTK=false); // Are we using FTK??

    
    // Monitoring variables
    //   General
    declareMonitoredStdContainer("Errors",     m_mon_Errors,     AutoClear);
    declareMonitoredStdContainer("Acceptance", m_mon_Acceptance, AutoClear);
    declareMonitoredVariable("nTriedCombinations", m_mon_nTriedCombinations );
    //   Timing
    declareMonitoredVariable("TotalRunTime",  m_mon_TotalRunTime);
    declareMonitoredVariable("VertexingTime", m_mon_VertexingTime);
    //   RoIs
    declareMonitoredStdContainer("RoI_RoI1Eta", m_mon_RoI_RoI1Eta, AutoClear);
    declareMonitoredStdContainer("RoI_RoI2Eta", m_mon_RoI_RoI2Eta, AutoClear);
    declareMonitoredStdContainer("RoI_RoI1Phi", m_mon_RoI_RoI1Phi, AutoClear);
    declareMonitoredStdContainer("RoI_RoI2Phi", m_mon_RoI_RoI2Phi, AutoClear);
    declareMonitoredStdContainer("RoI_dEtaRoI", m_mon_RoI_dEtaRoI, AutoClear);
    declareMonitoredStdContainer("RoI_dPhiRoI", m_mon_RoI_dPhiRoI, AutoClear);
    //   DiMuon
    declareMonitoredVariable("DiMu_n", m_mon_DiMu_n);
    declareMonitoredStdContainer("DiMu_Pt_Mu1",       m_mon_DiMu_Pt_Mu1,       AutoClear);
    declareMonitoredStdContainer("DiMu_Pt_Mu2",       m_mon_DiMu_Pt_Mu2,       AutoClear);
    declareMonitoredStdContainer("DiMu_Eta_Mu1",      m_mon_DiMu_Eta_Mu1,      AutoClear);
    declareMonitoredStdContainer("DiMu_Eta_Mu2",      m_mon_DiMu_Eta_Mu2,      AutoClear);
    declareMonitoredStdContainer("DiMu_Phi_Mu1",      m_mon_DiMu_Phi_Mu1,      AutoClear);
    declareMonitoredStdContainer("DiMu_Phi_Mu2",      m_mon_DiMu_Phi_Mu2,      AutoClear);
    declareMonitoredStdContainer("DiMu_dEtaMuMu",     m_mon_DiMu_dEtaMuMu,     AutoClear);
    declareMonitoredStdContainer("DiMu_dPhiMuMu",     m_mon_DiMu_dPhiMuMu,     AutoClear);
    declareMonitoredStdContainer("DiMu_pTsumMuMu",    m_mon_DiMu_pTsumMuMu,    AutoClear);
    declareMonitoredStdContainer("DiMu_InvMassMuMu",  m_mon_DiMu_InvMassMuMu, AutoClear);
    declareMonitoredStdContainer("DiMu_VtxMassMuMu",  m_mon_DiMu_VtxMassMuMu, AutoClear);
    declareMonitoredStdContainer("DiMu_Chi2MuMu",     m_mon_DiMu_Chi2MuMu,    AutoClear);
    //   Tracks
    declareMonitoredVariable("Tracks_n", m_mon_Tracks_n);
    declareMonitoredStdContainer("Tracks_Eta", m_mon_Tracks_Eta, AutoClear);
    declareMonitoredStdContainer("Tracks_Pt",  m_mon_Tracks_Pt,  AutoClear);
    declareMonitoredStdContainer("Tracks_Phi", m_mon_Tracks_Phi, AutoClear);
    //   B+
    declareMonitoredVariable("BMuMuK_n", m_mon_BMuMuK_n);
    declareMonitoredStdContainer("BMuMuK_Pt_K",      m_mon_BMuMuK_Pt_K,      AutoClear);
    declareMonitoredStdContainer("BMuMuK_Eta_K",     m_mon_BMuMuK_Eta_K,     AutoClear);
    declareMonitoredStdContainer("BMuMuK_Phi_K",     m_mon_BMuMuK_Phi_K,     AutoClear);
    declareMonitoredStdContainer("BMuMuK_InvMass_B", m_mon_BMuMuK_InvMass_B, AutoClear);
    declareMonitoredStdContainer("BMuMuK_VtxMass_B", m_mon_BMuMuK_VtxMass_B, AutoClear);
    declareMonitoredStdContainer("BMuMuK_Chi2_B",    m_mon_BMuMuK_Chi2_B,    AutoClear);
    //   Bd
    declareMonitoredVariable("BdMuMuKs_n", m_mon_BdMuMuKs_n);
    declareMonitoredStdContainer("BdMuMuKs_Pt_K",          m_mon_BdMuMuKs_Pt_K,          AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_Eta_K",         m_mon_BdMuMuKs_Eta_K,         AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_Phi_K",         m_mon_BdMuMuKs_Phi_K,         AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_Pt_Pi",         m_mon_BdMuMuKs_Pt_Pi,         AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_Eta_Pi",        m_mon_BdMuMuKs_Eta_Pi,        AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_Phi_Pi",        m_mon_BdMuMuKs_Phi_Pi,        AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_InvMass_Kstar", m_mon_BdMuMuKs_InvMass_Kstar, AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_VtxMass_Kstar", m_mon_BdMuMuKs_VtxMass_Kstar, AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_Chi2_Kstar",    m_mon_BdMuMuKs_Chi2_Kstar,    AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_InvMass_Bd",    m_mon_BdMuMuKs_InvMass_Bd,    AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_VtxMass_Bd",    m_mon_BdMuMuKs_VtxMass_Bd,    AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_Chi2_Bd",       m_mon_BdMuMuKs_Chi2_Bd,       AutoClear);
    //   Bs
    declareMonitoredVariable("BsMuMuPhi_n", m_mon_BsMuMuPhi_n);
    declareMonitoredStdContainer("BsMuMuPhi_Pt_K1",           m_mon_BsMuMuPhi_Pt_K1,           AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_Eta_K1",          m_mon_BsMuMuPhi_Eta_K1,          AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_Phi_K1",          m_mon_BsMuMuPhi_Phi_K1,          AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_Pt_K2",           m_mon_BsMuMuPhi_Pt_K2,           AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_Eta_K2",          m_mon_BsMuMuPhi_Eta_K2,          AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_Phi_K2",          m_mon_BsMuMuPhi_Phi_K2,          AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_InvMass_Phi1020", m_mon_BsMuMuPhi_InvMass_Phi1020, AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_VtxMass_Phi1020", m_mon_BsMuMuPhi_VtxMass_Phi1020, AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_Chi2_Phi1020",    m_mon_BsMuMuPhi_Chi2_Phi1020,    AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_InvMass_Bs",      m_mon_BsMuMuPhi_InvMass_Bs,      AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_VtxMass_Bs",      m_mon_BsMuMuPhi_VtxMass_Bs,      AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_Chi2_Bs",         m_mon_BsMuMuPhi_Chi2_Bs,         AutoClear);
    //   Lambda_b
    declareMonitoredVariable("LbMuMuLambda_n", m_mon_LbMuMuLambda_n);
    declareMonitoredStdContainer("LbMuMuLambda_Pt_P",           m_mon_LbMuMuLambda_Pt_P,           AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_Eta_P",          m_mon_LbMuMuLambda_Eta_P,          AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_Phi_P",          m_mon_LbMuMuLambda_Phi_P,          AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_Pt_Pi",          m_mon_LbMuMuLambda_Pt_Pi,          AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_Eta_Pi",         m_mon_LbMuMuLambda_Eta_Pi,         AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_Phi_Pi",         m_mon_LbMuMuLambda_Phi_Pi,         AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_InvMass_Lambda", m_mon_LbMuMuLambda_InvMass_Lambda, AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_VtxMass_Lambda", m_mon_LbMuMuLambda_VtxMass_Lambda, AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_FinMass_Lambda", m_mon_LbMuMuLambda_FinMass_Lambda, AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_Chi2_Lambda",    m_mon_LbMuMuLambda_Chi2_Lambda,    AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_InvMass_Lb",     m_mon_LbMuMuLambda_InvMass_Lb,     AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_VtxMass_Lb",     m_mon_LbMuMuLambda_VtxMass_Lb,     AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_FinMass_Lb",     m_mon_LbMuMuLambda_FinMass_Lb,     AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_Chi2_Lb",        m_mon_LbMuMuLambda_Chi2_Lb,        AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_pT_Lambda",      m_mon_LbMuMuLambda_pT_Lambda,      AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_pT_Lb",          m_mon_LbMuMuLambda_pT_Lb,          AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_Lxy_Lambda",     m_mon_LbMuMuLambda_Lxy_Lambda,     AutoClear);
    //   Bc
    declareMonitoredVariable("BcMuMuDs_n", m_mon_BcMuMuDs_n);
    declareMonitoredStdContainer("BcMuMuDs_Pt_K1",           m_mon_BcMuMuDs_Pt_K1,           AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Eta_K1",          m_mon_BcMuMuDs_Eta_K1,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Phi_K1",          m_mon_BcMuMuDs_Phi_K1,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Pt_K2",           m_mon_BcMuMuDs_Pt_K2,           AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Eta_K2",          m_mon_BcMuMuDs_Eta_K2,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Phi_K2",          m_mon_BcMuMuDs_Phi_K2,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Pt_pi",           m_mon_BcMuMuDs_Pt_pi,           AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Eta_pi",          m_mon_BcMuMuDs_Eta_pi,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Phi_pi",          m_mon_BcMuMuDs_Phi_pi,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_InvMass_PhiDs",   m_mon_BcMuMuDs_InvMass_PhiDs,   AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_InvMass_Ds",      m_mon_BcMuMuDs_InvMass_Ds,      AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_VtxMass_Ds",      m_mon_BcMuMuDs_VtxMass_Ds,      AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Chi2_Ds",         m_mon_BcMuMuDs_Chi2_Ds,         AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_InvMass_Bc",      m_mon_BcMuMuDs_InvMass_Bc,      AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_VtxMass_Bc",      m_mon_BcMuMuDs_VtxMass_Bc,      AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Chi2_Bc",         m_mon_BcMuMuDs_Chi2_Bc,         AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_pT_Ds",           m_mon_BcMuMuDs_pT_Ds,           AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_pT_Bc",           m_mon_BcMuMuDs_pT_Bc,           AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Lxy_Ds",          m_mon_BcMuMuDs_Lxy_Ds,          AutoClear);
    //   Bc -> Dplus
    declareMonitoredVariable("BcMuMuDplus_n", m_mon_BcMuMuDplus_n);
    declareMonitoredStdContainer("BcMuMuDplus_Pt_K",           m_mon_BcMuMuDplus_Pt_K,           AutoClear);
    declareMonitoredStdContainer("BcMuMuDplus_Eta_K",          m_mon_BcMuMuDplus_Eta_K,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDplus_Phi_K",          m_mon_BcMuMuDplus_Phi_K,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDplus_Pt_pi1",           m_mon_BcMuMuDplus_Pt_pi1,           AutoClear);
    declareMonitoredStdContainer("BcMuMuDplus_Eta_pi1",          m_mon_BcMuMuDplus_Eta_pi1,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDplus_Phi_pi1",          m_mon_BcMuMuDplus_Phi_pi1,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDplus_Pt_pi2",           m_mon_BcMuMuDplus_Pt_pi2,           AutoClear);
    declareMonitoredStdContainer("BcMuMuDplus_Eta_pi2",          m_mon_BcMuMuDplus_Eta_pi2,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDplus_Phi_pi2",          m_mon_BcMuMuDplus_Phi_pi2,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDplus_InvMass_Dplus",    m_mon_BcMuMuDplus_InvMass_Dplus,      AutoClear);
    declareMonitoredStdContainer("BcMuMuDplus_VtxMass_Dplus",    m_mon_BcMuMuDplus_VtxMass_Dplus,      AutoClear);
    declareMonitoredStdContainer("BcMuMuDplus_Chi2_Dplus",       m_mon_BcMuMuDplus_Chi2_Dplus,         AutoClear);
    declareMonitoredStdContainer("BcMuMuDplus_Lxy_Dplus",       m_mon_BcMuMuDplus_Lxy_Dplus,       AutoClear);
    declareMonitoredStdContainer("BcMuMuDplus_InvMass_Bc",      m_mon_BcMuMuDplus_InvMass_Bc,      AutoClear);
    declareMonitoredStdContainer("BcMuMuDplus_VtxMass_Bc",      m_mon_BcMuMuDplus_VtxMass_Bc,      AutoClear);
    declareMonitoredStdContainer("BcMuMuDplus_Chi2_Bc",         m_mon_BcMuMuDplus_Chi2_Bc,         AutoClear);
    declareMonitoredStdContainer("BcMuMuDplus_pT_Dplus",        m_mon_BcMuMuDplus_pT_Dplus,        AutoClear);
    declareMonitoredStdContainer("BcMuMuDplus_pT_Bc",           m_mon_BcMuMuDplus_pT_Bc,           AutoClear);
    //   Bc -> Dstar
    declareMonitoredVariable("BcMuMuDstar_n", m_mon_BcMuMuDstar_n);
    declareMonitoredStdContainer("BcMuMuDstar_Pt_K",           m_mon_BcMuMuDstar_Pt_K,           AutoClear);
    declareMonitoredStdContainer("BcMuMuDstar_Eta_K",          m_mon_BcMuMuDstar_Eta_K,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDstar_Phi_K",          m_mon_BcMuMuDstar_Phi_K,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDstar_Pt_pi",           m_mon_BcMuMuDstar_Pt_pi,           AutoClear);
    declareMonitoredStdContainer("BcMuMuDstar_Eta_pi",          m_mon_BcMuMuDstar_Eta_pi,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDstar_Phi_pi",          m_mon_BcMuMuDstar_Phi_pi,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDstar_Pt_pis",           m_mon_BcMuMuDstar_Pt_pis,           AutoClear);
    declareMonitoredStdContainer("BcMuMuDstar_Eta_pis",          m_mon_BcMuMuDstar_Eta_pis,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDstar_Phi_pis",          m_mon_BcMuMuDstar_Phi_pis,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDstar_InvMass_D0Dstar",   m_mon_BcMuMuDstar_InvMass_D0Dstar,   AutoClear);
    declareMonitoredStdContainer("BcMuMuDstar_VtxMass_D0Dstar",   m_mon_BcMuMuDstar_VtxMass_D0Dstar,   AutoClear);
    declareMonitoredStdContainer("BcMuMuDstar_DelMass_Dstar",      m_mon_BcMuMuDstar_DelMass_Dstar,      AutoClear);
    declareMonitoredStdContainer("BcMuMuDstar_VtxDelMass_Dstar",   m_mon_BcMuMuDstar_VtxDelMass_Dstar,   AutoClear);
    declareMonitoredStdContainer("BcMuMuDstar_Chi2_D0Dstar",         m_mon_BcMuMuDstar_Chi2_D0Dstar,         AutoClear);
    declareMonitoredStdContainer("BcMuMuDstar_InvMass_Bc",      m_mon_BcMuMuDstar_InvMass_Bc,      AutoClear);
    declareMonitoredStdContainer("BcMuMuDstar_VtxMass_Bc",      m_mon_BcMuMuDstar_VtxMass_Bc,      AutoClear);
    declareMonitoredStdContainer("BcMuMuDstar_Chi2_Bc",         m_mon_BcMuMuDstar_Chi2_Bc,         AutoClear);
    declareMonitoredStdContainer("BcMuMuDstar_pT_Dstar",        m_mon_BcMuMuDstar_pT_Dstar,        AutoClear);
    declareMonitoredStdContainer("BcMuMuDstar_pT_Bc",           m_mon_BcMuMuDstar_pT_Bc,           AutoClear);
    declareMonitoredStdContainer("BcMuMuDstar_Lxy_D0Dstar",     m_mon_BcMuMuDstar_Lxy_D0Dstar,     AutoClear);
    //   Bc -> D0
    declareMonitoredVariable("BcMuMuD0_n", m_mon_BcMuMuD0_n);
    declareMonitoredStdContainer("BcMuMuD0_Pt_K",           m_mon_BcMuMuD0_Pt_K,           AutoClear);
    declareMonitoredStdContainer("BcMuMuD0_Eta_K",          m_mon_BcMuMuD0_Eta_K,          AutoClear);
    declareMonitoredStdContainer("BcMuMuD0_Phi_K",          m_mon_BcMuMuD0_Phi_K,          AutoClear);
    declareMonitoredStdContainer("BcMuMuD0_Pt_pi",           m_mon_BcMuMuD0_Pt_pi,           AutoClear);
    declareMonitoredStdContainer("BcMuMuD0_Eta_pi",          m_mon_BcMuMuD0_Eta_pi,          AutoClear);
    declareMonitoredStdContainer("BcMuMuD0_Phi_pi",          m_mon_BcMuMuD0_Phi_pi,          AutoClear);
    declareMonitoredStdContainer("BcMuMuD0_InvMass_D0",   m_mon_BcMuMuD0_InvMass_D0,   AutoClear);
    declareMonitoredStdContainer("BcMuMuD0_VtxMass_D0",   m_mon_BcMuMuD0_VtxMass_D0,   AutoClear);
    declareMonitoredStdContainer("BcMuMuD0_Chi2_D0",         m_mon_BcMuMuD0_Chi2_D0,         AutoClear);
    declareMonitoredStdContainer("BcMuMuD0_InvMass_Bc",      m_mon_BcMuMuD0_InvMass_Bc,      AutoClear);
    declareMonitoredStdContainer("BcMuMuD0_VtxMass_Bc",      m_mon_BcMuMuD0_VtxMass_Bc,      AutoClear);
    declareMonitoredStdContainer("BcMuMuD0_Chi2_Bc",         m_mon_BcMuMuD0_Chi2_Bc,         AutoClear);
    declareMonitoredStdContainer("BcMuMuD0_pT_D0",           m_mon_BcMuMuD0_pT_D0,           AutoClear);
    declareMonitoredStdContainer("BcMuMuD0_pT_Bc",           m_mon_BcMuMuD0_pT_Bc,           AutoClear);
    declareMonitoredStdContainer("BcMuMuD0_Lxy_D0",          m_mon_BcMuMuD0_Lxy_D0,          AutoClear);
    
    
}


/*----------------------------------------------------------------------------*/
TrigEFBMuMuXFex::~TrigEFBMuMuXFex()
{ }


/*----------------------------------------------------------------------------*/
HLT::ErrorCode TrigEFBMuMuXFex::hltInitialize()
{
    ATH_MSG_INFO("Running TrigEFBMuMuXFex::hltInitialize" );
   
    ATH_MSG_DEBUG("Initialization completed successfully:" );
    ATH_MSG_DEBUG("AcceptAll          = "   << (m_acceptAll==true ? "True" : "False") );
    ATH_MSG_DEBUG("DoFTK         = "        << (m_FTK==true ? "True" : "False") );
    ATH_MSG_DEBUG("MaxNcombinations            = " << m_maxNcombinations );
    ATH_MSG_DEBUG("Activated decays:" );
    ATH_MSG_DEBUG("    B+ -> mu mu K+ : " << (m_doB_KMuMuDecay==true ? "True" : "False") );
    ATH_MSG_DEBUG("    Bd -> mu mu K*(K+ Pi-) : " << (m_doBd_KstarMuMuDecay==true ? "True" : "False") );
    ATH_MSG_DEBUG("    Bs -> mu mu Phi(K+ K-) : " << (m_doBs_Phi1020MuMuDecay==true ? "True" : "False") );
    ATH_MSG_DEBUG("    Lambda_b -> mu mu Lambda(P Pi) : " << (m_doLb_LambdaMuMuDecay==true ? "True" : "False") );
    ATH_MSG_DEBUG("    Bc -> mu mu Ds(Phi pi) : " << (m_doBc_DsMuMuDecay==true ? "True" : "False") );
    ATH_MSG_DEBUG("    Bc -> mu mu D+(K pi pi) : " << (m_doBc_DplusMuMuDecay==true ? "True" : "False") );
    ATH_MSG_DEBUG("    Bc -> mu mu D*(D0 pi) : " << (m_doBc_DstarMuMuDecay==true ? "True" : "False") );
    ATH_MSG_DEBUG("    Bc -> mu mu D0(K pi) : " << (m_doBc_D0MuMuDecay==true ? "True" : "False") );
    ATH_MSG_DEBUG("OppositeCharge     = "        << (m_oppositeCharge==true ? "True" : "False") );
    
    // setting up timers
    if ( timerSvc() ) {
        m_TotTimer    = addTimer("EFBMuMuXFexTot");
        m_VtxFitTimer = addTimer("EFBMuMuXFexVtxFit");
    }
    
    // retrieving the vertex fitting tool
    if (m_fitterSvc.retrieve().isFailure()) {
        ATH_MSG_ERROR("Can't find Trk::TrkVKalVrtFitter" );
        return HLT::BAD_JOB_SETUP;
    } else {
        ATH_MSG_DEBUG("Trk::TrkVKalVrtFitter found" );
        m_VKVFitter = dynamic_cast< Trk::TrkVKalVrtFitter* > (&(*m_fitterSvc));
    }
    
    // retrieving BphysHelperUtilsTool
    if (m_bphysHelperTool.retrieve().isFailure()) {
        ATH_MSG_ERROR("Can't find TrigBphysHelperUtilsTool" );
        return HLT::BAD_JOB_SETUP;
    } else {
        ATH_MSG_DEBUG("TrigBphysHelperUtilsTool found" );
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
    m_countPassedLambdaVtxMass=0;
    m_countPassedLbMass=0;
    m_countPassedLbVtxMass=0;
    m_countPassedLambdaVtx=0;
    m_countPassedLambdaVtxChi2=0;
    m_countPassedLbVtx=0;
    m_countPassedLbVtxChi2=0;
    m_countPassedLbLambdaLxy=0;
    
    m_countPassedPhiDsMass=0;
    m_countPassedDsMass=0;
    m_countPassedBcMass=0;
    m_countPassedDsVtx=0;
    m_countPassedDsVtxChi2=0;
    m_countPassedBcVtx=0;
    m_countPassedBcVtxChi2=0;
    m_countPassedBcDsLxy=0;

    m_countPassedDplusMass=0;
    m_countPassedBcDplusMass=0;
    m_countPassedDplusVtx=0;
    m_countPassedDplusVtxChi2=0;
    m_countPassedBcDplusVtx=0;
    m_countPassedBcDplusVtxChi2=0;
    m_countPassedBcDplusLxy=0;

    m_countPassedD0DstarMass=0;
    m_countPassedDstarMass=0;
    m_countPassedBcDstarMass=0;
    m_countPassedD0DstarVtx=0;
    m_countPassedD0DstarVtxChi2=0;
    m_countPassedBcDstarVtx=0;
    m_countPassedBcDstarVtxChi2=0;
    m_countPassedBcD0DstarLxy=0;

    m_countPassedD0Mass=0;
    m_countPassedBcD0Mass=0;
    m_countPassedD0Vtx=0;
    m_countPassedD0VtxChi2=0;
    m_countPassedBcD0Vtx=0;
    m_countPassedBcD0VtxChi2=0;
    m_countPassedBcD0Lxy=0;
    
    
    return HLT::OK;
}


/*----------------------------------------------------------------------------*/
HLT::ErrorCode TrigEFBMuMuXFex::hltFinalize()
{
    
    ATH_MSG_INFO("Running TrigEFBMuMuXFex::hltFinalize" );
    
    ATH_MSG_INFO("|----------------------- SUMMARY FROM TrigEFBMuMuXFex -------------|" );
    ATH_MSG_INFO("Run on events/RoIs      " << m_countTotalEvents << "/" << m_countTotalRoI );
    ATH_MSG_INFO("Passed events/RoIs      " << m_countPassedEvents << "/" << m_countPassedRoIs );
    ATH_MSG_INFO("Evts Passed B+:         " << m_countPassedEventsBplus );
    ATH_MSG_INFO("Evts Passed Bd:         " << m_countPassedEventsBd );
    ATH_MSG_INFO("Evts Passed Bs:         " << m_countPassedEventsBs );
    ATH_MSG_INFO("Evts Passed Lambda_b:   " << m_countPassedEventsLb );
    ATH_MSG_INFO("Evts Passed Bc:         " << m_countPassedEventsBc );
    ATH_MSG_INFO("Evts Passed Bc Dplus:   " << m_countPassedEventsBcDplus );
    ATH_MSG_INFO("Evts Passed Bc Dstar:   " << m_countPassedEventsBcDstar );
    ATH_MSG_INFO("Evts Passed Bc D0:   " << m_countPassedEventsBcD0 );
    ATH_MSG_INFO(std::endl );
    ATH_MSG_INFO("PassedMuMuID:           " << m_countPassedMuMuID );
    ATH_MSG_INFO("PassedMuMuOS:           " << m_countPassedMuMuOS );
    ATH_MSG_INFO("PassedMuMuMass:         " << m_countPassedMuMuMass );
    ATH_MSG_INFO("PassedMuMuVtx:          " << m_countPassedMuMuVtx );
    ATH_MSG_INFO("PassedMuMuVtxChi2:      " << m_countPassedMuMuVtxChi2 );
    ATH_MSG_INFO("PassedBplusMass:        " << m_countPassedBplusMass );
    ATH_MSG_INFO("PassedBplusVtx:         " << m_countPassedBplusVtx );
    ATH_MSG_INFO("PassedBplusVtxChi2:     " << m_countPassedBplusVtxChi2 );
    ATH_MSG_INFO("PassedKstarMass:        " << m_countPassedKstarMass );
    ATH_MSG_INFO("PassedKstarVtx:         " << m_countPassedKstarVtx );
    ATH_MSG_INFO("PassedKstarVtxChi2:     " << m_countPassedKstarVtxChi2 );
    ATH_MSG_INFO("PassedBdMass:           " << m_countPassedBdMass );
    ATH_MSG_INFO("PassedBdVtx:            " << m_countPassedBdVtx );
    ATH_MSG_INFO("PassedBdVtxChi2:        " << m_countPassedBdVtxChi2 );
    ATH_MSG_INFO("PassedPhi1020Mass:      " << m_countPassedPhi1020Mass );
    ATH_MSG_INFO("PassedPhi1020Vtx:       " << m_countPassedPhi1020Vtx );
    ATH_MSG_INFO("PassedPhi1020VtxChi2:   " << m_countPassedPhi1020VtxChi2 );
    ATH_MSG_INFO("PassedBsMass:           " << m_countPassedBsMass );
    ATH_MSG_INFO("PassedBsVtx:            " << m_countPassedBsVtx );
    ATH_MSG_INFO("PassedBsVtxChi2:        " << m_countPassedBsVtxChi2 );
    ATH_MSG_INFO("PassedLambdaMass:       " << m_countPassedLambdaMass );
    ATH_MSG_INFO("PassedLambdaVtxMass:    " << m_countPassedLambdaVtxMass );
    ATH_MSG_INFO("PassedLambdaVtx:        " << m_countPassedLambdaVtx );
    ATH_MSG_INFO("PassedLambdaVtxChi2:    " << m_countPassedLambdaVtxChi2 );
    ATH_MSG_INFO("PassedLbMass:           " << m_countPassedLbMass );
    ATH_MSG_INFO("PassedLbVtxMass:        " << m_countPassedLbVtxMass );
    ATH_MSG_INFO("PassedLbVtx:            " << m_countPassedLbVtx );
    ATH_MSG_INFO("PassedLbVtxChi2:        " << m_countPassedLbVtxChi2 );
    ATH_MSG_INFO("PassedPhiDsMass:        " << m_countPassedPhiDsMass );
    ATH_MSG_INFO("PassedDsMass:           " << m_countPassedDsMass );
    ATH_MSG_INFO("PassedDsVtx:            " << m_countPassedDsVtx );
    ATH_MSG_INFO("PassedDsVtxChi2:        " << m_countPassedDsVtxChi2 );
    ATH_MSG_INFO("PassedBcMass:           " << m_countPassedBcMass );
    ATH_MSG_INFO("PassedBcVtx:            " << m_countPassedBcVtx );
    ATH_MSG_INFO("PassedBcVtxChi2:        " << m_countPassedBcVtxChi2 );
    ATH_MSG_INFO("PassedBcDsLxy:          " << m_countPassedBcDsLxy );
    ATH_MSG_INFO("PassedDplusMass:           " << m_countPassedDplusMass );
    ATH_MSG_INFO("PassedDplusVtx:            " << m_countPassedDplusVtx );
    ATH_MSG_INFO("PassedDplusVtxChi2:        " << m_countPassedDplusVtxChi2 );
    ATH_MSG_INFO("PassedBcDplusMass:           " << m_countPassedBcDplusMass );
    ATH_MSG_INFO("PassedBcDplusVtx:            " << m_countPassedBcDplusVtx );
    ATH_MSG_INFO("PassedBcDplusVtxChi2:        " << m_countPassedBcDplusVtxChi2 );
    ATH_MSG_INFO("PassedBcDplusLxy:          " << m_countPassedBcDplusLxy );
    ATH_MSG_INFO("PassedD0DstarMass:         " << m_countPassedD0DstarMass );
    ATH_MSG_INFO("PassedDstarMass:           " << m_countPassedDstarMass );
    ATH_MSG_INFO("PassedD0DstarVtx:          " << m_countPassedD0DstarVtx );
    ATH_MSG_INFO("PassedD0DstarVtxChi2:        " << m_countPassedD0DstarVtxChi2 );
    ATH_MSG_INFO("PassedBcDstarMass:           " << m_countPassedBcDstarMass );
    ATH_MSG_INFO("PassedBcDstarVtx:            " << m_countPassedBcDstarVtx );
    ATH_MSG_INFO("PassedBcDstarVtxChi2:        " << m_countPassedBcDstarVtxChi2 );
    ATH_MSG_INFO("PassedBcD0DstarLxy:          " << m_countPassedBcD0DstarLxy );
    ATH_MSG_INFO("PassedD0Mass:         " << m_countPassedD0Mass );
    ATH_MSG_INFO("PassedD0Vtx:          " << m_countPassedD0Vtx );
    ATH_MSG_INFO("PassedD0VtxChi2:        " << m_countPassedD0VtxChi2 );
    ATH_MSG_INFO("PassedBcD0Mass:           " << m_countPassedBcD0Mass );
    ATH_MSG_INFO("PassedBcD0Vtx:            " << m_countPassedBcD0Vtx );
    ATH_MSG_INFO("PassedBcD0VtxChi2:        " << m_countPassedBcD0VtxChi2 );
    ATH_MSG_INFO("PassedBcD0Lxy:            " << m_countPassedBcD0Lxy );
    
    return HLT::OK;
}


/*----------------------------------------------------------------------------*/
HLT::ErrorCode TrigEFBMuMuXFex::acceptInputs(HLT::TEConstVec& , bool& pass)
{
    ATH_MSG_DEBUG("Running TrigEFBMuMuXFex::acceptInputs" );
    pass = true;
    
    return HLT::OK;
}


/*----------------------------------------------------------------------------*/
HLT::ErrorCode TrigEFBMuMuXFex::hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement*  outputTE )
{
    ATH_MSG_DEBUG("Running TrigEFBMuMuXFex::hltExecute" );
    
    if ( timerSvc() ) m_TotTimer->start();
    
    if ( timerSvc() ) {
        m_VtxFitTimer->start();
        m_VtxFitTimer->pause();
    }
    
    bool result(false);
    
    // Set monitoring counters to zeroes
    m_mon_nTriedCombinations = 0;
    m_mon_DiMu_n         = 0;
    m_mon_Tracks_n       = 0;
    m_mon_BMuMuK_n       = 0;
    m_mon_BdMuMuKs_n     = 0;
    m_mon_BsMuMuPhi_n    = 0;
    m_mon_LbMuMuLambda_n = 0;
    m_mon_BcMuMuDs_n     = 0;
    m_mon_BcMuMuDplus_n     = 0;
    m_mon_BcMuMuDstar_n     = 0;
    m_mon_BcMuMuD0_n     = 0;
    m_mon_TotalRunTime   = 0.;
    m_mon_VertexingTime  = 0.;
    
    ATH_MSG_VERBOSE("AcceptAll is set to : " << (m_acceptAll ? "True, taking all events " : "False, applying selection" ));
    
    // Retrieve event info
    int IdRun   = 0;
    int IdEvent = 0;
    
    // JW - Try to get the xAOD event info
    const EventInfo* pEventInfo(0);
    const xAOD::EventInfo *evtInfo(0);
    if ( store()->retrieve(evtInfo).isFailure() ) {
        ATH_MSG_DEBUG("Failed to get xAOD::EventInfo " );
        // now try the old event ifo
        if ( store()->retrieve(pEventInfo).isFailure() ) {
            ATH_MSG_DEBUG("Failed to get EventInfo " );
            m_mon_Errors.push_back( ERROR_No_EventInfo );
        } else {
            IdRun   = pEventInfo->event_ID()->run_number();
            IdEvent = pEventInfo->event_ID()->event_number();
            ATH_MSG_DEBUG(" Run " << IdRun << " Event " << IdEvent << " using algo " << "m_muonAlgo");
        }// found old event info
    }else { // found the xAOD event info
        ATH_MSG_DEBUG(" Run " << evtInfo->runNumber() << " Event " << evtInfo->eventNumber() << " using algo m_muonAlgo" );
        IdRun   = evtInfo->runNumber();
        IdEvent = evtInfo->eventNumber();
    } // get event info
    
    
    //Check that we got 2 input TEs
    int mu1_TE=-1;
    int mu2_TE=-1;
    if (!m_FTK) {
      if ( inputTE.size() != 2 ) {
        ATH_MSG_DEBUG("Got different than 2 number of input TEs: " << inputTE.size() );
          //m_mon_Errors.push_back(ERROR_Not_2_InputTEs);
          //if ( timerSvc() ) m_TotTimer->stop();
          //return HLT::BAD_JOB_SETUP;
        mu1_TE=0;
        mu2_TE=0;
      }else{
        mu1_TE=0;
        mu2_TE=1;
      }
    } else {
      if ( inputTE.size() != 3 ) {
        ATH_MSG_ERROR("FTK mode expect 3 input TEs, got : " << inputTE.size() );
        m_mon_Errors.push_back(ERROR_Not_2_InputTEs);
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
        ATH_MSG_DEBUG("Input TE " << iTE << "  " << inputTE[iTE] << " ID " << inputTE[iTE]->getId() );
    }


    //Retrieve ROIs
    const TrigRoiDescriptor *roiDescriptor1(0);
    const TrigRoiDescriptor *roiDescriptor2(0);
    const TrigRoiDescriptor *roiDescriptorTrk(0); // for FTK chain
    
    // get them from the navigation

     if ( getFeature(inputTE[mu1_TE], roiDescriptor1) != HLT::OK ) {
        ATH_MSG_ERROR("Navigation error while getting RoI descriptor 1" );
        m_mon_Errors.push_back(ERROR_No_RoIs);
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::NAV_ERROR;
     }
     if ( !roiDescriptor1 ) {
       ATH_MSG_ERROR("roiDescriptor1 is NULL" );
       return HLT::NAV_ERROR;
     }
    
     if ( getFeature(inputTE[mu2_TE], roiDescriptor2) != HLT::OK ) {
        ATH_MSG_ERROR("Navigation error while getting RoI descriptor 2" );
        m_mon_Errors.push_back(ERROR_No_RoIs);
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::NAV_ERROR;
     }
     if ( !roiDescriptor2 ) {
       ATH_MSG_ERROR("roiDescriptor2 is NULL" );
       return HLT::NAV_ERROR;
     }

     if (m_FTK) {
      if ( getFeature(inputTE[0], roiDescriptorTrk) != HLT::OK ) {
        ATH_MSG_ERROR("Navigation error while getting RoI descriptor Trk" );
        m_mon_Errors.push_back(ERROR_No_RoIs);
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::NAV_ERROR;
      }
     }

    ATH_MSG_DEBUG("Using inputTEs: "<< inputTE[mu1_TE] <<  " and "  << inputTE[mu2_TE] << " with Ids " << inputTE[mu1_TE]->getId()<< " AND "<< inputTE[mu2_TE]->getId()
        << "; RoI IDs = "   << roiDescriptor1->roiId()<< " AND   " <<roiDescriptor2->roiId() << ": Eta1 =    "   << roiDescriptor1->eta() << " Eta2= " <<roiDescriptor2->eta()
        << ", Phi1 =    "   << roiDescriptor1->phi() << " Phi2= " <<roiDescriptor2->phi() );
	if (m_FTK) {
	  ATH_MSG_DEBUG("Using inputTE for tracks: "<< inputTE[0] << " " << inputTE[0]->getId()
        << "; RoI IDs = "   << roiDescriptorTrk->roiId() << ": EtaTrk =    "   << roiDescriptorTrk->eta() << ", PhiTrk =    "   << roiDescriptorTrk->phi());
	}
    
    // Fill RoIs monitoring containers
    m_mon_RoI_RoI1Eta.push_back(roiDescriptor1->eta());
    m_mon_RoI_RoI1Phi.push_back(roiDescriptor1->phi());
    m_mon_RoI_RoI2Eta.push_back(roiDescriptor2->eta());
    m_mon_RoI_RoI2Phi.push_back(roiDescriptor2->phi());
    m_mon_RoI_dEtaRoI.push_back( fabs(roiDescriptor1->eta() - roiDescriptor2->eta()) );
    float tmp_RoI_dPhiRoI = roiDescriptor1->phi() - roiDescriptor2->phi();
    while (tmp_RoI_dPhiRoI >  M_PI) tmp_RoI_dPhiRoI -= 2*M_PI;
    while (tmp_RoI_dPhiRoI < -M_PI) tmp_RoI_dPhiRoI += 2*M_PI;
    m_mon_RoI_dPhiRoI.push_back( fabs(tmp_RoI_dPhiRoI) );
    
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
    ATH_MSG_DEBUG("Try to retrieve EFInfo container of muon 1" );
    //if(getFeatures(inputTE[mu1_TE], muonContainerEF1)!=HLT::OK ) {
    if(getFeaturesLinks<xAOD::MuonContainer,xAOD::MuonContainer>(inputTE[mu1_TE], muonContainerEF1)!=HLT::OK ) {
        ATH_MSG_DEBUG("Failed to get EFInfo feature of muon 1, exiting" );
        m_mon_Errors.push_back(ERROR_No_MuonEFInfoContainer);
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::OK; // FIXME should be HLT::MISSING_FEATURE ??
    }
    ATH_MSG_DEBUG("Found MuonContainer, Got MuonEF (1) Feature, size = " << muonContainerEF1.size());
    
    for ( const auto muel : muonContainerEF1 ) {
      if ( (*muel)->muonType() != xAOD::Muon::Combined && (*muel)->muonType() != xAOD::Muon::SegmentTagged) {
        ATH_MSG_DEBUG("Muon from roi1 is neither Combined or SegmentTagged - reject" );
        continue;
      }
      const xAOD::Muon* mu = *muel;
      if(!mu) continue;
      const xAOD::TrackParticle* idtp(0);
        const ElementLink<xAOD::TrackParticleContainer> & idtpEl = mu->inDetTrackParticleLink();
      if( idtpEl.isValid() ) idtp = *idtpEl;
      if (!idtp) {
          ATH_MSG_DEBUG("No innerdetector muon1 TrackParticle found" );
          continue;
      }
        //const Trk::Track* indetTrack = idtp->track();
        //if ( !indetTrack ) {
        //  ATH_MSG_DEBUG("No id muon1 id Trk::Track found" );
        //  continue;
        //}
        //addUnique(muidIDtracks1, indetTrack);
        addUnique(muonTPELtracks1,idtpEl);
        addUnique(muonTPELtracksMerged,idtpEl);
        
        ATH_MSG_DEBUG(" Comb muon 1 pt/eta/phi " << idtp->pt() << " / " << idtp->eta() << " / " << idtp->phi());
    } // for

    // second
    
    //std::vector<const xAOD::MuonContainer*> muonContainerEF2;
    ElementLinkVector<xAOD::MuonContainer> muonContainerEF2;
    ATH_MSG_DEBUG("Try to retrieve EFInfo container of muon 2" );
    //if(getFeatures(inputTE[mu2_TE], muonContainerEF2)!=HLT::OK ) {
    if(getFeaturesLinks<xAOD::MuonContainer,xAOD::MuonContainer>(inputTE[mu2_TE], muonContainerEF2)!=HLT::OK ) {
        ATH_MSG_DEBUG("Failed to get EFInfo feature of muon 2, exiting" );
        m_mon_Errors.push_back(ERROR_No_MuonEFInfoContainer);
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::OK; // FIXME - should be
        // return HLT::MISSING_FEATURE; // was HLT::OK
    }
    ATH_MSG_DEBUG("Found MuonContainer, Got MuonEF (2) Feature, size = " << muonContainerEF2.size());
    
    for ( const auto muel : muonContainerEF2 ) {
      if ( (*muel)->muonType() != xAOD::Muon::Combined && (*muel)->muonType() != xAOD::Muon::SegmentTagged) {
        ATH_MSG_DEBUG("Muon from roi2 is neither Combined or SegmentTagged - reject" );
        continue;
      }
      const xAOD::Muon* mu = *muel;
      if(!mu) continue;
      const xAOD::TrackParticle* idtp(0);
        const ElementLink<xAOD::TrackParticleContainer> & idtpEl = mu->inDetTrackParticleLink();
        if( idtpEl.isValid() ) idtp = *idtpEl;
      if (!idtp) {
          ATH_MSG_DEBUG("No innerdetector muon2 TrackParticle found" );
          continue;
      }
        //      const Trk::Track* indetTrack = idtp->track();
        //      if ( !indetTrack ) {
        //          ATH_MSG_DEBUG("No id muon2 id Trk::Track found" );
        //          continue;
        //      }
        //addUnique(muidIDtracks2, indetTrack);
        addUnique(muonTPELtracks2,idtpEl);
        addUnique(muonTPELtracksMerged,idtpEl);
        ATH_MSG_DEBUG("Comb muon 1 pt/eta/phi " << idtp->pt() << " / " << idtp->eta() << " / " << idtp->phi());

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
    
    if(msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "muonContainerEF1.size()= " << muonContainerEF1.size()<< " muonContainerEF2.size()= " << muonContainerEF2.size() 
          << " uonTPELtracks1.size()= "  << muonTPELtracks1.size() << " muonTPELtracks2.size()= "  << muonTPELtracks2.size() << " muonTPELtracksMerged.size()= "  << muonTPELtracksMerged.size() <<endmsg;
        
        for (auto muel: muonTPELtracks1) {
            msg() << MSG::DEBUG << "muonTPELtracks1: " << *muel << " " << (*muel)->pt() << " , " << (*muel)->eta() << " , " << (*muel)->phi() << " , " << (*muel)->charge() << endmsg;
        }
        for (auto muel: muonTPELtracks2) {
            msg() << MSG::DEBUG << "muonTPELtracks2: " << *muel << " " << (*muel)->pt() << " , " << (*muel)->eta() << " , " << (*muel)->phi() << " , " << (*muel)->charge() << endmsg;
        }
        for (auto muel: muonTPELtracksMerged) {
            msg() << MSG::DEBUG << "muonTPELtracksMerged: " << *muel << " " << (*muel)->pt() << " , " << (*muel)->eta() << " , " << (*muel)->phi() << " , " << (*muel)->charge() << endmsg;
        }
    } // if debug
    
    Amg::Vector3D vtx ( 0.,0.,0. );
    
    //   TrigEFBphys* trigPartBMuMuX (NULL);
    // FIXME - remove these 'new's
    //m_trigBphysColl_b = new TrigEFBphysContainer();
    //m_trigBphysColl_X = new TrigEFBphysContainer();
    
    m_TrigBphysColl_b = new xAOD::TrigBphysContainer();
    xAOD::TrigBphysAuxContainer xAODTrigBphysAuxColl_b;
    m_TrigBphysColl_b->setStore(&xAODTrigBphysAuxColl_b);
    
    m_TrigBphysColl_X = new xAOD::TrigBphysContainer();
    xAOD::TrigBphysAuxContainer xAODTrigBphysAuxColl_X;
    m_TrigBphysColl_X->setStore(&xAODTrigBphysAuxColl_X);
    
    // Tried combinations counter - protection against timeouts
    int nTriedCombinations(0);
    
    if( !(muonTPELtracks1.size()> 0 && muonTPELtracks2.size() > 0) )
    {
        ATH_MSG_DEBUG("No muon candidate found for one or both TEs" );
        m_mon_Errors.push_back(ERROR_No_MuonCandidate);
        //delete m_trigBphysColl_b;
        //delete m_trigBphysColl_X;
        delete m_TrigBphysColl_b;
        delete m_TrigBphysColl_X;
        m_TrigBphysColl_b = 0;
        m_TrigBphysColl_X = 0;
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
                ATH_MSG_DEBUG("Try to build muon pair from mu1 " << *(*pElItr) << ", mu2 " << *(*mElItr) );
                
                // check that we have two different muon tracks
                if (pTp==mTp) {
                    ATH_MSG_DEBUG("Muon tracks are same" );
                    m_mon_Errors.push_back(ERROR_SameMuon);
                    continue;
                }
                m_countPassedMuMuID++;
                
                // check if muons have opposite signs
                if( m_oppositeCharge && (mTp->charge())*(pTp->charge()) > 0) {
                    ATH_MSG_DEBUG("Muon pair rejected by opposite change check: mu1 " << pTp->charge() << ", mu2 " << mTp->charge() );
                    continue;
                }
                m_countPassedMuMuOS++;
                
                // check if dimuon mass within the range
                //float massMuMu = XMass(*pItr,*mItr,di_to_muons);
                float massMuMu = XMass(pTp,mTp,di_to_muons);
                if( massMuMu < m_lowerMuMuMassCut || massMuMu > m_upperMuMuMassCut ) {
                    ATH_MSG_DEBUG("Muon pair rejected by mass cut: m = " << massMuMu );
                    continue;
                }
                m_countPassedMuMuMass++;
		        ATH_MSG_DEBUG("Muon pair accepted by mass cut: m = " << massMuMu );
                
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
                    ATH_MSG_DEBUG(" Vertex fitting failed." );
                    if ( timerSvc() ) m_VtxFitTimer->pause();
                    continue;
                }
                if ( timerSvc() ) m_VtxFitTimer->pause();
                
                
                m_countPassedMuMuVtx++;
                
                float chi2MuMu = trigMuMuUniquePtr->fitchi2();
                if(chi2MuMu > m_muVtxChi2Cut || chi2MuMu < 0) {
                    ATH_MSG_DEBUG("Muon pair rejected by chi2 cut: chi2 = " << chi2MuMu );
                    continue;
                }
                m_countPassedMuMuVtxChi2++;
                
                
                // Fill muons monitoring containers
                m_mon_DiMu_Pt_Mu1.push_back (pTp->pt()/1000.);
                m_mon_DiMu_Pt_Mu2.push_back (mTp->pt()/1000.);
                m_mon_DiMu_Eta_Mu1.push_back(pTp->eta());
                m_mon_DiMu_Eta_Mu2.push_back(mTp->eta());
                m_mon_DiMu_Phi_Mu1.push_back(pTp->phi());
                m_mon_DiMu_Phi_Mu2.push_back(mTp->phi());
                
                m_mon_DiMu_dEtaMuMu.push_back( fabs(pTp->eta() - mTp->eta()) );
                float tmp_DiMu_dPhiMuMu = roiDescriptor1->phi() - roiDescriptor2->phi();
                while (tmp_DiMu_dPhiMuMu >  M_PI) tmp_DiMu_dPhiMuMu -= 2*M_PI;
                while (tmp_DiMu_dPhiMuMu < -M_PI) tmp_DiMu_dPhiMuMu += 2*M_PI;
                m_mon_DiMu_dPhiMuMu.push_back( fabs(tmp_DiMu_dPhiMuMu) );
                m_mon_DiMu_pTsumMuMu.push_back( (pTp->pt() + mTp->pt())/1000. );
                m_mon_DiMu_InvMassMuMu.push_back(massMuMu/1000.);
                m_mon_DiMu_VtxMassMuMu.push_back(trigMuMuUniquePtr->fitmass()/1000.);
                m_mon_DiMu_Chi2MuMu.push_back(chi2MuMu);
                
                m_mon_DiMu_n++;
                
                ATH_MSG_DEBUG(" Dimuon Sec Vtx at (" << trigMuMuUniquePtr->fitx() << " , " << trigMuMuUniquePtr->fity() << " , "
                    << trigMuMuUniquePtr->fitz()  <<  ") with chi2 = " << trigMuMuUniquePtr->fitchi2() << " ("
                    << trigMuMuUniquePtr->fitndof() << "  dof)" << " Mass= "<< massMuMu );

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
                HLT::ErrorCode status = getFeaturesLinks< xAOD::TrackParticleContainer, xAOD::TrackParticleContainer > (inputTE.front(), tracksRoiI1, m_input_trackCollectionKey);
                if(status != HLT::OK) {
                    ATH_MSG_WARNING( "Failed to get xAOD::TrackParticleCollection from the 1st trigger element" );
                    m_mon_Errors.push_back(ERROR_No_TrackColl);
                    delete m_TrigBphysColl_b;
                    delete m_TrigBphysColl_X;
                    m_TrigBphysColl_b = NULL;
                    m_TrigBphysColl_X = NULL;
                    return HLT::MISSING_FEATURE;
                }
                //status = getFeature(inputTE.back() , tracksRoiI2); // Need to insert the correct label
                status = getFeaturesLinks< xAOD::TrackParticleContainer, xAOD::TrackParticleContainer > (inputTE.back(), tracksRoiI2, m_input_trackCollectionKey);
                if(status != HLT::OK) {
                    ATH_MSG_WARNING( "Failed to get xAOD::TrackParticleCollection from the 2nd trigger element" );
                    m_mon_Errors.push_back(ERROR_No_TrackColl);
                    delete m_TrigBphysColl_b;
                    delete m_TrigBphysColl_X;
                    m_TrigBphysColl_b = NULL;
                    m_TrigBphysColl_X = NULL;
                    if ( timerSvc() ) m_TotTimer->stop();
                    return HLT::MISSING_FEATURE;
                }
                ATH_MSG_DEBUG("Ntracks RoI1: " << ( tracksRoiI1.empty() ? -1 : tracksRoiI1.size()) << ", Ntracks RoI2: " << ( tracksRoiI2.empty() ? -1 : tracksRoiI2.size() ) );
                // JW if containers are empty - it's not necessarily an issue; right?
                //                if ( tracksRoiI1.empty() || tracksRoiI2.empty()) {
                //                    ATH_MSG_WARNING( "Null track pointer" );
                //                    m_mon_Errors.push_back(ERROR_No_TrackColl);
                //                    delete m_TrigBphysColl_b;
                //                    delete m_TrigBphysColl_X;
                //                    m_TrigBphysColl_b = NULL;
                //                    m_TrigBphysColl_X = NULL;
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
                          if(trk->track()->perigeeParameters()) {
                            int qsign = (trk->track()->perigeeParameters()->parameters()[Trk::qOverP] > 0 ? +1 : -1);
                            msg() << MSG::DEBUG << trk->track() << " and Trk::Track: pt/eta/phi/q/d0/z0: "
                            << trk->track()->perigeeParameters()->pT()  << " , "
                            << trk->track()->perigeeParameters()->eta() << " , "
                            << trk->track()->perigeeParameters()->parameters()[Trk::phi0] << " , "
                            << qsign << " , "
                            << trk->track()->perigeeParameters()->parameters()[Trk::d0] << " , "
                            << trk->track()->perigeeParameters()->parameters()[Trk::z0] << "\n";
                          } else {
                              msg() << MSG::DEBUG << "Trk::Track with no perigeeParameters\n";
                          }
                        } else {
                            msg() << MSG::DEBUG << "   no Trk::Track\n" ;
                        }
                    }
                    msg() << MSG::DEBUG << endmsg;
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
                
                ATH_MSG_DEBUG("Matching summary: " << Found1Track<<Found2Track );
                
                
                ATH_MSG_DEBUG(" Now loop over TrackParticles to find tracks " );
                //                    xAOD::TrackParticleContainer::const_iterator trkIt =  merged_tracks.begin();
                //                    xAOD::TrackParticleContainer::const_iterator lastTrkIt = merged_tracks.end();
                //std::vector<const xAOD::TrackParticle*>::const_iterator trkIt     = merged_tracks.begin();
                std::vector<const xAOD::TrackParticle*>::const_iterator lastTrkIt = merged_tracks.end();
                
                ATH_MSG_DEBUG("Found tracks, ntrack= " << merged_tracks.size() );
                
                
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
                        ATH_MSG_FATAL("Track 1 doesn't match dereferenced elementlink: " << track1 << " " << *trackEL3 );
                        if ( timerSvc() ) m_TotTimer->stop();
                        return HLT::MISSING_FEATURE;
                    }
                    
                    // Check that it is not muon track
                    //                         if(itrk1==piTrk || itrk1==miTrk) {
                    //                             ATH_MSG_DEBUG("Track " << track1 << " was matched to a muon, skip it" );
                    //                             continue;
                    //                         }
                    
                    // ST: EL comparison does not work -- they appear to be always different
                    //                         if(trackEL3 == trackELmu1 || trackEL3 == trackELmu2 ) {
                    //                             ATH_MSG_DEBUG("Track " << track1 << " was matched to a muon, skip it" );
                    //                             continue;
                    //                         }
                    if( !(Found1Track && isUnique(*trkIt1,*trackELmu1)) || !(Found2Track &&isUnique(*trkIt1,*trackELmu2)) ) {
                        ATH_MSG_DEBUG("Track " << track1 << " was matched to a muon, skip it" );
                        continue;
                    }
                    
                    // Fill tracks monitoring containers
                    m_mon_Tracks_Pt.push_back (track1->pt()/1000.);
                    m_mon_Tracks_Eta.push_back(track1->eta());
                    m_mon_Tracks_Phi.push_back(track1->phi());
                    
                    m_mon_Tracks_n++;
                    
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
                                ATH_MSG_WARNING("Reached maximum number of B+ candidates to store " << m_maxBpToStore << "; following candidates won't be written out" );
                                m_mon_Errors.push_back(ERROR_MaxNumBpReached);
                              }
                              else
                                ATH_MSG_DEBUG("Do not write out " << m_countBpToStore+1 << "th B+ candidate" );
                            }
                            else {
                              m_TrigBphysColl_b->push_back(trigPartBplusMuMuKplus);
                              // trackParticleLinks are set by the vertexing method
                              trigPartBplusMuMuKplus->addParticleLink(ItrackEL3);				// Sergey S.
                              if(Found1Track) trigPartBplusMuMuKplus->addParticleLink(ELmu1);
                              if(Found2Track) trigPartBplusMuMuKplus->addParticleLink(ELmu2);
                            }
                            result=true;
                            m_mon_BMuMuK_n++;
                            m_countBpToStore++;
                            if(IdEvent!=m_lastEventPassedBplus) {
                                m_countPassedEventsBplus++;
                                m_lastEventPassedBplus = IdEvent;
                            }
                            
                        }
                    } //  end if(m_doB_KMuMuDecay)
                    
                    
                    // Protection
                    if( nTriedCombinations > m_maxNcombinations ) {
                        ATH_MSG_DEBUG("Too many track combinations: nTried = " << nTriedCombinations << ", while Max= " << m_maxNcombinations 
                                      << ". Terminate the loop." << "  (BphysColl size=" << m_TrigBphysColl_b->size() << ")" );
                        if(m_TrigBphysColl_b->size() > 0)
                            m_mon_Errors.push_back(ERROR_TooManyComb_Acc);
                        else
                            m_mon_Errors.push_back(ERROR_TooManyComb_Rej);
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
                        
			if(m_doBd_KstarMuMuDecay || m_doBs_Phi1020MuMuDecay || m_doLb_LambdaMuMuDecay || m_doBc_DsMuMuDecay || m_doBc_DplusMuMuDecay || m_doBc_DstarMuMuDecay || m_doBc_D0MuMuDecay) {
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
                                ATH_MSG_FATAL("Track 2 doesn't match dereferenced elementlink: " << track2 << " " << *trackEL4 );
                                if ( timerSvc() ) m_TotTimer->stop();
                                return HLT::MISSING_FEATURE;
                            }

                            
                                // Check that it is not muon track
//                                 if(itrk2==piTrk || itrk2==miTrk) {
//                                     ATH_MSG_DEBUG("Track " << track2 << " was matched to a muon, skip it" );
//                                     continue;
//                                 }
                                if( !(Found1Track && isUnique(*trkIt2,*trackELmu1)) || !(Found2Track &&isUnique(*trkIt2,*trackELmu2)) ) {
                                    ATH_MSG_DEBUG("Track " << track2 << " was matched to a muon, skip it" );
                                    continue;
                                }
                                
                                // check that track1 and track2 have opposite charges
				//                                if( (track1->charge())*(track2->charge()) > 0) {
				//                                    ATH_MSG_DEBUG("Track pair rejected by opposite charge check: "
				//                                    << track1 << " = " << track1->charge() << ",  "
				//                                    << track2 << " = " << track2->charge() );
				//                                    continue;
				//                                }
                                
                                //
                                // Bd -> mu mu K0* (K+ pi-) part
                                //
                                if(m_doBd_KstarMuMuDecay) {

				              // check that track1 and track2 have opposite charges
                                 if( (track1->charge())*(track2->charge()) > 0) {      
				                    ATH_MSG_DEBUG("Track pair rejected by opposite charge check: "
                                     << track1 << " = " << track1->charge() << ",  "  << track2 << " = " << track2->charge() );
				                 } else {
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
                                            ATH_MSG_WARNING("Reached maximum number of Bd candidates to store " << m_maxBdToStore << "; following candidates won't be written out" );
                                            m_mon_Errors.push_back(ERROR_MaxNumBdReached);
                                          }
                                          else
                                            ATH_MSG_DEBUG("Do not write out " << m_countBdToStore+1 << "th Bd candidate" );
                                        }
                                        else {
                                          m_TrigBphysColl_b->push_back(xaod_trigPartBdMuMuKstar);
                                          m_TrigBphysColl_X->push_back(xaod_trigPartKstar );
                                          // trackParticleLinks are set by the vertexing method
                                          xaod_trigPartBdMuMuKstar->addParticleLink(ItrackEL3);				// Sergey S.
                                          xaod_trigPartBdMuMuKstar->addParticleLink(ItrackEL4);
                                          if(Found1Track) xaod_trigPartBdMuMuKstar->addParticleLink(ELmu1);
                                          if(Found2Track) xaod_trigPartBdMuMuKstar->addParticleLink(ELmu2);
                                          
                                          xaod_trigPartKstar->addParticleLink(ItrackEL3);				// Sergey S.
                                          xaod_trigPartKstar->addParticleLink(ItrackEL4);

                                          // set the secondary link; note, does not set correctly for persistified data.
                                          // see code further down for the re-setting of these links
                                          int iKstar = m_TrigBphysColl_X->size() - 1;
                                          ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*m_TrigBphysColl_X,iKstar);
                                          xaod_trigPartBdMuMuKstar->setSecondaryDecayLink(trigPartXEL);
                                        }
                                        result=true;
                                        m_mon_BdMuMuKs_n++;
                                        m_countBdToStore++;
                                        if(IdEvent!=m_lastEventPassedBplus) {
                                            m_countPassedEventsBplus++;
                                            m_lastEventPassedBplus = IdEvent;
                                        }

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
                                            ATH_MSG_WARNING("Reached maximum number of Bd candidates to store " << m_maxBdToStore << "; following candidates won't be written out" );
                                            m_mon_Errors.push_back(ERROR_MaxNumBdReached);
                                          }
                                          else
                                            ATH_MSG_DEBUG("Do not write out " << m_countBdToStore+1 << "th Bd candidate" );
                                        }
                                        else {
                                          m_TrigBphysColl_b->push_back(xaod_trigPartBdMuMuKstar);
                                          m_TrigBphysColl_X->push_back(xaod_trigPartKstar );
                                          // trackParticleLinks are set by the vertexing method
                                          xaod_trigPartBdMuMuKstar->addParticleLink(ItrackEL4);				// Sergey S.
                                          xaod_trigPartBdMuMuKstar->addParticleLink(ItrackEL3);
                                          if(Found1Track) xaod_trigPartBdMuMuKstar->addParticleLink(ELmu1);
                                          if(Found2Track) xaod_trigPartBdMuMuKstar->addParticleLink(ELmu2);
                                          
                                          xaod_trigPartKstar->addParticleLink(ItrackEL4);				// Sergey S.
                                          xaod_trigPartKstar->addParticleLink(ItrackEL3);
                                          
                                          // set the secondary link; note, does not set correctly for persistified data.
                                          // see code further down for the re-setting of these links
                                          int iKstar = m_TrigBphysColl_X->size() - 1;
                                          ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*m_TrigBphysColl_X,iKstar);
                                          xaod_trigPartBdMuMuKstar->setSecondaryDecayLink(trigPartXEL);
                                        }
                                        result=true;
                                        m_mon_BdMuMuKs_n++;
                                        m_countBdToStore++;
                                        if(IdEvent!=m_lastEventPassedBplus) {
                                            m_countPassedEventsBplus++;
                                            m_lastEventPassedBplus = IdEvent;
                                        }
                                        
                                    } // if hypo2
                                   } // end check that track1 and track2 have opposite charges
                                } // if m_doBd_KstarMuMuDecay
                                
                                
                                //
                                // Bs -> mu mu Phi (K K) part
                                //
                                if(m_doBs_Phi1020MuMuDecay) {

				  // check that track1 and track2 have opposite charges
				  if( (track1->charge())*(track2->charge()) > 0) {      
				     ATH_MSG_DEBUG("Track pair rejected by opposite charge check: "
                                     << track1 << " = " << track1->charge() << ",  "  << track2 << " = " << track2->charge() );
				  } else {

                                    xAOD::TrigBphys* xaod_trigPartPhi(nullptr);
                                    xAOD::TrigBphys* xaod_trigPartBsMuMuPhi = checkBMuMu2X(trackEL3,trackEL4,trackELmu1,trackELmu2,
                                                                                           bS_to_Phi,xaod_trigPartPhi);

                                    ++nTriedCombinations;
                                    if (xaod_trigPartBsMuMuPhi) {
                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartBsMuMuPhi,{*trackEL3,*trackEL4,*trackELmu1,*trackELmu2});
                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartPhi,        {*trackEL3,*trackEL4});
                                        
                                        if(m_maxBsToStore >= 0 && m_countBsToStore >= m_maxBsToStore) {
                                          if(m_countBsToStore == m_maxBsToStore) {
                                            ATH_MSG_WARNING("Reached maximum number of Bs candidates to store " << m_maxBsToStore << "; following candidates won't be written out" );
                                            m_mon_Errors.push_back(ERROR_MaxNumBsReached);
                                          }
                                          else
                                            ATH_MSG_DEBUG("Do not write out " << m_countBsToStore+1 << "th Bs candidate" );
                                        }
                                        else {
                                          m_TrigBphysColl_b->push_back(xaod_trigPartBsMuMuPhi);
                                          m_TrigBphysColl_X->push_back(xaod_trigPartPhi );
                                          // trackParticleLinks are set by the vertexing method
                                          xaod_trigPartBsMuMuPhi->addParticleLink(ItrackEL3);				// Sergey S.
                                          xaod_trigPartBsMuMuPhi->addParticleLink(ItrackEL4);
                                          if(Found1Track) xaod_trigPartBsMuMuPhi->addParticleLink(ELmu1);
                                          if(Found2Track) xaod_trigPartBsMuMuPhi->addParticleLink(ELmu2);
                                          
                                          xaod_trigPartPhi->addParticleLink(ItrackEL3);				// Sergey S.
                                          xaod_trigPartPhi->addParticleLink(ItrackEL4);
                                          
                                          // set the secondary link; note, does not set correctly for persistified data.
                                          // see code further down for the re-setting of these links
                                          int iPhi = m_TrigBphysColl_X->size() - 1;
                                          ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*m_TrigBphysColl_X,iPhi);
                                          xaod_trigPartBsMuMuPhi->setSecondaryDecayLink(trigPartXEL);
                                        }
                                        result=true;
                                        m_mon_BsMuMuPhi_n++;
                                        m_countBsToStore++;
                                        if(IdEvent!=m_lastEventPassedBplus) {
                                            m_countPassedEventsBplus++;
                                            m_lastEventPassedBplus = IdEvent;
                                        }
                                        
                                        
                                    }
                                   } // end check that track1 and track2 have opposite charges
                                } // m_doBs_Phi1020MuMuDecay
                                
                                
                                //
                                // Lambda_b -> mu mu Lambda part
                                //
                                
                                if(m_doLb_LambdaMuMuDecay) {

                                  // check that track1 and track2 have opposite charges
                                  if( (track1->charge())*(track2->charge()) > 0) {      
                                    ATH_MSG_DEBUG("Track pair rejected by opposie charge check: "
                                                            << track1 << " = " << track1->charge() << ",  " << track2 << " = " << track2->charge() );
                                  } else {
                                    ATH_MSG_DEBUG("Track pair accepted by opposie charge check: "
                                                            << track1 << " = " << track1->charge() << ",  "  << track2 << " = " << track2->charge() );

                                    std::vector<const xAOD::TrackParticle*> bTracks;
                                    bTracks.clear();
                                    bTracks.push_back(track1);
                                    bTracks.push_back(track2);
                                            
                                    double PtLambda = PtVal (bTracks);
                                          
                                    if ( PtLambda > m_lowerLambda_PtCut ) {
                                      
                                      bTracks.push_back(*trackELmu1);
                                      bTracks.push_back(*trackELmu2);
                                              
                                      double PtLb = PtVal (bTracks);
                                            
                                      if ( PtLb > m_lowerLb_PtCut ) {


                                        // Now L_b uses simple 4-prong vertex fit instead of cascade vertexing
                                        // 29.06.2016, L.Gladilin: switch to cascade vertexing

                                        xAOD::TrigBphys* xaod_trigPartLambda(nullptr);

                                        if ( fabs(1./(track1->qOverP())) >=  fabs(1./(track2->qOverP())) ) {
                                          // 29.06.2016, L.Gladilin: take track with larger momemtum as a proton

                                          // Mass hypothesis 1: track1 = proton, track2 = pion
    //                                       xAOD::TrigBphys* xaod_trigPartLbMuMuLambda = checkBMuMu2X(trackEL3,trackEL4,trackELmu1,trackELmu2,
    //                                                                                                 lB_to_L,xaod_trigPartLambda);
                                          xAOD::TrigBphys* xaod_trigPartLbMuMuLambda = checkLbMuMuLambda(trackEL3,trackEL4,trackELmu1,trackELmu2,
                                                                                                  xaod_trigPartLambda);
                                          ++nTriedCombinations;
                                          if (xaod_trigPartLbMuMuLambda) {
                                            m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartLbMuMuLambda,{*trackEL3,*trackEL4,*trackELmu1,*trackELmu2});
                                            m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartLambda,      {*trackEL3,*trackEL4});
                                            
                                            if(m_maxLbToStore >= 0 && m_countLbToStore >= m_maxLbToStore) {
                                              if(m_countLbToStore == m_maxLbToStore) {
                                                ATH_MSG_WARNING("Reached maximum number of Lb candidates to store " << m_maxLbToStore << "; following candidates won't be written out" );
                                                m_mon_Errors.push_back(ERROR_MaxNumLbReached);
                                              }
                                              else
                                                ATH_MSG_DEBUG("Do not write out " << m_countLbToStore+1 << "th Lb candidate" );
                                            }
                                            else {
                                              m_TrigBphysColl_b->push_back(xaod_trigPartLbMuMuLambda);
                                              m_TrigBphysColl_X->push_back(xaod_trigPartLambda );
                                              // trackParticleLinks are set by the vertexing method
                                              xaod_trigPartLbMuMuLambda->addParticleLink(ItrackEL3);				// Sergey S.
                                              xaod_trigPartLbMuMuLambda->addParticleLink(ItrackEL4);
                                              if(Found1Track) xaod_trigPartLbMuMuLambda->addParticleLink(ELmu1);
                                              if(Found2Track) xaod_trigPartLbMuMuLambda->addParticleLink(ELmu2);
                                              
                                              xaod_trigPartLambda->addParticleLink(ItrackEL3);				// Sergey S.
                                              xaod_trigPartLambda->addParticleLink(ItrackEL4);
                                              
                                              // set the secondary link; note, does not set correctly for persistified data.
                                              // see code further down for the re-setting of these links
                                              int iLambda = m_TrigBphysColl_X->size() - 1;
                                              ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*m_TrigBphysColl_X,iLambda);
                                              xaod_trigPartLbMuMuLambda->setSecondaryDecayLink(trigPartXEL);
                                            }
                                            result=true;
                                            m_mon_LbMuMuLambda_n++;
                                            m_countLbToStore++;
                                            if(IdEvent!=m_lastEventPassedBplus) {
                                                m_countPassedEventsBplus++;
                                                m_lastEventPassedBplus = IdEvent;
                                            }
                                            
                                          } // if hypo1
      //                                     xaod_trigPartLambda = nullptr; //hypo 1 added to SG(if exists), so reuse the pointer
                                        
                                        } else {

                                          // Mass hypothesis 2: track1 = pion, track2 = proton
      //                                     xaod_trigPartLbMuMuLambda = checkBMuMu2X(trackEL4,trackEL3,trackELmu1,trackELmu2,
      //                                                                              lB_to_L,xaod_trigPartLambda);
                                          xAOD::TrigBphys* xaod_trigPartLbMuMuLambda = checkLbMuMuLambda(trackEL4,trackEL3,trackELmu1,trackELmu2,
                                                                                                    xaod_trigPartLambda);
                                          ++nTriedCombinations;
                                          if (xaod_trigPartLbMuMuLambda) {
                                              m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartLbMuMuLambda,{*trackEL4,*trackEL3,*trackELmu1,*trackELmu2});
                                              m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartLambda,      {*trackEL4,*trackEL3});
                                              
                                              if(m_maxLbToStore >= 0 && m_countLbToStore >= m_maxLbToStore) {
                                                if(m_countLbToStore == m_maxLbToStore) {
                                                  ATH_MSG_WARNING("Reached maximum number of Lb candidates to store " << m_maxLbToStore << "; following candidates won't be written out" );
                                                  m_mon_Errors.push_back(ERROR_MaxNumLbReached);
                                                }
                                                else
                                                  ATH_MSG_DEBUG("Do not write out " << m_countLbToStore+1 << "th Lb candidate" );
                                              }
                                              else {
                                                m_TrigBphysColl_b->push_back(xaod_trigPartLbMuMuLambda);
                                                m_TrigBphysColl_X->push_back(xaod_trigPartLambda );
                                                // trackParticleLinks are set by the vertexing method
                                                xaod_trigPartLbMuMuLambda->addParticleLink(ItrackEL4);				// Sergey S.
                                                xaod_trigPartLbMuMuLambda->addParticleLink(ItrackEL3);
                                                if(Found1Track) xaod_trigPartLbMuMuLambda->addParticleLink(ELmu1);
                                                if(Found2Track) xaod_trigPartLbMuMuLambda->addParticleLink(ELmu2);
                                                
                                                xaod_trigPartLambda->addParticleLink(ItrackEL4);				// Sergey S.
                                                xaod_trigPartLambda->addParticleLink(ItrackEL3);
                                                
                                                // set the secondary link; note, does not set correctly for persistified data.
                                                // see code further down for the re-setting of these links
                                                int iLambda = m_TrigBphysColl_X->size() - 1;
                                                ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*m_TrigBphysColl_X,iLambda);
                                                xaod_trigPartLbMuMuLambda->setSecondaryDecayLink(trigPartXEL);
                                              }
                                              result=true;
                                              m_mon_LbMuMuLambda_n++;
                                              m_countLbToStore++;
                                              if(IdEvent!=m_lastEventPassedBplus) {
                                                  m_countPassedEventsBplus++;
                                                  m_lastEventPassedBplus = IdEvent;
                                              }
                                          } // if hypo2
                                        } // take track with larger momemtum as a proton
                                        
                                      } // Pt Lb
                                    } // Pt Lambda

                                  } // end check that track1 and track2 have opposite charges
                                } // m_doLb_LambdaMuMuDecay
                                
                                
                                //
                                // Bc -> mu mu Ds (Phi pi) part
                                //
                                if(m_doBc_DsMuMuDecay) {

                                  // check that track1 and track2 have opposite charges
                                  if( (track1->charge())*(track2->charge()) > 0) {      
                                    ATH_MSG_DEBUG("Bc -> Ds: Track pair rejected by opposie charge check: "
                                                            << track1 << " = " << track1->charge() << ",  "   
                                                            << track2 << " = " << track2->charge() );
                                  } else {
                                    
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
                                                ATH_MSG_DEBUG("Track " << track3 << " is the same as another" );
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
                                                ATH_MSG_FATAL("Track 3 doesn't match dereferenced elementlink: " << track3 << " " << *trackEL5 );
                                                if ( timerSvc() ) m_TotTimer->stop();
                                                return HLT::MISSING_FEATURE;
                                            }

                                            
                                            
                                            // Check that it is not muon track
//                                                 if(itrk3==piTrk || itrk3==miTrk) {
//                                                     ATH_MSG_DEBUG("Track " << track3 << " was matched to a muon, skip it" );
//                                                     continue;
//                                                 }
                                            if( !(Found1Track && isUnique(*trkIt3,*trackELmu1)) || !(Found2Track &&isUnique(*trkIt3,*trackELmu2)) ) {
                                                ATH_MSG_DEBUG("Track " << track3 << " was matched to a muon, skip it" );
                                                continue;
                                            }

                                            std::vector<const xAOD::TrackParticle*> bTracks;
                                            bTracks.clear();
                                            bTracks.push_back(track1);
                                            bTracks.push_back(track2);
                                            bTracks.push_back(track3);
                                      
                                            double PtDs = PtVal (bTracks);
                                    
                                            if ( PtDs > m_lowerDs_PtCut ) {
                                              
                                              bTracks.push_back(*trackELmu1);
                                              bTracks.push_back(*trackELmu2);
                                        
                                              double PtBc = PtVal (bTracks);
                                      
                                              if ( PtBc > m_lowerBcDs_PtCut ) {
                                                
                                                xAOD::TrigBphys* trigPartDs(nullptr);
                                                xAOD::TrigBphys* trigPartBcMuMuDs = checkBcMuMuDs(trackEL3,trackEL4,trackEL5,
                                                                                                  trackELmu1,trackELmu2,
                                                                                                  xPhiMass,trigPartDs);
                                                nTriedCombinations++;
                                                if (trigPartBcMuMuDs) {
                                                    if(m_maxBcToStore >= 0 && m_countBcToStore >= m_maxBcToStore) {
                                                      if(m_countBcToStore == m_maxBcToStore) {
                                                        ATH_MSG_WARNING("Reached maximum number of Bc candidates to store " << m_maxBcToStore << "; following candidates won't be written out" );
                                                        m_mon_Errors.push_back(ERROR_MaxNumBcReached);
                                                      }
                                                      else
                                                        ATH_MSG_DEBUG("Do not write out " << m_countBcToStore+1 << "th Bc candidate" );
                                                    }
                                                    else {
                                                      m_TrigBphysColl_b->push_back( trigPartBcMuMuDs );
                                                      m_TrigBphysColl_X->push_back( trigPartDs );
                                                      m_bphysHelperTool->setBeamlineDisplacement(trigPartBcMuMuDs,
                                                                                                {*trkIt1,*trkIt2,*trkIt3,*trackELmu1,*trackELmu2});
                                                      m_bphysHelperTool->setBeamlineDisplacement(trigPartDs,
                                                                                                {*trkIt1,*trkIt2,*trkIt3});
                                                      trigPartDs->addParticleLink(ItrackEL3);
                                                      trigPartDs->addParticleLink(ItrackEL4);
                                                      trigPartDs->addParticleLink(ItrackEL5);
                                                    
                                                      int iDs = m_TrigBphysColl_X->size() - 1;
                                                      ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*m_TrigBphysColl_X,iDs);
                                                      
                                                      trigPartBcMuMuDs->addParticleLink(ItrackEL3);
                                                      trigPartBcMuMuDs->addParticleLink(ItrackEL4);
                                                      trigPartBcMuMuDs->addParticleLink(ItrackEL5);
                                                      if(Found1Track) trigPartBcMuMuDs->addParticleLink(ELmu1);
                                                      if(Found2Track) trigPartBcMuMuDs->addParticleLink(ELmu2);
                                                      trigPartBcMuMuDs->setSecondaryDecayLink(trigPartXEL);
                                                    }

                                                    result=true;
                                                    m_mon_BcMuMuDs_n++;
                                                    m_countBcToStore++;
                                                    if(IdEvent!=m_lastEventPassedBc) {
                                                        m_countPassedEventsBc++;
                                                        m_lastEventPassedBc = IdEvent;
                                                    }

                                                } // if trigPartBcMuMuDs

                                              } // Pt Bc
                                            } // Pt Ds
                                        } // for loop over track3
                                      } // in mass range
                                    } // in phi mass range
                                  } // end check that track1 and track2 have opposite charges
                                } // if m_doBc_DsMuMuDecay
                                
                                
                                                
                                
                                //
                                // Bc -> mu mu D+ (pi pi K) part
                                //
                                if(m_doBc_DplusMuMuDecay) {

				  // check that track1 and track2 have same charges
				  if( (track1->charge())*(track2->charge()) < 0) {      
				     ATH_MSG_DEBUG("Bc -> D+: Track pair rejected by same charge check: "
                                     << track1 << " = " << track1->charge() << ",  "   
                                     << track2 << " = " << track2->charge() );
				  } else {
				     ATH_MSG_DEBUG("Bc -> D+: Track pair accepted by same charge check: "
                                     << track1 << " = " << track1->charge() << ",  " << track2 << " = " << track2->charge() );
                                    
                                    double xPiPiMass = XMass(track1,track2,bC_to_PiPi); // decay = bC_to_PiPi to actually calculate m(pi pi)
                                    
                                    if ( xPiPiMass < m_upperDplus_MassCut - KPLUSMASS ) {
                                        
                                        double xPiPiMuMuMass = XMuMuMass (*trackELmu1,*trackELmu2,track1,track2,bC_to_PiPi);
                                        
                                        if ( xPiPiMuMuMass < m_upperBc_DplusMuMuMassCut - KPLUSMASS ) {
                                    
                                            // 3rd loop over tracks
                                            //xAOD::TrackParticleContainer::const_iterator trkIt3 =  xAODTrkParticleCont->begin();
                                            std::vector<const xAOD::TrackParticle*>::const_iterator trkIt3 = merged_tracks.begin();

                                            for (int itrk3=0 ; trkIt3 != lastTrkIt; itrk3++, trkIt3++)
                                            {
                                                //const Trk::Track* track3 = (*trkIt3)->track();
                                                const xAOD::TrackParticle* track3 = *trkIt3;

                                                // looping over all tracks, so expect to same tracks, but should skip those combinations
                                                if(itrk3==itrk1 || itrk3==itrk2) {
                                                    ATH_MSG_DEBUG("Track " << track3 << " is the same as another" );
                                                    continue;
                                                }

                                                
                                                ElementLink<xAOD::TrackParticleContainer> & trackEL5 = mapTrackToIndex[*trkIt3].elLink;
                                                ElementLink<xAOD::IParticleContainer> ItrackEL5;
                                                ItrackEL5.resetWithKeyAndIndex(trackEL5.dataID(), trackEL5.index());

                                                
                                                if (*trackEL5 != track3) {
                                                    ATH_MSG_FATAL("Track 3 doesn't match dereferenced elementlink: " << track3 << " " << *trackEL5 );
                                                    if ( timerSvc() ) m_TotTimer->stop();
                                                    return HLT::MISSING_FEATURE;
                                                }

                                                
                                                
                                                // Check that it is not muon track
                                                if( !(Found1Track && isUnique(*trkIt3,*trackELmu1)) || !(Found2Track &&isUnique(*trkIt3,*trackELmu2)) ) {
                                                    ATH_MSG_DEBUG("Track " << track3 << " was matched to a muon, skip it" );
                                                    continue;
                                                }
                                                // check that track1 and track3 have opposite charges
                                                if( (track1->charge())*(track3->charge()) > 0) {
                                                 ATH_MSG_DEBUG("Bc -> D+: 3rd track rejected by opposite charge check: "
                                                 << track1 << " = " << track1->charge() << ",  " << track3 << " = " << track3->charge() );
                                                } else {
                                    
                                                ATH_MSG_DEBUG("Bc -> D+: 3rd track accepted by opposite charge check: "
                                                << track1 << " = " << track1->charge() << ",  " << track3 << " = " << track3->charge() );

                                                std::vector<const xAOD::TrackParticle*> bTracks;
                                                bTracks.clear();
                                                bTracks.push_back(track1);
                                                bTracks.push_back(track2);
                                                bTracks.push_back(track3);
                                          
                                                double PtDplus = PtVal (bTracks);
                                        
                                                if ( PtDplus > m_lowerDplus_PtCut ) {
                                                
                                                bTracks.push_back(*trackELmu1);
                                                bTracks.push_back(*trackELmu2);
                                          
                                                double PtBc = PtVal (bTracks);
                                        
                                                if ( PtBc > m_lowerBcDplus_PtCut ) {
                                                
                                                xAOD::TrigBphys* trigPartDplus(nullptr);
                                                xAOD::TrigBphys* trigPartBcMuMuDplus = checkBcMuMuDplus(trackEL3,trackEL4,trackEL5,
                                                                                                  trackELmu1,trackELmu2,
                                                                                                  trigPartDplus);
                                                nTriedCombinations++;
                                                if (trigPartBcMuMuDplus) {
                                                    if(m_maxBcDplusToStore >= 0 && m_countBcDplusToStore >= m_maxBcDplusToStore) {
                                                      if(m_countBcDplusToStore == m_maxBcDplusToStore) {
                                                        ATH_MSG_WARNING("Reached maximum number of Bc->D+ candidates to store " << m_maxBcDplusToStore << "; following candidates won't be written out" );
                                                        m_mon_Errors.push_back(ERROR_MaxNumBcDplusReached);
                                                      }
                                                      else
                                                        ATH_MSG_DEBUG("Do not write out " << m_countBcDplusToStore+1 << "th Bc->D+ candidate" );
                                                      //TJ: delete trigPartBcMuMuDplus;
                                                      //TJ: delete trigPartDplus;
                                                    }
                                                    else {
                                                      m_TrigBphysColl_b->push_back( trigPartBcMuMuDplus );
                                                      m_TrigBphysColl_X->push_back( trigPartDplus );

                                                      m_bphysHelperTool->setBeamlineDisplacement(trigPartBcMuMuDplus,
                                                                                                {*trkIt1,*trkIt2,*trkIt3,*trackELmu1,*trackELmu2});
                                                      m_bphysHelperTool->setBeamlineDisplacement(trigPartDplus,
                                                                                                {*trkIt1,*trkIt2,*trkIt3});
                                                      trigPartDplus->addParticleLink(ItrackEL3);
                                                      trigPartDplus->addParticleLink(ItrackEL4);
                                                      trigPartDplus->addParticleLink(ItrackEL5);
                                                    
                                                      int iDplus = m_TrigBphysColl_X->size() - 1;
                                                      ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*m_TrigBphysColl_X,iDplus);
                                                      
                                                      trigPartBcMuMuDplus->addParticleLink(ItrackEL3);
                                                      trigPartBcMuMuDplus->addParticleLink(ItrackEL4);
                                                      trigPartBcMuMuDplus->addParticleLink(ItrackEL5);
                                                      if(Found1Track) trigPartBcMuMuDplus->addParticleLink(ELmu1);
                                                      if(Found2Track) trigPartBcMuMuDplus->addParticleLink(ELmu2);
                                                      trigPartBcMuMuDplus->setSecondaryDecayLink(trigPartXEL);
                                                    }
                                                    
                                                    result=true;
                                                    m_mon_BcMuMuDplus_n++;
                                                    m_countBcDplusToStore++;
                                                    if(IdEvent!=m_lastEventPassedBcDplus) {
                                                        m_countPassedEventsBcDplus++;
                                                        m_lastEventPassedBcDplus = IdEvent;
                                                    }

                                                } // if trigPartBcMuMuDs

						} // Pt D+
						} // Pt BC
						} // check of 3rd track charge
                                            } // for loop over track3
                                        } // in mass range
                                    } // in phi mass range
                                   } // end check that track1 and track2 have same charges
                                } // if m_doBc_DplusMuMuDecay

                                //
                                // Bc -> mu mu D* ( D0(K pi) pi) part
                                //
                                if(m_doBc_DstarMuMuDecay) {

				  // check that track1 and track2 have opposite charges
				  if( (track1->charge())*(track2->charge()) > 0) {      
				     ATH_MSG_DEBUG("Bc -> D*: Track pair rejected by opposite charge check: "
                                     << track1 << " = " << track1->charge() << ",  " << track2 << " = " << track2->charge() );
				  } else {

				     ATH_MSG_DEBUG("Bc -> D*: Track pair accepted by opposite charge check: "
                                     << track1 << " = " << track1->charge() << ",  " << track2 << " = " << track2->charge() );

				  if( (track1->pt()) < m_lowerDstarKpi_PtCut || (track2->pt()) < m_lowerDstarKpi_PtCut ) {      
				     ATH_MSG_DEBUG("Bc -> D*: Track pair rejected by minimal pT(track) check: "
                                     << track1 << " = " << track1->pt() << ",  " << track2 << " = " << track2->pt() );
				  } else {

				     ATH_MSG_DEBUG("Bc -> D*: Track pair accepted by minimal pT(track) check: "
                                     << track1 << " = " << track1->pt() << ",  " << track2 << " = " << track2->pt() );
                                    
                                    double xD0Mass1 = XMass(track1,track2,bD_to_Kstar); // decay = bD_to_Kstar to actually calculate m(K pi)
                                    double xD0Mass2 = XMass(track2,track1,bD_to_Kstar); // decay = bD_to_Kstar to actually calculate m(K pi)
                                                                        
                                    if ( (xD0Mass1 > m_lowerD0Dstar_MassCut && xD0Mass1 < m_upperD0Dstar_MassCut) ||
                                         (xD0Mass2 > m_lowerD0Dstar_MassCut && xD0Mass2 < m_upperD0Dstar_MassCut) ) {
                                        
				      double xD0MuMuMass1 = XMuMuMass (*trackELmu1,*trackELmu2,track1,track2,bD_to_Kstar);
				      double xD0MuMuMass2 = XMuMuMass (*trackELmu1,*trackELmu2,track2,track1,bD_to_Kstar);
                                        
                                        if ( (xD0Mass1 > m_lowerD0Dstar_MassCut && xD0Mass1 < m_upperD0Dstar_MassCut && xD0MuMuMass1 < m_upperBc_DstarMuMuMassCut - PIMASS) ||
                                             (xD0Mass2 > m_lowerD0Dstar_MassCut && xD0Mass2 < m_upperD0Dstar_MassCut && xD0MuMuMass2 < m_upperBc_DstarMuMuMassCut - PIMASS) ) {
                                    
                                            // 3rd loop over tracks
                                            //xAOD::TrackParticleContainer::const_iterator trkIt3 =  xAODTrkParticleCont->begin();
                                            std::vector<const xAOD::TrackParticle*>::const_iterator trkIt3 = merged_tracks.begin();

                                            for (int itrk3=0 ; trkIt3 != lastTrkIt; itrk3++, trkIt3++)
                                            {
                                                //const Trk::Track* track3 = (*trkIt3)->track();
                                                const xAOD::TrackParticle* track3 = *trkIt3;

                                                // looping over all tracks, so expect to same tracks, but should skip those combinations
                                                if(itrk3==itrk1 || itrk3==itrk2) {
                                                    ATH_MSG_DEBUG("Track " << track3 << " is the same as another" );
                                                    continue;
                                                }

                                                
                                                ElementLink<xAOD::TrackParticleContainer> & trackEL5 = mapTrackToIndex[*trkIt3].elLink;
                                                ElementLink<xAOD::IParticleContainer> ItrackEL5;
                                                ItrackEL5.resetWithKeyAndIndex(trackEL5.dataID(), trackEL5.index());

                                                
                                                if (*trackEL5 != track3) {
                                                    ATH_MSG_FATAL("Track 3 doesn't match dereferenced elementlink: " << track3 << " " << *trackEL5 );
                                                    if ( timerSvc() ) m_TotTimer->stop();
                                                    return HLT::MISSING_FEATURE;
                                                }
                                                
                                                
                                                // Check that it is not muon track
                                                if( !(Found1Track && isUnique(*trkIt3,*trackELmu1)) || !(Found2Track &&isUnique(*trkIt3,*trackELmu2)) ) {
                                                    ATH_MSG_DEBUG("Track " << track3 << " was matched to a muon, skip it" );
                                                    continue;
                                                }

                                                if ( !((xD0Mass1 > m_lowerD0Dstar_MassCut && xD0Mass1 < m_upperD0Dstar_MassCut && xD0MuMuMass1 < m_upperBc_DstarMuMuMassCut - PIMASS &&
						      ((track1->charge())*(track3->charge()) < 0) ) ||
                                                     (xD0Mass2 > m_lowerD0Dstar_MassCut && xD0Mass2 < m_upperD0Dstar_MassCut && xD0MuMuMass2 < m_upperBc_DstarMuMuMassCut - PIMASS &&
						      ((track2->charge())*(track3->charge()) < 0) )) ) {
						  //
                                                     ATH_MSG_DEBUG("Bc -> D*: Track pair rejected by track3 charge check: " << track3 << " = " << track3->charge() );
				                } else {
                                    
                                                ATH_MSG_DEBUG("Bc -> D*: 3rd track accepted by opposite to kaon charge check: "
                                                << track1 << " = " << track1->charge() << ",  " << track2 << " = " << track2->charge() << ",  " << track3 << " = " << track3->charge() );

                                                std::vector<const xAOD::TrackParticle*> bTracks;
                                                bTracks.clear();
                                                bTracks.push_back(track1);
                                                bTracks.push_back(track2);
                                                bTracks.push_back(track3);
                                          
                                                double PtDstar = PtVal (bTracks);
                                        
                                                if ( PtDstar > m_lowerDstar_PtCut ) {
                                                
                                                bTracks.push_back(*trackELmu1);
                                                bTracks.push_back(*trackELmu2);
                                          
                                                double PtBc = PtVal (bTracks);
                                        
                                                if ( PtBc > m_lowerBcDstar_PtCut ) {
                                                
                                                xAOD::TrigBphys* trigPartDstar(nullptr);
                                                xAOD::TrigBphys* trigPartBcMuMuDstar = checkBcMuMuDstar(trackEL3,trackEL4,trackEL5,
                                                                                                  trackELmu1,trackELmu2,
                                                                                                  trigPartDstar);
                                                nTriedCombinations++;
                                                if (trigPartBcMuMuDstar) {
                                                    if(m_maxBcDstarToStore >= 0 && m_countBcDstarToStore >= m_maxBcDstarToStore) {
                                                      if(m_countBcDstarToStore == m_maxBcDstarToStore) {
                                                        ATH_MSG_WARNING("Reached maximum number of Bc->D* candidates to store " << m_maxBcDstarToStore << "; following candidates won't be written out" );
                                                        m_mon_Errors.push_back(ERROR_MaxNumBcDstarReached);
                                                      } else {
                                                        ATH_MSG_DEBUG("Do not write out " << m_countBcDstarToStore+1 << "th Bc->D* candidate" );
                                                      }
                                                      //TJ: delete trigPartBcMuMuDstar;
                                                      //TJ: delete trigPartDstar;
                                                    }
                                                    else {
                                                      m_TrigBphysColl_b->push_back( trigPartBcMuMuDstar );
                                                      m_TrigBphysColl_X->push_back( trigPartDstar );

                                                      m_bphysHelperTool->setBeamlineDisplacement(trigPartBcMuMuDstar,
                                                                                                {*trkIt1,*trkIt2,*trkIt3,*trackELmu1,*trackELmu2});
                                                      m_bphysHelperTool->setBeamlineDisplacement(trigPartDstar,
                                                                                                {*trkIt1,*trkIt2,*trkIt3});
                                                      trigPartDstar->addParticleLink(ItrackEL3);
                                                      trigPartDstar->addParticleLink(ItrackEL4);
                                                      trigPartDstar->addParticleLink(ItrackEL5);
                                                    
                                                      int iDstar = m_TrigBphysColl_X->size() - 1;
                                                      ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*m_TrigBphysColl_X,iDstar);
                                                      
                                                      trigPartBcMuMuDstar->addParticleLink(ItrackEL3);
                                                      trigPartBcMuMuDstar->addParticleLink(ItrackEL4);
                                                      trigPartBcMuMuDstar->addParticleLink(ItrackEL5);
                                                      if(Found1Track) trigPartBcMuMuDstar->addParticleLink(ELmu1);
                                                      if(Found2Track) trigPartBcMuMuDstar->addParticleLink(ELmu2);
                                                      trigPartBcMuMuDstar->setSecondaryDecayLink(trigPartXEL);
                                                    }
                                                    
                                                    result=true;
                                                    m_mon_BcMuMuDstar_n++;
                                                    m_countBcDstarToStore++;
                                                    if(IdEvent!=m_lastEventPassedBcDstar) {
                                                        m_countPassedEventsBcDstar++;
                                                        m_lastEventPassedBcDstar = IdEvent;
                                                    }

                                                } // if trigPartBcMuMuDstar

						} // Pt D*
						} // Pt BC
						} // check of 3rd track charge
                                            } // for loop over track3
                                        } // in Bc mass range
                                    } // in D0/D* mass range
                                   } // end check that track1 and track2 have large enough pT values
                                   } // end check that track1 and track2 have opposite charges
                                } // if m_doBc_DstarMuMuDecay

                                //
                                // Bc -> mu mu D0 (K pi) part
                                //
                                if(m_doBc_D0MuMuDecay) {

				  // check that track1 and track2 have opposite charges
				  if( (track1->charge())*(track2->charge()) > 0) {      
				     ATH_MSG_DEBUG("Bc -> D0: Track pair rejected by opposite charge check: "
                                     << track1 << " = " << track1->charge() << ",  "   
                                     << track2 << " = " << track2->charge() );
				  } else {

				     ATH_MSG_DEBUG("Bc -> D0: Track pair accepted by opposite charge check: "
                                     << track1 << " = " << track1->charge() << ",  "   
                                     << track2 << " = " << track2->charge() );
                                    
                                    double xD0Mass1 = XMass(track1,track2,bD_to_Kstar); // decay = bD_to_Kstar to actually calculate m(K pi)
                                    double xD0Mass2 = XMass(track2,track1,bD_to_Kstar); // decay = bD_to_Kstar to actually calculate m(K pi)
                                                                        
                                    if ( (xD0Mass1 > m_lowerD0_MassCut && xD0Mass1 < m_upperD0_MassCut) ||
                                         (xD0Mass2 > m_lowerD0_MassCut && xD0Mass2 < m_upperD0_MassCut) ) {
                                        
				      double xD0MuMuMass1 = XMuMuMass (*trackELmu1,*trackELmu2,track1,track2,bD_to_Kstar);
				      double xD0MuMuMass2 = XMuMuMass (*trackELmu1,*trackELmu2,track2,track1,bD_to_Kstar);
                                        
                                        if ( (xD0Mass1 > m_lowerD0_MassCut && xD0Mass1 < m_upperD0_MassCut && xD0MuMuMass1 < m_upperBc_D0MuMuMassCut) ||
                                             (xD0Mass2 > m_lowerD0_MassCut && xD0Mass2 < m_upperD0_MassCut && xD0MuMuMass2 < m_upperBc_D0MuMuMassCut) ) {                                   
                                                std::vector<const xAOD::TrackParticle*> bTracks;
                                                bTracks.clear();
                                                bTracks.push_back(track1);
                                                bTracks.push_back(track2);
                                          
                                                double PtD0 = PtVal (bTracks);
                                        
                                                if ( PtD0 > m_lowerD0_PtCut ) {
                                                
                                                bTracks.push_back(*trackELmu1);
                                                bTracks.push_back(*trackELmu2);
                                          
                                                double PtBc = PtVal (bTracks);
                                        
                                                if ( PtBc > m_lowerBcD0_PtCut ) {
                                                
                                                xAOD::TrigBphys* trigPartD0(nullptr);
                                                xAOD::TrigBphys* trigPartBcMuMuD0 = checkBcMuMuD0(trackEL3,trackEL4,
                                                                                                  trackELmu1,trackELmu2,
                                                                                                  trigPartD0);
                                                nTriedCombinations++;
                                                if (trigPartBcMuMuD0) {
                                                    if(m_maxBcD0ToStore >= 0 && m_countBcD0ToStore >= m_maxBcD0ToStore) {
                                                      if(m_countBcD0ToStore == m_maxBcD0ToStore) {
                                                        ATH_MSG_WARNING("Reached maximum number of Bc->D0 candidates to store " << m_maxBcD0ToStore << "; following candidates won't be written out" );
                                                        m_mon_Errors.push_back(ERROR_MaxNumBcD0Reached);
                                                      }
                                                      else
                                                        ATH_MSG_DEBUG("Do not write out " << m_countBcD0ToStore+1 << "th Bc->D0 candidate" );
                                                      //TJ: delete trigPartBcMuMuD0;
                                                      //TJ: delete trigPartD0;
                                                    }
                                                    else {
                                                      m_TrigBphysColl_b->push_back( trigPartBcMuMuD0 );
                                                      m_TrigBphysColl_X->push_back( trigPartD0 );

                                                      m_bphysHelperTool->setBeamlineDisplacement(trigPartBcMuMuD0,
                                                                                                {*trkIt1,*trkIt2,*trackELmu1,*trackELmu2});
                                                      m_bphysHelperTool->setBeamlineDisplacement(trigPartD0,
                                                                                                {*trkIt1,*trkIt2});
                                                      trigPartD0->addParticleLink(ItrackEL3);
                                                      trigPartD0->addParticleLink(ItrackEL4);
                                                    
                                                      int iD0 = m_TrigBphysColl_X->size() - 1;
                                                      ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*m_TrigBphysColl_X,iD0);
                                                      
                                                      trigPartBcMuMuD0->addParticleLink(ItrackEL3);
                                                      trigPartBcMuMuD0->addParticleLink(ItrackEL4);
                                                      if(Found1Track) trigPartBcMuMuD0->addParticleLink(ELmu1);
                                                      if(Found2Track) trigPartBcMuMuD0->addParticleLink(ELmu2);
                                                      trigPartBcMuMuD0->setSecondaryDecayLink(trigPartXEL);
                                                    }
                                                    
                                                    result=true;
                                                    m_mon_BcMuMuD0_n++;
                                                    m_countBcD0ToStore++;
                                                    if(IdEvent!=m_lastEventPassedBcD0) {
                                                        m_countPassedEventsBcD0++;
                                                        m_lastEventPassedBcD0 = IdEvent;
                                                    }

                                                } // if trigPartBcMuMuD0

						} // Pt D0
						} // Pt Bc
                                        } // in Bc mass range
                                    } // in D0 mass range
                                   } // end check that track1 and track2 have opposite charges
                                } // if m_doBc_D0MuMuDecay


                            } // end if(m_doBd_KstarMuMuDecay || m_doBs_Phi1020MuMuDecay || m_doLb_LambdaMuMuDecay)
                    } // end of inner loop over tracks 2
                    
                } // end of outer loop over tracks 1
            } // end of inner muon loop
        } // end of outer muon loop
    } // end if muidIDtracks1,2 sizes >= 0
    
    ATH_MSG_DEBUG("Totally tried to build " << nTriedCombinations << " tracks cobinations" );
    m_mon_nTriedCombinations = nTriedCombinations;
    
//     std::string  KEY; //sivokl
    
    if(m_TrigBphysColl_b!=0 && m_TrigBphysColl_b->size()>0) {
        ATH_MSG_DEBUG("REGTEST: Store Bphys Collection size: " << m_TrigBphysColl_b->size() );
        HLT::ErrorCode sc = attachFeature(outputTE, m_TrigBphysColl_X, "EFBMuMuXFex_X" );
        if(sc != HLT::OK) {
            ATH_MSG_WARNING( "Failed to store trigBphys_X Collection in outputTE" );
            m_mon_Errors.push_back(ERROR_BphysCollStore_Fails);
            delete m_TrigBphysColl_b;
            delete m_TrigBphysColl_X;
            if ( timerSvc() ) m_TotTimer->stop();
            return HLT::ERROR;
        }
        // Sergey S.: get persistified TrigBphysContainer of X decays
        ElementLinkVector<xAOD::TrigBphysContainer> ELvecTBPh;
        sc = getFeaturesLinks< xAOD::TrigBphysContainer, xAOD::TrigBphysContainer > (outputTE, ELvecTBPh, "EFBMuMuXFex_X");
//         const xAOD::TrigBphysContainer * TrigBphysColl_X_persist(0);
//         sc = getFeature(outputTE, TrigBphysColl_X_persist, "EFBMuMuXFex_X");
        if(sc != HLT::OK) {
        ATH_MSG_WARNING( "Failed to getFeaturesLinks trigBphys_X Collection in outputTE" );
        } else {
            if(msgLvl() <= MSG::DEBUG){
              for ( const auto eltp: ELvecTBPh) {
                msg() << MSG::DEBUG << "  ===== TrigBphys Container ElementLinks : " 
                  << " index: "  << eltp.index()
                  << " sgkey: "  << eltp.dataID()
                  << " hashkey: "<< eltp.key()
                  << " valid: "  << eltp.isValid()
                  << " ptr: "    << (eltp.isValid() ? *eltp : nullptr)
                << endmsg;
//               KEY = eltp.dataID();
              }
            }
        }
        // Sergey S.: reset the secondary decay links to persistified container
        for( xAOD::TrigBphysContainer::iterator BPobj = m_TrigBphysColl_b->begin(); BPobj != m_TrigBphysColl_b->end(); ++BPobj) {
            if ((*BPobj)->secondaryDecayLink().isValid() ) {
              ElementLink<xAOD::TrigBphysContainer> secEL;
//               secEL.resetWithKeyAndIndex(KEY,(*BPobj)->secondaryDecayLink().index());
              // match transient secondary decay ELs with those from persistified container
              for(auto persistentSecEL : ELvecTBPh ) {
                if(*persistentSecEL == *(*BPobj)->secondaryDecayLink())
                  secEL = persistentSecEL;
              }
              if(!secEL.isValid()){
                ATH_MSG_WARNING( "Failed to find persistified secondary decay EL" );
              } else {
                (*BPobj)->setSecondaryDecayLink(secEL);
              }
              ATH_MSG_DEBUG("  ===== TrigBphys secondaryDecay ElementLinks AFTER reset: Slink isValid = " << (*BPobj)->secondaryDecayLink().isValid()
                << ", dataID = " << (*BPobj)->secondaryDecayLink().dataID() << ", index = " << (*BPobj)->secondaryDecayLink().index()
                << ", key = " << (*BPobj)->secondaryDecayLink().key() << ", cptr = " << (*BPobj)->secondaryDecayLink().cptr() );
            }
        }



	        
        sc = attachFeature(outputTE, m_TrigBphysColl_b, "EFBMuMuXFex" );
        if(sc != HLT::OK) {
            ATH_MSG_WARNING( "Failed to store trigBphys Collection in outputTE" );
            m_mon_Errors.push_back(ERROR_BphysCollStore_Fails);
            delete m_TrigBphysColl_b;
            delete m_TrigBphysColl_X;
            if ( timerSvc() ) m_TotTimer->stop();
            return HLT::ERROR;
        }
    } else {
        ATH_MSG_DEBUG("REGTEST: no bphys collection to store "  );
        delete m_TrigBphysColl_b;
        delete m_TrigBphysColl_X;
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
        m_mon_TotalRunTime   = m_TotTimer->elapsed();
        m_mon_VertexingTime  = m_VtxFitTimer->elapsed();
    }
    
    if ( timerSvc() ) {
        ATH_MSG_DEBUG("m_TotTimer->elapsed()    = " << m_TotTimer->elapsed() );
        ATH_MSG_DEBUG("m_VtxFitTimer->elapsed() = " << m_VtxFitTimer->elapsed() );
    }
    
    return HLT::OK;
    
}


xAOD::TrigBphys* TrigEFBMuMuXFex::checkBplusMuMuKplus(const ElementLink<xAOD::TrackParticleContainer> & eltrack1,
                                                      const ElementLink<xAOD::TrackParticleContainer> & elmu1,
                                                      const ElementLink<xAOD::TrackParticleContainer> & elmu2) {
    
    ATH_MSG_DEBUG("Try to build B+ -> mu mu K+ with track " << *eltrack1 );
    float massKMuMu = KMuMuMass(*elmu1,*elmu2,*eltrack1);
    
    static const std::vector<double> masses = {KPLUSMASS,MUMASS,MUMASS};
    
    if( !(massKMuMu > m_lowerKMuMuMassCut && massKMuMu < m_upperKMuMuMassCut) ) {
        ATH_MSG_DEBUG(" B+ -> mu mu K+ candidate rejected by mass cut: m = " << massKMuMu );
        return nullptr;
    }
    ++m_countPassedBplusMass;
    auto track1 = *eltrack1;
    
    // Fill BMuMuK monitoring containers
    m_mon_BMuMuK_Pt_K.push_back(track1->pt()*0.001);
    m_mon_BMuMuK_Eta_K.push_back(track1->eta());
    m_mon_BMuMuK_Phi_K.push_back(track1->phi());
    m_mon_BMuMuK_InvMass_B.push_back(massKMuMu*0.001);
    
    // create initial object
    xAOD::TrigBphys * result = new xAOD::TrigBphys;
    result->makePrivateStore();
    result->initialise(0,0.,0.,0.,
                       xAOD::TrigBphys::BKMUMU, massKMuMu, xAOD::TrigBphys::EF);
    
    std::vector<ElementLink<xAOD::TrackParticleContainer> > vec_tracks = {eltrack1,elmu1, elmu2};
    
    if (m_doB_KMuMuVertexing) {
        
        if ( timerSvc() ) m_VtxFitTimer->resume();
        if (!m_bphysHelperTool->vertexFit(result,vec_tracks,masses).isSuccess()) {
            ATH_MSG_DEBUG(" Vertex fitting failed." );
            if ( timerSvc() ) m_VtxFitTimer->pause();
            delete result;
            return nullptr;
        }
        m_countPassedBplusVtx++;
        
        double chi2val(result->fitchi2());
        
        if (chi2val > m_bVtxChi2Cut || chi2val < 0) {
            ATH_MSG_DEBUG(" B+ -> mu mu K+ candidate rejected by chi2 cut: chi2 = " << chi2val );
            delete result;
            return nullptr;
        }
        m_mon_BMuMuK_Chi2_B.push_back(chi2val);
        ++m_countPassedBplusVtxChi2;
        
        
        ATH_MSG_DEBUG(" Good B+ -> mu mu K+ found!" << std::endl
            << "  m = " << result->mass() << ", "
            << "chi2 = " << result->fitchi2() << ", vertex (" << result->fitx() << ", "
            << result->fity() << ", " << result->fitz() << ")" );
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
    std::vector<double> massHypo  = {MUMASS,MUMASS,KPLUSMASS,KPLUSMASS,PIMASS};
    
    m_countPassedPhiDsMass++;
    
    ATH_MSG_DEBUG("Try to build " << "D_s" << " with tracks " << *eltrack1 << ", " << *eltrack2 << ", " << *eltrack3 );
    
    
    float massX = X3Mass(*eltrack1, *eltrack2, *eltrack3);
    if( !(massX > m_lowerDs_MassCut && massX < m_upperDs_MassCut) ) {
        ATH_MSG_DEBUG(" " << "D_s" << " candidate rejected by the mass cut: m = " << massX );
        return nullptr;
    }
    m_countPassedDsMass++;
    
    float massXMuMu = X3MuMuMass(*elmu1, *elmu2, *eltrack1, *eltrack2, *eltrack3);
    if( !(massXMuMu > m_lowerBc_DsMuMuMassCut && massXMuMu < m_upperBc_DsMuMuMassCut) ) {
        ATH_MSG_DEBUG(" " << "B_c" << " candidate rejected by the mass cut: m = " << massXMuMu );
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
    
    m_mon_BcMuMuDs_Pt_K1.push_back ((*eltrack1)->pt()*0.001);
    m_mon_BcMuMuDs_Eta_K1.push_back((*eltrack1)->eta());
    m_mon_BcMuMuDs_Phi_K1.push_back((*eltrack1)->phi());
    m_mon_BcMuMuDs_Pt_K2.push_back( (*eltrack2)->pt()*0.001);
    m_mon_BcMuMuDs_Eta_K2.push_back((*eltrack2)->eta());
    m_mon_BcMuMuDs_Phi_K2.push_back((*eltrack2)->phi());
    m_mon_BcMuMuDs_Pt_pi.push_back ((*eltrack3)->pt()*0.001);
    m_mon_BcMuMuDs_Eta_pi.push_back((*eltrack3)->eta());
    m_mon_BcMuMuDs_Phi_pi.push_back((*eltrack3)->phi());
    
    
    m_mon_BcMuMuDs_InvMass_PhiDs.push_back(xPhiMass/1000.);

    m_mon_BcMuMuDs_InvMass_Ds.push_back(massX/1000.);
    m_mon_BcMuMuDs_InvMass_Bc.push_back(massXMuMu/1000.);
    
    
    if(m_doDs_Vertexing) {
        if ( timerSvc() ) m_VtxFitTimer->resume();
        if (!m_bphysHelperTool->vertexFit(fitVtx_X,vec_tracksX,massHypoX).isSuccess()) {
            ATH_MSG_DEBUG(" Failed to fit X vertex for " << "D_s" );
            if ( timerSvc() ) m_VtxFitTimer->pause();
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        if ( timerSvc() ) m_VtxFitTimer->pause();
        
        
        float chi2X = fitVtx_X->fitchi2();
        if( chi2X > m_DsVtxChi2Cut || chi2X < 0  ) {
            ATH_MSG_DEBUG(" " << "D_s" << " candidate rejected by X vertex chi2 cut: chi2 = " << chi2X );
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        m_countPassedDsVtxChi2++;
        m_mon_BcMuMuDs_InvMass_Ds.push_back(massX/1000.);
        //                   m_mon_BcMuMuDs_VtxMass_Ds.push_back(vtxMassX/1000.);
        m_mon_BcMuMuDs_Chi2_Ds.push_back(chi2X);
        
    } // m_doDs_Vertexing
    else {
      // if vertexting is not done - just attach trackParticle links
      for(auto tpel : vec_tracksX) {
        fitVtx_X->addTrackParticleLink(tpel);
      }
    }
    
    if(m_doBc_DsMuMuVertexing) {

      if(m_doBc_DsMuMuCascade) {
	// Cascade 2-vertex fit  

	//        Trk::VertexID dplusCascade, bcCascade;
        Trk::VertexID dsCascade;

        std::vector<const xAOD::TrackParticle*> trackpTripletX;
        trackpTripletX.push_back(*eltrack1);
        trackpTripletX.push_back(*eltrack2);
        trackpTripletX.push_back(*eltrack3);

	std::vector<double> massAtVrtX;
	massAtVrtX.push_back(KPLUSMASS);
	massAtVrtX.push_back(KPLUSMASS);    
	massAtVrtX.push_back(PIMASS);    

	m_VKVFitter->setDefault();

        dsCascade = m_VKVFitter->startVertex(trackpTripletX,massAtVrtX);

        std::vector<Trk::VertexID> vrtList;
        vrtList.push_back(dsCascade);

	std::vector<const xAOD::TrackParticle*> trackpXMuMu;
	trackpXMuMu.push_back(*elmu1);
        trackpXMuMu.push_back(*elmu2);

	std::vector<double> massAtVrtXMuMu;
	massAtVrtXMuMu.push_back(MUMASS);
	massAtVrtXMuMu.push_back(MUMASS);    

	//	bcCascade = m_VKVFitter->nextVertex(trackpXMuMu,massAtVrtXMuMu,vrtList);
        m_VKVFitter->nextVertex(trackpXMuMu,massAtVrtXMuMu,vrtList);

	Amg::Vector3D VertexX;
	Amg::Vector3D VertexXMuMu;

        Trk::VxCascadeInfo * result = m_VKVFitter->fitCascade();

        if ( timerSvc() ) m_VtxFitTimer->pause();
                            
        if( result==NULL ) {
          ATH_MSG_DEBUG(" Failed cascade fit for " << "B_c -> Ds" );
            delete fitVtx_X;
            delete fitVtx;
            delete result;
            return nullptr;
        } else {
          result->getSVOwnership(true); // to deleted the vertices of cascade together with VxCascadeInfo
          ATH_MSG_DEBUG(" Managed cascade fit for " << "B_c -> Ds" );
          m_countPassedBcVtx++;

  	  const std::vector<xAOD::Vertex*> cascadeVertices = result->vertices();

          VertexX[0]     = cascadeVertices[0]->x(); VertexX[1]     = cascadeVertices[0]->y(); VertexX[2]     = cascadeVertices[0]->z();
          VertexXMuMu[0] = cascadeVertices[1]->x(); VertexXMuMu[1] = cascadeVertices[1]->y(); VertexXMuMu[2] = cascadeVertices[1]->z();

	  ATH_MSG_DEBUG(" " << "Bc -> Ds Casc., VertexX     = " << VertexX[0] << " " << VertexX[1] << " " << VertexX[2] );
	  ATH_MSG_DEBUG(" " << "Bc -> Ds Casc., VertexXMuMu = " << VertexXMuMu[0] << " " << VertexXMuMu[1] << " " << VertexXMuMu[2] );

	  float chi2XMuMu = result->fitChi2();
          fitVtx->setFitchi2(chi2XMuMu);
          if( !(chi2XMuMu < m_bCVtxChi2Cut) ) {
            ATH_MSG_DEBUG(" " << "B_c -> Ds" << " candidate rejected by XMuMu vertex chi2 cut: chi2 = " << chi2XMuMu );
            delete fitVtx_X;
            delete fitVtx;
            delete result;
            return nullptr;
          } else {
            ATH_MSG_DEBUG(" " << "B_c -> Ds" << " candidate accepted by XMuMu vertex chi2 cut: chi2 = " << chi2XMuMu );
            m_countPassedBcVtxChi2++;

            const std::vector< std::vector<TLorentzVector> > cascadeMomenta = result->getParticleMoms();

            std::vector<TLorentzVector> bcTrkMom = cascadeMomenta[1];
            std::vector<TLorentzVector> dsTrkMom = cascadeMomenta[0];

            std::vector<TLorentzVector>::iterator momItr;

            TLorentzVector dsMom, bcMom;
            for(momItr=dsTrkMom.begin(); momItr!=dsTrkMom.end(); ++momItr) { dsMom += *momItr; }
            for(momItr=bcTrkMom.begin(); momItr!=bcTrkMom.end(); ++momItr) { bcMom += *momItr; }

	    double pxDs = dsMom.Px();
	    double pyDs = dsMom.Py();

            double LxyDs = LxyVal(pxDs,pyDs,VertexXMuMu,VertexX);
                                        
            if ( m_lowerDs_LxyCut > -99. && LxyDs < m_lowerDs_LxyCut ) {
              ATH_MSG_DEBUG(" " << "B_c -> Ds" << " candidate rejected by Lxy cut: Lxy = " << LxyDs );
              delete fitVtx_X;
              delete fitVtx;
              delete result;
              return nullptr;
	    } else {
              m_countPassedBcDsLxy++;
              ATH_MSG_DEBUG(" " << "B_c -> Ds" << " candidate accepted by Lxy cut: Lxy = " << LxyDs );

	      double vtxMassX     = dsMom.M();
	      double vtxMassXMuMu = bcMom.M();

              double PtDs = dsMom.Perp();
              double PtBc = bcMom.Perp();
                                    
	      //                                    trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::DPKPIPI, massX);

              ATH_MSG_DEBUG(" " << "Bc -> Ds" << " : vtxMassXMuMu = " << vtxMassXMuMu );

              m_mon_BcMuMuDs_VtxMass_Ds.push_back(vtxMassX/1000.);
              m_mon_BcMuMuDs_Lxy_Ds.push_back(LxyDs);
              m_mon_BcMuMuDs_pT_Ds.push_back(PtDs/1000.);
              m_mon_BcMuMuDs_pT_Bc.push_back(PtBc/1000.);
              m_mon_BcMuMuDs_VtxMass_Bc.push_back(vtxMassXMuMu/1000.);
              m_mon_BcMuMuDs_Chi2_Bc.push_back(chi2XMuMu);
              
              // manually attach the track links to fitVtx
              for(auto tpel : vec_tracks) {
                fitVtx->addTrackParticleLink(tpel);
              }
              
              // result of cascate fit is not needed anymore
              delete result;
                                    
            } // end XMuMu Lxy cut
          } // end XMuMu chi2 cut
	  //          } // end if(XMuMuVxCandidate)
        }  // result==NULL

                                                                                     
      } else {

        // Try to fit XMuMu -> mu mu X vertex
        if ( timerSvc() ) m_VtxFitTimer->resume();
        if (!m_bphysHelperTool->vertexFit(fitVtx,vec_tracks,massHypo).isSuccess()) {
            ATH_MSG_DEBUG(" Failed to fit XMuMu vertex for " << "B_c" );
            if ( timerSvc() ) m_VtxFitTimer->pause();
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        if ( timerSvc() ) m_VtxFitTimer->pause();
        m_countPassedBcVtx++;
        float chi2XMuMu = fitVtx->fitchi2();
        if( chi2XMuMu > m_bCVtxChi2Cut ||  chi2XMuMu < 0 ) {
            ATH_MSG_DEBUG(" " << "B_c" << " candidate rejected by XMuMu vertex chi2 cut: chi2 = " << chi2XMuMu );
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        m_countPassedBcVtxChi2++;
        
        m_mon_BcMuMuDs_InvMass_Bc.push_back(massXMuMu/1000.);
        //                   m_mon_BcMuMuDs_VtxMass_Bc.push_back(vtxMassXMuMu/1000.);
        m_mon_BcMuMuDs_Chi2_Bc.push_back(chi2XMuMu);
        
      } //m_doBc_DsMuMuCascade  
    } //m_doBc_DsMuMuVertexing
    else {
      // if vertexting is not done - just attach trackParticle links
      for(auto tpel : vec_tracks) {
        fitVtx->addTrackParticleLink(tpel);
      }
    }
    
    ATH_MSG_DEBUG(" Good " << "B_c" << " found!" << std::endl
        << " doDsVtx: " << (m_doDs_Vertexing       ? "Yes" : "No")
        << " doBcVtx: " << (m_doBc_DsMuMuVertexing ? "Yes" : "No")
        << "  m = " << fitVtx->mass() << ", "
        << "chi2 = " << fitVtx->fitchi2() << ", vertex (" << fitVtx->fitx() << ", "
        << fitVtx->fity() << ", " << fitVtx->fitz() << ")" );
    
    trigPartX = fitVtx_X;
    return fitVtx;
} //checkBcMuMuDs

xAOD::TrigBphys* TrigEFBMuMuXFex::checkBcMuMuDplus(const ElementLink<xAOD::TrackParticleContainer> & eltrack1,
                                                   const ElementLink<xAOD::TrackParticleContainer> & eltrack2,
                                                   const ElementLink<xAOD::TrackParticleContainer> & eltrack3,
                                                   const ElementLink<xAOD::TrackParticleContainer> & elmu1,
                                                   const ElementLink<xAOD::TrackParticleContainer> & elmu2,
                                                   xAOD::TrigBphys* & trigPartX) {
    
    trigPartX = nullptr;
    xAOD::TrigBphys * fitVtx_X(nullptr);
    xAOD::TrigBphys * fitVtx(nullptr);
    
    std::vector<double> massHypoX = {PIMASS,PIMASS,KPLUSMASS};
    std::vector<double> massHypo  = {MUMASS,MUMASS,PIMASS,PIMASS,KPLUSMASS};
    
    ATH_MSG_DEBUG("Try to build " << "D+" << " with tracks " << *eltrack1 << ", " << *eltrack2 << ", " << *eltrack3 );    
    
    float massX = XKPiPiMass(*eltrack3, *eltrack1, *eltrack2);
    if( !(massX > m_lowerDplus_MassCut && massX < m_upperDplus_MassCut) ) {
        ATH_MSG_DEBUG(" " << "D+" << " candidate rejected by the mass cut: m = " << massX );
        return nullptr;
    }
    m_countPassedDplusMass++;
    
    float massXMuMu = XKPiPiMuMuMass(*elmu1, *elmu2, *eltrack3, *eltrack1, *eltrack2);
    if( !(massXMuMu > m_lowerBc_DplusMuMuMassCut && massXMuMu < m_upperBc_DplusMuMuMassCut) ) {
        ATH_MSG_DEBUG(" " << "B_c -> D+" << " candidate rejected by the mass cut: m = " << massXMuMu );
        return nullptr;
    }
    m_countPassedBcDplusMass++;
    
    // create initial object
    fitVtx_X = new xAOD::TrigBphys;
    fitVtx_X->makePrivateStore();
    fitVtx_X->initialise(0,0.,0.,0.,
                         xAOD::TrigBphys::DPKPIPI, massX, xAOD::TrigBphys::EF);
    fitVtx = new xAOD::TrigBphys;
    fitVtx->makePrivateStore();
    fitVtx->initialise(0,0.,0.,0.,
                       xAOD::TrigBphys::BCDPMUMU, massXMuMu, xAOD::TrigBphys::EF);
    
    std::vector<ElementLink<xAOD::TrackParticleContainer> > vec_tracksX = {eltrack1,eltrack2,eltrack3};
    std::vector<ElementLink<xAOD::TrackParticleContainer> > vec_tracks  = {elmu1,elmu2,eltrack1,eltrack2,eltrack3};
    
    m_mon_BcMuMuDplus_Pt_pi1.push_back ((*eltrack1)->pt()*0.001);
    m_mon_BcMuMuDplus_Eta_pi1.push_back((*eltrack1)->eta());
    m_mon_BcMuMuDplus_Phi_pi1.push_back((*eltrack1)->phi());
    m_mon_BcMuMuDplus_Pt_pi2.push_back( (*eltrack2)->pt()*0.001);
    m_mon_BcMuMuDplus_Eta_pi2.push_back((*eltrack2)->eta());
    m_mon_BcMuMuDplus_Phi_pi2.push_back((*eltrack2)->phi());
    m_mon_BcMuMuDplus_Pt_K.push_back ((*eltrack3)->pt()*0.001);
    m_mon_BcMuMuDplus_Eta_K.push_back((*eltrack3)->eta());
    m_mon_BcMuMuDplus_Phi_K.push_back((*eltrack3)->phi());    

    m_mon_BcMuMuDplus_InvMass_Dplus.push_back(massX/1000.);
    m_mon_BcMuMuDplus_InvMass_Bc.push_back(massXMuMu/1000.);
    
    if(m_doDplus_Vertexing) {
        if ( timerSvc() ) m_VtxFitTimer->resume();
        if (!m_bphysHelperTool->vertexFit(fitVtx_X,vec_tracksX,massHypoX).isSuccess()) {
            ATH_MSG_DEBUG(" Failed to fit X vertex for " << "D+" );
            if ( timerSvc() ) m_VtxFitTimer->pause();
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        if ( timerSvc() ) m_VtxFitTimer->pause();
        
        
        float chi2X = fitVtx_X->fitchi2();
        if( chi2X > m_DplusVtxChi2Cut || chi2X < 0  ) {
            ATH_MSG_DEBUG(" " << "D+" << " candidate rejected by X vertex chi2 cut: chi2 = " << chi2X );
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        m_countPassedDplusVtxChi2++;
	//        m_mon_BcMuMuDplus_InvMass_Dplus.push_back(massX/1000.);
        //                   m_mon_BcMuMuDs_VtxMass_Ds.push_back(vtxMassX/1000.);
        m_mon_BcMuMuDplus_Chi2_Dplus.push_back(chi2X);
        
    } // m_doDplus_Vertexing
    else {
      // if vertexting is not done - just attach trackParticle links
      for(auto tpel : vec_tracksX) {
        fitVtx_X->addTrackParticleLink(tpel);
      }
    }
    
    if(m_doBc_DplusMuMuVertexing) {

      if(m_doBc_DplusMuMuCascade) {
	// Cascade 2-vertex fit  

	//        Trk::VertexID dplusCascade, bcCascade;
        Trk::VertexID dplusCascade;

        std::vector<const xAOD::TrackParticle*> trackpTripletX;
        trackpTripletX.push_back(*eltrack3);
        trackpTripletX.push_back(*eltrack1);
        trackpTripletX.push_back(*eltrack2);

	std::vector<double> massAtVrtX;
	massAtVrtX.push_back(KPLUSMASS);
	massAtVrtX.push_back(PIMASS);    
	massAtVrtX.push_back(PIMASS);    

	m_VKVFitter->setDefault();

        dplusCascade = m_VKVFitter->startVertex(trackpTripletX,massAtVrtX);

        std::vector<Trk::VertexID> vrtList;
        vrtList.push_back(dplusCascade);

	std::vector<const xAOD::TrackParticle*> trackpXMuMu;
	trackpXMuMu.push_back(*elmu1);
        trackpXMuMu.push_back(*elmu2);

	std::vector<double> massAtVrtXMuMu;
	massAtVrtXMuMu.push_back(MUMASS);
	massAtVrtXMuMu.push_back(MUMASS);    

	//	bcCascade = m_VKVFitter->nextVertex(trackpXMuMu,massAtVrtXMuMu,vrtList);
        m_VKVFitter->nextVertex(trackpXMuMu,massAtVrtXMuMu,vrtList);

	Amg::Vector3D VertexX;
	Amg::Vector3D VertexXMuMu;

        Trk::VxCascadeInfo * result = m_VKVFitter->fitCascade();

        if ( timerSvc() ) m_VtxFitTimer->pause();
                            
        if( result==NULL ) {
          ATH_MSG_DEBUG(" Failed cascade fit for " << "B_c -> Dplus" );
            delete fitVtx_X;
            delete fitVtx;
            delete result;
            return nullptr;
        } else {
          result->getSVOwnership(true); // to deleted the vertices of cascade together with VxCascadeInfo
          ATH_MSG_DEBUG(" Managed cascade fit for " << "B_c -> Dplus" );
          m_countPassedBcDplusVtx++;

  	  const std::vector<xAOD::Vertex*> cascadeVertices = result->vertices();

          VertexX[0]     = cascadeVertices[0]->x(); VertexX[1]     = cascadeVertices[0]->y(); VertexX[2]     = cascadeVertices[0]->z();
          VertexXMuMu[0] = cascadeVertices[1]->x(); VertexXMuMu[1] = cascadeVertices[1]->y(); VertexXMuMu[2] = cascadeVertices[1]->z();

	  ATH_MSG_DEBUG(" " << "Bc -> Dplus Casc., VertexX     = " << VertexX[0] << " " << VertexX[1] << " " << VertexX[2] );
	  ATH_MSG_DEBUG(" " << "Bc -> Dplus Casc., VertexXMuMu = " << VertexXMuMu[0] << " " << VertexXMuMu[1] << " " << VertexXMuMu[2] );

	  float chi2XMuMu = result->fitChi2();
          fitVtx->setFitchi2(chi2XMuMu);
          if( !(chi2XMuMu < m_bCDplusVtxChi2Cut) ) {
            ATH_MSG_DEBUG(" " << "B_c -> Dplus" << " candidate rejected by XMuMu vertex chi2 cut: chi2 = " << chi2XMuMu );
            delete fitVtx_X;
            delete fitVtx;
            delete result;
            return nullptr;
          } else {
            ATH_MSG_DEBUG(" " << "B_c -> Dplus" << " candidate accepted by XMuMu vertex chi2 cut: chi2 = " << chi2XMuMu );
            m_countPassedBcDplusVtxChi2++;

            const std::vector< std::vector<TLorentzVector> > cascadeMomenta = result->getParticleMoms();

            std::vector<TLorentzVector> bcTrkMom = cascadeMomenta[1];
            std::vector<TLorentzVector> dcTrkMom = cascadeMomenta[0];

            std::vector<TLorentzVector>::iterator momItr;

            TLorentzVector dcMom, bcMom;
            for(momItr=dcTrkMom.begin(); momItr!=dcTrkMom.end(); ++momItr) { dcMom += *momItr; }
            for(momItr=bcTrkMom.begin(); momItr!=bcTrkMom.end(); ++momItr) { bcMom += *momItr; }


	    double pxDplus = dcMom.Px();
	    double pyDplus = dcMom.Py();

            double LxyDplus = LxyVal(pxDplus,pyDplus,VertexXMuMu,VertexX);
                                        
            if ( m_lowerDplus_LxyCut > -99. && LxyDplus < m_lowerDplus_LxyCut ) {
              ATH_MSG_DEBUG(" " << "B_c -> Dplus" << " candidate rejected by Lxy cut: Lxy = " << LxyDplus );
              delete fitVtx_X;
              delete fitVtx;
              delete result;
              return nullptr;
	    } else {
              m_countPassedBcDplusLxy++;
              ATH_MSG_DEBUG(" " << "B_c -> Dplus" << " candidate accepted by Lxy cut: Lxy = " << LxyDplus );

	      double vtxMassX     = dcMom.M();
	      double vtxMassXMuMu = bcMom.M();

              double PtDplus = dcMom.Perp();
              double PtBc    = bcMom.Perp();
                                    
	      //                                    trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::DPKPIPI, massX);

              ATH_MSG_DEBUG(" " << "Bc -> Dplus" << " : vtxMassXMuMu = " << vtxMassXMuMu );

	      //              m_mon_BcMuMuDplus_InvMass_Dplus.push_back(massX/1000.);
              m_mon_BcMuMuDplus_VtxMass_Dplus.push_back(vtxMassX/1000.);
	      //              m_mon_BcMuMuDplus_Chi2_Dplus.push_back(chi2X);
              m_mon_BcMuMuDplus_Lxy_Dplus.push_back(LxyDplus);
              m_mon_BcMuMuDplus_pT_Dplus.push_back(PtDplus/1000.);
              m_mon_BcMuMuDplus_pT_Bc.push_back(PtBc/1000.);
	      //              m_mon_BcMuMuDplus_InvMass_Bc.push_back(massXMuMu/1000.);
              m_mon_BcMuMuDplus_VtxMass_Bc.push_back(vtxMassXMuMu/1000.);
              m_mon_BcMuMuDplus_Chi2_Bc.push_back(chi2XMuMu);
              
              // manually attach the track links to fitVtx
              for(auto tpel : vec_tracks) {
                fitVtx->addTrackParticleLink(tpel);
              }
              
              // result of cascate fit is not needed anymore
              delete result;
                                    
            } // end XMuMu Lxy cut
          } // end XMuMu chi2 cut
	  //          } // end if(XMuMuVxCandidate)
        }  // result==NULL

                                                                                     
      } else {

        // Try to fit XMuMu -> mu mu X vertex
        if ( timerSvc() ) m_VtxFitTimer->resume();
        if (!m_bphysHelperTool->vertexFit(fitVtx,vec_tracks,massHypo).isSuccess()) {
            ATH_MSG_DEBUG(" Failed to fit XMuMu vertex for " << "B_c -> D+" );
            if ( timerSvc() ) m_VtxFitTimer->pause();
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        if ( timerSvc() ) m_VtxFitTimer->pause();
        m_countPassedBcDplusVtx++;
        float chi2XMuMu = fitVtx->fitchi2();
        if( chi2XMuMu > m_bCDplusVtxChi2Cut ||  chi2XMuMu < 0 ) {
            ATH_MSG_DEBUG(" " << "B_c -> D+" << " candidate rejected by XMuMu vertex chi2 cut: chi2 = " << chi2XMuMu );
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        m_countPassedBcDplusVtxChi2++;
        
	//        m_mon_BcMuMuDplus_InvMass_Bc.push_back(massXMuMu/1000.);
        //                   m_mon_BcMuMuDs_VtxMass_Bc.push_back(vtxMassXMuMu/1000.);
        m_mon_BcMuMuDplus_Chi2_Bc.push_back(chi2XMuMu);
      
      } //m_doBc_DplusMuMuCascade  
    } //m_doBc_DplusMuMuVertexing
    else {
      // if vertexting is not done - just attach trackParticle links
      for(auto tpel : vec_tracks) {
        fitVtx->addTrackParticleLink(tpel);
      }
    }
    
    ATH_MSG_DEBUG(" Good B_c->D+ found! doDplusVtx: " << (m_doDplus_Vertexing       ? "Yes" : "No")
        << " doBcDplusVtx: " << (m_doBc_DplusMuMuVertexing ? "Yes" : "No")
        << "  m = " << fitVtx->mass() << ", "
        << "chi2 = " << fitVtx->fitchi2() << ", vertex (" << fitVtx->fitx() << ", "
        << fitVtx->fity() << ", " << fitVtx->fitz() << ")" );

    trigPartX = fitVtx_X;
    return fitVtx;
} //checkBcMuMuDplus

xAOD::TrigBphys* TrigEFBMuMuXFex::checkBcMuMuDstar(const ElementLink<xAOD::TrackParticleContainer> & eltrack1,
                                                   const ElementLink<xAOD::TrackParticleContainer> & eltrack2,
                                                   const ElementLink<xAOD::TrackParticleContainer> & eltrack3,
                                                   const ElementLink<xAOD::TrackParticleContainer> & elmu1,
                                                   const ElementLink<xAOD::TrackParticleContainer> & elmu2,
                                                   xAOD::TrigBphys* & trigPartX) {
    
    trigPartX = nullptr;
    xAOD::TrigBphys * fitVtx_X(nullptr);
    xAOD::TrigBphys * fitVtx(nullptr);
    
    std::vector<double> massHypoD0 = {KPLUSMASS,PIMASS};
    std::vector<double> massHypoX = {KPLUSMASS,PIMASS,PIMASS};
    std::vector<double> massHypo  = {MUMASS,MUMASS,KPLUSMASS,PIMASS,PIMASS};
    float massX;
    float massXMuMu;
    double xD0Mass;
    
    ATH_MSG_DEBUG("Try to build " << "Dstar" << " with tracks " << *eltrack1 << ", " << *eltrack2 << ", " << *eltrack3 );

    if ( ((*eltrack1)->charge())*((*eltrack3)->charge()) < 0 ) {
      massX = XKPiPiMass(*eltrack1, *eltrack2, *eltrack3);
      massXMuMu = XKPiPiMuMuMass(*elmu1, *elmu2, *eltrack1, *eltrack2, *eltrack3);
      xD0Mass = XMass(*eltrack1,*eltrack2,bD_to_Kstar); // decay = bD_to_Kstar to actually calculate m(K pi)
    } else {
      massX = XKPiPiMass(*eltrack2, *eltrack1, *eltrack3);
      massXMuMu = XKPiPiMuMuMass(*elmu1, *elmu2, *eltrack2, *eltrack1, *eltrack3);
      xD0Mass = XMass(*eltrack2,*eltrack1,bD_to_Kstar); // decay = bD_to_Kstar to actually calculate m(K pi)
    }

    if ( !(xD0Mass > m_lowerD0Dstar_MassCut && xD0Mass < m_upperD0Dstar_MassCut && (massX - xD0Mass) < m_upperDstar_DMCut) ) {
	  //
	  ATH_MSG_DEBUG(" " << "D*" << " candidate rejected by the mass cut: md0, dM = " << xD0Mass << " , " << massX );
        return nullptr;
    }
    m_countPassedDstarMass++;
    
    if ( !(massXMuMu > m_lowerBc_DstarMuMuMassCut && massXMuMu < m_upperBc_DstarMuMuMassCut) ) {
      //                                                                                                                       
         ATH_MSG_DEBUG(" " << "B_c -> D*" << " candidate rejected by the mass cut: m = " << massXMuMu );
         return nullptr;
    }
    m_countPassedBcDstarMass++;
  
    // create initial object
    fitVtx_X = new xAOD::TrigBphys;
    fitVtx_X->makePrivateStore();
    fitVtx_X->initialise(0,0.,0.,0.,
                         xAOD::TrigBphys::DSTDZPI, massX, xAOD::TrigBphys::EF);
    fitVtx = new xAOD::TrigBphys;
    fitVtx->makePrivateStore();
    fitVtx->initialise(0,0.,0.,0.,
                       xAOD::TrigBphys::BCDSTMUMU, massXMuMu, xAOD::TrigBphys::EF);

    std::vector<ElementLink<xAOD::TrackParticleContainer> > vec_tracksD0;
    std::vector<ElementLink<xAOD::TrackParticleContainer> > vec_tracksX;
    std::vector<ElementLink<xAOD::TrackParticleContainer> > vec_tracks;
    
    if ( ((*eltrack1)->charge())*((*eltrack3)->charge()) < 0 ) {
      vec_tracksD0 = {eltrack1,eltrack2};
      vec_tracksX = {eltrack1,eltrack2,eltrack3};
      vec_tracks  = {elmu1,elmu2,eltrack1,eltrack2,eltrack3};
    
      m_mon_BcMuMuDstar_Pt_K.push_back ((*eltrack1)->pt()*0.001);
      m_mon_BcMuMuDstar_Eta_K.push_back((*eltrack1)->eta());
      m_mon_BcMuMuDstar_Phi_K.push_back((*eltrack1)->phi());
      m_mon_BcMuMuDstar_Pt_pi.push_back( (*eltrack2)->pt()*0.001);
      m_mon_BcMuMuDstar_Eta_pi.push_back((*eltrack2)->eta());
      m_mon_BcMuMuDstar_Phi_pi.push_back((*eltrack2)->phi());
    } else {
      vec_tracksD0 = {eltrack2,eltrack1};
      vec_tracksX = {eltrack2,eltrack1,eltrack3};
      vec_tracks  = {elmu1,elmu2,eltrack2,eltrack1,eltrack3};
    
      m_mon_BcMuMuDstar_Pt_K.push_back ((*eltrack2)->pt()*0.001);
      m_mon_BcMuMuDstar_Eta_K.push_back((*eltrack2)->eta());
      m_mon_BcMuMuDstar_Phi_K.push_back((*eltrack2)->phi());
      m_mon_BcMuMuDstar_Pt_pi.push_back( (*eltrack1)->pt()*0.001);
      m_mon_BcMuMuDstar_Eta_pi.push_back((*eltrack1)->eta());
      m_mon_BcMuMuDstar_Phi_pi.push_back((*eltrack1)->phi());
    }

    m_mon_BcMuMuDstar_Pt_pis.push_back ((*eltrack3)->pt()*0.001);
    m_mon_BcMuMuDstar_Eta_pis.push_back((*eltrack3)->eta());
    m_mon_BcMuMuDstar_Phi_pis.push_back((*eltrack3)->phi());    

    m_mon_BcMuMuDstar_InvMass_D0Dstar.push_back(xD0Mass/1000.);
    m_mon_BcMuMuDstar_DelMass_Dstar.push_back((massX-xD0Mass)/1000.);
    m_mon_BcMuMuDstar_InvMass_Bc.push_back(massXMuMu/1000.);
    
    if(m_doD0Dstar_Vertexing) {
        if ( timerSvc() ) m_VtxFitTimer->resume();
        if (!m_bphysHelperTool->vertexFit(fitVtx_X,vec_tracksD0,massHypoD0).isSuccess()) {
            ATH_MSG_DEBUG(" Failed to fit D0 vertex for " << "D*" );
            if ( timerSvc() ) m_VtxFitTimer->pause();
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        if ( timerSvc() ) m_VtxFitTimer->pause();
        
        
        float chi2X = fitVtx_X->fitchi2();
        if( chi2X > m_D0DstarVtxChi2Cut || chi2X < 0  ) {
            ATH_MSG_DEBUG(" " << "D*" << " candidate rejected by D0 vertex chi2 cut: chi2 = " << chi2X );
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        m_countPassedD0DstarVtxChi2++;
	//        m_mon_BcMuMuDplus_InvMass_Dplus.push_back(massX/1000.);
        //                   m_mon_BcMuMuDs_VtxMass_Ds.push_back(vtxMassX/1000.);
        m_mon_BcMuMuDstar_Chi2_D0Dstar.push_back(chi2X);
        
    } // m_doDplus_Vertexing
    else {
      // if vertexting is not done - just attach trackParticle links
      for(auto tpel : vec_tracksD0) {
        fitVtx_X->addTrackParticleLink(tpel);
      }
    }
    
    if(m_doBc_DstarMuMuVertexing) {

      if(m_doBc_DstarMuMuCascade) {
	// Cascade 2-vertex fit  

	//        Trk::VertexID dplusCascade, bcCascade;
        Trk::VertexID d0Cascade;

        std::vector<const xAOD::TrackParticle*> trackpDupletX;

        if ( ((*eltrack1)->charge())*((*eltrack3)->charge()) < 0 ) {
          trackpDupletX.push_back(*eltrack1);
          trackpDupletX.push_back(*eltrack2);
	} else {
          trackpDupletX.push_back(*eltrack2);
          trackpDupletX.push_back(*eltrack1);
	}

	std::vector<double> massAtVrtX;
	massAtVrtX.push_back(KPLUSMASS);
	massAtVrtX.push_back(PIMASS);    

	m_VKVFitter->setDefault();

        d0Cascade = m_VKVFitter->startVertex(trackpDupletX,massAtVrtX);

        std::vector<Trk::VertexID> vrtList;
        vrtList.push_back(d0Cascade);

	std::vector<const xAOD::TrackParticle*> trackpXMuMu;
        trackpXMuMu.push_back(*eltrack3);
	trackpXMuMu.push_back(*elmu1);
        trackpXMuMu.push_back(*elmu2);

	std::vector<double> massAtVrtXMuMu;
	massAtVrtXMuMu.push_back(PIMASS);
	massAtVrtXMuMu.push_back(MUMASS);
	massAtVrtXMuMu.push_back(MUMASS);    

	//	bcCascade = m_VKVFitter->nextVertex(trackpXMuMu,massAtVrtXMuMu,vrtList);
        m_VKVFitter->nextVertex(trackpXMuMu,massAtVrtXMuMu,vrtList);

	Amg::Vector3D VertexX;
	Amg::Vector3D VertexXMuMu;

        Trk::VxCascadeInfo * result = m_VKVFitter->fitCascade();

        if ( timerSvc() ) m_VtxFitTimer->pause();
                            
        if( result==NULL ) {
          ATH_MSG_DEBUG(" Failed cascade fit for " << "B_c -> Dstar" );
            delete fitVtx_X;
            delete fitVtx;
            delete result;
            return nullptr;
        } else {
          result->getSVOwnership(true); // to deleted the vertices of cascade together with VxCascadeInfo
          ATH_MSG_DEBUG(" Managed cascade fit for " << "B_c -> Dstar" );
          m_countPassedBcDstarVtx++;

  	  const std::vector<xAOD::Vertex*> cascadeVertices = result->vertices();

          VertexX[0]     = cascadeVertices[0]->x(); VertexX[1]     = cascadeVertices[0]->y(); VertexX[2]     = cascadeVertices[0]->z();
          VertexXMuMu[0] = cascadeVertices[1]->x(); VertexXMuMu[1] = cascadeVertices[1]->y(); VertexXMuMu[2] = cascadeVertices[1]->z();

	  ATH_MSG_DEBUG(" " << "Bc -> Dstar Casc., VertexX     = " << VertexX[0] << " " << VertexX[1] << " " << VertexX[2] );
	  ATH_MSG_DEBUG(" " << "Bc -> Dstar Casc., VertexXMuMu = " << VertexXMuMu[0] << " " << VertexXMuMu[1] << " " << VertexXMuMu[2] );

	  float chi2XMuMu = result->fitChi2();
          fitVtx->setFitchi2(chi2XMuMu);
          if( !(chi2XMuMu < m_bCDstarVtxChi2Cut) ) {
            ATH_MSG_DEBUG(" " << "B_c -> Dstar" << " candidate rejected by XMuMu vertex chi2 cut: chi2 = " << chi2XMuMu );
            delete fitVtx_X;
            delete fitVtx;
            delete result;
            return nullptr;
          } else {
            ATH_MSG_DEBUG(" " << "B_c -> Dstar" << " candidate accepted by XMuMu vertex chi2 cut: chi2 = " << chi2XMuMu );
            m_countPassedBcDstarVtxChi2++;

            const std::vector< std::vector<TLorentzVector> > cascadeMomenta = result->getParticleMoms();

            std::vector<TLorentzVector> bcTrkMom = cascadeMomenta[1];
            std::vector<TLorentzVector> d0TrkMom = cascadeMomenta[0];

            std::vector<TLorentzVector>::iterator momItr;

            TLorentzVector d0Mom, bcMom;
            for(momItr=d0TrkMom.begin(); momItr!=d0TrkMom.end(); ++momItr) { d0Mom += *momItr; }
            for(momItr=bcTrkMom.begin(); momItr!=bcTrkMom.end(); ++momItr) { bcMom += *momItr; }


	    double pxD0 = d0Mom.Px();
	    double pyD0 = d0Mom.Py();

            double LxyD0 = LxyVal(pxD0,pyD0,VertexXMuMu,VertexX);

            if ( m_lowerD0Dstar_LxyCut > -99. && LxyD0 < m_lowerD0Dstar_LxyCut ) {
              ATH_MSG_DEBUG(" " << "B_c -> Dstar" << " candidate rejected by Lxy cut: Lxy = " << LxyD0 );
              delete fitVtx_X;
              delete fitVtx;
              delete result;
              return nullptr;
	    } else {
              m_countPassedBcD0DstarLxy++;
              ATH_MSG_DEBUG(" " << "B_c -> Dstar" << " candidate accepted by Lxy cut: Lxy = " << LxyD0 );

	      double vtxMassD0  = d0Mom.M();

 	      double pxDstar = pxD0 + bcTrkMom[0].Px();
	      double pyDstar = pyD0 + bcTrkMom[0].Py();
	      double pzDstar = d0Mom.Pz() + bcTrkMom[0].Pz();
	      double eDstar  = d0Mom.E()  + bcTrkMom[0].E();

	      double PtDstar = sqrt(pxDstar*pxDstar+pyDstar*pyDstar);

              double vtxDstarMass = sqrt(eDstar*eDstar-pxDstar*pxDstar-pyDstar*pyDstar-pzDstar*pzDstar);                          

	      double vtxMassXMuMu = bcMom.M();
              double PtBc         = bcMom.Perp();
                                    
              ATH_MSG_DEBUG(" " << "Bc -> Dstar" << " : vtxMassXMuMu = " << vtxMassXMuMu );

	      //              m_mon_BcMuMuDplus_InvMass_Dplus.push_back(massX/1000.);
              m_mon_BcMuMuDstar_VtxMass_D0Dstar.push_back(vtxMassD0/1000.);
              m_mon_BcMuMuDstar_VtxDelMass_Dstar.push_back((vtxDstarMass-vtxMassD0)/1000.);
	      //              m_mon_BcMuMuDplus_Chi2_Dplus.push_back(chi2X);
              m_mon_BcMuMuDstar_Lxy_D0Dstar.push_back(LxyD0);
              m_mon_BcMuMuDstar_pT_Dstar.push_back(PtDstar/1000.);
              m_mon_BcMuMuDstar_pT_Bc.push_back(PtBc/1000.);
	      //              m_mon_BcMuMuDplus_InvMass_Bc.push_back(massXMuMu/1000.);
              m_mon_BcMuMuDstar_VtxMass_Bc.push_back(vtxMassXMuMu/1000.);
              m_mon_BcMuMuDstar_Chi2_Bc.push_back(chi2XMuMu);
              
              // manually attach the track links to fitVtx
              for(auto tpel : vec_tracks) {
                fitVtx->addTrackParticleLink(tpel);
              }
              
              // result of cascate fit is not needed anymore
              delete result;
                                    
            } // end XMuMu Lxy cut
          } // end XMuMu chi2 cut
	  //          } // end if(XMuMuVxCandidate)
        }  // result==NULL

                                                                                     
      } else {

        // Try to fit XMuMu -> mu mu X vertex
        if ( timerSvc() ) m_VtxFitTimer->resume();
        if (!m_bphysHelperTool->vertexFit(fitVtx,vec_tracks,massHypo).isSuccess()) {
            ATH_MSG_DEBUG(" Failed to fit XMuMu vertex for " << "B_c -> Dstar" );
            if ( timerSvc() ) m_VtxFitTimer->pause();
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        if ( timerSvc() ) m_VtxFitTimer->pause();
        m_countPassedBcDstarVtx++;
        float chi2XMuMu = fitVtx->fitchi2();
        if( chi2XMuMu > m_bCDstarVtxChi2Cut ||  chi2XMuMu < 0 ) {
            ATH_MSG_DEBUG(" " << "B_c -> D*" << " candidate rejected by XMuMu vertex chi2 cut: chi2 = " << chi2XMuMu );
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        m_countPassedBcDstarVtxChi2++;
        
        m_mon_BcMuMuDstar_Chi2_Bc.push_back(chi2XMuMu);
      
      } //m_doBc_DstarMuMuCascade  
    } //m_doBc_DstarMuMuVertexing
    else {
      // if vertexting is not done - just attach trackParticle links
      for(auto tpel : vec_tracks) {
        fitVtx->addTrackParticleLink(tpel);
      }
    }
    
    ATH_MSG_DEBUG(" Good B_c->D* found! doD0DstarVtx: " << (m_doD0Dstar_Vertexing       ? "Yes" : "No")
        << " doBcDstarVtx: " << (m_doBc_DstarMuMuVertexing ? "Yes" : "No")
        << "  m = " << fitVtx->mass() << ", "
        << "chi2 = " << fitVtx->fitchi2() << ", vertex (" << fitVtx->fitx() << ", "
        << fitVtx->fity() << ", " << fitVtx->fitz() << ")" );
    
    trigPartX = fitVtx_X;
    return fitVtx;
} //checkBcMuMuDstar

xAOD::TrigBphys* TrigEFBMuMuXFex::checkBcMuMuD0(const ElementLink<xAOD::TrackParticleContainer> & eltrack1,
                                                const ElementLink<xAOD::TrackParticleContainer> & eltrack2,
                                                const ElementLink<xAOD::TrackParticleContainer> & elmu1,
                                                const ElementLink<xAOD::TrackParticleContainer> & elmu2,
                                                xAOD::TrigBphys* & trigPartX) {
    
    trigPartX = nullptr;
    xAOD::TrigBphys * fitVtx_X(nullptr);
    xAOD::TrigBphys * fitVtx(nullptr);
    
    std::vector<double> massHypoX = {KPLUSMASS,PIMASS};
    std::vector<double> massHypo  = {MUMASS,MUMASS,KPLUSMASS,PIMASS};
    float massX;
    float massXMuMu;
    
    ATH_MSG_DEBUG("Try to build " << "D0" << " with tracks " << *eltrack1 << ", " << *eltrack2 );

    double xD0Mass1 = XMass(*eltrack1,*eltrack2,bD_to_Kstar); // decay = bD_to_Kstar to actually calculate m(K pi)
    double xD0Mass2 = XMass(*eltrack2,*eltrack1,bD_to_Kstar); // decay = bD_to_Kstar to actually calculate m(K pi)

    if ( xD0Mass1 > m_lowerD0_MassCut && xD0Mass1 < m_upperD0_MassCut ) {
      massX = xD0Mass1;
      massXMuMu = XMuMuMass (*elmu1,*elmu2,*eltrack1,*eltrack2,bD_to_Kstar);
    } else {
      massX = xD0Mass2;
      massXMuMu = XMuMuMass (*elmu1,*elmu2,*eltrack2,*eltrack1,bD_to_Kstar);
    }

    m_countPassedD0Mass++;
    
    if ( !(massXMuMu > m_lowerBc_D0MuMuMassCut && massXMuMu < m_upperBc_D0MuMuMassCut) ) {
      //                                                                                                                       
         ATH_MSG_DEBUG(" " << "B_c -> D0" << " candidate rejected by the mass cut: m = " << massXMuMu );
         return nullptr;
    }
    m_countPassedBcD0Mass++;
  
    // create initial object
    fitVtx_X = new xAOD::TrigBphys;
    fitVtx_X->makePrivateStore();
    fitVtx_X->initialise(0,0.,0.,0.,
                         xAOD::TrigBphys::DZKPI, massX, xAOD::TrigBphys::EF);
    fitVtx = new xAOD::TrigBphys;
    fitVtx->makePrivateStore();
    fitVtx->initialise(0,0.,0.,0.,
                       xAOD::TrigBphys::BCDSTMUMU, massXMuMu, xAOD::TrigBphys::EF);

    std::vector<ElementLink<xAOD::TrackParticleContainer> > vec_tracksX;
    std::vector<ElementLink<xAOD::TrackParticleContainer> > vec_tracks;
    
    if ( xD0Mass1 > m_lowerD0_MassCut && xD0Mass1 < m_upperD0_MassCut ) {
      vec_tracksX = {eltrack1,eltrack2};
      vec_tracks  = {elmu1,elmu2,eltrack1,eltrack2};
    
      m_mon_BcMuMuD0_Pt_K.push_back ((*eltrack1)->pt()*0.001);
      m_mon_BcMuMuD0_Eta_K.push_back((*eltrack1)->eta());
      m_mon_BcMuMuD0_Phi_K.push_back((*eltrack1)->phi());
      m_mon_BcMuMuD0_Pt_pi.push_back( (*eltrack2)->pt()*0.001);
      m_mon_BcMuMuD0_Eta_pi.push_back((*eltrack2)->eta());
      m_mon_BcMuMuD0_Phi_pi.push_back((*eltrack2)->phi());
    } else {
      vec_tracksX = {eltrack2,eltrack1};
      vec_tracks  = {elmu1,elmu2,eltrack2,eltrack1};
    
      m_mon_BcMuMuD0_Pt_K.push_back ((*eltrack2)->pt()*0.001);
      m_mon_BcMuMuD0_Eta_K.push_back((*eltrack2)->eta());
      m_mon_BcMuMuD0_Phi_K.push_back((*eltrack2)->phi());
      m_mon_BcMuMuD0_Pt_pi.push_back( (*eltrack1)->pt()*0.001);
      m_mon_BcMuMuD0_Eta_pi.push_back((*eltrack1)->eta());
      m_mon_BcMuMuD0_Phi_pi.push_back((*eltrack1)->phi());
    }

    m_mon_BcMuMuD0_InvMass_D0.push_back(massX/1000.);
    m_mon_BcMuMuD0_InvMass_Bc.push_back(massXMuMu/1000.);
    
    if(m_doD0_Vertexing) {
        if ( timerSvc() ) m_VtxFitTimer->resume();
        if (!m_bphysHelperTool->vertexFit(fitVtx_X,vec_tracksX,massHypoX).isSuccess()) {
            ATH_MSG_DEBUG(" Failed to fit D0 vertex for " << "D0" );
            if ( timerSvc() ) m_VtxFitTimer->pause();
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        if ( timerSvc() ) m_VtxFitTimer->pause();
        
        
        float chi2X = fitVtx_X->fitchi2();
        if( chi2X > m_D0VtxChi2Cut || chi2X < 0  ) {
            ATH_MSG_DEBUG(" " << "D0" << " candidate rejected by D0 vertex chi2 cut: chi2 = " << chi2X );
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        m_countPassedD0VtxChi2++;
	//        m_mon_BcMuMuDplus_InvMass_Dplus.push_back(massX/1000.);
        //                   m_mon_BcMuMuDs_VtxMass_Ds.push_back(vtxMassX/1000.);
        m_mon_BcMuMuD0_Chi2_D0.push_back(chi2X);
        
    } // m_doDplus_Vertexing
    else {
      // if vertexting is not done - just attach trackParticle links
      for(auto tpel : vec_tracksX) {
        fitVtx_X->addTrackParticleLink(tpel);
      }
    }
    
    if(m_doBc_D0MuMuVertexing) {

      if(m_doBc_D0MuMuCascade) {
	// Cascade 2-vertex fit  

	//        Trk::VertexID dplusCascade, bcCascade;
        Trk::VertexID d0Cascade;

        std::vector<const xAOD::TrackParticle*> trackpDupletX;

        if ( xD0Mass1 > m_lowerD0_MassCut && xD0Mass1 < m_upperD0_MassCut ) {
          trackpDupletX.push_back(*eltrack1);
          trackpDupletX.push_back(*eltrack2);
	} else {
          trackpDupletX.push_back(*eltrack2);
          trackpDupletX.push_back(*eltrack1);
	}

	std::vector<double> massAtVrtX;
	massAtVrtX.push_back(KPLUSMASS);
	massAtVrtX.push_back(PIMASS);    

	m_VKVFitter->setDefault();

        d0Cascade = m_VKVFitter->startVertex(trackpDupletX,massAtVrtX);

        std::vector<Trk::VertexID> vrtList;
        vrtList.push_back(d0Cascade);

	std::vector<const xAOD::TrackParticle*> trackpXMuMu;
	trackpXMuMu.push_back(*elmu1);
        trackpXMuMu.push_back(*elmu2);

	std::vector<double> massAtVrtXMuMu;
	massAtVrtXMuMu.push_back(MUMASS);
	massAtVrtXMuMu.push_back(MUMASS);    

	//	bcCascade = m_VKVFitter->nextVertex(trackpXMuMu,massAtVrtXMuMu,vrtList);
        m_VKVFitter->nextVertex(trackpXMuMu,massAtVrtXMuMu,vrtList);

	Amg::Vector3D VertexX;
	Amg::Vector3D VertexXMuMu;

        Trk::VxCascadeInfo * result = m_VKVFitter->fitCascade();

        if ( timerSvc() ) m_VtxFitTimer->pause();
                            
        if( result==NULL ) {
          ATH_MSG_DEBUG(" Failed cascade fit for B_c -> D0" );
            delete fitVtx_X;
            delete fitVtx;
            delete result;
            return nullptr;
        } else {
          result->getSVOwnership(true); // to deleted the vertices of cascade together with VxCascadeInfo
          ATH_MSG_DEBUG(" Managed cascade fit for B_c -> D0" );
          m_countPassedBcD0Vtx++;

  	  const std::vector<xAOD::Vertex*> cascadeVertices = result->vertices();

          VertexX[0]     = cascadeVertices[0]->x(); VertexX[1]     = cascadeVertices[0]->y(); VertexX[2]     = cascadeVertices[0]->z();
          VertexXMuMu[0] = cascadeVertices[1]->x(); VertexXMuMu[1] = cascadeVertices[1]->y(); VertexXMuMu[2] = cascadeVertices[1]->z();

	  ATH_MSG_DEBUG(" Bc -> D0 Casc., VertexX     = " << VertexX[0] << " " << VertexX[1] << " " << VertexX[2] );
	  ATH_MSG_DEBUG(" Bc -> D0 Casc., VertexXMuMu = " << VertexXMuMu[0] << " " << VertexXMuMu[1] << " " << VertexXMuMu[2] );

	  float chi2XMuMu = result->fitChi2();
          fitVtx->setFitchi2(chi2XMuMu);
          if( !(chi2XMuMu < m_bCD0VtxChi2Cut) ) {
            ATH_MSG_DEBUG(" B_c -> D0 candidate rejected by XMuMu vertex chi2 cut: chi2 = " << chi2XMuMu );
            delete fitVtx_X;
            delete fitVtx;
            delete result;
            return nullptr;
          } else {
            ATH_MSG_DEBUG(" B_c -> D0 candidate accepted by XMuMu vertex chi2 cut: chi2 = " << chi2XMuMu );
            m_countPassedBcD0VtxChi2++;

            const std::vector< std::vector<TLorentzVector> > cascadeMomenta = result->getParticleMoms();

            std::vector<TLorentzVector> bcTrkMom = cascadeMomenta[1];
            std::vector<TLorentzVector> d0TrkMom = cascadeMomenta[0];

            std::vector<TLorentzVector>::iterator momItr;

            TLorentzVector d0Mom, bcMom;
            for(momItr=d0TrkMom.begin(); momItr!=d0TrkMom.end(); ++momItr) { d0Mom += *momItr; }
            for(momItr=bcTrkMom.begin(); momItr!=bcTrkMom.end(); ++momItr) { bcMom += *momItr; }

	    double pxD0 = d0Mom.Px();
	    double pyD0 = d0Mom.Py();

            double LxyD0 = LxyVal(pxD0,pyD0,VertexXMuMu,VertexX);

            if ( m_lowerD0_LxyCut > -99. && LxyD0 < m_lowerD0_LxyCut ) {
              ATH_MSG_DEBUG(" B_c -> D0 candidate rejected by Lxy cut: Lxy = " << LxyD0 );
              delete fitVtx_X;
              delete fitVtx;
              delete result;
              return nullptr;
	    } else {
              m_countPassedBcD0Lxy++;
              ATH_MSG_DEBUG(" B_c -> D0 candidate accepted by Lxy cut: Lxy = " << LxyD0 );

	      double vtxMassD0  = d0Mom.M();
              double PtD0       = d0Mom.Perp();

	      double vtxMassXMuMu = bcMom.M();
              double PtBc         = bcMom.Perp();
                                    
              ATH_MSG_DEBUG(" Bc -> D0 : vtxMassXMuMu = " << vtxMassXMuMu );

	      //              m_mon_BcMuMuDplus_InvMass_Dplus.push_back(massX/1000.);
              m_mon_BcMuMuD0_VtxMass_D0.push_back(vtxMassD0/1000.);
	      //              m_mon_BcMuMuDplus_Chi2_Dplus.push_back(chi2X);
              m_mon_BcMuMuD0_Lxy_D0.push_back(LxyD0);
              m_mon_BcMuMuD0_pT_D0.push_back(PtD0/1000.);
              m_mon_BcMuMuD0_pT_Bc.push_back(PtBc/1000.);
	      //              m_mon_BcMuMuDplus_InvMass_Bc.push_back(massXMuMu/1000.);
              m_mon_BcMuMuD0_VtxMass_Bc.push_back(vtxMassXMuMu/1000.);
              m_mon_BcMuMuD0_Chi2_Bc.push_back(chi2XMuMu);
              
              // manually attach the track links to fitVtx
              for(auto tpel : vec_tracks) {
                fitVtx->addTrackParticleLink(tpel);
              }
              
              // result of cascate fit is not needed anymore
              delete result;
                                    
            } // end XMuMu Lxy cut
          } // end XMuMu chi2 cut
	  //          } // end if(XMuMuVxCandidate)
        }  // result==NULL

                                                                                     
      } else {

        // Try to fit XMuMu -> mu mu X vertex
        if ( timerSvc() ) m_VtxFitTimer->resume();
        if (!m_bphysHelperTool->vertexFit(fitVtx,vec_tracks,massHypo).isSuccess()) {
            ATH_MSG_DEBUG(" Failed to fit XMuMu vertex for B_c -> Dstar" );
            if ( timerSvc() ) m_VtxFitTimer->pause();
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        if ( timerSvc() ) m_VtxFitTimer->pause();
        m_countPassedBcDstarVtx++;
        float chi2XMuMu = fitVtx->fitchi2();
        if( chi2XMuMu > m_bCDstarVtxChi2Cut ||  chi2XMuMu < 0 ) {
            ATH_MSG_DEBUG(" B_c -> D* candidate rejected by XMuMu vertex chi2 cut: chi2 = " << chi2XMuMu );
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        m_countPassedBcD0VtxChi2++;
        
        m_mon_BcMuMuD0_Chi2_Bc.push_back(chi2XMuMu);
      
      } //m_doBc_D0MuMuCascade  
    } //m_doBc_D0MuMuVertexing
    else {
      // if vertexting is not done - just attach trackParticle links
      for(auto tpel : vec_tracks) {
        fitVtx->addTrackParticleLink(tpel);
      }
    }
    
    ATH_MSG_DEBUG(" Good B_c->D0 found! doD0Vtx: " << (m_doD0_Vertexing       ? "Yes" : "No")
        << " doBcD0Vtx: " << (m_doBc_D0MuMuVertexing ? "Yes" : "No")
        << "  m = " << fitVtx->mass() << ", "
        << "chi2 = " << fitVtx->fitchi2() << ", vertex (" << fitVtx->fitx() << ", "
        << fitVtx->fity() << ", " << fitVtx->fitz() << ")" );
    
    trigPartX = fitVtx_X;
    return fitVtx;
} //checkBcMuMuD0

xAOD::TrigBphys* TrigEFBMuMuXFex::checkLbMuMuLambda(const ElementLink<xAOD::TrackParticleContainer> & eltrack1,
                                                    const ElementLink<xAOD::TrackParticleContainer> & eltrack2,
                                                    const ElementLink<xAOD::TrackParticleContainer> & elmu1,
                                                    const ElementLink<xAOD::TrackParticleContainer> & elmu2,
                                                    xAOD::TrigBphys* & trigPartX) {
    
    trigPartX = nullptr;
    xAOD::TrigBphys * fitVtx_X(nullptr);
    xAOD::TrigBphys * fitVtx(nullptr);
    
    std::vector<double> massHypoX = {PROTONMASS,PIMASS};
    std::vector<double> massHypo  = {MUMASS,MUMASS,PROTONMASS,PIMASS};
    
    ATH_MSG_DEBUG("Try to build Lambda with tracks " << *eltrack1 << ", " << *eltrack2 );
    
    
    float massX = XMass(*eltrack1, *eltrack2,lB_to_L);
    if( !(massX > m_lowerLambda_PrPiMassCut && massX < m_upperLambda_PrPiMassCut) ) {
        ATH_MSG_DEBUG(" Lambda candidate rejected by the mass cut: m = " << massX );
        return nullptr;
    }
        ATH_MSG_DEBUG(" Lambda candidate accepted by the mass cut: m = " << massX );
    m_countPassedLambdaMass++;
    
    float massXMuMu = XMuMuMass(*elmu1, *elmu2, *eltrack1, *eltrack2,lB_to_L);
    if( !(massXMuMu > m_lowerLb_LambdaMuMuMassCut && massXMuMu < m_upperLb_LambdaMuMuMassCut) ) {
        ATH_MSG_DEBUG(" L_b -> L candidate rejected by the mass cut: m = " << massXMuMu );
        return nullptr;
    }
        ATH_MSG_DEBUG(" L_b -> L candidate accepted by the mass cut: m = " << massXMuMu );
    m_countPassedLbMass++;
    
    // create initial object
    fitVtx_X = new xAOD::TrigBphys;
    fitVtx_X->makePrivateStore();
    fitVtx_X->initialise(0,0.,0.,0.,
                         xAOD::TrigBphys::LAMBDAPIP, massX, xAOD::TrigBphys::EF);
    fitVtx = new xAOD::TrigBphys;
    fitVtx->makePrivateStore();
    fitVtx->initialise(0,0.,0.,0.,
                       xAOD::TrigBphys::LBLMUMU, massXMuMu, xAOD::TrigBphys::EF);
    
    std::vector<ElementLink<xAOD::TrackParticleContainer> > vec_tracksX = {eltrack1,eltrack2};
    std::vector<ElementLink<xAOD::TrackParticleContainer> > vec_tracks  = {elmu1,elmu2,eltrack1,eltrack2};
    
    m_mon_LbMuMuLambda_Pt_P.push_back ((*eltrack1)->pt()*0.001);
    m_mon_LbMuMuLambda_Eta_P.push_back((*eltrack1)->eta());
    m_mon_LbMuMuLambda_Phi_P.push_back((*eltrack1)->phi());
    m_mon_LbMuMuLambda_Pt_Pi.push_back( (*eltrack2)->pt()*0.001);
    m_mon_LbMuMuLambda_Eta_Pi.push_back((*eltrack2)->eta());
    m_mon_LbMuMuLambda_Phi_Pi.push_back((*eltrack2)->phi());

    m_mon_LbMuMuLambda_InvMass_Lambda.push_back(massX/1000.);
    m_mon_LbMuMuLambda_InvMass_Lb.push_back(massXMuMu/1000.);
    
    if(m_doLambda_PPiVertexing) {
        if ( timerSvc() ) m_VtxFitTimer->resume();
        if (!m_bphysHelperTool->vertexFit(fitVtx_X,vec_tracksX,massHypoX).isSuccess()) {
            ATH_MSG_DEBUG(" Failed to fit X vertex for Lambda" );
            if ( timerSvc() ) m_VtxFitTimer->pause();
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        if ( timerSvc() ) m_VtxFitTimer->pause();
        
        
        float chi2X = fitVtx_X->fitchi2();
        if( chi2X > m_lambdaVtxChi2Cut || chi2X < 0  ) {
            ATH_MSG_DEBUG(" Lambda candidate rejected by X vertex chi2 cut: chi2 = " << chi2X );
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        m_countPassedLambdaVtxChi2++;
	//        m_mon_BcMuMuDplus_InvMass_Dplus.push_back(massX/1000.);
        //                   m_mon_BcMuMuDs_VtxMass_Ds.push_back(vtxMassX/1000.);
    m_mon_LbMuMuLambda_Chi2_Lambda.push_back(chi2X);
        
    } // m_doDplus_Vertexing
    else {
      // if vertexting is not done - just attach trackParticle links
      for(auto tpel : vec_tracksX) {
        fitVtx_X->addTrackParticleLink(tpel);
      }
    }
    
    if(m_doLb_LambdaMuMuVertexing) {

      if(m_doLb_LambdaMuMuCascade) {
	// Cascade 2-vertex fit  

	//        Trk::VertexID dplusCascade, bcCascade;
        Trk::VertexID LambdaCascade;

        std::vector<const xAOD::TrackParticle*> trackpDupletX;
        trackpDupletX.push_back(*eltrack1);
        trackpDupletX.push_back(*eltrack2);

	std::vector<double> massAtVrtX;
	massAtVrtX.push_back(PROTONMASS);
	massAtVrtX.push_back(PIMASS);    

	m_VKVFitter->setDefault();

        LambdaCascade = m_VKVFitter->startVertex(trackpDupletX,massAtVrtX);

        std::vector<Trk::VertexID> vrtList;
        vrtList.push_back(LambdaCascade);

	std::vector<const xAOD::TrackParticle*> trackpXMuMu;
	trackpXMuMu.push_back(*elmu1);
        trackpXMuMu.push_back(*elmu2);

	std::vector<double> massAtVrtXMuMu;
	massAtVrtXMuMu.push_back(MUMASS);
	massAtVrtXMuMu.push_back(MUMASS);    

	//	bcCascade = m_VKVFitter->nextVertex(trackpXMuMu,massAtVrtXMuMu,vrtList);
        m_VKVFitter->nextVertex(trackpXMuMu,massAtVrtXMuMu,vrtList);

	Amg::Vector3D VertexX;
	Amg::Vector3D VertexXMuMu;

        Trk::VxCascadeInfo * result = m_VKVFitter->fitCascade();

        if ( timerSvc() ) m_VtxFitTimer->pause();
                            
        if( result==NULL ) {
          ATH_MSG_DEBUG(" Failed cascade fit for L_b -> Lambda" );
            delete fitVtx_X;
            delete fitVtx;
            delete result;
            return nullptr;
        } else {
          result->getSVOwnership(true); // to deleted the vertices of cascade together with VxCascadeInfo
          ATH_MSG_DEBUG(" Managed cascade fit for L_b -> Lambda" );
          m_countPassedLbVtx++;

  	  const std::vector<xAOD::Vertex*> cascadeVertices = result->vertices();

          VertexX[0]     = cascadeVertices[0]->x(); VertexX[1]     = cascadeVertices[0]->y(); VertexX[2]     = cascadeVertices[0]->z();
          VertexXMuMu[0] = cascadeVertices[1]->x(); VertexXMuMu[1] = cascadeVertices[1]->y(); VertexXMuMu[2] = cascadeVertices[1]->z();

	  ATH_MSG_DEBUG(" Lb -> Lambda Casc., VertexX     = " << VertexX[0] << " " << VertexX[1] << " " << VertexX[2] );
	  ATH_MSG_DEBUG(" Lb -> Lambda Casc., VertexXMuMu = " << VertexXMuMu[0] << " " << VertexXMuMu[1] << " " << VertexXMuMu[2] );

	  float chi2XMuMu = result->fitChi2();
          fitVtx->setFitchi2(chi2XMuMu);
          if( !(chi2XMuMu < m_lBVtxChi2Cut) ) {
            ATH_MSG_DEBUG(" L_b -> Lambda candidate rejected by XMuMu vertex chi2 cut: chi2 = " << chi2XMuMu );
            delete fitVtx_X;
            delete fitVtx;
            delete result;
            return nullptr;
          } else {
            ATH_MSG_DEBUG(" L_b -> Lambda candidate accepted by XMuMu vertex chi2 cut: chi2 = " << chi2XMuMu );
            m_countPassedLbVtxChi2++;

            const std::vector< std::vector<TLorentzVector> > cascadeMomenta = result->getParticleMoms();

            std::vector<TLorentzVector> lbTrkMom = cascadeMomenta[1];
            std::vector<TLorentzVector> laTrkMom = cascadeMomenta[0];

            std::vector<TLorentzVector>::iterator momItr;

            TLorentzVector laMom, lbMom;
            for(momItr=laTrkMom.begin(); momItr!=laTrkMom.end(); ++momItr) { laMom += *momItr; }
            for(momItr=lbTrkMom.begin(); momItr!=lbTrkMom.end(); ++momItr) { lbMom += *momItr; }

	    double pxLambda = laMom.Px();
	    double pyLambda = laMom.Py();

            double LxyLambda = LxyVal(pxLambda,pyLambda,VertexXMuMu,VertexX);
                                        
            if ( m_lowerLambda_LxyCut > -99. && LxyLambda < m_lowerLambda_LxyCut ) {
              ATH_MSG_DEBUG(" L_b -> Lambda candidate rejected by Lxy cut: Lxy = " << LxyLambda );
              delete fitVtx_X;
              delete fitVtx;
              delete result;
              return nullptr;
	    } else {
              m_countPassedLbLambdaLxy++;
              ATH_MSG_DEBUG(" L_b -> Lambda candidate accepted by Lxy cut: Lxy = " << LxyLambda );

	      double vtxMassX     = laMom.M();
	      double vtxMassXMuMu = lbMom.M();
    
              if( !(vtxMassX < m_upperLambdaVtxMassCut) ) {
                ATH_MSG_DEBUG(" Lambda candidate rejected by the vtx mass cut: m = " << vtxMassX );
                delete fitVtx_X;
                delete fitVtx;
                delete result;
                return nullptr;
              }
              m_countPassedLambdaVtxMass++;
    
              if( !(vtxMassXMuMu > m_lowerLb_LambdaMuMuVtxMassCut && vtxMassXMuMu < m_upperLb_LambdaMuMuVtxMassCut) ) {
                ATH_MSG_DEBUG(" L_b -> L candidate rejected by the vtx mass cut: m = " << vtxMassXMuMu );
                delete fitVtx_X;
                delete fitVtx;
                delete result;
                return nullptr;
              }
              m_countPassedLbVtxMass++;
    
              double PtLambda = laMom.Perp();
              double PtLb     = lbMom.Perp();
                                    
	      //                                    trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::DPKPIPI, massX);

              ATH_MSG_DEBUG(" Lb -> Lambda : vtxMassXMuMu = " << vtxMassXMuMu );

	      //              m_mon_BcMuMuDplus_InvMass_Dplus.push_back(massX/1000.);
              m_mon_LbMuMuLambda_VtxMass_Lambda.push_back(vtxMassX/1000.);
	      //              m_mon_BcMuMuDplus_Chi2_Dplus.push_back(chi2X);
              m_mon_LbMuMuLambda_Lxy_Lambda.push_back(LxyLambda);
              m_mon_LbMuMuLambda_pT_Lambda.push_back(PtLambda/1000.);
              m_mon_LbMuMuLambda_pT_Lb.push_back(PtLb/1000.);
	      //              m_mon_BcMuMuDplus_InvMass_Bc.push_back(massXMuMu/1000.);
              m_mon_LbMuMuLambda_VtxMass_Lb.push_back(vtxMassXMuMu/1000.);
              m_mon_LbMuMuLambda_Chi2_Lb.push_back(chi2XMuMu);

              m_mon_LbMuMuLambda_FinMass_Lambda.push_back(massX/1000.);
              m_mon_LbMuMuLambda_FinMass_Lb.push_back(massXMuMu/1000.);
              
              // manually attach the track links to fitVtx
              for(auto tpel : vec_tracks) {
                fitVtx->addTrackParticleLink(tpel);
              }
              
              // result of cascate fit is not needed anymore
              delete result;
                                    
            } // end XMuMu Lxy cut
          } // end XMuMu chi2 cut
	  //          } // end if(XMuMuVxCandidate)
        }  // result==NULL

                                                                                     
      } else {

        // Try to fit XMuMu -> mu mu X vertex
        if ( timerSvc() ) m_VtxFitTimer->resume();
        if (!m_bphysHelperTool->vertexFit(fitVtx,vec_tracks,massHypo).isSuccess()) {
            ATH_MSG_DEBUG(" Failed to fit XMuMu vertex for L_b -> Lambda" );
            if ( timerSvc() ) m_VtxFitTimer->pause();
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        if ( timerSvc() ) m_VtxFitTimer->pause();
        m_countPassedLbVtx++;
        float chi2XMuMu = fitVtx->fitchi2();
        if( chi2XMuMu > m_lBVtxChi2Cut ||  chi2XMuMu < 0 ) {
            ATH_MSG_DEBUG(" L_b -> Lambda candidate rejected by XMuMu vertex chi2 cut: chi2 = " << chi2XMuMu );
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        m_countPassedLbVtxChi2++;
        
	//        m_mon_BcMuMuDplus_InvMass_Bc.push_back(massXMuMu/1000.);
        //                   m_mon_BcMuMuDs_VtxMass_Bc.push_back(vtxMassXMuMu/1000.);
        m_mon_LbMuMuLambda_Chi2_Lb.push_back(chi2XMuMu);
      
      } //m_doLb_LambdaMuMuCascade  
    } //m_doLb_LambdaMuMuVertexing
    else {
      // if vertexting is not done - just attach trackParticle links
      for(auto tpel : vec_tracks) {
        fitVtx->addTrackParticleLink(tpel);
      }
    }
    
    ATH_MSG_DEBUG(" Good L_b->Lambda found! doLambdaVtx: " << (m_doLambda_PPiVertexing ? "Yes" : "No")
        << " doLbLambdaVtx: " << (m_doLb_LambdaMuMuVertexing ? "Yes" : "No")
        << "  m = " << fitVtx->mass() << ", "
        << "chi2 = " << fitVtx->fitchi2() << ", vertex (" << fitVtx->fitx() << ", "
        << fitVtx->fity() << ", " << fitVtx->fitz() << ")" );
    
    trigPartX = fitVtx_X;
    return fitVtx;
} //checkLbMuMuLambda

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
        pMon_BMuMu2X_Pt_X1      = &m_mon_BdMuMuKs_Pt_K;
        pMon_BMuMu2X_Eta_X1     = &m_mon_BdMuMuKs_Eta_K;
        pMon_BMuMu2X_Phi_X1     = &m_mon_BdMuMuKs_Phi_K;
        pMon_BMuMu2X_Pt_X2      = &m_mon_BdMuMuKs_Pt_Pi;
        pMon_BMuMu2X_Eta_X2     = &m_mon_BdMuMuKs_Eta_Pi;
        pMon_BMuMu2X_Phi_X2     = &m_mon_BdMuMuKs_Phi_Pi;
        pMon_BMuMu2X_InvMass_2X = &m_mon_BdMuMuKs_InvMass_Kstar;
        pMon_BMuMu2X_VtxMass_2X = &m_mon_BdMuMuKs_VtxMass_Kstar;
        pMon_BMuMu2X_Chi2_2X    = &m_mon_BdMuMuKs_Chi2_Kstar;
        pMon_BMuMu2X_InvMass_B  = &m_mon_BdMuMuKs_InvMass_Bd;
        pMon_BMuMu2X_VtxMass_B  = &m_mon_BdMuMuKs_VtxMass_Bd;
        pMon_BMuMu2X_Chi2_B     = &m_mon_BdMuMuKs_Chi2_Bd;
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
        pMon_BMuMu2X_Pt_X1      = &m_mon_BsMuMuPhi_Pt_K1;
        pMon_BMuMu2X_Eta_X1     = &m_mon_BsMuMuPhi_Eta_K1;
        pMon_BMuMu2X_Phi_X1     = &m_mon_BsMuMuPhi_Phi_K1;
        pMon_BMuMu2X_Pt_X2      = &m_mon_BsMuMuPhi_Pt_K2;
        pMon_BMuMu2X_Eta_X2     = &m_mon_BsMuMuPhi_Eta_K2;
        pMon_BMuMu2X_Phi_X2     = &m_mon_BsMuMuPhi_Phi_K2;
        pMon_BMuMu2X_InvMass_2X = &m_mon_BsMuMuPhi_InvMass_Phi1020;
        pMon_BMuMu2X_VtxMass_2X = &m_mon_BsMuMuPhi_VtxMass_Phi1020;
        pMon_BMuMu2X_Chi2_2X    = &m_mon_BsMuMuPhi_Chi2_Phi1020;
        pMon_BMuMu2X_InvMass_B  = &m_mon_BsMuMuPhi_InvMass_Bs;
        pMon_BMuMu2X_VtxMass_B  = &m_mon_BsMuMuPhi_VtxMass_Bs;
        pMon_BMuMu2X_Chi2_B     = &m_mon_BsMuMuPhi_Chi2_Bs;
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
        pMon_BMuMu2X_Pt_X1      = &m_mon_LbMuMuLambda_Pt_P;
        pMon_BMuMu2X_Eta_X1     = &m_mon_LbMuMuLambda_Eta_P;
        pMon_BMuMu2X_Phi_X1     = &m_mon_LbMuMuLambda_Phi_P;
        pMon_BMuMu2X_Pt_X2      = &m_mon_LbMuMuLambda_Pt_Pi;
        pMon_BMuMu2X_Eta_X2     = &m_mon_LbMuMuLambda_Eta_Pi;
        pMon_BMuMu2X_Phi_X2     = &m_mon_LbMuMuLambda_Phi_Pi;
        pMon_BMuMu2X_InvMass_2X = &m_mon_LbMuMuLambda_InvMass_Lambda;
        pMon_BMuMu2X_VtxMass_2X = &m_mon_LbMuMuLambda_VtxMass_Lambda;
        pMon_BMuMu2X_Chi2_2X    = &m_mon_LbMuMuLambda_Chi2_Lambda;
        pMon_BMuMu2X_InvMass_B  = &m_mon_LbMuMuLambda_InvMass_Lb;
        pMon_BMuMu2X_VtxMass_B  = &m_mon_LbMuMuLambda_VtxMass_Lb;
        pMon_BMuMu2X_Chi2_B     = &m_mon_LbMuMuLambda_Chi2_Lb;
    } else {
        ATH_MSG_DEBUG("Wrong decay identifier passed to checkBMuMu2X: decay = " << decay );
        m_mon_Errors.push_back(ERROR_WrongDecayID);
        return nullptr;
    }
     
   ATH_MSG_DEBUG("Try to build " << decayName << " with tracks " << *eltrack1 << ", " << *eltrack2 );
    
    float massX = XMass(*eltrack1, *eltrack2, decay);
    if( !(massX > lowerXMassCut && massX < upperXMassCut) ) {
        ATH_MSG_DEBUG(" " << decayName << " candidate rejected by X mass cut: m = " << massX );
        trigPartX = nullptr;
        return nullptr;
    }
    
    ++(*countPassedXMass);
    
    float massXMuMu = XMuMuMass(*elmu1, *elmu2, *eltrack1, *eltrack2, decay);
    if( !(massXMuMu > lowerXMuMuMassCut && massXMuMu < upperXMuMuMassCut) ) {
        ATH_MSG_DEBUG(" " << decayName << " candidate rejected by XMuMu mass cut: m = " << massXMuMu );
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
            ATH_MSG_DEBUG(" Vertex fitting of X failed." << decayName );
        }
        if ( timerSvc() ) m_VtxFitTimer->pause();
        
        ++(*countPassedXVtx);
        double chi2X(fitVtx_X->fitchi2());
        if( chi2X > chi2XCut || chi2X < 0 ) {
            ATH_MSG_DEBUG(" " << decayName << " candidate rejected by X vertex chi2 cut: chi2 = " << chi2X );
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
            ATH_MSG_DEBUG(" Vertex fitting of MuMuX failed for " << decayName );
        }
        if ( timerSvc() ) m_VtxFitTimer->pause();
        (*countPassedXMuMuVtx)++;
        double chi2MuMuX(fitVtx->fitchi2());
        if( chi2MuMuX > chi2XMuMuCut || chi2MuMuX < 0 ) {
            ATH_MSG_DEBUG(" " << decayName << " candidate rejected by XMuMu vertex chi2 cut: chi2 = " << chi2MuMuX );
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
    if(decay == bC_to_PiPi){  
        massHypo.push_back(PIMASS);
        massHypo.push_back(PIMASS);    
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

double TrigEFBMuMuXFex::XKPiPiMass(const xAOD::TrackParticle* particle1, const xAOD::TrackParticle* particle2, const xAOD::TrackParticle* particle3 ) {
    
    std::vector<double> massHypo;
    massHypo.clear();
    massHypo.push_back(KPLUSMASS);
    massHypo.push_back(PIMASS);
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
    if(decay == bC_to_PiPi){  
        massHypo.push_back(PIMASS);
        massHypo.push_back(PIMASS);    
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

double TrigEFBMuMuXFex::XKPiPiMuMuMass(const xAOD::TrackParticle* mu1, const xAOD::TrackParticle* mu2, const xAOD::TrackParticle* particle1,
                                   const xAOD::TrackParticle* particle2, const xAOD::TrackParticle* particle3 ) {
    std::vector<double> massHypo;
    massHypo.clear();
    massHypo.push_back(MUMASS);
    massHypo.push_back(MUMASS);
    massHypo.push_back(KPLUSMASS);
    massHypo.push_back(PIMASS);
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

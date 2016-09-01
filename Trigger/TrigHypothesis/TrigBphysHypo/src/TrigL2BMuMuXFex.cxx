// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBPhysHypo/TrigL2BMuMuXFex.cxx
 **
 **   Description: L2 hypothesis algorithms for B+/- -> K+/- mu+ mu-, Bd -> K* mu+ mu-, Bs -> Phi(1020) mu+ mu-, Lb -> L mu+ mu-
 **                Create TrigL2Bphys
 **                Change log: (20.2.2010 Lb->LMuMu addon); (10.3.2010 monitoring addon); (11.4.2010 bugs removed); (5.5.2010 log levels downgrade); (6.6.2010 monitoring update);
 **							   (27.2.2011 bug fixed);
 **
 **   Author: Pavel.Strachota@cern.ch
 **           Semen.Turchikhin@cern.ch
 **
 **   Created:   10.01.2010 based on C. Adorisio's code TrigL2DiMuFex.cxx & D. Scheirich's code TrigL2LbMuMuLFex.cxx
 **
 **   Modified:  26.09.2012 : "B_c -> D_s(*) (->Phi(->K+K-)) mu+mu-" added (Leonid Gladilin <gladilin@mail.cern.ch>)
 **
 **                
 **************************************************************************/ 

#include "TrigL2BMuMuXFex.h"

#include "BtrigUtils.h"

#include "TrigBphysHypo/Constants.h"                                   

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "AthenaKernel/Timeout.h"

//#include "TrigMuonEvent/TrigCombDiMuonContainer.h"
//#include "TrigMuonEvent/CombinedMuonFeature.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "TrigInDetToolInterfaces/TrigParticleTable.h"                           
#include "TrigInDetToolInterfaces/ITrigL2VertexFitter.h"                         
#include "TrigInDetEvent/TrigL2Vertex.h"                         

#include "TrigParticle/TrigL2BphysContainer.h"

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"
#include "TrigBphysHelperUtilsTool.h"


#include <math.h>
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"


// temporary
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>


template <class T>
T merge_no_dupl(const T& v1, const T& v2) {
    T ret(v1);
    for (typename T::const_iterator itr = v2.begin(); itr != v2.end(); ++itr) {
        bool notfound = true;
        for (typename T::const_iterator z = ret.begin(); z != ret.end(); ++z) {
            if (*z == *itr) {
                notfound = false;
                break;
            }
        }
        if (notfound) {
            ret.push_back(*itr);
        }
    }
    return ret;
}

/*------------------------------------------------------------------------------------------*/
TrigL2BMuMuXFex::TrigL2BMuMuXFex(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::ComboAlgo(name, pSvcLocator)
,m_bphysHelperTool("TrigBphysHelperUtilsTool")
,m_TotTimer(0)
,m_VtxFitTimer(0)
,m_L2vertFitter(0)
,m_vertexingTool(0)
{                     
////////////////////////////// Declaration of python configurables /////////////////////////////////////////
  // Read cuts or accept all?
  declareProperty("AcceptAll",    m_acceptAll = false);                                 // default = false
  

  // sign & mass cuts                    
  declareProperty("OppositeSign", m_oppositeCharge = true);                            // deafult = true
  declareProperty("LowerMuMuMassCut", m_lowerMuMuMassCut = 100.);                      // default = 100.0 MeV
  declareProperty("UpperMuMuMassCut", m_upperMuMuMassCut = 5500.);                      // default = 5500.0 MeV
  declareProperty("LowerMuVtxMassCut", m_lowerMuVtxMassCut =100.);                    // default = 100.0 MeV
  declareProperty("UpperMuVtxMassCut", m_upperMuVtxMassCut = 5500.);                    // default = 5500.0 MeV
  declareProperty("MuVtxChi2Cut", m_muVtxChi2Cut = 40.);                              // default = 40.0

  // Maximum number of decay candidates (2 mu + 1 or 2 tracks) to test - protection against TimeOuts
  declareProperty("MaxNcombinations", m_maxNcombinations = 20000);
  
  // B+->K+MuMu cuts
  declareProperty("DoB_KMuMuDecay", m_doB_KMuMuDecay = true);                          //Proceed B->KMuMu part of algorithm  
  declareProperty("LowerKMuMuMassCut", m_lowerKMuMuMassCut = 4500.);                    // default = 4500.0 MeV
  declareProperty("UpperKMuMuMassCut", m_upperKMuMuMassCut = 5900.);                    // default = 5900.0 MeV
  declareProperty("LowerB_KMuMuMassCutVtxOff", m_lowerB_KMuMuMassCutVtxOff = 4500.);  // default = 4500.0 MeV
  declareProperty("UpperB_KMuMuMassCutVtxOff", m_upperB_KMuMuMassCutVtxOff= 5900.);  // default = 5900.0 MeV
  declareProperty("LowerBVtxMassCut", m_lowerBVtxMassCut = 4500.);                      // default = 4500.0 MeV
  declareProperty("UpperBVtxMassCut", m_upperBVtxMassCut = 5900.);                      // default = 5900.0 MeV
  declareProperty("BVtxChi2Cut", m_bVtxChi2Cut = 300.);                                // default = 300.0
  
  // Bd->K*MuMu cuts
  declareProperty("DoBd_KstarMuMuDecay", m_doBd_KstarMuMuDecay = true);                //Proceed Bd->K*MuMu part of algorithm
  declareProperty("LowerKstar_KaonMassCut", m_lowerKstar_KaonMassCut = 600.);          // default = 600.0 MeV
  declareProperty("UpperKstar_KaonMassCut", m_upperKstar_KaonMassCut = 1500.);          // default = 1500.0 MeV
  declareProperty("LowerBd_KstarMuMuMassCut", m_lowerBd_KstarMuMuMassCut= 4300.);      // default = 4600.0 MeV
  declareProperty("UpperBd_KstarMuMuMassCut", m_upperBd_KstarMuMuMassCut =6300.);      // default = 6300.0 MeV
  declareProperty("LowerBd_KstarMuMuMassCutVtxOff", m_lowerBd_KstarMuMuMassCutVtxOff = 4600.);  // default = 4600.0 MeV
  declareProperty("UpperBd_KstarMuMuMassCutVtxOff", m_upperBd_KstarMuMuMassCutVtxOff= 6300.);  // default = 6300.0 MeV
  declareProperty("LowerKstarVtxMassCut", m_lowerKstarVtxMassCut = 600.);              // default = 600.0 MeV
  declareProperty("UpperKstarVtxMassCut", m_upperKstarVtxMassCut= 1200.);              // default = 1200.0 MeV
  declareProperty("LowerBdVtxMassCut", m_lowerBdVtxMassCut = 4600.);                    // default = 4600.0 MeV
  declareProperty("UpperBdVtxMassCut", m_upperBdVtxMassCut = 6000.);                    // default = 6000.0 MeV
  declareProperty("KstarVtxChi2Cut", m_kStarVtxChi2Cut = 60.);                        // default = 60.0
  declareProperty("BdVtxChi2Cut", m_bDVtxChi2Cut = 60.);                              // default = 60.0
  
  // Bs->PhiMuMu cuts
  declareProperty("DoBs_Phi1020MuMuDecay", m_doBs_Phi1020MuMuDecay = true);            //Proceed Bs->PhiMuMu part of algorithm
  declareProperty("LowerPhi1020_KaonMassCut", m_lowerPhi1020_KaonMassCut  = 990.);      // default = 990.0 MeV
  declareProperty("UpperPhi1020_KaonMassCut", m_upperPhi1020_KaonMassCut = 1050.);      // default = 1050.0 MeV
  declareProperty("LowerBs_Phi1020MuMuMassCut", m_lowerBs_Phi1020MuMuMassCut = 5000.);  // default = 5000.0 MeV
  declareProperty("UpperBs_Phi1020MuMuMassCut", m_upperBs_Phi1020MuMuMassCut = 5800.);  // default = 5800.0 MeV
  declareProperty("LowerBs_Phi1020MuMuMassCutVtxOff", m_lowerBs_Phi1020MuMuMassCutVtxOff = 5000.);  // default = 5000.0 MeV
  declareProperty("UpperBs_Phi1020MuMuMassCutVtxOff", m_upperBs_Phi1020MuMuMassCutVtxOff = 5800.);  // default = 5800.0 MeV
  declareProperty("LowerPhi1020VtxMassCut", m_lowerPhi1020VtxMassCut = 990.);          // default = 990.0 MeV
  declareProperty("UpperPhi1020VtxMassCut", m_upperPhi1020VtxMassCut = 1060.);          // default = 1060.0 MeV
  declareProperty("LowerBsVtxMassCut", m_lowerBsVtxMassCut = 5000.);                    // default = 5000.0 MeV
  declareProperty("UpperBsVtxMassCut", m_upperBsVtxMassCut = 5800.);                    // default = 5800.0 MeV
  declareProperty("Phi1020VtxChi2Cut", m_phi1020VtxChi2Cut = 60.);                    // default = 60.0
  declareProperty("BsVtxChi2Cut", m_bSVtxChi2Cut = 60.);                              // default = 60.0
  
  // Lb->LMuMu cuts // cuts optimalization in progress
  declareProperty("DoLb_LambdaMuMuDecay", m_doLb_LambdaMuMuDecay = true);              //Proceed Lb->LMuMu part of algorithm
  declareProperty("LowerLambda_PrPiMassCut", m_lowerLambda_PrPiMassCut = 1040.);	   // optimal = 1080.0 MeV
  declareProperty("UpperLambda_PrPiMassCut", m_upperLambda_PrPiMassCut = 1220.);       // default = 2 500.0 MeV
  declareProperty("LowerLb_LambdaMuMuMassCut", m_lowerLb_LambdaMuMuMassCut = 5090.);    // default = 1 000.0 MeV
  declareProperty("UpperLb_LambdaMuMuMassCut", m_upperLb_LambdaMuMuMassCut = 6270.);    // default = 10 000.0 MeV
  declareProperty("LowerLb_LambdaMuMuMassCutVtxOff", m_lowerLb_LambdaMuMuMassCutVtxOff = 5090.); // default = 2 000.0 MeV
  declareProperty("UpperLb_LambdaMuMuMassCutVtxOff", m_upperLb_LambdaMuMuMassCutVtxOff = 6270.); // default = 8 000.0 MeV
  declareProperty("LowerLambdaVtxMassCut", m_lowerLambdaVtxMassCut = 1070.);            // default = 200.0 MeV
  declareProperty("UpperLambdaVtxMassCut", m_upperLambdaVtxMassCut = 1160.);            // default = 2 000.0 MeV
  declareProperty("LambdaVtxChi2Cut", m_lambdaVtxChi2Cut = 100.);                      // default = 500.0 MeV
  declareProperty("LowerLbVtxMassCut", m_lowerLbVtxMassCut = 4800.);                    // default = 1 000.0 MeV
  declareProperty("UpperLbVtxMassCut", m_upperLbVtxMassCut = 6100.);                    // default = 10 000.0 MeV
  declareProperty("LbVtxChi2Cut", m_lBVtxChi2Cut = 100.);                              // default = 500.0
  // cuts below anre not used now
//   declareProperty("LbVtxDistanceCut", m_lBVtxDistanceCut = 0.);                      // default = 0.0
//   declareProperty("PiImpactCut", m_piImpactCut = 0.);                                // default = 0.0
//   declareProperty("PrImpactCut", m_prImpactCut = 0.);                                // default = 0.0
  
  // Bc->Ds(->Phi(->K+K-))MuMu cuts
  declareProperty("DoBc_DsMuMuDecay", m_doBc_DsMuMuDecay = true);            //Proceed Bc->DsMuMu part of algorithm
  declareProperty("LowerPhiDs_MassCut", m_lowerPhiDs_MassCut =  980.);       // default =  980.0 MeV
  declareProperty("UpperPhiDs_MassCut", m_upperPhiDs_MassCut = 1080.);       // default = 1080.0 MeV
  declareProperty("LowerDs_MassCut", m_lowerDs_MassCut    = 1550.);          // default = 1550.0 MeV
  declareProperty("UpperDs_MassCut", m_upperDs_MassCut    = 2450.);          // default = 2450.0 MeV
  declareProperty("LowerBc_DsMuMuMassCut", m_lowerBc_DsMuMuMassCut = 1750.); // default = 1750.0 MeV
  declareProperty("UpperBc_DsMuMuMassCut", m_upperBc_DsMuMuMassCut = 7100.); // default = 7100.0 MeV

  declareProperty("LowerBc_DsMuMuMassCutVtxOff", m_lowerBc_DsMuMuMassCutVtxOff = 1800.);  // default = 1800.0 MeV
  declareProperty("UpperBc_DsMuMuMassCutVtxOff", m_upperBc_DsMuMuMassCutVtxOff = 7100.);  // default = 7100.0 MeV

  declareProperty("LowerDsVtxMassCut", m_lowerDsVtxMassCut = 1600.);        // default = 1600.0 MeV
  declareProperty("UpperDsVtxMassCut", m_upperDsVtxMassCut = 2400.);        // default = 2400.0 MeV

  declareProperty("LowerBcVtxMassCut", m_lowerBcVtxMassCut = 1800.);        // default = 1800.0 MeV
  declareProperty("UpperBcVtxMassCut", m_upperBcVtxMassCut = 7050.);        // default = 7050.0 MeV

  declareProperty("DsVtxChi2Cut",    m_DsVtxChi2Cut =  90.);                 // default =  90.0
  declareProperty("BcVtxChi2Cut",    m_bCVtxChi2Cut = 120.);                 // default = 120.0
  
  //
  // Vertexing flags
  //
  // vertexing on/off                                                                                  
  declareProperty("DoVertexFit", m_doVertexFit = false);                               // vertexing on/off (global), default = true
  // muon pair vertex
  declareProperty("DoMuMuVertexFit", m_doMuMuVertexFit = true);                        // mumu pair vertexing, default = true                       
  // B+->K+MuMu
  declareProperty("DoB_KMuMuVertexFit", m_doB_KMuMuVertexFit = true);                  // kaon&mu&mu vertexing, default = true                       
  // Bd->K*MuMu
  declareProperty("DoKstar_KaonPionVertexFit", m_doKstar_KaonPionVertexFit = true);    // kaon pion vertexing, default = true                                                       
  declareProperty("DoBd_KstarMuMuVertexFit", m_doBd_KstarMuMuVertexFit = true);        // kaon&pion&mu&mu vertexing, default = true
  // Bs->PhiMuMu
  declareProperty("DoPhi1020_KaonKaonVertexFit", m_doPhi1020_KaonKaonVertexFit = true); // kaon kaon vertexing, default = true
  declareProperty("DoBs_Phi1020MuMuVertexFit", m_doBs_Phi1020MuMuVertexFit = true);    // kaon&kaon&mu&mu vertexing, default = true
  // Lb->MuMuL
  declareProperty("DoLambda_ProtonPionVertexFit", m_doLambda_ProtonPionVertexFit = true); // default = true
  declareProperty("DoLb_LambdaMuMuVertexFit", m_doLb_LambdaMuMuVertexFit = false);      // p&pi&mu&mu vertexing default = true  
  // Bc->MuMuDs
  declareProperty("DoDs_PhiPionVertexFit", m_doDs_PhiPionVertexFit = true);      // default = true
  declareProperty("DoBc_DsMuMuVertexFit",  m_doBc_DsMuMuVertexFit = false);      // kaon&kaon&pi&mu&mu vertexing default = true  
    

////////////////////////////// end of python configurables /////////////////////////////////////////////////

////////////////////////////// Declaration of vertexing tools - now in initialization //////////////////////////////////////////////
//  m_vertFitter("TrigVertexFitter",this),
//  m_L2vertFitter("TrigL2VertexFitter",this),
//  m_vertexingTool("TrigVertexingTool",this);
  // Read properties
//  declareProperty("TrigVertexFitter"  , m_vertFitter);
//  declareProperty("TrigL2VertexFitter", m_L2vertFitter);
//  declareProperty("TrigVertexingTool" , m_vertexingTool, "TrigVertexingTool");
////////////////////////////// end of vertexing tools //////////////////////////////////////////////////////
  
////////////////////////////// Declaration of variables for monitoring histograms //////////////////////////
// main graphs with general content
  declareMonitoredStdContainer("Errors"                           , m_mon_Errors               , AutoClear);            // OK add Bmumux to the name of the varible?
  declareMonitoredStdContainer("Acceptance"                       , m_mon_Acceptance           , AutoClear);            // OK

// graphs with LVL1 parameters
  declareMonitoredStdContainer("ROIsEta"                          , m_mon_ROIsEta              , AutoClear);            // OK both rois parameters to this one
  declareMonitoredStdContainer("ROIsPhi"                          , m_mon_ROIsPhi              , AutoClear);            // OK both rois parameters to this one
  //declareMonitoredStdContainer("DeltaROIsEta"                     , m_mon_deltaROIsEta         , AutoClear);            // OK both rois parameters to this one
  //declareMonitoredStdContainer("DeltaROIsPhi"                     , m_mon_deltaROIsPhi         , AutoClear);            // OK both rois parameters to this one
  //declareMonitoredStdContainer("DeltaROIsR"                       , m_mon_deltaROIsR           , AutoClear);            // OK both rois parameters to this one
  declareMonitoredStdContainer("nTracks"                          , m_mon_nTracks              , AutoClear);            // OK = number of tracks in collection?
  //declareMonitoredStdContainer("DeltaEtaMuMu",                    , m_mon_dEtaMuMu             , AutoClear);            // OK = dEta for muons
  //declareMonitoredStdContainer("DeltaPhiMuMu"                     , m_mon_dPhiMuMu             , AutoClear);            // OK = dPhi for muons
  //declareMonitoredStdContainer("DeltaRMuMu"                       , m_mon_dRMuMu               , AutoClear);            // OK = dR for muons
  declareMonitoredStdContainer("EtaTracks"                        , m_mon_etaTracks            , AutoClear);            // OK = eta of all tracks
  declareMonitoredStdContainer("PhiTracks"                        , m_mon_phiTracks            , AutoClear);            // OK = phi of all tracks
  declareMonitoredStdContainer("PtTracks"                         , m_mon_pTTracks             , AutoClear);            // OK = eta of all tracks

// graphs with muons parameters
  declareMonitoredStdContainer("Muon1Eta"                         , m_mon_muon1Eta             , AutoClear);            // OK
  declareMonitoredStdContainer("Muon1Phi"                         , m_mon_muon1Phi             , AutoClear);            // OK
  declareMonitoredStdContainer("Muon1Pt"                          , m_mon_muon1Pt              , AutoClear);            // OK
  declareMonitoredStdContainer("Muon1Z0"                          , m_mon_muon1Z0              , AutoClear);            // OK
  declareMonitoredStdContainer("Muon1A0"                          , m_mon_muon1A0              , AutoClear);            // OK
  declareMonitoredStdContainer("Muon2Eta"                         , m_mon_muon2Eta             , AutoClear);            // OK
  declareMonitoredStdContainer("Muon2Phi"                         , m_mon_muon2Phi             , AutoClear);            // OK
  declareMonitoredStdContainer("Muon2Pt"                          , m_mon_muon2Pt              , AutoClear);            // OK
  declareMonitoredStdContainer("Muon2Z0"                          , m_mon_muon2Z0              , AutoClear);            // OK
  declareMonitoredStdContainer("Muon2A0"                          , m_mon_muon2A0              , AutoClear);            // OK

// graphs with B+/- -> K+/- mu+ mu-
  // before/no vertexing
  declareMonitoredStdContainer("BMuMuK_InvMass_B"                 , m_mon_BMuMuK_InvMass_B     , AutoClear);            // OK = invariant mass of K+&Mu&Mu
  declareMonitoredStdContainer("BMuMuK_PtB_K"                     , m_mon_BMuMuK_PtB_K         , AutoClear);            // OK = pt of K+ tracks
  declareMonitoredStdContainer("BMuMuK_EtaB_K"                    , m_mon_BMuMuK_EtaB_K        , AutoClear);            // OK = eta of K+ tracks
  declareMonitoredStdContainer("BMuMuK_PhiB_K"                    , m_mon_BMuMuK_PhiB_K        , AutoClear);            // OK = phi of K+ tracks
  // after vertexing
  declareMonitoredStdContainer("BMuMuK_VtxMass_B"                 , m_mon_BMuMuK_VtxMass_B     , AutoClear);            // OK = vertex mass of K&Mu&Mu
  declareMonitoredStdContainer("BMuMuK_Chi2toNDoF_B"              , m_mon_BMuMuK_Chi2toNDoF_B  , AutoClear);            // OK = chi2/ndof of B vertex
  declareMonitoredStdContainer("BMuMuK_VtxPtB_K"                  , m_mon_BMuMuK_VtxPtB_K      , AutoClear);            // OK = pt of K tracks after vertexing
  declareMonitoredStdContainer("BMuMuK_VtxEtaB_K"                 , m_mon_BMuMuK_VtxEtaB_K     , AutoClear);            // OK = eta of K tracks after vertexing
  declareMonitoredStdContainer("BMuMuK_VtxPhiB_K"                 , m_mon_BMuMuK_VtxPhiB_K     , AutoClear);            // OK = phi of K tracks after vertexing

        
// graphs with Bs -> Phi mu+ mu-
  // before/no vertexing 
  declareMonitoredStdContainer("BsMuMuPhi_InvMassBs_Phi"            , m_mon_BsMuMuPhi_InvMassBs_Phi   , AutoClear);            // OK = invariant mass of Phi
  declareMonitoredStdContainer("BsMuMuPhi_InvMass_Bs"               , m_mon_BsMuMuPhi_InvMass_Bs     , AutoClear);            // OK = invariant mass of Phi&Mu&Mu
  declareMonitoredStdContainer("BsMuMuPhi_PtPhi_Kp"                 , m_mon_BsMuMuPhi_PtPhi_Kp        , AutoClear);            // OK = pt of kstar's daughter tracks
  declareMonitoredStdContainer("BsMuMuPhi_EtaPhi_Kp"                , m_mon_BsMuMuPhi_EtaPhi_Kp       , AutoClear);            // OK = eta of kstar's daughter tracks
  declareMonitoredStdContainer("BsMuMuPhi_PhiPhi_Kp"                , m_mon_BsMuMuPhi_PhiPhi_Kp       , AutoClear);            // OK = phi of kstar's daughter tracks
  declareMonitoredStdContainer("BsMuMuPhi_PtPhi_Km"                 , m_mon_BsMuMuPhi_PtPhi_Km        , AutoClear);            // OK = pt of kstar's daughter tracks
  declareMonitoredStdContainer("BsMuMuPhi_EtaPhi_Km"                , m_mon_BsMuMuPhi_EtaPhi_Km       , AutoClear);            // OK = eta of kstar's daughter tracks
  declareMonitoredStdContainer("BsMuMuPhi_PhiPhi_Km"                , m_mon_BsMuMuPhi_PhiPhi_Km       , AutoClear);            // OK = phi of kstar's daughter tracks
  // after K* vertexing
  declareMonitoredStdContainer("BsMuMuPhi_VtxMassBs_Phi"            , m_mon_BsMuMuPhi_VtxMassBs_Phi   , AutoClear);            // OK = vertex mass of kaon&kaon
  declareMonitoredStdContainer("BsMuMuPhi_Chi2toNDoFBs_Phi"         , m_mon_BsMuMuPhi_Chi2toNDoFBs_Phi, AutoClear);            // OK = chi2/ndof of Phi vertex
  declareMonitoredStdContainer("BsMuMuPhi_VtxPtPhi_Kp"              , m_mon_BsMuMuPhi_VtxPtPhi_Kp     , AutoClear);            // OK = pT of phi's daughter tracks after vertex
  declareMonitoredStdContainer("BsMuMuPhi_VtxEtaPhi_Kp"             , m_mon_BsMuMuPhi_VtxEtaPhi_Kp    , AutoClear);            // OK = eta of phi's daughter tracks after vertex
  declareMonitoredStdContainer("BsMuMuPhi_VtxPhiPhi_Kp"             , m_mon_BsMuMuPhi_VtxPhiPhi_Kp    , AutoClear);            // OK = phi of phi's daughter tracks after vertex
  declareMonitoredStdContainer("BsMuMuPhi_VtxPtPhi_Km"              , m_mon_BsMuMuPhi_VtxPtPhi_Km     , AutoClear);            // OK = pT of phi's daughter tracks after vertex
  declareMonitoredStdContainer("BsMuMuPhi_VtxEtaPhi_Km"             , m_mon_BsMuMuPhi_VtxEtaPhi_Km    , AutoClear);            // OK = eta of phi's daughter tracks after vertex
  declareMonitoredStdContainer("BsMuMuPhi_VtxPhiPhi_Km"             , m_mon_BsMuMuPhi_VtxPhiPhi_Km    , AutoClear);            // OK = phi of phi's daughter tracks after vertex
  // after Bd vertexing
  declareMonitoredStdContainer("BsMuMuPhi_VtxMass_Bs"              , m_mon_BsMuMuPhi_VtxMass_Bs     , AutoClear);                // OK = vertex mass of phi&Mu&Mu
  declareMonitoredStdContainer("BsMuMuPhi_Chi2toNDoF_Bs"           , m_mon_BsMuMuPhi_Chi2toNDoF_Bs  , AutoClear);            // OK = chi2/ndof of Bs vertex
  declareMonitoredStdContainer("BsMuMuPhi_VtxPtBs_Kp"              , m_mon_BsMuMuPhi_VtxPtBs_Kp     , AutoClear);            // OK = pT of phi's daughter tracks after vertex
  declareMonitoredStdContainer("BsMuMuPhi_VtxEtaBs_Kp"             , m_mon_BsMuMuPhi_VtxEtaBs_Kp    , AutoClear);            // OK = eta of phi's daughter tracks after vertex
  declareMonitoredStdContainer("BsMuMuPhi_VtxPhiBs_Kp"             , m_mon_BsMuMuPhi_VtxPhiBs_Kp    , AutoClear);            // OK = phi of phi's daughter tracks after vertex
  declareMonitoredStdContainer("BsMuMuPhi_VtxPtBs_Km"              , m_mon_BsMuMuPhi_VtxPtBs_Km     , AutoClear);            // OK = pT of phi's daughter tracks after vertex
  declareMonitoredStdContainer("BsMuMuPhi_VtxEtaBs_Km"             , m_mon_BsMuMuPhi_VtxEtaBs_Km    , AutoClear);            // OK = eta of phi's daughter tracks after vertex
  declareMonitoredStdContainer("BsMuMuPhi_VtxPhiBs_Km"             , m_mon_BsMuMuPhi_VtxPhiBs_Km    , AutoClear);            // OK = phi of phi's daughter tracks after vertex
// graphs with B0 -> K* mu+ mu-
  // before/no vertexing 
  declareMonitoredStdContainer("BdMuMuKs_InvMassBd_Ks"            , m_mon_BdMuMuKs_InvMassBd_Ks   , AutoClear);            // OK = invariant mass of Kstar
  declareMonitoredStdContainer("BdMuMuKs_InvMass_Bd"              , m_mon_BdMuMuKs_InvMass_Bd     , AutoClear);            // OK = invariant mass of Kstar&Mu&Mu
  declareMonitoredStdContainer("BdMuMuKs_PtKs_Ka"                 , m_mon_BdMuMuKs_PtKs_Ka        , AutoClear);            // OK = pt of kstar's daughter tracks
  declareMonitoredStdContainer("BdMuMuKs_EtaKs_Ka"                , m_mon_BdMuMuKs_EtaKs_Ka       , AutoClear);            // OK = eta of kstar's daughter tracks
  declareMonitoredStdContainer("BdMuMuKs_PhiKs_Ka"                , m_mon_BdMuMuKs_PhiKs_Ka       , AutoClear);            // OK = phi of kstar's daughter tracks
  declareMonitoredStdContainer("BdMuMuKs_PtKs_Pi"                 , m_mon_BdMuMuKs_PtKs_Pi        , AutoClear);            // OK = pt of kstar's daughter tracks
  declareMonitoredStdContainer("BdMuMuKs_EtaKs_Pi"                , m_mon_BdMuMuKs_EtaKs_Pi       , AutoClear);            // OK = eta of kstar's daughter tracks
  declareMonitoredStdContainer("BdMuMuKs_PhiKs_Pi"                , m_mon_BdMuMuKs_PhiKs_Pi       , AutoClear);            // OK = phi of kstar's daughter tracks
  // after K* vertexing
  declareMonitoredStdContainer("BdMuMuKs_VtxMassBd_Ks"            , m_mon_BdMuMuKs_VtxMassBd_Ks   , AutoClear);            // OK = vertex mass of kaon&pion
  declareMonitoredStdContainer("BdMuMuKs_Chi2toNDoFBd_Ks"         , m_mon_BdMuMuKs_Chi2toNDoFBd_Ks, AutoClear);            // OK = chi2/ndof of K* vertex
  declareMonitoredStdContainer("BdMuMuKs_VtxPtKs_Ka"              , m_mon_BdMuMuKs_VtxPtKs_Ka     , AutoClear);            // OK = pT of kstar's daughter tracks after vertex
  declareMonitoredStdContainer("BdMuMuKs_VtxEtaKs_Ka"             , m_mon_BdMuMuKs_VtxEtaKs_Ka    , AutoClear);            // OK = eta of kstar's daughter tracks after vertex
  declareMonitoredStdContainer("BdMuMuKs_VtxPhiKs_Ka"             , m_mon_BdMuMuKs_VtxPhiKs_Ka    , AutoClear);            // OK = phi of kstar's daughter tracks after vertex
  declareMonitoredStdContainer("BdMuMuKs_VtxPtKs_Pi"              , m_mon_BdMuMuKs_VtxPtKs_Pi     , AutoClear);            // OK = pT of kstar's daughter tracks after vertex
  declareMonitoredStdContainer("BdMuMuKs_VtxEtaKs_Pi"             , m_mon_BdMuMuKs_VtxEtaKs_Pi    , AutoClear);            // OK = eta of kstar's daughter tracks after vertex
  declareMonitoredStdContainer("BdMuMuKs_VtxPhiKs_Pi"             , m_mon_BdMuMuKs_VtxPhiKs_Pi    , AutoClear);            // OK = phi of kstar's daughter tracks after vertex
  // after Bd vertexing
  declareMonitoredStdContainer("BdMuMuKs_VtxMass_Bd"              , m_mon_BdMuMuKs_VtxMass_Bd     , AutoClear);                // OK = vertex mass of Kstar&Mu&Mu
  declareMonitoredStdContainer("BdMuMuKs_Chi2toNDoF_Bd"           , m_mon_BdMuMuKs_Chi2toNDoF_Bd  , AutoClear);            // OK = chi2/ndof of B0+/- vertex
  declareMonitoredStdContainer("BdMuMuKs_VtxPtBd_Ka"              , m_mon_BdMuMuKs_VtxPtBd_Ka     , AutoClear);            // OK = pT of kstar's daughter tracks after vertex
  declareMonitoredStdContainer("BdMuMuKs_VtxEtaBd_Ka"             , m_mon_BdMuMuKs_VtxEtaBd_Ka    , AutoClear);            // OK = eta of kstar's daughter tracks after vertex
  declareMonitoredStdContainer("BdMuMuKs_VtxPhiBd_Ka"             , m_mon_BdMuMuKs_VtxPhiBd_Ka    , AutoClear);            // OK = phi of kstar's daughter tracks after vertex
  declareMonitoredStdContainer("BdMuMuKs_VtxPtBd_Pi"              , m_mon_BdMuMuKs_VtxPtBd_Pi     , AutoClear);            // OK = pT of kstar's daughter tracks after vertex
  declareMonitoredStdContainer("BdMuMuKs_VtxEtaBd_Pi"             , m_mon_BdMuMuKs_VtxEtaBd_Pi    , AutoClear);            // OK = eta of kstar's daughter tracks after vertex
  declareMonitoredStdContainer("BdMuMuKs_VtxPhiBd_Pi"             , m_mon_BdMuMuKs_VtxPhiBd_Pi    , AutoClear);            // OK = phi of kstar's daughter tracks after vertex
// graphs with Lb -> L mu+ mu-
  declareMonitoredStdContainer("LbMuMuL_InvMassLb_L"            , m_mon_LbMuMuL_InvMassLb_L   , AutoClear);            // OK = invariant mass of Ltar
  declareMonitoredStdContainer("LbMuMuL_InvMass_Lb"             , m_mon_LbMuMuL_InvMass_Lb    , AutoClear);            // OK = invariant mass of Ltar&Mu&Mu
  declareMonitoredStdContainer("LbMuMuL_PtL_Pr"                 , m_mon_LbMuMuL_PtL_Pr        , AutoClear);            // OK = pt of kstar's daughter tracks
  declareMonitoredStdContainer("LbMuMuL_EtaL_Pr"                , m_mon_LbMuMuL_EtaL_Pr       , AutoClear);            // OK = eta of kstar's daughter tracks
  declareMonitoredStdContainer("LbMuMuL_PhiL_Pr"                , m_mon_LbMuMuL_PhiL_Pr       , AutoClear);            // OK = phi of kstar's daughter tracks
  declareMonitoredStdContainer("LbMuMuL_PtL_Pi"                 , m_mon_LbMuMuL_PtL_Pi        , AutoClear);            // OK = pt of kstar's daughter tracks
  declareMonitoredStdContainer("LbMuMuL_EtaL_Pi"                , m_mon_LbMuMuL_EtaL_Pi       , AutoClear);            // OK = eta of kstar's daughter tracks
  declareMonitoredStdContainer("LbMuMuL_PhiL_Pi"                , m_mon_LbMuMuL_PhiL_Pi       , AutoClear);            // OK = phi of kstar's daughter tracks
  // after K* vertexing
  declareMonitoredStdContainer("LbMuMuL_VtxMassLb_L"            , m_mon_LbMuMuL_VtxMassLb_L   , AutoClear);            // OK = vertex mass of kaon&pion
  declareMonitoredStdContainer("LbMuMuL_Chi2toNDoFLb_L"         , m_mon_LbMuMuL_Chi2toNDoFLb_L, AutoClear);            // OK = chi2/ndof of K* vertex
  declareMonitoredStdContainer("LbMuMuL_VtxPtL_Pr"              , m_mon_LbMuMuL_VtxPtL_Pr     , AutoClear);            // OK = pT of kstar's daughter tracks after vertex
  declareMonitoredStdContainer("LbMuMuL_VtxEtaL_Pr"             , m_mon_LbMuMuL_VtxEtaL_Pr    , AutoClear);            // OK = eta of kstar's daughter tracks after vertex
  declareMonitoredStdContainer("LbMuMuL_VtxPhiL_Pr"             , m_mon_LbMuMuL_VtxPhiL_Pr    , AutoClear);            // OK = phi of kstar's daughter tracks after vertex
  declareMonitoredStdContainer("LbMuMuL_VtxPtL_Pi"              , m_mon_LbMuMuL_VtxPtL_Pi     , AutoClear);            // OK = pT of kstar's daughter tracks after vertex
  declareMonitoredStdContainer("LbMuMuL_VtxEtaL_Pi"             , m_mon_LbMuMuL_VtxEtaL_Pi    , AutoClear);            // OK = eta of kstar's daughter tracks after vertex
  declareMonitoredStdContainer("LbMuMuL_VtxPhiL_Pi"             , m_mon_LbMuMuL_VtxPhiL_Pi    , AutoClear);            // OK = phi of kstar's daughter tracks after vertex
  // after Lb vertexing
  declareMonitoredStdContainer("LbMuMuL_VtxMass_Lb"              , m_mon_LbMuMuL_VtxMass_Lb     , AutoClear);                // OK = vertex mass of Ltar&Mu&Mu
  declareMonitoredStdContainer("LbMuMuL_Chi2toNDoF_Lb"           , m_mon_LbMuMuL_Chi2toNDoF_Lb  , AutoClear);            // OK = chi2/ndof of B0+/- vertex
  declareMonitoredStdContainer("LbMuMuL_VtxPtLb_Pr"              , m_mon_LbMuMuL_VtxPtLb_Pr     , AutoClear);            // OK = pT of kstar's daughter tracks after vertex
  declareMonitoredStdContainer("LbMuMuL_VtxEtaLb_Pr"             , m_mon_LbMuMuL_VtxEtaLb_Pr    , AutoClear);            // OK = eta of kstar's daughter tracks after vertex
  declareMonitoredStdContainer("LbMuMuL_VtxPhiLb_Pr"             , m_mon_LbMuMuL_VtxPhiLb_Pr    , AutoClear);            // OK = phi of kstar's daughter tracks after vertex
  declareMonitoredStdContainer("LbMuMuL_VtxPtLb_Pi"              , m_mon_LbMuMuL_VtxPtLb_Pi     , AutoClear);            // OK = pT of kstar's daughter tracks after vertex
  declareMonitoredStdContainer("LbMuMuL_VtxEtaLb_Pi"             , m_mon_LbMuMuL_VtxEtaLb_Pi    , AutoClear);            // OK = eta of kstar's daughter tracks after vertex
  declareMonitoredStdContainer("LbMuMuL_VtxPhiLb_Pi"             , m_mon_LbMuMuL_VtxPhiLb_Pi    , AutoClear);            // OK = phi of kstar's daughter tracks after vertex
        
// graphs with Bc -> Ds mu+ mu-
  // before/no vertexing 
  declareMonitoredStdContainer("BcMuMuDs_InvMassBc_Phi"            , m_mon_BcMuMuDs_InvMassBc_Phi   , AutoClear);            // OK = invariant mass of Phi
  declareMonitoredStdContainer("BcMuMuDs_InvMassBc_Ds"             , m_mon_BcMuMuDs_InvMassBc_Ds    , AutoClear);            // OK = invariant mass of Ds
  declareMonitoredStdContainer("BcMuMuDs_InvMass_Bc"               , m_mon_BcMuMuDs_InvMass_Bc      , AutoClear);            // OK = invariant mass of Ds&Mu&Mu
  declareMonitoredStdContainer("BcMuMuDs_PtPhi_Kp"                 , m_mon_BcMuMuDs_PtPhi_Kp        , AutoClear);            // OK = pt  of Phi's daughter tracks
  declareMonitoredStdContainer("BcMuMuDs_EtaPhi_Kp"                , m_mon_BcMuMuDs_EtaPhi_Kp       , AutoClear);            // OK = eta of Phi's daughter tracks
  declareMonitoredStdContainer("BcMuMuDs_PhiPhi_Kp"                , m_mon_BcMuMuDs_PhiPhi_Kp       , AutoClear);            // OK = phi of Phi's daughter tracks
  declareMonitoredStdContainer("BcMuMuDs_PtPhi_Km"                 , m_mon_BcMuMuDs_PtPhi_Km        , AutoClear);            // OK = pt  of Phi's daughter tracks
  declareMonitoredStdContainer("BcMuMuDs_EtaPhi_Km"                , m_mon_BcMuMuDs_EtaPhi_Km       , AutoClear);            // OK = eta of Phi's daughter tracks
  declareMonitoredStdContainer("BcMuMuDs_PhiPhi_Km"                , m_mon_BcMuMuDs_PhiPhi_Km       , AutoClear);            // OK = phi of Phi's daughter tracks
  declareMonitoredStdContainer("BcMuMuDs_PtDs_pi"                  , m_mon_BcMuMuDs_PtDs_pi         , AutoClear);            // OK = pt  of pion from Ds
  declareMonitoredStdContainer("BcMuMuDs_EtaDs_pi"                 , m_mon_BcMuMuDs_EtaDs_pi        , AutoClear);            // OK = eta of pion from Ds
  declareMonitoredStdContainer("BcMuMuDs_PhiDs_pi"                 , m_mon_BcMuMuDs_PhiDs_pi        , AutoClear);            // OK = phi of pion from Ds
  // after Ds vertexing
  declareMonitoredStdContainer("BcMuMuDs_VtxMassBc_Ds"             , m_mon_BcMuMuDs_VtxMassBc_Ds    , AutoClear);            // OK = vertex mass of pion&kaon&kaon
  declareMonitoredStdContainer("BcMuMuDs_Chi2toNDoFBc_Ds"          , m_mon_BcMuMuDs_Chi2toNDoFBc_Ds , AutoClear);            // OK = chi2/ndof of Ds vertex
  declareMonitoredStdContainer("BcMuMuDs_VtxPtDs_Kp"               , m_mon_BcMuMuDs_VtxPtDs_Kp      , AutoClear);            // OK = pT  of phi's daughter tracks after vertex
  declareMonitoredStdContainer("BcMuMuDs_VtxEtaDs_Kp"              , m_mon_BcMuMuDs_VtxEtaDs_Kp     , AutoClear);            // OK = eta of phi's daughter tracks after vertex
  declareMonitoredStdContainer("BcMuMuDs_VtxPhiDs_Kp"              , m_mon_BcMuMuDs_VtxPhiDs_Kp     , AutoClear);            // OK = phi of phi's daughter tracks after vertex
  declareMonitoredStdContainer("BcMuMuDs_VtxPtDs_Km"               , m_mon_BcMuMuDs_VtxPtDs_Km      , AutoClear);            // OK = pT  of phi's daughter tracks after vertex
  declareMonitoredStdContainer("BcMuMuDs_VtxEtaDs_Km"              , m_mon_BcMuMuDs_VtxEtaDs_Km     , AutoClear);            // OK = eta of phi's daughter tracks after vertex
  declareMonitoredStdContainer("BcMuMuDs_VtxPhiDs_Km"              , m_mon_BcMuMuDs_VtxPhiDs_Km     , AutoClear);            // OK = phi of phi's daughter tracks after vertex
  declareMonitoredStdContainer("BcMuMuDs_VtxPtDs_pi"               , m_mon_BcMuMuDs_VtxPtDs_pi      , AutoClear);            // OK = pT  of pion from Ds after Ds vertex
  declareMonitoredStdContainer("BcMuMuDs_VtxEtaDs_pi"              , m_mon_BcMuMuDs_VtxEtaDs_pi     , AutoClear);            // OK = eta of pion from Ds after Ds vertex
  declareMonitoredStdContainer("BcMuMuDs_VtxPhiDs_pi"              , m_mon_BcMuMuDs_VtxPhiDs_pi     , AutoClear);            // OK = phi of pion from Ds after Ds vertex
  // after Bc vertexing
  declareMonitoredStdContainer("BcMuMuDs_VtxMass_Bc"              , m_mon_BcMuMuDs_VtxMass_Bc     , AutoClear);              // OK = vertex mass of Ds&Mu&Mu
  declareMonitoredStdContainer("BcMuMuDs_Chi2toNDoF_Bc"           , m_mon_BcMuMuDs_Chi2toNDoF_Bc  , AutoClear);              // OK = chi2/ndof of Bc vertex
  declareMonitoredStdContainer("BcMuMuDs_VtxPtBc_Kp"              , m_mon_BcMuMuDs_VtxPtBc_Kp     , AutoClear);              // OK = pT of phi's daughter tracks after vertex
  declareMonitoredStdContainer("BcMuMuDs_VtxEtaBc_Kp"             , m_mon_BcMuMuDs_VtxEtaBc_Kp    , AutoClear);              // OK = eta of phi's daughter tracks after vertex
  declareMonitoredStdContainer("BcMuMuDs_VtxPhiBc_Kp"             , m_mon_BcMuMuDs_VtxPhiBc_Kp    , AutoClear);              // OK = phi of phi's daughter tracks after vertex
  declareMonitoredStdContainer("BcMuMuDs_VtxPtBc_Km"              , m_mon_BcMuMuDs_VtxPtBc_Km     , AutoClear);              // OK = pT of phi's daughter tracks after vertex
  declareMonitoredStdContainer("BcMuMuDs_VtxEtaBc_Km"             , m_mon_BcMuMuDs_VtxEtaBc_Km    , AutoClear);              // OK = eta of phi's daughter tracks after vertex
  declareMonitoredStdContainer("BcMuMuDs_VtxPhiBc_Km"             , m_mon_BcMuMuDs_VtxPhiBc_Km    , AutoClear);              // OK = phi of phi's daughter tracks after vertex
  declareMonitoredStdContainer("BcMuMuDs_VtxPtBc_pi"              , m_mon_BcMuMuDs_VtxPtBc_pi     , AutoClear);              // OK = pT  of pion from Ds after Ds vertex
  declareMonitoredStdContainer("BcMuMuDs_VtxEtaBc_pi"             , m_mon_BcMuMuDs_VtxEtaBc_pi    , AutoClear);              // OK = eta of pion from Ds after Ds vertex
  declareMonitoredStdContainer("BcMuMuDs_VtxPhiBc_pi"             , m_mon_BcMuMuDs_VtxPhiBc_pi    , AutoClear);              // OK = phi of pion from Ds after Ds vertex
// review graphs
  declareMonitoredVariable("nTriedCombinations", m_mon_nTriedCombinations );             // OK = number of track combination tried
  declareMonitoredVariable("nBphys"            , m_mon_nBphys );             // OK = number of stored bphys particle
  declareMonitoredVariable("BMuMuK_n"          , m_mon_BMuMuK_n );           // OK = number of stored B+ particle
  declareMonitoredVariable("BdMuMuKs_n"        , m_mon_BdMuMuKs_n );         // OK = number of stored Bd particle
  declareMonitoredVariable("BsMuMuPhi_n"       , m_mon_BsMuMuPhi_n );        // OK = number of stored Bs particle
  declareMonitoredVariable("LbMuMuL_n"         , m_mon_LbMuMuL_n );          // OK = number of stored Lb particle
  declareMonitoredVariable("BcMuMuDs_n"        , m_mon_BcMuMuDs_n );         // OK = number of stored Bc particle
  declareMonitoredVariable("TotalRunTime"      , m_mon_TotalRunTime );       // OK = total run time:)
  declareMonitoredVariable("VertexingTime"     , m_mon_VertexingTime );      // OK = vertexing time
////////////////////////////// end of variables for monitoring histograms //////////////////////////////////

/////////////////////////////// Zero the counters for monitoring purposes //////////////////////////////////  
//  m_countTotalEvents    = 0;         // counter for events
//  m_countTotalRoI       = 0;         // counter for RoIs = counter for events
/////////////////////////////// end of the counters for monitoring purposes ////////////////////////////////

 
////////////////////////////// Declaration of output collections ///////////////////////////////////////////
  m_trigBphysColl_b = NULL;
  m_trigBphysColl_bxAOD = NULL;
  m_VertexColl = NULL;
  // temporary
  m_trigBphysColl_kStar = NULL;
  m_trigBphysColl_phi = NULL;
  m_trigBphysColl_lambda = NULL;
  m_trigBphysColl_ds = NULL;
  m_trigBphysColl_kStarxAOD = NULL;
  m_trigBphysColl_phixAOD = NULL;
  m_trigBphysColl_lambdaxAOD = NULL;
  m_trigBphysColl_dsxAOD = NULL;
//  m_trigBphysColl_kSplus = NULL;

  //////////////////////////// end of output collections ///////////////////////////////////////////////////
}
/*----------------------------------------*/
TrigL2BMuMuXFex::~TrigL2BMuMuXFex()                                            
{ 
  if(m_trigBphysColl_b!=NULL) delete m_trigBphysColl_b; 
  if(m_VertexColl!=NULL) delete m_VertexColl;
  if(m_trigBphysColl_kStar) delete m_trigBphysColl_kStar;
  if(m_trigBphysColl_phi) delete m_trigBphysColl_phi;
  if(m_trigBphysColl_lambda) delete m_trigBphysColl_lambda;
  if(m_trigBphysColl_ds) delete m_trigBphysColl_ds;
//  if(m_trigBphysColl_kSplus) delete m_trigBphysColl_kSplus;
 
}
/*-----------------------------------------------*/
HLT::ErrorCode TrigL2BMuMuXFex::hltInitialize()                                 
{
 if(msgLvl() <= MSG::DEBUG)msg() << MSG::DEBUG << "In initialize()" << endmsg;
//         only for performance test - obsolete now
  if(msgLvl() <= MSG::INFO){
  msg() << MSG::INFO << "-------------------INFO TrigL2BMuMuXFex---------------------" << endmsg;
  msg() << MSG::INFO << "Initialization ..." << endmsg;
  msg() << MSG::INFO << "OppositeCharge check        = " 
  << (m_oppositeCharge==true ? "True" : "False") << endmsg; 
//   msg() << MSG::INFO << "InDetAlgo                   = " << m_inDetAlgo << endmsg;    
//   msg() << MSG::INFO << "UseAllAlgo                  = "                              
//   << (m_useAllAlgo==true ? "True" : "False") << endmsg;                          
  msg() << MSG::INFO << "LowerMuMuMassCut            = " << m_lowerMuMuMassCut << endmsg; 
  msg() << MSG::INFO << "UpperMuMuMassCut            = " << m_upperMuMuMassCut << endmsg;
  msg() << MSG::INFO << "DoVertexFit                 = "
  << (m_doVertexFit==true ? "True" : "False") << endmsg;                     
  msg() << MSG::INFO << "DoMuMuVertexFit             = "
  << (m_doMuMuVertexFit==true ? "True" : "False") << endmsg;                     
  msg() << MSG::INFO << "LowerMuVtxMassCut           = " << m_lowerMuVtxMassCut << endmsg;   
  msg() << MSG::INFO << "UpperMuVtxMassCut           = " << m_upperMuVtxMassCut << endmsg;   
  msg() << MSG::INFO << "MuVtxChi2Cut                = " << m_muVtxChi2Cut << endmsg;    
  msg() << MSG::INFO << "MaxNcombinations            = " << m_maxNcombinations << endmsg;    
  msg() << MSG::INFO << " Information for B^{+/-} -> K^{+/-} Mu^{+} Mu^{-} part! " << endmsg;      
  msg() << MSG::INFO << "Look for B+ -> K+MuMu       = "
   << (m_doB_KMuMuDecay == true ? "TRUE" : "FALSE") << endmsg;
  //msg() << MSG::INFO << "LowerKaonPtCut  = " << m_lowerKaonPtCut << endmsg;
  //msg() << MSG::INFO << "UpperKaonPtCut  = " << m_upperKaonPtCut << endmsg;
  msg() << MSG::INFO << "LowerKMuMuMassCut           = " << m_lowerKMuMuMassCut << endmsg;
  msg() << MSG::INFO << "UpperKMuMuMassCut           = " << m_upperKMuMuMassCut << endmsg;
  msg() << MSG::INFO << "DoKMuMuVertexFit            = "
   << (m_doB_KMuMuVertexFit==true ? "True" : "False") << endmsg;               
  msg() << MSG::INFO << "LowerBVtxMassCut            = " << m_lowerBVtxMassCut << endmsg;
  msg() << MSG::INFO << "UpperBVtxMassCut            = " << m_upperBVtxMassCut << endmsg;
  msg() << MSG::INFO << "BVtxChi2Cut                 = " << m_bVtxChi2Cut << endmsg;
  msg() << MSG::INFO << " Information for B^{d} -> K^{*} Mu^{+} Mu^{-} part! " << endmsg;
  msg() << MSG::INFO << "Look for Bd -> K*MuMu       = "
   << (m_doBd_KstarMuMuDecay == true ? "TRUE" : "FALSE") << endmsg;
  msg() << MSG::INFO << "LowerKstar_KaonMassCut      = " << m_lowerKstar_KaonMassCut << endmsg;   
  msg() << MSG::INFO << "UpperKstar_KaonMassCut      = " << m_upperKstar_KaonMassCut << endmsg;   
  msg() << MSG::INFO << "LowerBd_KstarMuMuMassCut    = " << m_lowerBd_KstarMuMuMassCut << endmsg;
  msg() << MSG::INFO << "UpperBd_KstarMuMuMassCut    = " << m_upperBd_KstarMuMuMassCut << endmsg;
  msg() << MSG::INFO << "DoKstar_KaonPionVertexFit   = "
   << (m_doKstar_KaonPionVertexFit==true ? "True" : "False") << endmsg;               
  msg() << MSG::INFO << "LowerKstarVtxMassCut        = " << m_lowerKstarVtxMassCut << endmsg;
  msg() << MSG::INFO << "UpperKstarVtxMassCut        = " << m_upperKstarVtxMassCut << endmsg;
  msg() << MSG::INFO << "KstarVtxChi2Cut                 = " << m_kStarVtxChi2Cut << endmsg;   
  msg() << MSG::INFO << "DoBd_KstarMuMuVertexFit     = "
   << (m_doBd_KstarMuMuVertexFit==true ? "True" : "False") << endmsg;               
  msg() << MSG::INFO << "LowerBdVtxMassCut           = " << m_lowerBdVtxMassCut << endmsg;
  msg() << MSG::INFO << "UpperBdVtxMassCut           = " << m_upperBdVtxMassCut << endmsg;
  msg() << MSG::INFO << "BdVtxChi2Cut                = " << m_bDVtxChi2Cut << endmsg;
  msg() << MSG::INFO << " Information for B^{s} -> Phi Mu^{+} Mu^{-} part! " << endmsg;
  msg() << MSG::INFO << "Look for Bs -> PhiMuMu      = "
   << (m_doBs_Phi1020MuMuDecay == true ? "TRUE" : "FALSE" )<< endmsg;
  msg() << MSG::INFO << "LowerPhi1020_KaonMassCut    = " << m_lowerPhi1020_KaonMassCut << endmsg;   
  msg() << MSG::INFO << "UpperPhi1020_KaonMassCut    = " << m_upperPhi1020_KaonMassCut << endmsg;   
  msg() << MSG::INFO << "LowerBs_Phi1020MuMuMassCut  = " << m_lowerBs_Phi1020MuMuMassCut << endmsg;
  msg() << MSG::INFO << "UpperBs_Phi1020MuMuMassCut  = " << m_upperBs_Phi1020MuMuMassCut << endmsg;
  msg() << MSG::INFO << "DoPhi1020_KaonKaonVertexFit = "
   << (m_doPhi1020_KaonKaonVertexFit==true ? "True" : "False") << endmsg;               
  msg() << MSG::INFO << "LowerPhi1020VtxMassCut      = " << m_lowerPhi1020VtxMassCut << endmsg;
  msg() << MSG::INFO << "UpperPhi1020VtxMassCut      = " << m_upperPhi1020VtxMassCut << endmsg;
  msg() << MSG::INFO << "Phi1020VtxChi2Cut           = " << m_phi1020VtxChi2Cut << endmsg;
  msg() << MSG::INFO << "DoBs_Phi1020MuMuVertexFit   = "
   << (m_doBs_Phi1020MuMuVertexFit==true ? "True" : "False") << endmsg;               
  msg() << MSG::INFO << "LowerBsVtxMassCut           = " << m_lowerBsVtxMassCut << endmsg;
  msg() << MSG::INFO << "UpperBsVtxMassCut           = " << m_upperBsVtxMassCut << endmsg;
  msg() << MSG::INFO << "BsVtxChi2Cut                = " << m_bSVtxChi2Cut << endmsg;
  msg() << MSG::INFO << " Information for L_{b} -> Lambda Mu^{+} Mu^{-} part! " << endmsg;
  msg() << MSG::INFO << "Look for Lb -> LMuMu      = "
   << (m_doLb_LambdaMuMuDecay == true ? "TRUE" : "FALSE" )<< endmsg;
  msg() << MSG::INFO << "LowerLambda_PrPiMassCut    = " << m_lowerLambda_PrPiMassCut << endmsg;   
  msg() << MSG::INFO << "UpperLambda_PrPiMassCut    = " << m_upperLambda_PrPiMassCut << endmsg;   
  msg() << MSG::INFO << "LowerLb_LambdaMuMuMassCut  = " << m_lowerLb_LambdaMuMuMassCut << endmsg;
  msg() << MSG::INFO << "UpperLb_LambdaMuMuMassCut  = " << m_upperLb_LambdaMuMuMassCut << endmsg;
  msg() << MSG::INFO << "DoLambda_ProtonPionVertexFit = "
   << (m_doLambda_ProtonPionVertexFit==true ? "True" : "False") << endmsg;               
  msg() << MSG::INFO << "LowerLambdaVtxMassCut      = " << m_lowerLambdaVtxMassCut << endmsg;
  msg() << MSG::INFO << "UpperLambdaVtxMassCut      = " << m_upperLambdaVtxMassCut << endmsg;
  msg() << MSG::INFO << "LambdaVtxChi2Cut           = " << m_lambdaVtxChi2Cut << endmsg;
  msg() << MSG::INFO << "DoLb_LambdaMuMuVertexFit   = "
   << (m_doLb_LambdaMuMuVertexFit==true ? "True" : "False") << endmsg;               
  msg() << MSG::INFO << "LowerLbVtxMassCut           = " << m_lowerLbVtxMassCut << endmsg;
  msg() << MSG::INFO << "UpperLbVtxMassCut           = " << m_upperLbVtxMassCut << endmsg;
//   msg() << MSG::INFO << "LbVtxChi2Cut                = " << m_lBVtxChi2Cut << endmsg;  
//   msg() << MSG::INFO << "LbVtxDistanceCut            = " << m_lBVtxDistanceCut << endmsg;  
//   msg() << MSG::INFO << "PrImpactCut                = " << m_prImpactCut << endmsg;  
//   msg() << MSG::INFO << "PiImpactCut                = " << m_piImpactCut << endmsg;  
  msg() << MSG::INFO << " Information for B_{c} -> D_s Mu^{+} Mu^{-} part! " << endmsg;
  msg() << MSG::INFO << "Look for Bc -> DsMuMu      = "
   << (m_doBc_DsMuMuDecay == true ? "TRUE" : "FALSE" )<< endmsg;
  msg() << MSG::INFO << "LowerPhiDs_MassCut    = " << m_lowerPhiDs_MassCut << endmsg;   
  msg() << MSG::INFO << "UpperPhiDs_MassCut    = " << m_upperPhiDs_MassCut << endmsg;   
  msg() << MSG::INFO << "LowerDs_MassCut    = " << m_lowerDs_MassCut << endmsg;   
  msg() << MSG::INFO << "UpperDs_MassCut    = " << m_upperDs_MassCut << endmsg;   
  msg() << MSG::INFO << "LowerBc_DsMuMuMassCut  = " << m_lowerBc_DsMuMuMassCut << endmsg;
  msg() << MSG::INFO << "UpperBc_DsMuMuMassCut  = " << m_upperBc_DsMuMuMassCut << endmsg;
  msg() << MSG::INFO << "DoDs_PhiPionVertexFit = "
   << (m_doDs_PhiPionVertexFit==true ? "True" : "False") << endmsg;               
  msg() << MSG::INFO << "LowerDsVtxMassCut      = " << m_lowerDsVtxMassCut << endmsg;
  msg() << MSG::INFO << "UpperDsVtxMassCut      = " << m_upperDsVtxMassCut << endmsg;
  msg() << MSG::INFO << "DsVtxChi2Cut           = " << m_DsVtxChi2Cut << endmsg;
  msg() << MSG::INFO << "DoBc_DsMuMuVertexFit   = "
   << (m_doBc_DsMuMuVertexFit==true ? "True" : "False") << endmsg;               
  msg() << MSG::INFO << "LowerBcVtxMassCut           = " << m_lowerBcVtxMassCut << endmsg;
  msg() << MSG::INFO << "UpperBcVtxMassCut           = " << m_upperBcVtxMassCut << endmsg;
  msg() << MSG::INFO << "BcVtxChi2Cut                = " << m_bCVtxChi2Cut << endmsg;
  msg() << MSG::INFO << "-----------------------------------------------------" << endmsg;
  }
                                              
  
  // retrieving the vertex fitting tool                                          
  StatusCode sc = toolSvc()->retrieveTool("TrigL2VertexFitter","TrigL2VertexFitter",m_L2vertFitter);
  if ( sc.isFailure() ) {
    msg() << MSG::FATAL << "Unable to locate TrigL2VertexFitter tool " << endmsg;
    return HLT::BAD_JOB_SETUP;
  }
  sc = toolSvc()->retrieveTool("TrigVertexingTool","TrigVertexingTool",m_vertexingTool);
  if ( sc.isFailure() ) {
    msg() << MSG::FATAL << "Unable to locate TrigVertexingTool tool " << endmsg;
    return HLT::BAD_JOB_SETUP;
  }
    
    if (m_bphysHelperTool.retrieve().isFailure()) {
        msg() << MSG::ERROR << "Can't find TrigBphysHelperUtilsTool" << endmsg;
        return HLT::BAD_JOB_SETUP;
    } else {
        if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "TrigBphysHelperUtilsTool found" << endmsg;
    }

/////////////////////////// end of retrive the vertexing tools//////////////////


/////////////////////////// add timers /////////////////////////////////////////
  if(timerSvc()){
    m_TotTimer = addTimer("L2BMuMuXFexTot");   
    m_VtxFitTimer = addTimer("L2BMuMuXFexVFit");                                  
  }
///////////////////////////// end of add timers ////////////////////////////////
  
// /////////////////////////////// choose tracking algo ///////////////////////////
//   std::string inDetAlgo = m_inDetAlgo;
// 
//   if(inDetAlgo=="IDSCAN")        m_inDetAlgoId = TrigInDetTrack::IDSCANID;
//   else if(inDetAlgo=="SITRACK")  m_inDetAlgoId = TrigInDetTrack::SITRACKID;
//   else                           m_inDetAlgoId = TrigInDetTrack::IDSCANID;
// ///////////////////////////// end of choose tracking algo //////////////////////
  
  return HLT::OK;

} ///////// end of HLT - Errorcode - TrigL2BMuMuXFex - hltInitialize////////////
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
HLT::ErrorCode TrigL2BMuMuXFex::hltFinalize()
{

  if(msgLvl() <= MSG::DEBUG)msg() << MSG::DEBUG << "In finalize()" << endmsg;

///////////////////////// Declaration of monitoring histos /////////////////////
  // Define the bins for error-monitoring histogram
  #define ERROR_No_EventInfo           0
  #define ERROR_No_RoIs                1
  #define ERROR_No_CombineMuon         2
  #define ERROR_No_TrackColl           3
  // more steps below                    
  #define ERROR_Charge_Check           4
  #define ERROR_MuMu_MassCut           5
  // Separator                         
  #define ERROR_AddMuonTrack_Fails     6
  #define ERROR_MuonVtxFit_Fails       7
  #define ERROR_CalcMuonVtxMass_Fails  8
  // Separator                        
  #define ERROR_MuonVtx_MassCut        9
  #define ERROR_AddTrack_Fails        10
  #define ERROR_VtxFitFailed          11
  #define ERROR_MotherVtxFitFailed    12
  // Separator                        
  #define ERROR_TooManyComb_Acc       13
  #define ERROR_TooManyComb_Rej       14

/*   // more avalaible steps below               
  #define ERROR_Unique_AddTrack_Fails  6
  #define ERROR_CalcInvMass_Fails      7
  #define ERROR_CalcMother_Fails       8
  #define ERROR_CalcMassPull_Fails     9
  #define ERROR_BphysColl_Fails       10
*/
  // Define the bins for acceptance-monitoring histogram
  #define ACCEPT_Input                 0
  #define ACCEPT_AcceptAll             1
  #define ACCEPT_Got_RoIs              2
  #define ACCEPT_Got_Muons             3
  #define ACCEPT_Got_Muon1idTrack      4
  #define ACCEPT_Got_Muon2idTrack      5
  #define ACCEPT_Got_TrackColl         6
  #define ACCEPT_First_TrackColl       7
  #define ACCEPT_Both_TrackColls       8
  #define ACCEPT_Full_IDTracks         9
  #define ACCEPT_Pass_OppChargeC      10
  #define ACCEPT_MuonTracks_Added     11
  #define ACCEPT_Muon_Vertexing       12
  #define ACCEPT_CalcInvMass          13
  #define ACCEPT_MuonVtx_Part         14
  #define ACCEPT_MuMu_Mass            15
  #define ACCEPT_MotherVtxCreated     16
  #define ACCEPT_BphysCollParticle    17
  
/*  // more avalaible steps below               
  #define ACCEPT_BphysColl_not_Empty  24
  #define ACCEPT_Each_Vertex_not_NULL 42
  */
///////////////////////// end of declaration of monitoring histos //////////////

  return HLT::OK;
  
} /////////////end of HLT - ErrorCode - TrigL2BMuMuXFex - hltFinalize///////////
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
HLT::ErrorCode TrigL2BMuMuXFex::acceptInputs(HLT::TEConstVec& inputTEs, bool& pass)
{
  //starts timer
  if(timerSvc()) 
    m_TotTimer->start();

  if(inputTEs.size() != 2) {
    msg() << MSG::WARNING << "Wrong number of input trigger elements ("<<inputTEs.size()<<")."<< endmsg;
    pass = false;
    return HLT::OK;
  }else{
  if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "Right number of input trigger elements ("<<inputTEs.size()<<")."<< endmsg;
  }
  pass = true;
  
  return HLT::OK;
  
} /////////end of HLT - ErrorCode - TrigL2BMuMuXFex - acceptInputs//////////////

/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
HLT::ErrorCode TrigL2BMuMuXFex::hltExecute(HLT::TEConstVec& inputTEs, HLT::TriggerElement* outputTE)
{
  if(msgLvl() <= MSG::DEBUG)msg() << MSG::DEBUG << "In execute()" << endmsg;
//////////////// Initialization of the monitoring variables ////////////////////
//  m_mon_nTracks = -1;
  m_mon_nTriedCombinations = 0;
  m_mon_nBphys        = 0;
  m_mon_BMuMuK_n      = 0;
  m_mon_BdMuMuKs_n    = 0;
  m_mon_BsMuMuPhi_n   = 0;
  m_mon_LbMuMuL_n     = 0;
  m_mon_BcMuMuDs_n    = 0;
  m_mon_TotalRunTime  = 0;
  m_mon_VertexingTime = 0;
  
//////////////// end of initialization of the monitoring variables /////////////

  ////list of muon candidates: RoI & Combined Muon Feature & Kaon candidates////
    //std::vector<const CombinedMuonFeature*> muonCandidates;
    std::vector<const xAOD::L2CombinedMuon*> muonCandidates;


  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " After acceptInput" << endmsg;
  m_mon_Acceptance.push_back( ACCEPT_Input );

    // Retrieve event info
    int IdRun   = 0;
    int IdEvent = 0;
    
    uint32_t runNumber(0), evtNumber(0), lbBlock(0);
    if (m_bphysHelperTool->getRunEvtLb( runNumber, evtNumber, lbBlock).isFailure()) {
        msg() << MSG::ERROR << "Error retriving EventInfo" << endmsg;
    }
    IdRun   = runNumber;
    IdEvent = evtNumber;
    
    //event
  // Accept-All mode: temporary patch; should be done with force-accept
  if(m_acceptAll){
    if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "AcceptAll property is set: taking all events" << endmsg;
    // pass = true;
    m_mon_Acceptance.push_back( ACCEPT_AcceptAll );
  }
  else{
    if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "AcceptAll property not set: applying selection" << endmsg;
    // pass = false;
  }
    
    ///////////////////////////////// RoI's ////////////////////////////////////
    const TrigRoiDescriptor* roiDescriptor1;
    const TrigRoiDescriptor* roiDescriptor2;
    if ( getFeature(inputTEs.front(), roiDescriptor1) != HLT::OK ) roiDescriptor1 = 0; 
    if(roiDescriptor1) {
        if(msgLvl() <= MSG::DEBUG) {
          msg() << MSG::DEBUG << "RoI id " << roiDescriptor1->roiId()
                << " LVL1 id " << roiDescriptor1->l1Id()
                << " located at   phi = " <<  roiDescriptor1->phi()
                << ", eta = " << roiDescriptor1->eta() 
                << endmsg;
        }
    }else{
      msg() <<  MSG::WARNING << "No RoI for this Trigger Element! " << endmsg;
      if(timerSvc()) {
        m_TotTimer->stop();
        m_mon_TotalRunTime = m_TotTimer->elapsed();
      }
      m_mon_Errors.push_back( ERROR_No_RoIs );
      return HLT::NAV_ERROR;
    }
    if ( getFeature(inputTEs.back(), roiDescriptor2) != HLT::OK ) roiDescriptor2 = 0; 
    if(roiDescriptor2) {
      if(msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "RoI id " << roiDescriptor2->roiId()
              << " LVL1 id " << roiDescriptor2->l1Id()
              << " located at   phi = " <<  roiDescriptor2->phi()
              << ", eta = " << roiDescriptor2->eta() 
              << endmsg;
      }
    }else{
      msg() <<  MSG::WARNING << "No RoI for this Trigger Element! " << endmsg;
      if(timerSvc()) {
        m_TotTimer->stop();
        m_mon_TotalRunTime = m_TotTimer->elapsed();
      }
      m_mon_Errors.push_back( ERROR_No_RoIs );
      return HLT::NAV_ERROR;
    }

    m_mon_Acceptance.push_back( ACCEPT_Got_RoIs );
    // RoI monitoring
    m_mon_ROIsEta.push_back( roiDescriptor1->eta() );
    m_mon_ROIsPhi.push_back( roiDescriptor1->phi() );
    m_mon_ROIsEta.push_back( roiDescriptor2->eta() );
    m_mon_ROIsPhi.push_back( roiDescriptor2->phi() );
    
    // Tried combinations counter - protection against timeouts
    int nTriedCombinations(0);
    
    //////////////////////////// muon candidates ///////////////////////////////
    //ElementLink<xAOD::L2CombinedMuonContainer> l2combinedMuonEL[2];
    
    typedef  ElementLinkVector<xAOD::L2CombinedMuonContainer>  ELVMuons;
    ELVMuons l2combinedMuonEL[2];

    const xAOD::L2CombinedMuon* pCombinedMuonFeature1 = NULL;
    const xAOD::L2CombinedMuon* pCombinedMuonFeature2 = NULL;

    //    HLT::ErrorCode status = getFeature(inputTEs.front(), pCombinedMuonFeature1);
    //    HLT::ErrorCode status = getFeatureLink<xAOD::L2CombinedMuonContainer,xAOD::L2CombinedMuonContainer>(inputTEs.front(),l2combinedMuonEL[0]);
    HLT::ErrorCode status = getFeaturesLinks<xAOD::L2CombinedMuonContainer,xAOD::L2CombinedMuonContainer>(inputTEs.front(),l2combinedMuonEL[0]);
    if (l2combinedMuonEL[0].size()){
        pCombinedMuonFeature1 = l2combinedMuonEL[0][0].isValid() ? *(l2combinedMuonEL[0][0]): nullptr;
    }
    if (l2combinedMuonEL[0].size()>1){
        msg() << MSG::WARNING << "Unexpected number of containers for comb feature: " << 0 << endmsg;
    }
    if(status!=HLT::OK || pCombinedMuonFeature1==NULL) {
      msg() << MSG::ERROR
            << "Unable to retrieve the CombinedMuonFeature1 from TE" << endmsg;
      if(timerSvc()) {
        m_TotTimer->stop();
        m_mon_TotalRunTime = m_TotTimer->elapsed();
      }
      m_mon_Errors.push_back( ERROR_No_CombineMuon );
      return HLT::NAV_ERROR;
    }
    //status = getFeature(inputTEs.back(), pCombinedMuonFeature2);
    //status = getFeatureLink<xAOD::L2CombinedMuonContainer,xAOD::L2CombinedMuonContainer>(inputTEs.back(),l2combinedMuonEL[1]);
    status = getFeaturesLinks<xAOD::L2CombinedMuonContainer,xAOD::L2CombinedMuonContainer>(inputTEs.back(),l2combinedMuonEL[1]);
    //    pCombinedMuonFeature2 = l2combinedMuonEL[1].isValid() ? *l2combinedMuonEL[1]: nullptr;
    if (l2combinedMuonEL[1].size()){
        pCombinedMuonFeature2 = l2combinedMuonEL[1][0].isValid() ? *(l2combinedMuonEL[1][0]): nullptr;
    }
    if (l2combinedMuonEL[1].size()>1){
        msg() << MSG::WARNING << "Unexpected number of containers for comb feature: " << 1 << endmsg;
    }

    if(status!=HLT::OK || pCombinedMuonFeature2==NULL) {
      msg() << MSG::ERROR
            << "Unable to retrieve the CombinedMuonFeature2 from TE" << endmsg;
      if(timerSvc()) {
        m_TotTimer->stop();
        m_mon_TotalRunTime = m_TotTimer->elapsed();
      }
      m_mon_Errors.push_back( ERROR_No_CombineMuon );
      return HLT::NAV_ERROR;
    }
    if(msgLvl() <= MSG::DEBUG) {
        if (pCombinedMuonFeature1->idTrack()) {
            msg() << MSG::DEBUG << "Muon candidate1: track with "
            << "pt=" << pCombinedMuonFeature1->idTrack()->pt()
            << ", eta=" << pCombinedMuonFeature1->idTrack()->eta()
            << ", phi=" << pCombinedMuonFeature1->idTrack()->phi()
            << ", q="   << pCombinedMuonFeature1->idTrack()->charge()
            << ", Z0="  << pCombinedMuonFeature1->idTrack()->z0()
            << ", d0="  << pCombinedMuonFeature1->idTrack()->d0() // JW was a0 in old code
            << endmsg;
        } else {
            msg() << MSG::DEBUG << "Muon candidate1: track with No track!" << endmsg;
        }
        if (pCombinedMuonFeature2->idTrack()) {
            msg() << MSG::DEBUG << "Muon candidate2: track with "
            << "pt=" << pCombinedMuonFeature2->idTrack()->pt()
            << ", eta=" << pCombinedMuonFeature2->idTrack()->eta()
            << ", phi=" << pCombinedMuonFeature2->idTrack()->phi()
            << ", q="   << pCombinedMuonFeature1->idTrack()->charge()
            << ", Z0=" << pCombinedMuonFeature2->idTrack()->z0()
            << ", d0=" << pCombinedMuonFeature2->idTrack()->d0()  // JW was a0 in old code
            << endmsg;
        } else {
            msg() << MSG::DEBUG << "Muon candidate2: track with No track!" << endmsg;
        }
        
    } // if debug
    
    m_mon_Acceptance.push_back( ACCEPT_Got_Muons );

    muonCandidates.push_back(pCombinedMuonFeature1);
    muonCandidates.push_back(pCombinedMuonFeature2);
    
    if ( !pCombinedMuonFeature1->idTrack()) {
        msg() << MSG::DEBUG << "Muon candidate1: no id track!" << endmsg;
        auto idlink = pCombinedMuonFeature1->idTrackLink();
        msg() << MSG::DEBUG << "Muon elementlink has "<< idlink.dataID() << " " << idlink.index()
        << " " << idlink.isValid()  << endmsg;
        if(timerSvc()) {
            m_TotTimer->stop();
            m_mon_TotalRunTime = m_TotTimer->elapsed();
        }
        //m_mon_Errors.push_back( ERROR_No_CombineMuon );
        return HLT::OK;
    } else {
        m_mon_Acceptance.push_back( ACCEPT_Got_Muon1idTrack );
    }
    
    if ( !pCombinedMuonFeature2->idTrack()) {
        msg() << MSG::DEBUG << "Muon candidate2: no id track!" << endmsg;
        auto idlink = pCombinedMuonFeature2->idTrackLink();
        msg() << MSG::DEBUG << "Muon elementlink has "<< idlink.dataID() << " " << idlink.index()
        << " " << idlink.isValid()  << endmsg;
        if(timerSvc()) {
            m_TotTimer->stop();
            m_mon_TotalRunTime = m_TotTimer->elapsed();
        }
        //m_mon_Errors.push_back( ERROR_No_CombineMuon );
        return HLT::OK;
    } else {
        m_mon_Acceptance.push_back( ACCEPT_Got_Muon2idTrack );
    }

    

    // monitoring muons
    m_mon_muon1Eta.push_back( pCombinedMuonFeature1->idTrack()->eta() );
    m_mon_muon1Phi.push_back( pCombinedMuonFeature1->idTrack()->phi() );
    m_mon_muon1Pt.push_back( pCombinedMuonFeature1->idTrack()->pt()/1000. );
    m_mon_muon1Z0.push_back( pCombinedMuonFeature1->idTrack()->z0() );
    m_mon_muon1A0.push_back( pCombinedMuonFeature1->idTrack()->d0() );
    
    m_mon_muon2Eta.push_back( pCombinedMuonFeature2->idTrack()->eta() );
    m_mon_muon2Phi.push_back( pCombinedMuonFeature2->idTrack()->phi() );
    m_mon_muon2Pt.push_back( pCombinedMuonFeature2->idTrack()->pt()/1000. );
    m_mon_muon2Z0.push_back( pCombinedMuonFeature2->idTrack()->z0() );
    m_mon_muon2A0.push_back( pCombinedMuonFeature2->idTrack()->d0() );

  //////////////////////////////////////////////////////////////////////////////                                                                        
  /////////////////////////// loop over TE's////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////    

    /*
  std::vector<const TrigInDetTrackCollection*> vectorOfTrackCollections1;
  std::vector<const TrigInDetTrackCollection*> vectorOfTrackCollections2;
  //std::vector<const TrackCollection*> vectorOfTrackCollections1;
  //std::vector<const TrackCollection*> vectorOfTrackCollections2;

  std::vector<const HLT::TriggerElement*>::const_iterator iTE = inputTEs.begin();
  ////////////////////////////// other particle candidates //////////////////////////////  
  status = getFeatures(*iTE, vectorOfTrackCollections1);
  //status = getFeatures(*iTE, vectorOfTrackCollections1,"TrigFastTrackFinder_TrigInDetTrack_Muon");
  //status = getFeatures(*iTE, vectorOfTrackCollections1);
    if (status!=HLT::OK) { msg() << MSG::ERROR << "XX1: " << endmsg;return HLT::NAV_ERROR;}
    std::vector<const TrigInDetTrackCollection*> vectorOfTrackCollections1a;
    std::vector<const TrackCollection*> vectorOfTrackCollections1b;
    std::vector<const TrackCollection*> vectorOfTrackCollections1c;
    status = getFeatures(*iTE, vectorOfTrackCollections1a,"TrigFastTrackFinder_TrigInDetTrack_Muon");
    if (status!=HLT::OK) { msg() << MSG::ERROR << "XX1a: " << endmsg;return HLT::NAV_ERROR;}
    status = getFeatures(*iTE, vectorOfTrackCollections1b);
    if (status!=HLT::OK) { msg() << MSG::ERROR << "XX1b: " << endmsg;return HLT::NAV_ERROR;}
    status = getFeatures(*iTE, vectorOfTrackCollections1c,"TrigFastTrackFinder_Muon");
    if (status!=HLT::OK) { msg() << MSG::ERROR << "XX1c: " << endmsg;return HLT::NAV_ERROR;}
    msg() << MSG::INFO << " found: "
    << vectorOfTrackCollections1.size() << " "
    << vectorOfTrackCollections1a.size() << " "
    << vectorOfTrackCollections1b.size() << " "
    << vectorOfTrackCollections1c.size() << " " << endmsg;
    
    for (auto x: vectorOfTrackCollections1 ) { if (!x) continue; msg() << MSG::INFO << " 1 : " << x->size() << endmsg;}
    for (auto x: vectorOfTrackCollections1a) { if (!x) continue; msg() << MSG::INFO << " 1a: " << x->size() << endmsg;}
    for (auto x: vectorOfTrackCollections1b) { if (!x) continue; msg() << MSG::INFO << " 1b: " << x->size() << endmsg;}
    for (auto x: vectorOfTrackCollections1c) { if (!x) continue; msg() << MSG::INFO << " 1c: " << x->size() << endmsg;}
    */
    std::vector<const xAOD::TrackParticleContainer*> vectorOfTrackCollections1;
    std::vector<const xAOD::TrackParticleContainer*> vectorOfTrackCollections2;
    std::vector<const HLT::TriggerElement*>::const_iterator iTE = inputTEs.begin();
    ////////////////////////////// other particle candidates //////////////////////////////
    status = getFeatures(*iTE, vectorOfTrackCollections1);
  if(status!=HLT::OK) {
      msg() << MSG::ERROR
           << "Unable to retrieve vector of InDetTrackCollections from TE" << endmsg;
      if(timerSvc()) {
        m_TotTimer->stop();
        m_mon_TotalRunTime = m_TotTimer->elapsed();
      }
      m_mon_Errors.push_back( ERROR_No_TrackColl );
      return HLT::NAV_ERROR;
    }
    m_mon_Acceptance.push_back( ACCEPT_Got_TrackColl );

    if(vectorOfTrackCollections1.size()!=1) {                                              
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
            << vectorOfTrackCollections1.size() << " TrackCollections in TE" << endmsg;
    }
    m_mon_Acceptance.push_back( ACCEPT_First_TrackColl );
    ++iTE; // next trigger element, no more or less than two, checked by acceptInputs!!
    //status = getFeatures(*iTE, vectorOfTrackCollections2,"TrigFastTrackFinder_TrigInDetTrack_Muon");
    status = getFeatures(*iTE, vectorOfTrackCollections2);
    //status = getFeatures(*iTE, vectorOfTrackCollections2);
    //status = getFeatures( *iTE, vectorOfTrackCollections);
    if(status!=HLT::OK) {
      msg() << MSG::ERROR
           << "Unable to retrieve vector of InDetTrackCollections from TE" << endmsg;
      if(timerSvc()) {
        m_TotTimer->stop();
        m_mon_TotalRunTime = m_TotTimer->elapsed();
      }
      m_mon_Errors.push_back( ERROR_No_TrackColl );
      return HLT::NAV_ERROR;
    }

    if(vectorOfTrackCollections2.size()!=1) {                                              
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
            << vectorOfTrackCollections2.size() << " TrackCollections in TE" << endmsg;
    }
    m_mon_Acceptance.push_back( ACCEPT_Both_TrackColls );
    
    if(msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "Collection: " << IdRun << " " << IdEvent << " " << vectorOfTrackCollections1.size() << " " <<
        vectorOfTrackCollections2.size() << " "
        << ( vectorOfTrackCollections1[0] ? vectorOfTrackCollections1[0]->size() : 999) << " "
        << ( vectorOfTrackCollections2[0] ? vectorOfTrackCollections2[0]->size() : 999) << " " << endmsg;
        if ( vectorOfTrackCollections1[0]) {
            for (auto ptl: *vectorOfTrackCollections1[0] )
                msg() << MSG::DEBUG << " 1: " << ptl->pt() << " " << ptl->eta() << " " << ptl->phi() << endmsg;
        } // 1
        if ( vectorOfTrackCollections2[0]) {
            for (auto ptl: *vectorOfTrackCollections2[0] )
                msg() << MSG::DEBUG << " 2: " << ptl->pt() << " " << ptl->eta() << " " << ptl->phi() << endmsg;
        } // 2
    }
    
  //////////////////////////////////// Muon part ///////////////////////////////
  // opposite charge check
    msg() << MSG::DEBUG << "JWW: muon1 \n" << *muonCandidates[0] << endmsg;
    msg() << MSG::DEBUG << "JWW: muon2 \n" << *muonCandidates[1] << endmsg;
    msg() << MSG::DEBUG << "JWW: muon1 with IDTrack()->param()->pT(): " << (muonCandidates[0]->idTrack() ? muonCandidates[0]->idTrack()->pt() : 0) << endmsg;
    msg() << MSG::DEBUG << "JWW: " << muonCandidates[0]->charge() << "  " << muonCandidates[0]->pt()<< endmsg;
    msg() << MSG::DEBUG << "JWW: muon2 with IDTrack()->param()->pT(): " << (muonCandidates[1]->idTrack()? muonCandidates[1]->idTrack()->pt() : 0) << endmsg;
    msg() << MSG::DEBUG << "JWW: " << muonCandidates[1]->charge() << "  " << muonCandidates[1]->pt()<< endmsg;


  if(m_oppositeCharge && muonCandidates[0]->idTrack()->charge()*muonCandidates[1]->idTrack()->charge() > 0) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Muon combination rejected by opposite charge check" << endmsg;
    if(timerSvc()) {
      m_TotTimer->stop();
      m_mon_TotalRunTime = m_TotTimer->elapsed();
    }
    // m_mon_Errors.push_back(ERROR_Charge_Check);
    return HLT::OK;
  }
  m_mon_Acceptance.push_back( ACCEPT_Pass_OppChargeC );
  // invariant mass cut
  double muMuMass = XMass(muonCandidates[0]->idTrack(), muonCandidates[1]->idTrack(),di_to_muons);
  if ( muMuMass < m_lowerMuMuMassCut || muMuMass > m_upperMuMuMassCut) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Muon combination rejected by mass cut: " << muMuMass << "MeV" << endmsg;
    if(timerSvc()) {
      m_TotTimer->stop();
      m_mon_TotalRunTime = m_TotTimer->elapsed();
    }
    // m_mon_Errors.push_back(ERROR_MuMu_MassCut);
    return HLT::OK;
  }
  m_mon_Acceptance.push_back( ACCEPT_MuMu_Mass);
  ////////////////////////// vertexing - muons ///////////////////////////////////
  if(m_doVertexFit && m_doMuMuVertexFit){
                   
    // Prepare the vertexing timer
    if(timerSvc()){
      m_VtxFitTimer->start();
      m_VtxFitTimer->pause(); // resume it only for the vertexing periods
    }                   
    //////////////////////////////////// muons ///////////////////////////////////
    // Vertexing time processing
    if(timerSvc()) m_VtxFitTimer->resume();
    
    TrigL2Vertex* muMuVtx = NULL;
      //creates empty vertex
      muMuVtx = new TrigL2Vertex();
      //add tracks and masses to the vertex
      StatusCode status;
      status = m_vertexingTool->addTrack(muonCandidates[0]->idTrack()->track(),muMuVtx,Trk::muon);
      if( status.isFailure() ){
	    if(msgLvl() <= MSG::DEBUG) msg() <<MSG::DEBUG <<"Failed to add muon track 1 to vertexingTool of in DiMu vertex, with pT " <<muonCandidates[0]->idTrack()->pt() << " ." << endmsg;
        m_mon_Errors.push_back( ERROR_AddMuonTrack_Fails );
	    delete muMuVtx;
	    return HLT::OK;
      }
      status = m_vertexingTool->addTrack(muonCandidates[1]->idTrack()->track(),muMuVtx,Trk::muon);
      if( status.isFailure() ){
	    if(msgLvl() <= MSG::DEBUG) msg() <<MSG::DEBUG <<"Failed to add muon track 2 to vertexingTool of in DiMu vertex, with pT " <<muonCandidates[1]->idTrack()->pt() << " ." << endmsg;
        m_mon_Errors.push_back( ERROR_AddMuonTrack_Fails );
	    delete muMuVtx;
	    return HLT::OK;
      }

      m_mon_Acceptance.push_back( ACCEPT_MuonTracks_Added );

      //vertex fit
      status = m_L2vertFitter->fit(muMuVtx);
      if(status.isSuccess()) {
        m_mon_Acceptance.push_back( ACCEPT_Muon_Vertexing );
        status = m_vertexingTool->calculateInvariantMass(muMuVtx);
      }else{
        m_mon_Errors.push_back( ERROR_MuonVtxFit_Fails );
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "MuMu vertex fit failed!" << endmsg;
	    delete muMuVtx;
	    if(timerSvc()) {
	      m_TotTimer->stop();
	      m_mon_TotalRunTime = m_TotTimer->elapsed();
	      m_VtxFitTimer->stop();
          m_mon_VertexingTime = m_VtxFitTimer->elapsed();
        }
	    return HLT::OK;
      }
      //cut on chi2
      if( status.isFailure()) {
   	    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "MuMu vertex fit failed!" << endmsg;
      	m_mon_Errors.push_back( ERROR_CalcMuonVtxMass_Fails );
	    delete muMuVtx;
	    if(timerSvc()) {
          m_TotTimer->stop();
	      m_mon_TotalRunTime = m_TotTimer->elapsed();
   	      m_VtxFitTimer->stop();
          m_mon_VertexingTime = m_VtxFitTimer->elapsed();

        }
	    return HLT::OK;
      }else{
        m_mon_Acceptance.push_back( ACCEPT_CalcInvMass );
	    if(muMuVtx->chi2() < m_muVtxChi2Cut && muMuVtx->mass() > m_lowerMuVtxMassCut && muMuVtx->mass() < m_upperMuVtxMassCut)
        {
          if(msgLvl() <= MSG::DEBUG) {                           
	        msg() << MSG::DEBUG << "MuMu vertex fit success:"
		  	      <<  " x= " << muMuVtx->m_getParametersVector()[0]   //x
			      << ", y= " << muMuVtx->m_getParametersVector()[1]   //y
			      << ", z= " << muMuVtx->m_getParametersVector()[2]   //z
			      << ", chi2= " << muMuVtx->chi2()
			      << ", ndof= " << muMuVtx->ndof()
			      << endmsg;//->pt()
         }
       }else{
	     if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Mu mu combination rejected by vertex fit." << endmsg;
         m_mon_Errors.push_back( ERROR_MuonVtx_MassCut );
	     //muon vertex  
	     delete muMuVtx;
	     if(timerSvc()) {
           m_TotTimer->stop();
	       m_mon_TotalRunTime = m_TotTimer->elapsed();
           m_VtxFitTimer->pause(); // changed stop to pause -> overall vertexing time
           //m_mon_VertexingTime = m_VtxFitTimer->elapsed();	       
         }
         return HLT::OK;
       }
     }
     delete muMuVtx;
//   } // end of loop over muons 
  
    m_mon_Acceptance.push_back( ACCEPT_MuonVtx_Part );
  } // end of vertexing muon part
  ////////////////////////// end of muons part ///////////////////////////////

  //////////////////// merge of vectorOfTrackCollections /////////////////////
//    std::vector<const TrigInDetTrackCollection*> vectorOfTrackCollections= merge_no_dupl(vectorOfTrackCollections1, vectorOfTrackCollections2);
    std::vector<const xAOD::TrackParticleContainer*> vectorOfTrackCollections= merge_no_dupl(vectorOfTrackCollections1, vectorOfTrackCollections2);
    ///////////////////////////////////////////////////////////////////////////////
    ////////////////////////// output trigger element /////////////////////////////  
    //  create vector for TrigL2Bphys particles
    //    m_trigBphysColl_b = new TrigL2BphysContainer();
    //    m_VertexColl = new TrigVertexCollection();
    //    //temporary
    //    m_trigBphysColl_kStar = new TrigL2BphysContainer();
    //    m_trigBphysColl_phi = new TrigL2BphysContainer();
    //    m_trigBphysColl_lambda = new TrigL2BphysContainer();
    //    m_trigBphysColl_ds = new TrigL2BphysContainer();
    //    //m_trigBphysColl_kSplus = new TrigL2BphysContainer();
    
    m_trigBphysColl_bxAOD = new xAOD::TrigBphysContainer();
    xAOD::TrigBphysAuxContainer trigBphysAuxColl_bxAOD;
    m_trigBphysColl_bxAOD->setStore(&trigBphysAuxColl_bxAOD);
    m_VertexColl          = new TrigVertexCollection();
    //temporary
    m_trigBphysColl_kStarxAOD  = new xAOD::TrigBphysContainer();
    xAOD::TrigBphysAuxContainer trigBphysAuxColl_kStarxAOD;
    m_trigBphysColl_kStarxAOD->setStore(&trigBphysAuxColl_kStarxAOD);

    m_trigBphysColl_phixAOD    = new xAOD::TrigBphysContainer();
    xAOD::TrigBphysAuxContainer trigBphysAuxColl_phixAOD;
    m_trigBphysColl_phixAOD->setStore(&trigBphysAuxColl_phixAOD);

    m_trigBphysColl_lambdaxAOD = new xAOD::TrigBphysContainer();
    xAOD::TrigBphysAuxContainer trigBphysAuxColl_lambdaxAOD;
    m_trigBphysColl_lambdaxAOD->setStore(&trigBphysAuxColl_lambdaxAOD);

    m_trigBphysColl_dsxAOD     = new xAOD::TrigBphysContainer();
    xAOD::TrigBphysAuxContainer trigBphysAuxColl_dsxAOD;
    m_trigBphysColl_dsxAOD->setStore(&trigBphysAuxColl_dsxAOD);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
 
    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////// loop over InDetTrackCollections - quality check /////////////////////
    std::vector<const xAOD::TrackParticleContainer*>::const_iterator iTrackCollection   = vectorOfTrackCollections.begin();
    std::vector<const xAOD::TrackParticleContainer*>::const_iterator endTrackCollection = vectorOfTrackCollections.end();
    std::vector<bool> GoodTrack((*iTrackCollection)->size(), false); 
   
    // Number of tracks monitoring
    if((*iTrackCollection)->size() > 0 ) m_mon_Acceptance.push_back( ACCEPT_Full_IDTracks );
    //m_mon_nTracks.push_back((*iTrackCollection)->size());
    //Boolean flags indicating what stages were already reached (for monitoring purposes)
    //std::vector<bool> flag_stages( 43, false );

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////// loop over all track collections /////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    for(; iTrackCollection!=endTrackCollection;iTrackCollection++) {
      int nGoodTrack=0;
      //loop over tracks
      // Monitor the tracks that failed to be added into the vertexing procedure
      //std::vector<bool> addTrackFails ( (*iTrackCollection)->size(), false );
      xAOD::TrackParticleContainer::const_iterator iTrack   = (*iTrackCollection)->begin();
      xAOD::TrackParticleContainer::const_iterator endTrack = (*iTrackCollection)->end();
      GoodTrack.resize ((*iTrackCollection)->size());
      for(int itrk=0; iTrack != endTrack; ++itrk, ++iTrack) {
        GoodTrack[itrk]=false;
        
        if( (*iTrack) != NULL && (*iTrack)->chiSquared() < 200000. ) {
//           GoodTrack[itrk]=true;
//           nGoodTrack++;
          if(msgLvl() <= MSG::DEBUG){
              msg() << MSG::DEBUG << "InDet tracks:  AlgoId "
              << (*iTrack)->trackProperties() // JW was AlgoId
              << " track " << *iTrack << " with "
              << "pt=" << (*iTrack)->pt()
              << ", eta=" << (*iTrack)->eta()
              << ", phi=" << (*iTrack)->phi0()
              << ", Z0=" << (*iTrack)->z0()
              << ", d0=" << (*iTrack)->d0()
              << ", chi2" << (*iTrack)->chiSquared()
                    << endmsg;  
          }
          
          // Check that the track is not matched to muon
//           const Trk::Perigee* perigee = iTrack->perigeeParameters();
          if ( 
              ( fabs((*iTrack)->pt()     - muonCandidates[0]->idTrack()->pt())<100.
                && fabs((*iTrack)->eta() - muonCandidates[0]->idTrack()->eta())<0.02
                && fabs((*iTrack)->phi() - muonCandidates[0]->idTrack()->phi())<0.02 ) ||
              ( fabs((*iTrack)->pt()     - muonCandidates[1]->idTrack()->pt())<100.
                && fabs((*iTrack)->eta() - muonCandidates[1]->idTrack()->eta())<0.02
                && fabs((*iTrack)->phi() - muonCandidates[1]->idTrack()->phi())<0.02 )
          ) {
            ATH_MSG(DEBUG) << "Track " << (*iTrack) << " was matched to a muon, skip it" << endmsg;
//             GoodTrack[itrk]=false;
//             continue;
          } else {
            GoodTrack[itrk]=true;
            nGoodTrack++;
            // addon monitoring tracks		      
            m_mon_pTTracks.push_back((*iTrack)-> pt());
            m_mon_etaTracks.push_back((*iTrack)->eta());
            m_mon_phiTracks.push_back((*iTrack)->phi());
            // end of monitoring
          }
            
        }else{
          if(msgLvl() <= MSG::DEBUG) {
              msg() << MSG::DEBUG << "AllTracks:  In AlgoId found problem or bad chi2 of track or muon track, so rejected"
                << "chi2 = "<<(*iTrack)->chiSquared()
                << endmsg;
          }
        } // end test of response ID algorithm to track produce
              
      } // end of loop over tracks - itrk, iTrack

      ATH_MSG(DEBUG) << "Found " << nGoodTrack << " good tracks in the track collection" << endmsg;
      m_mon_nTracks.push_back(nGoodTrack);
      
//       // Protection against TimeOuts -- will stop searching for the B->mu mu X decays if there are too many tracks
//       if(nGoodTrack > m_maxNtracks) {
//         ATH_MSG(INFO) << "Found " << nGoodTrack << " in the track collection, while maxNtracks = " << m_maxNtracks 
//                        << endmsg;
//         ATH_MSG(INFO) << "Only first " << m_maxNtracks << " tracks will be processed"
//                        << endmsg;
//         m_mon_Errors.push_back(ERROR_TooManyTracks);
//         //continue;
//       }
    
   
      //////////////////////// first loop over tracks in track collections /////////////////////
      xAOD::TrackParticleContainer::const_iterator iTrack2     = (*iTrackCollection)->begin();
      xAOD::TrackParticleContainer::const_iterator lastiTrack2 = (*iTrackCollection)->end();

      for (int itrk=0; iTrack2 !=lastiTrack2; ++itrk, iTrack2++) {
        if (!(GoodTrack[itrk]) ) continue;
        
        ///////////////////////////   mumu + X part   ////////////////////////
        // B->K mu mu
        if(m_doB_KMuMuDecay) {
          ATH_MSG(DEBUG) << "Try to build B+ -> mu mu K+ with track " << *iTrack2 << endmsg;
          checkBMuMuK(muonCandidates[0],muonCandidates[1],(*iTrack2), (*iTrackCollection), itrk);
          nTriedCombinations++;
        } //end of B->KMuMu decay

        if(iTrack2 != lastiTrack2) {
          // Protection
          if(nTriedCombinations > m_maxNcombinations) {
            ATH_MSG(DEBUG) << "Too many track combinations: " << endmsg;
            ATH_MSG(DEBUG) << "  nTriedCombinations = " << nTriedCombinations 
                             << ", while MaxNcombinations = " << m_maxNcombinations << endmsg;
            ATH_MSG(DEBUG) << "Terminate the loop" << endmsg;
            ATH_MSG(DEBUG) << "  Bphys Collection size is " << m_trigBphysColl_bxAOD->size() << endmsg;
            if(m_trigBphysColl_bxAOD->size() > 0)
              m_mon_Errors.push_back(ERROR_TooManyComb_Acc);
            else
              m_mon_Errors.push_back(ERROR_TooManyComb_Rej);
            break;
          }
          
          xAOD::TrackParticleContainer::const_iterator iTrack3= iTrack2+1;
          for (int jtrk=itrk+1; iTrack3 !=lastiTrack2; ++jtrk, ++iTrack3) {
            if (!(GoodTrack[jtrk]) ) continue;

            //If timeout has happened in spite of the protection
            if ( Athena::Timeout::instance().reached() ) {
              return HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::TIMEOUT);
            }

            if((*iTrack2)->charge() * (*iTrack3)->charge() < 0 ) {
              
              ATH_MSG(DEBUG) << "Try to build X with tracks " << *iTrack2 << ", " << *iTrack3 << endmsg;
              // Bd ->K* mu mu
              if(m_doBd_KstarMuMuDecay){
                checkBdMuMuKstar(muonCandidates[0],muonCandidates[1],(*iTrack2),(*iTrack3), (*iTrackCollection), itrk, jtrk);
                nTriedCombinations++;
                checkBdMuMuKstar(muonCandidates[0],muonCandidates[1],(*iTrack3),(*iTrack2), (*iTrackCollection), jtrk, itrk);
                nTriedCombinations++;
              } // end of Bd -> K* Mu Mu decay
          

              // Bs ->Phi mu mu
              if(m_doBs_Phi1020MuMuDecay){
                checkBsMuMuPhi(muonCandidates[0],muonCandidates[1],(*iTrack2),(*iTrack3), (*iTrackCollection), itrk, jtrk);
                nTriedCombinations++;
              } // end of Bs -> Phi Mu Mu decay

          
              // Lb ->L mu mu
              if(m_doLb_LambdaMuMuDecay){
                checkLbMuMuLambda(muonCandidates[0],muonCandidates[1],(*iTrack2),(*iTrack3), (*iTrackCollection), itrk, jtrk);
                nTriedCombinations++;
                checkLbMuMuLambda(muonCandidates[0],muonCandidates[1],(*iTrack3),(*iTrack2), (*iTrackCollection), jtrk, itrk);
                nTriedCombinations++;
              } // end of Lb -> L Mu Mu decay
          

              // Bc ->Ds(->Phi pi) mu mu
              if(m_doBc_DsMuMuDecay){

                double xPhiMass = XMass((*iTrack2), (*iTrack3), bS_to_Phi); // decay = bS_to_Phi to actually calculate Phi->KK mass

                if ( xPhiMass > m_lowerPhiDs_MassCut && xPhiMass < m_upperPhiDs_MassCut ) {

                  double xPhiMuMuMass = XMuMuMass (muonCandidates[0]->idTrack(), muonCandidates[1]->idTrack(), (*iTrack2), (*iTrack3), bS_to_Phi);

                  if ( xPhiMuMuMass < m_upperBc_DsMuMuMassCut - PIMASS ) {

                    xAOD::TrackParticleContainer::const_iterator iTrack4 = (*iTrackCollection)->begin();
                    xAOD::TrackParticleContainer::const_iterator lastiTrack4 = (*iTrackCollection)->end();

                    for (int ktrk=0; iTrack4 !=lastiTrack4; ++ktrk, iTrack4++) {
                      if (!(GoodTrack[ktrk]) ) continue;
                      if ( iTrack4 != iTrack2 && iTrack4 != iTrack3 ) {

                        checkBcMuMuDs(muonCandidates[0],muonCandidates[1],(*iTrack2), (*iTrack3), (*iTrack4), (*iTrackCollection), itrk, jtrk, xPhiMass, ktrk);
                        nTriedCombinations++;

                      }
                    }
                  }
                }
              } // end of Bc -> Ds(->Phi pi) Mu Mu decay

            } else { // cut on different X candidates signs
              ATH_MSG(DEBUG) << "Track pair rejected by opposite charge check: " 
                                 << *iTrack2 << " = " << (*iTrack2)->charge() << ",  "
                                 << *iTrack3 << " = " << (*iTrack3)->charge() << endmsg;
            }
          } // end of loop over iTrack3 (internal)                      
        }
      }   // end of loop over iTrack2 (external)             
    } //////////////// end of loop over InDetTrackCollections //////////////////    
 
 
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////// update output TE ////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  
  if(msgLvl() <= MSG::DEBUG) msg()  << MSG::DEBUG << "Totally tried to build " << nTriedCombinations << " tracks cobinations" << endmsg;
  m_mon_nTriedCombinations = nTriedCombinations;
  
  if ((m_trigBphysColl_bxAOD!=NULL) && (m_trigBphysColl_bxAOD->size() > 0)) {
    if(msgLvl() <= MSG::DEBUG) msg()  << MSG::DEBUG << "REGTEST: Store Bphys Collection size: " << m_trigBphysColl_bxAOD->size() << endmsg;
    m_mon_nBphys = m_trigBphysColl_bxAOD->size();
    
    HLT::ErrorCode sc = attachFeature(outputTE, m_trigBphysColl_lambdaxAOD, "L2BMuMuXFex_Lambda" );
    if(sc != HLT::OK) {
      if(msgLvl() <= MSG::WARNING) msg()  << MSG::WARNING << "Failed to store Lambda trigBphys Collection" << endmsg;
      return sc;
    }
    sc = attachFeature(outputTE, m_trigBphysColl_phixAOD, "L2BMuMuXFex_Phi" );
    if(sc != HLT::OK) {
      if(msgLvl() <= MSG::WARNING) msg()  << MSG::WARNING << "Failed to store Phi trigBphys Collection" << endmsg;
      return sc;
    }
    sc = attachFeature(outputTE, m_trigBphysColl_kStarxAOD, "L2BMuMuXFex_Kstar" );
    if(sc != HLT::OK) {
      if(msgLvl() <= MSG::WARNING) msg()  << MSG::WARNING << "Failed to store Kstar trigBphys Collection" << endmsg;
      return sc;
    }
    sc = attachFeature(outputTE, m_trigBphysColl_dsxAOD, "L2BMuMuXFex_Ds" );
    if(sc != HLT::OK) {
      if(msgLvl() <= MSG::WARNING) msg()  << MSG::WARNING << "Failed to store Ds trigBphys Collection" << endmsg;
      return sc;
    }
    sc = attachFeature(outputTE, m_trigBphysColl_bxAOD, "L2BMuMuXFex" );
    if(sc != HLT::OK) {
      if(msgLvl() <= MSG::WARNING) msg()  << MSG::WARNING << "Failed to store B+/-, Bd, Bs or Lb (Bar) trigBphys Collection" << endmsg;
      return sc;
    }
  }else{
    if(msgLvl() <= MSG::DEBUG) msg()  << MSG::DEBUG << "REGTEST: no B+/-, Bd, Bs or Lb (Bar) bphys collection to store "  << endmsg;
    delete m_trigBphysColl_bxAOD;
    delete m_trigBphysColl_lambdaxAOD;
    delete m_trigBphysColl_phixAOD;
    delete m_trigBphysColl_kStarxAOD;
    delete m_trigBphysColl_dsxAOD;
  }
  m_trigBphysColl_bxAOD=NULL;
  delete m_VertexColl;
  m_VertexColl=NULL;
  // try add other collections to delete them!!
//   delete m_trigBphysColl_lambda;
  m_trigBphysColl_lambdaxAOD = NULL;
//   delete m_trigBphysColl_phi;
  m_trigBphysColl_phixAOD = NULL;
//   delete m_trigBphysColl_kStar;
  m_trigBphysColl_kStarxAOD = NULL;
//   delete m_trigBphysColl_ds;
  m_trigBphysColl_dsxAOD = NULL;
  
  ///////////////////////////////////////////////////////////////////////
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Stopping timer..." << endmsg;
  if(timerSvc()) {
    m_TotTimer->stop();
    m_mon_TotalRunTime = m_TotTimer->elapsed();
  }

  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Leaving execute..." << endmsg;
  return HLT::OK;
}
/*----------------------------------------------------------------------------*/


//NEW
void TrigL2BMuMuXFex::checkBMuMuK(const xAOD::L2CombinedMuon* mu1, const xAOD::L2CombinedMuon* mu2, const xAOD::TrackParticle* trk3,
                                  const xAOD::TrackParticleContainer* trkCollection, int iTrk3) {
    double kMuMumass = KMuMuMass(mu1->idTrack(), mu2->idTrack(), trk3);
    
    if(kMuMumass > m_lowerKMuMuMassCut && kMuMumass < m_upperKMuMuMassCut) {
        // Now make L2Bphys object - initial addon
        xAOD::TrigBphys* trigPartBmumuKplus = new xAOD::TrigBphys();
        trigPartBmumuKplus->makePrivateStore();
        trigPartBmumuKplus->initialise(0., 0., 0., xAOD::TrigBphys::BKMUMU, kMuMumass,xAOD::TrigBphys::L2);
        if(msgLvl() <= MSG::DEBUG) {
            msg() << MSG::DEBUG << " Create B+ Bphys particle with roIId - more RoIs => 0" << trigPartBmumuKplus->roiId() << " mass " << kMuMumass
            << " phi, eta - more RoIs => 0" << trigPartBmumuKplus->phi() << " " << trigPartBmumuKplus->eta() << " vertex type " << trigPartBmumuKplus->particleType()
            << endmsg;
        }
        ElementLinkVector<xAOD::TrackParticleContainer> trackVectorBmumuKplus;
        ElementLink<xAOD::TrackParticleContainer> track1EL = mu1->idTrackLink();
        ElementLink<xAOD::TrackParticleContainer> track2EL = mu2->idTrackLink();
        ElementLink<xAOD::TrackParticleContainer> track3EL(*trkCollection, iTrk3);

        if ( msgLvl() <= MSG::VERBOSE ){
            msg() << MSG::VERBOSE << "Just check track links... " << endmsg;
            msg() << MSG::VERBOSE << "Muon 1 pT " << (*track1EL)->pt() << " eta: " << (*track1EL)->eta() << " phi: " << (*track1EL)->phi() << endmsg;
            msg() << MSG::VERBOSE << "Muon 2 pT " << (*track2EL)->pt() << " eta: " << (*track2EL)->eta() << " phi: " << (*track2EL)->phi() << endmsg;
            msg() << MSG::VERBOSE << "Track 1 pT " << (*track3EL)->pt() << " eta: " << (*track3EL)->eta() << " phi: " << (*track3EL)->phi() << endmsg;
        }
        trackVectorBmumuKplus.push_back(track1EL); // temp
        trackVectorBmumuKplus.push_back(track2EL); // temp
        trackVectorBmumuKplus.push_back(track3EL); // temp
        
        trigPartBmumuKplus->addTrackParticleLink(track1EL);
        trigPartBmumuKplus->addTrackParticleLink(track2EL);
        trigPartBmumuKplus->addTrackParticleLink(track3EL);

        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " K candidate accept: " <<trk3->pt()<< "MeV and B invariant mass "<< kMuMumass<< "MeV"<< endmsg;
        m_mon_BMuMuK_InvMass_B.push_back((kMuMumass*0.001));
        m_mon_BMuMuK_PtB_K.push_back(trk3->pt());
        m_mon_BMuMuK_EtaB_K.push_back(trk3->eta());
        m_mon_BMuMuK_PhiB_K.push_back(trk3->phi());
        //////////////////////////////// vertexing /////////////////////////////////

        
        //////////////////////////////// X particles ///////////////////////////////
        // B+/- vertexing & mass cut
        if(m_doB_KMuMuVertexFit &&  m_doVertexFit) {
            // B -> K Mu Mu vertexing
            bool vertex=true;
            TrigL2Vertex* baplusVtx = NULL;
            TrigVertex*   mother_BplusVtx = NULL;
            if(timerSvc()){
                m_VtxFitTimer->start();
                m_VtxFitTimer->pause(); // resume it only for the vertexing periods
            }
            // Vertexing time processing
            if(timerSvc()) m_VtxFitTimer->resume();
            
            baplusVtx = new TrigL2Vertex();
            StatusCode status;
            status = m_vertexingTool->addTrack(mu1->idTrack()->track(),baplusVtx,Trk::muon);   //
            if( status.isFailure() ) {
                if(msgLvl() <= MSG::DEBUG) msg() <<MSG::DEBUG <<" Failed to add muon track 1 to vertexingTool of B vertex, with pT " <<mu1->idTrack()->pt() << " ." << endmsg;
                m_mon_Errors.push_back( ERROR_AddMuonTrack_Fails );
                //delete baplusVtx;
                //continue;
                vertex = false;
            }
            if(vertex) status = m_vertexingTool->addTrack(mu2->idTrack()->track(),baplusVtx,Trk::muon);   //
            if( status.isFailure() ) {
                if(msgLvl() <= MSG::DEBUG) msg() <<MSG::DEBUG <<" Failed to add muon track 2 to vertexingTool of B vertex, with pT " <<mu2->idTrack()->pt() << " ." << endmsg;
                m_mon_Errors.push_back( ERROR_AddMuonTrack_Fails );
                //delete baplusVtx;
                //continue;
                vertex = false;
            }
            if(vertex) status = m_vertexingTool->addTrack(trk3->track(),baplusVtx,Trk::kaon);
            if( status.isFailure() ) {
                if(msgLvl() <= MSG::DEBUG) msg() <<MSG::DEBUG <<" Failed to add particle track 3 to vertexingTool of B vertex with pT, chi2 " <<trk3->pt() << " MeV, " << trk3->chiSquared() <<" ."<< endmsg;
                m_mon_Errors.push_back( ERROR_AddTrack_Fails );
                //delete baplusVtx;
                //continue;
                vertex = false;
            }

            //vertex fit
            if(vertex) status = m_L2vertFitter->fit(baplusVtx);
            if(status.isSuccess()) status = m_vertexingTool->calculateInvariantMass(baplusVtx);
            if(status.isFailure()){
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " B vertex fit failed!" << endmsg;
                delete baplusVtx;
                delete trigPartBmumuKplus;
                m_mon_Errors.push_back( ERROR_VtxFitFailed );
                //continue;
                vertex = false;
            }else{
                mother_BplusVtx = m_vertexingTool->createTrigVertex(baplusVtx);
                status = m_vertexingTool->createMotherParticle(baplusVtx);
                if(status.isSuccess()){
                    m_mon_Acceptance.push_back( ACCEPT_MotherVtxCreated );
                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " B created, with pt: " << baplusVtx->m_getMotherTrack()->pT() << endmsg;
                }else{
                    vertex = false;
                }
                if(mother_BplusVtx == NULL){
                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  Vertex fit failed for particle candidate track " << trk3 << endmsg;
                    //delete mother_BplusVtx;
                    m_mon_Errors.push_back( ERROR_MotherVtxFitFailed );
                    //continue;
                    delete trigPartBmumuKplus;
                    vertex = false;
                }else{
                    if(msgLvl() <= MSG::DEBUG) {
                        msg() << MSG::DEBUG << " B vertex fit: x = " <<mother_BplusVtx->x()
                        <<", y = " <<mother_BplusVtx->y()
                        <<", z = " <<mother_BplusVtx->z()
                        <<", mass = " <<mother_BplusVtx->mass()
                        <<", chi2 = " <<mother_BplusVtx->chi2()
                        <<", ndof = " <<mother_BplusVtx->ndof() /// (number degrees of freedom)
                        <<", massVariance = " <<mother_BplusVtx->massVariance()
                        << endmsg;
                    }
                    if(mother_BplusVtx->massVariance() != 0) {
                        if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  B mass pull = " << (mother_BplusVtx->mass()-BPLUSMASS/mother_BplusVtx->massVariance()) << endmsg;
                    }else{
                        if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " B mass variance is zero, can't calculate the pull" << endmsg;
                    }
                    /// add vertex results to L2Bphys
                    trigPartBmumuKplus->setFitmass(mother_BplusVtx->mass());
                    trigPartBmumuKplus->setFitchi2(mother_BplusVtx->chi2());
                    trigPartBmumuKplus->setFitndof(mother_BplusVtx->ndof());
                    trigPartBmumuKplus->setFitx(mother_BplusVtx->x());
                    trigPartBmumuKplus->setFity(mother_BplusVtx->y());
                    trigPartBmumuKplus->setFitz(mother_BplusVtx->z());
                    
                    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " SigmaX =  SigmaY =  SigmaZ = "
                        << sqrt(mother_BplusVtx->cov()[0]) << " "
                        << sqrt(mother_BplusVtx->cov()[2]) << " "
                        << sqrt(mother_BplusVtx->cov()[5])
                        << endmsg;
                    
                    // cut on chi2 and invariant mass of bplus after vertexing.
                    if(baplusVtx->chi2() < m_bVtxChi2Cut &&
                       baplusVtx->mass() > m_lowerBVtxMassCut && baplusVtx->mass() < m_upperBVtxMassCut)
                    {
                        if(msgLvl() <= MSG::DEBUG)  {
                            msg() << MSG::DEBUG << " B vertex fit success: x= "
                            <<            baplusVtx->m_getParametersVector()[0] //x
                            << ", y= " << baplusVtx->m_getParametersVector()[1] //y
                            << ", z= " << baplusVtx->m_getParametersVector()[2] //z
                            << ", chi2= " << baplusVtx->chi2()
                            <<", ndof = " <<baplusVtx->ndof()
                            << endmsg;
                        }
                        // perferct -> Bphys particle was stored in Collection
                        m_VertexColl->push_back(mother_BplusVtx);
                        m_trigBphysColl_bxAOD->push_back(trigPartBmumuKplus);
                        if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Added B vertex to vertex collection" << endmsg;
                        
                        // monitoring
                        m_mon_BMuMuK_n++;
                        m_mon_Acceptance.push_back( ACCEPT_BphysCollParticle );
                        m_mon_BMuMuK_VtxMass_B.push_back((baplusVtx->mass())*0.001);
                        m_mon_BMuMuK_Chi2toNDoF_B.push_back((baplusVtx->chi2())/(baplusVtx->ndof()));
                        m_mon_BMuMuK_VtxPtB_K.push_back((trk3->pt())*0.001);
                        m_mon_BMuMuK_VtxEtaB_K.push_back(trk3->eta());
                        m_mon_BMuMuK_VtxPhiB_K.push_back(trk3->phi());
                        if(msgLvl() <= MSG::DEBUG) {
                            msg() << MSG::DEBUG << " Finnished Bphys particle with fitmass" << trigPartBmumuKplus->fitmass() << " mass " << trigPartBmumuKplus->mass()
                            << " phi, eta - more RoIs => 0" << trigPartBmumuKplus->phi() << " " << trigPartBmumuKplus->eta() << " vertex type " << trigPartBmumuKplus->particleType()
                            << endmsg;
                        }
                        
                    }else{
                        if(msgLvl() <= MSG::DEBUG) {
                            msg() << MSG::DEBUG << " B rejected by mass or chi2 cuts after vertex fit!"
                            << ", chi2= " << baplusVtx->chi2()
                            << endmsg;  
                        }
                        vertex = false;
                        delete mother_BplusVtx;
                        delete trigPartBmumuKplus;
                    }
                } // end of successful fit condition "mother"
                delete baplusVtx;
            } // end of successful fit condition initial vertex
            if(timerSvc()) {
                m_VtxFitTimer->pause(); // resume it only for the vertexing periods
            }
        } // end of  K Mu Mu vertex fit
        else{ // if doVertexFit,... == False     if((!m_doB_KMuMuVertexFit || m_doVertexFit)){     UPGRADE NEEDED
            
            ////////////////////////////////////////////////////////////////////////////////////////////////////////
            // B+/- end of mass cuts, vertexing off
            if (kMuMumass > m_lowerB_KMuMuMassCutVtxOff && kMuMumass < m_upperB_KMuMuMassCutVtxOff) {
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " K candidate accept after tight cuts: " <<trk3->pt()<< "MeV and B invariant mass "<< kMuMumass<< "MeV"<< endmsg;
                m_trigBphysColl_bxAOD->push_back(trigPartBmumuKplus);
                if(msgLvl() <= MSG::DEBUG) {
                    msg() << MSG::DEBUG << " Added B+ particle to Bphys particle collection   "
                    //<< ", eta=" trigPartBmumuKplus->eta()
                    //<< ", phi= " << trigPartBmumuKplus->phi()
                    << ", mass= " << trigPartBmumuKplus->mass()
                    << ", particle type= " << trigPartBmumuKplus->particleType()
                    << endmsg;
                }
                // monitoring
                m_mon_BMuMuK_n++;
                m_mon_Acceptance.push_back( ACCEPT_BphysCollParticle );
                m_mon_BMuMuK_InvMass_B.push_back((kMuMumass*0.001));
                m_mon_BMuMuK_PtB_K.push_back(trk3->pt());
                m_mon_BMuMuK_EtaB_K.push_back(trk3->eta());
                m_mon_BMuMuK_PhiB_K.push_back(trk3->phi());
            }else{
                if(msgLvl() <= MSG::DEBUG)
                    msg() << MSG::DEBUG << " No B+ particle stored due cuts! " << endmsg;
                delete trigPartBmumuKplus;
            }
            
        }
        ///////////////////////// end of B -> K Mu Mu part with vertexing off /////////////////////////
    } else {// end of invariant mass cuts
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " B+ rejected due do B+ mass cut" << endmsg;
    }
    ////////////////////////////////////////////////////////////////////////////////

} //checkBMuMuK new

// NEW
void TrigL2BMuMuXFex::checkBdMuMuKstar(const xAOD::L2CombinedMuon* mu1, const xAOD::L2CombinedMuon* mu2,
                                       const xAOD::TrackParticle* trk3, const xAOD::TrackParticle* trk4, const xAOD::TrackParticleContainer* trkCollection, int iTrk3, int iTrk4) {
    
    double xMass = 0;
    double xMuMuMass = 0;
    
    xMass = XMass(trk3,trk4, bD_to_Kstar);
    xMuMuMass = XMuMuMass(mu1->idTrack(), mu2->idTrack(), trk3, trk4, bD_to_Kstar);
    
    if ((xMass > m_lowerKstar_KaonMassCut && xMass < m_upperKstar_KaonMassCut) && (xMuMuMass > m_lowerBd_KstarMuMuMassCut && xMuMuMass < m_upperBd_KstarMuMuMassCut)) {
        // Now make L2Bphys object - initial addon
        xAOD::TrigBphys* trigPartBmumuKstar = new xAOD::TrigBphys();
        trigPartBmumuKstar->makePrivateStore();
        trigPartBmumuKstar->initialise(0., 0., 0., xAOD::TrigBphys::PHIKK, xMass,xAOD::TrigBphys::L2);

        if(msgLvl() <= MSG::DEBUG) {
            msg() << MSG::DEBUG << " Create K* Bphys particle with roIId - more RoIs => " << trigPartBmumuKstar->roiId() << " mass " << xMass
            << " phi, eta - more RoIs => " << trigPartBmumuKstar->phi() << " " << trigPartBmumuKstar->eta() << " vertex type " << trigPartBmumuKstar->particleType()
            << endmsg;
        }
        
        // Store links to the tracks forming bphys particle
        ElementLinkVector<xAOD::TrackParticleContainer> trackVectorBmumuKstar;
        ElementLink<xAOD::TrackParticleContainer> track3EL(*trkCollection, iTrk3);
        ElementLink<xAOD::TrackParticleContainer> track4EL(*trkCollection, iTrk4);
        //         ElementLink<TrigInDetTrackCollection> track3EL(*(*iTrackCollection), itrk);
        //         ElementLink<TrigInDetTrackCollection> track4EL(*(*iTrackCollection), jtrk);
        
        if ( msgLvl() <= MSG::VERBOSE ){
            msg() << MSG::VERBOSE << "Just check track links... " << endmsg;
            msg() << MSG::VERBOSE << "Track 1 pT " << (*track3EL)->pt() << " eta: " << (*track3EL)->eta() << " phi: " << (*track3EL)->phi() << endmsg;
            msg() << MSG::VERBOSE << "Track 2 pT " << (*track4EL)->pt() << " eta: " << (*track4EL)->eta() << " phi: " << (*track4EL)->phi() << endmsg;
        }
        trackVectorBmumuKstar.push_back(track3EL); // temp
        trackVectorBmumuKstar.push_back(track4EL); // temp
        
        trigPartBmumuKstar->addTrackParticleLink(track3EL);
        trigPartBmumuKstar->addTrackParticleLink(track4EL);
        // end of initial addon
        
        
        //         if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Kstar's Pion and Kaon Candidates accepted with K* mass: " << xMass << "MeV"
        //                                         << " and with Bd mass : " << xMuMuMass << "MeV"
        //                                         << " kaon particle with pt: "<< (trk3)->pt()
        //                                         << " pion charged particle with pt: "<< (trk4)->pt()
        //                                         << endmsg;
        m_mon_BdMuMuKs_InvMassBd_Ks.push_back((xMass*0.001));
        m_mon_BdMuMuKs_PtKs_Ka.push_back((trk3)->pt());
        m_mon_BdMuMuKs_EtaKs_Ka.push_back((trk3)->eta());
        m_mon_BdMuMuKs_PhiKs_Ka.push_back((trk3)->phi());
        m_mon_BdMuMuKs_PtKs_Pi.push_back((trk4)->pt());
        m_mon_BdMuMuKs_EtaKs_Pi.push_back((trk4)->eta());
        m_mon_BdMuMuKs_PhiKs_Pi.push_back((trk4)->phi());
        
        bool goodKstar = false;
        //K* vertexing & mass cut
        if(m_doKstar_KaonPionVertexFit && m_doVertexFit) {
            bool vertex = true;
            TrigL2Vertex* kaStarVtx = NULL;
            TrigVertex*   mother_KstarVtx = NULL;
            
            // timer
            if(timerSvc()) m_VtxFitTimer->resume();
            kaStarVtx = new TrigL2Vertex();
            
            StatusCode status;
            status = m_vertexingTool->addTrack(trk3->track(),kaStarVtx,Trk::kaon);
            if( status.isFailure() ) {
                //delete kaStarVtx;
                //continue;
                vertex = false;
            }
            if(vertex) status = m_vertexingTool->addTrack(trk4->track(),kaStarVtx,Trk::pion);
            if( status.isFailure() ) {
                //delete kaStarVtx;
                //continue;
                vertex = false;
            }
            
            //////////////
            //vertex fit
            if(vertex) status = m_L2vertFitter->fit(kaStarVtx);
            if(status.isSuccess() && kaStarVtx != NULL) status = m_vertexingTool->calculateInvariantMass(kaStarVtx);
            if(status.isFailure() || kaStarVtx == NULL){
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Kstar vertex fit failed!" << endmsg;
                delete kaStarVtx;
                if(timerSvc()) m_VtxFitTimer->pause();
                //continue; // for performance test not continue?
                vertex=false;
            }else{
                mother_KstarVtx = m_vertexingTool->createTrigVertex(kaStarVtx);
                status = m_vertexingTool->createMotherParticle(kaStarVtx);
                if(status.isSuccess()){
                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Kstar created, with pt: " << kaStarVtx->m_getMotherTrack()->pT() << endmsg;
                }
                if(mother_KstarVtx == NULL){
                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  Vertex fit failed for particle candidate tracks " << trk3
                        <<" and "<<trk4<< endmsg;
                    if(timerSvc()) m_VtxFitTimer->pause();
                    //continue; // for performance test not continue?
                    vertex = false;
                }else{
                    if(msgLvl() <= MSG::DEBUG) {
                        msg() << MSG::DEBUG << " Kstar vertex fit: x = " <<mother_KstarVtx->x()
                        <<", y = " <<mother_KstarVtx->y()
                        <<", z = " <<mother_KstarVtx->z()
                        <<", mass = " <<mother_KstarVtx->mass()
                        <<", chi2 = " <<mother_KstarVtx->chi2()
                        <<", ndof = " <<mother_KstarVtx->ndof() /// (number degrees of freedom)
                        <<", massVariance = " <<mother_KstarVtx->massVariance()
                        << endmsg;
                    }
                    if(mother_KstarVtx->massVariance() != 0) {
                        if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  Kstar mass pull = " << (mother_KstarVtx->mass()-KSTARMASS/mother_KstarVtx->massVariance()) << endmsg;
                    }else{
                        if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " Kstar mass variance is zero, can't calculate the pull" << endmsg;
                    }
                    
                    /// add vertex results to L2Bphys
                    trigPartBmumuKstar->setFitmass(mother_KstarVtx->mass());
                    trigPartBmumuKstar->setFitchi2(mother_KstarVtx->chi2());
                    trigPartBmumuKstar->setFitndof(mother_KstarVtx->ndof());
                    trigPartBmumuKstar->setFitx(mother_KstarVtx->x());
                    trigPartBmumuKstar->setFity(mother_KstarVtx->y());
                    trigPartBmumuKstar->setFitz(mother_KstarVtx->z());
                    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " SigmaX =  SigmaY =  SigmaZ = "
                        << sqrt(mother_KstarVtx->cov()[0]) << " "
                        << sqrt(mother_KstarVtx->cov()[2]) << " "
                        << sqrt(mother_KstarVtx->cov()[5])
                        << endmsg;
                    
                    // cut on chi2 and invariant mass of kStar after vertexing.
                    if(kaStarVtx->chi2() < m_kStarVtxChi2Cut &&
                       kaStarVtx->mass() > m_lowerKstarVtxMassCut && kaStarVtx->mass() < m_upperKstarVtxMassCut)
                    {
                        if(msgLvl() <= MSG::DEBUG)  {
                            msg() << MSG::DEBUG << " Kstar vertex fit success: x= "
                            <<            kaStarVtx->m_getParametersVector()[0] //x
                            << ", y= " << kaStarVtx->m_getParametersVector()[1] //y
                            << ", z= " << kaStarVtx->m_getParametersVector()[2] //z
                            << ", chi2= " << kaStarVtx->chi2()
                            <<", ndof = " <<kaStarVtx->ndof()
                            << endmsg;
                        }
                        goodKstar = true;
                        
                        // GREAT -> store daughter particle info
                        m_VertexColl->push_back(mother_KstarVtx);
                        if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Added Kstar vertex to vertex collection" << endmsg;
                        int index = m_VertexColl->size()-1;
                        ElementLink<TrigVertexCollection> KstarVertexEL(*m_VertexColl,index);
                        
                        // monitoring
                        m_mon_BdMuMuKs_VtxMassBd_Ks.push_back((kaStarVtx->mass())*0.001);
                        m_mon_BdMuMuKs_Chi2toNDoFBd_Ks.push_back((kaStarVtx->chi2())/(kaStarVtx->ndof()));
                        
                        m_mon_BdMuMuKs_VtxPtKs_Ka.push_back((trk3->pt())*0.001);
                        m_mon_BdMuMuKs_VtxEtaKs_Ka.push_back(trk3->eta());
                        m_mon_BdMuMuKs_VtxPhiKs_Ka.push_back(trk3->phi());
                        m_mon_BdMuMuKs_VtxPtKs_Pi.push_back((trk4->pt())*0.001);
                        m_mon_BdMuMuKs_VtxEtaKs_Pi.push_back(trk4->eta());
                        m_mon_BdMuMuKs_VtxPhiKs_Pi.push_back(trk4->phi());
                        
                        if(msgLvl() <= MSG::DEBUG) {
                            msg() << MSG::DEBUG << " Finnished Bphys daughter particle with fitmass" << trigPartBmumuKstar->fitmass() << " mass " << trigPartBmumuKstar->mass()
                            << " phi, eta - more RoIs => 0" << trigPartBmumuKstar->phi() << " " << trigPartBmumuKstar->eta() << " vertex type " << trigPartBmumuKstar->particleType()
                            << endmsg;
                        }
                        
                    }else{
                        if(msgLvl() <= MSG::DEBUG) {
                            msg() << MSG::DEBUG << " B0 rejected by mass or chi2 cuts after vertex fit!"
                            << ", chi2= " << kaStarVtx->chi2()
                            << endmsg;
                        }
                        if(timerSvc()) m_VtxFitTimer->pause();
                        //continue; // for performance test not continue
                        vertex = false;
                        delete mother_KstarVtx;
                    }
                } // end of successful fit condition "mother"
                //int index = m_VertexColl->size()-1;
                //ElementLink<TrigVertexCollection> BplusVertexEL(*m_VertexColl,index);
                delete kaStarVtx;
            } // end of successful fit condition initial vertex
            if(timerSvc()) m_VtxFitTimer->pause();
        } // end Kstar_KaonPion vertex fit
        
        m_trigBphysColl_kStarxAOD->push_back(trigPartBmumuKstar);
        int KstarIndex = m_trigBphysColl_kStarxAOD->size() - 1;
        
        if(m_doKstar_KaonPionVertexFit && goodKstar) {
            if(msgLvl() <= MSG::DEBUG) {
                msg() << MSG::DEBUG << " Well done in Kstar vertexing!!!" << endmsg;
                //continue; // OBSOLETE, done inside vertexing
            }
            
            
            // Bd+/- vertexing & mass cut
            if(m_doBd_KstarMuMuVertexFit && m_doVertexFit){
                bool vertex = true;
                if(timerSvc()) m_VtxFitTimer->resume();
                TrigL2Vertex* baDVtx = NULL;
                TrigVertex* mother_BdVtx=NULL;
                baDVtx = new TrigL2Vertex();
                StatusCode status;
                status = m_vertexingTool->addTrack(mu1->idTrack()->track(),baDVtx,Trk::muon);   //
                if(status.isFailure()){
                    //delete baDVtx;
                    if(timerSvc()) m_VtxFitTimer->pause();
                    //continue;
                    vertex = false;
                }
                if(vertex) status = m_vertexingTool->addTrack(mu2->idTrack()->track(),baDVtx,Trk::muon);   //
                if(status.isFailure()){
                    //delete baDVtx;
                    if(timerSvc()) m_VtxFitTimer->pause();
                    //continue;
                    vertex = false;
                }
                if(vertex) status = m_vertexingTool->addTrack(trk3->track(),baDVtx,Trk::kaon);
                if(status.isFailure()){
                    //delete baDVtx;
                    if(timerSvc()) m_VtxFitTimer->pause();
                    //continue;
                    vertex = false;
                }
                if(vertex) status = m_vertexingTool->addTrack(trk4->track(),baDVtx,Trk::pion);
                if(status.isFailure()){
                    //delete baDVtx;
                    if(timerSvc()) m_VtxFitTimer->pause();
                    //continue;
                    vertex = false;
                }
                
                //ElementLink<TrigL2BphysContainer> trigPartBdEL(*m_trigBphysColl_kStar,KstarIndex);
                ElementLink<xAOD::TrigBphysContainer> trigPartBdEL(*m_trigBphysColl_kStarxAOD,KstarIndex);
                
                //TrigL2Bphys* trigPartBmumuBd = new TrigL2Bphys(0.0, 0.0, 0.0, TrigL2Bphys::BDKSTMUMU, xMuMuMass, trigPartBdEL);
                xAOD::TrigBphys* trigPartBmumuBd = new xAOD::TrigBphys();
                trigPartBmumuBd->makePrivateStore();
                trigPartBmumuBd->initialise(0.0, 0.0, 0.0, xAOD::TrigBphys::BDKSTMUMU, xMuMuMass, trigPartBdEL,xAOD::TrigBphys::L2);
                
                //vertex fit
                if(vertex) status = m_L2vertFitter->fit(baDVtx);
                if(status.isSuccess()) status = m_vertexingTool->calculateInvariantMass(baDVtx);
                if(status.isFailure()){
                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Bd vertex fit failed!" << endmsg;
                    delete baDVtx;
                    delete trigPartBmumuBd;
                    //continue; // for performance test not continue
                    vertex = false;
                }else{
                    mother_BdVtx = m_vertexingTool->createTrigVertex(baDVtx);
                    status = m_vertexingTool->createMotherParticle(baDVtx);
                    if(status.isSuccess()){
                        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Bd created, with pt: " << baDVtx->m_getMotherTrack()->pT() << endmsg;
                    }
                    if(mother_BdVtx == NULL){
                        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  Vertex fit failed for particle candidate track " << trk3
                            <<" and "<<trk4<< endmsg;
                        delete trigPartBmumuBd;
                        //continue; // for performance test not continue
                        vertex = false;
                    }else{
                        if(msgLvl() <= MSG::DEBUG) {
                            msg() << MSG::DEBUG << " Bd vertex fit: x = " <<mother_BdVtx->x()
                            <<", y = " <<mother_BdVtx->y()
                            <<", z = " <<mother_BdVtx->z()
                            <<", mass = " <<mother_BdVtx->mass()
                            <<", chi2 = " <<mother_BdVtx->chi2()
                            <<", ndof = " <<mother_BdVtx->ndof() /// (number degrees of freedom)
                            <<", massVariance = " <<mother_BdVtx->massVariance()
                            << endmsg;
                        }
                        if(mother_BdVtx->massVariance() != 0) {
                            if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  Bd mass pull = " << (mother_BdVtx->mass()-BDMASS/mother_BdVtx->massVariance()) << endmsg;
                        }else{
                            if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " Bd mass variance is zero, can't calculate the pull" << endmsg;
                        }
                        /// add vertex results to L2Bphys
                        trigPartBmumuBd->setFitmass(mother_BdVtx->mass());
                        trigPartBmumuBd->setFitchi2(mother_BdVtx->chi2());
                        trigPartBmumuBd->setFitndof(mother_BdVtx->ndof());
                        trigPartBmumuBd->setFitx(mother_BdVtx->x());
                        trigPartBmumuBd->setFity(mother_BdVtx->y());
                        trigPartBmumuBd->setFitz(mother_BdVtx->z());
                        
                        if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " SigmaX =  SigmaY =  SigmaZ = "
                            << sqrt(mother_BdVtx->cov()[0]) << " "
                            << sqrt(mother_BdVtx->cov()[2]) << " "
                            << sqrt(mother_BdVtx->cov()[5])
                            << endmsg;
                        
                        // cut on chi2 and invariant mass of Bd after vertexing.
                        if(baDVtx->chi2() < m_bDVtxChi2Cut && baDVtx->mass() > m_lowerBdVtxMassCut && baDVtx->mass() < m_upperBdVtxMassCut)
                        {
                            if(msgLvl() <= MSG::DEBUG)  {
                                msg() << MSG::DEBUG << " Bd vertex fit success: x= "
                                <<            baDVtx->m_getParametersVector()[0] //x
                                << ", y= " << baDVtx->m_getParametersVector()[1] //y
                                << ", z= " << baDVtx->m_getParametersVector()[2] //z
                                << ", chi2= " << baDVtx->chi2()
                                <<", ndof = " <<baDVtx->ndof()
                                << endmsg;
                            }
                            
                            // PERFECT -> Bphys particle was stored in collection
                            m_VertexColl->push_back(mother_BdVtx);
                            m_trigBphysColl_bxAOD->push_back(trigPartBmumuBd);
                            if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Added Bd vertex to vertex collection" << endmsg;
                            
                            // monitoring of successfuly stored particles
                            m_mon_BdMuMuKs_n++;
                            m_mon_Acceptance.push_back( ACCEPT_BphysCollParticle );
                            m_mon_BdMuMuKs_VtxMass_Bd.push_back((baDVtx->mass())*0.001);
                            m_mon_BdMuMuKs_Chi2toNDoF_Bd.push_back((baDVtx->chi2())/(baDVtx->ndof()));
                            
                            m_mon_BdMuMuKs_VtxPtBd_Ka.push_back((trk3->pt())*0.001);
                            m_mon_BdMuMuKs_VtxEtaBd_Ka.push_back(trk3->eta());
                            m_mon_BdMuMuKs_VtxPhiBd_Ka.push_back(trk3->phi());
                            m_mon_BdMuMuKs_VtxPtBd_Pi.push_back((trk4->pt())*0.001);
                            m_mon_BdMuMuKs_VtxEtaBd_Pi.push_back(trk4->eta());
                            m_mon_BdMuMuKs_VtxPhiBd_Pi.push_back(trk4->phi());
                            
                            if(msgLvl() <= MSG::DEBUG) {
                                msg() << MSG::DEBUG << " Finnished Bphys particle with fitmass" << trigPartBmumuBd->fitmass() << " mass " << trigPartBmumuBd->mass()
                                << " phi, eta - more RoIs => 0" << trigPartBmumuBd->phi() << " " << trigPartBmumuBd->eta() << " vertex type " << trigPartBmumuBd->particleType()
                                << endmsg;
                            }
                            
                        }else{
                            if(msgLvl() <= MSG::DEBUG) {
                                msg() << MSG::DEBUG << " Bd rejected by mass or chi2 cuts after vertex fit!"
                                << ", chi2= " << baDVtx->chi2()
                                << endmsg;
                            }
                            delete trigPartBmumuBd;
                            //continue; // for performance test not continue
                            vertex = false;
                        }
                    } // end of successful fit condition "mother"
                    if (vertex){
                        int index = m_VertexColl->size()-1;
                        ElementLink<TrigVertexCollection> BdVertexEL(*m_VertexColl,index);
                    } else {
                        delete mother_BdVtx;
                    }
                    delete baDVtx;
                } // end of successful fit condition initial vertex
                
            } // Bd -> K* Mu Mu vertexing
        }  // test over failed vertexing
        
        //////////////////////////////// vertexing off ///////////////////////////////////////////////
        if(!m_doBd_KstarMuMuVertexFit || !m_doVertexFit){
            if ((xMuMuMass > m_lowerBd_KstarMuMuMassCutVtxOff && xMuMuMass < m_upperBd_KstarMuMuMassCutVtxOff)) {
                
                // Store links to the tracks forming bphys particle
                ElementLinkVector<xAOD::TrackParticleContainer> trackVectorBmumuBd;
                ElementLink<xAOD::TrackParticleContainer> track1ELBd = mu1->idTrackLink();
                ElementLink<xAOD::TrackParticleContainer> track2ELBd = mu2->idTrackLink();
                ElementLink<xAOD::TrackParticleContainer> track3ELBd(*trkCollection, iTrk3);
                ElementLink<xAOD::TrackParticleContainer> track4ELBd(*trkCollection, iTrk4);
                
                if ( msgLvl() <= MSG::VERBOSE ){
                    msg() << MSG::VERBOSE << "Just check track links... " << endmsg;
                    msg() << MSG::VERBOSE << "Muon 1 pT " << (*track1ELBd)->pt() << " eta: " << (*track1ELBd)->eta() << " phi: " << (*track1ELBd)->phi() << endmsg;
                    msg() << MSG::VERBOSE << "Muon 2 pT " << (*track2ELBd)->pt() << " eta: " << (*track2ELBd)->eta() << " phi: " << (*track2ELBd)->phi() << endmsg;
                    msg() << MSG::VERBOSE << "Track 1 pT " << (*track3ELBd)->pt() << " eta: " << (*track3ELBd)->eta() << " phi: " << (*track3ELBd)->phi() << endmsg;
                    msg() << MSG::VERBOSE << "Track 2 pT " << (*track4ELBd)->pt() << " eta: " << (*track4ELBd)->eta() << " phi: " << (*track4ELBd)->phi() << endmsg;
                }
                trackVectorBmumuBd.push_back(track1ELBd); // temp
                trackVectorBmumuBd.push_back(track2ELBd); // temp
                trackVectorBmumuBd.push_back(track3ELBd); // temp
                trackVectorBmumuBd.push_back(track4ELBd); // temp
                
                
                //                 if(m_doKstar_KaonPionVertexFit){
                ElementLink<xAOD::TrigBphysContainer> trigPartBdEL(*m_trigBphysColl_kStarxAOD,KstarIndex);
                //TrigL2Bphys* trigPartBmumuBd = new TrigL2Bphys(0.0, 0.0, 0.0, TrigL2Bphys::BDKSTMUMU, xMuMuMass, trigPartBdEL);
                xAOD::TrigBphys * trigPartBmumuBd = new xAOD::TrigBphys();
                trigPartBmumuBd->makePrivateStore();
                trigPartBmumuBd->initialise(0.0, 0.0, 0.0, xAOD::TrigBphys::BDKSTMUMU, xMuMuMass,trigPartBdEL, xAOD::TrigBphys::L2);
                
                trigPartBmumuBd->addTrackParticleLink(track1ELBd);
                trigPartBmumuBd->addTrackParticleLink(track2ELBd);
                trigPartBmumuBd->addTrackParticleLink(track3ELBd);
                trigPartBmumuBd->addTrackParticleLink(track4ELBd);
                m_trigBphysColl_bxAOD->push_back(trigPartBmumuBd);
                //                 }else{
                //                     ElementLink<TrigL2BphysContainer> trigPartBdEL(*m_trigBphysColl_kStar,KstarIndex);
                //                     TrigL2Bphys* trigPartBmumuBd = new TrigL2Bphys(0., 0., 0., TrigL2Bphys::BDKSTMUMU, xMuMuMass, trigPartBdEL);
                //                     trigPartBmumuBd->addTrack(track1ELBd);
                //                     trigPartBmumuBd->addTrack(track2ELBd);
                //                     trigPartBmumuBd->addTrack(track3ELBd);
                //                     trigPartBmumuBd->addTrack(track4ELBd);
                //                     m_trigBphysColl_b->push_back(trigPartBmumuBd);
                //                 }
                
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Kstar's Pion and Kaon Candidates accepted with K* mass: " << xMass << "MeV"
                    << " and with Bd mass : " << xMuMuMass << "MeV"
                    << " kaon particle with pt: "<< trk3->pt()
                    << " pion charged particle with pt: "<< trk4->pt()
                    << endmsg;
                if(msgLvl() <= MSG::DEBUG)
                    msg() << MSG::DEBUG << " Added Bd particle to Bphys particle collection   "
                    //<< ", eta=" trigPartBmumuKplus->eta()
                    //<< ", phi= " << trigPartBmumuKplus->phi()
                    << ", mass= " << trigPartBmumuBd->mass()
                    << ", particle type= " << trigPartBmumuBd->particleType()
                    << endmsg;
                m_mon_BdMuMuKs_n++;
                m_mon_BdMuMuKs_InvMassBd_Ks.push_back((xMass*0.001));
                m_mon_BdMuMuKs_InvMass_Bd.push_back((xMuMuMass*0.001));
                m_mon_BdMuMuKs_PtKs_Ka.push_back(trk3->pt());
                m_mon_BdMuMuKs_EtaKs_Ka.push_back(trk3->eta());
                m_mon_BdMuMuKs_PhiKs_Ka.push_back(trk3->phi());
                m_mon_BdMuMuKs_PtKs_Pi.push_back(trk4->pt());
                m_mon_BdMuMuKs_EtaKs_Pi.push_back(trk4->eta());
                m_mon_BdMuMuKs_PhiKs_Pi.push_back(trk4->phi());
                
            }else{// end of tighter mass cuts
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Bd rejected due to Bd mass cut (no vertexing) " << endmsg;
            }
            
        } // end of no vertexing part
        
        
    } else { // end of if Kstar and Bd successful mass cuts
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Bd rejected due to K* or Bd mass cut " << endmsg;
    }

    
    
} // checkBdMuMuKstar new

//NEW
void TrigL2BMuMuXFex::checkBsMuMuPhi(const xAOD::L2CombinedMuon* mu1, const xAOD::L2CombinedMuon* mu2, const xAOD::TrackParticle* trk3,
                                     const xAOD::TrackParticle* trk4, const xAOD::TrackParticleContainer* trkCollection, int iTrk3, int iTrk4){
    double xMass = XMass(trk3,trk4, bS_to_Phi);
    double xMuMuMass = XMuMuMass(mu1->idTrack(), mu2->idTrack(),trk3,trk4, bS_to_Phi);
    
    if ((xMass > m_lowerPhi1020_KaonMassCut && xMass < m_upperPhi1020_KaonMassCut) && (xMuMuMass > m_lowerBs_Phi1020MuMuMassCut && xMuMuMass < m_upperBs_Phi1020MuMuMassCut)) {
    // Now make L2Bphys object - initial addon
        //TrigL2Bphys* trigPartBmumuPhi = new TrigL2Bphys(0., 0., 0., TrigL2Bphys::PHIKK, xMass);
        xAOD::TrigBphys * trigPartBmumuPhi= new xAOD::TrigBphys();
        trigPartBmumuPhi->makePrivateStore();
        trigPartBmumuPhi->initialise(0., 0., 0., xAOD::TrigBphys::PHIKK, xMass,xAOD::TrigBphys::L2);
    if(msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << " Create Phi1020 Bphys particle with roIId - more RoIs => " << trigPartBmumuPhi->roiId() << " mass " << xMass
        << " phi, eta - more RoIs => " << trigPartBmumuPhi->phi() << " " << trigPartBmumuPhi->eta() << " vertex type " << trigPartBmumuPhi->particleType()
        << endmsg;
        }
        
        // Store links to the tracks forming bphys particle
        ElementLinkVector<xAOD::TrackParticleContainer> trackVectorBmumuPhi;
        ElementLink<xAOD::TrackParticleContainer> track3EL(*trkCollection, iTrk3);
        ElementLink<xAOD::TrackParticleContainer> track4EL(*trkCollection, iTrk4);
        
        if ( msgLvl() <= MSG::VERBOSE ){
            msg() << MSG::VERBOSE << "Just check track links... " << endmsg;
            msg() << MSG::VERBOSE << "Track 1 pT " << (*track3EL)->pt() << " eta: " << (*track3EL)->eta() << " phi: " << (*track3EL)->phi() << endmsg;
            msg() << MSG::VERBOSE << "Track 2 pT " << (*track4EL)->pt() << " eta: " << (*track4EL)->eta() << " phi: " << (*track4EL)->phi() << endmsg;
        }
        trackVectorBmumuPhi.push_back(track3EL); // temp
        trackVectorBmumuPhi.push_back(track4EL); // temp
        
        trigPartBmumuPhi->addTrackParticleLink(track3EL);
        trigPartBmumuPhi->addTrackParticleLink(track4EL);
        // end of initial addon
        
        if(trk3->pt()*trk3->charge() > 0) {
            //       if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Phi's Pion and Kaon Candidates accepted with K* mass: " << xMass << "MeV"
            //                                            << " and with Bd mass : " << xMuMuMass << "MeV"
            //                                          << " positive charged particle with pt: "<< trk3->pt()
            //                                          << " negative charged particle with pt: "<< trk4->pt()
            //                                          << endmsg;
            m_mon_BsMuMuPhi_InvMassBs_Phi.push_back((xMass*0.001));
            m_mon_BsMuMuPhi_InvMass_Bs.push_back((xMuMuMass*0.001));
            m_mon_BsMuMuPhi_PtPhi_Kp.push_back(trk3->pt());
            m_mon_BsMuMuPhi_EtaPhi_Kp.push_back(trk3->eta());
            m_mon_BsMuMuPhi_PhiPhi_Kp.push_back(trk3->phi());
            m_mon_BsMuMuPhi_PtPhi_Km.push_back(trk4->pt());
            m_mon_BsMuMuPhi_EtaPhi_Km.push_back(trk4->eta());
            m_mon_BsMuMuPhi_PhiPhi_Km.push_back(trk4->phi());
        }else{
            //       if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Phi's Pion and Kaon Candidates accepted with K* mass: " << xMass << "MeV"
            //                                            << " and with Bd mass : " << xMuMuMass << "MeV"
            //                                            << " positive charged particle with pt: "<< trk4->pt()
            //                                            << " negative charged particle with pt: "<< trk3->pt()
            //                                              <<endmsg;
            m_mon_BsMuMuPhi_InvMassBs_Phi.push_back((xMass*0.001));
            m_mon_BsMuMuPhi_InvMass_Bs.push_back((xMuMuMass*0.001));
            m_mon_BsMuMuPhi_PtPhi_Kp.push_back(trk4->pt());
            m_mon_BsMuMuPhi_EtaPhi_Kp.push_back(trk4->eta());
            m_mon_BsMuMuPhi_PhiPhi_Kp.push_back(trk4->phi());
            m_mon_BsMuMuPhi_PtPhi_Km.push_back(trk3->pt());
            m_mon_BsMuMuPhi_EtaPhi_Km.push_back(trk3->eta());
            m_mon_BsMuMuPhi_PhiPhi_Km.push_back(trk3->phi());
        }
        bool goodPhi = false;
        //K* vertexing & mass cut
        if(m_doPhi1020_KaonKaonVertexFit && m_doVertexFit) {
            bool vertex = true;
            TrigL2Vertex* phia1020Vtx = NULL;
            TrigVertex*   mother_PhiVtx = NULL;
            // timer
            if(timerSvc()) m_VtxFitTimer->resume();
            phia1020Vtx = new TrigL2Vertex();
            StatusCode status;
            status = m_vertexingTool->addTrack(trk3->track(),phia1020Vtx,Trk::kaon);
            if( status.isFailure() ) {
                //delete phia1020Vtx;
                //continue;
                vertex = false;
            }
            if(vertex) status = m_vertexingTool->addTrack(trk4->track(),phia1020Vtx,Trk::kaon);
            if( status.isFailure() ) {
                //delete phia1020Vtx;
                //continue;
                vertex = false;
            }
            //////////////
            //vertex fit
            if(vertex) status = m_L2vertFitter->fit(phia1020Vtx);
            if(status.isSuccess() && phia1020Vtx != NULL) status = m_vertexingTool->calculateInvariantMass(phia1020Vtx);
            if(status.isFailure() || phia1020Vtx == NULL){
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Phi vertex fit failed!" << endmsg;
                delete phia1020Vtx;
                if(timerSvc()) m_VtxFitTimer->pause();
                //continue; // for performance test not continue?
                vertex = false;
            }else{
                mother_PhiVtx = m_vertexingTool->createTrigVertex(phia1020Vtx);
                status = m_vertexingTool->createMotherParticle(phia1020Vtx);
                if(status.isSuccess()){
                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Phi created, with pt: " << phia1020Vtx->m_getMotherTrack()->pT() << endmsg;
                }
                if(mother_PhiVtx == NULL){
                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  Vertex fit failed for particle candidate tracks " << trk3
                        <<" and "<<trk4<< endmsg;
                    if(timerSvc()) m_VtxFitTimer->pause();
                    //continue; // for performance test not continue?
                    vertex = false;
                }else{
                    if(msgLvl() <= MSG::DEBUG) {
                        msg() << MSG::DEBUG << " Phi vertex fit: x = " <<mother_PhiVtx->x()
                        <<", y = " <<mother_PhiVtx->y()
                        <<", z = " <<mother_PhiVtx->z()
                        <<", mass = " <<mother_PhiVtx->mass()
                        <<", chi2 = " <<mother_PhiVtx->chi2()
                        <<", ndof = " <<mother_PhiVtx->ndof() /// (number degrees of freedom)
                        <<", massVariance = " <<mother_PhiVtx->massVariance()
                        << endmsg;
                    }
                    if(mother_PhiVtx->massVariance() != 0) {
                        if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  Phi mass pull = " << (mother_PhiVtx->mass()-PHIMASS/mother_PhiVtx->massVariance()) << endmsg;
                    }else{
                        if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " Phi mass variance is zero, can't calculate the pull" << endmsg;
                    }
                    
                    /// add vertex results to L2Bphys
                    trigPartBmumuPhi->setFitmass(mother_PhiVtx->mass());
                    trigPartBmumuPhi->setFitchi2(mother_PhiVtx->chi2());
                    trigPartBmumuPhi->setFitndof(mother_PhiVtx->ndof());
                    trigPartBmumuPhi->setFitx(mother_PhiVtx->x());
                    trigPartBmumuPhi->setFity(mother_PhiVtx->y());
                    trigPartBmumuPhi->setFitz(mother_PhiVtx->z());
                    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " SigmaX =  SigmaY =  SigmaZ = "
                        << sqrt(mother_PhiVtx->cov()[0]) << " "
                        << sqrt(mother_PhiVtx->cov()[2]) << " "
                        << sqrt(mother_PhiVtx->cov()[5])
                        << endmsg;
                    
                    // cut on chi2 and invariant mass of bplus after vertexing.
                    if(phia1020Vtx->chi2() < m_phi1020VtxChi2Cut &&
                       phia1020Vtx->mass() > m_lowerPhi1020VtxMassCut && phia1020Vtx->mass() < m_upperPhi1020VtxMassCut)
                    {
                        if(msgLvl() <= MSG::DEBUG)  {
                            msg() << MSG::DEBUG << " Phi vertex fit success: x= "
                            <<            phia1020Vtx->m_getParametersVector()[0] //x
                            << ", y= " << phia1020Vtx->m_getParametersVector()[1] //y
                            << ", z= " << phia1020Vtx->m_getParametersVector()[2] //z
                            << ", chi2= " << phia1020Vtx->chi2()
                            <<", ndof = " <<phia1020Vtx->ndof()
                            << endmsg;
                        }
                        goodPhi = true;
                        // GREAT -> store daughter particle info
                        m_VertexColl->push_back(mother_PhiVtx);
                        if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Added Phi vertex to vertex collection" << endmsg;
                        int index = m_VertexColl->size()-1;
                        ElementLink<TrigVertexCollection> PhiVertexEL(*m_VertexColl,index);
                        
                        // monitoring
                        m_mon_BsMuMuPhi_VtxMassBs_Phi.push_back((phia1020Vtx->mass())*0.001);
                        m_mon_BsMuMuPhi_Chi2toNDoFBs_Phi.push_back((phia1020Vtx->chi2())/(phia1020Vtx->ndof()));
                        if(trk3->pt() * trk3->charge() > 0) {
                            m_mon_BsMuMuPhi_VtxPtPhi_Kp.push_back((trk3->pt())*0.001);
                            m_mon_BsMuMuPhi_VtxEtaPhi_Kp.push_back(trk3->eta());
                            m_mon_BsMuMuPhi_VtxPhiPhi_Kp.push_back(trk3->phi());
                            m_mon_BsMuMuPhi_VtxPtPhi_Km.push_back((trk4->pt())*0.001);
                            m_mon_BsMuMuPhi_VtxEtaPhi_Km.push_back(trk4->eta());
                            m_mon_BsMuMuPhi_VtxPhiPhi_Km.push_back(trk4->phi());
                        }else{
                            m_mon_BsMuMuPhi_VtxPtPhi_Kp.push_back((trk4->pt())*0.001);
                            m_mon_BsMuMuPhi_VtxEtaPhi_Kp.push_back(trk4->eta());
                            m_mon_BsMuMuPhi_VtxPhiPhi_Kp.push_back(trk4->phi());
                            m_mon_BsMuMuPhi_VtxPtPhi_Km.push_back((trk3->pt())*0.001);
                            m_mon_BsMuMuPhi_VtxEtaPhi_Km.push_back(trk3->eta());
                            m_mon_BsMuMuPhi_VtxPhiPhi_Km.push_back(trk3->phi());
                        }
                        if(msgLvl() <= MSG::DEBUG) {
                            msg() << MSG::DEBUG << " Finnished Bphys daughter particle with fitmass" << trigPartBmumuPhi->fitmass() << " mass " << trigPartBmumuPhi->mass()
                            << " phi, eta - more RoIs => 0" << trigPartBmumuPhi->phi() << " " << trigPartBmumuPhi->eta() << " vertex type " << trigPartBmumuPhi->particleType()
                            << endmsg;
                        }
                        
                    }else{
                        if(msgLvl() <= MSG::DEBUG) {
                            msg() << MSG::DEBUG << " Phi rejected by mass or chi2 cuts after vertex fit!"
                            << ", chi2= " << phia1020Vtx->chi2()
                            << endmsg;
                        }
                        if(timerSvc()) m_VtxFitTimer->pause();
                        //continue; // for performance test not continue
                        vertex = false;
                        delete mother_PhiVtx;
                    }
                } // end of successful fit condition "mother"
                //int index = m_VertexColl->size()-1;
                //ElementLink<TrigVertexCollection> BplusVertexEL(*m_VertexColl,index);
                delete phia1020Vtx;
            } // end of successful fit condition initial vertex
            if(timerSvc()) m_VtxFitTimer->pause();
        } // end Phi_KaonPion vertex fit
        
        m_trigBphysColl_phixAOD->push_back(trigPartBmumuPhi);
        int PhiIndex = m_trigBphysColl_phixAOD->size() - 1;
        
        if(m_doPhi1020_KaonKaonVertexFit && goodPhi) {
            if(msgLvl() <= MSG::DEBUG) {
                msg() << MSG::DEBUG << " Well done in Phi vertexing!!!" << endmsg;
                //    continue; // OBSOLETE, done inside vertexing
            }
            
            // Bd+/- vertexing & mass cut
            if(m_doBs_Phi1020MuMuVertexFit && m_doVertexFit){
                if(timerSvc()) m_VtxFitTimer->resume();
                bool vertex = true;
                TrigL2Vertex* baSVtx = NULL;
                TrigVertex* mother_BsVtx=NULL;
                baSVtx = new TrigL2Vertex();
                StatusCode status;
                status = m_vertexingTool->addTrack(mu1->idTrack()->track(),baSVtx,Trk::muon);   //
                if(status.isFailure()){
                    //delete baSVtx;
                    if(timerSvc()) m_VtxFitTimer->pause();
                    //continue;
                    vertex = false;
                }
                if (vertex) status = m_vertexingTool->addTrack(mu2->idTrack()->track(),baSVtx,Trk::muon);   //
                if(status.isFailure()){
                    //delete baSVtx;
                    if(timerSvc()) m_VtxFitTimer->pause();
                    //continue;
                    vertex = false;
                }
                if (vertex) status = m_vertexingTool->addTrack(trk3->track(),baSVtx,Trk::kaon);
                if(status.isFailure()){
                    //delete baSVtx;
                    if(timerSvc()) m_VtxFitTimer->pause();
                    //continue;
                    vertex = false;
                }
                if (vertex) status = m_vertexingTool->addTrack(trk4->track(),baSVtx,Trk::kaon);
                if(status.isFailure()){
                    //delete baSVtx;
                    if(timerSvc()) m_VtxFitTimer->pause();
                    //continue;
                    vertex = false;
                }
                //ElementLink<TrigL2BphysContainer> trigPartBsEL(*m_trigBphysColl_phi,PhiIndex);
                ElementLink<xAOD::TrigBphysContainer> trigPartBsEL(*m_trigBphysColl_phixAOD,PhiIndex);
                //TrigL2Bphys* trigPartBmumuBs = new TrigL2Bphys(0.0, 0.0, 0.0, TrigL2Bphys::BSPHIMUMU, xMuMuMass, trigPartBsEL);
                xAOD::TrigBphys *trigPartBmumuBs = new xAOD::TrigBphys();
                trigPartBmumuBs->makePrivateStore();
                trigPartBmumuBs->initialise(0.0, 0.0, 0.0, xAOD::TrigBphys::BSPHIMUMU, xMuMuMass, trigPartBsEL,xAOD::TrigBphys::L2);
                
                //vertex fit
                if (vertex) status = m_L2vertFitter->fit(baSVtx);
                if(status.isSuccess()) status = m_vertexingTool->calculateInvariantMass(baSVtx);
                if(status.isFailure()){
                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Bs vertex fit failed!" << endmsg;
                    delete baSVtx;
                    delete trigPartBmumuBs;
                    //continue; // for performance test not continue
                    vertex = false;
                }else{
                    mother_BsVtx = m_vertexingTool->createTrigVertex(baSVtx);
                    status = m_vertexingTool->createMotherParticle(baSVtx);
                    if(status.isSuccess()){
                        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Bs created, with pt: " << baSVtx->m_getMotherTrack()->pT() << endmsg;
                    }
                    if(mother_BsVtx == NULL){
                        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  Vertex fit failed for particle candidate track " << trk3
                            <<" and "<<trk4<< endmsg;
                        vertex = false;
                        delete trigPartBmumuBs;
                        //continue; // for performance test not continue
                    }else{
                        if(msgLvl() <= MSG::DEBUG) {
                            msg() << MSG::DEBUG << " Bs vertex fit: x = " <<mother_BsVtx->x()
                            <<", y = " <<mother_BsVtx->y()
                            <<", z = " <<mother_BsVtx->z()
                            <<", mass = " <<mother_BsVtx->mass()
                            <<", chi2 = " <<mother_BsVtx->chi2()
                            <<", ndof = " <<mother_BsVtx->ndof() /// (number degrees of freedom)
                            <<", massVariance = " <<mother_BsVtx->massVariance()
                            << endmsg;
                        }
                        if(mother_BsVtx->massVariance() != 0) {
                            if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  Bs mass pull = " << (mother_BsVtx->mass()-BSMASS/mother_BsVtx->massVariance()) << endmsg;
                        }else{
                            if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " Bs mass variance is zero, can't calculate the pull" << endmsg;
                        }
                        
                        /// add vertex results to L2Bphys
                        trigPartBmumuBs->setFitmass(mother_BsVtx->mass());
                        trigPartBmumuBs->setFitchi2(mother_BsVtx->chi2());
                        trigPartBmumuBs->setFitndof(mother_BsVtx->ndof());
                        trigPartBmumuBs->setFitx(mother_BsVtx->x());
                        trigPartBmumuBs->setFity(mother_BsVtx->y());
                        trigPartBmumuBs->setFitz(mother_BsVtx->z());
                        
                        if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " SigmaX =  SigmaY =  SigmaZ = "
                            << sqrt(mother_BsVtx->cov()[0]) << " "
                            << sqrt(mother_BsVtx->cov()[2]) << " "
                            << sqrt(mother_BsVtx->cov()[5])
                            << endmsg;
                        
                        // cut on chi2 and invariant mass of Bd after vertexing.
                        if(baSVtx->chi2() < m_bSVtxChi2Cut && baSVtx->mass() > m_lowerBsVtxMassCut && baSVtx->mass() < m_upperBsVtxMassCut)
                        {
                            if(msgLvl() <= MSG::DEBUG)  {
                                msg() << MSG::DEBUG << " Bs vertex fit success: x= "
                                <<            baSVtx->m_getParametersVector()[0] //x
                                << ", y= " << baSVtx->m_getParametersVector()[1] //y
                                << ", z= " << baSVtx->m_getParametersVector()[2] //z
                                << ", chi2= " << baSVtx->chi2()
                                <<", ndof = " <<baSVtx->ndof()
                                << endmsg;
                            }
                            
                            // PERFECT -> Bphys particle was stored in collection
                            m_VertexColl->push_back(mother_BsVtx);
                            m_trigBphysColl_bxAOD->push_back(trigPartBmumuBs);
                            if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Added Bs vertex to vertex collection" << endmsg;
                            // monitoring of successfuly stored particles
                            m_mon_BsMuMuPhi_n++;
                            m_mon_Acceptance.push_back( ACCEPT_BphysCollParticle );
                            m_mon_BsMuMuPhi_VtxMass_Bs.push_back((baSVtx->mass())*0.001);
                            m_mon_BsMuMuPhi_Chi2toNDoF_Bs.push_back((baSVtx->chi2())/(baSVtx->ndof()));
                            if(trk3->pt() * trk3->charge() > 0) {
                                m_mon_BsMuMuPhi_VtxPtBs_Kp.push_back((trk3->pt())*0.001);
                                m_mon_BsMuMuPhi_VtxEtaBs_Kp.push_back(trk3->eta());
                                m_mon_BsMuMuPhi_VtxPhiBs_Kp.push_back(trk3->phi());
                                m_mon_BsMuMuPhi_VtxPtBs_Km.push_back((trk4->pt())*0.001);
                                m_mon_BsMuMuPhi_VtxEtaBs_Km.push_back(trk4->eta());
                                m_mon_BsMuMuPhi_VtxPhiBs_Km.push_back(trk4->phi());
                            }else{
                                m_mon_BsMuMuPhi_VtxPtBs_Kp.push_back((trk4->pt())*0.001);
                                m_mon_BsMuMuPhi_VtxEtaBs_Kp.push_back(trk4->eta());
                                m_mon_BsMuMuPhi_VtxPhiBs_Kp.push_back(trk4->phi());
                                m_mon_BsMuMuPhi_VtxPtBs_Km.push_back((trk3->pt())*0.001);
                                m_mon_BsMuMuPhi_VtxEtaBs_Km.push_back(trk3->eta());
                                m_mon_BsMuMuPhi_VtxPhiBs_Km.push_back(trk3->phi());
                            }
                            if(msgLvl() <= MSG::DEBUG) {
                                msg() << MSG::DEBUG << " Finnished Bphys particle with fitmass" << trigPartBmumuBs->fitmass() << " mass " << trigPartBmumuBs->mass()
                                << " phi, eta - more RoIs => 0" << trigPartBmumuBs->phi() << " " << trigPartBmumuBs->eta() << " vertex type " << trigPartBmumuBs->particleType()
                                << endmsg;
                            }
                            
                        }else{
                            if(msgLvl() <= MSG::DEBUG) {
                                msg() << MSG::DEBUG << " Bs rejected by mass or chi2 cuts after vertex fit!"
                                << ", chi2= " << baSVtx->chi2()
                                << endmsg;
                            }
                            delete trigPartBmumuBs;
                            //continue; // for performance test not continue
                            vertex = false;
                        }
                    } // end of successful fit condition "mother"
                    if(vertex) {
                        int index = m_VertexColl->size()-1;
                        ElementLink<TrigVertexCollection> BsVertexEL(*m_VertexColl,index);
                    } else {
                        delete mother_BsVtx;
                    }
                    delete baSVtx;
                } // end of successful fit condition initial vertex
                
                
            }   // Bs -> Phi Mu Mu vertexing
        } // end of test over failed
        //////////////////////////////// vertexing off ///////////////////////////////////////////////
        if(!m_doBs_Phi1020MuMuVertexFit || !m_doVertexFit){
            if ((xMuMuMass > m_lowerBs_Phi1020MuMuMassCutVtxOff && xMuMuMass < m_upperBs_Phi1020MuMuMassCutVtxOff)) {
                
                // Store links to the tracks forming bphys particle
                ElementLinkVector<xAOD::TrackParticleContainer> trackVectorBmumuBs;
                ElementLink<xAOD::TrackParticleContainer> track1ELBs = mu1->idTrackLink();
                ElementLink<xAOD::TrackParticleContainer> track2ELBs = mu2->idTrackLink();
                ElementLink<xAOD::TrackParticleContainer> track3ELBs(*trkCollection, iTrk3);
                ElementLink<xAOD::TrackParticleContainer> track4ELBs(*trkCollection, iTrk4);
                
                if ( msgLvl() <= MSG::VERBOSE ){
                    msg() << MSG::VERBOSE << "Just check track links... " << endmsg;
                    msg() << MSG::VERBOSE << "Muon 1 pT " << (*track1ELBs)->pt() << " eta: " << (*track1ELBs)->eta() << " phi: " << (*track1ELBs)->phi() << endmsg;
                    msg() << MSG::VERBOSE << "Muon 2 pT " << (*track2ELBs)->pt() << " eta: " << (*track2ELBs)->eta() << " phi: " << (*track2ELBs)->phi() << endmsg;
                    msg() << MSG::VERBOSE << "Track 1 pT " << (*track3ELBs)->pt() << " eta: " << (*track3ELBs)->eta() << " phi: " << (*track3ELBs)->phi() << endmsg;
                    msg() << MSG::VERBOSE << "Track 2 pT " << (*track4ELBs)->pt() << " eta: " << (*track4ELBs)->eta() << " phi: " << (*track4ELBs)->phi() << endmsg;
                }
                trackVectorBmumuBs.push_back(track1ELBs); // temp
                trackVectorBmumuBs.push_back(track2ELBs); // temp
                trackVectorBmumuBs.push_back(track3ELBs); // temp
                trackVectorBmumuBs.push_back(track4ELBs); // temp
                
                
                //         if(m_doPhi1020_KaonKaonVertexFit){
                //                ElementLink<TrigL2BphysContainer> trigPartBsEL(*m_trigBphysColl_phi,PhiIndex);
                //TrigL2Bphys* trigPartBmumuBs = new TrigL2Bphys(0.0, 0.0, 0.0, TrigL2Bphys::BSPHIMUMU, xMuMuMass, trigPartBsEL);
                ElementLink<xAOD::TrigBphysContainer> trigPartBsEL(*m_trigBphysColl_phixAOD,PhiIndex);
                xAOD::TrigBphys *trigPartBmumuBs = new xAOD::TrigBphys();
                trigPartBmumuBs->makePrivateStore();
                trigPartBmumuBs->initialise(0.0, 0.0, 0.0, xAOD::TrigBphys::BSPHIMUMU, xMuMuMass, trigPartBsEL,xAOD::TrigBphys::L2);
                
                trigPartBmumuBs->addTrackParticleLink(track1ELBs);
                trigPartBmumuBs->addTrackParticleLink(track2ELBs); 
                trigPartBmumuBs->addTrackParticleLink(track3ELBs); 
                trigPartBmumuBs->addTrackParticleLink(track4ELBs); 
                m_trigBphysColl_bxAOD->push_back(trigPartBmumuBs);
                //         }else{
                // //           m_trigBphysColl_phi->push_back(trigPartBmumuPhi);
                // //           int PhiIndex = m_trigBphysColl_phi->size() - 1;
                // //           ElementLink<TrigL2BphysContainer> trigPartBsEL(*m_trigBphysColl_phi,PhiIndex);
                //           
                //           TrigL2Bphys* trigPartBmumuBs = new TrigL2Bphys(0., 0., 0., TrigL2Bphys::BSPHIMUMU, xMuMuMass/*, trigPartBsEL*/);
                //           trigPartBmumuBs->addTrackParticleLink(track1ELBs); 
                //             trigPartBmumuBs->addTrackParticleLink(track2ELBs); 
                //             trigPartBmumuBs->addTrackParticleLink(track3ELBs); 
                //             trigPartBmumuBs->addTrackParticleLink(track4ELBs); 
                //           m_trigBphysColl_b->push_back(trigPartBmumuBs);
                //         }
                m_mon_BsMuMuPhi_n++;
                if(trk3->pt() * trk3->charge() > 0) {
                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Phi's Kplus and Kminus Candidates accepted with Phi mass: " << xMass << "MeV"  
                        << " and with Phi1020 mass : " << xMuMuMass << "MeV"  
                        << " positive charged particle with pt: "<< trk3->pt() * trk3->charge()
                        
                        << " negative charged particle with pt: "<< trk4->pt() *trk4->charge()
                        << endmsg;
                    m_mon_BsMuMuPhi_InvMassBs_Phi.push_back((xMass*0.001));
                    m_mon_BsMuMuPhi_InvMass_Bs.push_back((xMuMuMass*0.001));
                    m_mon_BsMuMuPhi_PtPhi_Kp.push_back(trk3->pt());
                    m_mon_BsMuMuPhi_EtaPhi_Kp.push_back(trk3->eta());
                    m_mon_BsMuMuPhi_PhiPhi_Kp.push_back(trk3->phi());
                    m_mon_BsMuMuPhi_PtPhi_Km.push_back(trk4->pt());
                    m_mon_BsMuMuPhi_EtaPhi_Km.push_back(trk4->eta());
                    m_mon_BsMuMuPhi_PhiPhi_Km.push_back(trk4->phi());
                }else{
                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Phi's Pion and Kaon Candidates accepted with K* mass: " << xMass << "MeV"  
                        << " and with Bd mass : " << xMuMuMass << "MeV"  
                        << " positive charged particle with pt: "<< trk4->pt() *trk4->charge()
                        << " negative charged particle with pt: "<< trk3->pt() * trk3->charge()
                        <<endmsg;
                    m_mon_BsMuMuPhi_InvMassBs_Phi.push_back((xMass*0.001));
                    m_mon_BsMuMuPhi_InvMass_Bs.push_back((xMuMuMass*0.001));
                    m_mon_BsMuMuPhi_PtPhi_Kp.push_back(trk4->pt() *trk4->charge());
                    m_mon_BsMuMuPhi_EtaPhi_Kp.push_back(trk4->eta());
                    m_mon_BsMuMuPhi_PhiPhi_Kp.push_back(trk4->phi());
                    m_mon_BsMuMuPhi_PtPhi_Km.push_back(trk3->pt() * trk3->charge());
                    m_mon_BsMuMuPhi_EtaPhi_Km.push_back(trk3->eta());
                    m_mon_BsMuMuPhi_PhiPhi_Km.push_back(trk3->phi());
                }
                if(msgLvl() <= MSG::DEBUG)
                    msg() << MSG::DEBUG << " Added Bs particle to Bphys particle collection   " 
                    //<< ", eta=" trigPartBmumuKplus->eta()   
                    //<< ", phi= " << trigPartBmumuKplus->phi()
                    << ", mass= " << trigPartBmumuBs->mass() 
                    << ", particle type= " << trigPartBmumuBs->particleType() 
                    << endmsg;
            } else {// end of tighter mass cuts
                if(msgLvl() <= MSG::DEBUG) 
                    msg() << MSG::DEBUG << " Bs rejected due to Bs mass cut (no vertexing) " << endmsg;
            } 
            
        } // end of no vertexing part
        
        
        } else { // end of if Phi and Bs successful mass cuts
            if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Bs rejected due to Phi or Bs mass cut " << endmsg;
        }


} // checkBsMuMuPhi new

//NEW
void TrigL2BMuMuXFex::checkLbMuMuLambda(const xAOD::L2CombinedMuon* mu1, const xAOD::L2CombinedMuon* mu2, const xAOD::TrackParticle* trk3,
                                        const xAOD::TrackParticle* trk4, const xAOD::TrackParticleContainer* trkCollection, int iTrk3, int iTrk4) {
    
    double xMass = 0;
    double xMuMuMass = 0;
    xMass = XMass(trk3,trk4, lB_to_L);
    xMuMuMass = XMuMuMass(mu1->idTrack(), mu2->idTrack(),trk3,trk4, lB_to_L);
    
    if ((xMass > m_lowerLambda_PrPiMassCut && xMass < m_upperLambda_PrPiMassCut) && (xMuMuMass > m_lowerLb_LambdaMuMuMassCut && xMuMuMass < m_upperLb_LambdaMuMuMassCut)) {
        // Now make L2Bphys object - initial addon
        //        TrigL2Bphys* trigPartBmumuL = new TrigL2Bphys(0., 0., 0., TrigL2Bphys::PHIKK, xMass);
        xAOD::TrigBphys * trigPartBmumuL = new xAOD::TrigBphys();
        trigPartBmumuL->makePrivateStore();
        trigPartBmumuL->initialise(0., 0., 0., xAOD::TrigBphys::PHIKK, xMass,xAOD::TrigBphys::L2);
        if(msgLvl() <= MSG::DEBUG) {
            msg() << MSG::DEBUG << " Create Lambda Bphys particle with roIId - more RoIs => " << trigPartBmumuL->roiId() << " mass " << xMass
            << " phi, eta - more RoIs => " << trigPartBmumuL->phi() << " " << trigPartBmumuL->eta() << " vertex type " << trigPartBmumuL->particleType()
            << endmsg;
        }
        
        // Store links to the tracks forming bphys particle
        ElementLinkVector<xAOD::TrackParticleContainer> trackVectorBmumuL;
        ElementLink<xAOD::TrackParticleContainer> track3EL(*trkCollection, iTrk3);
        ElementLink<xAOD::TrackParticleContainer> track4EL(*trkCollection, iTrk4);
        
        if ( msgLvl() <= MSG::VERBOSE ){
            msg() << MSG::VERBOSE << "Just check track links... " << endmsg;
            msg() << MSG::VERBOSE << "Track 1 pT " << (*track3EL)->pt() << " eta: " << (*track3EL)->eta() << " phi: " << (*track3EL)->phi() << endmsg;
            msg() << MSG::VERBOSE << "Track 2 pT " << (*track4EL)->pt() << " eta: " << (*track4EL)->eta() << " phi: " << (*track4EL)->phi() << endmsg;
        }
        trackVectorBmumuL.push_back(track3EL); // temp
        trackVectorBmumuL.push_back(track4EL); // temp
        
        trigPartBmumuL->addTrackParticleLink(track3EL);
        trigPartBmumuL->addTrackParticleLink(track4EL);
        // end of initial addon
        
        //     if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Lambda's Proton and Pion Candidates accepted with L mass: " << xMass << "MeV"
        //                                         << " and with Lb mass : " << xMuMuMass << "MeV"
        //                                       << " proton particle with pt: "<< trk3->pt()
        //                                       << " pion charged particle with pt: "<< trk4->pt()
        //                                       << endmsg;
        m_mon_LbMuMuL_InvMassLb_L.push_back((xMass*0.001));
        m_mon_LbMuMuL_InvMass_Lb.push_back((xMuMuMass*0.001));
        m_mon_LbMuMuL_PtL_Pr.push_back(trk3->pt());
        m_mon_LbMuMuL_EtaL_Pr.push_back(trk3->eta());
        m_mon_LbMuMuL_PhiL_Pr.push_back(trk3->phi());
        m_mon_LbMuMuL_PtL_Pi.push_back(trk4->pt());
        m_mon_LbMuMuL_EtaL_Pi.push_back(trk4->eta());
        m_mon_LbMuMuL_PhiL_Pi.push_back(trk4->phi());
        
        
        bool goodL = false;
        //Lambda vertexing & mass cut
        if(m_doLambda_ProtonPionVertexFit && m_doVertexFit) {
            bool vertex = true;
            TrigL2Vertex* lambdaaVtx = NULL;
            TrigVertex*   mother_LVtx = NULL;
            // timer
            if(timerSvc()) m_VtxFitTimer->resume();
            lambdaaVtx = new TrigL2Vertex();
            StatusCode status;
            
            status = m_vertexingTool->addTrack(trk3->track(),lambdaaVtx,Trk::proton);
            if( status.isFailure() ) {
                //delete lambdaaVtx;
                //continue;
                vertex = false;
            }
            if(vertex) status = m_vertexingTool->addTrack(trk4->track(),lambdaaVtx,Trk::pion);
            if( status.isFailure() ) {
                //delete lambdaaVtx;
                //continue;
                vertex = false;
            }
            
            //////////////
            //vertex fit
            if(vertex) status = m_L2vertFitter->fit(lambdaaVtx);
            if(status.isSuccess() && lambdaaVtx != NULL) status = m_vertexingTool->calculateInvariantMass(lambdaaVtx);
            if(status.isFailure() || lambdaaVtx == NULL){
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " L vertex fit failed!" << endmsg;
                delete lambdaaVtx;
                if(timerSvc()) m_VtxFitTimer->pause();
                //continue; // for performance test not continue?
                vertex = false;
            }else{
                mother_LVtx = m_vertexingTool->createTrigVertex(lambdaaVtx);
                status = m_vertexingTool->createMotherParticle(lambdaaVtx);
                if(status.isSuccess()){
                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " L created, with pt: " << lambdaaVtx->m_getMotherTrack()->pT() << endmsg;
                }
                if(mother_LVtx == NULL){
                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  Vertex fit failed for particle candidate tracks " << trk3
                        <<" and "<<trk4<< endmsg;
                    if(timerSvc()) m_VtxFitTimer->pause();
                    //continue; // for performance test not continue?
                    vertex = false;
                }else{
                    if(msgLvl() <= MSG::DEBUG) {
                        msg() << MSG::DEBUG << " L vertex fit: x = " <<mother_LVtx->x()
                        <<", y = " <<mother_LVtx->y()
                        <<", z = " <<mother_LVtx->z()
                        <<", mass = " <<mother_LVtx->mass()
                        <<", chi2 = " <<mother_LVtx->chi2()
                        <<", ndof = " <<mother_LVtx->ndof() /// (number degrees of freedom)
                        <<", massVariance = " <<mother_LVtx->massVariance()
                        << endmsg;
                    }
                    if(mother_LVtx->massVariance() != 0) {
                        if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  L mass pull = " << (mother_LVtx->mass()-LAMBDAMASS/mother_LVtx->massVariance()) << endmsg;
                    }else{
                        if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " L mass variance is zero, can't calculate the pull" << endmsg;
                    }
                    
                    /// add vertex results to L2Bphys
                    trigPartBmumuL->setFitmass(mother_LVtx->mass());
                    trigPartBmumuL->setFitchi2(mother_LVtx->chi2());
                    trigPartBmumuL->setFitndof(mother_LVtx->ndof());
                    trigPartBmumuL->setFitx(mother_LVtx->x());
                    trigPartBmumuL->setFity(mother_LVtx->y());
                    trigPartBmumuL->setFitz(mother_LVtx->z());
                    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " SigmaX =  SigmaY =  SigmaZ = "
                        << sqrt(mother_LVtx->cov()[0]) << " "
                        << sqrt(mother_LVtx->cov()[2]) << " "
                        << sqrt(mother_LVtx->cov()[5])
                        << endmsg;
                    
                    // cut on chi2 and invariant mass of bplus after vertexing.
                    if(lambdaaVtx->chi2() < m_lambdaVtxChi2Cut &&
                       lambdaaVtx->mass() > m_lowerLambdaVtxMassCut && lambdaaVtx->mass() < m_upperLambdaVtxMassCut)
                    {
                        if(msgLvl() <= MSG::DEBUG)  {
                            msg() << MSG::DEBUG << " L vertex fit success: x= "
                            <<            lambdaaVtx->m_getParametersVector()[0] //x
                            << ", y= " << lambdaaVtx->m_getParametersVector()[1] //y
                            << ", z= " << lambdaaVtx->m_getParametersVector()[2] //z
                            << ", chi2= " << lambdaaVtx->chi2()
                            <<", ndof = " <<lambdaaVtx->ndof()
                            << endmsg;
                        }
                        goodL = true;
                        // GREAT -> store daughter particle info
                        m_VertexColl->push_back(mother_LVtx);
                        if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Added L vertex to vertex collection" << endmsg;
                        int index = m_VertexColl->size()-1;
                        ElementLink<TrigVertexCollection> LVertexEL(*m_VertexColl,index);
                        
                        // monitoring
                        m_mon_LbMuMuL_VtxMassLb_L.push_back((lambdaaVtx->mass())*0.001);
                        m_mon_LbMuMuL_Chi2toNDoFLb_L.push_back((lambdaaVtx->chi2())/(lambdaaVtx->ndof()));
                        
                        m_mon_LbMuMuL_VtxPtL_Pr.push_back((trk3->pt())*0.001);
                        m_mon_LbMuMuL_VtxEtaL_Pr.push_back(trk3->eta());
                        m_mon_LbMuMuL_VtxPhiL_Pr.push_back(trk3->phi());
                        m_mon_LbMuMuL_VtxPtL_Pi.push_back((trk4->pt())*0.001);
                        m_mon_LbMuMuL_VtxEtaL_Pi.push_back(trk4->eta());
                        m_mon_LbMuMuL_VtxPhiL_Pi.push_back(trk4->phi());
                        
                        if(msgLvl() <= MSG::DEBUG) {
                            msg() << MSG::DEBUG << " Finnished Bphys daughter particle with fitmass" << trigPartBmumuL->fitmass() << " mass " << trigPartBmumuL->mass()
                            << " phi, eta - more RoIs => 0" << trigPartBmumuL->phi() << " " << trigPartBmumuL->eta() << " vertex type " << trigPartBmumuL->particleType()
                            << endmsg;
                        }
                        
                    }else{
                        if(msgLvl() <= MSG::DEBUG) {
                            msg() << MSG::DEBUG << " L rejected by mass or chi2 cuts after vertex fit!"
                            << ", chi2= " << lambdaaVtx->chi2()
                            << endmsg;
                        }
                        if(timerSvc()) m_VtxFitTimer->pause();
                        //continue; // for performance test not continue
                        vertex = false;
                        delete mother_LVtx;
                    }
                } // end of successful fit condition "mother"
                //int index = m_VertexColl->size()-1;
                //ElementLink<TrigVertexCollection> BplusVertexEL(*m_VertexColl,index);
                delete lambdaaVtx;
            } // end of successful fit condition initial vertex
            if(timerSvc()) m_VtxFitTimer->pause();
        } // end Lambda_ProtonPion vertex fit
        
        m_trigBphysColl_lambdaxAOD->push_back(trigPartBmumuL);
        int LIndex = m_trigBphysColl_lambdaxAOD->size() - 1;
        
        if(m_doLambda_ProtonPionVertexFit && goodL) {
            if(msgLvl() <= MSG::DEBUG) {
                msg() << MSG::DEBUG << " Well done in L vertexing!!!" << endmsg;
                //continue; // OBSOLETE, done inside vertexing
            }
            
            // Bd+/- vertexing & mass cut
            if(m_doLb_LambdaMuMuVertexFit && m_doVertexFit){
                bool vertex = true;
                if(timerSvc()) m_VtxFitTimer->resume();
                TrigL2Vertex* laBVtx = NULL;
                TrigVertex* mother_LbVtx=NULL;
                laBVtx = new TrigL2Vertex();
                StatusCode status;
                status = m_vertexingTool->addTrack(mu1->idTrack()->track(),laBVtx,Trk::muon);   //
                if(status.isFailure()){
                    //delete laBVtx;
                    if(timerSvc()) m_VtxFitTimer->pause();
                    //continue;
                    vertex = false;
                }
                if(vertex) status = m_vertexingTool->addTrack(mu2->idTrack()->track(),laBVtx,Trk::muon);   //
                if(status.isFailure()){
                    //delete laBVtx;
                    if(timerSvc()) m_VtxFitTimer->pause();
                    //continue;
                    vertex = false;
                }
                
                if(vertex) status = m_vertexingTool->addTrack(trk3->track(),laBVtx,Trk::proton);
                if(status.isFailure()){
                    //delete laBVtx;
                    if(timerSvc()) m_VtxFitTimer->pause();
                    //continue;
                    vertex = false;
                }
                if(vertex) status = m_vertexingTool->addTrack(trk4->track(),laBVtx,Trk::pion);
                if(status.isFailure()){
                    //delete laBVtx;
                    if(timerSvc()) m_VtxFitTimer->pause();
                    //continue;
                    vertex = false;
                }
                
                ElementLink<xAOD::TrigBphysContainer> trigPartLbEL(*m_trigBphysColl_lambdaxAOD,LIndex);
                //TrigL2Bphys* trigPartBmumuLb = new TrigL2Bphys(0.0, 0.0, 0.0, TrigL2Bphys::LBLMUMU, xMuMuMass, trigPartLbEL);
                xAOD::TrigBphys* trigPartBmumuLb = new xAOD::TrigBphys();
                trigPartBmumuLb->makePrivateStore();
                trigPartBmumuLb->initialise(0.0, 0.0, 0.0, xAOD::TrigBphys::LBLMUMU, xMuMuMass, trigPartLbEL,xAOD::TrigBphys::L2);

                //vertex fit
                if(vertex) status = m_L2vertFitter->fit(laBVtx);
                if(status.isSuccess()) status = m_vertexingTool->calculateInvariantMass(laBVtx);
                if(status.isFailure()){
                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Lb vertex fit failed!" << endmsg;
                    delete laBVtx;
                    delete trigPartBmumuLb;
                    //continue; // for performance test not continue
                    vertex = false;
                }else{
                    mother_LbVtx = m_vertexingTool->createTrigVertex(laBVtx);
                    status = m_vertexingTool->createMotherParticle(laBVtx);
                    if(status.isSuccess()){
                        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Lb created, with pt: " << laBVtx->m_getMotherTrack()->pT() << endmsg;
                    }
                    if(mother_LbVtx == NULL){
                        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  Vertex fit failed for particle candidate track " << trk3
                            <<" and "<<trk4<< endmsg;
                        delete trigPartBmumuLb;
                        //continue; // for performance test not continue
                        vertex = false;
                    }else{
                        if(msgLvl() <= MSG::DEBUG) {
                            msg() << MSG::DEBUG << " Lb vertex fit: x = " <<mother_LbVtx->x()
                            <<", y = " <<mother_LbVtx->y()
                            <<", z = " <<mother_LbVtx->z()
                            <<", mass = " <<mother_LbVtx->mass()
                            <<", chi2 = " <<mother_LbVtx->chi2()
                            <<", ndof = " <<mother_LbVtx->ndof() /// (number degrees of freedom)
                            <<", massVariance = " <<mother_LbVtx->massVariance()
                            << endmsg;
                        }
                        if(mother_LbVtx->massVariance() != 0) {
                            if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  Lb mass pull = " << (mother_LbVtx->mass()-LAMBDABMASS/mother_LbVtx->massVariance()) << endmsg;
                        }else{
                            if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " Lb mass variance is zero, can't calculate the pull" << endmsg;
                        }
                        /// add vertex results to L2Bphys
                        trigPartBmumuLb->setFitmass(mother_LbVtx->mass());
                        trigPartBmumuLb->setFitchi2(mother_LbVtx->chi2());
                        trigPartBmumuLb->setFitndof(mother_LbVtx->ndof());
                        trigPartBmumuLb->setFitx(mother_LbVtx->x());
                        trigPartBmumuLb->setFity(mother_LbVtx->y());
                        trigPartBmumuLb->setFitz(mother_LbVtx->z());
                        
                        if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " SigmaX =  SigmaY =  SigmaZ = "
                            << sqrt(mother_LbVtx->cov()[0]) << " "
                            << sqrt(mother_LbVtx->cov()[2]) << " "
                            << sqrt(mother_LbVtx->cov()[5])
                            << endmsg;
                        
                        // cut on chi2 and invariant mass of Bd after vertexing.
                        if(laBVtx->chi2() < m_bSVtxChi2Cut && laBVtx->mass() > m_lowerLbVtxMassCut && laBVtx->mass() < m_upperLbVtxMassCut)
                        {
                            if(msgLvl() <= MSG::DEBUG)  {
                                msg() << MSG::DEBUG << " Lb vertex fit success: x= "
                                <<            laBVtx->m_getParametersVector()[0] //x
                                << ", y= " << laBVtx->m_getParametersVector()[1] //y
                                << ", z= " << laBVtx->m_getParametersVector()[2] //z
                                << ", chi2= " << laBVtx->chi2()
                                <<", ndof = " <<laBVtx->ndof()
                                << endmsg;
                            }
                            
                            // PERFECT -> Bphys particle was stored in collection
                            m_VertexColl->push_back(mother_LbVtx);
                            m_trigBphysColl_bxAOD->push_back(trigPartBmumuLb);
                            if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Added Lb vertex to vertex collection" << endmsg;
                            
                            // monitoring of successfuly stored particles
                            m_mon_LbMuMuL_n++;
                            m_mon_Acceptance.push_back( ACCEPT_BphysCollParticle );
                            m_mon_LbMuMuL_VtxMass_Lb.push_back((laBVtx->mass())*0.001);
                            m_mon_LbMuMuL_Chi2toNDoF_Lb.push_back((laBVtx->chi2())/(laBVtx->ndof()));
                            
                            m_mon_LbMuMuL_VtxPtLb_Pr.push_back((trk3->pt())*0.001);
                            m_mon_LbMuMuL_VtxEtaLb_Pr.push_back(trk3->eta());
                            m_mon_LbMuMuL_VtxPhiLb_Pr.push_back(trk3->phi());
                            m_mon_LbMuMuL_VtxPtLb_Pi.push_back((trk4->pt())*0.001);
                            m_mon_LbMuMuL_VtxEtaLb_Pi.push_back(trk4->eta());
                            m_mon_LbMuMuL_VtxPhiLb_Pi.push_back(trk4->phi());
                            
                            if(msgLvl() <= MSG::DEBUG) {
                                msg() << MSG::DEBUG << " Finnished Bphys particle with fitmass" << trigPartBmumuLb->fitmass() << " mass " << trigPartBmumuLb->mass()
                                << " phi, eta - more RoIs => 0" << trigPartBmumuLb->phi() << " " << trigPartBmumuLb->eta() << " vertex type " << trigPartBmumuLb->particleType()
                                << endmsg;
                            }
                            
                        }else{
                            if(msgLvl() <= MSG::DEBUG) {
                                msg() << MSG::DEBUG << " Lb rejected by mass or chi2 cuts after vertex fit!"
                                << ", chi2= " << laBVtx->chi2()
                                << endmsg;
                            }
                            delete trigPartBmumuLb;
                            //continue; // for performance test not continue
                            vertex = false;
                        }
                    } // end of successful fit condition "mother"
                    if(vertex) {
                        int index = m_VertexColl->size()-1;
                        ElementLink<TrigVertexCollection> LbVertexEL(*m_VertexColl,index);
                    } else {
                        delete mother_LbVtx;
                    }
                    delete laBVtx;
                } // end of successful fit condition initial vertex
                
                
            }   // Lb -> L Mu Mu vertexing
        } // end of check over failed vertexing
        
        //////////////////////////////// vertexing off ///////////////////////////////////////////////
        if(!m_doLb_LambdaMuMuVertexFit || !m_doVertexFit){
            if ((xMuMuMass > m_lowerLb_LambdaMuMuMassCutVtxOff && xMuMuMass < m_upperLb_LambdaMuMuMassCutVtxOff)) {
                
                // Store links to the tracks forming bphys particle
                ElementLinkVector<xAOD::TrackParticleContainer> trackVectorBmumuLb;
                ElementLink<xAOD::TrackParticleContainer> track1ELLb = mu1->idTrackLink();
                ElementLink<xAOD::TrackParticleContainer> track2ELLb = mu2->idTrackLink();
                ElementLink<xAOD::TrackParticleContainer> track3ELLb(*trkCollection, iTrk3);
                ElementLink<xAOD::TrackParticleContainer> track4ELLb(*trkCollection, iTrk4);
                
                if ( msgLvl() <= MSG::VERBOSE ){
                    msg() << MSG::VERBOSE << "Just check track links... " << endmsg;
                    msg() << MSG::VERBOSE << "Muon 1 pT " << (*track1ELLb)->pt() << " eta: " << (*track1ELLb)->eta() << " phi: " << (*track1ELLb)->phi() << endmsg;
                    msg() << MSG::VERBOSE << "Muon 2 pT " << (*track2ELLb)->pt() << " eta: " << (*track2ELLb)->eta() << " phi: " << (*track2ELLb)->phi() << endmsg;
                    msg() << MSG::VERBOSE << "Track 1 pT " << (*track3ELLb)->pt() << " eta: " << (*track3ELLb)->eta() << " phi: " << (*track3ELLb)->phi() << endmsg;
                    msg() << MSG::VERBOSE << "Track 2 pT " << (*track4ELLb)->pt() << " eta: " << (*track4ELLb)->eta() << " phi: " << (*track4ELLb)->phi() << endmsg;
                }
                trackVectorBmumuLb.push_back(track1ELLb); // temp
                trackVectorBmumuLb.push_back(track2ELLb); // temp
                trackVectorBmumuLb.push_back(track3ELLb); // temp
                trackVectorBmumuLb.push_back(track4ELLb); // temp
                
                
                //         if(m_doLambda_ProtonPionVertexFit){
                ElementLink<xAOD::TrigBphysContainer> trigPartLbEL(*m_trigBphysColl_lambdaxAOD,LIndex);
                //TrigL2Bphys* trigPartBmumuLb = new TrigL2Bphys(0.0, 0.0, 0.0, TrigL2Bphys::LBLMUMU, xMuMuMass, trigPartLbEL);
                xAOD::TrigBphys* trigPartBmumuLb = new xAOD::TrigBphys();
                trigPartBmumuLb->makePrivateStore();
                trigPartBmumuLb->initialise(0.0, 0.0, 0.0, xAOD::TrigBphys::LBLMUMU, xMuMuMass, trigPartLbEL,xAOD::TrigBphys::L2);

                trigPartBmumuLb->addTrackParticleLink(track1ELLb);
                trigPartBmumuLb->addTrackParticleLink(track2ELLb);
                trigPartBmumuLb->addTrackParticleLink(track3ELLb);
                trigPartBmumuLb->addTrackParticleLink(track4ELLb);
                m_trigBphysColl_bxAOD->push_back(trigPartBmumuLb);
                //         }else{
                // //           m_trigBphysColl_lambda->push_back(trigPartBmumuL);
                // //           int LIndex = m_trigBphysColl_lambda->size() - 1;
                // //           ElementLink<xAOD::TrigBphysContainer> trigPartLbEL(*m_trigBphysColl_lambda,LIndex);
                //           
                //           TrigL2Bphys* trigPartBmumuLb = new TrigL2Bphys(0., 0., 0., TrigL2Bphys::LBLMUMU, xMuMuMass/*, trigPartLbEL*/);
                //           trigPartBmumuLb->addTrack(track1ELLb); 
                //           trigPartBmumuLb->addTrack(track2ELLb); 
                //           trigPartBmumuLb->addTrack(track3ELLb); 
                //           trigPartBmumuLb->addTrack(track4ELLb); 
                //           m_trigBphysColl_b->push_back(trigPartBmumuLb);
                //         }
                
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Lambda's Proton and Pion Candidates accepted with L mass: " << xMass << "MeV"  
                    << " and with LambdaB mass : " << xMuMuMass << "MeV"  
                    << " proton particle with pt: "<< trk3->pt() 
                    << " pion particle with pt: "<< trk4->pt() 
                    << endmsg;
                if(msgLvl() <= MSG::DEBUG)
                    msg() << MSG::DEBUG << " Added Lambda_b particle to Bphys particle collection   " 
                    //<< ", eta=" trigPartBmumuKplus->eta()   
                    //<< ", phi= " << trigPartBmumuKplus->phi()
                    << ", mass= " << trigPartBmumuLb->mass() 
                    << ", particle type= " << trigPartBmumuLb->particleType() 
                    << endmsg;
                m_mon_LbMuMuL_n++;
                m_mon_LbMuMuL_InvMassLb_L.push_back((xMass*0.001));
                m_mon_LbMuMuL_InvMass_Lb.push_back((xMuMuMass*0.001));
                m_mon_LbMuMuL_PtL_Pr.push_back(trk3->pt());
                m_mon_LbMuMuL_EtaL_Pr.push_back(trk3->eta());
                m_mon_LbMuMuL_PhiL_Pr.push_back(trk3->phi());
                m_mon_LbMuMuL_PtL_Pi.push_back(trk4->pt());
                m_mon_LbMuMuL_EtaL_Pi.push_back(trk4->eta());
                m_mon_LbMuMuL_PhiL_Pi.push_back(trk4->phi());
                
            }else{// end of tighter mass cuts
                if(msgLvl() <= MSG::DEBUG) 
                    msg() << MSG::DEBUG << " Lb rejected due to Lb mass cut (no vertexing) " << endmsg;
            } 
            
        } // end of no vertexing part
        
        
    } else { // end of if L and Lb successful mass cuts
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Lambda_b rejected due to Lambda or Lambda_b mass cut " << endmsg;
    }

    
} // checkLbMuMuLambda new


//NEW
void TrigL2BMuMuXFex::checkBcMuMuDs(const xAOD::L2CombinedMuon* mu1, const xAOD::L2CombinedMuon* mu2, const xAOD::TrackParticle* trk3,
                                    const xAOD::TrackParticle* trk4, const xAOD::TrackParticle* trk5, const xAOD::TrackParticleContainer* trkCollection,
                                    int iTrk3, int iTrk4, double xPhiMass, int iTrk5) {
    double xMass = X3Mass(trk3, trk4, trk5);
    double xMuMuMass = X3MuMuMass(mu1->idTrack(), mu2->idTrack(), trk3, trk4, trk5);
    
    if ((xMass > m_lowerDs_MassCut && xMass < m_upperDs_MassCut) && (xMuMuMass > m_lowerBc_DsMuMuMassCut && xMuMuMass < m_upperBc_DsMuMuMassCut)) {
        
        // Now make L2Bphys objects - initial addon
        //     TrigL2Bphys* trigPartBmumuPhiDs = new TrigL2Bphys(0., 0., 0., TrigL2Bphys::PHIKK, xPhiMass);
        //TrigL2Bphys* trigPartBmumuDs    = new TrigL2Bphys(0., 0., 0., TrigL2Bphys::DSPHIPI, xMass);
        xAOD::TrigBphys* trigPartBmumuDs    = new xAOD::TrigBphys();
        trigPartBmumuDs->makePrivateStore();
        trigPartBmumuDs->initialise(0., 0., 0., xAOD::TrigBphys::DSPHIPI, xMass,xAOD::TrigBphys::L2);
        
        if(msgLvl() <= MSG::DEBUG) {
            //       msg() << MSG::DEBUG << " Create PhiDs Bphys particle with roIId - more RoIs => " << trigPartBmumuPhiDs->roiId() << " mass " << xPhiMass
            //             << " phi, eta - more RoIs => " << trigPartBmumuPhiDs->phi() << " " << trigPartBmumuPhiDs->eta() << " vertex type " << trigPartBmumuPhiDs->particleType()
            //             << endmsg;
            msg() << MSG::DEBUG << " Create Ds Bphys particle with roIId - more RoIs => " << trigPartBmumuDs->roiId() << " mass " << xMass
            << " phi, eta - more RoIs => " << trigPartBmumuDs->phi() << " " << trigPartBmumuDs->eta() << " vertex type " << trigPartBmumuDs->particleType()
            << endmsg;
        }
        
        // Store links to the tracks forming bphys particle
        ElementLinkVector<xAOD::TrackParticleContainer> trackVectorBmumuDs;
        ElementLink<xAOD::TrackParticleContainer> track3EL(*trkCollection, iTrk3); //kaon
        ElementLink<xAOD::TrackParticleContainer> track4EL(*trkCollection, iTrk4); //kaon
        ElementLink<xAOD::TrackParticleContainer> track5EL(*trkCollection, iTrk5); //pion
        
        if ( msgLvl() <= MSG::VERBOSE ){
            msg() << MSG::VERBOSE << "Just check track links... " << endmsg;
            msg() << MSG::VERBOSE << "Track 1 pT " << (*track3EL)->pt() << " eta: " << (*track3EL)->eta() << " phi: " << (*track3EL)->phi() << endmsg;
            msg() << MSG::VERBOSE << "Track 2 pT " << (*track4EL)->pt() << " eta: " << (*track4EL)->eta() << " phi: " << (*track4EL)->phi() << endmsg;
            msg() << MSG::VERBOSE << "Track 3 pT " << (*track5EL)->pt() << " eta: " << (*track5EL)->eta() << " phi: " << (*track5EL)->phi() << endmsg;
        }
        trackVectorBmumuDs.push_back(track3EL); // temp
        trackVectorBmumuDs.push_back(track4EL); // temp
        trackVectorBmumuDs.push_back(track5EL); // temp
        
        trigPartBmumuDs->addTrackParticleLink(track3EL);
        trigPartBmumuDs->addTrackParticleLink(track4EL);
        trigPartBmumuDs->addTrackParticleLink(track5EL);
        // end of initial addon
        
        if(trk3->pt()*trk3->charge() > 0) {
            //       if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Phi's Pion and Kaon Candidates accepted with K* mass: " << xMass << "MeV"
            //                                            << " and with Bd mass : " << xMuMuMass << "MeV"
            //                                          << " positive charged particle with pt: "<< trk3->pt()
            //                                          << " negative charged particle with pt: "<< trk4->pt()
            //                                          << endmsg;
            m_mon_BcMuMuDs_InvMassBc_Phi.push_back((xPhiMass*0.001));
            m_mon_BcMuMuDs_InvMassBc_Ds.push_back((xMass*0.001));
            m_mon_BcMuMuDs_InvMass_Bc.push_back((xMuMuMass*0.001));
            m_mon_BcMuMuDs_PtPhi_Kp.push_back(trk3->pt());
            m_mon_BcMuMuDs_EtaPhi_Kp.push_back(trk3->eta());
            m_mon_BcMuMuDs_PhiPhi_Kp.push_back(trk3->phi());
            m_mon_BcMuMuDs_PtPhi_Km.push_back(trk4->pt());
            m_mon_BcMuMuDs_EtaPhi_Km.push_back(trk4->eta());
            m_mon_BcMuMuDs_PhiPhi_Km.push_back(trk4->phi());
            m_mon_BcMuMuDs_PtDs_pi.push_back(trk5->pt());
            m_mon_BcMuMuDs_EtaDs_pi.push_back(trk5->eta());
            m_mon_BcMuMuDs_PhiDs_pi.push_back(trk5->phi());
        }else{
            //       if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Phi's Pion and Kaon Candidates accepted with K* mass: " << xMass << "MeV"
            //                                            << " and with Bd mass : " << xMuMuMass << "MeV"
            //                                            << " positive charged particle with pt: "<< trk4->pt()
            //                                            << " negative charged particle with pt: "<< trk3->pt()
            //                                              <<endmsg;
            m_mon_BcMuMuDs_InvMassBc_Phi.push_back((xPhiMass*0.001));
            m_mon_BcMuMuDs_InvMassBc_Ds.push_back((xMass*0.001));
            m_mon_BcMuMuDs_InvMass_Bc.push_back((xMuMuMass*0.001));
            m_mon_BcMuMuDs_PtPhi_Kp.push_back(trk4->pt());
            m_mon_BcMuMuDs_EtaPhi_Kp.push_back(trk4->eta());
            m_mon_BcMuMuDs_PhiPhi_Kp.push_back(trk4->phi());
            m_mon_BcMuMuDs_PtPhi_Km.push_back(trk3->pt());
            m_mon_BcMuMuDs_EtaPhi_Km.push_back(trk3->eta());
            m_mon_BcMuMuDs_PhiPhi_Km.push_back(trk3->phi());
            m_mon_BcMuMuDs_PtDs_pi.push_back(trk5->pt());
            m_mon_BcMuMuDs_EtaDs_pi.push_back(trk5->eta());
            m_mon_BcMuMuDs_PhiDs_pi.push_back(trk5->phi());
        }
        bool goodDs = false;
        //Ds vertexing & mass cut
        if(m_doDs_PhiPionVertexFit && m_doVertexFit) {
            bool vertex = true;
            TrigL2Vertex* DsVtx = NULL;
            TrigVertex*   mother_DsVtx = NULL;
            // timer
            if(timerSvc()) m_VtxFitTimer->resume();
            DsVtx = new TrigL2Vertex();
            StatusCode status;
            status = m_vertexingTool->addTrack(trk3->track(),DsVtx,Trk::kaon);
            if( status.isFailure() ) {
                //delete phia1020Vtx;
                //continue;
                vertex = false;
            }
            if(vertex) status = m_vertexingTool->addTrack(trk4->track(),DsVtx,Trk::kaon);
            if( status.isFailure() ) {
                //delete phia1020Vtx;
                //continue;
                vertex = false;
            }
            if(vertex) status = m_vertexingTool->addTrack(trk5->track(),DsVtx,Trk::pion);
            if( status.isFailure() ) {
                //delete phia1020Vtx;
                //continue;
                vertex = false;
            }
            //////////////
            //vertex fit
            if(vertex) status = m_L2vertFitter->fit(DsVtx);
            if(status.isSuccess()) status = m_vertexingTool->calculateInvariantMass(DsVtx);
            if(status.isFailure()){
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Ds vertex fit failed!" << endmsg;
                delete DsVtx;
                if(timerSvc()) m_VtxFitTimer->pause();
                //continue; // for performance test not continue?
                vertex = false;
            }else{
                mother_DsVtx = m_vertexingTool->createTrigVertex(DsVtx);
                status = m_vertexingTool->createMotherParticle(DsVtx);
                if(status.isSuccess()){
                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Ds created, with pt: " << DsVtx->m_getMotherTrack()->pT() << endmsg;
                }
                if(mother_DsVtx == NULL){
                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  Vertex fit failed for particle candidate tracks " << trk3
                        <<" and "<<trk4<<" and "<<trk5 << endmsg;
                    if(timerSvc()) m_VtxFitTimer->pause();
                    //continue; // for performance test not continue?
                    vertex = false;
                }else{
                    if(msgLvl() <= MSG::DEBUG) {
                        msg() << MSG::DEBUG << " Ds vertex fit: x = " <<mother_DsVtx->x()
                        <<", y = " <<mother_DsVtx->y()
                        <<", z = " <<mother_DsVtx->z()
                        <<", mass = " <<mother_DsVtx->mass()
                        <<", chi2 = " <<mother_DsVtx->chi2()
                        <<", ndof = " <<mother_DsVtx->ndof() /// (number degrees of freedom)
                        <<", massVariance = " <<mother_DsVtx->massVariance()
                        << endmsg;
                    }
                    if(mother_DsVtx->massVariance() != 0) {
                        if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  Ds mass pull = " << ((mother_DsVtx->mass()-DSMASS)/mother_DsVtx->massVariance()) << endmsg;
                    }else{
                        if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " Ds mass variance is zero, can't calculate the pull" << endmsg;
                    }
                    
                    /// add vertex results to L2Bphys
                    trigPartBmumuDs->setFitmass(mother_DsVtx->mass());
                    trigPartBmumuDs->setFitchi2(mother_DsVtx->chi2());
                    trigPartBmumuDs->setFitndof(mother_DsVtx->ndof());
                    trigPartBmumuDs->setFitx(mother_DsVtx->x());
                    trigPartBmumuDs->setFity(mother_DsVtx->y());
                    trigPartBmumuDs->setFitz(mother_DsVtx->z());
                    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " SigmaX =  SigmaY =  SigmaZ = "
                        << sqrt(mother_DsVtx->cov()[0]) << " "
                        << sqrt(mother_DsVtx->cov()[2]) << " "
                        << sqrt(mother_DsVtx->cov()[5])
                        << endmsg;
                    
                    // cut on chi2 and invariant mass of bplus after vertexing.
                    if(DsVtx->chi2() < m_DsVtxChi2Cut &&
                       DsVtx->mass() > m_lowerDsVtxMassCut && DsVtx->mass() < m_upperDsVtxMassCut)
                    {
                        if(msgLvl() <= MSG::DEBUG)  {
                            msg() << MSG::DEBUG << " Ds vertex fit success: x= "
                            <<            DsVtx->m_getParametersVector()[0] //x
                            << ", y= " << DsVtx->m_getParametersVector()[1] //y
                            << ", z= " << DsVtx->m_getParametersVector()[2] //z
                            << ", chi2= " << DsVtx->chi2()
                            <<", ndof = " <<DsVtx->ndof()
                            << endmsg;
                        }
                        goodDs = true;
                        // GREAT -> store daughter particle info
                        m_VertexColl->push_back(mother_DsVtx);
                        if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Added Ds vertex to vertex collection" << endmsg;
                        int index = m_VertexColl->size()-1;
                        ElementLink<TrigVertexCollection> DsVertexEL(*m_VertexColl,index);
                        
                        // monitoring
                        m_mon_BcMuMuDs_VtxMassBc_Ds.push_back((DsVtx->mass())*0.001);
                        m_mon_BcMuMuDs_Chi2toNDoFBc_Ds.push_back((DsVtx->chi2())/(DsVtx->ndof()));
                        if(trk3->pt()*trk3->charge() > 0) {
                            m_mon_BcMuMuDs_VtxPtDs_Kp.push_back((trk3->pt())*0.001);
                            m_mon_BcMuMuDs_VtxEtaDs_Kp.push_back(trk3->eta());
                            m_mon_BcMuMuDs_VtxPhiDs_Kp.push_back(trk3->phi());
                            m_mon_BcMuMuDs_VtxPtDs_Km.push_back((trk4->pt())*0.001);
                            m_mon_BcMuMuDs_VtxEtaDs_Km.push_back(trk4->eta());
                            m_mon_BcMuMuDs_VtxPhiDs_Km.push_back(trk4->phi());
                            m_mon_BcMuMuDs_VtxPtDs_pi.push_back((trk5->pt())*0.001);
                            m_mon_BcMuMuDs_VtxEtaDs_pi.push_back(trk5->eta());
                            m_mon_BcMuMuDs_VtxPhiDs_pi.push_back(trk5->phi());
                        }else{
                            m_mon_BcMuMuDs_VtxPtDs_Kp.push_back((trk4->pt())*0.001);
                            m_mon_BcMuMuDs_VtxEtaDs_Kp.push_back(trk4->eta());
                            m_mon_BcMuMuDs_VtxPhiDs_Kp.push_back(trk4->phi());
                            m_mon_BcMuMuDs_VtxPtDs_Km.push_back((trk3->pt())*0.001);
                            m_mon_BcMuMuDs_VtxEtaDs_Km.push_back(trk3->eta());
                            m_mon_BcMuMuDs_VtxPhiDs_Km.push_back(trk3->phi());
                            m_mon_BcMuMuDs_VtxPtDs_pi.push_back((trk5->pt())*0.001);
                            m_mon_BcMuMuDs_VtxEtaDs_pi.push_back(trk5->eta());
                            m_mon_BcMuMuDs_VtxPhiDs_pi.push_back(trk5->phi());
                        }
                        if(msgLvl() <= MSG::DEBUG) {
                            msg() << MSG::DEBUG << " Finnished Bphys daughter particle with fitmass" << trigPartBmumuDs->fitmass() << " mass " << trigPartBmumuDs->mass()
                            << " phi, eta - more RoIs => 0" << trigPartBmumuDs->phi() << " " << trigPartBmumuDs->eta() << " vertex type " << trigPartBmumuDs->particleType()
                            << endmsg;
                        }
                        
                    }else{
                        if(msgLvl() <= MSG::DEBUG) {
                            msg() << MSG::DEBUG << " Ds rejected by mass or chi2 cuts after vertex fit!"
                            << ", chi2= " << DsVtx->chi2()
                            << endmsg;
                        }
                        if(timerSvc()) m_VtxFitTimer->pause();
                        //continue; // for performance test not continue
                        vertex = false;
                        delete mother_DsVtx;
                    }
                } // end of successful fit condition "mother"
                //int index = m_VertexColl->size()-1;
                //ElementLink<TrigVertexCollection> BplusVertexEL(*m_VertexColl,index);
                delete DsVtx;
            } // end of successful fit condition initial vertex
            if(timerSvc()) m_VtxFitTimer->pause();
        } // end DS_KaonKaonPion vertex fit
        
        m_trigBphysColl_dsxAOD->push_back(trigPartBmumuDs);
        int DsIndex = m_trigBphysColl_dsxAOD->size() - 1;
        
        if(m_doDs_PhiPionVertexFit && goodDs) {
            if(msgLvl() <= MSG::DEBUG) {
                msg() << MSG::DEBUG << " Well done in Ds vertexing!!!" << endmsg;
                //    continue; // OBSOLETE, done inside vertexing
            }
            
            // Bc+/- vertexing & mass cut
            if(m_doBc_DsMuMuVertexFit && m_doVertexFit){
                if(timerSvc()) m_VtxFitTimer->resume();
                bool vertex = true;
                TrigL2Vertex* BcVtx = NULL;
                TrigVertex* mother_BcVtx=NULL;
                BcVtx = new TrigL2Vertex();
                StatusCode status;
                status = m_vertexingTool->addTrack(mu1->idTrack()->track(),BcVtx,Trk::muon);   //
                if(status.isFailure()){
                    //delete BcVtx;
                    if(timerSvc()) m_VtxFitTimer->pause();
                    //continue;
                    vertex = false;
                }
                if (vertex) status = m_vertexingTool->addTrack(mu2->idTrack()->track(),BcVtx,Trk::muon);   //
                if(status.isFailure()){
                    //delete BcVtx;
                    if(timerSvc()) m_VtxFitTimer->pause();
                    //continue;
                    vertex = false;
                }
                if (vertex) status = m_vertexingTool->addTrack(trk3->track(),BcVtx,Trk::kaon);
                if(status.isFailure()){
                    //delete BcVtx;
                    if(timerSvc()) m_VtxFitTimer->pause();
                    //continue;
                    vertex = false;
                }
                if (vertex) status = m_vertexingTool->addTrack(trk4->track(),BcVtx,Trk::kaon);
                if(status.isFailure()){
                    //delete BcVtx;
                    if(timerSvc()) m_VtxFitTimer->pause();
                    //continue;
                    vertex = false;
                }
                if (vertex) status = m_vertexingTool->addTrack(trk5->track(),BcVtx,Trk::pion);
                if(status.isFailure()){
                    //delete BcVtx;
                    if(timerSvc()) m_VtxFitTimer->pause();
                    //continue;
                    vertex = false;
                }
                //ElementLink<TrigL2BphysContainer> trigPartBcEL(*m_trigBphysColl_ds,DsIndex);
                //TrigL2Bphys* trigPartBmumuBc = new TrigL2Bphys(0.0, 0.0, 0.0, TrigL2Bphys::BCDSMUMU, xMuMuMass, trigPartBcEL);
                ElementLink<xAOD::TrigBphysContainer> trigPartBcEL(*m_trigBphysColl_dsxAOD,DsIndex);
                xAOD::TrigBphys* trigPartBmumuBc = new xAOD::TrigBphys();
                trigPartBmumuBc->makePrivateStore();
                trigPartBmumuBc->initialise(0.0, 0.0, 0.0, xAOD::TrigBphys::BCDSMUMU, xMuMuMass, trigPartBcEL,xAOD::TrigBphys::L2);

                //vertex fit
                if (vertex) status = m_L2vertFitter->fit(BcVtx);
                if(status.isSuccess()) status = m_vertexingTool->calculateInvariantMass(BcVtx);
                if(status.isFailure()){
                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Bc vertex fit failed!" << endmsg;
                    delete BcVtx;
                    delete trigPartBmumuBc;
                    //continue; // for performance test not continue
                    vertex = false;
                }else{
                    mother_BcVtx = m_vertexingTool->createTrigVertex(BcVtx);
                    status = m_vertexingTool->createMotherParticle(BcVtx);
                    if(status.isSuccess()){
                        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Bc created, with pt: " << BcVtx->m_getMotherTrack()->pT() << endmsg;
                    }
                    if(mother_BcVtx == NULL){
                        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  Vertex fit failed for particle candidate tracks " << trk3
                            <<" and "<<trk4<<" and "<<trk5<< endmsg;
                        vertex = false;
                        delete trigPartBmumuBc;
                        //continue; // for performance test not continue
                    }else{
                        if(msgLvl() <= MSG::DEBUG) {
                            msg() << MSG::DEBUG << " Bc vertex fit: x = " <<mother_BcVtx->x()
                            <<", y = " <<mother_BcVtx->y()
                            <<", z = " <<mother_BcVtx->z()
                            <<", mass = " <<mother_BcVtx->mass()
                            <<", chi2 = " <<mother_BcVtx->chi2()
                            <<", ndof = " <<mother_BcVtx->ndof() /// (number degrees of freedom)
                            <<", massVariance = " <<mother_BcVtx->massVariance()
                            << endmsg;
                        }
                        if(mother_BcVtx->massVariance() != 0) {
                            if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  Bc mass pull = " << ((mother_BcVtx->mass()-BCMASS)/mother_BcVtx->massVariance()) << endmsg;
                        }else{
                            if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " Bc mass variance is zero, can't calculate the pull" << endmsg;
                        }
                        
                        /// add vertex results to L2Bphys
                        trigPartBmumuBc->setFitmass(mother_BcVtx->mass());
                        trigPartBmumuBc->setFitchi2(mother_BcVtx->chi2());
                        trigPartBmumuBc->setFitndof(mother_BcVtx->ndof());
                        trigPartBmumuBc->setFitx(mother_BcVtx->x());
                        trigPartBmumuBc->setFity(mother_BcVtx->y());
                        trigPartBmumuBc->setFitz(mother_BcVtx->z());
                        
                        if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " SigmaX =  SigmaY =  SigmaZ = "
                            << sqrt(mother_BcVtx->cov()[0]) << " "
                            << sqrt(mother_BcVtx->cov()[2]) << " "
                            << sqrt(mother_BcVtx->cov()[5])
                            << endmsg;
                        
                        // cut on chi2 and invariant mass of Bc after vertexing.
                        if(BcVtx->chi2() < m_bCVtxChi2Cut && BcVtx->mass() > m_lowerBcVtxMassCut && BcVtx->mass() < m_upperBcVtxMassCut)
                        {
                            if(msgLvl() <= MSG::DEBUG)  {
                                msg() << MSG::DEBUG << " Bc vertex fit success: x= "
                                <<            BcVtx->m_getParametersVector()[0] //x
                                << ", y= " << BcVtx->m_getParametersVector()[1] //y
                                << ", z= " << BcVtx->m_getParametersVector()[2] //z
                                << ", chi2= " << BcVtx->chi2()
                                <<", ndof = " <<BcVtx->ndof()
                                << endmsg;
                            }
                            
                            // PERFECT -> Bphys particle was stored in collection
                            m_VertexColl->push_back(mother_BcVtx);
                            m_trigBphysColl_bxAOD->push_back(trigPartBmumuBc);
                            if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Added Bc vertex to vertex collection" << endmsg;
                            // monitoring of successfuly stored particles
                            m_mon_BcMuMuDs_n++;
                            m_mon_Acceptance.push_back( ACCEPT_BphysCollParticle );
                            m_mon_BcMuMuDs_VtxMass_Bc.push_back((BcVtx->mass())*0.001);
                            m_mon_BcMuMuDs_Chi2toNDoF_Bc.push_back((BcVtx->chi2())/(BcVtx->ndof()));
                            if(trk3->pt()*trk3->charge() > 0) {
                                m_mon_BcMuMuDs_VtxPtBc_Kp.push_back((trk3->pt())*0.001);
                                m_mon_BcMuMuDs_VtxEtaBc_Kp.push_back(trk3->eta());
                                m_mon_BcMuMuDs_VtxPhiBc_Kp.push_back(trk3->phi());
                                m_mon_BcMuMuDs_VtxPtBc_Km.push_back((trk4->pt())*0.001);
                                m_mon_BcMuMuDs_VtxEtaBc_Km.push_back(trk4->eta());
                                m_mon_BcMuMuDs_VtxPhiBc_Km.push_back(trk4->phi());
                                m_mon_BcMuMuDs_VtxPtBc_pi.push_back((trk5->pt())*0.001);
                                m_mon_BcMuMuDs_VtxEtaBc_pi.push_back(trk5->eta());
                                m_mon_BcMuMuDs_VtxPhiBc_pi.push_back(trk5->phi());
                            }else{
                                m_mon_BcMuMuDs_VtxPtBc_Kp.push_back((trk4->pt())*0.001);
                                m_mon_BcMuMuDs_VtxEtaBc_Kp.push_back(trk4->eta());
                                m_mon_BcMuMuDs_VtxPhiBc_Kp.push_back(trk4->phi());
                                m_mon_BcMuMuDs_VtxPtBc_Km.push_back((trk3->pt())*0.001);
                                m_mon_BcMuMuDs_VtxEtaBc_Km.push_back(trk3->eta());
                                m_mon_BcMuMuDs_VtxPhiBc_Km.push_back(trk3->phi());
                                m_mon_BcMuMuDs_VtxPtBc_pi.push_back((trk5->pt())*0.001);
                                m_mon_BcMuMuDs_VtxEtaBc_pi.push_back(trk5->eta());
                                m_mon_BcMuMuDs_VtxPhiBc_pi.push_back(trk5->phi());
                            }
                            if(msgLvl() <= MSG::DEBUG) {
                                msg() << MSG::DEBUG << " Finnished Bphys particle with fitmass" << trigPartBmumuBc->fitmass() << " mass " << trigPartBmumuBc->mass()
                                << " phi, eta - more RoIs => 0" << trigPartBmumuBc->phi() << " " << trigPartBmumuBc->eta() << " vertex type " << trigPartBmumuBc->particleType()
                                << endmsg;
                            }
                            
                        }else{
                            if(msgLvl() <= MSG::DEBUG) {
                                msg() << MSG::DEBUG << " Bc rejected by mass or chi2 cuts after vertex fit!"
                                << ", chi2= " << BcVtx->chi2()
                                << endmsg;
                                //continue; // for performance test not continue
                            }
                            delete trigPartBmumuBc;
                            vertex = false;
                        }
                    } // end of successful fit condition "mother"
                    if(vertex) {
                        int index = m_VertexColl->size()-1;
                        ElementLink<TrigVertexCollection> BsVertexEL(*m_VertexColl,index);
                    } else {
                        delete mother_BcVtx;
                    }
                    delete BcVtx;
                } // end of successful fit condition initial vertex
                
                
            }   // Bc -> Ds Mu Mu vertexing
        } // end of test over failed
        //////////////////////////////// vertexing off ///////////////////////////////////////////////
        if(!m_doBc_DsMuMuVertexFit || !m_doVertexFit){
            if ((xMuMuMass > m_lowerBc_DsMuMuMassCutVtxOff && xMuMuMass < m_upperBc_DsMuMuMassCutVtxOff)) {
                
                // Store links to the tracks forming bphys particle
                ElementLinkVector<xAOD::TrackParticleContainer> trackVectorBmumuBc;
                ElementLink<xAOD::TrackParticleContainer> track1ELBc = mu1->idTrackLink();
                ElementLink<xAOD::TrackParticleContainer> track2ELBc = mu2->idTrackLink();
                ElementLink<xAOD::TrackParticleContainer> track3ELBc(*trkCollection, iTrk3);
                ElementLink<xAOD::TrackParticleContainer> track4ELBc(*trkCollection, iTrk4);
                ElementLink<xAOD::TrackParticleContainer> track5ELBc(*trkCollection, iTrk5);
                
                if ( msgLvl() <= MSG::VERBOSE ){
                    msg() << MSG::VERBOSE << "Just check track links... " << endmsg;
                    msg() << MSG::VERBOSE << "Muon 1 pT " << (*track1ELBc)->pt() << " eta: " << (*track1ELBc)->eta() << " phi: " << (*track1ELBc)->phi() << endmsg;
                    msg() << MSG::VERBOSE << "Muon 2 pT " << (*track2ELBc)->pt() << " eta: " << (*track2ELBc)->eta() << " phi: " << (*track2ELBc)->phi() << endmsg;
                    msg() << MSG::VERBOSE << "Track 1 pT " << (*track3ELBc)->pt() << " eta: " << (*track3ELBc)->eta() << " phi: " << (*track3ELBc)->phi() << endmsg;
                    msg() << MSG::VERBOSE << "Track 2 pT " << (*track4ELBc)->pt() << " eta: " << (*track4ELBc)->eta() << " phi: " << (*track4ELBc)->phi() << endmsg;
                    msg() << MSG::VERBOSE << "Track 3 pT " << (*track5ELBc)->pt() << " eta: " << (*track5ELBc)->eta() << " phi: " << (*track5ELBc)->phi() << endmsg;
                }
                trackVectorBmumuBc.push_back(track1ELBc); // temp
                trackVectorBmumuBc.push_back(track2ELBc); // temp
                trackVectorBmumuBc.push_back(track3ELBc); // temp
                trackVectorBmumuBc.push_back(track4ELBc); // temp
                trackVectorBmumuBc.push_back(track5ELBc); // temp
                
                
                //         if(m_doPhi1020_KaonKaonVertexFit){
                //ElementLink<TrigL2BphysContainer> trigPartBcEL(*m_trigBphysColl_ds,DsIndex);
                //TrigL2Bphys* trigPartBmumuBc = new TrigL2Bphys(0.0, 0.0, 0.0, TrigL2Bphys::BCDSMUMU, xMuMuMass, trigPartBcEL);
                ElementLink<xAOD::TrigBphysContainer> trigPartBcEL(*m_trigBphysColl_dsxAOD,DsIndex);
                xAOD::TrigBphys* trigPartBmumuBc = new xAOD::TrigBphys();
                trigPartBmumuBc->makePrivateStore();
                trigPartBmumuBc->initialise(0.0, 0.0, 0.0, xAOD::TrigBphys::BCDSMUMU, xMuMuMass, trigPartBcEL,xAOD::TrigBphys::L2);

                trigPartBmumuBc->addTrackParticleLink(track1ELBc);
                trigPartBmumuBc->addTrackParticleLink(track2ELBc);
                trigPartBmumuBc->addTrackParticleLink(track3ELBc);
                trigPartBmumuBc->addTrackParticleLink(track4ELBc);
                trigPartBmumuBc->addTrackParticleLink(track5ELBc);
                m_trigBphysColl_bxAOD->push_back(trigPartBmumuBc);
                //         }else{
                // //           m_trigBphysColl_phi->push_back(trigPartBmumuPhi);
                // //           int PhiIndex = m_trigBphysColl_phi->size() - 1;
                // //           ElementLink<TrigL2BphysContainer> trigPartBsEL(*m_trigBphysColl_phi,PhiIndex);
                //           
                //           TrigL2Bphys* trigPartBmumuBs = new TrigL2Bphys(0., 0., 0., TrigL2Bphys::BSPHIMUMU, xMuMuMass/*, trigPartBsEL*/);
                //           trigPartBmumuBs->addTrack(track1ELBs); 
                //             trigPartBmumuBs->addTrack(track2ELBs); 
                //             trigPartBmumuBs->addTrack(track3ELBs); 
                //             trigPartBmumuBs->addTrack(track4ELBs); 
                //           m_trigBphysColl_b->push_back(trigPartBmumuBs);
                //         }
                m_mon_BcMuMuDs_n++;
                if(trk3->pt()*trk3->charge() > 0) {
                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Ds's Kplus, Kminus and pion Candidates accepted with Ds mass: " << xMass << "MeV"  
                        << " and with B_c mass : " << xMuMuMass << "MeV"  
                        << " positive charged particle with pt: "<< trk3->pt() 
                        << " negative charged particle with pt: "<< trk4->pt() 
                        << " pion charged particle with pt: "<< trk5->pt() 
                        << endmsg;
                    m_mon_BcMuMuDs_InvMassBc_Phi.push_back((xPhiMass*0.001));
                    m_mon_BcMuMuDs_InvMassBc_Ds.push_back((xMass*0.001));
                    m_mon_BcMuMuDs_InvMass_Bc.push_back((xMuMuMass*0.001));
                    m_mon_BcMuMuDs_PtPhi_Kp.push_back(trk3->pt());
                    m_mon_BcMuMuDs_EtaPhi_Kp.push_back(trk3->eta());
                    m_mon_BcMuMuDs_PhiPhi_Kp.push_back(trk3->phi());
                    m_mon_BcMuMuDs_PtPhi_Km.push_back(trk4->pt());
                    m_mon_BcMuMuDs_EtaPhi_Km.push_back(trk4->eta());
                    m_mon_BcMuMuDs_PhiPhi_Km.push_back(trk4->phi());
                    m_mon_BcMuMuDs_PtDs_pi.push_back(trk5->pt());
                    m_mon_BcMuMuDs_EtaDs_pi.push_back(trk5->eta());
                    m_mon_BcMuMuDs_PhiDs_pi.push_back(trk5->phi());
                }else{
                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Ds's Kplus, Kminus and pion Candidates accepted with Ds mass: " << xMass << "MeV"  
                        << " and with B_c mass : " << xMuMuMass << "MeV"  
                        << " positive charged particle with pt: "<< trk4->pt() 
                        << " negative charged particle with pt: "<< trk3->pt() 
                        << " pion charged particle with pt: "<< trk5->pt() 
                        <<endmsg;
                    m_mon_BcMuMuDs_InvMassBc_Phi.push_back((xPhiMass*0.001));
                    m_mon_BcMuMuDs_InvMassBc_Ds.push_back((xMass*0.001));
                    m_mon_BcMuMuDs_InvMass_Bc.push_back((xMuMuMass*0.001));
                    m_mon_BcMuMuDs_PtPhi_Kp.push_back(trk4->pt());
                    m_mon_BcMuMuDs_EtaPhi_Kp.push_back(trk4->eta());
                    m_mon_BcMuMuDs_PhiPhi_Kp.push_back(trk4->phi());
                    m_mon_BcMuMuDs_PtPhi_Km.push_back(trk3->pt());
                    m_mon_BcMuMuDs_EtaPhi_Km.push_back(trk3->eta());
                    m_mon_BcMuMuDs_PhiPhi_Km.push_back(trk3->phi());
                    m_mon_BcMuMuDs_PtDs_pi.push_back(trk5->pt());
                    m_mon_BcMuMuDs_EtaDs_pi.push_back(trk5->eta());
                    m_mon_BcMuMuDs_PhiDs_pi.push_back(trk5->phi());
                }
                if(msgLvl() <= MSG::DEBUG)
                    msg() << MSG::DEBUG << " Added Bc particle to Bphys particle collection   " 
                    //<< ", eta=" trigPartBmumuKplus->eta()   
                    //<< ", phi= " << trigPartBmumuKplus->phi()
                    << ", mass= " << trigPartBmumuBc->mass() 
                    << ", particle type= " << trigPartBmumuBc->particleType() 
                    << endmsg;
            } else {// end of tighter mass cuts
                if(msgLvl() <= MSG::DEBUG) 
                    msg() << MSG::DEBUG << " Bc rejected due to Bc mass cut (no vertexing) " << endmsg;
            } 
            
        } // end of no vertexing part
        
        
    } else { // end of if Ds and Bc successful mass cuts
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Bc rejected due to Ds or Bc mass cut " << endmsg;
    }

} //checkBcMuMuDs new


//New
/*----------------------------------------------------------------------------*/
double TrigL2BMuMuXFex::XMass(const xAOD::TrackParticle* particle1, const xAOD::TrackParticle* particle2,int decay)
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
    std::vector<const xAOD::TrackParticle*> bTracks;
    bTracks.clear();
    bTracks.push_back(particle1);
    bTracks.push_back(particle2);
    //return InvMass(bTracks, massHypo);
    return m_bphysHelperTool->invariantMass(bTracks, massHypo);
}

double TrigL2BMuMuXFex::KMuMuMass(const xAOD::TrackParticle* mu1, const xAOD::TrackParticle* mu2,
                                  const xAOD::TrackParticle* kaon)
{
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
    //return InvMass(bTracks, massHypo);
    return m_bphysHelperTool->invariantMass(bTracks, massHypo);
}
double TrigL2BMuMuXFex::XMuMuMass(const xAOD::TrackParticle* mu1, const xAOD::TrackParticle* mu2,
                                  const xAOD::TrackParticle* particle1, const xAOD::TrackParticle* particle2, int decay)
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
    std::vector<const xAOD::TrackParticle*> bTracks;
    bTracks.clear();
    bTracks.push_back(mu1);
    bTracks.push_back(mu2);
    bTracks.push_back(particle1);
    bTracks.push_back(particle2);
    //return InvMass(bTracks, massHypo);
    return m_bphysHelperTool->invariantMass(bTracks, massHypo);
}
double TrigL2BMuMuXFex::X3Mass(const xAOD::TrackParticle* particle1, const xAOD::TrackParticle* particle2, const xAOD::TrackParticle* particle3)
{
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
    //return InvMass(bTracks, massHypo);
    return m_bphysHelperTool->invariantMass(bTracks, massHypo);
}
double TrigL2BMuMuXFex::X3MuMuMass(const xAOD::TrackParticle* mu1, const xAOD::TrackParticle* mu2,
                                   const xAOD::TrackParticle* particle1, const xAOD::TrackParticle* particle2, const xAOD::TrackParticle* particle3)
{
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
    //return InvMass(bTracks, massHypo);
    return m_bphysHelperTool->invariantMass(bTracks, massHypo);
}


//OLD
/*----------------------------------------------------------------------------*/
//void TrigL2BMuMuXFex::checkBMuMuK(const CombinedMuonFeature* mu1, const CombinedMuonFeature* mu2, const TrigInDetTrack* trk3, const TrigInDetTrackCollection* trkCollection, int iTrk3) {
//    
//    double kMuMumass = KMuMuMass(mu1->IDTrack(), mu2->IDTrack(), trk3);
//    
//    if(kMuMumass > m_lowerKMuMuMassCut && kMuMumass < m_upperKMuMuMassCut) {
//        // Now make L2Bphys object - initial addon
//        TrigL2Bphys* trigPartBmumuKplus = new TrigL2Bphys(0., 0., 0., TrigL2Bphys::BKMUMU, kMuMumass);
//        if(msgLvl() <= MSG::DEBUG) {
//            msg() << MSG::DEBUG << " Create B+ Bphys particle with roIId - more RoIs => 0" << trigPartBmumuKplus->roiId() << " mass " << kMuMumass
//            << " phi, eta - more RoIs => 0" << trigPartBmumuKplus->phi() << " " << trigPartBmumuKplus->eta() << " vertex type " << trigPartBmumuKplus->particleType()
//            << endmsg;
//        }
//        
//        // Store links to the tracks forming bphys particle
//        ElementLinkVector<TrigInDetTrackCollection> trackVectorBmumuKplus;
//        ElementLink<TrigInDetTrackCollection> track1EL = mu1->IDTrackLink();
//        ElementLink<TrigInDetTrackCollection> track2EL = mu2->IDTrackLink();
//        ElementLink<TrigInDetTrackCollection> track3EL(*trkCollection, iTrk3);
//        
//        if ( msgLvl() <= MSG::VERBOSE ){
//            msg() << MSG::VERBOSE << "Just check track links... " << endmsg;
//            msg() << MSG::VERBOSE << "Muon 1 pT " << (*track1EL)->param()->pT() << " eta: " << (*track1EL)->param()->eta() << " phi: " << (*track1EL)->param()->phi0() << endmsg;
//            msg() << MSG::VERBOSE << "Muon 2 pT " << (*track2EL)->param()->pT() << " eta: " << (*track2EL)->param()->eta() << " phi: " << (*track2EL)->param()->phi0() << endmsg;
//            msg() << MSG::VERBOSE << "Track 1 pT " << (*track3EL)->param()->pT() << " eta: " << (*track3EL)->param()->eta() << " phi: " << (*track3EL)->param()->phi0() << endmsg;
//        }
//        trackVectorBmumuKplus.push_back(track1EL); // temp
//        trackVectorBmumuKplus.push_back(track2EL); // temp
//        trackVectorBmumuKplus.push_back(track3EL); // temp
//        
//        trigPartBmumuKplus->addTrack(track1EL);
//        trigPartBmumuKplus->addTrack(track2EL);
//        trigPartBmumuKplus->addTrack(track3EL);
//        
//        
//        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " K candidate accept: " <<trk3->param()->pT()<< "MeV and B invariant mass "<< kMuMumass<< "MeV"<< endmsg;
//        m_mon_BMuMuK_InvMass_B.push_back((kMuMumass*0.001));
//        m_mon_BMuMuK_PtB_K.push_back(trk3->param()->pT());
//        m_mon_BMuMuK_EtaB_K.push_back(trk3->param()->eta());
//        m_mon_BMuMuK_PhiB_K.push_back(trk3->param()->phi0());
//        //////////////////////////////// vertexing /////////////////////////////////
//        //////////////////////////////// X particles ///////////////////////////////
//        // B+/- vertexing & mass cut
//        if(m_doB_KMuMuVertexFit &&  m_doVertexFit) {
//            // B -> K Mu Mu vertexing
//            bool vertex=true;
//            TrigL2Vertex* baplusVtx = NULL;
//            TrigVertex*   mother_BplusVtx = NULL;
//            if(timerSvc()){
//                m_VtxFitTimer->start();
//                m_VtxFitTimer->pause(); // resume it only for the vertexing periods
//            }
//            // Vertexing time processing
//            if(timerSvc()) m_VtxFitTimer->resume();
//            
//            baplusVtx = new TrigL2Vertex();
//            StatusCode status;
//            status = m_vertexingTool->addTrack(mu1->IDTrack(),baplusVtx,Trk::muon);   //
//            if( status.isFailure() ) {
//                if(msgLvl() <= MSG::DEBUG) msg() <<MSG::DEBUG <<" Failed to add muon track 1 to vertexingTool of B vertex, with pT " <<mu1->IDTrack()->param()->pT() << " ." << endmsg;
//                m_mon_Errors.push_back( ERROR_AddMuonTrack_Fails );
//                //delete baplusVtx;
//                //continue;
//                vertex = false;
//            }
//            if(vertex) status = m_vertexingTool->addTrack(mu2->IDTrack(),baplusVtx,Trk::muon);   //
//            if( status.isFailure() ) {
//                if(msgLvl() <= MSG::DEBUG) msg() <<MSG::DEBUG <<" Failed to add muon track 2 to vertexingTool of B vertex, with pT " <<mu2->IDTrack()->param()->pT() << " ." << endmsg;
//                m_mon_Errors.push_back( ERROR_AddMuonTrack_Fails );
//                //delete baplusVtx;
//                //continue;
//                vertex = false;
//            }
//            if(vertex) status = m_vertexingTool->addTrack(trk3,baplusVtx,Trk::kaon);
//            if( status.isFailure() ) {
//                if(msgLvl() <= MSG::DEBUG) msg() <<MSG::DEBUG <<" Failed to add particle track 3 to vertexingTool of B vertex with pT, chi2 " <<trk3->param()->pT() << " MeV, " << trk3->chi2() <<" ."<< endmsg;
//                m_mon_Errors.push_back( ERROR_AddTrack_Fails );
//                //delete baplusVtx;
//                //continue;
//                vertex = false;
//            }
//            
//            //vertex fit
//            if(vertex) status = m_L2vertFitter->fit(baplusVtx);
//            if(status.isSuccess()) status = m_vertexingTool->calculateInvariantMass(baplusVtx);
//            if(status.isFailure()){
//                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " B vertex fit failed!" << endmsg;
//                delete baplusVtx;
//                m_mon_Errors.push_back( ERROR_VtxFitFailed );
//                //continue;
//                vertex = false;
//            }else{
//                mother_BplusVtx = m_vertexingTool->createTrigVertex(baplusVtx);
//                status = m_vertexingTool->createMotherParticle(baplusVtx);
//                if(status.isSuccess()){
//                    m_mon_Acceptance.push_back( ACCEPT_MotherVtxCreated );
//                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " B created, with pt: " << baplusVtx->m_getMotherTrack()->pT() << endmsg;
//                }else{
//                    vertex = false;
//                }
//                if(mother_BplusVtx == NULL){
//                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  Vertex fit failed for particle candidate track " << trk3 << endmsg;
//                    //delete mother_BplusVtx;
//                    m_mon_Errors.push_back( ERROR_MotherVtxFitFailed );
//                    //continue;
//                    vertex = false;
//                }else{
//                    if(msgLvl() <= MSG::DEBUG) {
//                        msg() << MSG::DEBUG << " B vertex fit: x = " <<mother_BplusVtx->x()
//                        <<", y = " <<mother_BplusVtx->y()
//                        <<", z = " <<mother_BplusVtx->z()
//                        <<", mass = " <<mother_BplusVtx->mass()
//                        <<", chi2 = " <<mother_BplusVtx->chi2()
//                        <<", ndof = " <<mother_BplusVtx->ndof() /// (number degrees of freedom)
//                        <<", massVariance = " <<mother_BplusVtx->massVariance()
//                        << endmsg;
//                    }
//                    if(mother_BplusVtx->massVariance() != 0) {
//                        if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  B mass pull = " << (mother_BplusVtx->mass()-BPLUSMASS/mother_BplusVtx->massVariance()) << endmsg;
//                    }else{
//                        if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " B mass variance is zero, can't calculate the pull" << endmsg;
//                    }
//                    /// add vertex results to L2Bphys
//                    trigPartBmumuKplus->fitmass(mother_BplusVtx->mass());
//                    trigPartBmumuKplus->fitchi2(mother_BplusVtx->chi2());
//                    trigPartBmumuKplus->fitndof(mother_BplusVtx->ndof());
//                    trigPartBmumuKplus->fitx(mother_BplusVtx->x());
//                    trigPartBmumuKplus->fity(mother_BplusVtx->y());
//                    trigPartBmumuKplus->fitz(mother_BplusVtx->z());
//                    
//                    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " SigmaX =  SigmaY =  SigmaZ = "
//                        << sqrt(mother_BplusVtx->cov()[0]) << " "
//                        << sqrt(mother_BplusVtx->cov()[2]) << " "
//                        << sqrt(mother_BplusVtx->cov()[5])
//                        << endmsg;
//                    
//                    // cut on chi2 and invariant mass of bplus after vertexing.
//                    if(baplusVtx->chi2() < m_bVtxChi2Cut &&
//                       baplusVtx->mass() > m_lowerBVtxMassCut && baplusVtx->mass() < m_upperBVtxMassCut)
//                    {
//                        if(msgLvl() <= MSG::DEBUG)  {
//                            msg() << MSG::DEBUG << " B vertex fit success: x= "
//                            <<            baplusVtx->m_getParametersVector()[0] //x
//                            << ", y= " << baplusVtx->m_getParametersVector()[1] //y
//                            << ", z= " << baplusVtx->m_getParametersVector()[2] //z
//                            << ", chi2= " << baplusVtx->chi2()
//                            <<", ndof = " <<baplusVtx->ndof()
//                            << endmsg;
//                        }
//                        // perferct -> Bphys particle was stored in Collection
//                        m_VertexColl->push_back(mother_BplusVtx);
//                        m_trigBphysColl_b->push_back(trigPartBmumuKplus);
//                        if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Added B vertex to vertex collection" << endmsg;
//                        
//                        // monitoring
//                        m_mon_BMuMuK_n++;
//                        m_mon_Acceptance.push_back( ACCEPT_BphysCollParticle );
//                        m_mon_BMuMuK_VtxMass_B.push_back((baplusVtx->mass())*0.001);
//                        m_mon_BMuMuK_Chi2toNDoF_B.push_back((baplusVtx->chi2())/(baplusVtx->ndof()));
//                        m_mon_BMuMuK_VtxPtB_K.push_back((trk3->param()->pT())*0.001);
//                        m_mon_BMuMuK_VtxEtaB_K.push_back(trk3->param()->eta());
//                        m_mon_BMuMuK_VtxPhiB_K.push_back(trk3->param()->phi0());
//                        if(msgLvl() <= MSG::DEBUG) {
//                            msg() << MSG::DEBUG << " Finnished Bphys particle with fitmass" << trigPartBmumuKplus->fitmass() << " mass " << trigPartBmumuKplus->mass()
//                            << " phi, eta - more RoIs => 0" << trigPartBmumuKplus->phi() << " " << trigPartBmumuKplus->eta() << " vertex type " << trigPartBmumuKplus->particleType()
//                            << endmsg;
//                        }
//                        
//                    }else{
//                        if(msgLvl() <= MSG::DEBUG) {
//                            msg() << MSG::DEBUG << " B rejected by mass or chi2 cuts after vertex fit!"
//                            << ", chi2= " << baplusVtx->chi2()
//                            << endmsg;
//                            vertex = false;
//                        }
//                        delete mother_BplusVtx;
//                    }
//                } // end of successful fit condition "mother"
//                delete baplusVtx;
//            } // end of successful fit condition initial vertex
//            if(timerSvc()) {
//                m_VtxFitTimer->pause(); // resume it only for the vertexing periods
//            }
//        } // end of  K Mu Mu vertex fit
//        else{ // if doVertexFit,... == False     if((!m_doB_KMuMuVertexFit || m_doVertexFit)){     UPGRADE NEEDED
//            
//            ////////////////////////////////////////////////////////////////////////////////////////////////////////
//            // B+/- end of mass cuts, vertexing off
//            if (kMuMumass > m_lowerB_KMuMuMassCutVtxOff && kMuMumass < m_upperB_KMuMuMassCutVtxOff) {
//                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " K candidate accept after tight cuts: " <<trk3->param()->pT()<< "MeV and B invariant mass "<< kMuMumass<< "MeV"<< endmsg;
//                m_trigBphysColl_b->push_back(trigPartBmumuKplus);
//                if(msgLvl() <= MSG::DEBUG) {
//                    msg() << MSG::DEBUG << " Added B+ particle to Bphys particle collection   "
//                    //<< ", eta=" trigPartBmumuKplus->eta()   
//                    //<< ", phi= " << trigPartBmumuKplus->phi()
//                    << ", mass= " << trigPartBmumuKplus->mass() 
//                    << ", particle type= " << trigPartBmumuKplus->particleType() 
//                    << endmsg;
//                }
//                // monitoring
//                m_mon_BMuMuK_n++;
//                m_mon_Acceptance.push_back( ACCEPT_BphysCollParticle );
//                m_mon_BMuMuK_InvMass_B.push_back((kMuMumass*0.001));
//                m_mon_BMuMuK_PtB_K.push_back(trk3->param()->pT());
//                m_mon_BMuMuK_EtaB_K.push_back(trk3->param()->eta());
//                m_mon_BMuMuK_PhiB_K.push_back(trk3->param()->phi0());
//            }else{
//                if(msgLvl() <= MSG::DEBUG) 
//                    msg() << MSG::DEBUG << " No B+ particle stored due cuts! " << endmsg;
//            }
//            
//        } 
//        ///////////////////////// end of B -> K Mu Mu part with vertexing off /////////////////////////               
//    } else {// end of invariant mass cuts
//        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " B+ rejected due do B+ mass cut" << endmsg;
//    }
//    //////////////////////////////////////////////////////////////////////////////// 
//}
//
////OLD
///*----------------------------------------------------------------------------*/
//void TrigL2BMuMuXFex::checkBdMuMuKstar(const CombinedMuonFeature* mu1, const CombinedMuonFeature* mu2, const TrigInDetTrack* trk3, const TrigInDetTrack* trk4, const TrigInDetTrackCollection* trkCollection, int iTrk3, int iTrk4) {
//    
//    double xMass = 0;
//    double xMuMuMass = 0;
//    
//    xMass = XMass(trk3,trk4, bD_to_Kstar);
//    xMuMuMass = XMuMuMass(mu1->IDTrack(), mu2->IDTrack(), trk3, trk4, bD_to_Kstar);
//    
//    if ((xMass > m_lowerKstar_KaonMassCut && xMass < m_upperKstar_KaonMassCut) && (xMuMuMass > m_lowerBd_KstarMuMuMassCut && xMuMuMass < m_upperBd_KstarMuMuMassCut)) {
//        // Now make L2Bphys object - initial addon
//        TrigL2Bphys* trigPartBmumuKstar = new TrigL2Bphys(0., 0., 0., TrigL2Bphys::PHIKK, xMass);
//        if(msgLvl() <= MSG::DEBUG) {
//            msg() << MSG::DEBUG << " Create K* Bphys particle with roIId - more RoIs => " << trigPartBmumuKstar->roiId() << " mass " << xMass
//            << " phi, eta - more RoIs => " << trigPartBmumuKstar->phi() << " " << trigPartBmumuKstar->eta() << " vertex type " << trigPartBmumuKstar->particleType()
//            << endmsg;
//        }
//        
//        // Store links to the tracks forming bphys particle
//        ElementLinkVector<TrigInDetTrackCollection> trackVectorBmumuKstar;
//        ElementLink<TrigInDetTrackCollection> track3EL(*trkCollection, iTrk3);
//        ElementLink<TrigInDetTrackCollection> track4EL(*trkCollection, iTrk4);
//        //         ElementLink<TrigInDetTrackCollection> track3EL(*(*iTrackCollection), itrk);
//        //         ElementLink<TrigInDetTrackCollection> track4EL(*(*iTrackCollection), jtrk);
//        
//        if ( msgLvl() <= MSG::VERBOSE ){
//            msg() << MSG::VERBOSE << "Just check track links... " << endmsg;
//            msg() << MSG::VERBOSE << "Track 1 pT " << (*track3EL)->param()->pT() << " eta: " << (*track3EL)->param()->eta() << " phi: " << (*track3EL)->param()->phi0() << endmsg;
//            msg() << MSG::VERBOSE << "Track 2 pT " << (*track4EL)->param()->pT() << " eta: " << (*track4EL)->param()->eta() << " phi: " << (*track4EL)->param()->phi0() << endmsg;
//        }
//        trackVectorBmumuKstar.push_back(track3EL); // temp
//        trackVectorBmumuKstar.push_back(track4EL); // temp
//        
//        trigPartBmumuKstar->addTrack(track3EL);
//        trigPartBmumuKstar->addTrack(track4EL);
//        // end of initial addon
//        
//        
//        //         if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Kstar's Pion and Kaon Candidates accepted with K* mass: " << xMass << "MeV"
//        //                                         << " and with Bd mass : " << xMuMuMass << "MeV"
//        //                                         << " kaon particle with pt: "<< (trk3)->param()->pT()
//        //                                         << " pion charged particle with pt: "<< (trk4)->param()->pT()
//        //                                         << endmsg;
//        m_mon_BdMuMuKs_InvMassBd_Ks.push_back((xMass*0.001));
//        m_mon_BdMuMuKs_PtKs_Ka.push_back((trk3)->param()->pT());
//        m_mon_BdMuMuKs_EtaKs_Ka.push_back((trk3)->param()->eta());
//        m_mon_BdMuMuKs_PhiKs_Ka.push_back((trk3)->param()->phi0());
//        m_mon_BdMuMuKs_PtKs_Pi.push_back((trk4)->param()->pT());
//        m_mon_BdMuMuKs_EtaKs_Pi.push_back((trk4)->param()->eta());
//        m_mon_BdMuMuKs_PhiKs_Pi.push_back((trk4)->param()->phi0());
//        
//        bool goodKstar = false;
//        //K* vertexing & mass cut
//        if(m_doKstar_KaonPionVertexFit && m_doVertexFit) {
//            bool vertex = true;
//            TrigL2Vertex* kaStarVtx = NULL;
//            TrigVertex*   mother_KstarVtx = NULL;
//            
//            // timer
//            if(timerSvc()) m_VtxFitTimer->resume();
//            
//            StatusCode status;
//            status = m_vertexingTool->addTrack(trk3,kaStarVtx,Trk::kaon);
//            if( status.isFailure() ) {
//                //delete kaStarVtx;
//                //continue;
//                vertex = false;
//            }
//            if(vertex) status = m_vertexingTool->addTrack(trk4,kaStarVtx,Trk::pion);
//            if( status.isFailure() ) {
//                //delete kaStarVtx;
//                //continue;
//                vertex = false;
//            }
//            
//            //////////////
//            //vertex fit
//            if(vertex) status = m_L2vertFitter->fit(kaStarVtx);
//            if(status.isSuccess() && kaStarVtx != NULL) status = m_vertexingTool->calculateInvariantMass(kaStarVtx);
//            if(status.isFailure()){
//                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Kstar vertex fit failed!" << endmsg;
//                delete kaStarVtx;
//                if(timerSvc()) m_VtxFitTimer->pause();
//                //continue; // for performance test not continue?
//                vertex=false;
//            }else{
//                mother_KstarVtx = m_vertexingTool->createTrigVertex(kaStarVtx);
//                status = m_vertexingTool->createMotherParticle(kaStarVtx);
//                if(status.isSuccess()){
//                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Kstar created, with pt: " << kaStarVtx->m_getMotherTrack()->pT() << endmsg;
//                }
//                if(mother_KstarVtx == NULL){
//                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  Vertex fit failed for particle candidate tracks " << trk3
//                        <<" and "<<trk4<< endmsg;
//                    if(timerSvc()) m_VtxFitTimer->pause();
//                    //continue; // for performance test not continue?
//                    vertex = false;
//                }else{
//                    if(msgLvl() <= MSG::DEBUG) {
//                        msg() << MSG::DEBUG << " Kstar vertex fit: x = " <<mother_KstarVtx->x()
//                        <<", y = " <<mother_KstarVtx->y()
//                        <<", z = " <<mother_KstarVtx->z()
//                        <<", mass = " <<mother_KstarVtx->mass()
//                        <<", chi2 = " <<mother_KstarVtx->chi2()
//                        <<", ndof = " <<mother_KstarVtx->ndof() /// (number degrees of freedom)
//                        <<", massVariance = " <<mother_KstarVtx->massVariance()
//                        << endmsg;
//                    }
//                    if(mother_KstarVtx->massVariance() != 0) {
//                        if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  Kstar mass pull = " << (mother_KstarVtx->mass()-KSTARMASS/mother_KstarVtx->massVariance()) << endmsg;
//                    }else{
//                        if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " Kstar mass variance is zero, can't calculate the pull" << endmsg;
//                    }
//                    
//                    /// add vertex results to L2Bphys
//                    trigPartBmumuKstar->fitmass(mother_KstarVtx->mass());
//                    trigPartBmumuKstar->fitchi2(mother_KstarVtx->chi2());
//                    trigPartBmumuKstar->fitndof(mother_KstarVtx->ndof());
//                    trigPartBmumuKstar->fitx(mother_KstarVtx->x());
//                    trigPartBmumuKstar->fity(mother_KstarVtx->y());
//                    trigPartBmumuKstar->fitz(mother_KstarVtx->z());
//                    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " SigmaX =  SigmaY =  SigmaZ = "
//                        << sqrt(mother_KstarVtx->cov()[0]) << " "
//                        << sqrt(mother_KstarVtx->cov()[2]) << " "
//                        << sqrt(mother_KstarVtx->cov()[5])
//                        << endmsg;
//                    
//                    // cut on chi2 and invariant mass of kStar after vertexing.
//                    if(kaStarVtx->chi2() < m_kStarVtxChi2Cut &&
//                       kaStarVtx->mass() > m_lowerKstarVtxMassCut && kaStarVtx->mass() < m_upperKstarVtxMassCut)
//                    {
//                        if(msgLvl() <= MSG::DEBUG)  {
//                            msg() << MSG::DEBUG << " Kstar vertex fit success: x= "
//                            <<            kaStarVtx->m_getParametersVector()[0] //x
//                            << ", y= " << kaStarVtx->m_getParametersVector()[1] //y
//                            << ", z= " << kaStarVtx->m_getParametersVector()[2] //z
//                            << ", chi2= " << kaStarVtx->chi2()
//                            <<", ndof = " <<kaStarVtx->ndof()
//                            << endmsg;
//                        }
//                        goodKstar = true;
//                        
//                        // GREAT -> store daughter particle info
//                        m_VertexColl->push_back(mother_KstarVtx);
//                        if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Added Kstar vertex to vertex collection" << endmsg;
//                        int index = m_VertexColl->size()-1;
//                        ElementLink<TrigVertexCollection> KstarVertexEL(*m_VertexColl,index);
//                        
//                        // monitoring
//                        m_mon_BdMuMuKs_VtxMassBd_Ks.push_back((kaStarVtx->mass())*0.001);
//                        m_mon_BdMuMuKs_Chi2toNDoFBd_Ks.push_back((kaStarVtx->chi2())/(kaStarVtx->ndof()));
//                        
//                        m_mon_BdMuMuKs_VtxPtKs_Ka.push_back((trk3->param()->pT())*0.001);
//                        m_mon_BdMuMuKs_VtxEtaKs_Ka.push_back(trk3->param()->eta());
//                        m_mon_BdMuMuKs_VtxPhiKs_Ka.push_back(trk3->param()->phi0());
//                        m_mon_BdMuMuKs_VtxPtKs_Pi.push_back((trk4->param()->pT())*0.001);
//                        m_mon_BdMuMuKs_VtxEtaKs_Pi.push_back(trk4->param()->eta());
//                        m_mon_BdMuMuKs_VtxPhiKs_Pi.push_back(trk4->param()->phi0());
//                        
//                        if(msgLvl() <= MSG::DEBUG) {
//                            msg() << MSG::DEBUG << " Finnished Bphys daughter particle with fitmass" << trigPartBmumuKstar->fitmass() << " mass " << trigPartBmumuKstar->mass()
//                            << " phi, eta - more RoIs => 0" << trigPartBmumuKstar->phi() << " " << trigPartBmumuKstar->eta() << " vertex type " << trigPartBmumuKstar->particleType()
//                            << endmsg;
//                        }
//                        
//                    }else{
//                        if(msgLvl() <= MSG::DEBUG) {
//                            msg() << MSG::DEBUG << " B0 rejected by mass or chi2 cuts after vertex fit!"
//                            << ", chi2= " << kaStarVtx->chi2()
//                            << endmsg;
//                        }
//                        if(timerSvc()) m_VtxFitTimer->pause();
//                        //continue; // for performance test not continue
//                        vertex = false;
//                        delete mother_KstarVtx;
//                    }
//                } // end of successful fit condition "mother"
//                //int index = m_VertexColl->size()-1;
//                //ElementLink<TrigVertexCollection> BplusVertexEL(*m_VertexColl,index);
//                delete kaStarVtx;
//            } // end of successful fit condition initial vertex
//            if(timerSvc()) m_VtxFitTimer->pause();
//        } // end Kstar_KaonPion vertex fit
//        
//        m_trigBphysColl_kStar->push_back(trigPartBmumuKstar);
//        int KstarIndex = m_trigBphysColl_kStar->size() - 1;
//        
//        if(m_doKstar_KaonPionVertexFit && goodKstar) {
//            if(msgLvl() <= MSG::DEBUG) {
//                msg() << MSG::DEBUG << " Well done in Kstar vertexing!!!" << endmsg;
//                //continue; // OBSOLETE, done inside vertexing
//            }
//            
//            
//            // Bd+/- vertexing & mass cut
//            if(m_doBd_KstarMuMuVertexFit && m_doVertexFit){
//                bool vertex = true;
//                if(timerSvc()) m_VtxFitTimer->resume();
//                TrigL2Vertex* baDVtx = NULL;
//                TrigVertex* mother_BdVtx=NULL;
//                baDVtx = new TrigL2Vertex();
//                StatusCode status;
//                status = m_vertexingTool->addTrack(mu1->IDTrack(),baDVtx,Trk::muon);   //
//                if(status.isFailure()){
//                    //delete baDVtx;
//                    if(timerSvc()) m_VtxFitTimer->pause();
//                    //continue;
//                    vertex = false;
//                }
//                if(vertex) status = m_vertexingTool->addTrack(mu2->IDTrack(),baDVtx,Trk::muon);   //
//                if(status.isFailure()){
//                    //delete baDVtx;
//                    if(timerSvc()) m_VtxFitTimer->pause();
//                    //continue;
//                    vertex = false;
//                }
//                if(vertex) status = m_vertexingTool->addTrack(trk3,baDVtx,Trk::kaon);
//                if(status.isFailure()){
//                    //delete baDVtx;
//                    if(timerSvc()) m_VtxFitTimer->pause();
//                    //continue;
//                    vertex = false;
//                }
//                if(vertex) status = m_vertexingTool->addTrack(trk4,baDVtx,Trk::pion);
//                if(status.isFailure()){
//                    //delete baDVtx;
//                    if(timerSvc()) m_VtxFitTimer->pause();
//                    //continue;
//                    vertex = false;
//                }
//                
//                ElementLink<TrigL2BphysContainer> trigPartBdEL(*m_trigBphysColl_kStar,KstarIndex);
//                
//                TrigL2Bphys* trigPartBmumuBd = new TrigL2Bphys(0.0, 0.0, 0.0, TrigL2Bphys::BDKSTMUMU, xMuMuMass, trigPartBdEL);
//                
//                //vertex fit
//                if(vertex) status = m_L2vertFitter->fit(baDVtx);
//                if(status.isSuccess()) status = m_vertexingTool->calculateInvariantMass(baDVtx);
//                if(status.isFailure()){
//                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Bd vertex fit failed!" << endmsg;
//                    delete baDVtx;
//                    //continue; // for performance test not continue
//                    vertex = false;
//                }else{
//                    mother_BdVtx = m_vertexingTool->createTrigVertex(baDVtx);
//                    status = m_vertexingTool->createMotherParticle(baDVtx);
//                    if(status.isSuccess()){
//                        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Bd created, with pt: " << baDVtx->m_getMotherTrack()->pT() << endmsg;
//                    }
//                    if(mother_BdVtx == NULL){
//                        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  Vertex fit failed for particle candidate track " << trk3
//                            <<" and "<<trk4<< endmsg;
//                        //continue; // for performance test not continue
//                        vertex = false;
//                    }else{
//                        if(msgLvl() <= MSG::DEBUG) {
//                            msg() << MSG::DEBUG << " Bd vertex fit: x = " <<mother_BdVtx->x()
//                            <<", y = " <<mother_BdVtx->y()
//                            <<", z = " <<mother_BdVtx->z()
//                            <<", mass = " <<mother_BdVtx->mass()
//                            <<", chi2 = " <<mother_BdVtx->chi2()
//                            <<", ndof = " <<mother_BdVtx->ndof() /// (number degrees of freedom)
//                            <<", massVariance = " <<mother_BdVtx->massVariance()
//                            << endmsg;
//                        }
//                        if(mother_BdVtx->massVariance() != 0) {
//                            if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  Bd mass pull = " << (mother_BdVtx->mass()-BDMASS/mother_BdVtx->massVariance()) << endmsg;
//                        }else{
//                            if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " Bd mass variance is zero, can't calculate the pull" << endmsg;
//                        }
//                        /// add vertex results to L2Bphys
//                        trigPartBmumuBd->fitmass(mother_BdVtx->mass());
//                        trigPartBmumuBd->fitchi2(mother_BdVtx->chi2());
//                        trigPartBmumuBd->fitndof(mother_BdVtx->ndof());
//                        trigPartBmumuBd->fitx(mother_BdVtx->x());
//                        trigPartBmumuBd->fity(mother_BdVtx->y());
//                        trigPartBmumuBd->fitz(mother_BdVtx->z());
//                        
//                        if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " SigmaX =  SigmaY =  SigmaZ = "
//                            << sqrt(mother_BdVtx->cov()[0]) << " "
//                            << sqrt(mother_BdVtx->cov()[2]) << " "
//                            << sqrt(mother_BdVtx->cov()[5])
//                            << endmsg;
//                        
//                        // cut on chi2 and invariant mass of Bd after vertexing.
//                        if(baDVtx->chi2() < m_bDVtxChi2Cut && baDVtx->mass() > m_lowerBdVtxMassCut && baDVtx->mass() < m_upperBdVtxMassCut)
//                        {
//                            if(msgLvl() <= MSG::DEBUG)  {
//                                msg() << MSG::DEBUG << " Bd vertex fit success: x= "
//                                <<            baDVtx->m_getParametersVector()[0] //x
//                                << ", y= " << baDVtx->m_getParametersVector()[1] //y
//                                << ", z= " << baDVtx->m_getParametersVector()[2] //z
//                                << ", chi2= " << baDVtx->chi2()
//                                <<", ndof = " <<baDVtx->ndof()
//                                << endmsg;
//                            }
//                            
//                            // PERFECT -> Bphys particle was stored in collection
//                            m_VertexColl->push_back(mother_BdVtx);
//                            m_trigBphysColl_b->push_back(trigPartBmumuBd);
//                            if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Added Bd vertex to vertex collection" << endmsg;
//                            
//                            // monitoring of successfuly stored particles
//                            m_mon_BdMuMuKs_n++;
//                            m_mon_Acceptance.push_back( ACCEPT_BphysCollParticle );
//                            m_mon_BdMuMuKs_VtxMass_Bd.push_back((baDVtx->mass())*0.001);
//                            m_mon_BdMuMuKs_Chi2toNDoF_Bd.push_back((baDVtx->chi2())/(baDVtx->ndof()));
//                            
//                            m_mon_BdMuMuKs_VtxPtBd_Ka.push_back((trk3->param()->pT())*0.001);
//                            m_mon_BdMuMuKs_VtxEtaBd_Ka.push_back(trk3->param()->eta());
//                            m_mon_BdMuMuKs_VtxPhiBd_Ka.push_back(trk3->param()->phi0());
//                            m_mon_BdMuMuKs_VtxPtBd_Pi.push_back((trk4->param()->pT())*0.001);
//                            m_mon_BdMuMuKs_VtxEtaBd_Pi.push_back(trk4->param()->eta());
//                            m_mon_BdMuMuKs_VtxPhiBd_Pi.push_back(trk4->param()->phi0());
//                            
//                            if(msgLvl() <= MSG::DEBUG) {
//                                msg() << MSG::DEBUG << " Finnished Bphys particle with fitmass" << trigPartBmumuBd->fitmass() << " mass " << trigPartBmumuBd->mass()
//                                << " phi, eta - more RoIs => 0" << trigPartBmumuBd->phi() << " " << trigPartBmumuBd->eta() << " vertex type " << trigPartBmumuBd->particleType()
//                                << endmsg;
//                            }
//                            
//                        }else{
//                            if(msgLvl() <= MSG::DEBUG) {
//                                msg() << MSG::DEBUG << " Bd rejected by mass or chi2 cuts after vertex fit!"
//                                << ", chi2= " << baDVtx->chi2()
//                                << endmsg;
//                                //continue; // for performance test not continue
//                                vertex = false;
//                            }
//                        }
//                    } // end of successful fit condition "mother"
//                    if (vertex){
//                        int index = m_VertexColl->size()-1;
//                        ElementLink<TrigVertexCollection> BdVertexEL(*m_VertexColl,index);
//                    } else {
//                        delete mother_BdVtx;
//                    }
//                    delete baDVtx;
//                } // end of successful fit condition initial vertex
//                
//            } // Bd -> K* Mu Mu vertexing
//        }  // test over failed vertexing
//        
//        //////////////////////////////// vertexing off ///////////////////////////////////////////////
//        if(!m_doBd_KstarMuMuVertexFit || !m_doVertexFit){
//            if ((xMuMuMass > m_lowerBd_KstarMuMuMassCutVtxOff && xMuMuMass < m_upperBd_KstarMuMuMassCutVtxOff)) {
//                
//                // Store links to the tracks forming bphys particle
//                ElementLinkVector<TrigInDetTrackCollection> trackVectorBmumuBd;
//                ElementLink<TrigInDetTrackCollection> track1ELBd = mu1->IDTrackLink();
//                ElementLink<TrigInDetTrackCollection> track2ELBd = mu2->IDTrackLink();
//                ElementLink<TrigInDetTrackCollection> track3ELBd(*trkCollection, iTrk3);
//                ElementLink<TrigInDetTrackCollection> track4ELBd(*trkCollection, iTrk4);
//                
//                if ( msgLvl() <= MSG::VERBOSE ){
//                    msg() << MSG::VERBOSE << "Just check track links... " << endmsg;
//                    msg() << MSG::VERBOSE << "Muon 1 pT " << (*track1ELBd)->param()->pT() << " eta: " << (*track1ELBd)->param()->eta() << " phi: " << (*track1ELBd)->param()->phi0() << endmsg;
//                    msg() << MSG::VERBOSE << "Muon 2 pT " << (*track2ELBd)->param()->pT() << " eta: " << (*track2ELBd)->param()->eta() << " phi: " << (*track2ELBd)->param()->phi0() << endmsg;
//                    msg() << MSG::VERBOSE << "Track 1 pT " << (*track3ELBd)->param()->pT() << " eta: " << (*track3ELBd)->param()->eta() << " phi: " << (*track3ELBd)->param()->phi0() << endmsg;
//                    msg() << MSG::VERBOSE << "Track 2 pT " << (*track4ELBd)->param()->pT() << " eta: " << (*track4ELBd)->param()->eta() << " phi: " << (*track4ELBd)->param()->phi0() << endmsg;
//                }
//                trackVectorBmumuBd.push_back(track1ELBd); // temp
//                trackVectorBmumuBd.push_back(track2ELBd); // temp
//                trackVectorBmumuBd.push_back(track3ELBd); // temp
//                trackVectorBmumuBd.push_back(track4ELBd); // temp
//                
//                
//                //                 if(m_doKstar_KaonPionVertexFit){
//                ElementLink<TrigL2BphysContainer> trigPartBdEL(*m_trigBphysColl_kStar,KstarIndex);
//                TrigL2Bphys* trigPartBmumuBd = new TrigL2Bphys(0.0, 0.0, 0.0, TrigL2Bphys::BDKSTMUMU, xMuMuMass, trigPartBdEL);
//                trigPartBmumuBd->addTrack(track1ELBd);
//                trigPartBmumuBd->addTrack(track2ELBd);
//                trigPartBmumuBd->addTrack(track3ELBd);
//                trigPartBmumuBd->addTrack(track4ELBd);
//                m_trigBphysColl_b->push_back(trigPartBmumuBd);
//                //                 }else{
//                //                     ElementLink<TrigL2BphysContainer> trigPartBdEL(*m_trigBphysColl_kStar,KstarIndex);
//                //                     TrigL2Bphys* trigPartBmumuBd = new TrigL2Bphys(0., 0., 0., TrigL2Bphys::BDKSTMUMU, xMuMuMass, trigPartBdEL);
//                //                     trigPartBmumuBd->addTrack(track1ELBd);
//                //                     trigPartBmumuBd->addTrack(track2ELBd);
//                //                     trigPartBmumuBd->addTrack(track3ELBd);
//                //                     trigPartBmumuBd->addTrack(track4ELBd);
//                //                     m_trigBphysColl_b->push_back(trigPartBmumuBd);
//                //                 }
//                
//                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Kstar's Pion and Kaon Candidates accepted with K* mass: " << xMass << "MeV"
//                    << " and with Bd mass : " << xMuMuMass << "MeV"
//                    << " kaon particle with pt: "<< trk3->param()->pT()
//                    << " pion charged particle with pt: "<< trk4->param()->pT()
//                    << endmsg;
//                if(msgLvl() <= MSG::DEBUG)
//                    msg() << MSG::DEBUG << " Added Bd particle to Bphys particle collection   "
//                    //<< ", eta=" trigPartBmumuKplus->eta()
//                    //<< ", phi= " << trigPartBmumuKplus->phi()
//                    << ", mass= " << trigPartBmumuBd->mass()
//                    << ", particle type= " << trigPartBmumuBd->particleType()
//                    << endmsg;
//                m_mon_BdMuMuKs_n++;
//                m_mon_BdMuMuKs_InvMassBd_Ks.push_back((xMass*0.001));
//                m_mon_BdMuMuKs_InvMass_Bd.push_back((xMuMuMass*0.001));
//                m_mon_BdMuMuKs_PtKs_Ka.push_back(trk3->param()->pT());
//                m_mon_BdMuMuKs_EtaKs_Ka.push_back(trk3->param()->eta());
//                m_mon_BdMuMuKs_PhiKs_Ka.push_back(trk3->param()->phi0());
//                m_mon_BdMuMuKs_PtKs_Pi.push_back(trk4->param()->pT());
//                m_mon_BdMuMuKs_EtaKs_Pi.push_back(trk4->param()->eta());
//                m_mon_BdMuMuKs_PhiKs_Pi.push_back(trk4->param()->phi0());
//                
//            }else{// end of tighter mass cuts
//                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Bd rejected due to Bd mass cut (no vertexing) " << endmsg;
//            }
//            
//        } // end of no vertexing part
//        
//        
//    } else { // end of if Kstar and Bd successful mass cuts
//        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Bd rejected due to K* or Bd mass cut " << endmsg;
//    }
//}
//
//
////OLD
///*----------------------------------------------------------------------------*/
//void TrigL2BMuMuXFex::checkBsMuMuPhi(const CombinedMuonFeature* mu1, const CombinedMuonFeature* mu2, const TrigInDetTrack* trk3, const TrigInDetTrack* trk4, const TrigInDetTrackCollection* trkCollection, int iTrk3, int iTrk4) {
//    
//    double xMass = XMass(trk3,trk4, bS_to_Phi);
//    double xMuMuMass = XMuMuMass(mu1->IDTrack(), mu2->IDTrack(),trk3,trk4, bS_to_Phi);
//    
//    if ((xMass > m_lowerPhi1020_KaonMassCut && xMass < m_upperPhi1020_KaonMassCut) && (xMuMuMass > m_lowerBs_Phi1020MuMuMassCut && xMuMuMass < m_upperBs_Phi1020MuMuMassCut)) {
//        // Now make L2Bphys object - initial addon
//        TrigL2Bphys* trigPartBmumuPhi = new TrigL2Bphys(0., 0., 0., TrigL2Bphys::PHIKK, xMass);
//        if(msgLvl() <= MSG::DEBUG) {
//            msg() << MSG::DEBUG << " Create Phi1020 Bphys particle with roIId - more RoIs => " << trigPartBmumuPhi->roiId() << " mass " << xMass
//            << " phi, eta - more RoIs => " << trigPartBmumuPhi->phi() << " " << trigPartBmumuPhi->eta() << " vertex type " << trigPartBmumuPhi->particleType()
//            << endmsg;
//        }
//        
//        // Store links to the tracks forming bphys particle
//        ElementLinkVector<TrigInDetTrackCollection> trackVectorBmumuPhi;
//        ElementLink<TrigInDetTrackCollection> track3EL(*trkCollection, iTrk3);
//        ElementLink<TrigInDetTrackCollection> track4EL(*trkCollection, iTrk4);
//        
//        if ( msgLvl() <= MSG::VERBOSE ){
//            msg() << MSG::VERBOSE << "Just check track links... " << endmsg;
//            msg() << MSG::VERBOSE << "Track 1 pT " << (*track3EL)->param()->pT() << " eta: " << (*track3EL)->param()->eta() << " phi: " << (*track3EL)->param()->phi0() << endmsg;
//            msg() << MSG::VERBOSE << "Track 2 pT " << (*track4EL)->param()->pT() << " eta: " << (*track4EL)->param()->eta() << " phi: " << (*track4EL)->param()->phi0() << endmsg;
//        }
//        trackVectorBmumuPhi.push_back(track3EL); // temp
//        trackVectorBmumuPhi.push_back(track4EL); // temp
//        
//        trigPartBmumuPhi->addTrack(track3EL);
//        trigPartBmumuPhi->addTrack(track4EL);
//        // end of initial addon
//        
//        if(trk3->param()->pT() > 0) {
//            //       if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Phi's Pion and Kaon Candidates accepted with K* mass: " << xMass << "MeV"
//            //                                            << " and with Bd mass : " << xMuMuMass << "MeV"
//            //                                          << " positive charged particle with pt: "<< trk3->param()->pT()
//            //                                          << " negative charged particle with pt: "<< trk4->param()->pT()
//            //                                          << endmsg;
//            m_mon_BsMuMuPhi_InvMassBs_Phi.push_back((xMass*0.001));
//            m_mon_BsMuMuPhi_InvMass_Bs.push_back((xMuMuMass*0.001));
//            m_mon_BsMuMuPhi_PtPhi_Kp.push_back(trk3->param()->pT());
//            m_mon_BsMuMuPhi_EtaPhi_Kp.push_back(trk3->param()->eta());
//            m_mon_BsMuMuPhi_PhiPhi_Kp.push_back(trk3->param()->phi0());
//            m_mon_BsMuMuPhi_PtPhi_Km.push_back(trk4->param()->pT());
//            m_mon_BsMuMuPhi_EtaPhi_Km.push_back(trk4->param()->eta());
//            m_mon_BsMuMuPhi_PhiPhi_Km.push_back(trk4->param()->phi0());
//        }else{
//            //       if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Phi's Pion and Kaon Candidates accepted with K* mass: " << xMass << "MeV"
//            //                                            << " and with Bd mass : " << xMuMuMass << "MeV"
//            //                                            << " positive charged particle with pt: "<< trk4->param()->pT()
//            //                                            << " negative charged particle with pt: "<< trk3->param()->pT()
//            //                                              <<endmsg;
//            m_mon_BsMuMuPhi_InvMassBs_Phi.push_back((xMass*0.001));
//            m_mon_BsMuMuPhi_InvMass_Bs.push_back((xMuMuMass*0.001));
//            m_mon_BsMuMuPhi_PtPhi_Kp.push_back(trk4->param()->pT());
//            m_mon_BsMuMuPhi_EtaPhi_Kp.push_back(trk4->param()->eta());
//            m_mon_BsMuMuPhi_PhiPhi_Kp.push_back(trk4->param()->phi0());
//            m_mon_BsMuMuPhi_PtPhi_Km.push_back(trk3->param()->pT());
//            m_mon_BsMuMuPhi_EtaPhi_Km.push_back(trk3->param()->eta());
//            m_mon_BsMuMuPhi_PhiPhi_Km.push_back(trk3->param()->phi0());
//        }
//        bool goodPhi = false;
//        //K* vertexing & mass cut
//        if(m_doPhi1020_KaonKaonVertexFit && m_doVertexFit) {
//            bool vertex = true;
//            TrigL2Vertex* phia1020Vtx = NULL;
//            TrigVertex*   mother_PhiVtx = NULL;
//            // timer
//            if(timerSvc()) m_VtxFitTimer->resume();
//            phia1020Vtx = new TrigL2Vertex();
//            StatusCode status;
//            status = m_vertexingTool->addTrack(trk3,phia1020Vtx,Trk::kaon);
//            if( status.isFailure() ) {
//                //delete phia1020Vtx;
//                //continue;
//                vertex = false;
//            }
//            if(vertex) status = m_vertexingTool->addTrack(trk4,phia1020Vtx,Trk::kaon);
//            if( status.isFailure() ) {
//                //delete phia1020Vtx;
//                //continue;
//                vertex = false;
//            }
//            //////////////
//            //vertex fit
//            if(vertex) status = m_L2vertFitter->fit(phia1020Vtx);
//            if(status.isSuccess()) status = m_vertexingTool->calculateInvariantMass(phia1020Vtx);
//            if(status.isFailure()){
//                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Phi vertex fit failed!" << endmsg;
//                delete phia1020Vtx;
//                if(timerSvc()) m_VtxFitTimer->pause();
//                //continue; // for performance test not continue?
//                vertex = false;
//            }else{
//                mother_PhiVtx = m_vertexingTool->createTrigVertex(phia1020Vtx);
//                status = m_vertexingTool->createMotherParticle(phia1020Vtx);
//                if(status.isSuccess()){
//                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Phi created, with pt: " << phia1020Vtx->m_getMotherTrack()->pT() << endmsg;
//                }
//                if(mother_PhiVtx == NULL){
//                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  Vertex fit failed for particle candidate tracks " << trk3
//                        <<" and "<<trk4<< endmsg;
//                    if(timerSvc()) m_VtxFitTimer->pause();
//                    //continue; // for performance test not continue?
//                    vertex = false;
//                }else{
//                    if(msgLvl() <= MSG::DEBUG) {
//                        msg() << MSG::DEBUG << " Phi vertex fit: x = " <<mother_PhiVtx->x()
//                        <<", y = " <<mother_PhiVtx->y()
//                        <<", z = " <<mother_PhiVtx->z()
//                        <<", mass = " <<mother_PhiVtx->mass()
//                        <<", chi2 = " <<mother_PhiVtx->chi2()
//                        <<", ndof = " <<mother_PhiVtx->ndof() /// (number degrees of freedom)
//                        <<", massVariance = " <<mother_PhiVtx->massVariance()
//                        << endmsg;
//                    }
//                    if(mother_PhiVtx->massVariance() != 0) {
//                        if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  Phi mass pull = " << (mother_PhiVtx->mass()-PHIMASS/mother_PhiVtx->massVariance()) << endmsg;
//                    }else{
//                        if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " Phi mass variance is zero, can't calculate the pull" << endmsg;
//                    }
//                    
//                    /// add vertex results to L2Bphys
//                    trigPartBmumuPhi->fitmass(mother_PhiVtx->mass());
//                    trigPartBmumuPhi->fitchi2(mother_PhiVtx->chi2());
//                    trigPartBmumuPhi->fitndof(mother_PhiVtx->ndof());
//                    trigPartBmumuPhi->fitx(mother_PhiVtx->x());
//                    trigPartBmumuPhi->fity(mother_PhiVtx->y());
//                    trigPartBmumuPhi->fitz(mother_PhiVtx->z());
//                    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " SigmaX =  SigmaY =  SigmaZ = "
//                        << sqrt(mother_PhiVtx->cov()[0]) << " "
//                        << sqrt(mother_PhiVtx->cov()[2]) << " "
//                        << sqrt(mother_PhiVtx->cov()[5])
//                        << endmsg;
//                    
//                    // cut on chi2 and invariant mass of bplus after vertexing.
//                    if(phia1020Vtx->chi2() < m_phi1020VtxChi2Cut &&
//                       phia1020Vtx->mass() > m_lowerPhi1020VtxMassCut && phia1020Vtx->mass() < m_upperPhi1020VtxMassCut)
//                    {
//                        if(msgLvl() <= MSG::DEBUG)  {
//                            msg() << MSG::DEBUG << " Phi vertex fit success: x= "
//                            <<            phia1020Vtx->m_getParametersVector()[0] //x
//                            << ", y= " << phia1020Vtx->m_getParametersVector()[1] //y
//                            << ", z= " << phia1020Vtx->m_getParametersVector()[2] //z
//                            << ", chi2= " << phia1020Vtx->chi2()
//                            <<", ndof = " <<phia1020Vtx->ndof()
//                            << endmsg;
//                        }
//                        goodPhi = true;
//                        // GREAT -> store daughter particle info
//                        m_VertexColl->push_back(mother_PhiVtx);
//                        if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Added Phi vertex to vertex collection" << endmsg;
//                        int index = m_VertexColl->size()-1;
//                        ElementLink<TrigVertexCollection> PhiVertexEL(*m_VertexColl,index);
//                        
//                        // monitoring
//                        m_mon_BsMuMuPhi_VtxMassBs_Phi.push_back((phia1020Vtx->mass())*0.001);
//                        m_mon_BsMuMuPhi_Chi2toNDoFBs_Phi.push_back((phia1020Vtx->chi2())/(phia1020Vtx->ndof()));
//                        if(trk3->param()->pT() > 0) {
//                            m_mon_BsMuMuPhi_VtxPtPhi_Kp.push_back((trk3->param()->pT())*0.001);
//                            m_mon_BsMuMuPhi_VtxEtaPhi_Kp.push_back(trk3->param()->eta());
//                            m_mon_BsMuMuPhi_VtxPhiPhi_Kp.push_back(trk3->param()->phi0());
//                            m_mon_BsMuMuPhi_VtxPtPhi_Km.push_back((trk4->param()->pT())*0.001);
//                            m_mon_BsMuMuPhi_VtxEtaPhi_Km.push_back(trk4->param()->eta());
//                            m_mon_BsMuMuPhi_VtxPhiPhi_Km.push_back(trk4->param()->phi0());
//                        }else{
//                            m_mon_BsMuMuPhi_VtxPtPhi_Kp.push_back((trk4->param()->pT())*0.001);
//                            m_mon_BsMuMuPhi_VtxEtaPhi_Kp.push_back(trk4->param()->eta());
//                            m_mon_BsMuMuPhi_VtxPhiPhi_Kp.push_back(trk4->param()->phi0());
//                            m_mon_BsMuMuPhi_VtxPtPhi_Km.push_back((trk3->param()->pT())*0.001);
//                            m_mon_BsMuMuPhi_VtxEtaPhi_Km.push_back(trk3->param()->eta());
//                            m_mon_BsMuMuPhi_VtxPhiPhi_Km.push_back(trk3->param()->phi0());
//                        }
//                        if(msgLvl() <= MSG::DEBUG) {
//                            msg() << MSG::DEBUG << " Finnished Bphys daughter particle with fitmass" << trigPartBmumuPhi->fitmass() << " mass " << trigPartBmumuPhi->mass()
//                            << " phi, eta - more RoIs => 0" << trigPartBmumuPhi->phi() << " " << trigPartBmumuPhi->eta() << " vertex type " << trigPartBmumuPhi->particleType()
//                            << endmsg;
//                        }
//                        
//                    }else{
//                        if(msgLvl() <= MSG::DEBUG) {
//                            msg() << MSG::DEBUG << " Phi rejected by mass or chi2 cuts after vertex fit!"
//                            << ", chi2= " << phia1020Vtx->chi2()
//                            << endmsg;
//                        }
//                        if(timerSvc()) m_VtxFitTimer->pause();
//                        //continue; // for performance test not continue
//                        vertex = false;
//                        delete mother_PhiVtx;
//                    }
//                } // end of successful fit condition "mother"
//                //int index = m_VertexColl->size()-1;
//                //ElementLink<TrigVertexCollection> BplusVertexEL(*m_VertexColl,index);
//                delete phia1020Vtx;
//            } // end of successful fit condition initial vertex
//            if(timerSvc()) m_VtxFitTimer->pause();
//        } // end Phi_KaonPion vertex fit
//        
//        m_trigBphysColl_phi->push_back(trigPartBmumuPhi);
//        int PhiIndex = m_trigBphysColl_phi->size() - 1;
//        
//        if(m_doPhi1020_KaonKaonVertexFit && goodPhi) {
//            if(msgLvl() <= MSG::DEBUG) {
//                msg() << MSG::DEBUG << " Well done in Phi vertexing!!!" << endmsg;
//                //    continue; // OBSOLETE, done inside vertexing
//            }
//            
//            // Bd+/- vertexing & mass cut
//            if(m_doBs_Phi1020MuMuVertexFit && m_doVertexFit){
//                if(timerSvc()) m_VtxFitTimer->resume();
//                bool vertex = true;
//                TrigL2Vertex* baSVtx = NULL;
//                TrigVertex* mother_BsVtx=NULL;
//                baSVtx = new TrigL2Vertex();
//                StatusCode status;
//                status = m_vertexingTool->addTrack(mu1->IDTrack(),baSVtx,Trk::muon);   //
//                if(status.isFailure()){
//                    //delete baSVtx;
//                    if(timerSvc()) m_VtxFitTimer->pause();
//                    //continue;
//                    vertex = false;
//                }
//                if (vertex) status = m_vertexingTool->addTrack(mu2->IDTrack(),baSVtx,Trk::muon);   //
//                if(status.isFailure()){
//                    //delete baSVtx;
//                    if(timerSvc()) m_VtxFitTimer->pause();
//                    //continue;
//                    vertex = false;
//                }
//                if (vertex) status = m_vertexingTool->addTrack(trk3,baSVtx,Trk::kaon);
//                if(status.isFailure()){
//                    //delete baSVtx;
//                    if(timerSvc()) m_VtxFitTimer->pause();
//                    //continue;
//                    vertex = false;
//                }
//                if (vertex) status = m_vertexingTool->addTrack(trk4,baSVtx,Trk::kaon);
//                if(status.isFailure()){
//                    //delete baSVtx;
//                    if(timerSvc()) m_VtxFitTimer->pause();
//                    //continue;
//                    vertex = false;
//                }
//                ElementLink<TrigL2BphysContainer> trigPartBsEL(*m_trigBphysColl_phi,PhiIndex);
//                TrigL2Bphys* trigPartBmumuBs = new TrigL2Bphys(0.0, 0.0, 0.0, TrigL2Bphys::BSPHIMUMU, xMuMuMass, trigPartBsEL);
//                
//                //vertex fit
//                if (vertex) status = m_L2vertFitter->fit(baSVtx);
//                if(status.isSuccess()) status = m_vertexingTool->calculateInvariantMass(baSVtx);
//                if(status.isFailure()){
//                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Bs vertex fit failed!" << endmsg;
//                    delete baSVtx;
//                    //continue; // for performance test not continue
//                    vertex = false;
//                }else{
//                    mother_BsVtx = m_vertexingTool->createTrigVertex(baSVtx);
//                    status = m_vertexingTool->createMotherParticle(baSVtx);
//                    if(status.isSuccess()){
//                        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Bs created, with pt: " << baSVtx->m_getMotherTrack()->pT() << endmsg;
//                    }
//                    if(mother_BsVtx == NULL){
//                        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  Vertex fit failed for particle candidate track " << trk3
//                            <<" and "<<trk4<< endmsg;
//                        vertex = false;
//                        //continue; // for performance test not continue
//                    }else{
//                        if(msgLvl() <= MSG::DEBUG) {
//                            msg() << MSG::DEBUG << " Bs vertex fit: x = " <<mother_BsVtx->x()
//                            <<", y = " <<mother_BsVtx->y()
//                            <<", z = " <<mother_BsVtx->z()
//                            <<", mass = " <<mother_BsVtx->mass()
//                            <<", chi2 = " <<mother_BsVtx->chi2()
//                            <<", ndof = " <<mother_BsVtx->ndof() /// (number degrees of freedom)
//                            <<", massVariance = " <<mother_BsVtx->massVariance()
//                            << endmsg;
//                        }
//                        if(mother_BsVtx->massVariance() != 0) {
//                            if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  Bs mass pull = " << (mother_BsVtx->mass()-BSMASS/mother_BsVtx->massVariance()) << endmsg;
//                        }else{
//                            if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " Bs mass variance is zero, can't calculate the pull" << endmsg;
//                        }
//                        
//                        /// add vertex results to L2Bphys
//                        trigPartBmumuBs->fitmass(mother_BsVtx->mass());
//                        trigPartBmumuBs->fitchi2(mother_BsVtx->chi2());
//                        trigPartBmumuBs->fitndof(mother_BsVtx->ndof());
//                        trigPartBmumuBs->fitx(mother_BsVtx->x());
//                        trigPartBmumuBs->fity(mother_BsVtx->y());
//                        trigPartBmumuBs->fitz(mother_BsVtx->z());
//                        
//                        if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " SigmaX =  SigmaY =  SigmaZ = "
//                            << sqrt(mother_BsVtx->cov()[0]) << " "
//                            << sqrt(mother_BsVtx->cov()[2]) << " "
//                            << sqrt(mother_BsVtx->cov()[5])
//                            << endmsg;
//                        
//                        // cut on chi2 and invariant mass of Bd after vertexing.
//                        if(baSVtx->chi2() < m_bSVtxChi2Cut && baSVtx->mass() > m_lowerBsVtxMassCut && baSVtx->mass() < m_upperBsVtxMassCut)
//                        {
//                            if(msgLvl() <= MSG::DEBUG)  {
//                                msg() << MSG::DEBUG << " Bs vertex fit success: x= "
//                                <<            baSVtx->m_getParametersVector()[0] //x
//                                << ", y= " << baSVtx->m_getParametersVector()[1] //y
//                                << ", z= " << baSVtx->m_getParametersVector()[2] //z
//                                << ", chi2= " << baSVtx->chi2()
//                                <<", ndof = " <<baSVtx->ndof()
//                                << endmsg;
//                            }
//                            
//                            // PERFECT -> Bphys particle was stored in collection
//                            m_VertexColl->push_back(mother_BsVtx);
//                            m_trigBphysColl_b->push_back(trigPartBmumuBs);
//                            if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Added Bs vertex to vertex collection" << endmsg;
//                            // monitoring of successfuly stored particles
//                            m_mon_BsMuMuPhi_n++;
//                            m_mon_Acceptance.push_back( ACCEPT_BphysCollParticle );
//                            m_mon_BsMuMuPhi_VtxMass_Bs.push_back((baSVtx->mass())*0.001);
//                            m_mon_BsMuMuPhi_Chi2toNDoF_Bs.push_back((baSVtx->chi2())/(baSVtx->ndof()));
//                            if(trk3->param()->pT() > 0) {
//                                m_mon_BsMuMuPhi_VtxPtBs_Kp.push_back((trk3->param()->pT())*0.001);
//                                m_mon_BsMuMuPhi_VtxEtaBs_Kp.push_back(trk3->param()->eta());
//                                m_mon_BsMuMuPhi_VtxPhiBs_Kp.push_back(trk3->param()->phi0());
//                                m_mon_BsMuMuPhi_VtxPtBs_Km.push_back((trk4->param()->pT())*0.001);
//                                m_mon_BsMuMuPhi_VtxEtaBs_Km.push_back(trk4->param()->eta());
//                                m_mon_BsMuMuPhi_VtxPhiBs_Km.push_back(trk4->param()->phi0());
//                            }else{
//                                m_mon_BsMuMuPhi_VtxPtBs_Kp.push_back((trk4->param()->pT())*0.001);
//                                m_mon_BsMuMuPhi_VtxEtaBs_Kp.push_back(trk4->param()->eta());
//                                m_mon_BsMuMuPhi_VtxPhiBs_Kp.push_back(trk4->param()->phi0());
//                                m_mon_BsMuMuPhi_VtxPtBs_Km.push_back((trk3->param()->pT())*0.001);
//                                m_mon_BsMuMuPhi_VtxEtaBs_Km.push_back(trk3->param()->eta());
//                                m_mon_BsMuMuPhi_VtxPhiBs_Km.push_back(trk3->param()->phi0());
//                            }
//                            if(msgLvl() <= MSG::DEBUG) {
//                                msg() << MSG::DEBUG << " Finnished Bphys particle with fitmass" << trigPartBmumuBs->fitmass() << " mass " << trigPartBmumuBs->mass()
//                                << " phi, eta - more RoIs => 0" << trigPartBmumuBs->phi() << " " << trigPartBmumuBs->eta() << " vertex type " << trigPartBmumuBs->particleType()
//                                << endmsg;
//                            }
//                            
//                        }else{
//                            if(msgLvl() <= MSG::DEBUG) {
//                                msg() << MSG::DEBUG << " Bs rejected by mass or chi2 cuts after vertex fit!"
//                                << ", chi2= " << baSVtx->chi2()
//                                << endmsg;
//                                vertex = false;
//                                //continue; // for performance test not continue
//                            }
//                        }
//                    } // end of successful fit condition "mother"
//                    if(vertex) {
//                        int index = m_VertexColl->size()-1;
//                        ElementLink<TrigVertexCollection> BsVertexEL(*m_VertexColl,index);
//                        delete baSVtx;
//                    } else {
//                        delete mother_BsVtx;
//                    }
//                } // end of successful fit condition initial vertex
//                
//                
//            }   // Bs -> Phi Mu Mu vertexing
//        } // end of test over failed
//        //////////////////////////////// vertexing off ///////////////////////////////////////////////
//        if(!m_doBs_Phi1020MuMuVertexFit || !m_doVertexFit){
//            if ((xMuMuMass > m_lowerBs_Phi1020MuMuMassCutVtxOff && xMuMuMass < m_upperBs_Phi1020MuMuMassCutVtxOff)) {
//                
//                // Store links to the tracks forming bphys particle
//                ElementLinkVector<TrigInDetTrackCollection> trackVectorBmumuBs;
//                ElementLink<TrigInDetTrackCollection> track1ELBs = mu1->IDTrackLink();
//                ElementLink<TrigInDetTrackCollection> track2ELBs = mu2->IDTrackLink();
//                ElementLink<TrigInDetTrackCollection> track3ELBs(*trkCollection, iTrk3);
//                ElementLink<TrigInDetTrackCollection> track4ELBs(*trkCollection, iTrk4);
//                
//                if ( msgLvl() <= MSG::VERBOSE ){
//                    msg() << MSG::VERBOSE << "Just check track links... " << endmsg;
//                    msg() << MSG::VERBOSE << "Muon 1 pT " << (*track1ELBs)->param()->pT() << " eta: " << (*track1ELBs)->param()->eta() << " phi: " << (*track1ELBs)->param()->phi0() << endmsg;
//                    msg() << MSG::VERBOSE << "Muon 2 pT " << (*track2ELBs)->param()->pT() << " eta: " << (*track2ELBs)->param()->eta() << " phi: " << (*track2ELBs)->param()->phi0() << endmsg;
//                    msg() << MSG::VERBOSE << "Track 1 pT " << (*track3ELBs)->param()->pT() << " eta: " << (*track3ELBs)->param()->eta() << " phi: " << (*track3ELBs)->param()->phi0() << endmsg;
//                    msg() << MSG::VERBOSE << "Track 2 pT " << (*track4ELBs)->param()->pT() << " eta: " << (*track4ELBs)->param()->eta() << " phi: " << (*track4ELBs)->param()->phi0() << endmsg;
//                }
//                trackVectorBmumuBs.push_back(track1ELBs); // temp
//                trackVectorBmumuBs.push_back(track2ELBs); // temp
//                trackVectorBmumuBs.push_back(track3ELBs); // temp
//                trackVectorBmumuBs.push_back(track4ELBs); // temp
//                
//                
//                //         if(m_doPhi1020_KaonKaonVertexFit){
//                ElementLink<TrigL2BphysContainer> trigPartBsEL(*m_trigBphysColl_phi,PhiIndex);
//                TrigL2Bphys* trigPartBmumuBs = new TrigL2Bphys(0.0, 0.0, 0.0, TrigL2Bphys::BSPHIMUMU, xMuMuMass, trigPartBsEL);
//                trigPartBmumuBs->addTrack(track1ELBs); 
//                trigPartBmumuBs->addTrack(track2ELBs); 
//                trigPartBmumuBs->addTrack(track3ELBs); 
//                trigPartBmumuBs->addTrack(track4ELBs); 
//                m_trigBphysColl_b->push_back(trigPartBmumuBs);
//                //         }else{
//                // //           m_trigBphysColl_phi->push_back(trigPartBmumuPhi);
//                // //           int PhiIndex = m_trigBphysColl_phi->size() - 1;
//                // //           ElementLink<TrigL2BphysContainer> trigPartBsEL(*m_trigBphysColl_phi,PhiIndex);
//                //           
//                //           TrigL2Bphys* trigPartBmumuBs = new TrigL2Bphys(0., 0., 0., TrigL2Bphys::BSPHIMUMU, xMuMuMass/*, trigPartBsEL*/);
//                //           trigPartBmumuBs->addTrack(track1ELBs); 
//                //             trigPartBmumuBs->addTrack(track2ELBs); 
//                //             trigPartBmumuBs->addTrack(track3ELBs); 
//                //             trigPartBmumuBs->addTrack(track4ELBs); 
//                //           m_trigBphysColl_b->push_back(trigPartBmumuBs);
//                //         }
//                m_mon_BsMuMuPhi_n++;
//                if(trk3->param()->pT() > 0) {
//                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Phi's Kplus and Kminus Candidates accepted with Phi mass: " << xMass << "MeV"  
//                        << " and with Phi1020 mass : " << xMuMuMass << "MeV"  
//                        << " positive charged particle with pt: "<< trk3->param()->pT() 
//                        
//                        << " negative charged particle with pt: "<< trk4->param()->pT() 
//                        << endmsg;
//                    m_mon_BsMuMuPhi_InvMassBs_Phi.push_back((xMass*0.001));
//                    m_mon_BsMuMuPhi_InvMass_Bs.push_back((xMuMuMass*0.001));
//                    m_mon_BsMuMuPhi_PtPhi_Kp.push_back(trk3->param()->pT());
//                    m_mon_BsMuMuPhi_EtaPhi_Kp.push_back(trk3->param()->eta());
//                    m_mon_BsMuMuPhi_PhiPhi_Kp.push_back(trk3->param()->phi0());
//                    m_mon_BsMuMuPhi_PtPhi_Km.push_back(trk4->param()->pT());
//                    m_mon_BsMuMuPhi_EtaPhi_Km.push_back(trk4->param()->eta());
//                    m_mon_BsMuMuPhi_PhiPhi_Km.push_back(trk4->param()->phi0());
//                }else{
//                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Phi's Pion and Kaon Candidates accepted with K* mass: " << xMass << "MeV"  
//                        << " and with Bd mass : " << xMuMuMass << "MeV"  
//                        << " positive charged particle with pt: "<< trk4->param()->pT() 
//                        << " negative charged particle with pt: "<< trk3->param()->pT() 
//                        <<endmsg;
//                    m_mon_BsMuMuPhi_InvMassBs_Phi.push_back((xMass*0.001));
//                    m_mon_BsMuMuPhi_InvMass_Bs.push_back((xMuMuMass*0.001));
//                    m_mon_BsMuMuPhi_PtPhi_Kp.push_back(trk4->param()->pT());
//                    m_mon_BsMuMuPhi_EtaPhi_Kp.push_back(trk4->param()->eta());
//                    m_mon_BsMuMuPhi_PhiPhi_Kp.push_back(trk4->param()->phi0());
//                    m_mon_BsMuMuPhi_PtPhi_Km.push_back(trk3->param()->pT());
//                    m_mon_BsMuMuPhi_EtaPhi_Km.push_back(trk3->param()->eta());
//                    m_mon_BsMuMuPhi_PhiPhi_Km.push_back(trk3->param()->phi0());
//                }
//                if(msgLvl() <= MSG::DEBUG)
//                    msg() << MSG::DEBUG << " Added Bs particle to Bphys particle collection   " 
//                    //<< ", eta=" trigPartBmumuKplus->eta()   
//                    //<< ", phi= " << trigPartBmumuKplus->phi()
//                    << ", mass= " << trigPartBmumuBs->mass() 
//                    << ", particle type= " << trigPartBmumuBs->particleType() 
//                    << endmsg;
//            } else {// end of tighter mass cuts
//                if(msgLvl() <= MSG::DEBUG) 
//                    msg() << MSG::DEBUG << " Bs rejected due to Bs mass cut (no vertexing) " << endmsg;
//            } 
//            
//        } // end of no vertexing part
//        
//        
//    } else { // end of if Phi and Bs successful mass cuts
//        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Bs rejected due to Phi or Bs mass cut " << endmsg;
//    }
//}
//
//// OLD
///*----------------------------------------------------------------------------*/
//void TrigL2BMuMuXFex::checkLbMuMuLambda(const CombinedMuonFeature* mu1, const CombinedMuonFeature* mu2, const TrigInDetTrack* trk3, const TrigInDetTrack* trk4, const TrigInDetTrackCollection* trkCollection, int iTrk3, int iTrk4) {
//    
//    double xMass = 0;
//    double xMuMuMass = 0;
//    xMass = XMass(trk3,trk4, lB_to_L);
//    xMuMuMass = XMuMuMass(mu1->IDTrack(), mu2->IDTrack(),trk3,trk4, lB_to_L);
//    
//    if ((xMass > m_lowerLambda_PrPiMassCut && xMass < m_upperLambda_PrPiMassCut) && (xMuMuMass > m_lowerLb_LambdaMuMuMassCut && xMuMuMass < m_upperLb_LambdaMuMuMassCut)) {
//        // Now make L2Bphys object - initial addon
//        TrigL2Bphys* trigPartBmumuL = new TrigL2Bphys(0., 0., 0., TrigL2Bphys::PHIKK, xMass);
//        if(msgLvl() <= MSG::DEBUG) {
//            msg() << MSG::DEBUG << " Create Lambda Bphys particle with roIId - more RoIs => " << trigPartBmumuL->roiId() << " mass " << xMass
//            << " phi, eta - more RoIs => " << trigPartBmumuL->phi() << " " << trigPartBmumuL->eta() << " vertex type " << trigPartBmumuL->particleType()
//            << endmsg;
//        }
//        
//        // Store links to the tracks forming bphys particle
//        ElementLinkVector<TrigInDetTrackCollection> trackVectorBmumuL;
//        ElementLink<TrigInDetTrackCollection> track3EL(*trkCollection, iTrk3);
//        ElementLink<TrigInDetTrackCollection> track4EL(*trkCollection, iTrk4);
//        
//        if ( msgLvl() <= MSG::VERBOSE ){
//            msg() << MSG::VERBOSE << "Just check track links... " << endmsg;
//            msg() << MSG::VERBOSE << "Track 1 pT " << (*track3EL)->param()->pT() << " eta: " << (*track3EL)->param()->eta() << " phi: " << (*track3EL)->param()->phi0() << endmsg;
//            msg() << MSG::VERBOSE << "Track 2 pT " << (*track4EL)->param()->pT() << " eta: " << (*track4EL)->param()->eta() << " phi: " << (*track4EL)->param()->phi0() << endmsg;
//        }
//        trackVectorBmumuL.push_back(track3EL); // temp
//        trackVectorBmumuL.push_back(track4EL); // temp
//        
//        trigPartBmumuL->addTrack(track3EL);
//        trigPartBmumuL->addTrack(track4EL);
//        // end of initial addon
//        
//        //     if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Lambda's Proton and Pion Candidates accepted with L mass: " << xMass << "MeV"
//        //                                         << " and with Lb mass : " << xMuMuMass << "MeV"
//        //                                       << " proton particle with pt: "<< trk3->param()->pT()
//        //                                       << " pion charged particle with pt: "<< trk4->param()->pT()
//        //                                       << endmsg;
//        m_mon_LbMuMuL_InvMassLb_L.push_back((xMass*0.001));
//        m_mon_LbMuMuL_InvMass_Lb.push_back((xMuMuMass*0.001));
//        m_mon_LbMuMuL_PtL_Pr.push_back(trk3->param()->pT());
//        m_mon_LbMuMuL_EtaL_Pr.push_back(trk3->param()->eta());
//        m_mon_LbMuMuL_PhiL_Pr.push_back(trk3->param()->phi0());
//        m_mon_LbMuMuL_PtL_Pi.push_back(trk4->param()->pT());
//        m_mon_LbMuMuL_EtaL_Pi.push_back(trk4->param()->eta());
//        m_mon_LbMuMuL_PhiL_Pi.push_back(trk4->param()->phi0());
//        
//        
//        bool goodL = false;
//        //Lambda vertexing & mass cut
//        if(m_doLambda_ProtonPionVertexFit && m_doVertexFit) {
//            bool vertex = true;
//            TrigL2Vertex* lambdaaVtx = NULL;
//            TrigVertex*   mother_LVtx = NULL;
//            // timer
//            if(timerSvc()) m_VtxFitTimer->resume();
//            lambdaaVtx = new TrigL2Vertex();
//            StatusCode status;
//            
//            status = m_vertexingTool->addTrack(trk3,lambdaaVtx,Trk::proton);
//            if( status.isFailure() ) {
//                //delete lambdaaVtx;
//                //continue;
//                vertex = false;
//            }
//            if(vertex) status = m_vertexingTool->addTrack(trk4,lambdaaVtx,Trk::pion);
//            if( status.isFailure() ) {
//                //delete lambdaaVtx;
//                //continue;
//                vertex = false;
//            }
//            
//            //////////////
//            //vertex fit
//            if(vertex) status = m_L2vertFitter->fit(lambdaaVtx);
//            if(status.isSuccess()) status = m_vertexingTool->calculateInvariantMass(lambdaaVtx);
//            if(status.isFailure()){
//                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " L vertex fit failed!" << endmsg;
//                delete lambdaaVtx;
//                if(timerSvc()) m_VtxFitTimer->pause();
//                //continue; // for performance test not continue?
//                vertex = false;
//            }else{
//                mother_LVtx = m_vertexingTool->createTrigVertex(lambdaaVtx);
//                status = m_vertexingTool->createMotherParticle(lambdaaVtx);
//                if(status.isSuccess()){
//                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " L created, with pt: " << lambdaaVtx->m_getMotherTrack()->pT() << endmsg;
//                }
//                if(mother_LVtx == NULL){
//                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  Vertex fit failed for particle candidate tracks " << trk3
//                        <<" and "<<trk4<< endmsg;
//                    if(timerSvc()) m_VtxFitTimer->pause();
//                    //continue; // for performance test not continue?
//                    vertex = false;
//                }else{
//                    if(msgLvl() <= MSG::DEBUG) {
//                        msg() << MSG::DEBUG << " L vertex fit: x = " <<mother_LVtx->x()
//                        <<", y = " <<mother_LVtx->y()
//                        <<", z = " <<mother_LVtx->z()
//                        <<", mass = " <<mother_LVtx->mass()
//                        <<", chi2 = " <<mother_LVtx->chi2()
//                        <<", ndof = " <<mother_LVtx->ndof() /// (number degrees of freedom)
//                        <<", massVariance = " <<mother_LVtx->massVariance()
//                        << endmsg;
//                    }
//                    if(mother_LVtx->massVariance() != 0) {
//                        if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  L mass pull = " << (mother_LVtx->mass()-LAMBDAMASS/mother_LVtx->massVariance()) << endmsg;
//                    }else{
//                        if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " L mass variance is zero, can't calculate the pull" << endmsg;
//                    }
//                    
//                    /// add vertex results to L2Bphys
//                    trigPartBmumuL->fitmass(mother_LVtx->mass());
//                    trigPartBmumuL->fitchi2(mother_LVtx->chi2());
//                    trigPartBmumuL->fitndof(mother_LVtx->ndof());
//                    trigPartBmumuL->fitx(mother_LVtx->x());
//                    trigPartBmumuL->fity(mother_LVtx->y());
//                    trigPartBmumuL->fitz(mother_LVtx->z());
//                    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " SigmaX =  SigmaY =  SigmaZ = "
//                        << sqrt(mother_LVtx->cov()[0]) << " "
//                        << sqrt(mother_LVtx->cov()[2]) << " "
//                        << sqrt(mother_LVtx->cov()[5])
//                        << endmsg;
//                    
//                    // cut on chi2 and invariant mass of bplus after vertexing.
//                    if(lambdaaVtx->chi2() < m_lambdaVtxChi2Cut &&
//                       lambdaaVtx->mass() > m_lowerLambdaVtxMassCut && lambdaaVtx->mass() < m_upperLambdaVtxMassCut)
//                    {
//                        if(msgLvl() <= MSG::DEBUG)  {
//                            msg() << MSG::DEBUG << " L vertex fit success: x= "
//                            <<            lambdaaVtx->m_getParametersVector()[0] //x
//                            << ", y= " << lambdaaVtx->m_getParametersVector()[1] //y
//                            << ", z= " << lambdaaVtx->m_getParametersVector()[2] //z
//                            << ", chi2= " << lambdaaVtx->chi2()
//                            <<", ndof = " <<lambdaaVtx->ndof()
//                            << endmsg;
//                        }
//                        goodL = true;
//                        // GREAT -> store daughter particle info
//                        m_VertexColl->push_back(mother_LVtx);
//                        if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Added L vertex to vertex collection" << endmsg;
//                        int index = m_VertexColl->size()-1;
//                        ElementLink<TrigVertexCollection> LVertexEL(*m_VertexColl,index);
//                        
//                        // monitoring
//                        m_mon_LbMuMuL_VtxMassLb_L.push_back((lambdaaVtx->mass())*0.001);
//                        m_mon_LbMuMuL_Chi2toNDoFLb_L.push_back((lambdaaVtx->chi2())/(lambdaaVtx->ndof()));
//                        
//                        m_mon_LbMuMuL_VtxPtL_Pr.push_back((trk3->param()->pT())*0.001);
//                        m_mon_LbMuMuL_VtxEtaL_Pr.push_back(trk3->param()->eta());
//                        m_mon_LbMuMuL_VtxPhiL_Pr.push_back(trk3->param()->phi0());
//                        m_mon_LbMuMuL_VtxPtL_Pi.push_back((trk4->param()->pT())*0.001);
//                        m_mon_LbMuMuL_VtxEtaL_Pi.push_back(trk4->param()->eta());
//                        m_mon_LbMuMuL_VtxPhiL_Pi.push_back(trk4->param()->phi0());
//                        
//                        if(msgLvl() <= MSG::DEBUG) {
//                            msg() << MSG::DEBUG << " Finnished Bphys daughter particle with fitmass" << trigPartBmumuL->fitmass() << " mass " << trigPartBmumuL->mass()
//                            << " phi, eta - more RoIs => 0" << trigPartBmumuL->phi() << " " << trigPartBmumuL->eta() << " vertex type " << trigPartBmumuL->particleType()
//                            << endmsg;
//                        }
//                        
//                    }else{
//                        if(msgLvl() <= MSG::DEBUG) {
//                            msg() << MSG::DEBUG << " L rejected by mass or chi2 cuts after vertex fit!"
//                            << ", chi2= " << lambdaaVtx->chi2()
//                            << endmsg;
//                        }
//                        if(timerSvc()) m_VtxFitTimer->pause();
//                        //continue; // for performance test not continue
//                        vertex = false;
//                        delete mother_LVtx;
//                    }
//                } // end of successful fit condition "mother"
//                //int index = m_VertexColl->size()-1;
//                //ElementLink<TrigVertexCollection> BplusVertexEL(*m_VertexColl,index);
//                delete lambdaaVtx;
//            } // end of successful fit condition initial vertex
//            if(timerSvc()) m_VtxFitTimer->pause();
//        } // end Lambda_ProtonPion vertex fit
//        
//        m_trigBphysColl_lambda->push_back(trigPartBmumuL);
//        int LIndex = m_trigBphysColl_lambda->size() - 1;
//        
//        if(m_doLambda_ProtonPionVertexFit && goodL) {
//            if(msgLvl() <= MSG::DEBUG) {
//                msg() << MSG::DEBUG << " Well done in L vertexing!!!" << endmsg;
//                //continue; // OBSOLETE, done inside vertexing
//            }
//            
//            // Bd+/- vertexing & mass cut
//            if(m_doLb_LambdaMuMuVertexFit && m_doVertexFit){
//                bool vertex = true;
//                if(timerSvc()) m_VtxFitTimer->resume();
//                TrigL2Vertex* laBVtx = NULL;
//                TrigVertex* mother_LbVtx=NULL;
//                laBVtx = new TrigL2Vertex();
//                StatusCode status;
//                status = m_vertexingTool->addTrack(mu1->IDTrack(),laBVtx,Trk::muon);   //
//                if(status.isFailure()){
//                    //delete laBVtx;
//                    if(timerSvc()) m_VtxFitTimer->pause();
//                    //continue;
//                    vertex = false;
//                }
//                if(vertex) status = m_vertexingTool->addTrack(mu2->IDTrack(),laBVtx,Trk::muon);   //
//                if(status.isFailure()){
//                    //delete laBVtx;
//                    if(timerSvc()) m_VtxFitTimer->pause();
//                    //continue;
//                    vertex = false;
//                }
//                
//                if(vertex) status = m_vertexingTool->addTrack(trk3,laBVtx,Trk::proton);
//                if(status.isFailure()){
//                    //delete laBVtx;
//                    if(timerSvc()) m_VtxFitTimer->pause();
//                    //continue;
//                    vertex = false;
//                }
//                if(vertex) status = m_vertexingTool->addTrack(trk4,laBVtx,Trk::pion);
//                if(status.isFailure()){
//                    //delete laBVtx;
//                    if(timerSvc()) m_VtxFitTimer->pause();
//                    //continue;
//                    vertex = false;
//                }
//                
//                ElementLink<TrigL2BphysContainer> trigPartLbEL(*m_trigBphysColl_lambda,LIndex);
//                TrigL2Bphys* trigPartBmumuLb = new TrigL2Bphys(0.0, 0.0, 0.0, TrigL2Bphys::LBLMUMU, xMuMuMass, trigPartLbEL);
//                
//                //vertex fit
//                if(vertex) status = m_L2vertFitter->fit(laBVtx);
//                if(status.isSuccess()) status = m_vertexingTool->calculateInvariantMass(laBVtx);
//                if(status.isFailure()){
//                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Lb vertex fit failed!" << endmsg;
//                    delete laBVtx;
//                    //continue; // for performance test not continue
//                    vertex = false;
//                }else{
//                    mother_LbVtx = m_vertexingTool->createTrigVertex(laBVtx);
//                    status = m_vertexingTool->createMotherParticle(laBVtx);
//                    if(status.isSuccess()){
//                        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Lb created, with pt: " << laBVtx->m_getMotherTrack()->pT() << endmsg;
//                    }
//                    if(mother_LbVtx == NULL){
//                        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  Vertex fit failed for particle candidate track " << trk3
//                            <<" and "<<trk4<< endmsg;
//                        //continue; // for performance test not continue
//                        vertex = false;
//                    }else{
//                        if(msgLvl() <= MSG::DEBUG) {
//                            msg() << MSG::DEBUG << " Lb vertex fit: x = " <<mother_LbVtx->x()
//                            <<", y = " <<mother_LbVtx->y()
//                            <<", z = " <<mother_LbVtx->z()
//                            <<", mass = " <<mother_LbVtx->mass()
//                            <<", chi2 = " <<mother_LbVtx->chi2()
//                            <<", ndof = " <<mother_LbVtx->ndof() /// (number degrees of freedom)
//                            <<", massVariance = " <<mother_LbVtx->massVariance()
//                            << endmsg;
//                        }
//                        if(mother_LbVtx->massVariance() != 0) {
//                            if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  Lb mass pull = " << (mother_LbVtx->mass()-LAMBDABMASS/mother_LbVtx->massVariance()) << endmsg;
//                        }else{
//                            if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " Lb mass variance is zero, can't calculate the pull" << endmsg;
//                        }
//                        /// add vertex results to L2Bphys
//                        trigPartBmumuLb->fitmass(mother_LbVtx->mass());
//                        trigPartBmumuLb->fitchi2(mother_LbVtx->chi2());
//                        trigPartBmumuLb->fitndof(mother_LbVtx->ndof());
//                        trigPartBmumuLb->fitx(mother_LbVtx->x());
//                        trigPartBmumuLb->fity(mother_LbVtx->y());
//                        trigPartBmumuLb->fitz(mother_LbVtx->z());
//                        
//                        if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " SigmaX =  SigmaY =  SigmaZ = "
//                            << sqrt(mother_LbVtx->cov()[0]) << " "
//                            << sqrt(mother_LbVtx->cov()[2]) << " "
//                            << sqrt(mother_LbVtx->cov()[5])
//                            << endmsg;
//                        
//                        // cut on chi2 and invariant mass of Bd after vertexing.
//                        if(laBVtx->chi2() < m_bSVtxChi2Cut && laBVtx->mass() > m_lowerLbVtxMassCut && laBVtx->mass() < m_upperLbVtxMassCut)
//                        {
//                            if(msgLvl() <= MSG::DEBUG)  {
//                                msg() << MSG::DEBUG << " Lb vertex fit success: x= "
//                                <<            laBVtx->m_getParametersVector()[0] //x
//                                << ", y= " << laBVtx->m_getParametersVector()[1] //y
//                                << ", z= " << laBVtx->m_getParametersVector()[2] //z
//                                << ", chi2= " << laBVtx->chi2()
//                                <<", ndof = " <<laBVtx->ndof()
//                                << endmsg;
//                            }
//                            
//                            // PERFECT -> Bphys particle was stored in collection
//                            m_VertexColl->push_back(mother_LbVtx);
//                            m_trigBphysColl_b->push_back(trigPartBmumuLb);
//                            if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Added Lb vertex to vertex collection" << endmsg;
//                            
//                            // monitoring of successfuly stored particles
//                            m_mon_LbMuMuL_n++;
//                            m_mon_Acceptance.push_back( ACCEPT_BphysCollParticle );
//                            m_mon_LbMuMuL_VtxMass_Lb.push_back((laBVtx->mass())*0.001);
//                            m_mon_LbMuMuL_Chi2toNDoF_Lb.push_back((laBVtx->chi2())/(laBVtx->ndof()));
//                            
//                            m_mon_LbMuMuL_VtxPtLb_Pr.push_back((trk3->param()->pT())*0.001);
//                            m_mon_LbMuMuL_VtxEtaLb_Pr.push_back(trk3->param()->eta());
//                            m_mon_LbMuMuL_VtxPhiLb_Pr.push_back(trk3->param()->phi0());
//                            m_mon_LbMuMuL_VtxPtLb_Pi.push_back((trk4->param()->pT())*0.001);
//                            m_mon_LbMuMuL_VtxEtaLb_Pi.push_back(trk4->param()->eta());
//                            m_mon_LbMuMuL_VtxPhiLb_Pi.push_back(trk4->param()->phi0());
//                            
//                            if(msgLvl() <= MSG::DEBUG) {
//                                msg() << MSG::DEBUG << " Finnished Bphys particle with fitmass" << trigPartBmumuLb->fitmass() << " mass " << trigPartBmumuLb->mass()
//                                << " phi, eta - more RoIs => 0" << trigPartBmumuLb->phi() << " " << trigPartBmumuLb->eta() << " vertex type " << trigPartBmumuLb->particleType()
//                                << endmsg;
//                            }
//                            
//                        }else{
//                            if(msgLvl() <= MSG::DEBUG) {
//                                msg() << MSG::DEBUG << " Lb rejected by mass or chi2 cuts after vertex fit!"
//                                << ", chi2= " << laBVtx->chi2()
//                                << endmsg;
//                                //continue; // for performance test not continue
//                                vertex = false;
//                            }
//                        }
//                    } // end of successful fit condition "mother"
//                    if(vertex) {
//                        int index = m_VertexColl->size()-1;
//                        ElementLink<TrigVertexCollection> LbVertexEL(*m_VertexColl,index);
//                    } else {
//                        delete mother_LbVtx;
//                    }
//                    delete laBVtx;
//                } // end of successful fit condition initial vertex
//                
//                
//            }   // Lb -> L Mu Mu vertexing
//        } // end of check over failed vertexing
//        
//        //////////////////////////////// vertexing off ///////////////////////////////////////////////
//        if(!m_doLb_LambdaMuMuVertexFit || !m_doVertexFit){
//            if ((xMuMuMass > m_lowerLb_LambdaMuMuMassCutVtxOff && xMuMuMass < m_upperLb_LambdaMuMuMassCutVtxOff)) {
//                
//                // Store links to the tracks forming bphys particle
//                ElementLinkVector<TrigInDetTrackCollection> trackVectorBmumuLb;
//                ElementLink<TrigInDetTrackCollection> track1ELLb = mu1->IDTrackLink();
//                ElementLink<TrigInDetTrackCollection> track2ELLb = mu2->IDTrackLink();
//                ElementLink<TrigInDetTrackCollection> track3ELLb(*trkCollection, iTrk3);
//                ElementLink<TrigInDetTrackCollection> track4ELLb(*trkCollection, iTrk4);
//                
//                if ( msgLvl() <= MSG::VERBOSE ){
//                    msg() << MSG::VERBOSE << "Just check track links... " << endmsg;
//                    msg() << MSG::VERBOSE << "Muon 1 pT " << (*track1ELLb)->param()->pT() << " eta: " << (*track1ELLb)->param()->eta() << " phi: " << (*track1ELLb)->param()->phi0() << endmsg;
//                    msg() << MSG::VERBOSE << "Muon 2 pT " << (*track2ELLb)->param()->pT() << " eta: " << (*track2ELLb)->param()->eta() << " phi: " << (*track2ELLb)->param()->phi0() << endmsg;
//                    msg() << MSG::VERBOSE << "Track 1 pT " << (*track3ELLb)->param()->pT() << " eta: " << (*track3ELLb)->param()->eta() << " phi: " << (*track3ELLb)->param()->phi0() << endmsg;
//                    msg() << MSG::VERBOSE << "Track 2 pT " << (*track4ELLb)->param()->pT() << " eta: " << (*track4ELLb)->param()->eta() << " phi: " << (*track4ELLb)->param()->phi0() << endmsg;
//                }
//                trackVectorBmumuLb.push_back(track1ELLb); // temp
//                trackVectorBmumuLb.push_back(track2ELLb); // temp
//                trackVectorBmumuLb.push_back(track3ELLb); // temp
//                trackVectorBmumuLb.push_back(track4ELLb); // temp
//                
//                
//                //         if(m_doLambda_ProtonPionVertexFit){
//                ElementLink<TrigL2BphysContainer> trigPartLbEL(*m_trigBphysColl_lambda,LIndex);
//                TrigL2Bphys* trigPartBmumuLb = new TrigL2Bphys(0.0, 0.0, 0.0, TrigL2Bphys::LBLMUMU, xMuMuMass, trigPartLbEL);
//                trigPartBmumuLb->addTrack(track1ELLb); 
//                trigPartBmumuLb->addTrack(track2ELLb); 
//                trigPartBmumuLb->addTrack(track3ELLb); 
//                trigPartBmumuLb->addTrack(track4ELLb); 
//                m_trigBphysColl_b->push_back(trigPartBmumuLb);
//                //         }else{
//                // //           m_trigBphysColl_lambda->push_back(trigPartBmumuL);
//                // //           int LIndex = m_trigBphysColl_lambda->size() - 1;
//                // //           ElementLink<TrigL2BphysContainer> trigPartLbEL(*m_trigBphysColl_lambda,LIndex);
//                //           
//                //           TrigL2Bphys* trigPartBmumuLb = new TrigL2Bphys(0., 0., 0., TrigL2Bphys::LBLMUMU, xMuMuMass/*, trigPartLbEL*/);
//                //           trigPartBmumuLb->addTrack(track1ELLb); 
//                //           trigPartBmumuLb->addTrack(track2ELLb); 
//                //           trigPartBmumuLb->addTrack(track3ELLb); 
//                //           trigPartBmumuLb->addTrack(track4ELLb); 
//                //           m_trigBphysColl_b->push_back(trigPartBmumuLb);
//                //         }
//                
//                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Lambda's Proton and Pion Candidates accepted with L mass: " << xMass << "MeV"  
//                    << " and with LambdaB mass : " << xMuMuMass << "MeV"  
//                    << " proton particle with pt: "<< trk3->param()->pT() 
//                    << " pion particle with pt: "<< trk4->param()->pT() 
//                    << endmsg;
//                if(msgLvl() <= MSG::DEBUG)
//                    msg() << MSG::DEBUG << " Added Lambda_b particle to Bphys particle collection   " 
//                    //<< ", eta=" trigPartBmumuKplus->eta()   
//                    //<< ", phi= " << trigPartBmumuKplus->phi()
//                    << ", mass= " << trigPartBmumuLb->mass() 
//                    << ", particle type= " << trigPartBmumuLb->particleType() 
//                    << endmsg;
//                m_mon_LbMuMuL_n++;
//                m_mon_LbMuMuL_InvMassLb_L.push_back((xMass*0.001));
//                m_mon_LbMuMuL_InvMass_Lb.push_back((xMuMuMass*0.001));
//                m_mon_LbMuMuL_PtL_Pr.push_back(trk3->param()->pT());
//                m_mon_LbMuMuL_EtaL_Pr.push_back(trk3->param()->eta());
//                m_mon_LbMuMuL_PhiL_Pr.push_back(trk3->param()->phi0());
//                m_mon_LbMuMuL_PtL_Pi.push_back(trk4->param()->pT());
//                m_mon_LbMuMuL_EtaL_Pi.push_back(trk4->param()->eta());
//                m_mon_LbMuMuL_PhiL_Pi.push_back(trk4->param()->phi0());
//                
//            }else{// end of tighter mass cuts
//                if(msgLvl() <= MSG::DEBUG) 
//                    msg() << MSG::DEBUG << " Lb rejected due to Lb mass cut (no vertexing) " << endmsg;
//            } 
//            
//        } // end of no vertexing part
//        
//        
//    } else { // end of if L and Lb successful mass cuts
//        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Lambda_b rejected due to Lambda or Lambda_b mass cut " << endmsg;
//    }
//    
//}
//
////OLD
///*----------------------------------------------------------------------------*/
//void TrigL2BMuMuXFex::checkBcMuMuDs(const CombinedMuonFeature* mu1, const CombinedMuonFeature* mu2, const TrigInDetTrack* trk3, const TrigInDetTrack* trk4, const TrigInDetTrack* trk5, const TrigInDetTrackCollection* trkCollection, int iTrk3, int iTrk4, double xPhiMass, int iTrk5) {
//    
//    double xMass = X3Mass(trk3, trk4, trk5);
//    double xMuMuMass = X3MuMuMass(mu1->IDTrack(), mu2->IDTrack(), trk3, trk4, trk5);
//    
//    if ((xMass > m_lowerDs_MassCut && xMass < m_upperDs_MassCut) && (xMuMuMass > m_lowerBc_DsMuMuMassCut && xMuMuMass < m_upperBc_DsMuMuMassCut)) {
//        
//        // Now make L2Bphys objects - initial addon
//        //     TrigL2Bphys* trigPartBmumuPhiDs = new TrigL2Bphys(0., 0., 0., TrigL2Bphys::PHIKK, xPhiMass);
//        TrigL2Bphys* trigPartBmumuDs    = new TrigL2Bphys(0., 0., 0., TrigL2Bphys::DSPHIPI, xMass);
//        if(msgLvl() <= MSG::DEBUG) {
//            //       msg() << MSG::DEBUG << " Create PhiDs Bphys particle with roIId - more RoIs => " << trigPartBmumuPhiDs->roiId() << " mass " << xPhiMass
//            //             << " phi, eta - more RoIs => " << trigPartBmumuPhiDs->phi() << " " << trigPartBmumuPhiDs->eta() << " vertex type " << trigPartBmumuPhiDs->particleType()
//            //             << endmsg;
//            msg() << MSG::DEBUG << " Create Ds Bphys particle with roIId - more RoIs => " << trigPartBmumuDs->roiId() << " mass " << xMass
//            << " phi, eta - more RoIs => " << trigPartBmumuDs->phi() << " " << trigPartBmumuDs->eta() << " vertex type " << trigPartBmumuDs->particleType()
//            << endmsg;
//        }
//        
//        // Store links to the tracks forming bphys particle
//        ElementLinkVector<TrigInDetTrackCollection> trackVectorBmumuDs;
//        ElementLink<TrigInDetTrackCollection> track3EL(*trkCollection, iTrk3); //kaon
//        ElementLink<TrigInDetTrackCollection> track4EL(*trkCollection, iTrk4); //kaon
//        ElementLink<TrigInDetTrackCollection> track5EL(*trkCollection, iTrk5); //pion
//        
//        if ( msgLvl() <= MSG::VERBOSE ){
//            msg() << MSG::VERBOSE << "Just check track links... " << endmsg;
//            msg() << MSG::VERBOSE << "Track 1 pT " << (*track3EL)->param()->pT() << " eta: " << (*track3EL)->param()->eta() << " phi: " << (*track3EL)->param()->phi0() << endmsg;
//            msg() << MSG::VERBOSE << "Track 2 pT " << (*track4EL)->param()->pT() << " eta: " << (*track4EL)->param()->eta() << " phi: " << (*track4EL)->param()->phi0() << endmsg;
//            msg() << MSG::VERBOSE << "Track 3 pT " << (*track5EL)->param()->pT() << " eta: " << (*track5EL)->param()->eta() << " phi: " << (*track5EL)->param()->phi0() << endmsg;
//        }
//        trackVectorBmumuDs.push_back(track3EL); // temp
//        trackVectorBmumuDs.push_back(track4EL); // temp
//        trackVectorBmumuDs.push_back(track5EL); // temp
//        
//        trigPartBmumuDs->addTrack(track3EL);
//        trigPartBmumuDs->addTrack(track4EL);
//        trigPartBmumuDs->addTrack(track5EL);
//        // end of initial addon
//        
//        if(trk3->param()->pT() > 0) {
//            //       if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Phi's Pion and Kaon Candidates accepted with K* mass: " << xMass << "MeV"
//            //                                            << " and with Bd mass : " << xMuMuMass << "MeV"
//            //                                          << " positive charged particle with pt: "<< trk3->param()->pT()
//            //                                          << " negative charged particle with pt: "<< trk4->param()->pT()
//            //                                          << endmsg;
//            m_mon_BcMuMuDs_InvMassBc_Phi.push_back((xPhiMass*0.001));
//            m_mon_BcMuMuDs_InvMassBc_Ds.push_back((xMass*0.001));
//            m_mon_BcMuMuDs_InvMass_Bc.push_back((xMuMuMass*0.001));
//            m_mon_BcMuMuDs_PtPhi_Kp.push_back(trk3->param()->pT());
//            m_mon_BcMuMuDs_EtaPhi_Kp.push_back(trk3->param()->eta());
//            m_mon_BcMuMuDs_PhiPhi_Kp.push_back(trk3->param()->phi0());
//            m_mon_BcMuMuDs_PtPhi_Km.push_back(trk4->param()->pT());
//            m_mon_BcMuMuDs_EtaPhi_Km.push_back(trk4->param()->eta());
//            m_mon_BcMuMuDs_PhiPhi_Km.push_back(trk4->param()->phi0());
//            m_mon_BcMuMuDs_PtDs_pi.push_back(trk5->param()->pT());
//            m_mon_BcMuMuDs_EtaDs_pi.push_back(trk5->param()->eta());
//            m_mon_BcMuMuDs_PhiDs_pi.push_back(trk5->param()->phi0());
//        }else{
//            //       if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Phi's Pion and Kaon Candidates accepted with K* mass: " << xMass << "MeV"
//            //                                            << " and with Bd mass : " << xMuMuMass << "MeV"
//            //                                            << " positive charged particle with pt: "<< trk4->param()->pT()
//            //                                            << " negative charged particle with pt: "<< trk3->param()->pT()
//            //                                              <<endmsg;
//            m_mon_BcMuMuDs_InvMassBc_Phi.push_back((xPhiMass*0.001));
//            m_mon_BcMuMuDs_InvMassBc_Ds.push_back((xMass*0.001));
//            m_mon_BcMuMuDs_InvMass_Bc.push_back((xMuMuMass*0.001));
//            m_mon_BcMuMuDs_PtPhi_Kp.push_back(trk4->param()->pT());
//            m_mon_BcMuMuDs_EtaPhi_Kp.push_back(trk4->param()->eta());
//            m_mon_BcMuMuDs_PhiPhi_Kp.push_back(trk4->param()->phi0());
//            m_mon_BcMuMuDs_PtPhi_Km.push_back(trk3->param()->pT());
//            m_mon_BcMuMuDs_EtaPhi_Km.push_back(trk3->param()->eta());
//            m_mon_BcMuMuDs_PhiPhi_Km.push_back(trk3->param()->phi0());
//            m_mon_BcMuMuDs_PtDs_pi.push_back(trk5->param()->pT());
//            m_mon_BcMuMuDs_EtaDs_pi.push_back(trk5->param()->eta());
//            m_mon_BcMuMuDs_PhiDs_pi.push_back(trk5->param()->phi0());
//        }
//        bool goodDs = false;
//        //Ds vertexing & mass cut
//        if(m_doDs_PhiPionVertexFit && m_doVertexFit) {
//            bool vertex = true;
//            TrigL2Vertex* DsVtx = NULL;
//            TrigVertex*   mother_DsVtx = NULL;
//            // timer
//            if(timerSvc()) m_VtxFitTimer->resume();
//            DsVtx = new TrigL2Vertex();
//            StatusCode status;
//            status = m_vertexingTool->addTrack(trk3,DsVtx,Trk::kaon);
//            if( status.isFailure() ) {
//                //delete phia1020Vtx;
//                //continue;
//                vertex = false;
//            }
//            if(vertex) status = m_vertexingTool->addTrack(trk4,DsVtx,Trk::kaon);
//            if( status.isFailure() ) {
//                //delete phia1020Vtx;
//                //continue;
//                vertex = false;
//            }
//            if(vertex) status = m_vertexingTool->addTrack(trk5,DsVtx,Trk::pion);
//            if( status.isFailure() ) {
//                //delete phia1020Vtx;
//                //continue;
//                vertex = false;
//            }
//            //////////////
//            //vertex fit
//            if(vertex) status = m_L2vertFitter->fit(DsVtx);
//            if(status.isSuccess()) status = m_vertexingTool->calculateInvariantMass(DsVtx);
//            if(status.isFailure()){
//                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Ds vertex fit failed!" << endmsg;
//                delete DsVtx;
//                if(timerSvc()) m_VtxFitTimer->pause();
//                //continue; // for performance test not continue?
//                vertex = false;
//            }else{
//                mother_DsVtx = m_vertexingTool->createTrigVertex(DsVtx);
//                status = m_vertexingTool->createMotherParticle(DsVtx);
//                if(status.isSuccess()){
//                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Ds created, with pt: " << DsVtx->m_getMotherTrack()->pT() << endmsg;
//                }
//                if(mother_DsVtx == NULL){
//                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  Vertex fit failed for particle candidate tracks " << trk3
//                        <<" and "<<trk4<<" and "<<trk5 << endmsg;
//                    if(timerSvc()) m_VtxFitTimer->pause();
//                    //continue; // for performance test not continue?
//                    vertex = false;
//                }else{
//                    if(msgLvl() <= MSG::DEBUG) {
//                        msg() << MSG::DEBUG << " Ds vertex fit: x = " <<mother_DsVtx->x()
//                        <<", y = " <<mother_DsVtx->y()
//                        <<", z = " <<mother_DsVtx->z()
//                        <<", mass = " <<mother_DsVtx->mass()
//                        <<", chi2 = " <<mother_DsVtx->chi2()
//                        <<", ndof = " <<mother_DsVtx->ndof() /// (number degrees of freedom)
//                        <<", massVariance = " <<mother_DsVtx->massVariance()
//                        << endmsg;
//                    }
//                    if(mother_DsVtx->massVariance() != 0) {
//                        if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  Ds mass pull = " << ((mother_DsVtx->mass()-DSMASS)/mother_DsVtx->massVariance()) << endmsg;
//                    }else{
//                        if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " Ds mass variance is zero, can't calculate the pull" << endmsg;
//                    }
//                    
//                    /// add vertex results to L2Bphys
//                    trigPartBmumuDs->fitmass(mother_DsVtx->mass());
//                    trigPartBmumuDs->fitchi2(mother_DsVtx->chi2());
//                    trigPartBmumuDs->fitndof(mother_DsVtx->ndof());
//                    trigPartBmumuDs->fitx(mother_DsVtx->x());
//                    trigPartBmumuDs->fity(mother_DsVtx->y());
//                    trigPartBmumuDs->fitz(mother_DsVtx->z());
//                    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " SigmaX =  SigmaY =  SigmaZ = "
//                        << sqrt(mother_DsVtx->cov()[0]) << " "
//                        << sqrt(mother_DsVtx->cov()[2]) << " "
//                        << sqrt(mother_DsVtx->cov()[5])
//                        << endmsg;
//                    
//                    // cut on chi2 and invariant mass of bplus after vertexing.
//                    if(DsVtx->chi2() < m_DsVtxChi2Cut &&
//                       DsVtx->mass() > m_lowerDsVtxMassCut && DsVtx->mass() < m_upperDsVtxMassCut)
//                    {
//                        if(msgLvl() <= MSG::DEBUG)  {
//                            msg() << MSG::DEBUG << " Ds vertex fit success: x= "
//                            <<            DsVtx->m_getParametersVector()[0] //x
//                            << ", y= " << DsVtx->m_getParametersVector()[1] //y
//                            << ", z= " << DsVtx->m_getParametersVector()[2] //z
//                            << ", chi2= " << DsVtx->chi2()
//                            <<", ndof = " <<DsVtx->ndof()
//                            << endmsg;
//                        }
//                        goodDs = true;
//                        // GREAT -> store daughter particle info
//                        m_VertexColl->push_back(mother_DsVtx);
//                        if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Added Ds vertex to vertex collection" << endmsg;
//                        int index = m_VertexColl->size()-1;
//                        ElementLink<TrigVertexCollection> DsVertexEL(*m_VertexColl,index);
//                        
//                        // monitoring
//                        m_mon_BcMuMuDs_VtxMassBc_Ds.push_back((DsVtx->mass())*0.001);
//                        m_mon_BcMuMuDs_Chi2toNDoFBc_Ds.push_back((DsVtx->chi2())/(DsVtx->ndof()));
//                        if(trk3->param()->pT() > 0) {
//                            m_mon_BcMuMuDs_VtxPtDs_Kp.push_back((trk3->param()->pT())*0.001);
//                            m_mon_BcMuMuDs_VtxEtaDs_Kp.push_back(trk3->param()->eta());
//                            m_mon_BcMuMuDs_VtxPhiDs_Kp.push_back(trk3->param()->phi0());
//                            m_mon_BcMuMuDs_VtxPtDs_Km.push_back((trk4->param()->pT())*0.001);
//                            m_mon_BcMuMuDs_VtxEtaDs_Km.push_back(trk4->param()->eta());
//                            m_mon_BcMuMuDs_VtxPhiDs_Km.push_back(trk4->param()->phi0());
//                            m_mon_BcMuMuDs_VtxPtDs_pi.push_back((trk5->param()->pT())*0.001);
//                            m_mon_BcMuMuDs_VtxEtaDs_pi.push_back(trk5->param()->eta());
//                            m_mon_BcMuMuDs_VtxPhiDs_pi.push_back(trk5->param()->phi0());
//                        }else{
//                            m_mon_BcMuMuDs_VtxPtDs_Kp.push_back((trk4->param()->pT())*0.001);
//                            m_mon_BcMuMuDs_VtxEtaDs_Kp.push_back(trk4->param()->eta());
//                            m_mon_BcMuMuDs_VtxPhiDs_Kp.push_back(trk4->param()->phi0());
//                            m_mon_BcMuMuDs_VtxPtDs_Km.push_back((trk3->param()->pT())*0.001);
//                            m_mon_BcMuMuDs_VtxEtaDs_Km.push_back(trk3->param()->eta());
//                            m_mon_BcMuMuDs_VtxPhiDs_Km.push_back(trk3->param()->phi0());
//                            m_mon_BcMuMuDs_VtxPtDs_pi.push_back((trk5->param()->pT())*0.001);
//                            m_mon_BcMuMuDs_VtxEtaDs_pi.push_back(trk5->param()->eta());
//                            m_mon_BcMuMuDs_VtxPhiDs_pi.push_back(trk5->param()->phi0());
//                        }
//                        if(msgLvl() <= MSG::DEBUG) {
//                            msg() << MSG::DEBUG << " Finnished Bphys daughter particle with fitmass" << trigPartBmumuDs->fitmass() << " mass " << trigPartBmumuDs->mass()
//                            << " phi, eta - more RoIs => 0" << trigPartBmumuDs->phi() << " " << trigPartBmumuDs->eta() << " vertex type " << trigPartBmumuDs->particleType()
//                            << endmsg;
//                        }
//                        
//                    }else{
//                        if(msgLvl() <= MSG::DEBUG) {
//                            msg() << MSG::DEBUG << " Ds rejected by mass or chi2 cuts after vertex fit!"
//                            << ", chi2= " << DsVtx->chi2()
//                            << endmsg;
//                        }
//                        if(timerSvc()) m_VtxFitTimer->pause();
//                        //continue; // for performance test not continue
//                        vertex = false;
//                        delete mother_DsVtx;
//                    }
//                } // end of successful fit condition "mother"
//                //int index = m_VertexColl->size()-1;
//                //ElementLink<TrigVertexCollection> BplusVertexEL(*m_VertexColl,index);
//                delete DsVtx;
//            } // end of successful fit condition initial vertex
//            if(timerSvc()) m_VtxFitTimer->pause();
//        } // end DS_KaonKaonPion vertex fit
//        
//        m_trigBphysColl_ds->push_back(trigPartBmumuDs);
//        int DsIndex = m_trigBphysColl_ds->size() - 1;
//        
//        if(m_doDs_PhiPionVertexFit && goodDs) {
//            if(msgLvl() <= MSG::DEBUG) {
//                msg() << MSG::DEBUG << " Well done in Ds vertexing!!!" << endmsg;
//                //    continue; // OBSOLETE, done inside vertexing
//            }
//            
//            // Bc+/- vertexing & mass cut
//            if(m_doBc_DsMuMuVertexFit && m_doVertexFit){
//                if(timerSvc()) m_VtxFitTimer->resume();
//                bool vertex = true;
//                TrigL2Vertex* BcVtx = NULL;
//                TrigVertex* mother_BcVtx=NULL;
//                BcVtx = new TrigL2Vertex();
//                StatusCode status;
//                status = m_vertexingTool->addTrack(mu1->IDTrack(),BcVtx,Trk::muon);   //
//                if(status.isFailure()){
//                    //delete BcVtx;
//                    if(timerSvc()) m_VtxFitTimer->pause();
//                    //continue;
//                    vertex = false;
//                }
//                if (vertex) status = m_vertexingTool->addTrack(mu2->IDTrack(),BcVtx,Trk::muon);   //
//                if(status.isFailure()){
//                    //delete BcVtx;
//                    if(timerSvc()) m_VtxFitTimer->pause();
//                    //continue;
//                    vertex = false;
//                }
//                if (vertex) status = m_vertexingTool->addTrack(trk3,BcVtx,Trk::kaon);
//                if(status.isFailure()){
//                    //delete BcVtx;
//                    if(timerSvc()) m_VtxFitTimer->pause();
//                    //continue;
//                    vertex = false;
//                }
//                if (vertex) status = m_vertexingTool->addTrack(trk4,BcVtx,Trk::kaon);
//                if(status.isFailure()){
//                    //delete BcVtx;
//                    if(timerSvc()) m_VtxFitTimer->pause();
//                    //continue;
//                    vertex = false;
//                }
//                if (vertex) status = m_vertexingTool->addTrack(trk5,BcVtx,Trk::pion);
//                if(status.isFailure()){
//                    //delete BcVtx;
//                    if(timerSvc()) m_VtxFitTimer->pause();
//                    //continue;
//                    vertex = false;
//                }
//                ElementLink<TrigL2BphysContainer> trigPartBcEL(*m_trigBphysColl_ds,DsIndex);
//                TrigL2Bphys* trigPartBmumuBc = new TrigL2Bphys(0.0, 0.0, 0.0, TrigL2Bphys::BCDSMUMU, xMuMuMass, trigPartBcEL);
//                
//                //vertex fit
//                if (vertex) status = m_L2vertFitter->fit(BcVtx);
//                if(status.isSuccess()) status = m_vertexingTool->calculateInvariantMass(BcVtx);
//                if(status.isFailure()){
//                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Bc vertex fit failed!" << endmsg;
//                    delete BcVtx;
//                    //continue; // for performance test not continue
//                    vertex = false;
//                }else{
//                    mother_BcVtx = m_vertexingTool->createTrigVertex(BcVtx);
//                    status = m_vertexingTool->createMotherParticle(BcVtx);
//                    if(status.isSuccess()){
//                        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Bc created, with pt: " << BcVtx->m_getMotherTrack()->pT() << endmsg;
//                    }
//                    if(mother_BcVtx == NULL){
//                        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  Vertex fit failed for particle candidate tracks " << trk3
//                            <<" and "<<trk4<<" and "<<trk5<< endmsg;
//                        vertex = false;
//                        //continue; // for performance test not continue
//                    }else{
//                        if(msgLvl() <= MSG::DEBUG) {
//                            msg() << MSG::DEBUG << " Bc vertex fit: x = " <<mother_BcVtx->x()
//                            <<", y = " <<mother_BcVtx->y()
//                            <<", z = " <<mother_BcVtx->z()
//                            <<", mass = " <<mother_BcVtx->mass()
//                            <<", chi2 = " <<mother_BcVtx->chi2()
//                            <<", ndof = " <<mother_BcVtx->ndof() /// (number degrees of freedom)
//                            <<", massVariance = " <<mother_BcVtx->massVariance()
//                            << endmsg;
//                        }
//                        if(mother_BcVtx->massVariance() != 0) {
//                            if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  Bc mass pull = " << ((mother_BcVtx->mass()-BCMASS)/mother_BcVtx->massVariance()) << endmsg;
//                        }else{
//                            if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " Bc mass variance is zero, can't calculate the pull" << endmsg;
//                        }
//                        
//                        /// add vertex results to L2Bphys
//                        trigPartBmumuBc->fitmass(mother_BcVtx->mass());
//                        trigPartBmumuBc->fitchi2(mother_BcVtx->chi2());
//                        trigPartBmumuBc->fitndof(mother_BcVtx->ndof());
//                        trigPartBmumuBc->fitx(mother_BcVtx->x());
//                        trigPartBmumuBc->fity(mother_BcVtx->y());
//                        trigPartBmumuBc->fitz(mother_BcVtx->z());
//                        
//                        if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " SigmaX =  SigmaY =  SigmaZ = "
//                            << sqrt(mother_BcVtx->cov()[0]) << " "
//                            << sqrt(mother_BcVtx->cov()[2]) << " "
//                            << sqrt(mother_BcVtx->cov()[5])
//                            << endmsg;
//                        
//                        // cut on chi2 and invariant mass of Bc after vertexing.
//                        if(BcVtx->chi2() < m_bCVtxChi2Cut && BcVtx->mass() > m_lowerBcVtxMassCut && BcVtx->mass() < m_upperBcVtxMassCut)
//                        {
//                            if(msgLvl() <= MSG::DEBUG)  {
//                                msg() << MSG::DEBUG << " Bc vertex fit success: x= "
//                                <<            BcVtx->m_getParametersVector()[0] //x
//                                << ", y= " << BcVtx->m_getParametersVector()[1] //y
//                                << ", z= " << BcVtx->m_getParametersVector()[2] //z
//                                << ", chi2= " << BcVtx->chi2()
//                                <<", ndof = " <<BcVtx->ndof()
//                                << endmsg;
//                            }
//                            
//                            // PERFECT -> Bphys particle was stored in collection
//                            m_VertexColl->push_back(mother_BcVtx);
//                            m_trigBphysColl_b->push_back(trigPartBmumuBc);
//                            if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Added Bc vertex to vertex collection" << endmsg;
//                            // monitoring of successfuly stored particles
//                            m_mon_BcMuMuDs_n++;
//                            m_mon_Acceptance.push_back( ACCEPT_BphysCollParticle );
//                            m_mon_BcMuMuDs_VtxMass_Bc.push_back((BcVtx->mass())*0.001);
//                            m_mon_BcMuMuDs_Chi2toNDoF_Bc.push_back((BcVtx->chi2())/(BcVtx->ndof()));
//                            if(trk3->param()->pT() > 0) {
//                                m_mon_BcMuMuDs_VtxPtBc_Kp.push_back((trk3->param()->pT())*0.001);
//                                m_mon_BcMuMuDs_VtxEtaBc_Kp.push_back(trk3->param()->eta());
//                                m_mon_BcMuMuDs_VtxPhiBc_Kp.push_back(trk3->param()->phi0());
//                                m_mon_BcMuMuDs_VtxPtBc_Km.push_back((trk4->param()->pT())*0.001);
//                                m_mon_BcMuMuDs_VtxEtaBc_Km.push_back(trk4->param()->eta());
//                                m_mon_BcMuMuDs_VtxPhiBc_Km.push_back(trk4->param()->phi0());
//                                m_mon_BcMuMuDs_VtxPtBc_pi.push_back((trk5->param()->pT())*0.001);
//                                m_mon_BcMuMuDs_VtxEtaBc_pi.push_back(trk5->param()->eta());
//                                m_mon_BcMuMuDs_VtxPhiBc_pi.push_back(trk5->param()->phi0());
//                            }else{
//                                m_mon_BcMuMuDs_VtxPtBc_Kp.push_back((trk4->param()->pT())*0.001);
//                                m_mon_BcMuMuDs_VtxEtaBc_Kp.push_back(trk4->param()->eta());
//                                m_mon_BcMuMuDs_VtxPhiBc_Kp.push_back(trk4->param()->phi0());
//                                m_mon_BcMuMuDs_VtxPtBc_Km.push_back((trk3->param()->pT())*0.001);
//                                m_mon_BcMuMuDs_VtxEtaBc_Km.push_back(trk3->param()->eta());
//                                m_mon_BcMuMuDs_VtxPhiBc_Km.push_back(trk3->param()->phi0());
//                                m_mon_BcMuMuDs_VtxPtBc_pi.push_back((trk5->param()->pT())*0.001);
//                                m_mon_BcMuMuDs_VtxEtaBc_pi.push_back(trk5->param()->eta());
//                                m_mon_BcMuMuDs_VtxPhiBc_pi.push_back(trk5->param()->phi0());
//                            }
//                            if(msgLvl() <= MSG::DEBUG) {
//                                msg() << MSG::DEBUG << " Finnished Bphys particle with fitmass" << trigPartBmumuBc->fitmass() << " mass " << trigPartBmumuBc->mass()
//                                << " phi, eta - more RoIs => 0" << trigPartBmumuBc->phi() << " " << trigPartBmumuBc->eta() << " vertex type " << trigPartBmumuBc->particleType()
//                                << endmsg;
//                            }
//                            
//                        }else{
//                            if(msgLvl() <= MSG::DEBUG) {
//                                msg() << MSG::DEBUG << " Bc rejected by mass or chi2 cuts after vertex fit!"
//                                << ", chi2= " << BcVtx->chi2()
//                                << endmsg;
//                                vertex = false;
//                                //continue; // for performance test not continue
//                            }
//                        }
//                    } // end of successful fit condition "mother"
//                    if(vertex) {
//                        int index = m_VertexColl->size()-1;
//                        ElementLink<TrigVertexCollection> BsVertexEL(*m_VertexColl,index);
//                        delete BcVtx;
//                    } else {
//                        delete mother_BcVtx;
//                    }
//                } // end of successful fit condition initial vertex
//                
//                
//            }   // Bc -> Ds Mu Mu vertexing
//        } // end of test over failed
//        //////////////////////////////// vertexing off ///////////////////////////////////////////////
//        if(!m_doBc_DsMuMuVertexFit || !m_doVertexFit){
//            if ((xMuMuMass > m_lowerBc_DsMuMuMassCutVtxOff && xMuMuMass < m_upperBc_DsMuMuMassCutVtxOff)) {
//                
//                // Store links to the tracks forming bphys particle
//                ElementLinkVector<TrigInDetTrackCollection> trackVectorBmumuBc;
//                ElementLink<TrigInDetTrackCollection> track1ELBc = mu1->IDTrackLink();
//                ElementLink<TrigInDetTrackCollection> track2ELBc = mu2->IDTrackLink();
//                ElementLink<TrigInDetTrackCollection> track3ELBc(*trkCollection, iTrk3);
//                ElementLink<TrigInDetTrackCollection> track4ELBc(*trkCollection, iTrk4);
//                ElementLink<TrigInDetTrackCollection> track5ELBc(*trkCollection, iTrk5);
//                
//                if ( msgLvl() <= MSG::VERBOSE ){
//                    msg() << MSG::VERBOSE << "Just check track links... " << endmsg;
//                    msg() << MSG::VERBOSE << "Muon 1 pT " << (*track1ELBc)->param()->pT() << " eta: " << (*track1ELBc)->param()->eta() << " phi: " << (*track1ELBc)->param()->phi0() << endmsg;
//                    msg() << MSG::VERBOSE << "Muon 2 pT " << (*track2ELBc)->param()->pT() << " eta: " << (*track2ELBc)->param()->eta() << " phi: " << (*track2ELBc)->param()->phi0() << endmsg;
//                    msg() << MSG::VERBOSE << "Track 1 pT " << (*track3ELBc)->param()->pT() << " eta: " << (*track3ELBc)->param()->eta() << " phi: " << (*track3ELBc)->param()->phi0() << endmsg;
//                    msg() << MSG::VERBOSE << "Track 2 pT " << (*track4ELBc)->param()->pT() << " eta: " << (*track4ELBc)->param()->eta() << " phi: " << (*track4ELBc)->param()->phi0() << endmsg;
//                    msg() << MSG::VERBOSE << "Track 3 pT " << (*track5ELBc)->param()->pT() << " eta: " << (*track5ELBc)->param()->eta() << " phi: " << (*track5ELBc)->param()->phi0() << endmsg;
//                }
//                trackVectorBmumuBc.push_back(track1ELBc); // temp
//                trackVectorBmumuBc.push_back(track2ELBc); // temp
//                trackVectorBmumuBc.push_back(track3ELBc); // temp
//                trackVectorBmumuBc.push_back(track4ELBc); // temp
//                trackVectorBmumuBc.push_back(track5ELBc); // temp
//                
//                
//                //         if(m_doPhi1020_KaonKaonVertexFit){
//                ElementLink<TrigL2BphysContainer> trigPartBcEL(*m_trigBphysColl_ds,DsIndex);
//                TrigL2Bphys* trigPartBmumuBc = new TrigL2Bphys(0.0, 0.0, 0.0, TrigL2Bphys::BCDSMUMU, xMuMuMass, trigPartBcEL);
//                trigPartBmumuBc->addTrack(track1ELBc);
//                trigPartBmumuBc->addTrack(track2ELBc);
//                trigPartBmumuBc->addTrack(track3ELBc);
//                trigPartBmumuBc->addTrack(track4ELBc);
//                trigPartBmumuBc->addTrack(track5ELBc);
//                m_trigBphysColl_b->push_back(trigPartBmumuBc);
//                //         }else{
//                // //           m_trigBphysColl_phi->push_back(trigPartBmumuPhi);
//                // //           int PhiIndex = m_trigBphysColl_phi->size() - 1;
//                // //           ElementLink<TrigL2BphysContainer> trigPartBsEL(*m_trigBphysColl_phi,PhiIndex);
//                //           
//                //           TrigL2Bphys* trigPartBmumuBs = new TrigL2Bphys(0., 0., 0., TrigL2Bphys::BSPHIMUMU, xMuMuMass/*, trigPartBsEL*/);
//                //           trigPartBmumuBs->addTrack(track1ELBs); 
//                //             trigPartBmumuBs->addTrack(track2ELBs); 
//                //             trigPartBmumuBs->addTrack(track3ELBs); 
//                //             trigPartBmumuBs->addTrack(track4ELBs); 
//                //           m_trigBphysColl_b->push_back(trigPartBmumuBs);
//                //         }
//                m_mon_BcMuMuDs_n++;
//                if(trk3->param()->pT() > 0) {
//                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Ds's Kplus, Kminus and pion Candidates accepted with Ds mass: " << xMass << "MeV"  
//                        << " and with B_c mass : " << xMuMuMass << "MeV"  
//                        << " positive charged particle with pt: "<< trk3->param()->pT() 
//                        << " negative charged particle with pt: "<< trk4->param()->pT() 
//                        << " pion charged particle with pt: "<< trk5->param()->pT() 
//                        << endmsg;
//                    m_mon_BcMuMuDs_InvMassBc_Phi.push_back((xPhiMass*0.001));
//                    m_mon_BcMuMuDs_InvMassBc_Ds.push_back((xMass*0.001));
//                    m_mon_BcMuMuDs_InvMass_Bc.push_back((xMuMuMass*0.001));
//                    m_mon_BcMuMuDs_PtPhi_Kp.push_back(trk3->param()->pT());
//                    m_mon_BcMuMuDs_EtaPhi_Kp.push_back(trk3->param()->eta());
//                    m_mon_BcMuMuDs_PhiPhi_Kp.push_back(trk3->param()->phi0());
//                    m_mon_BcMuMuDs_PtPhi_Km.push_back(trk4->param()->pT());
//                    m_mon_BcMuMuDs_EtaPhi_Km.push_back(trk4->param()->eta());
//                    m_mon_BcMuMuDs_PhiPhi_Km.push_back(trk4->param()->phi0());
//                    m_mon_BcMuMuDs_PtDs_pi.push_back(trk5->param()->pT());
//                    m_mon_BcMuMuDs_EtaDs_pi.push_back(trk5->param()->eta());
//                    m_mon_BcMuMuDs_PhiDs_pi.push_back(trk5->param()->phi0());
//                }else{
//                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Ds's Kplus, Kminus and pion Candidates accepted with Ds mass: " << xMass << "MeV"  
//                        << " and with B_c mass : " << xMuMuMass << "MeV"  
//                        << " positive charged particle with pt: "<< trk4->param()->pT() 
//                        << " negative charged particle with pt: "<< trk3->param()->pT() 
//                        << " pion charged particle with pt: "<< trk5->param()->pT() 
//                        <<endmsg;
//                    m_mon_BcMuMuDs_InvMassBc_Phi.push_back((xPhiMass*0.001));
//                    m_mon_BcMuMuDs_InvMassBc_Ds.push_back((xMass*0.001));
//                    m_mon_BcMuMuDs_InvMass_Bc.push_back((xMuMuMass*0.001));
//                    m_mon_BcMuMuDs_PtPhi_Kp.push_back(trk4->param()->pT());
//                    m_mon_BcMuMuDs_EtaPhi_Kp.push_back(trk4->param()->eta());
//                    m_mon_BcMuMuDs_PhiPhi_Kp.push_back(trk4->param()->phi0());
//                    m_mon_BcMuMuDs_PtPhi_Km.push_back(trk3->param()->pT());
//                    m_mon_BcMuMuDs_EtaPhi_Km.push_back(trk3->param()->eta());
//                    m_mon_BcMuMuDs_PhiPhi_Km.push_back(trk3->param()->phi0());
//                    m_mon_BcMuMuDs_PtDs_pi.push_back(trk5->param()->pT());
//                    m_mon_BcMuMuDs_EtaDs_pi.push_back(trk5->param()->eta());
//                    m_mon_BcMuMuDs_PhiDs_pi.push_back(trk5->param()->phi0());
//                }
//                if(msgLvl() <= MSG::DEBUG)
//                    msg() << MSG::DEBUG << " Added Bc particle to Bphys particle collection   " 
//                    //<< ", eta=" trigPartBmumuKplus->eta()   
//                    //<< ", phi= " << trigPartBmumuKplus->phi()
//                    << ", mass= " << trigPartBmumuBc->mass() 
//                    << ", particle type= " << trigPartBmumuBc->particleType() 
//                    << endmsg;
//            } else {// end of tighter mass cuts
//                if(msgLvl() <= MSG::DEBUG) 
//                    msg() << MSG::DEBUG << " Bc rejected due to Bc mass cut (no vertexing) " << endmsg;
//            } 
//            
//        } // end of no vertexing part
//        
//        
//    } else { // end of if Ds and Bc successful mass cuts
//        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Bc rejected due to Ds or Bc mass cut " << endmsg;
//    }
//} // end of checkBcMuMuDs
//
//
//// OLD
///*----------------------------------------------------------------------------*/
//double TrigL2BMuMuXFex::XMass(const TrigInDetTrack* particle1, const TrigInDetTrack* particle2,int decay)                
//{
//  std::vector<double> massHypo;
//  massHypo.clear();
//  if(decay == di_to_muons){
//    massHypo.push_back(MUMASS);
//    massHypo.push_back(MUMASS);
//  }
//  if(decay == bD_to_Kstar){  
//    massHypo.push_back(KPLUSMASS);
//    massHypo.push_back(PIMASS);
//  }
//  if(decay == bS_to_Phi){  
//    massHypo.push_back(KPLUSMASS);
//    massHypo.push_back(KPLUSMASS);    
//  }
//  if(decay == lB_to_L){  
//    massHypo.push_back(PROTONMASS);
//    massHypo.push_back(PIMASS);    
//  }
//  std::vector<const TrigInDetTrack*> bTracks;
//  bTracks.clear();
//  bTracks.push_back(particle1);
//  bTracks.push_back(particle2);
//  return InvMass(bTracks, massHypo);                                            
//}
//                                                                             
//double TrigL2BMuMuXFex::KMuMuMass(const TrigInDetTrack* mu1, const TrigInDetTrack* mu2,
//                                     const TrigInDetTrack* kaon)                
//{
//  std::vector<double> massHypo;
//  massHypo.clear();
//  massHypo.push_back(MUMASS);
//  massHypo.push_back(MUMASS);
//  massHypo.push_back(KPLUSMASS);  //K
//  std::vector<const TrigInDetTrack*> bTracks;
//  bTracks.clear();
//  bTracks.push_back(mu1);
//  bTracks.push_back(mu2);
//  bTracks.push_back(kaon);
//  return InvMass(bTracks, massHypo);                                            
//}        
//double TrigL2BMuMuXFex::XMuMuMass(const TrigInDetTrack* mu1, const TrigInDetTrack* mu2,
//                                     const TrigInDetTrack* particle1, const TrigInDetTrack* particle2, int decay)
//{
//  std::vector<double> massHypo;
//  massHypo.clear();
//  massHypo.push_back(MUMASS);
//  massHypo.push_back(MUMASS);
//  if(decay == bD_to_Kstar){  
//    massHypo.push_back(KPLUSMASS);
//    massHypo.push_back(PIMASS);
//  }
//  if(decay == bS_to_Phi){  
//    massHypo.push_back(KPLUSMASS);
//    massHypo.push_back(KPLUSMASS);    
//  }
//  if(decay == lB_to_L){  
//    massHypo.push_back(PROTONMASS);
//    massHypo.push_back(PIMASS);    
//  }  
//  std::vector<const TrigInDetTrack*> bTracks;
//  bTracks.clear();
//  bTracks.push_back(mu1);
//  bTracks.push_back(mu2);
//  bTracks.push_back(particle1);
//  bTracks.push_back(particle2);  
//  return InvMass(bTracks, massHypo);                                            
//}                           
//double TrigL2BMuMuXFex::X3Mass(const TrigInDetTrack* particle1, const TrigInDetTrack* particle2, const TrigInDetTrack* particle3)                
//{
//  std::vector<double> massHypo;
//  massHypo.clear();
//  massHypo.push_back(KPLUSMASS);
//  massHypo.push_back(KPLUSMASS);    
//  massHypo.push_back(PIMASS);    
//  std::vector<const TrigInDetTrack*> bTracks;
//  bTracks.clear();
//  bTracks.push_back(particle1);
//  bTracks.push_back(particle2);
//  bTracks.push_back(particle3);
//  return InvMass(bTracks, massHypo);                                            
//}
//double TrigL2BMuMuXFex::X3MuMuMass(const TrigInDetTrack* mu1, const TrigInDetTrack* mu2,
//                                     const TrigInDetTrack* particle1, const TrigInDetTrack* particle2, const TrigInDetTrack* particle3)
//{
//  std::vector<double> massHypo;
//  massHypo.clear();
//  massHypo.push_back(MUMASS);
//  massHypo.push_back(MUMASS);
//  massHypo.push_back(KPLUSMASS);
//  massHypo.push_back(KPLUSMASS);    
//  massHypo.push_back(PIMASS);
//  std::vector<const TrigInDetTrack*> bTracks;
//  bTracks.clear();
//  bTracks.push_back(mu1);
//  bTracks.push_back(mu2);
//  bTracks.push_back(particle1);
//  bTracks.push_back(particle2);  
//  bTracks.push_back(particle3);  
//  return InvMass(bTracks, massHypo);                                            
//}                           





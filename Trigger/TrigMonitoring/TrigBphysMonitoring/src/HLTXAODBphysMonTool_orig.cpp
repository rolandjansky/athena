/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @file HLTXAODBphysMonTool.cxx
 *
 *
 *
 *
 *    @author name (uid@cern.ch)
 *    Based on HLTMonTool example by Christiane Risler and Martin zur Nedden
 */

// Gaudi and Athena
#include "GaudiKernel/IJobOptionsSvc.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"

#include "TrkParametersBase/ParametersBase.h"
#include "VxVertex/VxTrackAtVertex.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"

// ROOT
#include "TROOT.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TVector3.h"

// C++
#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>
#include <map>
#include <cmath>
#include <algorithm>
#include <iomanip>

// Jpsi's
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackingPrimitives.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODMuon/Muon.h"

// TrigBphysMonitoring
#include "TrigBphysMonitoring/HLTXAODBphysMonTool.h"

#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/TrigBphys.h"

// Vertex
#include "VxVertex/VxContainer.h"

// Tracks
#include "TrkParameters/TrackParameters.h"

// For GetKalmanUpdator
#include "GaudiKernel/ListItem.h"

// TriggerDecisionTool
#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/FeatureContainer.h"

using namespace std;

/** Constructor, calls base class constructor with parameters
 *
 *  several properties are "declared" here, allowing selection
 *  of the filepath for histograms, the first and second plane
 *  numbers to be used, and the timebin.
 */
HLTXAODBphysMonTool::HLTXAODBphysMonTool(const std::string & type,
                                         const std::string & name,
                                         const IInterface* parent)
:IHLTMonTool(type, name, parent),
m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
m_muonMass(105.66)
{
    
    // m_bphysItems.push_back("HLT_2mu6_bDimu");
    declareProperty("muonMass"          , m_muonMass);
    declareProperty("TrigNames"         , m_TrigNames);
    declareProperty("TrigNames1D"       , m_TrigNames1D);
    declareProperty("TrigDecisionTool"  , m_trigDec, "The tool to access TrigDecision");
    declareProperty("BphysicsItems"     , m_bphysItems, "Bphysics triggers to test");
    declareProperty("MonGroup"          , m_monGroup, "Monitoring groups");
    declareProperty("ContainerList"     , m_containerList, "Container list");
    declareProperty("JpsiCandidates"    , m_JpsiCandidatesKey = "JpsiCandidates","Offline di-muon candidates");
    declareProperty("EffNum"            , m_effNum, "Required triggers for numerators of efficiency histograms");
    declareProperty("EffDenom"          , m_effDenom, "Required triggers for denominators of efficiency histograms");
    declareProperty("EffNumGroup"       , m_effNumGroup, "Group for numerator");
    declareProperty("EffDenomGroup"     , m_effDenomGroup, "Group for denominator");
    
    // parameters such as cuts, mass windows and hist binnings
    declareProperty("MW_jpsi_forward_min"          , m_mw_jpsi_forward_min  , "Jpsi mass forward window low");
    declareProperty("MW_jpsi_forward_max"          , m_mw_jpsi_forward_max  , "Jpsi mass forward window high");
    declareProperty("MW_upsi_forward_min"          , m_mw_upsi_forward_min  , "Upsilon mass forward window low");
    declareProperty("MW_upsi_forward_max"          , m_mw_upsi_forward_max  , "Upsilon mass forward window high");
    declareProperty("MW_jpsi_central_min"          , m_mw_jpsi_central_min  , "Jpsi mass central window low");
    declareProperty("MW_jpsi_central_max"          , m_mw_jpsi_central_max  , "Jpsi mass central window high");
    declareProperty("MW_upsi_central_min"          , m_mw_upsi_central_min  , "Upsilon mass central window low");
    declareProperty("MW_upsi_central_max"          , m_mw_upsi_central_max  , "Upsilon mass central window high");
    declareProperty("Eta_min"                      , m_eta_min              , "Minimum Eta");
    declareProperty("Eta_max"                      , m_eta_max              , "Maximum Eta");
    declareProperty("Mu_pt_min"                    , m_mu_pt_min            , "Minimum muon pT");
    declareProperty("Mu_pt_max"                    , m_mu_pt_max            , "Maximum muon pT");
    declareProperty("Z0_min"                       , m_z0_min               , "Minimum z0");
    declareProperty("Z0_max"                       , m_z0_max               , "Maximum z0");
    declareProperty("D0_min"                       , m_d0_min               , "Minimum d0");
    declareProperty("D0_max"                       , m_d0_max               , "Maximum d0");
    declareProperty("DeltaPhi_min"                 , m_dphi_min             , "Minimum delta phi of muons");
    declareProperty("DeltaPhi_max"                 , m_dphi_max             , "Maximum delta phi of muons");
    declareProperty("DeltaEta_min"                 , m_deta_min             , "Minimum delta eta of muons");
    declareProperty("DeltaEta_max"                 , m_deta_max             , "Maximum delta eta of muons");
    declareProperty("DeltaR_min"                   , m_dr_min               , "Minimum delta R of muons");
    declareProperty("DeltaR_max"                   , m_dr_max               , "Maximum delta R of muons");
    declareProperty("PtSum_min"                    , m_ptsum_min            , "Minimum pT sum of muons");
    declareProperty("PtSum_max"                    , m_ptsum_max            , "Maximum pT sum of muons");
    declareProperty("OniaMass_min"                 , m_oniamass_min         , "Minimum onia mass");
    declareProperty("OniaMass_max"                 , m_oniamass_max         , "Maximum onia mass");
    declareProperty("OniaPt_min"                   , m_oniapt_min           , "Minimum onia pt");
    declareProperty("OniaPt_max"                   , m_oniapt_max           , "Maximum onia pt");
}



//-----------------------------------------------------------
HLTXAODBphysMonTool::~HLTXAODBphysMonTool()
//-----------------------------------------------------------
{
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::init()
//-----------------------------------------------------------
{
    
    ATH_MSG_INFO ("Initializing... ");
    
    m_all   = getTDT()->getChainGroup(".*");
    return StatusCode::SUCCESS;
}

//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::book() // suppress 'unused' compiler warning
//-----------------------------------------------------------
{
    
    ATH_MSG_INFO ("Booking... ");
    
    // Build Trigger Monitoring histograms
    // For each trigger in the list build a set of histograms
    //    for (int iTrigName = 0; iTrigName < (int)m_bphysItems.size(); ++iTrigName){
    //        addMonGroup(new MonGroup(this,"HLT/BphysMon/Trigger/"+m_monGroup[iTrigName]+"/"+m_bphysItems[iTrigName],run));
    //        buildHist(m_bphysItems[iTrigName],"TrigBphys_L2");
    //        buildHist(m_bphysItems[iTrigName],"TrigBphys_EF");
    //    }
    
    // build histograms for each group of triggers
    const TString triggerGroups = {"JpsiFinder","BMuMu","BMuMuX","MuTrk"};
    const unsigned int nTriggerGroups = sizeof(triggerGroups) / sizeof(triggerGroups[0]);
    
    for (auto& group : triggerGroups) {
        addMonGroup(new MonGroup(this,"HLT/BphysMon/"+group,run));
        buildHist(group,"TrigBphys_L2");
        buildHist(group,"TrigBphys_EF");
    } // book hists for each trigger group
    
    
    //Build monitoring histograms for containers
    // for each container in the list, build the set of histograms (works for EF and L2)
    for (auto containerItem : m_containerList){
        addMonGroup(new MonGroup(this,"HLT/BphysMon/Container/"+containerItem,run));
        buildHist(containerItem,"TrigBphys");
    }
    
    
    
    //Build monitoring histograms for offline reconstructed vertices
    
    addMonGroup(new MonGroup(this,"HLT/BphysMon/JpsiFinder",run));
    
    addHistogram( new TH1F(Form("offline_mumu_mass") ,   Form("offline_mumu_mass;mass(#mu#mu)[GeV];Candidates") ,    100, 2e3,5000.0) );
    addHistogram( new TH1F(Form("offline_mumu_mass_diff") ,   Form("offline_mumu_mass_diff;mass(#mu#mu)[GeV];Candidates") ,    50, -150.,150.) );
    
    
    // 2D Histograms for all triggers
    int TrigSize = (int)m_bphysItems.size();
    
    // central eta < |1.05| -- forward eta > |1.05|
    addHistogram( new TH2F("TrigBphys_Jpsi_mass_central",         "Bphys Jpsi Mass Central"             ,100, 2950,3250, TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("TrigBphys_Jpsi_high_pT_central",      "Bphys Jpsi Highest pT Muon Central"  ,50, 0.,50., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("TrigBphys_Jpsi_low_pT_central",       "Bphys Jpsi Lowest pT Muon Central"   ,50, 0.,50., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("TrigBphys_Jpsi_dphi_central",         "Bphys Jpsi DeltaPhi Central"         , 64, 0.,3.2, TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("TrigBphys_Jpsi_deta_central",         "Bphys Jpsi DeltaEta Central"         , 60, 0.,3., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("TrigBphys_Jpsi_cosThetaStar_central", "Bphys Jpsi CosThetaStar Central"     ,50, -1.,1., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("TrigBphys_Jpsi_phiStar_central",      "Bphys Jpsi PhiStar Central"          , 70, -3.5,3.5, TrigSize, 0., (double)TrigSize) );
    
    addHistogram( new TH2F("TrigBphys_Jpsi_mass_forward",         "Bphys Jpsi Mass Forward"             ,100, 2800,3400, TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("TrigBphys_Jpsi_high_pT_forward",      "Bphys Jpsi Highest pT Muon Forward"  ,50, 0.,50., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("TrigBphys_Jpsi_low_pT_forward",       "Bphys Jpsi Lowest pT Muon Forward"   ,50, 0.,50., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("TrigBphys_Jpsi_dphi_forward",         "Bphys Jpsi DeltaPhi Forward"         , 64, 0.,3.2, TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("TrigBphys_Jpsi_deta_forward",         "Bphys Jpsi DeltaEta Forward"         , 60, 0.,3., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("TrigBphys_Jpsi_cosThetaStar_forward", "Bphys Jpsi CosThetaStar Forward"     ,50, -1.,1., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("TrigBphys_Jpsi_phiStar_forward",      "Bphys Jpsi PhiStar Forward"          , 70, -3.5,3.5, TrigSize, 0., (double)TrigSize) );
    
    addHistogram( new TH2F("TrigBphys_Upsi_mass_central",         "Bphys Upsi Mass Central"             ,100, 9000,11500, TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("TrigBphys_Upsi_high_pT_central",      "Bphys Upsi Highest pT Muon Central"  ,50, 0.,50., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("TrigBphys_Upsi_low_pT_central",       "Bphys Upsi Lowest pT Muon Central"   ,50, 0.,50., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("TrigBphys_Upsi_dphi_central",         "Bphys Upsi DeltaPhi Central"         , 64, 0.,3.2, TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("TrigBphys_Upsi_deta_central",         "Bphys Upsi DeltaEta Central"         , 60, 0.,3., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("TrigBphys_Upsi_cosThetaStar_central", "Bphys Upsi CosThetaStar Central"     ,50, -1.,1., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("TrigBphys_Upsi_phiStar_central",      "Bphys Upsi PhiStar Central"          , 70, -3.5,3.5, TrigSize, 0., (double)TrigSize) );
    
    addHistogram( new TH2F("TrigBphys_Upsi_mass_forward",         "Bphys Upsi Mass Forward"             ,100, 8000,12000, TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("TrigBphys_Upsi_high_pT_forward",      "Bphys Upsi Highest pT Muon Forward"  ,50, 0.,50., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("TrigBphys_Upsi_low_pT_forward",       "Bphys Upsi Lowest pT Muon Forward"   ,50, 0.,50., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("TrigBphys_Upsi_dphi_forward",         "Bphys Upsi DeltaPhi Forward"         , 64, 0.,3.2, TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("TrigBphys_Upsi_deta_forward",         "Bphys Upsi DeltaEta Forward"         , 60, 0.,3., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("TrigBphys_Upsi_cosThetaStar_forward", "Bphys Upsi CosThetaStar Forward"     ,50, -1.,1., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("TrigBphys_Upsi_phiStar_forward",      "Bphys Upsi PhiStar Forward"          , 70, -3.5,3.5, TrigSize, 0., (double)TrigSize) );
    
    
    addHistogram( new TH2F("denom_TrigBphys_Jpsi_mass_central",         "Bphys Jpsi Mass Central"             ,100, 2950,3250, TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("denom_TrigBphys_Jpsi_high_pT_central",      "Bphys Jpsi Highest pT Muon Central"  ,50, 0.,50., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("denom_TrigBphys_Jpsi_low_pT_central",       "Bphys Jpsi Lowest pT Muon Central"   ,50, 0.,50., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("denom_TrigBphys_Jpsi_dphi_central",         "Bphys Jpsi DeltaPhi Central"         , 64, 0.,3.2, TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("denom_TrigBphys_Jpsi_deta_central",         "Bphys Jpsi DeltaEta Central"         , 60, 0.,3., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("denom_TrigBphys_Jpsi_cosThetaStar_central", "Bphys Jpsi CosThetaStar Central"     ,50, -1.,1., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("denom_TrigBphys_Jpsi_phiStar_central",      "Bphys Jpsi PhiStar Central"          , 70, -3.5,3.5, TrigSize, 0., (double)TrigSize) );
    
    addHistogram( new TH2F("denom_TrigBphys_Jpsi_mass_forward",         "Bphys Jpsi Mass Forward"             ,100, 2800,3400, TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("denom_TrigBphys_Jpsi_high_pT_forward",      "Bphys Jpsi Highest pT Muon Forward"  ,50, 0.,50., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("denom_TrigBphys_Jpsi_low_pT_forward",       "Bphys Jpsi Lowest pT Muon Forward"   ,50, 0.,50., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("denom_TrigBphys_Jpsi_dphi_forward",         "Bphys Jpsi DeltaPhi Forward"         , 64, 0.,3.2, TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("denom_TrigBphys_Jpsi_deta_forward",         "Bphys Jpsi DeltaEta Forward"         , 60, 0.,3., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("denom_TrigBphys_Jpsi_cosThetaStar_forward", "Bphys Jpsi CosThetaStar Forward"     ,50, -1.,1., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("denom_TrigBphys_Jpsi_phiStar_forward",      "Bphys Jpsi PhiStar Forward"          , 70, -3.5,3.5, TrigSize, 0., (double)TrigSize) );
    
    addHistogram( new TH2F("denom_TrigBphys_Upsi_mass_central",         "Bphys Upsi Mass Central"             ,100, 9000,11500, TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("denom_TrigBphys_Upsi_high_pT_central",      "Bphys Upsi Highest pT Muon Central"  ,50, 0.,50., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("denom_TrigBphys_Upsi_low_pT_central",       "Bphys Upsi Lowest pT Muon Central"   ,50, 0.,50., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("denom_TrigBphys_Upsi_dphi_central",         "Bphys Upsi DeltaPhi Central"         , 64, 0.,3.2, TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("denom_TrigBphys_Upsi_deta_central",         "Bphys Upsi DeltaEta Central"         , 60, 0.,3., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("denom_TrigBphys_Upsi_cosThetaStar_central", "Bphys Upsi CosThetaStar Central"     ,50, -1.,1., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("denom_TrigBphys_Upsi_phiStar_central",      "Bphys Upsi PhiStar Central"          , 70, -3.5,3.5, TrigSize, 0., (double)TrigSize) );
    
    addHistogram( new TH2F("denom_TrigBphys_Upsi_mass_forward",         "Bphys Upsi Mass Forward"             ,100, 8000,12000, TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("denom_TrigBphys_Upsi_high_pT_forward",      "Bphys Upsi Highest pT Muon Forward"  ,50, 0.,50., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("denom_TrigBphys_Upsi_low_pT_forward",       "Bphys Upsi Lowest pT Muon Forward"   ,50, 0.,50., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("denom_TrigBphys_Upsi_dphi_forward",         "Bphys Upsi DeltaPhi Forward"         , 64, 0.,3.2, TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("denom_TrigBphys_Upsi_deta_forward",         "Bphys Upsi DeltaEta Forward"         , 60, 0.,3., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("denom_TrigBphys_Upsi_cosThetaStar_forward", "Bphys Upsi CosThetaStar Forward"     ,50, -1.,1., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("denom_TrigBphys_Upsi_phiStar_forward",      "Bphys Upsi PhiStar Forward"          , 70, -3.5,3.5, TrigSize, 0., (double)TrigSize) );
    
    
    addHistogram( new TH2F("eff_TrigBphys_Jpsi_mass_central",         "Bphys Jpsi Mass Central"             ,100, 2950,3250, TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("eff_TrigBphys_Jpsi_high_pT_central",      "Bphys Jpsi Highest pT Muon Central"  ,50, 0.,50., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("eff_TrigBphys_Jpsi_low_pT_central",       "Bphys Jpsi Lowest pT Muon Central"   ,50, 0.,50., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("eff_TrigBphys_Jpsi_dphi_central",         "Bphys Jpsi DeltaPhi Central"         , 64, 0.,3.2, TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("eff_TrigBphys_Jpsi_deta_central",         "Bphys Jpsi DeltaEta Central"         , 60, 0.,3., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("eff_TrigBphys_Jpsi_cosThetaStar_central", "Bphys Jpsi CosThetaStar Central"     ,50, -1.,1., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("eff_TrigBphys_Jpsi_phiStar_central",      "Bphys Jpsi PhiStar Central"          , 70, -3.5,3.5, TrigSize, 0., (double)TrigSize) );
    
    addHistogram( new TH2F("eff_TrigBphys_Jpsi_mass_forward",         "Bphys Jpsi Mass Forward"             ,100, 2800,3400, TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("eff_TrigBphys_Jpsi_high_pT_forward",      "Bphys Jpsi Highest pT Muon Forward"  ,50, 0.,50., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("eff_TrigBphys_Jpsi_low_pT_forward",       "Bphys Jpsi Lowest pT Muon Forward"   ,50, 0.,50., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("eff_TrigBphys_Jpsi_dphi_forward",         "Bphys Jpsi DeltaPhi Forward"         , 64, 0.,3.2, TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("eff_TrigBphys_Jpsi_deta_forward",         "Bphys Jpsi DeltaEta Forward"         , 60, 0.,3., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("eff_TrigBphys_Jpsi_cosThetaStar_forward", "Bphys Jpsi CosThetaStar Forward"     ,50, -1.,1., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("eff_TrigBphys_Jpsi_phiStar_forward",      "Bphys Jpsi PhiStar Forward"          , 70, -3.5,3.5, TrigSize, 0., (double)TrigSize) );
    
    addHistogram( new TH2F("eff_TrigBphys_Upsi_mass_central",         "Bphys Upsi Mass Central"             ,100, 9000,11500, TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("eff_TrigBphys_Upsi_high_pT_central",      "Bphys Upsi Highest pT Muon Central"  ,50, 0.,50., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("eff_TrigBphys_Upsi_low_pT_central",       "Bphys Upsi Lowest pT Muon Central"   ,50, 0.,50., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("eff_TrigBphys_Upsi_dphi_central",         "Bphys Upsi DeltaPhi Central"         , 64, 0.,3.2, TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("eff_TrigBphys_Upsi_deta_central",         "Bphys Upsi DeltaEta Central"         , 60, 0.,3., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("eff_TrigBphys_Upsi_cosThetaStar_central", "Bphys Upsi CosThetaStar Central"     ,50, -1.,1., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("eff_TrigBphys_Upsi_phiStar_central",      "Bphys Upsi PhiStar Central"          , 70, -3.5,3.5, TrigSize, 0., (double)TrigSize) );
    
    addHistogram( new TH2F("eff_TrigBphys_Upsi_mass_forward",         "Bphys Upsi Mass Forward"             ,100, 8000,12000, TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("eff_TrigBphys_Upsi_high_pT_forward",      "Bphys Upsi Highest pT Muon Forward"  ,50, 0.,50., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("eff_TrigBphys_Upsi_low_pT_forward",       "Bphys Upsi Lowest pT Muon Forward"   ,50, 0.,50., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("eff_TrigBphys_Upsi_dphi_forward",         "Bphys Upsi DeltaPhi Forward"         , 64, 0.,3.2, TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("eff_TrigBphys_Upsi_deta_forward",         "Bphys Upsi DeltaEta Forward"         , 60, 0.,3., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("eff_TrigBphys_Upsi_cosThetaStar_forward", "Bphys Upsi CosThetaStar Forward"     ,50, -1.,1., TrigSize, 0., (double)TrigSize) );
    addHistogram( new TH2F("eff_TrigBphys_Upsi_phiStar_forward",      "Bphys Upsi PhiStar Forward"          , 70, -3.5,3.5, TrigSize, 0., (double)TrigSize) );
    
    for (int iTrigName = 1; iTrigName <= (int)m_bphysItems.size(); ++iTrigName){
        
        TString label = m_bphysItems[iTrigName-1];
        hist2("TrigBphys_Jpsi_mass_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("TrigBphys_Jpsi_high_pT_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("TrigBphys_Jpsi_low_pT_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("TrigBphys_Jpsi_dphi_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("TrigBphys_Jpsi_deta_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("TrigBphys_Jpsi_cosThetaStar_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("TrigBphys_Jpsi_phiStar_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        
        hist2("TrigBphys_Jpsi_mass_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("TrigBphys_Jpsi_high_pT_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("TrigBphys_Jpsi_low_pT_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("TrigBphys_Jpsi_dphi_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("TrigBphys_Jpsi_deta_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("TrigBphys_Jpsi_cosThetaStar_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("TrigBphys_Jpsi_phiStar_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        
        hist2("TrigBphys_Upsi_mass_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("TrigBphys_Upsi_high_pT_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("TrigBphys_Upsi_low_pT_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("TrigBphys_Upsi_dphi_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("TrigBphys_Upsi_deta_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("TrigBphys_Upsi_cosThetaStar_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("TrigBphys_Upsi_phiStar_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        
        hist2("TrigBphys_Upsi_mass_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("TrigBphys_Upsi_high_pT_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("TrigBphys_Upsi_low_pT_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("TrigBphys_Upsi_dphi_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("TrigBphys_Upsi_deta_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("TrigBphys_Upsi_cosThetaStar_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("TrigBphys_Upsi_phiStar_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        
        
        
        hist2("denom_TrigBphys_Jpsi_mass_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("denom_TrigBphys_Jpsi_high_pT_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("denom_TrigBphys_Jpsi_low_pT_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("denom_TrigBphys_Jpsi_dphi_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("denom_TrigBphys_Jpsi_deta_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("denom_TrigBphys_Jpsi_cosThetaStar_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("denom_TrigBphys_Jpsi_phiStar_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        
        hist2("denom_TrigBphys_Jpsi_mass_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("denom_TrigBphys_Jpsi_high_pT_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("denom_TrigBphys_Jpsi_low_pT_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("denom_TrigBphys_Jpsi_dphi_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("denom_TrigBphys_Jpsi_deta_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("denom_TrigBphys_Jpsi_cosThetaStar_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("denom_TrigBphys_Jpsi_phiStar_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        
        hist2("denom_TrigBphys_Upsi_mass_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("denom_TrigBphys_Upsi_high_pT_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("denom_TrigBphys_Upsi_low_pT_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("denom_TrigBphys_Upsi_dphi_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("denom_TrigBphys_Upsi_deta_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("denom_TrigBphys_Upsi_cosThetaStar_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("denom_TrigBphys_Upsi_phiStar_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        
        hist2("denom_TrigBphys_Upsi_mass_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("denom_TrigBphys_Upsi_high_pT_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("denom_TrigBphys_Upsi_low_pT_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("denom_TrigBphys_Upsi_dphi_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("denom_TrigBphys_Upsi_deta_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("denom_TrigBphys_Upsi_cosThetaStar_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("denom_TrigBphys_Upsi_phiStar_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        
        
        
        hist2("eff_TrigBphys_Jpsi_mass_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("eff_TrigBphys_Jpsi_high_pT_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("eff_TrigBphys_Jpsi_low_pT_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("eff_TrigBphys_Jpsi_dphi_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("eff_TrigBphys_Jpsi_deta_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("eff_TrigBphys_Jpsi_cosThetaStar_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("eff_TrigBphys_Jpsi_phiStar_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        
        hist2("eff_TrigBphys_Jpsi_mass_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("eff_TrigBphys_Jpsi_high_pT_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("eff_TrigBphys_Jpsi_low_pT_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("eff_TrigBphys_Jpsi_dphi_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("eff_TrigBphys_Jpsi_deta_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("eff_TrigBphys_Jpsi_cosThetaStar_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("eff_TrigBphys_Jpsi_phiStar_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        
        hist2("eff_TrigBphys_Upsi_mass_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("eff_TrigBphys_Upsi_high_pT_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("eff_TrigBphys_Upsi_low_pT_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("eff_TrigBphys_Upsi_dphi_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("eff_TrigBphys_Upsi_deta_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("eff_TrigBphys_Upsi_cosThetaStar_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("eff_TrigBphys_Upsi_phiStar_central")->GetYaxis()->SetBinLabel(iTrigName,label);
        
        hist2("eff_TrigBphys_Upsi_mass_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("eff_TrigBphys_Upsi_high_pT_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("eff_TrigBphys_Upsi_low_pT_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("eff_TrigBphys_Upsi_dphi_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("eff_TrigBphys_Upsi_deta_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("eff_TrigBphys_Upsi_cosThetaStar_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        hist2("eff_TrigBphys_Upsi_phiStar_forward")->GetYaxis()->SetBinLabel(iTrigName,label);
        
        
    }
    
    
    
    
    
    
    // Trigger Efficiency
    addMonGroup(new MonGroup(this,"HLT/BphysMon/TriggerEfficiency",run));
    
    
    // Creating histograms for denominator of efficiency
    int EffSize(0);
    
    for (int DenomOrNum = 0; DenomOrNum < 2; ++DenomOrNum){
        
        
        EffSize = (int)m_effDenom.size();
        
        for (int iTrigEff = 0; iTrigEff < EffSize; ++iTrigEff){
            
            string EffDenomNum_etaHist1;
            string EffDenomNum_etaHist2;
            string EffDenomNum_phiHist1;
            string EffDenomNum_phiHist2;
            string EffDenomNum_pTHist1;
            string EffDenomNum_pTHist2;
            
            if ( DenomOrNum == 0){
                EffDenomNum_etaHist1 = "EffDenom_" + m_effDenom[iTrigEff] + "_eta1";
                EffDenomNum_etaHist2 = "EffDenom_" + m_effDenom[iTrigEff] + "_eta2";
                EffDenomNum_phiHist1 = "EffDenom_" + m_effDenom[iTrigEff] + "_phi1";
                EffDenomNum_phiHist2 = "EffDenom_" + m_effDenom[iTrigEff] + "_phi2";
                EffDenomNum_pTHist1 = "EffDenom_" + m_effDenom[iTrigEff] + "_pT1";
                EffDenomNum_pTHist2 = "EffDenom_" + m_effDenom[iTrigEff] + "_pT2";
            } else {
                EffDenomNum_etaHist1 = "EffNum_" + m_effNum[iTrigEff] + "_eta1";
                EffDenomNum_etaHist2 = "EffNum_" + m_effNum[iTrigEff] + "_eta2";
                EffDenomNum_phiHist1 = "EffNum_" + m_effNum[iTrigEff] + "_phi1";
                EffDenomNum_phiHist2 = "EffNum_" + m_effNum[iTrigEff] + "_phi2";
                EffDenomNum_pTHist1 = "EffNum_" + m_effNum[iTrigEff] + "_pT1";
                EffDenomNum_pTHist2 = "EffNum_" + m_effNum[iTrigEff] + "_pT2";
            }
            
            
            addHistogram(EffHistMap[EffDenomNum_etaHist1] = new TH1F(EffDenomNum_etaHist1.c_str()    , EffDenomNum_etaHist1.c_str()    , 60, -3.,3.));
            addHistogram(EffHistMap[EffDenomNum_etaHist2] = new TH1F(EffDenomNum_etaHist2.c_str()    , EffDenomNum_etaHist2.c_str()    , 60, -3.,3.));
            addHistogram(EffHistMap[EffDenomNum_phiHist1] = new TH1F(EffDenomNum_phiHist1.c_str()    , EffDenomNum_phiHist1.c_str()    , 70, -3.5,3.5));
            addHistogram(EffHistMap[EffDenomNum_phiHist2] = new TH1F(EffDenomNum_phiHist2.c_str()    , EffDenomNum_phiHist2.c_str()    , 70, -3.5,3.5));
            addHistogram(EffHistMap[EffDenomNum_pTHist1] = new TH1F(EffDenomNum_pTHist1.c_str()    , EffDenomNum_pTHist1.c_str()    , 50, 0.,50.));
            addHistogram(EffHistMap[EffDenomNum_pTHist2] = new TH1F(EffDenomNum_pTHist2.c_str()    , EffDenomNum_pTHist2.c_str()   , 50, 0.,50.));
            
        }
    }
    
    
    // Creating histograms for efficiency
    for (int iEff = 0; iEff < (int)m_effDenom.size(); ++iEff){
        
        TString Eff_etaHist1 = "Eff_" + m_effNum[iEff] + "__" + m_effDenom[iEff] + "_eta1";
        TString Eff_etaHist2 = "Eff_" + m_effNum[iEff] + "__" + m_effDenom[iEff] + "_eta2";
        TString Eff_phiHist1 = "Eff_" + m_effNum[iEff] + "__" + m_effDenom[iEff] + "_phi1";
        TString Eff_phiHist2 = "Eff_" + m_effNum[iEff] + "__" + m_effDenom[iEff] + "_phi2";
        TString Eff_pTHist1 = "Eff_" + m_effNum[iEff] + "__" + m_effDenom[iEff] + "_pT1";
        TString Eff_pTHist2 = "Eff_" + m_effNum[iEff] + "__" + m_effDenom[iEff] + "_pT2";
        
        addHistogram( new TH1F(Eff_etaHist1     , Eff_etaHist1    ,  60, -3.,3.) );
        addHistogram( new TH1F(Eff_etaHist2     , Eff_etaHist2    ,  60, -3.,3.) );
        addHistogram( new TH1F(Eff_phiHist1     , Eff_phiHist1    ,  70, -3.5,3.5) );
        addHistogram( new TH1F(Eff_phiHist2     , Eff_phiHist2    ,  70, -3.5,3.5) );
        addHistogram( new TH1F(Eff_pTHist1      , Eff_pTHist1     , 50, 0.,50.) );
        addHistogram( new TH1F(Eff_pTHist2      , Eff_pTHist2     , 50, 0.,50.) );
        
        
        //TString Eff_etaHistPre1 = "Eff_" + EffNum[nEffNum] + "__" + EffDenom[iEff] + "_eta1_Prescale";
        //TString Eff_etaHistPre2 = "Eff_" + EffNum[nEffNum] + "__" + EffDenom[iEff] + "_eta2_Prescale";
        //TString Eff_phiHistPre1 = "Eff_" + EffNum[nEffNum] + "__" + EffDenom[iEff] + "_phi1_Prescale";
        //TString Eff_phiHistPre2 = "Eff_" + EffNum[nEffNum] + "__" + EffDenom[iEff] + "_phi2_Prescale";
        //TString Eff_pTHistPre1 = "Eff_" + EffNum[nEffNum] + "__" + EffDenom[iEff] + "_pT1_Prescale";
        //TString Eff_pTHistPre2 = "Eff_" + EffNum[nEffNum] + "__" + EffDenom[iEff] + "_pT2_Prescale";
        
        //addHistogram( new TH1F(Eff_etaHistPre1     , Eff_etaHistPre1    ,  60, -3.,3.) );
        //addHistogram( new TH1F(Eff_etaHistPre2     , Eff_etaHistPre2    ,  60, -3.,3.) );
        //addHistogram( new TH1F(Eff_phiHistPre1     , Eff_phiHistPre1    ,  70, -3.5,3.5) );
        //addHistogram( new TH1F(Eff_phiHistPre2     , Eff_phiHistPre2    ,  70, -3.5,3.5) );
        //addHistogram( new TH1F(Eff_pTHistPre1      , Eff_pTHistPre1     , 100, 0.,50.) );
        //addHistogram( new TH1F(Eff_pTHistPre2      , Eff_pTHistPre2     , 100, 0.,50.) );
    }
    
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
void HLTXAODBphysMonTool::buildHist( const std::string & trigname,const std::string & prefix) // suppress 'unused' compiler warning
//-----------------------------------------------------------
{
    
    
    
    
    if(prefix == "Offline" || prefix == "JpsiFinder"){
        
        addHistogram( new TH1F(Form("%s_%s_mass_diff",prefix.c_str(),trigname.c_str()) ,  Form("%s_%s_mass_diff;#Delta(#mu#mu)[MeV];Candidates",prefix.c_str(),trigname.c_str()) ,    50, -150.,150.) );
        
    }
    
    //if(prefix == "TrigBphys"){
    {
        // build...(bphysItem.c_str(), "TrigBphys");
        addHistogram( new TH1F(Form("%s_%s_eta",prefix.c_str(),trigname.c_str()) ,      Form("%s_%s_eta;#eta(#mu#mu);Candidates",prefix.c_str(),trigname.c_str()) ,     60, m_eta_min,m_eta_max) );
        addHistogram( new TH1F(Form("%s_%s_phi",prefix.c_str(),trigname.c_str()) ,      Form("%s_%s_phi;#phi(#mu#mu);Candidates",prefix.c_str(),trigname.c_str()) ,     70, -3.5,3.5) );
        addHistogram( new TH1F(Form("%s_%s_pTmu1",prefix.c_str(),trigname.c_str()) ,    Form("%s_%s_pTmu1;p_{T}(#mu_{1})[GeV];Candidates",prefix.c_str(),trigname.c_str()) ,    50, m_mu_pt_min,m_mu_pt_max) );
        addHistogram( new TH1F(Form("%s_%s_pTmu2",prefix.c_str(),trigname.c_str()) ,    Form("%s_%s_pTmu2;p_{T}(#mu_{2})[GeV];Candidates",prefix.c_str(),trigname.c_str()) ,    50, m_mu_pt_min,m_mu_pt_max) );
        addHistogram( new TH1F(Form("%s_%s_z0mu1",prefix.c_str(),trigname.c_str()) ,    Form("%s_%s_z0mu1;z_{0}(#mu_{1})[mm];Candidates",prefix.c_str(),trigname.c_str()) ,    150, m_z0_min,m_z0_max) );
        addHistogram( new TH1F(Form("%s_%s_z0mu2",prefix.c_str(),trigname.c_str()) ,    Form("%s_%s_z0mu2;z_{0}(#mu_{1})[mm];Candidates",prefix.c_str(),trigname.c_str()) ,    150, m_z0_min,m_z0_max) );
        addHistogram( new TH1F(Form("%s_%s_d0mu1",prefix.c_str(),trigname.c_str()) ,    Form("%s_%s_d0mu1;d_{0}(#mu_{1})[mm];Candidates",prefix.c_str(),trigname.c_str()) ,    100, m_d0_min,m_d0_max) );
        addHistogram( new TH1F(Form("%s_%s_d0mu2",prefix.c_str(),trigname.c_str()) ,    Form("%s_%s_d0mu2;d_{0}(#mu_{1})[mm];Candidates",prefix.c_str(),trigname.c_str()) ,    100, m_d0_min,m_d0_max) );
        addHistogram( new TH1F(Form("%s_%s_mass",prefix.c_str(),trigname.c_str()) ,     Form("%s_%s_mass;mass(#mu#mu)[GeV];Candidates",prefix.c_str(),trigname.c_str()) ,    100, m_oniamass_min,m_oniamass_max) );
        addHistogram( new TH1F(Form("%s_%s_dphi",prefix.c_str(),trigname.c_str()) ,     Form("%s_%s_dphi;d#phi(#mu_{1,2});Candidates",prefix.c_str(),trigname.c_str()) ,     64, m_dphi_min,m_dphi_max) );
        addHistogram( new TH1F(Form("%s_%s_deta",prefix.c_str(),trigname.c_str()) ,     Form("%s_%s_deta;d#eta(#mu_{1,2});Candidates",prefix.c_str(),trigname.c_str()) ,     60, m_deta_min,m_deta_max) );
        addHistogram( new TH1F(Form("%s_%s_dR",prefix.c_str(),trigname.c_str()) ,       Form("%s_%s_dR;dR(#mu_{1,2});Candidates",prefix.c_str(),trigname.c_str()) ,     60, m_dr_min,m_dr_max) );
        addHistogram( new TH1F(Form("%s_%s_pTsum",prefix.c_str(),trigname.c_str()) ,    Form("%s_%s_pTsum;#Sigmap_{T}(#mu_{1,2})[GeV];Candidates",prefix.c_str(),trigname.c_str()) ,    100, m_ptsum_min,m_ptsum_max) );
        addHistogram( new TH1F(Form("%s_%s_phiStar",prefix.c_str(),trigname.c_str()) ,  Form("%s_%s_phiStar;#phi*;Candidates",prefix.c_str(),trigname.c_str()) ,     50, -TMath::Pi(), TMath::Pi()) );
        addHistogram( new TH1F(Form("%s_%s_cosThetaStar",prefix.c_str(),trigname.c_str()) , Form("%s_%s_cosThetaStar;cos#theta*;Candidates",prefix.c_str(),trigname.c_str()) ,     50, -1., 1.) );
        addHistogram( new TH2F(Form("%s_%s_phiStarcosThetaStar",prefix.c_str(),trigname.c_str()) , Form("%s_%s_phiStarcosThetaStar;cos#theta*;#phi*",prefix.c_str(),trigname.c_str()) ,  50, -1., 1., 50, -TMath::Pi(), TMath::Pi()));
        addHistogram( new TH1F(Form("%s_%s_fitmass",prefix.c_str(),trigname.c_str()) ,  Form("%s_%s_fitmass;fitmass(#mu#mu)[GeV];Candidates",prefix.c_str(),trigname.c_str()) ,    100, m_oniamass_min,m_oniamass_max) );
        addHistogram( new TH1F(Form("%s_%s_fitchi2",prefix.c_str(),trigname.c_str()) ,  Form("%s_%s_fitchi2;#chi^{2}(#mu#mu);Candidates",prefix.c_str(),trigname.c_str()) ,    50, 0.,10.) );
        addHistogram( new TH1F(Form("%s_%s_numBLayerHitsmu1",prefix.c_str(),trigname.c_str()) ,      Form("%s_%s_numBLayerHitsmu1;B Layer Hits(#mu_{1});Candidates",prefix.c_str(),trigname.c_str()) ,     11, -1.,10.) );
        addHistogram( new TH1F(Form("%s_%s_numBLayerHitsmu2",prefix.c_str(),trigname.c_str()) ,      Form("%s_%s_numBLayerHitsmu2;B Layer Hits(#mu_{2});Candidates",prefix.c_str(),trigname.c_str()) ,     11, -1.,10.) );
        addHistogram( new TH1F(Form("%s_%s_numSCTHitsmu1",prefix.c_str(),trigname.c_str()) ,      Form("%s_%s_numSCTHitsmu1;SCT Hits(#mu_{1});Candidates",prefix.c_str(),trigname.c_str()) ,     11, -1.,10.) );
        addHistogram( new TH1F(Form("%s_%s_numSCTHitsmu2",prefix.c_str(),trigname.c_str()) ,      Form("%s_%s_numSCTHitsmu2;SCT Hits(#mu_{2});Candidates",prefix.c_str(),trigname.c_str()) ,     11, -1.,10.) );
        addHistogram( new TH1F(Form("%s_%s_numPixelHitsmu1",prefix.c_str(),trigname.c_str()) ,      Form("%s_%s_numPixelHitsmu1;Pixel Hits(#mu_{1});Candidates",prefix.c_str(),trigname.c_str()) ,     11, -1.,10.) );
        addHistogram( new TH1F(Form("%s_%s_numPixelHitsmu2",prefix.c_str(),trigname.c_str()) ,      Form("%s_%s_numPixelHitsmu2;Pixel Hits(#mu_{2});Candidates",prefix.c_str(),trigname.c_str()) ,     11, -1.,10.) );
        addHistogram( new TH1F(Form("%s_%s_numTRTHitsmu1",prefix.c_str(),trigname.c_str()) ,      Form("%s_%s_numTRTHitsmu1;TRT Hits(#mu_{1});Candidates",prefix.c_str(),trigname.c_str()) ,     11, -1.,10.) );
        addHistogram( new TH1F(Form("%s_%s_numTRTHitsmu2",prefix.c_str(),trigname.c_str()) ,      Form("%s_%s_numTRTHitsmu2;TRT Hits(#mu_{2});Candidates",prefix.c_str(),trigname.c_str()) ,     11, -1.,10.) );
        
    }
    
    // return StatusCode::SUCCESS;
}


void HLTXAODBphysMonTool::fillHist(const xAOD::TrigBphys *bphysItem, const std::string & trigItem,const std::string & prefix) // suppress 'unused' compiler warning
//-----------------------------------------------------------
{
    
    ATH_MSG_DEBUG(":: FillHist() Test 1 :: ");
    
    float eta = bphysItem->eta();
    float phi = bphysItem->phi();
    float mass = bphysItem->mass();
    float fitmass = bphysItem->fitmass();
    float fitchi2 = bphysItem->fitchi2();
    
    ATH_MSG_DEBUG("mass :: "<< mass);
    // Dimu
    hist(Form("%s_%s_eta",prefix.c_str(),trigItem.c_str()))->Fill(eta);
    hist(Form("%s_%s_phi",prefix.c_str(),trigItem.c_str()))->Fill(phi);
    hist(Form("%s_%s_mass",prefix.c_str(),trigItem.c_str()))->Fill(mass);
    hist(Form("%s_%s_fitmass",prefix.c_str(),trigItem.c_str()))->Fill(fitmass);
    if (fitchi2 != -99) hist(Form("%s_%s_fitchi2",prefix.c_str(),trigItem.c_str()))->Fill(fitchi2);
    
    ATH_MSG_DEBUG(":: FillHist() Test 1 :: ");
    
    const std::vector<ElementLink<xAOD::TrackParticleContainer> > trackVector = bphysItem->trackParticleLinks();
    if (trackVector.size() != 0) {
        ATH_MSG_DEBUG(" REGTEST got track vector size: " << trackVector.size() );
    } else {
        ATH_MSG_DEBUG(" REGTEST no track vector!!! " );
    }
    
    
    std::vector<ElementLink<xAOD::TrackParticleContainer> >::const_iterator trkIt=trackVector.begin();
    for (int itrk=0 ; trkIt!= trackVector.end(); ++itrk, ++trkIt) {
        if (!(trkIt->isValid())) {
            ATH_MSG_WARNING("TrackParticleContainer::Invalid ElementLink to track ");
            continue;
        }

        const xAOD::TrackParticle * ptl1 = **trkIt;
        if (!ptl1) {
            msg() << MSG::WARNING << "TrackParticle with nullptr but valid elementlink" << endmsg;
        }
        
        float phiTrk1    = ptl1->phi();
        float z0Trk1     = ptl1->z0();
        float d0Trk1     = ptl1->d0();
        float qOverPTrk1 = ptl1->qOverP();
        float ptTrk1     = ptl1->pt();
        float etaTrk1    = ptl1->eta();
        
        uint8_t tmpValue(0);
        int bLayerHitsTrk1 = ptl1->summaryValue(tmpValue,xAOD::numberOfInnermostPixelLayerHits) ?  tmpValue : -99;
        int sctHitsTrk1    = ptl1->summaryValue(tmpValue,xAOD::numberOfSCTHits)    ?  tmpValue : -99;
        int pixHitsTrk1    = ptl1->summaryValue(tmpValue,xAOD::numberOfPixelHits)  ?  tmpValue : -99;
        int trtHitsTrk1    = ptl1->summaryValue(tmpValue,xAOD::numberOfTRTHits)    ?  tmpValue : -99;
        
        
        ATH_MSG_DEBUG("track " << itrk << " pt phi eta " << ptTrk1 << " " <<
                      phiTrk1 << " " << etaTrk1 << "SCT hits: " << sctHitsTrk1);
        
        
        itrk++; trkIt++;
        
        if (!(trkIt->isValid())) {
            ATH_MSG_WARNING("TrackParticleContainer::Invalid ElementLink to track ");
            continue;
        }
        
        const xAOD::TrackParticle * ptl2 = **trkIt;
        if (!ptl2) {
            msg() << MSG::WARNING << "TrackParticle with nullptr but valid elementlink" << endmsg;
        }
        
        float phiTrk2    = ptl2->phi();
        float z0Trk2     = ptl2->z0();
        float d0Trk2     = ptl2->d0();
        float ptTrk2     = ptl2->pt();
        float etaTrk2    = ptl2->eta();
        
        tmpValue = 0;
        int bLayerHitsTrk2 = ptl2->summaryValue(tmpValue,xAOD::numberOfInnermostPixelLayerHits) ?  tmpValue : -99;
        int sctHitsTrk2    = ptl2->summaryValue(tmpValue,xAOD::numberOfSCTHits)    ?  tmpValue : -99;
        int pixHitsTrk2    = ptl2->summaryValue(tmpValue,xAOD::numberOfPixelHits)  ?  tmpValue : -99;
        int trtHitsTrk2    = ptl2->summaryValue(tmpValue,xAOD::numberOfTRTHits)    ?  tmpValue : -99;
        
        
        
        float dphi = phiTrk1 - phiTrk2;
        
        
        float absdphi = fabs(dphi);
        
        if( 2.* TMath::Pi() -absdphi < absdphi )
        {
            if( dphi > 0 )
            {
                dphi = dphi - 2.* TMath::Pi();
            }
            else
            {
                dphi = 2* TMath::Pi() - absdphi;
            }
        }
        
        dphi=fabs(dphi);
        
        float pTsum = ptTrk1 + ptTrk2;
        float deta = etaTrk1 - etaTrk2;
        float dR = deltaR(etaTrk1, phiTrk1, etaTrk2, phiTrk2);
        
        
        TLorentzVector trk1_tl, trk2_tl;
        trk1_tl.SetPtEtaPhiM(ptTrk1, etaTrk1, phiTrk1, m_muonMass);
        trk2_tl.SetPtEtaPhiM(ptTrk2, etaTrk2, phiTrk2, m_muonMass);
        
        
        int qTrk1(1);//, qTrk2(-1);
        
        if(qOverPTrk1 < 0.){
            qTrk1 = -1;
            //qTrk2 = 1;
        }
        
        float phiStar      = phiMethod(trk1_tl, trk2_tl, qTrk1);
        float cosThetaStar = cosMethod(trk1_tl, trk2_tl, qTrk1);
        
        hist(Form("%s_%s_pTmu1",prefix.c_str(),trigItem.c_str()))->Fill(ptTrk1);
        hist(Form("%s_%s_pTmu2",prefix.c_str(),trigItem.c_str()))->Fill(ptTrk2);
        hist(Form("%s_%s_d0mu1",prefix.c_str(),trigItem.c_str()))->Fill(d0Trk1);
        hist(Form("%s_%s_d0mu2",prefix.c_str(),trigItem.c_str()))->Fill(d0Trk2);
        hist(Form("%s_%s_z0mu1",prefix.c_str(),trigItem.c_str()))->Fill(z0Trk1);
        hist(Form("%s_%s_z0mu2",prefix.c_str(),trigItem.c_str()))->Fill(z0Trk2);
        hist(Form("%s_%s_dphi",prefix.c_str(),trigItem.c_str()))->Fill(dphi);
        hist(Form("%s_%s_deta",prefix.c_str(),trigItem.c_str()))->Fill(deta);
        hist(Form("%s_%s_dR",prefix.c_str(),trigItem.c_str()))->Fill(dR);
        hist(Form("%s_%s_pTsum",prefix.c_str(),trigItem.c_str()))->Fill(pTsum);
        hist(Form("%s_%s_phiStar",prefix.c_str(),trigItem.c_str()))->Fill(phiStar);
        hist(Form("%s_%s_cosThetaStar",prefix.c_str(),trigItem.c_str()))->Fill(cosThetaStar);
        
        hist(Form("%s_%s_numBLayerHitsmu1",prefix.c_str(),trigItem.c_str()))->Fill(bLayerHitsTrk1);
        hist(Form("%s_%s_numBLayerHitsmu2",prefix.c_str(),trigItem.c_str()))->Fill(bLayerHitsTrk2);
        hist(Form("%s_%s_numSCTHitsmu1",prefix.c_str(),trigItem.c_str()))->Fill(sctHitsTrk1);
        hist(Form("%s_%s_numSCTHitsmu2",prefix.c_str(),trigItem.c_str()))->Fill(sctHitsTrk2);
        hist(Form("%s_%s_numPixelHitsmu1",prefix.c_str(),trigItem.c_str()))->Fill(pixHitsTrk1);
        hist(Form("%s_%s_numPixelHitsmu2",prefix.c_str(),trigItem.c_str()))->Fill(pixHitsTrk2);
        hist(Form("%s_%s_numTRTHitsmu1",prefix.c_str(),trigItem.c_str()))->Fill(trtHitsTrk1);
        hist(Form("%s_%s_numTRTHitsmu2",prefix.c_str(),trigItem.c_str()))->Fill(trtHitsTrk2);
        
        
        hist2(Form("%s_%s_phiStarcosThetaStar",prefix.c_str(),trigItem.c_str()))->Fill(cosThetaStar, phiStar,1);
        
        //ATH_MSG_DEBUG("track " << itrk << " pt phi eta " << ptTrk2 << " " << phiTrk2 << " " << etaTrk2);
        
    }
    
    
    
    
}


void HLTXAODBphysMonTool::fillHistEff(const xAOD::TrigBphys *bphysItem, const std::string & /*trigItem*/,const std::string & /*prefix*/, int /*DenomOrNum*/, const std::string & trigEffHistName) // suppress 'unused' compiler warning
//-----------------------------------------------------------
{
    
    ATH_MSG_DEBUG(":: FillHistEff() Test 1 :: ");
    
        //float eta = bphysItem->eta();
        //float phi = bphysItem->phi();
    float mass = bphysItem->mass();
        //float fitmass = bphysItem->fitmass();
        //float fitchi2 = bphysItem->fitchi2();
    
    ATH_MSG_DEBUG("mass :: "<< mass);
    
    
    const std::vector<ElementLink<xAOD::TrackParticleContainer> > trackVector = bphysItem->trackParticleLinks();
    if (trackVector.size() != 0) {
        ATH_MSG_DEBUG(" REGTEST got track vector size: " << trackVector.size() );
    } else {
        ATH_MSG_DEBUG(" REGTEST no track vector!!! " );
    }
    
    
    std::vector<ElementLink<xAOD::TrackParticleContainer> >::const_iterator trkIt=trackVector.begin();
    for (int itrk=0 ; trkIt!= trackVector.end(); ++itrk, ++trkIt) {
        if (!(trkIt->isValid())) {
            ATH_MSG_WARNING("TrackParticleContainer::Invalid ElementLink to track ");
            continue;
        }
        
        const xAOD::TrackParticle * ptl1 = **trkIt;
        if (!ptl1) {
            msg() << MSG::WARNING << "TrackParticle with nullptr but valid elementlink" << endmsg;
        }
        
        float phiTrk1    = ptl1->phi();
        //        float thetaTrk1  = ptl1->p4().Theta();
        //        float z0Trk1     = ptl1->z0();
        //        float d0Trk1     = ptl1->d0();
        //float qOverPTrk1 = ptl1->qOverP();
        //        float pxTrk1     = ptl1->p4().Px();
        //        float pyTrk1     = ptl1->p4().Py();
        float ptTrk1     = ptl1->pt();
        float etaTrk1    = ptl1->eta();
        
        uint8_t tmpValue(0);
        //        int bLayerHitsTrk1 = ptl1->summaryValue(tmpValue,xAOD::numberOfInnermostPixelLayerHits) ?  tmpValue : -99;
        int sctHitsTrk1    = ptl1->summaryValue(tmpValue,xAOD::numberOfSCTHits)    ?  tmpValue : -99;
        //        int pixHitsTrk1    = ptl1->summaryValue(tmpValue,xAOD::numberOfPixelHits)  ?  tmpValue : -99;
        //        int trtHitsTrk1    = ptl1->summaryValue(tmpValue,xAOD::numberOfTRTHits)    ?  tmpValue : -99;
        
        
        ATH_MSG_DEBUG("track " << itrk << " pt phi eta " << ptTrk1 << " " <<
                      phiTrk1 << " " << etaTrk1 << "SCT hits: " << sctHitsTrk1);
        
        
        itrk++; trkIt++;
        
        if (!(trkIt->isValid())) {
            ATH_MSG_WARNING("TrackParticleContainer::Invalid ElementLink to track ");
            continue;
        }
        
        const xAOD::TrackParticle * ptl2 = **trkIt;
        if (!ptl2) {
            msg() << MSG::WARNING << "TrackParticle with nullptr but valid elementlink" << endmsg;
        }
        
        float phiTrk2    = ptl2->phi();
        //float z0Trk2     = ptl2->z0();
        //float d0Trk2     = ptl2->d0();
        //float qOverPTrk2 = ptl2->qOverP();
        float ptTrk2     = ptl2->pt();
        float etaTrk2    = ptl2->eta();
        
        //tmpValue = 0;
        //        int bLayerHitsTrk2 = ptl2->summaryValue(tmpValue,xAOD::numberOfInnermostPixelLayerHits) ?  tmpValue : -99;
        //        int sctHitsTrk2    = ptl2->summaryValue(tmpValue,xAOD::numberOfSCTHits)    ?  tmpValue : -99;
        //        int pixHitsTrk2    = ptl2->summaryValue(tmpValue,xAOD::numberOfPixelHits)  ?  tmpValue : -99;
        //        int trtHitsTrk2    = ptl2->summaryValue(tmpValue,xAOD::numberOfTRTHits)    ?  tmpValue : -99;
        
        float dphi = phiTrk1 - phiTrk2;
        
        float absdphi = fabs(dphi);
        
        if( 2.* TMath::Pi() -absdphi < absdphi )
        {
            if( dphi > 0 )
            {
                dphi = dphi - 2.* TMath::Pi();
            }
            else
            {
                dphi = 2* TMath::Pi() - absdphi;
            }
        }
        
        dphi=fabs(dphi);
        
        //float pTsum = ptTrk1 + ptTrk2;
        //float deta = etaTrk1 - etaTrk2;
        //float dR = deltaR(etaTrk1, phiTrk1, etaTrk2, phiTrk2);
        
        TLorentzVector trk1_tl, trk2_tl;
        trk1_tl.SetPtEtaPhiM(ptTrk1, etaTrk1, phiTrk1, m_muonMass);
        trk2_tl.SetPtEtaPhiM(ptTrk2, etaTrk2, phiTrk2, m_muonMass);
        
        //int qTrk1(1);, qTrk2(-1);
        
        //        if(qOverPTrk1 < 0.){
        //            qTrk1 = -1;
        //            qTrk2 = 1;
        //        }
        
            //float phiStar = phiMethod(trk1_tl, trk2_tl, qTrk1);
            //float cosThetaStar = cosMethod(trk1_tl, trk2_tl, qTrk1);
        
        
        string Eff_etaName1 = trigEffHistName + "_eta1";
        string Eff_etaName2 = trigEffHistName + "_eta2";
        string Eff_phiName1 = trigEffHistName + "_phi1";
        string Eff_phiName2 = trigEffHistName + "_phi2";
        string Eff_pTName1 = trigEffHistName + "_pT1";
        string Eff_pTName2 = trigEffHistName + "_pT2";
        
        hist(Eff_etaName1)->Fill(etaTrk1);
        hist(Eff_etaName2)->Fill(etaTrk2);
        hist(Eff_phiName1)->Fill(phiTrk1);
        hist(Eff_phiName2)->Fill(phiTrk2);
        hist(Eff_pTName1)->Fill(ptTrk1);
        hist(Eff_pTName2)->Fill(ptTrk1);
        
        
        //ATH_MSG_DEBUG("track " << itrk << " pt phi eta " << ptTrk2 << " " << phiTrk2 << " " << etaTrk2);
        
    }
    
    
    
    
}


//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::fill()
//-----------------------------------------------------------
{
    ATH_MSG_DEBUG ("Filling... ");
    
    *m_log << MSG::DEBUG << " ====== Begin fillHists() ====== " << endmsg;
    
    // Loop over a vector of B-physics trigger containers and dump to histograms
    for (auto containerItem : m_containerList){
        if( fillContainer(containerItem).isFailure() ) *m_log << MSG::ERROR << "Method test failed " << endmsg;
    }
    
    
    // Loop over the given trigger names and perform test and hist filling
    //    for (int iTrigName = 0; iTrigName < (int)m_bphysItems.size(); ++iTrigName){
    //        if ( fillTriggerChain(m_bphysItems[iTrigName], m_monGroup[iTrigName]).isFailure() ) {
    //            ATH_MSG_WARNING ("Could not finish fillTriggerChain test for chain " << m_bphysItems[iTrigName]);
    //        }
    //    }
    
    
    
    if( JpsiFinder().isFailure() )  ATH_MSG_WARNING ("Method JpsiFinder failed ");
    
    if( fillTriggerEfficiency().isFailure() )  ATH_MSG_WARNING ("Method fillTriggerEfficiency failed ");
    
    
    
    return StatusCode::SUCCESS;
}


//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::fillContainer(const std::string& containerItem)
//-----------------------------------------------------------
{
    ATH_MSG_DEBUG ("Testing... ");
    
    
    setCurrentMonGroup("HLT/BphysMon/Container/"+containerItem);
    
    const xAOD::TrigBphysContainer*  trigBphysContainer(nullptr);
    StatusCode sc = m_storeGate->retrieve(trigBphysContainer,containerItem); //"HLT_xAOD__TrigBphysContainer_EFBMuMuFex"
    
    if (sc.isFailure() || ! trigBphysContainer) {
        ATH_MSG_DEBUG ("  No TrigBphysContainer found with tag: "<< containerItem);
        return StatusCode::SUCCESS;
    }
    
    const std::string sEF("EF");
    const std::string sL2("L2");
    
    // from the container name, determine the level
    xAOD::TrigBphys::levelType container_level(xAOD::TrigBphys::UNKOWNLEVEL);
    if (containerItem.find(sEF) != std::string::npos) {
        ATH_MSG_VERBOSE ("Container of type EF: "<< containerItem);
        container_level = xAOD::TrigBphys::EF;
    } else if (containerItem.find(sL2) != std::string::npos) {
        ATH_MSG_VERBOSE ("Container of type L2: "<< containerItem);
        container_level = xAOD::TrigBphys::L2;
    } else {
        ATH_MSG_INFO ("Container of type unknown: "<< containerItem);
    }
    
    // loop over the items, checking the consistency of the level and fill the hists
    for ( auto bphys:  (*trigBphysContainer))  {
        if (container_level != bphys->level()) {
            ATH_MSG_INFO ("Container level mismatch: "<< containerItem << " : " << bphys->level());
        }
        fillHist(bphys,containerItem,"TrigBphys");
    }
    
    return StatusCode::SUCCESS;
}


//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::fillTriggerChain(const std::string& trigItem, const std::string& monGroup)
//-----------------------------------------------------------
{
    msg(MSG::DEBUG) << "REGTEST ==========START of Bphysics EDM/Navigation check for chain " << trigItem << " ===========" << endmsg;
    
    //setCurrentMonGroup("HLT/BphysMon/Trigger");
    setCurrentMonGroup("HLT/BphysMon/Trigger/"+monGroup+"/"+trigItem);
    
    ATH_MSG_DEBUG("Chain passed = " << m_trigDec->isPassed(trigItem));
    
    Trig::FeatureContainer fc = m_trigDec->features(trigItem);
    
    const std::vector< Trig::Feature<xAOD::TrigBphysContainer> > fc_bphys = fc.get<xAOD::TrigBphysContainer>();
    ATH_MSG_DEBUG("Size of vector< Trig::Feature<xAOD::TrigBphysContainer> > = " << fc_bphys.size());
    
    for( auto cont_bphys : fc_bphys ) {
        ATH_MSG_DEBUG("REGTEST Got Bphysics container, size = " << cont_bphys.cptr()->size());
        for ( auto bphys:  *(cont_bphys.cptr()) )  {
            
            if ( bphys->level() == xAOD::TrigBphys::L2) {
                fillHist(bphys,trigItem,"TrigBphys_L2");
            } else if (bphys->level() == xAOD::TrigBphys::EF) {
                fillHist(bphys,trigItem,"TrigBphys_EF");
            } else {
                ATH_MSG_WARNING("REGTEST Bphysics object has no defined level");

            }
            
            
            
        }
    }
    
    
    msg(MSG::DEBUG) << "REGTEST ==========END of Bphysics EDM/Navigation check for chain " << trigItem << " ===========" << endmsg;
    
    
    
    
    
    return StatusCode::SUCCESS;
}


//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::fillTriggerEfficiency()
//-----------------------------------------------------------
{
    
    //setCurrentMonGroup("HLT/BphysMon/Trigger");
    setCurrentMonGroup("HLT/BphysMon/TriggerEfficiency");
    
    int EffSize(0);
    string TrigEffHistName;
    string TrigEffName;
    
    for (int DenomOrNum = 0; DenomOrNum < 2; ++DenomOrNum){
        EffSize = (int)m_effDenom.size();
        
        
        for (int iTrigEff = 0; iTrigEff < EffSize; ++iTrigEff){
            if (DenomOrNum == 0) {
                TrigEffHistName = "EffDenom_" + m_effDenom[iTrigEff];
                TrigEffName = m_effDenom[iTrigEff];
            } else {
                TrigEffHistName = "EffNum_" + m_effNum[iTrigEff];
                TrigEffName = m_effNum[iTrigEff];
            }
            
            ATH_MSG_DEBUG("Chain passed = " << m_trigDec->isPassed(TrigEffName));
            
            //ensure the denominator trigger is passed before filling numerator
            if(DenomOrNum==1){
                bool passDenom = false;
                passDenom = m_trigDec->isPassed(m_effDenom[iTrigEff]);
                if(passDenom != true) continue;
                
            }
            
            
            Trig::FeatureContainer fc = m_trigDec->features(TrigEffName);
            
            const std::vector< Trig::Feature<xAOD::TrigBphysContainer> > fc_bphys = fc.get<xAOD::TrigBphysContainer>();
            ATH_MSG_DEBUG("Size of vector< Trig::Feature<xAOD::TrigBphysContainer> > = " << fc_bphys.size());
            
            for( auto cont_bphys : fc_bphys ) {
                ATH_MSG_DEBUG("REGTEST Got Bphysics container, size = " << cont_bphys.cptr()->size());
                for ( auto bphys:  *(cont_bphys.cptr()) )  {
                    
                    // When L2 containers are migrated to xAOD; need to be able to differentiate based ont pType method
                    fillHistEff(bphys,TrigEffName,"TrigBphys", DenomOrNum, TrigEffHistName);
                    
                    /*
                     string Eff_etaName1 = TrigEffHistName + "_eta1";
                     string Eff_etaName2 = TrigEffHistName + "_eta2";
                     string Eff_phiName1 = TrigEffHistName + "_phi1";
                     string Eff_phiName2 = TrigEffHistName + "_phi2";
                     string Eff_pTName1 = TrigEffHistName + "_pT1";
                     string Eff_pTName2 = TrigEffHistName + "_pT2";
                     
                     
                     hist(Eff_etaName1)->Fill(0.1);
                     hist(Eff_etaName2)->Fill(0.2);
                     hist(Eff_phiName1)->Fill(0.3);
                     hist(Eff_phiName2)->Fill(0.4);
                     hist(Eff_pTName1)->Fill(5.);
                     hist(Eff_pTName2)->Fill(6.);
                     */
                    
                }
            }
        }
    }
    // msg(MSG::DEBUG) << "REGTEST ==========END of Bphysics EDM/Navigation check for chain " << trigItem << " ===========" << endmsg;
    
    
    
    
    
    return StatusCode::SUCCESS;
}







//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::proc()
//-----------------------------------------------------------
{
    
    
    setCurrentMonGroup("HLT/BphysMon/JpsiFinder");
    
    divide2(hist2("TrigBphys_Jpsi_mass_central"),hist2("denom_TrigBphys_Jpsi_mass_central"),hist2("eff_TrigBphys_Jpsi_mass_central") );
    divide2(hist2("TrigBphys_Jpsi_high_pT_central"),hist2("denom_TrigBphys_Jpsi_high_pT_central"),hist2("eff_TrigBphys_Jpsi_high_pT_central") );
    divide2(hist2("TrigBphys_Jpsi_low_pT_central"),hist2("denom_TrigBphys_Jpsi_low_pT_central"),hist2("eff_TrigBphys_Jpsi_low_pT_central") );
    divide2(hist2("TrigBphys_Jpsi_dphi_central"),hist2("denom_TrigBphys_Jpsi_dphi_central"),hist2("eff_TrigBphys_Jpsi_dphi_central") );
    divide2(hist2("TrigBphys_Jpsi_deta_central"),hist2("denom_TrigBphys_Jpsi_deta_central"),hist2("eff_TrigBphys_Jpsi_deta_central") );
    divide2(hist2("TrigBphys_Jpsi_cosThetaStar_central"),hist2("denom_TrigBphys_Jpsi_cosThetaStar_central"),hist2("eff_TrigBphys_Jpsi_cosThetaStar_central") );
    divide2(hist2("TrigBphys_Jpsi_phiStar_central"),hist2("denom_TrigBphys_Jpsi_phiStar_central"),hist2("eff_TrigBphys_Jpsi_phiStar_central") );
    
    divide2(hist2("TrigBphys_Jpsi_mass_forward"),hist2("denom_TrigBphys_Jpsi_mass_forward"),hist2("eff_TrigBphys_Jpsi_mass_forward") );
    divide2(hist2("TrigBphys_Jpsi_high_pT_forward"),hist2("denom_TrigBphys_Jpsi_high_pT_forward"),hist2("eff_TrigBphys_Jpsi_high_pT_forward") );
    divide2(hist2("TrigBphys_Jpsi_low_pT_forward"),hist2("denom_TrigBphys_Jpsi_low_pT_forward"),hist2("eff_TrigBphys_Jpsi_low_pT_forward") );
    divide2(hist2("TrigBphys_Jpsi_dphi_forward"),hist2("denom_TrigBphys_Jpsi_dphi_forward"),hist2("eff_TrigBphys_Jpsi_dphi_forward") );
    divide2(hist2("TrigBphys_Jpsi_deta_forward"),hist2("denom_TrigBphys_Jpsi_deta_forward"),hist2("eff_TrigBphys_Jpsi_deta_forward") );
    divide2(hist2("TrigBphys_Jpsi_cosThetaStar_forward"),hist2("denom_TrigBphys_Jpsi_cosThetaStar_forward"),hist2("eff_TrigBphys_Jpsi_cosThetaStar_forward") );
    divide2(hist2("TrigBphys_Jpsi_phiStar_forward"),hist2("denom_TrigBphys_Jpsi_phiStar_forward"),hist2("eff_TrigBphys_Jpsi_phiStar_forward") );
    
    divide2(hist2("TrigBphys_Upsi_mass_central"),hist2("denom_TrigBphys_Upsi_mass_central"),hist2("eff_TrigBphys_Upsi_mass_central") );
    divide2(hist2("TrigBphys_Upsi_high_pT_central"),hist2("denom_TrigBphys_Upsi_high_pT_central"),hist2("eff_TrigBphys_Upsi_high_pT_central") );
    divide2(hist2("TrigBphys_Upsi_low_pT_central"),hist2("denom_TrigBphys_Upsi_low_pT_central"),hist2("eff_TrigBphys_Upsi_low_pT_central") );
    divide2(hist2("TrigBphys_Upsi_dphi_central"),hist2("denom_TrigBphys_Upsi_dphi_central"),hist2("eff_TrigBphys_Upsi_dphi_central") );
    divide2(hist2("TrigBphys_Upsi_deta_central"),hist2("denom_TrigBphys_Upsi_deta_central"),hist2("eff_TrigBphys_Upsi_deta_central") );
    divide2(hist2("TrigBphys_Upsi_cosThetaStar_central"),hist2("denom_TrigBphys_Upsi_cosThetaStar_central"),hist2("eff_TrigBphys_Upsi_cosThetaStar_central") );
    divide2(hist2("TrigBphys_Upsi_phiStar_central"),hist2("denom_TrigBphys_Upsi_phiStar_central"),hist2("eff_TrigBphys_Upsi_phiStar_central") );
    
    divide2(hist2("TrigBphys_Upsi_mass_forward"),hist2("denom_TrigBphys_Upsi_mass_forward"),hist2("eff_TrigBphys_Upsi_mass_forward") );
    divide2(hist2("TrigBphys_Upsi_high_pT_forward"),hist2("denom_TrigBphys_Upsi_high_pT_forward"),hist2("eff_TrigBphys_Upsi_high_pT_forward") );
    divide2(hist2("TrigBphys_Upsi_low_pT_forward"),hist2("denom_TrigBphys_Upsi_low_pT_forward"),hist2("eff_TrigBphys_Upsi_low_pT_forward") );
    divide2(hist2("TrigBphys_Upsi_dphi_forward"),hist2("denom_TrigBphys_Upsi_dphi_forward"),hist2("eff_TrigBphys_Upsi_dphi_forward") );
    divide2(hist2("TrigBphys_Upsi_deta_forward"),hist2("denom_TrigBphys_Upsi_deta_forward"),hist2("eff_TrigBphys_Upsi_deta_forward") );
    divide2(hist2("TrigBphys_Upsi_cosThetaStar_forward"),hist2("denom_TrigBphys_Upsi_cosThetaStar_forward"),hist2("eff_TrigBphys_Upsi_cosThetaStar_forward") );
    divide2(hist2("TrigBphys_Upsi_phiStar_forward"),hist2("denom_TrigBphys_Upsi_phiStar_forward"),hist2("eff_TrigBphys_Upsi_phiStar_forward") );
    
    
    
    setCurrentMonGroup("HLT/BphysMon/TriggerEfficiency");
    
    // Creating histograms for efficiency
    for (int iEff = 0; iEff < (int)m_effDenom.size(); ++iEff){
        
        string EffDenom_etaHist1 = "EffDenom_" + m_effDenom[iEff] + "_eta1";
        string EffDenom_etaHist2 = "EffDenom_" + m_effDenom[iEff] + "_eta2";
        string EffDenom_phiHist1 = "EffDenom_" + m_effDenom[iEff] + "_phi1";
        string EffDenom_phiHist2 = "EffDenom_" + m_effDenom[iEff] + "_phi2";
        string EffDenom_pTHist1 = "EffDenom_" + m_effDenom[iEff] + "_pT1";
        string EffDenom_pTHist2 = "EffDenom_" + m_effDenom[iEff] + "_pT2";
        
        string EffNum_etaHist1 = "EffNum_" + m_effNum[iEff] + "_eta1";
        string EffNum_etaHist2 = "EffNum_" + m_effNum[iEff] + "_eta2";
        string EffNum_phiHist1 = "EffNum_" + m_effNum[iEff] + "_phi1";
        string EffNum_phiHist2 = "EffNum_" + m_effNum[iEff] + "_phi2";
        string EffNum_pTHist1 = "EffNum_" + m_effNum[iEff] + "_pT1";
        string EffNum_pTHist2 = "EffNum_" + m_effNum[iEff] + "_pT2";
        
        string Eff_etaHist1 = "Eff_" + m_effNum[iEff] + "__" + m_effDenom[iEff] + "_eta1";
        string Eff_etaHist2 = "Eff_" + m_effNum[iEff] + "__" + m_effDenom[iEff] + "_eta2";
        string Eff_phiHist1 = "Eff_" + m_effNum[iEff] + "__" + m_effDenom[iEff] + "_phi1";
        string Eff_phiHist2 = "Eff_" + m_effNum[iEff] + "__" + m_effDenom[iEff] + "_phi2";
        string Eff_pTHist1 = "Eff_" + m_effNum[iEff] + "__" + m_effDenom[iEff] + "_pT1";
        string Eff_pTHist2 = "Eff_" + m_effNum[iEff] + "__" + m_effDenom[iEff] + "_pT2";
        
        
        ATH_MSG_DEBUG("line ::: "<< __LINE__);
        divide(hist(EffNum_etaHist1), hist(EffDenom_etaHist1), hist(Eff_etaHist1));
        divide(hist(EffNum_etaHist2), hist(EffDenom_etaHist2), hist(Eff_etaHist2));
        divide(hist(EffNum_phiHist1), hist(EffDenom_phiHist1), hist(Eff_phiHist1));
        divide(hist(EffNum_phiHist2), hist(EffDenom_phiHist2), hist(Eff_phiHist2));
        divide(hist(EffNum_pTHist1), hist(EffDenom_pTHist1), hist(Eff_pTHist1));
        divide(hist(EffNum_pTHist2), hist(EffDenom_pTHist2), hist(Eff_pTHist2));
        
        
        
    }
    
    EffHistMap.clear();
    
    
    
    //    JW not sure this would work as intended.
    //    const int effNumSize = m_effNum.size();
    //    const int effDenomSize = m_effDenom.size();
    //    //    TH1F *h_Eff_num[effNumSize];
    //    //    TH1F *h_Eff_denom[effDenomSize];
    //    std::vector<TH1F*> h_Eff_num  (m_effNum.size()  ,nullptr);
    //    std::vector<TH1F*> h_Eff_denom(m_effDenom.size(),nullptr);
    //
    //    if (m_effNum.size() != m_effDenom.size()) {
    //        ATH_MSG_DEBUG("ERROR: mismatched vector sizes - what were you trying to do?")
    //        StatusCode::FAILURE;
    //    }
    //
    //    for (int iTrigName = 0; iTrigName < (int)m_effNum.size(); ++iTrigName){
    //        setCurrentMonGroup("HLT/BphysMon/Trigger/"+m_effNumGroup[iTrigName]+"/"+m_effNum[iTrigName]);
    //        h_Eff_num[iTrigName]=(TH1F*)hist(Form("TrigBphys_%s_mass",m_effNum[iTrigName].c_str()))->Clone();
    //
    //        setCurrentMonGroup("HLT/BphysMon/Trigger/"+m_effDenomGroup[iTrigName]+"/"+m_effDenom[iTrigName]);
    //        h_Eff_denom[iTrigName]=(TH1F*)hist(Form("TrigBphys_%s_mass",m_effDenom[iTrigName].c_str()))->Clone();
    //
    //    }
    //
    //    setCurrentMonGroup("HLT/BphysMon/TriggerEfficiency");
    //
    //    for (int iTrigName = 0; iTrigName < (int)m_effNum.size(); ++iTrigName){
    //
    //        divide(hist(Form("TrigBphys_%s_eta",m_effNum[iTrigName].c_str())),hist(Form("TrigBphys_%s_eta",m_effDenom[iTrigName].c_str())),hist(Form("Eff_%s_%s_eta",m_effNum[iTrigName].c_str(),m_effDenom[iTrigName].c_str() ) ));
    //
    //        divide(h_Eff_num[iTrigName],h_Eff_denom[iTrigName],hist(Form("Eff_%s_%s_mass",m_effNum[iTrigName].c_str(),m_effDenom[iTrigName].c_str() ) ));
    //    }
    
    
    
    return StatusCode::SUCCESS;
}

//---------------------------------------------------------
StatusCode HLTXAODBphysMonTool::JpsiFinder()
//---------------------------------------------------------
{
    setCurrentMonGroup("HLT/BphysMon/JpsiFinder");
    ATH_MSG_DEBUG("in JpsiFinder()");
    const xAOD::VertexContainer*    jpsiContainer(nullptr);
    
    if (evtStore()->retrieve(jpsiContainer   , m_JpsiCandidatesKey).isFailure() || !jpsiContainer) {
        ATH_MSG_WARNING("No Jpsi Container Found, skipping jpsiFinder method");
        return StatusCode::RECOVERABLE;
    }
    ATH_MSG_DEBUG("number of jpsi candidates " << jpsiContainer->size());
    
    // Extract information from the Jpsi candidates
    for ( xAOD::VertexContainer::const_iterator vxcItr = jpsiContainer->begin() ; vxcItr != jpsiContainer->end() ; vxcItr++ ) {
        const xAOD::Vertex* jpsiCandidate = (*vxcItr);
        if (!jpsiCandidate) continue;
        // to see how to access variables; refer to:
        // https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/JpsiUpsilonTools/trunk/src/JpsiExample.cxx
        
        
        // refitted track parameters
        TLorentzVector refTrk1 = track4Momentum(jpsiCandidate, 0, m_muonMass);
        TLorentzVector refTrk2 = track4Momentum(jpsiCandidate, 1, m_muonMass);
        
        TLorentzVector origTrk1 = origTrack4Momentum(jpsiCandidate, 0, m_muonMass);
        TLorentzVector origTrk2 = origTrack4Momentum(jpsiCandidate, 1, m_muonMass);
        
        TLorentzVector ref_onia  = refTrk1 + refTrk2;
        TLorentzVector orig_onia = origTrk1 + origTrk2;
        
        // Calculate the invariant masses and their errors
        double orig_mass = orig_onia.M();                      // mass from original tracks
        double mass      = ref_onia.M();                        // mass from refitted tracks
            // double pt      = ref_onia.Pt();                        // pt from refitted tracks
        double error     = invariantMassError(jpsiCandidate, std::vector<double>(2, m_muonMass));  // invariant mass error
        double rapidity  = ref_onia.Rapidity();
        
        float phiTrk1 = refTrk1.Phi();
        float thetaTrk1 = refTrk1.Theta();
        //float z0Trk1 = 0.;
        //float d0Trk1 = 0.;
        //float qOverPTrk1 = 0.;
        float pxTrk1 = refTrk1.Px();
        float pyTrk1 = refTrk1.Py();
        float ptTrk1 = sqrt(pxTrk1*pxTrk1 + pyTrk1*pyTrk1);
        float etaTrk1 = -std::log(tan(thetaTrk1/2));
        
        
        //      msg() << MSG::VERBOSE << "track, iterator, pointer " << itrk << " " << *trkIt << " " << *(*trkIt) << endmsg;
        float phiTrk2 = refTrk2.Phi();
        float thetaTrk2 = refTrk2.Theta();
        //float z0Trk2 = 0.;
        //float d0Trk2 = 0.;
        //float qOverPTrk2 = 0.;
        float pxTrk2 = refTrk2.Px();
        float pyTrk2 = refTrk2.Py();
        float ptTrk2 = sqrt(pxTrk2*pxTrk2 + pyTrk2*pyTrk2);
        float etaTrk2 = -std::log(tan(thetaTrk2/2));
        
        float dphi = phiTrk1 - phiTrk2;
        
        
        float absdphi = fabs(dphi);
        
        if( 2.* TMath::Pi() -absdphi < absdphi )
        {
            if( dphi > 0 )
            {
                dphi = dphi - 2.* TMath::Pi();
            }
            else
            {
                dphi = 2* TMath::Pi() - absdphi;
            }
        }
        
        dphi=fabs(dphi);
        
        //float pTsum = ptTrk1 + ptTrk2;
        float deta = etaTrk1 - etaTrk2;
        //float dR = deltaR(etaTrk1, phiTrk1, etaTrk2, phiTrk2);
        
        int m_trkOrigCharge1=trackCharge(jpsiCandidate, 0);
        //int m_trkOrigCharge2=trackCharge(jpsiCandidate, 1);
        
        int qTrk1(m_trkOrigCharge1);//, qTrk2(m_trkOrigCharge2);
        
        
        
        float phiStar = phiMethod(refTrk1, refTrk2, qTrk1);
        float cosThetaStar = cosMethod(refTrk1, refTrk2, qTrk1);
        
        
        
        
        
        ATH_MSG_DEBUG("JpsiFinder: Original mass: " << orig_mass << ", refitted mass: " << mass << ", error: " <<
                      error);
        
        hist(Form("offline_mumu_mass"))->Fill(mass);
        hist(Form("offline_mumu_mass_diff"))->Fill(orig_mass - mass);
        
        
        
        for (int iTrigName = 0; iTrigName < (int)m_bphysItems.size(); ++iTrigName){
            ATH_MSG_DEBUG("JpsiFinder in trigger loop!!");
            const string chainEF = m_bphysItems[iTrigName];
            bool passEF = getTDT()->isPassed(chainEF);
            
            ///First fill denominator histograms for efficiencies (no trigger requirements)
            // Central region
            if (fabs(rapidity) < 1.05){
                
                if(mass > m_mw_jpsi_central_min && mass < m_mw_jpsi_central_max){
                    hist2("denom_TrigBphys_Jpsi_mass_central")->Fill(mass, chainEF.c_str(), 1);
                    hist2("denom_TrigBphys_Jpsi_high_pT_central")->Fill(ptTrk1/1000., chainEF.c_str(), 1);
                    hist2("denom_TrigBphys_Jpsi_low_pT_central")->Fill(ptTrk2/1000., chainEF.c_str(), 1);
                    hist2("denom_TrigBphys_Jpsi_dphi_central")->Fill(dphi, chainEF.c_str(), 1);
                    hist2("denom_TrigBphys_Jpsi_deta_central")->Fill(deta, chainEF.c_str(), 1);
                    hist2("denom_TrigBphys_Jpsi_cosThetaStar_central")->Fill(cosThetaStar, chainEF.c_str(), 1);
                    hist2("denom_TrigBphys_Jpsi_phiStar_central")->Fill(phiStar, chainEF.c_str(), 1);
                    
                }
                if(mass > m_mw_upsi_central_min && mass < m_mw_upsi_central_max){
                    hist2("denom_TrigBphys_Upsi_mass_central")->Fill(mass, chainEF.c_str(), 1);
                    hist2("denom_TrigBphys_Upsi_high_pT_central")->Fill(ptTrk1/1000., chainEF.c_str(), 1);
                    hist2("denom_TrigBphys_Upsi_low_pT_central")->Fill(ptTrk2/1000., chainEF.c_str(), 1);
                    hist2("denom_TrigBphys_Upsi_dphi_central")->Fill(dphi, chainEF.c_str(), 1);
                    hist2("denom_TrigBphys_Upsi_deta_central")->Fill(deta, chainEF.c_str(), 1);
                    hist2("denom_TrigBphys_Upsi_cosThetaStar_central")->Fill(cosThetaStar, chainEF.c_str(), 1);
                    hist2("denom_TrigBphys_Upsi_phiStar_central")->Fill(phiStar, chainEF.c_str(), 1);
                    
                }
            }
            
            
            // Forward region
            if (fabs(rapidity) > 1.05){
                
                if(mass > m_mw_jpsi_forward_min && mass < m_mw_jpsi_forward_max){
                    hist2("denom_TrigBphys_Jpsi_mass_forward")->Fill(mass, chainEF.c_str(), 1);
                    hist2("denom_TrigBphys_Jpsi_high_pT_forward")->Fill(ptTrk1/1000., chainEF.c_str(), 1);
                    hist2("denom_TrigBphys_Jpsi_low_pT_forward")->Fill(ptTrk2/1000., chainEF.c_str(), 1);
                    hist2("denom_TrigBphys_Jpsi_dphi_forward")->Fill(dphi, chainEF.c_str(), 1);
                    hist2("denom_TrigBphys_Jpsi_deta_forward")->Fill(deta, chainEF.c_str(), 1);
                    hist2("denom_TrigBphys_Jpsi_cosThetaStar_forward")->Fill(cosThetaStar, chainEF.c_str(), 1);
                    hist2("denom_TrigBphys_Jpsi_phiStar_forward")->Fill(phiStar, chainEF.c_str(), 1);
                    
                }
                if(mass > m_mw_upsi_forward_min && mass < m_mw_upsi_forward_max){
                    hist2("denom_TrigBphys_Upsi_mass_forward")->Fill(mass, chainEF.c_str(), 1);
                    hist2("denom_TrigBphys_Upsi_high_pT_forward")->Fill(ptTrk1/1000., chainEF.c_str(), 1);
                    hist2("denom_TrigBphys_Upsi_low_pT_forward")->Fill(ptTrk2/1000., chainEF.c_str(), 1);
                    hist2("denom_TrigBphys_Upsi_dphi_forward")->Fill(dphi, chainEF.c_str(), 1);
                    hist2("denom_TrigBphys_Upsi_deta_forward")->Fill(deta, chainEF.c_str(), 1);
                    hist2("denom_TrigBphys_Upsi_cosThetaStar_forward")->Fill(cosThetaStar, chainEF.c_str(), 1);
                    hist2("denom_TrigBphys_Upsi_phiStar_forward")->Fill(phiStar, chainEF.c_str(), 1);
                }
            }
            
            
            
            if (passEF) {
                // Central region
                if (fabs(rapidity) < 1.05){
                    
                    if(mass > m_mw_jpsi_central_min && mass < m_mw_jpsi_central_max){
                        hist2("TrigBphys_Jpsi_mass_central")->Fill(mass, chainEF.c_str(), 1);
                        hist2("TrigBphys_Jpsi_high_pT_central")->Fill(ptTrk1/1000., chainEF.c_str(), 1);
                        hist2("TrigBphys_Jpsi_low_pT_central")->Fill(ptTrk2/1000., chainEF.c_str(), 1);
                        hist2("TrigBphys_Jpsi_dphi_central")->Fill(dphi, chainEF.c_str(), 1);
                        hist2("TrigBphys_Jpsi_deta_central")->Fill(deta, chainEF.c_str(), 1);
                        hist2("TrigBphys_Jpsi_cosThetaStar_central")->Fill(cosThetaStar, chainEF.c_str(), 1);
                        hist2("TrigBphys_Jpsi_phiStar_central")->Fill(phiStar, chainEF.c_str(), 1);
                        
                        
                    }
                    if(mass > m_mw_upsi_central_min && mass < m_mw_upsi_central_max){
                        hist2("TrigBphys_Upsi_mass_central")->Fill(mass, chainEF.c_str(), 1);
                        hist2("TrigBphys_Upsi_high_pT_central")->Fill(ptTrk1/1000., chainEF.c_str(), 1);
                        hist2("TrigBphys_Upsi_low_pT_central")->Fill(ptTrk2/1000., chainEF.c_str(), 1);
                        hist2("TrigBphys_Upsi_dphi_central")->Fill(dphi, chainEF.c_str(), 1);
                        hist2("TrigBphys_Upsi_deta_central")->Fill(deta, chainEF.c_str(), 1);
                        hist2("TrigBphys_Upsi_cosThetaStar_central")->Fill(cosThetaStar, chainEF.c_str(), 1);
                        hist2("TrigBphys_Upsi_phiStar_central")->Fill(phiStar, chainEF.c_str(), 1);
                        
                    }
                }
                
                
                // Forward region
                if (fabs(rapidity) > 1.05){
                    
                    if(mass > m_mw_jpsi_forward_min && mass < m_mw_jpsi_forward_max){
                        hist2("TrigBphys_Jpsi_mass_forward")->Fill(mass, chainEF.c_str(), 1);
                        hist2("TrigBphys_Jpsi_high_pT_forward")->Fill(ptTrk1/1000., chainEF.c_str(), 1);
                        hist2("TrigBphys_Jpsi_low_pT_forward")->Fill(ptTrk2/1000., chainEF.c_str(), 1);
                        hist2("TrigBphys_Jpsi_dphi_forward")->Fill(dphi, chainEF.c_str(), 1);
                        hist2("TrigBphys_Jpsi_deta_forward")->Fill(deta, chainEF.c_str(), 1);
                        hist2("TrigBphys_Jpsi_cosThetaStar_forward")->Fill(cosThetaStar, chainEF.c_str(), 1);
                        hist2("TrigBphys_Jpsi_phiStar_forward")->Fill(phiStar, chainEF.c_str(), 1);
                        
                    }
                    if(mass > m_mw_upsi_forward_min && mass < m_mw_upsi_forward_max){
                        hist2("TrigBphys_Upsi_mass_forward")->Fill(mass, chainEF.c_str(), 1);
                        hist2("TrigBphys_Upsi_high_pT_forward")->Fill(ptTrk1/1000., chainEF.c_str(), 1);
                        hist2("TrigBphys_Upsi_low_pT_forward")->Fill(ptTrk2/1000., chainEF.c_str(), 1);
                        hist2("TrigBphys_Upsi_dphi_forward")->Fill(dphi, chainEF.c_str(), 1);
                        hist2("TrigBphys_Upsi_deta_forward")->Fill(deta, chainEF.c_str(), 1);
                        hist2("TrigBphys_Upsi_cosThetaStar_forward")->Fill(cosThetaStar, chainEF.c_str(), 1);
                        hist2("TrigBphys_Upsi_phiStar_forward")->Fill(phiStar, chainEF.c_str(), 1);
                        
                    }
                }
                
                ATH_MSG_DEBUG("JpsiFinder passed EF for chain: " << m_bphysItems[iTrigName] << "!!");
            }
        }
        
        
        
        
    } // loop over candidates
    
    //
    return StatusCode::SUCCESS;
    
    
}



//---------------------------------------------------------
StatusCode HLTXAODBphysMonTool::Triggers()
//---------------------------------------------------------
{
    
    // For each trigger in the list, get the L2 and EF items, check if passed one or both.
    // Obtain the corresponding features and build efficienct plots and
    //
    setCurrentMonGroup("HLT/BphysMon/Trigger");

    for (const auto& triggerChain : m_bphysItems) {
        std::string baseName = triggerChain.substr(4); // remove "HLT_" from the name
        
        const auto cg   = getTDT()->getChainGroup(triggerChain);
        const auto cgL2 = getTDT()->getChainGroup("L2_"+baseName);
        const auto cgEF = getTDT()->getChainGroup("EF_"+baseName);

        bool isPassed   (cg-> isPassed(TrigDefs::Physics));
        bool isPassedL2(cgL2->isPassed(TrigDefs::Physics));
        bool isPassedEF(cgEF->isPassed(TrigDefs::Physics));

        ATH_MSG_DEBUG("Triger passed(HLT/EF/L2): " << isPassed << isPassedL2 << isPassedEF << " " << triggerChain);

        
        // get HLT items
        const auto& fc       = getTDT()->features(triggerChain);
        const auto& fc_bphys = fc.get<xAOD::TrigBphysContainer>();

        
        
        // get EF items
        
        // get L2 items
        
        // get L1 Roi (will this work for L1Topo?)
        
        // get Muons / tracking
        
        
    } // loop over trigger names
    
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::L2DsPhiPiMon()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::L2TopoDiMuMon()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::L2TopoDiMuXMon()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::L2DiMuMon()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::L2DiMuXMon()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::L2BJpsieeMon()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::EFDsPhiPiMon()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::EFTopoDiMuMon()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::EFDiMuMon()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::L2Tracks()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::EFTracks()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::L2BJpsimumu()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::EFBJpsimumu()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::CompareDiMuL2EF()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}


//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::CompareTopoDiMuL2EF()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}

void HLTXAODBphysMonTool::divide(TH1 *num, TH1 *den, TH1 *quo) {
    
    quo->Divide(num, den, 1., 1., "B");
}

void HLTXAODBphysMonTool::divide2(TH2 *num, TH2 *den, TH2 *quo) {
    
    quo->Divide(num, den, 1., 1., "B");
}



// *********************************************************************************
// ---------------------------------------------------------------------------------
// trackMomentum: returns refitted track momentum
// ---------------------------------------------------------------------------------

TVector3 HLTXAODBphysMonTool::trackMomentum(const xAOD::Vertex * vxCandidate, uint trkIndex) const
{
    float px(0.), py(0.), pz(0.);
    
    static SG::AuxElement::Accessor< std::vector<float> > refTrackPxAcc("RefTrackPx");
    static SG::AuxElement::Accessor< std::vector<float> > refTrackPyAcc("RefTrackPy");
    static SG::AuxElement::Accessor< std::vector<float> > refTrackPzAcc("RefTrackPz");
    const std::vector<float>& refTrackPx = refTrackPxAcc(*vxCandidate);
    const std::vector<float>& refTrackPy = refTrackPyAcc(*vxCandidate);
    const std::vector<float>& refTrackPz = refTrackPzAcc(*vxCandidate);
    
    if(trkIndex < refTrackPx.size() && refTrackPx.size() == refTrackPy.size() && refTrackPx.size() == refTrackPz.size()) {
        px = refTrackPx[trkIndex];
        py = refTrackPy[trkIndex];
        pz = refTrackPz[trkIndex];
    } else {
        ATH_MSG_WARNING("Size inconsistencies in trackMomentum");
    }
    
    TVector3 mom(px,py,pz);
    return mom;
}

// ---------------------------------------------------------------------------------
// track4Momentum: returns refitted track 4-momentum given mass hypothesis
// ---------------------------------------------------------------------------------

TLorentzVector HLTXAODBphysMonTool::track4Momentum(const xAOD::Vertex * vxCandidate, int trkIndex, double mass) const
{
    TVector3 mom = trackMomentum(vxCandidate, trkIndex);
    TLorentzVector lorentz;
    lorentz.SetVectM(mom, mass);
    return lorentz;
}


// ---------------------------------------------------------------------------------
// origTrackMomentum: returns original track momentum
// ---------------------------------------------------------------------------------

TVector3 HLTXAODBphysMonTool::origTrackMomentum(const xAOD::Vertex * vxCandidate, int trkIndex) const
{
    TVector3 mom;
    const xAOD::TrackParticle* origTrack = vxCandidate->trackParticle(trkIndex);
    if(origTrack==NULL) {
        ATH_MSG_DEBUG("origTrackMomentum unable to get original track " << vxCandidate << " " << trkIndex);
        return mom;
    }
    
    mom.SetPtEtaPhi(origTrack->pt(), origTrack->eta(), origTrack->phi());
    return mom;
}

// ---------------------------------------------------------------------------------
// origTrack4Momentum: returns original track 4-momentum given mass hypothesis
// ---------------------------------------------------------------------------------

TLorentzVector HLTXAODBphysMonTool::origTrack4Momentum(const xAOD::Vertex * vxCandidate, int trkIndex, double mass) const
{
    TVector3 mom = origTrackMomentum(vxCandidate, trkIndex);
    TLorentzVector lorentz;
    lorentz.SetVectM(mom, mass);
    return lorentz;
}

// ---------------------------------------------------------------------------------
// invariantMassError: returns invariant mass error
// ---------------------------------------------------------------------------------

double HLTXAODBphysMonTool::invariantMassError(const xAOD::Vertex* vxCandidate, std::vector<double> masses) const
{
    uint NTrk = vxCandidate->vxTrackAtVertex().size();
    if (masses.size() != NTrk) {
        ATH_MSG_WARNING("The provided number of masses does not match the number of tracks in the vertex");
        return -999999.;
    }
    
    uint ndim    = vxCandidate->covariance().size();
    uint ndimExp = (3*NTrk+3)*(3*NTrk+3+1)/2;
    if (ndim == ndimExp) {
        return massErrorVKalVrt(vxCandidate,masses);
    } else {
        ATH_MSG_WARNING("Unknown covariance matrix dimension: " << ndim << ", expected: " << ndimExp);
        return -999999.;
    }
}

// ---------------------------------------------------------------------------------
// massErrorVKalVrt: returns invariant mass error for vertex created by VKalVrtFitter
// ---------------------------------------------------------------------------------

double HLTXAODBphysMonTool::massErrorVKalVrt(const xAOD::Vertex * vxCandidate, std::vector<double> masses) const
{
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    
    // construct the full covariance matrix
    Amg::MatrixX* fullCov = convertVKalCovMatrix(NTrk, vxCandidate->covariance());
    
    std::vector<CLHEP::HepLorentzVector> particleMom(NTrk);
    std::vector<CLHEP::HepMatrix> particleDeriv(NTrk);
    CLHEP::HepLorentzVector totalMom;
    CLHEP::HepMatrix tmpDeriv(3,3);
    
    double phi=0.,theta=0.,invP=0.;
    for( unsigned int it=0; it<NTrk; it++){
        const Trk::TrackParameters*  bPer = vxCandidate->vxTrackAtVertex()[it].perigeeAtVertex();
        phi   =  bPer->parameters()[Trk::phi];
        theta =  bPer->parameters()[Trk::theta];
        invP  =  bPer->parameters()[Trk::qOverP];
        CLHEP::HepLorentzVector tmp( cos(phi)*sin(theta)/fabs(invP),
                                    sin(phi)*sin(theta)/fabs(invP),
                                    cos(theta)/fabs(invP) );
        double esq = tmp.px()*tmp.px() + tmp.py()*tmp.py() + tmp.pz()*tmp.pz() + masses[it]*masses[it];
        double e = (esq>0.) ? sqrt(esq) : 0.;
        tmp.setE(e);
        particleMom[it] = tmp;
        totalMom += tmp;
        
        //  d(Px,Py,Pz)/d(Phi,Theta,InvP)
        tmpDeriv(1,1) = - tmp.py();
        tmpDeriv(2,1) =   tmp.px();
        tmpDeriv(3,1) =   0.;
        tmpDeriv(1,2) =   cos(phi) * tmp.pz();
        tmpDeriv(2,2) =   sin(phi) * tmp.pz();
        tmpDeriv(3,2) = - sin(theta)/fabs(invP);
        tmpDeriv(1,3) = - tmp.px()/invP;
        tmpDeriv(2,3) = - tmp.py()/invP;
        tmpDeriv(3,3) = - tmp.pz()/invP;
        particleDeriv[it] = tmpDeriv;
    }
    
    double dMdPx=0., dMdPy=0., dMdPz=0., dMdPhi=0., dMdTheta=0., dMdInvP=0.;
    std::vector<double> Deriv(3*NTrk+3, 0.);
    for(unsigned int it=0; it<NTrk; it++){
        dMdPx = ( totalMom.e() * particleMom[it].px()/particleMom[it].e() - totalMom.px() ) / totalMom.m();
        dMdPy = ( totalMom.e() * particleMom[it].py()/particleMom[it].e() - totalMom.py() ) / totalMom.m();
        dMdPz = ( totalMom.e() * particleMom[it].pz()/particleMom[it].e() - totalMom.pz() ) / totalMom.m();
        
        dMdPhi   = dMdPx*particleDeriv[it](1,1) + dMdPy*particleDeriv[it](2,1) + dMdPz*particleDeriv[it](3,1);
        dMdTheta = dMdPx*particleDeriv[it](1,2) + dMdPy*particleDeriv[it](2,2) + dMdPz*particleDeriv[it](3,2);
        dMdInvP  = dMdPx*particleDeriv[it](1,3) + dMdPy*particleDeriv[it](2,3) + dMdPz*particleDeriv[it](3,3);
        
        Deriv[3*it + 3 + 0] = dMdPhi;    Deriv[3*it + 3 + 1] = dMdTheta; Deriv[3*it + 3 + 2] = dMdInvP;
    }
    
    double err = 0;
    for(unsigned int i=0; i<3*NTrk+3; i++){
        for(unsigned int j=0; j<3*NTrk+3; j++){
            err += Deriv[i]*( (*fullCov)(i,j))*Deriv[j];
        }
    }
    
    // cleanup
    delete fullCov;
    
    return (err>0.) ? sqrt(err) : 0.;
}

// ---------------------------------------------------------------------------------
// trackCharge: returns charge of the i-th track
// ---------------------------------------------------------------------------------

double HLTXAODBphysMonTool::trackCharge(const xAOD::Vertex * vxCandidate, int i) const
{
    if(vxCandidate!=NULL && vxCandidate->trackParticle(i)!=NULL) {
        return vxCandidate->trackParticle(i)->charge();
    }
    
    return -999999;
}

// ---------------------------------------------------------------------------------
// convertVKalCovMatrix: converts xAOD covariance matrix (i.e. std::vector<float>)
//                       into an actual matrix
// ---------------------------------------------------------------------------------

Amg::MatrixX * HLTXAODBphysMonTool::convertVKalCovMatrix(int NTrk, const std::vector<float> & Matrix) const
{
    // stolen from VKalVrtFitter
    Amg::MatrixX * mtx = new Amg::MatrixX(3+3*NTrk,3+3*NTrk);
    long int ij=0;
    for(int i=1; i<=(3+3*NTrk); i++){
        for(int j=1; j<=i; j++){
            if(i==j) {
                (*mtx)(i-1,j-1)=Matrix[ij];
            }else{
                (*mtx).fillSymmetric(i-1,j-1,Matrix[ij]);
            }
            ij++;
        }
    }
    
    return mtx;
}

float HLTXAODBphysMonTool::phiMethod(const TLorentzVector & Mu1, const TLorentzVector & Mu2, int Mu1_q) const
{
    
    TLorentzVector MuPlus  = ( Mu1_q > 0 ? Mu1 : Mu2);
    TLorentzVector MuMinus = ( Mu1_q > 0 ? Mu2 : Mu1);
    
    TLorentzVector MuMu = MuPlus + MuMinus;
    
    TVector3 V0 = MuMu.Vect();
    MuPlus.RotateZ(-V0.Phi());
    MuPlus.RotateY(-V0.Theta());
    if(MuMu.Rapidity() < 0) {MuPlus.RotateZ(-TMath::Pi());
        return -atan2(MuPlus.Y(),MuPlus.X());
    }
    else return atan2(MuPlus.Y(),MuPlus.X());
}


float HLTXAODBphysMonTool::cosMethod(const TLorentzVector & Mu1, const TLorentzVector & Mu2, int Mu1_q) const
{
    TLorentzVector MuPlus  = ( Mu1_q > 0 ? Mu1 : Mu2);
    TLorentzVector MuMinus = ( Mu1_q > 0 ? Mu2 : Mu1);
    
    TLorentzVector MuMu = MuPlus + MuMinus;
    
    double MuMu_mass = MuMu.M();
    double MuPlus_mass = MuPlus.M();
    double MuMinus_mass = MuMinus.M();
    double P_MuMu = MuMu.Rho();
    
    double pssq = ((MuMu_mass*MuMu_mass) - (MuPlus_mass+MuMinus_mass)*(MuPlus_mass+MuMinus_mass)) *  ((MuMu_mass*MuMu_mass) - (MuPlus_mass-MuMinus_mass)*(MuPlus_mass-MuMinus_mass));
    
    double ps = (pssq>0.) ? sqrt(pssq) : 0.;
    
    ps/=2.*MuMu_mass;
    
    double pp = MuMu.Px()*MuPlus.Px() + MuMu.Py()*MuPlus.Py() + MuMu.Pz()*MuPlus.Pz();
    
    float cosTheta = (float)((MuMu.Energy()*pp - P_MuMu*P_MuMu*MuPlus.Energy())/(MuMu_mass*ps*P_MuMu));
    
    
    return cosTheta;
    
}

float HLTXAODBphysMonTool::deltaR( float eta_1, float phi_1, float eta_2, float phi_2 ) const
{
    float dEta = fabs( eta_1 - eta_2 );
    float dPhi = fabs( phi_1 - phi_2 );
    if ( dPhi > TMath::Pi() ){ dPhi = 2*TMath::Pi() - dPhi ;}
    return sqrt( dEta*dEta + dPhi*dPhi ) ;
}






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
#include <boost/regex.hpp>

// Jpsi's
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackingPrimitives.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODMuon/Muon.h"

#include "xAODTrigMuon/L2StandAloneMuon.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuon.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"


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

namespace  { // variable binning
    const double ptmus[] = {0.,2.,4.,6.,8.,10.,12.,15.,20.,25.,30.,40.,50.,75.,100.};
    const int nptmus = (int) sizeof(ptmus) / sizeof(ptmus[0])  - 1;
    const double ptmumus[] = {0.,4.,8.,10.,12.,15.,20.,25.,30.,40.,50.,60.,80.,100.};
    const int nptmumus = (int) sizeof(ptmumus) / sizeof(ptmumus[0])  - 1;
    
    //const double taus[] = {-1,-0.3,-0.2,-0.1,0.,0.1,0.2,0.3,0.5,0.75,1.0,1.25,1.5,2.0,3.0,5.0,7.5,10,12.5,15,20};
    //const int ntaus = (int) sizeof(taus) / sizeof(taus[0])  - 1;

}

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
m_tdt("Trig::TrigDecisionTool/TrigDecisionTool"),
m_all(nullptr),
m_muonMass(105.66)
{
    declareProperty("muonMass"          , m_muonMass);
    declareProperty("JpsiCandidates"    , m_JpsiCandidatesKey = "JpsiCandidates","Offline di-muon candidates");

    declareProperty("BphysShifterPath"  , m_base_path_shifter="HLT/BphysMon/shifter","Shifter hist path");
    declareProperty("BphysExpertPath"   , m_base_path_expert ="HLT/BphysMon/expert" ,"Expert hist path");
    declareProperty("BphysHistPrefix"   , m_prefix ="TrigBphys" ,"Histogram prefix");

    
    declareProperty("monitoring_bphys"   , m_monitored_chains, "List of general monitoring chains");
    declareProperty("primary_bphys"      , m_primary_chains, "List of chains for detailed monitoring");
    declareProperty("GenerateChainDictsFromDB"      , m_GenerateChainDictsFromDB, "flag to overwrite the monitored catagory maps");
    declareProperty("ContainerList"     , m_containerList, "Container list");

    declareProperty("DetailedChains"    ,m_dedicated_chains , "table<string,string> of menu-idep/aware labels/chainnames");
    declareProperty("DetailedL1TopoChains"    ,m_dedicatedL1Topo_chains , "table<string,string> of menu-idep/aware labels/chainnames for L1Topo");
    declareProperty("EfficiencyChains"  ,m_efficiency_chains, "table<string,string> of menu-idep/aware labels/eff. names");
    
    // ID Tracking efficiency
    // MS efficieny
    // L1 & L2 efficieny
    // EF efficieny
    // noVtx_noOs efficiency
    // L1Topo efficiencies

    // efficiency triggers
    declareProperty("EffTrigDenom_ID"      , m_trigchain_denomID , "Trigger for ID denominator efficiencies");
    declareProperty("EffTrigDenom_MS"      , m_trigchain_denomMS , "Trigger for MS denominator efficiencies");
    declareProperty("EffTrigDenom_L1"      , m_trigchain_denomL1 , "Trigger for L1 denominator efficiencies");
    declareProperty("EffTrigDenom_L2"      , m_trigchain_denomL2 , "Trigger for L2 denominator efficiencies");
    declareProperty("EffTrigDenom_EF"      , m_trigchain_denomEF , "Trigger for EF denominator efficiencies");
    declareProperty("EffTrigDenom_noVtxOS" , m_trigchain_denomnoVtxOS , "Trigger for noVtxOS denominator efficiencies");
    declareProperty("EffTrigDenom_L1Topo"  , m_trigchain_denomL1Topo , "Trigger for L1Topo denominator efficiencies");
    
    // patterns for the above, if generated on-the-fly from m_primary_chains list
    declareProperty("DetailedChains_patterns"    ,m_dedicated_chains_patterns , "table<string,string> of menu-idep/aware labels/patterns");
    declareProperty("DetailedL1TopoChains_patterns"    ,m_dedicatedL1Topo_chains_patterns , "table<string,string> of menu-idep/aware labels/patterns for L1Topo");
    declareProperty("EfficiencyChains_patterns"  ,m_efficiency_chains_patterns, "table<string,string> of menu-idep/aware labels/eff. patterns");
    
    declareProperty("EffTrigDenom_noVtxOS_pattern" , m_trigchain_denomnoVtxOS_pattern , "Trigger pattern for noVtxOS denominator efficiencies");

    
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
    declareProperty("Trk_pt_min"                   , m_trk_pt_min           , "Minimum ID track pT");
    declareProperty("Trk_pt_max"                   , m_trk_pt_max           , "Maximum ID track pT");
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
    declareProperty("TauMass_min"                  , m_taumass_min          , "Minimum tau mass");
    declareProperty("TauMass_max"                  , m_taumass_max          , "Maximum tau mass");
    declareProperty("OniaPt_min"                   , m_oniapt_min           , "Minimum onia pt");
    declareProperty("OniaPt_max"                   , m_oniapt_max           , "Maximum onia pt");

    declareProperty("MassErr_min"                   , m_massErr_min         , "Minimum of Mass Error");
    declareProperty("MassErr_max"                   , m_massErr_max         , "Maximum of Mass Error");
    declareProperty("Lxy_min"                       , m_lxy_min             , "Minimum of Lxy Dist");
    declareProperty("Lxy_max"                       , m_lxy_max             , "Maximum of Lxy Dist");
    declareProperty("LxyErr_min"                    , m_lxyErr_min          , "Minimum of Lxy Error Dist");
    declareProperty("LxyErr_max"                    , m_lxyErr_max          , "Maximum of Lxy Error Dist");
    declareProperty("Tau_min"                       , m_tau_min             , "Minimum of Lifetime Dist");
    declareProperty("Tau_max"                       , m_tau_max             , "Maximum of Lifetime Dist");
    declareProperty("TauErr_min"                    , m_tauErr_min          , "Minimum of Lifetime Error Dist");
    declareProperty("TauErr_max"                    , m_tauErr_max          , "Maximum of Lifetime Error Dist");
    declareProperty("pT_min"                        , m_pt_min              , "Minimum of pT Dist");
    declareProperty("pT_max"                        , m_pt_max              , "Maximum of pT Dist");
    declareProperty("pTErr_min"                     , m_ptErr_min           , "Minimum of pT Error Dist");
    declareProperty("pTErr_max"                     , m_ptErr_max           , "Maximum of pT Error Dist");
    declareProperty("EffMatchThresh_Eta"            , m_EffMatchThresh_Eta  , "Eta threshold for matching in efficiencies");
    declareProperty("EffMatchThresh_phi"            , m_EffMatchThresh_phi  , "phi threshold for matching in efficiencies");
    declareProperty("EffMatchThresh_pT"             , m_EffMatchThresh_pT   , "pT threshold for matching in efficiencies");
    
} //HLTXAODBphysMonTool

//-----------------------------------------------------------
HLTXAODBphysMonTool::~HLTXAODBphysMonTool()
//-----------------------------------------------------------
{
    ATH_MSG_DEBUG ("~HLTXAODBphysMonTool ... ");
} // ~HLTXAODBphysMonTool

//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::init()
//-----------------------------------------------------------
{
    
    ATH_MSG_INFO ("Initializing... ");
    ATH_CHECK( m_tdt.retrieve());
    
    if(m_GenerateChainDictsFromDB)
      if(generateChainDicts().isFailure())
      {
        ATH_MSG_ERROR("Cannot generate map between monitoring categories and chains"); 
        return StatusCode::FAILURE;
      }
        
    
    m_all   = getTDT()->getChainGroup(".*");
    return StatusCode::SUCCESS;
}

StatusCode HLTXAODBphysMonTool::generateChainDicts() {
    m_dedicated_chains.clear();
    m_dedicatedL1Topo_chains.clear();
    m_efficiency_chains.clear();
    m_trigchain_denomnoVtxOS = "";
    
    ATH_MSG_INFO ("Forming DetailedChains... ");
    for(const auto& patternPair : m_dedicated_chains_patterns) {
        boost::regex pattern(patternPair.second.c_str());
        for(const auto& chainName : m_primary_chains) {
            if( boost::regex_match(chainName,pattern) ) {
              m_dedicated_chains.insert( Pair_t(patternPair.first,chainName) );
              ATH_MSG_INFO ("Add " << patternPair.first << " : " << chainName << " to DetailedChains dictionary");
              break;
            }
        }
    }
    
    ATH_MSG_DEBUG ("Forming DetailedL1TopoChains... ");
    for(const auto& patternPair : m_dedicatedL1Topo_chains_patterns) {
        boost::regex pattern(patternPair.second.c_str());
        for(const auto& chainName : m_primary_chains) {
            if( boost::regex_match(chainName,pattern) ) {
              m_dedicatedL1Topo_chains.insert( Pair_t(patternPair.first,chainName) );
              ATH_MSG_INFO ("Add " << patternPair.first << " : " << chainName << " to DetailedL1TopoChains dictionary");
              break;
            }
        }
    }
    
    ATH_MSG_INFO ("Forming EfficiencyChains... ");
    for(const auto& patternPair : m_efficiency_chains_patterns) {
        boost::regex pattern(patternPair.second.c_str());
        for(const auto& chainName : m_primary_chains) {
            if( boost::regex_match(chainName,pattern) ) {
              m_efficiency_chains.insert( Pair_t(patternPair.first,chainName) );
              ATH_MSG_INFO ("Add " << patternPair.first << " : " << chainName << " to EfficiencyChains dictionary");
              break;
            }
        }
    }
    
    ATH_MSG_DEBUG ("Choosing EffTrigDenom_noVtxOS_pattern... ");
    boost::regex patternEffTrigDenom_noVtxOS(m_trigchain_denomnoVtxOS_pattern);
    for(const auto& chainName : m_primary_chains) {
      if( boost::regex_match(chainName,patternEffTrigDenom_noVtxOS) ) {
        m_trigchain_denomnoVtxOS = chainName;
        ATH_MSG_INFO ("Set EffTrigDenom_noVtxOS = " << chainName);
      }
    }
    
    return StatusCode::SUCCESS;
}

//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::book()
//-----------------------------------------------------------
{
    ATH_MSG_DEBUG ("Booking... ");

    if (!newRunFlag()) {
        ATH_MSG_DEBUG ("Booking... not a new run, continuing");
        return StatusCode::SUCCESS;
    }
    
    // book the offline di-muon hists
    ATH_MSG_DEBUG ("Booking... JpsiFinder");
    if (bookJpsiFinder().isFailure()) {ATH_MSG_ERROR("Problems booking JpsiFinder method"); return StatusCode::FAILURE;}
    
    // book the general monitoring hists
    ATH_MSG_DEBUG ("Booking... Triggers");
    if (bookTriggers().isFailure())   {ATH_MSG_ERROR("Problems booking Triggers method"); return StatusCode::FAILURE;}
    
    // book the container hists
    ATH_MSG_DEBUG ("Booking... TriggerGroups");
    if (bookTriggerGroups().isFailure())   {ATH_MSG_ERROR("Problems booking Containers method"); return StatusCode::FAILURE;}
    
    // book the chain specific hists
    ATH_MSG_DEBUG ("Booking... Containers");
    if (bookContainers().isFailure())   {ATH_MSG_ERROR("Problems booking Containers method"); return StatusCode::FAILURE;}

    // book the efficiency hists
    ATH_MSG_DEBUG ("Booking... Efficiency");
    if (bookEfficiencyGroups().isFailure())   {ATH_MSG_ERROR("Problems booking Efficiency method"); return StatusCode::FAILURE;}
 
    // book the JpsiFinder efficiency hists                                                                                                        
    ATH_MSG_DEBUG ("Booking...JpsiFinderEfficiency");                                                                                                       
    if (bookJpsiFinderEfficiency().isFailure())   {ATH_MSG_ERROR("Problems booking JpsiFinder Efficiency method"); return StatusCode::FAILURE;}    
     
    return StatusCode::SUCCESS;
} // book

//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::fill()
//-----------------------------------------------------------
{
    ATH_MSG_DEBUG ("fill... ");
    
    // Check HLTResult
    if(getTDT()->ExperimentalAndExpertMethods()->isHLTTruncated()){
        ATH_MSG_WARNING("HLTResult truncated, skip event");
        return StatusCode::SUCCESS;
        //return false;
    }
    
    // fill the offline di-muon hists
    ATH_MSG_DEBUG ("fill... JpsiFinder");
    if (fillJpsiFinder().isFailure()) {ATH_MSG_WARNING("Problems filling JpsiFinder method");}
    
    // fill the general monitoring hists
    ATH_MSG_DEBUG ("fill... Triggers");
    if (fillTriggers().isFailure())   {ATH_MSG_WARNING("Problems filling Triggers method");}
    
    // fill the container hists
    ATH_MSG_DEBUG ("fill... Containers");
    if (fillContainers().isFailure())   {ATH_MSG_WARNING("Problems filling Containers method");}

    // fill the chain specific hists
    ATH_MSG_DEBUG ("fill... TriggerGroups");
    if (fillTriggerGroups().isFailure())   {ATH_MSG_WARNING("Problems filling TriggerGroups method");}

    // fill the efficiency hists
    ATH_MSG_DEBUG ("fill... Efficiency");
    if (fillEfficiencyGroups().isFailure())   {ATH_MSG_WARNING("Problems filling Efficiency method");}
    
    // fill the JpsiFinder efficiency hists
    ATH_MSG_DEBUG ("fill...JpsiFinder Efficiency");
    if (fillJpsiFinderEfficiency().isFailure())   {ATH_MSG_WARNING("Problems filling JpsiFinder Efficiency method");}

    return StatusCode::SUCCESS;
} // fill


StatusCode HLTXAODBphysMonTool::proc() {
    ATH_MSG_DEBUG ("proc... ");
    
    if (!endOfRunFlag())
    {
        ATH_MSG_DEBUG ("proc... Not end-of-run, returning");
        return StatusCode::SUCCESS;
    }
    
    return StatusCode::SUCCESS;
} // proc


StatusCode HLTXAODBphysMonTool::bookTriggers() {
    //Booking of histograms releated to overall trigger information
    
    // number of chains to monitor
    //    size_t nTriggers = m_monitored_chains.size();
    
    // *************** SHIFTER ****************** //
    addMonGroup(new MonGroup(this,m_base_path_shifter+"/Triggers",run));

    // *************** EXPERT ****************** //
    addMonGroup(new MonGroup(this,m_base_path_expert+"/Triggers",run));

    return StatusCode::SUCCESS;
} //bookTriggers


StatusCode HLTXAODBphysMonTool::fillTriggers(){
    //Booking of histograms releated to overall trigger information

//    setCurrentMonGroup(m_base_path_shifter+"/Triggers");
//    setCurrentMonGroup(m_base_path_expert+"/Triggers");

    for (const auto& chainName : m_monitored_chains) {
        std::string basename(stripHLT(chainName));
        
        const auto cg   = getTDT()->getChainGroup(chainName);
        const auto cgL2 = getTDT()->getChainGroup("L2_"+basename);
        const auto cgEF = getTDT()->getChainGroup("EF_"+basename);
        
        bool isPassed   (cg-> isPassed(TrigDefs::Physics));
        bool isPassedL2(cgL2->isPassed(TrigDefs::Physics));
        bool isPassedEF(cgEF->isPassed(TrigDefs::Physics));
        
        ATH_MSG_DEBUG("Triger passed(HLT/EF/L2): " << isPassed << isPassedL2 << isPassedEF << " " << chainName);
        ATH_MSG_VERBOSE("Triger passed(HLT/EF/L2): INT " << m_tdt->isPassed(chainName) << m_tdt->isPassed("L2_"+basename) << m_tdt->isPassed("EF_"+basename) << " " << chainName);
        // get variables and fill
    
        ATH_MSG_DEBUG( " Retrieving HLT details");
        // Getting the EF chain's features:
        const Trig::FeatureContainer fc = getTDT()->features(chainName);
        auto & fc_bphysEF   = fc.get<xAOD::TrigBphysContainer>("");
        auto & fc_bphysL2   = fc.get<xAOD::TrigBphysContainer>("L2BMuMuFex");
        auto & fc_muonsEF   = fc.get<xAOD::MuonContainer>();
        auto & fc_muonsL2SA = fc.get<xAOD::L2StandAloneMuonContainer>();
        auto & fc_muonsL2CB = fc.get<xAOD::L2CombinedMuonContainer>();

        ATH_MSG_DEBUG(  "Trigger(): number of L2 Bphys: " << fc_bphysL2.size() );
        for (auto bphys: fc_bphysL2){
            ATH_MSG_DEBUG(  "       number of L2 Bphys size: " << bphys.cptr()->size() );
            for (auto b: *bphys.cptr()) {
                ATH_MSG_DEBUG(  "\t\t\t:" << b->mass() << " " << b->fitmass() << " " << b->eta() << " " << b->phi() );
            }
        }
        ATH_MSG_DEBUG(  "Trigger(): number of EF Bphys: " << fc_bphysEF.size() );
        for (auto bphys: fc_bphysEF){
            ATH_MSG_DEBUG(  "       number of EF Bphys size: " << bphys.cptr()->size() );
            for (auto b: *bphys.cptr()) {
                ATH_MSG_DEBUG(  "\t\t\t:" << b->mass() << " " << b->fitmass() << " " << b->eta() << " " << b->phi() );
            }
        }
        ATH_MSG_DEBUG(  "Trigger(): number of EF Muons: " << fc_muonsEF.size() );
        for (auto muons: fc_muonsEF){
            ATH_MSG_DEBUG(  "       number of EF Muons size: " << muons.cptr()->size() );
            for (auto mu: *muons.cptr()) {
                ATH_MSG_DEBUG(  "\t\t\t:" << mu->pt() << " " << mu->eta() << " " << mu->phi() << " " << mu->charge() );
            }
        }
        ATH_MSG_DEBUG(  "Trigger(): number of L2SA Muons: " << fc_muonsL2SA.size() );
        for (auto muons: fc_muonsL2SA){
            ATH_MSG_DEBUG(  "       number of L2SA Muons size: " << muons.cptr()->size() );
            for (auto mu: *muons.cptr()) {
                ATH_MSG_DEBUG(  "\t\t\t:" << mu->pt() << " " << mu->eta() << " " << mu->phi()  );
            }
        }
        ATH_MSG_DEBUG(  "Trigger(): number of L2CB Muons: " << fc_muonsL2CB.size() );
        for (auto muons: fc_muonsL2CB){
            ATH_MSG_DEBUG(  "       number of L2CB Muons size: " << muons.cptr()->size() );
            for (auto mu: *muons.cptr()) {
                ATH_MSG_DEBUG(  "\t\t\t:" << mu->pt() << " " << mu->eta() << " " << mu->phi() << " " << mu->charge() );
            }
        }

        
        const std::vector<Trig::Combination>& combinations = fc.getCombinations();
        ATH_MSG_DEBUG(  "Trigger(): number of combinations: " << combinations.size() );

        std::vector<Trig::Combination>::const_iterator cit;
        
        for (cit = combinations.begin(); cit != combinations.end(); ++cit) { // Loop over combinations
            const Trig::Combination& combination = *cit;
            if (not cit->active()) {
                ATH_MSG_DEBUG( "Trigger(): combination " << *cit << " is not active" );
            } //if not active
            
            std::vector<Trig::Feature<xAOD::TrigBphys> > triggerFeatures = combination.get<xAOD::TrigBphys>();
            std::vector<Trig::Feature<xAOD::TrigBphys> >::const_iterator tfIt;
            ATH_MSG_DEBUG( "Trigger features: " << triggerFeatures.size()  );

            for (tfIt = triggerFeatures.begin(); tfIt != triggerFeatures.end(); ++tfIt) { // Loop over trigger features
                //retrieves EF info
                const xAOD::TrigBphys* trigObj = tfIt->cptr();
                ATH_MSG_DEBUG( "  features: " << trigObj->mass() << " " << trigObj->eta() << " " << trigObj->phi()  );
            } // loop over trigger (bphys) features
            
            std::vector<Trig::Feature<xAOD::Muon> > triggerFeaturesMuon = combination.get<xAOD::Muon>();
            std::vector<Trig::Feature<xAOD::Muon> >::const_iterator tfItmu;
            ATH_MSG_DEBUG( "Trigger features Muons: " << triggerFeaturesMuon.size()  );
            
            for (tfItmu = triggerFeaturesMuon.begin(); tfItmu != triggerFeaturesMuon.end(); ++tfItmu) { // Loop over trigger features
                                                                                          //retrieves EF info
                const xAOD::Muon* trigObj = tfItmu->cptr();
                ATH_MSG_DEBUG( "  features: " << trigObj->pt() << " " << trigObj->eta() << " " << trigObj->phi()  );
            } // loop over trigger (muons) features

            
        } // loop over combinations
    } // for loop over chain names
    
    return StatusCode::SUCCESS;
} // fillTriggers


StatusCode HLTXAODBphysMonTool::bookEfficiencyGroups(){
    // book the hists for the efficiency groups
    ATH_MSG_DEBUG("bookEfficiencyGroups");
    // ID Tracking efficiency
    // MS efficieny
    // L1 & L2 efficieny
    // EF efficieny
    // noVtx_noOs efficiency
    for (const auto& effpair: m_efficiency_chains) {
        if (bookEfficiencyGroup(Form("noVtx_noOS_%s_eff",effpair.first.c_str())).isFailure()) {
            ATH_MSG_ERROR("Problems in Eff. booking Eff " << effpair.first << " " << effpair.second);
            return StatusCode::FAILURE;
        }
    } // for
    
    // L1Topo efficiencies
    
    return StatusCode::SUCCESS;
}

StatusCode HLTXAODBphysMonTool::bookEfficiencyGroup(const std::string & groupName){  
    //book profile for the efficiency groups
    ATH_MSG_DEBUG("bookEfficiencyGroup: " << groupName);
    
    int nBins(40);
    
    TString prefix = m_prefix + "_" + groupName; // convert from std::string to TString
    
    if (groupName.find("BMuMuX")!=std::string::npos) nBins=20;     
    
    // *************** SHIFTER ****************** //
    addMonGroup(new MonGroup(this,m_base_path_shifter+"/Efficiency/"+groupName,run));
    ATH_MSG_DEBUG("bookEfficiencyGroup: Example hist name: " << prefix+"_eta");
    
    addProfile  ( new TProfile(prefix+"_eta", prefix+"_eta"   ,nBins,-3.,3.) );
    addProfile  ( new TProfile(prefix+"_phi", prefix+"_phi"   ,nBins, -TMath::Pi(), TMath::Pi()) );
    addProfile  ( new TProfile(prefix+"_pt",  prefix+"_pt"    ,nBins, 0,50) );
    
    addProfile  ( new TProfile(prefix+"_eta1", prefix+"_eta1", nBins, -3.,3.) );
    addProfile  ( new TProfile(prefix+"_phi1", prefix+"_phi1", nBins, -TMath::Pi(), TMath::Pi()) );
    addProfile  ( new TProfile(prefix+"_pt1",  prefix+"_pt1" , nBins, 0,50) );
    
    addProfile  ( new TProfile(prefix+"_eta2", prefix+"_eta2", nBins, -3.,3.) );
    addProfile  ( new TProfile(prefix+"_phi2", prefix+"_phi2", nBins, -TMath::Pi(), TMath::Pi()) );
    addProfile  ( new TProfile(prefix+"_pt2",  prefix+"_pt2" , nBins, 0,50) );
    
    
    // *************** EXPERT ****************** //
    addMonGroup(new MonGroup(this,m_base_path_expert+"/Efficiency/"+groupName,run));

    return StatusCode::SUCCESS;
}

StatusCode HLTXAODBphysMonTool::fillEfficiencyGroups() {
    // fill the hists for the efficiency groups
    ATH_MSG_DEBUG("fillEfficiencyGroups");

    // ID Tracking efficiency
    // MS efficieny
    // L1 & L2 efficieny
    // EF efficieny
    // noVtx_noOs efficiency
    m_efficiency_denomnoVtxOS_pairs.clear();
    
    const auto cg   = getTDT()->getChainGroup(m_trigchain_denomnoVtxOS);
    bool isPassed   (cg-> isPassed(TrigDefs::Physics)); 
    
    if (isPassed){
    
      const auto& fc       = getTDT()->features(m_trigchain_denomnoVtxOS);
      const auto& fc_bphys = fc.get<xAOD::TrigBphysContainer>();
      
      ATH_MSG_DEBUG("Size of vector: m_trigchain_denomnoVtxOS " << fc_bphys.size());
    
      for( auto cont_bphys : fc_bphys ) {
        ATH_MSG_DEBUG("REGTEST Got Bphysics container, size = " << cont_bphys.cptr()->size());
        for ( auto bphys:  *(cont_bphys.cptr()) )  {
             ATH_MSG_DEBUG("REGTEST Level = " << bphys->level());
             // ignore l2 objects
             if ((bphys->level() != xAOD::TrigBphys::EF) &&  (bphys->level() != xAOD::TrigBphys::HLT)) continue;
             
             const std::vector<ElementLink<xAOD::TrackParticleContainer> > trackVector = bphys->trackParticleLinks();
             if (trackVector.size() <2) {
                 ATH_MSG_DEBUG("REGTEST Unexpected number of tracks " << trackVector.size());
                 continue;
             }
           
             const xAOD::TrackParticle *trk1(nullptr),*trk2(nullptr);
             if (!trackVector.at(0).isValid() || (*trackVector.at(0) == nullptr)) {
                 ATH_MSG_DEBUG("REGTEST Track 1 is invalid");
                 continue;
             }
             if (!trackVector.at(1).isValid() || (*trackVector.at(1) == nullptr)) {
                 ATH_MSG_DEBUG("REGTEST Track 2 is invalid");
                 continue;
             }
             trk1 = *trackVector.at(0);
             trk2 = *trackVector.at(1);

             auto cur_pair_efficiency     = std::make_pair (trk1, trk2);
             auto cur_pair_efficiency_rev = std::make_pair (trk2, trk1);
             bool sawThisPair_rev=std::find(m_efficiency_denomnoVtxOS_pairs.begin(),m_efficiency_denomnoVtxOS_pairs.end(),cur_pair_efficiency_rev)!=m_efficiency_denomnoVtxOS_pairs.end();
             
             if(!sawThisPair_rev){
             m_efficiency_denomnoVtxOS_pairs.push_back(cur_pair_efficiency);
             }
        }
      }

   
        
      for (const auto& effpair: m_efficiency_chains) {
        if (fillEfficiencyGroup(Form("noVtx_noOS_%s_eff",effpair.first.c_str()),effpair.second).isFailure()) {
            ATH_MSG_ERROR("Problems in Eff. booking noVtx_noOS " << effpair.first << " " << effpair.second); 
            continue;
        }
      }
    } 
   
    
    // L1Topo efficiencies
    return StatusCode::SUCCESS;
}

StatusCode HLTXAODBphysMonTool::fillEfficiencyGroup(const std::string & groupName, const std::string & chainName){
     // helper method for dedicated Efficiency chains
     ATH_MSG_DEBUG("fillEfficiencyGroup: " << groupName << " " << chainName);
     TString prefix = m_prefix + "_" + groupName; // convert from std::string to TString
     

     const auto& fc       = getTDT()->features(chainName);
     const auto& fc_bphys   = fc.get<xAOD::TrigBphysContainer>();
     
     std::list<std::pair<const xAOD::TrackParticle *,const xAOD::TrackParticle *>>cur_pairs_list(m_efficiency_denomnoVtxOS_pairs);                           
                               
     ATH_MSG_DEBUG("Size of vector< Trig::Feature<xAOD::TrigBphysContainer> > = " << fc_bphys.size());
    
     for( auto cont_bphys : fc_bphys ) {
       ATH_MSG_DEBUG("REGTEST Got Bphysics container, size = " << cont_bphys.cptr()->size());
       for ( auto bphys:  *(cont_bphys.cptr()) )  {
            ATH_MSG_DEBUG("REGTEST Level = " << bphys->level());
            // ignore l2 objects
            if ((bphys->level() != xAOD::TrigBphys::EF) &&  (bphys->level() != xAOD::TrigBphys::HLT)) continue;
            
            const std::vector<ElementLink<xAOD::TrackParticleContainer> > trackVector = bphys->trackParticleLinks();
            if (trackVector.size() <2) {
                ATH_MSG_DEBUG("REGTEST Unexpected number of tracks " << trackVector.size());
                continue;
            }
          
            const xAOD::TrackParticle *trk1(nullptr),*trk2(nullptr);
            if (!trackVector.at(0).isValid() || (*trackVector.at(0) == nullptr)) {
                ATH_MSG_DEBUG("REGTEST Track 1 is invalid");
                continue;
            }
            if (!trackVector.at(1).isValid() || (*trackVector.at(1) == nullptr)) {
                ATH_MSG_DEBUG("REGTEST Track 2 is invalid");
                continue;
            }
            trk1 = *trackVector.at(0);
            trk2 = *trackVector.at(1);
            
            auto cur_pair    = std::make_pair (trk1, trk2);
            auto cur_pair_rev= std::make_pair (trk2, trk1);             
            bool sawThisPair      =std::find(cur_pairs_list.begin(),cur_pairs_list.end(),cur_pair    )!=cur_pairs_list.end();
            bool sawThisPair_rev  =std::find(cur_pairs_list.begin(),cur_pairs_list.end(),cur_pair_rev)!=cur_pairs_list.end();


            // refitted track parameters
            TLorentzVector origTrk1 = trk1->p4();
            TLorentzVector origTrk2 = trk2->p4();
            
            TLorentzVector orig_onia = origTrk1 + origTrk2;
            
            double eta     = orig_onia.Eta(); 
            double pt      = orig_onia.Pt();  
            double phi     = orig_onia.Phi(); 
            
            double phiTrk1 = origTrk1.Phi();
            double etaTrk1 = origTrk1.Eta();
            double ptTrk1  = origTrk1.Pt();
            
            double phiTrk2 = origTrk2.Phi();
            double etaTrk2 = origTrk2.Eta();
            double ptTrk2  = origTrk2.Pt();

            ATH_MSG_VERBOSE("Efficiency shifter hists " << m_base_path_shifter << " " << groupName);
            ATH_MSG_VERBOSE("Efficiency eta ");
            if ((sawThisPair) || (sawThisPair_rev)) {
               
               if(sawThisPair) { 
                 cur_pairs_list.erase(std::find(cur_pairs_list.begin(),cur_pairs_list.end(),cur_pair    )); 
               }           
               else  { 
                 cur_pairs_list.erase(std::find(cur_pairs_list.begin(),cur_pairs_list.end(),cur_pair_rev)); 
               }         
                // ******** SHIFTER ********* //
               setCurrentMonGroup(m_base_path_shifter+"/Efficiency/"+groupName);
               profile(Form("%s_eta",prefix.Data()))->Fill(eta,1.0,1.0);
               profile(Form("%s_phi",prefix.Data()))->Fill(phi,1.0,1.0);
               profile(Form("%s_pt", prefix.Data()))->Fill(pt*0.001,1.0,1.0);
               profile(Form("%s_eta1",prefix.Data()))->Fill(etaTrk1,1.0,1.0);
               profile(Form("%s_phi1",prefix.Data()))->Fill(phiTrk1,1.0,1.0);
               profile(Form("%s_pt1", prefix.Data()))->Fill(ptTrk1*0.001,1.0,1.0);
               profile(Form("%s_eta2",prefix.Data()))->Fill(etaTrk2,1.0,1.0);
               profile(Form("%s_phi2",prefix.Data()))->Fill(phiTrk2,1.0,1.0);
               profile(Form("%s_pt2", prefix.Data()))->Fill(ptTrk2*0.001,1.0,1.0);
             }
             // ******** EXPERT ********* //
             setCurrentMonGroup(m_base_path_expert+"/Efficiency/"+groupName);
             ATH_MSG_VERBOSE("Efficiency expert hists " << m_base_path_expert << " " << groupName);

        }  
     }
     for(auto It=cur_pairs_list.begin();It!=cur_pairs_list.end();It++) {
   
        auto cur_pair=*It;
        // refitted track parameters
        TLorentzVector origTrk1 = cur_pair.first ->p4();
        TLorentzVector origTrk2 = cur_pair.second->p4();
        
        TLorentzVector orig_onia = origTrk1 + origTrk2;
        
        double eta     = orig_onia.Eta();  
        double pt      = orig_onia.Pt();                      
        double phi     = orig_onia.Phi();                        
        
        double phiTrk1 = origTrk1.Phi();
        double etaTrk1 = origTrk1.Eta();
        double ptTrk1  = origTrk1.Pt();
        
        double phiTrk2 = origTrk2.Phi();
        double etaTrk2 = origTrk2.Eta();
        double ptTrk2  = origTrk2.Pt();
        // ******** SHIFTER ********* //
        ATH_MSG_VERBOSE("Efficiency shifter hists " << m_base_path_shifter << " " << groupName);
        setCurrentMonGroup(m_base_path_shifter+"/Efficiency/"+groupName);
        profile(Form("%s_eta",prefix.Data()))->Fill(eta,0.0,1.0);
        profile(Form("%s_phi",prefix.Data()))->Fill(phi,0.0,1.0);
        profile(Form("%s_pt", prefix.Data()))->Fill(pt*0.001,0.0,1.0);
        profile(Form("%s_eta1",prefix.Data()))->Fill(etaTrk1,0.0,1.0);
        profile(Form("%s_phi1",prefix.Data()))->Fill(phiTrk1,0.0,1.0);                     
        profile(Form("%s_pt1", prefix.Data()))->Fill(ptTrk1*0.001,0.0,1.0); 
        profile(Form("%s_eta2",prefix.Data()))->Fill(etaTrk2,0.0,1.0);       
        profile(Form("%s_phi2",prefix.Data()))->Fill(phiTrk2,0.0,1.0);      
        profile(Form("%s_pt2", prefix.Data()))->Fill(ptTrk2*0.001,0.0,1.0);             
         
                
        // ******** EXPERT ********* //
        setCurrentMonGroup(m_base_path_expert+"/Efficiency/"+groupName+"_effV2");
        ATH_MSG_VERBOSE("Efficiency expert hists " << m_base_path_expert << " " << groupName);
     }
     return StatusCode::SUCCESS;
}




StatusCode HLTXAODBphysMonTool::bookJpsiFinder() {
    // book the hists with offline comparisons
    
    int nTrigSize = (int) m_monitored_chains.size() +1; // plus one for the no-trigger offline entry
    Float_t nTrigSizeF = (Float_t)nTrigSize;
    
    TString prefix = m_prefix; // convert from std::string to TString
    
    // *************** SHIFTER ****************** //
    addMonGroup(new MonGroup(this,m_base_path_shifter+"/JpsiFinder",run));
    // central eta < |1.05| -- forward eta > |1.05|
    addHistogram( new TH2F(prefix+"_Jpsi_mass_central",         "Bphys Jpsi Mass Central"               , 50, 2950,3250, nTrigSize, 0., nTrigSizeF) );
    
    addHistogram( new TH2F(prefix+"_Jpsi_mass_forward",         "Bphys Jpsi Mass Forward"               , 50, 2800,3400, nTrigSize, 0., nTrigSizeF) );
    
    addHistogram( new TH2F(prefix+"_Upsi_mass_central",         "Bphys Upsi Mass Central"               , 100, 9000,11500,nTrigSize, 0., nTrigSizeF) );
    
    addHistogram( new TH2F(prefix+"_Upsi_mass_forward",         "Bphys Upsi Mass Forward"               , 100, 8000,12000,nTrigSize, 0., nTrigSizeF) );
    
    addHistogram( new TH2F(prefix+"_sumCharges",        "Bphys sum of track charges in offline di-muon"              , 3,-1.5,1.5,nTrigSize, 0., nTrigSizeF) );
    
    // A few 1d histograms for the chains speceified in m_dedicated_chains
    for (const auto& trigpair : m_dedicated_chains) {
        addMonGroup(new MonGroup(this,m_base_path_shifter+"/JpsiFinder/"+trigpair.first,run));
        addHistogram( new TH1F(prefix+"_Jpsi_mass_central_"+trigpair.first, prefix+"_Jpsi_mass_central_"+trigpair.second, 50, 2950,3250) );
        addHistogram( new TH1F(prefix+"_Jpsi_rap_central_"+trigpair.first, prefix+"_Jpsi_rap_central_"+trigpair.second, 50, -2.5,2.5) );
        addHistogram( new TH1F(prefix+"_Jpsi_dphi_central_"+trigpair.first, prefix+"_Jpsi_dphi_central_"+trigpair.second, 32,  0.,3.2) );
        addHistogram( new TH1F(prefix+"_Jpsi_deta_central_"+trigpair.first, prefix+"_Jpsi_deta_central_"+trigpair.second, 30, 0.,3.) );
        addHistogram( new TH1F(prefix+"_Jpsi_pt_central_"+trigpair.first, prefix+"_Jpsi_pt_central_"+trigpair.second, nptmumus,ptmumus) );
//         addHistogram( new TH1F(prefix+"_Jpsi_tau_bs_central_"+trigpair.first, prefix+"_Jpsi_tau_bs_central_"+trigpair.second, ntaus,taus) );
//         addHistogram( new TH1F(prefix+"_Jpsi_lxy_bs_central_"+trigpair.first, prefix+"_Jpsi_lxy_bs_central_"+trigpair.second, ntaus,taus) );
        addHistogram( new TH1F(prefix+"_Jpsi_tau_bs_central_"+trigpair.first, prefix+"_Jpsi_tau_bs_central_"+trigpair.second, 105,-1.,20.) );
        addHistogram( new TH1F(prefix+"_Jpsi_lxy_bs_central_"+trigpair.first, prefix+"_Jpsi_lxy_bs_central_"+trigpair.second, 105,-1.,20.) );
        
        addHistogram( new TH1F(prefix+"_Jpsi_mass_forward_"+trigpair.first, prefix+"_Jpsi_mass_forward_"+trigpair.second, 50, 2950,3250) );
        addHistogram( new TH1F(prefix+"_Jpsi_rap_forward_"+trigpair.first, prefix+"_Jpsi_rap_forward_"+trigpair.second, 50, -2.5,2.5) );
        addHistogram( new TH1F(prefix+"_Jpsi_dphi_forward_"+trigpair.first, prefix+"_Jpsi_dphi_forward_"+trigpair.second, 32,  0.,3.2) );
        addHistogram( new TH1F(prefix+"_Jpsi_deta_forward_"+trigpair.first, prefix+"_Jpsi_deta_forward_"+trigpair.second, 30, 0.,3.) );
        addHistogram( new TH1F(prefix+"_Jpsi_pt_forward_"+trigpair.first, prefix+"_Jpsi_pt_forward_"+trigpair.second, nptmumus,ptmumus) );
//         addHistogram( new TH1F(prefix+"_Jpsi_tau_bs_forward_"+trigpair.first, prefix+"_Jpsi_tau_bs_forward_"+trigpair.second, ntaus,taus) );
//         addHistogram( new TH1F(prefix+"_Jpsi_lxy_bs_forward_"+trigpair.first, prefix+"_Jpsi_lxy_bs_forward_"+trigpair.second, ntaus,taus) );
        addHistogram( new TH1F(prefix+"_Jpsi_tau_bs_forward_"+trigpair.first, prefix+"_Jpsi_tau_bs_forward_"+trigpair.second,  105,-1.,20.) );
        addHistogram( new TH1F(prefix+"_Jpsi_lxy_bs_forward_"+trigpair.first, prefix+"_Jpsi_lxy_bs_forward_"+trigpair.second,  105,-1.,20.) );
        
        addHistogram( new TH1F(prefix+"_Jpsi_mass_"+trigpair.first, prefix+"_Jpsi_mass_"+trigpair.second, 50, 2950,3250) );
        addHistogram( new TH1F(prefix+"_Jpsi_rap_"+trigpair.first, prefix+"_Jpsi_rap_"+trigpair.second, 50, -2.5,2.5) );
        addHistogram( new TH1F(prefix+"_Jpsi_dphi_"+trigpair.first, prefix+"_Jpsi_dphi_"+trigpair.second, 32,  0.,3.2) );
        addHistogram( new TH1F(prefix+"_Jpsi_deta_"+trigpair.first, prefix+"_Jpsi_deta_"+trigpair.second, 30, 0.,3.) );
        addHistogram( new TH1F(prefix+"_Jpsi_pt_"+trigpair.first, prefix+"_Jpsi_pt_"+trigpair.second, nptmumus,ptmumus) );
//         addHistogram( new TH1F(prefix+"_Jpsi_tau_bs_"+trigpair.first, prefix+"_Jpsi_tau_bs_"+trigpair.second, ntaus,taus) );
//         addHistogram( new TH1F(prefix+"_Jpsi_lxy_bs_"+trigpair.first, prefix+"_Jpsi_lxy_bs_"+trigpair.second, ntaus,taus) );
        addHistogram( new TH1F(prefix+"_Jpsi_tau_bs_"+trigpair.first, prefix+"_Jpsi_tau_bs_"+trigpair.second,  105,-1.,20.) );
        addHistogram( new TH1F(prefix+"_Jpsi_lxy_bs_"+trigpair.first, prefix+"_Jpsi_lxy_bs_"+trigpair.second,  105,-1.,20.) );
    }


    // *************** EXPERT ****************** //
    addMonGroup(new MonGroup(this,m_base_path_expert+"/JpsiFinder",run));
    addHistogram( new TH2F(prefix+"_Jpsi_cosThetaStar_central", "Bphys Jpsi CosThetaStar Central"     ,50, -1.,1.,    nTrigSize, 0., nTrigSizeF) );
    addHistogram( new TH2F(prefix+"_Jpsi_phiStar_central",      "Bphys Jpsi PhiStar Central"          , 70, -M_PI,M_PI, nTrigSize, 0., nTrigSizeF) );
    
    addHistogram( new TH2F(prefix+"_Jpsi_cosThetaStar_forward", "Bphys Jpsi CosThetaStar Forward"     ,50, -1.,1.,    nTrigSize, 0., nTrigSizeF) );
    addHistogram( new TH2F(prefix+"_Jpsi_phiStar_forward",      "Bphys Jpsi PhiStar Forward"          , 70, -M_PI,M_PI, nTrigSize, 0., nTrigSizeF) );
    
    addHistogram( new TH2F(prefix+"_Upsi_cosThetaStar_central", "Bphys Upsi CosThetaStar Central"     ,50, -1.,1.,    nTrigSize, 0., nTrigSizeF) );
    addHistogram( new TH2F(prefix+"_Upsi_phiStar_central",      "Bphys Upsi PhiStar Central"          , 70, -M_PI,M_PI, nTrigSize, 0., nTrigSizeF) );
    
    addHistogram( new TH2F(prefix+"_Upsi_cosThetaStar_forward", "Bphys Upsi CosThetaStar Forward"     ,50, -1.,1.,    nTrigSize, 0., nTrigSizeF) );
    addHistogram( new TH2F(prefix+"_Upsi_phiStar_forward",      "Bphys Upsi PhiStar Forward"          , 70, -M_PI,M_PI, nTrigSize, 0., nTrigSizeF) );
    
    //addHistogram( new TH2I(prefix+"_TriggerCounts",         "Trigger Counts"             ,100, 2950,3250, nTrigSize, 0., nTrigSizeF) );

    for (int iTrigName = 0; iTrigName != nTrigSize; ++iTrigName){
        // set the axis labels, define 0 to be the offline values
        TString label("");
        if (iTrigName == 0)
            label = "Offline";
        else
            label = m_monitored_chains.at(iTrigName-1);
        // ******** SHIFTER ********* //
        setCurrentMonGroup(m_base_path_shifter+"/JpsiFinder");
        hist2(m_prefix+"_Jpsi_mass_central")->GetYaxis()   ->SetBinLabel(iTrigName+1,label); //+1 for hist offset
        
        hist2(m_prefix+"_Jpsi_mass_forward")->GetYaxis()   ->SetBinLabel(iTrigName+1,label);
        
        hist2(m_prefix+"_Upsi_mass_central")->GetYaxis()   ->SetBinLabel(iTrigName+1,label);

        hist2(m_prefix+"_Upsi_mass_forward")->GetYaxis()   ->SetBinLabel(iTrigName+1,label);

        hist2(m_prefix+"_sumCharges")->GetYaxis() ->SetBinLabel(iTrigName+1,label);

        
        // ******** EXPERT ********* //
        setCurrentMonGroup(m_base_path_expert+"/JpsiFinder");
        hist2(m_prefix+"_Jpsi_cosThetaStar_central")->GetYaxis()->SetBinLabel(iTrigName+1,label);
        hist2(m_prefix+"_Jpsi_phiStar_central")->GetYaxis()     ->SetBinLabel(iTrigName+1,label);
        hist2(m_prefix+"_Jpsi_cosThetaStar_forward")->GetYaxis()->SetBinLabel(iTrigName+1,label);
        hist2(m_prefix+"_Jpsi_phiStar_forward")->GetYaxis()     ->SetBinLabel(iTrigName+1,label);
        hist2(m_prefix+"_Upsi_cosThetaStar_central")->GetYaxis()->SetBinLabel(iTrigName+1,label);
        hist2(m_prefix+"_Upsi_phiStar_central")->GetYaxis()     ->SetBinLabel(iTrigName+1,label);
        hist2(m_prefix+"_Upsi_cosThetaStar_forward")->GetYaxis()->SetBinLabel(iTrigName+1,label);
        hist2(m_prefix+"_Upsi_phiStar_forward")->GetYaxis()     ->SetBinLabel(iTrigName+1,label);

        
    } // for loop over trigger names

    
    return StatusCode::SUCCESS;
} //bookJpsiFinder

StatusCode HLTXAODBphysMonTool::fillJpsiFinder(){
    // fill the hists with offline comparisons
    ATH_MSG_DEBUG("in JpsiFinder()");
    
    const xAOD::VertexContainer*    jpsiContainer(nullptr);
    if (evtStore()->retrieve(jpsiContainer   , m_JpsiCandidatesKey).isFailure() || !jpsiContainer) {
        ATH_MSG_WARNING("No Jpsi Container Found, skipping jpsiFinder method");
        return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("number of jpsi candidates " << jpsiContainer->size());

    // Extract information from the Jpsi candidates
    for ( xAOD::VertexContainer::const_iterator vxcItr = jpsiContainer->begin() ; vxcItr != jpsiContainer->end() ; ++vxcItr ) {
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
        //double orig_mass = orig_onia.M();                      // mass from original tracks
        ///!double mass      = ref_onia.M();                        // mass from refitted tracks
        // double pt      = ref_onia.Pt();                        // pt from refitted tracks
        //double error     = invariantMassError(jpsiCandidate, std::vector<double>(2, m_muonMass));  // invariant mass error
        double rapidity  = ref_onia.Rapidity();
        double abs_rapidity = fabs(rapidity);
        //double phi       = ref_onia.Phi();

        float phiTrk1 = refTrk1.Phi();
        float etaTrk1 = refTrk1.Eta();
        //float ptTrk1  = refTrk1.Pt();

        float phiTrk2 = refTrk2.Phi();
        float etaTrk2 = refTrk2.Eta();
        //float ptTrk2  = refTrk2.Pt();

        float sumCharges(0.);
        for (auto tpel: jpsiCandidate->trackParticleLinks()) {
            if (!tpel.isValid() || !tpel.cptr()) continue;
            sumCharges += (*tpel)->charge();
        } // loop over track particle element links

        
        float absDphi = absDeltaPhi(phiTrk1,phiTrk2);
        float absDeta = absDeltaEta(etaTrk1,etaTrk2);

      ///!  int m_trkOrigCharge1=trackCharge(jpsiCandidate, 0);
        //int m_trkOrigCharge2=trackCharge(jpsiCandidate, 1);
      ///!  int qTrk1(m_trkOrigCharge1);//, qTrk2(m_trkOrigCharge2);
      ///!  float phiStar = phiMethod(refTrk1, refTrk2, qTrk1);
      ///!  float cosThetaStar = cosMethod(refTrk1, refTrk2, qTrk1);

        
        // check for, and access, the decorated quantities
        bool hasAccessorVariables(true);
        //static Accessor< float > acc("");
#define createAccesor(name) static SG::AuxElement::Accessor< float > acc_##name(#name)
        createAccesor(Lxy_bs);
        createAccesor(LxyError_bs);
        createAccesor(Tau_bs);
        createAccesor(TauError_bs);
        createAccesor(Lxy_pv);
        createAccesor(LxyError_pv);
        createAccesor(Tau_pv);
        createAccesor(TauError_pv);
        createAccesor(pT);
        createAccesor(pTError);
        createAccesor(phiStar);
        createAccesor(cosThetaStar);
        createAccesor(mass);
        createAccesor(massError);
        

        if (acc_cosThetaStar.isAvailable(*jpsiCandidate)) {
            ATH_MSG_VERBOSE("Expected, and found, decorated variables");
        } else {
            ATH_MSG_DEBUG("Expected, but did not find, decorated variables");
            hasAccessorVariables = false;
        }

        float aLxy_bs      = (hasAccessorVariables ? acc_Lxy_bs(*jpsiCandidate) : 0.);
        float aTau_bs      = (hasAccessorVariables ? acc_Tau_bs(*jpsiCandidate) : 0.);
        float apT          = (hasAccessorVariables ? acc_pT(*jpsiCandidate) : 0.);
        float aphiStar     = (hasAccessorVariables ? acc_phiStar(*jpsiCandidate) : 0.);
        float acosThetaStar= (hasAccessorVariables ? acc_cosThetaStar(*jpsiCandidate) : 0.);
        float amass        = (hasAccessorVariables ? acc_mass(*jpsiCandidate) : 0.);

        //ATH_MSG_INFO("old phiStar: " << phiStar);
        //ATH_MSG_INFO("new phiStar: " << aphiStar);
        //ATH_MSG_INFO("old cosThetaStar: " << cosThetaStar);
        //ATH_MSG_INFO("new cosThetaStar: " << acosThetaStar);
        //ATH_MSG_INFO("old mass: " << mass);
        //ATH_MSG_INFO("new mass: " << amass);
        
        bool isCentral = abs_rapidity < 1.05;
        bool isForward = abs_rapidity > 1.05;

        bool isJpsiCentral = isCentral && (amass > m_mw_jpsi_central_min && amass < m_mw_jpsi_central_max);
        bool isJpsiForward = isForward && (amass > m_mw_jpsi_forward_min && amass < m_mw_jpsi_forward_max);
        
        bool isUpsiCentral = isCentral && (amass > m_mw_upsi_central_min && amass < m_mw_upsi_central_max);
        bool isUpsiForward = isForward && (amass > m_mw_upsi_forward_min && amass < m_mw_upsi_forward_max);

        
        // **** fill the offline entries **** //
        // ******** SHIFTER ********* //
        setCurrentMonGroup(m_base_path_shifter+"/JpsiFinder");
        if (isJpsiCentral) {
            hist2(m_prefix+"_Jpsi_mass_central")->     Fill(amass, "Offline", 1);
    
        }
        if (isJpsiForward) {
            hist2(m_prefix+"_Jpsi_mass_forward")->     Fill(amass, "Offline", 1);
    
        }
        if (isUpsiCentral) {
            hist2(m_prefix+"_Upsi_mass_central")->     Fill(amass, "Offline", 1);
    
        }
        if (isUpsiForward) {
            hist2(m_prefix+"_Upsi_mass_forward")->     Fill(amass, "Offline", 1);
    
        }
        
        hist2(m_prefix+"_sumCharges")->    Fill(sumCharges, "Offline", 1);
        
        // Fill 1d histograms
        for (const auto& trigpair : m_dedicated_chains) {
            
            const auto cg   = getTDT()->getChainGroup(trigpair.second);
            bool isPassed   (cg-> isPassed(TrigDefs::Physics));
            if (!isPassed) continue; // only passed chains from here on
            
            setCurrentMonGroup(m_base_path_shifter+"/JpsiFinder/"+trigpair.first);
            if(isJpsiCentral) {
                hist(m_prefix+"_Jpsi_mass_central_"+trigpair.first)->    Fill(amass, 1);
                hist(m_prefix+"_Jpsi_rap_central_"+trigpair.first)->     Fill(rapidity, 1);
                hist(m_prefix+"_Jpsi_dphi_central_"+trigpair.first)->    Fill(absDphi, 1);
                hist(m_prefix+"_Jpsi_deta_central_"+trigpair.first)->    Fill(absDeta, 1);
                hist(m_prefix+"_Jpsi_pt_central_"+trigpair.first)->      Fill(apT/1000., 1);
                hist(m_prefix+"_Jpsi_tau_bs_central_"+trigpair.first)->  Fill(aTau_bs, 1);
                hist(m_prefix+"_Jpsi_lxy_bs_central_"+trigpair.first)->  Fill(aLxy_bs, 1);
            }
            if(isJpsiForward) {
                hist(m_prefix+"_Jpsi_mass_forward_"+trigpair.first)->    Fill(amass, 1);
                hist(m_prefix+"_Jpsi_rap_forward_"+trigpair.first)->     Fill(rapidity, 1);
                hist(m_prefix+"_Jpsi_dphi_forward_"+trigpair.first)->    Fill(absDphi, 1);
                hist(m_prefix+"_Jpsi_deta_forward_"+trigpair.first)->    Fill(absDeta, 1);
                hist(m_prefix+"_Jpsi_pt_forward_"+trigpair.first)->      Fill(apT/1000., 1);
                hist(m_prefix+"_Jpsi_tau_bs_forward_"+trigpair.first)->  Fill(aTau_bs, 1);
                hist(m_prefix+"_Jpsi_lxy_bs_forward_"+trigpair.first)->  Fill(aLxy_bs, 1);
            }
            if(isJpsiCentral || isJpsiForward) {
                hist(m_prefix+"_Jpsi_mass_"+trigpair.first)->    Fill(amass, 1);
                hist(m_prefix+"_Jpsi_rap_"+trigpair.first)->     Fill(rapidity, 1);
                hist(m_prefix+"_Jpsi_dphi_"+trigpair.first)->    Fill(absDphi, 1);
                hist(m_prefix+"_Jpsi_deta_"+trigpair.first)->    Fill(absDeta, 1);
                hist(m_prefix+"_Jpsi_pt_"+trigpair.first)->      Fill(apT/1000., 1);
                hist(m_prefix+"_Jpsi_tau_bs_"+trigpair.first)->  Fill(aTau_bs, 1);
                hist(m_prefix+"_Jpsi_lxy_bs_"+trigpair.first)->  Fill(aLxy_bs, 1);
            }
        }

        
        // ******** EXPERT ********* //
        setCurrentMonGroup(m_base_path_expert+"/JpsiFinder");
        if (isJpsiCentral) {
            hist2(m_prefix+"_Jpsi_cosThetaStar_central")->Fill(acosThetaStar, "Offline", 1);
            hist2(m_prefix+"_Jpsi_phiStar_central")->     Fill(aphiStar,      "Offline", 1);
        }
        if (isJpsiForward) {
            hist2(m_prefix+"_Jpsi_cosThetaStar_forward")->Fill(acosThetaStar, "Offline", 1);
            hist2(m_prefix+"_Jpsi_phiStar_forward")->     Fill(aphiStar,      "Offline", 1);
        }
        if (isUpsiCentral) {
            hist2(m_prefix+"_Upsi_cosThetaStar_central")->Fill(acosThetaStar, "Offline", 1);
            hist2(m_prefix+"_Upsi_phiStar_central")->     Fill(aphiStar,      "Offline", 1);
        }
        if (isUpsiForward) {
            hist2(m_prefix+"_Upsi_cosThetaStar_forward")->Fill(acosThetaStar, "Offline", 1);
            hist2(m_prefix+"_Upsi_phiStar_forward")->     Fill(aphiStar,      "Offline", 1);
        }


        // **** loop over trigger items **** //
        for (const auto& chainName : m_monitored_chains) {
            const auto cg   = getTDT()->getChainGroup(chainName);
            bool isPassed   (cg-> isPassed(TrigDefs::Physics));
            
            
            if (!isPassed) continue; // only passed chains from here on
            ATH_MSG_DEBUG("JpsiFinder passed chain: " << chainName << "!!");

            // ******** SHIFTER ********* //
            setCurrentMonGroup(m_base_path_shifter+"/JpsiFinder");
            if (isJpsiCentral) {
                hist2(m_prefix+"_Jpsi_mass_central")->   Fill(amass, chainName.c_str(), 1);

            }
            if (isJpsiForward) {
                hist2(m_prefix+"_Jpsi_mass_forward")->   Fill(amass, chainName.c_str(), 1);

            }
            if (isUpsiCentral) {
                hist2(m_prefix+"_Upsi_mass_central")->   Fill(amass, chainName.c_str(), 1);

            }
            if (isUpsiForward) {
                hist2(m_prefix+"_Upsi_mass_forward")->   Fill(amass, chainName.c_str(), 1);

            }
            hist2(m_prefix+"_sumCharges")->    Fill(sumCharges, chainName.c_str(), 1);

            
            // ******** EXPERT ********* //
            setCurrentMonGroup(m_base_path_expert+"/JpsiFinder");
            if (isJpsiCentral) {
                hist2(m_prefix+"_Jpsi_cosThetaStar_central")->Fill(acosThetaStar, chainName.c_str(), 1);
                hist2(m_prefix+"_Jpsi_phiStar_central")->     Fill(aphiStar,      chainName.c_str(), 1);
            }
            if (isJpsiForward) {
                hist2(m_prefix+"_Jpsi_cosThetaStar_forward")->Fill(acosThetaStar, chainName.c_str(), 1);
                hist2(m_prefix+"_Jpsi_phiStar_forward")->     Fill(aphiStar,      chainName.c_str(), 1);
            }
            if (isUpsiCentral) {
                hist2(m_prefix+"_Upsi_cosThetaStar_central")->Fill(acosThetaStar, chainName.c_str(), 1);
                hist2(m_prefix+"_Upsi_phiStar_central")->     Fill(aphiStar,      chainName.c_str(), 1);
            }
            if (isUpsiForward) {
                hist2(m_prefix+"_Upsi_cosThetaStar_forward")->Fill(acosThetaStar, chainName.c_str(), 1);
                hist2(m_prefix+"_Upsi_phiStar_forward")->     Fill(aphiStar,      chainName.c_str(), 1);
            }

        } // loop over trigger names
        
        
    } // loop over jpsi candidates
    return StatusCode::SUCCESS;
} // fillJpsiFinder

StatusCode HLTXAODBphysMonTool::bookJpsiFinderEfficiency() {

    int nBins(40);

    for (const auto& effpair: m_efficiency_chains) {
         TString prefix = m_prefix + "_noVtx_noOS_" + effpair.first;
         addMonGroup(new MonGroup(this,m_base_path_shifter+"/JpsiFinder/Efficiency/"+"noVtx_noOS_"+effpair.first,run));
             
         addProfile  ( new TProfile(prefix+"_eta", prefix+"_eta"   ,nBins,-3.,3.) );
         addProfile  ( new TProfile(prefix+"_phi", prefix+"_phi"   ,nBins, -TMath::Pi(), TMath::Pi()) );
         addProfile  ( new TProfile(prefix+"_pt",  prefix+"_pt"    ,nBins, 0,50) );
      
         addProfile  ( new TProfile(prefix+"_eta1", prefix+"_eta1", nBins, -3.,3.) );
         addProfile  ( new TProfile(prefix+"_phi1", prefix+"_phi1", nBins, -TMath::Pi(), TMath::Pi()) );
         addProfile  ( new TProfile(prefix+"_pt1",  prefix+"_pt1" , nBins, 0,50) );

         addProfile  ( new TProfile(prefix+"_eta2", prefix+"_eta2", nBins, -3.,3.) );
         addProfile  ( new TProfile(prefix+"_phi2", prefix+"_phi2", nBins, -TMath::Pi(), TMath::Pi()) );
         addProfile  ( new TProfile(prefix+"_pt2",  prefix+"_pt2" , nBins, 0,50) );
     }

    return StatusCode::SUCCESS;
}


StatusCode HLTXAODBphysMonTool::fillJpsiFinderEfficiency() {
    
    m_JpsiFinderEfficiency_denomnoVtxOS_pairs.clear();
    
    const auto cg   = getTDT()->getChainGroup(m_trigchain_denomnoVtxOS);
    bool isPassed   (cg-> isPassed(TrigDefs::Physics)); 
    
    if (isPassed){
    
      const auto& fc       = getTDT()->features(m_trigchain_denomnoVtxOS);
      const auto& fc_bphys = fc.get<xAOD::TrigBphysContainer>();
      
      ATH_MSG_DEBUG("Size of vector: m_trigchain_denomnoVtxOS " << fc_bphys.size());
    
      for( auto cont_bphys : fc_bphys ) {
        ATH_MSG_DEBUG("REGTEST Got Bphysics container, size = " << cont_bphys.cptr()->size());
        for ( auto bphys:  *(cont_bphys.cptr()) )  {
             ATH_MSG_DEBUG("REGTEST Level = " << bphys->level());
             // ignore l2 objects
             if ((bphys->level() != xAOD::TrigBphys::EF) &&  (bphys->level() != xAOD::TrigBphys::HLT)) continue;
             
             const std::vector<ElementLink<xAOD::TrackParticleContainer> > trackVector = bphys->trackParticleLinks();
             if (trackVector.size() <2) {
                 ATH_MSG_DEBUG("REGTEST Unexpected number of tracks " << trackVector.size());
                 continue;
             }
           
             const xAOD::TrackParticle *trk1(nullptr),*trk2(nullptr);
             if (!trackVector.at(0).isValid() || (*trackVector.at(0) == nullptr)) {
                 ATH_MSG_DEBUG("REGTEST Track 1 is invalid");
                 continue;
             }
             if (!trackVector.at(1).isValid() || (*trackVector.at(1) == nullptr)) {
                 ATH_MSG_DEBUG("REGTEST Track 2 is invalid");
                 continue;
             }
             trk1 = *trackVector.at(0);
             trk2 = *trackVector.at(1);

             auto cur_pair_efficiency     = std::make_pair (trk1, trk2);
             m_JpsiFinderEfficiency_denomnoVtxOS_pairs.push_back(cur_pair_efficiency);
             }
        }


        for (const auto& effpair: m_efficiency_chains) {
             if (fillJpsiFinderEfficiencyHelper(Form("noVtx_noOS_%s",effpair.first.c_str()),effpair.second).isFailure()) {                 
                 ATH_MSG_ERROR("Problems in Eff. booking noVtx_noOS " << effpair.first << " " << effpair.second); 
                 continue;                                            
             }                  
        }
    }

    return StatusCode::SUCCESS;

}

StatusCode HLTXAODBphysMonTool::fillJpsiFinderEfficiencyHelper(const std::string & groupName, const std::string & chainName) {

    TString prefix = m_prefix + "_" + groupName; // convert from std::string to TString          
    const auto cg   = getTDT()->getChainGroup(chainName);
    bool isPassed   (cg-> isPassed(TrigDefs::Physics)); 
    
    if (isPassed) { 
      const xAOD::VertexContainer*    jpsiContainer(nullptr);
    
      if (evtStore()->retrieve(jpsiContainer   , m_JpsiCandidatesKey).isFailure() || !jpsiContainer) {
      ATH_MSG_WARNING("No Jpsi Container Found, skipping jpsiFinder method");
      return StatusCode::SUCCESS;
      }
    
      ATH_MSG_DEBUG("number of jpsi candidates " << jpsiContainer->size());

      // Extract information from the Jpsi candidates
      for ( xAOD::VertexContainer::const_iterator vxcItr = jpsiContainer->begin() ; vxcItr != jpsiContainer->end() ; ++vxcItr ) {
           const xAOD::Vertex* jpsiCandidate = (*vxcItr);
           if (!jpsiCandidate) continue;
           // to see how to access variables; refer to:
           // https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/JpsiUpsilonTools/trunk/src/JpsiExample.cxx
          
          
           // refitted track parameters
           TLorentzVector refTrk1 = track4Momentum(jpsiCandidate, 0, m_muonMass);
           TLorentzVector refTrk2 = track4Momentum(jpsiCandidate, 1, m_muonMass);
       
           TLorentzVector ref_onia  = refTrk1 + refTrk2;
           double phi     = ref_onia.Phi();
           double eta     = ref_onia.Eta();
           double pt      = ref_onia.Pt();
      
           double phiTrk1 = refTrk1.Phi();
           double etaTrk1 = refTrk1.Eta();
           double ptTrk1  = refTrk1.Pt();
      
           double phiTrk2 = refTrk2.Phi();
           double etaTrk2 = refTrk2.Eta();
           double ptTrk2  = refTrk2.Pt();
      
           int It_number=0;
      
           for(auto It=m_JpsiFinderEfficiency_denomnoVtxOS_pairs.begin();It!=m_JpsiFinderEfficiency_denomnoVtxOS_pairs.end();It++) {
             
             auto cur_pair_Noos=*It;
             // refitted track parameters 
             TLorentzVector NoosTrk1 = cur_pair_Noos.first ->p4();
             TLorentzVector NoosTrk2 = cur_pair_Noos.second->p4();
             It_number=It_number+1;         
             double phiNoosTrk1 = NoosTrk1.Phi();
             double etaNoosTrk1 = NoosTrk1.Eta();
             double ptNoosTrk1  = NoosTrk1.Pt();
      
             double phiNoosTrk2 = NoosTrk2.Phi();
             double etaNoosTrk2 = NoosTrk2.Eta();
             double ptNoosTrk2  = NoosTrk2.Pt();
             if(   ((abs(phiNoosTrk1-phiTrk1)<=m_EffMatchThresh_phi) && (abs(etaNoosTrk1-etaTrk1)<=m_EffMatchThresh_Eta) && (((abs(ptNoosTrk1-ptTrk1))/ptNoosTrk1)<=m_EffMatchThresh_pT) && (abs(phiNoosTrk2-phiTrk2)<=m_EffMatchThresh_phi) && (abs(etaNoosTrk2-etaTrk2)<=m_EffMatchThresh_Eta) && (((abs(ptNoosTrk2-ptTrk2))/ptNoosTrk2)<=m_EffMatchThresh_pT))
               ||  ((abs(phiNoosTrk2-phiTrk1)<=m_EffMatchThresh_phi) && (abs(etaNoosTrk2-etaTrk1)<=m_EffMatchThresh_Eta) && (((abs(ptNoosTrk2-ptTrk1))/ptNoosTrk2)<=m_EffMatchThresh_pT) && (abs(phiNoosTrk1-phiTrk2)<=m_EffMatchThresh_phi) && (abs(etaNoosTrk1-etaTrk2)<=m_EffMatchThresh_Eta) && (((abs(ptNoosTrk1-ptTrk2))/ptNoosTrk1)<=m_EffMatchThresh_pT)) ) {
            // ******** SHIFTER ********* //               
             setCurrentMonGroup(m_base_path_shifter+"/JpsiFinder/Efficiency/"+groupName);
             profile(Form("%s_eta",prefix.Data()))->Fill(eta,1.0,1.0);
             profile(Form("%s_phi",prefix.Data()))->Fill(phi,1.0,1.0);
             profile(Form("%s_pt", prefix.Data()))->Fill(pt*0.001,1.0,1.0);
             
             profile(Form("%s_eta1",prefix.Data()))->Fill(etaTrk1,1.0,1.0);
             profile(Form("%s_phi1",prefix.Data()))->Fill(phiTrk1,1.0,1.0);
             profile(Form("%s_pt1", prefix.Data()))->Fill(ptTrk1*0.001,1.0,1.0);
             
             profile(Form("%s_eta2",prefix.Data()))->Fill(etaTrk2,1.0,1.0);
             profile(Form("%s_phi2",prefix.Data()))->Fill(phiTrk2,1.0,1.0);
             profile(Form("%s_pt2", prefix.Data()))->Fill(ptTrk2*0.001,1.0,1.0);
             continue;}
           }
           if (It_number==m_JpsiFinderEfficiency_denomnoVtxOS_pairs.size()) {
               
               
             setCurrentMonGroup(m_base_path_shifter+"/JpsiFinder/Efficiency/"+groupName); 
             
             profile(Form("%s_eta",prefix.Data()))->Fill(eta,0.0,1.0);
             profile(Form("%s_phi",prefix.Data()))->Fill(phi,0.0,1.0);
             profile(Form("%s_pt", prefix.Data()))->Fill(pt*0.001,0.0,1.0);
             
             profile(Form("%s_eta1",prefix.Data()))->Fill(etaTrk1,0.0,1.0);
             profile(Form("%s_phi1",prefix.Data()))->Fill(phiTrk1,0.0,1.0);
             profile(Form("%s_pt1", prefix.Data()))->Fill(ptTrk1*0.001,0.0,1.0);
             
             profile(Form("%s_eta2",prefix.Data()))->Fill(etaTrk2,0.0,1.0);
             profile(Form("%s_phi2",prefix.Data()))->Fill(phiTrk2,0.0,1.0);
             profile(Form("%s_pt2", prefix.Data()))->Fill(ptTrk2*0.001,0.0,1.0);
           }
      }
    }

    return StatusCode::SUCCESS;

}
StatusCode HLTXAODBphysMonTool::bookTriggerGroups() {
    // book the hists for the triggered groups
    
    // each group is allowed one or more triggers
    // the 'trigger name' (first argument) must be indep of the trigger chain name itself,
    // inorder to remain trigger menu-aware
    
    for (const auto& trigpair : m_dedicated_chains) {
        ATH_MSG_DEBUG("Building Dedicated chain monitoring for: " << trigpair.first<< " " <<trigpair.second );
        // groupName, prefix, pathInRoot, chainName
        bookTrigBphysHists(trigpair.first,  m_prefix, trigpair.first, trigpair.second);
    } // for
    for (const auto& trigpair : m_dedicatedL1Topo_chains) {
        ATH_MSG_DEBUG("Building Dedicated L1Topo chain monitoring for: " << trigpair.first<< " " <<trigpair.second );
        // groupName, prefix, pathInRoot, chainName
        bookTrigBphysHists(trigpair.first,  m_prefix, trigpair.first, trigpair.second, false); // book only limited number of histograms
    } // for
    return StatusCode::SUCCESS;
} // bookTriggerGroups

StatusCode HLTXAODBphysMonTool::fillTriggerGroups() {
    // fill the hists for the triggered groups
    for (const auto& trigpair : m_dedicated_chains) {
        ATH_MSG_DEBUG("Filling Dedicated chain monitoring for: " << trigpair.first<< " " <<trigpair.second );
        if (!fillTriggerGroup(trigpair.first,trigpair.second).isSuccess()) {
            ATH_MSG_WARNING("Problems filling group/chain: " << trigpair.first<< " " <<trigpair.second);
            continue;
        }
    } // for
    for (const auto& trigpair : m_dedicatedL1Topo_chains) {
        ATH_MSG_DEBUG("Filling Dedicated L1Topo chain monitoring for: " << trigpair.first<< " " <<trigpair.second );
        if (!fillTriggerGroup(trigpair.first,trigpair.second, false).isSuccess()) {
            ATH_MSG_WARNING("Problems filling group/chain: " << trigpair.first<< " " <<trigpair.second);
            continue;
        }
    } // for
return StatusCode::SUCCESS;
} // fillTriggerGroups

StatusCode HLTXAODBphysMonTool::fillTriggerGroup(const std::string & groupName, const std::string & chainName, const bool fullSetOfHists) {
    // helper method for filling individual groups
    
    std::string baseName = stripHLT(chainName); // remove "HLT_" from the name
    
    const auto cg   = getTDT()->getChainGroup(chainName);
    const auto cgL2 = getTDT()->getChainGroup("L2_"+baseName);
    const auto cgEF = getTDT()->getChainGroup("EF_"+baseName);
    
    bool isPassed   (cg-> isPassed(TrigDefs::Physics));
    bool isPassedL2(cgL2->isPassed(TrigDefs::Physics));
    bool isPassedEF(cgEF->isPassed(TrigDefs::Physics));
    
    ATH_MSG_DEBUG("Trigger passed(HLT/EF/L2): " << isPassed << isPassedL2 << isPassedEF << " " << chainName << " group: " << groupName);
    if (!isPassed) return StatusCode::SUCCESS; // only HLT passed chains
    
    // get HLT items
    const auto& fc       = getTDT()->features(chainName);
    
    std::string label   = "";
    std::string labelEF = "";
    std::string labelL2 = "";
    if ( chainName.find("Bmumux") != std::string::npos) {
//         label   = "EFBMuMuXFex";
        labelEF = "EFBMuMuXFex";
        labelL2 = "L2BMuMuFex"; // in new world, seed from BMuMu 
    } else if (chainName.find("Trkloose") != std::string::npos) {
        labelEF = "EFTrackMass";
        labelL2 = "L2TrackMass";
    } else {
        labelEF = "EFBMuMuFex";
        labelL2 = "L2BMuMuFex";
    }

    const auto& fc_bphys   = fc.get<xAOD::TrigBphysContainer>(label);
    const auto& fc_bphysEF = fc.get<xAOD::TrigBphysContainer>(labelEF);
    const auto& fc_bphysL2 = fc.get<xAOD::TrigBphysContainer>(labelL2);
    
    ATH_MSG_DEBUG("Features: HLT: " << fc_bphys.size());
    ATH_MSG_DEBUG("Features:  EF: " << fc_bphysEF.size());
    ATH_MSG_DEBUG("Features:  L2: " << fc_bphysL2.size());

    
    ATH_MSG_DEBUG("Size of vector< Trig::Feature<xAOD::TrigBphysContainer> > = " << fc_bphys.size());
    
    m_muon_pairs_processed.clear();
    
    for( auto cont_bphys : fc_bphys ) {
        ATH_MSG_DEBUG("REGTEST Got Bphysics container, size = " << cont_bphys.cptr()->size());
        for ( auto bphys:  *(cont_bphys.cptr()) )  {
            ATH_MSG_DEBUG("REGTEST Level = " << bphys->level());

            // ignore l2 objects
            if ((bphys->level() != xAOD::TrigBphys::EF) &&  (bphys->level() != xAOD::TrigBphys::HLT)) continue;
            
            fillTrigBphysHists(bphys,groupName,  m_prefix,groupName,chainName, fullSetOfHists);
            // to be added with more complete informations
            
        } // loop over bphys objects
    } // loop over containers
    
    
    return StatusCode::SUCCESS;
} //fillTriggerGroup

StatusCode HLTXAODBphysMonTool::bookContainers(){
    // book the hists for containers

    //************ SHIFTER ************* //
    addMonGroup(new MonGroup(this,m_base_path_shifter+"/Containers",run));
    addHistogram( new TH2I((m_prefix+"_Containers_size").c_str(),"Container sizes",(int)m_containerList.size(),0,(int) m_containerList.size(),20,-0.5,19.5));

    for (const auto& containerItem: m_containerList) {
        bookTrigBphysHists(containerItem, m_prefix,"Containers/"+containerItem, containerItem);
    } // loop over container names
    
    return StatusCode::SUCCESS;
}
StatusCode HLTXAODBphysMonTool::fillContainers(){
    // fill the hists for containers
    for (const auto& containerItem: m_containerList) {
        
        m_muon_pairs_processed.clear();
        const xAOD::TrigBphysContainer*  trigBphysContainer(nullptr);
        StatusCode sc = evtStore()->retrieve(trigBphysContainer,containerItem); //"HLT_xAOD__TrigBphysContainer_EFBMuMuFex"
        if (sc.isFailure() || ! trigBphysContainer) {
            ATH_MSG_WARNING("No TrigBphysContainer found with tag: "<< containerItem);
            continue;
        }
        //************ SHIFTER ************* //
        setCurrentMonGroup(m_base_path_shifter+"/Containers");
        std::string containerNameAbbr = containerItem.substr(std::string("HLT_xAOD__TrigBphysContainer_").length(),-1);
        hist2((m_prefix+"_Containers_size").c_str())->Fill(containerNameAbbr.c_str(), trigBphysContainer->size(),1);

        for ( auto bphys:  (*trigBphysContainer))  {
            fillTrigBphysHists(bphys,containerItem, m_prefix,"Containers/"+containerItem,containerItem);
        } // loop over items in the container
    } // loop over container names
    return StatusCode::SUCCESS;
}


void HLTXAODBphysMonTool::bookTrigBphysHists(const std::string & groupName ,const std::string & prefix,
                                             const std::string & path, const std::string & chainName, const bool fullSetOfHists) {
    ATH_MSG_DEBUG("bookTrigBphysHists: " << groupName << " " << prefix << " " << " " << path << " " << chainName);

    auto pref = prefix.c_str(); // conversion
    auto name = groupName.c_str(); // conversion
    auto detTitle = chainName.c_str(); // conversion

    //************ SHIFTER ************* //
    addMonGroup(new MonGroup(this,m_base_path_shifter+"/"+path,run));
    
    if ( chainName.find("bTau") != std::string::npos) 
      addHistogram( new TH1F(Form("%s_%s_mass",pref,name) ,     Form("%s_%s_mass;mass(#mu#mu)[MeV];Candidates",pref,detTitle) ,    200, m_taumass_min,m_taumass_max) );
    else
      addHistogram( new TH1F(Form("%s_%s_mass",pref,name) ,     Form("%s_%s_mass;mass(#mu#mu)[MeV];Candidates",pref,detTitle) ,    200, m_oniamass_min,m_oniamass_max) );
    addHistogram( new TH1F(Form("%s_%s_eta",pref,name) ,      Form("%s_%s_eta;#eta(#mu#mu);Candidates",pref,detTitle) ,     30, m_eta_min,m_eta_max) );
    addHistogram( new TH1F(Form("%s_%s_dR",pref,name) ,       Form("%s_%s_dR;dR(#mu_{1,2});Candidates",pref,detTitle) ,     30, m_dr_min,m_dr_max) );
    addHistogram( new TH1F(Form("%s_%s_pTmu1",pref,name) ,    Form("%s_%s_pTmu1;p_{T}(#mu_{1})[GeV];Candidates",pref,detTitle) ,    nptmus,ptmus) );
    addHistogram( new TH1F(Form("%s_%s_pTmu2",pref,name) ,    Form("%s_%s_pTmu2;p_{T}(#mu_{2})[GeV];Candidates",pref,detTitle) ,    nptmus,ptmus) );
        
    if(fullSetOfHists) {
        addHistogram( new TH1F(Form("%s_%s_phi",pref,name) ,      Form("%s_%s_phi;#phi(#mu#mu);Candidates",pref,detTitle) ,     32, -M_PI,M_PI) );
        addHistogram( new TH1F(Form("%s_%s_z0mu1",pref,name) ,    Form("%s_%s_z0mu1;z_{0}(#mu_{1})[mm];Candidates",pref,detTitle) ,    150, m_z0_min,m_z0_max) );
        addHistogram( new TH1F(Form("%s_%s_z0mu2",pref,name) ,    Form("%s_%s_z0mu2;z_{0}(#mu_{1})[mm];Candidates",pref,detTitle) ,    150, m_z0_min,m_z0_max) );
        addHistogram( new TH1F(Form("%s_%s_d0mu1",pref,name) ,    Form("%s_%s_d0mu1;d_{0}(#mu_{1})[mm];Candidates",pref,detTitle) ,    220, m_d0_min,m_d0_max) );
        addHistogram( new TH1F(Form("%s_%s_d0mu2",pref,name) ,    Form("%s_%s_d0mu2;d_{0}(#mu_{1})[mm];Candidates",pref,detTitle) ,    220, m_d0_min,m_d0_max) );
        addHistogram( new TH1F(Form("%s_%s_dphi",pref,name) ,     Form("%s_%s_dphi;d#phi(#mu_{1,2});Candidates",pref,detTitle) ,     32, m_dphi_min,m_dphi_max) );
        addHistogram( new TH1F(Form("%s_%s_deta",pref,name) ,     Form("%s_%s_deta;d#eta(#mu_{1,2});Candidates",pref,detTitle) ,     30, m_deta_min,m_deta_max) );
        addHistogram( new TH1F(Form("%s_%s_pTsum",pref,name) ,    Form("%s_%s_pTsum;#Sigmap_{T}(#mu_{1,2})[GeV];Candidates",pref,detTitle) ,    nptmumus,ptmumus) );
        if ( chainName.find("bTau") != std::string::npos) 
          addHistogram( new TH1F(Form("%s_%s_fitmass",pref,name) ,  Form("%s_%s_fitmass;fitmass(#mu#mu)[MeV];Candidates",pref,detTitle) ,    200, m_taumass_min,m_taumass_max) );
        else
          addHistogram( new TH1F(Form("%s_%s_fitmass",pref,name) ,  Form("%s_%s_fitmass;fitmass(#mu#mu)[MeV];Candidates",pref,detTitle) ,    200, m_oniamass_min,m_oniamass_max) );
        addHistogram( new TH1F(Form("%s_%s_fitchi2",pref,name) ,  Form("%s_%s_fitchi2;#chi^{2}(#mu#mu);Candidates",pref,detTitle) ,    50, 0.,20.) );
        // ID tracks monitoring for Bmumux-like chains
        if ( chainName.find("bBmumux") != std::string::npos ) {
          addHistogram( new TH1F(Form("%s_%s_pTtrk",pref,name) ,    Form("%s_%s_pTtrk;p_{T}(trk)[GeV];Candidates",pref,detTitle) ,    50, m_trk_pt_min,m_trk_pt_max) );
          addHistogram( new TH1F(Form("%s_%s_z0trk",pref,name) ,    Form("%s_%s_z0trk;z_{0}(trk)[mm];Candidates",pref,detTitle) ,    150, m_z0_min,m_z0_max) );
          addHistogram( new TH1F(Form("%s_%s_d0trk",pref,name) ,    Form("%s_%s_d0trk;d_{0}(trk)[mm];Candidates",pref,detTitle) ,    220, m_d0_min,m_d0_max) );
          
        }
    }

    //************ EXPERT ************* //
    addMonGroup(new MonGroup(this,m_base_path_expert+"/"+path,run));
    addHistogram( new TH1F(Form("%s_%s_phiStar",pref,name) ,  Form("%s_%s_phiStar;#phi*;Candidates",pref,detTitle) ,     50, -TMath::Pi(), TMath::Pi()) );
    addHistogram( new TH1F(Form("%s_%s_cosThetaStar",pref,name) , Form("%s_%s_cosThetaStar;cos#theta*;Candidates",pref,detTitle) ,     50, -1., 1.) );
        
    if(fullSetOfHists) {
        addHistogram( new TH1F(Form("%s_%s_numBLayerHitsmu1",pref,name) ,      Form("%s_%s_numBLayerHitsmu1;B Layer Hits(#mu_{1});Candidates",pref,detTitle) ,  11, -1.,10.) );
        addHistogram( new TH1F(Form("%s_%s_numBLayerHitsmu2",pref,name) ,      Form("%s_%s_numBLayerHitsmu2;B Layer Hits(#mu_{2});Candidates",pref,detTitle) ,  11, -1.,10.) );
        addHistogram( new TH1F(Form("%s_%s_numSCTHitsmu1",pref,name) ,      Form("%s_%s_numSCTHitsmu1;SCT Hits(#mu_{1});Candidates",pref,detTitle) ,            21, -1.,20.) );
        addHistogram( new TH1F(Form("%s_%s_numSCTHitsmu2",pref,name) ,      Form("%s_%s_numSCTHitsmu2;SCT Hits(#mu_{2});Candidates",pref,detTitle) ,            21, -1.,20.) );
        addHistogram( new TH1F(Form("%s_%s_numPixelHitsmu1",pref,name) ,      Form("%s_%s_numPixelHitsmu1;Pixel Hits(#mu_{1});Candidates",pref,detTitle) ,      11, -1.,10.) );
        addHistogram( new TH1F(Form("%s_%s_numPixelHitsmu2",pref,name) ,      Form("%s_%s_numPixelHitsmu2;Pixel Hits(#mu_{2});Candidates",pref,detTitle) ,      11, -1.,10.) );
        addHistogram( new TH1F(Form("%s_%s_numTRTHitsmu1",pref,name) ,      Form("%s_%s_numTRTHitsmu1;TRT Hits(#mu_{1});Candidates",pref,detTitle) ,            61, -1.,60) );
        addHistogram( new TH1F(Form("%s_%s_numTRTHitsmu2",pref,name) ,      Form("%s_%s_numTRTHitsmu2;TRT Hits(#mu_{2});Candidates",pref,detTitle) ,            61, -1.,60.) );

        addHistogram( new TH2F(Form("%s_%s_phiStarcosThetaStar",pref,name) , Form("%s_%s_phiStarcosThetaStar;cos#theta*;#phi*",pref,detTitle) ,  20, -1., 1., 32, -TMath::Pi(), TMath::Pi()));
        addHistogram( new TH2F(Form("%s_%s_phiStarRapidity",pref,name) , Form("%s_%s_phiStarRapidity;y;#phi*",pref,detTitle) ,  22, -2.2,2.2, 50, -TMath::Pi(), TMath::Pi()));
        addHistogram( new TH2F(Form("%s_%s_phiStarPhi",pref,name) ,      Form("%s_%s_phiStarPhi;y;#phi*",pref,detTitle) ,       32, -TMath::Pi(), TMath::Pi(), 32, -TMath::Pi(), TMath::Pi()));
    
    }



} // bookTrigBphysHists


void HLTXAODBphysMonTool::fillTrigBphysHists(const xAOD::TrigBphys *bphysItem, const std::string & groupName,
                                             const std::string & prefix,const std::string & path, const std::string & chainName, const bool fullSetOfHists) {
    if (!bphysItem) {
        ATH_MSG_WARNING("fillTrigBphysHists null pointer provided");
    }
    ATH_MSG_DEBUG("fillTrigBphysHists for: " <<  bphysItem << " " <<  groupName << " "
                  << prefix << " " <<path << " " <<chainName );

    
    auto pref = prefix.c_str(); // conversion
    auto name = groupName.c_str(); // conversion
    
    float eta = bphysItem->eta();
    float phi = bphysItem->phi();
    float mass = bphysItem->mass();
    float fitmass = bphysItem->fitmass();
    float fitchi2 = bphysItem->fitchi2();
    
    const std::vector<ElementLink<xAOD::TrackParticleContainer> > trackVector = bphysItem->trackParticleLinks();
    if (trackVector.size() != 0) {
        ATH_MSG_DEBUG(" REGTEST got track vector size: " << trackVector.size() );
    } else {
        ATH_MSG_DEBUG(" REGTEST no track vector!!! " );
    }

    if (trackVector.size() %2 != 0) {
        ATH_MSG_DEBUG(" REGTEST uneven number of tracks (may be ok for Bmumux-like chains)" );
    }

    
    
    
    //************ SHIFTER ************* //
    setCurrentMonGroup(m_base_path_shifter+"/"+path);
    hist(Form("%s_%s_mass",pref,name))->Fill(mass);
    hist(Form("%s_%s_eta",pref,name))->Fill(eta);
    if(fullSetOfHists) {
        hist(Form("%s_%s_phi",pref,name))->Fill(phi);
        hist(Form("%s_%s_fitmass",pref,name))->Fill(fitmass);
        hist(Form("%s_%s_fitchi2",pref,name))->Fill(fitchi2);
    }



    
    
    std::vector<ElementLink<xAOD::TrackParticleContainer> >::const_iterator trkIt1=trackVector.begin();
    std::vector<ElementLink<xAOD::TrackParticleContainer> >::const_iterator trkIt2=trackVector.begin();
    for (; trkIt1!= trackVector.end(); ++trkIt1) {
        
        if (!(trkIt1->isValid())) {
            ATH_MSG_WARNING("TrackParticleContainer::Invalid ElementLink to track ");
            ATH_MSG_WARNING("   Invalid ElementLink for: " <<  bphysItem << " " <<  groupName << " "
                            << prefix << " " <<path << " " <<chainName );
            ATH_MSG_WARNING("   Invalid ElementLink: " << (trkIt1->dataID() == "" ? "<empty string> " : trkIt1->dataID())   << " " << trkIt1->index());
            continue;
        }
        const xAOD::TrackParticle * ptl1 = **trkIt1;
        if (!ptl1) {
            msg() << MSG::WARNING << "TrackParticle with nullptr but valid elementlink" << endmsg;
            continue;
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
        
        if(fullSetOfHists && chainName.find("bBmumux") != std::string::npos && trkIt1->dataID().find("Bphysics_IDTrig") != std::string::npos ) {
          // // first fill ID track histograms
          setCurrentMonGroup(m_base_path_shifter+"/"+path);
          hist(Form("%s_%s_pTtrk",pref,name))->Fill(ptTrk1/1000.);
          hist(Form("%s_%s_d0trk",pref,name))->Fill(d0Trk1);
          hist(Form("%s_%s_z0trk",pref,name))->Fill(z0Trk1);
          continue; // not consider ID tracks which appear in Bmumux-like chains
        }
        
        
        ATH_MSG_DEBUG("track1 "  << " pt phi eta " << ptTrk1 << " " <<
                      phiTrk1 << " " << etaTrk1 << "SCT hits: " << sctHitsTrk1);
        
        // assume that if we found a muon track, the next one is the second muon
        trkIt2 = trkIt1 +1; // for track two
        
        if (trkIt2 != trackVector.end()) {
            if (!(trkIt2->isValid())) {
                ATH_MSG_WARNING("TrackParticleContainer::Invalid ElementLink to track ");
                continue;
            }
            
            if( chainName.find("bBmumux") != std::string::npos && trkIt2->dataID().find("Bphysics_IDTrig") != std::string::npos )
              continue; // not consider ID tracks which appear in Bmumux-like chains
            
            const xAOD::TrackParticle * ptl2 = **trkIt2;
            if (!ptl2) {
                msg() << MSG::WARNING << "TrackParticle with nullptr but valid elementlink" << endmsg;
            }

            if (ptl2) {
                
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

                ATH_MSG_DEBUG("track2 "  << " pt phi eta " << ptTrk2 << " " <<
                              phiTrk2 << " " << etaTrk2 << "SCT hits: " << sctHitsTrk2);

                
                float dphi = absDeltaPhi(phiTrk1,phiTrk2);
                float deta = absDeltaEta(etaTrk1,etaTrk2);
                
                float pTsum = ptTrk1 + ptTrk2;
                float dR = deltaR(etaTrk1, phiTrk1, etaTrk2, phiTrk2);

                TLorentzVector trk1_tl, trk2_tl;
                trk1_tl.SetPtEtaPhiM(ptTrk1, etaTrk1, phiTrk1, m_muonMass);
                trk2_tl.SetPtEtaPhiM(ptTrk2, etaTrk2, phiTrk2, m_muonMass);
                TLorentzVector ditrk = trk1_tl+trk2_tl;

                float ditrk_rapidity = ditrk.Rapidity();
                float ditrk_phi      = ditrk.Phi();

                int qTrk1(1);//, qTrk2(-1);
                if(qOverPTrk1 < 0.){
                    qTrk1 = -1;
                    //qTrk2 = 1;
                }
                auto cur_pair            = std::make_pair (ptl1, ptl2);
                auto cur_pair_rev        = std::make_pair (ptl2, ptl1);
                float phiStar      = phiMethod(trk1_tl, trk2_tl, qTrk1);
                float cosThetaStar = cosMethod(trk1_tl, trk2_tl, qTrk1);
                bool sawThisMuonPair  =std::find(m_muon_pairs_processed.begin(),m_muon_pairs_processed.end(),cur_pair)!=m_muon_pairs_processed.end();
                if(sawThisMuonPair) {
                    continue;
                } else  {
             
                m_muon_pairs_processed.push_back(cur_pair);
                m_muon_pairs_processed.push_back(cur_pair_rev);
            
                //************ SHIFTER ************* //
                setCurrentMonGroup(m_base_path_shifter+"/"+path);
                hist(Form("%s_%s_dR",pref,name))->Fill(dR);
                hist(Form("%s_%s_pTmu1",pref,name))->Fill(ptTrk1/1000.);
                hist(Form("%s_%s_pTmu2",pref,name))->Fill(ptTrk2/1000.);    
                if(fullSetOfHists) {
                    hist(Form("%s_%s_d0mu1",pref,name))->Fill(d0Trk1);
                    hist(Form("%s_%s_d0mu2",pref,name))->Fill(d0Trk2);
                    hist(Form("%s_%s_z0mu1",pref,name))->Fill(z0Trk1);
                    hist(Form("%s_%s_z0mu2",pref,name))->Fill(z0Trk2);
                    hist(Form("%s_%s_dphi",pref,name))->Fill(dphi);
                    hist(Form("%s_%s_deta",pref,name))->Fill(deta);
                    hist(Form("%s_%s_pTsum",pref,name))->Fill(pTsum/1000.);
                }

                 
                //************ EXPERT ************* //
                setCurrentMonGroup(m_base_path_expert+"/"+path);
                hist(Form("%s_%s_phiStar",pref,name))->Fill(phiStar);
                hist(Form("%s_%s_cosThetaStar",pref,name))->Fill(cosThetaStar);
                if(fullSetOfHists) {           
                  hist(Form("%s_%s_numBLayerHitsmu1",pref,name))->Fill(bLayerHitsTrk1);
                  hist(Form("%s_%s_numBLayerHitsmu2",pref,name))->Fill(bLayerHitsTrk2);
                  hist(Form("%s_%s_numSCTHitsmu1",pref,name))->Fill(sctHitsTrk1);
                  hist(Form("%s_%s_numSCTHitsmu2",pref,name))->Fill(sctHitsTrk2);
                  hist(Form("%s_%s_numPixelHitsmu1",pref,name))->Fill(pixHitsTrk1);
                  hist(Form("%s_%s_numPixelHitsmu2",pref,name))->Fill(pixHitsTrk2);
                  hist(Form("%s_%s_numTRTHitsmu1",pref,name))->Fill(trtHitsTrk1);
                  hist(Form("%s_%s_numTRTHitsmu2",pref,name))->Fill(trtHitsTrk2);
                  
                  
                  hist2(Form("%s_%s_phiStarcosThetaStar",pref,name))->Fill(cosThetaStar,   phiStar,1);
                  hist2(Form("%s_%s_phiStarRapidity",pref,name))    ->Fill(ditrk_rapidity, phiStar,1);
                  hist2(Form("%s_%s_phiStarPhi",pref,name))         ->Fill(ditrk_phi,      phiStar,1);
                }
                }
                
            } // valid second track
        } // if a second track
        

    } // loop over the track vector

    
}


void HLTXAODBphysMonTool::divide(TH1 *num, TH1 *den, TH1 *quo) {
    
    quo->Divide(num, den, 1., 1., "B");
}

void HLTXAODBphysMonTool::divide2(TH2 *num, TH2 *den, TH2 *quo) {
    
    quo->Divide(num, den, 1., 1., "B");
}

void HLTXAODBphysMonTool::divide2WithRow(TH2 *num, TH2 *quo,int denom_row ) {
    // use one row as the denominator hist.
    // used for 2-d histograms where one row is the denominator
    
    int nBinsX = num->GetNbinsX();
    int nBinsY = num->GetNbinsY();
    if (denom_row > nBinsY) {
        ATH_MSG_WARNING("divide2WithRow Denominator bin is out of range");
        return;
    }
    
    TH2* tmp = (TH2*)num->Clone("tmphist");
    
    // @todo #FIXME - add under/overflow bins, do fully correct error estimates
    for (int jy = 1; jy <= nBinsY;++jy) {
        if (jy == denom_row) continue;
        for (int ix = 1; ix <= nBinsX;++ix) {
            double numval = num->GetBinContent(ix,denom_row);
            double numerr = num->GetBinError(ix,denom_row);
            tmp->SetBinContent(ix,jy,numval);
            tmp->SetBinError(ix,jy,numerr);
        } // ix
    }//iy
    tmp->Sumw2();
    
    quo->Divide(num,tmp,1.,1.,"B");
    delete tmp; tmp = nullptr;
} //divide2WithRow

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
//
//double HLTXAODBphysMonTool::invariantMassError(const xAOD::Vertex* vxCandidate, std::vector<double> masses) const
//{
//    uint NTrk = vxCandidate->vxTrackAtVertex().size();
//    if (masses.size() != NTrk) {
//        ATH_MSG_WARNING("The provided number of masses does not match the number of tracks in the vertex");
//        return -999999.;
//    }
//    
//    uint ndim    = vxCandidate->covariance().size();
//    uint ndimExp = (3*NTrk+3)*(3*NTrk+3+1)/2;
//    if (ndim == ndimExp) {
//        return massErrorVKalVrt(vxCandidate,masses);
//    } else {
//        ATH_MSG_WARNING("Unknown covariance matrix dimension: " << ndim << ", expected: " << ndimExp);
//        return -999999.;
//    }
//}
//
//// ---------------------------------------------------------------------------------
//// massErrorVKalVrt: returns invariant mass error for vertex created by VKalVrtFitter
//// ---------------------------------------------------------------------------------
//
//double HLTXAODBphysMonTool::massErrorVKalVrt(const xAOD::Vertex * vxCandidate, std::vector<double> masses) const
//{
//    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
//    
//    // construct the full covariance matrix
//    Amg::MatrixX* fullCov = convertVKalCovMatrix(NTrk, vxCandidate->covariance());
//    
//    std::vector<CLHEP::HepLorentzVector> particleMom(NTrk);
//    std::vector<CLHEP::HepMatrix> particleDeriv(NTrk);
//    CLHEP::HepLorentzVector totalMom;
//    CLHEP::HepMatrix tmpDeriv(3,3);
//    
//    double phi=0.,theta=0.,invP=0.;
//    for( unsigned int it=0; it<NTrk; it++){
//        const Trk::TrackParameters*  bPer = vxCandidate->vxTrackAtVertex()[it].perigeeAtVertex();
//        phi   =  bPer->parameters()[Trk::phi];
//        theta =  bPer->parameters()[Trk::theta];
//        invP  =  bPer->parameters()[Trk::qOverP];
//        CLHEP::HepLorentzVector tmp( cos(phi)*sin(theta)/fabs(invP),
//                                    sin(phi)*sin(theta)/fabs(invP),
//                                    cos(theta)/fabs(invP) );
//        double esq = tmp.px()*tmp.px() + tmp.py()*tmp.py() + tmp.pz()*tmp.pz() + masses[it]*masses[it];
//        double e = (esq>0.) ? sqrt(esq) : 0.;
//        tmp.setE(e);
//        particleMom[it] = tmp;
//        totalMom += tmp;
//        
//        //  d(Px,Py,Pz)/d(Phi,Theta,InvP)
//        tmpDeriv(1,1) = - tmp.py();
//        tmpDeriv(2,1) =   tmp.px();
//        tmpDeriv(3,1) =   0.;
//        tmpDeriv(1,2) =   cos(phi) * tmp.pz();
//        tmpDeriv(2,2) =   sin(phi) * tmp.pz();
//        tmpDeriv(3,2) = - sin(theta)/fabs(invP);
//        tmpDeriv(1,3) = - tmp.px()/invP;
//        tmpDeriv(2,3) = - tmp.py()/invP;
//        tmpDeriv(3,3) = - tmp.pz()/invP;
//        particleDeriv[it] = tmpDeriv;
//    }
//    
//    double dMdPx=0., dMdPy=0., dMdPz=0., dMdPhi=0., dMdTheta=0., dMdInvP=0.;
//    std::vector<double> Deriv(3*NTrk+3, 0.);
//    for(unsigned int it=0; it<NTrk; it++){
//        dMdPx = ( totalMom.e() * particleMom[it].px()/particleMom[it].e() - totalMom.px() ) / totalMom.m();
//        dMdPy = ( totalMom.e() * particleMom[it].py()/particleMom[it].e() - totalMom.py() ) / totalMom.m();
//        dMdPz = ( totalMom.e() * particleMom[it].pz()/particleMom[it].e() - totalMom.pz() ) / totalMom.m();
//        
//        dMdPhi   = dMdPx*particleDeriv[it](1,1) + dMdPy*particleDeriv[it](2,1) + dMdPz*particleDeriv[it](3,1);
//        dMdTheta = dMdPx*particleDeriv[it](1,2) + dMdPy*particleDeriv[it](2,2) + dMdPz*particleDeriv[it](3,2);
//        dMdInvP  = dMdPx*particleDeriv[it](1,3) + dMdPy*particleDeriv[it](2,3) + dMdPz*particleDeriv[it](3,3);
//        
//        Deriv[3*it + 3 + 0] = dMdPhi;    Deriv[3*it + 3 + 1] = dMdTheta; Deriv[3*it + 3 + 2] = dMdInvP;
//    }
//    
//    double err = 0;
//    for(unsigned int i=0; i<3*NTrk+3; i++){
//        for(unsigned int j=0; j<3*NTrk+3; j++){
//            err += Deriv[i]*( (*fullCov)(i,j))*Deriv[j];
//        }
//    }
//    
//    // cleanup
//    delete fullCov;
//    
//    return (err>0.) ? sqrt(err) : 0.;
//}

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

//Amg::MatrixX * HLTXAODBphysMonTool::convertVKalCovMatrix(int NTrk, const std::vector<float> & Matrix) const
//{
//    // stolen from VKalVrtFitter
//    Amg::MatrixX * mtx = new Amg::MatrixX(3+3*NTrk,3+3*NTrk);
//    long int ij=0;
//    for(int i=1; i<=(3+3*NTrk); i++){
//        for(int j=1; j<=i; j++){
//            if(i==j) {
//                (*mtx)(i-1,j-1)=Matrix[ij];
//            }else{
//                (*mtx).fillSymmetric(i-1,j-1,Matrix[ij]);
//            }
//            ij++;
//        }
//    }
//    
//    return mtx;
//}

float HLTXAODBphysMonTool::phiMethod(const TLorentzVector & Mu1, const TLorentzVector & Mu2, int Mu1_q) const
{
    
    TLorentzVector MuPlus  = ( Mu1_q > 0 ? Mu1 : Mu2);
    TLorentzVector MuMinus = ( Mu1_q > 0 ? Mu2 : Mu1);
    
    TLorentzVector MuMu = MuPlus + MuMinus;
    
    TVector3 V0 = MuMu.Vect();
    MuPlus.RotateZ(-V0.Phi());
    MuPlus.RotateY(-V0.Theta());
   // if(MuMu.Rapidity() < 0) {MuPlus.RotateZ(-TMath::Pi());
   //     return -atan2(MuPlus.Y(),MuPlus.X());
   // }
    return atan2(MuPlus.Y(),MuPlus.X());
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
    
    //ATH_MSG_DEBUG("pssq = " << pssq << ", ps = " << ps << ", P_MuMu = " << P_MuMu << ", MuMu_mass = " << MuMu_mass);
    
    float cosTheta(-99.);
    if(ps == 0.) 
      ATH_MSG_DEBUG("Two muons with equal momenta - cosTheta will return an unphysical value");
    else
      cosTheta = (float)((MuMu.Energy()*pp - P_MuMu*P_MuMu*MuPlus.Energy())/(MuMu_mass*ps*P_MuMu));
    
    
    return cosTheta;
    
}

float HLTXAODBphysMonTool::deltaR( float eta_1, float phi_1, float eta_2, float phi_2 ) const
{
    float dEta = fabs( eta_1 - eta_2 );
    float dPhi = fabs( phi_1 - phi_2 );
    if ( dPhi > M_PI ){ dPhi = 2*TMath::Pi() - dPhi ;}
    return sqrt( dEta*dEta + dPhi*dPhi ) ;
}


float HLTXAODBphysMonTool::absDeltaPhi(float phi_1, float phi_2 ) const {
    // @todo FIXME - condense if possible
    float dphi = fabs(phi_1 - phi_2);
    
    if( 2.* M_PI -dphi < dphi )
    {
        if( dphi > 0. )
            dphi = dphi - 2.* M_PI;
        else
            dphi = 2* M_PI - dphi;
    }
    return fabs(dphi);
}

float HLTXAODBphysMonTool::absDeltaEta(float eta_1, float eta_2 ) const {
    return fabs(eta_1 - eta_2);
}



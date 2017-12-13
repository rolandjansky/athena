/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopAnalysis/EventSaverFlatNtuple.h"

#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"
#include "TopConfiguration/TopConfig.h"
#include "TopEventSelectionTools/TreeManager.h"

#include "AthContainers/AuxTypeRegistry.h"

#include "TFile.h"
#include <cmath>
#include <iostream>
#include <boost/algorithm/string.hpp>

#include "TopParticleLevel/ParticleLevelEvent.h"
#include "TopParticleLevel/ParticleLevelRCJetObjectLoader.h"

#include "TopFakes/TopFakesMMWeightCalculator.h"

namespace top {

    EventSaverFlatNtuple::EventSaverFlatNtuple() :
        asg::AsgTool( "top::EventSaverFlatNtuple" ),
        m_config(nullptr),
        m_sfRetriever(nullptr),
        m_outputFile(nullptr),
        m_truthTreeManager(nullptr),
        m_particleLevelTreeManager(nullptr),

        m_weight_mc(0.),
        m_weight_pileup(0.),

        // cumulative SF
        m_weight_leptonSF(0.),

        m_weight_leptonSF_EL_SF_Trigger_UP(0.),
        m_weight_leptonSF_EL_SF_Trigger_DOWN(0.),
        m_weight_leptonSF_EL_SF_Reco_UP(0.),
        m_weight_leptonSF_EL_SF_Reco_DOWN(0.),
        m_weight_leptonSF_EL_SF_ID_UP(0.),
        m_weight_leptonSF_EL_SF_ID_DOWN(0.),
        m_weight_leptonSF_EL_SF_Isol_UP(0.),
        m_weight_leptonSF_EL_SF_Isol_DOWN(0.),

        m_weight_leptonSF_MU_SF_Trigger_STAT_UP(0.),
        m_weight_leptonSF_MU_SF_Trigger_STAT_DOWN(0.),
        m_weight_leptonSF_MU_SF_Trigger_SYST_UP(0.),
        m_weight_leptonSF_MU_SF_Trigger_SYST_DOWN(0.),
        // Muon ID SF systematics (regular)
        m_weight_leptonSF_MU_SF_ID_STAT_UP(0.),
        m_weight_leptonSF_MU_SF_ID_STAT_DOWN(0.),
        m_weight_leptonSF_MU_SF_ID_SYST_UP(0.),
        m_weight_leptonSF_MU_SF_ID_SYST_DOWN(0.),
        // Muon ID SF systematics (low pt)
        m_weight_leptonSF_MU_SF_ID_STAT_LOWPT_UP(0.),
        m_weight_leptonSF_MU_SF_ID_STAT_LOWPT_DOWN(0.),
        m_weight_leptonSF_MU_SF_ID_SYST_LOWPT_UP(0.),
        m_weight_leptonSF_MU_SF_ID_SYST_LOWPT_DOWN(0.),
        // Muon isolation SF systematics
        m_weight_leptonSF_MU_SF_Isol_STAT_UP(0.),
        m_weight_leptonSF_MU_SF_Isol_STAT_DOWN(0.),
        m_weight_leptonSF_MU_SF_Isol_SYST_UP(0.),
        m_weight_leptonSF_MU_SF_Isol_SYST_DOWN(0.),
        m_weight_leptonSF_MU_SF_TTVA_STAT_UP(0.),
        m_weight_leptonSF_MU_SF_TTVA_STAT_DOWN(0.),
        m_weight_leptonSF_MU_SF_TTVA_SYST_UP(0.),
        m_weight_leptonSF_MU_SF_TTVA_SYST_DOWN(0.),

        // individual components electrons
        m_weight_indiv_SF_EL_Trigger(0.),
        m_weight_indiv_SF_EL_Trigger_UP(0.),
        m_weight_indiv_SF_EL_Trigger_DOWN(0.),
        m_weight_indiv_SF_EL_Reco(0.),
        m_weight_indiv_SF_EL_Reco_UP(0.),
        m_weight_indiv_SF_EL_Reco_DOWN(0.),
        m_weight_indiv_SF_EL_ID(0.),
        m_weight_indiv_SF_EL_ID_UP(0.),
        m_weight_indiv_SF_EL_ID_DOWN(0.),
        m_weight_indiv_SF_EL_Isol(0.),
        m_weight_indiv_SF_EL_Isol_UP(0.),
        m_weight_indiv_SF_EL_Isol_DOWN(0.),
        m_weight_indiv_SF_EL_ChargeID(0.),
        m_weight_indiv_SF_EL_ChargeID_UP(0.),
        m_weight_indiv_SF_EL_ChargeID_DOWN(0.),
        m_weight_indiv_SF_EL_ChargeMisID(0.),
        m_weight_indiv_SF_EL_ChargeMisID_STAT_UP(0.),
        m_weight_indiv_SF_EL_ChargeMisID_STAT_DOWN(0.),
        m_weight_indiv_SF_EL_ChargeMisID_SYST_UP(0.),
        m_weight_indiv_SF_EL_ChargeMisID_SYST_DOWN(0.),

        m_weight_indiv_SF_MU_Trigger(0.),
        m_weight_indiv_SF_MU_Trigger_STAT_UP(0.),
        m_weight_indiv_SF_MU_Trigger_STAT_DOWN(0.),
        m_weight_indiv_SF_MU_Trigger_SYST_UP(0.),
        m_weight_indiv_SF_MU_Trigger_SYST_DOWN(0.),
        // Muon ID SF systematics (regular)
        m_weight_indiv_SF_MU_ID(0.),
        m_weight_indiv_SF_MU_ID_STAT_UP(0.),
        m_weight_indiv_SF_MU_ID_STAT_DOWN(0.),
        m_weight_indiv_SF_MU_ID_SYST_UP(0.),
        m_weight_indiv_SF_MU_ID_SYST_DOWN(0.),
        // Muon ID SF systematics (low pt)
        m_weight_indiv_SF_MU_ID_STAT_LOWPT_UP(0.),
        m_weight_indiv_SF_MU_ID_STAT_LOWPT_DOWN(0.),
        m_weight_indiv_SF_MU_ID_SYST_LOWPT_UP(0.),
        m_weight_indiv_SF_MU_ID_SYST_LOWPT_DOWN(0.),
        // Muon isolation SF systematics
        m_weight_indiv_SF_MU_Isol(0.),
        m_weight_indiv_SF_MU_Isol_SYST_UP(0.),
        m_weight_indiv_SF_MU_Isol_SYST_DOWN(0.),
        m_weight_indiv_SF_MU_Isol_STAT_UP(0.),
        m_weight_indiv_SF_MU_Isol_STAT_DOWN(0.),
        m_weight_indiv_SF_MU_TTVA(0.),
        m_weight_indiv_SF_MU_TTVA_SYST_UP(0.),
        m_weight_indiv_SF_MU_TTVA_SYST_DOWN(0.),
        m_weight_indiv_SF_MU_TTVA_STAT_UP(0.),
        m_weight_indiv_SF_MU_TTVA_STAT_DOWN(0.),

        m_fakesMM_weights(),

        m_eventNumber(0),
        m_runNumber(0),
        m_randomRunNumber(0),
        m_mcChannelNumber(0),

        m_mu_original(0.),
        m_mu(0.),

        m_backgroundFlags(0),
	m_hasBadMuon(0),
	m_makeRCJets(false),
	m_makeVarRCJets(false),
	m_met_met(0.),
        m_met_phi(0.)
    {
    }

    EventSaverFlatNtuple::~EventSaverFlatNtuple()
    {
        for (std::unordered_map<std::string,int*>::iterator i=m_extraTruthVars_int.begin();i!=m_extraTruthVars_int.end();++i)
            delete (*i).second;

        for (std::unordered_map<std::string,float*>::iterator i=m_extraTruthVars_float.begin();i!=m_extraTruthVars_float.end();++i)
            delete (*i).second;
    }

    std::shared_ptr<top::TopConfig> EventSaverFlatNtuple::topConfig()
    {
        return m_config;
    }

    std::vector<std::shared_ptr<top::TreeManager>> EventSaverFlatNtuple::treeManagers()
    {
        return m_treeManagers;
    }

    std::shared_ptr<top::TreeManager> EventSaverFlatNtuple::truthTreeManager()
    {
        return m_truthTreeManager;
    }

    std::shared_ptr<top::TreeManager> EventSaverFlatNtuple::particleLevelTreeManager()
    {
        return m_particleLevelTreeManager;
    }

    std::shared_ptr<top::ScaleFactorRetriever> EventSaverFlatNtuple::scaleFactorRetriever()
    {
        return m_sfRetriever;
    }

    std::vector<top::TreeManager::BranchFilter> & EventSaverFlatNtuple::branchFilters()
    {
        return m_branchFilters;
    }

    void EventSaverFlatNtuple::initialize(std::shared_ptr<top::TopConfig> config, TFile* file, const std::vector<std::string>& extraBranches) {
        m_config = config;
        m_outputFile = file;
        m_extraBranches = extraBranches;
        m_selectionDecisions.resize(m_extraBranches.size());


	// fixed-R re-clustering (RC)
	if (config->useRCJets() == true){
	  m_makeRCJets = true;
	  m_rc = std::unique_ptr<RCJetMC15> ( new RCJetMC15( "RCJetMC15" ) );
	  top::check(m_rc->setProperty( "config" , config ) , "Failed to set config property of RCJetMC15");
	  top::check(m_rc->initialize(),"Failed to initialize RCJetMC15");
	}

	// variable-R re-clustering (VarRC)
	if (config->useVarRCJets() == true){
	  m_makeVarRCJets = true;
	  m_VarRCjetBranches.clear();    // clear map of branches just in case
	  m_VarRCjetsubBranches.clear();

	  boost::split(m_VarRCJetRho, config->VarRCJetRho(), boost::is_any_of(","));
	  boost::split(m_VarRCJetMassScale, config->VarRCJetMassScale(), boost::is_any_of(","));

	  for (auto& rho : m_VarRCJetRho){
            for (auto& mass_scale : m_VarRCJetMassScale){
	      std::replace( rho.begin(), rho.end(), '.', '_');
	      std::string name = rho+mass_scale;
	      m_VarRC[name] = std::unique_ptr<RCJetMC15> ( new RCJetMC15( "VarRCJetMC15_"+name ) );
	      top::check(m_VarRC[name]->setProperty( "config" , config ) , "Failed to set config property of VarRCJetMC15");
	      top::check(m_VarRC[name]->setProperty( "VarRCjets", true ) , "Failed to set VarRCjets property of VarRCJetMC15");
	      top::check(m_VarRC[name]->setProperty( "VarRCjets_rho",  rho ) , "Failed to set VarRCjets rho property of VarRCJetMC15");
	      top::check(m_VarRC[name]->setProperty( "VarRCjets_mass_scale", mass_scale ) , "Failed to set VarRCjets mass scale property of VarRCJetMC15");
	      top::check(m_VarRC[name]->initialize(),"Failed to initialize VarRCJetMC15");
            } // end loop over mass scale parameters (e.g., top mass, w mass, etc.)
	  } // end loop over mass scale multiplies (e.g., 1.,2.,etc.)
	} // end make VarRC jets


        //make a tree for each systematic
        std::string nominalTTreeName("SetMe"),nominalLooseTTreeName("SetMe");
        if (m_config->doTightEvents() && !m_config->HLLHC()) {
          for (auto treeName : *config->systAllTTreeNames()) {
              m_treeManagers.push_back(std::shared_ptr<top::TreeManager>( new top::TreeManager(treeName.second, file , m_config->outputFileSetAutoFlushZero()) ) );
              m_treeManagers.back()->branchFilters() = branchFilters();
              if (treeName.first == m_config->nominalHashValue()) {
                  nominalTTreeName = treeName.second;
              }
          }
        }

        if (m_config->doLooseEvents()) {
            for (auto treeName : *config->systAllTTreeNames()) {
                m_treeManagers.push_back(std::shared_ptr<top::TreeManager>( new top::TreeManager(treeName.second+"_Loose", file , m_config->outputFileSetAutoFlushZero()) ) );
                m_treeManagers.back()->branchFilters() = branchFilters();
                if (treeName.first == m_config->nominalHashValue()) {
                    nominalLooseTTreeName = treeName.second+"_Loose";
                }
            }
        }

        // Truth tree
        if (m_config->isMC()) {

            m_sfRetriever = std::unique_ptr<top::ScaleFactorRetriever> ( new top::ScaleFactorRetriever( m_config ) );

            m_truthTreeManager = std::shared_ptr<top::TreeManager>( new top::TreeManager( "truth" , file , m_config->outputFileSetAutoFlushZero()) );
            m_truthTreeManager->branchFilters() = branchFilters();
            m_truthTreeManager->makeOutputVariable(m_weight_mc, "weight_mc");
            m_truthTreeManager->makeOutputVariable(m_eventNumber, "eventNumber");
            m_truthTreeManager->makeOutputVariable(m_runNumber, "runNumber");
            m_truthTreeManager->makeOutputVariable(m_mu, "mu");
            m_truthTreeManager->makeOutputVariable(m_weight_pileup, "weight_pileup");
            if (m_config->isMC() && m_config->doPileupReweighting()) m_truthTreeManager->makeOutputVariable(m_randomRunNumber, "randomRunNumber");
            m_truthTreeManager->makeOutputVariable(m_mcChannelNumber, "mcChannelNumber");

            // Only if you really really want it - this is BIG
            if (m_config->useTruthParticles() && m_config->doTruthBlockInfo() ) {
                m_truthTreeManager->makeOutputVariable(m_mc_pt, "mc_pt");
                m_truthTreeManager->makeOutputVariable(m_mc_eta, "mc_eta");
                m_truthTreeManager->makeOutputVariable(m_mc_phi, "mc_phi");
                m_truthTreeManager->makeOutputVariable(m_mc_e, "mc_e");
                m_truthTreeManager->makeOutputVariable(m_mc_pdgId, "mc_pdgId");
            }

            // PDF information
            if (m_config->doTruthPDFInfo()) {
                if (m_config->doTruthPDFInfoInNominalTrees()) {//if PDF info requested to be saved in nominal trees of truth one
                    for (auto systematicTree : m_treeManagers) {//loop on systematic trees to only get the nominal ones
                        if (systematicTree->name() == nominalTTreeName || systematicTree->name() == nominalLooseTTreeName) {
                            systematicTree->makeOutputVariable(m_PDFinfo_X1,"PDFinfo_X1");
                            systematicTree->makeOutputVariable(m_PDFinfo_X2,"PDFinfo_X2");
                            systematicTree->makeOutputVariable(m_PDFinfo_PDGID1,"PDFinfo_PDGID1");
                            systematicTree->makeOutputVariable(m_PDFinfo_PDGID2,"PDFinfo_PDGID2");
                            systematicTree->makeOutputVariable(m_PDFinfo_Q,"PDFinfo_Q");
                            systematicTree->makeOutputVariable(m_PDFinfo_XF1,"PDFinfo_XF1");
                            systematicTree->makeOutputVariable(m_PDFinfo_XF2,"PDFinfo_XF2");
                        }
                    }
                }
                else {
                    m_truthTreeManager->makeOutputVariable(m_PDFinfo_X1,"PDFinfo_X1");
                    m_truthTreeManager->makeOutputVariable(m_PDFinfo_X2,"PDFinfo_X2");
                    m_truthTreeManager->makeOutputVariable(m_PDFinfo_PDGID1,"PDFinfo_PDGID1");
                    m_truthTreeManager->makeOutputVariable(m_PDFinfo_PDGID2,"PDFinfo_PDGID2");
                    m_truthTreeManager->makeOutputVariable(m_PDFinfo_Q,"PDFinfo_Q");
                    m_truthTreeManager->makeOutputVariable(m_PDFinfo_XF1,"PDFinfo_XF1");
                    m_truthTreeManager->makeOutputVariable(m_PDFinfo_XF2,"PDFinfo_XF2");
                }
            }

            // MC Generator weights
            if (m_config->doMCGeneratorWeights()) {
                if (m_config->doMCGeneratorWeightsInNominalTrees()) {//if MCGeneratorWeights requested to be saved in nominal trees of truth one
                    for (auto systematicTree : m_treeManagers) {//loop on systematic trees to only get the nominal ones
                        if (systematicTree->name() == nominalTTreeName || systematicTree->name() == nominalLooseTTreeName) {
			  systematicTree->makeOutputVariable(m_mc_generator_weights,"mc_generator_weights");
			}
		    }
		}
		else {
		  m_truthTreeManager->makeOutputVariable(m_mc_generator_weights,"mc_generator_weights");
		}
	    }

            if( m_config->saveLHAPDFEvent() ){
                for( const auto& pdfset : m_config->LHAPDFSets() )
                    m_PDF_eventWeights[ pdfset ] = std::vector<float>();

                for( auto& pdf : m_PDF_eventWeights ) {
                    if (m_config->doLHAPDFInNominalTrees()) {//if PDF weights requested to be saved in nominal trees instead of truth one
                        for (auto systematicTree : m_treeManagers) {//loop on systematic trees to only get the nominal ones
                            if (systematicTree->name() == nominalTTreeName || systematicTree->name() == nominalLooseTTreeName)
                                systematicTree->makeOutputVariable( pdf.second, pdf.first );
                        }
                    }
                    else {
                        m_truthTreeManager->makeOutputVariable( pdf.second, pdf.first );
                    }
                }
            }

        }

        //loop over systematics and attach variables
        for (auto systematicTree : m_treeManagers) {

            if (m_config->isMC()) {

                systematicTree->makeOutputVariable(m_weight_mc,     "weight_mc");

                //some event weights
                systematicTree->makeOutputVariable(m_weight_pileup, "weight_pileup");
                systematicTree->makeOutputVariable(m_weight_leptonSF , "weight_leptonSF");

                if (m_config->usePhotons())
                  systematicTree->makeOutputVariable(m_weight_photonSF,
                                                     "weight_photonSF");

                if (m_config->useTaus())
                  systematicTree->makeOutputVariable(m_weight_tauSF,
                                                     "weight_tauSF");

                // nominal b-tagging SFs
                for( auto& tagWP : m_config -> bTagWP_available()){
                  // skip uncalibrated though available WPs
                  if (std::find(m_config->bTagWP_calibrated().begin(), m_config->bTagWP_calibrated().end(), tagWP) == m_config->bTagWP_calibrated().end()) continue;
                  m_weight_bTagSF[tagWP] = 0.;
                  systematicTree->makeOutputVariable(m_weight_bTagSF[tagWP] , "weight_bTagSF_"+shortBtagWP(tagWP));
                }
                if( m_config->useTrackJets() ) {
                  for( auto& tagWP : m_config -> bTagWP_available_trkJet()) {
                    // skip uncalibrated though available WPs
                    if (std::find(m_config->bTagWP_calibrated().begin(), m_config->bTagWP_calibrated().end(), tagWP) == m_config->bTagWP_calibrated().end()) continue;
                    m_weight_trackjet_bTagSF[tagWP] = 0.;
                    systematicTree->makeOutputVariable(m_weight_trackjet_bTagSF[tagWP] , "weight_trackjet_bTagSF_"+shortBtagWP(tagWP));
                  }
                }

                systematicTree->makeOutputVariable(m_weight_jvt, "weight_jvt");

                if (m_config->isSherpa22Vjets())
                  systematicTree->makeOutputVariable(m_weight_sherpa_22_vjets,
                                                     "weight_sherpa_22_vjets");

                if (systematicTree->name() == nominalTTreeName || systematicTree->name() == nominalLooseTTreeName) {

                    systematicTree->makeOutputVariable(m_weight_pileup_UP	, "weight_pileup_UP");
                    systematicTree->makeOutputVariable(m_weight_pileup_DOWN	, "weight_pileup_DOWN");

                    systematicTree->makeOutputVariable(m_weight_leptonSF_EL_SF_Trigger_UP,   "weight_leptonSF_EL_SF_Trigger_UP");
                    systematicTree->makeOutputVariable(m_weight_leptonSF_EL_SF_Trigger_DOWN, "weight_leptonSF_EL_SF_Trigger_DOWN");
                    systematicTree->makeOutputVariable(m_weight_leptonSF_EL_SF_Reco_UP,      "weight_leptonSF_EL_SF_Reco_UP");
                    systematicTree->makeOutputVariable(m_weight_leptonSF_EL_SF_Reco_DOWN,    "weight_leptonSF_EL_SF_Reco_DOWN");
                    systematicTree->makeOutputVariable(m_weight_leptonSF_EL_SF_ID_UP,        "weight_leptonSF_EL_SF_ID_UP");
                    systematicTree->makeOutputVariable(m_weight_leptonSF_EL_SF_ID_DOWN,      "weight_leptonSF_EL_SF_ID_DOWN");
                    systematicTree->makeOutputVariable(m_weight_leptonSF_EL_SF_Isol_UP,      "weight_leptonSF_EL_SF_Isol_UP");
                    systematicTree->makeOutputVariable(m_weight_leptonSF_EL_SF_Isol_DOWN,    "weight_leptonSF_EL_SF_Isol_DOWN");

                    systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_Trigger_STAT_UP,   "weight_leptonSF_MU_SF_Trigger_STAT_UP");
                    systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_Trigger_STAT_DOWN, "weight_leptonSF_MU_SF_Trigger_STAT_DOWN");
                    systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_Trigger_SYST_UP,   "weight_leptonSF_MU_SF_Trigger_SYST_UP");
                    systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_Trigger_SYST_DOWN, "weight_leptonSF_MU_SF_Trigger_SYST_DOWN");
                    // Muon ID SF systematics (regular)
                    systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_ID_STAT_UP,   "weight_leptonSF_MU_SF_ID_STAT_UP");
                    systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_ID_STAT_DOWN, "weight_leptonSF_MU_SF_ID_STAT_DOWN");
                    systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_ID_SYST_UP,   "weight_leptonSF_MU_SF_ID_SYST_UP");
                    systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_ID_SYST_DOWN, "weight_leptonSF_MU_SF_ID_SYST_DOWN");
                    // Muon ID SF systematics (low pt)
                    systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_ID_STAT_LOWPT_UP,   "weight_leptonSF_MU_SF_ID_STAT_LOWPT_UP");
                    systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_ID_STAT_LOWPT_DOWN, "weight_leptonSF_MU_SF_ID_STAT_LOWPT_DOWN");
                    systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_ID_SYST_LOWPT_UP,   "weight_leptonSF_MU_SF_ID_SYST_LOWPT_UP");
                    systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_ID_SYST_LOWPT_DOWN, "weight_leptonSF_MU_SF_ID_SYST_LOWPT_DOWN");
                    // Muon isolation SF systematics
                    systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_Isol_STAT_UP,      "weight_leptonSF_MU_SF_Isol_STAT_UP");
                    systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_Isol_STAT_DOWN,    "weight_leptonSF_MU_SF_Isol_STAT_DOWN");
                    systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_Isol_SYST_UP,      "weight_leptonSF_MU_SF_Isol_SYST_UP");
                    systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_Isol_SYST_DOWN,    "weight_leptonSF_MU_SF_Isol_SYST_DOWN");
                    systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_TTVA_STAT_UP,      "weight_leptonSF_MU_SF_TTVA_STAT_UP");
                    systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_TTVA_STAT_DOWN,    "weight_leptonSF_MU_SF_TTVA_STAT_DOWN");
                    systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_TTVA_SYST_UP,      "weight_leptonSF_MU_SF_TTVA_SYST_UP");
                    systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_TTVA_SYST_DOWN,    "weight_leptonSF_MU_SF_TTVA_SYST_DOWN");


                    // write also out the individual components:

                    systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_Trigger,         "weight_indiv_SF_EL_Trigger");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_Trigger_UP,      "weight_indiv_SF_EL_Trigger_UP");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_Trigger_DOWN,    "weight_indiv_SF_EL_Trigger_DOWN");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_Reco,            "weight_indiv_SF_EL_Reco");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_Reco_UP,         "weight_indiv_SF_EL_Reco_UP");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_Reco_DOWN,       "weight_indiv_SF_EL_Reco_DOWN");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_ID,              "weight_indiv_SF_EL_ID");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_ID_UP,           "weight_indiv_SF_EL_ID_UP");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_ID_DOWN,         "weight_indiv_SF_EL_ID_DOWN");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_Isol,            "weight_indiv_SF_EL_Isol");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_Isol_UP,         "weight_indiv_SF_EL_Isol_UP");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_Isol_DOWN,       "weight_indiv_SF_EL_Isol_DOWN");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_ChargeID,        "weight_indiv_SF_EL_ChargeID");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_ChargeID_UP,     "weight_indiv_SF_EL_ChargeID_UP");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_ChargeID_DOWN,   "weight_indiv_SF_EL_ChargeID_DOWN");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_ChargeMisID,     "weight_indiv_SF_EL_ChargeMisID");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_ChargeMisID_STAT_UP,  "weight_indiv_SF_EL_ChargeMisID_STAT_UP");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_ChargeMisID_STAT_DOWN,"weight_indiv_SF_EL_ChargeMisID_STAT_DOWN");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_ChargeMisID_SYST_UP,  "weight_indiv_SF_EL_ChargeMisID_SYST_UP");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_ChargeMisID_SYST_DOWN,"weight_indiv_SF_EL_ChargeMisID_SYST_DOWN");

                    systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_Trigger,         "weight_indiv_SF_MU_Trigger");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_Trigger_STAT_UP,      "weight_indiv_SF_MU_Trigger_STAT_UP");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_Trigger_STAT_DOWN,    "weight_indiv_SF_MU_Trigger_STAT_DOWN");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_Trigger_SYST_UP,      "weight_indiv_SF_MU_Trigger_SYST_UP");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_Trigger_SYST_DOWN,    "weight_indiv_SF_MU_Trigger_SYST_DOWN");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_ID,              "weight_indiv_SF_MU_ID");
                    // Muon ID SF systematics (regular)
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_ID_STAT_UP,      "weight_indiv_SF_MU_ID_STAT_UP");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_ID_STAT_DOWN,    "weight_indiv_SF_MU_ID_STAT_DOWN");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_ID_SYST_UP,      "weight_indiv_SF_MU_ID_SYST_UP");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_ID_SYST_DOWN,    "weight_indiv_SF_MU_ID_SYST_DOWN");
                    // Muon ID SF systematics (low pt)
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_ID_STAT_LOWPT_UP,      "weight_indiv_SF_MU_ID_STAT_LOWPT_UP");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_ID_STAT_LOWPT_DOWN,    "weight_indiv_SF_MU_ID_STAT_LOWPT_DOWN");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_ID_SYST_LOWPT_UP,      "weight_indiv_SF_MU_ID_SYST_LOWPT_UP");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_ID_SYST_LOWPT_DOWN,    "weight_indiv_SF_MU_ID_SYST_LOWPT_DOWN");
                    // Muon isolation SF systematics
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_Isol,            "weight_indiv_SF_MU_Isol");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_Isol_STAT_UP,    "weight_indiv_SF_MU_Isol_STAT_UP");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_Isol_STAT_DOWN,  "weight_indiv_SF_MU_Isol_STAT_DOWN");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_Isol_SYST_UP,    "weight_indiv_SF_MU_Isol_SYST_UP");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_Isol_SYST_DOWN,  "weight_indiv_SF_MU_Isol_SYST_DOWN");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_TTVA,            "weight_indiv_SF_MU_TTVA");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_TTVA_STAT_UP,    "weight_indiv_SF_MU_TTVA_STAT_UP");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_TTVA_STAT_DOWN,  "weight_indiv_SF_MU_TTVA_STAT_DOWN");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_TTVA_SYST_UP,    "weight_indiv_SF_MU_TTVA_SYST_UP");
                    systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_TTVA_SYST_DOWN,  "weight_indiv_SF_MU_TTVA_SYST_DOWN");

                    if (m_config->useTaus()) {
                      systematicTree->makeOutputVariable(m_weight_tauSF_ELEOLR_UP,
                                                         "weight_tauSF_ELEOLR_UP");
                      systematicTree->makeOutputVariable(m_weight_tauSF_ELEOLR_DOWN,
                                                         "weight_tauSF_ELEOLR_DOWN");
                      systematicTree->makeOutputVariable(m_weight_tauSF_TRUEELECTRON_ELEOLR_UP,
                                                         "weight_tauSF_TRUEELECTRON_ELEOLR_UP");
                      systematicTree->makeOutputVariable(m_weight_tauSF_TRUEELECTRON_ELEOLR_DOWN,
                                                         "weight_tauSF_TRUEELECTRON_ELEOLR_DOWN");
                      systematicTree->makeOutputVariable(m_weight_tauSF_JETID_UP,
                                                         "weight_tauSF_JETID_UP");
                      systematicTree->makeOutputVariable(m_weight_tauSF_JETID_DOWN,
                                                         "weight_tauSF_JETID_DOWN");
                      systematicTree->makeOutputVariable(m_weight_tauSF_JETID_HIGHPT_UP,
                                                         "weight_tauSF_JETID_HIGHPT_UP");
                      systematicTree->makeOutputVariable(m_weight_tauSF_JETID_HIGHPT_DOWN,
                                                         "weight_tauSF_JETID_HIGHPT_DOWN");
                      systematicTree->makeOutputVariable(m_weight_tauSF_RECO_UP,
                                                         "weight_tauSF_RECO_UP");
                      systematicTree->makeOutputVariable(m_weight_tauSF_RECO_DOWN,
                                                         "weight_tauSF_RECO_DOWN");
                      systematicTree->makeOutputVariable(m_weight_tauSF_RECO_HIGHPT_UP,
                                                         "weight_tauSF_RECO_HIGHPT_UP");
                      systematicTree->makeOutputVariable(m_weight_tauSF_RECO_HIGHPT_DOWN,
                                                         "weight_tauSF_RECO_HIGHPT_DOWN");
                    }

                    if (m_config->usePhotons()) {
                      systematicTree->makeOutputVariable(m_weight_photonSF_ID_UP,
                                                         "weight_photonSF_ID_UP");
                      systematicTree->makeOutputVariable(m_weight_photonSF_ID_DOWN,
                                                         "weight_photonSF_ID_DOWN");
                      systematicTree->makeOutputVariable(m_weight_photonSF_effIso,
                                                         "weight_photonSF_effIso");
                      systematicTree->makeOutputVariable(m_weight_photonSF_effLowPtIso_UP,
                                                         "weight_photonSF_effLowPtIso_UP");
                      systematicTree->makeOutputVariable(m_weight_photonSF_effLowPtIso_DOWN,
                                                         "weight_photonSF_effLowPtIso_DOWN");
                      systematicTree->makeOutputVariable(m_weight_photonSF_effTrkIso_UP,
                                                         "weight_photonSF_effTrkIso_UP");
                      systematicTree->makeOutputVariable(m_weight_photonSF_effTrkIso_DOWN,
                                                         "weight_photonSF_effTrkIso_DOWN");
                    }

                    systematicTree->makeOutputVariable(m_weight_jvt_up, "weight_jvt_UP");
                    systematicTree->makeOutputVariable(m_weight_jvt_down, "weight_jvt_DOWN");

                }

                // for b-tagging SFs, can also have systematic-shifted in systematics trees
                if (systematicTree->name() == nominalTTreeName || systematicTree->name() == nominalLooseTTreeName || m_config->dumpBtagSystsInSystTrees()) {
                    // b-tagging SFs: eigenvectors and named systematics
                    for( auto& tagWP : m_config -> bTagWP_available()) {
                      // skip uncalibrated though available WPs
                      if (std::find(m_config->bTagWP_calibrated().begin(), m_config->bTagWP_calibrated().end(), tagWP) == m_config->bTagWP_calibrated().end()) continue;
                      // up
                      systematicTree->makeOutputVariable(m_weight_bTagSF_eigen_B_up[tagWP], "weight_bTagSF_"+shortBtagWP(tagWP)+"_eigenvars_B_up");
                      systematicTree->makeOutputVariable(m_weight_bTagSF_eigen_C_up[tagWP], "weight_bTagSF_"+shortBtagWP(tagWP)+"_eigenvars_C_up");
                      systematicTree->makeOutputVariable(m_weight_bTagSF_eigen_Light_up[tagWP], "weight_bTagSF_"+shortBtagWP(tagWP)+"_eigenvars_Light_up");
                      // down
                      systematicTree->makeOutputVariable(m_weight_bTagSF_eigen_B_down[tagWP], "weight_bTagSF_"+shortBtagWP(tagWP)+"_eigenvars_B_down");
                      systematicTree->makeOutputVariable(m_weight_bTagSF_eigen_C_down[tagWP], "weight_bTagSF_"+shortBtagWP(tagWP)+"_eigenvars_C_down");
                      systematicTree->makeOutputVariable(m_weight_bTagSF_eigen_Light_down[tagWP], "weight_bTagSF_"+shortBtagWP(tagWP)+"_eigenvars_Light_down");
                      // named systematics
                      for (auto name : m_config->btagging_namedSysts(tagWP)) {
                        systematicTree->makeOutputVariable(m_weight_bTagSF_named_up[tagWP][name], "weight_bTagSF_"+shortBtagWP(tagWP)+"_"+betterBtagNamedSyst(name)+"_up" );
                        systematicTree->makeOutputVariable(m_weight_bTagSF_named_down[tagWP][name], "weight_bTagSF_"+shortBtagWP(tagWP)+"_"+betterBtagNamedSyst(name)+"_down" );
                      }
                    }
                    if( m_config->useTrackJets() ) {
                      for( auto& tagWP : m_config -> bTagWP_available_trkJet()) {
                        // skip uncalibrated though available WPs
                        if (std::find(m_config->bTagWP_calibrated().begin(), m_config->bTagWP_calibrated().end(), tagWP) == m_config->bTagWP_calibrated().end()) continue;
                        // up
                        systematicTree->makeOutputVariable(m_weight_trackjet_bTagSF_eigen_B_up[tagWP], "weight_trackjet_bTagSF_"+shortBtagWP(tagWP)+"_eigenvars_B_up");
                        systematicTree->makeOutputVariable(m_weight_trackjet_bTagSF_eigen_C_up[tagWP], "weight_trackjet_bTagSF_"+shortBtagWP(tagWP)+"_eigenvars_C_up");
                        systematicTree->makeOutputVariable(m_weight_trackjet_bTagSF_eigen_Light_up[tagWP], "weight_trackjet_bTagSF_"+shortBtagWP(tagWP)+"_eigenvars_Light_up");
                        // down
                        systematicTree->makeOutputVariable(m_weight_trackjet_bTagSF_eigen_B_down[tagWP], "weight_trackjet_bTagSF_"+shortBtagWP(tagWP)+"_eigenvars_B_down");
                        systematicTree->makeOutputVariable(m_weight_trackjet_bTagSF_eigen_C_down[tagWP], "weight_trackjet_bTagSF_"+shortBtagWP(tagWP)+"_eigenvars_C_down");
                        systematicTree->makeOutputVariable(m_weight_trackjet_bTagSF_eigen_Light_down[tagWP], "weight_trackjet_bTagSF_"+shortBtagWP(tagWP)+"_eigenvars_Light_down");
                        // named systematics
                        for (auto name : m_config->trkjet_btagging_namedSysts(tagWP)) {
                          systematicTree->makeOutputVariable(m_weight_trackjet_bTagSF_named_up[tagWP][name], "weight_trackjet_bTagSF_"+shortBtagWP(tagWP)+"_"+betterBtagNamedSyst(name)+"_up" );
                          systematicTree->makeOutputVariable(m_weight_trackjet_bTagSF_named_down[tagWP][name], "weight_trackjet_bTagSF_"+shortBtagWP(tagWP)+"_"+betterBtagNamedSyst(name)+"_down" );
                        }
                      }
                    }
                }
            }

            ///-- weights for matrix-method fakes estimate --///
            if (!m_config->isMC() && systematicTree->name().find("Loose") != std::string::npos && m_config->doFakesMMWeights()) {
		top::TopFakesMMWeightCalculator const* fakesMMWeightCalc(nullptr);
		if ( asg::ToolStore::contains<top::TopFakesMMWeightCalculator>("MMWeightCalculator") ) {
		  fakesMMWeightCalc = asg::ToolStore::get<top::TopFakesMMWeightCalculator>("MMWeightCalculator");
		}
		else {
		  ATH_MSG_ERROR("EventSaverFlatNtuple::initialize" );
		  throw std::runtime_error("Unable to retrieve top::TopFakesMMWeightCalculator tool");
		}
		for(const auto& branchName : m_extraBranches){//loop on selections
		  for(const auto& conf : fakesMMWeightCalc->GetFakesMMConfigNames(branchName)) {
		    std::string MMweight_branch_name = "fakesMM_weight_" + branchName + "_" + conf;
		    systematicTree->makeOutputVariable(m_fakesMM_weights[branchName][conf], MMweight_branch_name);
		  }
		}
            }

	    /// Bootstrapping poisson weights
	    if (m_config->saveBootstrapWeights()){
	      systematicTree->makeOutputVariable(m_weight_poisson, "weight_poisson");
	    }

            //event info
            systematicTree->makeOutputVariable(m_eventNumber,     "eventNumber");
            systematicTree->makeOutputVariable(m_runNumber,       "runNumber");
            if (m_config->isMC() && m_config->doPileupReweighting()) systematicTree->makeOutputVariable(m_randomRunNumber, "randomRunNumber");
            systematicTree->makeOutputVariable(m_mcChannelNumber, "mcChannelNumber");
            systematicTree->makeOutputVariable(m_mu,              "mu");
            if (!m_config->isMC()) {
                systematicTree->makeOutputVariable(m_mu_original,       "mu_original_xAOD");
            }
            systematicTree->makeOutputVariable(m_backgroundFlags, "backgroundFlags");
            if (m_config->useMuons())
	      systematicTree->makeOutputVariable(m_hasBadMuon, "hasBadMuon");

            //electrons
            if (m_config->useElectrons()) {
                systematicTree->makeOutputVariable(m_el_pt,      "el_pt");
                systematicTree->makeOutputVariable(m_el_eta,     "el_eta");
                systematicTree->makeOutputVariable(m_el_cl_eta,  "el_cl_eta");
                systematicTree->makeOutputVariable(m_el_phi,     "el_phi");
                systematicTree->makeOutputVariable(m_el_e,       "el_e");
                systematicTree->makeOutputVariable(m_el_charge,  "el_charge");
                systematicTree->makeOutputVariable(m_el_topoetcone20, "el_topoetcone20");
                systematicTree->makeOutputVariable(m_el_ptvarcone20, "el_ptvarcone20");
                if (systematicTree->name().find("Loose") != std::string::npos) {
                    systematicTree->makeOutputVariable(m_el_isTight, "el_isTight");
                }
                systematicTree->makeOutputVariable(m_el_CF, "el_CF");
                systematicTree->makeOutputVariable(m_el_d0sig, "el_d0sig");
                systematicTree->makeOutputVariable(m_el_delta_z0_sintheta, "el_delta_z0_sintheta");
              if (m_config->isMC()) {
                systematicTree->makeOutputVariable(m_el_true_type,      "el_true_type");
                systematicTree->makeOutputVariable(m_el_true_origin,    "el_true_origin");
                systematicTree->makeOutputVariable(m_el_true_typebkg,   "el_true_typebkg");
                systematicTree->makeOutputVariable(m_el_true_originbkg, "el_true_originbkg");
              }
            }

            //muons
            if (m_config->useMuons()) {
                systematicTree->makeOutputVariable(m_mu_pt,      "mu_pt");
                systematicTree->makeOutputVariable(m_mu_eta,     "mu_eta");
                systematicTree->makeOutputVariable(m_mu_phi,     "mu_phi");
                systematicTree->makeOutputVariable(m_mu_e,       "mu_e");
                systematicTree->makeOutputVariable(m_mu_charge,  "mu_charge");
                systematicTree->makeOutputVariable(m_mu_topoetcone20, "mu_topoetcone20");
                systematicTree->makeOutputVariable(m_mu_ptvarcone30, "mu_ptvarcone30");
                if (systematicTree->name().find("Loose") != std::string::npos) {
                    systematicTree->makeOutputVariable(m_mu_isTight, "mu_isTight");
                }
                systematicTree->makeOutputVariable(m_mu_d0sig, "mu_d0sig");
                systematicTree->makeOutputVariable(m_mu_delta_z0_sintheta, "mu_delta_z0_sintheta");
              if (m_config->isMC()) {
                systematicTree->makeOutputVariable(m_mu_true_type,   "mu_true_type");
                systematicTree->makeOutputVariable(m_mu_true_origin, "mu_true_origin");
              }
            }

            //photons
            if (m_config->usePhotons()) {
                systematicTree->makeOutputVariable(m_ph_pt, "ph_pt");
                systematicTree->makeOutputVariable(m_ph_eta, "ph_eta");
                systematicTree->makeOutputVariable(m_ph_phi, "ph_phi");
                systematicTree->makeOutputVariable(m_ph_e, "ph_e");
                systematicTree->makeOutputVariable(m_ph_iso, "ph_iso");
            }

            //taus
            if (m_config->useTaus()) {
                systematicTree->makeOutputVariable(m_tau_pt,     "tau_pt");
                systematicTree->makeOutputVariable(m_tau_eta,    "tau_eta");
                systematicTree->makeOutputVariable(m_tau_phi,    "tau_phi");
                systematicTree->makeOutputVariable(m_tau_charge, "tau_charge");
            }

            //jets
            if (m_config->useJets()) {
                systematicTree->makeOutputVariable(m_jet_pt,      "jet_pt");
                systematicTree->makeOutputVariable(m_jet_eta,     "jet_eta");
                systematicTree->makeOutputVariable(m_jet_phi,     "jet_phi");
                systematicTree->makeOutputVariable(m_jet_e,       "jet_e");
                systematicTree->makeOutputVariable(m_jet_mv2c00,  "jet_mv2c00");
                systematicTree->makeOutputVariable(m_jet_mv2c10,  "jet_mv2c10");
                systematicTree->makeOutputVariable(m_jet_mv2c20,  "jet_mv2c20");
                systematicTree->makeOutputVariable(m_jet_ip3dsv1, "jet_ip3dsv1");
                systematicTree->makeOutputVariable(m_jet_jvt,     "jet_jvt");
                systematicTree->makeOutputVariable(m_jet_passfjvt,"jet_passfjvt");
                if (m_config->isMC()) {
                  systematicTree->makeOutputVariable(m_jet_truthflav, "jet_truthflav");
                  systematicTree->makeOutputVariable(m_jet_truthPartonLabel, "jet_truthPartonLabel");
                  systematicTree->makeOutputVariable(m_jet_isTrueHS, "jet_isTrueHS");
                }
                for( auto& tagWP : m_config -> bTagWP_available()){
                  if (tagWP!= "Continuous") systematicTree->makeOutputVariable(m_jet_isbtagged[tagWP] , "jet_isbtagged_"+shortBtagWP(tagWP));
                  else systematicTree->makeOutputVariable(m_jet_tagWeightBin , "jet_tagWeightBin");
                }
		// R21 - new b-tagging variables
		if(m_config->getReleaseSeries() == 25){
		  systematicTree->makeOutputVariable(m_jet_MV2c10mu, "jet_MV2c10mu");
		  systematicTree->makeOutputVariable(m_jet_MV2c10rnn, "jet_MV2c10rnn");
		  systematicTree->makeOutputVariable(m_jet_DL1, "jet_DL1");
		  systematicTree->makeOutputVariable(m_jet_DL1mu, "jet_DL1mu");
		  systematicTree->makeOutputVariable(m_jet_DL1rnn, "jet_DL1rnn");
		  systematicTree->makeOutputVariable(m_jet_MV2cl100, "jet_MV2cl100");
		  systematicTree->makeOutputVariable(m_jet_MV2c100, "jet_MV2c100");
		  systematicTree->makeOutputVariable(m_jet_DL1_pu, "jet_DL1_pu");
		  systematicTree->makeOutputVariable(m_jet_DL1_pc, "jet_DL1_pc");
		  systematicTree->makeOutputVariable(m_jet_DL1_pb, "jet_DL1_pb");
		  systematicTree->makeOutputVariable(m_jet_DL1mu_pu, "jet_DL1mu_pu");
		  systematicTree->makeOutputVariable(m_jet_DL1mu_pc, "jet_DL1mu_pc");
		  systematicTree->makeOutputVariable(m_jet_DL1mu_pb, "jet_DL1mu_pb");
		  systematicTree->makeOutputVariable(m_jet_DL1rnn_pu, "jet_DL1rnn_pu");
		  systematicTree->makeOutputVariable(m_jet_DL1rnn_pc, "jet_DL1rnn_pc");
		  systematicTree->makeOutputVariable(m_jet_DL1rnn_pb, "jet_DL1rnn_pb");
		}

            }

            //large-R jets
            if (m_config->useLargeRJets()) {
                systematicTree->makeOutputVariable(m_ljet_pt,   "ljet_pt");
                systematicTree->makeOutputVariable(m_ljet_eta,  "ljet_eta");
                systematicTree->makeOutputVariable(m_ljet_phi,  "ljet_phi");
                systematicTree->makeOutputVariable(m_ljet_e,    "ljet_e");
                systematicTree->makeOutputVariable(m_ljet_m,    "ljet_m");
                systematicTree->makeOutputVariable(m_ljet_sd12, "ljet_sd12");
                systematicTree->makeOutputVariable(m_ljet_isTopTagged_50, "ljet_isTopTagged_50");
                systematicTree->makeOutputVariable(m_ljet_isTopTagged_80, "ljet_isTopTagged_80");
                systematicTree->makeOutputVariable(m_ljet_isWTagged_80, "ljet_isWTagged_80");
                systematicTree->makeOutputVariable(m_ljet_isWTagged_50, "ljet_isWTagged_50");
                systematicTree->makeOutputVariable(m_ljet_isZTagged_80, "ljet_isZTagged_80");
                systematicTree->makeOutputVariable(m_ljet_isZTagged_50, "ljet_isZTagged_50");
            }

            //track jets
            if (m_config->useTrackJets()) {
                systematicTree->makeOutputVariable(m_tjet_pt,   "tjet_pt");
                systematicTree->makeOutputVariable(m_tjet_eta,  "tjet_eta");
                systematicTree->makeOutputVariable(m_tjet_phi,  "tjet_phi");
                systematicTree->makeOutputVariable(m_tjet_e,    "tjet_e");
                systematicTree->makeOutputVariable(m_tjet_mv2c00,  "tjet_mv2c00");
                systematicTree->makeOutputVariable(m_tjet_mv2c10,  "tjet_mv2c10");
                systematicTree->makeOutputVariable(m_tjet_mv2c20,  "tjet_mv2c20");
                for( auto& tagWP : m_config -> bTagWP_available_trkJet()){
                  if (tagWP!= "Continuous") systematicTree->makeOutputVariable(m_tjet_isbtagged[tagWP] , "tjet_isbtagged_"+shortBtagWP(tagWP));
                  else systematicTree->makeOutputVariable(m_tjet_tagWeightBin , "tjet_tagWeightBin");
                }
            }

	    // RC branches
	    if (m_makeRCJets){
	      systematicTree->makeOutputVariable(m_rcjet_pt,     "rcjet_pt");
	      systematicTree->makeOutputVariable(m_rcjet_eta,    "rcjet_eta");
	      systematicTree->makeOutputVariable(m_rcjet_phi,    "rcjet_phi");
	      systematicTree->makeOutputVariable(m_rcjet_e,      "rcjet_e");
	      systematicTree->makeOutputVariable(m_rcjet_d12,    "rcjet_d12"); // requires >= 2 subjets
	      systematicTree->makeOutputVariable(m_rcjet_d23,    "rcjet_d23"); // requires >= 3 subjets
	      systematicTree->makeOutputVariable(m_rcjetsub_pt,  "rcjetsub_pt");  // vector of vectors for subjet info
	      systematicTree->makeOutputVariable(m_rcjetsub_eta, "rcjetsub_eta");
	      systematicTree->makeOutputVariable(m_rcjetsub_phi, "rcjetsub_phi");
	      systematicTree->makeOutputVariable(m_rcjetsub_e,   "rcjetsub_e");
	      systematicTree->makeOutputVariable(m_rcjetsub_mv2c10, "rcjetsub_mv2c10");
	    }

	    // vRC branches
	    if (m_makeVarRCJets){
	      std::string VarRC = "vrcjet";

	      for (auto& rho : m_VarRCJetRho){
                for (auto& mass_scale : m_VarRCJetMassScale){
		  std::replace( rho.begin(), rho.end(), '.', '_');
		  std::string name = rho+mass_scale;
		  systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC+"_"+name+"_pt"], VarRC+"_"+name+"_pt");
		  systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC+"_"+name+"_eta"],VarRC+"_"+name+"_eta");
		  systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC+"_"+name+"_phi"],VarRC+"_"+name+"_phi");
		  systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC+"_"+name+"_e"],  VarRC+"_"+name+"_e");
		  systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC+"_"+name+"_d12"],VarRC+"_"+name+"_d12"); // requires >= 2 subjets
		  systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC+"_"+name+"_d23"],VarRC+"_"+name+"_d23"); // requires >= 3 subjets
		  systematicTree->makeOutputVariable(m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_pt"],  VarRC+"sub_"+name+"_pt");  // vector of vectors for subjet info
		  systematicTree->makeOutputVariable(m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_eta"], VarRC+"sub_"+name+"_eta");
		  systematicTree->makeOutputVariable(m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_phi"], VarRC+"sub_"+name+"_phi");
		  systematicTree->makeOutputVariable(m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_e"],   VarRC+"sub_"+name+"_e");
		  systematicTree->makeOutputVariable(m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_mv2c10"], VarRC+"sub_"+name+"_mv2c10");
		} // end loop over mass parameters
	      } // end loop over multipliers for mass scale
	    } // end if VarRC jets

            //met
            systematicTree->makeOutputVariable(m_met_met, "met_met");
            systematicTree->makeOutputVariable(m_met_phi, "met_phi");

            if (m_config->doKLFitter()) {

                /// Global result
                systematicTree->makeOutputVariable(m_klfitter_logLikelihood,"klfitter_logLikelihood");
                systematicTree->makeOutputVariable(m_klfitter_eventProbability,"klfitter_eventProbability");
                systematicTree->makeOutputVariable(m_klfitter_selected,"klfitter_selected");

                // If FULL information is requested
                if( m_config->KLFitterOutput() == "FULL"){
                  /// Debugging information
                  systematicTree->makeOutputVariable(m_klfitter_minuitDidNotConverge,"klfitter_minuitDidNotConverge");
                  systematicTree->makeOutputVariable(m_klfitter_fitAbortedDueToNaN,"klfitter_fitAbortedDueToNaN");
                  systematicTree->makeOutputVariable(m_klfitter_atLeastOneFitParameterAtItsLimit,"klfitter_atLeastOneFitParameterAtItsLimit");
                  systematicTree->makeOutputVariable(m_klfitter_invalidTransferFunctionAtConvergence,"klfitter_invalidTransferFunctionAtConvergence");
                  /// Global
                  systematicTree->makeOutputVariable(m_klfitter_parameters,"klfitter_parameters");
                  systematicTree->makeOutputVariable(m_klfitter_parameterErrors,"klfitter_parameterErrors");
                  systematicTree->makeOutputVariable(m_klfitter_bestPermutation,"klfitter_bestPermutation");
                }

                if( m_config->KLFitterOutput() == "FULL" || m_config->KLFitterOutput() == "JETPERM_ONLY" ) {
                  /// Model
                  if (m_config->KLFitterLH() == "ttbar" || m_config->KLFitterLH() == "ttZTrilepton" || m_config->KLFitterLH() == "ttH" || m_config->KLFitterLH() == "ttbar_JetAngles" || m_config->KLFitterLH() == "ttbar_BoostedLJets"){
                    systematicTree->makeOutputVariable(m_klfitter_model_bhad_pt,"klfitter_model_bhad_pt");
                    systematicTree->makeOutputVariable(m_klfitter_model_bhad_eta,"klfitter_model_bhad_eta");
                    systematicTree->makeOutputVariable(m_klfitter_model_bhad_phi,"klfitter_model_bhad_phi");
                    systematicTree->makeOutputVariable(m_klfitter_model_bhad_E,"klfitter_model_bhad_E");
                    systematicTree->makeOutputVariable(m_klfitter_model_bhad_jetIndex,"klfitter_model_bhad_jetIndex");

                    systematicTree->makeOutputVariable(m_klfitter_model_blep_pt,"klfitter_model_blep_pt");
                    systematicTree->makeOutputVariable(m_klfitter_model_blep_eta,"klfitter_model_blep_eta");
                    systematicTree->makeOutputVariable(m_klfitter_model_blep_phi,"klfitter_model_blep_phi");
                    systematicTree->makeOutputVariable(m_klfitter_model_blep_E,"klfitter_model_blep_E");
                    systematicTree->makeOutputVariable(m_klfitter_model_blep_jetIndex,"klfitter_model_blep_jetIndex");

                    systematicTree->makeOutputVariable(m_klfitter_model_lq1_pt,"klfitter_model_lq1_pt");
                    systematicTree->makeOutputVariable(m_klfitter_model_lq1_eta,"klfitter_model_lq1_eta");
                    systematicTree->makeOutputVariable(m_klfitter_model_lq1_phi,"klfitter_model_lq1_phi");
                    systematicTree->makeOutputVariable(m_klfitter_model_lq1_E,"klfitter_model_lq1_E");
                    systematicTree->makeOutputVariable(m_klfitter_model_lq1_jetIndex,"klfitter_model_lq1_jetIndex");

                    systematicTree->makeOutputVariable(m_klfitter_model_lep_pt,"klfitter_model_lep_pt");
                    systematicTree->makeOutputVariable(m_klfitter_model_lep_eta,"klfitter_model_lep_eta");
                    systematicTree->makeOutputVariable(m_klfitter_model_lep_phi,"klfitter_model_lep_phi");
                    systematicTree->makeOutputVariable(m_klfitter_model_lep_E,"klfitter_model_lep_E");

                    systematicTree->makeOutputVariable(m_klfitter_model_nu_pt,"klfitter_model_nu_pt");
                    systematicTree->makeOutputVariable(m_klfitter_model_nu_eta,"klfitter_model_nu_eta");
                    systematicTree->makeOutputVariable(m_klfitter_model_nu_phi,"klfitter_model_nu_phi");
                    systematicTree->makeOutputVariable(m_klfitter_model_nu_E,"klfitter_model_nu_E");

                    if(m_config->KLFitterLH() != "ttbar_BoostedLJets"){
                      systematicTree->makeOutputVariable(m_klfitter_model_lq2_pt,"klfitter_model_lq2_pt");
                      systematicTree->makeOutputVariable(m_klfitter_model_lq2_eta,"klfitter_model_lq2_eta");
                      systematicTree->makeOutputVariable(m_klfitter_model_lq2_phi,"klfitter_model_lq2_phi");
                      systematicTree->makeOutputVariable(m_klfitter_model_lq2_E,"klfitter_model_lq2_E");
                      systematicTree->makeOutputVariable(m_klfitter_model_lq2_jetIndex,"klfitter_model_lq2_jetIndex");

                      if(m_config->KLFitterLH() == "ttZTrilepton"){
                        systematicTree->makeOutputVariable(m_klfitter_model_lep_index,"klfitter_model_lep_index");

                        systematicTree->makeOutputVariable(m_klfitter_model_lepZ1_pt,"klfitter_model_lepZ1_pt");
                        systematicTree->makeOutputVariable(m_klfitter_model_lepZ1_eta,"klfitter_model_lepZ1_eta");
                        systematicTree->makeOutputVariable(m_klfitter_model_lepZ1_phi,"klfitter_model_lepZ1_phi");
                        systematicTree->makeOutputVariable(m_klfitter_model_lepZ1_E,"klfitter_model_lepZ1_E");
                        systematicTree->makeOutputVariable(m_klfitter_model_lepZ1_index,"klfitter_model_lepZ1_index");

                        systematicTree->makeOutputVariable(m_klfitter_model_lepZ2_pt,"klfitter_model_lepZ2_pt");
                        systematicTree->makeOutputVariable(m_klfitter_model_lepZ2_eta,"klfitter_model_lepZ2_eta");
                        systematicTree->makeOutputVariable(m_klfitter_model_lepZ2_phi,"klfitter_model_lepZ2_phi");
                        systematicTree->makeOutputVariable(m_klfitter_model_lepZ2_E,"klfitter_model_lepZ2_E");
                        systematicTree->makeOutputVariable(m_klfitter_model_lepZ2_index,"klfitter_model_lepZ2_index");
                      }

                      if(m_config->KLFitterLH() == "ttH"){

                        systematicTree->makeOutputVariable(m_klfitter_model_Higgs_b1_pt,"klfitter_model_Higgs_b1_pt");
                        systematicTree->makeOutputVariable(m_klfitter_model_Higgs_b1_eta,"klfitter_model_Higgs_b1_eta");
                        systematicTree->makeOutputVariable(m_klfitter_model_Higgs_b1_phi,"klfitter_model_Higgs_b1_phi");
                        systematicTree->makeOutputVariable(m_klfitter_model_Higgs_b1_E,"klfitter_model_Higgs_b1_E");
                        systematicTree->makeOutputVariable(m_klfitter_model_Higgs_b1_jetIndex,"klfitter_model_Higgs_b1_jetIndex");

                        systematicTree->makeOutputVariable(m_klfitter_model_Higgs_b2_pt,"klfitter_model_Higgs_b2_pt");
                        systematicTree->makeOutputVariable(m_klfitter_model_Higgs_b2_eta,"klfitter_model_Higgs_b2_eta");
                        systematicTree->makeOutputVariable(m_klfitter_model_Higgs_b2_phi,"klfitter_model_Higgs_b2_phi");
                        systematicTree->makeOutputVariable(m_klfitter_model_Higgs_b2_E,"klfitter_model_Higgs_b2_E");
                        systematicTree->makeOutputVariable(m_klfitter_model_Higgs_b2_jetIndex,"klfitter_model_Higgs_b2_jetIndex");
                      }
                    }
                  } else if (m_config->KLFitterLH() == "ttbar_AllHadronic"){

                    systematicTree->makeOutputVariable(m_klfitter_model_b_from_top1_pt,"klfitter_model_b_from_top1_pt");
                    systematicTree->makeOutputVariable(m_klfitter_model_b_from_top1_eta,"klfitter_model_b_from_top1_eta");
                    systematicTree->makeOutputVariable(m_klfitter_model_b_from_top1_phi,"klfitter_model_b_from_top1_phi");
                    systematicTree->makeOutputVariable(m_klfitter_model_b_from_top1_E,"klfitter_model_b_from_top1_E");
                    systematicTree->makeOutputVariable(m_klfitter_model_b_from_top1_jetIndex,"klfitter_model_b_from_top1_jetIndex");

                    systematicTree->makeOutputVariable(m_klfitter_model_b_from_top2_pt,"klfitter_model_b_from_top2_pt");
                    systematicTree->makeOutputVariable(m_klfitter_model_b_from_top2_eta,"klfitter_model_b_from_top2_eta");
                    systematicTree->makeOutputVariable(m_klfitter_model_b_from_top2_phi,"klfitter_model_b_from_top2_phi");
                    systematicTree->makeOutputVariable(m_klfitter_model_b_from_top2_E,"klfitter_model_b_from_top2_E");
                    systematicTree->makeOutputVariable(m_klfitter_model_b_from_top2_jetIndex,"klfitter_model_b_from_top2_jetIndex");

                    systematicTree->makeOutputVariable(m_klfitter_model_lj1_from_top1_pt,"klfitter_model_lj1_from_top1_pt");
                    systematicTree->makeOutputVariable(m_klfitter_model_lj1_from_top1_eta,"klfitter_model_lj1_from_top1_eta");
                    systematicTree->makeOutputVariable(m_klfitter_model_lj1_from_top1_phi,"klfitter_model_lj1_from_top1_phi");
                    systematicTree->makeOutputVariable(m_klfitter_model_lj1_from_top1_E,"klfitter_model_lj1_from_top1_E");
                    systematicTree->makeOutputVariable(m_klfitter_model_lj1_from_top1_jetIndex,"klfitter_model_lj1_from_top1_jetIndex");

                    systematicTree->makeOutputVariable(m_klfitter_model_lj2_from_top1_pt,"klfitter_model_lj1_from_top1_pt");
                    systematicTree->makeOutputVariable(m_klfitter_model_lj2_from_top1_eta,"klfitter_model_lj1_from_top1_eta");
                    systematicTree->makeOutputVariable(m_klfitter_model_lj2_from_top1_phi,"klfitter_model_lj1_from_top1_phi");
                    systematicTree->makeOutputVariable(m_klfitter_model_lj2_from_top1_E,"klfitter_model_lj1_from_top1_E");
                    systematicTree->makeOutputVariable(m_klfitter_model_lj2_from_top1_jetIndex,"klfitter_model_lj1_from_top1_jetIndex");

                    systematicTree->makeOutputVariable(m_klfitter_model_lj1_from_top2_pt,"klfitter_model_lj1_from_top2_pt");
                    systematicTree->makeOutputVariable(m_klfitter_model_lj1_from_top2_eta,"klfitter_model_lj1_from_top2_eta");
                    systematicTree->makeOutputVariable(m_klfitter_model_lj1_from_top2_phi,"klfitter_model_lj1_from_top2_phi");
                    systematicTree->makeOutputVariable(m_klfitter_model_lj1_from_top2_E,"klfitter_model_lj1_from_top2_E");
                    systematicTree->makeOutputVariable(m_klfitter_model_lj1_from_top2_jetIndex,"klfitter_model_lj1_from_top2_jetIndex");

                    systematicTree->makeOutputVariable(m_klfitter_model_lj2_from_top2_pt,"klfitter_model_lj1_from_top2_pt");
                    systematicTree->makeOutputVariable(m_klfitter_model_lj2_from_top2_eta,"klfitter_model_lj1_from_top2_eta");
                    systematicTree->makeOutputVariable(m_klfitter_model_lj2_from_top2_phi,"klfitter_model_lj1_from_top2_phi");
                    systematicTree->makeOutputVariable(m_klfitter_model_lj2_from_top2_E,"klfitter_model_lj1_from_top2_E");
                    systematicTree->makeOutputVariable(m_klfitter_model_lj2_from_top2_jetIndex,"klfitter_model_lj1_from_top2_jetIndex");

                  }
                }

                if( m_config->KLFitterOutput() == "FULL" || m_config->KLFitterOutput() == "FITTEDTOPS_ONLY" ){
                  if (m_config->KLFitterLH() == "ttbar" || m_config->KLFitterLH() == "ttZTrilepton" || m_config->KLFitterLH() == "ttH" || m_config->KLFitterLH() == "ttbar_JetAngles"){
                    systematicTree->makeOutputVariable(m_klfitter_bestPerm_topLep_pt,"klfitter_bestPerm_topLep_pt");
                    systematicTree->makeOutputVariable(m_klfitter_bestPerm_topLep_eta,"klfitter_bestPerm_topLep_eta");
                    systematicTree->makeOutputVariable(m_klfitter_bestPerm_topLep_phi,"klfitter_bestPerm_topLep_phi");
                    systematicTree->makeOutputVariable(m_klfitter_bestPerm_topLep_E,"klfitter_bestPerm_topLep_E");
                    systematicTree->makeOutputVariable(m_klfitter_bestPerm_topLep_m,"klfitter_bestPerm_topLep_m");

                    systematicTree->makeOutputVariable(m_klfitter_bestPerm_topHad_pt,"klfitter_bestPerm_topHad_pt");
                    systematicTree->makeOutputVariable(m_klfitter_bestPerm_topHad_eta,"klfitter_bestPerm_topHad_eta");
                    systematicTree->makeOutputVariable(m_klfitter_bestPerm_topHad_phi,"klfitter_bestPerm_topHad_phi");
                    systematicTree->makeOutputVariable(m_klfitter_bestPerm_topHad_E,"klfitter_bestPerm_topHad_E");
                    systematicTree->makeOutputVariable(m_klfitter_bestPerm_topHad_m,"klfitter_bestPerm_topHad_m");

                    systematicTree->makeOutputVariable(m_klfitter_bestPerm_ttbar_pt,"klfitter_bestPerm_ttbar_pt");
                    systematicTree->makeOutputVariable(m_klfitter_bestPerm_ttbar_eta,"klfitter_bestPerm_ttbar_eta");
                    systematicTree->makeOutputVariable(m_klfitter_bestPerm_ttbar_phi,"klfitter_bestPerm_ttbar_phi");
                    systematicTree->makeOutputVariable(m_klfitter_bestPerm_ttbar_E,"klfitter_bestPerm_ttbar_E");
                    systematicTree->makeOutputVariable(m_klfitter_bestPerm_ttbar_m,"klfitter_bestPerm_ttbar_m");
                  }
                }
            }

            if (m_config->doPseudoTop()) {

              systematicTree->makeOutputVariable(m_PseudoTop_Reco_ttbar_pt,    "PseudoTop_Reco_ttbar_pt");
              systematicTree->makeOutputVariable(m_PseudoTop_Reco_ttbar_eta,   "PseudoTop_Reco_ttbar_eta");
              systematicTree->makeOutputVariable(m_PseudoTop_Reco_ttbar_phi,   "PseudoTop_Reco_ttbar_phi");
              systematicTree->makeOutputVariable(m_PseudoTop_Reco_ttbar_m,     "PseudoTop_Reco_ttbar_m");
              systematicTree->makeOutputVariable(m_PseudoTop_Reco_top_had_pt,  "PseudoTop_Reco_top_had_pt");
              systematicTree->makeOutputVariable(m_PseudoTop_Reco_top_had_eta, "PseudoTop_Reco_top_had_eta");
              systematicTree->makeOutputVariable(m_PseudoTop_Reco_top_had_phi, "PseudoTop_Reco_top_had_phi");
              systematicTree->makeOutputVariable(m_PseudoTop_Reco_top_had_m,   "PseudoTop_Reco_top_had_m");
              systematicTree->makeOutputVariable(m_PseudoTop_Reco_top_lep_pt,  "PseudoTop_Reco_top_lep_pt");
              systematicTree->makeOutputVariable(m_PseudoTop_Reco_top_lep_eta, "PseudoTop_Reco_top_lep_eta");
              systematicTree->makeOutputVariable(m_PseudoTop_Reco_top_lep_phi, "PseudoTop_Reco_top_lep_phi");
              systematicTree->makeOutputVariable(m_PseudoTop_Reco_top_lep_m,   "PseudoTop_Reco_top_lep_m");

            }

            //extra branches telling you if the event passed / failed a selection
            int index(0);
            for (const auto& branchName : m_extraBranches) {
                m_selectionDecisions[index] = 0;
                systematicTree->makeOutputVariable(m_selectionDecisions[index], branchName);
                // Add all triggers to a map so we don't get any duplicates
                for (auto& trigger_name : m_config->allTriggers_Tight( branchName ) ) {
                    m_triggerDecisions [trigger_name] = 0;
                    if (!m_config->isMC() && m_config->doFakesMMWeights())
                        m_triggerPrescales [trigger_name] = -99999.;//initialised to dummmy value, in case it can't be retrieved by the tool
                }
                for (auto& trigger_name : m_config->allTriggers_Loose( branchName ) ) {
                  // let's make sure this isn't done twice
                  if (m_triggerDecisions.find(trigger_name) != m_triggerDecisions.end()
                    && m_triggerPrescales.find(trigger_name) != m_triggerPrescales.end()) continue;
                    m_triggerDecisions [trigger_name] = 0;
                    if (!m_config->isMC() && m_config->doFakesMMWeights())
                        m_triggerPrescales [trigger_name] = -99999.;//initialised to dummmy value, in case it can't be retrieved by the tool
                }
                for (auto& trigger_name : m_config->electronTriggers_Tight( branchName ) )
                    m_el_trigMatched [trigger_name] = std::vector<char>();
                for (auto& trigger_name : m_config->electronTriggers_Loose( branchName ) ) {
                    // let's make sure this isn't done twice
                    if (m_el_trigMatched.find(trigger_name) != m_el_trigMatched.end()) continue;
                    m_el_trigMatched [trigger_name] = std::vector<char>();
                }
                for (auto& trigger_name : m_config->muonTriggers_Tight( branchName ) )
                    m_mu_trigMatched [trigger_name] = std::vector<char>();
                for (auto& trigger_name : m_config->muonTriggers_Loose( branchName ) ) {
                    // let's make sure this isn't done twice
                    if (m_mu_trigMatched.find(trigger_name) != m_mu_trigMatched.end()) continue;
                    m_mu_trigMatched [trigger_name] = std::vector<char>();
                }
                ++index;
            }

            for( auto& trig_name : m_triggerDecisions )
                systematicTree->makeOutputVariable( trig_name.second, trig_name.first );
            if (!m_config->isMC() && m_config->doFakesMMWeights()) {
              for( auto& trig_name : m_triggerPrescales )
                  systematicTree->makeOutputVariable( trig_name.second, "PS_"+trig_name.first );
            }
            for( auto& trig_name : m_el_trigMatched )
                systematicTree->makeOutputVariable( trig_name.second, "el_trigMatch_"+trig_name.first );
            for( auto& trig_name : m_mu_trigMatched )
                systematicTree->makeOutputVariable( trig_name.second, "mu_trigMatch_"+trig_name.first );

        }

        setupUpgradeTreeManager();
        setupParticleLevelTreeManager();
    }

    void EventSaverFlatNtuple::setupParticleLevelTreeManager(/*const top::ParticleLevelEvent& plEvent*/){
        // Quick return if particle level is disabled or the tree is already initialised.
        // If particle level is disabled, no tree will be created.
        if ( not m_config->doTopParticleLevel() or m_particleLevelTreeManager ){
            return;
        }

        m_particleLevelTreeManager = std::make_shared<top::TreeManager>( "particleLevel", m_outputFile, m_config->outputFileSetAutoFlushZero() );

        m_particleLevelTreeManager->makeOutputVariable(m_weight_mc, "weight_mc");

        //event info
        m_particleLevelTreeManager->makeOutputVariable(m_eventNumber, "eventNumber");
        m_particleLevelTreeManager->makeOutputVariable(m_runNumber, "runNumber");
        if (m_config->isMC() && m_config->doPileupReweighting()) m_particleLevelTreeManager->makeOutputVariable(m_randomRunNumber, "randomRunNumber");
        m_particleLevelTreeManager->makeOutputVariable(m_mcChannelNumber, "mcChannelNumber");
        m_particleLevelTreeManager->makeOutputVariable(m_mu, "mu");
        m_particleLevelTreeManager->makeOutputVariable(m_weight_pileup, "weight_pileup");

	if (m_config->doPseudoTop()) {

	  m_particleLevelTreeManager->makeOutputVariable(m_PseudoTop_Particle_ttbar_pt,    "PseudoTop_Particle_ttbar_pt");
	  m_particleLevelTreeManager->makeOutputVariable(m_PseudoTop_Particle_ttbar_eta,   "PseudoTop_Particle_ttbar_eta");
	  m_particleLevelTreeManager->makeOutputVariable(m_PseudoTop_Particle_ttbar_phi,   "PseudoTop_Particle_ttbar_phi");
	  m_particleLevelTreeManager->makeOutputVariable(m_PseudoTop_Particle_ttbar_m,     "PseudoTop_Particle_ttbar_m");
	  m_particleLevelTreeManager->makeOutputVariable(m_PseudoTop_Particle_top_had_pt,  "PseudoTop_Particle_top_had_pt");
	  m_particleLevelTreeManager->makeOutputVariable(m_PseudoTop_Particle_top_had_eta, "PseudoTop_Particle_top_had_eta");
	  m_particleLevelTreeManager->makeOutputVariable(m_PseudoTop_Particle_top_had_phi, "PseudoTop_Particle_top_had_phi");
	  m_particleLevelTreeManager->makeOutputVariable(m_PseudoTop_Particle_top_had_m,   "PseudoTop_Particle_top_had_m");
	  m_particleLevelTreeManager->makeOutputVariable(m_PseudoTop_Particle_top_lep_pt,  "PseudoTop_Particle_top_lep_pt");
	  m_particleLevelTreeManager->makeOutputVariable(m_PseudoTop_Particle_top_lep_eta, "PseudoTop_Particle_top_lep_eta");
	  m_particleLevelTreeManager->makeOutputVariable(m_PseudoTop_Particle_top_lep_phi, "PseudoTop_Particle_top_lep_phi");
	  m_particleLevelTreeManager->makeOutputVariable(m_PseudoTop_Particle_top_lep_m,   "PseudoTop_Particle_top_lep_m");

	}

        //electrons
        if ( m_config->useTruthElectrons() ){
            m_particleLevelTreeManager->makeOutputVariable(m_el_pt, "el_pt");
            m_particleLevelTreeManager->makeOutputVariable(m_el_eta, "el_eta");
            m_particleLevelTreeManager->makeOutputVariable(m_el_phi, "el_phi");
            m_particleLevelTreeManager->makeOutputVariable(m_el_e, "el_e");
            m_particleLevelTreeManager->makeOutputVariable(m_el_charge, "el_charge");

            m_particleLevelTreeManager->makeOutputVariable(m_el_pt_bare, "el_pt_bare");
            m_particleLevelTreeManager->makeOutputVariable(m_el_eta_bare, "el_eta_bare");
            m_particleLevelTreeManager->makeOutputVariable(m_el_phi_bare, "el_phi_bare");
            m_particleLevelTreeManager->makeOutputVariable(m_el_e_bare, "el_e_bare");
        }

        //muons
        if ( m_config->useTruthMuons() ){
            m_particleLevelTreeManager->makeOutputVariable(m_mu_pt, "mu_pt");
            m_particleLevelTreeManager->makeOutputVariable(m_mu_eta, "mu_eta");
            m_particleLevelTreeManager->makeOutputVariable(m_mu_phi, "mu_phi");
            m_particleLevelTreeManager->makeOutputVariable(m_mu_e, "mu_e");
            m_particleLevelTreeManager->makeOutputVariable(m_mu_charge, "mu_charge");

            m_particleLevelTreeManager->makeOutputVariable(m_mu_pt_bare, "mu_pt_bare");
            m_particleLevelTreeManager->makeOutputVariable(m_mu_eta_bare, "mu_eta_bare");
            m_particleLevelTreeManager->makeOutputVariable(m_mu_phi_bare, "mu_phi_bare");
            m_particleLevelTreeManager->makeOutputVariable(m_mu_e_bare, "mu_e_bare");
        }

        //photons
        if (m_config->useTruthPhotons()) {
            m_particleLevelTreeManager->makeOutputVariable(m_ph_pt, "ph_pt");
            m_particleLevelTreeManager->makeOutputVariable(m_ph_eta, "ph_eta");
            m_particleLevelTreeManager->makeOutputVariable(m_ph_phi, "ph_phi");
            m_particleLevelTreeManager->makeOutputVariable(m_ph_e, "ph_e");
        }

        //jets
        if ( m_config->useTruthJets() ){
            m_particleLevelTreeManager->makeOutputVariable(m_jet_pt, "jet_pt");
            m_particleLevelTreeManager->makeOutputVariable(m_jet_eta, "jet_eta");
            m_particleLevelTreeManager->makeOutputVariable(m_jet_phi, "jet_phi");
            m_particleLevelTreeManager->makeOutputVariable(m_jet_e, "jet_e");

            m_particleLevelTreeManager->makeOutputVariable(m_jet_Ghosts_BHadron_Final_Count, "jet_nGhosts_bHadron");
            m_particleLevelTreeManager->makeOutputVariable(m_jet_Ghosts_CHadron_Final_Count, "jet_nGhosts_cHadron");
        }

        //large R jets
        if ( m_config->useTruthLargeRJets() ){
            m_particleLevelTreeManager->makeOutputVariable(m_ljet_pt, "ljet_pt");
            m_particleLevelTreeManager->makeOutputVariable(m_ljet_eta, "ljet_eta");
            m_particleLevelTreeManager->makeOutputVariable(m_ljet_phi, "ljet_phi");
            m_particleLevelTreeManager->makeOutputVariable(m_ljet_e, "ljet_e");

            m_particleLevelTreeManager->makeOutputVariable(m_ljet_Ghosts_BHadron_Final_Count, "ljet_nGhosts_bHadron");
            m_particleLevelTreeManager->makeOutputVariable(m_ljet_Ghosts_CHadron_Final_Count, "ljet_nGhosts_cHadron");
        }

        // RC branches
        if (m_makeRCJets){
          // first initialise the ParticleLevelRCJetObjectLoader
          m_rcjet_particle = std::unique_ptr<ParticleLevelRCJetObjectLoader> ( new ParticleLevelRCJetObjectLoader( m_config ) );
          top::check(m_rcjet_particle->initialize(),"Failed to initialize ParticleLevelRCJetObjectLoader");
          // then create the branches
          m_particleLevelTreeManager->makeOutputVariable(m_rcjet_pt,     "rcjet_pt");
          m_particleLevelTreeManager->makeOutputVariable(m_rcjet_eta,    "rcjet_eta");
          m_particleLevelTreeManager->makeOutputVariable(m_rcjet_phi,    "rcjet_phi");
          m_particleLevelTreeManager->makeOutputVariable(m_rcjet_e,      "rcjet_e");
          m_particleLevelTreeManager->makeOutputVariable(m_rcjet_d12,    "rcjet_d12"); // requires >= 2 subjets
          m_particleLevelTreeManager->makeOutputVariable(m_rcjet_d23,    "rcjet_d23"); // requires >= 3 subjets
          m_particleLevelTreeManager->makeOutputVariable(m_rcjetsub_pt,  "rcjetsub_pt");  // vector of vectors for subjet info
          m_particleLevelTreeManager->makeOutputVariable(m_rcjetsub_eta, "rcjetsub_eta");
          m_particleLevelTreeManager->makeOutputVariable(m_rcjetsub_phi, "rcjetsub_phi");
          m_particleLevelTreeManager->makeOutputVariable(m_rcjetsub_e,   "rcjetsub_e");
          m_particleLevelTreeManager->makeOutputVariable(m_rcjetsub_Ghosts_BHadron_Final_Count, "rcjetsub_nGhosts_bHadron");
          m_particleLevelTreeManager->makeOutputVariable(m_rcjetsub_Ghosts_CHadron_Final_Count, "rcjetsub_nGhosts_cHadron");
        }

        //met
        if ( m_config->useTruthMET() ){
            m_particleLevelTreeManager->makeOutputVariable(m_met_met, "met_met");
            m_particleLevelTreeManager->makeOutputVariable(m_met_phi, "met_phi");
        }

        // Setup PDF info branches if PDF info is requested.
        if (m_config->doTruthPDFInfo()) {
            m_particleLevelTreeManager->makeOutputVariable(m_PDFinfo_X1, "PDFinfo_X1");
            m_particleLevelTreeManager->makeOutputVariable(m_PDFinfo_X2, "PDFinfo_X2");
            m_particleLevelTreeManager->makeOutputVariable(m_PDFinfo_PDGID1, "PDFinfo_PDGID1");
            m_particleLevelTreeManager->makeOutputVariable(m_PDFinfo_PDGID2, "PDFinfo_PDGID2");
            m_particleLevelTreeManager->makeOutputVariable(m_PDFinfo_Q, "PDFinfo_Q");
            m_particleLevelTreeManager->makeOutputVariable(m_PDFinfo_XF1, "PDFinfo_XF1");
            m_particleLevelTreeManager->makeOutputVariable(m_PDFinfo_XF2, "PDFinfo_XF2");
        }

        // Setup MC generator weights branch if requested.
        if (m_config->doMCGeneratorWeights()) {
            m_particleLevelTreeManager->makeOutputVariable(m_mc_generator_weights, "mc_generator_weights");
	}

        // Setup PDF weight branches if PDF systematics are requested,
        if( m_config->saveLHAPDFEvent() ){
            for ( auto & pdf : m_PDF_eventWeights ) {
                m_particleLevelTreeManager->makeOutputVariable( pdf.second, pdf.first );
            }
        }

        // Setup the selection decision branches. This is a two-step process:
        //
        // (1) First, we need to create an array of variables which will be used to
        //     store the data in the TTree. The variable
        //     m_particleLevel_SelectionDecisions is just that: A std::vector of
        //     pairs, thwere the .first corresponds to the name of the selection,
        //     and the .second is the actual variable.
        // (2) Secondly, we need to attach the newly created variables to the
        //     particle level tree manager.
        m_particleLevel_SelectionDecisions.reserve( m_extraBranches.size() );
        for ( const auto & selection : m_extraBranches ){
            m_particleLevel_SelectionDecisions.push_back( std::make_pair( selection, int() ) );
        }

        for ( auto & selectionDecision : m_particleLevel_SelectionDecisions ){
            m_particleLevelTreeManager->makeOutputVariable( selectionDecision.second, selectionDecision.first );
        }
    }

  void EventSaverFlatNtuple::setupUpgradeTreeManager(/*const top::ParticleLevelEvent& upgradeEvent*/){
        // Quick return if upgrade is disabled or the tree is already initialised.
        if ( not m_config->HLLHC() or m_upgradeTreeManager ){
            return;
        }

        m_upgradeTreeManager = std::make_shared<top::TreeManager>( "upgrade", m_outputFile, m_config->outputFileSetAutoFlushZero() );

        m_upgradeTreeManager->makeOutputVariable(m_weight_mc, "weight_mc");

        //event info
        m_upgradeTreeManager->makeOutputVariable(m_eventNumber, "eventNumber");
        m_upgradeTreeManager->makeOutputVariable(m_runNumber, "runNumber");
        m_upgradeTreeManager->makeOutputVariable(m_mcChannelNumber, "mcChannelNumber");
        m_upgradeTreeManager->makeOutputVariable(m_mu, "mu");

       // electrons
       m_upgradeTreeManager->makeOutputVariable(m_el_pt, "el_pt");
       m_upgradeTreeManager->makeOutputVariable(m_el_eta, "el_eta");
       m_upgradeTreeManager->makeOutputVariable(m_el_phi, "el_phi");
       m_upgradeTreeManager->makeOutputVariable(m_el_e, "el_e");
       m_upgradeTreeManager->makeOutputVariable(m_el_charge, "el_charge");

       // muons
       m_upgradeTreeManager->makeOutputVariable(m_mu_pt, "mu_pt");
       m_upgradeTreeManager->makeOutputVariable(m_mu_eta, "mu_eta");
       m_upgradeTreeManager->makeOutputVariable(m_mu_phi, "mu_phi");
       m_upgradeTreeManager->makeOutputVariable(m_mu_e, "mu_e");
       m_upgradeTreeManager->makeOutputVariable(m_mu_charge, "mu_charge");

       // jets
       m_upgradeTreeManager->makeOutputVariable(m_jet_pt, "jet_pt");
       m_upgradeTreeManager->makeOutputVariable(m_jet_eta, "jet_eta");
       m_upgradeTreeManager->makeOutputVariable(m_jet_phi, "jet_phi");
       m_upgradeTreeManager->makeOutputVariable(m_jet_e, "jet_e");
       m_upgradeTreeManager->makeOutputVariable(m_jet_mv1eff, "jet_mv1eff");
       m_upgradeTreeManager->makeOutputVariable(m_jet_isPileup, "jet_isPileup");

       m_upgradeTreeManager->makeOutputVariable(m_jet_Ghosts_BHadron_Final_Count, "jet_nGhosts_bHadron");
       m_upgradeTreeManager->makeOutputVariable(m_jet_Ghosts_CHadron_Final_Count, "jet_nGhosts_cHadron");

       // MET
       m_upgradeTreeManager->makeOutputVariable(m_met_met, "met_met");
       m_upgradeTreeManager->makeOutputVariable(m_met_phi, "met_phi");
   }//setupUpgradeTreeManager

    void EventSaverFlatNtuple::recordSelectionDecision(const top::Event& event) {
        int index(0);
        for (const auto& branchName : m_extraBranches) {
            m_selectionDecisions[index] = event.m_info->auxdataConst<int>(branchName);
            ++index;
        }
    }

    void EventSaverFlatNtuple::recordTriggerDecision(const top::Event& event) {

        for (const auto& trigger : m_triggerDecisions)
            m_triggerDecisions[trigger.first] = event.m_info->auxdataConst<char>("TRIGDEC_"+trigger.first);
        if (!m_config->isMC() && m_config->doFakesMMWeights()) {
          for (const auto& trigger : m_triggerPrescales)
              m_triggerPrescales[trigger.first] = event.m_info->auxdataConst<float>("TRIGPS_"+trigger.first);
        }

    }

    void EventSaverFlatNtuple::saveEvent(const top::Event& event) {
      // record the event?
      if (m_config->saveOnlySelectedEvents() && !event.m_saveEvent)
          return;
      this -> cleanEvent();
      this -> calculateEvent(event);
      this -> fillEvent(event);
    }

    void EventSaverFlatNtuple::cleanEvent() {
      /* Doing nothing for now, but we should put here the (re-)initialisation to dummy values
       * of all variables later calculated in calculateEvent(const top::Event& event)
       * For example all weights set to 1, all vectors cleaned, all kinematic or angular variables set to -99999.
       */
    }

    void EventSaverFlatNtuple::calculateEvent(const top::Event& event) {
        //We have decorated event-info with a variable for each selection - with pass / fail
        recordSelectionDecision(event);

        recordTriggerDecision(event);

        //some event weights
        m_weight_mc = 0.;
        if (m_config->isMC())
//             m_weight_mc = event.m_info->mcEventWeight();
            m_weight_mc = event.m_truthEvent->at(0)->weights()[0];// FIXME temporary bugfix

        if (m_config->isMC()) {

            m_weight_pileup    = m_sfRetriever->pileupSF(event);

            m_weight_leptonSF  = m_sfRetriever->leptonSF(event,top::topSFSyst::nominal);

            if (m_config->useTaus())
              m_weight_tauSF = m_sfRetriever->tauSF(event, top::topSFSyst::nominal);

            if (m_config->usePhotons())
              m_weight_photonSF = m_sfRetriever->photonSF(event, top::topSFSyst::nominal);

            for( auto& tagWP : m_config -> bTagWP_available()) {
              m_weight_bTagSF[tagWP] = m_sfRetriever->btagSF(event, top::topSFSyst::nominal, tagWP);
            }
            if (m_config->useTrackJets()) {
              for( auto& tagWP : m_config -> bTagWP_available_trkJet()) {
                m_weight_trackjet_bTagSF[tagWP] = m_sfRetriever->btagSF(event, top::topSFSyst::nominal, tagWP, true);
              }
            }

            m_weight_jvt = m_sfRetriever->jvtSF(event, top::topSFSyst::nominal);

            if (m_config->isSherpa22Vjets()) {
              if (event.m_info->isAvailable<double>("Sherpa22VJetsWeight")) {
                m_weight_sherpa_22_vjets = event.m_info->auxdataConst<double>("Sherpa22VJetsWeight");
              }
            }

            // writing the systematic-shifted SFs only in the nominal (or nominal_Loose) tree
            if (event.m_hashValue == m_config->nominalHashValue()) {

                m_weight_pileup_UP = m_sfRetriever->pileupSF( event, +1 ); // up variation
                m_weight_pileup_DOWN = m_sfRetriever->pileupSF( event, -1 ); // down variation

                m_weight_leptonSF_EL_SF_Trigger_UP   = m_sfRetriever->leptonSF(event,top::topSFSyst::EL_SF_Trigger_UP);
                m_weight_leptonSF_EL_SF_Trigger_DOWN = m_sfRetriever->leptonSF(event,top::topSFSyst::EL_SF_Trigger_DOWN);
                m_weight_leptonSF_EL_SF_Reco_UP      = m_sfRetriever->leptonSF(event,top::topSFSyst::EL_SF_Reco_UP);
                m_weight_leptonSF_EL_SF_Reco_DOWN    = m_sfRetriever->leptonSF(event,top::topSFSyst::EL_SF_Reco_DOWN);
                m_weight_leptonSF_EL_SF_ID_UP        = m_sfRetriever->leptonSF(event,top::topSFSyst::EL_SF_ID_UP);
                m_weight_leptonSF_EL_SF_ID_DOWN      = m_sfRetriever->leptonSF(event,top::topSFSyst::EL_SF_ID_DOWN);
                m_weight_leptonSF_EL_SF_Isol_UP      = m_sfRetriever->leptonSF(event,top::topSFSyst::EL_SF_Isol_UP);
                m_weight_leptonSF_EL_SF_Isol_DOWN    = m_sfRetriever->leptonSF(event,top::topSFSyst::EL_SF_Isol_DOWN);

                m_weight_leptonSF_MU_SF_Trigger_STAT_UP   = m_sfRetriever->leptonSF(event,top::topSFSyst::MU_SF_Trigger_STAT_UP);
                m_weight_leptonSF_MU_SF_Trigger_STAT_DOWN = m_sfRetriever->leptonSF(event,top::topSFSyst::MU_SF_Trigger_STAT_DOWN);
                m_weight_leptonSF_MU_SF_Trigger_SYST_UP   = m_sfRetriever->leptonSF(event,top::topSFSyst::MU_SF_Trigger_SYST_UP);
                m_weight_leptonSF_MU_SF_Trigger_SYST_DOWN = m_sfRetriever->leptonSF(event,top::topSFSyst::MU_SF_Trigger_SYST_DOWN);
                // Muon ID SF systematics (regular)
                m_weight_leptonSF_MU_SF_ID_STAT_UP = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_ID_STAT_UP);
                m_weight_leptonSF_MU_SF_ID_STAT_DOWN = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_ID_STAT_DOWN);
                m_weight_leptonSF_MU_SF_ID_SYST_UP = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_ID_SYST_UP);
                m_weight_leptonSF_MU_SF_ID_SYST_DOWN = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_ID_SYST_DOWN);
                // Muon ID SF systematics (regular)
                m_weight_leptonSF_MU_SF_ID_STAT_LOWPT_UP = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_ID_STAT_LOWPT_UP);
                m_weight_leptonSF_MU_SF_ID_STAT_LOWPT_DOWN = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_ID_STAT_LOWPT_DOWN);
                m_weight_leptonSF_MU_SF_ID_SYST_LOWPT_UP = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_ID_SYST_LOWPT_UP);
                m_weight_leptonSF_MU_SF_ID_SYST_LOWPT_DOWN = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_ID_SYST_LOWPT_DOWN);
                // Muon isolation SF systematics
                m_weight_leptonSF_MU_SF_Isol_STAT_UP   = m_sfRetriever->leptonSF(event,top::topSFSyst::MU_SF_Isol_STAT_UP);
                m_weight_leptonSF_MU_SF_Isol_STAT_DOWN = m_sfRetriever->leptonSF(event,top::topSFSyst::MU_SF_Isol_STAT_DOWN);
                m_weight_leptonSF_MU_SF_Isol_SYST_UP   = m_sfRetriever->leptonSF(event,top::topSFSyst::MU_SF_Isol_SYST_UP);
                m_weight_leptonSF_MU_SF_Isol_SYST_DOWN = m_sfRetriever->leptonSF(event,top::topSFSyst::MU_SF_Isol_SYST_DOWN);
                m_weight_leptonSF_MU_SF_TTVA_STAT_UP   = m_sfRetriever->leptonSF(event,top::topSFSyst::MU_SF_TTVA_STAT_UP);
                m_weight_leptonSF_MU_SF_TTVA_STAT_DOWN = m_sfRetriever->leptonSF(event,top::topSFSyst::MU_SF_TTVA_STAT_DOWN);
                m_weight_leptonSF_MU_SF_TTVA_SYST_UP   = m_sfRetriever->leptonSF(event,top::topSFSyst::MU_SF_TTVA_SYST_UP);
                m_weight_leptonSF_MU_SF_TTVA_SYST_DOWN = m_sfRetriever->leptonSF(event,top::topSFSyst::MU_SF_TTVA_SYST_DOWN);


                m_weight_indiv_SF_EL_Trigger      = m_sfRetriever -> triggerSF(event,top::topSFSyst::nominal);
                m_weight_indiv_SF_EL_Trigger_UP   = m_sfRetriever -> triggerSF(event,top::topSFSyst::EL_SF_Trigger_UP);
                m_weight_indiv_SF_EL_Trigger_DOWN = m_sfRetriever -> triggerSF(event,top::topSFSyst::EL_SF_Trigger_DOWN);
                m_weight_indiv_SF_EL_Reco         = m_sfRetriever -> electronSF(event,top::topSFSyst::nominal,            top::topSFComp::RECO);
                m_weight_indiv_SF_EL_Reco_UP      = m_sfRetriever -> electronSF(event,top::topSFSyst::EL_SF_Reco_UP,      top::topSFComp::RECO);
                m_weight_indiv_SF_EL_Reco_DOWN    = m_sfRetriever -> electronSF(event,top::topSFSyst::EL_SF_Reco_DOWN,    top::topSFComp::RECO);
                m_weight_indiv_SF_EL_ID           = m_sfRetriever -> electronSF(event,top::topSFSyst::nominal,            top::topSFComp::ID);
                m_weight_indiv_SF_EL_ID_UP        = m_sfRetriever -> electronSF(event,top::topSFSyst::EL_SF_ID_UP,        top::topSFComp::ID);
                m_weight_indiv_SF_EL_ID_DOWN      = m_sfRetriever -> electronSF(event,top::topSFSyst::EL_SF_ID_DOWN,      top::topSFComp::ID);
                m_weight_indiv_SF_EL_Isol         = m_sfRetriever -> electronSF(event,top::topSFSyst::nominal,            top::topSFComp::ISOLATION);
                m_weight_indiv_SF_EL_Isol_UP      = m_sfRetriever -> electronSF(event,top::topSFSyst::EL_SF_Isol_UP,      top::topSFComp::ISOLATION);
                m_weight_indiv_SF_EL_Isol_DOWN    = m_sfRetriever -> electronSF(event,top::topSFSyst::EL_SF_Isol_DOWN,    top::topSFComp::ISOLATION);
                m_weight_indiv_SF_EL_ChargeID      = m_sfRetriever -> electronSF(event,top::topSFSyst::nominal,            top::topSFComp::CHARGEID);
                m_weight_indiv_SF_EL_ChargeID_UP   = m_sfRetriever -> electronSF(event,top::topSFSyst::EL_SF_ChargeID_UP,  top::topSFComp::CHARGEID);
                m_weight_indiv_SF_EL_ChargeID_DOWN = m_sfRetriever -> electronSF(event,top::topSFSyst::EL_SF_ChargeID_DOWN,top::topSFComp::CHARGEID);
                m_weight_indiv_SF_EL_ChargeMisID      = m_sfRetriever -> electronSF(event,top::topSFSyst::nominal,               top::topSFComp::CHARGEMISID);
                m_weight_indiv_SF_EL_ChargeMisID_STAT_UP   = m_sfRetriever -> electronSF(event,top::topSFSyst::EL_SF_ChargeMisID_STAT_UP,  top::topSFComp::CHARGEMISID);
                m_weight_indiv_SF_EL_ChargeMisID_STAT_DOWN = m_sfRetriever -> electronSF(event,top::topSFSyst::EL_SF_ChargeMisID_STAT_DOWN,top::topSFComp::CHARGEMISID);
                m_weight_indiv_SF_EL_ChargeMisID_SYST_UP   = m_sfRetriever -> electronSF(event,top::topSFSyst::EL_SF_ChargeMisID_SYST_UP,  top::topSFComp::CHARGEMISID);
                m_weight_indiv_SF_EL_ChargeMisID_SYST_DOWN = m_sfRetriever -> electronSF(event,top::topSFSyst::EL_SF_ChargeMisID_SYST_DOWN,top::topSFComp::CHARGEMISID);
                ATH_MSG_DEBUG("Electron Trigger SF = "<<m_weight_indiv_SF_EL_Trigger<<" + "<<m_weight_indiv_SF_EL_Trigger_UP<<" - "<<m_weight_indiv_SF_EL_Trigger_DOWN);
                ATH_MSG_DEBUG("Electron Reco SF = "<<m_weight_indiv_SF_EL_Reco<<" + "<<m_weight_indiv_SF_EL_Reco_UP<<" - "<<m_weight_indiv_SF_EL_Reco_DOWN);
                ATH_MSG_DEBUG("Electron ID SF = "<<m_weight_indiv_SF_EL_ID<<" + "<<m_weight_indiv_SF_EL_ID_UP<<" - "<<m_weight_indiv_SF_EL_ID_DOWN);
                ATH_MSG_DEBUG("Electron Charge ID SF = "<<m_weight_indiv_SF_EL_ChargeID<<" + "<<m_weight_indiv_SF_EL_ChargeID_UP<<" - "<<m_weight_indiv_SF_EL_ChargeID_DOWN);
                ATH_MSG_DEBUG("Electron Charge Mis ID SF = "<<m_weight_indiv_SF_EL_ChargeMisID<<" + "<<m_weight_indiv_SF_EL_ChargeMisID_STAT_UP<<" - "<<m_weight_indiv_SF_EL_ChargeMisID_STAT_DOWN<<" + "<<m_weight_indiv_SF_EL_ChargeMisID_SYST_UP<<" - "<<m_weight_indiv_SF_EL_ChargeMisID_SYST_DOWN);

                m_weight_indiv_SF_MU_Trigger      = m_sfRetriever -> triggerSF(event,top::topSFSyst::nominal);
                m_weight_indiv_SF_MU_Trigger_STAT_UP   = m_sfRetriever -> triggerSF(event,top::topSFSyst::MU_SF_Trigger_STAT_UP);
                m_weight_indiv_SF_MU_Trigger_STAT_DOWN = m_sfRetriever -> triggerSF(event,top::topSFSyst::MU_SF_Trigger_STAT_DOWN);
                m_weight_indiv_SF_MU_Trigger_SYST_UP   = m_sfRetriever -> triggerSF(event,top::topSFSyst::MU_SF_Trigger_SYST_UP);
                m_weight_indiv_SF_MU_Trigger_SYST_DOWN = m_sfRetriever -> triggerSF(event,top::topSFSyst::MU_SF_Trigger_SYST_DOWN);
                m_weight_indiv_SF_MU_ID             = m_sfRetriever -> muonSF(event,top::topSFSyst::nominal,            top::topSFComp::ID);
                // Muon ID SF systematics (regular)
                m_weight_indiv_SF_MU_ID_STAT_UP = m_sfRetriever->muonSF(event, top::topSFSyst::MU_SF_ID_STAT_UP, top::topSFComp::ID);
                m_weight_indiv_SF_MU_ID_STAT_DOWN = m_sfRetriever->muonSF(event, top::topSFSyst::MU_SF_ID_STAT_DOWN, top::topSFComp::ID);
                m_weight_indiv_SF_MU_ID_SYST_UP = m_sfRetriever->muonSF(event, top::topSFSyst::MU_SF_ID_SYST_UP, top::topSFComp::ID);
                m_weight_indiv_SF_MU_ID_SYST_DOWN = m_sfRetriever->muonSF(event, top::topSFSyst::MU_SF_ID_SYST_DOWN, top::topSFComp::ID);
                // Muon ID SF systematics (low pt)
                m_weight_indiv_SF_MU_ID_STAT_LOWPT_UP = m_sfRetriever->muonSF(event, top::topSFSyst::MU_SF_ID_STAT_LOWPT_UP, top::topSFComp::ID);
                m_weight_indiv_SF_MU_ID_STAT_LOWPT_DOWN = m_sfRetriever->muonSF(event, top::topSFSyst::MU_SF_ID_STAT_LOWPT_DOWN, top::topSFComp::ID);
                m_weight_indiv_SF_MU_ID_SYST_LOWPT_UP = m_sfRetriever->muonSF(event, top::topSFSyst::MU_SF_ID_SYST_LOWPT_UP, top::topSFComp::ID);
                m_weight_indiv_SF_MU_ID_SYST_LOWPT_DOWN = m_sfRetriever->muonSF(event, top::topSFSyst::MU_SF_ID_SYST_LOWPT_DOWN, top::topSFComp::ID);
                // Muon isolation SF systematics
                m_weight_indiv_SF_MU_Isol           = m_sfRetriever -> muonSF(event,top::topSFSyst::nominal,            top::topSFComp::ISOLATION);
                m_weight_indiv_SF_MU_Isol_STAT_UP   = m_sfRetriever -> muonSF(event,top::topSFSyst::MU_SF_Isol_STAT_UP,      top::topSFComp::ISOLATION);
                m_weight_indiv_SF_MU_Isol_STAT_DOWN = m_sfRetriever -> muonSF(event,top::topSFSyst::MU_SF_Isol_STAT_DOWN,    top::topSFComp::ISOLATION);
                m_weight_indiv_SF_MU_Isol_SYST_UP   = m_sfRetriever -> muonSF(event,top::topSFSyst::MU_SF_Isol_SYST_UP,      top::topSFComp::ISOLATION);
                m_weight_indiv_SF_MU_Isol_SYST_DOWN = m_sfRetriever -> muonSF(event,top::topSFSyst::MU_SF_Isol_SYST_DOWN,    top::topSFComp::ISOLATION);
                m_weight_indiv_SF_MU_TTVA           = m_sfRetriever -> muonSF(event,top::topSFSyst::nominal,            top::topSFComp::TTVA);
                m_weight_indiv_SF_MU_TTVA_STAT_UP   = m_sfRetriever -> muonSF(event,top::topSFSyst::MU_SF_TTVA_STAT_UP,      top::topSFComp::TTVA);
                m_weight_indiv_SF_MU_TTVA_STAT_DOWN = m_sfRetriever -> muonSF(event,top::topSFSyst::MU_SF_TTVA_STAT_DOWN,    top::topSFComp::TTVA);
                m_weight_indiv_SF_MU_TTVA_SYST_UP   = m_sfRetriever -> muonSF(event,top::topSFSyst::MU_SF_TTVA_SYST_UP,      top::topSFComp::TTVA);
                m_weight_indiv_SF_MU_TTVA_SYST_DOWN = m_sfRetriever -> muonSF(event,top::topSFSyst::MU_SF_TTVA_SYST_DOWN,    top::topSFComp::TTVA);


                ATH_MSG_DEBUG("Muon Trigger SF = "<<m_weight_indiv_SF_MU_Trigger<<" + "<< m_weight_indiv_SF_MU_Trigger_STAT_UP<<" - "<<  m_weight_indiv_SF_MU_Trigger_STAT_DOWN<<"     + "<< m_weight_indiv_SF_MU_Trigger_SYST_UP<<" - "<< m_weight_indiv_SF_MU_Trigger_SYST_DOWN);

                ATH_MSG_DEBUG("Muon ID SF = "<<m_weight_indiv_SF_MU_ID<<" + "<< m_weight_indiv_SF_MU_ID_STAT_UP<<" - "<<  m_weight_indiv_SF_MU_ID_STAT_DOWN<<"     + "<< m_weight_indiv_SF_MU_ID_SYST_UP<<" - "<< m_weight_indiv_SF_MU_ID_SYST_DOWN);


                if (m_config->useTaus()) {
                  // Tau-electron overlap removal
                  m_weight_tauSF_ELEOLR_UP = m_sfRetriever->tauSF(event, top::topSFSyst::TAU_SF_ELEOLR_TOTAL_UP);
                  m_weight_tauSF_ELEOLR_DOWN = m_sfRetriever->tauSF(event, top::topSFSyst::TAU_SF_ELEOLR_TOTAL_DOWN);
                  m_weight_tauSF_TRUEELECTRON_ELEOLR_UP = m_sfRetriever->tauSF(event, top::topSFSyst::TAU_SF_TRUEELECTRON_ELEOLR_TOTAL_UP);
                  m_weight_tauSF_TRUEELECTRON_ELEOLR_DOWN = m_sfRetriever->tauSF(event, top::topSFSyst::TAU_SF_TRUEELECTRON_ELEOLR_TOTAL_DOWN);
                  // Tau Jet IDWP
                  m_weight_tauSF_JETID_UP = m_sfRetriever->tauSF(event, top::topSFSyst::TAU_SF_JETID_TOTAL_UP);
                  m_weight_tauSF_JETID_DOWN = m_sfRetriever->tauSF(event, top::topSFSyst::TAU_SF_JETID_TOTAL_DOWN);
                  m_weight_tauSF_JETID_HIGHPT_UP = m_sfRetriever->tauSF(event, top::topSFSyst::TAU_SF_JETID_HIGHPT_UP);
                  m_weight_tauSF_JETID_HIGHPT_DOWN = m_sfRetriever->tauSF(event, top::topSFSyst::TAU_SF_JETID_HIGHPT_DOWN);
                  // Tau reconstruction
                  m_weight_tauSF_RECO_UP = m_sfRetriever->tauSF(event, top::topSFSyst::TAU_SF_RECO_TOTAL_UP);
                  m_weight_tauSF_RECO_DOWN = m_sfRetriever->tauSF(event, top::topSFSyst::TAU_SF_RECO_TOTAL_DOWN);
                  m_weight_tauSF_RECO_HIGHPT_UP = m_sfRetriever->tauSF(event, top::topSFSyst::TAU_SF_RECO_HIGHPT_UP);
                  m_weight_tauSF_RECO_HIGHPT_DOWN = m_sfRetriever->tauSF(event, top::topSFSyst::TAU_SF_RECO_HIGHPT_DOWN);
                }

                if (m_config->usePhotons()) {
                  m_weight_photonSF_ID_UP = m_sfRetriever->photonSF(event, top::topSFSyst::PHOTON_IDSF_UP);
                  m_weight_photonSF_ID_DOWN = m_sfRetriever->photonSF(event, top::topSFSyst::PHOTON_IDSF_DOWN);
                  m_weight_photonSF_effIso = m_sfRetriever->photonSF(event, top::topSFSyst::PHOTON_EFF_ISO);
                  m_weight_photonSF_effLowPtIso_UP = m_sfRetriever->photonSF(event, top::topSFSyst::PHOTON_EFF_LOWPTISO_UP);
                  m_weight_photonSF_effLowPtIso_DOWN = m_sfRetriever->photonSF(event, top::topSFSyst::PHOTON_EFF_LOWPTISO_DOWN);
                  m_weight_photonSF_effTrkIso_UP = m_sfRetriever->photonSF(event, top::topSFSyst::PHOTON_EFF_TRKISO_UP);
                  m_weight_photonSF_effTrkIso_DOWN = m_sfRetriever->photonSF(event, top::topSFSyst::PHOTON_EFF_TRKISO_DOWN);
                }


                m_weight_jvt_up = m_sfRetriever->jvtSF(event, top::topSFSyst::JVT_UP);
                m_weight_jvt_down = m_sfRetriever->jvtSF(event, top::topSFSyst::JVT_DOWN);
            }

            // for b-tagging SFs, can also have systematic-shifted in systematics trees
            if (event.m_hashValue == m_config->nominalHashValue() || m_config->dumpBtagSystsInSystTrees()) {
                for( auto& tagWP : m_config -> bTagWP_available()) {
                  // skip uncalibrated though available WPs
                  if (std::find(m_config->bTagWP_calibrated().begin(), m_config->bTagWP_calibrated().end(), tagWP) == m_config->bTagWP_calibrated().end()) continue;
                  m_sfRetriever->btagSF_eigen_vars(event, top::topSFSyst::BTAG_SF_EIGEN_B,
                                                    m_weight_bTagSF_eigen_B_up[tagWP],
                                                    m_weight_bTagSF_eigen_B_down[tagWP], tagWP);
                  m_sfRetriever->btagSF_eigen_vars(event, top::topSFSyst::BTAG_SF_EIGEN_C,
                                                    m_weight_bTagSF_eigen_C_up[tagWP],
                                                    m_weight_bTagSF_eigen_C_down[tagWP], tagWP);
                  m_sfRetriever->btagSF_eigen_vars(event, top::topSFSyst::BTAG_SF_EIGEN_LIGHT,
                                                    m_weight_bTagSF_eigen_Light_up[tagWP],
                                                    m_weight_bTagSF_eigen_Light_down[tagWP], tagWP);
                  for (auto name : m_config->btagging_namedSysts(tagWP)) {
                    m_weight_bTagSF_named_up[tagWP][name] = m_sfRetriever->btagSF( event, top::topSFSyst::BTAG_SF_NAMED_UP, tagWP, false, name );
                    m_weight_bTagSF_named_down[tagWP][name] = m_sfRetriever->btagSF( event, top::topSFSyst::BTAG_SF_NAMED_DOWN, tagWP, false, name );
                  }
                }
                if (m_config->useTrackJets()) {
                  for( auto& tagWP : m_config -> bTagWP_available_trkJet()) {
                    // skip uncalibrated though available WPs
                    if (std::find(m_config->bTagWP_calibrated().begin(), m_config->bTagWP_calibrated().end(), tagWP) == m_config->bTagWP_calibrated().end()) continue;
                    m_sfRetriever->btagSF_eigen_vars(event, top::topSFSyst::BTAG_SF_EIGEN_B,
                                                      m_weight_trackjet_bTagSF_eigen_B_up[tagWP],
                                                      m_weight_trackjet_bTagSF_eigen_B_down[tagWP], tagWP, true);
                    m_sfRetriever->btagSF_eigen_vars(event, top::topSFSyst::BTAG_SF_EIGEN_C,
                                                      m_weight_trackjet_bTagSF_eigen_C_up[tagWP],
                                                      m_weight_trackjet_bTagSF_eigen_C_down[tagWP], tagWP, true);
                    m_sfRetriever->btagSF_eigen_vars(event, top::topSFSyst::BTAG_SF_EIGEN_LIGHT,
                                                      m_weight_trackjet_bTagSF_eigen_Light_up[tagWP],
                                                      m_weight_trackjet_bTagSF_eigen_Light_down[tagWP], tagWP, true);
                    for (auto name : m_config->btagging_namedSysts(tagWP)) {
                      m_weight_trackjet_bTagSF_named_up[tagWP][name] = m_sfRetriever->btagSF( event, top::topSFSyst::BTAG_SF_NAMED_UP, tagWP, true, name);
                      m_weight_trackjet_bTagSF_named_down[tagWP][name] = m_sfRetriever->btagSF( event, top::topSFSyst::BTAG_SF_NAMED_DOWN, tagWP, true, name );
                    }
                  }
                }
            }
        }

	/// Bootstrapping poisson weights - Moved to run for MC and data
	if(m_config->saveBootstrapWeights()){
	  m_weight_poisson = event.m_info->auxdataConst<std::vector<int> >("weight_poisson");
	}

        ///-- weights for matrix-method fakes estimate --///
        if (!m_config->isMC() && m_config->doFakesMMWeights()) {
	    top::TopFakesMMWeightCalculator const* fakesMMWeightCalc(nullptr);
	    if ( asg::ToolStore::contains<top::TopFakesMMWeightCalculator>("MMWeightCalculator") ) {
	      fakesMMWeightCalc = asg::ToolStore::get<top::TopFakesMMWeightCalculator>("MMWeightCalculator");
	    }
	    else {
	      ATH_MSG_ERROR("EventSaverFlatNtuple::initialize" );
	      throw std::runtime_error("Unable to retrieve top::TopFakesMMWeightCalculator tool");
	    }
	    for(const auto& branchName : m_extraBranches) {//loop on selections
	      for(const auto& conf : fakesMMWeightCalc->GetFakesMMConfigNames(branchName)) {
		std::string MMweight_branch_name = "fakesMM_weight_" + branchName + "_" + conf;
		std::string decorName = "MMWeight_"; decorName += branchName; decorName += "_"; decorName += conf;
		if( event.m_info->isAvailable<float>(decorName.c_str()) ) {
		  m_fakesMM_weights[branchName][conf] = event.m_info->auxdataConst<float>(decorName.c_str());
		}
		else {//if decoration is not present, it means this weight is not relevant for this channel - a hurtless weight=1. is then applied
		   m_fakesMM_weights[branchName][conf] = 1.;
		}
	      }
	    }
        }

        //event info
        m_eventNumber = event.m_info -> eventNumber();
        m_runNumber   = event.m_info -> runNumber();
        m_randomRunNumber = 0;
        if (m_config->isMC() && m_config->doPileupReweighting()) {
          if (event.m_info->isAvailable<unsigned int>("RandomRunNumber"))
            m_randomRunNumber = event.m_info->auxdataConst<unsigned int>("RandomRunNumber");
        }
        m_mcChannelNumber = 0;

        if (m_config->isMC())
            m_mcChannelNumber = event.m_info -> mcChannelNumber();

        // (non-collision-)background flags
        m_backgroundFlags = 0;
	if (event.m_info->isAvailable<unsigned int>("backgroundFlags"))
	  m_backgroundFlags = event.m_info->auxdataConst<unsigned int>("backgroundFlags");

	// hasBadMuon flag
	m_hasBadMuon = 0;
	if (m_config->useMuons() && event.m_info->isAvailable<char>("AnalysisTop_HASBADMUON"))
	  m_hasBadMuon = event.m_info->auxdataConst<char>("AnalysisTop_HASBADMUON");

        // mu values (original and corrected by pileup reweighting tool)
        m_mu_original = event.m_info->averageInteractionsPerCrossing();

        // set these all to zero (maybe not the most sensible value...)
        m_mu = 0;

        if (m_config->isMC())
            m_mu = m_mu_original;
        if (!m_config->isMC()) {
            // If we have the corrected mu value from pileup reweighting then save that
            // instead of mu value in (d)xAOD.
            if( event.m_info->isAvailable<float>("corrected_averageInteractionsPerCrossing") )
                m_mu = event.m_info->auxdataConst<float>("corrected_averageInteractionsPerCrossing");
        }

        ATH_MSG_DEBUG(" mu = "<<m_mu_original<<" -> "<<m_mu);

        //electrons
        if (m_config->useElectrons()) {
            unsigned int i(0);
            unsigned int n_electrons = event.m_electrons.size();
            m_el_pt.resize(n_electrons);
            m_el_eta.resize(n_electrons);
            m_el_cl_eta.resize(n_electrons);
            m_el_phi.resize(n_electrons);
            m_el_e.resize(n_electrons);
            m_el_charge.resize(n_electrons);
            m_el_topoetcone20.resize(n_electrons);
            m_el_ptvarcone20.resize(n_electrons);
            m_el_isTight.resize(n_electrons);
            m_el_CF.resize(n_electrons);
            for( const auto& trigger : m_el_trigMatched )
                m_el_trigMatched[trigger.first].resize(n_electrons);
            m_el_d0sig.resize(n_electrons);
            m_el_delta_z0_sintheta.resize(n_electrons);
            if (m_config->isMC()) {
              m_el_true_type.resize(n_electrons);
              m_el_true_origin.resize(n_electrons);
              m_el_true_typebkg.resize(n_electrons);
              m_el_true_originbkg.resize(n_electrons);
            }

            for (const auto* const elPtr : event.m_electrons) {
                m_el_pt[i] = elPtr->pt();
                m_el_eta[i] = elPtr->eta();
                m_el_cl_eta[i] = elPtr->caloCluster()->etaBE(2);
                m_el_phi[i] = elPtr->phi();
                m_el_e[i] = elPtr->e();
                m_el_charge[i] = elPtr->charge();
                elPtr->isolationValue( m_el_topoetcone20[i] , xAOD::Iso::topoetcone20 );
                elPtr->isolationValue( m_el_ptvarcone20[i] , xAOD::Iso::ptvarcone20 );
                if (event.m_isLoose) {
                    if (elPtr->isAvailable<char>("passPreORSelection")) {
                        m_el_isTight[i] = elPtr->auxdataConst<char>("passPreORSelection");
                    }
                }
		if (elPtr->isAvailable<char>("passChargeID"))
		  m_el_CF[i] = elPtr->auxdataConst<char>("passChargeID");
                for( const auto& trigger : m_el_trigMatched ){
                    std::string trig = "TRIGMATCH_"+trigger.first;
                    m_el_trigMatched[trigger.first][i] = elPtr->auxdataConst<char>(trig);
                }
                if( elPtr->isAvailable<float>("d0sig") )
                    m_el_d0sig[i] = elPtr->auxdataConst<float>("d0sig");
                if( elPtr->isAvailable<float>("delta_z0_sintheta") )
                    m_el_delta_z0_sintheta[i] = elPtr->auxdataConst<float>("delta_z0_sintheta");

                //retrieve the truth-matching variables from MCTruthClassifier
                if (m_config->isMC()) {
                  m_el_true_type[i] = 0;
                  m_el_true_origin[i] = 0;
                  m_el_true_typebkg[i] = 0;
                  m_el_true_originbkg[i] = 0;
                  static SG::AuxElement::Accessor<int> typeel("truthType");
                  static SG::AuxElement::Accessor<int> origel("truthOrigin");
                  static SG::AuxElement::Accessor<int> typebkgel("bkgTruthType");
                  static SG::AuxElement::Accessor<int> origbkgel("bkgTruthOrigin");
                  if (typeel.isAvailable(*elPtr)) m_el_true_type[i] = typeel(*elPtr);
                  if (origel.isAvailable(*elPtr)) m_el_true_origin[i] = origel(*elPtr);
                  if (typebkgel.isAvailable(*elPtr)) m_el_true_typebkg[i] = typebkgel(*elPtr);
                  if (origbkgel.isAvailable(*elPtr)) m_el_true_originbkg[i] = origbkgel(*elPtr);
                }
                ++i;
            }
        }

        //muons
        if (m_config->useMuons()) {
            unsigned int i(0);
            unsigned int n_muons = event.m_muons.size();
            m_mu_pt.resize(n_muons);
            m_mu_eta.resize(n_muons);
            m_mu_phi.resize(n_muons);
            m_mu_e.resize(n_muons);
            m_mu_charge.resize(n_muons);
            m_mu_topoetcone20.resize(n_muons);
            m_mu_ptvarcone30.resize(n_muons);
            m_mu_isTight.resize(n_muons);
            for( const auto& trigger : m_mu_trigMatched )
                m_mu_trigMatched[trigger.first].resize(n_muons);
            m_mu_d0sig.resize(n_muons);
            m_mu_delta_z0_sintheta.resize(n_muons);
            if (m_config->isMC()) {
              m_mu_true_type.resize(n_muons);
              m_mu_true_origin.resize(n_muons);
            }

            for (const auto* const muPtr : event.m_muons) {
                m_mu_pt[i] = muPtr->pt();
                m_mu_eta[i] = muPtr->eta();
                m_mu_phi[i] = muPtr->phi();
                m_mu_e[i] = muPtr->e();
                m_mu_charge[i] = muPtr->charge();
                muPtr->isolation( m_mu_topoetcone20[i] , xAOD::Iso::topoetcone20 );
                muPtr->isolation( m_mu_ptvarcone30[i] , xAOD::Iso::ptvarcone30 );
                if (event.m_isLoose) {
                    if (muPtr->isAvailable<char>("passPreORSelection")) {
                        m_mu_isTight[i] = muPtr->auxdataConst<char>("passPreORSelection");
                    }
                }
                for( const auto& trigger : m_mu_trigMatched ){
                    std::string trig = "TRIGMATCH_"+trigger.first;
                    m_mu_trigMatched[trigger.first][i] = muPtr->auxdataConst<char>(trig);
                }
                if( muPtr->isAvailable<float>("d0sig") )
                    m_mu_d0sig[i] = muPtr->auxdataConst<float>("d0sig");
                if( muPtr->isAvailable<float>("delta_z0_sintheta") )
                    m_mu_delta_z0_sintheta[i] = muPtr->auxdataConst<float>("delta_z0_sintheta");

                //retrieve the truth-matching variables from MCTruthClassifier
                if (m_config->isMC()) {
                  static SG::AuxElement::Accessor<int> acc_mctt("truthType");
                  static SG::AuxElement::Accessor<int> acc_mcto("truthOrigin");
                  m_mu_true_type[i]=0;
                  m_mu_true_origin[i]=0;
                  const xAOD::TrackParticle* mutrack = muPtr->primaryTrackParticle();
                  if (mutrack!=nullptr) {
                      if (acc_mctt.isAvailable(*mutrack)) m_mu_true_type[i] = acc_mctt(*mutrack);
                      if (acc_mcto.isAvailable(*mutrack)) m_mu_true_origin[i] = acc_mcto(*mutrack);
                  }
                }
                ++i;
            }
        }

        //photons
        if (m_config->usePhotons()) {
            unsigned int i(0);
            m_ph_pt.resize(event.m_photons.size());
            m_ph_eta.resize(event.m_photons.size());
            m_ph_phi.resize(event.m_photons.size());
            m_ph_e.resize(event.m_photons.size());
            m_ph_iso.resize(event.m_photons.size());
            for (const auto* const phPtr : event.m_photons) {
                m_ph_pt[i] = phPtr->pt();
                m_ph_eta[i] = phPtr->eta();
                m_ph_phi[i] = phPtr->phi();
                m_ph_e[i] = phPtr->e();

                m_ph_iso[i] = 0.;
                if (phPtr->isAvailable<float>("ptvarcone20"))
                    m_ph_iso[i] = phPtr->auxdata<float>("ptvarcone20");

                ++i;
            }
        }


        //taus
        if (m_config->useTaus()) {
            unsigned int i(0);
            m_tau_pt.resize(event.m_tauJets.size());
            m_tau_eta.resize(event.m_tauJets.size());
            m_tau_phi.resize(event.m_tauJets.size());
            m_tau_charge.resize(event.m_tauJets.size());
            for (const auto* const tauPtr : event.m_tauJets) {
                m_tau_pt[i] = tauPtr->pt();
                m_tau_eta[i] = tauPtr->eta();
                m_tau_phi[i] = tauPtr->phi();
                m_tau_charge[i] = tauPtr->charge();
                ++i;
            }
        }

        //jets
        if (m_config->useJets()) {
            unsigned int i(0);
            m_jet_pt.resize(event.m_jets.size());
            m_jet_eta.resize(event.m_jets.size());
            m_jet_phi.resize(event.m_jets.size());
            m_jet_e.resize(event.m_jets.size());
            m_jet_mv2c00.resize(event.m_jets.size());
            m_jet_mv2c10.resize(event.m_jets.size());
            m_jet_mv2c20.resize(event.m_jets.size());
            m_jet_ip3dsv1.resize(event.m_jets.size());
            m_jet_jvt.resize(event.m_jets.size());
            m_jet_passfjvt.resize(event.m_jets.size());
	    // R21 b-tagging
	    if(m_config->getReleaseSeries() == 25){
	      m_jet_MV2c10mu.resize(event.m_jets.size());
	      m_jet_MV2c10rnn.resize(event.m_jets.size());
	      m_jet_DL1.resize(event.m_jets.size());
	      m_jet_DL1mu.resize(event.m_jets.size());
	      m_jet_DL1rnn.resize(event.m_jets.size());
	      m_jet_MV2cl100.resize(event.m_jets.size());
	      m_jet_MV2c100.resize(event.m_jets.size());
	      m_jet_DL1_pu.resize(event.m_jets.size());
	      m_jet_DL1_pc.resize(event.m_jets.size());
	      m_jet_DL1_pb.resize(event.m_jets.size());
	      m_jet_DL1mu_pu.resize(event.m_jets.size());
	      m_jet_DL1mu_pc.resize(event.m_jets.size());
	      m_jet_DL1mu_pb.resize(event.m_jets.size());
	      m_jet_DL1rnn_pu.resize(event.m_jets.size());
	      m_jet_DL1rnn_pc.resize(event.m_jets.size());
	      m_jet_DL1rnn_pb.resize(event.m_jets.size());
	    }
            if (m_config->isMC()) {
              m_jet_truthflav.resize(event.m_jets.size());
              m_jet_truthPartonLabel.resize(event.m_jets.size());
              m_jet_isTrueHS.resize(event.m_jets.size());
            }
            for( auto& tagWP : m_config -> bTagWP_available()){
              if (tagWP!= "Continuous") m_jet_isbtagged[tagWP].resize(event.m_jets.size());
              else m_jet_tagWeightBin.resize(event.m_jets.size());
            }
            for (const auto* const jetPtr : event.m_jets) {
                m_jet_pt[i] = jetPtr->pt();
                m_jet_eta[i] = jetPtr->eta();
                m_jet_phi[i] = jetPtr->phi();
                m_jet_e[i] = jetPtr->e();
                double SV1IP3 = -999;
		// In R21, list of b-tagging variables is changing and this is outdated
		const xAOD::BTagging* btag(nullptr);
		btag = jetPtr->btagging();
		if(m_config->getReleaseSeries() == 24){
		  if (btag) SV1IP3 = btag->SV1plusIP3D_discriminant();
		}
                m_jet_ip3dsv1[i] = SV1IP3;
                if (m_config->isMC()) {
                  m_jet_truthflav[i] = -99;
                  if(jetPtr->isAvailable<int>("HadronConeExclTruthLabelID")){
                    jetPtr->getAttribute("HadronConeExclTruthLabelID", m_jet_truthflav[i]);
                  }
                  m_jet_truthPartonLabel[i] = -99;
                  if(jetPtr->isAvailable<int>("PartonTruthLabelID")){
                    jetPtr->getAttribute("PartonTruthLabelID", m_jet_truthPartonLabel[i]);
                  }
                  m_jet_isTrueHS[i] = false;
                  if(jetPtr->isAvailable<char>("AnalysisTop_isHS")){
                    jetPtr->getAttribute("AnalysisTop_isHS", m_jet_isTrueHS[i]);
                  }
                }
                for( auto& tagWP : m_config -> bTagWP_available()){
                  if (tagWP!= "Continuous") {
                    m_jet_isbtagged[tagWP][i] = false;
                    if(jetPtr->isAvailable<char>("isbtagged_"+tagWP))
                      m_jet_isbtagged[tagWP][i] = jetPtr->auxdataConst<char>("isbtagged_"+tagWP);
                  }
                  else {
                    m_jet_tagWeightBin[i] = -2;// AT default value
                    if(jetPtr->isAvailable<int>("tagWeightBin"))
                      m_jet_tagWeightBin[i] = jetPtr->auxdataConst<int>("tagWeightBin");
                  }
                }

                // for studies on high performance b-tagging
                // the following are in DC14
                double mvx = -999;
                if (btag) btag->MVx_discriminant("MV2c00", mvx);
                m_jet_mv2c00[i] = mvx;
                mvx = -999;
                if (btag) btag->MVx_discriminant("MV2c10", mvx);
                m_jet_mv2c10[i] = mvx;
                mvx = -999;
                if (btag) btag->MVx_discriminant("MV2c20", mvx);
                m_jet_mv2c20[i] = mvx;

                m_jet_jvt[i] = -1;
                if (jetPtr->isAvailable<float>("AnalysisTop_JVT")) {
                    m_jet_jvt[i] = jetPtr->auxdataConst<float>("AnalysisTop_JVT");
                }
                m_jet_passfjvt[i] = -1;
                if (jetPtr->isAvailable<char>("passFJVT")) {
                    m_jet_passfjvt[i] = jetPtr->getAttribute<char>("passFJVT");
                }

		// BTagging variables supported for R21 but method is only in newer version so preprocessor requirement
		#if ROOTCORE_RELEASE_SERIES >= 25
		if(m_config->getReleaseSeries() == 25){
		  m_jet_MV2c10mu[i] = -999;
		  m_jet_MV2c10rnn[i] = -999;
		  // Cannot currently calculate DLX from the EDM, so only store weight components
		  m_jet_DL1[i] = -999;
		  m_jet_DL1mu[i] = -999;
		  m_jet_DL1rnn[i] = -999;
		  m_jet_MV2cl100[i] = -999;
		  m_jet_MV2c100[i] = -999;
		  m_jet_DL1_pu[i] = -999;
		  m_jet_DL1_pc[i] = -999;
		  m_jet_DL1_pb[i] = -999;
		  m_jet_DL1mu_pu[i] = -999;
		  m_jet_DL1mu_pc[i] = -999;
		  m_jet_DL1mu_pb[i] = -999;
		  m_jet_DL1rnn_pu[i] = -999;
                  m_jet_DL1rnn_pc[i] = -999;
		  m_jet_DL1rnn_pb[i] = -999;

		  if(btag){
		    // MVX
		    mvx = -999;
		    btag->MVx_discriminant("MV2c10mu", mvx);
		    m_jet_MV2c10mu[i] = mvx;
		    
		    mvx = -999;
		    btag->MVx_discriminant("MV2c10rnn", mvx);
		    m_jet_MV2c10rnn[i] = mvx;
		    
		    mvx = -999;
                    btag->MVx_discriminant("MV2cl100", mvx);
                    m_jet_MV2cl100[i] = mvx;

		    mvx = -999;
                    btag->MVx_discriminant("MV2c100", mvx);
                    m_jet_MV2c100[i] = mvx;
		    
		    // DL1
		    double _pu, _pc, _pb = -999;

		    // DL1rnnCTag - Calculation in xAODBTaggingEfficiency/BTaggingSelectionTool.cxx but depends on fraction
		    // so just providing the DL1rnn weights to construct tagger offline
		    btag->pu("DL1rnn",_pu);
		    btag->pb("DL1rnn",_pb);
                    btag->pc("DL1rnn",_pc);
		    m_jet_DL1rnn_pu[i] = _pu;
                    m_jet_DL1rnn_pc[i] = _pc;
                    m_jet_DL1rnn_pb[i] = _pb;		    
		    // DL1mu - as above
		    btag->pu("DL1mu",_pu);
                    btag->pb("DL1mu",_pb);
                    btag->pc("DL1mu",_pc);
		    m_jet_DL1mu_pu[i] = _pu;
		    m_jet_DL1mu_pc[i] = _pc;
		    m_jet_DL1mu_pb[i] = _pb;
		    // DL1 - as above
		    btag->pu("DL1",_pu);
                    btag->pb("DL1",_pb);
                    btag->pc("DL1",_pc);
		    m_jet_DL1_pu[i] = _pu;
		    m_jet_DL1_pc[i] = _pc;
		    m_jet_DL1_pb[i] = _pb;
		    
		  }
		} // getReleaseSeries == 25
                #endif // ROOTCORE_RELEASE_SERIES

                ++i;
            }
        }

        //large-R jets
        if (m_config->useLargeRJets()) {
            unsigned int i(0);
            m_ljet_pt.resize(event.m_largeJets.size());
            m_ljet_eta.resize(event.m_largeJets.size());
            m_ljet_phi.resize(event.m_largeJets.size());
            m_ljet_e.resize(event.m_largeJets.size());
            m_ljet_m.resize(event.m_largeJets.size());
            m_ljet_sd12.resize(event.m_largeJets.size());
            m_ljet_isTopTagged_50.resize(  event.m_largeJets.size() );
            m_ljet_isTopTagged_80.resize(  event.m_largeJets.size() );
            m_ljet_isWTagged_80.resize(   event.m_largeJets.size() );
            m_ljet_isWTagged_50.resize( event.m_largeJets.size() );
            m_ljet_isZTagged_80.resize(   event.m_largeJets.size() );
            m_ljet_isZTagged_50.resize( event.m_largeJets.size() );
            for (const auto* const jetPtr : event.m_largeJets) {
                m_ljet_pt[i] = jetPtr->pt();
                m_ljet_eta[i] = jetPtr->eta();
                m_ljet_phi[i] = jetPtr->phi();
                m_ljet_e[i] = jetPtr->e();
                m_ljet_m[i] = jetPtr->m();
                m_ljet_sd12[i] = 0;
                float Split12 = 0;
                jetPtr->getAttribute("Split12", Split12);
                m_ljet_sd12[i] = Split12;

                try { m_ljet_isTopTagged_50[i]  = jetPtr->getAttribute<char>("isTopTagged_50" );} catch (...) { }
                try { m_ljet_isTopTagged_80[i]  = jetPtr->getAttribute<char>("isTopTagged_80" );} catch (...) { }
                try { m_ljet_isWTagged_80[i] = jetPtr->getAttribute<char>("isWTagged_80");} catch (...) { }
                try { m_ljet_isWTagged_50[i] = jetPtr->getAttribute<char>("isWTagged_50");} catch (...) { }
                try { m_ljet_isZTagged_80[i] = jetPtr->getAttribute<char>("isZTagged_80"); } catch (...) { }
                try { m_ljet_isZTagged_50[i] = jetPtr->getAttribute<char>("isZTagged_50"); } catch (...) { }

                ++i;
            }
        }

        //track jets
        if (m_config->useTrackJets()) {
            unsigned int i(0);
            m_tjet_pt.resize(event.m_trackJets.size());
            m_tjet_eta.resize(event.m_trackJets.size());
            m_tjet_phi.resize(event.m_trackJets.size());
            m_tjet_e.resize(event.m_trackJets.size());
            m_tjet_mv2c00.resize(event.m_trackJets.size());
            m_tjet_mv2c10.resize(event.m_trackJets.size());
            m_tjet_mv2c20.resize(event.m_trackJets.size());
            for( auto& tagWP : m_config -> bTagWP_available_trkJet()) {
              if (tagWP!= "Continuous") m_tjet_isbtagged[tagWP].resize(event.m_trackJets.size());
              else m_tjet_tagWeightBin.resize(event.m_trackJets.size());
            }
            for (const auto* const jetPtr : event.m_trackJets) {
                m_tjet_pt[i] = jetPtr->pt();
                m_tjet_eta[i] = jetPtr->eta();
                m_tjet_phi[i] = jetPtr->phi();
                m_tjet_e[i] = jetPtr->e();

                const xAOD::BTagging* btag(nullptr);
                btag = jetPtr->btagging();
                double mvx = -999;
                if (btag) btag->MVx_discriminant("MV2c00", mvx);
                m_tjet_mv2c00[i] = mvx;
                mvx = -999;
                if (btag) btag->MVx_discriminant("MV2c10", mvx);
                m_tjet_mv2c10[i] = mvx;
                mvx = -999;
                if (btag) btag->MVx_discriminant("MV2c20", mvx);
                m_tjet_mv2c20[i] = mvx;
                for( auto& tagWP : m_config -> bTagWP_available_trkJet()){
                  if (tagWP!= "Continuous") {
                    m_tjet_isbtagged[tagWP][i] = false;
                    if(jetPtr->isAvailable<char>("isbtagged_"+tagWP))
                      m_tjet_isbtagged[tagWP][i] = jetPtr->auxdataConst<char>("isbtagged_"+tagWP);
                  }
                  else {
                    m_tjet_tagWeightBin[i] = -2;// AT default value
                    if(jetPtr->isAvailable<int>("tagWeightBin"))
                      m_tjet_tagWeightBin[i] = jetPtr->auxdataConst<int>("tagWeightBin");
                  }
                }
                ++i;
            }
        }

	if (m_makeRCJets){
	  // Execute the re-clustering code
	  // - make jet container of small-r jets in the event, put it in TStore, do re-clustering
	  top::check(m_rc->execute(event),"Failed to execute RCJetMC15 container");

	  // Get the name of the container of re-clustered jets in TStore
	  m_RCJetContainer = m_rc->rcjetContainerName(event.m_hashValue,event.m_isLoose);

	  // -- Retrieve the re-clustered jets from TStore & save good re-clustered jets -- //
	  const xAOD::JetContainer* rc_jets(nullptr);
	  top::check(evtStore()->retrieve(rc_jets,m_RCJetContainer),"Failed to retrieve RC JetContainer");

	  // re-clustered jet substructure
	  static SG::AuxElement::ConstAccessor<float> RCSplit12("Split12");
	  static SG::AuxElement::ConstAccessor<float> RCSplit23("Split23");

	  // Initialize the vectors to be saved as branches
	  unsigned int sizeOfRCjets(rc_jets->size());

    m_rcjet_pt.clear();
    m_rcjet_eta.clear();
    m_rcjet_phi.clear();
    m_rcjet_e.clear();
    m_rcjet_d12.clear();
    m_rcjet_d23.clear();
    m_rcjetsub_pt.clear();
    m_rcjetsub_eta.clear();
    m_rcjetsub_phi.clear();
    m_rcjetsub_e.clear();
    m_rcjetsub_mv2c10.clear();

    m_rcjet_pt.resize(sizeOfRCjets,-999.);
    m_rcjet_eta.resize(sizeOfRCjets,-999.);
    m_rcjet_phi.resize(sizeOfRCjets,-999.);
    m_rcjet_e.resize(sizeOfRCjets,-999.);
    m_rcjet_d12.resize(sizeOfRCjets,-999.);
    m_rcjet_d23.resize(sizeOfRCjets,-999.);
    m_rcjetsub_pt.resize(sizeOfRCjets, std::vector<float>());
    m_rcjetsub_eta.resize(sizeOfRCjets, std::vector<float>());
    m_rcjetsub_phi.resize(sizeOfRCjets, std::vector<float>());
    m_rcjetsub_e.resize(sizeOfRCjets, std::vector<float>());
    m_rcjetsub_mv2c10.resize(sizeOfRCjets, std::vector<float>());

	  unsigned int i = 0;
	  for (xAOD::JetContainer::const_iterator jet_itr = rc_jets->begin(); jet_itr != rc_jets->end(); ++jet_itr) {
	    const xAOD::Jet* rc_jet = *jet_itr;

            if (!m_rc->passSelection(*rc_jet))
	      continue;

            m_rcjet_pt[i]   = rc_jet->pt();
            m_rcjet_eta[i]  = rc_jet->eta();
            m_rcjet_phi[i]  = rc_jet->phi();
            m_rcjet_e[i]    = rc_jet->e();

            m_rcjet_d12[i] = (RCSplit12.isAvailable(*rc_jet)) ? RCSplit12(*rc_jet) : -999.;
            m_rcjet_d23[i] = (RCSplit23.isAvailable(*rc_jet)) ? RCSplit23(*rc_jet) : -999.;

            // loop over subjets
            m_rcjetsub_pt[i].clear();     // clear the vector size (otherwise it grows out of control!)
            m_rcjetsub_eta[i].clear();
            m_rcjetsub_phi[i].clear();
            m_rcjetsub_e[i].clear();
            m_rcjetsub_mv2c10[i].clear();

            const xAOD::Jet* subjet(nullptr);
            const xAOD::BTagging* btag(nullptr);
            for(auto rc_jet_subjet : rc_jet->getConstituents()){
	      subjet = static_cast<const xAOD::Jet*>(rc_jet_subjet->rawConstituent());
	      btag   = subjet->btagging();

	      double mvx10(-999.);  // b-tagging mv2c10

	      if (btag){
		btag->MVx_discriminant("MV2c10",mvx10);
	      }
	      else{
		mvx10 = -999.;
	      }

	      m_rcjetsub_pt[i].push_back(subjet->pt());
	      m_rcjetsub_eta[i].push_back(subjet->eta());
	      m_rcjetsub_phi[i].push_back(subjet->phi());
	      m_rcjetsub_e[i].push_back(subjet->e());
	      m_rcjetsub_mv2c10[i].push_back(mvx10);
            } // end for-loop over subjets
            ++i;
	  } // end for-loop over re-clustered jets

	  m_rcjet_pt.resize(i);
	  m_rcjet_eta.resize(i);
	  m_rcjet_phi.resize(i);
	  m_rcjet_e.resize(i);
	  m_rcjet_d12.resize(i);
	  m_rcjet_d23.resize(i);
	  m_rcjetsub_pt.resize(i, std::vector<float>());
	  m_rcjetsub_eta.resize(i, std::vector<float>());
	  m_rcjetsub_phi.resize(i, std::vector<float>());
	  m_rcjetsub_e.resize(i, std::vector<float>());
	  m_rcjetsub_mv2c10.resize(i, std::vector<float>());
	} // end if make rcjets
	// end re-clustered jets

	/**********************************/
	// VarRC jets
	if (m_makeVarRCJets){
	  // Execute the re-clustering code
          // - make jet container, put it in TStore, do re-clustering
          std::string VarRC = "vrcjet";
	  for (auto& rho : m_VarRCJetRho){
            for (auto& mass_scale : m_VarRCJetMassScale){
	      std::replace( rho.begin(), rho.end(), '.', '_');
	      std::string name = rho+mass_scale;

	      top::check(m_VarRC[name]->execute(event),"Failed to execute RCJetMC15 container");

	      // Get the name of the container of re-clustered jets in TStore
              m_RCJetContainer = m_VarRC[name]->rcjetContainerName(event.m_hashValue,event.m_isLoose);

	      // -- Retrieve the re-clustered jets from TStore & save good re-clustered jets -- //
              const xAOD::JetContainer* vrc_jets(nullptr);
	      top::check(evtStore()->retrieve(vrc_jets,m_RCJetContainer),"Failed to retrieve RC JetContainer");

	      // re-clustered jet substructure
              static SG::AuxElement::ConstAccessor<float> VarRCSplit12("Split12");
	      static SG::AuxElement::ConstAccessor<float> VarRCSplit23("Split23");

	      // Initialize the vectors to be saved as branches
              unsigned int sizeOfRCjets(vrc_jets->size());
	      m_VarRCjetBranches[VarRC+"_"+name+"_pt"].resize(sizeOfRCjets,-999.);
	      m_VarRCjetBranches[VarRC+"_"+name+"_eta"].resize(sizeOfRCjets,-999.);
	      m_VarRCjetBranches[VarRC+"_"+name+"_phi"].resize(sizeOfRCjets,-999.);
	      m_VarRCjetBranches[VarRC+"_"+name+"_e"].resize(sizeOfRCjets,-999.);
	      m_VarRCjetBranches[VarRC+"_"+name+"_d12"].resize(sizeOfRCjets,-999.);
	      m_VarRCjetBranches[VarRC+"_"+name+"_d23"].resize(sizeOfRCjets,-999.);
	      m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_pt"].resize(sizeOfRCjets, std::vector<float>());
	      m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_eta"].resize(sizeOfRCjets, std::vector<float>());
	      m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_phi"].resize(sizeOfRCjets, std::vector<float>());
	      m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_e"].resize(sizeOfRCjets, std::vector<float>());
	      m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_mv2c10"].resize(sizeOfRCjets, std::vector<float>());

	      unsigned int i = 0;
	      for (xAOD::JetContainer::const_iterator jet_itr = vrc_jets->begin(); jet_itr != vrc_jets->end(); ++jet_itr) {
		const xAOD::Jet* rc_jet = *jet_itr;

		if (!m_VarRC[name]->passSelection(*rc_jet))
		  continue;

		m_VarRCjetBranches[VarRC+"_"+name+"_pt"][i]   = rc_jet->pt();
		m_VarRCjetBranches[VarRC+"_"+name+"_eta"][i]  = rc_jet->eta();
		m_VarRCjetBranches[VarRC+"_"+name+"_phi"][i]  = rc_jet->phi();
		m_VarRCjetBranches[VarRC+"_"+name+"_e"][i]    = rc_jet->e();

		m_VarRCjetBranches[VarRC+"_"+name+"_d12"][i] = (VarRCSplit12.isAvailable(*rc_jet)) ? VarRCSplit12(*rc_jet) : -999.;
		m_VarRCjetBranches[VarRC+"_"+name+"_d23"][i] = (VarRCSplit23.isAvailable(*rc_jet)) ? VarRCSplit23(*rc_jet) : -999.;

		// loop over subjets
                const xAOD::Jet* subjet(nullptr);
		const xAOD::BTagging* btag(nullptr);
		m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_pt"][i].clear();     // clear the vector size (otherwise it grows out of control!)
		m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_eta"][i].clear();
		m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_phi"][i].clear();
		m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_e"][i].clear();
		m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_mv2c10"][i].clear();
		for(auto rc_jet_subjet : rc_jet->getConstituents()){
		  subjet = static_cast<const xAOD::Jet*>(rc_jet_subjet->rawConstituent());
		  btag   = subjet->btagging();

                  double mvx10(-999.);  // b-tagging mv2c10

		  if (btag){
                    btag->MVx_discriminant("MV2c10",mvx10);
		  }
		  else{
		    mvx10 = -999.;
		  }

		  m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_pt"][i].push_back(subjet->pt());
		  m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_eta"][i].push_back(subjet->eta());
		  m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_phi"][i].push_back(subjet->phi());
		  m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_e"][i].push_back(subjet->e());
		  m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_mv2c10"][i].push_back(mvx10);
		} // end for-loop over subjets
		++i;

	      } // end for-loop over re-clustered jets

	      m_VarRCjetBranches[VarRC+"_"+name+"_pt"].resize(i);
	      m_VarRCjetBranches[VarRC+"_"+name+"_eta"].resize(i);
	      m_VarRCjetBranches[VarRC+"_"+name+"_phi"].resize(i);
	      m_VarRCjetBranches[VarRC+"_"+name+"_e"].resize(i);
	      m_VarRCjetBranches[VarRC+"_"+name+"_d12"].resize(i);
	      m_VarRCjetBranches[VarRC+"_"+name+"_d23"].resize(i);
	      m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_pt"].resize(i, std::vector<float>());
	      m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_eta"].resize(i, std::vector<float>());
	      m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_phi"].resize(i, std::vector<float>());
	      m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_e"].resize(i, std::vector<float>());
	      m_VarRCjetsubBranches[VarRC+"_"+name+"_sub_mv2c10"].resize(i, std::vector<float>());
	    } // end loop over mass parameters
	  } // end loop over multipliers for mass scale
	} // end if make VarRC jets
	// end VarRC jets


        //met
        m_met_met = event.m_met->met();
        m_met_phi = event.m_met->phi();

        //KLFitter
        if (m_config->doKLFitter()) {
            unsigned int nPermutations(0),iPerm(0),bestPerm(0);
            bool validKLFitter(false);
            m_klfitter_selected = 0;
            if (event.m_KLFitterResults != nullptr) {
                validKLFitter = true;
                m_klfitter_selected = 1;
                nPermutations = event.m_KLFitterResults->size();
            }

            m_klfitter_minuitDidNotConverge.resize(nPermutations);
            m_klfitter_fitAbortedDueToNaN.resize(nPermutations);
            m_klfitter_atLeastOneFitParameterAtItsLimit.resize(nPermutations);
            m_klfitter_invalidTransferFunctionAtConvergence.resize(nPermutations);

            /// Global result
            m_klfitter_bestPermutation.resize(nPermutations);
            m_klfitter_logLikelihood.resize(nPermutations);
            m_klfitter_eventProbability.resize(nPermutations);
            m_klfitter_parameters.resize(nPermutations);
            m_klfitter_parameterErrors.resize(nPermutations);

            if (m_config->KLFitterLH() == "ttbar" || m_config->KLFitterLH() == "ttZTrilepton" || m_config->KLFitterLH() == "ttH" || m_config->KLFitterLH() == "ttbar_JetAngles" || m_config->KLFitterLH() == "ttbar_BoostedLJets"){
              /// Model
              m_klfitter_model_bhad_pt.resize(nPermutations);
              m_klfitter_model_bhad_eta.resize(nPermutations);
              m_klfitter_model_bhad_phi.resize(nPermutations);
              m_klfitter_model_bhad_E.resize(nPermutations);
              m_klfitter_model_bhad_jetIndex.resize(nPermutations);

              m_klfitter_model_blep_pt.resize(nPermutations);
              m_klfitter_model_blep_eta.resize(nPermutations);
              m_klfitter_model_blep_phi.resize(nPermutations);
              m_klfitter_model_blep_E.resize(nPermutations);
              m_klfitter_model_blep_jetIndex.resize(nPermutations);

              m_klfitter_model_lq1_pt.resize(nPermutations);
              m_klfitter_model_lq1_eta.resize(nPermutations);
              m_klfitter_model_lq1_phi.resize(nPermutations);
              m_klfitter_model_lq1_E.resize(nPermutations);
              m_klfitter_model_lq1_jetIndex.resize(nPermutations);

              if(m_config -> KLFitterLH() != "ttbar_BoostedLJets"){
                m_klfitter_model_lq2_pt.resize(nPermutations);
                m_klfitter_model_lq2_eta.resize(nPermutations);
                m_klfitter_model_lq2_phi.resize(nPermutations);
                m_klfitter_model_lq2_E.resize(nPermutations);
                m_klfitter_model_lq2_jetIndex.resize(nPermutations);

	        if(m_config -> KLFitterLH() == "ttH"){

	          m_klfitter_model_Higgs_b1_pt.resize(nPermutations);
	          m_klfitter_model_Higgs_b1_eta.resize(nPermutations);
	          m_klfitter_model_Higgs_b1_phi.resize(nPermutations);
	          m_klfitter_model_Higgs_b1_E.resize(nPermutations);
	          m_klfitter_model_Higgs_b1_jetIndex.resize(nPermutations);

	          m_klfitter_model_Higgs_b2_pt.resize(nPermutations);
	          m_klfitter_model_Higgs_b2_eta.resize(nPermutations);
	          m_klfitter_model_Higgs_b2_phi.resize(nPermutations);
	          m_klfitter_model_Higgs_b2_E.resize(nPermutations);
	          m_klfitter_model_Higgs_b2_jetIndex.resize(nPermutations);

	        }
              }

              m_klfitter_model_lep_pt.resize(nPermutations);
              m_klfitter_model_lep_eta.resize(nPermutations);
              m_klfitter_model_lep_phi.resize(nPermutations);
              m_klfitter_model_lep_E.resize(nPermutations);
              m_klfitter_model_lep_index.resize(nPermutations);

              m_klfitter_model_lepZ1_pt.resize(nPermutations);
              m_klfitter_model_lepZ1_eta.resize(nPermutations);
              m_klfitter_model_lepZ1_phi.resize(nPermutations);
              m_klfitter_model_lepZ1_E.resize(nPermutations);
              m_klfitter_model_lepZ1_index.resize(nPermutations);

              m_klfitter_model_lepZ2_pt.resize(nPermutations);
              m_klfitter_model_lepZ2_eta.resize(nPermutations);
              m_klfitter_model_lepZ2_phi.resize(nPermutations);
              m_klfitter_model_lepZ2_E.resize(nPermutations);
              m_klfitter_model_lepZ2_index.resize(nPermutations);

              m_klfitter_model_nu_pt.resize(nPermutations);
              m_klfitter_model_nu_eta.resize(nPermutations);
              m_klfitter_model_nu_phi.resize(nPermutations);
              m_klfitter_model_nu_E.resize(nPermutations);

            } else if (m_config->KLFitterLH() == "ttbar_AllHadronic"){
              m_klfitter_model_b_from_top1_pt.resize(nPermutations);
              m_klfitter_model_b_from_top1_eta.resize(nPermutations);
              m_klfitter_model_b_from_top1_phi.resize(nPermutations);
              m_klfitter_model_b_from_top1_E.resize(nPermutations);
              m_klfitter_model_b_from_top1_jetIndex.resize(nPermutations);

              m_klfitter_model_b_from_top2_pt.resize(nPermutations);
              m_klfitter_model_b_from_top2_eta.resize(nPermutations);
              m_klfitter_model_b_from_top2_phi.resize(nPermutations);
              m_klfitter_model_b_from_top2_E.resize(nPermutations);
              m_klfitter_model_b_from_top2_jetIndex.resize(nPermutations);

              m_klfitter_model_lj1_from_top1_pt.resize(nPermutations);
              m_klfitter_model_lj1_from_top1_eta.resize(nPermutations);
              m_klfitter_model_lj1_from_top1_phi.resize(nPermutations);
              m_klfitter_model_lj1_from_top1_E.resize(nPermutations);
              m_klfitter_model_lj1_from_top1_jetIndex.resize(nPermutations);

              m_klfitter_model_lj2_from_top1_pt.resize(nPermutations);
              m_klfitter_model_lj2_from_top1_eta.resize(nPermutations);
              m_klfitter_model_lj2_from_top1_phi.resize(nPermutations);
              m_klfitter_model_lj2_from_top1_E.resize(nPermutations);
              m_klfitter_model_lj2_from_top1_jetIndex.resize(nPermutations);

              m_klfitter_model_lj1_from_top2_pt.resize(nPermutations);
              m_klfitter_model_lj1_from_top2_eta.resize(nPermutations);
              m_klfitter_model_lj1_from_top2_phi.resize(nPermutations);
              m_klfitter_model_lj1_from_top2_E.resize(nPermutations);
              m_klfitter_model_lj1_from_top2_jetIndex.resize(nPermutations);

              m_klfitter_model_lj2_from_top2_pt.resize(nPermutations);
              m_klfitter_model_lj2_from_top2_eta.resize(nPermutations);
              m_klfitter_model_lj2_from_top2_phi.resize(nPermutations);
              m_klfitter_model_lj2_from_top2_E.resize(nPermutations);
              m_klfitter_model_lj2_from_top2_jetIndex.resize(nPermutations);

            }

            if (validKLFitter) {
                for (const auto* const klPtr : *event.m_KLFitterResults) {
                    m_klfitter_minuitDidNotConverge[iPerm] = klPtr->minuitDidNotConverge();
                    m_klfitter_fitAbortedDueToNaN[iPerm] = klPtr->fitAbortedDueToNaN();
                    m_klfitter_atLeastOneFitParameterAtItsLimit[iPerm] = klPtr->atLeastOneFitParameterAtItsLimit();
                    m_klfitter_invalidTransferFunctionAtConvergence[iPerm] = klPtr->invalidTransferFunctionAtConvergence();

                    /// Global result
                    m_klfitter_bestPermutation[iPerm] = klPtr->bestPermutation();
                    if (klPtr->bestPermutation() == 1){
                        bestPerm = iPerm;
                    }
                    m_klfitter_logLikelihood[iPerm] = klPtr->logLikelihood();
                    m_klfitter_eventProbability[iPerm] = klPtr->eventProbability();
                    m_klfitter_parameters[iPerm] = klPtr->parameters();
                    m_klfitter_parameterErrors[iPerm] = klPtr->parameterErrors();

                    /// Model
                    if (m_config->KLFitterLH() == "ttbar" || m_config->KLFitterLH() == "ttZTrilepton" || m_config->KLFitterLH() == "ttH" || m_config->KLFitterLH() == "ttbar_JetAngles" || m_config->KLFitterLH() == "ttbar_BoostedLJets"){
                      m_klfitter_model_bhad_pt[iPerm] = klPtr->model_bhad_pt();
                      m_klfitter_model_bhad_eta[iPerm] = klPtr->model_bhad_eta();
                      m_klfitter_model_bhad_phi[iPerm] = klPtr->model_bhad_phi();
                      m_klfitter_model_bhad_E[iPerm] = klPtr->model_bhad_E();
                      m_klfitter_model_bhad_jetIndex[iPerm] = klPtr->model_bhad_jetIndex();

                      m_klfitter_model_blep_pt[iPerm] = klPtr->model_blep_pt();
                      m_klfitter_model_blep_eta[iPerm] = klPtr->model_blep_eta();
                      m_klfitter_model_blep_phi[iPerm] = klPtr->model_blep_phi();
                      m_klfitter_model_blep_E[iPerm] = klPtr->model_blep_E();
                      m_klfitter_model_blep_jetIndex[iPerm] = klPtr->model_blep_jetIndex();

                      m_klfitter_model_lq1_pt[iPerm] = klPtr->model_lq1_pt();
                      m_klfitter_model_lq1_eta[iPerm] = klPtr->model_lq1_eta();
                      m_klfitter_model_lq1_phi[iPerm] = klPtr->model_lq1_phi();
                      m_klfitter_model_lq1_E[iPerm] = klPtr->model_lq1_E();
                      m_klfitter_model_lq1_jetIndex[iPerm] = klPtr->model_lq1_jetIndex();

                      if(m_config -> KLFitterLH() != "ttbar_BoostedLJets"){
                        m_klfitter_model_lq2_pt[iPerm] = klPtr->model_lq2_pt();
                        m_klfitter_model_lq2_eta[iPerm] = klPtr->model_lq2_eta();
                        m_klfitter_model_lq2_phi[iPerm] = klPtr->model_lq2_phi();
                        m_klfitter_model_lq2_E[iPerm] = klPtr->model_lq2_E();
                        m_klfitter_model_lq2_jetIndex[iPerm] = klPtr->model_lq2_jetIndex();

		        if(m_config -> KLFitterLH() == "ttH"){

		          m_klfitter_model_Higgs_b1_pt[iPerm] = klPtr->model_Higgs_b1_pt();
		          m_klfitter_model_Higgs_b1_eta[iPerm] = klPtr->model_Higgs_b1_eta();
		          m_klfitter_model_Higgs_b1_phi[iPerm] = klPtr->model_Higgs_b1_phi();
		          m_klfitter_model_Higgs_b1_E[iPerm] = klPtr->model_Higgs_b1_E();
		          m_klfitter_model_Higgs_b1_jetIndex[iPerm] = klPtr->model_Higgs_b1_jetIndex();

		          m_klfitter_model_Higgs_b2_pt[iPerm] = klPtr->model_Higgs_b2_pt();
		          m_klfitter_model_Higgs_b2_eta[iPerm] = klPtr->model_Higgs_b2_eta();
		          m_klfitter_model_Higgs_b2_phi[iPerm] = klPtr->model_Higgs_b2_phi();
		          m_klfitter_model_Higgs_b2_E[iPerm] = klPtr->model_Higgs_b2_E();
		          m_klfitter_model_Higgs_b2_jetIndex[iPerm] = klPtr->model_Higgs_b2_jetIndex();

		        }
                      }
                      m_klfitter_model_lep_pt[iPerm] = klPtr->model_lep_pt();
                      m_klfitter_model_lep_eta[iPerm] = klPtr->model_lep_eta();
                      m_klfitter_model_lep_phi[iPerm] = klPtr->model_lep_phi();
                      m_klfitter_model_lep_E[iPerm] = klPtr->model_lep_E();
                      m_klfitter_model_lep_index[iPerm] = klPtr->model_lep_index();

                      m_klfitter_model_lepZ1_pt[iPerm] = klPtr->model_lepZ1_pt();
                      m_klfitter_model_lepZ1_eta[iPerm] = klPtr->model_lepZ1_eta();
                      m_klfitter_model_lepZ1_phi[iPerm] = klPtr->model_lepZ1_phi();
                      m_klfitter_model_lepZ1_E[iPerm] = klPtr->model_lepZ1_E();
                      m_klfitter_model_lepZ1_index[iPerm] = klPtr->model_lepZ1_index();

                      m_klfitter_model_lepZ2_pt[iPerm] = klPtr->model_lepZ2_pt();
                      m_klfitter_model_lepZ2_eta[iPerm] = klPtr->model_lepZ2_eta();
                      m_klfitter_model_lepZ2_phi[iPerm] = klPtr->model_lepZ2_phi();
                      m_klfitter_model_lepZ2_E[iPerm] = klPtr->model_lepZ2_E();
                      m_klfitter_model_lepZ2_index[iPerm] = klPtr->model_lepZ2_index();

                      m_klfitter_model_nu_pt[iPerm] = klPtr->model_nu_pt();
                      m_klfitter_model_nu_eta[iPerm] = klPtr->model_nu_eta();
                      m_klfitter_model_nu_phi[iPerm] = klPtr->model_nu_phi();
                      m_klfitter_model_nu_E[iPerm] = klPtr->model_nu_E();
                    
                    } else if (m_config->KLFitterLH() == "ttbar_AllHadronic"){
                      m_klfitter_model_b_from_top1_pt[iPerm] = klPtr->model_b_from_top1_pt();
                      m_klfitter_model_b_from_top1_eta[iPerm] = klPtr->model_b_from_top1_eta();
                      m_klfitter_model_b_from_top1_phi[iPerm] = klPtr->model_b_from_top1_phi();
                      m_klfitter_model_b_from_top1_E[iPerm] = klPtr->model_b_from_top1_E();
                      m_klfitter_model_b_from_top1_jetIndex[iPerm] = klPtr->model_b_from_top1_jetIndex();

                      m_klfitter_model_b_from_top2_pt[iPerm] = klPtr->model_b_from_top2_pt();
                      m_klfitter_model_b_from_top2_eta[iPerm] = klPtr->model_b_from_top2_eta();
                      m_klfitter_model_b_from_top2_phi[iPerm] = klPtr->model_b_from_top2_phi();
                      m_klfitter_model_b_from_top2_E[iPerm] = klPtr->model_b_from_top2_E();
                      m_klfitter_model_b_from_top2_jetIndex[iPerm] = klPtr->model_b_from_top2_jetIndex();

                      m_klfitter_model_lj1_from_top1_pt[iPerm] = klPtr->model_lj1_from_top1_pt();
                      m_klfitter_model_lj1_from_top1_eta[iPerm] = klPtr->model_lj1_from_top1_eta();
                      m_klfitter_model_lj1_from_top1_phi[iPerm] = klPtr->model_lj1_from_top1_phi();
                      m_klfitter_model_lj1_from_top1_E[iPerm] = klPtr->model_lj1_from_top1_E();
                      m_klfitter_model_lj1_from_top1_jetIndex[iPerm] = klPtr->model_lj1_from_top1_jetIndex();

                      m_klfitter_model_lj2_from_top1_pt[iPerm] = klPtr->model_lj2_from_top1_pt();
                      m_klfitter_model_lj2_from_top1_eta[iPerm] = klPtr->model_lj2_from_top1_eta();
                      m_klfitter_model_lj2_from_top1_phi[iPerm] = klPtr->model_lj2_from_top1_phi();
                      m_klfitter_model_lj2_from_top1_E[iPerm] = klPtr->model_lj2_from_top1_E();
                      m_klfitter_model_lj2_from_top1_jetIndex[iPerm] = klPtr->model_lj2_from_top1_jetIndex();

                      m_klfitter_model_lj1_from_top2_pt[iPerm] = klPtr->model_lj1_from_top2_pt();
                      m_klfitter_model_lj1_from_top2_eta[iPerm] = klPtr->model_lj1_from_top2_eta();
                      m_klfitter_model_lj1_from_top2_phi[iPerm] = klPtr->model_lj1_from_top2_phi();
                      m_klfitter_model_lj1_from_top2_E[iPerm] = klPtr->model_lj1_from_top2_E();
                      m_klfitter_model_lj1_from_top2_jetIndex[iPerm] = klPtr->model_lj1_from_top2_jetIndex();

                      m_klfitter_model_lj2_from_top2_pt[iPerm] = klPtr->model_lj2_from_top2_pt();
                      m_klfitter_model_lj2_from_top2_eta[iPerm] = klPtr->model_lj2_from_top2_eta();
                      m_klfitter_model_lj2_from_top2_phi[iPerm] = klPtr->model_lj2_from_top2_phi();
                      m_klfitter_model_lj2_from_top2_E[iPerm] = klPtr->model_lj2_from_top2_E();
                      m_klfitter_model_lj2_from_top2_jetIndex[iPerm] = klPtr->model_lj2_from_top2_jetIndex();

                    }


                    ++iPerm;
                }

                // now take the best permutation and build the tops and the ttbar system!
                if (m_config->KLFitterLH() == "ttbar" || m_config->KLFitterLH() == "ttZTrilepton" || m_config->KLFitterLH() == "ttH" || m_config->KLFitterLH() == "ttbar_JetAngles"){
                  if(nPermutations != 0){
                      TLorentzVector bhad,blep,lq1,lq2,lep,nu,top_had,top_lep,ttbar;

                      bhad.SetPtEtaPhiE(m_klfitter_model_bhad_pt[bestPerm], m_klfitter_model_bhad_eta[bestPerm], m_klfitter_model_bhad_phi[bestPerm], m_klfitter_model_bhad_E[bestPerm]);
                      blep.SetPtEtaPhiE(m_klfitter_model_blep_pt[bestPerm], m_klfitter_model_blep_eta[bestPerm], m_klfitter_model_blep_phi[bestPerm], m_klfitter_model_blep_E[bestPerm]);
                      lq1.SetPtEtaPhiE(m_klfitter_model_lq1_pt[bestPerm],   m_klfitter_model_lq1_eta[bestPerm],  m_klfitter_model_lq1_phi[bestPerm],  m_klfitter_model_lq1_E[bestPerm]);
                      lq2.SetPtEtaPhiE(m_klfitter_model_lq2_pt[bestPerm],   m_klfitter_model_lq2_eta[bestPerm],  m_klfitter_model_lq2_phi[bestPerm],  m_klfitter_model_lq2_E[bestPerm]);
                      lep.SetPtEtaPhiE(m_klfitter_model_lep_pt[bestPerm],   m_klfitter_model_lep_eta[bestPerm],  m_klfitter_model_lep_phi[bestPerm],  m_klfitter_model_lep_E[bestPerm]);
                      nu.SetPtEtaPhiE(m_klfitter_model_nu_pt[bestPerm],     m_klfitter_model_nu_eta[bestPerm],   m_klfitter_model_nu_phi[bestPerm],   m_klfitter_model_nu_E[bestPerm]);

                      top_had = bhad+lq1+lq2;
                      top_lep = blep+lep+nu;
                      ttbar   = top_had+top_lep;

                      m_klfitter_bestPerm_topLep_pt  = top_lep.Pt();
                      m_klfitter_bestPerm_topLep_eta = top_lep.Eta();
                      m_klfitter_bestPerm_topLep_phi = top_lep.Phi();
                      m_klfitter_bestPerm_topLep_E   = top_lep.E();
                      m_klfitter_bestPerm_topLep_m   = top_lep.M();

                      m_klfitter_bestPerm_topHad_pt  = top_had.Pt();
                      m_klfitter_bestPerm_topHad_eta = top_had.Eta();
                      m_klfitter_bestPerm_topHad_phi = top_had.Phi();
                      m_klfitter_bestPerm_topHad_E   = top_had.E();
                      m_klfitter_bestPerm_topHad_m   = top_had.M();

                      m_klfitter_bestPerm_ttbar_pt   = ttbar.Pt();
                      m_klfitter_bestPerm_ttbar_eta  = ttbar.Eta();
                      m_klfitter_bestPerm_ttbar_phi  = ttbar.Phi();
                      m_klfitter_bestPerm_ttbar_E    = ttbar.E();
                      m_klfitter_bestPerm_ttbar_m    = ttbar.M();
                  }

                }

            }

        }

	if(m_config->doPseudoTop()){

	  const xAOD::PseudoTopResultContainer* pseudoTopResultContainer(nullptr);
	  const xAOD::PseudoTopResult* pseudoTopResult(nullptr);

          if ( (!event.m_isLoose && evtStore()->contains<xAOD::PseudoTopResultContainer>(topConfig()->sgKeyPseudoTop(event.m_hashValue))) ||
               ( event.m_isLoose && evtStore()->contains<xAOD::PseudoTopResultContainer>(topConfig()->sgKeyPseudoTopLoose(event.m_hashValue))) )
          {
            if (!event.m_isLoose)
            {
                top::check(evtStore()->retrieve(pseudoTopResultContainer, topConfig()->sgKeyPseudoTop(event.m_hashValue)),"Failed to retrieve PseudoTop");
            }
            else
            {
                top::check(evtStore()->retrieve(pseudoTopResultContainer, topConfig()->sgKeyPseudoTopLoose(event.m_hashValue)),"Failed to retrieve PseudoTop");
            }

	    pseudoTopResult = pseudoTopResultContainer->at(0);

	    m_PseudoTop_Reco_ttbar_pt  = pseudoTopResult -> auxdecor<float>("PseudoTop_Reco_ttbar_pt");
	    m_PseudoTop_Reco_ttbar_eta = pseudoTopResult -> auxdecor<float>("PseudoTop_Reco_ttbar_eta");
	    m_PseudoTop_Reco_ttbar_phi = pseudoTopResult -> auxdecor<float>("PseudoTop_Reco_ttbar_phi");
	    m_PseudoTop_Reco_ttbar_m   = pseudoTopResult -> auxdecor<float>("PseudoTop_Reco_ttbar_m");

	    m_PseudoTop_Reco_top_lep_pt  = pseudoTopResult -> auxdecor<float>("PseudoTop_Reco_top_lep_pt");
            m_PseudoTop_Reco_top_lep_eta = pseudoTopResult -> auxdecor<float>("PseudoTop_Reco_top_lep_eta");
            m_PseudoTop_Reco_top_lep_phi = pseudoTopResult -> auxdecor<float>("PseudoTop_Reco_top_lep_phi");
            m_PseudoTop_Reco_top_lep_m   = pseudoTopResult -> auxdecor<float>("PseudoTop_Reco_top_lep_m");

	    m_PseudoTop_Reco_top_had_pt  = pseudoTopResult -> auxdecor<float>("PseudoTop_Reco_top_had_pt");
            m_PseudoTop_Reco_top_had_eta = pseudoTopResult -> auxdecor<float>("PseudoTop_Reco_top_had_eta");
            m_PseudoTop_Reco_top_had_phi = pseudoTopResult -> auxdecor<float>("PseudoTop_Reco_top_had_phi");
            m_PseudoTop_Reco_top_had_m   = pseudoTopResult -> auxdecor<float>("PseudoTop_Reco_top_had_m");


	  }
	}

    }

    void EventSaverFlatNtuple::fillEvent(const top::Event& event) {
        //do it!
        m_treeManagers[event.m_ttreeIndex]->fill();
    }


    void EventSaverFlatNtuple::saveTruthEvent() {
      this -> cleanTruthEvent();
      this -> calculateTruthEvent();
      this -> fillTruthEvent();
    }

    void EventSaverFlatNtuple::cleanTruthEvent() {
      /* Doing nothing for now, but we should put here the (re-)initialisation to dummy values
      * of all variables later calculated in cleanTruthEvent()
      * For example all weights set to 1, all vectors cleaned, all kinematic or angular variables set to -99999.
      */
    }

    void EventSaverFlatNtuple::calculateTruthEvent() {
        const xAOD::EventInfo* eventInfo(nullptr);
        top::check( evtStore()->retrieve(eventInfo,m_config->sgKeyEventInfo()) , "Failed to retrieve EventInfo" );

        const xAOD::TruthEventContainer * truthEvent(nullptr);
        top::check( evtStore()->retrieve(truthEvent, m_config->sgKeyTruthEvent()) , "Failed to retrieve truth event container" );
        unsigned int truthEventSize = truthEvent->size();
        top::check( truthEventSize==1 , "Failed to retrieve truth PDF info - truth event container size is different from 1 (strange)" );

//         m_weight_mc       = eventInfo -> mcEventWeight();
        m_weight_mc = truthEvent->at(0)->weights()[0];// FIXME temporary bugfix
        m_eventNumber     = eventInfo -> eventNumber();
        m_runNumber       = eventInfo -> runNumber();
        m_mcChannelNumber = eventInfo -> mcChannelNumber();
        m_mu = eventInfo->averageInteractionsPerCrossing();

        if (m_config->doPileupReweighting() && !m_config->isTruthDxAOD()) {
            m_weight_pileup = eventInfo->auxdataConst<float>("PileupWeight");
            m_randomRunNumber = eventInfo->auxdataConst<unsigned int>("RandomRunNumber");
        }
        else {
            m_weight_pileup = NAN;
            m_randomRunNumber = 0;
        }

        //MC particle
        if (m_config->useTruthParticles() && m_config->doTruthBlockInfo()) {

            const xAOD::TruthParticleContainer* truth(nullptr);
            top::check( evtStore()->retrieve(truth,m_config->sgKeyMCParticle()) , "Failed to retrieve TruthParticleContainer" );

            if (truth != nullptr) {
                unsigned int i(0);
                unsigned int truthSize = truth->size();
                m_mc_pt.resize(truthSize);
                m_mc_eta.resize(truthSize);
                m_mc_phi.resize(truthSize);
                m_mc_e.resize(truthSize);
                m_mc_pdgId.resize(truthSize);
                for (const auto* const mcPtr : *truth) {

                    // Fix for
                    // Warning in <TVector3::PseudoRapidity>: transvers momentum = 0! return +/- 10e10

                    float eta(-999.9);

                    if ( !( mcPtr->p4().CosTheta()*mcPtr->p4().CosTheta() >= 1 && mcPtr->p4().Pz() != 0 ) ) {
                        eta = mcPtr->eta();
                    }

                    m_mc_pt[i] = mcPtr->pt();
                    m_mc_eta[i] = eta;
                    m_mc_phi[i] = mcPtr->phi();
                    m_mc_e[i] = mcPtr->e();
                    m_mc_pdgId[i] = mcPtr->pdgId();
                    ++i;
                }
            }
        }

        if (m_config->doTruthPDFInfo()) {
            // delegate to helper function.
            loadPdfInfo();
        }

        if (m_config->doMCGeneratorWeights()) {
            // delegate to helper function.
            loadMCGeneratorWeights();
        }

        if (m_config->doTopPartonHistory()) {

            const xAOD::PartonHistoryContainer* partonHistoryContainer(nullptr);
            const xAOD::PartonHistory* partonHistory(nullptr);
            if (evtStore()->contains<xAOD::PartonHistoryContainer>(m_config->sgKeyTopPartonHistory())) {
                top::check(evtStore()->retrieve(partonHistoryContainer, m_config->sgKeyTopPartonHistory()),"Failed to retrieve Top Parton History");
                if (partonHistoryContainer->size() == 1) {
                    partonHistory = partonHistoryContainer->at(0);
                }
            }

            if (partonHistory != nullptr) {
                if (!m_config->isTopPartonHistoryRegisteredInNtuple()) {
                    registerObjectIntoTruthTree( *(partonHistory) );
                    m_config->setTopPartonHistoryRegisteredInNtuple();
                }
                saveObjectIntoTruthTree( *(partonHistory) );
            }

        }

        if( m_config->saveLHAPDFEvent() ){
            // Delegate to helper function.
            loadPdfWeights();
        }

    }

    void EventSaverFlatNtuple::fillTruthEvent() {
        m_truthTreeManager->fill();
    }

    void EventSaverFlatNtuple::saveParticleLevelEvent(const top::ParticleLevelEvent& plEvent){
        // Quick return if particle level is disabled.
        if ( not m_config->doTopParticleLevel() ){
            return;
        }
        // No need to attempt to write out anything for non-MC data.
        if ( ! m_config->isMC() ){
            return;
        }
        this -> cleanParticleLevelEvent();
        this -> calculateParticleLevelEvent(plEvent);
        this -> fillParticleLevelEvent();
    }

    void EventSaverFlatNtuple::cleanParticleLevelEvent() {
      /* Doing nothing for now, but we should put here the (re-)initialisation to dummy values
      * of all variables later calculated in calculateParticleLevelEvent(const top::ParticleLevelEvent& plEvent)
      * For example all weights set to 1, all vectors cleaned, all kinematic or angular variables set to -99999.
      */
    }

    void EventSaverFlatNtuple::calculateParticleLevelEvent(const top::ParticleLevelEvent& plEvent) {
        for ( auto & selectionDecision : m_particleLevel_SelectionDecisions ){
            selectionDecision.second = plEvent.m_selectionDecisions[ selectionDecision.first ];
        }

        // to get the fixed mc weight
        const xAOD::TruthEventContainer * truthEvent(nullptr);
        top::check( evtStore()->retrieve(truthEvent, m_config->sgKeyTruthEvent()) , "Failed to retrieve truth event container" );
        unsigned int truthEventSize = truthEvent->size();
        top::check( truthEventSize==1 , "Failed to retrieve truth PDF info - truth event container size is different from 1 (strange)" );

//         m_weight_mc = plEvent.m_info->mcEventWeight();
        m_weight_mc = truthEvent->at(0)->weights()[0];// FIXME temporary bugfix

        m_eventNumber = plEvent.m_info->eventNumber();
        m_runNumber = plEvent.m_info->runNumber();
        m_mcChannelNumber = plEvent.m_info->mcChannelNumber();

        m_mu = plEvent.m_info->averageInteractionsPerCrossing();

        if (m_config->doPileupReweighting() && !m_config->isTruthDxAOD()) {
            m_weight_pileup = plEvent.m_info->auxdataConst<float>("PileupWeight");
            m_randomRunNumber = plEvent.m_info->auxdataConst<unsigned int>("RandomRunNumber");
        }
        else {
            m_weight_pileup = NAN;
            m_randomRunNumber = 0;
        }

        //electrons
        if ( m_config->useTruthElectrons() ){
            unsigned int i = 0;

            m_el_pt.resize(plEvent.m_electrons->size());
            m_el_eta.resize(plEvent.m_electrons->size());
            m_el_phi.resize(plEvent.m_electrons->size());
            m_el_e.resize(plEvent.m_electrons->size());
            m_el_charge.resize(plEvent.m_electrons->size());

            m_el_pt_bare.resize(plEvent.m_electrons->size());
            m_el_eta_bare.resize(plEvent.m_electrons->size());
            m_el_phi_bare.resize(plEvent.m_electrons->size());
            m_el_e_bare.resize(plEvent.m_electrons->size());

            for (const auto & elPtr : * plEvent.m_electrons) {
                m_el_pt[i] = elPtr->pt();
                m_el_eta[i] = elPtr->eta();
                m_el_phi[i] = elPtr->phi();
                m_el_e[i] = elPtr->e();
                m_el_charge[i] = elPtr->charge();

                m_el_pt_bare[i] = elPtr->auxdata<float>( "pt_bare" );
                m_el_eta_bare[i] = elPtr->auxdata<float>( "eta_bare" );
                m_el_phi_bare[i] = elPtr->auxdata<float>( "phi_bare" );
                m_el_e_bare[i] = elPtr->auxdata<float>( "e_bare" );

                ++i;
            }
        }

        //muons
        if ( m_config->useTruthMuons() ){
            unsigned int i = 0;

            m_mu_pt.resize(plEvent.m_muons->size());
            m_mu_eta.resize(plEvent.m_muons->size());
            m_mu_phi.resize(plEvent.m_muons->size());
            m_mu_e.resize(plEvent.m_muons->size());
            m_mu_charge.resize(plEvent.m_muons->size());

            m_mu_pt_bare.resize(plEvent.m_muons->size());
            m_mu_eta_bare.resize(plEvent.m_muons->size());
            m_mu_phi_bare.resize(plEvent.m_muons->size());
            m_mu_e_bare.resize(plEvent.m_muons->size());

            for (const auto & muPtr : * plEvent.m_muons) {
                m_mu_pt[i] = muPtr->pt();
                m_mu_eta[i] = muPtr->eta();
                m_mu_phi[i] = muPtr->phi();
                m_mu_e[i] = muPtr->e();
                m_mu_charge[i] = muPtr->charge();

                m_mu_pt_bare[i] = muPtr->auxdata<float>( "pt_bare" );
                m_mu_eta_bare[i] = muPtr->auxdata<float>( "eta_bare" );
                m_mu_phi_bare[i] = muPtr->auxdata<float>( "phi_bare" );
                m_mu_e_bare[i] = muPtr->auxdata<float>( "e_bare" );

                ++i;
            }
        }

        //photons
        if (m_config->useTruthPhotons()) {
            unsigned int i(0);
            m_ph_pt.resize(plEvent.m_photons->size());
            m_ph_eta.resize(plEvent.m_photons->size());
            m_ph_phi.resize(plEvent.m_photons->size());
            m_ph_e.resize(plEvent.m_photons->size());
            for (const auto* const phPtr : * plEvent.m_photons) {
                m_ph_pt[i] = phPtr->pt();
                m_ph_eta[i] = phPtr->eta();
                m_ph_phi[i] = phPtr->phi();
                m_ph_e[i] = phPtr->e();

                ++i;
            }
        }

        //jets
        if ( m_config->useTruthJets() ){
            unsigned int i = 0;

            m_jet_pt.resize(plEvent.m_jets->size());
            m_jet_eta.resize(plEvent.m_jets->size());
            m_jet_phi.resize(plEvent.m_jets->size());
            m_jet_e.resize(plEvent.m_jets->size());
            m_jet_Ghosts_BHadron_Final_Count.resize(plEvent.m_jets->size());
            m_jet_Ghosts_CHadron_Final_Count.resize(plEvent.m_jets->size());
            for (const auto & jetPtr : * plEvent.m_jets) {
                m_jet_pt[i] = jetPtr->pt();
                m_jet_eta[i] = jetPtr->eta();
                m_jet_phi[i] = jetPtr->phi();
                m_jet_e[i] = jetPtr->e();

                m_jet_Ghosts_BHadron_Final_Count[i] = jetPtr->auxdata<int>( "GhostBHadronsFinalCount" );
                m_jet_Ghosts_CHadron_Final_Count[i] = jetPtr->auxdata<int>( "GhostCHadronsFinalCount" );

                ++i;
            }
        }

        //large R jets
        if ( m_config->useTruthLargeRJets() ){
            unsigned int i = 0;

            m_ljet_pt.resize(plEvent.m_largeRJets->size());
            m_ljet_eta.resize(plEvent.m_largeRJets->size());
            m_ljet_phi.resize(plEvent.m_largeRJets->size());
            m_ljet_e.resize(plEvent.m_largeRJets->size());
            m_ljet_Ghosts_BHadron_Final_Count.resize(plEvent.m_largeRJets->size());
            m_ljet_Ghosts_CHadron_Final_Count.resize(plEvent.m_largeRJets->size());
            for (const auto & jetPtr : * plEvent.m_largeRJets) {
                m_ljet_pt[i] = jetPtr->pt();
                m_ljet_eta[i] = jetPtr->eta();
                m_ljet_phi[i] = jetPtr->phi();
                m_ljet_e[i] = jetPtr->e();

                m_ljet_Ghosts_BHadron_Final_Count[i] = jetPtr->auxdata<int>( "GhostBHadronsFinalCount" );
                m_ljet_Ghosts_CHadron_Final_Count[i] = jetPtr->auxdata<int>( "GhostCHadronsFinalCount" );

                ++i;
            }
        }

        if(m_makeRCJets){
            // Execute the re-clustering code
            // - make jet container of small-r jets in the event, put it in TStore, do re-clustering
            top::check(m_rcjet_particle->execute(plEvent),"Failed to execute ParticleLevelRCJetObjectLoader container");

            // Get the name of the container of re-clustered jets
            m_RCJetContainerParticle = m_rcjet_particle->rcjetContainerName();

            // -- Retrieve the re-clustered jets from TStore & save good re-clustered jets -- //
            const xAOD::JetContainer* rc_jets_particle(nullptr);
            top::check(evtStore()->retrieve(rc_jets_particle,m_RCJetContainerParticle),"Failed to retrieve particle RC JetContainer");

            // re-clustered jet substructure
            static SG::AuxElement::ConstAccessor<float> RCSplit12("Split12");
            static SG::AuxElement::ConstAccessor<float> RCSplit23("Split23");

            // Initialize the vectors to be saved as branches
            unsigned int sizeOfRCjets(rc_jets_particle->size());

            m_rcjet_pt.clear();
            m_rcjet_eta.clear();
            m_rcjet_phi.clear();
            m_rcjet_e.clear();
            m_rcjet_d12.clear();
            m_rcjet_d23.clear();
            m_rcjetsub_pt.clear();
            m_rcjetsub_eta.clear();
            m_rcjetsub_phi.clear();
            m_rcjetsub_e.clear();
            m_rcjetsub_Ghosts_BHadron_Final_Count.clear();
            m_rcjetsub_Ghosts_CHadron_Final_Count.clear();

            m_rcjet_pt.resize(sizeOfRCjets,-999.);
            m_rcjet_eta.resize(sizeOfRCjets,-999.);
            m_rcjet_phi.resize(sizeOfRCjets,-999.);
            m_rcjet_e.resize(sizeOfRCjets,-999.);
            m_rcjet_d12.resize(sizeOfRCjets,-999.);
            m_rcjet_d23.resize(sizeOfRCjets,-999.);
            m_rcjetsub_pt.resize(sizeOfRCjets, std::vector<float>());
            m_rcjetsub_eta.resize(sizeOfRCjets, std::vector<float>());
            m_rcjetsub_phi.resize(sizeOfRCjets, std::vector<float>());
            m_rcjetsub_e.resize(sizeOfRCjets, std::vector<float>());
            m_rcjetsub_Ghosts_BHadron_Final_Count.resize(sizeOfRCjets, std::vector<int>());
            m_rcjetsub_Ghosts_CHadron_Final_Count.resize(sizeOfRCjets, std::vector<int>());

            unsigned int i = 0;
            std::vector<int> rc_particle_selected_jets;
            rc_particle_selected_jets.clear();

            for (xAOD::JetContainer::const_iterator jet_itr = rc_jets_particle->begin(); jet_itr != rc_jets_particle->end(); ++jet_itr) {
                const xAOD::Jet* rc_jet = *jet_itr;
                if (!m_rcjet_particle->passSelection(*rc_jet))
                    continue;

                rc_particle_selected_jets.push_back(rc_jet->index());

                m_rcjet_pt[i]   = rc_jet->pt();
                m_rcjet_eta[i]  = rc_jet->eta();
                m_rcjet_phi[i]  = rc_jet->phi();
                m_rcjet_e[i]    = rc_jet->e();

                m_rcjet_d12[i] = (RCSplit12.isAvailable(*rc_jet)) ? RCSplit12(*rc_jet) : -999.;
                m_rcjet_d23[i] = (RCSplit23.isAvailable(*rc_jet)) ? RCSplit23(*rc_jet) : -999.;

                // loop over subjets
                m_rcjetsub_pt[i].clear();     // clear the vector size (otherwise it grows out of control!)
                m_rcjetsub_eta[i].clear();
                m_rcjetsub_phi[i].clear();
                m_rcjetsub_e[i].clear();

                const xAOD::Jet* subjet(nullptr);
                for(auto rc_jet_subjet : rc_jet->getConstituents()){
                    subjet = static_cast<const xAOD::Jet*>(rc_jet_subjet->rawConstituent());
                    m_rcjetsub_pt[i].push_back(subjet->pt());
                    m_rcjetsub_eta[i].push_back(subjet->eta());
                    m_rcjetsub_phi[i].push_back(subjet->phi());
                    m_rcjetsub_e[i].push_back(subjet->e());
                    m_rcjetsub_Ghosts_BHadron_Final_Count[i].push_back(subjet->auxdata<int>( "GhostBHadronsFinalCount" ));
                    m_rcjetsub_Ghosts_CHadron_Final_Count[i].push_back(subjet->auxdata<int>( "GhostCHadronsFinalCount" ));
                } // end for-loop over subjets
                ++i;
            } // end for-loop over re-clustered jets
	    // we resized earlier to the size of rc_jets_particle container, but then only stored a sub-set
	    // so have to resize again to shrink to correct size incase some jets were not stored
            m_rcjet_pt.resize(i,-999);
            m_rcjet_eta.resize(i,-999.);
            m_rcjet_phi.resize(i,-999.);
            m_rcjet_e.resize(i,-999.);
            m_rcjet_d12.resize(i,-999.);
            m_rcjet_d23.resize(i,-999.);
            m_rcjetsub_pt.resize(i, std::vector<float>());
            m_rcjetsub_eta.resize(i, std::vector<float>());
            m_rcjetsub_phi.resize(i, std::vector<float>());
            m_rcjetsub_e.resize(i, std::vector<float>());
            m_rcjetsub_Ghosts_BHadron_Final_Count.resize(i, std::vector<int>());
            m_rcjetsub_Ghosts_CHadron_Final_Count.resize(i, std::vector<int>());

        }

        //met
        if ( m_config->useTruthMET() ){
            m_met_met = plEvent.m_met->met();
            m_met_phi = plEvent.m_met->phi();
        }

        // Let us write the PDF Info into the branch variable(s).
        if ( m_config->doTruthPDFInfo() ) {
            // Delegate to helper function.
            loadPdfInfo();
        }

        if (m_config->doMCGeneratorWeights()) {
            // delegate to helper function.
            loadMCGeneratorWeights();
        }

        // Let us write the PDF weights into the branch variable(s).
        if( m_config->saveLHAPDFEvent() ){
            // Delegate to helper function.
            loadPdfWeights();
        }

	if(m_config->doPseudoTop()){

	  const xAOD::PseudoTopResultContainer* pseudoTopResultContainer(nullptr);
          const xAOD::PseudoTopResult* pseudoTopResult(nullptr);

          if (evtStore()->contains<xAOD::PseudoTopResultContainer>(m_config->sgKeyPseudoTop(0))) {
	    top::check(evtStore()->retrieve(pseudoTopResultContainer, m_config->sgKeyPseudoTop(0)), "Failed to retrieve PseudoTop");

            pseudoTopResult = pseudoTopResultContainer->at(0);

            m_PseudoTop_Particle_ttbar_pt  = pseudoTopResult -> auxdecor<float>("PseudoTop_Particle_ttbar_pt");
            m_PseudoTop_Particle_ttbar_eta = pseudoTopResult -> auxdecor<float>("PseudoTop_Particle_ttbar_eta");
            m_PseudoTop_Particle_ttbar_phi = pseudoTopResult -> auxdecor<float>("PseudoTop_Particle_ttbar_phi");
            m_PseudoTop_Particle_ttbar_m   = pseudoTopResult -> auxdecor<float>("PseudoTop_Particle_ttbar_m");

            m_PseudoTop_Particle_top_lep_pt  = pseudoTopResult -> auxdecor<float>("PseudoTop_Particle_top_lep_pt");
            m_PseudoTop_Particle_top_lep_eta = pseudoTopResult -> auxdecor<float>("PseudoTop_Particle_top_lep_eta");
            m_PseudoTop_Particle_top_lep_phi = pseudoTopResult -> auxdecor<float>("PseudoTop_Particle_top_lep_phi");
            m_PseudoTop_Particle_top_lep_m   = pseudoTopResult -> auxdecor<float>("PseudoTop_Particle_top_lep_m");

            m_PseudoTop_Particle_top_had_pt  = pseudoTopResult -> auxdecor<float>("PseudoTop_Particle_top_had_pt");
            m_PseudoTop_Particle_top_had_eta = pseudoTopResult -> auxdecor<float>("PseudoTop_Particle_top_had_eta");
            m_PseudoTop_Particle_top_had_phi = pseudoTopResult -> auxdecor<float>("PseudoTop_Particle_top_had_phi");
            m_PseudoTop_Particle_top_had_m   = pseudoTopResult -> auxdecor<float>("PseudoTop_Particle_top_had_m");


          }
	}

    }

    void EventSaverFlatNtuple::fillParticleLevelEvent() {
        //do it!
        m_particleLevelTreeManager->fill();
    }

    void EventSaverFlatNtuple::saveUpgradeEvent(const top::ParticleLevelEvent& upgradeEvent){
      // Quick return if upgrade is disabled.
      if ( not m_config->HLLHC() ){
        return;
      }
      // No need to attempt to write out anything for non-MC data.
      if ( ! m_config->isMC() ){
        return;
      }
      this -> cleanUpgradeEvent();
      this -> calculateUpgradeEvent(upgradeEvent);
      this -> fillUpgradeEvent();
    }

    void EventSaverFlatNtuple::cleanUpgradeEvent() {
      /* Doing nothing for now, but we should put here the (re-)initialisation to dummy values
      * of all variables later calculated in calculateParticleLevelEvent(const top::ParticleLevelEvent& plEvent)
      * For example all weights set to 1, all vectors cleaned, all kinematic or angular variables set to -99999.
      */
    }

    void EventSaverFlatNtuple::calculateUpgradeEvent(const top::ParticleLevelEvent& upgradeEvent) {

        // to get the fixed mc weight
        const xAOD::TruthEventContainer * truthEvent(nullptr);
        top::check( evtStore()->retrieve(truthEvent, m_config->sgKeyTruthEvent()) , "Failed to retrieve truth event container" );
        unsigned int truthEventSize = truthEvent->size();
        top::check( truthEventSize==1 , "Failed to retrieve truth PDF info - truth event container size is different from 1 (strange)" );

//         m_weight_mc = plEvent.m_info->mcEventWeight();
        m_weight_mc = truthEvent->at(0)->weights()[0];// FIXME temporary bugfix

        m_eventNumber = upgradeEvent.m_info->eventNumber();
        m_runNumber = upgradeEvent.m_info->runNumber();
        m_mcChannelNumber = upgradeEvent.m_info->mcChannelNumber();

        m_mu = upgradeEvent.m_info->averageInteractionsPerCrossing();

       // save electrons
       unsigned int i = 0;
       m_el_pt.resize(upgradeEvent.m_electrons->size());
       m_el_eta.resize(upgradeEvent.m_electrons->size());
       m_el_phi.resize(upgradeEvent.m_electrons->size());
       m_el_e.resize(upgradeEvent.m_electrons->size());
       m_el_charge.resize(upgradeEvent.m_electrons->size());

       for (const auto  elPtr : * upgradeEvent.m_electrons) {
         m_el_pt[i] = elPtr->pt();
         m_el_eta[i] = elPtr->eta();
         m_el_phi[i] = elPtr->phi();
         m_el_e[i] = elPtr->e();
         m_el_charge[i] = elPtr->charge();
         ++i;
       }

       // save muons
       i = 0;
       m_mu_pt.resize(upgradeEvent.m_muons->size());
       m_mu_eta.resize(upgradeEvent.m_muons->size());
       m_mu_phi.resize(upgradeEvent.m_muons->size());
       m_mu_e.resize(upgradeEvent.m_muons->size());
       m_mu_charge.resize(upgradeEvent.m_muons->size());

       for (const auto  muPtr : * upgradeEvent.m_muons) {
         m_mu_pt[i] = muPtr->pt();
         m_mu_eta[i] = muPtr->eta();
         m_mu_phi[i] = muPtr->phi();
         m_mu_e[i] = muPtr->e();
         m_mu_charge[i] = muPtr->charge();
         ++i;
       }

       // save jets
       i = 0;
       m_jet_pt.resize(upgradeEvent.m_jets->size());
       m_jet_eta.resize(upgradeEvent.m_jets->size());
       m_jet_phi.resize(upgradeEvent.m_jets->size());
       m_jet_e.resize(upgradeEvent.m_jets->size());
       m_jet_mv1eff.resize(upgradeEvent.m_jets->size());
       m_jet_isPileup.resize(upgradeEvent.m_jets->size());
       m_jet_Ghosts_BHadron_Final_Count.resize(upgradeEvent.m_jets->size());
       m_jet_Ghosts_CHadron_Final_Count.resize(upgradeEvent.m_jets->size());
       for (const auto & jetPtr : * upgradeEvent.m_jets) {
         m_jet_pt[i] = jetPtr->pt();
         m_jet_eta[i] = jetPtr->eta();
         m_jet_phi[i] = jetPtr->phi();
         m_jet_e[i] = jetPtr->e();

         m_jet_mv1eff[i] = jetPtr->auxdata<float>("mv1TagEff");

         if(jetPtr->auxdata<int>("pileUp")==0) {
           m_jet_isPileup[i]=0;
           m_jet_Ghosts_BHadron_Final_Count[i] = jetPtr->auxdata<int>( "GhostBHadronsFinalCount" );
           m_jet_Ghosts_CHadron_Final_Count[i] = jetPtr->auxdata<int>( "GhostCHadronsFinalCount" );
         } else {
           m_jet_isPileup[i]=1;
           m_jet_Ghosts_BHadron_Final_Count[i] = 0;
           m_jet_Ghosts_CHadron_Final_Count[i] = 0;
         }

         ++i;
       }

       // MET
       m_met_met = upgradeEvent.m_met->met();
       m_met_phi = upgradeEvent.m_met->phi();

    }

    void EventSaverFlatNtuple::fillUpgradeEvent() {
       // fill the tree
       m_upgradeTreeManager->fill();

    }//saveUpgradeEvent

    void EventSaverFlatNtuple::finalize()
    {
        m_outputFile->Write();
    }

    void EventSaverFlatNtuple::loadPdfInfo(){
        const xAOD::TruthEventContainer * truthEvent(nullptr);
        top::check( evtStore()->retrieve(truthEvent, m_config->sgKeyTruthEvent()) , "Failed to retrieve truth PDF info" );

        if (truthEvent != nullptr) {
            unsigned int i(0);
            unsigned int truthEventSize = truthEvent->size();
            m_PDFinfo_X1.resize(truthEventSize);
            m_PDFinfo_X2.resize(truthEventSize);
            m_PDFinfo_PDGID1.resize(truthEventSize);
            m_PDFinfo_PDGID2.resize(truthEventSize);
            m_PDFinfo_Q.resize(truthEventSize);
            m_PDFinfo_XF1.resize(truthEventSize);
            m_PDFinfo_XF2.resize(truthEventSize);

            for (const auto * const tePtr : * truthEvent) {
                // LMTODO: ASG tools/pdfInfo() can't be used yet, since scale Q is not available
                // xAOD::TruthEvent::PdfInfo _pdfInfo=tePtr->pdfInfo();
                // if _pdfInfo.valid()  { m_PDFinfo_X1[i] = _pdfInfo.x1; //etc. }
                std::string PDFinfoVarName="X1";
                if (truthEvent->isAvailable<float>(PDFinfoVarName))
                    m_PDFinfo_X1[i] = (*tePtr).auxdataConst< float >( PDFinfoVarName );
                PDFinfoVarName="X2";
                if (truthEvent->isAvailable<float>(PDFinfoVarName))
                    m_PDFinfo_X2[i] = (*tePtr).auxdataConst< float >( PDFinfoVarName );
                PDFinfoVarName="PDGID1";
                if (truthEvent->isAvailable<int>(PDFinfoVarName))
                    m_PDFinfo_PDGID1[i] = (*tePtr).auxdataConst< int >( PDFinfoVarName );
                PDFinfoVarName="PDGID2";
                if (truthEvent->isAvailable<int>(PDFinfoVarName))
                    m_PDFinfo_PDGID2[i] = (*tePtr).auxdataConst< int >( PDFinfoVarName );
                PDFinfoVarName="XF1";
                if (truthEvent->isAvailable<float>(PDFinfoVarName))
                    m_PDFinfo_XF1[i] = (*tePtr).auxdataConst< float >( PDFinfoVarName );
                PDFinfoVarName="XF2";
                if (truthEvent->isAvailable<float>(PDFinfoVarName))
                    m_PDFinfo_XF2[i] = (*tePtr).auxdataConst< float >( PDFinfoVarName );
                PDFinfoVarName="Q";
                if (truthEvent->isAvailable<float>(PDFinfoVarName))
                    m_PDFinfo_Q[i] = (*tePtr).auxdataConst< float >( PDFinfoVarName );
                ++i;
            }
        }
    }

    void EventSaverFlatNtuple::loadPdfWeights(){
        const xAOD::TruthEventContainer * truthEvent(nullptr);
        top::check( evtStore()->retrieve(truthEvent, m_config->sgKeyTruthEvent()) , "Failed to retrieve truth PDF info" );

        for( auto tePtr : * truthEvent ){
            for( const std::string & pdf_name : m_config->LHAPDFSets() ){
                if( tePtr->isAvailable<std::vector<float> >( "AnalysisTop_" + pdf_name + "_Weights") ){
                    m_PDF_eventWeights[ pdf_name ] = tePtr->auxdata<std::vector<float> >("AnalysisTop_" + pdf_name + "_Weights");
                }
            }
        }
    }

    void EventSaverFlatNtuple::loadMCGeneratorWeights(){
      const xAOD::TruthEventContainer * truthEvent(nullptr);
      top::check( evtStore()->retrieve(truthEvent, m_config->sgKeyTruthEvent()) , "Failed to retrieve MC Generator weights - impossible to retrieve truth event container" );
      unsigned int truthEventSize = truthEvent->size();
      top::check( truthEventSize==1 , "Failed to retrieve truth PDF info - truth event container size is different from 1 (strange)" );
      m_mc_generator_weights = truthEvent->at(0)->weights();
    }


    // This bit is new
    // It is not optimal and can do with a lot of improvement
    // Consider it a first pass
    //
    // We can save xAOD objects into flat ntuple by looking at their SG::AuxElement data
    //
    // inspired by:
    // https://svnweb.cern.ch/trac/atlasoff/browser/Event/xAOD/xAODCore/tags/xAODCore-00-00-93/Root/PrintHelpers.cxx

    void EventSaverFlatNtuple::registerObjectIntoTruthTree(const SG::AuxElement& obj)
    {
        // If the container doesn't have an auxiliary store, bail now:
        if (!obj.container()->getConstStore())
            return;

        // Get a convenience pointer to the aux type registry:
        SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();

        // Get the auxiliary IDs of the object:
        const SG::auxid_set_t& auxids =
            ( obj.container()->getConstStore() ?
              obj.container()->getConstStore()->getAuxIDs() :
              obj.getConstStore()->getAuxIDs() );

        // find the variables and fill them
        for (SG::auxid_t auxid : auxids) {
            // get the type of the variable
            const std::type_info* ti = reg.getType( auxid );
            // get the name of the variable
            const std::string name = reg.getName( auxid );

            // Add variable
            if (*ti == typeid( int ) ){
                m_extraTruthVars_int.insert( std::make_pair( name , new int ) );
            }
            else if (*ti == typeid( float ) ){
                m_extraTruthVars_float.insert( std::make_pair( name , new float ) );
            }
            else {
                ATH_MSG_INFO("insertObjectIntoTruthTree - you need another variable map for more types!");
            }
        }

        // add the variables to the truthTreeManager
        for (std::unordered_map<std::string,int*>::const_iterator i=m_extraTruthVars_int.begin();i!=m_extraTruthVars_int.end();++i) {
            m_truthTreeManager->makeOutputVariable(*(*i).second,(*i).first);
        }
        for (std::unordered_map<std::string,float*>::const_iterator i=m_extraTruthVars_float.begin();i!=m_extraTruthVars_float.end();++i) {
            m_truthTreeManager->makeOutputVariable(*(*i).second,(*i).first);
        }
    }

    void EventSaverFlatNtuple::saveObjectIntoTruthTree(const SG::AuxElement& obj)
    {
        // If the container doesn't have an auxiliary store, bail now:
        if (!obj.container()->getConstStore())
            return;

        // Get a convenience pointer to the aux type registry:
        SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
        // Get the auxiliary IDs of the object:
        const SG::auxid_set_t& auxids =
            ( obj.container()->getConstStore() ?
              obj.container()->getConstStore()->getAuxIDs() :
              obj.getConstStore()->getAuxIDs() );

        // setup new variables
        for (SG::auxid_t auxid : auxids) {
            // get the type of the variable
            const std::type_info* ti = reg.getType( auxid );
            // get the name of the variable
            const std::string name = reg.getName( auxid );

            // Add variable
            if (*ti == typeid( int ) ){
                std::unordered_map<std::string,int*>::iterator itr = m_extraTruthVars_int.find( name );
                if (itr != m_extraTruthVars_int.end()) {
                    *((*itr).second) = obj.auxdataConst<int> (name);
                }
            }
            else if (*ti == typeid( float ) ){
                std::unordered_map<std::string,float*>::iterator itr = m_extraTruthVars_float.find( name );
                if (itr != m_extraTruthVars_float.end()) {
                    *((*itr).second) = obj.auxdataConst<float> (name);
                }
            }
            else {
                ATH_MSG_INFO("saveObjectIntoTruthTree - you need another variable map for more types!");
            }
        }
    }

    // get short name for FixedCutBEff_* WPs
    std::string EventSaverFlatNtuple::shortBtagWP(std::string const & WP) const {
      std::string str = "FixedCutBEff_";
      std::string out = WP;
      if (out.find(str) != std::string::npos) {
        out.replace(out.find(str),str.length(),"");
      }
      return out;
    }

    // remove "FT_EFF_", spaces, and "-" in named systematics
    std::string EventSaverFlatNtuple::betterBtagNamedSyst (const std::string name) {
      std::string str = "FT_EFF_";
      std::string out = name;
      if (out.find(str) != std::string::npos) {
        out.replace(out.find(str),str.length(),"");
      }
      str = " ";
      while (out.find(str) != std::string::npos) {
        out.replace(out.find(str),str.length(),"_");
      }
      str = "-";
      while (out.find(str) != std::string::npos) {
        out.replace(out.find(str),str.length(),"_");
      }
      return out;
    }

} // namespace

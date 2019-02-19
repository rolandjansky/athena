/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TopSystematicObjectMaker/GhostTrackSystematicsMaker.h"

#include <limits>

#include "PATInterfaces/SystematicsUtil.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODBase/IParticleHelpers.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

#include "TopEvent/EventTools.h"
#include "TopConfiguration/TopConfig.h"

#include "InDetTrackSystematicsTools/InDetTrackSystematics.h"
#include "InDetTrackSystematicsTools/InDetTrackSmearingTool.h"
#include "InDetTrackSystematicsTools/InDetTrackBiasingTool.h"
#include "InDetTrackSystematicsTools/InDetTrackTruthFilterTool.h"
#include "InDetTrackSystematicsTools/InDetTrackTruthOriginTool.h"
#include "InDetTrackSystematicsTools/JetTrackFilterTool.h"

namespace top {

    /* explicit */ GhostTrackSystematicsMaker::GhostTrackSystematicsMaker(const std::string & name)
        : ::asg::AsgTool(name),
        m_config(nullptr),
	m_jetPtCut(0.),
        m_runPeriods(),
        m_specifiedSystematics(),
        m_recommendedSystematics(),
        m_nominalSystematicSet(),
        m_tools()
	 {

        declareProperty("config" , m_config);
    }

    /* virtual */ GhostTrackSystematicsMaker::~GhostTrackSystematicsMaker(){

    }

    StatusCode GhostTrackSystematicsMaker::initialize(){
        ATH_MSG_INFO(" top::GhostTrackSystematicsMaker initialize" );
        
        m_runPeriods = m_config->runPeriodsJetGhostTrack();

        // Retrieve the systematics tools. Each individual function will cause
        // program abortion upon failure. This is intended as there's no
        // meaningful method of recovery.
        // These tools are setup and initialised in the TopCPTools package, and we just retrieve them here.
        top::check(retrieveGhostTrackCPTool(),
             "Failed to retrieve the ghost track CP tools");

        // Setup the recommended systematics. This utilises the tools that were
        // setup before.
        const std:: string & syststr = m_config->systematics();

        if (m_config->isSystNominal(syststr)){
            specifiedSystematics({});
        } else if (m_config->isSystAll(syststr)){
            specifiedSystematics({"AllGhostTrack"});
        } else {
            std::set<std::string> syst;
            bool ok = m_config->getSystematicsList(syststr, syst);
            if (not ok) {
                ATH_MSG_ERROR(" top::GhostTrackSystematicsMaker could not determine systematic list" );
                return StatusCode::FAILURE;
            }

            // No need to manually handle the AllGhostTrack case in here.
            specifiedSystematics(syst);
        }

        // Pass the systematics list back to the top::TopConfig object.
        m_config->systematicsJetGhostTrack(specifiedSystematics());
	
	
	m_jetPtCut=m_config->jetPtGhostTracks();

	ATH_MSG_INFO(" top::GhostTrackSystematicsMaker: Systematic variations of ghost tracks will be done only for jets with pt >= " << m_jetPtCut << " MeV.");
	
        ATH_MSG_INFO(" top::GhostTrackSystematicsMaker completed initialize" );
        return StatusCode::SUCCESS;
    }

    StatusCode GhostTrackSystematicsMaker::applyNoOpSystematic(xAOD::JetContainer * nominal,
                                                               const CP::SystematicSet & syst) const {
        ///-- Loop over the xAOD Container --///
        for(const auto & jet : * nominal ){
	    if( jet->pt()<m_jetPtCut )continue;
            // Copy nominal ghost track container into the systematic variation.
	    
	    const auto & ghostTracks =
                jet->getAssociatedObjects<xAOD::IParticle>(m_config->decoKeyJetGhostTrack());
            std::vector<const xAOD::IParticle *> newGhosts;
	    
	    for (std::size_t iGhost=0; iGhost<ghostTracks.size(); ++iGhost){
		
		if(ghostTracks[iGhost]==nullptr)continue;
		
                // We can re-use the existing xAOD::IParticle.
                newGhosts.push_back(ghostTracks[iGhost]);
            }
	    
	    
	    
            jet->setAssociatedObjects(m_config->decoKeyJetGhostTrack(syst.hash()),
                                      newGhosts);
        }

        return StatusCode::SUCCESS;
    }

    StatusCode GhostTrackSystematicsMaker::applyTruthFilterSystematic(xAOD::JetContainer * nominal,
                                                                      InDet::InDetTrackTruthFilterTool * tool,
                                                                      const CP::SystematicSet & syst) const {
        ///-- Inform the tool --///
        top::check(tool->applySystematicVariation(syst),
                   "Failed to configure tool for systematic variation");

        ///-- Loop over the xAOD Container --///
        for(const auto & jet : * nominal ){
	    if( jet->pt()<m_jetPtCut )continue;
            const auto & ghostTracks =
                jet->getAssociatedObjects<xAOD::IParticle>(m_config->decoKeyJetGhostTrack());
            std::vector<const xAOD::IParticle *> newGhosts;

            for (std::size_t iGhost=0; iGhost<ghostTracks.size(); ++iGhost){
		
		if(ghostTracks[iGhost]==nullptr)continue;
                const xAOD::TrackParticle *
                    tp{dynamic_cast<const xAOD::TrackParticle*>(ghostTracks[iGhost])};
                top::check(tp, "Failed to convert xAOD::IParticle to xAOD::TrackParticle for ghost track");

                if (not tool->accept(tp)){
                    continue;
                }

                // We can re-use the existing xAOD::IParticle.
                newGhosts.push_back(ghostTracks[iGhost]);
            }

            jet->setAssociatedObjects(m_config->decoKeyJetGhostTrack(syst.hash()), newGhosts);
        }

        return StatusCode::SUCCESS;
    }

    StatusCode GhostTrackSystematicsMaker::applyJetTrackFilterSystematic(xAOD::JetContainer * nominal,
                                                                         InDet::JetTrackFilterTool * tool,
                                                                         const CP::SystematicSet & syst) const {
        ///-- Inform the tool --///
        top::check(tool->applySystematicVariation(syst),
                   "Failed to configure tool for systematic variation");

        ///-- Loop over the xAOD Container --///
        for(const auto & jet : * nominal ){
	    if( jet->pt()<m_jetPtCut )continue;
            const auto & ghostTracks = jet->getAssociatedObjects<xAOD::IParticle>(m_config->decoKeyJetGhostTrack());
            std::vector<const xAOD::IParticle *> newGhosts;


            for (std::size_t iGhost=0; iGhost<ghostTracks.size(); ++iGhost){
		
		if(ghostTracks[iGhost]==nullptr)continue;
                const xAOD::TrackParticle *
                    tp{dynamic_cast<const xAOD::TrackParticle*>(ghostTracks[iGhost])};
                top::check(tp, "Failed to convert xAOD::IParticle to xAOD::TrackParticle for ghost track");

                if (not tool->accept(tp, nominal)){
                    continue;
                }

                // We can re-use the existing xAOD::IParticle.
                newGhosts.push_back(ghostTracks[iGhost]);
            }

            jet->setAssociatedObjects(m_config->decoKeyJetGhostTrack(syst.hash()), newGhosts);
        }

        return StatusCode::SUCCESS;
    }

    StatusCode GhostTrackSystematicsMaker::applySmearingSystematic(xAOD::JetContainer * nominal,
                                                                   InDet::InDetTrackSmearingTool * tool,
                                                                   const CP::SystematicSet & syst) const {
        ///-- Inform the tool --///
        top::check(tool->applySystematicVariation(syst),
                   "Failed to configure tool for systematic variation");

        const std::string sgKey{m_config->decoKeyJetGhostTrack() +
                "_Particles_" + syst.name()};
        const std::string sgKeyAux{sgKey + "Aux."};


        ///-- Loop over the xAOD Container --///
        xAOD::TrackParticleContainer * newTrackParticles = new xAOD::TrackParticleContainer;
        top::check(evtStore()->record(newTrackParticles, sgKey),
                   "Failed to record systematically varied track particle container" );
        xAOD::TrackParticleAuxContainer * newTrackParticlesAux = new xAOD::TrackParticleAuxContainer;
        top::check(evtStore()->record(newTrackParticlesAux, sgKeyAux),
                   "Failed to record systematically varied track particle container" );
        newTrackParticles->setStore(newTrackParticlesAux);

        for (const auto & jet : * nominal){
	    if( jet->pt()<m_jetPtCut )continue;
            const auto & ghostTracks = jet->getAssociatedObjects<xAOD::TrackParticle>(m_config->decoKeyJetGhostTrack());

            std::vector<const xAOD::IParticle *> newGhosts;

            for (std::size_t iGhost=0; iGhost<ghostTracks.size(); ++iGhost){
	      
		if(ghostTracks[iGhost]==nullptr)continue;
		
                const xAOD::TrackParticle *
                    tp{dynamic_cast<const xAOD::TrackParticle*>(ghostTracks.at(iGhost))};
                top::check(tp, "Failed to convert xAOD::IParticle to xAOD::TrackParticle for ghost track");

                xAOD::TrackParticle * newTp{nullptr};
                top::check(tool->correctedCopy(* tp, newTp),
                           "Failure to apply ghost track systematic");
                top::check(newTp, "Systematically varied xAOD::TrackParticle is nullptr");

                newTrackParticles->push_back(newTp);
                newGhosts.push_back(newTp);
            }

            jet->setAssociatedObjects(m_config->decoKeyJetGhostTrack(syst.hash()), newGhosts);
        }

        return StatusCode::SUCCESS;
    }

    StatusCode GhostTrackSystematicsMaker::applyBiasingSystematic(xAOD::JetContainer * nominal,
                                                                   InDet::InDetTrackBiasingTool * tool,
                                                                   const CP::SystematicSet & syst) const {
        
	///-- Inform the tool --///
        top::check(tool->applySystematicVariation(syst),
                   "Failed to configure tool for systematic variation");

        const std::string sgKey{m_config->decoKeyJetGhostTrack() +
                "_Particles_" + syst.name()};
        const std::string sgKeyAux{sgKey + "Aux."};


        ///-- Loop over the xAOD Container --///
        xAOD::TrackParticleContainer * newTrackParticles = new xAOD::TrackParticleContainer;
        top::check(evtStore()->record(newTrackParticles, sgKey),
                   "Failed to record systematically varied track particle container" );
        xAOD::TrackParticleAuxContainer * newTrackParticlesAux = new xAOD::TrackParticleAuxContainer;
        top::check(evtStore()->record(newTrackParticlesAux, sgKeyAux),
                   "Failed to record systematically varied track particle container" );
        newTrackParticles->setStore(newTrackParticlesAux);

        for (const auto & jet : * nominal){
	  
	    if( jet->pt()<m_jetPtCut )continue;
            const auto & ghostTracks = jet->getAssociatedObjects<xAOD::TrackParticle>(m_config->decoKeyJetGhostTrack());

            std::vector<const xAOD::IParticle *> newGhosts;

            for (std::size_t iGhost=0; iGhost<ghostTracks.size(); ++iGhost){
	      
		if(ghostTracks[iGhost]==nullptr)continue;
	      
                const xAOD::TrackParticle *
                    tp{dynamic_cast<const xAOD::TrackParticle*>(ghostTracks.at(iGhost))};
                top::check(tp, "Failed to convert xAOD::IParticle to xAOD::TrackParticle for ghost track");

                xAOD::TrackParticle * newTp{nullptr};
                top::check(tool->correctedCopy(* tp, newTp),
                           "Failure to apply ghost track systematic");
                top::check(newTp, "Systematically varied xAOD::TrackParticle is nullptr");

                newTrackParticles->push_back(newTp);
                newGhosts.push_back(newTp);
            }

            jet->setAssociatedObjects(m_config->decoKeyJetGhostTrack(syst.hash()), newGhosts);
        }

        return StatusCode::SUCCESS;
    }

    StatusCode GhostTrackSystematicsMaker::execute(bool executeNominal){
        ATH_MSG_DEBUG(" top::GhostTrackSystematicsMaker execute:" );

        
        ///-- Get nominal jets --///
        xAOD::JetContainer * nominalJets(nullptr);
        top::check(evtStore()->retrieve(nominalJets,
                                        m_config->sgKeyJetsStandAlone(m_nominalSystematicSet.hash())),
                   "Failed to retrieve Jets");

	// applyNoOpSystematic is used just to remove ghost track vector from thinned jets
	top::check(applyNoOpSystematic(nominalJets, m_nominalSystematicSet),
                               "Failure to apply GhostTrackSystematic");

	// We don't want to do anything on Data -> bail early so that we can
        // rely on the inputs to be MC.
        if (not m_config->isMC()){
            return StatusCode::SUCCESS;
        }

        ///-- Only run this on the systematic execution --///
        if(executeNominal) return StatusCode::SUCCESS;


        ///-- SMEARING --///
        for (const auto & syst : m_systs.smearing){
             //std::cout << "[A] SYSTEMATIC = " << m_config->systematicName(syst.hash()) << std::endl;
            top::check(applySmearingSystematic(nominalJets, &(* m_tools.smearing), syst),
                       "Failure to apply GhostTrackSystematic");
        }

        ///-- BIASING --///
        InDet::InDetTrackBiasingTool * biasingTool{nullptr};
        if (m_runPeriods.size() == 1){
            biasingTool = &(* m_tools.bias[0]);

            top::check(biasingTool, "Failure to selected biasing tool");
            for (const auto & syst : m_systs.bias){
                 //std::cout << "[B] SYSTEMATIC = " << m_config->systematicName(syst.hash()) << std::endl;
                top::check(applyBiasingSystematic(nominalJets, biasingTool, syst),
                           "Failure to apply GhostTrackSystematic");
            }
        } else {
            const xAOD::EventInfo * ei{nullptr};
            top::check(evtStore()->retrieve(ei, "EventInfo"),
                       "Failure to retrieve EventInfo");

            top::check(ei->isAvailable<unsigned int>("RandomRunNumber"),
                       "Require that RandomRunNumber decoration is available.");
            auto randomRunNumber = ei->auxdataConst<unsigned int>("RandomRunNumber");

            if (randomRunNumber == 0){
                for (const auto & syst : m_systs.bias){
                     //std::cout << "[B.1] SYSTEMATIC = " << m_config->systematicName(syst.hash()) << std::endl;
                    top::check(applyNoOpSystematic(nominalJets, syst),
                               "Failure to apply GhostTrackSystematic");
                }
            } else {
                top::check(m_runPeriods[0] <= randomRunNumber,
                           "RandomRunNumber is below valid range.");
                top::check(randomRunNumber < m_runPeriods[m_runPeriods.size() - 1],
                           "RandomRunNumber is above valid range.");

                for (std::size_t i=1; i<m_runPeriods.size(); ++i){
                    if (randomRunNumber < m_runPeriods[i]){
                        biasingTool = &(* m_tools.bias[i - 1]);
                        break;
                    }
                }
                top::check(biasingTool, "Failure to selected biasing tool");
                for (const auto & syst : m_systs.bias){
                     //std::cout << "[B.2] SYSTEMATIC = " << m_config->systematicName(syst.hash()) << std::endl;
                    top::check(applyBiasingSystematic(nominalJets, biasingTool, syst),
                               "Failure to apply GhostTrackSystematic");
                }
            }
        }

        ///-- TRUTH FILTER --///
        for (const auto & syst : m_systs.truthFilter){
             //std::cout << "[C] SYSTEMATIC = " << m_config->systematicName(syst.hash()) << std::endl;
            top::check(applyTruthFilterSystematic(nominalJets, &(* m_tools.truthFilter), syst),
                       "Failure to apply GhostTrackSystematic");
        }

        ///-- JET TRACK FILTER --///
        for (const auto & syst : m_systs.jetTrackFilter){
             //std::cout << "[D] SYSTEMATIC = " << m_config->systematicName(syst.hash()) << std::endl;
            top::check(applyJetTrackFilterSystematic(nominalJets, &(* m_tools.jetTrackFilter), syst),
                       "Failure to apply GhostTrackSystematic");
        }

        ATH_MSG_DEBUG(" top::GhostTrackSystematicsMaker completed execute" );
        return StatusCode::SUCCESS;
    }


    void GhostTrackSystematicsMaker::specifiedSystematics(const std::set<std::string> & specSys) {
        // NOTE: For this function -- unlike the "proper" ObjectCollectionMakers in AnalysisTop -- an empty specifiedSystematics input is considered to represent "No Systematics". All GhostTrack systematics can be requested with the string "AllGhostTrack".
	const bool allGhostTrackSystematics = m_config->contains(specSys,
                                                                 "AllGhostTrack");

        // Acquire the recommended systematics from the various tools and put
        // into containers that store the per-tool systematics as well as a
        // global container. At a later point we'll make the per-tool containers
        // unique and check that the containers for various tools are
        // orthogonal.
        std::set<CP::SystematicSet> systs;

        // SMEARING
        m_systs.smearing =
            CP::make_systematics_vector(m_tools.smearing->recommendedSystematics());
        systs.insert(m_systs.smearing.begin(), m_systs.smearing.end());

        // BIAS
        for (const auto & tool : m_tools.bias){
            m_systs.bias =
                CP::make_systematics_vector(tool->recommendedSystematics());
            systs.insert(m_systs.bias.begin(), m_systs.bias.end());
        }

        // TRUTH FILTER
        m_systs.truthFilter =
            CP::make_systematics_vector(m_tools.truthFilter->recommendedSystematics());
        systs.insert(m_systs.truthFilter.begin(), m_systs.truthFilter.end());

        // JET TRACK FILTER
        m_systs.jetTrackFilter =
            CP::make_systematics_vector(m_tools.jetTrackFilter->recommendedSystematics());
        systs.insert(m_systs.jetTrackFilter.begin(), m_systs.jetTrackFilter.end());

        // PUT INTO RECOMMENDED AND SPECIFIED
        for (const auto & s : systs){
            // We don't want nominal.
            if (s.name() == ""){
                continue;
            }

            m_recommendedSystematics.push_back(s);

            ///-- MC only --///
            if (m_config->isMC()) {
                ///-- Are we only doing Nominal? Did the user specify specific systematics to use? --///
                if (not m_config->isSystNominal(m_config->systematics())) {
                    if (allGhostTrackSystematics) {
                        m_specifiedSystematics.push_back(s);
                        continue;
                    }

                    for (const auto & i : specSys) {
                        if (i == s.name()) {
                            m_specifiedSystematics.push_back(s);
                        }
                    }
                }
            }
        }

        m_recommendedSystematics.sort();
        m_recommendedSystematics.unique();

        m_specifiedSystematics.sort();
        m_specifiedSystematics.unique();

        // Make the sublists unique and remove any elements that have not "been specified".
        m_systs.makeUnique();
        m_systs.removeNonSpecified(m_specifiedSystematics);

        // Lastly, let use introduce a safeguard which ensures that the
        // systematics selected for the various tools are orthogonal. This is
        // done by asserting that the sum of the sizes of the individual
        // systematics lists equals the size of the list of specified
        // systematics. We can do this because we've made the individual
        // per-tool containers unique (in makeUnique).
        top::check(m_systs.numSystematics() == m_specifiedSystematics.size(),
                   "Sanity check failed");
    }

    StatusCode GhostTrackSystematicsMaker::retrieveGhostTrackCPTool(){
        ATH_MSG_INFO(" Retrieving ghost track CPTools" );
        
        std::string smearingToolName{"top::GhostTrackCPTools::InDetTrackSmearingTool"};
        if (asg::ToolStore::contains<InDet::InDetTrackSmearingTool>(smearingToolName)) {
            m_tools.smearing =
                asg::ToolStore::get<InDet::InDetTrackSmearingTool>(smearingToolName);
        }
        else {
          ATH_MSG_ERROR(" Impossible to retrieve " + smearingToolName );
          return StatusCode::FAILURE;
        }
        
        std::string biasToolPrefix{"top::GhostTrackCPTools::InDetTrackBiasingTool"};
        top::check(not m_runPeriods.empty(), "Assertion failed");
        // Two cases are possible:
        //     - Either a single run number was specified to the runPeriods
        //       parameter in which case we'll use exactly that run number, or
        //     - at least two numbers have been specified, which then define
        //       (potentially multiple) run number ranges.
        unsigned int end = 0;
        if (m_runPeriods.size() == 1) {
          end = 1;
          m_tools.bias.resize(1);
        }
        else {
          end = m_runPeriods.size()-1;
          m_tools.bias.resize(m_runPeriods.size()-1);
        }
        for (unsigned int i = 0; i<end; i++) {
          std::string biasToolName{""};
          if (m_runPeriods.size() == 1) {
            biasToolName = biasToolPrefix + "_" + std::to_string(m_runPeriods[0]);
          }
          else {
            biasToolName = biasToolPrefix + "_" + std::to_string(m_runPeriods[i]) + "_" + std::to_string(m_runPeriods[i + 1]);
          }
          if (asg::ToolStore::contains<InDet::InDetTrackBiasingTool>(biasToolName)){
            m_tools.bias[i] = asg::ToolStore::get<InDet::InDetTrackBiasingTool>(biasToolName);
          }
          else {
            ATH_MSG_ERROR(" Impossible to retrieve " + biasToolName );
            return StatusCode::FAILURE;
          }
        }
        
        std::string truthOriginToolName{"top::GhostTrackCPTools::InDetTrackTruthOriginTool"};
        if (asg::ToolStore::contains<InDet::InDetTrackTruthOriginTool>(truthOriginToolName)){
          m_tools.truthOrigin = asg::ToolStore::get<InDet::InDetTrackTruthOriginTool>(truthOriginToolName);
        }
        else {
          ATH_MSG_ERROR(" Impossible to retrieve " + truthOriginToolName );
          return StatusCode::FAILURE;
        }
        
        std::string truthFilterToolName{"top::GhostTrackCPTools::InDetTrackTruthFilterTool"};
        if (asg::ToolStore::contains<InDet::InDetTrackTruthFilterTool>(truthFilterToolName)){
          m_tools.truthFilter = asg::ToolStore::get<InDet::InDetTrackTruthFilterTool>(truthFilterToolName);
        }
        else {
          ATH_MSG_ERROR(" Impossible to retrieve " + truthFilterToolName );
          return StatusCode::FAILURE;
        }
        
        std::string jetTrackFilterToolName{"top::GhostTrackCPTools::JetTrackFilterTool"};
        if (asg::ToolStore::contains<InDet::JetTrackFilterTool>(jetTrackFilterToolName)){
          m_tools.jetTrackFilter = asg::ToolStore::get<InDet::JetTrackFilterTool>(jetTrackFilterToolName);
        }
        else {
          ATH_MSG_ERROR(" Impossible to retrieve " + jetTrackFilterToolName );
          return StatusCode::FAILURE;
        }
        
        return StatusCode::SUCCESS;
    }
    
}




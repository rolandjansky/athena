/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopSystematicObjectMaker/TrackSystematicsMaker.h"

#include <limits>

#include "PATInterfaces/SystematicsUtil.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODBase/IParticleHelpers.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

#include "TopEvent/EventTools.h"
#include "TopConfiguration/TopConfig.h"
#include "TopConfiguration/TreeFilter.h"

#include "InDetTrackSystematicsTools/InDetTrackSystematics.h"
#include "InDetTrackSystematicsTools/InDetTrackSmearingTool.h"
#include "InDetTrackSystematicsTools/InDetTrackBiasingTool.h"
#include "InDetTrackSystematicsTools/InDetTrackTruthFilterTool.h"
#include "InDetTrackSystematicsTools/InDetTrackTruthOriginTool.h"

// Other includes:                                                                                                                                                                                       
#include "xAODCore/ShallowCopy.h"

namespace top {
  /* explicit */
  TrackSystematicsMaker::TrackSystematicsMaker(const std::string& name)
    : ::asg::AsgTool(name),
    m_config(nullptr),
    m_runPeriods(),
    m_specifiedSystematics(),
    m_recommendedSystematics(),
    m_nominalSystematicSet(),
    m_smearingTool("top::TrackCPTools::InDetTrackSmearingTool"),
    m_truthOriginTool("top::TrackCPTools::InDetTrackTruthOriginTool"),   
    m_truthFilterTool("top::TrackCPTools::InDetTrackTruthFilterTool")                 
  {
    declareProperty("config", m_config);
  }
  
  /* virtual */ TrackSystematicsMaker::~TrackSystematicsMaker() {
  }

  StatusCode TrackSystematicsMaker::initialize() {
    ATH_MSG_INFO(" top::TrackSystematicsMaker initialize");

    ATH_MSG_WARNING("Usage of tracks in AnalysisTop is experimental for now!\n");

    m_runPeriods = m_config->runPeriodsTrack();

    m_specifiedSystematics.push_back(CP::SystematicSet());

    // Retrieve the systematics tools. Each individual function will cause
    // program abortion upon failure. This is intended as there's no
    // meaningful method of recovery.
    // These tools are setup and initialised in the TopCPTools package, and we just retrieve them here.
    top::check(retrieveTrackCPTool(), "Failed to retrieve the track CP tools");
    
    // Setup the recommended systematics. This utilises the tools that were
    // setup before.
    const std:: string& syststr = m_config->systematics();
    std::set<std::string> syst;

    if (m_config->isSystNominal(syststr)) {

      specifiedSystematics({});


    } else if (m_config->isSystAll(syststr)) {

      specifiedSystematics({"AllTracks"});

    } else {

      std::set<std::string> syst;
      bool ok = m_config->getSystematicsList(syststr, syst);
      if (!ok) {
        ATH_MSG_ERROR(" top::TrackSystematicsMaker could not determine systematic list");
        return StatusCode::FAILURE;
      }


      // No need to manually handle the AllTrack case in here.
      specifiedSystematics(syst);
    }


    // Pass the systematics list back to the top::TopConfig object.
    m_config->systematicsTracks(specifiedSystematics());

    ATH_MSG_INFO(" top::TrackSystematicsMaker completed initialize");
    return StatusCode::SUCCESS;
  }

  

  StatusCode TrackSystematicsMaker::applyTruthFilterSystematic(InDet::InDetTrackTruthFilterTool* tool,
							       const CP::SystematicSet& syst) const {

    if(m_config->sgKeyTracks(syst.hash()) == "InDetTrackParticles_")
      return StatusCode::SUCCESS;

    // Create a shallow copy of InDetTrackParticles:                                                                                                                                                    
    const xAOD::TrackParticleContainer *tp(nullptr);
    top::check(evtStore()->retrieve(tp, "InDetTrackParticles"), "Retrieve InDetParticles");

    std::pair< xAOD::TrackParticleContainer*, xAOD::ShallowAuxContainer* > shallow_xaod_copy = xAOD::shallowCopyContainer( *tp );
    
    ///-- Inform the tool --///
    top::check(tool->applySystematicVariation(syst),
               "Failed to configure tool for systematic variation");
    


    for ( xAOD::TrackParticle* trkCpy : *(shallow_xaod_copy.first) ) {

      
      if (not tool->accept(trkCpy)) trkCpy->auxdecor<char>("passTruthFilter") = 0;
      else trkCpy->auxdecor<char>("passTruthFilter") = 1;
	

    }

    ///-- Save corrected xAOD Container to StoreGate / TStore --///                                                                                                                                      
    std::string outputSGKey = m_config->sgKeyTracks(syst.hash());
    std::string outputSGKeyAux = outputSGKey + "Aux.";


    xAOD::TReturnCode save = evtStore()->tds()->record(shallow_xaod_copy.first, outputSGKey);
    xAOD::TReturnCode saveAux = evtStore()->tds()->record(shallow_xaod_copy.second, outputSGKeyAux);

    if (!save || !saveAux) {
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }


  StatusCode TrackSystematicsMaker::applySmearingSystematic(InDet::InDetTrackSmearingTool* tool,
							    const CP::SystematicSet& syst) const {


    if(m_config->sgKeyTracks(syst.hash()) == "InDetTrackParticles_")
      return StatusCode::SUCCESS;

    // Create a shallow copy of InDetTrackParticles:                                                                                                                                                      
    const xAOD::TrackParticleContainer *tp(nullptr);
    top::check(evtStore()->retrieve(tp, "InDetTrackParticles"), "Retrieve InDetParticles");

    std::pair< xAOD::TrackParticleContainer*, xAOD::ShallowAuxContainer* > shallow_xaod_copy = xAOD::shallowCopyContainer( *tp );

    ///-- Inform the tool --///
    top::check(tool->applySystematicVariation(syst), "Failed to configure tool for systematic variation");


    for ( xAOD::TrackParticle* trkCpy : *(shallow_xaod_copy.first) ) { 

      top::check(tool->applyCorrection(*trkCpy), "Failure to apply track systematic");
    
    }

    ///-- Save corrected xAOD Container to StoreGate / TStore --///                                                                                                                                       
    std::string outputSGKey = m_config->sgKeyTracks(syst.hash());
    std::string outputSGKeyAux = outputSGKey + "Aux.";

 
    xAOD::TReturnCode save = evtStore()->tds()->record(shallow_xaod_copy.first, outputSGKey);
    xAOD::TReturnCode saveAux = evtStore()->tds()->record(shallow_xaod_copy.second, outputSGKeyAux);
    
    if (!save || !saveAux) {
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  StatusCode TrackSystematicsMaker::applyBiasingSystematic(InDet::InDetTrackBiasingTool* tool,
                                                                const CP::SystematicSet& syst) const {
    
    if(m_config->sgKeyTracks(syst.hash()) == "InDetTrackParticles_")
      return StatusCode::SUCCESS;


    // Create a shallow copy of InDetTrackParticles:                                                                                                                                                     
    const xAOD::TrackParticleContainer *tp(nullptr);
    top::check(evtStore()->retrieve(tp, "InDetTrackParticles"), "Retrieve InDetParticles");

    std::pair< xAOD::TrackParticleContainer*, xAOD::ShallowAuxContainer* > shallow_xaod_copy = xAOD::shallowCopyContainer( *tp );

    ///-- Inform the tool --///                                                                                                                                                                          
    top::check(tool->applySystematicVariation(syst),
               "Failed to configure tool for systematic variation");

    for ( xAOD::TrackParticle* trkCpy : *(shallow_xaod_copy.first) ) {
    
      top::check(tool->applyCorrection(*trkCpy), "Failure to apply track systematic");

    }

    ///-- Save corrected xAOD Container to StoreGate / TStore --///                                                                                                                                       
    std::string outputSGKey = m_config->sgKeyTracks(syst.hash());
    std::string outputSGKeyAux = outputSGKey + "Aux.";

    xAOD::TReturnCode save = evtStore()->tds()->record(shallow_xaod_copy.first, outputSGKey);
    xAOD::TReturnCode saveAux = evtStore()->tds()->record(shallow_xaod_copy.second, outputSGKeyAux);

    if (!save || !saveAux) {
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  StatusCode TrackSystematicsMaker::execute(bool executeNominal) {
    ATH_MSG_DEBUG(" top::TrackSystematicsMaker execute:");


    if (!m_config->isMC() && executeNominal) {

      // Create a shallow copy of InDetTrackParticles:                                                                                                                                                   
      const xAOD::TrackParticleContainer *tp(nullptr);
      top::check(evtStore()->retrieve(tp, "InDetTrackParticles"), "Retrieve InDetParticles");

      std::pair< xAOD::TrackParticleContainer*, xAOD::ShallowAuxContainer* > shallow_xaod_copy = xAOD::shallowCopyContainer( *tp );

      ///-- Save corrected xAOD Container to StoreGate / TStore --///                                                                                                                                     
      std::string outputSGKey = m_config->sgKeyTracks(m_nominalSystematicSet.hash());
      std::string outputSGKeyAux = outputSGKey + "Aux.";

      xAOD::TReturnCode save = evtStore()->tds()->record(shallow_xaod_copy.first, outputSGKey);
      xAOD::TReturnCode saveAux = evtStore()->tds()->record(shallow_xaod_copy.second, outputSGKeyAux);

      if (!save || !saveAux) {
	return StatusCode::FAILURE;
      }


      return StatusCode::SUCCESS;
    }
    if (!m_config->isMC() && !executeNominal) {
      return StatusCode::SUCCESS;
    }


    ///-- Only run this on the systematic execution --///
    if (executeNominal){

      // Create a shallow copy of InDetTrackParticles:                                                                                                                                                    
      const xAOD::TrackParticleContainer *tp(nullptr);
      top::check(evtStore()->retrieve(tp, "InDetTrackParticles"), "Retrieve InDetParticles");

      std::pair< xAOD::TrackParticleContainer*, xAOD::ShallowAuxContainer* > shallow_xaod_copy = xAOD::shallowCopyContainer( *tp );

      ///-- Save corrected xAOD Container to StoreGate / TStore --///                                                                                                                                     
      std::string outputSGKey = m_config->sgKeyTracks(m_nominalSystematicSet.hash());
      std::string outputSGKeyAux = outputSGKey + "Aux.";

      xAOD::TReturnCode save = evtStore()->tds()->record(shallow_xaod_copy.first, outputSGKey);
      xAOD::TReturnCode saveAux = evtStore()->tds()->record(shallow_xaod_copy.second, outputSGKeyAux);

      if (!save || !saveAux) {
	return StatusCode::FAILURE;
      }


      return StatusCode::SUCCESS;

    }

    ///-- SMEARING --///
    for (const auto& syst : m_systs.smearing) {
      top::check(applySmearingSystematic(&(*m_smearingTool), syst),
		 "Failure to apply TrackSystematic");
    }

    ///-- BIASING --///
    InDet::InDetTrackBiasingTool* biasingTool {
      nullptr
    };

    if (m_runPeriods.size() == 1) {
      biasingTool = &(*m_biasTool[0]);

      top::check(biasingTool, "Failure to selected biasing tool");
      for (const auto& syst : m_systs.bias) {
	top::check(applyBiasingSystematic(biasingTool, syst),
                   "Failure to apply TrackSystematic");
      }
    } 
    else {
      const xAOD::EventInfo* ei {
        nullptr
	  };
      top::check(evtStore()->retrieve(ei, "EventInfo"),
                 "Failure to retrieve EventInfo");
      
      top::check(ei->isAvailable<unsigned int>("RandomRunNumber"),
                 "Require that RandomRunNumber decoration is available.");
      auto randomRunNumber = ei->auxdataConst<unsigned int>("RandomRunNumber");
      
      if (randomRunNumber != 0 ){
        top::check(m_runPeriods[0] <= randomRunNumber,
                   "RandomRunNumber is below valid range.");
        top::check(randomRunNumber < m_runPeriods[m_runPeriods.size() - 1],
                   "RandomRunNumber is above valid range.");
	
        for (std::size_t i = 1; i < m_runPeriods.size(); ++i) {
          if (randomRunNumber < m_runPeriods[i]) {
            biasingTool = &(*m_biasTool[i - 1]);
            break;
          }
        }
        top::check(biasingTool, "Failure to selected biasing tool");
        for (const auto& syst : m_systs.bias) {
	  top::check(applyBiasingSystematic(biasingTool, syst),
		     "Failure to apply TrackSystematic");
        }
      }
    }

    ///-- TRUTH FILTER --///                                                                                                                                                                           
    for (const auto& syst : m_systs.truthFilter) {                                                                                                                                                 

      top::check(applyTruthFilterSystematic(&(*m_truthFilterTool), syst),
		 "Failure to apply TrackSystematic");                                                                                                                                                      
    } 

    ATH_MSG_DEBUG(" top::TrackSystematicsMaker completed execute");
    return StatusCode::SUCCESS;
  }

  void TrackSystematicsMaker::specifiedSystematics(const std::set<std::string>& specSys) {

    // NOTE: For this function -- unlike the "proper" ObjectCollectionMakers in AnalysisTop -- an empty
    // specifiedSystematics input is considered to represent "No Systematics". All Track systematics can be
    // requested with the string "AllTrack".
    const bool allTrackSystematics = m_config->contains(specSys, "AllTracks");

    // Acquire the recommended systematics from the various tools and put
    // into containers that store the per-tool systematics as well as a
    // global container. At a later point we'll make the per-tool containers
    // unique and check that the containers for various tools are
    // orthogonal.
    std::set<CP::SystematicSet> systs;

    // SMEARING
    m_systs.smearing = CP::make_systematics_vector(m_smearingTool->recommendedSystematics());
    systs.insert(m_systs.smearing.begin(), m_systs.smearing.end());

    // BIAS
    for (const auto& tool : m_biasTool) {
      m_systs.bias =
        CP::make_systematics_vector(tool->recommendedSystematics());
      systs.insert(m_systs.bias.begin(), m_systs.bias.end());
    }

    // TRUTH FILTER                                                                                                                                                                                   
    m_systs.truthFilter =                                                                                                                                                                                 
      CP::make_systematics_vector(m_truthFilterTool->recommendedSystematics());                                                                                                                         
    systs.insert(m_systs.truthFilter.begin(), m_systs.truthFilter.end()); 

    // PUT INTO RECOMMENDED AND SPECIFIED
    for (const auto& s : systs) {

      // We don't want nominal.
      if (s.name() == "") {
        continue;
      }

      m_recommendedSystematics.push_back(s);

      ///-- MC only --///
      if (m_config->isMC()) {
        ///-- Are we only doing Nominal? Did the user specify specific systematics to use? --///
        if (!m_config->isSystNominal(m_config->systematics())) {
          if (allTrackSystematics) {

            m_specifiedSystematics.push_back(s);
            continue;
          }

          for (const auto& i : specSys) {
            TreeFilter filter(i);
	    if (!filter.filterTree(s.name())) {
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
    if(m_specifiedSystematics.size() > 1)
      top::check(systs.size() == m_specifiedSystematics.size(), "Sanity check failed");
    else
      top::check(m_specifiedSystematics.size() == 1, "Sanity check failed");


  }

  StatusCode TrackSystematicsMaker::retrieveTrackCPTool() {
    ATH_MSG_INFO(" Retrieving track CPTools");


    top::check(m_smearingTool.retrieve(), "Failed to retrieve track smearing tool");
    
    std::string biasToolPrefix {"top::TrackCPTools::InDetTrackBiasingTool"};
    top::check(not m_runPeriods.empty(), "Assertion failed");
    // Two cases are possible:
    //     - Either a single run number was specified to the runPeriods
    //       parameter in which case we'll use exactly that run number, or
    //     - at least two numbers have been specified, which then define
    //       (potentially multiple) run number ranges.
    unsigned int end = 0;
    if (m_runPeriods.size() == 1) {
      end = 1;
      m_biasTool.resize(1);
    } else {
      end = m_runPeriods.size() - 1;
      m_biasTool.resize(m_runPeriods.size() - 1);
    }

    for (unsigned int i = 0; i < end; i++) {
  

      std::string biasToolName {
        ""
      };
      if (m_runPeriods.size() == 1) {
        biasToolName = biasToolPrefix + "_" + std::to_string(m_runPeriods[0]);
      } 
      else {
        biasToolName = biasToolPrefix + "_" + std::to_string(m_runPeriods[i]) + "_" +
                       std::to_string(m_runPeriods[i + 1]);
      }


      if (asg::ToolStore::contains<InDet::InDetTrackBiasingTool>(biasToolName)) {
	m_biasTool[i] = asg::ToolStore::get<InDet::InDetTrackBiasingTool>(biasToolName);
      } else {
        ATH_MSG_ERROR(" Impossible to retrieve " + biasToolName);
        return StatusCode::FAILURE;
      }



    }

    top::check(m_truthOriginTool.retrieve(), "Failed to retrieve track truth origin tool");
    top::check(m_truthFilterTool.retrieve(), "Failed to retrieve track truth filter tool");
    

    return StatusCode::SUCCESS;
  }

  
  inline const std::list<CP::SystematicSet>& TrackSystematicsMaker::specifiedSystematics() const {
    return m_specifiedSystematics;
  }

  inline const std::list<CP::SystematicSet>& TrackSystematicsMaker::recommendedSystematics() const {
    return m_recommendedSystematics;
  }

}

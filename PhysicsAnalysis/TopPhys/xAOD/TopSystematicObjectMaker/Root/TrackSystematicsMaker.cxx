/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "InDetTrackSystematicsTools/JetTrackFilterTool.h"

// Other includes:                                                                                                                                                                                       
#include "xAODCore/ShallowCopy.h"
#include "CxxUtils/make_unique.h"

namespace top {
  /* explicit */
  TrackSystematicsMaker::TrackSystematicsMaker(const std::string& name)
    : ::asg::AsgTool(name),
    m_config(nullptr),
    m_jetPtCut(0.),
    m_runPeriods(),
    m_specifiedSystematics(),
    m_recommendedSystematics(),
    m_nominalSystematicSet(),
    m_tools() {
    declareProperty("config", m_config);
  }

  /* virtual */ TrackSystematicsMaker::~TrackSystematicsMaker() {
  }

  StatusCode TrackSystematicsMaker::initialize() {
    ATH_MSG_INFO(" top::TrackSystematicsMaker initialize");

    ATH_MSG_WARNING("Usage of tracks in AnalysisTop is experimental for now!\n");

    m_runPeriods = m_config->runPeriodsJetGhostTrack();

    m_specifiedSystematics.push_back(CP::SystematicSet());

    // Retrieve the systematics tools. Each individual function will cause
    // program abortion upon failure. This is intended as there's no
    // meaningful method of recovery.
    // These tools are setup and initialised in the TopCPTools package, and we just retrieve them here.
    top::check(retrieveTrackCPTool(),
               "Failed to retrieve the track CP tools");

    ////std::cout << "init 1 " << std::endl;



    //    const xAOD::TrackParticleContainer* newTrackParticles = new xAOD::TrackParticleContainer;
    // top::check(evtStore()->record(newTrackParticles, m_config->sgKeyTracks(m_config->nominalHashValue())),
    //          "Failed to record systematically varied track particle container");


    // Setup the recommended systematics. This utilises the tools that were
    // setup before.
    const std:: string& syststr = m_config->systematics();
    std::set<std::string> syst;

    ////std::cout << "init 2 " << std::endl;
    /*
 
    if (!m_config->isSystNominal(syststr) && !m_config->isSystAll(syststr)) {
      bool ok = m_config->getSystematicsList(syststr, syst);
      if (!ok) {
        ATH_MSG_ERROR(" top::TrackSystematicsMaker could not determine systematic list");
        return StatusCode::FAILURE;
      }
      //here the idea is that if the user specifies AllXXX, we leave syst as an empty string, so that all recommended CP                                                                       
      // systematics are then used                                                                                                                                                                     
      if (m_config->contains(syst, "AllTracks")) {
        syst.clear();
      }
    }
    
    //std::cout << "init  3 " << syst.size() << std::endl; 

    specifiedSystematics(syst);

    */
    
    ////std::cout << "init 2 a " << std::endl;

    if (m_config->isSystNominal(syststr)) {

      ////std::cout << "init 2 b " << std::endl;

      specifiedSystematics({});

      ////std::cout << "init 2 c " << std::endl;

    } else if (m_config->isSystAll(syststr)) {

      ////std::cout << "init 2 d " << std::endl;

      specifiedSystematics({"AllTracks"});

      ////std::cout << "init 2 e " << std::endl;

    } else {

      ////std::cout << "init 2 f " << std::endl;

      std::set<std::string> syst;
      bool ok = m_config->getSystematicsList(syststr, syst);
      if (not ok) {

	////std::cout << "init 2 g " << std::endl;

        ATH_MSG_ERROR(" top::TrackSystematicsMaker could not determine systematic list");
        return StatusCode::FAILURE;
      }

      ////std::cout << "init 3 " << std::endl;

      // No need to manually handle the AllTrack case in here.
      specifiedSystematics(syst);
    }


    

    ////std::cout << "init 4 " << std::endl;


    // Pass the systematics list back to the top::TopConfig object.
    m_config->systematicsTracks(specifiedSystematics());


    // ANDREA
    /*
    m_jetPtCut = m_config->jetPtTracks();
    m_jetEtaCut = m_config->jetEtacut();

    ATH_MSG_INFO(
      " top::TrackSystematicsMaker: Systematic variations of tracks will be done only for jets with pt >= " << m_jetPtCut << " MeV and eta <= " << m_jetEtaCut <<
        ".");

    */

    ATH_MSG_INFO(" top::TrackSystematicsMaker completed initialize");
    return StatusCode::SUCCESS;
  }

  

  StatusCode TrackSystematicsMaker::applyTruthFilterSystematic(InDet::InDetTrackTruthFilterTool* tool,
							       const CP::SystematicSet& syst) const {
    ///-- Inform the tool --///
    top::check(tool->applySystematicVariation(syst),
               "Failed to configure tool for systematic variation");

    const xAOD::TrackParticleContainer* xaod(nullptr);
    top::check(evtStore()->retrieve(xaod, m_config -> sgKeyTracks()), "Failed to retrieve Jets");


    ///-- Shallow copy of the xAOD --///                                                                                                                                                                  
    //    std::pair< xAOD::TrackParticleContainer*, xAOD::ShallowAuxContainer* >
    //  shallow_xaod_copy = xAOD::shallowCopyContainer(*xaod);

    /*
    ///-- Loop over the xAOD Container --///
    for (const auto& track : *nominal) {

      if (track == nullptr) continue;
    
      if (not tool->accept(track)) {
          continue;
        }

        // We can re-use the existing xAOD::IParticle.
        news.push_back(tracks[i]);
      }

    //

    ///-- Shallow copy of the xAOD --///                                                                                                                                                                
    std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* >
      shallow_xaod_copy = xAOD::shallowCopyContainer(*xaod);

    ///-- Loop over the xAOD Container --///                                                                                                                                                            
    for (auto jet : *(shallow_xaod_copy.first)) {
      if (isLargeR && m_config->isMC()) { //JES for large-R jets only exist for MC                                                                                                                      
	///-- Only large R jets with the following properties can be calibrated.--///                                                                                                                   
	bool calibratable_jet = (std::fabs(jet->eta()) <= 2.0
				 && jet->pt() > 175e3); //lower than 200e3 to allow studying on migration                                                                                               
	std::string jetCalibrationNameLargeR = m_config->sgKeyLargeRJets();
	if (jetCalibrationNameLargeR.find("TrackCaloCluster") != std::string::npos) { //TCC jet                                                                                                         
	  calibratable_jet = (jet->m() / jet->pt() <= 1.
			      && jet->m() / jet->pt() >= 0.
			      && std::fabs(jet->eta()) <= 2.0
			      && jet->pt() > 150e3
			      && jet->pt() < 3000e3);
	}
	if (!calibratable_jet) continue;
      }
      ///-- Apply Corrrection --///                                                                                                                                                                     
      if (!(isLargeR && !m_config->isMC())) { //Large-R jet uncertainties don't apply to Data                                                                                                           
	top::check(tool->applyCorrection(*jet), "Failed to applyCorrection");
      }
      ///-- Update JVT --///                                                                                                                                                                            
      if (!isLargeR) jet->auxdecor<float>("AnalysisTop_JVT") = m_jetUpdateJvtTool->updateJvt(*jet);

      ///-- Apply large-R jet tagging SF uncertainties --///                                                                                                                                            
      if (isLargeR && m_config->isMC()) {
	for (std::pair<const std::string, ToolHandle<ICPJetUncertaintiesTool> >& tagSF : m_tagSFuncertTool) {
	  top::check(tagSF.second->applyCorrection(*jet), "Failed to applyCorrection");
	}
      }
    }

    */

    return StatusCode::SUCCESS;
  }


  StatusCode TrackSystematicsMaker::applySmearingSystematic(InDet::InDetTrackSmearingTool* tool,
							    const CP::SystematicSet& syst) const {


    if(m_config->sgKeyTracks(syst.hash()) == "InDetTrackParticles_")
      return StatusCode::SUCCESS;

    // Create a shallow copy of InDetTrackParticles:                                                                                                                                                      
    const xAOD::TrackParticleContainer *tp(0);
    top::check(evtStore()->retrieve(tp, "InDetTrackParticles"), "Retrieve InDetParticles");

    std::pair< xAOD::TrackParticleContainer*, xAOD::ShallowAuxContainer* > shallow_xaod_copy = xAOD::shallowCopyContainer( *tp );

    ///-- Inform the tool --///
    top::check(tool->applySystematicVariation(syst), "Failed to configure tool for systematic variation");


    //std::cout << "In apply smearing systematic = " << m_config->sgKeyTracks(syst.hash()) << std::endl;

    for ( xAOD::TrackParticle* trkCpy : *(shallow_xaod_copy.first) ) { 

      //std::cout << "Track pt and d0 before " << trkCpy->pt() << "\t" << trkCpy->d0() << "\t" << trkCpy->z0() << std::endl;

      top::check(tool->applyCorrection(*trkCpy), "Failure to apply track systematic");
    
      //std::cout << "Track pt and d0 after " << trkCpy->pt() << "\t" << trkCpy->d0() << "\t" << trkCpy->z0() << std::endl;

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
    const xAOD::TrackParticleContainer *tp(0);
    top::check(evtStore()->retrieve(tp, "InDetTrackParticles"), "Retrieve InDetParticles");


    //std::cout << "In apply biasing systematic = " << m_config->sgKeyTracks(syst.hash()) << std::endl;

    std::pair< xAOD::TrackParticleContainer*, xAOD::ShallowAuxContainer* > shallow_xaod_copy = xAOD::shallowCopyContainer( *tp );

    ///-- Inform the tool --///                                                                                                                                                                          
    top::check(tool->applySystematicVariation(syst),
               "Failed to configure tool for systematic variation");

    for ( xAOD::TrackParticle* trkCpy : *(shallow_xaod_copy.first) ) {
    
      //std::cout << "Track pt and d0 before " << trkCpy->pt() << "\t" << trkCpy->d0() << "\t" << trkCpy->z0() << std::endl;

      top::check(tool->applyCorrection(*trkCpy), "Failure to apply track systematic");

      //std::cout << "Track pt and d0 after " << trkCpy->pt() << "\t" << trkCpy->d0() << "\t" << trkCpy->z0() << std::endl;

    }

    ///-- Save corrected xAOD Container to StoreGate / TStore --///                                                                                                                                       
    std::string outputSGKey = m_config->sgKeyTracks(syst.hash());
    std::string outputSGKeyAux = outputSGKey + "Aux.";

    xAOD::TReturnCode save = evtStore()->tds()->record(shallow_xaod_copy.first, outputSGKey);
    xAOD::TReturnCode saveAux = evtStore()->tds()->record(shallow_xaod_copy.second, outputSGKeyAux);

    if (!save || !saveAux) {
      return StatusCode::FAILURE;
    }


    /*
    ///-- Inform the tool --///
    top::check(tool->applySystematicVariation(syst),
               "Failed to configure tool for systematic variation");

    const std::string sgKey {
      //     m_config->decoKeyJetTrack() +
      "InDetTrackParticles_" + syst.name()
    };
    const std::string sgKeyAux {
      sgKey + "Aux."
    };


    ///-- Loop over the xAOD Container --///
    xAOD::TrackParticleContainer* newTrackParticles = new xAOD::TrackParticleContainer;
    top::check(evtStore()->record(newTrackParticles, sgKey),
               "Failed to record systematically varied track particle container");
    xAOD::TrackParticleAuxContainer* newTrackParticlesAux = new xAOD::TrackParticleAuxContainer;
    top::check(evtStore()->record(newTrackParticlesAux, sgKeyAux),
               "Failed to record systematically varied track particle container");
    newTrackParticles->setStore(newTrackParticlesAux);

    top::check(tool->applyContainerCorrection(*newTrackParticles),
	       "Failure to apply track systematic");

    */
    /*
    for (const auto& jet : *nominal) {
      if ((jet->pt() < m_jetPtCut) || (fabs(jet->eta()) > m_jetEtaCut)) continue;
      const auto& tracks = jet->getAssociatedObjects<xAOD::TrackParticle>();

      std::vector<const xAOD::IParticle*> news;

      for (std::size_t i = 0; i < tracks.size(); ++i) {
        if (tracks[i] == nullptr) continue;

        const xAOD::TrackParticle*
          tp {
          dynamic_cast<const xAOD::TrackParticle*>(tracks.at(i))
        };
        top::check(tp, "Failed to convert xAOD::IParticle to xAOD::TrackParticle for ghost track");

        xAOD::TrackParticle* newTp {
          nullptr
        };
        top::check(tool->correctedCopy(*tp, newTp),
                   "Failure to apply ghost track systematic");
        top::check(newTp, "Systematically varied xAOD::TrackParticle is nullptr");

        newTrackParticles->push_back(newTp);
        news.push_back(newTp);
      }

      //      jet->setAssociatedObjects(m_config->decoKeyJetTrack(syst.hash()), news);
    }

    */

    return StatusCode::SUCCESS;
  }

  StatusCode TrackSystematicsMaker::execute(bool executeNominal) {
    ATH_MSG_DEBUG(" top::TrackSystematicsMaker execute:");


    ///-- Get nominal jets --///
    // xAOD::TrackParticleContainer* nominalJets(nullptr);
    //    top::check(evtStore()->retrieve(nominalJets,
    //                              m_config->sgKeyJetsStandAlone(m_nominalSystematicSet.hash())),
    //           "Failed to retrieve Jets");

    // applyNoOpSystematic is used just to remove ghost track vector from thinned jets
    // top::check(applyNoOpSystematic(nominalJets, m_nominalSystematicSet),
    //           "Failure to apply TrackSystematic");

    //    //std::cout << "execute 1" << std::endl;

    //std::cout << m_nominalSystematicSet.hash() << std::endl;
  
    //    //std::cout << m_config->sgKeyTracks() << std::endl;

    ///-- Get nominal jets --///                                                                                                                                                                        
    //   const xAOD::TrackParticleContainer* nominalTracks(0);                                                                                                                                                 

    //    //std::cout << "execute 1 b" << std::endl;

    //const xAOD::TrackParticleContainer* tp(0);
    // top::check(evtStore()->retrieve(tp, "InDetTrackParticles"), "Retrieve InDetParticles");

    //    top::check(evtStore()->retrieve(nominalTracks, m_config -> sgKeyTracks()), "Failed to retrieve Tracks"); 

    //    //std::cout << "execute 2" <<std::endl;

    // We don't want to do anything on Data -> bail early so that we can
    // rely on the inputs to be MC.
    if (not m_config->isMC()) {

      // Create a shallow copy of InDetTrackParticles:                                                                                                                                                        
      const xAOD::TrackParticleContainer *tp(0);
      top::check(evtStore()->retrieve(tp, "InDetTrackParticles"), "Retrieve InDetParticles");

      std::pair< xAOD::TrackParticleContainer*, xAOD::ShallowAuxContainer* > shallow_xaod_copy = xAOD::shallowCopyContainer( *tp );

      ///-- Save corrected xAOD Container to StoreGate / TStore --///                                                                                                                                         
      std::string outputSGKey = m_config->sgKeyTracks(m_nominalSystematicSet.hash());
      std::string outputSGKeyAux = outputSGKey + "Aux.";

      //      //std::cout << outputSGKey.c_str() << std::endl;

      xAOD::TReturnCode save = evtStore()->tds()->record(shallow_xaod_copy.first, outputSGKey);
      xAOD::TReturnCode saveAux = evtStore()->tds()->record(shallow_xaod_copy.second, outputSGKeyAux);

      if (!save || !saveAux) {
	return StatusCode::FAILURE;
      }


      return StatusCode::SUCCESS;
    }

    //    //std::cout << "execute 7" << std::endl;

    ///-- Only run this on the systematic execution --///
    if (executeNominal){

      // Create a shallow copy of InDetTrackParticles:                                                                                                                                                        
      const xAOD::TrackParticleContainer *tp(0);
      top::check(evtStore()->retrieve(tp, "InDetTrackParticles"), "Retrieve InDetParticles");

      std::pair< xAOD::TrackParticleContainer*, xAOD::ShallowAuxContainer* > shallow_xaod_copy = xAOD::shallowCopyContainer( *tp );

      ///-- Save corrected xAOD Container to StoreGate / TStore --///                                                                                                                                         
      std::string outputSGKey = m_config->sgKeyTracks(m_nominalSystematicSet.hash());
      std::string outputSGKeyAux = outputSGKey + "Aux.";

      // //std::cout << outputSGKey.c_str() << std::endl;

      xAOD::TReturnCode save = evtStore()->tds()->record(shallow_xaod_copy.first, outputSGKey);
      xAOD::TReturnCode saveAux = evtStore()->tds()->record(shallow_xaod_copy.second, outputSGKeyAux);

      if (!save || !saveAux) {
	return StatusCode::FAILURE;
      }


      return StatusCode::SUCCESS;

    }

    ////std::cout << "execute 8" << std::endl;

    ///-- SMEARING --///
    for (const auto& syst : m_systs.smearing) {
      top::check(applySmearingSystematic(&(*m_tools.smearing), syst),
		 "Failure to apply TrackSystematic");
    }

    ////std::cout << "execute 9" << std::endl;

    ///-- BIASING --///
    InDet::InDetTrackBiasingTool* biasingTool {
      nullptr
    };
    if (m_runPeriods.size() == 1) {
      biasingTool = &(*m_tools.bias[0]);

      top::check(biasingTool, "Failure to selected biasing tool");
      for (const auto& syst : m_systs.bias) {
	top::check(applyBiasingSystematic(biasingTool, syst),
                   "Failure to apply TrackSystematic");
      }
    } else {
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
            biasingTool = &(*m_tools.bias[i - 1]);
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
    //    for (const auto& syst : m_systs.truthFilter) {
      // top::check(applyTruthFilterSystematic(nominalJets, &(*m_tools.truthFilter), syst),
      //          "Failure to apply TrackSystematic");
    // }

    ///-- JET TRACK FILTER --///
    /*    for (const auto& syst : m_systs.jetTrackFilter) {
      top::check(applyJetTrackFilterSystematic(nominalJets, &(*m_tools.jetTrackFilter), syst),
                 "Failure to apply TrackSystematic");
		 }*/

    ATH_MSG_DEBUG(" top::TrackSystematicsMaker completed execute");
    return StatusCode::SUCCESS;
  }

  void TrackSystematicsMaker::specifiedSystematics(const std::set<std::string>& specSys) {
    // NOTE: For this function -- unlike the "proper" ObjectCollectionMakers in AnalysisTop -- an empty
    // specifiedSystematics input is considered to represent "No Systematics". All Track systematics can be
    // requested with the string "AllTrack".
    const bool allTrackSystematics = m_config->contains(specSys, "AllTracks");


    ////std::cout << "bla 1 " << std::endl;

    // Acquire the recommended systematics from the various tools and put
    // into containers that store the per-tool systematics as well as a
    // global container. At a later point we'll make the per-tool containers
    // unique and check that the containers for various tools are
    // orthogonal.
    std::set<CP::SystematicSet> systs;

    ////std::cout << "bla 2 " << std::endl;

    // SMEARING
    m_systs.smearing = CP::make_systematics_vector(m_tools.smearing->recommendedSystematics());
    systs.insert(m_systs.smearing.begin(), m_systs.smearing.end());

    ////std::cout << "bla 3 " << std::endl;

    // BIAS
    for (const auto& tool : m_tools.bias) {
      m_systs.bias =
        CP::make_systematics_vector(tool->recommendedSystematics());
      systs.insert(m_systs.bias.begin(), m_systs.bias.end());
    }

    ////std::cout << "bla 4 " << std::endl;

    // TRUTH FILTER
    //m_systs.truthFilter =
    //  CP::make_systematics_vector(m_tools.truthFilter->recommendedSystematics());
    //systs.insert(m_systs.truthFilter.begin(), m_systs.truthFilter.end());

    // JET TRACK FILTER
    //m_systs.jetTrackFilter =
    //  CP::make_systematics_vector(m_tools.jetTrackFilter->recommendedSystematics());
    //systs.insert(m_systs.jetTrackFilter.begin(), m_systs.jetTrackFilter.end());

    // PUT INTO RECOMMENDED AND SPECIFIED
    for (const auto& s : systs) {

      //std::cout << "PUT SYSTEMATICS IN LIST ======================================================> " <<  s.name() <<  "==================" << std::endl;

      // We don't want nominal.
      if (s.name() == "") {
        continue;
      }

      //std::cout << "after" << std::endl;
      
      //      if(!m_config->getTreeFilter()->filterTree(s.name())) continue; // Applying tree filter
      m_recommendedSystematics.push_back(s);

      ///-- MC only --///
      if (m_config->isMC()) {
        ///-- Are we only doing Nominal? Did the user specify specific systematics to use? --///
        if (not m_config->isSystNominal(m_config->systematics())) {
          if (allTrackSystematics) {

	    //std::cout << "Put into specified systematics list " << std::endl;

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

    //std::cout << "bla 5 " << std::endl;

    m_recommendedSystematics.sort();
    m_recommendedSystematics.unique();

    m_specifiedSystematics.sort();
    m_specifiedSystematics.unique();

    //std::cout << "bla 6 " << std::endl;

    // Make the sublists unique and remove any elements that have not "been specified".
    m_systs.makeUnique();
    m_systs.removeNonSpecified(m_specifiedSystematics);

    //    for (const auto& s : m_systs) {

      //      //std::cout << "FINAL LIST =============================================================================> " << s.name() << std::endl;

    //    }

    //std::cout << "bla 7 " << std::endl;

    // Lastly, let use introduce a safeguard which ensures that the
    // systematics selected for the various tools are orthogonal. This is
    // done by asserting that the sum of the sizes of the individual
    // systematics lists equals the size of the list of specified
    // systematics. We can do this because we've made the individual
    // per-tool containers unique (in makeUnique).
    top::check(systs.size() == m_specifiedSystematics.size(),
               "Sanity check failed");


    //std::cout << "bla 8 " << std::endl;

  }

  StatusCode TrackSystematicsMaker::retrieveTrackCPTool() {
    ATH_MSG_INFO(" Retrieving track CPTools");

    std::string smearingToolName {
      "top::TrackCPTools::InDetTrackSmearingTool"
    };

    //std::cout << "retrieve Track CP 1 " << std::endl;

    if (asg::ToolStore::contains<InDet::InDetTrackSmearingTool>(smearingToolName)) {
      m_tools.smearing =
        asg::ToolStore::get<InDet::InDetTrackSmearingTool>(smearingToolName);
    } else {
      ATH_MSG_ERROR(" Impossible to retrieve " + smearingToolName);
      return StatusCode::FAILURE;
    }

    //std::cout << "retrieve Track CP 2 " << std::endl;

    //std::cout << m_runPeriods.size() << std::endl;
    
    std::string biasToolPrefix {
      "top::TrackCPTools::InDetTrackBiasingTool"
    };
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
    } else {
      end = m_runPeriods.size() - 1;
      m_tools.bias.resize(m_runPeriods.size() - 1);
    }

    //std::cout << "retrieve Track CP 3 " << std::endl;

    for (unsigned int i = 0; i < end; i++) {
      std::string biasToolName {
        ""
      };
      if (m_runPeriods.size() == 1) {
        biasToolName = biasToolPrefix + "_" + std::to_string(m_runPeriods[0]);
      } else {
        biasToolName = biasToolPrefix + "_" + std::to_string(m_runPeriods[i]) + "_" +
                       std::to_string(m_runPeriods[i + 1]);
      }
      if (asg::ToolStore::contains<InDet::InDetTrackBiasingTool>(biasToolName)) {
        m_tools.bias[i] = asg::ToolStore::get<InDet::InDetTrackBiasingTool>(biasToolName);
      } else {
        ATH_MSG_ERROR(" Impossible to retrieve " + biasToolName);
        return StatusCode::FAILURE;
      }
    }

    //std::cout << "retrieve Track CP 4 " << std::endl;

    std::string truthOriginToolName {
      "top::TrackCPTools::InDetTrackTruthOriginTool"
    };
    if (asg::ToolStore::contains<InDet::InDetTrackTruthOriginTool>(truthOriginToolName)) {
      m_tools.truthOrigin = asg::ToolStore::get<InDet::InDetTrackTruthOriginTool>(truthOriginToolName);
    } else {
      ATH_MSG_ERROR(" Impossible to retrieve " + truthOriginToolName);
      return StatusCode::FAILURE;
    }

    //std::cout << "retrieve Track CP 5 " << std::endl;

    std::string truthFilterToolName {
      "top::TrackCPTools::InDetTrackTruthFilterTool"
    };
    if (asg::ToolStore::contains<InDet::InDetTrackTruthFilterTool>(truthFilterToolName)) {
      m_tools.truthFilter = asg::ToolStore::get<InDet::InDetTrackTruthFilterTool>(truthFilterToolName);
    } else {
      ATH_MSG_ERROR(" Impossible to retrieve " + truthFilterToolName);
      return StatusCode::FAILURE;
    }

    //std::cout << "retrieve Track CP 6 " << std::endl;

    /*
    std::string jetTrackFilterToolName {
      "top::TrackCPTools::JetTrackFilterTool"
    };
    if (asg::ToolStore::contains<InDet::JetTrackFilterTool>(jetTrackFilterToolName)) {
      m_tools.jetTrackFilter = asg::ToolStore::get<InDet::JetTrackFilterTool>(jetTrackFilterToolName);
    } else {
      ATH_MSG_ERROR(" Impossible to retrieve " + jetTrackFilterToolName);
      return StatusCode::FAILURE;
    }
    */

    return StatusCode::SUCCESS;
  }
}

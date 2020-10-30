/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

// $Id: TopEventMaker.cxx 809847 2017-08-29 15:18:19Z iconnell $
#include "TopEvent/TopEventMaker.h"
#include "TopEvent/EventTools.h"

#include "TopConfiguration/TopConfig.h"

#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "FourMomUtils/xAODP4Helpers.h"

#include "TopPartons/PartonHistory.h"

#include <boost/algorithm/string.hpp>

#include <iostream>

namespace top {
  bool TopEventMaker::s_hasTruthEvent = true;

  TopEventMaker::TopEventMaker(const std::string& name) :
    asg::AsgTool(name),
    m_config(nullptr) {
    declareProperty("config", m_config);
  }

  StatusCode TopEventMaker::initialize() {
    if (m_config->useRCJets() == true) {
      m_rc = std::unique_ptr<RCJetMC15> (new RCJetMC15("RCJetMC15"));
      top::check(m_rc->setProperty("config", m_config), "Failed to set config property of RCJetMC15");
      top::check(m_rc->initialize(), "Failed to initialize RCJetMC15");
    }

    if (m_config->useVarRCJets() == true) {
      boost::split(m_VarRCJetRho, m_config->VarRCJetRho(), boost::is_any_of(","));
      boost::split(m_VarRCJetMassScale, m_config->VarRCJetMassScale(), boost::is_any_of(","));

      for (auto& rho : m_VarRCJetRho) {
        for (auto& mass_scale : m_VarRCJetMassScale) {
          std::replace(rho.begin(), rho.end(), '.', '_');
          std::string name = rho + mass_scale;
          m_VarRC[name] = std::unique_ptr<RCJetMC15> (new RCJetMC15("VarRCJetMC15_" + name));
          top::check(m_VarRC[name]->setProperty("config", m_config), "Failed to set config property of VarRCJetMC15");
          top::check(m_VarRC[name]->setProperty("VarRCjets", true), "Failed to set VarRCjets property of VarRCJetMC15");
          top::check(m_VarRC[name]->setProperty("VarRCjets_rho",
                                                rho), "Failed to set VarRCjets rho property of VarRCJetMC15");
          top::check(m_VarRC[name]->setProperty("VarRCjets_mass_scale",
                                                mass_scale),
                     "Failed to set VarRCjets mass scale property of VarRCJetMC15");
          top::check(m_VarRC[name]->initialize(), "Failed to initialize VarRCJetMC15");
        } // end loop over mass scale parameters (e.g., top mass, w mass, etc.)
      } // end loop over mass scale multiplies (e.g., 1.,2.,etc.)
    }

    return StatusCode::SUCCESS;
  }

  /// As top-xaod isn't an asg::AsgTool, it doesn't have access to all the information
  /// Very annoying, as it's actually quite simple
  const xAOD::SystematicEventContainer* TopEventMaker::systematicEvents(const std::string& sgKey) const {
    const xAOD::SystematicEventContainer* systEvents(nullptr);

    top::check(evtStore()->retrieve(systEvents, sgKey), "Failed to get xAOD::SystematicEventContainer");
    return systEvents;
  }

  top::Event TopEventMaker::makeTopEvent(const xAOD::SystematicEvent* currentSystematicPtr) {
    xAOD::SystematicEvent const& currentSystematic = *currentSystematicPtr;
    //create a new event object
    top::Event event;

    // Set systematic hash value
    std::size_t hash = currentSystematic.hashValue();
    event.m_hashValue = hash;

    // TTree index
    event.m_ttreeIndex = currentSystematic.ttreeIndex();

    // Is Loose event?
    event.m_isLoose = currentSystematic.isLooseEvent();

    //event info
    top::check(evtStore()->retrieve(event.m_info, m_config->sgKeyEventInfo()), "Failed to retrieve EventInfo");
    event.m_systematicEvent = currentSystematicPtr;

    //Primary Vertices
    if (evtStore()->contains<xAOD::VertexContainer>(m_config->sgKeyPrimaryVertices())) {
      top::check(evtStore()->retrieve(event.m_primaryVertices,
                                      m_config->sgKeyPrimaryVertices()), "Failed to retrieve Primary Vertices");
    }

    //Poisson bootstrap weights
    if (m_config->saveBootstrapWeights()) {
      int second_seed;
      if (m_config->isMC()) {
        second_seed = event.m_info->mcChannelNumber();
      } else {
        second_seed = event.m_info->runNumber();
      }
      std::vector<int> weight_poisson = top::calculateBootstrapWeights(m_config->getNumberOfBootstrapReplicas(),
                                                                       event.m_info->eventNumber(),
                                                                       second_seed);
      event.m_info->auxdecor< std::vector<int> >("weight_poisson") = weight_poisson;
    }

    //electrons
    if (m_config->useElectrons()) {
      ///-- Need to read const collections for mini-xaod read back --///

      const xAOD::ElectronContainer* calibratedElectrons(nullptr);
      top::check(evtStore()->retrieve(calibratedElectrons, m_config->sgKeyElectrons(
                                        hash)), "Failed to retrieve electrons");

      ///-- Shallow copy and save to TStore --///
      if (!evtStore()->contains<xAOD::ElectronContainer>(m_config->sgKeyElectronsTDS(hash))) {
        std::pair< xAOD::ElectronContainer*,
                   xAOD::ShallowAuxContainer* > shallow_electrons = xAOD::shallowCopyContainer(*calibratedElectrons);

        xAOD::TReturnCode save = evtStore()->tds()->record(shallow_electrons.first, m_config->sgKeyElectronsTDS(hash));
        xAOD::TReturnCode saveAux =
          evtStore()->tds()->record(shallow_electrons.second, m_config->sgKeyElectronsTDSAux(hash));
        top::check((save && saveAux), "Failed to store object in TStore");
      }

      ///-- Pull shallow copy back out of TStore in non-const way --///
      xAOD::ElectronContainer* calibratedElectronsTDS(nullptr);
      top::check(evtStore()->retrieve(calibratedElectronsTDS, m_config->sgKeyElectronsTDS(
                                        hash)), "Failed to retrieve electrons");


      // re-write overlap information if it is decorated onto currentSystematic
      if (currentSystematic.isAvailable< std::vector<unsigned int> >("overlapsEl")) {
        std::vector<unsigned int> overlapsEl =
          currentSystematic.auxdataConst< std::vector<unsigned int> >("overlapsEl");
        if (overlapsEl.size() == calibratedElectronsTDS->size()) {
          unsigned int counter(0);
          for (auto x : *calibratedElectronsTDS) {
            x->auxdecor<char>("overlaps") = overlapsEl.at(counter);
            ++counter;
          }
        }
      }

      for (auto index : currentSystematic.goodElectrons()) {
        event.m_electrons.push_back(calibratedElectronsTDS->at(index));
      }

      //shallow copies aren't sorted!
      //sort only the selected electrons (faster)
      event.m_electrons.sort(top::descendingPtSorter);
    }

    //forward electrons
    if (m_config->useFwdElectrons()) {
      const xAOD::ElectronContainer* calibratedFwdElectrons(nullptr);
      top::check(evtStore()->retrieve(calibratedFwdElectrons, m_config->sgKeyFwdElectrons(
                                        hash)), "Failed to retrieve fwd electrons");

      ///-- Shallow copy and save to TStore --///
      if (!evtStore()->contains<xAOD::ElectronContainer>(m_config->sgKeyFwdElectronsTDS(hash))) {
        std::pair< xAOD::ElectronContainer*,
                   xAOD::ShallowAuxContainer* > shallow_fwdelectrons = xAOD::shallowCopyContainer(
          *calibratedFwdElectrons);

        xAOD::TReturnCode save =
          evtStore()->tds()->record(shallow_fwdelectrons.first, m_config->sgKeyFwdElectronsTDS(hash));
        xAOD::TReturnCode saveAux = evtStore()->tds()->record(shallow_fwdelectrons.second, m_config->sgKeyFwdElectronsTDSAux(
                                                                hash));
        top::check((save && saveAux), "Failed to store object in TStore");
      }

      ///-- Pull shallow copy back out of TStore in non-const way --///
      xAOD::ElectronContainer* calibratedFwdElectronsTDS(nullptr);
      top::check(evtStore()->retrieve(calibratedFwdElectronsTDS, m_config->sgKeyFwdElectronsTDS(
                                        hash)), "Failed to retrieve fwd electrons");

      for (const auto& index : currentSystematic.goodFwdElectrons()) {
        event.m_fwdElectrons.push_back(calibratedFwdElectronsTDS->at(index));
      }

      //shallow copies aren't sorted!
      //sort only the selected fwd electrons (faster)
      event.m_fwdElectrons.sort(top::descendingPtSorter);
    }

    //photons
    if (m_config->usePhotons()) {
      ///-- Need to read const collections for mini-xaod read back --///

      const xAOD::PhotonContainer* calibratedPhotons(nullptr);
      top::check(evtStore()->retrieve(calibratedPhotons, m_config->sgKeyPhotons(hash)), "Failed to retrieve photons");

      ///-- Shallow copy and save to TStore --///
      if (!evtStore()->contains<xAOD::PhotonContainer>(m_config->sgKeyPhotonsTDS(hash))) {
        std::pair< xAOD::PhotonContainer*, xAOD::ShallowAuxContainer* > shallow_photons = xAOD::shallowCopyContainer(
          *calibratedPhotons);

        xAOD::TReturnCode save = evtStore()->tds()->record(shallow_photons.first, m_config->sgKeyPhotonsTDS(hash));
        xAOD::TReturnCode saveAux =
          evtStore()->tds()->record(shallow_photons.second, m_config->sgKeyPhotonsTDSAux(hash));
        top::check((save && saveAux), "Failed to store object in TStore");
      }

      ///-- Pull shallow copy back out of TStore in non-const way --///
      xAOD::PhotonContainer* calibratedPhotonsTDS(nullptr);
      top::check(evtStore()->retrieve(calibratedPhotonsTDS, m_config->sgKeyPhotonsTDS(
                                        hash)), "Failed to retrieve photons");

      for (auto index : currentSystematic.goodPhotons()) {
        event.m_photons.push_back(calibratedPhotonsTDS->at(index));
      }
      //shallow copies aren't sorted!
      //sort only the selected photons (faster)
      event.m_photons.sort(top::descendingPtSorter);
    }

    //muons
    if (m_config->useMuons()) {
      ///-- Need to read const collections for mini-xaod read back --///
      const xAOD::MuonContainer* calibratedMuons(nullptr);
      top::check(evtStore()->retrieve(calibratedMuons, m_config->sgKeyMuons(hash)), "Failed to retrieve muons");

      ///-- Shallow copy and save to TStore --///
      if (!evtStore()->contains<xAOD::MuonContainer>(m_config->sgKeyMuonsTDS(hash))) {
        std::pair< xAOD::MuonContainer*, xAOD::ShallowAuxContainer* > shallow_muons = xAOD::shallowCopyContainer(
          *calibratedMuons);

        xAOD::TReturnCode save = evtStore()->tds()->record(shallow_muons.first, m_config->sgKeyMuonsTDS(hash));
        xAOD::TReturnCode saveAux = evtStore()->tds()->record(shallow_muons.second, m_config->sgKeyMuonsTDSAux(hash));
        top::check((save && saveAux), "Failed to store object in TStore");
      }

      ///-- Pull shallow copy back out of TStore in non-const way --///
      xAOD::MuonContainer* calibratedMuonsTDS(nullptr);
      top::check(evtStore()->retrieve(calibratedMuonsTDS, m_config->sgKeyMuonsTDS(hash)), "Failed to retrieve muons");

      // re-write overlap information if it is decorated onto currentSystematic
      if (currentSystematic.isAvailable< std::vector<unsigned int> >("overlapsMu")) {
        std::vector<unsigned int> overlapsMu =
          currentSystematic.auxdataConst< std::vector<unsigned int> >("overlapsMu");
        if (overlapsMu.size() == calibratedMuonsTDS->size()) {
          unsigned int counter(0);
          for (auto x : *calibratedMuons) {
            x->auxdecor<char>("overlaps") = overlapsMu.at(counter);
            ++counter;
          }
        }
      }

      for (auto index : currentSystematic.goodMuons()) {
        event.m_muons.push_back(calibratedMuonsTDS->at(index));
      }

      //shallow copies aren't sorted!
      //sort only the selected muons (faster)
      event.m_muons.sort(top::descendingPtSorter);
    }

    //soft muons
    if (m_config->useSoftMuons()) {
      ///-- Need to read const collections for mini-xaod read back --///
      const xAOD::MuonContainer* calibratedSoftMuons(nullptr);

      top::check(evtStore()->retrieve(calibratedSoftMuons, m_config->sgKeySoftMuons(hash)), "Failed to retrieve muons");


      ///-- Shallow copy and save to TStore --///
      if (!evtStore()->contains<xAOD::MuonContainer>(m_config->sgKeySoftMuonsTDS(hash))) {
        std::pair< xAOD::MuonContainer*, xAOD::ShallowAuxContainer* > shallow_softmuons = xAOD::shallowCopyContainer(
          *calibratedSoftMuons);

        xAOD::TReturnCode save = evtStore()->tds()->record(shallow_softmuons.first, m_config->sgKeySoftMuonsTDS(hash));
        xAOD::TReturnCode saveAux =
          evtStore()->tds()->record(shallow_softmuons.second, m_config->sgKeySoftMuonsTDSAux(hash));
        top::check((save && saveAux), "Failed to store object in TStore");
      }

      ///-- Pull shallow copy back out of TStore in non-const way --///
      xAOD::MuonContainer* calibratedSoftMuonsTDS(nullptr);
      top::check(evtStore()->retrieve(calibratedSoftMuonsTDS, m_config->sgKeySoftMuonsTDS(
                                        hash)), "Failed to retrieve soft muons");

      //no overlap procedure applied to soft muons for the time being

      for (auto index : currentSystematic.goodSoftMuons()) {
        event.m_softmuons.push_back(calibratedSoftMuonsTDS->at(index));
      }

      //shallow copies aren't sorted!
      //sort only the selected soft muons (faster)
      event.m_softmuons.sort(top::descendingPtSorter);
    }

    //taus
    if (m_config->useTaus()) {
      ///-- Need to read const collections for mini-xaod read back --///
      const xAOD::TauJetContainer* calibratedTaus(nullptr);
      top::check(evtStore()->retrieve(calibratedTaus, m_config->sgKeyTaus(hash)), "Failed to retrieve taus");

      ///-- Shallow copy and save to TStore --///
      if (!evtStore()->contains<xAOD::TauJetContainer>(m_config->sgKeyTausTDS(hash))) {
        std::pair< xAOD::TauJetContainer*, xAOD::ShallowAuxContainer* > shallow_taus = xAOD::shallowCopyContainer(
          *calibratedTaus);

        xAOD::TReturnCode save = evtStore()->tds()->record(shallow_taus.first, m_config->sgKeyTausTDS(hash));
        xAOD::TReturnCode saveAux = evtStore()->tds()->record(shallow_taus.second, m_config->sgKeyTausTDSAux(hash));
        top::check((save && saveAux), "Failed to store object in TStore");
      }

      ///-- Pull shallow copy back out of TStore in non-const way --///
      xAOD::TauJetContainer* calibratedTausTDS(nullptr);
      top::check(evtStore()->retrieve(calibratedTausTDS, m_config->sgKeyTausTDS(hash)), "Failed to retrieve taus");

      for (auto index : currentSystematic.goodTaus()) {
        event.m_tauJets.push_back(calibratedTausTDS->at(index));
      }

      //shallow copies aren't sorted!
      //sort only the selected taus (faster)
      event.m_tauJets.sort(top::descendingPtSorter);
    }

    //jets
    // We need to be careful with the boosted analysis,
    // Do we take the jets collection that has loose or tight leptons subtracted?
    bool looseJets(false);
    if (m_config->applyElectronInJetSubtraction()) {
      if (m_config->doOverlapRemovalOnLooseLeptonDef() || currentSystematic.isLooseEvent()) {
        looseJets = true;
      }
    }

    if (m_config->useJets()) {
      ///-- Need to read const collections for mini-xaod read back --///
      std::string sgKeyCalibrated(m_config->sgKeyJets(hash, looseJets));
      std::string sgKeyTmp = "tmp_" + sgKeyCalibrated;
      std::string sgKeyTmpAux = sgKeyTmp + "Aux.";

      const xAOD::JetContainer* calibratedJets(nullptr);
      top::check(evtStore()->retrieve(calibratedJets, m_config->sgKeyJets(hash, looseJets)), "Failed to retrieve jets");

      ///-- Shallow copy and save to TStore --///
      if (!evtStore()->contains<xAOD::JetContainer>(m_config->sgKeyJetsTDS(hash, looseJets))) {
        std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > shallow_jets = xAOD::shallowCopyContainer(
          *calibratedJets);

        xAOD::TReturnCode save = evtStore()->tds()->record(shallow_jets.first, m_config->sgKeyJetsTDS(hash, looseJets));
        xAOD::TReturnCode saveAux =
          evtStore()->tds()->record(shallow_jets.second, m_config->sgKeyJetsTDSAux(hash, looseJets));
        top::check((save && saveAux), "Failed to store object in TStore");
      }

      ///-- Pull shallow copy back out of TStore in non-const way --///
      xAOD::JetContainer* calibratedJetsTDS(nullptr);
      top::check(evtStore()->retrieve(calibratedJetsTDS, m_config->sgKeyJetsTDS(hash,
                                                                                looseJets)), "Failed to retrieve taus");


      for (auto index : currentSystematic.goodJets()) {
        auto jet = calibratedJetsTDS->at(index);

        if (m_config->isMC()) {
          // JVT and fJVT, make product of SF (initialised to 1 in the header)
          top::check(jet->isAvailable<float>(
                       "JET_SF_jvt"),
                     " Can't find jet decoration \"JET_SF_jvt\" - we need it to calculate the jet scale-factors!");
          event.m_jvtSF *= jet->auxdataConst<float>("JET_SF_jvt");
	  
	  // fJVT scale factors not added to jets unless fJVT is requested
	  if (m_config->getfJVTWP() != "None") {
	    top::check(jet->isAvailable<float>(
		        "JET_SF_fjvt"),
		       " Can't find jet decoration \"JET_SF_fjvt\" - we need it to calculate the forward jet scale-factors!");
	    event.m_fjvtSF *= jet->auxdataConst<float>("JET_SF_fjvt");
	  }
          if (currentSystematic.hashValue() == m_config->nominalHashValue()) {// we only need the up/down JVT SF systs
                                                                              // for nominal
            top::check(jet->isAvailable<float>(
                         "JET_SF_jvt_UP"),
                       " Can't find jet decoration \"JET_SF_jvt_UP\" - we need it to calculate the jet scale-factors!");
            event.m_jvtSF_UP *= jet->auxdataConst<float>("JET_SF_jvt_UP");
            top::check(jet->isAvailable<float>(
                         "JET_SF_jvt_DOWN"),
                       " Can't find jet decoration \"JET_SF_jvt_DOWN\" - we need it to calculate the jet scale-factors!");
            event.m_jvtSF_DOWN *= jet->auxdataConst<float>("JET_SF_jvt_DOWN");

	    // fJVT scale factors not added to jets unless fJVT is requested
	    if (m_config->getfJVTWP() != "None") {
	      top::check(jet->isAvailable<float>(
						 "JET_SF_fjvt_UP"),
			 " Can't find jet decoration \"JET_SF_fjvt_UP\" - we need it to calculate the forward jet scale-factors!");
	      event.m_fjvtSF_UP *= jet->auxdataConst<float>("JET_SF_fjvt_UP");
	      top::check(jet->isAvailable<float>(
						 "JET_SF_fjvt_DOWN"),
			 " Can't find jet decoration \"JET_SF_fjvt_DOWN\" - we need it to calculate the forward jet scale-factors!");
	      event.m_fjvtSF_DOWN *= jet->auxdataConst<float>("JET_SF_fjvt_DOWN");
	    }
          } //isNominal
        } //isMC

        top::check(jet->isAvailable<char>(
                     "passJVT"),
                   " Can't find jet decoration \"passJVT\" - we need it to decide if we should keep the jet in the top::Event instance or not!");
	bool passfJVT(true);
	if (m_config->doForwardJVTinMET() || m_config->getfJVTWP() != "None") {
	  top::check(jet->isAvailable<char>("AnalysisTop_fJVTdecision"),
		     " Can't find jet decoration \"AnalysisTop_fJVTdecision\" - we need it to decide if we should keep forward jets in the top::Event instance or not!");
	  passfJVT = jet->auxdataConst<char>("AnalysisTop_fJVTdecision");
	  if (m_config->saveFailForwardJVTJets()) {
	    if (!passfJVT) event.m_failFJvt_jets.push_back(calibratedJetsTDS->at(index));
	  }
	  //Add to failFJVT collection but don't actually cut on fJVT if fJVT is only requested in MET calculation (I'm not sure people will ever actually do this)
	  if (m_config->getfJVTWP() == "None") passfJVT = true;
	}

        if (jet->auxdataConst<char>("passJVT") && passfJVT) event.m_jets.push_back(calibratedJetsTDS->at(index));
        if (m_config->saveFailJVTJets()) {
          if (!jet->auxdataConst<char>("passJVT")) event.m_failJvt_jets.push_back(calibratedJetsTDS->at(index));
        }
      }

      //shallow copies aren't sorted!
      //sort only the selected taus (faster)
      event.m_jets.sort(top::descendingPtSorter);
      if (m_config->saveFailJVTJets()) event.m_failJvt_jets.sort(top::descendingPtSorter);
      if ( (m_config->doForwardJVTinMET() || m_config->getfJVTWP() != "None") && m_config->saveFailForwardJVTJets()){
	event.m_failFJvt_jets.sort(top::descendingPtSorter);
      }
    }

    // Reclustered jets
    if (m_config->useRCJets()) {
      top::check(m_rc->execute(event), "Failed to execute RCJetMC15 container");
      std::string rcJetContainerName = m_rc->rcjetContainerName(event.m_hashValue, event.m_isLoose);
      const xAOD::JetContainer* rc_jets(nullptr);
      top::check(evtStore()->retrieve(rc_jets, rcJetContainerName), "Failed to retrieve RC JetContainer");
      //Object selection
      for (auto rcjet : *rc_jets) {
        top::check(rcjet->isAvailable<bool>(
                     "PassedSelection"),
                   " Can't find jet decoration \"PassedSelection\" - we need it to decide if we should keep the reclustered jet in the top::Event instance or not!");
        if (rcjet->auxdataConst<bool>("PassedSelection")) event.m_RCJets.push_back((xAOD::Jet*) rcjet);
      }
    }
    // Variable-R reclustered jets
    if (m_config->useVarRCJets()) {
      for (auto& rho : m_VarRCJetRho) {
        for (auto& mass_scale : m_VarRCJetMassScale) {
          std::replace(rho.begin(), rho.end(), '.', '_');
          std::string name = rho + mass_scale;
          top::check(m_VarRC[name]->execute(event), "Failed to execute RCJetMC15 container");

          // Get the name of the container of re-clustered jets in TStore
          std::string varRCJetContainerName = m_VarRC[name]->rcjetContainerName(event.m_hashValue, event.m_isLoose);

          // -- Retrieve the re-clustered jets from TStore & save good re-clustered jets -- //
          const xAOD::JetContainer* vrc_jets(nullptr);
          top::check(evtStore()->retrieve(vrc_jets, varRCJetContainerName), "Failed to retrieve RC JetContainer");

          event.m_VarRCJets[name] = std::make_shared<xAOD::JetContainer>(SG::VIEW_ELEMENTS);
          for (auto vrcjet : *vrc_jets) {
            top::check(vrcjet->isAvailable<bool>(
                         "PassedSelection"),
                       " Can't find jet decoration \"PassedSelection\" - we need it to decide if we should keep the variable-R reclustered jet in the top::Event instance or not!");
            if (vrcjet->auxdataConst<bool>("PassedSelection")) event.m_VarRCJets[name]->push_back((xAOD::Jet*) vrcjet);
          }
        }
      }
    }


    //large-R jets
    if (m_config->useLargeRJets()) {
      ///-- Need to read const collections for mini-xaod read back --///
      const xAOD::JetContainer* calibratedJets(nullptr);
      top::check(evtStore()->retrieve(calibratedJets, m_config->sgKeyLargeRJets(hash)),
                 "Failed to retrieve largeR jets");

      ///-- Shallow copy and save to TStore --///
      if (!evtStore()->contains<xAOD::JetContainer>(m_config->sgKeyLargeRJetsTDS(hash))) {
        std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > shallow_jets = xAOD::shallowCopyContainer(
          *calibratedJets);

        xAOD::TReturnCode save = evtStore()->tds()->record(shallow_jets.first, m_config->sgKeyLargeRJetsTDS(hash));
        xAOD::TReturnCode saveAux =
          evtStore()->tds()->record(shallow_jets.second, m_config->sgKeyLargeRJetsTDSAux(hash));
        top::check((save && saveAux), "Failed to store object in TStore");
      }

      ///-- Pull shallow copy back out of TStore in non-const way --///
      xAOD::JetContainer* calibratedJetsTDS(nullptr);
      top::check(evtStore()->retrieve(calibratedJetsTDS, m_config->sgKeyLargeRJetsTDS(
                                        hash)), "Failed to retrieve largeR jets");

      for (auto index : currentSystematic.goodLargeRJets()) {
        event.m_largeJets.push_back(calibratedJetsTDS->at(index));
      }

      //shallow copies aren't sorted!
      //sort only the selected jets (faster)
      event.m_largeJets.sort(top::descendingPtSorter);
    }

    //track jets
    if (m_config->useTrackJets()) {
      ///-- Need to read const collections for mini-xaod read back --///
      const xAOD::JetContainer* calibratedJets(nullptr);
      top::check(evtStore()->retrieve(calibratedJets, m_config->sgKeyTrackJets(hash)), "Failed to retrieve track jets");

      ///-- Shallow copy and save to TStore --///
      if (!evtStore()->contains<xAOD::JetContainer>(m_config->sgKeyTrackJetsTDS(hash))) {
        std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > shallow_jets = xAOD::shallowCopyContainer(
          *calibratedJets);

        xAOD::TReturnCode save = evtStore()->tds()->record(shallow_jets.first, m_config->sgKeyTrackJetsTDS(hash));
        xAOD::TReturnCode saveAux =
          evtStore()->tds()->record(shallow_jets.second, m_config->sgKeyTrackJetsTDSAux(hash));
        top::check((save && saveAux), "Failed to store object in TStore");
      }

      ///-- Pull shallow copy back out of TStore in non-const way --///
      xAOD::JetContainer* calibratedJetsTDS(nullptr);
      top::check(evtStore()->retrieve(calibratedJetsTDS, m_config->sgKeyTrackJetsTDS(
                                        hash)), "Failed to retrieve track jets");

      for (auto index : currentSystematic.goodTrackJets()) {
        event.m_trackJets.push_back(calibratedJetsTDS->at(index));
      }

      //shallow copies aren't sorted!
      //sort only the selected jets (faster)
      event.m_trackJets.sort(top::descendingPtSorter);
    }

    if (m_config->useTracks()) {

      ///-- Need to read const collections for mini-xaod read back --///                                                                                                                                 
      const xAOD::TrackParticleContainer* calibratedTracks(nullptr);
      top::check(evtStore()->retrieve(calibratedTracks, m_config->sgKeyTracks(hash)), "Failed to retrieve tracks");

      ///-- Shallow copy and save to TStore --///                                                                                                                                                         
      if (!evtStore()->contains<xAOD::TrackParticleContainer>(m_config->sgKeyTracksTDS(hash))) {

	std::pair< xAOD::TrackParticleContainer*, xAOD::ShallowAuxContainer* > shallow_tracks = xAOD::shallowCopyContainer(*calibratedTracks);

	xAOD::TReturnCode save = evtStore()->tds()->record(shallow_tracks.first, m_config->sgKeyTracksTDS(hash));
	xAOD::TReturnCode saveAux =
          evtStore()->tds()->record(shallow_tracks.second, m_config->sgKeyTracksTDSAux(hash));
	top::check((save && saveAux), "Failed to store object in TStore");
      }

      ///-- Pull shallow copy back out of TStore in non-const way --/// 
      xAOD::TrackParticleContainer* calibratedTracksTDS(nullptr);
      top::check(evtStore()->retrieve(calibratedTracksTDS, m_config->sgKeyTracksTDS(hash)), "Failed to retrieve tracks");

      for (auto index : currentSystematic.goodTracks()) {
        event.m_tracks.push_back(calibratedTracksTDS->at(index));
      }

      //shallow copies aren't sorted!                                                                                                                                                                     
      //sort only the selected tracks (faster)     
      event.m_tracks.sort(top::descendingPtSorter);

    }


    //met
    const xAOD::MissingETContainer* mets(nullptr);
    if (!currentSystematic.isLooseEvent()) {
      top::check(evtStore()->retrieve(mets, m_config->sgKeyMissingEt(hash)), "Failed to retrieve MET");
    }
    if (currentSystematic.isLooseEvent()) {
      top::check(evtStore()->retrieve(mets, m_config->sgKeyMissingEtLoose(hash)), "Failed to retrieve MET");
    }
    event.m_met = (*mets)["FinalTrk"];  // MissingETBase::Source::total()];

    //MC
    if (m_config->isMC()) {
      // Truth particles (these are BIG)
      if (m_config->useTruthParticles()) {
        top::check(evtStore()->retrieve(event.m_truth,
                                        m_config->sgKeyMCParticle()), "Failed to retrieve truth particles");
      }

      // Truth Event
      if (TopEventMaker::s_hasTruthEvent) {
        bool hasTruthEvent = evtStore()->retrieve(event.m_truthEvent, m_config->sgKeyTruthEvent());
        if (!hasTruthEvent) {
          ATH_MSG_WARNING("Failed to retrieve truth Event. TopEvent::m_truthEvent will be nullptr!");
          TopEventMaker::s_hasTruthEvent = false;
        }
      }

      // Parton History
      if (m_config->doTopPartonHistory()) {
        if (evtStore()->contains<xAOD::PartonHistoryContainer>(m_config->sgKeyTopPartonHistory())) {
          const xAOD::PartonHistoryContainer* partonHistory(nullptr);
          top::check(evtStore()->retrieve(partonHistory,
                                          m_config->sgKeyTopPartonHistory()), "Failed to retrieve Top Parton History");
          if (partonHistory->size() == 1) {
            event.m_partonHistory = partonHistory->at(0);
          }
        }
      } // end doTopPartonHistory
    } // end isMC

    decorateTopEvent(event);
    
    return event;
  }
  
  void TopEventMaker::decorateTopEvent(top::Event &event)
  {
    if(m_config->useSoftMuons()) decorateTopEventSoftMuons(event);
  }
  
  void TopEventMaker::decorateTopEventSoftMuons(top::Event &event)
  {
    if(!m_config->useJets()) return;
    
    //first we initialize decorations for all jets
    for(const xAOD::Jet* jet : event.m_jets) 
    {
      jet->auxdecor<int>("AT_SoftMuonIndex")=-1;
      jet->auxdecor<float>("AT_SoftMuonDR")=-1;
    }
      
    int imuon=0;
    for(const xAOD::Muon* sm : event.m_softmuons)
    {
      //writing auxiliary info for SMT jet tagging
      double dRmin=100.;
      int nearestJetIndex=-1;
      int ijet=0;
      for(const xAOD::Jet *jet : event.m_jets)
      {
        double dr= xAOD::P4Helpers::deltaR(sm,jet,m_config->softmuonDRJetcutUseRapidity());
        if(dr<dRmin && dr<m_config->softmuonDRJetcut())
        {
          dRmin=dr;
          nearestJetIndex=ijet;
        }
        ijet++;
      }
      sm->auxdecor<int>("AT_SMTJetIndex")=nearestJetIndex;
      sm->auxdecor<float>("AT_SMTJetDR")=dRmin;
      
      if(nearestJetIndex>=0)
      {
        const xAOD::Jet *jet = event.m_jets[nearestJetIndex];
        if(jet->auxdecor<int>("AT_SoftMuonIndex")<0) //in this way we only associate a jet with the highest pt soft muon
        {
          jet->auxdecor<int>("AT_SoftMuonIndex")=imuon;
          jet->auxdecor<float>("AT_SoftMuonDR")=dRmin;
        }
      }//end of case where we found a jet nearby
      
      imuon++;
    }//end of loop on muons
    
  }//end of TopEventMaker::decorateTopEvent
}

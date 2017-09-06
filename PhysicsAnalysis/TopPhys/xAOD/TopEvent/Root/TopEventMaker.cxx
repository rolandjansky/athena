/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TopEventMaker.cxx 795594 2017-02-04 22:12:45Z tpelzer $
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

#include "TopPartons/PartonHistory.h"

namespace top {

  TopEventMaker::TopEventMaker( const std::string& name ) :
    asg::AsgTool( name ),
    m_config(nullptr)
  {
    declareProperty( "config" , m_config );
  }
  
  /// As top-xaod isn't an asg::AsgTool, it doesn't have access to all the information
  /// Very annoying, as it's actually quite simple
  const xAOD::SystematicEventContainer* TopEventMaker::systematicEvents(const std::string& sgKey) const
  {
    const xAOD::SystematicEventContainer* systEvents(nullptr);
    top::check( evtStore()->retrieve(systEvents,sgKey) , "Failed to get xAOD::SystematicEventContainer");
    return systEvents;
  }

  top::Event TopEventMaker::makeTopEvent(const xAOD::SystematicEvent& currentSystematic)
  {
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
    
    //Primary Vertices
    if (evtStore()->contains<xAOD::VertexContainer>(m_config->sgKeyPrimaryVertices())) {
      top::check(evtStore()->retrieve(event.m_primaryVertices, m_config->sgKeyPrimaryVertices()), "Failed to retrieve Primary Vertices");
    }   
    
    //electrons
    if (m_config->useElectrons()) {
      ///-- Need to read const collections for mini-xaod read back --///
      
      const xAOD::ElectronContainer* calibratedElectrons(nullptr);
      top::check(evtStore()->retrieve(calibratedElectrons, m_config->sgKeyElectrons(hash) ), "Failed to retrieve electrons");
        
      ///-- Shallow copy and save to TStore --///
      if (!evtStore()->contains<xAOD::ElectronContainer>(m_config->sgKeyElectronsTDS(hash))) {
        std::pair< xAOD::ElectronContainer*, xAOD::ShallowAuxContainer* > shallow_electrons = xAOD::shallowCopyContainer( *calibratedElectrons );
          
        xAOD::TReturnCode save = evtStore()->tds()->record( shallow_electrons.first , m_config->sgKeyElectronsTDS(hash) );
        xAOD::TReturnCode saveAux = evtStore()->tds()->record( shallow_electrons.second , m_config->sgKeyElectronsTDSAux(hash) );
        top::check( (save && saveAux) , "Failed to store object in TStore");
      }
      
      ///-- Pull shallow copy back out of TStore in non-const way --///
      xAOD::ElectronContainer* calibratedElectronsTDS(nullptr);
      top::check(evtStore()->retrieve(calibratedElectronsTDS, m_config->sgKeyElectronsTDS(hash) ), "Failed to retrieve electrons");      
      
      
      // re-write overlap information if it is decorated onto currentSystematic
      if (currentSystematic.isAvailable< std::vector<unsigned int> >("overlapsEl")) {
        std::vector<unsigned int> overlapsEl = currentSystematic.auxdataConst< std::vector<unsigned int> >("overlapsEl");
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
      //sort only the selected muons (faster)
      event.m_electrons.sort(top::descendingPtSorter);             
    }

    //photons
    if (m_config->usePhotons()) {
      ///-- Need to read const collections for mini-xaod read back --///

      const xAOD::PhotonContainer* calibratedPhotons(nullptr);
      top::check(evtStore()->retrieve(calibratedPhotons, m_config->sgKeyPhotons(hash) ), "Failed to retrieve photons");

      ///-- Shallow copy and save to TStore --///
      if (!evtStore()->contains<xAOD::PhotonContainer>(m_config->sgKeyPhotonsTDS(hash))) {
        std::pair< xAOD::PhotonContainer*, xAOD::ShallowAuxContainer* > shallow_photons = xAOD::shallowCopyContainer( *calibratedPhotons );

        xAOD::TReturnCode save = evtStore()->tds()->record( shallow_photons.first , m_config->sgKeyPhotonsTDS(hash) );
        xAOD::TReturnCode saveAux = evtStore()->tds()->record( shallow_photons.second , m_config->sgKeyPhotonsTDSAux(hash) );
        top::check( (save && saveAux) , "Failed to store object in TStore");
      }

      ///-- Pull shallow copy back out of TStore in non-const way --///
      xAOD::PhotonContainer* calibratedPhotonsTDS(nullptr);
      top::check(evtStore()->retrieve(calibratedPhotonsTDS, m_config->sgKeyPhotonsTDS(hash) ), "Failed to retrieve photons");

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
      top::check(evtStore()->retrieve(calibratedMuons, m_config->sgKeyMuons(hash) ), "Failed to retrieve muons");
      
      ///-- Shallow copy and save to TStore --///
      if (!evtStore()->contains<xAOD::MuonContainer>(m_config->sgKeyMuonsTDS(hash))) {
        std::pair< xAOD::MuonContainer*, xAOD::ShallowAuxContainer* > shallow_muons = xAOD::shallowCopyContainer( *calibratedMuons );
          
        xAOD::TReturnCode save = evtStore()->tds()->record( shallow_muons.first , m_config->sgKeyMuonsTDS(hash) );
        xAOD::TReturnCode saveAux = evtStore()->tds()->record( shallow_muons.second , m_config->sgKeyMuonsTDSAux(hash) );
        top::check( (save && saveAux) , "Failed to store object in TStore");
      }
      
      ///-- Pull shallow copy back out of TStore in non-const way --///
      xAOD::MuonContainer* calibratedMuonsTDS(nullptr);
      top::check(evtStore()->retrieve(calibratedMuonsTDS, m_config->sgKeyMuonsTDS(hash) ), "Failed to retrieve muons");         
        
      // re-write overlap information if it is decorated onto currentSystematic
      if (currentSystematic.isAvailable< std::vector<unsigned int> >("overlapsMu")) {
        std::vector<unsigned int> overlapsMu = currentSystematic.auxdataConst< std::vector<unsigned int> >("overlapsMu");
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

    //taus
    if (m_config->useTaus()) {
      ///-- Need to read const collections for mini-xaod read back --///     
      const xAOD::TauJetContainer* calibratedTaus(nullptr);
      top::check(evtStore()->retrieve(calibratedTaus,m_config->sgKeyTaus(hash) ), "Failed to retrieve taus");

      ///-- Shallow copy and save to TStore --///
      if (!evtStore()->contains<xAOD::TauJetContainer>(m_config->sgKeyTausTDS(hash))) {
        std::pair< xAOD::TauJetContainer*, xAOD::ShallowAuxContainer* > shallow_taus = xAOD::shallowCopyContainer( *calibratedTaus );
          
        xAOD::TReturnCode save = evtStore()->tds()->record( shallow_taus.first , m_config->sgKeyTausTDS(hash) );
        xAOD::TReturnCode saveAux = evtStore()->tds()->record( shallow_taus.second , m_config->sgKeyTausTDSAux(hash) );
        top::check( (save && saveAux) , "Failed to store object in TStore");
      }
      
      ///-- Pull shallow copy back out of TStore in non-const way --///
      xAOD::TauJetContainer* calibratedTausTDS(nullptr);
      top::check(evtStore()->retrieve(calibratedTausTDS, m_config->sgKeyTausTDS(hash) ), "Failed to retrieve taus");  
      
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
      std::string sgKeyCalibrated( m_config->sgKeyJets(hash,looseJets) );
      std::string sgKeyTmp = "tmp_"+sgKeyCalibrated;
      std::string sgKeyTmpAux = sgKeyTmp+"Aux.";         
      
      const xAOD::JetContainer* calibratedJets(nullptr);
      top::check(evtStore()->retrieve(calibratedJets, m_config->sgKeyJets(hash,looseJets)), "Failed to retrieve jets");

      ///-- Shallow copy and save to TStore --///
      if (!evtStore()->contains<xAOD::JetContainer>(m_config->sgKeyJetsTDS(hash,looseJets))) {
        std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > shallow_jets = xAOD::shallowCopyContainer( *calibratedJets );
          
        xAOD::TReturnCode save = evtStore()->tds()->record( shallow_jets.first , m_config->sgKeyJetsTDS(hash,looseJets) );
        xAOD::TReturnCode saveAux = evtStore()->tds()->record( shallow_jets.second , m_config->sgKeyJetsTDSAux(hash,looseJets) );
        top::check( (save && saveAux) , "Failed to store object in TStore");
      }
      
      ///-- Pull shallow copy back out of TStore in non-const way --///
      xAOD::JetContainer* calibratedJetsTDS(nullptr);
      top::check(evtStore()->retrieve(calibratedJetsTDS, m_config->sgKeyJetsTDS(hash,looseJets) ), "Failed to retrieve taus");  
      
      for (auto index : currentSystematic.goodJets()) {
        auto jet = calibratedJetsTDS->at(index);

        if (m_config->isMC()) {
          // make product of SF (initialised to 1 in the header)
          top::check( jet->isAvailable<float>("JET_SF_jvt") , " Can't find jet decoration \"JET_SF_jvt\" - we need it to calculate the jet scale-factors!");
          event.m_jvtSF *= jet->auxdataConst<float>("JET_SF_jvt");
          if (currentSystematic.hashValue() == m_config->nominalHashValue()) {// we only need the up/down JVT SF systs for nominal
            top::check( jet->isAvailable<float>("JET_SF_jvt_UP") , " Can't find jet decoration \"JET_SF_jvt_UP\" - we need it to calculate the jet scale-factors!");
            event.m_jvtSF_UP *= jet->auxdataConst<float>("JET_SF_jvt_UP");
            top::check( jet->isAvailable<float>("JET_SF_jvt_DOWN") , " Can't find jet decoration \"JET_SF_jvt_DOWN\" - we need it to calculate the jet scale-factors!");
            event.m_jvtSF_DOWN *= jet->auxdataConst<float>("JET_SF_jvt_DOWN");
          }
        }

        top::check( jet->isAvailable<char>("passJVT") , " Can't find jet decoration \"passJVT\" - we need it to decide if we should keep the jet in the top::Event instance or not!");
        if (jet->auxdataConst<char>( "passJVT" ))
          event.m_jets.push_back(calibratedJetsTDS->at(index));
      }
      
      //shallow copies aren't sorted!
      //sort only the selected taus (faster)
      event.m_jets.sort(top::descendingPtSorter);
    }

    //large-R jets
    if (m_config->useLargeRJets()) {
      ///-- Need to read const collections for mini-xaod read back --///     
      const xAOD::JetContainer* calibratedJets(nullptr);
      top::check(evtStore()->retrieve(calibratedJets,m_config->sgKeyLargeRJets(hash) ), "Failed to retrieve largeR jets");

      ///-- Shallow copy and save to TStore --///
      if (!evtStore()->contains<xAOD::JetContainer>(m_config->sgKeyLargeRJetsTDS(hash))) {
        std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > shallow_jets = xAOD::shallowCopyContainer( *calibratedJets );
          
        xAOD::TReturnCode save = evtStore()->tds()->record( shallow_jets.first , m_config->sgKeyLargeRJetsTDS(hash) );
        xAOD::TReturnCode saveAux = evtStore()->tds()->record( shallow_jets.second , m_config->sgKeyLargeRJetsTDSAux(hash) );
        top::check( (save && saveAux) , "Failed to store object in TStore");
      }
      
      ///-- Pull shallow copy back out of TStore in non-const way --///
      xAOD::JetContainer* calibratedJetsTDS(nullptr);
      top::check(evtStore()->retrieve(calibratedJetsTDS, m_config->sgKeyLargeRJetsTDS(hash) ), "Failed to retrieve largeR jets");  
      
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
      top::check(evtStore()->retrieve(calibratedJets,m_config->sgKeyTrackJets(hash) ), "Failed to retrieve track jets");

      ///-- Shallow copy and save to TStore --///
      if (!evtStore()->contains<xAOD::JetContainer>(m_config->sgKeyTrackJetsTDS(hash))) {
        std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > shallow_jets = xAOD::shallowCopyContainer( *calibratedJets );
          
        xAOD::TReturnCode save = evtStore()->tds()->record( shallow_jets.first , m_config->sgKeyTrackJetsTDS(hash) );
        xAOD::TReturnCode saveAux = evtStore()->tds()->record( shallow_jets.second , m_config->sgKeyTrackJetsTDSAux(hash) );
        top::check( (save && saveAux) , "Failed to store object in TStore");
      }
      
      ///-- Pull shallow copy back out of TStore in non-const way --///
      xAOD::JetContainer* calibratedJetsTDS(nullptr);
      top::check(evtStore()->retrieve(calibratedJetsTDS, m_config->sgKeyTrackJetsTDS(hash) ), "Failed to retrieve track jets");  
      
      for (auto index : currentSystematic.goodTrackJets()) {
        event.m_trackJets.push_back(calibratedJetsTDS->at(index));
      }         
      
      //shallow copies aren't sorted!
      //sort only the selected jets (faster)
      event.m_trackJets.sort(top::descendingPtSorter);
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
        top::check(evtStore()->retrieve(event.m_truth, m_config->sgKeyMCParticle()), "Failed to retrieve truth particles");
      }     
      
      // Truth Event
      top::check(evtStore()->retrieve(event.m_truthEvent,m_config->sgKeyTruthEvent()),"Failed to retrieve truth Event");
    
      // Parton History
      if (m_config->doTopPartonHistory()) {
        if (evtStore()->contains<xAOD::PartonHistoryContainer>(m_config->sgKeyTopPartonHistory())) {
          const xAOD::PartonHistoryContainer* partonHistory(nullptr);
          top::check(evtStore()->retrieve(partonHistory, m_config->sgKeyTopPartonHistory()),"Failed to retrieve Top Parton History");
          if (partonHistory->size() == 1) {
            event.m_partonHistory = partonHistory->at(0);
          } 
        }
      } // end doTopPartonHistory
                      
    } // end isMC 
    


    return event;
  }
}

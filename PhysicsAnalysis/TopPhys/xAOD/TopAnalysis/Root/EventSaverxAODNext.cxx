/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopAnalysis/EventSaverxAODNext.h"

#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"
#include "TopEvent/SystematicEvent.h"

#include "TopPartons/PartonHistory.h"

#include "TopConfiguration/ConfigurationSettings.h"
#include "TopConfiguration/TopConfig.h"

#include "xAODRootAccess/TEvent.h"

#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODJet/JetContainer.h"
// #include "xAODBtagging/BTaggingContainer.h"
#include "xAODMissingET/MissingETContainer.h"


#include "TFile.h"

namespace top {

  EventSaverxAODNext::EventSaverxAODNext() : 
    asg::AsgMetadataTool( "top::EventSaverxAODNext" ),
    m_config(nullptr), 
    m_outputFile(nullptr),
    m_saveAllObjects(false)
  {
  }
  
  EventSaverxAODNext::~EventSaverxAODNext()
  {
  }

  void EventSaverxAODNext::initialize(std::shared_ptr<top::TopConfig> config, TFile* file, const std::vector<std::string>& extraBranches) {
    m_config = config;
    m_outputFile = file;
    top::check(evtStore()->event()->writeTo(m_outputFile), "EventSaverxAODNext trying to call writeTo on output file");

    // Persist the top::TopConfig settings - we'll need this to make sense of the output when reading it back 
    top::check( outputMetaStore()->record( m_config->createPersistentSettings() , "TopPersistentSettings" ) , "Failed to save TopPersistentSettings" );    
    auto* const settings = top::ConfigurationSettings::get();
    
    //EventInfo
    std::string eventInfoList = settings->value("EventVariableSaveList");
    if (config->isMC())
        eventInfoList += ".mcChannelNumber.mcEventWeights";

    //Add extra branches - e.g. if it passed the ee / mumu / emu selection per event
    for (const auto& branchName : extraBranches)
        eventInfoList += "." + branchName;

    //std::cout << "EventLoaderCorrectSelect::branchesToSave - EventInfo " << eventInfoList << std::endl;
    evtStore()->event()->setAuxItemList("EventInfoAux.", eventInfoList);
    
    // Photons
    if (m_config->usePhotons()) {
      std::string photonVariableSaveList = settings->value("PhotonVariableSaveList");
      if (config->isMC()) {
        photonVariableSaveList += ".truthType.truthOrigin";
      }
      
      for (auto currentSystematic : *m_config->systSgKeyMapPhotons()) {
        std::string sgKey = currentSystematic.second;
        evtStore()->event()->setAuxItemList(sgKey + "Aux." , photonVariableSaveList );
      }
    } // Photons    

    // Electrons
    if (m_config->useElectrons()) {
      std::string electronVariableSaveList = settings->value("ElectronVariableSaveList");
      if (config->isMC()) {
        electronVariableSaveList += ".truthType.truthOrigin";
      }
      
      for (auto currentSystematic : *m_config->systSgKeyMapElectrons()) {
        std::string sgKey = currentSystematic.second;
        evtStore()->event()->setAuxItemList(sgKey + "Aux." , electronVariableSaveList );
      }
    } // Electrons
    
    // Muons
    if (m_config->useMuons()) {
      std::string muonVariableSaveList = settings->value("MuonVariableSaveList");

      // When these variables exist
      // if (config->isMC()) {
      //   muonVariableSaveList += ".truthType.truthOrigin";
      // }
      
      for (auto currentSystematic : *m_config->systSgKeyMapMuons()) {
        std::string sgKey = currentSystematic.second;
        evtStore()->event()->setAuxItemList(sgKey + "Aux." , muonVariableSaveList );
      }
    } // Muons   
        
    // Taus
    if (m_config->useTaus()) {
      std::string tauVariableSaveList = settings->value("TauVariableSaveList");
      
      for (auto currentSystematic : *m_config->systSgKeyMapTaus()) {
        std::string sgKey = currentSystematic.second;
        evtStore()->event()->setAuxItemList(sgKey + "Aux." , tauVariableSaveList );
      }
    } // Taus    
    
    
    // Jets
    if (m_config->useJets()) {
      std::string jetVariableSaveList = settings->value("JetVariableSaveList");
      
      for (auto currentSystematic : *m_config->systSgKeyMapJets(false)) {
        std::string sgKey = currentSystematic.second;
        evtStore()->event()->setAuxItemList(sgKey + "Aux." , jetVariableSaveList );
      }     
    }
    
    if (m_config->useLargeRJets()) {
      std::string jetVariableSaveList = settings->value("JetVariableSaveList");
      
      for (auto currentSystematic : *m_config->systSgKeyMapLargeRJets()) {
        std::string sgKey = currentSystematic.second;
        evtStore()->event()->setAuxItemList(sgKey + "Aux." , jetVariableSaveList );
      }     
    } 
    
    if (m_config->useTrackJets()) {
      std::string jetVariableSaveList = settings->value("JetVariableSaveList");
      
      for (auto currentSystematic : *m_config->systSgKeyMapTrackJets()) {
        std::string sgKey = currentSystematic.second;
        evtStore()->event()->setAuxItemList(sgKey + "Aux." , jetVariableSaveList );
      }     
    }     
    
    if (m_config->useJets() || m_config->useLargeRJets() || m_config->useTrackJets()) {
      // B-tagging
      std::string bTagVariableSaveList = settings->value("BTagVariableSaveList");
      evtStore()->event()->setAuxItemList("BTagging_AntiKt4EMTopoAux.",bTagVariableSaveList);
    } // B-tagging  
    

  }

  void EventSaverxAODNext::saveEventToxAOD() 
  {
    // Systematic Event 
    const xAOD::SystematicEventContainer* allSystematics(nullptr);
    top::check(evtStore()->retrieve(allSystematics,m_config->sgKeyTopSystematicEvents()) , "Failed to retrieve xAOD::SystematicEventContainer" );  
    
    // Save all reco event objects?
    // If no systematic in the event passes the event selection
    // We don't fill the collections - this saves a lot of disk space
    bool saveEvent(false),saveEventObjects(false);
    if (!m_config->saveOnlySelectedEvents()) {
      saveEvent = true;
    }
    if (m_config->saveOnlySelectedEvents()) {
      for (auto x : *allSystematics) {
        if (x->auxdataConst<char>(m_config->passEventSelectionDecoration()) == 1) {
          saveEvent = true;
          saveEventObjects = true;
          break;
        }
      }
    } // Save reco event?
    
    if (saveEvent) {

      //Event Info
      top::check(evtStore()->event()->copy("EventInfo"), "copying event info failed");   
      
      // BTagging - can consider thinning this at some point. 
      // Tricky computation as all jet collections need updated ElementLinks
      top::check(evtStore()->event()->copy("BTagging_AntiKt4EMTopo") , "copying btagging failed" );

      top::check(evtStore()->event()->copy("egammaClusters"), "copying egamma clusters failed");
            
      // Object Thinning maps       
      std::shared_ptr<ThinningMap_t> photonThinningMap = savePhotons(saveEventObjects);      
      std::shared_ptr<ThinningMap_t> electronThinningMap = saveElectrons(saveEventObjects);
      std::shared_ptr<ThinningMap_t> muonThinningMap = saveMuons(saveEventObjects);      
      std::shared_ptr<ThinningMap_t> tauThinningMap = saveTaus(saveEventObjects);
      std::shared_ptr<ThinningMap_t> jetThinningMap = saveJets(saveEventObjects);      
      std::shared_ptr<ThinningMap_t> largeRJetThinningMap = saveLargeRJets(saveEventObjects); 
      std::shared_ptr<ThinningMap_t> trackJetThinningMap = saveTrackJets(saveEventObjects);
      
      // Save SystematicEventContainer
      xAOD::SystematicEventContainer* allSystematics_output = new xAOD::SystematicEventContainer{};
      SG::IAuxStore* allSystematics_output_aux = evtStore()->event()->recordAux( m_config->sgKeyTopSystematicEvents() + "Aux." );
      allSystematics_output->setStore( allSystematics_output_aux );
      if (saveEventObjects) {
        for (auto systematicPtr : *allSystematics) {
	  if (!(systematicPtr->hashValue() == m_config->nominalHashValue() || m_config->doTightSysts() || m_config->doLooseSysts())) continue;
          xAOD::SystematicEvent* out = new xAOD::SystematicEvent{};
          out->makePrivateStore( *systematicPtr );
          out->setGoodPhotons   ( thinObjectSelection( systematicPtr->hashValue() , systematicPtr->goodPhotons()    , photonThinningMap    ) );
          out->setGoodElectrons ( thinObjectSelection( systematicPtr->hashValue() , systematicPtr->goodElectrons()  , electronThinningMap  ) );
          out->setGoodMuons     ( thinObjectSelection( systematicPtr->hashValue() , systematicPtr->goodMuons()      , muonThinningMap      ) );
          out->setGoodTaus      ( thinObjectSelection( systematicPtr->hashValue() , systematicPtr->goodTaus()       , tauThinningMap       ) );
          out->setGoodJets      ( thinObjectSelection( systematicPtr->hashValue() , systematicPtr->goodJets()       , jetThinningMap       ) );
          out->setGoodLargeRJets( thinObjectSelection( systematicPtr->hashValue() , systematicPtr->goodLargeRJets() , largeRJetThinningMap ) );
          out->setGoodTrackJets ( thinObjectSelection( systematicPtr->hashValue() , systematicPtr->goodTrackJets()  , trackJetThinningMap  ) );
          allSystematics_output->push_back( out );
          
          // Missing ET   
          xAOD::MissingETContainer* met(nullptr);
          top::check( evtStore()->retrieve(met,m_config->sgKeyMissingEt( systematicPtr->hashValue() ) ) , "Failed to retrieve MET" );
          
          xAOD::MissingETContainer* output_met = new xAOD::MissingETContainer{};
          SG::IAuxStore* output_met_aux = evtStore()->event()->recordAux( m_config->sgKeyMissingEt( systematicPtr->hashValue() ) + "Aux." );
          output_met->setStore( output_met_aux );
          for (auto x : *met) {
            xAOD::MissingET* out = new xAOD::MissingET{};
            out->makePrivateStore( *x );
            output_met->push_back( out );
          }
          top::check( evtStore()->event()->record(output_met,m_config->sgKeyMissingEt( systematicPtr->hashValue() )) , "Failed to save MET" );
          
          
        }
      }
      top::check(evtStore()->event()->record(allSystematics_output,m_config->sgKeyTopSystematicEvents()),"Failed to record xAOD::SystematicEventContainer" );   
      
      // Write to disk
      top::check(evtStore()->event()->fill(), "xAOD::TEvent fill failed");
    }
  }
  
  

  std::vector<unsigned int> EventSaverxAODNext::thinObjectSelection(const std::size_t hashValue , 
                                                                const std::vector<unsigned int>& objectList , 
                                                                const std::shared_ptr<ThinningMap_t> thinningMap) const
  {
    // reduced output
    std::vector<unsigned int> out;
    
    // Find the correct systematic in the thinningMap
    ThinningMap_Itr Itr = thinningMap->find( hashValue );
    // If the systematic is not in the map, then we use the nominal hashValue
    // This happens, for example, when we look at a jet systematic and are looping over electrons
    if (Itr == thinningMap->end()) {
      Itr = thinningMap->find( m_config->nominalHashValue() );
    }
    
    // double check that we have a valid systematic
    if (Itr != thinningMap->end()) {
      
      // Get the relevant thinningMap for this specific systematic
      std::map<unsigned int,unsigned int> currentSystematicThinningMap = (*Itr).second;
      
      // Loop over the original good object list, this contains information about objects we are not going to save
      // Use the thinningMap to build a new good object list based on the objects we are saving
      for (auto i : objectList) {
        std::map<unsigned int,unsigned int>::const_iterator Itr2 = currentSystematicThinningMap.find( i );
        if (Itr2 != currentSystematicThinningMap.end()) {
          out.push_back( (*Itr2).second );
        }
      }    
    }
    
    return out;
  }

  /**
  * All the saveObject functions work the same way
  *    - we should probably turn this into a template method at some point
  *
  * We only save objects that pass the pre overlap removal selection
  *    - This throws away a lot of objects and saves a lot of disk space  
  * 
  * We need to consider every calibration systematic variation
  *     - Each calibration systematic contains a vector of goodIndices used to form top::Event's
  *     - We need to re-map these vectors if we throw away objects
  *     - For example, lets consider a dummy jet systematic:
  *          - calibrated JetContainer has objects [0,1,2,3,4,5,6,7,8]
  *          - The pre OR selection cuts keep objects [3,4,7]
  *          - We only want to save objects [3,4,7], objects [0,1,2,5,6,8] are a waste of disk space
  *          - The saved objects in the output mini-xAOD become [0,1,2]
  *          - We need to map [ 3->0 , 4->1 , 7->2 ]
  *     - This needs to be done for each xAOD::SystematicEvent 
  * 
  */

  std::shared_ptr<std::map<std::size_t,std::map<unsigned int,unsigned int>>> EventSaverxAODNext::savePhotons(const bool saveEventObjects)
  {
    std::shared_ptr<ThinningMap_t> thinningMap( new ThinningMap_t );
    if (m_config->usePhotons()) {
      
      for (auto currentSystematic : *m_config->systSgKeyMapPhotons()) {
        std::map<unsigned int,unsigned int> currentSystematicThinningMap;
        std::string sgKey = currentSystematic.second;
        const xAOD::PhotonContainer* xaod(nullptr);
        top::check(evtStore()->retrieve(xaod,sgKey),"Failed to retrieve photons");
        
        xAOD::PhotonContainer* output = new xAOD::PhotonContainer{};
        SG::IAuxStore* output_aux = evtStore()->event()->recordAux( sgKey + "Aux." );
        output->setStore( output_aux );      
  
        if( saveEventObjects ) {      
          unsigned int index(0),indexReduced(0);
          for (auto x : *xaod) {
            
            bool save(m_saveAllObjects);
            if (x->isAvailable<char>("passPreORSelection")) {
              if (x->auxdataConst<char>("passPreORSelection") == 1) {
                save = true;
              }
            }
            if (x->isAvailable<char>("passPreORSelectionLoose")) {
              if (x->auxdataConst<char>("passPreORSelectionLoose") == 1) {
                save = true;
              }
            }
            
            if (save) {
              currentSystematicThinningMap.insert( std::make_pair( index , indexReduced ) );
              xAOD::Photon* out = new xAOD::Photon{};
              out->makePrivateStore( *x );
              output->push_back( out );
              ++indexReduced;
            }
            ++index;
          } // Loop over container
        } // saveEventObjects       
        top::check(evtStore()->event()->record(output,sgKey),"Failed to record xAOD::PhotonContainer"); 
        thinningMap->insert( std::make_pair(currentSystematic.first, currentSystematicThinningMap) );
      } // Loop over all systematics;
    } // use object type
    
    return thinningMap;   
  }

  std::shared_ptr<std::map<std::size_t,std::map<unsigned int,unsigned int>>> EventSaverxAODNext::saveElectrons(const bool saveEventObjects)
  {
    std::shared_ptr<ThinningMap_t> thinningMap( new ThinningMap_t );
    if (m_config->useElectrons()) {
      
      for (auto currentSystematic : *m_config->systSgKeyMapElectrons()) {
        std::map<unsigned int,unsigned int> currentSystematicThinningMap;
        std::string sgKey = currentSystematic.second;
        const xAOD::ElectronContainer* xaod(nullptr);
        top::check(evtStore()->retrieve(xaod,sgKey),"Failed to retrieve electrons");
        
        xAOD::ElectronContainer* output = new xAOD::ElectronContainer{};
        SG::IAuxStore* output_aux = evtStore()->event()->recordAux( sgKey + "Aux." );
        output->setStore( output_aux );      

        if( saveEventObjects ) {      
          unsigned int index(0),indexReduced(0);
          for (auto x : *xaod) {
            
            bool save(m_saveAllObjects);
            if (x->isAvailable<char>("passPreORSelection")) {
              if (x->auxdataConst<char>("passPreORSelection") == 1) {
                save = true;
              }
            }
            if (x->isAvailable<char>("passPreORSelectionLoose")) {
              if (x->auxdataConst<char>("passPreORSelectionLoose") == 1) {
                save = true;
              }
            }
            
            if (save) {
              currentSystematicThinningMap.insert( std::make_pair( index , indexReduced ) );
              xAOD::Electron* out = new xAOD::Electron{};
              out->makePrivateStore( *x );
              output->push_back( out );
              ++indexReduced;
            }
            ++index;
          } // Loop over container
	  // Reset calo cluster links
	  for (std::map<unsigned int,unsigned int>::const_iterator i=currentSystematicThinningMap.begin();i!=currentSystematicThinningMap.end();++i) {
	    output->at((*i).second)->setCaloClusterLinks( xaod->at((*i).first)->caloClusterLinks() );
	  }
        } // saveEventObjects       
        top::check(evtStore()->event()->record(output,sgKey),"Failed to record xAOD::ElectronContainer");
        thinningMap->insert( std::make_pair(currentSystematic.first, currentSystematicThinningMap) );    
      } // Loop over all systematics  
    } // use object type
    
    return thinningMap; 
  }

      

  std::shared_ptr<std::map<std::size_t,std::map<unsigned int,unsigned int>>> EventSaverxAODNext::saveMuons(const bool saveEventObjects)
  {
    std::shared_ptr<ThinningMap_t> thinningMap( new ThinningMap_t );
    if (m_config->useMuons()) {
      
      for (auto currentSystematic : *m_config->systSgKeyMapMuons()) {
        std::map<unsigned int,unsigned int> currentSystematicThinningMap;
        std::string sgKey = currentSystematic.second;
        const xAOD::MuonContainer* xaod(nullptr);
        top::check(evtStore()->retrieve(xaod,sgKey),"Failed to retrieve muons");
        
        xAOD::MuonContainer* output = new xAOD::MuonContainer{};
        SG::IAuxStore* output_aux = evtStore()->event()->recordAux( sgKey + "Aux." );
        output->setStore( output_aux );      

        if( saveEventObjects ) {      
          unsigned int index(0),indexReduced(0);
          for (auto x : *xaod) {
            
            bool save(m_saveAllObjects);
            if (x->isAvailable<char>("passPreORSelection")) {
              if (x->auxdataConst<char>("passPreORSelection") == 1) {
                save = true;
              }
            }
            if (x->isAvailable<char>("passPreORSelectionLoose")) {
              if (x->auxdataConst<char>("passPreORSelectionLoose") == 1) {
                save = true;
              }
            }
            
            if (save) {
              currentSystematicThinningMap.insert( std::make_pair( index , indexReduced ) );
              xAOD::Muon* out = new xAOD::Muon{};
              out->makePrivateStore( *x );
              output->push_back( out );
              ++indexReduced;
            }
            ++index;
          } // Loop over container
        } // saveEventObjects       
        top::check(evtStore()->event()->record(output,sgKey),"Failed to record xAOD::MuonContainer");
        thinningMap->insert( std::make_pair(currentSystematic.first, currentSystematicThinningMap) );    
      } // Loop over all systematics  
    } // use object type
    
    return thinningMap;  
  }

  std::shared_ptr<std::map<std::size_t,std::map<unsigned int,unsigned int>>> EventSaverxAODNext::saveTaus(const bool saveEventObjects)
  {
    std::shared_ptr<ThinningMap_t> thinningMap( new ThinningMap_t );
    if (m_config->useTaus()) {
      
      for (auto currentSystematic : *m_config->systSgKeyMapTaus()) {
        std::map<unsigned int,unsigned int> currentSystematicThinningMap;
        std::string sgKey = currentSystematic.second;
        const xAOD::TauJetContainer* xaod(nullptr);
        top::check(evtStore()->retrieve(xaod,sgKey),"Failed to retrieve taus");
        
        xAOD::TauJetContainer* output = new xAOD::TauJetContainer{};
        SG::IAuxStore* output_aux = evtStore()->event()->recordAux( sgKey + "Aux." );
        output->setStore( output_aux );      

        if( saveEventObjects ) {      
          unsigned int index(0),indexReduced(0);
          for (auto x : *xaod) {
            
            bool save(m_saveAllObjects);
            if (x->isAvailable<char>("passPreORSelection")) {
              if (x->auxdataConst<char>("passPreORSelection") == 1) {
                save = true;
              }
            }
            if (x->isAvailable<char>("passPreORSelectionLoose")) {
              if (x->auxdataConst<char>("passPreORSelectionLoose") == 1) {
                save = true;
              }
            }
            
            if (save) {
              currentSystematicThinningMap.insert( std::make_pair( index , indexReduced ) );
              xAOD::TauJet* out = new xAOD::TauJet{};
              out->makePrivateStore( *x );
              output->push_back( out );
              ++indexReduced;
            }
            ++index;
          } // Loop over container
        } // saveEventObjects
        top::check(evtStore()->event()->record(output,sgKey),"Failed to record xAOD::TauJetContainer"); 
        thinningMap->insert( std::make_pair(currentSystematic.first, currentSystematicThinningMap) );    
      } // Loop over all systematics  
    } // use object type
    
    return thinningMap;  
  }

  std::shared_ptr<std::map<std::size_t,std::map<unsigned int,unsigned int>>> EventSaverxAODNext::saveJets(const bool saveEventObjects)
  {
    std::shared_ptr<ThinningMap_t> thinningMap( new ThinningMap_t );
    if (m_config->useJets()) {
      
      for (auto currentSystematic : *m_config->systSgKeyMapJets(false)) {
        std::map<unsigned int,unsigned int> currentSystematicThinningMap;
        std::string sgKey = currentSystematic.second;
        const xAOD::JetContainer* xaod(nullptr);
        top::check(evtStore()->retrieve(xaod,sgKey),"Failed to retrieve electrons");
        
        xAOD::JetContainer* output = new xAOD::JetContainer{};
        SG::IAuxStore* output_aux = evtStore()->event()->recordAux( sgKey + "Aux." );
        output->setStore( output_aux );      
        
        if( saveEventObjects ) {      
          unsigned int index(0),indexReduced(0);
          for (auto x : *xaod) {
            
            bool save(m_saveAllObjects);
            if (x->isAvailable<char>("passPreORSelection")) {
              if (x->auxdataConst<char>("passPreORSelection") == 1) {
                save = true;
              }
            }
            if (x->isAvailable<char>("passPreORSelectionLoose")) {
              if (x->auxdataConst<char>("passPreORSelectionLoose") == 1) {
                save = true;
              }
            }
            
            if (save) {
              currentSystematicThinningMap.insert( std::make_pair( index , indexReduced ) );
              xAOD::Jet* out = new xAOD::Jet{};
              out->makePrivateStore( *x );
              output->push_back( out );
              ++indexReduced;
            }
            ++index;
          } // Loop over container
          
          // Reset b-tagging links
          for (std::map<unsigned int,unsigned int>::const_iterator i=currentSystematicThinningMap.begin();i!=currentSystematicThinningMap.end();++i) {
            output->at((*i).second)->setBTaggingLink ( xaod->at((*i).first)->btaggingLink() );
          }
          
        } // saveEventObjects      
        top::check(evtStore()->event()->record(output,sgKey),"Failed to record xAOD::JetContainer"); 
        thinningMap->insert( std::make_pair(currentSystematic.first, currentSystematicThinningMap) );    
      } // Loop over all systematics  
    } // use object type
    
    return thinningMap;  
  }

  std::shared_ptr<std::map<std::size_t,std::map<unsigned int,unsigned int>>> EventSaverxAODNext::saveLargeRJets(const bool saveEventObjects)
  {
    std::shared_ptr<ThinningMap_t> thinningMap( new ThinningMap_t );
    if (m_config->useLargeRJets()) {
      
      for (auto currentSystematic : *m_config->systSgKeyMapLargeRJets()) {
        std::map<unsigned int,unsigned int> currentSystematicThinningMap;
        std::string sgKey = currentSystematic.second;
        const xAOD::JetContainer* xaod(nullptr);
        top::check(evtStore()->retrieve(xaod,sgKey),"Failed to retrieve electrons");
        
        xAOD::JetContainer* output = new xAOD::JetContainer{};
        SG::IAuxStore* output_aux = evtStore()->event()->recordAux( sgKey + "Aux." );
        output->setStore( output_aux );      
        
        if( saveEventObjects ) {
          unsigned int index(0),indexReduced(0);
          for (auto x : *xaod) {
            
            bool save(m_saveAllObjects);
            if (x->isAvailable<char>("passPreORSelection")) {
              if (x->auxdataConst<char>("passPreORSelection") == 1) {
                save = true;
              }
            }
            if (x->isAvailable<char>("passPreORSelectionLoose")) {
              if (x->auxdataConst<char>("passPreORSelectionLoose") == 1) {
                save = true;
              }
            }
            
            if (save) {
              currentSystematicThinningMap.insert( std::make_pair( index , indexReduced ) );
              xAOD::Jet* out = new xAOD::Jet{};
              out->makePrivateStore( *x );
              output->push_back( out );
              ++indexReduced;
            }
            ++index;
          } // Loop over container
          
          // Reset b-tagging links
          for (std::map<unsigned int,unsigned int>::const_iterator i=currentSystematicThinningMap.begin();i!=currentSystematicThinningMap.end();++i) {
            output->at((*i).second)->setBTaggingLink ( xaod->at((*i).first)->btaggingLink() );
          }
          
        } // saveEventObjects
        top::check(evtStore()->event()->record(output,sgKey),"Failed to record xAOD::JetContainer");
        thinningMap->insert( std::make_pair(currentSystematic.first, currentSystematicThinningMap) );    
      } // Loop over all systematics  
    } // use object type
    
    return thinningMap;  
  }  
  
  std::shared_ptr<std::map<std::size_t,std::map<unsigned int,unsigned int>>> EventSaverxAODNext::saveTrackJets(const bool saveEventObjects)
  {
    std::shared_ptr<ThinningMap_t> thinningMap( new ThinningMap_t );
    if (m_config->useTrackJets()) {
      
      for (auto currentSystematic : *m_config->systSgKeyMapTrackJets()) {
        std::map<unsigned int,unsigned int> currentSystematicThinningMap;
        std::string sgKey = currentSystematic.second;
        const xAOD::JetContainer* xaod(nullptr);
        top::check(evtStore()->retrieve(xaod,sgKey),"Failed to retrieve electrons");
        
        xAOD::JetContainer* output = new xAOD::JetContainer{};
        SG::IAuxStore* output_aux = evtStore()->event()->recordAux( sgKey + "Aux." );
        output->setStore( output_aux );      
        
        if( saveEventObjects ) {
          unsigned int index(0),indexReduced(0);
          for (auto x : *xaod) {
            
            bool save(m_saveAllObjects);
            if (x->isAvailable<char>("passPreORSelection")) {
              if (x->auxdataConst<char>("passPreORSelection") == 1) {
                save = true;
              }
            }
            if (x->isAvailable<char>("passPreORSelectionLoose")) {
              if (x->auxdataConst<char>("passPreORSelectionLoose") == 1) {
                save = true;
              }
            }
            
            if (save) {
              currentSystematicThinningMap.insert( std::make_pair( index , indexReduced ) );
              xAOD::Jet* out = new xAOD::Jet{};
              out->makePrivateStore( *x );
              output->push_back( out );
              ++indexReduced;
            }
            ++index;
          } // Loop over container
          
          // Reset b-tagging links
          for (std::map<unsigned int,unsigned int>::const_iterator i=currentSystematicThinningMap.begin();i!=currentSystematicThinningMap.end();++i) {
            output->at((*i).second)->setBTaggingLink ( xaod->at((*i).first)->btaggingLink() );
          }
          
        } // saveEventObjects
        top::check(evtStore()->event()->record(output,sgKey),"Failed to record xAOD::JetContainer");
        thinningMap->insert( std::make_pair(currentSystematic.first, currentSystematicThinningMap) );    
      } // Loop over all systematics  
    } // use object type
    
    return thinningMap;  
  }    

  void EventSaverxAODNext::finalize() {
      m_outputFile->Write();
      top::check(evtStore()->event()->finishWritingTo(m_outputFile), "EventSaverxAODNext::finalise trying to call finishWritingTo");
  }

}

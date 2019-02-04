/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopObjectSelectionTools/TopObjectSelection.h"

#include "TopConfiguration/TopConfig.h"

#include "TopEvent/EventTools.h"
#include "TopEvent/SystematicEvent.h"

#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODEventInfo/EventInfo.h"


namespace top {

TopObjectSelection::TopObjectSelection( const std::string& name ) :
  asg::AsgTool( name ),
  m_config(nullptr),

  m_electronSelection(nullptr),
  m_muonSelection(nullptr),
  m_tauSelection(nullptr),
  m_jetSelection(nullptr),
  m_photonSelection(nullptr),
  m_largeJetSelection(nullptr),
  m_trackJetSelection(nullptr),

  m_overlapRemovalToolPostSelection(nullptr),
  
  m_electronInJetSubtractor(nullptr),
  
  m_passPreORSelection("passPreORSelection"),
  m_passPreORSelectionLoose("passPreORSelectionLoose"),
  // the following two are used to give failing JVT jets a lower priority in the OR
  m_ORToolDecoration("ORToolDecoration"),
  m_ORToolDecorationLoose("ORToolDecorationLoose"),
  
  m_doLooseCuts(false)
{
  declareProperty( "config" , m_config );
}

  StatusCode TopObjectSelection::initialize()
  {
  // If there are loose selection requirements, then we need additional computations
  // Tight leptons are not constrained to be a sub-set of loose
  // We will call the selections in the following order:
  //   (1) passSelection();
  //   (2) passSelectionLoose();
  // 
  // If your tight is not a sub-set of your loose, then you have 2 different functions
  // If your tight is a sub-set of your loose, then I suggest you set a loose flag
  // when calling passSelection(), then have passSelectionLoose() return this flag
  //
  // Requests to process loose selections can come from:
  //   (1) Data event
  //   (2) User request overlap removal be done on loose objects
  //         - The top recommendation is that you do OR on tight objects
  //   (3) Determination of Fakes control regions in MC - expert fakes mode
  //   
  
  if (!m_config->isMC())
      m_doLooseCuts = true;
  
  if (m_config->doOverlapRemovalOnLooseLeptonDef())
      m_doLooseCuts = true;
  
  if (m_config->doLooseEvents())
      m_doLooseCuts = true;
  
  if (m_config->applyElectronInJetSubtraction()) {
    m_electronInJetSubtractor = std::unique_ptr<top::ElectronInJetSubtractionCollectionMaker>
    ( new top::ElectronInJetSubtractionCollectionMaker( "top::ElectronInJetSubtractionCollectionMaker" ) );
    top::check( m_electronInJetSubtractor->setProperty( "config" , m_config ) , "Failed to setProperty of top::ElectronInJetSubtractionCollectionMaker" );
    top::check( m_electronInJetSubtractor->initialize() , "Failed to initialize top::ElectronInJetSubtractionCollectionMaker" );
  }
  
  // b-tagging stuff
  // for calo jets
  std::vector<std::string> availableWPs = m_config->bTagWP_available();
  for (auto& WP : availableWPs) {
    m_btagSelTools[WP] = "BTaggingSelectionTool_"+WP+"_"+m_config->sgKeyJets();
    top::check( m_btagSelTools[WP].retrieve(), "Failed to retrieve b-tagging Selection tool" );
  }
  // for track jets
  availableWPs = m_config->bTagWP_available_trkJet();
  for (auto& WP : availableWPs) {
    m_trkjet_btagSelTools[WP] = "BTaggingSelectionTool_"+WP+"_"+m_config->sgKeyTrackJets();
    top::check( m_trkjet_btagSelTools[WP].retrieve(), "Failed to retrieve b-tagging Selection tool" );
  }
  
  // boosted tagging stuff
  if (m_config->useLargeRJets()) {
    m_topTag50 = ToolHandle<SmoothedTopTagger>("topTag50");
    top::check( m_topTag50.retrieve(), "Failed to retrieve 50% top-tagging tool" );
    m_topTag80 = ToolHandle<SmoothedTopTagger>("topTag80");
    top::check( m_topTag80.retrieve(), "Failed to retrieve 80% top-tagging tool" );
    m_WTag50 = ToolHandle<SmoothedWZTagger>("WTag50");
    top::check( m_WTag50.retrieve(), "Failed to retrieve 50% W-tagging tool" );
    m_WTag80 = ToolHandle<SmoothedWZTagger>("WTag80");
    top::check( m_WTag80.retrieve(), "Failed to retrieve 80% W-tagging tool" );
    m_ZTag50 = ToolHandle<SmoothedWZTagger>("ZTag50");
    top::check( m_ZTag50.retrieve(), "Failed to retrieve 50% Z-tagging tool" );
    m_ZTag80 = ToolHandle<SmoothedWZTagger>("ZTag80");
    top::check( m_ZTag80.retrieve(), "Failed to retrieve 80% Z-tagging tool" );
  }
  
  return StatusCode::SUCCESS;  
}



void TopObjectSelection::electronSelection(ElectronSelectionBase* ptr) {
    m_electronSelection.reset(ptr);
}

void TopObjectSelection::muonSelection(MuonSelectionBase* ptr) {
    m_muonSelection.reset(ptr);
}

void TopObjectSelection::tauSelection(TauSelectionBase* ptr) {
    m_tauSelection.reset(ptr);
}

void TopObjectSelection::jetSelection(JetSelectionBase* ptr) {
    m_jetSelection.reset(ptr);
}

void TopObjectSelection::photonSelection(PhotonSelectionBase* ptr) {
  m_photonSelection.reset(ptr);
}
  
void TopObjectSelection::largeJetSelection(JetSelectionBase* ptr) {
    m_largeJetSelection.reset(ptr);
}

void TopObjectSelection::trackJetSelection(JetSelectionBase* ptr) {
    m_trackJetSelection.reset(ptr);
}

void TopObjectSelection::overlapRemovalPostSelection(OverlapRemovalBase* ptr) {
    m_overlapRemovalToolPostSelection.reset(ptr);
}

StatusCode TopObjectSelection::execute(bool executeNominal)
{
  // Set variable to tell us if we are doing this execution on nominal or systematic events
  m_executeNominal = executeNominal;
  applySelectionPreOverlapRemoval();
  top::check( applyOverlapRemoval() , "Failed to apply overlap removal" );

  return StatusCode::SUCCESS;
}

void TopObjectSelection::applySelectionPreOverlapRemoval() 
{

  // If we are doing Electron in Jet subtraction,
  // We need to:
  //  (1) Apply object selection to the electrons
  //  (2) Subtract the electron from the jet, which:
  //      - Changes the jet 4-momentum
  //      - Reject additional electrons
  //  (3) Apply object selection to the modified jets
  
  if( m_config->useElectrons()  ){applySelectionPreOverlapRemovalElectrons()  ;}
  
  if (m_config->applyElectronInJetSubtraction()) {
    top::check( m_electronInJetSubtractor->execute(m_executeNominal) , "Failed to execute top::ElectronInJetSubtractionCollectionMaker" );
  }


  if( m_config->usePhotons() && m_photonSelection != nullptr ){applySelectionPreOverlapRemovalPhotons()    ;}
  if( m_config->useMuons() && m_muonSelection != nullptr     ){applySelectionPreOverlapRemovalMuons()      ;}
  if( m_config->useTaus()  && m_tauSelection != nullptr      ){applySelectionPreOverlapRemovalTaus()       ;}
  if( m_config->useJets()  && m_jetSelection != nullptr      ){applySelectionPreOverlapRemovalJets()       ;}
  if( m_config->useLargeRJets() && m_largeJetSelection != nullptr){applySelectionPreOverlapRemovalLargeRJets() ;}
  if( m_config->useTrackJets()  && m_trackJetSelection != nullptr){applySelectionPreOverlapRemovalTrackJets()  ;}
}

void TopObjectSelection::applySelectionPreOverlapRemovalPhotons() {
    for (auto currentSystematic : *m_config->systSgKeyMapPhotons()) {
        ///-- if executeNominal, skip other systematics (and vice-versa) --///
        if(m_executeNominal && !m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
        if(!m_executeNominal && m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
        
        const xAOD::PhotonContainer* photons(nullptr);
        top::check(evtStore()->retrieve(photons, currentSystematic.second), "TopObjectSelection::applySelectionPreOverlapRemovalPhotons() failed to retrieve photons");
        ATH_MSG_DEBUG(" Cut on Photons with key = "<<currentSystematic.second);
        
        for (auto photonPtr : *photons) {
            photonPtr->auxdecor<char>( m_passPreORSelection ) = m_photonSelection->passSelection(*photonPtr);
            photonPtr->auxdecor<char>( m_ORToolDecoration ) = photonPtr->auxdataConst<char>( m_passPreORSelection ) * 2;
            if (m_doLooseCuts) {
                photonPtr->auxdecor<char>( m_passPreORSelectionLoose ) = m_photonSelection->passSelectionLoose(*photonPtr);
                photonPtr->auxdecor<char>( m_ORToolDecorationLoose ) = photonPtr->auxdataConst<char>( m_passPreORSelectionLoose ) * 2;
            }
        }
    }
}

/**
 * @brief For each systematic load each of the electrons and test if they pass
 * the object selection.
 */
void TopObjectSelection::applySelectionPreOverlapRemovalElectrons() {
    for (auto currentSystematic : *m_config->systSgKeyMapElectrons()) {
        ///-- if executeNominal, skip other systematics (and vice-versa) --///
        if(m_executeNominal && !m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
        if(!m_executeNominal && m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
        
        const xAOD::ElectronContainer* electrons(nullptr);
        top::check(evtStore()->retrieve(electrons, currentSystematic.second), "TopObjectSelection::applySelectionPreOverlapRemovalElectrons() failed to retrieve electrons");
        ATH_MSG_DEBUG(" Cut on Electrons with key = "<<currentSystematic.second);
        
        for (auto electronPtr : *electrons) {
            electronPtr->auxdecor<char>( m_passPreORSelection ) = m_electronSelection->passSelection(*electronPtr);
            electronPtr->auxdecor<char>( m_ORToolDecoration ) = electronPtr->auxdataConst<char>( m_passPreORSelection ) * 2;
            if (m_doLooseCuts) {
                electronPtr->auxdecor<char>( m_passPreORSelectionLoose ) = m_electronSelection->passSelectionLoose(*electronPtr);
                electronPtr->auxdecor<char>( m_ORToolDecorationLoose ) = electronPtr->auxdataConst<char>( m_passPreORSelectionLoose ) * 2;
            }
        }
    }
}

void TopObjectSelection::applySelectionPreOverlapRemovalMuons() {
    for (auto currentSystematic : *m_config->systSgKeyMapMuons()) {
        ///-- if executeNominal, skip other systematics (and vice-versa) --///
        if(m_executeNominal && !m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
        if(!m_executeNominal && m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
        const xAOD::MuonContainer* muons(nullptr);
        top::check(evtStore()->retrieve(muons, currentSystematic.second) , "TopObjectSelection::applySelectionPreOverlapRemovalMuons() failed to retrieve muons" );
        
        for (auto muonPtr : *muons) {
            muonPtr->auxdecor<char>( m_passPreORSelection ) = m_muonSelection->passSelection(*muonPtr);
            muonPtr->auxdecor<char>( m_ORToolDecoration ) = muonPtr->auxdataConst<char>( m_passPreORSelection ) * 2;
            if (m_doLooseCuts) {
                muonPtr->auxdecor<char>( m_passPreORSelectionLoose ) = m_muonSelection->passSelectionLoose(*muonPtr);
                muonPtr->auxdecor<char>( m_ORToolDecorationLoose ) = muonPtr->auxdataConst<char>( m_passPreORSelectionLoose ) * 2;
            }
        }
    }
}

void TopObjectSelection::applySelectionPreOverlapRemovalTaus() {
    for (auto currentSystematic : *m_config->systSgKeyMapTaus()) {
        ///-- if executeNominal, skip other systematics (and vice-versa) --///
        if(m_executeNominal && !m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
        if(!m_executeNominal && m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
        const xAOD::TauJetContainer* taus(nullptr);
        top::check(evtStore()->retrieve(taus, currentSystematic.second) , "TopObjectSelection::applySelectionPreOverlapRemovalTaus() failed to retrieve taus" );
        
        for (auto tauPtr : *taus) {
            tauPtr->auxdecor<char>( m_passPreORSelection ) = m_tauSelection->passSelection(*tauPtr);
            tauPtr->auxdecor<char>( m_ORToolDecoration ) = tauPtr->auxdataConst<char>( m_passPreORSelection ) * 2;
            if (m_doLooseCuts) {
                tauPtr->auxdecor<char>( m_passPreORSelectionLoose ) = m_tauSelection->passSelectionLoose(*tauPtr);
                tauPtr->auxdecor<char>( m_ORToolDecorationLoose ) = tauPtr->auxdataConst<char>( m_passPreORSelectionLoose ) * 2;
            }
        }
    }  
}

void TopObjectSelection::applySelectionPreOverlapRemovalJets() {
  
  // Boosted analysis subtract electrons from jets 
  // If we use Loose electrons we end up with loose jet collections
  
  bool isLooseJets(false);
  
  for (auto currentSystematic : *m_config->systSgKeyMapJets(isLooseJets)) {
    ///-- if executeNominal, skip other systematics (and vice-versa) --///
    if(m_executeNominal && !m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
    if(!m_executeNominal && m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
      
    const xAOD::JetContainer* jets(nullptr);
    top::check(evtStore()->retrieve(jets, currentSystematic.second) , "TopObjectSelection::applySelectionPreOverlapRemovalJets() failed to retrieve jets" );
    ATH_MSG_DEBUG(" Cut on Jets with key = "<<currentSystematic.second);

    for (auto jetPtr : *jets) {
      ATH_MSG_DEBUG("   Jet pt = "<<(jetPtr)->pt());
      bool passed = m_jetSelection->passSelection(*jetPtr);
      jetPtr->auxdecor<char>(m_passPreORSelection) = passed;
      jetPtr->auxdecor<char>(m_ORToolDecoration) = (passed ? (jetPtr->auxdataConst<char>("passJVT") ? 2 : 1) : 0);
      if (m_doLooseCuts) {
        jetPtr->auxdecor<char>(m_passPreORSelectionLoose) = passed;
        jetPtr->auxdecor<char>(m_ORToolDecorationLoose) = (passed ? (jetPtr->auxdataConst<char>("passJVT") ? 2 : 1) : 0);
      }
      //decorate with b-tagging flags
      std::vector<std::string> availableWPs = m_config->bTagWP_available();
      for (auto& WP : availableWPs) {
        if (WP.find("Continuous") == std::string::npos) {
          bool  isTagged = false;
          if (std::fabs(jetPtr->eta()) <= 2.5 ) {
            ToolHandle<IBTaggingSelectionTool>& btagsel = m_btagSelTools[WP];
            isTagged = btagsel->accept(*jetPtr);
          }
          jetPtr->auxdecor<char>("isbtagged_"+WP) = isTagged;
        }
        else {
          int tagWeightBin = -2; // AT default
          if (std::fabs(jetPtr->eta()) <= 2.5 ) {
            ToolHandle<IBTaggingSelectionTool>& btagsel = m_btagSelTools[WP];
            tagWeightBin = btagsel->getQuantile(*jetPtr);
          }
          jetPtr->auxdecor<int>("tagWeightBin_"+WP) = tagWeightBin;
        }
      }
    }
  }
  
  // Are we using the ElectronInJetSubtraction and running with loose lepton definitons
  if (m_doLooseCuts && m_config->applyElectronInJetSubtraction()) {
    for (auto currentSystematic : *m_config->systSgKeyMapJets(m_doLooseCuts)) {
      ///-- if executeNominal, skip other systematics (and vice-versa) --///
      if(m_executeNominal && !m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
      if(!m_executeNominal && m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
        
      const xAOD::JetContainer* jets(nullptr);
      top::check(evtStore()->retrieve(jets, currentSystematic.second) , "TopObjectSelection::applySelectionPreOverlapRemovalJets() failed to retrieve jets" );
      ATH_MSG_DEBUG(" Cut on Jets with key = "<<currentSystematic.second);

      for (auto jetPtr : *jets) {
        char decoration = m_jetSelection->passSelection(*jetPtr);
        jetPtr->auxdecor<char>( m_passPreORSelection ) = decoration;
        // if JVT cut enabled: jets that pass JVT get a 2, otherwise the same as passPreORSelection
        // if not, passPreORSelection * 2
        if (jetPtr->isAvailable<char>("passJVT")) {
          jetPtr->auxdecor<char>( m_ORToolDecoration ) = decoration + jetPtr->auxdataConst<char>( "passJVT" );
        } else {
          jetPtr->auxdecor<char>( m_ORToolDecoration ) = decoration * 2;
        }
        if (m_doLooseCuts) {
          jetPtr->auxdecor<char>( m_passPreORSelectionLoose ) = decoration;
          if (jetPtr->isAvailable<char>("passJVT")) {
            jetPtr->auxdecor<char>( m_ORToolDecorationLoose ) = decoration + jetPtr->auxdataConst<char>( "passJVT" );
          } else {
            jetPtr->auxdecor<char>( m_ORToolDecorationLoose ) = decoration * 2;
          }
        }
        //decorate with b-tagging flags
        std::vector<std::string> availableWPs = m_config->bTagWP_available();
        for (auto& WP : availableWPs) {
          if (WP.find("Continuous") == std::string::npos) {
            bool  isTagged = false;
            if (std::fabs(jetPtr->eta()) < 2.5 ) {
              ToolHandle<IBTaggingSelectionTool>& btagsel = m_btagSelTools[WP];
              isTagged = btagsel->accept(*jetPtr);
            }
            jetPtr->auxdecor<char>("isbtagged_"+WP) = isTagged;
          }
          else {
            int tagWeightBin = -2; // AT default
            if (std::fabs(jetPtr->eta()) < 2.5 ) {
              ToolHandle<IBTaggingSelectionTool>& btagsel = m_btagSelTools[WP];
              tagWeightBin = btagsel->getQuantile(*jetPtr);
            }
            jetPtr->auxdecor<int>("tagWeightBin_"+WP) = tagWeightBin;
          }
        }
      }
    }    
  }
  
}

void TopObjectSelection::applySelectionPreOverlapRemovalLargeRJets() 
{
    for (auto currentSystematic : *m_config->systSgKeyMapLargeRJets()) {
        ///-- if executeNominal, skip other systematics (and vice-versa) --///
        if(m_executeNominal && !m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
        if(!m_executeNominal && m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
        
        const xAOD::JetContainer* jets(nullptr);
        top::check(evtStore()->retrieve(jets, currentSystematic.second) , "TopObjectSelection::applySelectionPreOverlapRemovalLargeRJets() failed to retrieve large R jets" );

        for (auto jetPtr : *jets) {
          char decoration = m_largeJetSelection->passSelection(*jetPtr);
          jetPtr->auxdecor<char>( m_passPreORSelection ) = decoration;
          jetPtr->auxdecor<char>( m_ORToolDecoration ) = decoration * 2;
          if (m_doLooseCuts) {
            jetPtr->auxdecor<char>( m_passPreORSelectionLoose ) = decoration;
            jetPtr->auxdecor<char>( m_ORToolDecorationLoose ) = decoration * 2;
          }
          //decorate with boosted-tagging flags
          char isTop_80    = m_topTag80->tag(*jetPtr).getCutResultInverted()==0?1:0;
          char isTop_50    = m_topTag50->tag(*jetPtr).getCutResultInverted()==0?1:0;
          char Wtagged_80  = m_WTag80->tag(*jetPtr).getCutResultInverted()==0?1:0;
          char Wtagged_50  = m_WTag50->tag(*jetPtr).getCutResultInverted()==0?1:0;
          char Ztagged_80  = m_ZTag80->tag(*jetPtr).getCutResultInverted()==0?1:0;
          char Ztagged_50  = m_ZTag50->tag(*jetPtr).getCutResultInverted()==0?1:0;

          jetPtr->auxdecor<char>("isTopTagged_80")  = isTop_80;
          jetPtr->auxdecor<char>("isTopTagged_50")  = isTop_50;
          jetPtr->auxdecor<char>("isWTagged_80")    = Wtagged_80;
          jetPtr->auxdecor<char>("isWTagged_50")    = Wtagged_50;
          jetPtr->auxdecor<char>("isZTagged_80")    = Ztagged_80;
          jetPtr->auxdecor<char>("isZTagged_50")    = Ztagged_50;
        }
    }
}

void TopObjectSelection::applySelectionPreOverlapRemovalTrackJets() 
{
  ///-- if executeNominal, skip other systematics (and vice-versa) --///
  if(!m_executeNominal) return;
    
  const xAOD::JetContainer* jets(nullptr);
  top::check(evtStore()->retrieve(jets, m_config->sgKeyTrackJets()) , "TopObjectSelection::applySelectionPreOverlapRemovalTrackJets() failed to retrieve track jets" );
  
  
  
  for (const xAOD::Jet* jetPtr : *jets) {
    char decoration = m_trackJetSelection->passSelection(*jetPtr);
    jetPtr->auxdecor<char>( m_passPreORSelection ) = decoration;
    if (m_doLooseCuts) {
      jetPtr->auxdecor<char>( m_passPreORSelectionLoose ) = decoration;         
    }
    
    if( m_config->sgKeyTrackJets() == "AntiKtVR30Rmax4Rmin02TrackJets"){ // Event cleaning for variable-R track jets
      
      float pt_baseline = 5e3;
      float radius1=std::max(0.02,std::min(0.4,30000./jetPtr->pt()));
      
      float dr_jets;
      bool passDRcut = true;
      for( const xAOD::Jet* jet2 : *jets ) {
	
	if (jet2->pt()<pt_baseline) continue;
	if (jetPtr == jet2) continue;
	
	float radius2=std::max(0.02,std::min(0.4,30000./jet2->pt()));
	
	dr_jets = sqrt( pow(jetPtr->eta()-jet2->eta(),2) + pow(jetPtr->phi()- jet2->phi(),2) );
	if ( dr_jets < std::min(radius1,radius2) ) passDRcut = false;
      
      }
      jetPtr->auxdecor<char>("passDRcut") = passDRcut;
      
    }
    
    std::vector<std::string> availableWPs = m_config->bTagWP_available_trkJet();
    for (auto& WP : availableWPs) {
        if (WP.find("Continuous") == std::string::npos){
        bool  isTagged = false;
        if (std::fabs(jetPtr->eta()) < 2.5 ) {
          ToolHandle<IBTaggingSelectionTool>& btagsel = m_trkjet_btagSelTools[WP];
          isTagged = btagsel->accept(*jetPtr);
        }
        jetPtr->auxdecor<char>("isbtagged_"+WP) = isTagged;
      }
      else {
        int tagWeightBin = -2; // AT default
        if (std::fabs(jetPtr->eta()) < 2.5 ) {
          ToolHandle<IBTaggingSelectionTool>& btagsel = m_btagSelTools[WP];
          tagWeightBin = btagsel->getQuantile(*jetPtr);
        }
        jetPtr->auxdecor<int>("tagWeightBin") = tagWeightBin;
      }
    }
  }
}

StatusCode TopObjectSelection::applyOverlapRemoval()
{
  bool aLooseEvent(true),aTightEvent(false);
  if (m_config->doTightEvents()) {
    top::check( applyOverlapRemoval(aTightEvent,m_config->sgKeyTopSystematicEvents()) , "Failed to apply overlap removal" );
  }
  if (m_config->doLooseEvents()) {
    top::check( applyOverlapRemoval(aLooseEvent,m_config->sgKeyTopSystematicEventsLoose()) , "Failed to apply overlap removal" );
  } 
  return StatusCode::SUCCESS;    
}

StatusCode TopObjectSelection::applyOverlapRemoval(const bool isLoose,const std::string& sgKey) 
{
  // create the xAOD::SystematicEventContainer
  xAOD::SystematicEventAuxContainer* systEventAuxCont = new xAOD::SystematicEventAuxContainer{};
  xAOD::SystematicEventContainer* systEventCont = new xAOD::SystematicEventContainer{};
  systEventCont->setStore( systEventAuxCont );  
  
  // We need to be able to have the nominal systematic processed first 
  // but we cannot alter this inside TopConfig as we use unordered_set
  // Using an ordered set will only order on hash and not on insertion 
  // so we still cannot control it there
  // Best place to handle it is when the systematic object container is being filled (before it is const)
  // We read all hashes into a vector, and then custom sort to be nominal, then alphabetical
  std::vector<size_t> sortedSystHashAll(m_config->systHashAll()->begin(), m_config->systHashAll()->end());
  std::sort(sortedSystHashAll.begin(), sortedSystHashAll.end(),
            [this](const size_t &lhs, const size_t &rhs){
	      // if lhs is nominal, true - move
	      if (this->m_config->isSystNominal(this->m_config->systematicName(lhs))) return true;
	      // if rhs is nominal, false - fix
	      else if (this->m_config->isSystNominal(this->m_config->systematicName(rhs))) return false;
              // otherwise alphabetical - sort
	      else return (this->m_config->systematicName(lhs) < this->m_config->systematicName(rhs));
            });

  for (auto systematicNumber : sortedSystHashAll) {
    ATH_MSG_DEBUG(systematicNumber << " " << m_config->systematicName(systematicNumber) );
    ///-- if executeNominal, skip other systematics (and vice-versa) --///
    if(m_executeNominal && !m_config->isSystNominal(m_config->systematicName(systematicNumber))) continue;
    if(!m_executeNominal && m_config->isSystNominal(m_config->systematicName(systematicNumber))) continue;

    if((!m_config->doTightSysts() && !isLoose) && !m_config->isSystNominal(m_config->systematicName(systematicNumber))) continue;
    if((!m_config->doLooseSysts() && isLoose)  && !m_config->isSystNominal(m_config->systematicName(systematicNumber))) continue;

    xAOD::SystematicEvent* systEvent = new xAOD::SystematicEvent{};
    systEventCont->push_back( systEvent );
    systEvent->setHashValue( systematicNumber );
    systEvent->setIsLooseEvent( isLoose );
    if (!isLoose) {systEvent->setTtreeIndex( m_config->ttreeIndex( systematicNumber ) );}
    if (isLoose)  {systEvent->setTtreeIndex( m_config->ttreeIndexLoose( systematicNumber ) );}
    systEvent->auxdecor<char> (m_config->passEventSelectionDecoration()) = 0;
    top::check( applyOverlapRemoval( systEvent ) , "Failed to apply overlap removal" );
    
  }
  
  if(m_executeNominal){
    // Create a new StoreGate key
    std::string sgKeyNominal = sgKey + "Nominal";

    // Save to StoreGate / TStore                                                              
    std::string outputSGKeyNominalAux = sgKeyNominal + "Aux.";

    xAOD::TReturnCode save = evtStore()->tds()->record( systEventCont ,sgKeyNominal );
    xAOD::TReturnCode saveAux = evtStore()->tds()->record( systEventAuxCont , outputSGKeyNominalAux );
    if( !save || !saveAux ){
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;

  }
  else{
    // Retrieve nominal container, get nominal event, copy into full systematic container
    xAOD::SystematicEventContainer* systEventContNominal = new xAOD::SystematicEventContainer{};
    top::check( evtStore()->retrieve(systEventContNominal, sgKey+"Nominal"), "Failed to retrieve nominal container");
    for (auto x: *systEventContNominal){
      xAOD::SystematicEvent* systEventNominal = new xAOD::SystematicEvent{};      
      systEventCont->push_back( systEventNominal );
      *systEventNominal = *x;
    }

    // Save to StoreGate / TStore
    std::string outputSGKeyAux = sgKey + "Aux.";
  
    xAOD::TReturnCode save = evtStore()->tds()->record( systEventCont ,sgKey  );
    xAOD::TReturnCode saveAux = evtStore()->tds()->record( systEventAuxCont , outputSGKeyAux );
    if( !save || !saveAux ){
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;  
  }
}


StatusCode TopObjectSelection::applyOverlapRemoval(xAOD::SystematicEvent* currentSystematic) {
    
  // Which lepton definition are we using for the overlap removal?
  // Default for top analysis is "Tight"
  // 
  // Use "Loose" for Fakes estimates
  //
  // Other physics groups choose to do overlap removal on "Loose" lepton definitions
  // As such, this mode is fully supported, simply set:
  //    OverlapRemovalLeptonDef Loose
  // In your configuration file
  
  bool looseLeptonOR(false); // default behaviour for top analysis - use the "Tight" definitions
  if (currentSystematic->isLooseEvent()) {
    looseLeptonOR = true;
  }
  if (m_config->doOverlapRemovalOnLooseLeptonDef()) {
    looseLeptonOR = true; 
  }
  
  std::size_t hash = currentSystematic->hashValue();

  // Retrieve the relevant shallow copies
  const xAOD::PhotonContainer* xaod_photon(nullptr);
  if (m_config->usePhotons())
      top::check(evtStore()->retrieve(xaod_photon, m_config->sgKeyPhotons(hash)) , "TopObjectSelection::applyOverlapRemovalPostSelection() failed to retrieve photons");
    
  const xAOD::ElectronContainer* xaod_el(nullptr);
  if (m_config->useElectrons())
      top::check(evtStore()->retrieve(xaod_el, m_config->sgKeyElectrons(hash)) , "TopObjectSelection::applyOverlapRemovalPostSelection() failed to retrieve electrons" );

  const xAOD::MuonContainer* xaod_mu(nullptr);
  if (m_config->useMuons())
      top::check(evtStore()->retrieve(xaod_mu, m_config->sgKeyMuons(hash)) , "TopObjectSelection::applyOverlapRemovalPostSelection() failed to retrieve muons" );
  
  const xAOD::TauJetContainer* xaod_tau(nullptr);
  if (m_config->useTaus())
      top::check(evtStore()->retrieve(xaod_tau, m_config->sgKeyTaus(hash)) , "TopObjectSelection::applyOverlapRemovalPostSelection() failed to retrieve taus");
  
  const xAOD::JetContainer* xaod_jet(nullptr);
  if (m_config->useJets())
      top::check(evtStore()->retrieve(xaod_jet, m_config->sgKeyJets(hash,looseLeptonOR)) , "TopObjectSelection::applyOverlapRemovalPostSelection() failed to retrieve jets" );

  const xAOD::JetContainer* xaod_ljet(nullptr);
  if (m_config->useLargeRJets())
      top::check(evtStore()->retrieve(xaod_ljet, m_config->sgKeyLargeRJets(hash)) , "TopObjectSelection::applyOverlapRemovalPostSelection() failed to retrieve large-R jets");

  const xAOD::JetContainer* xaod_tjet(nullptr);
  if (m_config->useTrackJets())
      top::check(evtStore()->retrieve(xaod_tjet, m_config->sgKeyTrackJets()) , "TopObjectSelection::applyOverlapRemovalPostSelection() failed to retrieve track jets");  
  
  // vectors to store the indices of objects passing selection and overlap removal
  std::vector<unsigned int> goodPhotons,goodElectrons,goodMuons,goodTaus,goodJets,goodLargeRJets,goodTrackJets;
  
  // Apply overlap removal
  m_overlapRemovalToolPostSelection->overlapremoval(xaod_photon,xaod_el,xaod_mu,xaod_tau,
                                                    xaod_jet,xaod_ljet,
                                                    goodPhotons,goodElectrons,goodMuons,goodTaus,
                                                    goodJets,goodLargeRJets,looseLeptonOR); 
  
  // Additonal lepton information
  std::vector<unsigned int> overlapsEl,overlapsMu;
  if (m_overlapRemovalToolPostSelection->overlapsEl(overlapsEl) ) {
    currentSystematic->auxdecor< std::vector<unsigned int> >("overlapsEl") = overlapsEl;
  }
  if (m_overlapRemovalToolPostSelection->overlapsMu(overlapsMu) ) {
    currentSystematic->auxdecor< std::vector<unsigned int> >("overlapsMu") = overlapsMu;
  }  
  

  
  // If we did use overlap removal on "Loose" lepton definitions
  // We take the remaining leptons and only keep those passing the "Tight" cuts
  if (!currentSystematic->isLooseEvent() && m_config->doOverlapRemovalOnLooseLeptonDef()) {
    applyTightSelectionPostOverlapRemoval( xaod_photon , goodPhotons );    
    applyTightSelectionPostOverlapRemoval( xaod_el , goodElectrons );
    applyTightSelectionPostOverlapRemoval( xaod_mu , goodMuons );
    applyTightSelectionPostOverlapRemoval( xaod_tau , goodTaus );    
    applyTightSelectionPostOverlapRemoval( xaod_jet , goodJets );
    applyTightSelectionPostOverlapRemoval( xaod_ljet , goodLargeRJets );
  }
  
  if (m_config->useTrackJets()){
    for (unsigned int i=0; i<xaod_tjet->size(); ++i) goodTrackJets.push_back(i);
    trackJetOverlapRemoval(xaod_el, xaod_mu, xaod_tjet, goodElectrons, goodMuons, goodTrackJets);
    applyTightSelectionPostOverlapRemoval( xaod_tjet , goodTrackJets );
  }  
  
  // Post overlap removal decorations
  decorateMuonsPostOverlapRemoval(xaod_mu, xaod_jet, goodMuons, goodJets);
  
  // set the indices in the xAOD::SystematicEvent
  currentSystematic->setGoodPhotons( goodPhotons );
  currentSystematic->setGoodElectrons( goodElectrons );
  currentSystematic->setGoodMuons( goodMuons );
  currentSystematic->setGoodTaus( goodTaus );
  currentSystematic->setGoodJets( goodJets );
  currentSystematic->setGoodLargeRJets( goodLargeRJets );
  currentSystematic->setGoodTrackJets( goodTrackJets );
    
  decorateEventInfoPostOverlapRemoval(goodJets.size(), currentSystematic->isLooseEvent());
    
  return StatusCode::SUCCESS;
}

void TopObjectSelection::applyTightSelectionPostOverlapRemoval( const  xAOD::IParticleContainer* xaod , std::vector<unsigned int>& indices )
{
  // Copy the original indices of the xAOD objects in 
  // the collection that pass the overlap removal
  std::vector<unsigned int> tmpCopy;
  for (auto i : indices) {
    tmpCopy.push_back( i );
  }
  
  // Clear the original indices
  indices.clear();
  
  // Only save indices that are in tmpCopy && passPreORSelection
  // This will keep the objects that pass the "Loose" overlap selection
  // and pass the "Tight" cuts
  for (auto i : tmpCopy) {
    if (xaod->at(i)->isAvailable< char> ( m_passPreORSelection )) {
      if (xaod->at(i)->auxdataConst< char >( m_passPreORSelection ) == 1) {
        indices.push_back( i );
      }
    }
  }
}

void TopObjectSelection::trackJetOverlapRemoval(const  xAOD::IParticleContainer* xaod_el, 
                                                const  xAOD::IParticleContainer* xaod_mu, 
                                                const  xAOD::IParticleContainer* xaod_tjet, 
                                                std::vector<unsigned int>& goodElectrons, 
                                                std::vector<unsigned int>& goodMuons, 
                                                std::vector<unsigned int>& goodTrackJets)
{

  // Copy the original indices of the xAOD objects in 
  // the collection that pass the overlap removal
  std::vector<unsigned int> tmpGoodTrackJets;
  for (auto i : goodTrackJets) {
    tmpGoodTrackJets.push_back( i );
  }

  // Clear the original indices
  goodTrackJets.clear();

  int counterTrk = 0;
  for (auto jet : *xaod_tjet){
    bool matchEl(false);
    int counterEl = 0;
    for (auto el : *xaod_el){
      if ((std::find(goodElectrons.begin(), goodElectrons.end(), counterEl) != goodElectrons.end())){
        if ( el->p4().DeltaR(jet->p4())<0.2){
          matchEl = true;
          break;
        }
      }
      counterEl++;
    }

    bool matchMu(false);
    int counterMu = 0;
    for (auto mu : *xaod_mu){
      if ((std::find(goodMuons.begin(), goodMuons.end(), counterMu) != goodMuons.end())){
        if ( mu->p4().DeltaR(jet->p4())<0.2){
          matchMu = true;
          break;
        }
      }
      counterMu++;
    }

    if (!(matchEl || matchMu)) goodTrackJets.push_back(counterTrk);
    counterTrk++;
  }
}

void TopObjectSelection::decorateMuonsPostOverlapRemoval(const xAOD::MuonContainer* xaod_mu, 
							 const xAOD::JetContainer* xaod_jet,
							 std::vector<unsigned int>& goodMuons,
							 std::vector<unsigned int>& goodJets){
  // Decorate muons with the dR of closest jet (after OR is applied)
  // Use the good indicies to loop through the good objects
  for(auto iMu : goodMuons){
    // Get muon iMu
    const xAOD::Muon* muPtr = xaod_mu->at(iMu);
    float dRMin = 100.0;
    // Loop over jets, calculate dR and record smallest value
    for(auto iJet : goodJets){
      const xAOD::Jet* jetPtr = xaod_jet->at(iJet);
      float dR = muPtr->p4().DeltaR(jetPtr->p4()); 
      if(dR < dRMin) dRMin = dR; 
    }
    // Decorate the good muon with dR of closest jet (ie smallest dR)
    muPtr->auxdecor< float >("dRJet") = dRMin;
  }
  return;
}

void TopObjectSelection::decorateEventInfoPostOverlapRemoval(int nGoodJets, bool isLoose){
    ///-- Decorate event info with number of nominal jets for flavour composition
    // Only continue if this is nominal processing
    if(!m_executeNominal) return;
    bool UseLooseNJets = false;
    // If we only run on loose nominal, we need to use loose event, otherwise use tight
    if (m_config->doLooseEvents() && !m_config->doTightEvents()) UseLooseNJets = true;
    // If we use loose, but this is tight, skip
    if (UseLooseNJets && !isLoose) return;
    // Get the EventInfo object
    const xAOD::EventInfo* eventInfo(nullptr);
    top::check( evtStore()->retrieve( eventInfo, m_config->sgKeyEventInfo() ), "Failed to retrieve EventInfo");
    // Decorate with number of good jets
    eventInfo->auxdecor< int >("Njet") = nGoodJets;
    
    return;
}
    
    
void TopObjectSelection::print() const
{
  asg::AsgTool::print();
}

void TopObjectSelection::print(std::ostream& os) const {
    os << "TopObjectSelection configuration\n";

    os << "\n";
    os << "Electrons\n";
    os << "  ContainerName: " << m_config->sgKeyElectrons() << "\n";
    if (m_config->useElectrons()) {
        os << "  Selection: ";
        if (!m_electronSelection)
            os << "All";
        else
            os << *m_electronSelection;
    }

    os << "\n";
    os << "Photons\n";
    os << "  ContainerName: " << m_config->sgKeyPhotons() << "\n";
    if (m_config->usePhotons()) {
        os << "  Selection: ";
        if (!m_photonSelection)
            os << "All";
        else
            os << *m_photonSelection;
    }

    os << "\n";
    os << "Muons\n";
    os << "  ContainerName: " << m_config->sgKeyMuons() << "\n";
    if (m_config->useMuons()) {
        os << "  Selection: ";
        if (!m_muonSelection)
            os << "All";
        else
            os << *m_muonSelection;
    }
//
//    os << "\n";
//    os << "Taus\n";
//    os << "  ContainerName: " << containerNames().tauCollectionName << "\n";
//    if (containerNames().tauCollectionName != "None") {
//        os << "  Selection: ";
//        if (!m_tauSelection)
//            os << "All";
//        else
//            os << *m_tauSelection;
//    }

    os << "\n";
    os << "Jets\n";
    os << "  ContainerName: " << m_config->sgKeyJets() << "\n";
    if (m_config->useJets()) {
        os << "  Selection: ";
        if (!m_jetSelection)
            os << "All";
        else
            os << *m_jetSelection;
    }

    os << "\n";
    os << "LargeJets\n";
    os << "  ContainerName: " << m_config->sgKeyLargeRJets() << "\n";
    if (m_config->useLargeRJets()) {
        os << "  Selection: ";
        if (!m_largeJetSelection)
            os << "All";
        else
            os << *m_largeJetSelection;
    }

    os << "\n";
    os << "MET\n";
    os << "  ContainerName: " << m_config->sgKeyMissingEt() << "\n";

    os << "\n\n";
    os << "OverlapRemoval after object selection: ";
    if (!m_overlapRemovalToolPostSelection)
        os << "None";
    else
        os << *m_overlapRemovalToolPostSelection;

    os << "\n\n";
}

}

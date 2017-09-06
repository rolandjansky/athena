/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopAnalysis/EventSaverxAOD.h"

#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"
#include "TopEvent/SystematicEvent.h"

#include "TopPartons/PartonHistory.h"

#include "TopConfiguration/TopConfig.h"

#include "xAODRootAccess/TEvent.h"

#include "TFile.h"

namespace top {

EventSaverxAOD::EventSaverxAOD() : 
  asg::AsgTool( "top::EventSaverxAOD" ),
  m_prefix("CorrectedSelected"), 
  m_config(nullptr), 
  m_outputFile(nullptr) {
}

EventSaverxAOD::~EventSaverxAOD() 
{
}

void EventSaverxAOD::initialize(std::shared_ptr<top::TopConfig> config, TFile* file, const std::vector<std::string>& extraBranches) {
    m_config = config;
    m_outputFile = file;
    top::check(evtStore()->event()->writeTo(m_outputFile), "EventSaverxAOD trying to call writeTo on output file");

    //EventInfo
    std::string eventInfoList = "runNumber.eventNumber.eventTypeBitmask.averageInteractionsPerCrossing";
    if (config->isMC())
        eventInfoList += ".mcChannelNumber.mcEventWeights.PileupWeight";

    //Add extra branches - e.g. if it passed the ee / mumu / emu selection per event
    for (const auto& branchName : extraBranches)
        eventInfoList += "." + branchName;

    //std::cout << "EventLoaderCorrectSelect::branchesToSave - EventInfo " << eventInfoList << std::endl;
    evtStore()->event()->setAuxItemList("EventInfoAux.", eventInfoList);

    //Electrons
    evtStore()->event()->setAuxItemList(m_prefix + m_config->sgKeyElectrons() + "Aux.", "pt.eta.phi.m.charge");

    //Muons
    evtStore()->event()->setAuxItemList(m_prefix + m_config->sgKeyMuons() + "Aux.", "pt.eta.phi.m.charge.muonType.combinedTrackParticleLink");

    //Once mu has a built-in charge we can get rid of this
    //Muon charge is a property of the associated track - e.g. d0.z0.phi.theta.qOverP
    evtStore()->event()->setAuxItemList("InDetTrackParticlesAux.", "qOverP");
    evtStore()->event()->setAuxItemList("ExtrapolatedMuonTrackParticlesAux.", "qOverP");
    evtStore()->event()->setAuxItemList("CombinedMuonTrackParticlesAux.", "qOverP");

    //Jets
    evtStore()->event()->setAuxItemList(m_prefix + m_config->sgKeyJets() + "Aux.", "pt.eta.phi.m.btaggingLink");

    //b-tagging
    const std::string btagName = "BTagging_" + m_config->sgKeyJets().substr(0, m_config->sgKeyJets().size() - 4);
    evtStore()->event()->setAuxItemList(btagName + "Aux.", "MV1_discriminant");

    //MET seems to be pretty small already
}

void EventSaverxAOD::saveEvent(const top::Event& event) {
    //only save the nominal variation for now - speak to Attila about TEvent.fill not taking a tree name
    if (event.m_hashValue != m_config->nominalHashValue())
        return;
    
    // record the event?
    if (m_config->saveOnlySelectedEvents() && !event.m_saveEvent)
        return;    
    
    // Systematic Event 
    const xAOD::SystematicEventContainer* allSystematics(nullptr);
    top::check(evtStore()->retrieve(allSystematics,m_config->sgKeyTopSystematicEvents()) , "Failed to retrieve xAOD::SystematicEventContainer" );
         
    xAOD::SystematicEventContainer* allSystematics_output = new xAOD::SystematicEventContainer{};
    SG::IAuxStore* allSystematics_aux = evtStore()->event()->recordAux( m_config->sgKeyTopSystematicEvents() + "Aux." );
    allSystematics_output->setStore( allSystematics_aux );
    for (auto currentSystematic : *allSystematics) {
      xAOD::SystematicEvent* out = new xAOD::SystematicEvent{};
      out->makePrivateStore( *currentSystematic );
      allSystematics_output->push_back( out );
    }
    top::check(evtStore()->event()->record(allSystematics_output,m_config->sgKeyTopSystematicEvents()),"Failed to record xAOD::SystematicEventContainer" );
    
    
    // Top Parton History 
    if (m_config->doTopPartonHistory()) {
      if (evtStore()->contains<xAOD::PartonHistoryContainer>(m_config->sgKeyTopPartonHistory())) {
        const xAOD::PartonHistoryContainer* partonHistory(nullptr);
        top::check(evtStore()->retrieve(partonHistory, m_config->sgKeyTopPartonHistory()),"Failed to retrieve Top Parton History"); 
        
        xAOD::PartonHistoryContainer* partonHistory_output = new xAOD::PartonHistoryContainer{};
        SG::IAuxStore* partonHistory_aux = evtStore()->event()->recordAux( m_config->sgKeyTopPartonHistory() + "Aux." );
        partonHistory_output->setStore( partonHistory_aux );
        for (auto x : *partonHistory) {
          xAOD::PartonHistory* out = new xAOD::PartonHistory{};
          out->makePrivateStore( *x );
          partonHistory_output->push_back( out );
        }
        top::check(evtStore()->event()->record(partonHistory_output,m_config->sgKeyTopPartonHistory()),"Failed to record Parton History" );
      }
    }
    
    

    //Event Info
    top::check(evtStore()->event()->copy("EventInfo"), "copying event info failed");

    //Electrons
    if (m_config->useElectrons()) {
        xAOD::ElectronContainer* electrons_output = new xAOD::ElectronContainer();
        SG::IAuxStore* electrons_aux = evtStore()->event()->recordAux(m_prefix + m_config->sgKeyElectrons() + "Aux.");
        electrons_output->setStore(electrons_aux);

        for (const auto* const elPtr : event.m_electrons) {
            xAOD::Electron* outputElPtr = new xAOD::Electron();
            outputElPtr->makePrivateStore(*elPtr);
            electrons_output->push_back(outputElPtr);
        }

        top::check(evtStore()->event()->record(electrons_output, m_prefix + m_config->sgKeyElectrons()), "xAOD::TEvent record " + m_prefix + m_config->sgKeyElectrons() + " failed");
    }

    //Muons
    if (m_config->useMuons()) {
        xAOD::MuonContainer* muons_output = new xAOD::MuonContainer();
        SG::IAuxStore* muons_aux = evtStore()->event()->recordAux(m_prefix + m_config->sgKeyMuons() + "Aux.");
        muons_output->setStore(muons_aux);

        for (const auto* const muPtr : event.m_muons) {
            xAOD::Muon* outputMuPtr = new xAOD::Muon();
            outputMuPtr->makePrivateStore(*muPtr);
            muons_output->push_back(outputMuPtr);
        }

        top::check(evtStore()->event()->record(muons_output, m_prefix + m_config->sgKeyMuons()), "xAOD::TEvent record " + m_prefix + m_config->sgKeyMuons() + " failed");

        //for now...
        //Muon charge is a property of the associated track
        //Copy tracks (for muon charge)
        check(evtStore()->event()->copy("InDetTrackParticles"), "xAOD::TEvent copy InDetTrackParticles failed");
        check(evtStore()->event()->copy("ExtrapolatedMuonTrackParticles"), "xAOD::TEvent copy InDetTrackParticles ExtrapolatedMuonTrackParticles");
        check(evtStore()->event()->copy("CombinedMuonTrackParticles"), "xAOD::TEvent copy InDetTrackParticles CombinedMuonTrackParticles");
    }

    //Jets
    if (m_config->useJets()) {
        xAOD::JetContainer* jets_output = new xAOD::JetContainer();
        SG::IAuxStore* jets_aux = evtStore()->event()->recordAux(m_prefix + m_config->sgKeyJets() + "Aux.");
        jets_output->setStore(jets_aux);

        for (const auto* const jetPtr : event.m_jets) {
            xAOD::Jet* outputJetPtr = new xAOD::Jet();
            outputJetPtr->makePrivateStore(*jetPtr);
            jets_output->push_back(outputJetPtr);
        }

        top::check(evtStore()->event()->record(jets_output, m_prefix + m_config->sgKeyJets()), "xAOD::TEvent record " + m_prefix + m_config->sgKeyJets() + " failed");

        //b-tagging variables need the BTagging_JetNameWithoutJetsOnTheEnd container
        //remove Jets from the end of the container name and add BTagging_
        const std::string btagName = "BTagging_" + m_config->sgKeyJets().substr(0, m_config->sgKeyJets().size() - 4);
        check(evtStore()->event()->copy(btagName), "xAOD::TEvent copy " + btagName + " failed");
    }

    //Large Jets
    if (m_config->useLargeRJets()) {
        xAOD::JetContainer* largeJets_output = new xAOD::JetContainer();
        SG::IAuxStore* largeJets_aux = evtStore()->event()->recordAux(m_prefix + m_config->sgKeyLargeRJets() + "Aux.");
        largeJets_output->setStore(largeJets_aux);

        for (const auto* const jetPtr : event.m_largeJets) {
            xAOD::Jet* outputJetPtr = new xAOD::Jet();
            outputJetPtr->makePrivateStore(*jetPtr);
            largeJets_output->push_back(outputJetPtr);
        }

        check(evtStore()->event()->record(largeJets_output, m_prefix +  m_config->sgKeyLargeRJets()), "xAOD::TEvent record " + m_prefix +  m_config->sgKeyLargeRJets() + " failed");
    }

    //MET
    top::check(evtStore()->event()->copy(m_config->sgKeyMissingEt()), "xAOD::TEvent copy " + m_config->sgKeyMissingEt() + " failed");

    //do it!
    top::check(evtStore()->event()->fill(), "xAOD::TEvent fill failed");
}

void EventSaverxAOD::finalize() {
    m_outputFile->Write();
    top::check(evtStore()->event()->finishWritingTo(m_outputFile), "EventSaverxAOD::finalise trying to call finishWritingTo");
}

}

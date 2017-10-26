/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/ExamplePlots.h"

#include "TopEvent/EventTools.h"
#include "TopConfiguration/TopConfig.h"

#include "TopCorrections/ScaleFactorRetriever.h"

#include <cmath>
#include <array>

#include "TH1.h"
#include "TFile.h"

// Systematic include(s):
#include "PATInterfaces/SystematicSet.h"

namespace top {

const double ExamplePlots::toGeV = 0.001;

  ExamplePlots::ExamplePlots(const std::string& name, TFile* outputFile, EL::Worker* wk, std::shared_ptr<top::TopConfig> config) :
    m_hists(name, outputFile, wk), m_nominalHashValue(0) {

    m_config = config;
    CP::SystematicSet nominal; 
    m_nominalHashValue = nominal.hash();

    m_hists.addHist("event_mu", ";<#mu>;Events", 25, 0., 50.);
    if (m_config->isMC()) {
      m_hists.addHist("mc_weight", ";MC Event Weight", 30, -1, 2);
      m_hists.addHist("jvt_SF", ";JVT SF", 120, -0.5, 2.5);
    }

    m_hists.addHist("pileup_weight", ";Pileup Weight", 20, -1, 5);

    m_hists.addHist("el_n", ";Electron multiplicity;Events", 10, -0.5, 9.5);
    m_hists.addHist("el_pt", ";Electron p_{T} / GeV;Electrons", 20, 0., 250.);
    m_hists.addHist("el_eta", ";Electron #eta;Electrons", 20, -2.5, 2.5);
    m_hists.addHist("el_phi", ";Electron #phi;Electrons", 20, -M_PI, M_PI);
    m_hists.addHist("el_charge", ";Electron charge;Electrons", 3, -1.5, 1.5);
    m_hists.addHist("el_trkpart_charge", ";Electron TrackParticle charge;Electrons", 3, -1.5, 1.5);
    if (m_config->isMC()) m_hists.addHist("el_true_type", ";Electron true type;Electrons", 20, 0., 20.);
    m_hists.addHist("el_topoetcone20", ";Electron topoetcone20 / GeV;Electrons", 20, 0., 20.);
    m_hists.addHist("el_topoetcone30", ";Electron topoetcone30 / GeV;Electrons", 20, 0., 20.);
    m_hists.addHist("el_topoetcone40", ";Electron topoetcone40 / GeV;Electrons", 20, 0., 20.);
    m_hists.addHist("el_ptcone20", ";Electron ptcone20 / GeV;Electrons", 20, 0., 10.);
    m_hists.addHist("el_ptcone30", ";Electron ptcone30 / GeV;Electrons", 20, 0., 10.);
    m_hists.addHist("el_ptcone40", ";Electron ptcone40 / GeV;Electrons", 20, 0., 10.);
    m_hists.addHist("el_ptvarcone20", ";Electron ptvarcone20 / GeV;Electrons", 20, 0., 10.);
    m_hists.addHist("el_ptvarcone30", ";Electron ptvarcone30 / GeV;Electrons", 20, 0., 10.);
    m_hists.addHist("el_ptvarcone40", ";Electron ptvarcone40 / GeV;Electrons", 20, 0., 10.);

    m_hists.addHist("mu_n", ";Muon multiplicity;Events", 10, -0.5, 9.5);
    m_hists.addHist("mu_pt", ";Muon p_{T} / GeV;Muons", 20, 0., 250.);
    m_hists.addHist("mu_eta", ";Muon #eta;Muons", 20, -2.5, 2.5);
    m_hists.addHist("mu_phi", ";Muon #phi;Muons", 20, -M_PI, M_PI);
    m_hists.addHist("mu_charge", ";Muon charge;Muons", 3, -1.5, 1.5);
    if (m_config->isMC()) m_hists.addHist("mu_true_type", ";Muon true type;Muons", 20, 0., 20.);
    m_hists.addHist("mu_topoetcone20", ";Muon topoetcone20 / GeV;Muons", 20, 0., 20.);
    m_hists.addHist("mu_topoetcone30", ";Muon topoetcone30 / GeV;Muons", 20, 0., 20.);
    m_hists.addHist("mu_topoetcone40", ";Muon topoetcone40 / GeV;Muons", 20, 0., 20.);
    m_hists.addHist("mu_ptcone20", ";Muon ptcone20 / GeV;Muons", 20, 0., 10.);
    m_hists.addHist("mu_ptcone30", ";Muon ptcone30 / GeV;Muons", 20, 0., 10.);
    m_hists.addHist("mu_ptcone40", ";Muon ptcone40 / GeV;Muons", 20, 0., 10.);
    m_hists.addHist("mu_ptvarcone20", ";Muon ptvarcone20 / GeV;Muons", 20, 0., 10.);
    m_hists.addHist("mu_ptvarcone30", ";Muon ptvarcone30 / GeV;Muons", 20, 0., 10.);
    m_hists.addHist("mu_ptvarcone40", ";Muon ptvarcone40 / GeV;Muons", 20, 0., 10.);

    m_hists.addHist("ph_n", ";Photon multiplicity;Events", 10, -0.5, 9.5);
    m_hists.addHist("ph_pt", ";Photon p_{T} / GeV;Photons", 20, 0., 250.);
    m_hists.addHist("ph_eta", ";Photon #eta;Photons", 20, -2.5, 2.5);
    m_hists.addHist("ph_phi", ";Photon #phi;Photons", 20, -M_PI, M_PI);
    m_hists.addHist("ph_e", ";Photon e / GeV;Photons", 3, -1.5, 1.5);
    m_hists.addHist("ph_ptvarcone20", ";Photon ptvarcone20 / GeV;Photons", 20, 0., 10.);

    m_hists.addHist("jet_n", ";Jet multiplicity;Events", 10, -0.5, 9.5);
    m_hists.addHist("jet_pt", ";Jet p_{T} / GeV;Jets", 40, 0., 500.);
    m_hists.addHist("jet_e", ";Jet e / GeV;Jets", 40, 0., 1000.);
    m_hists.addHist("jet_eta", ";Jet #eta;Jets", 20, -2.5, 2.5);
    m_hists.addHist("jet_phi", ";Jet #phi;Jets", 20, -M_PI, M_PI);
    m_hists.addHist("jet_mv2c10", ";Jet mv2c10;Jets", 20, -1., 1.);
    if (m_config->isMC()) {
      m_hists.addHist("jet_truthflav", ";Jet truth flavor;Jets", 20, 0., 20.);
      m_hists.addHist("jet_btagSF_MV2c10_77", ";Jet btag SF;Jets", 100, 0.5, 1.5);

    }
    m_hists.addHist("jet_isbtagged_MV2c10_77", ";Jet is b-tagged;Jets", 2, 0., 2.);

    m_hists.addHist("jet0_pt", ";Jet0 p_{T} / GeV; Events / 10 GeV", 25, 5, 505);
    m_hists.addHist("jet0_eta", ";Jet0 #eta; Jets", 25, -2.5, 2.5);
    m_hists.addHist("jet0_phi", ";Jet0 #phi; Jets", 25, -M_PI, M_PI);
    m_hists.addHist("jet0_e", ";Jet0 E / GeV; Jets", 100, 0, 1000);
    m_hists.addHist("jet0_mv2c10", ";Jet0 mv2c10; Jets", 50, 0, 1);
    if (m_config->isMC()) {
      m_hists.addHist("jet0_truthflav", ";Jet0 truth flavor;Jets", 20, 0., 20.);
      m_hists.addHist("jet0_btagSF_MV2c10_77", ";Jet0 btag SF;Jets", 100, 0.5, 1.5);
    }
    m_hists.addHist("jet0_isbtagged_MV2c10_77", ";Jet is b-tagged;Jets", 2, 0., 2.);

    m_hists.addHist("jet1_pt", ";Jet1 p_{T} / GeV; Events / 10 GeV", 25, 5, 505);
    m_hists.addHist("jet1_eta", ";Jet1 #eta; Jets", 25, -2.5, 2.5);
    m_hists.addHist("jet1_phi", ";Jet1 #phi; Jets", 25, -M_PI, M_PI);
    m_hists.addHist("jet1_e", ";Jet1 E / GeV; Jets", 100, 0, 1000);
    m_hists.addHist("jet1_mv2c10", ";Jet1 mv2c10; Jets", 50, 0, 1);
    if (m_config->isMC()) {
      m_hists.addHist("jet1_truthflav", ";Jet1 truth flavor;Jets", 20, 0., 20.);
      m_hists.addHist("jet1_btagSF_MV2c10_77", ";Jet1 btag SF;Jets", 100, 0.5, 1.5);
    }
    m_hists.addHist("jet1_isbtagged_MV2c10_77", ";Jet is b-tagged;Jets", 2, 0., 2.);

    m_hists.addHist("jet2_pt", ";Jet2 p_{T} / GeV; Events / 10 GeV", 25, 5, 505);
    m_hists.addHist("jet2_eta", ";Jet2 #eta; Jets", 25, -2.5, 2.5);
    m_hists.addHist("jet2_phi", ";Jet2 #phi; Jets", 25, -M_PI, M_PI);
    m_hists.addHist("jet2_e", ";Jet2 E / GeV; Jets", 100, 0, 1000);
    m_hists.addHist("jet2_mv2c10", ";Jet2 mv2c10; Jets", 50, 0, 1);
    if (m_config->isMC()) {
      m_hists.addHist("jet2_truthflav", ";Jet2 truth flavor;Jets", 20, 0., 20.);
      m_hists.addHist("jet2_btagSF_MV2c10_77", ";Jet2 btag SF;Jets", 100, 0.5, 1.5);
    }
    m_hists.addHist("jet2_isbtagged_MV2c10_77", ";Jet is b-tagged;Jets", 2, 0., 2.);

    m_hists.addHist("jet3_pt", ";Jet3 p_{T} / GeV; Events / 10 GeV", 25, 5, 505);
    m_hists.addHist("jet3_eta", ";Jet3 #eta; Jets", 25, -2.5, 2.5);
    m_hists.addHist("jet3_phi", ";Jet3 #phi; Jets", 25, -M_PI, M_PI);
    m_hists.addHist("jet3_e", ";Jet3 E / GeV; Jets", 100, 0, 1000);
    m_hists.addHist("jet3_mv2c10", ";Jet3 mv2c10; Jets", 50, 0, 1);
    if (m_config->isMC()) {
      m_hists.addHist("jet3_truthflav", ";Jet3 truth flavor;Jets", 20, 0., 20.);
      m_hists.addHist("jet3_btagSF_MV2c10_77", ";Jet3 btag SF;Jets", 100, 0.5, 1.5);
    }
    m_hists.addHist("jet3_isbtagged_MV2c10_77", ";Jet is b-tagged;Jets", 2, 0., 2.);

    //Large-R jet
    m_hists.addHist("ljet_n", ";Large-R Jet multiplicity; Jets ", 5, 0, 5);
    m_hists.addHist("ljet_pt", ";Large-R Jet p_{T} / GeV; Jets / 26 GeV", 50, 200, 1500);
    m_hists.addHist("ljet_eta", ";Large-R Jet #eta; Jets", 15, -2., 2.);
    m_hists.addHist("ljet_phi", ";Large-R Jet #phi; Jets", 25, -M_PI, M_PI);
    m_hists.addHist("ljet_m", ";Large-R mass / GeV; Jets/ 10 GeV", 35, 50, 400);
    m_hists.addHist("ljet_sd12", "Large-R #sqrt{d_{12}} / GeV; Jets / 10 GeV", 20, 0, 200);

    m_hists.addHist("taujet_n", ";TauJet multiplicity;Events", 20, -0.5, 19.5);
    m_hists.addHist("taujet_pt", ";TauJet p_{T} / GeV;TauJet", 20, 0., 200.);
    m_hists.addHist("taujet_eta", ";TauJet #eta;TauJet", 20, -2.5, 2.5);
    m_hists.addHist("taujet_phi", ";TauJet #phi;TauJet", 20, -M_PI, M_PI);
    m_hists.addHist("taujet_charge", ";TauJet charge;TauJet", 3, -1.5, 1.5);

    m_hists.addHist("event_met_et", ";MET / GeV;Events", 50, 0., 400.);
    m_hists.addHist("event_met_phi", ";MET #phi;Events", 20, -M_PI, M_PI);
}

bool ExamplePlots::apply(const top::Event& event) const {
    //only nominal
    
    if (event.m_hashValue != m_nominalHashValue)
        return true;

    //only plot tight selection
    //before, we were plotting tight and loose together
    if (event.m_isLoose)
        return true;

    //std::cout << event << std::endl;

    double eventWeight = 1.;
    if (top::isSimulation(event))
//         eventWeight = event.m_info->mcEventWeight();
        eventWeight = event.m_truthEvent->at(0)->weights()[0];// FIXME temporary bugfix

    if (m_config->isMC()) {
      m_hists.hist("mc_weight")->Fill(eventWeight, eventWeight);
      m_hists.hist("jvt_SF")->Fill(event.m_jvtSF, eventWeight);

      //pileup weight needs pileup reweighting tool to have run
      if (top::ScaleFactorRetriever::hasPileupSF(event))
          m_hists.hist("pileup_weight")->Fill(top::ScaleFactorRetriever::pileupSF(event), eventWeight);
    }

    m_hists.hist("event_mu")->Fill(event.m_info->averageInteractionsPerCrossing(), eventWeight);

    m_hists.hist("el_n")->Fill(event.m_electrons.size(), eventWeight);
    for (const auto* const elPtr : event.m_electrons) {
        m_hists.hist("el_pt")     -> Fill(elPtr->pt() * toGeV, eventWeight);
        m_hists.hist("el_eta")    -> Fill(elPtr->eta(),        eventWeight);
        m_hists.hist("el_phi")    -> Fill(elPtr->phi(),        eventWeight);
        m_hists.hist("el_charge") -> Fill(elPtr->charge(),     eventWeight);

        //protection from the derivation framework -
        //So far we keep tracks associated with "good" electrons.
        //It could be you have a super-loose selection here (just for testing)
        //so we need to check if the electron has a track associated with it
        if (elPtr->trackParticle())
            m_hists.hist("el_trkpart_charge")->Fill(elPtr->trackParticle()->charge(), eventWeight);

	//retrieve the truth-matching variables from MCTruthClassifier
	if (m_config->isMC()) {
	  static SG::AuxElement::Accessor<int> typeel("truthType");
	  if (typeel.isAvailable(*elPtr)) m_hists.hist("el_true_type")->Fill(typeel(*elPtr), eventWeight);
	}

        //testing isolation in the derivation framework
        float iso = 0;

        if (elPtr->isolationValue(iso, xAOD::Iso::topoetcone20))
            m_hists.hist("el_topoetcone20")->Fill(iso * toGeV, eventWeight);

        if (elPtr->isolationValue(iso, xAOD::Iso::topoetcone30))
            m_hists.hist("el_topoetcone30")->Fill(iso * toGeV, eventWeight);

        if (elPtr->isolationValue(iso, xAOD::Iso::topoetcone40))
            m_hists.hist("el_topoetcone40")->Fill(iso * toGeV, eventWeight);

        if (elPtr->isolationValue(iso, xAOD::Iso::ptcone20))
            m_hists.hist("el_ptcone20")->Fill(iso * toGeV, eventWeight);

        if (elPtr->isolationValue(iso, xAOD::Iso::ptcone30))
            m_hists.hist("el_ptcone30")->Fill(iso * toGeV, eventWeight);

        if (elPtr->isolationValue(iso, xAOD::Iso::ptcone40))
            m_hists.hist("el_ptcone40")->Fill(iso * toGeV, eventWeight);

        if (elPtr->isAvailable<float>("ptvarcone20")) {
            float ptvarcone20 = elPtr->auxdataConst<float>("ptvarcone20");
            m_hists.hist("el_ptvarcone20")->Fill(ptvarcone20 * toGeV, eventWeight);
        }

        if (elPtr->isAvailable<float>("ptvarcone30")) {
            float ptvarcone30 = elPtr->auxdataConst<float>("ptvarcone30");
            m_hists.hist("el_ptvarcone30")->Fill(ptvarcone30 * toGeV, eventWeight);
        }

        if (elPtr->isAvailable<float>("ptvarcone40")) {
            float ptvarcone40 = elPtr->auxdataConst<float>("ptvarcone40");
            m_hists.hist("el_ptvarcone40")->Fill(ptvarcone40 * toGeV, eventWeight);
        }
    }

    m_hists.hist("mu_n")->Fill(event.m_muons.size(), eventWeight);
    for (const auto* const muPtr : event.m_muons) {
        m_hists.hist("mu_pt")->Fill(muPtr->pt() * toGeV, eventWeight);
        m_hists.hist("mu_eta")->Fill(muPtr->eta(), eventWeight);
        m_hists.hist("mu_phi")->Fill(muPtr->phi(), eventWeight);

        //protection from derivation framework removing tracks
	const xAOD::TrackParticle* mutrack = muPtr->primaryTrackParticle();
        if (mutrack!=nullptr) {
	    m_hists.hist("mu_charge")->Fill(mutrack->charge(), eventWeight);
	    if (m_config->isMC()) {
	        static SG::AuxElement::Accessor<int> acc_mctt("truthType");
		if (acc_mctt.isAvailable(*mutrack)) m_hists.hist("mu_true_type")->Fill(acc_mctt(*mutrack), eventWeight);
	    }
	}

        //testing isolation in the derivation framework
        float iso = 0;

        if (muPtr->isAvailable<float>("topoetcone20") && muPtr->isolation(iso, xAOD::Iso::topoetcone20))
            m_hists.hist("mu_topoetcone20")->Fill(iso * toGeV, eventWeight);

        if (muPtr->isAvailable<float>("topoetcone30") && muPtr->isolation(iso, xAOD::Iso::topoetcone30))
            m_hists.hist("mu_topoetcone30")->Fill(iso * toGeV, eventWeight);

        if (muPtr->isAvailable<float>("topoetcone40") && muPtr->isolation(iso, xAOD::Iso::topoetcone40))
            m_hists.hist("mu_topoetcone40")->Fill(iso * toGeV, eventWeight);

        if (muPtr->isolation(iso, xAOD::Iso::ptcone20))
            m_hists.hist("mu_ptcone20")->Fill(iso * toGeV, eventWeight);

        if (muPtr->isolation(iso, xAOD::Iso::ptcone30))
            m_hists.hist("mu_ptcone30")->Fill(iso * toGeV, eventWeight);

        if (muPtr->isolation(iso, xAOD::Iso::ptcone40))
            m_hists.hist("mu_ptcone40")->Fill(iso * toGeV, eventWeight);

        if (muPtr->isAvailable<float>("ptvarcone20")) {
            float ptvarcone20 = muPtr->auxdataConst<float>("ptvarcone20");
            m_hists.hist("mu_ptvarcone20")->Fill(ptvarcone20 * toGeV, eventWeight);
        }

        if (muPtr->isAvailable<float>("ptvarcone30")) {
            float ptvarcone30 = muPtr->auxdataConst<float>("ptvarcone30");
            m_hists.hist("mu_ptvarcone30")->Fill(ptvarcone30 * toGeV, eventWeight);
        }

        if (muPtr->isAvailable<float>("ptvarcone40")) {
            float ptvarcone40 = muPtr->auxdataConst<float>("ptvarcone40");
            m_hists.hist("mu_ptvarcone40")->Fill(ptvarcone40 * toGeV, eventWeight);
        }
    }

    m_hists.hist("ph_n")->Fill(event.m_photons.size(), eventWeight);
    for (const auto* const phPtr : event.m_photons) {
        m_hists.hist("ph_pt")     -> Fill(phPtr->pt() * toGeV, eventWeight);
        m_hists.hist("ph_eta")    -> Fill(phPtr->eta(),        eventWeight);
        m_hists.hist("ph_phi")    -> Fill(phPtr->phi(),        eventWeight);
        m_hists.hist("ph_e")      -> Fill(phPtr->e() * toGeV,  eventWeight);
        if (phPtr->isAvailable<float>("ptvarcone20")) {
            float ptvarcone20 = phPtr->auxdataConst<float>("ptvarcone20");
            m_hists.hist("ph_ptvarcone20")->Fill(ptvarcone20 * toGeV, eventWeight);
        }
    }

    m_hists.hist("jet_n")->Fill(event.m_jets.size(), eventWeight);
    unsigned int i = 0;
    std::array<std::string, 4> numbers{ {"jet0", "jet1", "jet2", "jet3"} };

    for (const auto* const jetPtr : event.m_jets) {
        m_hists.hist("jet_pt")     -> Fill(jetPtr->pt() * toGeV, eventWeight);
        m_hists.hist("jet_eta")    -> Fill(jetPtr->eta(), eventWeight);
        m_hists.hist("jet_phi")    -> Fill(jetPtr->phi(), eventWeight);
        m_hists.hist("jet_e")      -> Fill(jetPtr->e() * toGeV, eventWeight);

        double mv2c10_discriminant = 0.;
        const bool hasmv2c10 = jetPtr->btagging()->MVx_discriminant("MV2c10", mv2c10_discriminant);
        if (hasmv2c10){
            m_hists.hist("jet_mv2c10")->Fill(mv2c10_discriminant, eventWeight);
	    
	}

        int jet_truthflav = -1;
	if (m_config->isMC()) {
	    if(jetPtr->isAvailable<int>("HadronConeExclTruthLabelID")){
	        jetPtr->getAttribute("HadronConeExclTruthLabelID", jet_truthflav);
                m_hists.hist("jet_truthflav")->Fill(jet_truthflav, eventWeight);
	    }
	}

	int isbtagged = 0;
        bool hasBtag = false;
        float btagSF = 1.;
        bool hasBtagSF = false;
        hasBtag = jetPtr->isAvailable<char>("isbtagged_MV2c10_FixedCutBEff_77");
        if(hasBtag) {
          isbtagged = jetPtr->auxdataConst<char>("isbtagged_MV2c10_FixedCutBEff_77");
          m_hists.hist("jet_isbtagged_MV2c10_77")->Fill(isbtagged, eventWeight);
          if (m_config->isMC()) {
            if (jetPtr -> isAvailable<float>("btag_SF_MV2c10_FixedCutBEff_77_nom")) {
              btagSF = jetPtr -> auxdataConst<float>("btag_SF_MV2c10_FixedCutBEff_77_nom");
              m_hists.hist("jet_btagSF_MV2c10_77")->Fill(btagSF, eventWeight);
              hasBtagSF = true;
            }
          }
        }

        if (i < numbers.size()) {
            m_hists.hist(numbers[i] + "_pt")     -> Fill(jetPtr->pt() * toGeV, eventWeight);
            m_hists.hist(numbers[i] + "_eta")    -> Fill(jetPtr->eta(),        eventWeight);
            m_hists.hist(numbers[i] + "_phi")    -> Fill(jetPtr->phi(),        eventWeight);
            m_hists.hist(numbers[i] + "_e")      -> Fill(jetPtr->e() * toGeV,  eventWeight);
            m_hists.hist(numbers[i] + "_mv2c10") -> Fill(mv2c10_discriminant, eventWeight);
	    if(hasBtag) m_hists.hist(numbers[i] + "_isbtagged_MV2c10_77")->Fill(isbtagged, eventWeight);
            if (m_config->isMC()) {
              if (hasBtagSF) m_hists.hist(numbers[i] + "_btagSF_MV2c10_77")->Fill(btagSF, eventWeight);
              m_hists.hist(numbers[i] + "_truthflav")->Fill(jet_truthflav, eventWeight);
            }
        }


        ++i;
    }

    m_hists.hist("ljet_n") -> Fill(event.m_largeJets.size(), eventWeight);
    for (const auto* const jetPtr : event.m_largeJets) {
        m_hists.hist("ljet_pt")     -> Fill(jetPtr->pt() * toGeV, eventWeight);
        m_hists.hist("ljet_eta")    -> Fill(jetPtr->eta(), eventWeight);
        m_hists.hist("ljet_phi")    -> Fill(jetPtr->phi(), eventWeight);
        m_hists.hist("ljet_m")      -> Fill(jetPtr->m() * toGeV, eventWeight);
        
        float Split12 = 0;
        jetPtr->getAttribute("Split12", Split12);
        m_hists.hist("ljet_sd12")   -> Fill(Split12 * toGeV, eventWeight);

    }

    m_hists.hist("taujet_n")->Fill(event.m_tauJets.size(), eventWeight);
    for (const auto* const tauPtr : event.m_tauJets) {
        m_hists.hist("taujet_pt")     -> Fill(tauPtr->pt() * toGeV, eventWeight);
        m_hists.hist("taujet_eta")    -> Fill(tauPtr->eta(),        eventWeight);
        m_hists.hist("taujet_phi")    -> Fill(tauPtr->phi(),        eventWeight);
        m_hists.hist("taujet_charge") -> Fill(tauPtr->charge(),     eventWeight);
    }

    if (event.m_met != nullptr) {
        m_hists.hist("event_met_et")  -> Fill(event.m_met->met() * toGeV, eventWeight);
        m_hists.hist("event_met_phi") -> Fill(event.m_met->phi(),         eventWeight);
    }

    return true;
}

std::string ExamplePlots::name() const {
    return "EXAMPLEPLOTS";
}

}

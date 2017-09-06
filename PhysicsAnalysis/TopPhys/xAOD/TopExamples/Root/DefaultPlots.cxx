/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopExamples/DefaultPlots.h"

#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"
#include "TopEventSelectionTools/PlotManager.h"

#include "TopCorrections/ScaleFactorRetriever.h"

#include <cmath>
#include <array>

#include "TH1.h"

namespace top {

void addPlots(top::PlotManager& manager) {
    manager.addHist("control_sig_njet", ";Number of selected jets;Events", 12, -0.5, 11.5);
    manager.addHist("control_sig_met", ";E_{T}^{miss} [GeV];Events / 10 GeV", 22, 0, 220);
    manager.addHist("control_met", ";E_{T}^{miss} [GeV];Events / 5 GeV", 45, 0, 225);
    manager.addHist("control_njet", ";Number of selected jets;Events", 12, -0.5, 11.5);
    manager.addHist("control_invmass", ";Invariant mass [GeV];Events / 5 GeV", 34, 15, 185);
    manager.addHist("control_sig_invmass", ";Invariant mass [GeV];Events / 10 GeV", 40, 11, 411);
    manager.addHist("control_sig_invmass_cut", ";Invariant mass [GeV];Events / 10 GeV", 40, 11, 411);
    manager.addHist("dy_dphi",";#Delta#phi;Events", 10, 0., M_PI);
    manager.addHist("dy_dphi_zpt",";Z p_{T} [GeV];Events", 30., 0., 300.);
    manager.addHist("dy_counter",";DD norm;Events", 5., 0., 5.);

    manager.addHist("mu", ";<#mu>;Events", 50, 0., 50.);
    manager.addHist("mc_weight", ";MC Event Weight", 100, -1.5, 5e6);

    manager.addHist("pileup_weight", ";Pileup Weight", 20, -1.5, 10);

    manager.addHist("el_n", ";Number of electrons;Events", 5, -0.5, 4.5);
    manager.addHist("el_pt", ";Electron p_{T} / GeV;Electrons / 10 GeV", 20, 5, 205);
    manager.addHist("el_eta", ";Electron #eta;Electrons", 20, -2.5, 2.5);
    manager.addHist("el_phi", ";Electron #phi;Electrons", 20, -M_PI, M_PI);
    manager.addHist("el_e", ";Electron E / GeV;Electrons", 40, 0, 200);
    manager.addHist("el_m", ";Electron m / GeV;Electrons", 40, 0, 200);
    manager.addHist("el_charge", ";Electron charge;Electrons", 2, -1.5, 1.5);

    manager.addHist("mu_n", ";Number of muons;Events", 5, -0.5, 4.5);
    manager.addHist("mu_pt", ";Muon p_{T} / GeV;Muons / 10 GeV", 20, 5, 205);
    manager.addHist("mu_eta", ";Muon #eta;Muons", 20, -2.5, 2.5);
    manager.addHist("mu_phi", ";Muon #phi;Muons", 20, -M_PI, M_PI);
    manager.addHist("mu_e", ";Muon E / GeV;Muons", 40, 0, 200);
    manager.addHist("mu_m", ";Muon m / GeV;Muons", 40, 0, 200);
    manager.addHist("mu_charge", ";Muon charge;Muons", 2, -1.5, 1.5);

    manager.addHist("jet_n", ";Number of jets;Events", 12, -0.5, 11.5);
    manager.addHist("jet_pt", ";Jet p_{T} / GeV;Jets / 10 GeV", 25, 5, 255);
    manager.addHist("jet_eta", ";Jet #eta;Jets", 25, -2.5, 2.5);
    manager.addHist("jet_phi", ";Jet #phi;Jets", 25, -M_PI, M_PI);
    manager.addHist("jet_e", ";Jet E / GeV;Jets", 50, 0, 250);
    manager.addHist("jet_mv2c20", ";Jet mv2c20;Jets", 50, 0, 1);

    manager.addHist("jet0_pt", ";Jet0 p_{T} / GeV; Events / 10 GeV", 25, 5, 255);
    manager.addHist("jet0_eta", ";Jet0 #eta; Jets", 25, -2.5, 2.5);
    manager.addHist("jet0_phi", ";Jet0 #phi; Jets", 25, -M_PI, M_PI);
    manager.addHist("jet0_e", ";Jet0 E / GeV; Jets", 50, 0, 250);
    manager.addHist("jet0_mv2c20", ";Jet0 mv2c20; Jets", 50, 0, 1);

    manager.addHist("jet1_pt", ";Jet1 p_{T} / GeV; Events / 10 GeV", 25, 5, 255);
    manager.addHist("jet1_eta", ";Jet1 #eta; Jets", 25, -2.5, 2.5);
    manager.addHist("jet1_phi", ";Jet1 #phi; Jets", 25, -M_PI, M_PI);
    manager.addHist("jet1_e", ";Jet1 E / GeV; Jets", 50, 0, 250);
    manager.addHist("jet1_mv2c20", ";Jet1 mv2c20; Jets", 50, 0, 1);

    manager.addHist("jet2_pt", ";Jet2 p_{T} / GeV; Events / 10 GeV", 25, 5, 255);
    manager.addHist("jet2_eta", ";Jet2 #eta; Jets", 25, -2.5, 2.5);
    manager.addHist("jet2_phi", ";Jet2 #phi; Jets", 25, -M_PI, M_PI);
    manager.addHist("jet2_e", ";Jet2 E / GeV; Jets", 50, 0, 250);
    manager.addHist("jet2_mv2c20", ";Jet2 mv2c20; Jets", 50, 0, 1);

    manager.addHist("jet3_pt", ";Jet3 p_{T} / GeV; Events / 10 GeV", 25, 5, 255);
    manager.addHist("jet3_eta", ";Jet3 #eta; Jets", 25, -2.5, 2.5);
    manager.addHist("jet3_phi", ";Jet3 #phi; Jets", 25, -M_PI, M_PI);
    manager.addHist("jet3_e", ";Jet3 E / GeV; Jets", 50, 0, 250);
    manager.addHist("jet3_mv2c20", ";Jet3 mv2c20; Jets", 50, 0, 1);

    manager.addHist("met_et", ";MET / GeV;Events / 5 GeV", 40, 0, 200);
    manager.addHist("met_phi", ";MET #phi;Events", 40, -M_PI, M_PI);
}

void fillPlots(const top::Event& topEvent, top::PlotManager& manager, double eventWeight) {
    const double toGeV = 0.001;

    manager.hist("mu")->Fill(topEvent.m_info->averageInteractionsPerCrossing(), eventWeight);

    if (top::isSimulation(topEvent)) {
//         manager.hist("mc_weight")->Fill(topEvent.m_info->mcEventWeight(), eventWeight);
        manager.hist("mc_weight")->Fill(topEvent.m_truthEvent->at(0)->weights()[0], eventWeight);// FIXME temporary bugfix

        if (top::ScaleFactorRetriever::hasPileupSF(topEvent))
            manager.hist("pileup_weight")->Fill(top::ScaleFactorRetriever::pileupSF(topEvent), eventWeight);
    }

    manager.hist("el_n")->Fill(topEvent.m_electrons.size(), eventWeight);
    manager.hist("mu_n")->Fill(topEvent.m_muons.size(), eventWeight);
    manager.hist("jet_n")->Fill(topEvent.m_jets.size(), eventWeight);

    //loop over all the electrons - new for loop style is cool
    for (const auto* const elPtr : topEvent.m_electrons) {
        manager.hist("el_pt")->Fill(elPtr->pt() * toGeV, eventWeight);
        manager.hist("el_eta")->Fill(elPtr->eta(), eventWeight);
        manager.hist("el_phi")->Fill(elPtr->phi(), eventWeight);
        manager.hist("el_e")->Fill(elPtr->e() * toGeV, eventWeight);
        manager.hist("el_m")->Fill(elPtr->m() * toGeV, eventWeight);
        manager.hist("el_charge")->Fill(elPtr->charge(), eventWeight);
    }

    for (const auto* const muPtr : topEvent.m_muons) {
        manager.hist("mu_pt")->Fill(muPtr->pt() * toGeV, eventWeight);
        manager.hist("mu_eta")->Fill(muPtr->eta(), eventWeight);
        manager.hist("mu_phi")->Fill(muPtr->phi(), eventWeight);
        manager.hist("mu_e")->Fill(muPtr->e() * toGeV, eventWeight);
        manager.hist("mu_m")->Fill(muPtr->m() * toGeV, eventWeight);
        manager.hist("mu_charge")->Fill(muPtr->charge(), eventWeight);
    }

    //plot for all jets, and also the four leading jets separately
    unsigned int i = 0;
    std::array<std::string, 4> numbers{ {"jet0", "jet1", "jet2", "jet3"} };
    for (const auto* const jetPtr : topEvent.m_jets) {
        manager.hist("jet_pt")->Fill(jetPtr->pt() * toGeV, eventWeight);
        manager.hist("jet_eta")->Fill(jetPtr->eta(), eventWeight);
        manager.hist("jet_phi")->Fill(jetPtr->phi(), eventWeight);
        manager.hist("jet_e")->Fill(jetPtr->e() * toGeV, eventWeight);

	double mv2c20_discriminant = 0.;
	jetPtr->btagging()->MVx_discriminant("MV2c20", mv2c20_discriminant);

        manager.hist("jet_mv2c20")->Fill(mv2c20_discriminant, eventWeight);

        if (i < numbers.size()) {
            manager.hist(numbers[i] + "_pt")->Fill(jetPtr->pt() * toGeV, eventWeight);
            manager.hist(numbers[i] + "_eta")->Fill(jetPtr->eta(), eventWeight);
            manager.hist(numbers[i] + "_phi")->Fill(jetPtr->phi(), eventWeight);
            manager.hist(numbers[i] + "_e")->Fill(jetPtr->e() * toGeV, eventWeight);
            manager.hist(numbers[i] + "_mv2c20")->Fill(mv2c20_discriminant, eventWeight);
        }

        ++i;
    }


    manager.hist("met_et")->Fill(topEvent.m_met->met() * toGeV, eventWeight);
    manager.hist("met_phi")->Fill(topEvent.m_met->phi(), eventWeight);


}
  
}

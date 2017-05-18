/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopExamples/DileptonAnalysis.h"
#include "TopExamples/DefaultPlots.h"
#include "TopExamples/AnalysisTools.h"

#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"

#include "TFile.h"
#include "TH1.h"

#include <sstream>
#include <iostream>
#include <fstream>

namespace top {

/**
 * @brief Setup the analysis, add some histograms.
 *
 * This sets up a histogram manager for each of the three channels (ee, mumu,
 * emu).  It also defaults the histograms used to save cutflows for each channel
 * and with / without weights to nullptr.  They are loaded from the input files.
 * Also three counters are set to 0 for the number of processed events.
 */
DileptonAnalysis::DileptonAnalysis(TFile* outputFile, EL::Worker* wk) :
                m_mcChannelNumber(0),
                m_runNumber(0),
                m_histsElEl("ee", outputFile, wk),
                m_histsMuMu("mumu", outputFile, wk),
                m_histsElMu("emu", outputFile, wk),
                m_cutflowEventsElEl(nullptr),
                m_cutflowMCWeightsElEl(nullptr),
                m_cutflowEventsMuMu(nullptr),
                m_cutflowMCWeightsMuMu(nullptr),
                m_cutflowEventsElMu(nullptr),
                m_cutflowMCWeightsElMu(nullptr),
                m_counterElEl(0),
                m_counterMuMu(0),
                m_counterElMu(0) {
            //add some default histograms for each channel
            top::addPlots(m_histsElEl);
            top::addPlots(m_histsMuMu);
            top::addPlots(m_histsElMu);

            //a special histogram for emu
            m_histsElMu.addHist("ht", ";HT / GeV;Events / 20 GeV", 40, 0, 800);
}

DileptonAnalysis::~DileptonAnalysis() {
}

/**
 * @brief When a new input file is opened we want to grab the cutflow histograms
 * from it and update.
 *
 * @param inputFile The file that was just opened.
 */
void DileptonAnalysis::newFile(TFile* inputFile) {
    updateCutflow("ee/cutflow", m_cutflowEventsElEl, inputFile);
    updateCutflow("ee/cutflow_mc", m_cutflowMCWeightsElEl, inputFile);
    updateCutflow("mumu/cutflow", m_cutflowEventsMuMu, inputFile);
    updateCutflow("mumu/cutflow_mc", m_cutflowMCWeightsMuMu, inputFile);
    updateCutflow("emu/cutflow", m_cutflowEventsElMu, inputFile);
    updateCutflow("emu/cutflow_mc", m_cutflowMCWeightsElMu, inputFile);
}

/**
 * @brief Work out if it's ee, mumu or emu and send the event to the correct
 * analysis.
 *
 * Before filling any plots figure out if this is data or MC simulation and get
 * the event weight if appropriate.
 *
 * Use the number of electrons or muons to figure out which channel this
 * is.  Note that overlap removal and some of the cuts have been done already.
 *
 * @param topEvent The event in question
 */
void DileptonAnalysis::event(const top::Event& topEvent) {
    m_runNumber = topEvent.m_info->runNumber();

    double eventWeight = 1.;
    if (top::isSimulation(topEvent)) {
        m_mcChannelNumber = topEvent.m_info->mcChannelNumber();
//         eventWeight = topEvent.m_info->mcEventWeight();
        eventWeight = topEvent.m_truthEvent->at(0)->weights()[0];// FIXME temporary bugfix
    }

    //std::cout << topEvent << std::endl;

    if (top::passesPreSelection(topEvent, "ee"))
        eeAnalysis(topEvent, eventWeight);

    if (top::passesPreSelection(topEvent, "mumu"))
        mumuAnalysis(topEvent, eventWeight);

    if (top::passesPreSelection(topEvent, "emu"))
        emuAnalysis(topEvent, eventWeight);
}

/**
 * @brief Plot some control regions, then the final plots for the analysis.
 *
 * Most of the code here is spent defining different combinations of cuts for
 * the control plots.  The very last few lines make the plots in the signal
 * region.
 *
 * Note that for now we are manually calculating TLorentzVectors for the two
 * leptons, since this appears to be broken in the current xAOD code.
 *
 * @param topEvent The event to perform the analysis on.  We assume it has
 * exactly two electrons for the rest of this function.
 * @param eventWeight The event weight to be used when filling plots.
 */
void DileptonAnalysis::eeAnalysis(const top::Event& topEvent, double eventWeight) {
    ++m_counterElEl;

    const auto* e0 = topEvent.m_electrons[0];
    const auto* e1 = topEvent.m_electrons[1];
    const TLorentzVector Z = e0->p4() + e1->p4();

    //does it pass some cuts?
    const bool gteq2jets = topEvent.m_jets.size() >= 2;
    const bool metgt30 = topEvent.m_met->met() > 30000.;
    const bool metgt60 = topEvent.m_met->met() > 60000.;
    const bool inZWindow = fabs(Z.M() - 91000.) < 10000.;

    //plot the met for all events in the Z-window
    if (inZWindow && gteq2jets)
        m_histsElEl.hist("control_met")->Fill(topEvent.m_met->met() * toGeV, eventWeight);

    //plot the invariant mass for low MET events
    if (!metgt60 && gteq2jets)
        m_histsElEl.hist("control_invmass")->Fill(Z.M() / 1000., eventWeight);

    //plot njet in zwindow for low MET events
    if (inZWindow && !metgt60)
        m_histsElEl.hist("control_njet")->Fill(topEvent.m_jets.size(), eventWeight);

    //DY control region: inside Z window, but lower pt cut to signal region
    if (inZWindow && metgt30 && gteq2jets) {
        m_histsElEl.hist("dy_dphi")->Fill(fabs(top::deltaPhi(*e0, *e1)), eventWeight);
        m_histsElEl.hist("dy_dphi_zpt")->Fill(Z.Pt() * toGeV, eventWeight);
        m_histsElEl.hist("dy_counter")->Fill(1., eventWeight);
    }

    //signal region - all cuts except Z mass
    if (gteq2jets && metgt60)
        m_histsElEl.hist("control_sig_invmass")->Fill(Z.M() * toGeV, eventWeight);

    if (!inZWindow && gteq2jets && metgt60)
        m_histsElEl.hist("control_sig_invmass_cut")->Fill(Z.M() * toGeV, eventWeight);

    //signal region - all cuts except njet
    if (!inZWindow && metgt60)
        m_histsElEl.hist("control_sig_njet")->Fill(topEvent.m_jets.size(), eventWeight);

    //signal region - all cuts except met
    if (!inZWindow && gteq2jets)
        m_histsElEl.hist("control_sig_met")->Fill(topEvent.m_met->met() * toGeV, eventWeight);

    //pretag selection
    if (gteq2jets && !inZWindow && metgt60)
        top::fillPlots(topEvent, m_histsElEl, eventWeight);
}

void DileptonAnalysis::mumuAnalysis(const top::Event& topEvent, double eventWeight) {
    ++m_counterMuMu;

    const auto* mu0 = topEvent.m_muons[0];
    const auto* mu1 = topEvent.m_muons[1];
    const TLorentzVector Z = mu0->p4() + mu1->p4();

    //does it pass some cuts?
    const bool gteq2jets = topEvent.m_jets.size() >= 2;
    const bool metgt30 = topEvent.m_met->met() > 30000.;
    const bool metgt60 = topEvent.m_met->met() > 60000.;
    const bool inZWindow = fabs(Z.M() - 91000.) < 10000.;

    //plot the met for all events in the Z-window
    if (inZWindow && gteq2jets)
        m_histsMuMu.hist("control_met")->Fill(topEvent.m_met->met() * toGeV, eventWeight);

    //plot the invariant mass for low MET events
    if (!metgt60 && gteq2jets)
        m_histsMuMu.hist("control_invmass")->Fill(Z.M() / 1000., eventWeight);

    //plot njet in zwindow for low MET events
    if (inZWindow && !metgt60)
        m_histsMuMu.hist("control_njet")->Fill(topEvent.m_jets.size(), eventWeight);

    //DY control region: inside Z window, but lower pt cut to signal region
    if (inZWindow && metgt30 && gteq2jets) {
        m_histsMuMu.hist("dy_dphi")->Fill(fabs(top::deltaPhi(*mu0, *mu1)), eventWeight);
        m_histsMuMu.hist("dy_dphi_zpt")->Fill(Z.Pt() * toGeV, eventWeight);
        m_histsMuMu.hist("dy_counter")->Fill(1., eventWeight);
    }

    //signal region - all cuts except Z mass
    if (gteq2jets && metgt60)
        m_histsMuMu.hist("control_sig_invmass")->Fill(Z.M() * toGeV, eventWeight);

    if (!inZWindow && gteq2jets && metgt60)
        m_histsMuMu.hist("control_sig_invmass_cut")->Fill(Z.M() * toGeV,eventWeight);

    //signal region - all cuts except njet
    if (!inZWindow && metgt60)
        m_histsMuMu.hist("control_sig_njet")->Fill(topEvent.m_jets.size(), eventWeight);

    //signal region - all cuts except met
    if (!inZWindow && gteq2jets)
        m_histsMuMu.hist("control_sig_met")->Fill(topEvent.m_met->met() * toGeV, eventWeight);

    //pretag selection
    if (gteq2jets && !inZWindow && metgt60)
        top::fillPlots(topEvent, m_histsMuMu, eventWeight);
}

/**
 * @brief emu channel
 *
 * The way it's setup at the moment we just need to add a cut on jet
 * multiplicity and ht to get a "full" set of pretag cuts.  Apply these
 * and then fill the plots.
 *
 * @param topEvent The event in question
 * @param eventWeight The weights that are used when filling the histograms.
 */
void DileptonAnalysis::emuAnalysis(const top::Event& topEvent, double eventWeight) {
    ++m_counterElMu;

    //does it pass some cuts?
    const bool gteq2jets = topEvent.m_jets.size() >= 2;
    const double ht = top::ht(topEvent);

    if (gteq2jets && ht > 120. * toGeV) {
        m_histsElMu.hist("ht")->Fill(ht * toGeV, eventWeight);
        top::fillPlots(topEvent, m_histsElMu, eventWeight);
    }
}

/**
 * @brief Print cutflows for each of the three channels.  Save any histograms
 * that we've made. If running on MC simulation then normalise the histograms
 * to 1 fb-1.
 *
 * @param outputFile Root file to save the histograms to.
 */
void DileptonAnalysis::finalise(TFile* outputFile) {
    //to screen
    std::cout << "Cutflows directly from the input files:\n";
    printCutflow(std::cout, m_cutflowEventsElEl, m_cutflowMCWeightsElEl, "ee", m_counterElEl);
    printCutflow(std::cout, m_cutflowEventsMuMu, m_cutflowMCWeightsMuMu, "mumu", m_counterMuMu);
    printCutflow(std::cout, m_cutflowEventsElMu, m_cutflowMCWeightsElMu, "emu", m_counterElMu);

    //tofile - mc use channel number, data use run number
    std::stringstream ss;
    if (m_mcChannelNumber > 0)
        ss << m_mcChannelNumber;
    else
        ss << m_runNumber;

    std::ofstream cutflowfile_ee(ss.str() + "_ee_cutflow.txt");
    std::ofstream cutflowfile_mumu(ss.str() + "_mumu_cutflow.txt");
    std::ofstream cutflowfile_emu(ss.str() + "_emu_cutflow.txt");

    printCutflow(cutflowfile_ee, m_cutflowEventsElEl, m_cutflowMCWeightsElEl, "ee", m_counterElEl);
    printCutflow(cutflowfile_mumu, m_cutflowEventsMuMu, m_cutflowMCWeightsMuMu, "mumu", m_counterMuMu);
    printCutflow(cutflowfile_emu, m_cutflowEventsElMu, m_cutflowMCWeightsElMu, "emu", m_counterElMu);

    cutflowfile_ee.close();
    cutflowfile_mumu.close();
    cutflowfile_emu.close();

    //if MC then calculate a SF to 1 fb-1
    double sf = 1.;
    if (m_mcChannelNumber != 0) {
        const double mcWeightsInSample = m_cutflowMCWeightsElEl->GetBinContent(1);
        sf = sfToOneInversefb(m_mcChannelNumber, mcWeightsInSample);
    }

    //save the plots for the three channels
    m_histsElEl.scaleHistograms(sf);
    m_histsMuMu.scaleHistograms(sf);
    m_histsElMu.scaleHistograms(sf);
    outputFile->Write();

}

/**
 * @brief Prints the cutflow table to the screen or a file.  This is not scaled
 * to the integrated to 1fb-1 like the output histograms are for MC simulation.
 * Should it be?
 *
 * @param out For the screen use std::cout, for a text file use an ofstream, e.g.
 * std::ofstream file("myfile").
 * @param eventHist Histogram containing the raw event yield.  No weights. Also
 * used for the row headings / cut names. The numbers here come from the grid,
 * not from the local analysis code.
 * @param mcWeightHist Histogram containing yield weighted by the mc event
 * weight. The numbers here come from the grid/ input file, not from the local
 * analysis code.
 * @param name The name of the current selection (e.g. ee, mumu, emu)
 * @param localYield Yield that the local code ran over.  It should match the
 * number from the grid (since you ran on the same events).
 */
void DileptonAnalysis::printCutflow(std::ostream& out, const TH1D* const eventHist, const TH1D* const mcWeightHist, const std::string& name, const unsigned int localYield) {
    out << name << " cutflow:\n";
    out << "    " <<
            std::setw(4) << "Num" << 
            std::setw(30) << "Name" <<
            std::setw(20) << "Grid (Events)" <<
            std::setw(20) << "Grid (MC Weights)" <<
            std::setw(20) << "Local (Events)" << "\n";

    for (int i = 1; i <= eventHist->GetNbinsX(); ++i) {
        out << "    " << std::setw(4) << i
                << std::setw(30) << eventHist->GetXaxis()->GetBinLabel(i)
                << std::setw(20) << eventHist->GetBinContent(i)
                << std::setw(20) << mcWeightHist->GetBinContent(i);

        if (i == eventHist->GetNbinsX())
            out << std::setw(20) << localYield;

        out << "\n";
    }

    out << "\n";
}

}

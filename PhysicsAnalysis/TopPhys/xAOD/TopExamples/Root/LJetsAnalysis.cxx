/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopExamples/LJetsAnalysis.h"
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

LJetsAnalysis::LJetsAnalysis(TFile* outputFile, EL::Worker* wk) :
        m_mcChannelNumber(0),
        m_runNumber(0),
        m_histsEl("ejets", outputFile, wk),
        m_histsMu("mujets", outputFile, wk),
        m_cutflowEventsEl(nullptr),
        m_cutflowMCWeightsEl(nullptr),
        m_cutflowEventsMu(nullptr),
        m_cutflowMCWeightsMu(nullptr),
        m_counterEl(0),
        m_counterMu(0) {
    //add some default histograms for each channel
    top::addPlots(m_histsEl);
    top::addPlots(m_histsMu);

    //some extra histograms
    m_histsEl.addHist("mwt", ";MWT [GeV];Events", 20, 0, 200);
    m_histsMu.addHist("mwt", ";MWT [GeV];Events", 20, 0, 200);
}

LJetsAnalysis::~LJetsAnalysis() {
}

void LJetsAnalysis::newFile(TFile* inputFile) {
    updateCutflow("ejets/cutflow", m_cutflowEventsEl, inputFile);
    updateCutflow("ejets/cutflow_mc", m_cutflowMCWeightsEl, inputFile);
    updateCutflow("mujets/cutflow", m_cutflowEventsMu, inputFile);
    updateCutflow("mujets/cutflow_mc", m_cutflowMCWeightsMu, inputFile);
}

void LJetsAnalysis::event(const top::Event& topEvent) {
    m_runNumber = topEvent.m_info->runNumber();

    double eventWeight = 1.;
    if (top::isSimulation(topEvent)) {
        m_mcChannelNumber = topEvent.m_info->mcChannelNumber();
//         eventWeight = topEvent.m_info->mcEventWeight();
        eventWeight = topEvent.m_truthEvent->at(0)->weights()[0];// FIXME temporary bugfix
    }

    //std::cout << topEvent << std::endl;

    if (top::passesPreSelection(topEvent, "ejets")) {
        top::fillPlots(topEvent, m_histsEl, eventWeight);
        const double mwt = top::mwt(*topEvent.m_electrons[0], *topEvent.m_met);
        m_histsEl.hist("mwt")->Fill(mwt * top::toGeV, eventWeight);
        ++m_counterEl;
    }

    if (top::passesPreSelection(topEvent, "mujets")) {
        top::fillPlots(topEvent, m_histsMu, eventWeight);
        const double mwt = top::mwt(*topEvent.m_muons[0], *topEvent.m_met);
        m_histsMu.hist("mwt")->Fill(mwt * top::toGeV, eventWeight);
        ++m_counterMu;
    }
}

void LJetsAnalysis::finalise(TFile* outputFile) {
    //to screen
    std::cout << "Cutflows directly from the input files:\n";
    printCutflow(std::cout, m_cutflowEventsEl, m_cutflowMCWeightsEl, "ejets", m_counterEl);
    printCutflow(std::cout, m_cutflowEventsMu, m_cutflowMCWeightsMu, "mujets", m_counterMu);

    //tofile - mc use channel number, data use run number
    std::stringstream ss;
    if (m_mcChannelNumber > 0)
        ss << m_mcChannelNumber;
    else
        ss << m_runNumber;

    std::ofstream cutflowfile_ejets(ss.str() + "_ejets_cutflow.txt");
    std::ofstream cutflowfile_mujets(ss.str() + "_mujets_cutflow.txt");

    printCutflow(cutflowfile_ejets, m_cutflowEventsEl, m_cutflowMCWeightsEl, "ejets", m_counterEl);
    printCutflow(cutflowfile_mujets, m_cutflowEventsMu, m_cutflowMCWeightsMu, "mujets", m_counterMu);

    cutflowfile_ejets.close();
    cutflowfile_mujets.close();

    //normalise to 1 fb-1 for MC simulation
    double sf = 1.;
    if (m_mcChannelNumber != 0) {
        const double mcWeightsInSample = m_cutflowMCWeightsEl->GetBinContent(1);
        sf = sfToOneInversefb(m_mcChannelNumber, mcWeightsInSample);
    }

    m_histsEl.scaleHistograms(sf);
    m_histsMu.scaleHistograms(sf);
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
void LJetsAnalysis::printCutflow(std::ostream& out, const TH1D* const eventHist, const TH1D* const mcWeightHist, const std::string& name, const unsigned int localYield) {
    out << name << " cutflow:\n";
    out << "    " <<
            std::setw(4) << "Num" << 
            std::setw(20) << "Name" <<
            std::setw(20) << "Grid (Events)" <<
            std::setw(20) << "Grid (MC Weights)" <<
            std::setw(20) << "Local (Events)" << "\n";

    for (int i = 1; i <= eventHist->GetNbinsX(); ++i) {
        out << "    " << std::setw(4) << i
                << std::setw(20) << eventHist->GetXaxis()->GetBinLabel(i)
                << std::setw(20) << eventHist->GetBinContent(i)
                << std::setw(20) << mcWeightHist->GetBinContent(i);

        if (i == eventHist->GetNbinsX())
            out << std::setw(20) << localYield;

        out << "\n";
    }

    out << "\n";
}

}

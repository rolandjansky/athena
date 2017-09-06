/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopExamples/NoSelectionAnalysis.h"
#include "TopExamples/DefaultPlots.h"
#include "TopExamples/AnalysisTools.h"

#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"

#include "TFile.h"
#include "TH1.h"

#include <iostream>

namespace top {

NoSelectionAnalysis::NoSelectionAnalysis(TFile* outputFile, EL::Worker* wk) :
                m_hists("all", outputFile, wk),
                m_cutflowEvents(nullptr),
                m_cutflowMCWeights(nullptr),
                m_counter(0) {
    top::addPlots(m_hists);
}

NoSelectionAnalysis::~NoSelectionAnalysis() {
}

void NoSelectionAnalysis::newFile(TFile* inputFile) {
    top::updateCutflow("all/cutflow", m_cutflowEvents, inputFile);
    top::updateCutflow("all/cutflow_mc", m_cutflowMCWeights, inputFile);
}

void NoSelectionAnalysis::event(const top::Event& topEvent) {
    ++m_counter;

    double eventWeight = 1.;
    if (top::isSimulation(topEvent))
//         eventWeight = topEvent.m_info->mcEventWeight();
        eventWeight = topEvent.m_truthEvent->at(0)->weights()[0];// FIXME temporary bugfix

    //std::cout << topEvent << std::endl;
    top::fillPlots(topEvent, m_hists, eventWeight);
}

void NoSelectionAnalysis::finalise(TFile* outputFile) {
    std::cout << "Cutflow directly from the input files:\n";
    printCutflow(m_cutflowEvents, m_cutflowMCWeights, "all", m_counter);

    //write out plots
    outputFile->Write();
}

void NoSelectionAnalysis::printCutflow(TH1D* eventHist, TH1D* mcWeightHist, const std::string& name, unsigned int localYield) {
    std::cout << name << " cutflow:\n";
    std::cout << "    " << 
            std::setw(4) << "Num" << 
            std::setw(20) << "Name" <<
            std::setw(20) << "Grid (Events)" <<
            std::setw(20) << "Grid (MC Weights)" <<
            std::setw(20) << "Local (Events)" << "\n";

    for (int i = 1; i <= eventHist->GetNbinsX(); ++i) {
        std::cout << "    " << std::setw(4) << i
                << std::setw(20) << eventHist->GetXaxis()->GetBinLabel(i)
                << std::setw(20) << eventHist->GetBinContent(i)
                << std::setw(20) << mcWeightHist->GetBinContent(i);

        if (i == eventHist->GetNbinsX())
            std::cout << std::setw(20) << localYield;

        std::cout << "\n";
    }

    std::cout << "\n";
}

}

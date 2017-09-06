/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NOSELECTIONANALYSIS_H_
#define NOSELECTIONANALYSIS_H_

#include <string>

#include "TopExamples/AnalysisBase.h"
#include "TopEventSelectionTools/PlotManager.h"

class TH1D;
class TFile;

namespace EL {
  class Worker;
}

namespace top {

class NoSelectionAnalysis : public AnalysisBase {
public:
    NoSelectionAnalysis(TFile* outputFile, EL::Worker* wk=nullptr);
    ~NoSelectionAnalysis();

    void newFile(TFile* inputFile) override;
    void event(const top::Event& topEvent) override;
    void finalise(TFile* outputFile) override;

private:
    void printCutflow(TH1D* eventHist, TH1D* mcWeightHist, const std::string& name, unsigned int localYield);

    top::PlotManager m_hists;

    TH1D* m_cutflowEvents;
    TH1D* m_cutflowMCWeights;

    unsigned int m_counter;
};

}

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LJETSANALYSIS_H_
#define LJETSANALYSIS_H_

#include "TopExamples/AnalysisBase.h"
#include "TopEventSelectionTools/PlotManager.h"

#include <string>

class TH1D;
class TFile;

namespace EL {
  class Worker;
}

namespace top {

class LJetsAnalysis : public AnalysisBase {
public:
    LJetsAnalysis(TFile* outputFile, EL::Worker* wk=nullptr);
    ~LJetsAnalysis();

    void newFile(TFile* inputFile) override;
    void event(const top::Event& topEvent) override;
    void finalise(TFile* outputFile) override;

private:
    void printCutflow(std::ostream& out, const TH1D* const eventHist, const TH1D* const mcWeightHist, const std::string& name, const unsigned int localYield);

    ///used for output filename (for MC)
    unsigned int m_mcChannelNumber;

    ///runNumber used for output filename
    unsigned int m_runNumber;

    top::PlotManager m_histsEl;
    top::PlotManager m_histsMu;

    TH1D* m_cutflowEventsEl;
    TH1D* m_cutflowMCWeightsEl;
    TH1D* m_cutflowEventsMu;
    TH1D* m_cutflowMCWeightsMu;

    unsigned int m_counterEl;
    unsigned int m_counterMu;
};

}

#endif

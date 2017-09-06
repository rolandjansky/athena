/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DILEPTONANALYSIS_H_
#define DILEPTONANALYSIS_H_

#include "TopExamples/AnalysisBase.h"
#include "TopEventSelectionTools/PlotManager.h"

class TH1D;
class TFile;

namespace EL {
  class Worker;
}

namespace top {

class DileptonAnalysis : public AnalysisBase {
public:
    DileptonAnalysis(TFile* outputFile, EL::Worker* wk=nullptr);
    ~DileptonAnalysis();

    void newFile(TFile* inputFile) override;
    void event(const top::Event& topEvent) override;
    void finalise(TFile* outputFile) override;

private:
    void eeAnalysis(const top::Event& topEvent, double eventWeight);
    void mumuAnalysis(const top::Event& topEvent, double eventWeight);
    void emuAnalysis(const top::Event& topEvent, double eventWeight);

    void printCutflow(std::ostream& out, const TH1D* const eventHist, const TH1D* const mcWeightHist, const std::string& name, const unsigned int localYield);

    ///MC Channel number is needed to calculate the SF (to 1 fb-1) at the end of the job
    unsigned int m_mcChannelNumber;

    ///runNumber used for output filename
    unsigned int m_runNumber;

    ///Histograms for the ee, mumu and emu channels
    top::PlotManager m_histsElEl;
    top::PlotManager m_histsMuMu;
    top::PlotManager m_histsElMu;

    ///Cutflows for (1) raw events and (2) events with MC weights
    TH1D* m_cutflowEventsElEl;
    TH1D* m_cutflowMCWeightsElEl;
    TH1D* m_cutflowEventsMuMu;
    TH1D* m_cutflowMCWeightsMuMu;
    TH1D* m_cutflowEventsElMu;
    TH1D* m_cutflowMCWeightsElMu;

    ///Counters for events running locally
    unsigned int m_counterElEl;
    unsigned int m_counterMuMu;
    unsigned int m_counterElMu;
};

}

#endif /* DILEPTONANALYSIS_H_ */

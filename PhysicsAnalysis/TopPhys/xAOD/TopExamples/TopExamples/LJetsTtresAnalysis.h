/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LJETSTTRESANALYSIS_H_
#define LJETSTTRESANALYSIS_H_

#include "TopExamples/AnalysisBase.h"
#include "TopEventSelectionTools/PlotManager.h"

#include "TopEventReconstructionTools/TtresNeutrinoBuilder.h"
#include "TopEventReconstructionTools/TtresChi2.h"

#include <string>

class TH1D;
class TFile;

namespace top {

class LJetsTtresAnalysis : public AnalysisBase {
public:
    LJetsTtresAnalysis(TFile* outputFile);
    ~LJetsTtresAnalysis();

    void newFile(TFile* inputFile) override;
    void event(const top::Event& topEvent) override;
    void finalise(TFile* outputFile) override;

private:
    void printCutflow(TH1D* eventHist, TH1D* mcWeightHist, const std::string& name, unsigned int localYield);

    void fillIt(top::PlotManager &hist, const top::Event& topEvent, bool isBoosted);

    unsigned int m_mcChannelNumber;

    top::PlotManager m_histsREl;
    top::PlotManager m_histsRMu;
    top::PlotManager m_histsBEl;
    top::PlotManager m_histsBMu;

    TH1D* m_cutflowEventsREl;
    TH1D* m_cutflowMCWeightsREl;
    TH1D* m_cutflowEventsRMu;
    TH1D* m_cutflowMCWeightsRMu;

    TH1D* m_cutflowEventsBEl;
    TH1D* m_cutflowMCWeightsBEl;
    TH1D* m_cutflowEventsBMu;
    TH1D* m_cutflowMCWeightsBMu;

    unsigned int m_counterREl;
    unsigned int m_counterRMu;

    unsigned int m_counterBEl;
    unsigned int m_counterBMu;

    TtresNeutrinoBuilder m_neutrinoBuilder;
    TtresChi2 m_chi2;
};

}

#endif

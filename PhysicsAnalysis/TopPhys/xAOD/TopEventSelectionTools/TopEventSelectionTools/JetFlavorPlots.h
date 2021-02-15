/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TOPEVENTSELECTIONTOOLS_JETFLAVORPLOTS_H_
#define TOPEVENTSELECTIONTOOLS_JETFLAVORPLOTS_H_

#include <string>

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopEventSelectionTools/PlotManager.h"
#include "PMGTools/PMGTruthWeightTool.h"

class TFile;

namespace EL {
  class Worker;
}

namespace top {
  class TopConfig;

/**
 * @brief An example of how to quickly make some plots at a certain point in
 * the cutflow.
 */
  class JetFlavorPlots: public EventSelectorBase {
  public:
    /**
     * @brief Setup some example plots.
     *
     * Add a bunch of histograms.
     *
     * @param name The name of the directory to store histograms in, in the
     * output file.  e.g. you might have ee, mumu and emu.
     * @param outputFile The output file. Needs setting up at the very start
     * so that we can attach the files.
     * @param params The arguments, e.g. for the binning of the plots.
     * @param config Instance of TopConfig
     * @param wk Only used by EventLoop, ok as nullptr as default.
     */
    JetFlavorPlots(const std::string& name, TFile* outputFile, const std::string& params,
                   std::shared_ptr<top::TopConfig> config,
                   EL::Worker* wk = nullptr);

    /**
     * @brief Fill the histograms.
     *
     * @return True because it doesn't select any events.
     */
    bool apply(const top::Event& event) const override;

    /**
     * @brief Return the name for the cutflow table.
     *
     * @return The word JETFLAVORPLOTS.
     */
    std::string name() const override;
  private:
    // File units are MeV and normally people like plots in GeV.
    static const double toGeV;

    // Easy access to histograms.
    std::shared_ptr<PlotManager> m_hists = nullptr;
    std::shared_ptr<PlotManager> m_hists_Loose = nullptr;
    std::shared_ptr<PlotManager> m_hists_RadHigh = nullptr;
    std::shared_ptr<PlotManager> m_hists_RadHigh_Loose = nullptr;
    std::shared_ptr<PlotManager> m_hists_RadLow = nullptr;
    std::shared_ptr<PlotManager> m_hists_RadLow_Loose = nullptr;
    std::shared_ptr<PlotManager> m_hists_RadHighNoVar3c = nullptr;
    std::shared_ptr<PlotManager> m_hists_RadHighNoVar3c_Loose = nullptr;
    std::shared_ptr<PlotManager> m_hists_RadLowNoVar3c = nullptr;
    std::shared_ptr<PlotManager> m_hists_RadLowNoVar3c_Loose = nullptr;

    // Nominal hash value
    std::size_t m_nominalHashValue;

    // for detailed plots (e.g. vs. Njets)
    bool m_detailed;

    // to choose between radiation varied and nominal plots
    bool m_doNominal;
    bool m_doRadHigh;
    bool m_doRadLow;
    bool m_doRadHighNoVar3c; // doRadHigh but don't take Var3c into account
    bool m_doRadLowNoVar3c; // doRadLow but don't take Var3c into account

    // pT and eta bin edges, separated by colons
    std::string m_ptBins;
    std::string m_etaBins;

    // max value for nJets, when doing the plots vs. nJets
    int m_nJetsMax;

    // name of the jet collection - this is needed for the names of the histograms
    std::string m_jetCollection;

    // shared pointed to instance of TopConfig
    std::shared_ptr<top::TopConfig> m_config;

    //PMGTruthWeights
    PMGTools::PMGTruthWeightTool* m_PMGTruthWeights;

    // function to translate the binnings into vector of bin edges
    void formatBinning(const std::string& str, std::vector<double>& binEdges);
    //helper function to book histograms
    void BookHistograms(std::shared_ptr<PlotManager> h_ptr, std::vector<double> ptBins, std::vector<double> etaBins);
    //helper function to fill histograms
    void FillHistograms(std::shared_ptr<PlotManager> h_ptr, double w_event, const top::Event& event) const;
    //This is to suppress excessive printout
    mutable std::atomic<bool> m_throwwarningPMG;
  };
}  // namespace top

#endif  // TOPEVENTSELECTIONTOOLS_JETFLAVORPLOTS_H_

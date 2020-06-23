/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TOPEVENTSELECTIONTOOLS_JETRESPONSEPLOTS_H_
#define TOPEVENTSELECTIONTOOLS_JETRESPONSEPLOTS_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopEventSelectionTools/PlotManager.h"
#include "PMGAnalysisInterfaces/IPMGTruthWeightTool.h"
#include "PMGTools/PMGTruthWeightTool.h"
#include "AsgTools/ToolHandle.h"
#include "AsgTools/ToolHandleArray.h"
#include "AsgTools/AnaToolHandle.h"

#include <string>

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
  class JetResponsePlots: public EventSelectorBase {
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
    JetResponsePlots(const std::string& name,
                     TFile* outputFile,
                     const std::string& params,
                     std::shared_ptr<top::TopConfig> config,
                     EL::Worker* wk = nullptr);

    /**
     * @brief Fill the histograms.
     *
     * @return True because it doesn't select any events.
     */
    bool apply(const top::Event& event) const override;

    /**
     * @brief Helper function to fill the histograms
     *
     */
    void FillHistograms(const double w_event,
                        const top::Event& event) const;

    /**
     * @brief Return the name for the cutflow table.
     *
     * @return The word JETRESPONSEPLOTS.
     */
    std::string name() const override;
  private:

    // Easy access to histograms.
    std::shared_ptr<PlotManager> m_hists;

    // Nominal hash value
    std::size_t m_nominalHashValue;
    
    // deltaR used for jet matching
    float m_deltaR;
    int m_bins;
    float m_min;
    float m_max;
    std::vector<double> m_ptBinning;

    // shared pointed to instance of TopConfig
    std::shared_ptr<top::TopConfig> m_config;

    //PMGTruthWeights
    ToolHandle<PMGTools::IPMGTruthWeightTool> m_PMGTruthWeights;

  };
}  // namespace top

#endif  // TOPEVENTSELECTIONTOOLS_JETRESPONSEPLOTS_H_

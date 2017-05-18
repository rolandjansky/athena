/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXAMPLEPLOTS_H_
#define EXAMPLEPLOTS_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopEventSelectionTools/PlotManager.h"

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
class ExamplePlots : public EventSelectorBase {
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
     * @param wk Only used by EventLoop, ok as nullptr as default.
     */
    ExamplePlots(const std::string& name, TFile* outputFile, EL::Worker* wk = nullptr, std::shared_ptr<top::TopConfig> config = nullptr);

    /**
     * @brief Fill the histograms.
     *
     * @return True because it doesn't select any events.
     */
    virtual bool apply(const top::Event& event) const override;

    /**
     * @brief Return the name for the cutflow table.
     *
     * @return The word EXAMPLEPLOTS.
     */
    std::string name() const override;

private:
    ///File units are MeV and normally people like plots in GeV.
    static const double toGeV;

    ///Easy access to histograms.
    PlotManager m_hists;
    
    // Nominal hash value
    std::size_t m_nominalHashValue;

    std::shared_ptr<top::TopConfig> m_config;
};

}

#endif

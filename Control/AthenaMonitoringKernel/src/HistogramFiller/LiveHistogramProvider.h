/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_HistogramFiller_LiveHistogramProvider_h
#define AthenaMonitoringKernel_HistogramFiller_LiveHistogramProvider_h

#include <memory>

#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "AthenaMonitoringKernel/HistogramDef.h"
#include "AthenaMonitoringKernel/IHistogramProvider.h"

#include "HistogramFactory.h"

namespace Monitored {
  /**
   * @brief Provides latest-N-lumiblock histograms to be filled
   *
   * Provides histograms which shows only information from the last N lumiblocks. The
   * highest encountered lumiblock is calculated, and only data from Max-N to N is shown.
   * Note: kLBNLive must be defined in histogram definition options.
   */
  class LiveHistogramProvider : public IHistogramProvider {
  public:

    /**
     * @brief Constructor
     *
     * @param gmTool Source of the lumi block info.
     * @param factory ROOT object factory.
     * @param def Definition of the histogram.
     */
    LiveHistogramProvider(
      GenericMonitoringTool* const gmTool,
      std::shared_ptr<HistogramFactory> factory,
      const HistogramDef& histDef)
      : IHistogramProvider()
      , m_gmTool(gmTool)
      , m_factory(factory)
      , m_histDef(new HistogramDef(histDef))
      {}

    /**
     * @brief Getter of ROOT histogram
     *
     * Each time the method is called, factory produces ROOT object based on the current
     * lumi block. Note: ROOT objects are cached at the factory. Nevertheless, it is
     * recommended to call this method as rarely as possible.
     *
     * @return ROOT object
     */
    TNamed* histogram() override {
      // Get the LB for this event.
      const int lumiBlock = m_gmTool->lumiBlock();

      if (lumiBlock <= m_currentLumiBlock and m_currentHistogram != nullptr) {
        // The histogram exists, and this event's LB is the most recent LB, so just fill
        // it. If the LB is too far in the past (i.e. less than ($current - $N)), it will
        // just fill the underflow bin, which is acceptable. Or, if the histogram hasn't
        // been created yet.
        return m_currentHistogram;

      } else { // This event's LB is now the largest LB. The histogram must be remade.
        // Update the variable keeping track of the highest LB.
        m_currentLumiBlock = std::max((float) lumiBlock, m_histDef->xmax);

        // De-register the old histogram.
        m_factory->remove(*m_histDef);

        // Update the range of the x-bins.
        m_histDef->xmax = std::max(m_currentLumiBlock + 0.5f, m_histDef->xmax);
        m_histDef->xmin = std::max(m_currentLumiBlock + 0.5f - m_histDef->kLive, 0.5f);
        m_histDef->xbins = m_histDef->xmax - m_histDef->xmin;

        // Create a new one using the alias which was just made available.
        m_currentHistogram = m_factory->create(*m_histDef);
        return m_currentHistogram;
      }

    }
  private:
    GenericMonitoringTool* const m_gmTool;
    std::shared_ptr<HistogramFactory> m_factory;
    std::shared_ptr<HistogramDef> m_histDef;
    TNamed *m_currentHistogram = nullptr;
    int m_currentLumiBlock = 0;
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_LiveHistogramProvider_h */
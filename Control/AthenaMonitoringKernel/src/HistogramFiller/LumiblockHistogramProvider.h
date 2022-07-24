/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_HistogramFiller_LumiblockHistogramProvider_h
#define AthenaMonitoringKernel_HistogramFiller_LumiblockHistogramProvider_h

#include <map>
#include <memory>
#include <utility>

#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "AthenaMonitoringKernel/HistogramDef.h"
#include "AthenaMonitoringKernel/IHistogramProvider.h"

#include "HistogramFactory.h"

namespace Monitored {
  /**
   * @brief Implementation of IHistogramProvider for lumi block based histograms
   *
   * This provider produces histograms that groups data based on current lumi block and defined history depth.
   * Note: kLBNHistoryDepth must be defined in histogram definition options
   */
  class LumiblockHistogramProvider : public IHistogramProvider {
  public:
    /**
     * @brief Constructor
     *
     * @param gmTool Source of the lumi block info
     * @param factory ROOT object factory
     * @param def General definition of a histogram
     */
    LumiblockHistogramProvider(GenericMonitoringTool* const gmTool,
        std::shared_ptr<HistogramFactory> factory,
        const HistogramDef& histDef)
      : IHistogramProvider()
      , m_gmTool(gmTool)
      , m_factory(factory)
      , m_histDef(histDef)
      {}

    /**
     * @brief Getter of ROOT object
     *
     * Return the histogram for the current lumiblock. Create a new one if needed.
     * A configurable number of histograms is kept active at a given time.
     *
     * @return ROOT object
     */
    TNamed* histogram() override {

      const unsigned lumiBlock = m_gmTool->lumiBlock();
      std::scoped_lock lock(m_mutex);  // access to m_hists

      /* Find existing histogram. In the unlikely case of a very old lumiblock
         being processed, it would be filled into the oldest histogram. */
      const auto it = m_hists.lower_bound(lumiBlock);
      if (it!=m_hists.end() && it->second.second!=nullptr) {
        return it->second.second;
      }

      if (m_histDef.kLBNHistoryDepth <= 0) {
        throw HistogramException("Histogram >"+ m_histDef.path + "< has invalid kLBNHistoryDepth.");
      }

      const int historyDepth = m_histDef.kLBNHistoryDepth;
      const unsigned lumiPage = lumiBlock/historyDepth;

      // Helpers to calculate min/max lumiblock for a given "page" (histogram)
      auto minLumiBlock = [=](unsigned lumiPage) { return lumiPage * historyDepth; };
      auto maxLumiBlock = [=](unsigned lumiPage) { return (lumiPage+1) * historyDepth - 1; };

      // create new histogram definition with updated alias
      HistogramDef def = m_histDef;
      def.alias += "_LB" + std::to_string(minLumiBlock(lumiPage));
      if (historyDepth > 1) {
        def.alias += "_" + std::to_string(maxLumiBlock(lumiPage));
      }

      TNamed* hist = m_factory->create(def);
      m_hists.emplace(maxLumiBlock(lumiPage), std::make_pair(std::move(def), hist));

      // deregister old histograms
      for (auto it = m_hists.begin(); it!=m_hists.end();) {
        const unsigned maxLB = it->first;
        if (maxLB + s_deregDelay <= lumiBlock) {
          m_factory->remove(it->second.first);
          it = m_hists.erase(it);  // this advances iterator by one
        }
        else break;  // don't need to search further as std::map is sorted
      }

      return hist;
    }

  private:
    GenericMonitoringTool* const m_gmTool;
    std::shared_ptr<HistogramFactory> m_factory;
    const HistogramDef m_histDef;

    /**
     * Number of lumiblocks before histogram gets deregistered/deleted.
     * This number needs to be set large enough for each histogram to be at
     * least published once in the online. E.g. with a publication interval
     * of 80 seconds, it needs to be at least 2 (see ADHI-4905).
     */
    static const unsigned s_deregDelay{5};

    /**
     * Map storing upper end of lumi page (or lumiblock if depth==1)
     * with the corresponding histogram definition and pointer of currently
     * active histograms.
     *
     * Note: map is sorted by key (lumiblock)
     */
    std::map<unsigned, std::pair<HistogramDef, TNamed*>> m_hists;
    std::mutex m_mutex;  ///<! mutex for the map
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_LumiblockHistogramProvider_h */

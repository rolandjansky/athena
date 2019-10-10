/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_HistogramFiller_OfflineHistogramProvider_h
#define AthenaMonitoringKernel_HistogramFiller_OfflineHistogramProvider_h

#include <memory>

#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "AthenaMonitoringKernel/HistogramDef.h"
#include "AthenaMonitoringKernel/IHistogramProvider.h"

#include "GaudiKernel/ContextSpecificPtr.h"

#include "HistogramFactory.h"

namespace Monitored {
  /**
   * @brief Implementation of IHistogramProvider for offline histograms
   */
  class OfflineHistogramProvider : public IHistogramProvider {
  public:
    /**
     * @brief Constructor
     *
     * @param gmTool Source of the lumi block info
     * @param factory ROOT object factory
     * @param def General definition of a histogram
     */
    OfflineHistogramProvider(GenericMonitoringTool* const gmTool,
                             std::shared_ptr<HistogramFactory> factory,
                             const HistogramDef& histDef)
    : IHistogramProvider()
    , m_gmTool(gmTool)
    , m_factory(factory)
    , m_histDef(new HistogramDef(histDef))
    , m_objcache({0, 0, nullptr})
    {}

    /**
     * @brief Getter of ROOT object
     *
     * Each time the method is called, factory produces ROOT object based on the current
     * lumi block. Note: ROOT objects are cached at the factory. Nevertheless, it is 
     * recommended to call this method as rarely as possible.
     *
     * @return ROOT object
     */
    TNamed* histogram() override {
      const unsigned runNumber = m_gmTool->runNumber();
      const unsigned lumiBlock = m_gmTool->lumiBlock();

      // do we have the object already?
      std::scoped_lock lock(m_cacheMutex);
      objcache& objcacheref = m_objcache;
      if (objcacheref.lumiBlock == lumiBlock
	  && objcacheref.runNumber == runNumber
	  && objcacheref.object) {
	return objcacheref.object;
      }

      std::string conv = m_histDef->convention;
      std::string lbString;
      if ( conv.find("run")!=std::string::npos ) {
        lbString = "";
      } else if ( conv.find("lowStat")!=std::string::npos ) {
        const unsigned lbBase = lumiBlock-(lumiBlock%20);
        lbString = "/lowStat_LB"+std::to_string(lbBase+1)+"-"+std::to_string(lbBase+20);
      } else {
        lbString = "/lb_"+std::to_string(lumiBlock);
      }
      m_histDef->tld = "/run_"+std::to_string(runNumber)+lbString+"/";

      objcacheref.lumiBlock = lumiBlock;
      objcacheref.runNumber = runNumber;
      objcacheref.object = m_factory->create(*m_histDef);
      return objcacheref.object;
    }

  private:
    GenericMonitoringTool* const m_gmTool;
    std::shared_ptr<HistogramFactory> m_factory;
    std::shared_ptr<HistogramDef> m_histDef;

    struct objcache {
      unsigned int runNumber;
      unsigned int lumiBlock;
      TNamed* object;
    };
    mutable Gaudi::Hive::ContextSpecificData<objcache> m_objcache ATLAS_THREAD_SAFE;
    std::mutex m_cacheMutex;
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_OfflineHistogramProvider_h */

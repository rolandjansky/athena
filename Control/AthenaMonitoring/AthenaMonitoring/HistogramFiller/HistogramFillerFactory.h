/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_HistogramFillerFactory_h
#define AthenaMonitoring_HistogramFiller_HistogramFillerFactory_h

#include <vector>

#include "TH1.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include "AthenaMonitoring/HistogramDef.h"

#include "AthenaMonitoring/HistogramFiller/HistogramFillerCreateException.h"

namespace Monitored {
  class HistogramFiller;

  class HistogramFillerFactory {
  public:
    HistogramFillerFactory(const ServiceHandle<ITHistSvc>& histSvc, std::string groupName);
    virtual ~HistogramFillerFactory();
    HistogramFiller* create(const HistogramDef& def); //!< creates filler

  private:
    template<class H, class HBASE, typename... Types> 
    HBASE* create(const HistogramDef& def, Types&&... hargs);
    template<class H> 
    TH1* create1D(const HistogramDef& def);
    template<class H> 
    TH1* create1DProfile(const HistogramDef& def);
    template<class H> 
    TH1* create2D(const HistogramDef& def);
    template<class H> 
    TH1* create2DProfile(const HistogramDef& def);
    TEfficiency* createEfficiency(const HistogramDef& def);

    std::string getFullName(const HistogramDef& def);
    
    static void setOpts(TH1* hist, const std::string& opt);
    static void setLabels(TH1* hist, const std::vector<std::string>& labels);

    ServiceHandle<ITHistSvc> m_histSvc;
    std::string m_groupName; //!< defines location of histograms
  };
}

#endif /* AthenaMonitoring_HistogramFiller_HistogramFillerFactory_h */
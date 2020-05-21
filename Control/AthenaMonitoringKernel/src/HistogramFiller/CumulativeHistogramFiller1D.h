/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoringKernel_HistogramFiller_CumulativeHistogramFiller1D_h
#define AthenaMonitoringKernel_HistogramFiller_CumulativeHistogramFiller1D_h

#include "HistogramFiller1D.h"

namespace Monitored {
  /**
   * @brief Filler for 1D histograms filled in cummulative mode
   */
  class CumulativeHistogramFiller1D : public HistogramFiller1D {
  public:
    CumulativeHistogramFiller1D(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : HistogramFiller1D(definition, provider) {}

    virtual CumulativeHistogramFiller1D* clone() const override {
      return new CumulativeHistogramFiller1D( *this );
    }

    
    virtual unsigned fill() const override {
      if (m_monVariables.size() != 1) {
        return 0;
      }

      size_t varVecSize = m_monVariables.at(0).get().size();

      // handling of the cutmask
      auto cutMaskValuePair = getCutMaskFunc();
      if (cutMaskValuePair.first == 0) { return 0; }
      if (ATH_UNLIKELY(cutMaskValuePair.first > 1 && cutMaskValuePair.first != varVecSize)) {
        MsgStream log(Athena::getMessageSvc(), "CumulativeHistogramFiller1D");
        log << MSG::ERROR << "CutMask does not match the size of plotted variable: " 
            << cutMaskValuePair.first << " " << varVecSize << endmsg;
      }
      auto cutMaskValue = cutMaskValuePair.second;

      unsigned i(0);
      auto histogram = this->histogram<TH1>();
      auto valuesVector{m_monVariables[0].get().getVectorRepresentation()};
      size_t idx = 0;
      for (auto value : valuesVector) {
        if (!cutMaskValue(idx++)) { continue; }
        unsigned bin = histogram->FindBin(value);

        for (unsigned j = bin; j > 0; --j) {
          histogram->AddBinContent(j);
        }

        ++i;
      }

      return i;  
    }
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_CumulativeHistogramFiller1D_h */

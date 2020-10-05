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


    
    virtual unsigned fill( const HistogramFiller::VariablesPack& vars ) const override {
      if ( vars.size() != 1) {
        return 0;
      }

      const size_t varVecSize = vars.var[0]->size();

      // handling of the cutmask
      auto cutMaskValuePair = getCutMaskFunc( vars.cut );
      if (cutMaskValuePair.first == 0) { return 0; }
      if (ATH_UNLIKELY(cutMaskValuePair.first > 1 && cutMaskValuePair.first != varVecSize)) {
        MsgStream log(Athena::getMessageSvc(), "CumulativeHistogramFiller1D");
        log << MSG::ERROR << "CutMask does not match the size of plotted variable: " 
            << cutMaskValuePair.first << " " << varVecSize << endmsg;
      }
      auto cutMaskValue = cutMaskValuePair.second;
      unsigned i{0};
      auto histogram = this->histogram<TH1>();
      for (; i < varVecSize; i++) {
        if (!cutMaskValue(i)) { continue; }
        const unsigned bin = histogram->FindBin(vars.var[0]->get(i));

        for (unsigned j = bin; j > 0; --j) {
          histogram->AddBinContent(j);
        }
      }

      return i;  
    }
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_CumulativeHistogramFiller1D_h */

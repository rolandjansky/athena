/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoringKernel_HistogramFiller_VecHistogramFiller1D_h
#define AthenaMonitoringKernel_HistogramFiller_VecHistogramFiller1D_h

#include "HistogramFiller1D.h"

namespace Monitored {
  class VecHistogramFiller1D : public HistogramFiller1D {
  public:
    VecHistogramFiller1D(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : HistogramFiller1D(definition, provider) {}

    virtual VecHistogramFiller1D* clone() const override {
      return new VecHistogramFiller1D( *this );
    }

    virtual unsigned fill() const override {
      if (m_monVariables.size() != 1) { return 0; }
      const IMonitoredVariable& var = m_monVariables[0].get();

      std::function<bool(size_t)> cutMaskAccessor;
      if (m_monCutMask) {
        // handling of the cutmask
        auto cutMaskValuePair = getCutMaskFunc();
        if (cutMaskValuePair.first == 0) { return 0; }
        if (ATH_UNLIKELY(cutMaskValuePair.first > 1 && cutMaskValuePair.first != var.size())) {
          MsgStream log(Athena::getMessageSvc(), "VecHistogramFiller1D");
          log << MSG::ERROR << "CutMask does not match the size of plotted variable: "
              << cutMaskValuePair.first << " " << var.size() << endmsg;
        }
        cutMaskAccessor = cutMaskValuePair.second;
      }

      auto histogram = this->histogram<TH1>();
      const unsigned offset = m_histDef->kVecUO ? 0 : 1;
      for (unsigned i = 0; i < var.size(); ++i) {
        if (cutMaskAccessor && cutMaskAccessor(i)) {
          const double value = var.get(i);
          histogram->AddBinContent(i+offset, value);
          histogram->SetEntries(histogram->GetEntries() + value);
        }
      }

      return var.size();
    }
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_VecHistogramFiller1D_h */

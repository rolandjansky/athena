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


    virtual unsigned fill(const HistogramFiller::VariablesPack& vars) const override {
      if ( ATH_UNLIKELY( vars.var[0] == nullptr or vars.size() != 0 ) ) { return 0; }

      std::function<bool(size_t)> cutMaskAccessor;
      if (vars.cut) {
        // handling of the cutmask
        auto cutMaskValuePair = getCutMaskFunc(vars.cut);
        if (cutMaskValuePair.first == 0) { return 0; }
        if (ATH_UNLIKELY(cutMaskValuePair.first > 1 && cutMaskValuePair.first != vars.var[0]->size())) {
          MsgStream log(Athena::getMessageSvc(), "VecHistogramFiller1D");
          log << MSG::ERROR << "CutMask does not match the size of plotted variable: "
              << cutMaskValuePair.first << " " << vars.var[0]->size() << endmsg;
        }
        cutMaskAccessor = cutMaskValuePair.second;
      }

      auto histogram = this->histogram<TH1>();
      const unsigned offset = m_histDef->kVecUO ? 0 : 1;
      for (unsigned i = 0; i < vars.var[0]->size(); ++i) {
        if (cutMaskAccessor && cutMaskAccessor(i)) {
          const double value = vars.var[0]->get(i);
          histogram->AddBinContent(i+offset, value);
          histogram->SetEntries(histogram->GetEntries() + value);
        }
      }

      return vars.var[0]->size();
    }
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_VecHistogramFiller1D_h */

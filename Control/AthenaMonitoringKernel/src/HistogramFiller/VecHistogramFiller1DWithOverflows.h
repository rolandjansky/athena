/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoringKernel_HistogramFiller_VecHistogramFiller1DWithOverflows_h
#define AthenaMonitoringKernel_HistogramFiller_VecHistogramFiller1DWithOverflows_h

#include "HistogramFiller1D.h"

namespace Monitored {
  class VecHistogramFiller1DWithOverflows : public HistogramFiller1D {
  public:
    VecHistogramFiller1DWithOverflows(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : HistogramFiller1D(definition, provider) {}

    virtual VecHistogramFiller1DWithOverflows* clone() const override {
      return new VecHistogramFiller1DWithOverflows( *this );
    }

    virtual unsigned fill() const override {
      if (m_monVariables.size() != 1) {
        return 0;
      }

      // handling of the cutmask
      auto cutMaskValuePair = getCutMaskFunc();
      if (cutMaskValuePair.first == 0) { return 0; }
      auto cutMaskAccessor = cutMaskValuePair.second;

      auto histogram = this->histogram<TH1>();
      const auto valuesVector{m_monVariables[0].get().getVectorRepresentation()};
      if (ATH_UNLIKELY(cutMaskValuePair.first > 1 && cutMaskValuePair.first != valuesVector.size())) {
        MsgStream log(Athena::getMessageSvc(), "VecHistogramFiller1DWithOverflows");
        log << MSG::ERROR << "CutMask does not match the size of plotted variable: " 
            << cutMaskValuePair.first << " " << valuesVector.size() << endmsg;
      }
      std::scoped_lock lock(*m_mutex);

      for (unsigned i = 0; i < std::size(valuesVector); ++i) {
        if (cutMaskAccessor(i)) {
          auto value = valuesVector[i];
          histogram->AddBinContent(i, value);
          histogram->SetEntries(histogram->GetEntries() + value);
        }
      }

      return std::size(valuesVector);
    }
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_VecHistogramFiller1DWithOverflows_h */

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoringKernel_HistogramFiller_HistogramFillerEfficiency_h
#define AthenaMonitoringKernel_HistogramFiller_HistogramFillerEfficiency_h

#include "TEfficiency.h"

#include "AthenaMonitoringKernel/HistogramFiller.h"
#include "HistogramFillerUtils.h"

namespace Monitored {
  /**
   * @brief Filler for TEfficiency graphs
   */
  class HistogramFillerEfficiency : public HistogramFiller {
  public:
    HistogramFillerEfficiency(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : HistogramFiller(definition, provider) {}

    virtual HistogramFillerEfficiency* clone() const override {
      return new HistogramFillerEfficiency( *this );
    }

    virtual unsigned fill() const override {
      const size_t nMonVar = m_monVariables.size();
      if ( nMonVar<2 ) return 0;

      const IMonitoredVariable& var0 = m_monVariables[0].get();

      auto cutMaskValuePair = getCutMaskFunc();
      if (cutMaskValuePair.first == 0) { return 0; }
      if (ATH_UNLIKELY(cutMaskValuePair.first > 1 && cutMaskValuePair.first != var0.size())) {
        MsgStream log(Athena::getMessageSvc(), "HistogramFillerEfficiency");
        log << MSG::ERROR << "CutMask does not match the size of plotted variable: " 
            << cutMaskValuePair.first << " " << var0.size() << endmsg;
      }
      auto cutMaskAccessor = cutMaskValuePair.second;

      TEfficiency* efficiency = this->histogram<TEfficiency>();
      const TH1* efftot = efficiency->GetTotalHistogram();

      if ( nMonVar==2 ) { // Single observable (1D TEfficiency)
        for (unsigned i = 0; i < var0.size(); ++i) {
          if (cutMaskAccessor(i)) {
            efficiency->Fill(var0.get(i),
                             detail::getFillValue<Axis::X>(efftot, m_monVariables[1].get(), i));
          }
        }
        return var0.size();
      } else if ( nMonVar==3 ) { // Two observables (2D TEfficiency)
        for (unsigned i = 0; i < var0.size(); ++i) {
          if (cutMaskAccessor(i)) {
            efficiency->Fill(var0.get(i),
                             detail::getFillValue<Axis::X>(efftot, m_monVariables[1].get(), i),
                             detail::getFillValue<Axis::Y>(efftot, m_monVariables[2].get(), i));
          }
        }
        return var0.size();
      } else if ( nMonVar==4 ) { // Three observables (3D Efficiency)
        for (unsigned i = 0; i < var0.size(); ++i) {
          if (cutMaskAccessor(i)) {
            efficiency->Fill(var0.get(i),
                             detail::getFillValue<Axis::X>(efftot, m_monVariables[1].get(), i),
                             detail::getFillValue<Axis::Y>(efftot, m_monVariables[2].get(), i),
                             detail::getFillValue<Axis::Z>(efftot, m_monVariables[3].get(), i));
          }
        }
        return var0.size();
      } else {
        return 0;
      }
    }
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_HistogramFillerEfficiency_h */

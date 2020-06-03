/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_HistogramFiller_HistogramFiller1D_h
#define AthenaMonitoringKernel_HistogramFiller_HistogramFiller1D_h

#include "TH1.h"

#include "AthenaMonitoringKernel/HistogramFiller.h"
#include "HistogramFillerUtils.h"

#include "CxxUtils/AthUnlikelyMacros.h"
#include "GaudiKernel/MsgStream.h"

namespace Monitored {

  /**
   * @brief Filler for plain 1D histograms
   */
  class HistogramFiller1D : public HistogramFiller {
  public:
    HistogramFiller1D(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : HistogramFiller(definition, provider) {
    }

    virtual HistogramFiller1D* clone() const override {
      return new HistogramFiller1D( *this );
    }

    virtual unsigned fill() const override {
      if (m_monVariables.size() != 1) { return 0; }
      const IMonitoredVariable& var = m_monVariables.at(0).get();

      std::function<bool(size_t)> cutMaskAccessor;
      if (m_monCutMask) {
        // handling of the cutmask
        auto cutMaskValuePair = getCutMaskFunc();
        if (cutMaskValuePair.first == 0) { return 0; }
        if (ATH_UNLIKELY(cutMaskValuePair.first > 1 && cutMaskValuePair.first != var.size())) {
          MsgStream log(Athena::getMessageSvc(), "HistogramFiller1D");
          log << MSG::ERROR << "CutMask does not match the size of plotted variable: "
              << cutMaskValuePair.first << " " << var.size() << endmsg;
        }
        cutMaskAccessor = cutMaskValuePair.second;
      }

      if (m_monWeight) {
        auto weightAccessor = [&](size_t i){ return m_monWeight->get(i); };

        if (ATH_UNLIKELY(m_monWeight->size() != var.size())) {
          MsgStream log(Athena::getMessageSvc(), "HistogramFiller1D");
          log << MSG::ERROR << "Weight does not match the size of plotted variable: "
              << m_monWeight->size() << " " << var.size() << endmsg;
        }
        // Need to fill here while weightVector is still in scope
        if (not m_monCutMask) return HistogramFiller::fill<TH1>(weightAccessor, detail::noCut, var);
        else                  return HistogramFiller::fill<TH1>(weightAccessor, cutMaskAccessor, var);
      }

      if (not m_monCutMask) return HistogramFiller::fill<TH1>(detail::noWeight, detail::noCut, var);
      else                  return HistogramFiller::fill<TH1>(detail::noWeight, cutMaskAccessor, var);
    }
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_HistogramFiller1D_h */

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_HistogramFiller_HistogramFiller2D_h
#define AthenaMonitoringKernel_HistogramFiller_HistogramFiller2D_h

#include "TH2.h"

#include "AthenaMonitoringKernel/HistogramFiller.h"
#include "HistogramFillerUtils.h"
#include "CxxUtils/AthUnlikelyMacros.h"
#include "GaudiKernel/MsgStream.h"

namespace Monitored {

  /**
   * @brief Generic filler for 2D histogram
   *
   * @tparam H  Type of 2D histogram (TH2, TProfile)
   */
  template<typename H>
  class HistogramFiller2DGeneric : public HistogramFiller {
  public:
    HistogramFiller2DGeneric(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : HistogramFiller(definition, provider) {}

    virtual HistogramFiller2DGeneric* clone() const override {
      return new HistogramFiller2DGeneric( *this );
    }
    
    virtual unsigned fill() const override {
      if (ATH_UNLIKELY(m_monVariables.size() != 2)) return 0;

      const IMonitoredVariable& var1 = m_monVariables[0].get();
      const IMonitoredVariable& var2 = m_monVariables[1].get();
      const size_t size1 = var1.size();
      const size_t size2 = var2.size();

      if (ATH_UNLIKELY(size1 == 0 || size2 == 0)) {
        // nothing to do
        return 0;
      }

      if (ATH_UNLIKELY(size1 > 1 && size2 > 1 && size1 != size2)) {
        MsgStream log(Athena::getMessageSvc(), "HistogramFiller2D");
        log << MSG::ERROR << "Mismatch of provided vector sizes "
            << size1 << "," << size2 << " for " << m_histDef->alias << endmsg;
        return 0;
      }

      std::function<bool(size_t)> cutMaskAccessor;
      if (m_monCutMask) {
        // handling of the cutmask
        auto cutMaskValuePair = getCutMaskFunc();
        if (cutMaskValuePair.first == 0) { return 0; }
        if (ATH_UNLIKELY(size1 > 1 && size2 > 1 &&
                         cutMaskValuePair.first > 1 && size1 != cutMaskValuePair.first)) {
          MsgStream log(Athena::getMessageSvc(), "HistogramFiller2D");
          log << MSG::ERROR << "CutMask does not match the size of plotted variable: "
              << cutMaskValuePair.first << " " << size1 << endmsg;
          return 0;
        }
        cutMaskAccessor = cutMaskValuePair.second;
      }

      if (m_monWeight) {
        auto weightAccessor = [&](size_t i){ return m_monWeight->get(i); };

        if (ATH_UNLIKELY(size1 > 1 && size2 > 1 &&
                         m_monWeight->size() > 1 && size1 != m_monWeight->size())) {
          MsgStream log(Athena::getMessageSvc(), "HistogramFiller2D");
          log << MSG::ERROR << "Weight does not match the size of plotted variable: "
              << m_monWeight->size() << " " << size1 << endmsg;
          return 0;
        }
        // Need to fill here while weightVector is still in scope
        if (not m_monCutMask) return HistogramFiller::fill<H>(weightAccessor, detail::noCut, var1, var2);
        else                  return HistogramFiller::fill<H>(weightAccessor, cutMaskAccessor, var1, var2);
      }

      if (not m_monCutMask) return HistogramFiller::fill<H>(detail::noWeight, detail::noCut, var1, var2);
      else                  return HistogramFiller::fill<H>(detail::noWeight, cutMaskAccessor, var1, var2);
    }
  };

  typedef HistogramFiller2DGeneric<TH2> HistogramFiller2D;
}

#endif /* AthenaMonitoringKernel_HistogramFiller_HistogramFiller2D_h */

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
   * @brief Filler for plain 2D histogram
   */
  class HistogramFiller2D : public HistogramFiller {
  public:
    HistogramFiller2D(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : HistogramFiller(definition, provider) {}

    virtual HistogramFiller2D* clone() const override {
      return new HistogramFiller2D( *this ); 
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
        const std::vector<double> weightVector{m_monWeight->getVectorRepresentation()};
        auto weightAccessor = [&](size_t i){ return weightVector[i]; };

        if (ATH_UNLIKELY(size1 > 1 && size2 > 1 &&
                         weightVector.size() > 1 && size1 != weightVector.size())) {
          MsgStream log(Athena::getMessageSvc(), "HistogramFiller2D");
          log << MSG::ERROR << "Weight does not match the size of plotted variable: "
              << weightVector.size() << " " << size1 << endmsg;
          return 0;
        }
        // Need to fill here while weightVector is still in scope
        if (not m_monCutMask) return HistogramFiller::fill<TH2>(weightAccessor, detail::noCut, var1, var2);
        else                  return HistogramFiller::fill<TH2>(weightAccessor, cutMaskAccessor, var1, var2);
      }

      if (not m_monCutMask) return HistogramFiller::fill<TH2>(detail::noWeight, detail::noCut, var1, var2);
      else                  return HistogramFiller::fill<TH2>(detail::noWeight, cutMaskAccessor, var1, var2);
    }
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_HistogramFiller2D_h */

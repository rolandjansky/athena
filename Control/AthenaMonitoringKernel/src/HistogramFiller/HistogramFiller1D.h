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

    virtual unsigned fill( const HistogramFiller::VariablesPack& vars ) const override {
      std::function<bool(size_t)> cutMaskAccessor;
      if ( vars.cut ) {
        // handling of the cutmask
        auto cutMaskValuePair = getCutMaskFunc(vars.cut);
        if (cutMaskValuePair.first == 0) { return 0; }
        if (ATH_UNLIKELY(cutMaskValuePair.first > 1 && cutMaskValuePair.first != vars.var[0]->size())) {
          MsgStream log(Athena::getMessageSvc(), "HistogramFiller1D");
          log << MSG::ERROR << "CutMask does not match the size of plotted variable: "
              << cutMaskValuePair.first << " " << vars.var[0]->size() << endmsg;
        }
        cutMaskAccessor = cutMaskValuePair.second;
      }

      if (vars.weight) {
        auto weightAccessor = [&](size_t i){ return vars.weight->get(i); };

        if (ATH_UNLIKELY(vars.weight->size() != vars.var[0]->size())) {
          MsgStream log(Athena::getMessageSvc(), "HistogramFiller1D");
          log << MSG::ERROR << "Weight does not match the size of plotted variable: "
              << vars.weight->size() << " " << vars.var[0]->size() << endmsg;
        }
        // Need to fill here while weightVector is still in scope
        if (not vars.cut) return HistogramFiller::fill<TH1>(weightAccessor, detail::noCut, *vars.var[0]);
        else                  return HistogramFiller::fill<TH1>(weightAccessor, cutMaskAccessor, *vars.var[0]);
      }

      if (not vars.cut) return HistogramFiller::fill<TH1>(detail::noWeight, detail::noCut, *vars.var[0]);
      else                  return HistogramFiller::fill<TH1>(detail::noWeight, cutMaskAccessor, *vars.var[0]);      
    }
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_HistogramFiller1D_h */

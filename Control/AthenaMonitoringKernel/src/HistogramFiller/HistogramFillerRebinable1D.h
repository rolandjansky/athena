/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_HistogramFiller_HistogramFillerRebinable1D_h
#define AthenaMonitoringKernel_HistogramFiller_HistogramFillerRebinable1D_h

#include "AthenaMonitoringKernel/OHLockedHist.h"
#include "HistogramFiller1D.h"
#include "HistogramFillerUtils.h"

#include <cmath>

namespace Monitored {
  /**
   * @brief Filler for dynamically rebinable 1D histograms
   */
  class HistogramFillerRebinable1D : public HistogramFiller1D {
  public:
    HistogramFillerRebinable1D(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : HistogramFiller1D(definition, provider) {
      if (definition.kAddBinsDynamically) {
        m_rebinMode = RebinMode::AddBins;
      } else {
        m_rebinMode = RebinMode::ExtendAxes;
      }
    }

    virtual HistogramFillerRebinable1D* clone() const override {
      return new HistogramFillerRebinable1D( *this );
    }

    virtual unsigned fill() override {
      if (m_monVariables.size() != 1) { return 0; }
      {
        const auto valuesVector = m_monVariables[0].get().getVectorRepresentation();
        std::scoped_lock lock(*m_mutex);

        auto histogram = this->histogram<TH1>();
        if (m_rebinMode == RebinMode::AddBins) {
          const auto max = std::max_element(begin(valuesVector), end(valuesVector));
          if (Monitored::detail::shouldRebinHistogram(histogram, *max)) {
            Monitored::detail::rebinHistogram(histogram, *max);
          }
        } else {
          histogram->SetCanExtend(TH1::kAllAxes);
          const auto max = std::max_element(begin(valuesVector), end(valuesVector));
          const auto min = std::min_element(begin(valuesVector), end(valuesVector));
          // ExtendAxis is always an extremely expensive operation; do not
          // invoke if not necessary
          auto xaxis = histogram->GetXaxis();
          if (*max >= xaxis->GetXmax()) histogram->ExtendAxis(*max,histogram->GetXaxis());
          if (*min < xaxis->GetXmin()) histogram->ExtendAxis(*min,histogram->GetXaxis());
        }
      }
      return HistogramFiller1D::fill();
    }

  private:
    enum class RebinMode { AddBins, ExtendAxes };
    RebinMode m_rebinMode;
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_HistogramFillerRebinable1D_h */

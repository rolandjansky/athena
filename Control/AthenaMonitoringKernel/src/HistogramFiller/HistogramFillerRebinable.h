/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_HistogramFiller_HistogramFillerRebinable1D_h
#define AthenaMonitoringKernel_HistogramFiller_HistogramFillerRebinable1D_h

#include "AthenaMonitoringKernel/OHLockedHist.h"
#include "HistogramFiller1D.h"
#include "HistogramFiller2D.h"
#include "HistogramFillerUtils.h"

#include <cmath>

namespace Monitored {

  /**
   * @brief Mixin class for rebinning of histogram axis
   *
   * @tparam BASE  HistogramFiller base class
   * @tparam AXIS  Histogram axis that should be made rebinable
   */
  template<typename BASE, Monitored::Axis AXIS>
  class HistogramFillerRebinableAxis : public BASE {
  public:

    HistogramFillerRebinableAxis(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : BASE(definition, provider) {
      if (definition.kAddBinsDynamically) {
        m_rebinMode = RebinMode::AddBins;
      } else {
        m_rebinMode = RebinMode::ExtendAxes;
      }
    }

    virtual unsigned fill( const HistogramFiller::VariablesPack& vars ) const override {
      if (AXIS >= vars.size() ) { return 0; }
      if (vars.var[AXIS]->size()==0) { return 0; }

      double min = std::numeric_limits<double>::max();
      double max = std::numeric_limits<double>::min();
      const IMonitoredVariable& var = *vars.var[AXIS];
      for (size_t i = 0; i < var.size(); i++) {
        const double v = var.get(i);
        if (v < min) min = v;
        if (v > max) max = v;
      }

      TH1* hist = this->template histogram<TH1>();
      TAxis* axis = detail::getAxis<AXIS>(hist);
      if (m_rebinMode == RebinMode::AddBins) {
        if (Monitored::detail::shouldRebinHistogram(axis, max)) {
          Monitored::detail::rebinHistogram<AXIS>(hist, max);
        }
      } else {
        hist->SetCanExtend(detail::axis_bit[AXIS]);
        // ExtendAxis is an extremely expensive operation; only call if necessary
        if (max >= axis->GetXmax()) hist->ExtendAxis(max, axis);
        if (min < axis->GetXmin()) hist->ExtendAxis(min, axis);
      }
      return BASE::fill( vars );
    }

  private:
    enum class RebinMode { AddBins, ExtendAxes };
    RebinMode m_rebinMode;
  };

  /// Rebinable 1D histogram
  typedef HistogramFillerRebinableAxis<HistogramFiller1D, Axis::X> HistogramFillerRebinable1D;
  /// Rebinable 2D histogram (x-axis only)
  typedef HistogramFillerRebinableAxis<HistogramFiller2D, Axis::X> HistogramFillerRebinable2DX;
  /// Rebinable 2D histogram (y-axis only)
  typedef HistogramFillerRebinableAxis<HistogramFiller2D, Axis::Y> HistogramFillerRebinable2DY;
  /// Rebinable 2D histogram (both axes)
  typedef HistogramFillerRebinableAxis<HistogramFillerRebinable2DX, Axis::Y> HistogramFillerRebinable2D;
}

#endif /* AthenaMonitoringKernel_HistogramFiller_HistogramFillerRebinable1D_h */

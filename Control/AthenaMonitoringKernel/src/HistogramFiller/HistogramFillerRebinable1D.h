/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_HistogramFiller_HistogramFillerRebinable1D_h
#define AthenaMonitoringKernel_HistogramFiller_HistogramFillerRebinable1D_h

#include "HistogramFiller1D.h"

#include <cmath>

namespace Monitored {
  /**
   * @brief Filler for dynamically rebinable 1D histograms
   */
  class HistogramFillerRebinable1D : public HistogramFiller1D {
  public:
    HistogramFillerRebinable1D(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : HistogramFiller1D(definition, provider) {
      if (definition.opt.find("kAddBinsDynamically") != std::string::npos) {
	m_rebinMode = RebinMode::AddBins;
      } else {
	m_rebinMode = RebinMode::ExtendAxes;
      }
    }

    virtual HistogramFillerRebinable1D* clone() override {
      return new HistogramFillerRebinable1D( *this );
    }

    virtual unsigned fill() override {
      if (m_monVariables.size() != 1) { return 0; }
      {
	const auto valuesVector = m_monVariables[0].get().getVectorRepresentation();
	std::lock_guard<std::mutex> lock(*(this->m_mutex));

	if (m_rebinMode == RebinMode::AddBins) {
	  const auto max = std::max_element(begin(valuesVector), end(valuesVector));
	  if (shouldRebinHistogram(*max)) { rebinHistogram(*max); }
	} else {
	  auto histogram = this->histogram<TH1>();
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
    /**
     * Method checks if histogram should be rebinned. It should happen when the new value
     * is greater or equal to the greatest value of the histogram.
     */
    bool shouldRebinHistogram(const double value) {
      const auto histogram = this->histogram<TH1>();
      const double xMax = histogram->GetXaxis()->GetXmax();

      return xMax <= value;
    }

    /**
     * Method that actualy rebin entire histogram.
     *
     * Algorithm will increase a size of the histogram until `value` can be included in new range of values.
     * Examples for xMin=1.0, xMax=3.0;
     *  value=2.9, xMax will be unchanged
     *  value=3.0, xMax will be doubled
     *  value=4.9, xMax will be doubled
     *  value=5.0, xMax will be quadrupled
     *  value=5.1, xMax will be quadrupled
     *  value=8.9, xMax will be quadrupled
     *  value=9.0, xMax will be octupled
     *  value=9.1, xMax will be octupled
     */
    void rebinHistogram(const double value) {
      auto histogram = this->histogram<TH1>();

      histogram->SetCanExtend(TH1::kAllAxes);

      do {
        histogram->LabelsInflate();
      } while (shouldRebinHistogram(value));
    }
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_HistogramFillerRebinable1D_h */

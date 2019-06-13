/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_HistogramFillerRebinable1D_h
#define AthenaMonitoring_HistogramFiller_HistogramFillerRebinable1D_h

#include "HistogramFiller1D.h"

#include <cmath>

namespace Monitored {
  /**
   * @brief Filler for dynamically rebinable 1D histograms
   */
  class HistogramFillerRebinable1D : public HistogramFiller1D {
  public: 
    HistogramFillerRebinable1D(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : HistogramFiller1D(definition, provider) {}

    HistogramFillerRebinable1D* clone() override { return new HistogramFillerRebinable1D(*this); };

    virtual unsigned fill() override {
      if (m_monVariables.size() != 1) { return 0; }

      const auto valuesVector = m_monVariables[0].get().getVectorRepresentation();
      const auto max = std::max_element(begin(valuesVector), end(valuesVector));

      std::lock_guard<std::mutex> lock(*(this->m_mutex));

      if (shouldRebinHistogram(*max)) { rebinHistogram(*max); }

      return fillHistogram();
    } 

  private:
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

#endif /* AthenaMonitoring_HistogramFiller_HistogramFillerRebinable1D_h */
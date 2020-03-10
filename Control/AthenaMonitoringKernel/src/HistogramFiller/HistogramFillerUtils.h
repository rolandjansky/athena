/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_HistogramFillerUtils_h
#define AthenaMonitoringKernel_HistogramFillerUtils_h

namespace Monitored {
  namespace detail {
    /**
     * Method checks if histogram should be rebinned. It should happen when the new value
     * is greater or equal to the greatest value of the histogram.
     */
    template<typename H>
    bool shouldRebinHistogram(const H* hist, const double value) {
      return hist->GetXaxis()->GetXmax() <= value;
    }

    /**
     * Method that rebins a histogram.
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
    template<typename H>
    void rebinHistogram(H* hist, const double value) {
      hist->SetCanExtend(TH1::kAllAxes);

      // Rebinning requires to take OH lock in online (no-op offline)
      oh_scoped_lock_histogram lock;
      do {
        hist->LabelsInflate();
      } while (shouldRebinHistogram(hist, value));
    }
  }
}

#endif

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_HistogramFillerUtils_h
#define AthenaMonitoringKernel_HistogramFillerUtils_h

#include "AthenaMonitoringKernel/OHLockedHist.h"

#include "TH1.h"
#include "THashList.h"

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

    /**
     * Check if Fill would result in rebinning.
     *
     * Determine if Fill(value) would result in a rebinning of the histogram.
     */
    template<typename T>
    bool fillWillRebinHistogram(const TAxis* axis, T value) {
      if (not axis) return false;
      const int bin = axis->FindFixBin(value);
      // if under/overflow
      if ( bin==0 or bin==axis->GetNbins()+1 ) {
        return true;
      }
      return false;
    }

    /**
     * Check if Fill would result in rebinning for alphanumeric axis.
     */
    template<>
    bool fillWillRebinHistogram(const TAxis* axis, const char* value) {
      // valid bin found
      if ( not axis or axis->FindFixBin(value)>0 ) return false;

      // If there are no labels yet at least one unlabeled bin is available
      const THashList* labels = axis->GetLabels();
      if ( not labels ) return false;

      // check if unlabeled bins exist
      if ( axis->GetNbins() > labels->GetEntries() ) {
        return false;
      }
      return true;
    }

  }
}

#endif

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_HistogramFillerUtils_h
#define AthenaMonitoringKernel_HistogramFillerUtils_h

#include <algorithm>
#include <utility>

#include "AthenaMonitoringKernel/IMonitoredVariable.h"
#include "AthenaMonitoringKernel/OHLockedHist.h"
#include "CxxUtils/AthUnlikelyMacros.h"

#include "TH1.h"
#include "TProfile.h"
#include "THashList.h"

namespace Monitored {

  /** Helper type for histogram axis selection */
  enum Axis {X = 0, Y, Z};

  namespace detail {

    auto noWeight = [](size_t){ return 1.0; }; ///< no weight for filling
    auto noCut = [](size_t){ return true; };   ///< no cut for filling

    /** Convert axis to ROOT-compatible character */
    constexpr std::array axis_name{"X", "Y", "Z"};
    constexpr std::array axis_bit{TH1::kXaxis, TH1::kYaxis, TH1::kZaxis};

    /**
     * Helper to get corresponding TAxis selected by Monitored::Axis.
     * (works for const/non-const TAxis/TH1 thanks to auto return type)
     */
    template<Axis AXIS, typename H>
    constexpr auto getAxis(H* hist) {
      if constexpr (AXIS==Axis::X) return hist->GetXaxis();
      else if constexpr (AXIS==Axis::Y) return hist->GetYaxis();
      else return hist->GetZaxis();
    }

    /**
     * Return value for filling i'th entry of var into AXIS for hist.
     * If var is string-valued performs a bin lookup first.
     *
     * @tparam AXIS    Histograms axis
     * @param  hist    Histogram
     * @param  var     MonitoredVariable for value lookup
     * @param  i       index for IMonitoredVariable value lookup
     */
    template<Axis AXIS, typename H>
    double getFillValue(const H* hist, const IMonitoredVariable* var, size_t i) {
      if ( var->hasStringRepresentation() ) {
        const TAxis* axis = getAxis<AXIS>(hist);
        const int binNumber = axis->FindFixBin( var->getString(i).c_str() );
        return axis->GetBinCenter(binNumber);
      } else {
        return var->get(i);
      }
    }

    /**
     * Method checks if histogram should be rebinned. It should happen when the new value
     * is greater or equal to the greatest value of the histogram axis.
     */
    bool shouldRebinHistogram(const TAxis* axis, const double value) {
      return axis ? axis->GetXmax() <= value : false;
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
    template<Axis AXIS, typename H>
    void rebinHistogram(H* hist, const double value) {
      hist->SetCanExtend(axis_bit[AXIS]);
      TAxis* a = getAxis<AXIS>(hist);

      // Rebinning requires to take OH lock in online (no-op offline)
      oh_scoped_lock_histogram lock;
      do {
        hist->LabelsInflate(axis_name[AXIS]);
      } while (shouldRebinHistogram(a, value));
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

    // gcc bug #94505
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wparentheses"
    /**
     * Check if any of the histogram axes will be rebinned
     *
     * @param hist  histogram to check
     * @param a...     integer sequence of axes to check (0, 1, ...)
     * @param v...     x, y, ... values to fill
     */
    template<typename H, typename T, T... a, typename ...Vs>
    bool fillWillRebinHistogram(H* hist, std::integer_sequence<T, a...>, const Vs&... v) {
      // First check if axis is extensible, then if value would be outside of range
      return (... || (getAxis<static_cast<Axis>(a)>(hist)->CanExtend() and
                      detail::fillWillRebinHistogram(getAxis<static_cast<Axis>(a)>(hist), v)));
    }
    #pragma GCC diagnostic pop


    /**
     * Perform (arbitrary dimension) histogram fill with weight
     *
     * @param hist     histogram to fill
     * @param weight   weight accessor
     * @param i        IMonitoredVariable entry to fill
     * @param m1,m...  IMonitoredVariable list to fill from
     */
    template<typename H, typename W, typename M, typename ...Ms>
    void doFill(H* hist, W weight, size_t i, const M& m1, const Ms&... m) {

      // Template magic: Recursively convert all M to double or string
      if constexpr(std::is_same_v<M, Monitored::IMonitoredVariable>) {
        // For >=2D: If one variable has a single entry, do repeated fills with that value
        const size_t j = m1.size()==1 ? 0 : i;
        if (not m1.hasStringRepresentation())
          doFill(hist, weight, i, m..., m1.get(j));
        else
          doFill(hist, weight, i, m..., m1.getString(j).c_str());
      } else {
        // In case re-binning occurs need to take the OH lock for online (no-op offline)
        if ( ATH_UNLIKELY(fillWillRebinHistogram(hist, std::index_sequence_for<M, Ms...>{},
                                                 m1, m...)) ){
          oh_scoped_lock_histogram lock;
          hist->Fill(m1, m..., weight(i));
        }
        else hist->Fill(m1, m..., weight(i));
      }
    }

    // TProfile does not support string as y-value.
    // (this terminates the above pack expansion)
    template<typename W>
    void doFill(TProfile*, W, size_t, const double&, const char* const&) {}
    template<typename W>
    void doFill(TProfile*, W, size_t, const char* const&, const char* const&) {}
  }
}

#endif

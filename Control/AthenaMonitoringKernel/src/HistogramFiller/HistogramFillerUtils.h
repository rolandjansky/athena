/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_HistogramFillerUtils_h
#define AthenaMonitoringKernel_HistogramFillerUtils_h

#include <algorithm>
#include <utility>

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
     * Helper to get corresponding TAxis selected by Monitored::Axis
     */
    template<typename H, Axis AXIS>
    constexpr TAxis* getAxis(H* hist) {
      if constexpr (AXIS==Axis::X) return hist->GetXaxis();
      else if constexpr (AXIS==Axis::Y) return hist->GetYaxis();
      else return hist->GetZaxis();
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
      TAxis* a = getAxis<H,AXIS>(hist);

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
      return (... || (getAxis<H, static_cast<Axis>(a)>(hist)->CanExtend() and
                      detail::fillWillRebinHistogram(getAxis<H, static_cast<Axis>(a)>(hist), v)));
    }
    #pragma GCC diagnostic pop


    /**
     * Return value that can be digested by TH1::Fill
     * (We only support double and string)
     */
    template<typename T>
    constexpr auto toFill(const T& value) {
      if constexpr(std::is_same_v<double,T>) return value;
      else return value.c_str();
    }

    /**
     * Perform generic histogram fill with weight
     */
    template<typename H, typename ...Ts>
    void doFill(H* hist, double weight, const Ts&... v) {
      hist->Fill(toFill(v)..., weight);
    }

    /**
     * Perform TProfile fill with weight
     */
    template<typename X, typename Y>
    void doFill(TProfile* hist, [[maybe_unused]] double weight, const X& x, const Y& y) {
      // TProfile does not support string as y-value. It would never be called that way
      // but is required to make the pack expansion compile in HistogramFiller::fill
      if constexpr(std::is_same_v<std::string,Y>) return;
      else hist->Fill(toFill(x), y, weight);
    }

    /**
     * Generic histogram filling helper
     *
     * Works for any dimension and double/string-valued entries.
     * If weight and/or cut are not needed use a lambda expression (instead of std::function)
     * as this will allow the compiler to produce more optimal code.
     *
     * @param hist    histogram to fill
     * @param weight  weight accessor
     * @param cut     cut mask accessor
     * @param v...    vectors used for filling (one for each dimension)
     * @return  number of fills performed
     */
    template<typename H, typename W, typename C, typename ...Vs>
    unsigned fill(H* hist, W weight, C cut, const Vs&... v) {

      // For >=2D: If one variable has a single entry, do repeated fills with that value
      auto get = [](const auto& v, size_t i) { return v.size()==1 ? v[0] : v[i]; };

      unsigned fills = 0;
      for ( size_t i = 0; i < std::max({v.size()...}); ++i ) {
        if ( cut(i) ) {
          fills++;
          // In case re-binning occurs need to take the OH lock for online (no-op offline)
          if ( ATH_UNLIKELY(fillWillRebinHistogram(hist, std::index_sequence_for<Vs...>{},
                                                   toFill(get(v,i))...)) ){
            oh_scoped_lock_histogram lock;
            doFill(hist, weight(i), get(v,i)...);
          }
          else doFill(hist, weight(i), get(v,i)...);
        }
      }
      return fills;
    }
  }
}

#endif

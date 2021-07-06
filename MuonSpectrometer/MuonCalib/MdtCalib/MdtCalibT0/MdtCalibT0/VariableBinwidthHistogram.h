/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//   VariableBinwidthHistogram.h
//   Header file for class VariableBinwidthHistogram
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////

#ifndef VARIABLEBINWIDTHHISTOGRAM_H
#define VARIABLEBINWIDTHHISTOGRAM_H

// c - c++
#include <algorithm>
#include <iostream>
#include <vector>

// ROOT
//#include "TH1.h"
//#include "TGraph.h"

class TH1F;
class TGraph;

// this
#include "VariableBinwidthHistogramBin.h"

namespace MuonCalib {

    /** @class VariableBinwidthHistogram
        A histogram where every bin has the same number of entries. The density is
        represented by the bin width.
        @author Felix.Rauscher@Physik.Uni-Muenchen.De
        @date February 2006

    */
    class VariableBinwidthHistogram {
    public:
        //----------------------------constructors--------------------------------------
        /** Default constructor */
        inline VariableBinwidthHistogram() : m_binc(0), m_max_bin_width(0), m_error(false), m_sorted(false) {}

        /** destructor */
        inline ~VariableBinwidthHistogram() {
            for (unsigned int i = 0; i < m_bins.size(); i++) { delete m_bins[i]; }
        }

        //-----------------------------public member functinos--------------------------

        /** Initialize with new input histogram
            Returns on error false
            @param hist the input histogram
            @param binc_r number of entries per bin / max number of entries in hist
            @param max_bin_width The binwidth will not exceed the maximum_bin_width
            @param min_x only the range between min_x and max_x will be used
            @param max_x only the range between min_x and max_x will be used
        */
        bool Initialize(TH1F *hist, double binc_r, double max_bin_width, double min_x = -9e9, double max_x = 9e9);

        /** Removes steps that origin in a binning effekt
            @param width smoothin parameter (typically the binwidth of the input-hisatogram)
        */
        bool Smooth(double width);

        /** Get a bin
            @param bin The bin index of the bin. bin < VariableBinwidthHistogram :: GetNumberOfBins()
        */
        inline const VariableBinwidthHistogramBin &GetBin(unsigned int bin) const { return *(m_bins[bin]); }

        /** Get a bin sorted by content
            @param bin The bin index of the bin. bin < VariableBinwidthHistogram :: GetNumberOfBins()
        */
        inline const VariableBinwidthHistogramBin &GetSortedBin(unsigned int bin) {
            if (!m_sorted) {
                sort(m_sort_bins.begin(), m_sort_bins.end());
                m_sorted = true;
            }
            return m_sort_bins[bin].Bin();
        }

        /** Get the number of bins  */
        inline unsigned int GetNumberOfBins() const { return m_bins.size(); }

        /** create density graph - density vs bin center */
        TGraph *DenistyGraph() const;

        /** Plot binwidth graph - binwidth versus bin center */
        TGraph *BinWidthGraph() const;

        /** Plot bin content graph - bin content vs bin center */
        TGraph *BinContentGraph() const;

        /** Plot graph with differential density*/
        TGraph *DiffDensityGraph() const;

        /** Plot graph with differential binwidth*/
        TGraph *DiffBinwidthGraph() const;

    private:
        //----------------------------private data members------------------------------
        /// vector containing histogram bins
        std::vector<VariableBinwidthHistogramBin *> m_bins;

        /// bins sorted by content
        std::vector<VBHBinPtrSrt> m_sort_bins;

        /// number of entries per bin
        unsigned int m_binc;

        /// maximum bin width
        double m_max_bin_width;

        /// error flag
        bool m_error;

        bool m_sorted;

        //--------------------------privite member functions----------------------------
        inline double sign(double val) const {
            if (val > 0.0) return 1.0;
            if (val < 0.0) return -1.0;
            if (val == 0.0) return 0;
            return 0;
        }
    };

}  // namespace MuonCalib

#endif

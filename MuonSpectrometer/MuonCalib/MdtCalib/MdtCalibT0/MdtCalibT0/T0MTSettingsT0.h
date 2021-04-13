/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//   MTT0SettingsT0.h
//   Header file for class MTSettings
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////

#ifndef MTT0SETTINGST0_H
#define MTT0SETTINGST0_H

/**@class T0MTSettingsT0
    Settings for the t0 fit to the drift time spectrum
    @author Felix.Rauscher@Physik.Uni-Muenchen.De
    @date Mar 2006
*/

namespace MuonCalib {

    class T0MTSettingsT0 {
    public:
        //===========================constructor========================================
        /** default constructor*/
        T0MTSettingsT0();
        //===========================public member functions============================
        /** @name Parameters for pattern recognition
        Note: These parameters default to well proven parameters. The algorithm should work an all spectra with these. The only parameter
        which is likely to be adapted is MinBackgroundBins()
        */
        //@{
        /** Number of hits per histogram bin for the VariableBinwidthHistogram
            The number is given relative to the maximum bin content of the time-Spectrum
            This number must be >1
            */
        inline const double &VBHBinContent() const { return m_vbh_bin_content_rel; }
        inline double &VBHBinContent() { return m_vbh_bin_content_rel; }
        /** Maximum bin width for the VariableBinwidthHistogram
            The bins will not be wider than this even if it means that they will be underfull */
        inline const double &MaxBinWidth() const { return m_max_bin_width; }

        inline double &MaxBinWidth() { return m_max_bin_width; }
        /** The minimum width of the region for the background estimation.
        This width is given in number of bins of the time spectrum.
        If the pattern recognition selects a smaller region, the lower boarder of the region will be moved to the first bin of the time
        spectrum.

        NOTE: This should be the approximate width of the trigger matching window before . If this parmeter is to big, the background may be
        underestimated (with verry little effect on ).
        */
        inline const int &MinBackgroundBins() const { return m_min_background_bins; }
        inline int &MinBackgroundBins() { return m_min_background_bins; }
        /** Start value for the T-Parameter in the -fit.*/
        inline const double &Tstart() const { return m_T_start; }
        inline double &Tstart() { return m_T_start; }
        /** If true use only the top part of the function for the chi2 calculation*/
        inline const bool &UseTopChi2() const { return m_use_top_chi2; }
        inline bool &UseTopChi2() { return m_use_top_chi2; }
        /** the chi2 threshold at which the scrambling method is used */
        inline const double &ScrambleThreshold() const { return m_scramble_threshold; }
        inline double &ScrambleThreshold() { return m_scramble_threshold; }
        inline double &ScrambleThreshold(const double &scramble_threshold) {
            m_scramble_threshold = scramble_threshold;
            return m_scramble_threshold;
        }
        /** the chi2 threshold at which the slicing method is used */
        inline const double &SlicingThreshold() const { return m_slicing_chi2; }
        inline double &SlicingThreshold() { return m_slicing_chi2; }
        inline double &SlicingThreshold(const double &slicing_threshold) {
            m_slicing_chi2 = slicing_threshold;
            return m_slicing_chi2;
        }
        //@}
        inline const bool &CorrectRelT0s() const { return m_correct_rel_t0s; }
        inline bool &CorrectRelT0s() { return m_correct_rel_t0s; }

    private:
        //===========================private data members=============================
        //! relative bin content for t0-VBH
        double m_vbh_bin_content_rel;
        //! maximum bin width for -VBH
        double m_max_bin_width;
        //! minimum number of bins for -background estimation
        int m_min_background_bins;
        //! T start value
        double m_T_start;
        //! use top chi2 instead of total chi2
        bool m_use_top_chi2;
        //! chi2 limit for scrambling
        double m_scramble_threshold;
        //! chi2 limit for slicing method
        double m_slicing_chi2;
        //! correct for relative t0s
        bool m_correct_rel_t0s;
    };

}  // namespace MuonCalib

#endif

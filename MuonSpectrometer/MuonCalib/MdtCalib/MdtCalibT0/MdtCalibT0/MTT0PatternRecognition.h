/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//   MTT0PattternRecognition.h
//   Header file for class VariableBinwidthHistogram
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////

#ifndef MTT0PATTTERNRECOGNITION_H
#define MTT0PATTTERNRECOGNITION_H

// c - c++
//#include "iostream"
class TH1F;

// root
#include "TH1.h"

// this
#include "T0MTSettings.h"
#include "VariableBinwidthHistogram.h"

namespace MuonCalib {

    /** @class MTT0PattternRecognition
        Performs the pattern recognition for the MT t0-fit routine. Fir ranges and start values are estimated
        @author Felix.Rauscher@Physik.Uni-Muenchen.De
        @date February 2006

    */
    class MTT0PatternRecognition {
    public:
        //-------------------------constructors-----------------------------------------
        /** Default constructor*/
        inline MTT0PatternRecognition() :
            m_settings(NULL),
            m_draw_debug_graph(false),
            m_background(0.),
            m_height(0.),
            m_fit_min(0.),
            m_fit_max(0.),
            m_t0_est(0.),
            m_error(false) {}

        //-------------------------public member functions------------------------------
        /** Initialize class - returns true if pattern recognition was successfull
            @param hist Histogram which is to be fitted
        */
        inline bool Initialize(TH1F *hist, const T0MTSettings *settings) {
            m_settings = settings->T0Settings();
            m_draw_debug_graph = settings->DrawDebugGraphs();
            m_error = !m_vbh.Initialize(hist, m_settings->VBHBinContent(), m_settings->MaxBinWidth());
            double scale_min = 0.;
            if (!m_error) scale_min = estimate_height(hist);
            if (!m_error) estimate_background(hist, scale_min);
            return !m_error;
        }

        /** get the background level */
        inline double GetBackground() const { return m_background; }

        /** get height */
        inline double GetHeight() const { return m_height; }

        /** get estimated t0*/
        inline double GetEstimatedT0() const { return m_t0_est; }

        /** get fit range */
        inline double GetFitRangeMin() const { return m_fit_min; }

        /** get fit range */
        inline double GetFitRangeMax() const { return m_fit_max; }

        /** return error flag */
        inline bool GetError() const { return m_error; }

    private:
        //------------------------private data members----------------------------------
        //! settings
        const T0MTSettingsT0 *m_settings;
        bool m_draw_debug_graph;

        //! background level
        double m_background;

        //! height
        double m_height;

        //! fit range
        double m_fit_min, m_fit_max;

        //! t0 estimate
        double m_t0_est;

        //! Variable binwidth histogram
        VariableBinwidthHistogram m_vbh;

        //! error flag
        bool m_error;

        //-----------------------private member functions-------------------------------

        /**estimates the background level
           @param hist input histogram
           @param scale_min lower end of the scale region as returned from estimate_height()
        */
        bool estimate_background(TH1F *hist, double scale_min);

        /**estimates the height of the spectrum. It returns the lower end of the scale region
           @param hist input histogram
        */
        double estimate_height(TH1F *hist);
    };

}  // namespace MuonCalib
#endif

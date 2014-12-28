/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//   MTTmaxPattternRecognition.h
//   Header file for class VariableBinwidthHistogram
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////


#ifndef MTTMAXPATTTERNRECOGNITION_H
#define MTTMAXPATTTERNRECOGNITION_H

//c - c++
#include "iostream"

//root 
//#include "TH1.h"
class TH1F;

//this
#include "VariableBinwidthHistogram.h"
#include "T0MTSettings.h"

namespace MuonCalib {

/** @class MTTmaxPattternRecognition
    Performs the pattern recognition for the MT tmax-fit routine. 
    Fit ranges and start values are estimated
    @author Felix.Rauscher@Physik.Uni-Muenchen.De
    @date February 2006
*/

  class MTTmaxPatternRecognition {
  public:
    //-------------------------constructors-----------------------------------------
    /** Default constructor*/
    inline MTTmaxPatternRecognition() : 
      m_settings(NULL),
      m_draw_debug_graph(false),
      m_background(0.),
      m_a(0.), 
      m_b(0.),
      m_fit_min(0.), 
      m_fit_max(0.),
      m_tmax_est(0.),
      m_t0(0.),
      m_error(false) {}

//-------------------------public member functions------------------------------
    /** Initialize class
	@param hist Histogram which is to be fitted
	@param t0 comes from the t0 fit
    */
    bool Initialize( TH1F *hist, double t0, const T0MTSettings * settings);
	    
    /** get the background level */
    inline double GetBackground() const {return m_background;}
		
    /** get parameter a in exp-function  representing the end of the spectrum */
    inline double GetA() const {return m_a;}

    /** get parameter a in exp-function  representing the end of the spectrum */
    inline double GetB() const {return m_b;}
		
    /** get estimated t0*/
    inline double GetEstimatedTMax() const {return m_tmax_est;}
    
    /** get fit range */
    inline double GetFitRangeMin() const {return m_fit_min;}
		
    /** get fit range */
    inline double GetFitRangeMax() const {return m_fit_max;}
		
    /** return error flag */
    inline bool GetError() const {return m_error;}	
  private:
//------------------------private data members----------------------------------
    //! settings
    const T0MTSettingsTMax *m_settings;
    bool m_draw_debug_graph;
    
    ///background level
    double m_background;
	
    ///parameters a, b in a*exp(b*x) 
    double m_a, m_b;
		
    ///fit range
    double m_fit_min, m_fit_max;
    
    ///t0 estimate
    double m_tmax_est;
    
    ///Variable binwidth histogram
    VariableBinwidthHistogram m_vbh;
    
    ///t0
    double m_t0;

    /// error flag
    bool m_error;

//-----------------------private member functions-------------------------------
    /**estimates the background level
       @param hist input histogram
    */
    bool estimate_background( TH1F *hist);
		
    /**estimates the height of the spectrum. The height is representet by an exponetial function a*exp(b*t). It returns the upper end of the height region.
       @param hist input histogram
    */
    bool estimate_height( TH1F *hist);
	
  };

}
#endif

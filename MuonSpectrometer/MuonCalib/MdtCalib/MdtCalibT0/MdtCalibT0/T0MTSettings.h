/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//   T0MTSettings.h
//   Header file for class T0MTSettings
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////

#ifndef T0MTSETTINGS_H
#define T0MTSETTINGS_H

#include "T0MTSettingsT0.h"
#include "T0MTSettingsTMax.h"

namespace MuonCalib {

/**@class T0MTSettings
   Settings for the T0 calibration (histogram booking and fitting parameters)
   Parameters for pattern recognition
   @author Felix.Rauscher@Physik.Uni-Muenchen.De
   @date Mar 2006 
   
*/
  class T0MTSettings{
  public:
 //===========================constructor=======================================
  /** default constructor*/
    T0MTSettings();
//============================public emmber functions===========================
  /** @name Histogram binning 
   */
  //@{	
  /** Number of bins for ADC histogram and range*/
    inline const int & NBinsADC() const {return m_n_bins_adc;}
    inline int & NBinsADC() {return m_n_bins_adc;}
    inline const double & ADCMin() const {return m_adc_min;}
    inline double & ADCMin() {return m_adc_min;}		
    inline const double & ADCMax() const {return m_adc_max;}
    inline double & ADCMax() {return m_adc_max;}		
    /** Number of bins for time histogram and range*/
    inline const int & NBinsTime() const {return m_n_bins_time;}
    inline int & NBinsTime() {return m_n_bins_time;}
    inline const double & TimeMin() const {return m_time_min;}
    inline double & TimeMin() {return m_time_min;}		
    inline const double & TimeMax() const {return m_time_max;}
    inline double & TimeMax() {return m_time_max;}
    //@}
    /** @name Debug settings
     */
    //@{
    
    /** If set to true for every tube a TDirectory will be created. It
	will contain a Graph created from the VariableBinwidthHistograms
	used in the Pattern recognition algorithms
    */
    inline const bool & DrawDebugGraphs() const {return m_draw_debug_graphs;}
    inline bool & DrawDebugGraphs() {return m_draw_debug_graphs;}
    /** If set to true the fitted functions are added to the histograms */
    inline const bool &AddFitfun() const {return m_add_fitfun;}
    inline bool &AddFitfun() {return m_add_fitfun;}
    inline bool &AddFitfun(bool add_fitfun) {m_add_fitfun = add_fitfun;return m_add_fitfun;}
    /** verbose level
	0: no output
	1: Fitter output
    */
    inline const int &VerboseLevel() const {return m_verbose_level;}
    inline  int &VerboseLevel()  {return m_verbose_level;}
    
    //@}
    /** get settings for the t0-fit */
    inline const T0MTSettingsT0 * T0Settings() const {return &m_t0_settings;}
    inline T0MTSettingsT0 * T0Settings() {return &m_t0_settings;}
    /** get settings for the tmax-fit*/
    inline const T0MTSettingsTMax * TMaxSettings() const {return &m_tmax_settings;}
    inline T0MTSettingsTMax * TMaxSettings() {return &m_tmax_settings;}
    /** if true fit drift time spectrum*/
    inline const bool & FitTime() const {return m_fit_time;}
    inline bool & FitTime() {return m_fit_time;}
    /** if true fit adc spectrum*/
    inline const bool & FitADC() const {return m_fit_time;}
    inline bool & FitADC() {return m_fit_time;}
    /* minimum number of entries per fit */
    inline const int & MinEntriesTime() const {return m_min_entries_time;}	
    inline  int & MinEntriesTime()  {return m_min_entries_time;}	
    inline const int & MinEntriesADC() const {return m_min_entries_adc;}	
    inline  int & MinEntriesADC()  {return m_min_entries_adc;}	
  
  private:
    //===========================private data members=============================
    //----------------------------histogram binning--------------------------------
    //! ADC bhistogram binning
    int m_n_bins_adc;
    double m_adc_min, m_adc_max;
    //! drift time binning
    int m_n_bins_time;
    double m_time_min, m_time_max;
    //---------------------------debug settings-------------------------------------
    //! if set to true for each debug graphs for the pattern recognition are drawn
    bool m_draw_debug_graphs;
    //! if set to true fitted functions are added to hostograms
    bool m_add_fitfun;
    //! verbose level
    int m_verbose_level;
    //---------------------------t0 - tmax - settings-------------------------------
    bool m_fit_time;
    int m_min_entries_time, m_min_entries_adc;
    T0MTSettingsT0 m_t0_settings;
    T0MTSettingsTMax m_tmax_settings;
  };
  
}

#endif

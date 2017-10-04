/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//   MTTMaxSettingsTMax.h
//   Header file for class MTSettings
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////

#ifndef  MTTMAXSETTINGSTMAX_H
#define MTTMAXSETTINGSTMAX_H

/**@class T0MTSettingsTMax
    Settings for the TMax fit to the drift time spectrum   
    @author Felix.Rauscher@Physik.Uni-Muenchen.De
    @date Mar 2006 
*/

namespace MuonCalib {

class T0MTSettingsTMax
	{
	public:
//===========================constructor========================================
	/** default constructor*/
		T0MTSettingsTMax();
//===========================public member functions============================
 	/** @name Parameters for tmax-pattern recognition
	Note: These parameters default to well proven parameters. The algorithm should work an all spectra with these. The only parameter which is likely to be adapted is TMaxMinBackgroundBins()
	*/
	//@{
	/** Number of hits per histogram bin for the VariableBinwidthHistogram 
	    The number is given relative to the maximum bin content of the time-Spectrum
	    This number must be >1
	    */
		inline const double & VBHBinContent() const {return m_vbh_bin_content_rel;}
		inline  double &VBHBinContent() {return m_vbh_bin_content_rel;}
	/** Maximum bin width for the VariableBinwidthHistogram 
	    The bins will not be wider than this even if it means that they will be underfull */
	    	inline const double & MaxBinwidth() const {return m_max_bin_width;}
		
	    	inline double & MaxBinwidth() {return m_max_bin_width;}
	/** For the  pattern-recognition only a part of the drift time spectrum is used for creating the VariableBinwidthHistogram. This parameter gives the lower limit for this region minus t0 */
		inline const double & VBHLow() const {return m_vbh_low;}
		inline double & VBHLow() {return m_vbh_low;}
	/** The range of the falling edge of the spectrum is searched in a region after t0. This range is given here. The range defaults to a verry large area which should be ok for all spectra
	*/
		inline const double & EndMin() const {return m_end_min;}
		inline double & EndMin() {return m_end_min;}
		inline const double & EndMax() const {return m_end_max;}
		inline double & EndMax() {return m_end_max;}
	/** distance between the detected falling edge and the begin of the background region 
	*/
		inline const double & DistBackground() const {return m_dist_background;} 
		inline double & DistBackground() {return m_dist_background;}
	/** The minimum width of the region for the background estimation. 
	This width is given in number of bins of the time spectrum.
	If the pattern recognition selects a smaller region, the upper boarder of the region will be moved to the last bin of the time spectrum.
	
	NOTE: This should be the approximate width of the trigger matching window after . If this parmeter is to big, the background may be underestimated (with verry little effect on t_max).
	*/
		inline const int & MinBackgroundBins() const {return m_min_background_bins;}
		inline int & MinBackgroundBins() {return m_min_background_bins;}
	/** Distance of the a/b region from the detected falling edge
	*/
		inline const double & DistAB() const {return m_dist_ab;}
		inline double & DistAB() {return m_dist_ab;}
	/** Width of the region in which the parameters a and b are estimated
	*/
		inline const double & WidthAB() const {return m_width_ab;}
		inline double & WidthAB() {return m_width_ab;}
	//@}
private:
 //===========================private data members=============================
	//! relative bin content for tmax-VBH
		double m_vbh_bin_content_rel;
	//! maximum bin width for tmax-VBH
		double m_max_bin_width;
	//! lower edge of tmax-VBH relative to t0
		double m_vbh_low;
	//! range in which the falling edge is searched
		double m_end_min, m_end_max;
	//! distance between detected falling edge and begin of background region
		double m_dist_background;
	//! minimum number of bins for background region
		int m_min_background_bins;
	//! distance of a/b region from detected falling edge
		double m_dist_ab;
	//! width of a/b region
		double m_width_ab;
	};
	
}

#endif

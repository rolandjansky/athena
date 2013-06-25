/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//   ADCMTHistos.h
//   Header file for class ADCMTHistos
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////

#ifndef ADCMTHistos_H
#define ADCMTHistos_H

//c - c++
#include <iostream>

//root
#include "TH1.h"
/*#include "TF1.h"
#include "TDirectory.h"*/
//class TH1F;
class TF1;
class TDirectory;

//this
#include "T0MTSettings.h"


namespace MuonCalib {

/**@class ADCMTHistos
	Histogram and fitter class for drift time and pulsehight spectra
	The rising slope is fitted by a fermi-function: @f$f(t)=r_{u,0} + \frac{A}{1+exp((t_0 - t)/T_0}@f$.
	The falling slope is fitted by @f$g(t)=r_{u,max} + \frac{e(t,a,b)}{1+exp((t_{max} - t)/T_{max}}@f$ where @f$e(t, a, b) = a e^{bt}@f$.
	A pattern recognition determines the fit ranges and the parameters @f$r_u@f$, @f$A@f$, @f$a@f$ and @f$b@f$.
   @author Felix.Rauscher@Physik.Uni-Muenchen.De
   @date June 2006

}

*/
class ADCMTHistos
	{
	public:
//---------------------------constructor----------------------------------------
	/** Default Constructor */
		inline ADCMTHistos() : 
		                      m_adc(NULL),
				      m_id(-1), 
				      m_adc_fit(NULL),
				      m_adc_ok(false)
		       {}
	/** Initializing constructor
	    @param id tube id
	    @param settings t0-fit settings: settings will be asked about histogram binning
	    
	*/
		inline ADCMTHistos(int id, const T0MTSettings * settings, const char *hname=NULL): 
		                      m_adc(NULL),
				      m_id(-1), 
				      m_adc_fit(NULL),
				      m_adc_ok(false)
			{
			Initialize(id, settings, hname);
			}
//---------------------------public member functions----------------------------
	/** Initialize class
	    @param id tube id
	    @param settings t0-fit settings: settings will be asked about histogram binning
	    
	*/
		void Initialize(int id, const T0MTSettings * settings, const char * hname=NULL);
			
	/** get adc spectrum */
		inline TH1F * GetADCSpec() {return m_adc;}
				
	/** fill adc value
	    @param a adc value
	    
	*/
		void FillA(double a);
		
		
	/** return tube id
	
	*/
		inline int Id() const {return m_id;}
		
	/** fit adc*/
		bool FitAdc();	

	/** returnd function fitted to adc-spectrum */
		inline TF1 * GetAdcFunction() const
			{
			return m_adc->GetFunction("adc_fun");
			}
		inline bool AdcOk() const
			{
			return m_adc_ok;
			}
					
	private:	
//---------------------------private data members-------------------------------
	//! pulse height spectrum
		TH1F *m_adc;	
	//! tube id;
		int m_id;
	//! function which is fitted to the adc-spectrum
		TF1 *m_adc_fit;
	//! is true if the adc-fit is ok;
		bool m_adc_ok;
	//! Pointer to settings class
		const T0MTSettings *m_settings;
	};

}

#endif

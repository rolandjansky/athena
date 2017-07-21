/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//   T0MTHistos.h
//   Header file for class T0MTHistos
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////

#ifndef T0MTHISTOS_H
#define T0MTHISTOS_H

//c - c++
#include <iostream>

//root
#include "TH1.h"
//#include "TF1.h"
//#include "TDirectory.h"
//class TH1F;
class TF1;
class TDirectory;

//this
#include "T0MTSettings.h"

namespace MuonCalib {

/**@class T0MTHistos
	Histogram and fitter class for drift time and pulsehight spectra
	The rising slope is fitted by a fermi-function: @f$f(t)=r_{u,0} + \frac{A}{1+exp((t_0 - t)/T_0}@f$.
	The falling slope is fitted by @f$g(t)=r_{u,max} + \frac{e(t,a,b)}{1+exp((t_{max} - t)/T_{max}}@f$ where @f$e(t, a, b) = a e^{bt}@f$.
	A pattern recognition determines the fit ranges and the parameters @f$r_u@f$, @f$A@f$, @f$a@f$ and @f$b@f$.
   @author Felix.Rauscher@Physik.Uni-Muenchen.De
   @date June 2006

}

*/
  class T0MTHistos {
  public:
//---------------------------constructor----------------------------------------
    /** Default Constructor */
    inline T0MTHistos() : 
      m_time(NULL),
      m_id(-1), 
      m_t0_fermi(NULL),
      m_t0_ok(false),
      m_status_code(1),
      m_tmax_fermi(NULL),
      m_tmax_ok(false),
      m_dir(NULL),
      m_settings(NULL) {
      m_chi2=9e9;
    }
    /** Initializing constructor
	@param id tube id
	@param settings t0-fit settings: settings will be asked about histogram binning
	
    */
    inline T0MTHistos(int id, const T0MTSettings * settings, const char *hname=NULL):
      m_t0_fermi(NULL),
      m_tmax_fermi(NULL) {
      Initialize(id, settings, hname);
      m_status_code=99;
      m_chi2=9e9;
    }
//---------------------------static constants-----------------------------------
    //! number of parameters in t0 fit
    static const int N_T0_FIT_PAR=4;
    //! parameter numbers in t0 fit
    static const int T0_PAR_NR_T0=0,
      T0_PAR_NR_T=1,
      T0_PAR_NR_BACK=2,
      T0_PAR_NR_A=3;
    //! number of parameters for tmax fit
    static const int N_TMAX_FIT_PAR=6;
    //! parameters numbers for tmax fit
    static const int TMAX_PAR_NR_TMAX=0,
      TMAX_PAR_NR_T=1,
      TMAX_PAR_NR_BACK=2,
      TMAX_PAR_NR_A=3,
      TMAX_PAR_NR_B=4,
      TMAX_PAR_NR_T0=5;
//---------------------------public member functions----------------------------
    /** Initialize class
	@param id tube id
	@param settings t0-fit settings: settings will be asked about histogram binning
	
    */
    void Initialize(int id, const T0MTSettings * settings, const char * hname=NULL);
    
    /** get drift time spectrum */
    inline TH1F * GetTSpec() {return m_time;}
    	
    /** set the pointer of the drift-time spectrum to an existing spectrum. 
	This is for testapps
	@param id tube id
	@param spec Pointer to an existing spectrum	
    */
    void SetTSpec(int id, TH1F *spec, const T0MTSettings * settings, bool copy_spec=true);
    
    /** fill drift time spectrum
	@param t drift time
    */
    void FillT(double t);

    /** Perform t0-fit 
	Returns true if fit is successfull
    */
    bool FitT0();
    
    /** Performs tmax-fit
	Returns true if fit is successfull
    */
    bool FitTmax();
		
    /** return tube id  */
    inline int Id() const {return m_id;}
		
    /** returns true if t0-fit was successfull */
    inline bool T0Ok() const {return m_t0_ok;}
    /** returns status code - the status code applies only to the t0 fit*/
    inline int StatusCode() const {return m_status_code;}
    /** returns function fitted to the riding edge of the spectrum */
    inline const TF1 * GetT0Function() const {return m_t0_fermi;}
    /** returns function fitted to the riding edge of the spectrum */
    inline  TF1 * GetT0Function() {return m_t0_fermi;}
    /** returns true if tmax-fir was successfull */
    inline bool TmaxOk() const {return m_tmax_ok;}
    /** returns function fitted to the riding edge of the spectrum */
    inline const TF1 * GetTMaxFunction() const {return m_tmax_fermi;}
    /** returns function fitted to the riding edge of the spectrum */
    inline  TF1 * GetTMaxFunctionNC() {return m_tmax_fermi;}
    /** returns t0 chi2*/
    inline const double & T0Chi2() const { return m_chi2; }
					
  private:	
//---------------------------private data members-------------------------------
    //! time spectrum
    TH1F *m_time;
    //! tube id;
    int m_id;
    //! function fitted to the riding edghe of the spectrum
    TF1 *m_t0_fermi;
    //! is true if t0 fit was successful
    bool m_t0_ok;
    //! status code for t0 fit (0 ok, 1 not fitted, 2 low statistics, 3 failed)
    int m_status_code;
    //! function fitted to the falling edge of the spectrum
    TF1 *m_tmax_fermi;
    //! is true if tmax fit was successful
    double m_tmax_ok;
    //! TDirectory where debug and result histograms are stored
    TDirectory *m_dir;
    //! Pointer to settings class
    const T0MTSettings *m_settings;
    //! chi2/NDF value 
    double m_chi2;
    /** normal t0 fit */
    bool NormalFit();
    /** try to get better start values from a scrambled histogram*/	
    bool T0Scramble();
    /** top chi2 calculation */
    void TopChi2();
    /** top slicing metyhod */
    void TopSlicing();
    class Slice {
    public:
      double chi_2;
      int min_bin;
      int max_bin;
      int n_bins;
    };

  };  //end  class T0MTHistos 

}     //end namespace MuonCalib

#endif

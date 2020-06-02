/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef T0CALIBRATIONCLASSIC_H
#define T0CALIBRATIONCLASSIC_H

#include "MdtCalibInterfaces/IMdtCalibration.h"
#include "MdtCalibData/MdtTubeFullInfoContainer.h"
#include "MdtCalibData/MdtTubeFitContainer.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <iostream>
#include <vector>

class TH1;
class TH2;
class TFile;
class TDirectory;

namespace MuonCalib {

  class MuonCalibSegment;
  class IMdtCalibrationOutput;

/**@class T0ClassicSettings
   Settings for the T0 calibration (histogram booking and fitting parameters) 
   @author domizia.orestano@cern.ch
   @date June 2005 
   
*/
  class T0ClassicSettings {
  public:
  /** constructor */
    T0ClassicSettings( double minAdc, double maxAdc, int adcBins,  
		       double minTime, double maxTime, int timeBins,
		       bool fitTime , int minEntries, int initParam,
		       int numParams, double fitParams[], double chiMax, int printLevel) : 
      m_minAdc(minAdc),m_maxAdc(maxAdc),m_binAdc(adcBins), 
      m_minTime(minTime),m_maxTime(maxTime),m_binTime(timeBins), 
      m_fitTime(fitTime),m_entries(minEntries),
      m_initParamFlag(initParam),m_numParams(numParams),
      m_params(fitParams), m_chi2max(chiMax), m_printLevel(printLevel) { }
      /** Access functions to values of private settings */
  /** @return m_minAdc, minimum for Adc histogram */
    double minAdc() const {return m_minAdc;};
  /** @return m_maxAdc, maximum for Adc histogram */
    double maxAdc() const {return m_maxAdc;};
  /** @return m_binAdc, number of bins for Adc histogram */
    int binAdc() const {return m_binAdc;};
  /** @return m_minTime, minimum for Time histogram */
    double minTime() const {return m_minTime;};
  /** @return m_maxTime, maximum for Time histogram */
    double maxTime() const {return m_maxTime;};
  /** @return m_binTime, number of bins for Time histogram */
    int binTime() const {return m_binTime;};
  /** @return m_fitTime, flag activating the fitting of Time histogram */
    bool fitTime() const {return m_fitTime;};
  /** @return m_entries, the minimum number of entries to perform an histogram  fit */
    int entries() const {return m_entries;};
  /** @return m_initParamFlag: false if intial parameters are fixed, true if  
      they have to be searched */
    int initParamFlag() const {return m_initParamFlag;};
  /** @return number of parameters to be fit */
    int numParams() const {return m_numParams;};
  /** @return the pointer to the vector of parameters */
    double * params() const {return m_params;};
  /** @return the maximum values of chi2 to accept the fit result */
    double chi2max() const {return m_chi2max;};
  /** @return the print level */
    int printLevel() const {return m_printLevel;};
  /** a method to dump all the settings */
    void print() const {
      MsgStream log(Athena::getMessageSvc(),"T0ClassicSettings");
      log<<MSG::INFO<<"T0ClassicSettings: "<<endmsg;
      log<<MSG::INFO<<"booking of Adc histos: min max bins " <<minAdc()<<" "<<maxAdc()<<" "<<binAdc()<<endmsg;
      log<<MSG::INFO<<"booking of Time histos: min max bins " <<minTime()<<" "<<maxTime()<<" "<<binTime()<<endmsg;
      log<<MSG::INFO<<"fitting of Time histos: active minentries paramflag chi2cut nparams"<<fitTime()<<" "<<entries()<<" "<<initParamFlag()<<" "<<chi2max()<<" "<<numParams()<<endmsg;
      log<<MSG::INFO<<"Parameters: "<<endmsg;
      for(int i=0;i<numParams();i++) {
        log<<MSG::INFO<<m_params[i]<<endmsg;
      }
    };
  
  private:
  /** Private settings */
    double m_minAdc;     //!< Adc spectrum low edge 
    double m_maxAdc;     //!< Adc spectrum high edge 
    int m_binAdc;        //!< Adc spectrum number of bins 
    double m_minTime;    //!< Time spectrum low edge 
    double m_maxTime;    //!< Time spectrum high edge
    int m_binTime;       //!< Time spectrum number of bins 
    bool m_fitTime;      //!< flag to select or deselect the fit to Time Spectra
    int m_entries;       //!< minimum number of entries per spectrum fitting
    int m_initParamFlag; //!< fix or search initial parameters for spectrum fit 
    int m_numParams;     //!< number of paramaters for spectrum fit
    double * m_params;   //!< initial parameters for spectrum fit 
    double m_chi2max;    //!< normalized chi2 cut 
    int m_printLevel;    //!< output level flag (1 for verbose)
  };  //end class T0ClassicSettings

/**@class T0ClassicHistos
   Tube histograms used in T0 calibration. 
   @author domizia.orestano@cern.ch
   @date June 2005 
*/
  class T0ClassicHistos {
  public:
    TH1* time; //!< time spectrum
    TH1* adc;  //!< adc spectrum
  //TH2* adc_vs_time;
    int id;  //!< tube identifier
  };

/**@class T0CalibrationClassic
   Implementation of a T0 calibration using the classical approach.
   @author domizia.orestano@cern.ch
   @date June 2005 
*/
  class T0CalibrationClassic : public IMdtCalibration {
  public:
  /** constructor 
   * @param[in] name of the region/chamber to be calibrated  
   * @param[in] pointer to settings vector 
   */
    T0CalibrationClassic( std::string name, const T0ClassicSettings* settings );
  /** destructor */
    ~T0CalibrationClassic();

    bool  handleSegment( MuonCalibSegment& seg );         //!< fill tube spectra
    void  setInput( const IMdtCalibrationOutput* input ); //!< unused
    bool  analyse();                                      //!< extract parameters from spectra
    bool  converged() const;				//!< return m_converged (always false?)

  /** @return the calibration results */
    const IMdtCalibrationOutput* getResults() const;
    const IMdtCalibrationOutput* analyseSegments( const std::vector<MuonCalibSegment*> & segs );//!< new interface function

  private:
    T0ClassicHistos *bookHistos(unsigned int idtube);  //!< booking of histograms
    T0ClassicHistos *getHistos(unsigned int idtube);   //!< retrieve pointer for tube idtube histograms
    void doTimeFit(T0ClassicHistos*,MdtTubeFitContainer::SingleTubeFit&,MdtTubeFitContainer::SingleTubeCalib&); //!< fit time spectrum 
    void doAdcFit(T0ClassicHistos*, MdtTubeFitContainer::SingleTubeFit&,MdtTubeFitContainer::SingleTubeCalib&); //!< fit adc spectrum 
    void searchParams(TH1 * h, double * p, int np); //!< estimate initial parameters for time spectrum fit from the spectrum itself
    const T0ClassicSettings* m_settings;   //!< pointer to the settings 
    bool                     m_converged;  //!< convergence status
    std::string              m_name;       //!< calibration region name  
    int m_currentItnum;                    //!< current iteration (always 1?)
    TFile *p_file;			   //!< pointer to the histogram file 
    TDirectory *m_regiondir;               //!< pointer to the ROOT directory

    std::vector<T0ClassicHistos*>m_histos; //!< vector of pointers to tube histograms 
    MdtTubeFitContainer* m_result;         //!<  tube constants 
    bool m_delete_settings;

    // hidden assignment operator and copy constructor
    T0CalibrationClassic & operator=(const T0CalibrationClassic &right);
    T0CalibrationClassic(const T0CalibrationClassic&);
  };  //end class T0CalibrationClassic
}

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//   T0CalibrationMT.h
//   Header file for class T0CalibrationMT
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////

#ifndef T0CALIBRATIONMT_H
#define T0CALIBRATIONMT_H
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "MdtCalibInterfaces/IMdtCalibration.h"
#include "MdtCalibData/MdtTubeFitContainer.h"
#include "MdtCalibData/MdtTubeFitContainer.h"
#include "MuonCalibStandAloneBase/NtupleStationId.h"
//#include "T0MTSettings.h"
//#include "T0MTHistos.h"
//#include "ADCMTHistos.h"
//#include "HistogramId.h"

class TH1;
class TFile;
class TDirectory;

namespace MuonCalib {

class MuonCalibSegment;
class IMdtCalibrationOutput;
class MuonFixedId;
class T0MTSettings;
class T0MTHistos;
class T0ADCHistos;
class ADCMTHistos;
class HistogramId;
class MdtRelativeTubeT0;
//class MdtTubeFitContainer;



/**@class T0CalibrationMT
   Implementation of a T0 calibration using the MT approach.
   Copied from T0CalibrationClassic  
   @author Felix.Rauscher@Physik.Uni-Muenchen.De
   @date June 2005 
*/
class T0CalibrationMT : public IMdtCalibration {

 public:

  /** constructor
   * @param[in] name of the region/chamber to be calibrated
   * @param[in] pointer to settings vector    
   * @param[in] sorting criteria (TUBE, CHAMBER, MULTILAYER...) default is by TUBE
   */
 
  T0CalibrationMT( std::string name, const T0MTSettings * settings, const std::vector<int> &sort_by, const std::vector<int> &adc_sort_by);

  /** destructor */
  ~T0CalibrationMT();

  bool  handleSegment( MuonCalibSegment& seg );               //!< fill tube spectra
  
  void  setInput( const IMdtCalibrationOutput* input ); //!< unused
  
  bool  analyse();                                      //!< extract parameters from spectra
  
  bool  converged() const;				//!< return m_converged 

  /** @return the calibration results 
  */
  const IMdtCalibrationOutput* getResults() const;

const IMdtCalibrationOutput* analyseSegments( const std::vector<MuonCalibSegment*> & segs );//!< new interface function

 private:


  T0MTHistos * getHistos(const MuonFixedId & idtube, unsigned int nr); //!< retrieve pointer for tube idtube histograms
  ADCMTHistos * getADCHistos(const MuonFixedId & idtube, unsigned int nr); //!< retrieve pointer for tube idtube histograms
bool  analyse_tdc(const int & nr, std::map<int, MdtTubeFitContainer::SingleTubeFit> & full, std::map<int, MdtTubeFitContainer::SingleTubeCalib> & st, std::map<int, std::string> & fit_by_map);
bool  analyse_adc(const int & nr, std::map<int, MdtTubeFitContainer::SingleTubeFit> & full, std::map<int, MdtTubeFitContainer::SingleTubeCalib> & st);

  void doTimeFit(T0MTHistos * T0h, const std::set<MuonFixedId> & tube_ids, std::map<int, MdtTubeFitContainer::SingleTubeFit> & fim, std::map<int, MdtTubeFitContainer::SingleTubeCalib> & stcm, std::map<int, std::string> & fit_by_map, const std::string & fit_by); //!< fit time spectrum 

  void doAdcFit(ADCMTHistos * T0h, const std::set<MuonFixedId> & tube_ids, std::map<int, MdtTubeFitContainer::SingleTubeFit> & fim, std::map<int, MdtTubeFitContainer::SingleTubeCalib> & stcm); //!< fit adc spectrum 


  const T0MTSettings* m_settings;   //!< pointer to the settings 
  bool                     m_converged;  //!< convergence status
  std::string              m_name;       //!< calibration region name  
  int m_currentItnum;                    //!< current iteration (always 1?)
  TFile * p_file;			 //!< pointer to the histogram file 
  TDirectory * m_regiondir;              //!< pointer to the ROOT directory

  std::vector<std::map<HistogramId, T0MTHistos*> >m_histos;  //!< vector of pointers tube histograms 
  std::vector<std::map<HistogramId, ADCMTHistos*> >m_adc_histos;  //!< vector of pointers tube histograms 
  std::vector<std::map<HistogramId, std::set<MuonFixedId> > > m_tube_ids;
  std::vector<std::map<HistogramId, std::set<MuonFixedId> > > m_adc_tube_ids;
  std::map<int, int> m_nhits_per_tube; //!<number of hits per tube
  std::map<NtupleStationId, MdtTubeFitContainer *> m_result;   //!<  tube constants 
  std::map<NtupleStationId, MdtRelativeTubeT0> m_rel_tube_t0s;
  const std::vector<int> &m_sort_by;
  const std::vector<int> &m_adc_sort_by;
  bool m_delete_settings;

  // hidden assignment operator and copy constructor
  T0CalibrationMT & operator=(const T0CalibrationMT &right);
  T0CalibrationMT(const T0CalibrationMT&);
};
}

#endif  
 

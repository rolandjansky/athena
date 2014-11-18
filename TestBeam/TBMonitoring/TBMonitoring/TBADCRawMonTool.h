//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBMONITORING_TBADCRAWMONTOOL_H
#define TBMONITORING_TBADCRAWMONTOOL_H

#include "AthenaMonitoring/MonitorToolBase.h"

#include "GaudiKernel/IHistogramSvc.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h" 

#include "StoreGate/StoreGateSvc.h" 

#include <string>
#include <vector>

/** @class TBADCRawMonTool TBADCRawMonTool.h 
    "TBMonitoring/TBADCRawMonTool.h"
    
    This tool is used to create histograms of tdc data
    @author Malachi Schram
*/


class TBADCRawMonTool: public MonitorToolBase
{

 public:
  
  TBADCRawMonTool(const std::string & type, const std::string & name,
		  const IInterface* parent);
    

  ~TBADCRawMonTool();

  StatusCode initialize();
    
  StatusCode bookHists(); // this is called before 1st event
  StatusCode mybookHists(); // called at 1st event
  StatusCode fillHists();

  // Test method
  void FillRandomDetect();

 private:

  std::string m_path;

  //Declared properties
  bool m_monitor_adc;
  bool m_testTool;
  bool m_isBooked;

  // ADC histograms
  IHistogram1D** m_histo_adc;

  // Histos ranges
  float m_posMax,m_posMin;
  int m_numBin;
  int m_adcNum;
  std::vector<std::string>  m_adcNames;
};


#endif // not TBMONITORING_TBADCRAWMONTOOL_H

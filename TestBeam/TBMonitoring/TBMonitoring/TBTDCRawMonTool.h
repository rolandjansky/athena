//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBTDCRAWMONTOOL_H
#define TBTDCRAWMONTOOL_H

#include "AthenaMonitoring/MonitorToolBase.h"

#include "GaudiKernel/IHistogramSvc.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h" 

#include <string>
#include <vector>

/** @class TBTDCRawMonTool TBTDCRawMonTool.h 
    "TBMonitoring/TBTDCRawMonTool.h"
    
    This tool is used to create histograms of tdc data
    @author Malachi Schram
*/


class TBTDCRawMonTool: public MonitorToolBase
{

 public:
  
  TBTDCRawMonTool(const std::string & type, const std::string & name,
		  const IInterface* parent); 

  ~TBTDCRawMonTool();

  StatusCode initialize();
    
  StatusCode bookHists(); // this is called before 1st event
  StatusCode mybookHists(); // called at 1st event
  StatusCode fillHists();

  // Test method
  void FillRandomDetect();

 private:
  //std::string m_path;

  //Declared properties
  bool m_monitor_tdc;
  bool m_testTool;

  // TDC histograms
  IHistogram1D** m_histo_tdc;

  // Histos ranges
  float m_posMax,m_posMin;
  int m_numBin;
  int m_tdcNum;
  std::vector<std::string>  m_tdcNames;
};
#endif

//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBTAILCATCHERRAWMONTOOL_H
#define TBTAILCATCHERRAWMONTOOL_H

#include "AthenaMonitoring/MonitorToolBase.h"

#include "GaudiKernel/IHistogramSvc.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h" 

#include <string>
#include <vector>

/** @class TBTailCatcherRawMonTool TBTailCatcherRawMonTool.h 
    "TBMonitoring/TBTailCatcherRawMonTool.h"
    
    This tool is used to create histograms of tailcatcher scintillators
*/


class TBTailCatcherRawMonTool: public MonitorToolBase
{

 public:
  
  TBTailCatcherRawMonTool(const std::string & type, const std::string & name,
		  const IInterface* parent);
    

  ~TBTailCatcherRawMonTool();

  StatusCode initialize();
    
  StatusCode bookHists(); // this is called before 1st event
  StatusCode mybookHists(); // called at 1st event
  StatusCode fillHists();

  // Test method
  void FillRandomDetect();

 private:
  //std::string m_path;

  //Declared properties
  bool m_monitor_tailcatcher_raw;
  bool m_monitor_tailcatcher_reco;
  bool m_testTool;

  // Tailcatcher histogram
  IHistogram1D** m_histo_tcScintADC;
  IHistogram1D** m_histo_tcreco;
  IHistogram1D* m_histo_tcTotal;

  // Histos ranges
  float m_posMax,m_posMin;
  float m_posMaxreco,m_posMinreco;
  int m_numBin;
  int m_tcScintNum;
  std::vector<std::string>  m_scintNames;
};


#endif

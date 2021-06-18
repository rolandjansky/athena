//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBMONITORING_TBMWPCMONTOOL_H
#define TBMONITORING_TBMWPCMONTOOL_H

#include "AthenaMonitoring/MonitorToolBase.h"

#include "GaudiKernel/IHistogramSvc.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h" 

#include "StoreGate/StoreGateSvc.h" 

#include <string>
#include <vector>

/** @class TBMWPCMonTool TBMWPCMonTool.h 
    "TBMonitoring/TBMWPCMonTool.h"
    
    This tool is used to create histograms of tdc data
    @author Pierre-Antoine Delsart
*/


class TBMWPCMonTool: public MonitorToolBase
{

 public:
  
  TBMWPCMonTool(const std::string & type, const std::string & name,
		  const IInterface* parent);
    

  ~TBMWPCMonTool();

  StatusCode initialize();
    
  StatusCode bookHists(); // this is called before 1st event
  StatusCode mybookHists(); // called at 1st event
  StatusCode fillHists();

  // Test method
  void FillRandomDetect();

 private:
  //std::string m_path;

  //Declared properties
  bool m_monitor_mwpcraw;
  bool m_monitor_mwpc;
  bool m_testTool;

  // ADC histograms
  IHistogram1D* m_histo_mwpcrawHitDist[8];
  IHistogram1D* m_histo_mwpcrawClustDist[8];
  IHistogram1D* m_histo_mwpcrawHits;
  IHistogram1D* m_histo_mwpcrawC[8];
  IHistogram1D* m_histo_mwpcrawN[8];


  IHistogram1D* m_histo_mwpcCenter[8];
  IHistogram1D* m_histo_mwpcSize[8];

  // Histos ranges
  std::vector<float> m_range_rawcenter,m_range_rawwidth,m_range_rawhits,m_range_rawclust;
  std::vector<float> m_range_center,m_range_size;

  std::string m_SGkeymwpc,m_SGkeymwpcraw;
  std::string m_mwpcnames[8];

};


#endif // not TBMONITORING_TBMWPCMONTOOL_H

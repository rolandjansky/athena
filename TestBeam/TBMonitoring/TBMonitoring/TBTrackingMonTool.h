//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBTRACKINGMONTOOL_H
#define TBTRACKINGMONTOOL_H

#include "AthenaMonitoring/MonitorToolBase.h"

#include "GaudiKernel/IHistogramSvc.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h" 


/** @class TBTrackingMonTool TBTrackingMonTool.h 
    "TBMonitoring/TBTrackingMonTool.h"
This tool is used to create histograms of tracking information.
*/


class TBTrackingMonTool: public MonitorToolBase
{

 public:
  
  TBTrackingMonTool(const std::string & type, const std::string & name,
		  const IInterface* parent);
    

  ~TBTrackingMonTool();

  StatusCode initialize();
    
  StatusCode bookHists(); // this is called before 1st event
  StatusCode mybookHists(); // called at 1st event
  StatusCode fillHists();

  // Test method
  void FillRandomDetect();

 private:
  std::string m_path;

  //Declared properties
  bool m_monitor_track;
  bool m_testTool;
  bool m_isBooked;

  // Track histograms 
  IHistogram1D* m_histo_track_chi2;
  IHistogram1D* m_histo_track_angle;
  IHistogram1D* m_histo_track_Uslope;
  IHistogram1D* m_histo_track_Vslope;
  IHistogram1D* m_histo_track_Ucryo;
  IHistogram1D* m_histo_track_Vcryo;
  IHistogram1D* m_histo_track_Wcryo;    
  //IHistogram1D** m_histo_track_Residualu;
  //IHistogram1D** m_histo_track_Residualv; 

  // Histos ranges
  float m_posMax,m_posMin;
  int m_numBin;
  
};


#endif

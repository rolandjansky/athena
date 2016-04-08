//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     BeamDetectorMonTool.h
// PACKAGE:  TBMonitoring
//
// AUTHOR:   Malachi Schram
//
//
// ********************************************************************
#ifndef TBBPCMONTOOL_H
#define TBBPCMONTOOL_H

#include "AthenaMonitoring/MonitorToolBase.h"

#include "GaudiKernel/IHistogramSvc.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h" 

#include "StoreGate/StoreGateSvc.h" 

class TBBPCMonTool: public MonitorToolBase
{

 public:
  
  TBBPCMonTool(const std::string & type, const std::string & name,
	       const IInterface* parent);
    

  ~TBBPCMonTool();

  StatusCode initialize();
    
  //pure virtual methods
  StatusCode bookHists(); // this is called before 1st event
  StatusCode mybookHists(); // called at 1st event
  StatusCode fillHists();
  //  bool histsNotBooked(); 

  // Test method
  void FillRandomDetect();
  
  //StatusCode execute();

 private:
  std::string m_path,m_SGkeybpc,m_SGkeybpcraw;
  std::vector<std::string> m_bpc_names;
  std::vector<short> m_bpc_map;

  std::string m_histoPath;

  bool m_monitor_bpc;
  bool m_monitor_bpcraw;

  bool m_fake_detector;

  bool m_isBooked;

  // BPC Histograms
  IHistogram1D** m_histo_bpcposX;
  IHistogram1D** m_histo_bpcposY;
  IHistogram2D** m_histo_bpcXvsY;


  IHistogram1D** m_histo_bpctdcLeft;	   
  IHistogram1D** m_histo_bpctdcRight;	   
  IHistogram1D** m_histo_bpctdcUp;	   
  IHistogram1D** m_histo_bpctdcDown;	   
  IHistogram1D** m_histo_bpcadcHorizontal; 
  IHistogram1D** m_histo_bpcadcVertical;   



  //IHistogramSvc* m_testhist;

  int m_bpcnum;
  bool m_bookatfirstevent;
  
  // Histos ranges
  float m_posmax,m_posmin;
  int m_posbin;
  int m_tdcbin,m_tdcmin,m_tdcmax;

};


#endif

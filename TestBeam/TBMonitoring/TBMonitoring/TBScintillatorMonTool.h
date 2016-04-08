//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TBScintillatorMonTool.h
// PACKAGE:  TBMonitoring
//
// AUTHOR:   Pierre-Antoine Delsart
//
//
// ********************************************************************
#ifndef TBSCINTILLATORMONTOOL_H
#define TBSCINTILLATORMONTOOL_H

#include "AthenaMonitoring/MonitorToolBase.h"

#include "GaudiKernel/IHistogramSvc.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h" 

#include "StoreGate/StoreGateSvc.h" 

class TBScintillatorMonTool: public MonitorToolBase
{

 public:
  
  TBScintillatorMonTool(const std::string & type, const std::string & name,
		  const IInterface* parent);
    

  ~TBScintillatorMonTool();

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
  std::string m_path,m_SGkeyscint;
  std::vector<std::string> m_scint_names;
  std::vector<short> m_scint_map;
  std::string m_histoPath;

  bool m_fake_detector;

  bool m_isBooked;



  // Scint histograms
  IHistogram1D** m_histo_scint;
  IHistogram1D** m_histo_scint_time;


  //IHistogramSvc* m_testhist;

  int m_scintnum;
  bool m_bookatfirstevent;

  // Histos ranges
  //float m_posmax,m_posmin;
  //int m_posbin;
  int m_tdcbin;
  float m_tdcmin,m_tdcmax;
  int m_adcbin;
  float m_adcmin,m_adcmax;
};


#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TBTriggerPatternUnitMonTool.h
// PACKAGE:  TBMonitoring
//
// AUTHOR:   Warren Shaw
//
//
// ********************************************************************
#ifndef TBTRIGGERPATTERNUNITMONTOOL_H
#define TBTRIGGERPATTERNUNITMONTOOL_H

#include "AthenaMonitoring/MonitorToolBase.h"

#include "GaudiKernel/IHistogramSvc.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h" 

class TBTriggerPatternUnitMonTool: public MonitorToolBase
{

 public:
  
  TBTriggerPatternUnitMonTool(const std::string & type, const std::string & name,
		  const IInterface* parent);
    

  ~TBTriggerPatternUnitMonTool();

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
  std::string m_path,m_SGkey;
  
  bool m_monitor_triggpat;

  bool m_fake_detector;
  //bool m_testTool;
  bool m_isBooked;

  // Trigger pattern histos
  IHistogram1D* m_histo_triggpat1;
  IHistogram2D* m_histo_triggpat2;

  //IHistogramSvc* m_testhist;

  std::vector<unsigned int> m_triggflag;

  // Histos ranges
  //float m_posmax,m_posmin;
  //int m_posbin;

  
};


#endif

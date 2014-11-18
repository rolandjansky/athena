/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#####################################################
//#                                                   #
//# File   : TBBeamQualityDoubleHitsMonTool.h         #
//# Package: TBMonitoring                             #
//#                                                   #
//# Author : Marco Bieri <mbieri@sfu.ca>              #
//#                                                   #
//# Last Modified: Nov 18/2004                        #
//#####################################################

#ifndef TBBEAMQUALITYDOUBLEHITSMONTOOL_H
#define TBBEAMQUALITYDOUBLEHITSMONTOOL_H

#include "AthenaMonitoring/MonitorToolBase.h"
#include "GaudiKernel/IHistogramSvc.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h" 

#include "StoreGate/StoreGateSvc.h" 

#include <string>
#include <vector>

class TBBeamQualityDoubleHitsMonTool: public MonitorToolBase
{
 public:
  TBBeamQualityDoubleHitsMonTool(const std::string & type, 
				 const std::string & name,
				 const IInterface* parent);
  
  ~TBBeamQualityDoubleHitsMonTool();
  
  StatusCode initialize();
  
  StatusCode bookHists(); // this is called before 1st event
  StatusCode fillHists();
  
 private:
  //keys
  std::string m_SGScintRawKey;
  
  //before or after beam scintillator cut
  std::string m_time;  
  
  // which scintillators to plot, entry 0 is the reference scintillator 
  std::vector<std::string> m_scintselect;
  
  //array of adc values for selected scintillators
  float m_scintselectadc [25];
  
  // histogram options
  std::string htitle;
  std::string hname;
  int m_scintbin;  
  float m_scintadcmin;
  float m_scintadcmax; 
  int m_scintnum;
  
  bool m_isBooked; 
  std::string m_histoPath;
  std::string m_path;
  
  //histograms  
  IHistogram2D** m_histo_double;
};  
#endif



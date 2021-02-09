/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DataQualityFatherMonTool.h
// PACKAGE:  DataQualityTools
//
// AUTHOR:   Luca Fiorini (Luca.Fiorini@cern.ch)
//
// July 2006	
// ********************************************************************
#ifndef DATAQUALITYFATHERMONTOOL_H
#define DATAQUALITYFATHERMONTOOL_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

//
// StoreGate and Gaudi

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ITHistSvc.h"

//
// ROOT classes

#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile_LW.h"
#include "LWHists/TProfile2D_LW.h"

#include "TTree.h"

#include "TH1.h"
#include "TGraph.h"

class DataQualityFatherMonTool: public ManagedMonitorToolBase
{

 public:
  
  DataQualityFatherMonTool(const std::string & type, const std::string & name,
			   const IInterface* parent);
    

  virtual ~DataQualityFatherMonTool();

  virtual StatusCode initialize();

  //
  // pure virtual methods

  virtual StatusCode bookHistogramsRecurrent( );
  virtual StatusCode bookHistograms( );
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms( );
  virtual StatusCode checkHists(bool fromFinalize);

  StatusCode registerHist(const std::string& path, TH1* h1, Interval_t interval=run, MgmtAttr_t histo_mgmt = ATTRIB_MANAGED, const std::string& merge_algo="");
  StatusCode registerHist(const std::string& path, TGraph* h1, Interval_t interval=run, MgmtAttr_t histo_mgmt = ATTRIB_MANAGED);
  StatusCode registerHist(const std::string& path, TH1I_LW* h1, Interval_t interval=run, MgmtAttr_t histo_mgmt = ATTRIB_MANAGED);
  StatusCode registerHist(const std::string& path, TH2I_LW* h1, Interval_t interval=run, MgmtAttr_t histo_mgmt = ATTRIB_MANAGED);
  StatusCode registerHist(const std::string& path, TH1F_LW* h1, Interval_t interval=run, MgmtAttr_t histo_mgmt = ATTRIB_MANAGED, const std::string& merge_algo="");
  StatusCode registerHist(const std::string& path, TH2F_LW* h1, Interval_t interval=run, MgmtAttr_t histo_mgmt = ATTRIB_MANAGED);
  StatusCode registerHist(const std::string& path, TProfile_LW* h1, Interval_t interval=run, MgmtAttr_t histo_mgmt = ATTRIB_MANAGED);
  StatusCode registerHist(const std::string& path, TProfile2D_LW* h1, Interval_t interval=run, MgmtAttr_t histo_mgmt = ATTRIB_MANAGED);
  
  StatusCode registerTree(const std::string& path, TTree* t1, Interval_t interval=run, MgmtAttr_t histo_mgmt = ATTRIB_MANAGED);
  
  //StatusCode registerHist(const std::string& path, TH1F_LW* h1, Interval_t interval=run, std::string dqmAlgorithm="");
  //StatusCode registerHist(const std::string& path, TH1I_LW* h1, Interval_t interval=run, std::string dqmAlgorithm="");
  //StatusCode registerHist(const std::string& path, TH2F_LW* h1, Interval_t interval=run, std::string dqmAlgorithm="");
  //StatusCode registerHist(const std::string& path, TH2I_LW* h1, Interval_t interval=run, std::string dqmAlgorithm="");
  //StatusCode registerHist(const std::string& path, TProfile_LW* h1, Interval_t interval=run, std::string dqmAlgorithm="");
  //StatusCode registerHist(const std::string& path, TProfile2D_LW* h1, Interval_t interval=run, std::string dqmAlgorithm="");
 
protected:

  StoreGateSvc* m_detStore  ;
  bool m_doRunCosmics;
  bool m_doRunBeam;
  bool m_doOfflineHists;
  bool m_doOnlineHists;

};

#endif

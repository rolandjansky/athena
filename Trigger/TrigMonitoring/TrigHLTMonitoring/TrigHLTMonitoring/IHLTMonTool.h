/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IHLTMONTOOL_H
#define IHLTMONTOOL_H

#include <string>
#include <vector>

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigConfigSvc/TrigConfigSvc.h"
#include "GaudiKernel/ServiceHandle.h" 
#include "TrigConfInterfaces/ITrigConfigTool.h"

// Forward declarations
class IInterface;
class TH1;
class StatusCode;
class TTree;
class TNtuple;
class TGraph;
class TProfile;
class CoolQuery;

///Concrete monitoring tool derived from MonitorToolBase
class IHLTMonTool : public ManagedMonitorToolBase {

  public:

  IHLTMonTool(const std::string & type, const std::string & name, const IInterface* parent); 
  virtual ~IHLTMonTool();

  StatusCode initialize();

  // Book & fill & check, should NOT be over-written: over-write the ones below
  // LS 20 Jan 2014 - Modified to reflect new ManagedMonitorToolBase class.
  virtual StatusCode bookHistograms();
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

  // Should be over-written
  virtual StatusCode init() = 0; // called by initialize
  virtual StatusCode book() = 0; // called by bookHistograms
  virtual StatusCode fill() = 0; // called by fillHistograms
  virtual StatusCode proc() = 0; // called by procHistograms

  // Available to aid the child
  int getL1info();
  int getRunNr();
  int getEventNr();
  int getLumiBlockNr();

  // Set current MonGroup
  void setCurrentMonGroup(const std::string &monGroup);

  // Accessor members
  void addMonGroup(MonGroup *m);
  void addHistogram(TH1 *h, const std::string &monGroup = "");
  void addHistogram(TH2 *h, const std::string &monGroup = "");
  void addTree(TTree *t, const std::string &monGroup = "");
  void addGraph(TGraph* g, const std::string &monGroup = "");
  void addProfile(TProfile *h, const std::string &monGroup = "");

  void removeHistogram(const std::string &histName, const std::string &monGroup = "");
  void removeHistogram2(const std::string &histName, const std::string &monGroup = "");
  void removeProfile(const std::string &histName, const std::string &monGroup = "");
  
  TH1 *hist(const std::string &histName, const std::string &monGroup = "");
  TH2 *hist2(const std::string &histName, const std::string &monGroup = "");
  TTree *tree(const std::string &treeName, const std::string &monGroup = "");
  TGraph* graph(const std::string &graphName, const std::string &monGroup ="");
  TProfile *profile(const std::string &histName, const std::string &monGroup = "");
  
  // Automatically casts a TTree to ntuple
  TNtuple *ntuple(const std::string &treeName, const std::string &monGroup = "");

  protected:

  MsgStream *m_log;

  bool m_cafonly;
  bool m_ignoreTruncationCheck;

  ServiceHandle<StoreGateSvc> m_storeGate;
  ServiceHandle<StoreGateSvc> m_inputMetaStore;
  ServiceHandle<TrigConf::ITrigConfigSvc> m_configsvc;
  
  // Infra-structure members  
  std::map<std::string, MonGroup *> m_group; // maintain Monitoring groups
  std::map<std::string, TH1 *> m_hist; // maintain histograms
  std::map<std::string, TH2 *> m_hist2; // maintain histograms
  std::map<std::string, TTree *> m_tree; // maintain trees
  std::map<std::string, TGraph *> m_graph; //maintain graphs
  std::map<std::string, TProfile *> m_profile; // maintain histograms

  // Book-keeping the current mon group
  std::string m_currentMonGroup;

  //trigger db connection
  CoolQuery* m_coolquery;

  Trig::TrigDecisionTool* getTDT(){return &*m_tdthandle;}

 private:
  ToolHandle<Trig::TrigDecisionTool> m_tdthandle;

 protected:
  ToolHandle<TrigConf::ITrigConfigTool> m_configTool; //< trigger configuration tool  

};
#endif


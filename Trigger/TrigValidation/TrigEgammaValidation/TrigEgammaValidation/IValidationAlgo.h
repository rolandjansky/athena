/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IVALIDATIONALGO_H
#define IVALIDATIONALGO_H

#include <string>
#include <vector>
#include <map>

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

#include "GaudiKernel/StatusCode.h"

///Concrete monitoring tool derived from MonitorToolBase
class IValidationAlgo : public Algorithm {

  public:

  IValidationAlgo(const std::string &myname, ISvcLocator* svcloc, const std::string &myfile); 
  virtual ~IValidationAlgo();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  
  // Should be over-written
  virtual StatusCode childInitialize() = 0;
  virtual StatusCode childExecute() = 0;
  virtual StatusCode childFinalize() = 0;

  // Set current MonGroup
  void cd(const std::string &dir);

  // Accessor members
  void addDirectory(const std::string &s);
  void addHistogram(TH1 *h, const std::string &dir = "");
  void addHistogram(TH2 *h, const std::string &dir = "");
  void addTree(TTree *t, const std::string &dir = "");

  TH1 *hist1(const std::string &histName, const std::string &dir = "");
  TH2 *hist2(const std::string &histName, const std::string &dir = "");
  TTree *tree(const std::string &treeName, const std::string &dir = "");

  void getHistsFromPath(const std::vector<std::string> &pattern, const std::vector<std::string> &notpattern, std::map<std::string, TH1 *> &ret);
  std::string getPath(const std::string &histName, const std::string &dir = "");

  protected:

  MsgStream *m_log;
  IToolSvc* m_toolSvc;
  StoreGateSvc * m_storeGate;
  std::string m_file;

  // Histograms
  ITHistSvc* m_histsvc;

  // Infra-structure members
  std::vector<std::string> m_dir; // maintain directories
  std::map<std::string, TH1 *> m_hist1; // maintain histograms
  std::map<std::string, TH2 *> m_hist2; // maintain histograms
  std::map<std::string, TTree *> m_tree; // maintain trees

  // Book-keeping the current mon group
  std::string m_currentDir;

};

#endif


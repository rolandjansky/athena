/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaAnalysisBaseTool_H
#define TrigEgammaAnalysisBaseTool_H


#include "TrigEgammaAnalysisTools/ITrigEgammaAnalysisBaseTool.h"
#include "AsgTools/AsgTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigEgammaAnalysisTools/ITrigEgammaMatchingTool.h"
class TrigEgammaAnalysisBaseTool
: public asg::AsgTool,
  virtual public ITrigEgammaAnalysisBaseTool {
ASG_TOOL_CLASS(TrigEgammaAnalysisBaseTool, ITrigEgammaAnalysisBaseTool)

public:

  TrigEgammaAnalysisBaseTool( const std::string& myname);
  ~TrigEgammaAnalysisBaseTool() {};

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  // Should be over-written
  //StatusCode childInitialize()=0;
  //StatusCode childExecute()=0;
  //StatusCode childFinalize()=0;

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
  virtual void print() const;

private:
  std::string m_msg;
protected:

  //IToolSvc* m_toolSvc;
  StoreGateSvc * m_storeGate;
  std::string m_file;

  // Histograms
  ITHistSvc* m_histsvc;

  // ToolHandles
  ToolHandle<Trig::TrigDecisionTool> m_trigdec;
  ToolHandle<ITrigEgammaMatchingTool> m_matchTool;
  // Infra-structure members
  std::vector<std::string> m_dir; // maintain directories
  std::map<std::string, TH1 *> m_hist1; // maintain histograms
  std::map<std::string, TH2 *> m_hist2; // maintain histograms
  std::map<std::string, TTree *> m_tree; // maintain trees

  // Book-keeping the current mon group
  std::string m_currentDir;
  /*! Simple setter to pick up correct probe PID for given trigger */
  void parseTriggerName(const std::string,const std::string, std::string &,float &, float &, std::string &,std::string &, bool&, bool&);
  /*! Creates static map to return L1 item from trigger name */
  std::string getL1Item(std::string trigger);
  
  std::string getProbePid(const std::string);// {return m_offProbeTightness;}
  /*! String for offline container key */
  std::string m_offElContKey;
};

#endif

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ITrigEgammaPlotTool.h

#ifndef ITrigEgammaPlotTool_H
#define ITrigEgammaPlotTool_H

#include "AsgTools/IAsgTool.h"
#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "TrigEgammaAnalysisTools/TrigEgammaInfo.h"

#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

#include <string>
#include <map>

class ITrigEgammaPlotTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(ITrigEgammaPlotTool)

public:

  virtual StatusCode initialize()=0;
  virtual StatusCode book(std::map<std::string,TrigInfo>)=0;
  virtual StatusCode execute()=0;
  virtual StatusCode finalize()=0;
  virtual StatusCode finalizeShifterHistos(std::map<std::string,TrigInfo>)=0;
  virtual void setParent(IHLTMonTool *)=0;
  virtual void setDetail(bool)=0;
  virtual void setAltBinning(bool)=0;
  virtual void setTP(bool)=0;
  virtual void setEmulation(bool)=0;
  virtual std::string getBasePath()=0;
  virtual void cd(const std::string &dir)=0;
  virtual void addDirectory(const std::string &s)=0;
  virtual void addHistogram(TH1 *h, const std::string &dir = "")=0;
  virtual void addHistogram(TH2 *h, const std::string &dir = "")=0;
  virtual void addTree(TTree *t, const std::string &dir = "")=0;

  virtual TH1 *hist1(const std::string &histName, const std::string &dir = "")=0;
  virtual TH2 *hist2(const std::string &histName, const std::string &dir = "")=0;
  virtual TTree *tree(const std::string &treeName, const std::string &dir = "")=0;
  virtual void setLabels(TH1* histo, const std::vector<std::string>& labels)=0;
  virtual std::map<std::string,TrigInfo> getTrigInfoMap()=0;
};

#endif

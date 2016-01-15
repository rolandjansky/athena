/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEGAMMAANALYSISTOOLS_EGAMMA_H_
#define TRIGEGAMMAANALYSISTOOLS_EGAMMA_H_


#include "TrigEgammaAnalysisTools/ITrigEgammaPlotTool.h"
#include "AsgTools/AsgTool.h"
#include "TrigHLTMonitoring/IHLTMonTool.h"
class TrigEgammaPlotTool
: public asg::AsgTool,
  virtual public ITrigEgammaPlotTool {
      ASG_TOOL_CLASS(TrigEgammaPlotTool, ITrigEgammaPlotTool)

      public:
          TrigEgammaPlotTool( const std::string& myname);
          ~TrigEgammaPlotTool() {};

          StatusCode initialize();
          StatusCode book(const std::string dir);
          StatusCode execute();
          StatusCode finalize();
          void setParent(IHLTMonTool *parent){ m_parent = parent;};
          void setDetail(bool doDetail){ m_detailedHists = doDetail; }
          void setAltBinning(bool doAltBins){ m_doJpsiee = doAltBins; }

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
          void setLabels(TH1* histo, const std::vector<std::string>& labels);

      protected:

      private:
          // Athena services
          StoreGateSvc * m_storeGate;
          ITHistSvc* m_histsvc;
          IHLTMonTool *m_parent;
          
          // Infra-structure members
          std::string m_file;
          std::set<std::string> m_mongroups; // MonGroups for T0
          std::vector<std::string> m_dir; // maintain directories
          std::map<std::string, TH1 *> m_hist1; // maintain histograms
          std::map<std::string, TH2 *> m_hist2; // maintain histograms
          std::map<std::string, TTree *> m_tree; // maintain trees

          // Book-keeping the current mon group
          std::string m_currentDir;

          // Properties
          bool m_doJpsiee;
          bool m_detailedHists;
          
          void getHistsFromPath(const std::vector<std::string> &pattern, const std::vector<std::string> &notpattern, std::map<std::string, TH1 *> &ret);
          std::string getPath(const std::string &histName, const std::string &dir = "");
          void bookAnalysisHistos(const std::string dir);
          void parseCaloRingsLayers( unsigned layer, unsigned &minRing, unsigned &maxRing, std::string &caloLayerName);
  };
#endif


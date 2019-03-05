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
          StatusCode book(std::map<std::string,TrigInfo> trigInfo);
          StatusCode execute();
          StatusCode finalizeShifterHistos(std::map<std::string,TrigInfo> trigInfo);
          StatusCode finalize();
          void setParent(IHLTMonTool *parent){ m_parent = parent;};
          void setDetail(bool doDetail){ m_detailedHists = doDetail; }
          void setTP(bool tp){ m_tp = tp; }
          void setEmulation(bool doEmu){ m_doEmulation = doEmu;}
          void setAltBinning(bool doAltBins){ m_doJpsiee = doAltBins; }
          std::string getBasePath() { return m_baseDir; }
          std::map<std::string,TrigInfo> getTrigInfoMap() { return m_trigInfo; }
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
          std::string m_baseDir;

          // Arrays for bin limits
          int m_nEtbins;
          int m_nEtabins;
          int m_ndefaultEtbins; //2D
          int m_ndefaultEtabins; //2D
          int m_ncoarseEtbins; //2D
          int m_ncoarseEtabins; //2D
          std::vector<float>  m_etbins;
          std::vector<float>  m_etabins;
          std::vector<float>  m_ringerEtbins;
          std::vector<float>  m_ringerEtabins;
          std::vector<double> m_defaultEtbins;
          std::vector<double> m_defaultEtabins;
          std::vector<double> m_coarseEtbins;
          std::vector<double> m_coarseEtabins;

          //Strings for bin labels
          std::vector<std::string> m_label_trigstep;
          std::vector<std::string> m_label_hltobj;
          std::vector<std::string> m_label_hltte;
          std::vector<std::string> m_label_isem;

          //Strings for shifter histograms
          std::vector<std::string> m_effplots;
          std::vector<std::string> m_distplots;
          std::vector<std::string> m_resplots;
          
          /*! creates map of trigger name and TrigInfo struct */
          std::map<std::string,TrigInfo> m_trigInfo;
          /* **************************************************
           * Menu Aware Monitoring Map category to trigger name 
           * 1-1 mapping of category to trigger name
           * **************************************************/
          std::map<std::string,std::string> m_mam;
          
          // Properties
          bool m_doJpsiee;
          bool m_detailedHists;
          bool m_tp;
          bool m_doEmulation;
          
          // Methods
          void setBinning();

          void copyArrayToVector(std::vector<float> vec,float arr[], int n); 
          void copyArrayToVector(std::vector<double> vec,double arr[],int n); 
          bool getCategoryFromTrigger(const std::string,std::string &);
          void getHistsFromPath(const std::vector<std::string> &pattern, const std::vector<std::string> &notpattern, std::map<std::string, TH1 *> &ret);
          std::string getPath(const std::string &histName, const std::string &dir = "");
          void bookExpertHistos(TrigInfo);
          void bookL1Histos(TrigInfo);
          void bookShifterHistos();
          //void bookAnalysisHistos(const std::string dir);
          void bookEfficiencyTProfile(const std::string dir);
          void bookEfficiencyHistos(const std::string dir);
          void bookEfficiency2DHistos(const std::string dir);
          void bookEgammaDistributionHistos(const std::string dir);
          void bookElectronDistributionHistos(const std::string dir);
          void bookDistributionHistos(const std::string dir);
          void bookResolutionHistos(const std::string dir);
          void bookElectronResolutionHistos(const std::string dir);
          void bookElectronIsoResolutionHistos(const std::string dir);
          void bookPhotonResolutionHistos(const std::string dir);
          void bookPhotonIsoResolutionHistos(const std::string dir);
          void bookExpertResolutionHistos(const std::string dir);
          void bookExpertL2CaloResolutionHistos(const std::string dir);
          void bookAbsResolutionHistos(const std::string dir);
          void bookRnnDistributionHistos(const std::string dir);
          void parseCaloRingsLayers( unsigned layer, unsigned &minRing, unsigned &maxRing, std::string &caloLayerName);
  };
#endif


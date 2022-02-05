/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**

   @class LArSamples::LArCellsEmptyMonitoring
   @Basic class aimed at running simple LAr monitoring function on ntuples produced from that very same package.
*/

#ifndef LArSamples_LArCellsEmptyMonitoring_H
#define LArSamples_LArCellsEmptyMonitoring_H

#include "LArSamplesMon/LArIdTranslatorHelper.h"
#include "TString.h"

namespace LArSamples {

  class LArCellsEmptyMonitoring
  {
  
    public:

      LArCellsEmptyMonitoring();
      ~LArCellsEmptyMonitoring();
      LArCellsEmptyMonitoring(const LArCellsEmptyMonitoring&) = delete;
      LArCellsEmptyMonitoring& operator= (const LArCellsEmptyMonitoring&) = delete;
      void Run(const TString& inputfile);
      void Run(const char* inputfile){ TString str(inputfile); Run(str); } // wrapper
      void TestRun(const TString& inputfile);
      void TestRun(const char* inputfile){ TString str(inputfile); Run(str); } // wrapper

      void GetLimits_EqLB(const char* inputfile, int& lbmin, int& lbmax, int& emin, int& emax, int& qmin, int& qmax, int& runNumber, const std::vector<int, std::allocator<int> >& BadLBList);
      std::vector<int, std::allocator<int> >  ReadBadLBList(const TString& LBfile);
      std::vector<int, std::allocator<int> >  GetBadLBList(const char* inputfile, int lbmin, int lbmax, double nsigma, int nlb, std::vector<int, std::allocator<int> >  DQLBList);
      bool CheckBadLBList(int lumiBlock, std::vector<int, std::allocator<int> > BadLBList);
      void GetMeanCellHits(const char* inputfile, int nlb, int lbmin, int lbmax, int nsigma, const std::vector<int, std::allocator<int> >& BadLBList, double& MeanHits, double& rmsHits, int& nlb_corr);
      bool CheckEventSelectionCriteria(int lumiBlock, int nsigma, double energy, Float_t noise, int lbmin, int lbmax);
      int CheckCellSelectionCriteria(int EventCount, int nsigmaHits, double MeanHits, double rmsHits, int nEvents_E_gt_ecut, double EventEnergySum, int nBadLBs, int nlb) const;
      bool CheckForPresamplerCells(int index);
      bool CheckPartition(int index) const;


      // setters
      void ReadDefectLBList(bool ReadList, const TString& LBfile);

      void SetSaveRootFile(bool flag=false){ m_SaveRootFile = flag; }
      void SetSaveRootTree(bool flag=false){ m_SaveRootTree = flag; }
      void SetSaveTextFile(bool flag=false){ m_SaveTextFile = flag; }
      void SetSelectRecurringBadCells(bool flag=false){ m_SelectRecurringBadCells = flag; }
      void SetAlgo(int algoindex = 2){ m_Algo = algoindex; }
      void SetNsigmaHitsFromMean(double nsigmahits = 10.){ m_nsigmaHits = nsigmahits; }
      void SetLumiblockRange(bool inputRange=false, int input_lbmin = 0, int input_lbmax = 0){
	m_SetLumiblockRange = inputRange; 
	m_input_lbmin = input_lbmin;
	m_input_lbmax = input_lbmax;
      }
      void SetBadLBRemovalOption(bool removeLB=true){ m_RemoveBadLB = removeLB; }
      void SetQthreshold(int qvalue = 4000){ m_Qthreshold = qvalue; }
      void SetNoiseNsigma(double nsig = 10.){ m_nsigma = nsig; }
      void SetEnergyCountThresholds(double ecut = 1., int counts1 = 250, double enecell1 = 1.5, int counts2 = 25, double enecell2 = 30){
	m_Ethreshold = ecut;
	m_UpperCountThreshold = counts1;
	m_LowerCountThreshold = counts2;
	m_LowerCellEnergyThreshold = enecell1;
	m_UpperCellEnergyThreshold = enecell2;
      }
      void SetPresamplerMask(bool PS = false){ m_MaskPresampler = PS; }
      void SetCaloMask(bool calo = false){ m_MaskCalo = calo; }
      void SetPartition(bool setpart, const std::string& partname );      
      std::string GetCryostat(int calo);

      // satellite functions that do not affect the main Run() functionalities
      void ScanOnlids(const TString& inputfile);
      void DoEtaPhiMonitoring(const char* inputfile,const char* optionplot,const char* optionsave);
      void TriggerEfficiency(const char* inputfile,float fractionInPS=0.8);

    private:

      std::unique_ptr<LArIdTranslatorHelper> m_LarIdTranslator; // LArIdTranslator object

      int m_nexpected;

      // int m_PrintLevel; // controls the level of printouts
      bool m_SaveRootFile; // whether to save output root histos
      bool m_SaveRootTree; // whether to save output root tree
      bool m_SaveTextFile; // whether to save output text file
      int m_Algo; // which algorithm to run
      bool m_SetLumiblockRange; // allows user to input LB range
      int m_input_lbmin;
      int m_input_lbmax;
      bool m_RemoveBadLB; // whether to remove bad LBs
      int m_Qthreshold; 
      double m_nsigma;
      double m_nsigmaHits;
      double m_Ethreshold;
      int m_UpperCountThreshold;
      int m_LowerCountThreshold;
      double m_LowerCellEnergyThreshold;
      double m_UpperCellEnergyThreshold;
      bool m_MaskPresampler = false; // whether to mask the EMBPS
      bool m_MaskCalo = false; // whether to mask everything but the EMBPS
      bool m_SelectRecurringBadCells; // whether to only select channels flagged by both algorithms
      bool m_SetPartition;
      int m_PartitionIndex = 0;
      std::string m_PartitionName;
      double m_Mean_checkBadLBs = 0.0;
      double m_RMS_checkBadLBs = 0.0;
      bool m_ReadDefectLBList = false;
      TString m_LBfile;

  };
}

#endif


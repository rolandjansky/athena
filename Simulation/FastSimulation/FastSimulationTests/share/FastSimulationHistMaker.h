#ifndef FastSimulationHistMaker_h
#define FastSimulationHistMaker_h

#include <TH1.h>
#include <TH2.h>
#include <TString.h>

#include "FastSimulationNTUPToHist.h"

class FastSimulationHistMaker : public FastSimulationNTUPToHist {
 public:
   
  FastSimulationHistMaker(TTree *tree=0):FastSimulationNTUPToHist(tree){}
  virtual ~FastSimulationHistMaker(){}
  virtual void Loop(const TString outputFileName, const Int_t maxEvents=-1);
  virtual void initialize();
  virtual void initEachEvent();
  virtual void execute();
  virtual void finalize();


 protected:
  TFile *fOutputFile;
  TString fOutputFileName;
  
  Int_t fMaxEvents;
  
  Bool_t m_verbose;

  // histograms
  TH1F *h_ph_E_ph_truth_E_deltaRatio;
  TH1F *h_el_pt_el_truth_pt_deltaRatio;
  TH1F *h_jet_AntiKt4TopoEM_pt_jet_AntiKt4Truth_pt_deltaRatio;
  TH1F *h_MET_LocHadTopo_et;

};

#endif

#ifdef FastSimulationHistMaker_cxx

#endif

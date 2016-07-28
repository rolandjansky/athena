/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>

#include "TObject.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TGraph.h"

#include "Region.cc"
#include "IBoardBase.hh"


class FTKTimeSim : public TObject {

private:
  std::vector<Region> region;

protected:

public:
  double DF_FwIn, DF_FwOut, DF_EwIn, DF_EwOut;  
  double DO_FwIn, DO_FwOut, DO_EwIn, DO_EwOut;  
  double AM_FwIn, AM_FwOut, AM_EwIn, AM_EwOut;  
  double SecDO_FwIn, SecDO_FwOut, SecDO_EwIn, SecDO_EwOut;  
  double TF_FwIn, TF_FwOut, TF_EwIn, TF_EwOut;  
  double SecStageSecDO_FwIn, SecStageSecDO_FwOut, SecStageSecDO_EwIn, SecStageSecDO_EwOut;  
  double SecStageTF_FwIn, SecStageTF_FwOut, SecStageTF_EwIn, SecStageTF_EwOut;  

  double pre_DF_EwOut;  
  double pre_DO_EwOut;  
  double pre_AM_EwOut;  
  double pre_SecDO_EwOut;  
  double pre_TF_EwOut;  
  double pre_SecStageSecDO_EwOut;  
  double pre_SecStageTF_EwOut;  



  TFile *tf;
  FTKTimeSim();
  ~FTKTimeSim();

  //  int temp_denom=1;
  int GetNRegion() {return NREGION;}
  int GetNPU() {return NPU;}

  void SetInputValueForRegion(int index, InputValues iv);
  Region GetRegion(int index) {return this->region[index];}


  void Calc();
  
  void FillInputValue();
  double GetTF_EwOut(int index);
  double GetNkill(int index);
  void FillResults(int dump, int jentry, char *description, int temp_denom, int s_split);

  void PrintResults(double sf_hit_overlapregion, int nloop, int nloopsloop, double sf_nregion, char *description2, int ntrack_ok, int ntrack_tot, int s_split);
  void PlotInputValues(char *description, int s_split, int nrbin);
  void PlotResults(char *description, int temp_denom, int s_split, int nrbin);
  

  TH1D *hreg_hit_pix0[NPU];
  TH1D *hreg_fit[NPU];
  TH1D *hreg_road[NPU];
  TH1D *hreg_naxFitRoad[NPU];

  TH1D *h_ftkhit;
  TH1D *h_ftkss;
  TH1D *h_maxftkhit;
  TH1D *h_ftkhit_layer[NLAYER];  
  TH1D *h_ftkss_layer[NLAYER];  

  TH1D *h_road;
  TH1D *h_road_rw;
  TH1D *h_maxroad;
  TH1D *h_road_AM[NASSOCIATIVEMEMORY];
  TH1D *h_road_rw_AM[NASSOCIATIVEMEMORY];

  TH1D *h_fit;
  TH1D *h_maxfit;

  TH2D *h_ftkhit_vs_road;
  TH2D *h_ftkhit_vs_fit;
  TH2D *h_road_vs_fit;

  TH1D *h_timeee;
  TH1D *h_timeee_full;

  TH1D *h_DF_EwOut;
  TH1D *h_DO_EwOut;
  TH1D *h_AM_EwOut;
  TH1D *h_SecDO_EwOut;
  TH1D *h_TF_EwOut;
  TH1D *h_SecStageSecDO_EwOut;
  TH1D *h_SecStageTF_EwOut;

  TH2D *h_event_detail;

  //  TGraph *DF_buff[NPU][NDATAFORMATTER];
  TGraph *DO_buff[NPU][NDATAORGANIZER];
  TGraph *AM_buff[NPU][NASSOCIATIVEMEMORY];
  TGraph *SecDO_buff[NPU][NDATAORGANIZER];
  TGraph *SecDO_buff2[NPU][NDATAORGANIZER];
  TGraph *TF_buff[NPU][NTRACKFITTER];

  vector<double> DO_buff_vec[NPU][NDATAORGANIZER];
  vector<double> AM_buff_vec[NPU][NASSOCIATIVEMEMORY];
  vector<double> SecDO_buff_vec[NPU][NDATAORGANIZER];
  vector<double> SecDO_buff2_vec[NPU][NDATAORGANIZER];
  vector<double> TF_buff_vec[NPU][NTRACKFITTER];

  // ** hist for secound stage ** //
  TH1D *h_SecStage_road;
  TH1D *h_SecStage_maxroad;
  TH1D *h_SecStage_road_AM[NASSOCIATIVEMEMORY];

  TH1D *h_SecStage_fit;
  TH1D *h_SecStage_maxfit;

  TH1D *h_SecStage_timeee;
  TH1D *h_SecStage_SecDO_EwOut;
  TH1D *h_SecStage_TF_EwOut;
  TH2D *h_SecStage_event_detail;




  TGraph *g_DO_EwOut[NPU];
  TGraph *g_AM_EwOut[NPU];
  TGraph *g_SecDO_EwOut[NPU];
  TGraph *g_TF_EwOut[NPU];
  TGraph *g_SecStageSecDO_EwOut[NPU];
  TGraph *g_SecStageTF_EwOut[NPU];
  TGraph *g_EwOut;
  
  TGraph *g_RoadsX5[NPU];
  TGraph *g_FitsX1[NPU];
  TGraph *g_maxFitRoad[NPU];
  vector<double> RoadsX5_vec[NPU];
  vector<double> FitsX1_vec[NPU];
  vector<double> maxFitRoad_vec[NPU];



  vector<double> DO_EwOut_vec[NPU];
  vector<double> AM_EwOut_vec[NPU];
  vector<double> SecDO_EwOut_vec[NPU];
  vector<double> TF_EwOut_vec[NPU];
  vector<double> SecStageSecDO_EwOut_vec[NPU];
  vector<double> SecStageTF_EwOut_vec[NPU];
  vector<double> EwOut_vec;

  vector<double> xAxis_vec;

  //  ClassDef(FTKTimeSim,2) 
};

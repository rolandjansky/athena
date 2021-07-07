/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBBPCALIGNMENTMONTOOL_H
#define TBBPCALIGNMENTMONTOOL_H

#include "AthenaMonitoring/MonitorToolBase.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IAxis.h"
#include "StoreGate/StoreGateSvc.h" 

class TBBPCAlignmentMonTool: public MonitorToolBase
{

 public:  
  
  // constructor and destructor
  TBBPCAlignmentMonTool(const std::string & type, const std::string & name,
               const IInterface* parent);
  ~TBBPCAlignmentMonTool();

  // public methods
  StatusCode initialize();
  StatusCode bookHists();
  StatusCode fillHists();
  StatusCode finalHists();
  void recalculate();
  void mean_rms_print();

  // for the prob(chi2/dof)
  float gammq(float a, float x);
  void gcf(float *gammcf, float a, float x, float *gln);
  void gser(float *gamser, float a, float x, float *gln);
  float gammln(float xx);
  
 private:
  StatusCode getnewcalib();

  // private members
  //std::string m_path;
  std::string m_SGkeybpc, m_SGkeytrack;
  std::vector<std::string> m_bpc_names;
  std::vector<float> xmean;
  std::vector<float> xrms;
  std::vector<float> ymean;
  std::vector<float> yrms;
  std::vector<float> xentries;
  std::vector<float> yentries;

  // BPC alignment constant file variables  
  std::vector<float> m_bpc_posZX,m_bpc_posZY,m_bpc_alignX,m_bpc_alignY;
  std::vector<float> m_bpc_errposZX,m_bpc_errposZY,m_bpc_erralignX,m_bpc_erralignY;
  std::vector<float> m_bpc_errmeasX,m_bpc_errmeasY;

  std::vector<float> m_bpc_widths;

  std::string m_calib_filename;
  unsigned int m_runnumber;

  std::string m_out_file;
  int m_bpcnum; 
  bool m_monitor_bpccalib;  

  //float x2, y2;
  //float x3, y3;
  //int counter;

  //chi2s
  float m_chi2x;
  float m_chi2y;
  float m_Qx;
  float m_Qy;

  // BPC histograms
  IHistogram1D** m_histo_bpcX_res;
  IHistogram1D** m_histo_bpcY_res;
  IHistogram1D** m_histo_chi2x;
  IHistogram1D** m_histo_chi2y;
  IHistogram1D** m_histo_Qx;
  IHistogram1D** m_histo_Qy;
   
  // histogram parameters
  std::vector<float> m_posmax,m_posmin;
  std::vector<int> m_posbin;

  

};


#endif

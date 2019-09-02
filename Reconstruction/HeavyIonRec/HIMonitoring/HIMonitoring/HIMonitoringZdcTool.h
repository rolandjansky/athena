/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef HIMONITORINGZDCTOOL_H
#define HIMONITORINGZDCTOOL_H


#include "AthenaMonitoring/ManagedMonitorToolBase.h"


class TH1D;
class TH2D_LW;


class HIMonitoringZdcTool: public ManagedMonitorToolBase
{
public:
  HIMonitoringZdcTool(const std::string& type, const std::string& name,
                      const IInterface* parent);

  virtual ~HIMonitoringZdcTool();

  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();


  void book_hist();
private:
  double m_NEW_AEM {}, m_NEW_AHAD1 {}, m_NEW_AHAD2 {}, m_NEW_AHAD3 {};
  double m_NEW_CEM {}, m_NEW_CHAD1 {}, m_NEW_CHAD2 {}, m_NEW_CHAD3 {};
  //double m_AEM {}, m_AHAD1 {}, m_AHAD2 {}, m_AHAD3 {};
  //double m_CEM {}, m_CHAD1 {}, m_CHAD2 {}, m_CHAD3 {};
  double m_G0AEM {}, m_G0AHAD1 {}, m_G0AHAD2 {}, m_G0AHAD3 {};
  double m_G0CEM {}, m_G0CHAD1 {}, m_G0CHAD2 {}, m_G0CHAD3 {};
  double m_G1AEM {}, m_G1AHAD1 {}, m_G1AHAD2 {}, m_G1AHAD3 {};
  double m_G1CEM {}, m_G1CHAD1 {}, m_G1CHAD2 {}, m_G1CHAD3 {};
  //double m_SumSideA {}, m_SumSideC {};
  double m_NEW_SumSideA {}, m_NEW_SumSideC {};

  static constexpr int s_Nsamp {
    7
  };
  static constexpr int s_Nmod {
    4
  };
  static constexpr int s_Nside {
    2
  };


  /// histograms
  TH1D* m_hamp[s_Nmod][s_Nside] {};
  //TH1D* m_hamp_NEW[s_Nmod][s_Nside] {};
  TH1D* m_hampG0[s_Nmod][s_Nside] {};
  TH1D* m_hampG1[s_Nmod][s_Nside] {};
  TH1D* m_hSumSideAmp[s_Nside] {};
  //TH1D* m_hSumSideAmp_NEW[s_Nside] {};
  TH1D* m_hSumSideAmpG0[s_Nside] {};
  TH1D* m_hSumSideAmpG1[s_Nside] {};
  TH2D_LW* m_hEM_HAD1[s_Nside] {};
  TH2D_LW* m_hHAD1_HAD2[s_Nside] {};
  TH2D_LW* m_hHAD2_HAD3[s_Nside] {};
  TH2D_LW* m_hSideAC {};

  //TH2D_LW* m_hEM_HAD1_NEW[s_Nside] {};
  //TH2D_LW* m_hHAD1_HAD2_NEW[s_Nside] {};
  //TH2D_LW* m_hHAD2_HAD3_NEW[s_Nside] {};
  //TH2D_LW* m_hSideAC_NEW {};
};

#endif

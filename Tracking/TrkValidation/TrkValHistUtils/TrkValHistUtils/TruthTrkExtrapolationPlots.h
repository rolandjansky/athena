/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVALHISTUTILS_TRUTHTRKEXTRAPOLATIONPLOTS_H
#define TRKVALHISTUTILS_TRUTHTRKEXTRAPOLATIONPLOTS_H

#include "PlotBase.h"
#include "xAODTruth/TruthParticle.h"
#include "TVector3.h"

namespace Trk{
  //this class creates kinematic plots at different layers in the detector
  class ExtrLayerPlots:public PlotBase {
  public:
    ExtrLayerPlots(PlotBase *pParent, const std::string& sDir, std::string sLayerName);

    void fill (const xAOD::TruthParticle& truthprt, const std::string& sLayerName, float weight=1.0);
    std::string m_sLayerName;

    TH1* p;
    TH1* px;
    TH1* py;
    TH1* pz;
    TH1* p_extr;
    TH1* px_extr;
    TH1* py_extr;
    TH1* pz_extr;
    TH1* dp_truth;
    TH1* dpx_truth;
    TH1* dpy_truth;
    TH1* dpz_truth;
    TH1* pt;
    TH1* eta;
    TH1* phi;
    TH1* theta;
    	
  private:
    void initializePlots();
  };
  //this class creates kinematic plots for different regions in the detector
  class ExtrRegionPlots:public PlotBase {
  public:
    ExtrRegionPlots(PlotBase *pParent, const std::string& sDir, std::string sRegionBegin, std::string sRegionEnd);

    void fill (const xAOD::TruthParticle& truthprt, const std::string& sRegionBegin, const std::string& sRegionEnd, float weight=1.0);
    std::string m_sRegionBegin;
    std::string m_sRegionEnd;

    TH1* dp ;
    TH1* dpt;
    TH1* dpx;
    TH1* dpy;
    TH1* dpz;
    TH1* dp_extr ;
    TH1* dpx_extr ;
    TH1* dpy_extr ;
    TH1* dpz_extr ;

    TH2* dp_vs_p;
    TH2* dp_vs_phi;
    TH2* dp_vs_eta; 
    TH2* dp_vs_eta_phi;
    TH2* p_vs_p;
    TH2* p_extr_vs_p_extr;
    TH2* dp_extr_vs_eta;

 
    TH1* dR;
    TH2* dR_vs_p ;
    TH2* dR_vs_dp;
    TH2* dR_vs_eta;
    TH2* dR_vs_phi;
    TH2* dR_vs_eta_phi;

    TH1* dphi;
    TH1* dtheta;
    TH1* dAngle;
    
    TH2* dp_CaloEntry_IP_dp_CaloEntry_etxr;

   private: 
    void initializePlots();
  };


class TruthTrkExtrapolationPlots: public PlotBase {
  public:
    TruthTrkExtrapolationPlots(PlotBase *pParent, const std::string& sDir);
    void fill(const xAOD::TruthParticle& truthprt, float weight=1.0);

    ExtrLayerPlots m_CaloEntry;
    ExtrLayerPlots m_MuonEntry;
    ExtrLayerPlots m_MuonExit;
    
    /* ExtrRegionPlots m_Calo; */
    /* ExtrRegionPlots m_MS; */
    /* ExtrRegionPlots m_IP; */
    /* ExtrRegionPlots m_IP_Calo; */
    /* ExtrRegionPlots m_Calo_MS; */
    /* ExtrRegionPlots m_IP_MS; */
};

}

#endif


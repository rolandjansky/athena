/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
// Author: M. T. Anthony

#include "PFOHistUtils/PFO_FE_Comparison_PlotMaker.h"
#include <math.h>
namespace PFO {
  PFO_FE_Comparison_PlotMaker::PFO_FE_Comparison_PlotMaker(PlotBase* pParent,std::string sDir, std::string sPFOContainerName, std::string sFEContainerName): PlotBase(pParent,sDir), asg::AsgMessaging("PFO_FE_Comparison_PlotMaker"), m_sPFOContainerName(sPFOContainerName), m_sFEContainerName(sFEContainerName) {
    // init all hists as nullptrs
    m_PFO_FE_ptratio=nullptr;
    m_PFO_FE_delta_pt=nullptr;
    m_PFO_FE_delta_R=nullptr;
    m_PFO_FE_delta_eta=nullptr;
    m_PFO_FE_delta_phi=nullptr;
    m_PFO_FE_dE=nullptr;
  } // end of constructor
  
  void PFO_FE_Comparison_PlotMaker::initializePlots(){
    m_PFO_FE_ptratio= Book1D("_ptratio",m_sPFOContainerName+m_sFEContainerName+"_ptratio",40,0,2);
    m_PFO_FE_delta_pt= Book1D("_deltapt",m_sPFOContainerName+m_sFEContainerName+"_deltapt (GeV)",40,-200,200);
    m_PFO_FE_delta_R=Book1D("_deltaR",m_sPFOContainerName+m_sFEContainerName+"_deltaR",30,0,3);
    m_PFO_FE_delta_eta=Book1D("_abs_delta_eta",m_sPFOContainerName+m_sFEContainerName+"_abs_delta_eta",20,0,5); // use absolute value since we check consistency, as opposed to absolute difference.
    m_PFO_FE_delta_phi=Book1D("_delta_phi",m_sPFOContainerName+m_sFEContainerName+"_delta_phi",60,-3,3);
    m_PFO_FE_dE=Book1D("_dE",m_sPFOContainerName+m_sFEContainerName+"_dE (GeV)",40,-200,200);
  }
  
  void PFO_FE_Comparison_PlotMaker::Fill(const xAOD::PFO* pfo, const xAOD::FlowElement* fe){
    TLorentzVector fe_tlv=fe->p4();  
    TLorentzVector pfo_tlv=pfo->p4();
 
    double pfo_pt=pfo_tlv.Pt();
    double pfo_eta=pfo_tlv.Eta();
    double pfo_e=pfo_tlv.E();
    
    double fe_pt=fe_tlv.Pt();
    double fe_eta=fe_tlv.Eta();
    double fe_e=fe_tlv.E();
    double ptratio=0;
    
    double deltaPt=fe_pt-pfo_pt;
    if(fe_pt>0){
      ptratio = pfo_pt/fe_pt;
    }
    double dR=fe_tlv.DeltaR(pfo_tlv);
    double abs_dEta=std::abs(fe_eta-pfo_eta); 
    double dphi=fe_tlv.DeltaPhi(pfo_tlv);
    double dE=fe_e-pfo_e;
    
    // now fill the histos
    
    m_PFO_FE_dE->Fill(dE);
    m_PFO_FE_ptratio->Fill(ptratio);
    m_PFO_FE_delta_pt->Fill(deltaPt);
    m_PFO_FE_delta_R->Fill(dR);
    m_PFO_FE_delta_eta->Fill(abs_dEta);
    m_PFO_FE_delta_phi->Fill(dphi);
  } // end of fill function
}// end of PFO namespace

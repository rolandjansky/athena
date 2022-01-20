// this file is -*- C++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JETCALIBTOOLS_PURESIDUAL3DCORRECTION_H
#define JETCALIBTOOLS_PURESIDUAL3DCORRECTION_H
////////////////////////////////////////////////////////////////
/// \class PU3DCorrectionHelper
///
/// PU correction helper for "3D" method
/// This self class allows :
///  - to load the 3D calibration constants from a filename
///  - to calculate the corrected pT (or momentum scale factor) given pt, eta, rho, mu,NPV
/// 
/// This class is self contained in a single header file : this is to ease its re-usage in
/// the development phases of calib derivation frameworks.
///

#include "TH2D.h"
#include "TFile.h"
#include "TAxis.h"
#include <stdio.h>
#include <vector>

namespace PUCorrection {
  
  struct PU3DCorrectionHelper {

    ~PU3DCorrectionHelper(){
      
    }

    /// Main function which returns the corrected pT
    float correctedPt(float pt, float eta, float area, float rho, float mu, int NPV ) const {
      float areaCorr = area*rho*m_rhoEnergyScale;

      float  pt_ref = pt*m_pTEnergyScale ;
      float calibration3D = correction3D(pt_ref - areaCorr ,
					 eta,
					 mu,
					 NPV);
      pt_ref =  pt_ref - areaCorr - calibration3D;
      float deltaPt = 0.0;
      if(m_applyDeltaPtTerm){
	deltaPt = deltaPtCorrection( pt_ref, eta );
      }

      return (pt*m_pTEnergyScale -areaCorr - calibration3D + deltaPt)/m_pTEnergyScale;      
    }

    /// same as above but returns the ration pT_corrected/pT_uncorrected
    float correctionFactor(float pt, float eta, float area, float rho, float mu, int NPV ) const {
      float ptCorr = correctedPt(pt,eta,area,rho,mu,NPV);
      return ptCorr/pt;
    }

    
    
    /// calculate the mu,NPV dependent part of the correction.
    /// IMPORTANT : the pt must be given in GeV 
    float correction3D(float pt, float eta , float mu, int NPV) const {
      pt = pt < 1 ? 1 : pt;
      int muNPVbin = m_ref3DHisto->FindBin(mu, NPV);
      int etaBin = m_etaBins->FindFixBin(std::abs(eta)) - 1;
      float t0 = m_3Dp0_vs_muNPV[ etaBin ]->GetBinContent(muNPVbin);
      if(t0 <= -999.9) {
	muNPVbin = m_closestNonEmpty[etaBin][muNPVbin];
      }

      float p0 = m_3Dp0_vs_muNPV[ etaBin ]->GetBinContent(muNPVbin);
      float p1 = m_3Dp1_vs_muNPV[ etaBin ]->GetBinContent(muNPVbin);
      
      if(m_use3Dp2) {
	float p2= m_3Dp2_vs_muNPV[ etaBin ]->GetBinContent(muNPVbin) ;
	return p0+p1*pt+p2*log(pt); 
      }
      return p0+p1*pt;            
    }

    
    
    /// IMPORTANT : the pt must be given in GeV 
    float deltaPtCorrection(float pt, float eta) const {
      int etabin = m_Dptp0_vs_eta->FindBin(std::abs(eta)) ;
      float p0 = m_Dptp0_vs_eta->GetBinContent(etabin);
      float p1 = m_Dptp1_vs_eta->GetBinContent(etabin);
      return p0+pt*p1;
    }
    

    
    /// Loads the calib constants from histograms in TFile named fileName. 
    void loadParameters(const std::string & fileName,
			const std::string &param3D_name = "param3D",
			const std::string &paramDelta_name = "paramDeltaPt",
			const std::string &etaBins_name = "etaBins"
			){
      std::unique_ptr<TFile> tmpF(TFile::Open( fileName.c_str() ));
      std::vector<float> * etaBins_v = (std::vector<float>*)tmpF->Get(etaBins_name.c_str());
      std::vector<double> tmp(etaBins_v->begin(), etaBins_v->end() );
      m_etaBins.reset( new TAxis( tmp.size()-1, tmp.data() ) );
      TList *param3D_l = (TList*) tmpF->Get(param3D_name.c_str());

      TList *param3D_p0 = (TList*) param3D_l->At(0);
      m_3Dp0_vs_muNPV.resize( param3D_p0->GetSize() );
      TList *param3D_p1 = (TList*) param3D_l->At(1);
      m_3Dp1_vs_muNPV.resize( param3D_p1->GetSize() );

      TList *param3D_p2 = nullptr;
      if(m_use3Dp2) {
	param3D_p2 = (TList*) param3D_l->At(2);
	m_3Dp2_vs_muNPV.resize( param3D_p2->GetSize() );
      }

      for(size_t i=0 ; i<(etaBins_v->size()-1); i++){
	m_3Dp0_vs_muNPV[i].reset((TH2D*)param3D_p0->At(i));
	m_3Dp0_vs_muNPV[i]->SetDirectory(nullptr);
	m_3Dp1_vs_muNPV[i].reset((TH2D*)param3D_p1->At(i));
	m_3Dp1_vs_muNPV[i]->SetDirectory(nullptr);
	if(m_use3Dp2) {
	  m_3Dp2_vs_muNPV[i].reset( (TH2D*)param3D_p2->At(i) );
	  m_3Dp2_vs_muNPV[i]->SetDirectory(nullptr);
	}
      }
      m_ref3DHisto = m_3Dp0_vs_muNPV[0].get();
      
      TList* paramDelta_l = (TList*) tmpF->Get(paramDelta_name.c_str());
      m_Dptp0_vs_eta.reset( (TH1F*) paramDelta_l->At(0) );
      m_Dptp0_vs_eta->SetDirectory(nullptr);
      m_Dptp1_vs_eta.reset( (TH1F*) paramDelta_l->At(1) ) ;      
      m_Dptp1_vs_eta->SetDirectory(nullptr);
      setupClosestNonEmptyBins();

      // Finding the maximum values of mu and NPV for each eta bin,
      // which is used to avoid using overflow bins in the residual calculation.
      // The bin center is used to avoid any effects on the boundaries.
      for(size_t i=0 ; i<(etaBins_v->size()-1); i++){
        m_maxMu.push_back(m_3Dp0_vs_muNPV[i]->GetXaxis()->GetBinCenter(m_3Dp0_vs_muNPV[i]->GetNbinsX()));
        m_maxNPV.push_back(m_3Dp0_vs_muNPV[i]->GetYaxis()->GetBinCenter(m_3Dp0_vs_muNPV[i]->GetNbinsY()));
      }
    }



    void setupClosestNonEmptyBins(){
      // Pre calculate the positions of the closest non empty bins 

      // we have a map (bin -> non-empty bin) for each eta slice :
      m_closestNonEmpty.resize( m_3Dp0_vs_muNPV.size() );
      for(size_t etabin=0;  etabin< m_closestNonEmpty.size() ;etabin++ ){

	TH2D *refHisto =  m_3Dp0_vs_muNPV[etabin].get() ;
	int nTot = refHisto->GetNcells();
	TAxis * xax = refHisto->GetXaxis();
	TAxis * yax = refHisto->GetYaxis();
	float xscale = 1./(xax->GetXmax()-xax->GetXmin()); xscale *= xscale;
	float yscale = 1./(yax->GetXmax()-yax->GetXmin()); yscale *= yscale;
	int nbinX = xax->GetNbins();
	int nbinY = yax->GetNbins();
	// initialize the map with -1 :
	m_closestNonEmpty[etabin].resize( nTot, -1 );

	// loop over each bin
	for(int xi=1;xi<nbinX+1;xi++) for(int yi=1;yi<nbinY+1;yi++) {
	    int bi = refHisto->GetBin(xi,yi);
	    if(refHisto->GetBinContent( bi ) >-999.) continue; // non empty bin, we don't need to find anything for it.

	    int clBin = -1;
	    float x0 = xax->GetBinCenter(xi);
	    float y0 = yax->GetBinCenter(yi);
	    float minDr2=1e10; // just pick a bigger value than any distance in the (mu,NPV) plan
	    // loop over other bins to find the closest non-empty :
	    for(int xj=1;xj<nbinX+1;xj++) for(int yj=1;yj<nbinY+1;yj++) {
		int bj = refHisto->GetBin(xj,yj);
		if(refHisto->GetBinContent( bj ) <=-999.) continue; // this is an empty bin
		float x = xax->GetBinCenter(xj);
		float y = yax->GetBinCenter(yj);
		float dr2 = (x0-x)*(x0-x)*xscale+(y0-y)*(y0-y)*yscale;
		if(dr2<minDr2){ minDr2 = dr2; clBin = bj;} // found a closest-bin candidate
	      }
	    m_closestNonEmpty[etabin][bi] = clBin;
	  }

      }
    }


    
    //************************************
    // data members
    
    // 3D corrections parameters :
    std::unique_ptr<TAxis> m_etaBins ;
    std::vector<std::unique_ptr<TH2D> > m_3Dp0_vs_muNPV;
    std::vector<std::unique_ptr<TH2D> > m_3Dp1_vs_muNPV;
    std::vector<std::unique_ptr<TH2D> > m_3Dp2_vs_muNPV;
    TH2D* m_ref3DHisto = nullptr;
    bool m_use3Dp2=true;

    // DeltaPt corrections parameters :
    std::unique_ptr<TH1F> m_Dptp0_vs_eta=nullptr;
    std::unique_ptr<TH1F> m_Dptp1_vs_eta=nullptr;

    std::vector<float> m_maxMu;
    std::vector<float> m_maxNPV;

    //
    float m_maxPt=170.0 ; // GeV !!
    float m_rhoEnergyScale = 0.001; // 0.001 when rho is given in MeV. 
    float m_pTEnergyScale = 0.001; // 0.001 when pT is given in MeV. 

    bool m_applyDeltaPtTerm = true; //boolean to switch on/off the deltaPt correction

    // ***************
    // 
    std::vector< std::vector<int> > m_closestNonEmpty;












    // *******************************************************
    // function belows are not used in the correction evaluation but have proven useful for tests during developments
    // of the calibration methods. We keep them here just in case.
    // 

    /// calculate the mu,NPV dependent part of the correction (this is only used for tests and validation)    
    /// IMPORTANT : the pt must be given in GeV 
    float correction3D_noextrap(float pt, float eta , float mu, int NPV) const {
      int muNPVbin = m_ref3DHisto->FindBin(mu, NPV);
      int etaBin = m_etaBins->FindFixBin(std::abs(eta)) - 1;

      // Don't want to use the overflow bins, so using this instead
      if(mu > m_maxMu[etaBin]){ 
        mu=m_maxMu[etaBin];
      }
      if(NPV > m_maxNPV[etaBin]){ 
        NPV=m_maxNPV[etaBin];
      }

      float p0 = m_3Dp0_vs_muNPV[ etaBin ]->GetBinContent(muNPVbin);
      float p1 = m_3Dp1_vs_muNPV[ etaBin ]->GetBinContent(muNPVbin);       

      
      if ( (p0<= -999.9) || (p1<=-999.9) ) return 0;
      
      if(m_use3Dp2) {
	float p2= m_3Dp2_vs_muNPV[ etaBin ]->GetBinContent(muNPVbin) ;
	if ( p2<=-999.9 ) return 0;
	return p0+p1*log(pt-p2);	
      }
      return p0+p1*pt;            
    }


    // Just a test to see if we get smoother calib by doing an interpolation at point (mu,NPV), not used yet
    float correction3D_interp(float pt, float eta , float mu, int NPV) const {
      int etaBin = m_etaBins->FindFixBin(std::abs(eta)) - 1;
      float p0 = m_3Dp0_vs_muNPV[ etaBin ]->Interpolate(mu, NPV);
      float p1 = m_3Dp1_vs_muNPV[ etaBin ]->Interpolate(mu,NPV);
      
      if ( (p0<= -999.9) || (p1<=-999.9) ) return 0;
      
      if(m_use3Dp2) {
	float p2= m_3Dp2_vs_muNPV[ etaBin ]->Interpolate(mu,NPV) ;
	if ( p2<=-999.9 ) return 0;
	return p0+p1*log(pt-p2);	
      }
      return p0+p1*pt;            
    }
    

  };

}

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrkPhysMonitoring/OnlinePlots.h"
#include <cmath>

void OnlinePlots::initializePlots(){	
  Book1D(m_Good_t0, "m_Good_t0",m_sTrackCollectionName+":High Quality Refitted t0 per Region::<t0> [ns];Large Detector #eta Regions",4,0,4.);
  Book1D(m_Good_t0_EA, "m_Good_t0_EA",m_sTrackCollectionName+": Good Refitted t0 - EA - [ns];[ns]", 100,-10.0,10.0);
  Book1D(m_Good_t0_BA, "m_Good_t0_BA",m_sTrackCollectionName+": Good Refitted t0 - BA - [ns];[ns]", 100,-10.0,10.0);
  Book1D(m_Good_t0_BC, "m_Good_t0_BC",m_sTrackCollectionName+": Good Refitted t0 - BC - [ns];[ns]", 100,-10.0,10.0);
  Book1D(m_Good_t0_EC, "m_Good_t0_EC",m_sTrackCollectionName+": Good Refitted t0 - EC - [ns];[ns]", 100,-10.0,10.0);                    
}

void OnlinePlots::fill(int region, float t0){
  if( region == 0 )      m_Good_t0_EC->Fill(t0);
  else if( region == 2 ) m_Good_t0_BC->Fill(t0);
  else if( region == 3 ) m_Good_t0_BA->Fill(t0);
  else if( region == 4 ) m_Good_t0_EA->Fill(t0);
}

void OnlinePlots::finalizePlots(){
  CalculateResults(m_Good_t0_EC, 1, "Endcap C");
  CalculateResults(m_Good_t0_BC, 2, "Barrel C");
  CalculateResults(m_Good_t0_BA, 3, "Barrel A");
  CalculateResults(m_Good_t0_EC, 4, "Endcap A");
}

void OnlinePlots::CalculateResults(TH1F* hist, int iBin, std::string sLabel){
  m_Good_t0->SetBinContent(iBin,hist->GetMean(1));
  m_Good_t0->SetBinError(  iBin,hist->GetEntries() ? m_Good_t0_EC->GetRMS(1)/sqrt(hist->GetEntries()) : 0.0);
  m_Good_t0->GetXaxis()->SetBinLabel(  iBin,sLabel.c_str()); 
}
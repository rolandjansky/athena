/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrkPhysMonitoring/ZSignalPlots.h"
#include "MuonTrkPhysMonitoring/MuonTrkPhysMonUtils.h"

using namespace MuonTrkPhysMonUtils;

void ZSignalPlots::initializePlots(){	
  Book1D(m_Z_Mass, "m_Z_Mass",m_sTrackCollectionName+": M_{Z} Mass;Mass [GeV];", 100,60.0,120.0);
  Book1D(m_Z_occupancy, "m_Z_occupancy",m_sTrackCollectionName+": M_{Z} Track Occupancy;;N_{#mu}", 8,0.,8.);
        
  Book2D(m_Z_Efficiency, "m_Z_Efficiency",m_sTrackCollectionName+": M_{Z} Efficiency", 8,0.,8.,16,1.,17.);    
  Book2D(m_Z_EffNumerator, "m_Z_EffNumerator",m_sTrackCollectionName+": M_{Z} Efficiency Numerator", 8,0.,8.,16,1.,17.);    
  Book2D(m_Z_EffDenominator, "m_Z_EffDenominator",m_sTrackCollectionName+": M_{Z} Efficiency Denominator", 8,0.,8.,16,1.,17.);

  Book1D(m_Z_Efficiency_eta, "m_Z_Efficiency_eta",m_sTrackCollectionName+": M_{Z} Efficiency;#eta", 20,-2.5,2.5);
  Book1D(m_Z_EffNumerator_eta, "m_Z_EffNumerator_eta",m_sTrackCollectionName+": M_{Z} Efficiency Numerator;#eta", 20,-2.5,2.5);
  Book1D(m_Z_EffDenominator_eta, "m_Z_EffDenominator_eta",m_sTrackCollectionName+": M_{Z} Efficiency Denominator;#eta", 20,-2.5,2.5);

  Book1D(m_M_Z_Mean, "m_M_Z_Mean",m_sTrackCollectionName+": M_{Z} Mean;;M_{Z}[GeV]", 16,1.,17.);

  Book1D(m_M_Z_EA_EA, "m_M_Z_EA_EA",m_sTrackCollectionName+"M_{Z} EA_{+} - EA_{-}  ;M_{Z}[GeV];", 100,60.,120.);
  Book1D(m_M_Z_EA_BA, "m_M_Z_EA_BA",m_sTrackCollectionName+"M_{Z} EA_{+} - BA_{-}  ;M_{Z}[GeV];", 100,60.,120.);
  Book1D(m_M_Z_EA_BC, "m_M_Z_EA_BC",m_sTrackCollectionName+"M_{Z} EA_{+} - BC_{-}  ;M_{Z}[GeV];", 100,60.,120.);
  Book1D(m_M_Z_EA_EC, "m_M_Z_EA_EC",m_sTrackCollectionName+"M_{Z} EA_{+} - EC_{-}  ;M_{Z}[GeV];", 100,60.,120.);
  Book1D(m_M_Z_BA_EA, "m_M_Z_BA_EA",m_sTrackCollectionName+"M_{Z} BA_{+} - EA_{-}  ;M_{Z}[GeV];", 100,60.,120.);
  Book1D(m_M_Z_BA_BA, "m_M_Z_BA_BA",m_sTrackCollectionName+"M_{Z} BA_{+} - BA_{-}  ;M_{Z}[GeV];", 100,60.,120.);
  Book1D(m_M_Z_BA_BC, "m_M_Z_BA_BC",m_sTrackCollectionName+"M_{Z} BA_{+} - BC_{-}  ;M_{Z}[GeV];", 100,60.,120.);
  Book1D(m_M_Z_BA_EC, "m_M_Z_BA_EC",m_sTrackCollectionName+"M_{Z} BA_{+} - EC_{-}  ;M_{Z}[GeV];", 100,60.,120.);
  Book1D(m_M_Z_BC_EA, "m_M_Z_BC_EA",m_sTrackCollectionName+"M_{Z} BC_{+} - EA_{-}  ;M_{Z}[GeV];", 100,60.,120.);
  Book1D(m_M_Z_BC_BA, "m_M_Z_BC_BA",m_sTrackCollectionName+"M_{Z} BC_{+} - BA_{-}  ;M_{Z}[GeV];", 100,60.,120.);
  Book1D(m_M_Z_BC_BC, "m_M_Z_BC_BC",m_sTrackCollectionName+"M_{Z} BC_{+} - BC_{-}  ;M_{Z}[GeV];", 100,60.,120.);
  Book1D(m_M_Z_BC_EC, "m_M_Z_BC_EC",m_sTrackCollectionName+"M_{Z} BC_{+} - EC_{-}  ;M_{Z}[GeV];", 100,60.,120.);
  Book1D(m_M_Z_EC_EA, "m_M_Z_EC_EA",m_sTrackCollectionName+"M_{Z} EC_{+} - EA_{-}  ;M_{Z}[GeV];", 100,60.,120.);
  Book1D(m_M_Z_EC_BA, "m_M_Z_EC_BA",m_sTrackCollectionName+"M_{Z} EC_{+} - BA_{-}  ;M_{Z}[GeV];", 100,60.,120.);
  Book1D(m_M_Z_EC_BC, "m_M_Z_EC_BC",m_sTrackCollectionName+"M_{Z} EC_{+} - BC_{-}  ;M_{Z}[GeV];", 100,60.,120.);
  Book1D(m_M_Z_EC_EC, "m_M_Z_EC_EC",m_sTrackCollectionName+"M_{Z} EC_{+} - EC_{-}  ;M_{Z}[GeV];", 100,60.,120.);
    
  Book1D(m_M_Z_Sigma, "m_M_Z_Sigma",m_sTrackCollectionName+": #sigma(M_{Z});;#sigma(M_{Z})[GeV]", 16,1.,17.);
  Book1D(m_N_Z_Lumi, "m_N_Z_Lumi",m_sTrackCollectionName+": N_{#mu} from Z Decay;;N_{#mu}", 16,1.,17.);  
}

void ZSignalPlots::fill(const float fEta1, const float fEta2, const float fIVmass_record) {

  float etaRegionSmall1 = EtaRegionSmall(fEta1);
  float etaRegionSmall2 = EtaRegionSmall(fEta2);
  
	if((etaRegionSmall1 == 3.5 && etaRegionSmall2 == 3.5)) m_M_Z_EA_EA->Fill(fIVmass_record);
	if((etaRegionSmall1 == 3.5 && etaRegionSmall2 == 2.5)) m_M_Z_EA_BA->Fill(fIVmass_record);
	if((etaRegionSmall1 == 3.5 && etaRegionSmall2 == 1.5)) m_M_Z_EA_BC->Fill(fIVmass_record);
	if((etaRegionSmall1 == 3.5 && etaRegionSmall2 == 0.5)) m_M_Z_EA_EC->Fill(fIVmass_record);
                
	if((etaRegionSmall1 == 2.5 && etaRegionSmall2 == 3.5)) m_M_Z_BA_EA->Fill(fIVmass_record);
	if((etaRegionSmall1 == 2.5 && etaRegionSmall2 == 2.5)) m_M_Z_BA_BA->Fill(fIVmass_record);
	if((etaRegionSmall1 == 2.5 && etaRegionSmall2 == 1.5)) m_M_Z_BA_BC->Fill(fIVmass_record);
	if((etaRegionSmall1 == 2.5 && etaRegionSmall2 == 0.5)) m_M_Z_BA_EC->Fill(fIVmass_record);
                
	if((etaRegionSmall1 == 1.5 && etaRegionSmall2 == 3.5)) m_M_Z_BC_EA->Fill(fIVmass_record);
	if((etaRegionSmall1 == 1.5 && etaRegionSmall2 == 2.5)) m_M_Z_BC_BA->Fill(fIVmass_record);
	if((etaRegionSmall1 == 1.5 && etaRegionSmall2 == 1.5)) m_M_Z_BC_BC->Fill(fIVmass_record);
	if((etaRegionSmall1 == 1.5 && etaRegionSmall2 == 0.5)) m_M_Z_BC_EC->Fill(fIVmass_record);
                
	if((etaRegionSmall1 == 0.5 && etaRegionSmall2 == 3.5)) m_M_Z_EC_EA->Fill(fIVmass_record);
	if((etaRegionSmall1 == 0.5 && etaRegionSmall2 == 2.5)) m_M_Z_EC_BA->Fill(fIVmass_record);
	if((etaRegionSmall1 == 0.5 && etaRegionSmall2 == 1.5)) m_M_Z_EC_BC->Fill(fIVmass_record);
	if((etaRegionSmall1 == 0.5 && etaRegionSmall2 == 0.5)) m_M_Z_EC_EC->Fill(fIVmass_record);

}

void ZSignalPlots::finalizePlots(){
  LabelCombinedEtaRegions(m_M_Z_Mean->GetXaxis());
  LabelCombinedEtaRegions(m_M_Z_Sigma->GetXaxis());
  LabelCombinedEtaRegions(m_N_Z_Lumi->GetXaxis());
  LabelLargeEtaRegions(m_Z_occupancy->GetXaxis());
  LabelLargeEtaRegions(m_Z_Efficiency->GetXaxis());
  LabelLargeEtaRegions(m_Z_EffNumerator->GetXaxis());
  LabelLargeEtaRegions(m_Z_EffDenominator->GetXaxis());
  LabelSectorAxis(m_Z_Efficiency->GetYaxis());
  LabelSectorAxis(m_Z_EffNumerator->GetYaxis());
  LabelSectorAxis(m_Z_EffDenominator->GetYaxis());
  
  for(int bin = 1; bin < m_Z_EffNumerator->GetSize(); bin++){
    if(m_Z_EffDenominator->GetBinContent(bin) > 0)
      m_Z_Efficiency->SetBinContent(bin, (m_Z_EffNumerator->GetBinContent(bin)) / (m_Z_EffDenominator->GetBinContent(bin)));
  }

  for(int bin = 1; bin < m_Z_Efficiency_eta->GetSize(); bin++) {
    if(m_Z_EffDenominator_eta->GetBinContent(bin) > 0)
      m_Z_Efficiency_eta->SetBinContent(bin, (m_Z_EffNumerator_eta->GetBinContent(bin)) / (m_Z_EffDenominator_eta->GetBinContent(bin)));
  }

  SetResultsBin( 1,m_M_Z_EC_EC);
  SetResultsBin( 2,m_M_Z_EC_BC);
  SetResultsBin( 3,m_M_Z_EC_BA);
  SetResultsBin( 4,m_M_Z_EC_EA);
  SetResultsBin( 5,m_M_Z_BC_EC);
  SetResultsBin( 6,m_M_Z_BC_BC);
  SetResultsBin( 7,m_M_Z_BC_BA);
  SetResultsBin( 8,m_M_Z_BC_EA);
  SetResultsBin( 9,m_M_Z_BA_EC);
  SetResultsBin(10,m_M_Z_BA_BC);
  SetResultsBin(11,m_M_Z_BA_BA);
  SetResultsBin(12,m_M_Z_BA_EA);
  SetResultsBin(13,m_M_Z_EA_EC);
  SetResultsBin(14,m_M_Z_EA_BC);
  SetResultsBin(15,m_M_Z_EA_BA);
  SetResultsBin(16,m_M_Z_EA_EA);
}

void ZSignalPlots::SetResultsBin(int iBin, TH1F* pInputHist){      
  m_M_Z_Mean->  SetBinContent(iBin, pInputHist->GetMean(1));
  m_M_Z_Mean->  SetBinError(  iBin, pInputHist->GetMeanError(1));
  m_M_Z_Sigma-> SetBinContent(iBin, pInputHist->GetRMS(1));
  m_N_Z_Lumi->  SetBinContent(iBin, pInputHist->GetEntries());
}

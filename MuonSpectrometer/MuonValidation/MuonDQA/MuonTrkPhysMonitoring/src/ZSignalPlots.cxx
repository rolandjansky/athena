/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrkPhysMonitoring/ZSignalPlots.h"
#include "MuonTrkPhysMonitoring/MuonTrkPhysMonUtils.h"

using namespace MuonTrkPhysMonUtils;

ZSignalPlots::ZSignalPlots(PlotBase *pParent, std::string sDir):PlotBase(pParent, sDir),
                                                                m_Z_Mass(0),
                                                                m_Z_occupancy(0),
                                                                m_Z_Efficiency(0),
                                                                m_Z_EffNumerator(0),
                                                                m_Z_EffDenominator(0),
                                                                m_Z_Efficiency_eta(0),
                                                                m_Z_EffNumerator_eta(0),
                                                                m_Z_EffDenominator_eta(0),
                                                                m_M_Z_Mean(0),
                                                                m_M_Z_Sigma(0),
                                                                m_N_Z_Lumi(0),
                                                                m_M_Z_EA_EA(0),
                                                                m_M_Z_EA_BA(0),
                                                                m_M_Z_EA_BC(0),
                                                                m_M_Z_EA_EC(0),
                                                                m_M_Z_BA_EA(0),
                                                                m_M_Z_BA_BA(0),
                                                                m_M_Z_BA_BC(0),
                                                                m_M_Z_BA_EC(0),
                                                                m_M_Z_BC_EA(0),
                                                                m_M_Z_BC_BA(0),
                                                                m_M_Z_BC_BC(0),
                                                                m_M_Z_BC_EC(0),
                                                                m_M_Z_EC_EA(0),
                                                                m_M_Z_EC_BA(0),
                                                                m_M_Z_EC_BC(0),
                                                                m_M_Z_EC_EC(0)
{}

void ZSignalPlots::initializePlots(){	

    m_Z_Mass      = Book1D("m_Z_Mass",      "m_Z_Mass: M_{Z} Mass;Mass [GeV];",              100, 60.0, 120.0);
    m_Z_occupancy = Book1D("m_Z_occupancy", "m_Z_occupancy: M_{Z} Track Occupancy;;N_{#mu}",   8,  0.0,   8.0);
    
    m_Z_Efficiency     = Book2D("m_Z_Efficiency",     "m_Z_Efficiency: M_{Z} Efficiency",                 8, 0.0, 8.0, 16, 1.0, 17.0);    
    m_Z_EffNumerator   = Book2D("m_Z_EffNumerator",   "m_Z_EffNumerator: M_{Z} Efficiency Numerator",     8, 0.0, 8.0, 16, 1.0, 17.0);    
    m_Z_EffDenominator = Book2D("m_Z_EffDenominator", "m_Z_EffDenominator: M_{Z} Efficiency Denominator", 8, 0.0, 8.0, 16, 1.0, 17.0);
    
    m_Z_Efficiency_eta     = Book1D("m_Z_Efficiency_eta",     "m_Z_Efficiency_eta: M_{Z} Efficiency;#eta",                 20, -2.5, 2.5);
    m_Z_EffNumerator_eta   = Book1D("m_Z_EffNumerator_eta",   "m_Z_EffNumerator_eta: M_{Z} Efficiency Numerator;#eta",     20, -2.5, 2.5);
    m_Z_EffDenominator_eta = Book1D("m_Z_EffDenominator_eta", "m_Z_EffDenominator_eta: M_{Z} Efficiency Denominator;#eta", 20, -2.5, 2.5);
    
    m_M_Z_EA_EA = Book1D("m_M_Z_EA_EA", "m_M_Z_EA_EAM_{Z} EA_{+} - EA_{-}; M_{Z}[GeV];", 100, 60.0, 120.0);
    m_M_Z_EA_BA = Book1D("m_M_Z_EA_BA", "m_M_Z_EA_BAM_{Z} EA_{+} - BA_{-}; M_{Z}[GeV];", 100, 60.0, 120.0);
    m_M_Z_EA_BC = Book1D("m_M_Z_EA_BC", "m_M_Z_EA_BCM_{Z} EA_{+} - BC_{-}; M_{Z}[GeV];", 100, 60.0, 120.0);
    m_M_Z_EA_EC = Book1D("m_M_Z_EA_EC", "m_M_Z_EA_ECM_{Z} EA_{+} - EC_{-}; M_{Z}[GeV];", 100, 60.0, 120.0);
    m_M_Z_BA_EA = Book1D("m_M_Z_BA_EA", "m_M_Z_BA_EAM_{Z} BA_{+} - EA_{-}; M_{Z}[GeV];", 100, 60.0, 120.0);
    m_M_Z_BA_BA = Book1D("m_M_Z_BA_BA", "m_M_Z_BA_BAM_{Z} BA_{+} - BA_{-}; M_{Z}[GeV];", 100, 60.0, 120.0);
    m_M_Z_BA_BC = Book1D("m_M_Z_BA_BC", "m_M_Z_BA_BCM_{Z} BA_{+} - BC_{-}; M_{Z}[GeV];", 100, 60.0, 120.0);
    m_M_Z_BA_EC = Book1D("m_M_Z_BA_EC", "m_M_Z_BA_ECM_{Z} BA_{+} - EC_{-}; M_{Z}[GeV];", 100, 60.0, 120.0);
    m_M_Z_BC_EA = Book1D("m_M_Z_BC_EA", "m_M_Z_BC_EAM_{Z} BC_{+} - EA_{-}; M_{Z}[GeV];", 100, 60.0, 120.0);
    m_M_Z_BC_BA = Book1D("m_M_Z_BC_BA", "m_M_Z_BC_BAM_{Z} BC_{+} - BA_{-}; M_{Z}[GeV];", 100, 60.0, 120.0);
    m_M_Z_BC_BC = Book1D("m_M_Z_BC_BC", "m_M_Z_BC_BCM_{Z} BC_{+} - BC_{-}; M_{Z}[GeV];", 100, 60.0, 120.0);
    m_M_Z_BC_EC = Book1D("m_M_Z_BC_EC", "m_M_Z_BC_ECM_{Z} BC_{+} - EC_{-}; M_{Z}[GeV];", 100, 60.0, 120.0);
    m_M_Z_EC_EA = Book1D("m_M_Z_EC_EA", "m_M_Z_EC_EAM_{Z} EC_{+} - EA_{-}; M_{Z}[GeV];", 100, 60.0, 120.0);
    m_M_Z_EC_BA = Book1D("m_M_Z_EC_BA", "m_M_Z_EC_BAM_{Z} EC_{+} - BA_{-}; M_{Z}[GeV];", 100, 60.0, 120.0);
    m_M_Z_EC_BC = Book1D("m_M_Z_EC_BC", "m_M_Z_EC_BCM_{Z} EC_{+} - BC_{-}; M_{Z}[GeV];", 100, 60.0, 120.0);
    m_M_Z_EC_EC = Book1D("m_M_Z_EC_EC", "m_M_Z_EC_ECM_{Z} EC_{+} - EC_{-}; M_{Z}[GeV];", 100, 60.0, 120.0);
    
    m_M_Z_Mean  = Book1D("m_M_Z_Mean",  "m_M_Z_Mean: M_{Z} Mean;;M_{Z}[GeV]",             16, 1.0, 17.0);
    m_M_Z_Sigma = Book1D("m_M_Z_Sigma", "m_M_Z_Sigma: #sigma(M_{Z});;#sigma(M_{Z})[GeV]", 16, 1.0, 17.0);
    m_N_Z_Lumi  = Book1D("m_N_Z_Lumi",  "m_N_Z_Lumi: N_{#mu} from Z Decay;;N_{#mu}",      16, 1.0, 17.0);  
}

void ZSignalPlots::fill(const float eta_mu_plus, const float eta_mu_minus, const float invariant_mass){

    float etaRegionCoarsePlus  = EtaRegionCoarse(eta_mu_plus);
    float etaRegionCoarseMinus = EtaRegionCoarse(eta_mu_minus);
    float etaRegionFinePlus    = EtaRegionFine(eta_mu_plus);
    float etaRegionFineMinus   = EtaRegionFine(eta_mu_minus);
  
	if((etaRegionCoarsePlus == 3.5 && etaRegionCoarseMinus == 3.5)) m_M_Z_EA_EA->Fill(invariant_mass/1000.0);
	if((etaRegionCoarsePlus == 3.5 && etaRegionCoarseMinus == 2.5)) m_M_Z_EA_BA->Fill(invariant_mass/1000.0);
	if((etaRegionCoarsePlus == 3.5 && etaRegionCoarseMinus == 1.5)) m_M_Z_EA_BC->Fill(invariant_mass/1000.0);
	if((etaRegionCoarsePlus == 3.5 && etaRegionCoarseMinus == 0.5)) m_M_Z_EA_EC->Fill(invariant_mass/1000.0);
                
	if((etaRegionCoarsePlus == 2.5 && etaRegionCoarseMinus == 3.5)) m_M_Z_BA_EA->Fill(invariant_mass/1000.0);
	if((etaRegionCoarsePlus == 2.5 && etaRegionCoarseMinus == 2.5)) m_M_Z_BA_BA->Fill(invariant_mass/1000.0);
	if((etaRegionCoarsePlus == 2.5 && etaRegionCoarseMinus == 1.5)) m_M_Z_BA_BC->Fill(invariant_mass/1000.0);
	if((etaRegionCoarsePlus == 2.5 && etaRegionCoarseMinus == 0.5)) m_M_Z_BA_EC->Fill(invariant_mass/1000.0);
                
	if((etaRegionCoarsePlus == 1.5 && etaRegionCoarseMinus == 3.5)) m_M_Z_BC_EA->Fill(invariant_mass/1000.0);
	if((etaRegionCoarsePlus == 1.5 && etaRegionCoarseMinus == 2.5)) m_M_Z_BC_BA->Fill(invariant_mass/1000.0);
	if((etaRegionCoarsePlus == 1.5 && etaRegionCoarseMinus == 1.5)) m_M_Z_BC_BC->Fill(invariant_mass/1000.0);
	if((etaRegionCoarsePlus == 1.5 && etaRegionCoarseMinus == 0.5)) m_M_Z_BC_EC->Fill(invariant_mass/1000.0);
                
	if((etaRegionCoarsePlus == 0.5 && etaRegionCoarseMinus == 3.5)) m_M_Z_EC_EA->Fill(invariant_mass/1000.0);
	if((etaRegionCoarsePlus == 0.5 && etaRegionCoarseMinus == 2.5)) m_M_Z_EC_BA->Fill(invariant_mass/1000.0);
	if((etaRegionCoarsePlus == 0.5 && etaRegionCoarseMinus == 1.5)) m_M_Z_EC_BC->Fill(invariant_mass/1000.0);
	if((etaRegionCoarsePlus == 0.5 && etaRegionCoarseMinus == 0.5)) m_M_Z_EC_EC->Fill(invariant_mass/1000.0);

    m_Z_Mass->Fill(invariant_mass/1000.0);
    m_Z_occupancy->Fill(etaRegionFinePlus);
    m_Z_occupancy->Fill(etaRegionFineMinus);

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
    
    for(int bin = 1; bin < m_Z_EffNumerator->GetXaxis()->GetNbins(); bin++){
        if(m_Z_EffDenominator->GetBinContent(bin) > 0){
            m_Z_Efficiency->SetBinContent(bin, (m_Z_EffNumerator->GetBinContent(bin)) / (m_Z_EffDenominator->GetBinContent(bin)));
        }
    }
    
    for(int bin = 1; bin < m_Z_Efficiency_eta->GetXaxis()->GetNbins(); bin++){
        if(m_Z_EffDenominator_eta->GetBinContent(bin) > 0){
            m_Z_Efficiency_eta->SetBinContent(bin, (m_Z_EffNumerator_eta->GetBinContent(bin)) / (m_Z_EffDenominator_eta->GetBinContent(bin)));
        }
    }

    SetResultsBin( 1, m_M_Z_EC_EC);
    SetResultsBin( 2, m_M_Z_EC_BC);
    SetResultsBin( 3, m_M_Z_EC_BA);
    SetResultsBin( 4, m_M_Z_EC_EA);
    SetResultsBin( 5, m_M_Z_BC_EC);
    SetResultsBin( 6, m_M_Z_BC_BC);
    SetResultsBin( 7, m_M_Z_BC_BA);
    SetResultsBin( 8, m_M_Z_BC_EA);
    SetResultsBin( 9, m_M_Z_BA_EC);
    SetResultsBin(10, m_M_Z_BA_BC);
    SetResultsBin(11, m_M_Z_BA_BA);
    SetResultsBin(12, m_M_Z_BA_EA);
    SetResultsBin(13, m_M_Z_EA_EC);
    SetResultsBin(14, m_M_Z_EA_BC);
    SetResultsBin(15, m_M_Z_EA_BA);
    SetResultsBin(16, m_M_Z_EA_EA);
}

void ZSignalPlots::SetResultsBin(int iBin, TH1* pInputHist){

    m_M_Z_Mean-> SetBinContent(iBin, pInputHist->GetMean(1));
    m_M_Z_Mean-> SetBinError(  iBin, pInputHist->GetMeanError(1));
    m_M_Z_Sigma->SetBinContent(iBin, pInputHist->GetRMS(1));
    m_N_Z_Lumi-> SetBinContent(iBin, pInputHist->GetEntries());

}

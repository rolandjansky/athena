/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackMonitoring/RecoPhysPlots.h"


RecoPhysPlots::RecoPhysPlots(PlotBase *pParent, std::string sDir, std::string recObj):PlotBase(pParent, sDir),
    m_Mass(0),
    m_occupancy(0),
    m_2occupancy(0),
    m_M_Mean(0),
    m_M_Sigma(0),
    m_M_EA_EA(0),
    m_M_EA_BA(0),
    m_M_EA_BC(0),
    m_M_EA_EC(0),
    m_M_BA_EA(0),
    m_M_BA_BA(0),
    m_M_BA_BC(0),
    m_M_BA_EC(0),
    m_M_BC_EA(0),
    m_M_BC_BA(0),
    m_M_BC_BC(0),
    m_M_BC_EC(0),
    m_M_EC_EA(0),
    m_M_EC_BA(0),
    m_M_EC_BC(0),
    m_M_EC_EC(0),
    type(recObj)
{}

void RecoPhysPlots::initializePlots(){

    std::string Zsig("Z");
    std::string Jsig("Jpsi");
    std::string name;
    bool doZ = false; bool doJ = false;

    if (!type.compare(Zsig)) { name="Z"; doZ = true; }
    else if (!type.compare(Jsig)) { name="J/#psi"; doJ = true; }
    else { name="other"; }

    int nBins = 100;
    double minBin = 0.0;
    double maxBin = 1000.0;

    /// Error check for doZ and doJ both true here;

    if (doZ) {
      nBins = 100;
      minBin = 60.0;
      maxBin = 120.0;
    }
    else if (doJ) {
      nBins = 30;
      minBin = 2.5;
      maxBin = 3.5;
    }
    //resonance mass distributuioin
    m_Mass      = Book1D("m_"+type+"_Mass",      "m_Mass: M_{"+name+"} Mass;Mass [GeV];",              nBins, minBin, maxBin);
    //resonance two muon distribution
    m_occupancy = Book1D("m_"+type+"_occupancy", "m_occupancy: M_{"+name+"} Track Occupancy;;N_{#mu}",   8,  0.0,   8.0);
    m_2occupancy = Book1D("m_"+type+"_2occupancy", "m_2occupancy: M_{"+name+"} Two Track Occupancy;;N_{#mu}",   16,  0.5,   16.5);

    //resonance mass distribution in each mass range
    m_M_EA_EA = Book1D("m_"+type+"_M_EA_EA", "m_M_EA_EAM_{"+name+"} EA_{+} - EA_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
    m_M_EA_BA = Book1D("m_"+type+"_M_EA_BA", "m_M_EA_BAM_{"+name+"} EA_{+} - BA_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
    m_M_EA_BC = Book1D("m_"+type+"_M_EA_BC", "m_M_EA_BCM_{"+name+"} EA_{+} - BC_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
    m_M_EA_EC = Book1D("m_"+type+"_M_EA_EC", "m_M_EA_ECM_{"+name+"} EA_{+} - EC_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
    m_M_BA_EA = Book1D("m_"+type+"_M_BA_EA", "m_M_BA_EAM_{"+name+"} BA_{+} - EA_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
    m_M_BA_BA = Book1D("m_"+type+"_M_BA_BA", "m_M_BA_BAM_{"+name+"} BA_{+} - BA_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
    m_M_BA_BC = Book1D("m_"+type+"_M_BA_BC", "m_M_BA_BCM_{"+name+"} BA_{+} - BC_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
    m_M_BA_EC = Book1D("m_"+type+"_M_BA_EC", "m_M_BA_ECM_{"+name+"} BA_{+} - EC_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
    m_M_BC_EA = Book1D("m_"+type+"_M_BC_EA", "m_M_BC_EAM_{"+name+"} BC_{+} - EA_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
    m_M_BC_BA = Book1D("m_"+type+"_M_BC_BA", "m_M_BC_BAM_{"+name+"} BC_{+} - BA_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
    m_M_BC_BC = Book1D("m_"+type+"_M_BC_BC", "m_M_BC_BCM_{"+name+"} BC_{+} - BC_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
    m_M_BC_EC = Book1D("m_"+type+"_M_BC_EC", "m_M_BC_ECM_{"+name+"} BC_{+} - EC_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
    m_M_EC_EA = Book1D("m_"+type+"_M_EC_EA", "m_M_EC_EAM_{"+name+"} EC_{+} - EA_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
    m_M_EC_BA = Book1D("m_"+type+"_M_EC_BA", "m_M_EC_BAM_{"+name+"} EC_{+} - BA_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
    m_M_EC_BC = Book1D("m_"+type+"_M_EC_BC", "m_M_EC_BCM_{"+name+"} EC_{+} - BC_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
    m_M_EC_EC = Book1D("m_"+type+"_M_EC_EC", "m_M_EC_ECM_{"+name+"} EC_{+} - EC_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
    //for compare mass mean and sigma, done in post processing
    m_M_Mean  = Book1D("m_"+type+"_M_Mean",  "m_M_Mean: M_{"+name+"} Mean;;M_{"+name+"}[GeV]",             16, 1.0, 17.0);
    m_M_Sigma = Book1D("m_"+type+"_M_Sigma", "m_M_Sigma: #sigma(M_{"+name+"});;#sigma(M_{"+name+"})[GeV]", 16, 1.0, 17.0);

    //label the plots
    LabelCombinedEtaRegions(m_M_Mean->GetXaxis());
    LabelCombinedEtaRegions(m_M_Sigma->GetXaxis());
    LabelCombinedEtaRegions(m_2occupancy->GetXaxis());
    LabelLargeEtaRegions(m_occupancy->GetXaxis());
}

void RecoPhysPlots::fill(std::vector<std::pair<const xAOD::Muon*, const xAOD::Muon*> > mumucandidates){
    for (auto mumu: mumucandidates){
        const float invariant_mass = (mumu.first->p4() + mumu.second->p4()).M();
        fill(mumu.first->eta(), mumu.second->eta(), invariant_mass);
    }
}

void RecoPhysPlots::fill(const float eta_mu_plus, const float eta_mu_minus, const float invariant_mass){

    float etaRegionCoarsePlus  = EtaRegionCoarse(eta_mu_plus);
    float etaRegionCoarseMinus = EtaRegionCoarse(eta_mu_minus);
    float etaRegionFinePlus    = EtaRegionFine(eta_mu_plus);
    float etaRegionFineMinus   = EtaRegionFine(eta_mu_minus);
  
    if((etaRegionCoarsePlus == 3.5 && etaRegionCoarseMinus == 3.5)) {m_M_EA_EA->Fill(invariant_mass/1000.0); m_2occupancy->Fill(16);}
    if((etaRegionCoarsePlus == 3.5 && etaRegionCoarseMinus == 2.5)) {m_M_EA_BA->Fill(invariant_mass/1000.0); m_2occupancy->Fill(15);}
    if((etaRegionCoarsePlus == 3.5 && etaRegionCoarseMinus == 1.5)) {m_M_EA_BC->Fill(invariant_mass/1000.0); m_2occupancy->Fill(14);}
    if((etaRegionCoarsePlus == 3.5 && etaRegionCoarseMinus == 0.5)) {m_M_EA_EC->Fill(invariant_mass/1000.0); m_2occupancy->Fill(13);}

    if((etaRegionCoarsePlus == 2.5 && etaRegionCoarseMinus == 3.5)) {m_M_BA_EA->Fill(invariant_mass/1000.0); m_2occupancy->Fill(12);}
    if((etaRegionCoarsePlus == 2.5 && etaRegionCoarseMinus == 2.5)) {m_M_BA_BA->Fill(invariant_mass/1000.0); m_2occupancy->Fill(11);}
    if((etaRegionCoarsePlus == 2.5 && etaRegionCoarseMinus == 1.5)) {m_M_BA_BC->Fill(invariant_mass/1000.0); m_2occupancy->Fill(10);}
    if((etaRegionCoarsePlus == 2.5 && etaRegionCoarseMinus == 0.5)) {m_M_BA_EC->Fill(invariant_mass/1000.0); m_2occupancy->Fill(9);}

    if((etaRegionCoarsePlus == 1.5 && etaRegionCoarseMinus == 3.5)) {m_M_BC_EA->Fill(invariant_mass/1000.0); m_2occupancy->Fill(8);}
    if((etaRegionCoarsePlus == 1.5 && etaRegionCoarseMinus == 2.5)) {m_M_BC_BA->Fill(invariant_mass/1000.0); m_2occupancy->Fill(7);}
    if((etaRegionCoarsePlus == 1.5 && etaRegionCoarseMinus == 1.5)) {m_M_BC_BC->Fill(invariant_mass/1000.0); m_2occupancy->Fill(6);}
    if((etaRegionCoarsePlus == 1.5 && etaRegionCoarseMinus == 0.5)) {m_M_BC_EC->Fill(invariant_mass/1000.0); m_2occupancy->Fill(5);}

    if((etaRegionCoarsePlus == 0.5 && etaRegionCoarseMinus == 3.5)) {m_M_EC_EA->Fill(invariant_mass/1000.0); m_2occupancy->Fill(4);}
    if((etaRegionCoarsePlus == 0.5 && etaRegionCoarseMinus == 2.5)) {m_M_EC_BA->Fill(invariant_mass/1000.0); m_2occupancy->Fill(3);}
    if((etaRegionCoarsePlus == 0.5 && etaRegionCoarseMinus == 1.5)) {m_M_EC_BC->Fill(invariant_mass/1000.0); m_2occupancy->Fill(2);}
    if((etaRegionCoarsePlus == 0.5 && etaRegionCoarseMinus == 0.5)) {m_M_EC_EC->Fill(invariant_mass/1000.0); m_2occupancy->Fill(1);}

    m_Mass->Fill(invariant_mass/1000.0);
    m_occupancy->Fill(etaRegionFinePlus);
    m_occupancy->Fill(etaRegionFineMinus);

}

void RecoPhysPlots::finalizeRecoPlots(){
    SetResultsBin( 1, m_M_EC_EC);
    SetResultsBin( 2, m_M_EC_BC);
    SetResultsBin( 3, m_M_EC_BA);
    SetResultsBin( 4, m_M_EC_EA);
    SetResultsBin( 5, m_M_BC_EC);
    SetResultsBin( 6, m_M_BC_BC);
    SetResultsBin( 7, m_M_BC_BA);
    SetResultsBin( 8, m_M_BC_EA);
    SetResultsBin( 9, m_M_BA_EC);
    SetResultsBin(10, m_M_BA_BC);
    SetResultsBin(11, m_M_BA_BA);
    SetResultsBin(12, m_M_BA_EA);
    SetResultsBin(13, m_M_EA_EC);
    SetResultsBin(14, m_M_EA_BC);
    SetResultsBin(15, m_M_EA_BA);
    SetResultsBin(16, m_M_EA_EA);
}

float RecoPhysPlots::EtaRegionFine(double eta)
{
  if (eta < -2.50) {return -1.;} // underflow
  if (eta < -2.00) {return 0.5;} // CSC C
  if (eta < -1.70) {return 1.5;} // ENDCAP C
  if (eta < -1.05) {return 2.5;} // Trans C
  if (eta <  0.00) {return 3.5;} // Barrel C
  if (eta >  2.50) {return 8.5;} // OverFlow
  if (eta >  2.00) {return 7.5;} // CSC A
  if (eta >  1.70) {return 6.5;} // ENDCAP A
  if (eta >  1.05) {return 5.5;} // Trans A
  if (eta >  0.00) {return 4.5;} // Barrel A
  return -1.;
}

float RecoPhysPlots::EtaRegionCoarse(double eta)
{

  if(eta < -2.50) {return -1.;} // underflow
  if(eta < -1.05) {return 0.5;} // EC
  if(eta <  0.00) {return 1.5;} // BC
  if(eta <  1.05) {return 2.5;} // BA
  if(eta <  2.50) {return 3.5;} // BC
  return 4.5; // Overflow

}
bool RecoPhysPlots::LabelLargeEtaRegions(TAxis * axis)
{
  axis->SetBinLabel(1, "CSC C");
  axis->SetBinLabel(2, "EC C");
  axis->SetBinLabel(3, "TRANS C");
  axis->SetBinLabel(4, "BARR C");
  axis->SetBinLabel(5, "BARR A");
  axis->SetBinLabel(6, "TRANS A");
  axis->SetBinLabel(7, "EC A");
  axis->SetBinLabel(8," CSC A");
  axis->SetTitle("Detector #eta Regions");

  return true;
}

bool RecoPhysPlots::LabelCombinedEtaRegions(TAxis * axis)
{
  axis->SetBinLabel( 1, "EC-EC");
  axis->SetBinLabel( 2, "EC-BC");
  axis->SetBinLabel( 3, "EC-BA");
  axis->SetBinLabel( 4, "EC-EA");
  axis->SetBinLabel( 5, "BC-EC");
  axis->SetBinLabel( 6, "BC-BC");
  axis->SetBinLabel( 7, "BC-BA");
  axis->SetBinLabel( 8, "BC-EA");
  axis->SetBinLabel( 9, "BA-EC");
  axis->SetBinLabel(10, "BA-BC");
  axis->SetBinLabel(11, "BA-BA");
  axis->SetBinLabel(12, "BA-EA");
  axis->SetBinLabel(13, "EA-EC");
  axis->SetBinLabel(14, "EA-BC");
  axis->SetBinLabel(15, "EA-BA");
  axis->SetBinLabel(16, "EA-EA");

  axis->SetTitle("#eta Region Permutations [+#mu, -#mu]");

  return true;
}

bool RecoPhysPlots::LabelSectorAxis(TAxis * axis){

  axis->SetTitle("#phi Sectors 1-16");
  if(axis->GetNbins() == 16) axis->CenterLabels(kTRUE);

  // Sector Axis Label Error, not a multiple of 16 bins
  if(axis->GetNbins()%16){
    return false;
  }

  int nSectorDevisions = axis->GetNbins()/16;
  int skip_counter     = 1;

  axis->SetNdivisions(16,nSectorDevisions,0,kFALSE);


  for (int i = 1; i <= axis->GetNbins(); i++) {

    if (skip_counter == nSectorDevisions) {
      axis->SetBinLabel(i,Form("%d",i/nSectorDevisions));
      skip_counter = 1;
    }else {
      axis->SetBinLabel(i," ");
      skip_counter++;
    }

  }

  return true;
}

void RecoPhysPlots::SetResultsBin(int iBin, TH1* pInputHist){

    m_M_Mean-> SetBinContent(iBin, pInputHist->GetMean(1));
    m_M_Mean-> SetBinError(  iBin, pInputHist->GetMeanError(1));
    m_M_Sigma->SetBinContent(iBin, pInputHist->GetRMS(1));

}



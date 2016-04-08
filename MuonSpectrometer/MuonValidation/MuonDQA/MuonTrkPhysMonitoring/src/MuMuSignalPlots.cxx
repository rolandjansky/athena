/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// #include "MuonTrkPhysMonitoring/MuMuSignalPlots.h"
// #include "MuonTrkPhysMonitoring/MuonTrkPhysMonUtils.h"

// using namespace MuonTrkPhysMonUtils;

// MuMuSignalPlots::MuMuSignalPlots(PlotBase *pParent, std::string sDir, std::string recObj):PlotBase(pParent, sDir),
//                                                                 m_Mass(0),
//                                                                 m_N_quality(0),
//                                                                 m_N_author(0),
//                                                                 m_occupancy(0),
//                                                                 m_Efficiency(0),
//                                                                 m_EffNumerator(0),
//                                                                 m_EffDenominator(0),
//                                                                 m_Efficiency_eta(0),
//                                                                 m_EffNumerator_eta(0),
//                                                                 m_EffDenominator_eta(0),
//                                                                 m_M_Mean(0),
//                                                                 m_M_Sigma(0),
//                                                                 m_N_Lumi(0),
//                                                                 m_M_EA_EA(0),
//                                                                 m_M_EA_BA(0),
//                                                                 m_M_EA_BC(0),
//                                                                 m_M_EA_EC(0),
//                                                                 m_M_BA_EA(0),
//                                                                 m_M_BA_BA(0),
//                                                                 m_M_BA_BC(0),
//                                                                 m_M_BA_EC(0),
//                                                                 m_M_BC_EA(0),
//                                                                 m_M_BC_BA(0),
//                                                                 m_M_BC_BC(0),
//                                                                 m_M_BC_EC(0),
//                                                                 m_M_EC_EA(0),
//                                                                 m_M_EC_BA(0),
//                                                                 m_M_EC_BC(0),
//                                                                 m_M_EC_EC(0),
//                                                                 type(recObj)
// {}

// void MuMuSignalPlots::initializePlots(){

//     std::string Zsig("Z");
//     std::string Jsig("Jpsi");
//     std::string name;
//     bool doZ = false; bool doJ = false;

//     if (!type.compare(Zsig)) { name="Z"; doZ = true; }
//     else if (!type.compare(Jsig)) { name="J/#psi"; doJ = true; }
//     else { name="other"; }

//     int nBins = 100;
//     double minBin = 0.0;
//     double maxBin = 1000.0;

//     /// Error check for doZ and doJ both true here;

//     if (doZ) {
//       nBins = 100;
//       minBin = 60.0;
//       maxBin = 120.0;
//     }
//     else if (doJ) {
//       nBins = 60;
//       minBin = 0.0;
//       maxBin = 6.0;
//     }

//     m_Mass      = Book1D("m_"+type+"_Mass",      "m_Mass: M_{"+name+"} Mass;Mass [GeV];",              nBins, minBin, maxBin);

//     m_N_quality     = Book1D("m_"+type+"_N_Quality",     "m_N: N_{"+name+"};Muon Quality;",              4, -0.5, 4.5);
//     m_N_author      = Book1D("m_"+type+"_N_Author",      "m_N: N_{"+name+"};Muon Author;",              10,    1,  11);

//     m_occupancy = Book1D("m_"+type+"_occupancy", "m_occupancy: M_{"+name+"} Track Occupancy;;N_{#mu}",   8,  0.0,   8.0);

//     m_Efficiency     = Book2D("m_"+type+"_Efficiency",     "m_Efficiency: M_{"+name+"} Efficiency",                 8, 0.0, 8.0, 16, 1.0, 17.0);    
//     m_EffNumerator   = Book2D("m_"+type+"_EffNumerator",   "m_EffNumerator: M_{"+name+"} Efficiency Numerator",     8, 0.0, 8.0, 16, 1.0, 17.0);    
//     m_EffDenominator = Book2D("m_"+type+"_EffDenominator", "m_EffDenominator: M_{"+name+"} Efficiency Denominator", 8, 0.0, 8.0, 16, 1.0, 17.0);
    
//     m_Efficiency_eta     = Book1D("m_"+type+"_Efficiency_eta",     "m_Efficiency_eta: M_{"+name+"} Efficiency;#eta",                 20, -2.5, 2.5);
//     m_EffNumerator_eta   = Book1D("m_"+type+"_EffNumerator_eta",   "m_EffNumerator_eta: M_{"+name+"} Efficiency Numerator;#eta",     20, -2.5, 2.5);
//     m_EffDenominator_eta = Book1D("m_"+type+"_EffDenominator_eta", "m_EffDenominator_eta: M_{"+name+"} Efficiency Denominator;#eta", 20, -2.5, 2.5);
    
//     m_M_EA_EA = Book1D("m_"+type+"_M_EA_EA", "m_M_EA_EAM_{"+name+"} EA_{+} - EA_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
//     m_M_EA_BA = Book1D("m_"+type+"_M_EA_BA", "m_M_EA_BAM_{"+name+"} EA_{+} - BA_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
//     m_M_EA_BC = Book1D("m_"+type+"_M_EA_BC", "m_M_EA_BCM_{"+name+"} EA_{+} - BC_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
//     m_M_EA_EC = Book1D("m_"+type+"_M_EA_EC", "m_M_EA_ECM_{"+name+"} EA_{+} - EC_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
//     m_M_BA_EA = Book1D("m_"+type+"_M_BA_EA", "m_M_BA_EAM_{"+name+"} BA_{+} - EA_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
//     m_M_BA_BA = Book1D("m_"+type+"_M_BA_BA", "m_M_BA_BAM_{"+name+"} BA_{+} - BA_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
//     m_M_BA_BC = Book1D("m_"+type+"_M_BA_BC", "m_M_BA_BCM_{"+name+"} BA_{+} - BC_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
//     m_M_BA_EC = Book1D("m_"+type+"_M_BA_EC", "m_M_BA_ECM_{"+name+"} BA_{+} - EC_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
//     m_M_BC_EA = Book1D("m_"+type+"_M_BC_EA", "m_M_BC_EAM_{"+name+"} BC_{+} - EA_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
//     m_M_BC_BA = Book1D("m_"+type+"_M_BC_BA", "m_M_BC_BAM_{"+name+"} BC_{+} - BA_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
//     m_M_BC_BC = Book1D("m_"+type+"_M_BC_BC", "m_M_BC_BCM_{"+name+"} BC_{+} - BC_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
//     m_M_BC_EC = Book1D("m_"+type+"_M_BC_EC", "m_M_BC_ECM_{"+name+"} BC_{+} - EC_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
//     m_M_EC_EA = Book1D("m_"+type+"_M_EC_EA", "m_M_EC_EAM_{"+name+"} EC_{+} - EA_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
//     m_M_EC_BA = Book1D("m_"+type+"_M_EC_BA", "m_M_EC_BAM_{"+name+"} EC_{+} - BA_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
//     m_M_EC_BC = Book1D("m_"+type+"_M_EC_BC", "m_M_EC_BCM_{"+name+"} EC_{+} - BC_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
//     m_M_EC_EC = Book1D("m_"+type+"_M_EC_EC", "m_M_EC_ECM_{"+name+"} EC_{+} - EC_{-}; M_{"+name+"}[GeV];", nBins, minBin, maxBin);
    
//     m_M_Mean  = Book1D("m_"+type+"_M_Mean",  "m_M_Mean: M_{"+name+"} Mean;;M_{"+name+"}[GeV]",             16, 1.0, 17.0);
//     m_M_Sigma = Book1D("m_"+type+"_M_Sigma", "m_M_Sigma: #sigma(M_{"+name+"});;#sigma(M_{"+name+"})[GeV]", 16, 1.0, 17.0);
//     m_N_Lumi  = Book1D("m_"+type+"_N_Lumi",  "m_N_Lumi: N_{#mu} from "+name+" Decay;;N_{#mu}",      16, 1.0, 17.0);  
// }

// void MuMuSignalPlots::fill(const float eta_mu_plus, const float eta_mu_minus, const float invariant_mass){

//     float etaRegionCoarsePlus  = EtaRegionCoarse(eta_mu_plus);
//     float etaRegionCoarseMinus = EtaRegionCoarse(eta_mu_minus);
//     float etaRegionFinePlus    = EtaRegionFine(eta_mu_plus);
//     float etaRegionFineMinus   = EtaRegionFine(eta_mu_minus);
  
//     if((etaRegionCoarsePlus == 3.5 && etaRegionCoarseMinus == 3.5)) m_M_EA_EA->Fill(invariant_mass/1000.0);
//     if((etaRegionCoarsePlus == 3.5 && etaRegionCoarseMinus == 2.5)) m_M_EA_BA->Fill(invariant_mass/1000.0);
//     if((etaRegionCoarsePlus == 3.5 && etaRegionCoarseMinus == 1.5)) m_M_EA_BC->Fill(invariant_mass/1000.0);
//     if((etaRegionCoarsePlus == 3.5 && etaRegionCoarseMinus == 0.5)) m_M_EA_EC->Fill(invariant_mass/1000.0);
                
//     if((etaRegionCoarsePlus == 2.5 && etaRegionCoarseMinus == 3.5)) m_M_BA_EA->Fill(invariant_mass/1000.0);
//     if((etaRegionCoarsePlus == 2.5 && etaRegionCoarseMinus == 2.5)) m_M_BA_BA->Fill(invariant_mass/1000.0);
//     if((etaRegionCoarsePlus == 2.5 && etaRegionCoarseMinus == 1.5)) m_M_BA_BC->Fill(invariant_mass/1000.0);
//     if((etaRegionCoarsePlus == 2.5 && etaRegionCoarseMinus == 0.5)) m_M_BA_EC->Fill(invariant_mass/1000.0);
                
//     if((etaRegionCoarsePlus == 1.5 && etaRegionCoarseMinus == 3.5)) m_M_BC_EA->Fill(invariant_mass/1000.0);
//     if((etaRegionCoarsePlus == 1.5 && etaRegionCoarseMinus == 2.5)) m_M_BC_BA->Fill(invariant_mass/1000.0);
//     if((etaRegionCoarsePlus == 1.5 && etaRegionCoarseMinus == 1.5)) m_M_BC_BC->Fill(invariant_mass/1000.0);
//     if((etaRegionCoarsePlus == 1.5 && etaRegionCoarseMinus == 0.5)) m_M_BC_EC->Fill(invariant_mass/1000.0);
                
//     if((etaRegionCoarsePlus == 0.5 && etaRegionCoarseMinus == 3.5)) m_M_EC_EA->Fill(invariant_mass/1000.0);
//     if((etaRegionCoarsePlus == 0.5 && etaRegionCoarseMinus == 2.5)) m_M_EC_BA->Fill(invariant_mass/1000.0);
//     if((etaRegionCoarsePlus == 0.5 && etaRegionCoarseMinus == 1.5)) m_M_EC_BC->Fill(invariant_mass/1000.0);
//     if((etaRegionCoarsePlus == 0.5 && etaRegionCoarseMinus == 0.5)) m_M_EC_EC->Fill(invariant_mass/1000.0);

//     m_Mass->Fill(invariant_mass/1000.0);
//     m_occupancy->Fill(etaRegionFinePlus);
//     m_occupancy->Fill(etaRegionFineMinus);

// }

// void MuMuSignalPlots::fill(xAOD::Muon::Quality qual_plus, xAOD::Muon::Quality qual_minus){

//     float val_plus  = 3.0;
//     float val_minus = 3.0;

//     // Tight, Medium, Loose, VeryLoose == 0, 1, 2, 3

//     if      (qual_plus <= xAOD::Muon::Tight)      { val_plus = 0.0; }
//     else if (qual_plus <= xAOD::Muon::Medium)     { val_plus = 1.0; }
//     else if (qual_plus <= xAOD::Muon::Loose)      { val_plus = 2.0; }
//     else if (qual_plus <= xAOD::Muon::VeryLoose)  { val_plus = 3.0; }

//     if      (qual_minus <= xAOD::Muon::Tight)     { val_minus = 0.0; }
//     else if (qual_minus <= xAOD::Muon::Medium)    { val_minus = 1.0; }
//     else if (qual_minus <= xAOD::Muon::Loose)     { val_minus = 2.0; }
//     else if (qual_minus <= xAOD::Muon::VeryLoose) { val_minus = 3.0; }

//     m_N_quality->Fill(val_plus);
//     m_N_quality->Fill(val_minus);

// }

// void MuMuSignalPlots::fill(xAOD::Muon::Author auth_plus, xAOD::Muon::Author auth_minus){

//     unsigned int binPlus = 0;
//     unsigned int binMinus = 0;

//     if      ( auth_plus == xAOD::Muon::MuidCo )              { binPlus = 1; }
//     else if ( auth_plus == xAOD::Muon::STACO )               { binPlus = 2; }
//     else if ( auth_plus == xAOD::Muon::MuTag )               { binPlus = 3; }
//     else if ( auth_plus == xAOD::Muon::MuTagIMO )            { binPlus = 4; }
//     else if ( auth_plus == xAOD::Muon::MuidSA )              { binPlus = 5; }
//     else if ( auth_plus == xAOD::Muon::MuGirl )              { binPlus = 6; }
//     else if ( auth_plus == xAOD::Muon::MuGirlLowBeta )       { binPlus = 7; }
//     else if ( auth_plus == xAOD::Muon::CaloTag )             { binPlus = 8; }
//     else if ( auth_plus == xAOD::Muon::CaloLikelihood )      { binPlus = 9; }
//     else if ( auth_plus == xAOD::Muon::ExtrapolateMuonToIP ) { binPlus = 10; }

//     if      ( auth_minus == xAOD::Muon::MuidCo )              { binMinus = 1; }
//     else if ( auth_minus == xAOD::Muon::STACO )               { binMinus = 2; }
//     else if ( auth_minus == xAOD::Muon::MuTag )               { binMinus = 3; }
//     else if ( auth_minus == xAOD::Muon::MuTagIMO )            { binMinus = 4; }
//     else if ( auth_minus == xAOD::Muon::MuidSA )              { binMinus = 5; }
//     else if ( auth_minus == xAOD::Muon::MuGirl )              { binMinus = 6; }
//     else if ( auth_minus == xAOD::Muon::MuGirlLowBeta )       { binMinus = 7; }
//     else if ( auth_minus == xAOD::Muon::CaloTag )             { binMinus = 8; }
//     else if ( auth_minus == xAOD::Muon::CaloLikelihood )      { binMinus = 9; }
//     else if ( auth_minus == xAOD::Muon::ExtrapolateMuonToIP ) { binMinus = 10; }

//     m_N_author->Fill(binPlus);
//     m_N_author->Fill(binMinus);

// }

// void MuMuSignalPlots::finalizePlots(){

//     LabelQuality(m_N_quality->GetXaxis());
//     LabelAuthor(m_N_author->GetXaxis());

//     LabelCombinedEtaRegions(m_M_Mean->GetXaxis());
//     LabelCombinedEtaRegions(m_M_Sigma->GetXaxis());
//     LabelCombinedEtaRegions(m_N_Lumi->GetXaxis());
    
//     LabelLargeEtaRegions(m_occupancy->GetXaxis());
//     LabelLargeEtaRegions(m_Efficiency->GetXaxis());
//     LabelLargeEtaRegions(m_EffNumerator->GetXaxis());
//     LabelLargeEtaRegions(m_EffDenominator->GetXaxis());
    
//     LabelSectorAxis(m_Efficiency->GetYaxis());
//     LabelSectorAxis(m_EffNumerator->GetYaxis());
//     LabelSectorAxis(m_EffDenominator->GetYaxis());
    
//     for(int bin = 1; bin < m_EffNumerator->GetXaxis()->GetNbins(); bin++){
//         if(m_EffDenominator->GetBinContent(bin) > 0){
//             m_Efficiency->SetBinContent(bin, (m_EffNumerator->GetBinContent(bin)) / (m_EffDenominator->GetBinContent(bin)));
//         }
//     }
    
//     for(int bin = 1; bin < m_Efficiency_eta->GetXaxis()->GetNbins(); bin++){
//         if(m_EffDenominator_eta->GetBinContent(bin) > 0){
//             m_Efficiency_eta->SetBinContent(bin, (m_EffNumerator_eta->GetBinContent(bin)) / (m_EffDenominator_eta->GetBinContent(bin)));
//         }
//     }

//     SetResultsBin( 1, m_M_EC_EC);
//     SetResultsBin( 2, m_M_EC_BC);
//     SetResultsBin( 3, m_M_EC_BA);
//     SetResultsBin( 4, m_M_EC_EA);
//     SetResultsBin( 5, m_M_BC_EC);
//     SetResultsBin( 6, m_M_BC_BC);
//     SetResultsBin( 7, m_M_BC_BA);
//     SetResultsBin( 8, m_M_BC_EA);
//     SetResultsBin( 9, m_M_BA_EC);
//     SetResultsBin(10, m_M_BA_BC);
//     SetResultsBin(11, m_M_BA_BA);
//     SetResultsBin(12, m_M_BA_EA);
//     SetResultsBin(13, m_M_EA_EC);
//     SetResultsBin(14, m_M_EA_BC);
//     SetResultsBin(15, m_M_EA_BA);
//     SetResultsBin(16, m_M_EA_EA);
// }

// void MuMuSignalPlots::SetResultsBin(int iBin, TH1* pInputHist){

//     m_M_Mean-> SetBinContent(iBin, pInputHist->GetMean(1));
//     m_M_Mean-> SetBinError(  iBin, pInputHist->GetMeanError(1));
//     m_M_Sigma->SetBinContent(iBin, pInputHist->GetRMS(1));
//     m_N_Lumi-> SetBinContent(iBin, pInputHist->GetEntries());

// }

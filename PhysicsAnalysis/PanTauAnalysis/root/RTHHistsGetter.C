/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>

#include "/afs/cern.ch/user/l/limbach/ATLAS-Style/AtlasUtils.C"
#include "/afs/cern.ch/user/l/limbach/ATLAS-Style/AtlasLabels.C"

//for easy usage, make input and outputfile global constants...

// const TString   g_InFileName    = "PanTau_RunTimeHists_MC12_IsolatedIn12_5k.root";
const TString   g_InFileName    = "PanTauHists.root";

const TString   g_OutFileName   = "OrderedHists_" + g_InFileName;


//constant names in the inputfile
TFile*          g_InFile        = 0;
const TString   g_TopLevelDir   = "RunTimeHistograms";
const TString   g_EFO           = "EFOVariables";
const TString   g_Event         = "EventVariables";
const TString   g_Tau           = "TauVariables";


//main histo access function
TH1F* getHist(TString histName, TString Type);

//helpers to easily get histo from inputfile
TH1F* getEFOHist(TString histName);
TH1F* getTauHist(TString histName);
TH1F* getEventHist(TString histName);

TCanvas*                singleEFOTypePlot(TString varName, double xMin = 0, double xMax = -1);
TCanvas*                singleTauPlot(TString varName, double xMax = -1);

void                    getBinValueTimesEntries(TH1F* hist, TCanvas* targetCan, double yPos);

void                    storeCanvasList(std::vector<TCanvas*> canList, TFile* outfile);

std::string             numToString(double number);


void RTHHistsGetter() {
    gROOT->LoadMacro("/afs/cern.ch/user/l/limbach/ATLAS-Style/AtlasStyle.C");
    SetAtlasStyle();
    
    TFile* InputFile = TFile::Open(g_InFileName);
//     TString InFileName  = "PanTau_RunTimeHists_macrotest.root";
//     TString OutFileName = "OrderedHists_" + InFileName;

    g_InFile = TFile::Open(g_InFileName);
    TFile* OutputFile = new TFile(g_OutFileName, "RECREATE");
    
    
    //! ==============================================================
    //! Plots for EFO Types and usage
    //! ==============================================================
    
    /// Overview plot for EFO types
    TCanvas*    EFO_Usage_Overview_C = new TCanvas("EFO_Usage_Overview", "EFO_Usage_Overview");
    TH1F*       EFO_Usage_Overview_H = getEFOHist("(E)Usage_EFOIsUsedXTimes");
    EFO_Usage_Overview_H->GetXaxis()->SetBinLabel(2, "Vetoed");
    EFO_Usage_Overview_H->GetXaxis()->SetBinLabel(3, "Unused");
    EFO_Usage_Overview_H->GetXaxis()->SetBinLabel(4, "Once");
    EFO_Usage_Overview_H->GetXaxis()->SetBinLabel(5, "Twice");
    EFO_Usage_Overview_H->GetXaxis()->SetBinLabel(6, "Three times");
    EFO_Usage_Overview_H->GetXaxis()->SetBinLabel(7, "Four times");
    EFO_Usage_Overview_H->GetXaxis()->SetTitle("EFO Usage");
    EFO_Usage_Overview_H->Draw("text");
    EFO_Usage_Overview_H->GetYaxis()->SetRangeUser(0, EFO_Usage_Overview_H->GetMaximum()*1.5);
    EFO_Usage_Overview_C->Write();
    
    /// Overlay plots of 4 momentum and deltaR of different EFO Types
    std::vector<TCanvas*>   EFOTypesOverlay;
    EFOTypesOverlay.push_back(singleEFOTypePlot("E"));
    EFOTypesOverlay.push_back(singleEFOTypePlot("E", -1000, 10000));
    EFOTypesOverlay.push_back(singleEFOTypePlot("Et"));
    EFOTypesOverlay.push_back(singleEFOTypePlot("Et", -1000, 10000));
    EFOTypesOverlay.push_back(singleEFOTypePlot("Eta", -6, 6));
    EFOTypesOverlay.push_back(singleEFOTypePlot("Phi", -4, 4));
    EFOTypesOverlay.push_back(singleEFOTypePlot("MinDrTau", 0.0, 3.));
    EFOTypesOverlay.push_back(singleEFOTypePlot("MinDrTau_ChrgEFO", 0.0, 1.));
    EFOTypesOverlay.push_back(singleEFOTypePlot("MinDrTau_NeutEFO", 0.0, 1.));
    EFOTypesOverlay.push_back(singleEFOTypePlot("Charge", -5, 5));
    storeCanvasList(EFOTypesOverlay, OutputFile);
    
    
    
    //! ==============================================================
    //! Plots for Tau properties
    //! ==============================================================
    
    /// Plot for max delta R of tau-associated objects to tau
    TCanvas*    Tau_ClusTrak_DeltaRMax_C = new TCanvas("Tau_ClusTrak_DeltaRMaxToTau", "Tau_ClusTrak_DeltaRMaxToTau");
    Tau_ClusTrak_DeltaRMax_C->cd();
    
//     TH1F*       Tau_ClusTrak_DeltaRMax_TrakTau_H = getTauHist("(T)InSeedTrak_DeltaRMaxTauSeedTrackToTauSeed");
//     
//     Tau_ClusTrak_DeltaRMax_TrakTau_H->GetXaxis()->SetRangeUser(0.0, 1.0);
//     Tau_ClusTrak_DeltaRMax_TrakTau_H->SetLineColor(kGreen+2);
//     Tau_ClusTrak_DeltaRMax_TrakTau_H->Draw();
//     
//     TH1F*       Tau_ClusTrak_DeltaRMax_TrakJet_H = getTauHist("(T)InSeedTrak_DeltaRMaxTauSeedTrackToTauJet");
//     Tau_ClusTrak_DeltaRMax_TrakJet_H->GetXaxis()->SetRangeUser(0.0, 1.0);
//     Tau_ClusTrak_DeltaRMax_TrakJet_H->SetLineColor(kYellow+2);
//     Tau_ClusTrak_DeltaRMax_TrakJet_H->Draw("same");
//     
//     TH1F*       Tau_ClusTrak_DeltaRMax_ClusJet_H = getTauHist("(T)InSeedClus_DeltaRMaxTauJetClusterToTauJet");
//     Tau_ClusTrak_DeltaRMax_ClusJet_H->GetXaxis()->SetRangeUser(0., 1.);
//     Tau_ClusTrak_DeltaRMax_ClusJet_H->SetLineColor(kBlue);
//     Tau_ClusTrak_DeltaRMax_ClusJet_H->Draw("same");
    
    TH1F*       Tau_ClusTrak_DeltaRMax_ClusTau_H = getTauHist("(T)InSeedClus_DeltaRMaxTauJetClusterToTauSeed");
    Tau_ClusTrak_DeltaRMax_ClusTau_H->GetXaxis()->SetTitle("#Delta R_{max}(associated clusters and tau)");
    Tau_ClusTrak_DeltaRMax_ClusTau_H->SetLineColor(kRed);
    Tau_ClusTrak_DeltaRMax_ClusTau_H->Draw();
    
    TLegend*    Tau_ClusTrak_DeltaRMax_L = new TLegend(0.5, 0.7, 0.90, 0.93);
    Tau_ClusTrak_DeltaRMax_L->SetFillColor(0);
    Tau_ClusTrak_DeltaRMax_L->SetFillStyle(0);
    Tau_ClusTrak_DeltaRMax_L->SetBorderSize(0);
//     Tau_ClusTrak_DeltaRMax_L->AddEntry(Tau_ClusTrak_DeltaRMax_ClusJet_H,       "#Delta R(Assoc. Clusters, #tau_{Jet})", "l");
    Tau_ClusTrak_DeltaRMax_L->AddEntry(Tau_ClusTrak_DeltaRMax_ClusTau_H,       "#Delta R(Assoc. Clusters, #tau_{HLV})", "l");
//     Tau_ClusTrak_DeltaRMax_L->AddEntry(Tau_ClusTrak_DeltaRMax_TrakJet_H,       "#Delta R(Assoc. Tracks, #tau_{Jet})", "l");
//     Tau_ClusTrak_DeltaRMax_L->AddEntry(Tau_ClusTrak_DeltaRMax_TrakTau_H,       "#Delta R(Assoc. Tracks, #tau_{HLV})", "l");
    Tau_ClusTrak_DeltaRMax_L->Draw();
    
    Tau_ClusTrak_DeltaRMax_C->Write();
    
    
    
    
    
    /// Plot for number of tau-associated objects out of collection region
    TCanvas*    Tau_ClusTrak_NOutsideCone_C = new TCanvas("Tau_ClusTrak_NOutsideCollectionCone", "Tau_ClusTrak_NOutsideCollectionCone");
    Tau_ClusTrak_NOutsideCone_C->cd();
    
//     TH1F*       Tau_ClusTrak_NOutsideCone_ClusJet_H = getTauHist("(T)InSeedClus_NClusterOutsideCollectionConeWrtJet");
//     Tau_ClusTrak_NOutsideCone_ClusJet_H->SetLineColor(kBlue);
    
    TH1F*       Tau_ClusTrak_NOutsideCone_ClusTau_H = getTauHist("(T)InSeedClus_NClusterOutsideCollectionConeWrtSeed");
    Tau_ClusTrak_NOutsideCone_ClusTau_H->SetLineColor(kRed);
    
//     TH1F*       Tau_ClusTrak_NOutsideCone_TrakTau_H = getTauHist("(T)InSeedTrak_NTracksOutsideCollectionConeWrtSeed");
//     Tau_ClusTrak_NOutsideCone_TrakTau_H->SetLineColor(kGreen+2);
    
//     TH1F*       Tau_ClusTrak_NOutsideCone_TrakJet_H = getTauHist("(T)InSeedTrak_NTracksOutsideCollectionConeWrtJet");
//     Tau_ClusTrak_NOutsideCone_TrakJet_H->SetLineColor(kYellow+2);
    
    Tau_ClusTrak_NOutsideCone_ClusTau_H->GetXaxis()->SetTitle("N(tracks/cluster) outside collection cone (of #Delta R = 0.4)");
    Tau_ClusTrak_NOutsideCone_ClusTau_H->GetXaxis()->SetRangeUser(0., 10.);
    Tau_ClusTrak_NOutsideCone_ClusTau_H->Draw();
//     Tau_ClusTrak_NOutsideCone_TrakJet_H->Draw("same");
//     Tau_ClusTrak_NOutsideCone_ClusJet_H->Draw("same");
//     Tau_ClusTrak_NOutsideCone_ClusTau_H->Draw("same");
    
    TLegend*    Tau_ClusTrak_NOutsideCone_L = new TLegend(0.4, 0.7, 0.90, 0.93);
    Tau_ClusTrak_NOutsideCone_L->SetFillColor(0);
    Tau_ClusTrak_NOutsideCone_L->SetFillStyle(0);
    Tau_ClusTrak_NOutsideCone_L->SetBorderSize(0);
//     Tau_ClusTrak_NOutsideCone_L->AddEntry(Tau_ClusTrak_NOutsideCone_ClusJet_H,       "N(cluster) out of 0.4-Cone wrt. #tau_{Jet}", "l");
    Tau_ClusTrak_NOutsideCone_L->AddEntry(Tau_ClusTrak_NOutsideCone_ClusTau_H,       "N(cluster) out of 0.4-Cone wrt. #tau_{HLV}", "l");
//     Tau_ClusTrak_NOutsideCone_L->AddEntry(Tau_ClusTrak_NOutsideCone_TrakJet_H,       "N(tracks) out of 0.4-Cone wrt. #tau_{Jet}", "l");
//     Tau_ClusTrak_NOutsideCone_L->AddEntry(Tau_ClusTrak_NOutsideCone_TrakTau_H,       "N(tracks) out of 0.4-Cone wrt. #tau_{HLV}", "l");
    Tau_ClusTrak_NOutsideCone_L->Draw();
    
    //also calculate bin_xValue times bin_entries in this case, to get number of tracks / clusters outside collection cone
//     getBinValueTimesEntries(Tau_ClusTrak_NOutsideCone_ClusJet_H, Tau_ClusTrak_NOutsideCone_C, 0.60);
    getBinValueTimesEntries(Tau_ClusTrak_NOutsideCone_ClusTau_H, Tau_ClusTrak_NOutsideCone_C, 0.55);
//     getBinValueTimesEntries(Tau_ClusTrak_NOutsideCone_TrakJet_H, Tau_ClusTrak_NOutsideCone_C, 0.50);
//     getBinValueTimesEntries(Tau_ClusTrak_NOutsideCone_TrakTau_H, Tau_ClusTrak_NOutsideCone_C, 0.45);
    
    Tau_ClusTrak_NOutsideCone_C->Write();
    
    
    /// Plot for number of tau-associated objects out of collection region
//     TCanvas*    Tau_ClusTrak_NValidOutsideCone_C = new TCanvas("Tau_ClusTrak_NValidOutsideCollectionCone", "Tau_ClusTrak_NValidOutsideCollectionCone");
//     Tau_ClusTrak_NValidOutsideCone_C->cd();
//     
//     TH1F*       Tau_ClusTrak_NValidOutsideCone_ClusJet_H = getTauHist("(T)InSeedClus_NClusterOutsideCollectionConeWrtJet");
//     Tau_ClusTrak_NValidOutsideCone_ClusJet_H->SetLineColor(kBlue);
//     
//     TH1F*       Tau_ClusTrak_NValidOutsideCone_ClusTau_H = getTauHist("(T)InSeedClus_NClusterOutsideCollectionConeWrtSeed");
//     Tau_ClusTrak_NValidOutsideCone_ClusTau_H->SetLineColor(kRed);
//     
//     TH1F*       Tau_ClusTrak_NValidOutsideCone_TrakTau_H = getTauHist("(T)InSeedTrak_NTracksOutsideCollectionConeWrtSeed");
//     Tau_ClusTrak_NValidOutsideCone_TrakTau_H->SetLineColor(kGreen+2);
//     
//     Tau_ClusTrak_NValidOutsideCone_TrakTau_H->GetXaxis()->SetTitle("N(tracks/cluster) outside collection cone (of #Delta R = 0.4)");
//     Tau_ClusTrak_NValidOutsideCone_TrakTau_H->GetXaxis()->SetRangeUser(0., 10.);
//     Tau_ClusTrak_NValidOutsideCone_TrakTau_H->Draw();
//     Tau_ClusTrak_NValidOutsideCone_ClusTau_H->Draw("same");
//     Tau_ClusTrak_NValidOutsideCone_ClusJet_H->Draw("same");
//     
//     TLegend*    Tau_ClusTrak_NValidOutsideCone_L = new TLegend(0.4, 0.7, 0.90, 0.93);
//     Tau_ClusTrak_NValidOutsideCone_L->SetFillColor(0);
//     Tau_ClusTrak_NValidOutsideCone_L->SetFillStyle(0);
//     Tau_ClusTrak_NValidOutsideCone_L->SetBorderSize(0);
//     Tau_ClusTrak_NValidOutsideCone_L->AddEntry(Tau_ClusTrak_NValidOutsideCone_ClusJet_H,    "N(valid cluster) out of 0.4-Cone wrt. #tau_{Jet}", "l");
//     Tau_ClusTrak_NValidOutsideCone_L->AddEntry(Tau_ClusTrak_NValidOutsideCone_ClusTau_H,    "N(valid cluster) out of 0.4-Cone wrt. #tau_{HLV}", "l");
//     Tau_ClusTrak_NValidOutsideCone_L->AddEntry(Tau_ClusTrak_NValidOutsideCone_TrakTau_H,    "N(tracks) out of 0.4-Cone wrt. #tau_{HLV}", "l");
//     Tau_ClusTrak_NValidOutsideCone_L->Draw();
//     
//     //also calculate bin_xValue times bin_entries in this case, to get number of tracks / clusters outside collection cone
//     getBinValueTimesEntries(Tau_ClusTrak_NValidOutsideCone_ClusJet_H, Tau_ClusTrak_NValidOutsideCone_C, 0.60);
//     getBinValueTimesEntries(Tau_ClusTrak_NValidOutsideCone_ClusTau_H, Tau_ClusTrak_NValidOutsideCone_C, 0.55);
//     getBinValueTimesEntries(Tau_ClusTrak_NValidOutsideCone_TrakTau_H, Tau_ClusTrak_NValidOutsideCone_C, 0.50);
    
    Tau_ClusTrak_NOutsideCone_C->Write();
    
    
    /// Plot for numTrack in input seeds
    TCanvas*    Tau_NumTrack_C  = new TCanvas("Tau_NumTrack", "Tau_NumTrack");
    Tau_NumTrack_C->cd();
    
    TH1F*   Tau_NumTrack_All_H = getTauHist("(T)InSeed_numTrackAll");
    Tau_NumTrack_All_H->GetXaxis()->SetRangeUser(-0.5, 15.5);
    Tau_NumTrack_All_H->GetXaxis()->SetTitle("numTrack of tauRec tau");
    Tau_NumTrack_All_H->SetLineColor(kBlack);
    Tau_NumTrack_All_H->SetLineStyle(2);
    Tau_NumTrack_All_H->SetLineWidth(2);
    Tau_NumTrack_All_H->Draw();
    
    TH1F*   Tau_NumTrack_Val_H = getTauHist("(T)InSeed_numTrackValid");
    Tau_NumTrack_Val_H->SetLineColor(kGreen+2);
    Tau_NumTrack_Val_H->Draw("same");
    
    TLegend*    Tau_NumTrack_L = new TLegend(0.3, 0.8, 0.99, 0.93);
    Tau_NumTrack_L->SetFillColor(0);
    Tau_NumTrack_L->SetFillStyle(0);
    Tau_NumTrack_L->SetBorderSize(0);
    Tau_NumTrack_L->AddEntry(Tau_NumTrack_All_H,       "All tauRec candidates", "l");
    Tau_NumTrack_L->AddEntry(Tau_NumTrack_Val_H,       "tauRec candidates used as seed", "l");
    Tau_NumTrack_L->Draw();
    
    int nTotalTauRec    =   Tau_NumTrack_All_H->GetEntries();
    int nValidTauRec    =   Tau_NumTrack_All_H->GetBinContent(2)
                        +   Tau_NumTrack_All_H->GetBinContent(3)
                        +   Tau_NumTrack_All_H->GetBinContent(4)
                        +   Tau_NumTrack_All_H->GetBinContent(5)
                        +   Tau_NumTrack_All_H->GetBinContent(6);
    int nInvalidTauRec  =   Tau_NumTrack_All_H->GetEntries() - nValidTauRec;
    
    std::string nTotalauStr     = "Total    TauRecs: " + numToString(nTotalTauRec);
    std::string nValidTauStr    = "Valid    TauRecs: " + numToString(nValidTauRec);
    std::string nInvalidTauStr  = "Invalid TauRecs: " + numToString(nInvalidTauRec);
    
    TLatex      Tau_NumTrack_all_T;
    Tau_NumTrack_all_T.SetNDC();
    Tau_NumTrack_all_T.DrawLatex(0.5, 0.65, nTotalauStr.c_str());
    
    TLatex      Tau_NumTrack_val_T;
    Tau_NumTrack_val_T.SetNDC();
    Tau_NumTrack_val_T.DrawLatex(0.5, 0.58, nValidTauStr.c_str());
    
    TLatex      Tau_NumTrack_inv_T;
    Tau_NumTrack_inv_T.SetNDC();
    Tau_NumTrack_inv_T.DrawLatex(0.5, 0.53, nInvalidTauStr.c_str());
    
    Tau_NumTrack_C->Write();
    
    
    /// Plot for Seed is valid
    TCanvas*    Tau_SeedValid_C  = new TCanvas("Seed_IsValid", "Seed_IsValid");
    Tau_SeedValid_C->cd();
    
    TH1F*       Tau_SeedValid_H = getTauHist("(T)InSeed_IsValid");
    Tau_SeedValid_H->GetXaxis()->SetTitle("Input tauRec candidate is valid");
    Tau_SeedValid_H->GetXaxis()->SetBinLabel(2, "Invalid");
    Tau_SeedValid_H->GetXaxis()->SetBinLabel(3, "Valid");
    Tau_SeedValid_H->Draw("text");
    Tau_SeedValid_H->GetYaxis()->SetRangeUser(0., Tau_SeedValid_H->GetMaximum()*1.5);

    Tau_SeedValid_C->Write();
    
    
    /// Plot for cluster Delta R in tauRec seed
    TCanvas*    Tau_ClusterDeltaR_C  = new TCanvas("Seed_ClusterDeltaR", "Seed_ClusterDeltaR");
    Tau_ClusterDeltaR_C->cd();
    
    TH1F*       Tau_ClusterDeltaR_H = getTauHist("(T)InSeedClus_DeltaRTauJetClusterToTauSeed");
    Tau_ClusterDeltaR_H->GetXaxis()->SetTitle("#Delta R(#tau^{jet}_{cluster i}, #tau^{seed})");
    Tau_ClusterDeltaR_H->GetXaxis()->SetRangeUser(0.0, 0.7);
    Tau_ClusterDeltaR_H->Draw();
    Tau_ClusterDeltaR_H->GetYaxis()->SetRangeUser(0., Tau_ClusterDeltaR_H->GetMaximum()*1.2);
    
    double clusterIn04  = 0.;
    double clusterOut04 = 0.;
    for(int iClusBin=0; iClusBin<Tau_ClusterDeltaR_H->GetNbinsX(); iClusBin++) {
        double binCenter = Tau_ClusterDeltaR_H->GetBinCenter(iClusBin);
        double binValue  = Tau_ClusterDeltaR_H->GetBinContent(iClusBin);
        double* clusterLink = 0;
        if(binCenter<=0.4) clusterLink = &clusterIn04;
        if(binCenter>0.4) clusterLink = &clusterOut04;
        *clusterLink += binValue;
    }
    
    std::string clusterInStr    = "Cluster in  0.4: " + numToString(clusterIn04);
    std::string clusterOutStr   = "Cluster out 0.4: " + numToString(clusterOut04);
    
    TLatex clusIn;
    clusIn.SetNDC();
    clusIn.DrawLatex(0.6, 0.8, clusterInStr.c_str());
    
    TLatex clusOut;
    clusOut.SetNDC();
    clusOut.DrawLatex(0.6, 0.74, clusterOutStr.c_str());
    
    Tau_ClusterDeltaR_C->Write();
    
    
    /// Plots comparing seeds with valid and invalid numTracks
    std::vector<TCanvas*> TauSeedsOverlay;
    TauSeedsOverlay.push_back( singleTauPlot("AssociatedEFOs") );
    TauSeedsOverlay.push_back( singleTauPlot("AssociatedNeutralEFOs") );
    TauSeedsOverlay.push_back( singleTauPlot("AssociatedChargedEFOs") );
    TauSeedsOverlay.push_back( singleTauPlot("DeltaRSeedToClosestEFO_NoEFOs", 4.) );
    TauSeedsOverlay.push_back( singleTauPlot("DeltaRSeedToClosestChrgEFO_NoEFOs", 4.) );
    TauSeedsOverlay.push_back( singleTauPlot("DeltaRSeedToClosestNeutEFO_NoEFOs", 4.) );
    TauSeedsOverlay.push_back( singleTauPlot("DeltaRSeedToFarestAssocEFO", 1.) );
    TauSeedsOverlay.push_back( singleTauPlot("DeltaRSeedToFarestAssocChrgEFO", 1.) );
    TauSeedsOverlay.push_back( singleTauPlot("DeltaRSeedToFarestAssocNeutEFO", 1.) );
    storeCanvasList(TauSeedsOverlay, OutputFile);
    
    
    //! ==============================================================
    //! Plots for Event properties
    //! ==============================================================
    
    
    /// EFO Multiplicities
//     TCanvas*    Event_EFO_Multiplicity_C  =   new TCanvas("Event_EFO_Multiplicity", "Event_EFO_Multiplicity");
//     Event_EFO_Multiplicity_C->cd();
//     
//     double Event_EFO_Multiplicity_yMax = 0;
//     TH1F*   Event_EFO_Multiplicity_Cont = getEventHist("(G)Number_EFOsInEFOContainer");
//     Event_EFO_Multiplicity_Cont->GetXaxis()->SetRangeUser(0.0, 100.);
//     Event_EFO_Multiplicity_Cont->GetXaxis()->SetTitle("Number of eflowObjects");
//     Event_EFO_Multiplicity_Cont->SetLineColor(kBlack);
//     Event_EFO_Multiplicity_Cont->SetLineWidth(2);
//     Event_EFO_Multiplicity_Cont->Draw();
//     
//     TH1F*   Event_EFO_Multiplicity_Core = getEventHist("(G)Number_Used_EFOs_Core");
//     Event_EFO_Multiplicity_Core->SetLineColor(kBlue-4);
//     Event_EFO_Multiplicity_Core->Draw("same");
//     
//     TH1F*   Event_EFO_Multiplicity_Sum  = getEventHist("(G)Number_Used_EFOs_Sum");
//     Event_EFO_Multiplicity_Sum->SetLineColor(kRed);
//     Event_EFO_Multiplicity_Sum->Draw("same");
//     
//     TLegend*    Event_EFO_Multiplicity_L = new TLegend(0.3, 0.8, 0.99, 0.93);
//     Event_EFO_Multiplicity_L->SetFillColor(0);
//     Event_EFO_Multiplicity_L->SetFillStyle(0);
//     Event_EFO_Multiplicity_L->SetBorderSize(0);
//     Event_EFO_Multiplicity_L->AddEntry(Event_EFO_Multiplicity_Cont, "EFOs in container", "l");
//     Event_EFO_Multiplicity_L->AddEntry(Event_EFO_Multiplicity_Core, "EFOs used in core region", "l");
//     Event_EFO_Multiplicity_L->AddEntry(Event_EFO_Multiplicity_Sum,  "EFOs used in sum region", "l");
//     Event_EFO_Multiplicity_L->Draw();
//     
//     if(Event_EFO_Multiplicity_Cont->GetMaximum() > Event_EFO_Multiplicity_yMax) Event_EFO_Multiplicity_yMax = Event_EFO_Multiplicity_Cont->GetMaximum();
//     if(Event_EFO_Multiplicity_Core->GetMaximum() > Event_EFO_Multiplicity_yMax) Event_EFO_Multiplicity_yMax = Event_EFO_Multiplicity_Core->GetMaximum();
//     if(Event_EFO_Multiplicity_Sum->GetMaximum() > Event_EFO_Multiplicity_yMax) Event_EFO_Multiplicity_yMax = Event_EFO_Multiplicity_Sum->GetMaximum();
//     Event_EFO_Multiplicity_Cont->GetYaxis()->SetRangeUser(0.0, Event_EFO_Multiplicity_yMax*1.2);
//     
//     Event_EFO_Multiplicity_C->Write();
    
    
    /// Input and output seeds
    TCanvas*    Event_InOutSeeds_C = new TCanvas("Event_InOutSeeds", "Event_InOutSeeds");
    Event_InOutSeeds_C->cd();
    
    double Event_InOutSeeds_yMax = 0.0;
    TH1F*   Event_InOutSeeds_Bad = getEventHist("(G)Number_BadSeeds");
    Event_InOutSeeds_Bad->GetXaxis()->SetTitle("Number of Seeds");
    Event_InOutSeeds_Bad->GetXaxis()->SetRangeUser(-0.5, 19.5);
    Event_InOutSeeds_Bad->SetLineColor(kRed);
    Event_InOutSeeds_Bad->SetLineWidth(1);
    Event_InOutSeeds_Bad->Draw();
    
    TH1F*   Event_InOutSeeds_Good = getEventHist("(G)Number_GoodSeeds");
    Event_InOutSeeds_Good->SetLineColor(kGreen+2);
    Event_InOutSeeds_Good->Draw("same");
    
    TH1F*   Event_InOutSeeds_Input = getEventHist("(G)Number_InputSeeds");
    Event_InOutSeeds_Input->SetLineColor(kBlue);
    Event_InOutSeeds_Input->SetLineWidth(2);
    Event_InOutSeeds_Input->SetLineStyle(1);
    Event_InOutSeeds_Input->Draw("same");
    
    TH1F*   Event_InOutSeeds_Output = getEventHist("(G)Number_OutputSeeds");
    Event_InOutSeeds_Output->SetLineColor(kMagenta);
    Event_InOutSeeds_Output->SetLineWidth(2);
    Event_InOutSeeds_Output->SetLineStyle(2);
    Event_InOutSeeds_Output->Draw("same");
    
    TLegend*    Event_InOutSeeds_L = new TLegend(0.6, 0.7, 0.99, 0.93);
    Event_InOutSeeds_L->SetFillColor(0);
    Event_InOutSeeds_L->SetFillStyle(0);
    Event_InOutSeeds_L->SetBorderSize(0);
    Event_InOutSeeds_L->AddEntry(Event_InOutSeeds_Bad,      "All bad seeds", "l");
    Event_InOutSeeds_L->AddEntry(Event_InOutSeeds_Good,     "All good seeds", "l");
    Event_InOutSeeds_L->AddEntry(Event_InOutSeeds_Input,    "Inputseeds", "l");
    Event_InOutSeeds_L->AddEntry(Event_InOutSeeds_Output,   "Outputseeds", "l");
    Event_InOutSeeds_L->Draw();
    
    //put number of bad tauRecs from track requirement
    int nBadTotal = 0;
    for(int iBin=1; iBin<Event_InOutSeeds_Bad->GetNbinsX(); iBin++) {
        double binCenter = Event_InOutSeeds_Bad->GetBinCenter(iBin);
        double binValue  = Event_InOutSeeds_Bad->GetBinContent(iBin);
        double toAdd = binCenter * binValue;
        nBadTotal += toAdd;
    }
    std::string nBadSeedsStr  = "Bad seeds in total . . . . .: " + numToString(nBadTotal);
    std::string nInvalidTauTrkStr  = "N(#tau_{tauRec}) bad numTrack: " + numToString(nInvalidTauRec);
    
    TLatex      Tau_NumTrack_all_T1;
    Tau_NumTrack_all_T.SetNDC();
    Tau_NumTrack_all_T.DrawLatex(0.4, 0.60, nBadSeedsStr.c_str());
    
    TLatex      Tau_NumTrack_all_T2;
    Tau_NumTrack_all_T.SetNDC();
    Tau_NumTrack_all_T.DrawLatex(0.4, 0.54, nInvalidTauTrkStr.c_str());
    
    if(Event_InOutSeeds_Bad->GetMaximum() > Event_InOutSeeds_yMax)
        Event_InOutSeeds_yMax = Event_InOutSeeds_Bad->GetMaximum();
    if(Event_InOutSeeds_Good->GetMaximum() > Event_InOutSeeds_yMax)
        Event_InOutSeeds_yMax = Event_InOutSeeds_Good->GetMaximum();
    if(Event_InOutSeeds_Input->GetMaximum() > Event_InOutSeeds_yMax)
        Event_InOutSeeds_yMax = Event_InOutSeeds_Input->GetMaximum();
    if(Event_InOutSeeds_Output->GetMaximum() > Event_InOutSeeds_yMax)
        Event_InOutSeeds_yMax = Event_InOutSeeds_Output->GetMaximum();
    Event_InOutSeeds_Bad->GetYaxis()->SetRangeUser(0.0, Event_InOutSeeds_yMax*1.2);
    
    Event_InOutSeeds_C->Write();
    
    
    /// Bad Seeds composition
//     TCanvas*    Event_BadSeeds_Composition_C = new TCanvas("Event_BadSeeds_Composition", "Event_BadSeeds_Composition");
//     Event_BadSeeds_Composition_C->cd();
//     
//     double Event_BadSeeds_Composition_yMax = 0.0;
//     TH1F*   Event_BadSeeds_Composition_All = getEventHist("(G)Number_BadSeeds");
//     Event_BadSeeds_Composition_All->GetXaxis()->SetTitle("Number of BadSeeds (not passed to ModeID)");
//     Event_BadSeeds_Composition_All->GetXaxis()->SetRangeUser(-0.5, 19.5);
//     Event_BadSeeds_Composition_All->SetLineColor(kBlack);
//     Event_BadSeeds_Composition_All->SetLineWidth(2);
//     Event_BadSeeds_Composition_All->SetLineStyle(2);
//     Event_BadSeeds_Composition_All->Draw();
//     
//     TH1F*   Event_BadSeeds_Composition_Presel = getEventHist("(G)Number_BadSeeds_FailPresel");
//     Event_BadSeeds_Composition_Presel->SetLineColor(kGreen+2);
//     Event_BadSeeds_Composition_Presel->Draw("same");
//     
//     TH1F*   Event_BadSeeds_Composition_Invalid = getEventHist("(G)Number_BadSeeds_Invalid");
//     Event_BadSeeds_Composition_Invalid->SetLineColor(kRed);
//     Event_BadSeeds_Composition_Invalid->Draw("same");
//     
//     TLegend*    Event_BadSeeds_Composition_L = new TLegend(0.3, 0.8, 0.99, 0.93);
//     Event_BadSeeds_Composition_L->SetFillColor(0);
//     Event_BadSeeds_Composition_L->SetFillStyle(0);
//     Event_BadSeeds_Composition_L->SetBorderSize(0);
//     Event_BadSeeds_Composition_L->AddEntry(Event_BadSeeds_Composition_All,      "All bad seeds", "l");
//     Event_BadSeeds_Composition_L->AddEntry(Event_BadSeeds_Composition_Presel,   "Bad seeds due to preselection", "l");
//     Event_BadSeeds_Composition_L->AddEntry(Event_BadSeeds_Composition_Invalid,  "Bad seeds due to technicalities", "l");
//     Event_BadSeeds_Composition_L->Draw();
//     
//     if(Event_BadSeeds_Composition_All->GetMaximum() > Event_BadSeeds_Composition_yMax)
//         Event_BadSeeds_Composition_yMax = Event_BadSeeds_Composition_All->GetMaximum();
//     if(Event_BadSeeds_Composition_Presel->GetMaximum() > Event_BadSeeds_Composition_yMax)
//         Event_BadSeeds_Composition_yMax = Event_BadSeeds_Composition_Presel->GetMaximum();
//     if(Event_BadSeeds_Composition_Invalid->GetMaximum() > Event_BadSeeds_Composition_yMax)
//         Event_BadSeeds_Composition_yMax = Event_BadSeeds_Composition_Invalid->GetMaximum();
//     Event_BadSeeds_Composition_All->GetYaxis()->SetRangeUser(0.0, Event_BadSeeds_Composition_yMax*1.2);
//     
//     Event_BadSeeds_Composition_C->Write();
    
    
    /// TauRecSeeds and AlTauRecInvalid, compared to nEvents without EFOs
    TCanvas*    Event_AllTauRecInvalid_C = new TCanvas("Event_AllTauRecInvalid", "Event_AllTauRecInvalid");
    Event_AllTauRecInvalid_C->cd();
    
    TH1F*   Event_AllTauRecInvalidAll = getEventHist("(G)Number_TausInTauContainer");
    Event_AllTauRecInvalidAll->GetXaxis()->SetTitle("Multiplicity");
    Event_AllTauRecInvalidAll->GetXaxis()->SetRangeUser(-0.5, 19.5);
    Event_AllTauRecInvalidAll->SetLineColor(kBlack);
    Event_AllTauRecInvalidAll->SetLineWidth(2);
    Event_AllTauRecInvalidAll->Draw();
    
    TH1F*   Event_AllTauRecInvalidEFO = getEventHist("(G)Number_EFOsInEFOContainer");
    Event_AllTauRecInvalidEFO->SetLineColor(kBlue-3);
    Event_AllTauRecInvalidEFO->SetLineWidth(2);
    Event_AllTauRecInvalidEFO->Draw("same");
    
    TH1F*   Event_AllTauRecInvalidInvalids = getEventHist("(G)EventsWithInvalidTauSeedsOnly");
    Event_AllTauRecInvalidInvalids->SetLineColor(kRed);
    Event_AllTauRecInvalidInvalids->SetFillColor(kRed+1);
    Event_AllTauRecInvalidInvalids->SetFillStyle(3254);
    Event_AllTauRecInvalidInvalids->Draw("same");
    
    double nWithoutEFO         = Event_AllTauRecInvalidEFO->GetBinContent(1);
    double nAllTauRecInvalid   = Event_AllTauRecInvalidInvalids->GetEntries();
    
    std::string EventsWithNoEFOs = "no EFOs . . . . . . .: " + numToString(nWithoutEFO);
    std::string EventsWithAllInv = "invalid #tau_{tauRec} only: " + numToString(nAllTauRecInvalid);
    
    TLatex Event_AllTauRecInvalid_Text;
    Event_AllTauRecInvalid_Text.SetNDC();
    Event_AllTauRecInvalid_Text.DrawLatex(0.45, 0.55, "Events with...");
    
    TLatex Event_AllTauRecInvalid_TEFOs;
    Event_AllTauRecInvalid_TEFOs.SetNDC();
    Event_AllTauRecInvalid_TEFOs.DrawLatex(0.48, 0.49, EventsWithNoEFOs.c_str());
    
    TLatex Event_AllTauRecInvalid_TTauRec;
    Event_AllTauRecInvalid_TTauRec.SetNDC();
    Event_AllTauRecInvalid_TTauRec.DrawLatex(0.48, 0.43, EventsWithAllInv.c_str());
    
    TLegend*    Event_AllTauRecInvalidL = new TLegend(0.3, 0.7, 0.99, 0.93);
    Event_AllTauRecInvalidL->SetFillColor(0);
    Event_AllTauRecInvalidL->SetFillStyle(0);
    Event_AllTauRecInvalidL->SetBorderSize(0);
    Event_AllTauRecInvalidL->AddEntry(Event_AllTauRecInvalidAll,        "Taus in TauRec Container", "l");
    Event_AllTauRecInvalidL->AddEntry(Event_AllTauRecInvalidEFO,        "EFOs in eflowRec Container", "l");
    Event_AllTauRecInvalidL->AddEntry(Event_AllTauRecInvalidInvalids,   "All Taus have bad numTrack", "fl");
    Event_AllTauRecInvalidL->Draw();
    
    double yMaxHistogram = 0.5;
    if(Event_AllTauRecInvalidAll->GetMaximum() > yMaxHistogram)
        yMaxHistogram = Event_AllTauRecInvalidAll->GetMaximum();
    if(Event_AllTauRecInvalidEFO->GetMaximum() > yMaxHistogram)
        yMaxHistogram = Event_AllTauRecInvalidEFO->GetMaximum();
    if(Event_AllTauRecInvalidInvalids->GetMaximum() > yMaxHistogram)
        yMaxHistogram = Event_AllTauRecInvalidInvalids->GetMaximum();
    Event_AllTauRecInvalidAll->GetYaxis()->SetRangeUser(0.0, yMaxHistogram*1.2);
    
    Event_AllTauRecInvalid_C->Write();
    
    
    
    /// TauRecSeeds and AlTauRecInvalid, compared to nEvents without EFOs
    TCanvas*    Event_UnusedEFOAndCluster_C = new TCanvas("Event_UnusedEFOAndCluster", "Event_UnusedEFOAndCluster");
    Event_UnusedEFOAndCluster_C->cd();
    
    TH1F*   Event_UnusedEFOAndCluster = getEventHist("(G)Usage_UnusedEFO_nOutClusters-nUnused");
    Event_UnusedEFOAndCluster->GetXaxis()->SetTitle("N(clusters outside 0.4 wrt tauSeed) - N(unused EFOs)");
    Event_UnusedEFOAndCluster->GetXaxis()->SetRangeUser(-5.5, 24.5);
    Event_UnusedEFOAndCluster->SetLineColor(kBlack);
    Event_UnusedEFOAndCluster->SetLineWidth(2);
    Event_UnusedEFOAndCluster->Draw();
    
    double nLeft        = 0.0;Event_UnusedEFOAndCluster->GetBinContent(1);
    double nMiddle      = 0.0;Event_UnusedEFOAndCluster->GetEntries();
    double nRight       = 0.0;Event_UnusedEFOAndCluster->GetEntries();
    
    for(int iBin=0; iBin<Event_UnusedEFOAndCluster->GetNbinsX(); iBin++) {
        double binCenter = Event_UnusedEFOAndCluster->GetBinCenter(iBin);
        double binValue = Event_UnusedEFOAndCluster->GetBinContent(iBin);
        
        double* counterLink = 0;
        if(binCenter < 0)   counterLink = &nLeft;
        if(binCenter == 0)  counterLink = &nMiddle;
        if(binCenter > 0)   counterLink = &nRight;
        
        *counterLink += binValue;
    }
    
    std::string nLeftStr    = "N(unused) > N(outcluster): " + numToString(nLeft);
    std::string nMiddleStr  = "N(unused) = N(outcluster): " + numToString(nMiddle);
    std::string nRightStr   = "N(unused) < N(outcluster): " + numToString(nRight);
    
    TLatex Event_UnusedEFOAndCluster_TextLeft;
    Event_UnusedEFOAndCluster_TextLeft.SetNDC();
    Event_UnusedEFOAndCluster_TextLeft.DrawLatex(0.45, 0.85, nLeftStr.c_str());
    
    TLatex Event_UnusedEFOAndCluster_TextMiddle;
    Event_UnusedEFOAndCluster_TextMiddle.SetNDC();
    Event_UnusedEFOAndCluster_TextMiddle.DrawLatex(0.45, 0.79, nMiddleStr.c_str());
    
    TLatex Event_UnusedEFOAndCluster_TextRight;
    Event_UnusedEFOAndCluster_TextRight.SetNDC();
    Event_UnusedEFOAndCluster_TextRight.DrawLatex(0.45, 0.73, nRightStr.c_str());
    
    Event_UnusedEFOAndCluster_C->Write();
    
    
    
    
    
    /// DeltaR of unused EFOs in detail
    TCanvas*    Event_UnusedEFO_DeltaRDetail_C = new TCanvas("Event_UnusedEFO_DeltaRDetail", "Event_UnusedEFO_DeltaRDetail");
    Event_UnusedEFO_DeltaRDetail_C->cd();
    
    TH1F*   Event_UnusedEFO_DeltaRDetail_All = getEventHist("(G)Usage_Unused_MinDrTau");
    Event_UnusedEFO_DeltaRDetail_All->Rebin(2);
    Event_UnusedEFO_DeltaRDetail_All->GetXaxis()->SetTitle("#Delta R_{min}(EFO_{unused}, #tau)");
    Event_UnusedEFO_DeltaRDetail_All->GetXaxis()->SetRangeUser(0.35, 0.6);
    Event_UnusedEFO_DeltaRDetail_All->SetLineColor(kBlack);
    Event_UnusedEFO_DeltaRDetail_All->SetLineWidth(2);
    Event_UnusedEFO_DeltaRDetail_All->SetLineStyle(2);
    Event_UnusedEFO_DeltaRDetail_All->Draw();
    
    TH1F*   Event_UnusedEFO_DeltaRDetail_Less = getEventHist("(G)Usage_Unused_LessUnusedThanCluster_MinDrTau");
    Event_UnusedEFO_DeltaRDetail_Less->Rebin(2);
    Event_UnusedEFO_DeltaRDetail_Less->SetLineColor(kGreen+2);
    Event_UnusedEFO_DeltaRDetail_Less->Draw("same");
    
    TH1F*   Event_UnusedEFO_DeltaRDetail_Equal = getEventHist("(G)Usage_Unused_EqualUnusedThanCluster_MinDrTau");
    Event_UnusedEFO_DeltaRDetail_Equal->Rebin(2);
    Event_UnusedEFO_DeltaRDetail_Equal->SetLineColor(kBlue-3);
    Event_UnusedEFO_DeltaRDetail_Equal->Draw("same");
    
    TH1F*   Event_UnusedEFO_DeltaRDetail_More = getEventHist("(G)Usage_Unused_MoreUnusedThanCluster_MinDrTau");
    Event_UnusedEFO_DeltaRDetail_More->Rebin(2);
    Event_UnusedEFO_DeltaRDetail_More->SetLineColor(kRed);
    Event_UnusedEFO_DeltaRDetail_More->SetFillColor(kRed);
    Event_UnusedEFO_DeltaRDetail_More->SetFillStyle(3254);
    Event_UnusedEFO_DeltaRDetail_More->Draw("same");
    
    TLegend*    Event_UnusedEFO_DeltaRDetail_L = new TLegend(0.4, 0.73, 0.99, 0.93);
    Event_UnusedEFO_DeltaRDetail_L->SetFillColor(0);
    Event_UnusedEFO_DeltaRDetail_L->SetFillStyle(0);
    Event_UnusedEFO_DeltaRDetail_L->SetBorderSize(0);
    Event_UnusedEFO_DeltaRDetail_L->AddEntry(Event_UnusedEFO_DeltaRDetail_All,      "All unused EFOs", "l");
    Event_UnusedEFO_DeltaRDetail_L->AddEntry(Event_UnusedEFO_DeltaRDetail_Less,     "N(Unused) < N(Outcluster)", "l");
    Event_UnusedEFO_DeltaRDetail_L->AddEntry(Event_UnusedEFO_DeltaRDetail_Equal,    "N(Unused) = N(Outcluster)", "l");
    Event_UnusedEFO_DeltaRDetail_L->AddEntry(Event_UnusedEFO_DeltaRDetail_More,     "N(Unused) > N(Outcluster)", "fl");
    Event_UnusedEFO_DeltaRDetail_L->Draw();
    
    Event_UnusedEFO_DeltaRDetail_All->GetYaxis()->SetRangeUser(0.001, Event_UnusedEFO_DeltaRDetail_All->GetMaximum()*20);
    
    Event_UnusedEFO_DeltaRDetail_C->SetLogy(1);
    Event_UnusedEFO_DeltaRDetail_C->Write();
    
    
    
    
    
    OutputFile->Close();
    delete OutputFile;
    
    InputFile->Close();
    delete InputFile;
    
    return;
}




//! =====================================================================
//! =====================================================================
//! =====================================================================

//!                  H E L P E R   F U N C T I O N S

//! =====================================================================
//! =====================================================================
//! =====================================================================



//main histo access function
TH1F* getHist(TString histName, TString Type) {
    TH1F* theHist = 0;
    TString pathName = g_TopLevelDir + "/" + Type + "/";
    std::cout << "(GetHist)\tTry to get histo from: " << pathName << histName << std::endl;
    theHist = (TH1F*)g_InFile->Get(pathName + histName);
    return theHist;
}


//helpers to easily get histo from inputfile
TH1F* getEFOHist(TString histName) {
    return getHist(histName, g_EFO);
}

TH1F* getTauHist(TString histName) {
    return getHist(histName, g_Tau);
}

TH1F* getEventHist(TString histName) {
    return getHist(histName, g_Event);
}


TCanvas*    singleEFOTypePlot(TString varName, double xMin, double xMax) {
    TString canvasName = varName;
    if(xMax != -1) canvasName += "_ModXAxis";
    TCanvas*    can   = new TCanvas(canvasName, canvasName);
    
    
    
    TString xAxisTitle = "";
    if(varName == "E")                  xAxisTitle = "Energy / MeV";
    if(varName == "Et")                 xAxisTitle = "Transverse energy / MeV";
    if(varName == "Eta")                xAxisTitle = "#eta";
    if(varName == "Phi")                xAxisTitle = "#phi";
    if(varName == "MinDrTau")           xAxisTitle = "#Delta R_{min}(EFO, #tau_{tauRec})";
    if(varName == "MinDrTau_ChrgEFO")   xAxisTitle = "#Delta R_{min}(EFO^{#pm}, #tau_{tauRec})";
    if(varName == "MinDrTau_NeutEFO")   xAxisTitle = "#Delta R_{min}(EFO^{0}, #tau_{tauRec})";
    if(varName == "Charge")             xAxisTitle = "Charge / q_{e}";
    
    
    
    TH1F*       hist_All        = getEFOHist("(E)AllEFOs_" + varName);
    if(xMax == -1) xMax = hist_All->GetXaxis()->GetXmax();
    hist_All->GetXaxis()->SetRangeUser(xMin, xMax);
    hist_All->GetXaxis()->SetTitle(xAxisTitle);
    hist_All->SetLineColor(kBlack);
    hist_All->SetLineWidth(2);
    hist_All->SetLineStyle(2);
    hist_All->Draw();
    
    double yMax = hist_All->GetMaximum()*1.2;
    if(varName == "Charge") yMax = hist_All->GetMaximum()*20;
    
    hist_All->GetYaxis()->SetRangeUser(0.1, yMax);
    hist_All->DrawCopy();
    
    TH1F*       hist_Vetoed     = getEFOHist("(E)Usage_VetoedEFO_" + varName);
    hist_Vetoed->SetLineWidth(1);
    hist_Vetoed->SetLineColor(kGreen+2);
    hist_Vetoed->DrawCopy("same");
    
    TH1F*       hist_Unused     = getEFOHist("(E)Usage_UnusedEFO_" + varName);
    hist_Unused->SetLineWidth(1);
    hist_Unused->SetLineColor(kRed);
    hist_Unused->DrawCopy("same");
    
    TH1F*       hist_Used       = getEFOHist("(E)Usage_UsedEFO_" + varName);
    hist_Used->SetLineWidth(1);
    hist_Used->SetLineColor(kBlue-7);
    hist_Used->DrawCopy("same");
    
    TH1F*       hist_MultiUsed  = getEFOHist("(E)Usage_MultiUsedEFO_" + varName);
    hist_MultiUsed->SetLineWidth(1);
    hist_MultiUsed->SetLineColor(kYellow-2);
    hist_MultiUsed->DrawCopy("same");
    
    if(varName == "Et" && xMax == -1) can->SetLogy(1);
    if(varName == "E" && xMax == -1) can->SetLogy(1);
    if(varName == "Charge") can->SetLogy(1);
    
    TLegend*    Legend = new TLegend(0.65, 0.6, 0.95, 0.93);
    Legend->SetFillColor(0);
    Legend->SetFillStyle(0);
    Legend->SetBorderSize(0);
    Legend->AddEntry(hist_All,          "All EFOs", "l");
    Legend->AddEntry(hist_Vetoed,       "Vetoed EFOs", "l");
    Legend->AddEntry(hist_Unused,       "Unused EFOs", "l");
    Legend->AddEntry(hist_Used,         "Used EFOs", "l");
    Legend->AddEntry(hist_MultiUsed,    "Multiused EFOs", "l");
    Legend->Draw();
    
    return can;
}



TCanvas*                singleTauPlot(TString varName, double xMax) {
    TString canvasName = varName;
    if(xMax != -1) canvasName += "_ModXAxis";
    TCanvas*    can   = new TCanvas(canvasName, canvasName);
    
    TString xAxisTitle = varName;
    if(varName == "AssociatedEFOs")                     xAxisTitle = "Number of associated EFOs in tauRec seed";
    if(varName == "AssociatedNeutralEFOs")              xAxisTitle = "Number of associated EFO^{0} in tauRec seed";
    if(varName == "AssociatedChargedEFOs")              xAxisTitle = "Number of associated EFO^{#pm} in tauRec seed";
    if(varName == "DeltaRSeedToClosestEFO_NoEFOs")      xAxisTitle = "In tauRec without EFOs: #Delta R of closest EFO";
    if(varName == "DeltaRSeedToClosestChrgEFO_NoEFOs")  xAxisTitle = "In tauRec without EFOs: #Delta R of closest EFO^{#pm}";
    if(varName == "DeltaRSeedToClosestNeutEFO_NoEFOs")  xAxisTitle = "In tauRec without EFOs: #Delta R of closest EFO^{0}";
    if(varName == "DeltaRSeedToFarestAssocEFO")         xAxisTitle = "In tauRec with EFOs: #Delta R of farthest EFO";
    if(varName == "DeltaRSeedToFarestAssocChrgEFO")     xAxisTitle = "In tauRec with EFOs: #Delta R of farthest EFO^{#pm}";
    if(varName == "DeltaRSeedToFarestAssocNeutEFO")     xAxisTitle = "In tauRec with EFOs: #Delta R of farthest EFO^{0}";
    
    
    TH1F* hist_All = getTauHist("(T)AllTauRec_" + varName);
    hist_All->GetXaxis()->SetTitle(xAxisTitle);
    if(xMax != -1) hist_All->GetXaxis()->SetRangeUser(hist_All->GetXaxis()->GetXmin(), xMax);
    
    double yMaxFactor = 1.3;
    if(varName.Contains("Associated")) yMaxFactor = 10;
    hist_All->GetYaxis()->SetRangeUser(0.1, hist_All->GetMaximum()*yMaxFactor);
    hist_All->SetLineWidth(2);
    hist_All->SetLineStyle(2);
    hist_All->SetLineColor(kBlack);
    hist_All->DrawCopy();
    
    TH1F* hist_Seed = getTauHist("(T)Seed_" + varName);
    hist_Seed->SetLineColor(kBlue);
    hist_Seed->DrawCopy("same");
    
    TH1F* hist_MultTrkTauRec = getTauHist("(T)MultTrkTauRec_" + varName);
    hist_MultTrkTauRec->SetLineColor(kRed);
    hist_MultTrkTauRec->DrawCopy("same");
    
    TLegend*    Legend = new TLegend(0.45, 0.70, 0.90, 0.93);
    Legend->SetFillColor(0);
    Legend->SetFillStyle(0);
    Legend->SetBorderSize(0);
    Legend->AddEntry(hist_All,              "#tau_{tauRec} with any numTrack", "l");
    Legend->AddEntry(hist_MultTrkTauRec,    "#tau^{bad}_{tauRec} with numTrack not in [1;5]", "l");
    Legend->AddEntry(hist_Seed,             "#tau^{good}_{tauRec} with numTrack in [1;5]", "l");
    Legend->Draw();
    
    if(varName.Contains("Associated")) can->SetLogy(1);
    
    return can;
}





void                     getBinValueTimesEntries(TH1F* hist, TCanvas* targetCan, double yPos) {
    int result = 0;
//     std::cout << "(getBinValueTimesEntries)\thist called: " << hist->GetName() << std::endl; 
    for(int iBin=1; iBin<hist->GetNbinsX(); iBin++) {
        int BinValue    = hist->GetXaxis()->GetBinCenter(iBin);
        int BinEntries  = hist->GetBinContent(iBin);
        int Product     = BinValue * BinEntries;
        result += Product;
//         std::cout << "(getBinValueTimesEntries)\t\tBin, BinCenter: Entries => Product, SumSoFar: ";
//         std::cout << iBin << ", " << BinValue << ": " << BinEntries << " => " << Product << ", " << result << std::endl;
    }
//     std::cout << "(getBinValueTimesEntries)\tReturning " << result << std::endl;
    
    targetCan->cd();
    
    std::string LatexString = "Outside 0.4: " + numToString(result);
    
    TLatex latex;
    latex.SetNDC();
    latex.SetTextColor(hist->GetLineColor());
    latex.DrawLatex(0.50, yPos, LatexString.c_str());
    
    return;
}





void    storeCanvasList(std::vector<TCanvas*> canList, TFile* outfile) {
    outfile->cd();
    for(int iCan=0; iCan<canList.size(); iCan++) canList[iCan]->Write();
    return;
}


std::string             numToString(double number) {
    std::stringstream s;
    s << number;
    return s.str();
}




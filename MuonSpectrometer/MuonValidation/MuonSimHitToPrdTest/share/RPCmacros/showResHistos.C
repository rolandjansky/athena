//#include "AtlasUtils.h"
#include "plotUtils.C"

void showResHistos() {
    TCanvas* cHitMap = new TCanvas("HitMap", "Hit Maps", 1000, 600);
    TCanvas* cPhiDigForEffMap = new TCanvas("PhiDigForEffMap", "Phi Digits for Efficiency Map", 1000, 600);
    TCanvas* cEtaDigForEffMap = new TCanvas("EtaDigForEffMap", "Eta Digits for Efficiency Map", 1000, 600);
    TCanvas* cGapDigForEffMap = new TCanvas("GapDigForEffMap", "Gap Digits for Efficiency Map", 1000, 600);
    TCanvas* cGapEffMap = new TCanvas("GapEffMap", "Gap Efficiency Maps", 1000, 600);
    TCanvas* cPhiEffMap = new TCanvas("PhiEffMap", "Phi Efficiency Maps", 1000, 600);
    TCanvas* cEtaEffMap = new TCanvas("EtaEffMap", "Eta Efficiency Maps", 1000, 600);
    TCanvas* cAll = new TCanvas("All", "All");
    TCanvas* cLowPt = new TCanvas("LowPt", "LowPt");
    TCanvas* cHighPt = new TCanvas("HighPt", "HighPt");
    TCanvas* cPivot = new TCanvas("Pivot", "Pivot");
    TCanvas* cSideCvsZ = new TCanvas("SideCvsZ", "Res w.r.t Hits SideCvsZ", 700, 900);
    TCanvas* cEffVsZlpt = new TCanvas("EffVsZ_Lpt", "EffVsZ_Lpt");
    TCanvas* cEffVsZhpt = new TCanvas("EffVsZ_Hpt", "EffVsZ_Hpt");
    TCanvas* cEffVsZpiv = new TCanvas("EffVsZ_piv", "EffVsZ_piv");
    TCanvas* cEffVsPlpt = new TCanvas("EffVsP_Lpt", "EffVsP_Lpt");
    TCanvas* cEffVsPhpt = new TCanvas("EffVsP_Hpt", "EffVsP_Hpt");
    TCanvas* cEffVsPpiv = new TCanvas("EffVsP_piv", "EffVsP_piv");
    TCanvas* cEffDistr = new TCanvas("EffDistr", "Efficiency Distributions", 1100, 900);

    //  gROOT->ProcessLine(".L AtlasStyle.C");
    //  char* fileName = "residuals.root";
    // TFile* file =new TFile("/afs/le.infn.it/user/s/spagnolo/atlas/sw/validation/digit/residuals_100k_step5new_25trk.root","OLD");
    TFile* file = new TFile("residuals.root", "OLD");
    if (file->IsOpen()) {
        file->ls();
        TH2F* h_digitEtaZResidualsA = (TH2F*)gROOT->FindObject("digitEtaZResidualsA");
        TH2F* h_digitEtaZResidualsC = (TH2F*)gROOT->FindObject("digitEtaZResidualsC");

        TH2F* h_dighitZResVsZlay1 = (TH2F*)gROOT->FindObject("digithitZResVsZlay1");
        TH2F* h_dighitZResVsZlay2 = (TH2F*)gROOT->FindObject("digithitZResVsZlay2");
        TH2F* h_dighitZResVsZlay3 = (TH2F*)gROOT->FindObject("digithitZResVsZlay3");
        TH2F* h_dighitZResVsZlay4 = (TH2F*)gROOT->FindObject("digithitZResVsZlay4");
        TH2F* h_dighitZResVsZlay5 = (TH2F*)gROOT->FindObject("digithitZResVsZlay5");
        TH2F* h_dighitZResVsZlay6 = (TH2F*)gROOT->FindObject("digithitZResVsZlay6");

        TH2F* h_digitPhiResidualsA = (TH2F*)gROOT->FindObject("digitPhiResidualsA");
        TH2F* h_digitPhiResidualsC = (TH2F*)gROOT->FindObject("digitPhiResidualsC");
        //      TH2F* h_digithitEtaZResidualsA = (TH2F *)gROOT->FindObject("digithitEtaZResidualsA");
        //      TH2F* h_digithitEtaZResidualsC = (TH2F *)gROOT->FindObject("digithitEtaZResidualsC");
        TH2F* h_digithitPhiResidualsA = (TH2F*)gROOT->FindObject("digithitPhiResidualsA");
        TH2F* h_digithitPhiResidualsC = (TH2F*)gROOT->FindObject("digithitPhiResidualsC");

        TH1F* h_hitVsZlay1 = (TH1F*)gROOT->FindObject("hitVsZlay1");
        TH1F* h_hitVsZlay2 = (TH1F*)gROOT->FindObject("hitVsZlay2");
        TH1F* h_hitVsZlay3 = (TH1F*)gROOT->FindObject("hitVsZlay3");
        TH1F* h_hitVsZlay4 = (TH1F*)gROOT->FindObject("hitVsZlay4");
        TH1F* h_hitVsZlay5 = (TH1F*)gROOT->FindObject("hitVsZlay5");
        TH1F* h_hitVsZlay6 = (TH1F*)gROOT->FindObject("hitVsZlay6");

        TH1F* h_digitVsZlay1 = (TH1F*)gROOT->FindObject("digitVsZlay1");
        TH1F* h_digitVsZlay2 = (TH1F*)gROOT->FindObject("digitVsZlay2");
        TH1F* h_digitVsZlay3 = (TH1F*)gROOT->FindObject("digitVsZlay3");
        TH1F* h_digitVsZlay4 = (TH1F*)gROOT->FindObject("digitVsZlay4");
        TH1F* h_digitVsZlay5 = (TH1F*)gROOT->FindObject("digitVsZlay5");
        TH1F* h_digitVsZlay6 = (TH1F*)gROOT->FindObject("digitVsZlay6");

        TH1F* h_digeffVsZlay1 = (TH1F*)gROOT->FindObject("digeffVsZlay1");
        TH1F* h_digeffVsZlay2 = (TH1F*)gROOT->FindObject("digeffVsZlay2");
        TH1F* h_digeffVsZlay3 = (TH1F*)gROOT->FindObject("digeffVsZlay3");
        TH1F* h_digeffVsZlay4 = (TH1F*)gROOT->FindObject("digeffVsZlay4");
        TH1F* h_digeffVsZlay5 = (TH1F*)gROOT->FindObject("digeffVsZlay5");
        TH1F* h_digeffVsZlay6 = (TH1F*)gROOT->FindObject("digeffVsZlay6");

        TH1F* h_hitVsPlay1 = (TH1F*)gROOT->FindObject("hitVsPlay1");
        TH1F* h_hitVsPlay2 = (TH1F*)gROOT->FindObject("hitVsPlay2");
        TH1F* h_hitVsPlay3 = (TH1F*)gROOT->FindObject("hitVsPlay3");
        TH1F* h_hitVsPlay4 = (TH1F*)gROOT->FindObject("hitVsPlay4");
        TH1F* h_hitVsPlay5 = (TH1F*)gROOT->FindObject("hitVsPlay5");
        TH1F* h_hitVsPlay6 = (TH1F*)gROOT->FindObject("hitVsPlay6");

        TH1F* h_digitVsPlay1 = (TH1F*)gROOT->FindObject("digitVsPlay1");
        TH1F* h_digitVsPlay2 = (TH1F*)gROOT->FindObject("digitVsPlay2");
        TH1F* h_digitVsPlay3 = (TH1F*)gROOT->FindObject("digitVsPlay3");
        TH1F* h_digitVsPlay4 = (TH1F*)gROOT->FindObject("digitVsPlay4");
        TH1F* h_digitVsPlay5 = (TH1F*)gROOT->FindObject("digitVsPlay5");
        TH1F* h_digitVsPlay6 = (TH1F*)gROOT->FindObject("digitVsPlay6");

        TH1F* h_digeffVsPlay1 = (TH1F*)gROOT->FindObject("digeffVsPlay1");
        TH1F* h_digeffVsPlay2 = (TH1F*)gROOT->FindObject("digeffVsPlay2");
        TH1F* h_digeffVsPlay3 = (TH1F*)gROOT->FindObject("digeffVsPlay3");
        TH1F* h_digeffVsPlay4 = (TH1F*)gROOT->FindObject("digeffVsPlay4");
        TH1F* h_digeffVsPlay5 = (TH1F*)gROOT->FindObject("digeffVsPlay5");
        TH1F* h_digeffVsPlay6 = (TH1F*)gROOT->FindObject("digeffVsPlay6");

        TH2F* h_effmapHits1 = (TH2F*)gROOT->FindObject("effmapHitslay1");
        TH2F* h_effmapHits2 = (TH2F*)gROOT->FindObject("effmapHitslay2");
        TH2F* h_effmapHits3 = (TH2F*)gROOT->FindObject("effmapHitslay3");
        TH2F* h_effmapHits4 = (TH2F*)gROOT->FindObject("effmapHitslay4");
        TH2F* h_effmapHits5 = (TH2F*)gROOT->FindObject("effmapHitslay5");
        TH2F* h_effmapHits6 = (TH2F*)gROOT->FindObject("effmapHitslay6");

        TH2F* h_effmapPhiAndEtaDig1 = (TH2F*)gROOT->FindObject("effmapPhiAndEtaDiglay1");
        TH2F* h_effmapPhiAndEtaDig2 = (TH2F*)gROOT->FindObject("effmapPhiAndEtaDiglay2");
        TH2F* h_effmapPhiAndEtaDig3 = (TH2F*)gROOT->FindObject("effmapPhiAndEtaDiglay3");
        TH2F* h_effmapPhiAndEtaDig4 = (TH2F*)gROOT->FindObject("effmapPhiAndEtaDiglay4");
        TH2F* h_effmapPhiAndEtaDig5 = (TH2F*)gROOT->FindObject("effmapPhiAndEtaDiglay5");
        TH2F* h_effmapPhiAndEtaDig6 = (TH2F*)gROOT->FindObject("effmapPhiAndEtaDiglay6");

        TH2F* h_effmapPhiOnlyDig1 = (TH2F*)gROOT->FindObject("effmapPhiOnlyDiglay1");
        TH2F* h_effmapPhiOnlyDig2 = (TH2F*)gROOT->FindObject("effmapPhiOnlyDiglay2");
        TH2F* h_effmapPhiOnlyDig3 = (TH2F*)gROOT->FindObject("effmapPhiOnlyDiglay3");
        TH2F* h_effmapPhiOnlyDig4 = (TH2F*)gROOT->FindObject("effmapPhiOnlyDiglay4");
        TH2F* h_effmapPhiOnlyDig5 = (TH2F*)gROOT->FindObject("effmapPhiOnlyDiglay5");
        TH2F* h_effmapPhiOnlyDig6 = (TH2F*)gROOT->FindObject("effmapPhiOnlyDiglay6");

        TH2F* h_effmapEtaOnlyDig1 = (TH2F*)gROOT->FindObject("effmapEtaOnlyDiglay1");
        TH2F* h_effmapEtaOnlyDig2 = (TH2F*)gROOT->FindObject("effmapEtaOnlyDiglay2");
        TH2F* h_effmapEtaOnlyDig3 = (TH2F*)gROOT->FindObject("effmapEtaOnlyDiglay3");
        TH2F* h_effmapEtaOnlyDig4 = (TH2F*)gROOT->FindObject("effmapEtaOnlyDiglay4");
        TH2F* h_effmapEtaOnlyDig5 = (TH2F*)gROOT->FindObject("effmapEtaOnlyDiglay5");
        TH2F* h_effmapEtaOnlyDig6 = (TH2F*)gROOT->FindObject("effmapEtaOnlyDiglay6");

        TH2F* h_effmapGap1 = (TH2F*)gROOT->FindObject("effmapGaplay1");
        TH2F* h_effmapGap2 = (TH2F*)gROOT->FindObject("effmapGaplay2");
        TH2F* h_effmapGap3 = (TH2F*)gROOT->FindObject("effmapGaplay3");
        TH2F* h_effmapGap4 = (TH2F*)gROOT->FindObject("effmapGaplay4");
        TH2F* h_effmapGap5 = (TH2F*)gROOT->FindObject("effmapGaplay5");
        TH2F* h_effmapGap6 = (TH2F*)gROOT->FindObject("effmapGaplay6");

        TH2F* h_effmapPhi1 = (TH2F*)gROOT->FindObject("effmapPhilay1");
        TH2F* h_effmapPhi2 = (TH2F*)gROOT->FindObject("effmapPhilay2");
        TH2F* h_effmapPhi3 = (TH2F*)gROOT->FindObject("effmapPhilay3");
        TH2F* h_effmapPhi4 = (TH2F*)gROOT->FindObject("effmapPhilay4");
        TH2F* h_effmapPhi5 = (TH2F*)gROOT->FindObject("effmapPhilay5");
        TH2F* h_effmapPhi6 = (TH2F*)gROOT->FindObject("effmapPhilay6");

        TH2F* h_effmapEta1 = (TH2F*)gROOT->FindObject("effmapEtalay1");
        TH2F* h_effmapEta2 = (TH2F*)gROOT->FindObject("effmapEtalay2");
        TH2F* h_effmapEta3 = (TH2F*)gROOT->FindObject("effmapEtalay3");
        TH2F* h_effmapEta4 = (TH2F*)gROOT->FindObject("effmapEtalay4");
        TH2F* h_effmapEta5 = (TH2F*)gROOT->FindObject("effmapEtalay5");
        TH2F* h_effmapEta6 = (TH2F*)gROOT->FindObject("effmapEtalay6");

        h_digitEtaZResidualsA->ProjectionY("digZsideA_all", 0, -1);
        h_digitEtaZResidualsA->ProjectionY("digZsideA_lay1", 1, 1);
        h_digitEtaZResidualsA->ProjectionY("digZsideA_lay2", 2, 2);
        h_digitEtaZResidualsA->ProjectionY("digZsideA_lay3", 3, 3);
        h_digitEtaZResidualsA->ProjectionY("digZsideA_lay4", 4, 4);
        h_digitEtaZResidualsA->ProjectionY("digZsideA_lay5", 5, 5);
        h_digitEtaZResidualsA->ProjectionY("digZsideA_lay6", 6, 6);

        h_digitEtaZResidualsC->ProjectionY("digZsideC_all", 0, -1);
        h_digitEtaZResidualsC->ProjectionY("digZsideC_lay1", 1, 1);
        h_digitEtaZResidualsC->ProjectionY("digZsideC_lay2", 2, 2);
        h_digitEtaZResidualsC->ProjectionY("digZsideC_lay3", 3, 3);
        h_digitEtaZResidualsC->ProjectionY("digZsideC_lay4", 4, 4);
        h_digitEtaZResidualsC->ProjectionY("digZsideC_lay5", 5, 5);
        h_digitEtaZResidualsC->ProjectionY("digZsideC_lay6", 6, 6);

        h_dighitZResVsZlay1->ProjectionY("dighitZResVsZlay1C", 1, 500);
        h_dighitZResVsZlay2->ProjectionY("dighitZResVsZlay2C", 1, 500);
        h_dighitZResVsZlay3->ProjectionY("dighitZResVsZlay3C", 1, 500);
        h_dighitZResVsZlay4->ProjectionY("dighitZResVsZlay4C", 1, 500);
        h_dighitZResVsZlay5->ProjectionY("dighitZResVsZlay5C", 1, 500);
        h_dighitZResVsZlay6->ProjectionY("dighitZResVsZlay6C", 1, 500);
        TH1F* h_dighitZResVsZlay1C = (TH1F*)gROOT->FindObject("dighitZResVsZlay1C");
        TH1F* h_dighitZResVsZlay2C = (TH1F*)gROOT->FindObject("dighitZResVsZlay2C");
        TH1F* h_dighitZResVsZlay3C = (TH1F*)gROOT->FindObject("dighitZResVsZlay3C");
        TH1F* h_dighitZResVsZlay4C = (TH1F*)gROOT->FindObject("dighitZResVsZlay4C");
        TH1F* h_dighitZResVsZlay5C = (TH1F*)gROOT->FindObject("dighitZResVsZlay5C");
        TH1F* h_dighitZResVsZlay6C = (TH1F*)gROOT->FindObject("dighitZResVsZlay6C");
        TH1F* h_dighitZResVsZC = (TH1F*)h_dighitZResVsZlay1C->Clone();
        h_dighitZResVsZC->Add(h_dighitZResVsZlay2C);
        h_dighitZResVsZC->Add(h_dighitZResVsZlay3C);
        h_dighitZResVsZC->Add(h_dighitZResVsZlay4C);
        h_dighitZResVsZC->Add(h_dighitZResVsZlay5C);
        h_dighitZResVsZC->Add(h_dighitZResVsZlay6C);

        h_dighitZResVsZlay1->ProjectionY("dighitZResVsZlay1A", 500, 1000);
        h_dighitZResVsZlay2->ProjectionY("dighitZResVsZlay2A", 500, 1000);
        h_dighitZResVsZlay3->ProjectionY("dighitZResVsZlay3A", 500, 1000);
        h_dighitZResVsZlay4->ProjectionY("dighitZResVsZlay4A", 500, 1000);
        h_dighitZResVsZlay5->ProjectionY("dighitZResVsZlay5A", 500, 1000);
        h_dighitZResVsZlay6->ProjectionY("dighitZResVsZlay6A", 500, 1000);
        TH1F* h_dighitZResVsZlay1A = (TH1F*)gROOT->FindObject("dighitZResVsZlay1A");
        TH1F* h_dighitZResVsZlay2A = (TH1F*)gROOT->FindObject("dighitZResVsZlay2A");
        TH1F* h_dighitZResVsZlay3A = (TH1F*)gROOT->FindObject("dighitZResVsZlay3A");
        TH1F* h_dighitZResVsZlay4A = (TH1F*)gROOT->FindObject("dighitZResVsZlay4A");
        TH1F* h_dighitZResVsZlay5A = (TH1F*)gROOT->FindObject("dighitZResVsZlay5A");
        TH1F* h_dighitZResVsZlay6A = (TH1F*)gROOT->FindObject("dighitZResVsZlay6A");
        TH1F* h_dighitZResVsZA = (TH1F*)h_dighitZResVsZlay1A->Clone();
        h_dighitZResVsZA->Add(h_dighitZResVsZlay2A);
        h_dighitZResVsZA->Add(h_dighitZResVsZlay3A);
        h_dighitZResVsZA->Add(h_dighitZResVsZlay4A);
        h_dighitZResVsZA->Add(h_dighitZResVsZlay5A);
        h_dighitZResVsZA->Add(h_dighitZResVsZlay6A);

        h_digitPhiResidualsA->ProjectionY("digXYsideA_all", 0, -1);
        h_digithitPhiResidualsA->ProjectionY("dighitXYsideA_all", 0, -1);
        h_digitPhiResidualsA->ProjectionY("digXYsideA_lay1", 1, 1);
        h_digitPhiResidualsA->ProjectionY("digXYsideA_lay2", 2, 2);
        h_digitPhiResidualsA->ProjectionY("digXYsideA_lay3", 3, 3);
        h_digitPhiResidualsA->ProjectionY("digXYsideA_lay4", 4, 4);
        h_digitPhiResidualsA->ProjectionY("digXYsideA_lay5", 5, 5);
        h_digitPhiResidualsA->ProjectionY("digXYsideA_lay6", 6, 6);

        h_digitPhiResidualsC->ProjectionY("digXYsideC_all", 0, -1);
        h_digithitPhiResidualsC->ProjectionY("dighitXYsideC_all", 0, -1);
        h_digitPhiResidualsC->ProjectionY("digXYsideC_lay1", 1, 1);
        h_digitPhiResidualsC->ProjectionY("digXYsideC_lay2", 2, 2);
        h_digitPhiResidualsC->ProjectionY("digXYsideC_lay3", 2, 3);
        h_digitPhiResidualsC->ProjectionY("digXYsideC_lay4", 3, 4);
        h_digitPhiResidualsC->ProjectionY("digXYsideC_lay5", 4, 5);
        h_digitPhiResidualsC->ProjectionY("digXYsideC_lay6", 6, 6);

        cAll->Divide(2, 2);
        cAll->cd(1);
        TH1F* digXYsideC_all = (TH1F*)gROOT->FindObject("digXYsideC_all");
        digXYsideC_all->Draw();
        TH1F* dighitXYsideC_all = (TH1F*)gROOT->FindObject("dighitXYsideC_all");
        dighitXYsideC_all->SetLineColor(2);
        dighitXYsideC_all->Draw("SAME");
        cAll->cd(2);
        TH1F* digXYsideA_all = (TH1F*)gROOT->FindObject("digXYsideA_all");
        digXYsideA_all->Draw();
        TH1F* dighitXYsideA_all = (TH1F*)gROOT->FindObject("dighitXYsideA_all");
        dighitXYsideA_all->SetLineColor(2);
        dighitXYsideA_all->Draw("SAME");
        cAll->cd(3);
        h_dighitZResVsZC->SetLineColor(2);
        h_dighitZResVsZC->Draw();
        TH1F* digZsideC_all = (TH1F*)gROOT->FindObject("digZsideC_all");
        digZsideC_all->Draw("SAME");
        cAll->cd(4);
        h_dighitZResVsZA->SetLineColor(2);
        h_dighitZResVsZA->Draw();
        TH1F* digZsideA_all = (TH1F*)gROOT->FindObject("digZsideA_all");
        digZsideA_all->Draw("SAME");
        cAll->SaveAs("resDigAll.png");

        cLowPt->Divide(2, 2);
        cLowPt->cd(1);
        TH1F* digZsideC_lay1 = (TH1F*)gROOT->FindObject("digZsideC_lay1");
        digZsideC_lay1->Draw();
        cLowPt->cd(2);
        TH1F* digZsideA_lay1 = (TH1F*)gROOT->FindObject("digZsideA_lay1");
        digZsideA_lay1->Draw();
        cLowPt->cd(3);
        TH1F* digZsideC_lay2 = (TH1F*)gROOT->FindObject("digZsideC_lay2");
        digZsideC_lay2->Draw();
        cLowPt->cd(4);
        TH1F* digZsideA_lay2 = (TH1F*)gROOT->FindObject("digZsideA_lay2");
        digZsideA_lay2->Draw();
        cLowPt->SaveAs("resZdigLowPt.png");

        cPivot->Divide(2, 2);
        cPivot->cd(1);
        TH1F* digZsideC_lay3 = (TH1F*)gROOT->FindObject("digZsideC_lay3");
        digZsideC_lay3->Draw();
        cPivot->cd(2);
        TH1F* digZsideA_lay3 = (TH1F*)gROOT->FindObject("digZsideA_lay3");
        digZsideA_lay3->Draw();
        cPivot->cd(3);
        TH1F* digZsideC_lay4 = (TH1F*)gROOT->FindObject("digZsideC_lay4");
        digZsideC_lay4->Draw();
        cPivot->cd(4);
        TH1F* digZsideA_lay4 = (TH1F*)gROOT->FindObject("digZsideA_lay4");
        digZsideA_lay4->Draw();
        cPivot->SaveAs("resZdigPivot.png");

        cHighPt->Divide(2, 2);
        cHighPt->cd(1);
        TH1F* digZsideC_lay5 = (TH1F*)gROOT->FindObject("digZsideC_lay5");
        digZsideC_lay5->Draw();
        cHighPt->cd(2);
        TH1F* digZsideA_lay5 = (TH1F*)gROOT->FindObject("digZsideA_lay5");
        digZsideA_lay5->Draw();
        cHighPt->cd(3);
        TH1F* digZsideC_lay6 = (TH1F*)gROOT->FindObject("digZsideC_lay6");
        digZsideC_lay6->Draw();
        cHighPt->cd(4);
        TH1F* digZsideA_lay6 = (TH1F*)gROOT->FindObject("digZsideA_lay6");
        digZsideA_lay6->Draw();
        cHighPt->SaveAs("resZdigHighPt.png");

        cSideCvsZ->Divide(2, 3);
        cSideCvsZ->cd(1);
        h_dighitZResVsZlay1->Draw();
        cSideCvsZ->cd(2);
        h_dighitZResVsZlay2->Draw();
        cSideCvsZ->cd(3);
        h_dighitZResVsZlay3->Draw();
        cSideCvsZ->cd(4);
        h_dighitZResVsZlay4->Draw();
        cSideCvsZ->cd(5);
        h_dighitZResVsZlay5->Draw();
        cSideCvsZ->cd(6);
        h_dighitZResVsZlay6->Draw();
        cSideCvsZ->SaveAs("resZdig_hitVsZ.png");

        plotIdealEff(cEffVsZlpt, h_hitVsZlay1, h_digeffVsZlay1, h_digitVsZlay1, h_hitVsZlay2, h_digeffVsZlay2, h_digitVsZlay2);
        plotIdealEff(cEffVsZpiv, h_hitVsZlay3, h_digeffVsZlay3, h_digitVsZlay3, h_hitVsZlay4, h_digeffVsZlay4, h_digitVsZlay4);
        plotIdealEff(cEffVsZhpt, h_hitVsZlay5, h_digeffVsZlay5, h_digitVsZlay5, h_hitVsZlay6, h_digeffVsZlay6, h_digitVsZlay6);

        plotIdealEff(cEffVsPlpt, h_hitVsPlay1, h_digeffVsPlay1, h_digitVsPlay1, h_hitVsPlay2, h_digeffVsPlay2, h_digitVsPlay2);
        plotIdealEff(cEffVsPpiv, h_hitVsPlay3, h_digeffVsPlay3, h_digitVsPlay3, h_hitVsPlay4, h_digeffVsPlay4, h_digitVsPlay4);
        plotIdealEff(cEffVsPhpt, h_hitVsPlay5, h_digeffVsPlay5, h_digitVsPlay5, h_hitVsPlay6, h_digeffVsPlay6, h_digitVsPlay6);

        TH1F* h_ntrackOnPanel = (TH1F*)gROOT->FindObject("ntrackOnPanel");
        TH1F* h_phiPanelEff = (TH1F*)gROOT->FindObject("phiPanelEff");
        TH1F* h_etaPanelEff = (TH1F*)gROOT->FindObject("etaPanelEff");
        TH1F* h_gapEff = (TH1F*)gROOT->FindObject("gapEff");

        drawEffHist(h_phiPanelEff, 0.92, 1.);
        drawEffHist(h_etaPanelEff, 0.92, 1.);
        drawEffHist(h_gapEff, 0.95, 1.);

        cEffDistr->Divide(2, 2);
        cEffDistr->cd(1);
        h_phiPanelEff->Draw();
        gPad->Update();
        TPaveStats* st = (TPaveStats*)h_phiPanelEff->FindObject("stats");
        st->SetX1NDC(0.15);  // new x start position
        st->SetX2NDC(0.6);   // new x start position
        st->SetY1NDC(0.5);   // new x start position
        st->SetY2NDC(0.9);   // new x start position
        gPad->Update();

        cEffDistr->cd(2);
        h_etaPanelEff->Draw();
        gPad->Update();
        TPaveStats* st1 = (TPaveStats*)h_etaPanelEff->FindObject("stats");
        st1->SetX1NDC(0.15);  // new x start position
        st1->SetX2NDC(0.6);   // new x start position
        st1->SetY1NDC(0.5);   // new x start position
        st1->SetY2NDC(0.9);   // new x start position
        gPad->Update();

        cEffDistr->cd(3);
        h_gapEff->Draw();
        gPad->Update();
        TPaveStats* st2 = (TPaveStats*)h_gapEff->FindObject("stats");
        st2->SetX1NDC(0.15);  // new x start position
        st2->SetX2NDC(0.6);   // new x start position
        st2->SetY1NDC(0.5);   // new x start position
        st2->SetY2NDC(0.9);   // new x start position
        gPad->Update();

        cEffDistr->cd(4);
        h_ntrackOnPanel->Draw();
        cEffDistr->SaveAs("EffDistr.png");

        TCanvas* cphieff = new TCanvas("cphiEff", "phi panel eff");
        h_phiPanelEff->Draw();
        cphieff->SaveAs("phiEff.png");
        TCanvas* cetaeff = new TCanvas("cetaEff", "eta panel eff");
        h_etaPanelEff->Draw();
        cetaeff->SaveAs("etaEff.png");
        TCanvas* cgapeff = new TCanvas("gapEff", "gap eff");
        h_gapEff->Draw();
        cgapeff->SaveAs("gapEff.png");

        h_effmapHits1->SetStats(0);
        h_effmapHits2->SetStats(0);
        h_effmapHits3->SetStats(0);
        h_effmapHits4->SetStats(0);
        h_effmapHits5->SetStats(0);
        h_effmapHits6->SetStats(0);
        cHitMap->Divide(3, 2);
        cHitMap->cd(1);
        h_effmapHits1->Draw("COLZ");
        cHitMap->cd(4);
        h_effmapHits2->Draw("COLZ");
        cHitMap->cd(2);
        h_effmapHits3->Draw("COLZ");
        cHitMap->cd(5);
        h_effmapHits4->Draw("COLZ");
        cHitMap->cd(3);
        h_effmapHits5->Draw("COLZ");
        cHitMap->cd(6);
        h_effmapHits6->Draw("COLZ");
        cHitMap->SaveAs("HitMap.png");

        h_effmapPhi1->SetStats(0);
        h_effmapPhi2->SetStats(0);
        h_effmapPhi3->SetStats(0);
        h_effmapPhi4->SetStats(0);
        h_effmapPhi5->SetStats(0);
        h_effmapPhi6->SetStats(0);
        cPhiEffMap->Divide(3, 2);
        cPhiEffMap->cd(1);
        h_effmapPhi1->Draw("COLZ");
        cPhiEffMap->cd(4);
        h_effmapPhi2->Draw("COLZ");
        cPhiEffMap->cd(2);
        h_effmapPhi3->Draw("COLZ");
        cPhiEffMap->cd(5);
        h_effmapPhi4->Draw("COLZ");
        cPhiEffMap->cd(3);
        h_effmapPhi5->Draw("COLZ");
        cPhiEffMap->cd(6);
        h_effmapPhi6->Draw("COLZ");
        cPhiEffMap->SaveAs("PhiEffMap.png");

        h_effmapEta1->SetStats(0);
        h_effmapEta2->SetStats(0);
        h_effmapEta3->SetStats(0);
        h_effmapEta4->SetStats(0);
        h_effmapEta5->SetStats(0);
        h_effmapEta6->SetStats(0);
        cEtaEffMap->Divide(3, 2);
        cEtaEffMap->cd(1);
        h_effmapEta1->Draw("COLZ");
        cEtaEffMap->cd(4);
        h_effmapEta2->Draw("COLZ");
        cEtaEffMap->cd(2);
        h_effmapEta3->Draw("COLZ");
        cEtaEffMap->cd(5);
        h_effmapEta4->Draw("COLZ");
        cEtaEffMap->cd(3);
        h_effmapEta5->Draw("COLZ");
        cEtaEffMap->cd(6);
        h_effmapEta6->Draw("COLZ");
        cEtaEffMap->SaveAs("EtaEffMap.png");

        h_effmapGap1->SetStats(0);
        h_effmapGap2->SetStats(0);
        h_effmapGap3->SetStats(0);
        h_effmapGap4->SetStats(0);
        h_effmapGap5->SetStats(0);
        h_effmapGap6->SetStats(0);
        cGapEffMap->Divide(3, 2);
        cGapEffMap->cd(1);
        h_effmapGap1->Draw("COLZ");
        cGapEffMap->cd(4);
        h_effmapGap2->Draw("COLZ");
        cGapEffMap->cd(2);
        h_effmapGap3->Draw("COLZ");
        cGapEffMap->cd(5);
        h_effmapGap4->Draw("COLZ");
        cGapEffMap->cd(3);
        h_effmapGap5->Draw("COLZ");
        cGapEffMap->cd(6);
        h_effmapGap6->Draw("COLZ");
        cGapEffMap->SaveAs("GapEffMap.png");

    } else
        std::cout << "File  not found " << std::endl;

    //  if (file) file->Close();
}

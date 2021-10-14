#include <string>

void showMapEffMCData(unsigned short ilay, bool isPhi, TH2F* h_effmapPhiMC, TH2F* h_effmapPhiData, TH2F* h_effmapPhiRes,
                      TH2F* h_effmapPhiResNorm) {
    std::string title;
    std::string titleShort;
    if (isPhi) {
        title = "EffMapDataMC Layer=" + to_string(ilay) + " View=Phi";
        titleShort = "EffMapDataMC" + to_string(ilay) + "phi";
    } else {
        title = "EffMapDataMC Layer=" + to_string(ilay) + " View=Eta";
        titleShort = "EffMapDataMC" + to_string(ilay) + "eta";
    }
    std::string titlePlot = titleShort + ".png";
    TCanvas* cEffMap1phi = new TCanvas(titleShort.c_str(), title.c_str(), 800, 800);
    cEffMap1phi->Divide(2, 2);
    h_effmapPhiMC->SetStats(0);
    h_effmapPhiData->SetStats(0);
    h_effmapPhiRes->SetStats(0);
    h_effmapPhiResNorm->SetStats(0);
    cEffMap1phi->cd(1);
    h_effmapPhiMC->Draw("COLZ");
    cEffMap1phi->cd(2);
    h_effmapPhiData->Draw("COLZ");
    cEffMap1phi->cd(3);
    h_effmapPhiRes->Draw("COLZ");
    cEffMap1phi->cd(4);
    h_effmapPhiResNorm->Draw("COLZ");
    cEffMap1phi->SaveAs(titlePlot.c_str());
}

void drawEffHist(TH1F* h, float xmin = -999, float xmax = 999) {
    // TCanvas* c = new TCanvas();
    if (xmin < -998. || xmax > 998.) {
        xmin = h->GetXaxis()->GetXmin();
        xmax = h->GetXaxis()->GetXmax();
    }
    gStyle->SetOptFit(111111);
    gStyle->SetOptStat(111111);
    h->SetFillColor(5);
    h->SetLineWidth(2);
    h->SetLineColor(kOrange);
    h->SetFillColor(5);
    h->Fit("gaus", "", "", xmin, xmax);
    //  c->SaveAs("hfit.png");
    float ntot = (float)h->GetEntries();
    double neff0 = h->GetBinContent(1);
    double frac_eff0 = neff0 / ntot;
    double neffUnder50 = h->Integral(1, h->GetXaxis()->FindBin(0.5));
    double neffUnder80 = h->Integral(1, h->GetXaxis()->FindBin(0.8));
    double neffUnder90 = h->Integral(1, h->GetXaxis()->FindBin(0.9));
    double frac_eff50 = neffUnder50 / ntot;
    double frac_eff80 = neffUnder80 / ntot;
    double frac_eff90 = neffUnder90 / ntot;
    std::cout << "------------------------------------- histogram " << h->GetName() << std::endl;
    std::cout << " # eff=0, <50%, <80%, <90% " << std::endl
              << neff0 << " " << neffUnder50 << " " << neffUnder80 << " " << neffUnder90 << std::endl
              << std::setprecision(3) << frac_eff0 * 100. << "% " << frac_eff50 * 100 << "% " << frac_eff80 * 100 << "% "
              << frac_eff90 * 100 << "%" << std::setprecision(6) << std::endl;
    std::cout << "----------------------------------------------------------------------------------------" << std::endl;
}

void plotIdealEff(TCanvas* cEffVsZlpt, TH1* h_hitVsZlay1, TH1* h_digeffVsZlay1, TH1* h_digitVsZlay1, TH1* h_hitVsZlay2,
                  TH1* h_digeffVsZlay2, TH1* h_digitVsZlay2) {
    cEffVsZlpt->Divide(2, 2);

    cEffVsZlpt->cd(1);
    h_hitVsZlay1->Draw();
    h_digeffVsZlay1->SetLineColor(2);
    h_digeffVsZlay1->Draw("SAME");
    cEffVsZlpt->cd(2);
    h_hitVsZlay2->Draw();
    h_digeffVsZlay2->SetLineColor(2);
    h_digeffVsZlay2->Draw("SAME");
    cEffVsZlpt->cd(3);
    TH1* h_effVsZlay1 = (TH1*)h_hitVsZlay1->Clone();
    h_effVsZlay1->Divide(h_digeffVsZlay1, h_hitVsZlay1, 1, 1, "B");
    TH1* h_csVsZlay1 = (TH1*)h_hitVsZlay1->Clone();
    h_csVsZlay1->Divide(h_digitVsZlay1, h_digeffVsZlay1, 1, 1);
    h_effVsZlay1->SetMinimum(0.);
    h_effVsZlay1->SetMaximum(1.7);
    h_effVsZlay1->Draw();
    h_csVsZlay1->SetLineColor(kGreen);
    h_csVsZlay1->Draw("SAME");
    cEffVsZlpt->cd(4);
    TH1* h_effVsZlay2 = (TH1*)h_hitVsZlay2->Clone();
    h_effVsZlay2->Divide(h_digeffVsZlay2, h_hitVsZlay2, 1, 1, "B");
    TH1* h_csVsZlay2 = (TH1*)h_hitVsZlay2->Clone();
    h_csVsZlay2->Divide(h_digitVsZlay2, h_digeffVsZlay2, 1, 1);
    h_effVsZlay2->SetMinimum(0.);
    h_effVsZlay2->SetMaximum(1.7);
    h_effVsZlay2->Draw();
    h_csVsZlay2->SetLineColor(kGreen);
    h_csVsZlay2->Draw("SAME");
    std::string fileName = cEffVsZlpt->GetName();
    fileName = fileName + ".png";
    cEffVsZlpt->SaveAs(fileName.c_str());
}

int getIndWithID(unsigned long MCid, unsigned long* effDataMapID, int nPanelDataTotal) {
    for (unsigned int j = 0; j < nPanelDataTotal; ++j) {
        if (effDataMapID[j] == MCid) { return j; }
    }
    std::cout << " nothing found for ID " << MCid << std::endl;
    return -1;
}

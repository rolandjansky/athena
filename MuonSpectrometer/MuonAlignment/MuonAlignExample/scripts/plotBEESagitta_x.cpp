/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "ChamberNames.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "TH2F.h"
#include "TStyle.h"

int main() {
    // gStyle->SetOptStat(0);
    // gStyle->SetOptFit(0);
    gStyle->SetFrameFillStyle(0);
    gStyle->SetTitleFillColor(0);
    gStyle->SetTitleBorderSize(1);
    gStyle->SetPadLeftMargin(0.15);
    gStyle->SetPadRightMargin(0.15);
    gStyle->SetPalette(1);
    gStyle->SetOptFit(111111);

    const int NXBINS = 15;
    const int NYBINS = 15;

    TFile* outputfile = new TFile("out_iter0.root", "RECREATE");

    TH1F* trackSag = new TH1F("trackSag", "trackSag", 100, -150., 150.);
    TH1F* trackSagNorm = new TH1F("trackSagNorm", "trackSagNorm", 100, -3., 3.);

    TH1F**** beeRes = new TH1F***[2];
    TH1F**** eelRes = new TH1F***[2];
    TH2F**** beeHists = new TH2F***[2];
    TH2F**** beeHistsAvg = new TH2F***[2];
    TH2F**** beeNTrksHists = new TH2F***[2];
    string*** hnames = new string**[2];
    for (int isideAC = 0; isideAC < 2; isideAC++) {
        beeRes[isideAC] = new TH1F**[2];
        eelRes[isideAC] = new TH1F**[2];
        beeHists[isideAC] = new TH2F**[2];
        beeHistsAvg[isideAC] = new TH2F**[2];
        beeNTrksHists[isideAC] = new TH2F**[2];
        hnames[isideAC] = new string*[2];
        for (int ieta = 0; ieta < 2; ieta++) {
            beeRes[isideAC][ieta] = new TH1F*[8];
            eelRes[isideAC][ieta] = new TH1F*[8];
            beeHists[isideAC][ieta] = new TH2F*[8];
            beeHistsAvg[isideAC][ieta] = new TH2F*[8];
            beeNTrksHists[isideAC][ieta] = new TH2F*[8];
            hnames[isideAC][ieta] = new string[8];
            for (int iphi = 0; iphi < 8; iphi++) {
                stringstream hname;
                hname << "BEE" << ieta + 1;
                if (isideAC == 0)
                    hname << "A";
                else
                    hname << "C";
                hname << setfill('0') << setw(2) << (iphi + 1) * 2;
                hnames[isideAC][ieta][iphi] = hname.str();
                beeHists[isideAC][ieta][iphi] =
                    new TH2F(hname.str().c_str(), hname.str().c_str(), NXBINS, -400., 400., NYBINS, -700., 700.);
                beeHistsAvg[isideAC][ieta][iphi] =
                    new TH2F((hname.str() + "_avg").c_str(), (hname.str() + "_avg").c_str(), NXBINS, -400., 400., NYBINS, -700., 700.);
                beeNTrksHists[isideAC][ieta][iphi] =
                    new TH2F((hname.str() + "_ntrks").c_str(), (hname.str() + "_ntrks").c_str(), NXBINS, -400., 400., NYBINS, -700., 700.);

                beeRes[isideAC][ieta][iphi] = new TH1F((hname.str() + "_res").c_str(), (hname.str() + "_res").c_str(), 100, -1., 1.);
                stringstream eelname;
                eelname << "EEL" << ieta + 1;
                if (isideAC == 0)
                    eelname << "A";
                else
                    eelname << "C";
                eelname << setfill('0') << setw(2) << (iphi + 1) * 2 - 1;
                eelRes[isideAC][ieta][iphi] = new TH1F((eelname.str() + "_res").c_str(), (eelname.str() + "_res").c_str(), 100, -1., 1.);
            }
        }
    }

    ChamberNames chNames("chambers.txt");

    TChain* tree = new TChain("MuonTrackPreProcessor");
    tree->Add("/tmp/roberth/user.roberth.TMU15GEV.20101119_EE_iter0_V1.110216012437/iter0/*root*");

    double qOverP, trackSagitta;
    double beePosX, beePosY;
    int nChambers;
    int *chamberId(new int[50]), *resIndex(new int[50]);
    int nresiduals;
    double* residuals(new double[100]);
    tree->SetBranchAddress("qOverP", &qOverP);
    tree->SetBranchAddress("trackSagitta", &trackSagitta);
    tree->SetBranchAddress("BEEPosX", &beePosX);
    tree->SetBranchAddress("BEEPosY", &beePosY);
    tree->SetBranchAddress("nChambers", &nChambers);
    tree->SetBranchAddress("chamberId", chamberId);
    tree->SetBranchAddress("resIndex", resIndex);
    tree->SetBranchAddress("nresiduals", &nresiduals);
    tree->SetBranchAddress("residuals", residuals);

    for (int ientry = 0; ientry < (int)tree->GetEntries(); ientry++) {
        if (ientry % 100000 == 0) cout << "ientry: " << ientry << endl;
        tree->GetEntry(ientry);
        if (beePosX < -9998.) continue;

        // cout<<"ientry: "<<ientry<<endl;
        std::string beeName = "";
        std::string eelName = "";
        int firstBEERes(0), lastBEERes(0);
        int firstEELRes(0), lastEELRes(0);
        for (int ich = 0; ich < nChambers; ich++) {
            int id = chamberId[ich];
            std::string name = chNames.chamberName(id);
            if (name.substr(0, 3) == "BEE") {
                beeName = name;
                firstBEERes = resIndex[ich];
                lastBEERes = (ich + 1 < nChambers) ? resIndex[ich + 1] : nresiduals;
            }
            if (name.substr(0, 3) == "EEL") {
                eelName = name;
                firstEELRes = resIndex[ich];
                lastEELRes = (ich + 1 < nChambers) ? resIndex[ich + 1] : nresiduals;
            }
            // cout<<"name: "<<name<<endl;
        }

        // remove tracks not passing selection in MuonTrackPreProcessor
        if (beeName == "" && eelName == "") { continue; }

        if (beeName != "") {
            int stPhi = chNames.stPhi(beeName);
            int stEta = chNames.stEta(beeName);
            int isideAC = (stEta > 0) ? 0 : 1;
            int ieta = std::abs(stEta) - 1;
            int iphi = stPhi - 1;
            // cout<<"isideAC: "<<isideAC<<", ieta: "<<ieta<<", stPhi: "<<stPhi<<", iphi: "<<iphi<<endl;

            double trackSagVal = trackSagitta;
            if (isideAC == 0) trackSagVal *= -1.;
            double trackSagNormVal = 1.e-6 * trackSagVal / qOverP;
            if (trackSagNormVal < 3. && trackSagNormVal > -3.) {
                beeHists[isideAC][ieta][iphi]->Fill(beePosX, beePosY, trackSagNormVal);
                beeNTrksHists[isideAC][ieta][iphi]->Fill(beePosX, beePosY);
            }
            trackSag->Fill(trackSagitta);
            trackSagNorm->Fill(trackSagNormVal);
            for (int ires = firstBEERes; ires < lastBEERes; ires++) { beeRes[isideAC][ieta][iphi]->Fill(residuals[ires]); }
        }
        if (eelName != "") {
            int stPhi = chNames.stPhi(eelName);
            int stEta = chNames.stEta(eelName);
            int isideAC = (stEta > 0) ? 0 : 1;
            int ieta = std::abs(stEta) - 1;
            int iphi = stPhi - 1;
            for (int ires = firstEELRes; ires < lastEELRes; ires++) { eelRes[isideAC][ieta][iphi]->Fill(residuals[ires]); }
        }
    }

    TCanvas* canv = new TCanvas("canv", "canv");
    canv->SetFillColor(10);

    trackSag->Draw();
    canv->Print("trackSag.eps");
    trackSagNorm->Draw();
    canv->Print("trackSagNorm.eps");

    outputfile->cd();
    trackSag->Write();
    trackSagNorm->Write();

    TCanvas* canvres = new TCanvas("canvres", "canvres");
    canvres->SetFillColor(10);

    for (int isideAC = 0; isideAC < 2; isideAC++) {
        for (int iphi = 0; iphi < 8; iphi++) {
            // for (int isideAC=0;isideAC<1;isideAC++) {
            // for (int iphi=7;iphi<8;iphi++) {
            double max = -999.;
            double min = 999.;
            canv->Clear();
            canv->Divide(1, 2);
            for (int ieta = 0; ieta < 2; ieta++) {
                for (int ix = 0; ix < NXBINS; ix++) {
                    for (int iy = 0; iy < NYBINS; iy++) {
                        double ntrks = beeNTrksHists[isideAC][ieta][iphi]->GetBinContent(ix + 1, iy + 1);

                        if (ntrks > 0) {
                            double val = beeHists[isideAC][ieta][iphi]->GetBinContent(ix + 1, iy + 1) / ntrks;
                            if (val > max) max = val;
                            if (val < min) min = val;
                            beeHistsAvg[isideAC][ieta][iphi]->SetBinContent(ix + 1, iy + 1, val);
                        }
                        // else
                        // beeHists[isideAC][ieta][iphi]->SetBinContent(ix+1,iy+1,-9999.);
                    }
                }
                canvres->cd();
                canvres->Clear();
                // gStyle->SetOptStat(1111111);
                // gStyle->SetOptFit(11111);
                beeRes[isideAC][ieta][iphi]->Draw();
                beeRes[isideAC][ieta][iphi]->Fit("gaus", "Q");
                outputfile->cd();
                beeRes[isideAC][ieta][iphi]->Write();
                canvres->Update();
                stringstream canvresname;
                canvresname << "BEERes_" << ieta + 1;
                if (isideAC == 0)
                    canvresname << "A";
                else
                    canvresname << "C";
                canvresname << iphi + 1 << ".eps";
                canvres->Print(canvresname.str().c_str());
                // gStyle->SetOptStat(0);
                // gStyle->SetOptFit(0);
            }
            for (int ieta = 0; ieta < 2; ieta++) {
                canv->cd(2 - ieta);
                beeHistsAvg[isideAC][ieta][iphi]->SetAxisRange(-3., 3., "Z");
                beeHistsAvg[isideAC][ieta][iphi]->Draw("colz");
                outputfile->cd();
                beeHistsAvg[isideAC][ieta][iphi]->Write();
            }
            std::stringstream canvname;
            canvname << "beeTrackSag_";
            if (isideAC == 0)
                canvname << "A";
            else
                canvname << "C";
            canvname << "_" << iphi + 1 << ".eps";
            canv->Print(canvname.str().c_str());
        }
    }
    outputfile->Close();
}

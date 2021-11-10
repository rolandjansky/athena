/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <TBox.h>
#include <TCanvas.h>
#include <TH2F.h>

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>

#include "plotUtils.C"

void showEffMaps(int view, RPCpanelList* myPanList, std::map<unsigned long, double>* effPhiPanelMap,
                 std::map<unsigned long, double>* effGapPanelMap, std::map<unsigned long, double>* ntrkOnPanelMap,
                 bool emulateMC15c = false) {
    if (emulateMC15c) std::cout << " **************************** Emulating MC15c ********* " << std::endl;
    // define color scale
    Int_t MyPalette[100];
    Double_t r[] = {0., 0.0, 0.0, 0.5, 1.0};
    Double_t g[] = {0., 0.0, 0.5, 1.0, 0.0};
    Double_t b[] = {0., 0.5, 1.0, 0.0, 0.0};

    Double_t stop[] = {0., .1, .50, .80, 1.0};
    Int_t FI = TColor::CreateGradientColorTable(5, stop, r, g, b, 100);
    for (int i = 0; i < 100; i++) {
        MyPalette[i] = FI + i;
        std::cout << "MyPalette[" << i << "]=" << MyPalette[i] << std::endl;
    }
    gStyle->SetPalette(100, MyPalette);

    TBox* t1 = new TBox();

    std::string viewString = "phi";
    if (view == 0) viewString = "eta";
    TCanvas* cLayer =
        new TCanvas(std::string("panelEffMaps_" + viewString).c_str(), std::string("panelEffMaps_" + viewString).c_str(), 800, 1000);
    TCanvas* cLayerArr[6];
    std::string canvasName[6];

    cLayer->Divide(2, 3);

    Double_t hphimin = -M_PI - M_PI / 10.;
    Double_t hphimax = M_PI + M_PI / 10.;
    TH2F* hLayer = new TH2F("hLayer", "hLayer", 100, -1.2, 1.2, 100, hphimin, hphimax);
    hLayer->GetXaxis()->SetTitle("eta");
    hLayer->GetYaxis()->SetTitle("phi");
    hLayer->SetTitle("");

    float binw = (hphimax - hphimin) / 100.;
    cLayer->cd(1);
    hLayer->Draw();
    cLayer->cd(2);
    hLayer->Draw();
    cLayer->cd(3);
    hLayer->Draw();
    cLayer->cd(4);
    hLayer->Draw();
    cLayer->cd(5);
    hLayer->Draw();
    cLayer->cd(6);
    hLayer->Draw();
    for (int j = 0; j < 6; ++j) {
        canvasName[j] = "panelEffMap_" + viewString + "_layer" + to_string(j + 1);
        cLayerArr[j] = new TCanvas(canvasName[j].c_str(), canvasName[j].c_str(), 1000, 800);
        cLayerArr[j]->cd();
        hLayer->Draw();
        for (int i = 0; i < 100; ++i) {
            t1->SetFillColor(MyPalette[i]);
            t1->DrawBox(1.2, hphimin + i * binw, 1.35, hphimin + (i + 1) * binw);
            if ((i + 1) % 10 == 0) {
                TText* t = new TText(1.37, hphimin + i * binw, to_string(i + 1).c_str());
                t->SetTextSize(0.02);
                t->Draw();
            }
        }
        TText* title = new TText(0, hphimax + 5. * binw, canvasName[j].c_str());
        title->SetTextSize(0.04);
        title->SetTextAlign(22);
        title->Draw();
    }
    cLayer->cd(1);

    TBox* t = new TBox();
    t->SetLineWidth(1);
    t->SetLineColor(kBlack);

    std::cout << " in showEffMaps - myPanList = " << myPanList << std::endl;

    cRPCpanelListIterator it = myPanList->begin();
    Int_t nPanels = 0;

    double etamin, etamax, phimin, phimax, zmin, zmax;
    std::string stName;
    unsigned short viewName;
    unsigned short layerRPC;
    std::string stringId;
    for (/*it=myPanList->begin()*/; it != myPanList->end(); ++it) {
        unsigned long pid = it->first;
        ++nPanels;
        etamin = myPanList->getEtaMin(pid);
        etamax = myPanList->getEtaMax(pid);
        phimin = myPanList->getPhiMin(pid);
        phimax = myPanList->getPhiMax(pid);
        zmin = myPanList->getZMin(pid);
        zmax = myPanList->getZMax(pid);
        viewName = myPanList->getView(pid);
        if (viewName != view) continue;
        viewString = "phi";
        if (viewName == 0) { viewString = "eta"; }
        layerRPC = myPanList->getLayer(pid);

        stringId = myPanList->getStringId(pid);
        stName = myPanList->getStName(pid);
        int index = myPanList->getIndexInList(pid);

        int ieffPanel = 0;
        int ieffGap = 0;
        int intrkOnP = 0;
        std::map<unsigned long, double>::const_iterator itntrk = ntrkOnPanelMap->find(pid);
        if (itntrk != ntrkOnPanelMap->end()) {
            double ntrk = (*itntrk).second;
            ieffPanel = -1;
            ieffGap = -1;
            if (ntrk < 1.) {
                if (emulateMC15c) {
                    ieffPanel = 98;
                    ieffGap = 98;
                    std::cout << " id " << pid << " NOT FOUND in panel eff. map - station Name = " << stName << " stringId = " << stringId
                              << " no extrapolated tracks (" << ntrk << ") resetting eff. to 0.98 (as MC15a)" << std::endl;
                } else {
                    std::cout << " id " << pid << " not found in panel eff. map - station Name = " << stName << " stringId = " << stringId
                              << " no extrapolated tracks !!! " << std::endl;
                }
            } else if (emulateMC15c && (ntrk < 100)) {
                ieffPanel = 98;
                ieffGap = 98;

                std::cout << " id " << pid << " FOUND in panel eff. map - station Name = " << stName << " stringId = " << stringId
                          << " eff is measured with <100 tracks (" << ntrk << ") resetting eff. to 0.98 (as MC15a)" << std::endl;
            } else {
                std::map<unsigned long, double>::const_iterator iteff = effPhiPanelMap->find(pid);
                if (iteff != effPhiPanelMap->end()) {
                    double eff = (*iteff).second;
                    ieffPanel = int(100. * eff);
                    if ((eff * 100. - ieffPanel) > 0.5) ieffPanel = ieffPanel + 1;
                    if (emulateMC15c) {
                        if (ieffPanel < 50) {
                            ieffPanel = 50;
                            std::cout << " id " << pid << " FOUND in panel eff. map - station Name = " << stName
                                      << " stringId = " << stringId << " eff<50% (" << eff << ") resetting eff. to 0.5 (for MC15c)"
                                      << std::endl;
                        }
                        if (ieffGap < 50) { ieffGap = 50; }
                    }
                } else {
                    if (emulateMC15c) {
                        ieffPanel = 98;
                        ieffGap = 98;
                        std::cout << " id " << pid << " NOT FOUND in panel eff. map - station Name = " << stName
                                  << " stringId = " << stringId << " HOWEVER n. of extrapolated tracks=" << ntrk
                                  << " - resetting eff. to 0.98 (as MC15a)" << std::endl;
                    } else
                        std::cout << " id " << pid << " not found in panel eff. map - station Name = " << stName
                                  << " stringId = " << stringId << std::endl;
                }
            }
        }

        t->SetLineColor(kBlack);
        if (stName == "BML" && layerRPC < 3) {
            if (stringId.substr(0, 7) == "[7.2.-7" || stringId.substr(0, 7) == "[7.2.7." || stringId.substr(0, 9) == "[7.2.-6.7" ||
                stringId.substr(0, 9) == "[7.2.6.7.") {
                layerRPC = layerRPC + 2;
            }
        }
        if (stName == "BOL" && layerRPC > 4) {
            if (stringId.substr(0, 7) == "[7.4.-8" || stringId.substr(0, 7) == "[7.4.8.") {}
        }

        cLayer->cd(layerRPC);

        if (ieffPanel >= 0) {
            t->SetFillColor(MyPalette[ieffPanel]);
            t->SetFillStyle(1001);
            cLayer->cd(layerRPC);
            t->DrawBox(etamin, phimin, etamax, phimax);
            t->SetFillStyle(0);
            t->DrawBox(etamin, phimin, etamax, phimax);
            cLayerArr[layerRPC - 1]->cd();
            t->SetFillStyle(1001);
            t->DrawBox(etamin, phimin, etamax, phimax);
            t->SetFillStyle(0);
            t->DrawBox(etamin, phimin, etamax, phimax);
        } else {
            t->SetFillColor(kWhite);
            t->SetFillStyle(1001);
            cLayer->cd(layerRPC);
            t->DrawBox(etamin, phimin, etamax, phimax);
            t->SetFillStyle(0);
            t->DrawBox(etamin, phimin, etamax, phimax);
            cLayerArr[layerRPC - 1]->cd();
            t->SetFillStyle(1001);
            t->DrawBox(etamin, phimin, etamax, phimax);
            t->SetFillStyle(0);
            t->DrawBox(etamin, phimin, etamax, phimax);
        }
    }

    std::cout << "Finished; n. of panels found is " << nPanels << std::endl;
    cLayer->SaveAs("RPClayers.png");
    for (int i = 0; i < 6; ++i) { cLayerArr[i]->SaveAs((canvasName[i] + ".png").c_str()); }
    return;
}

void plotEffOnPanelMap(Int_t inputLayer) {
    RPCpanelList* myPanList = new RPCpanelList();
    myPanList->init();

    TCanvas* cMCEffp = new TCanvas("MCEffp", "MC panel efficiency", 700, 700);
    TCanvas* cMCEffg = new TCanvas("MCEffg", "MC gap efficiency", 700, 700);
    TCanvas* cDataEffp = new TCanvas("DataEffp", "Data panel efficiency", 700, 700);
    TCanvas* cDataEffg = new TCanvas("DataEffg", "Data panel efficiency", 700, 700);
    cMCEffp->Divide(2, 2);
    cMCEffg->Divide(2, 2);
    cDataEffp->Divide(2, 2);
    cDataEffg->Divide(2, 2);

    TH1F* hMCEffPanelThr0 = new TH1F("hEffMCPanelThr0", "hEffMCPanelThr0", 101, -0.005, 1.005);
    TH1F* hMCEffPanelThr10 = new TH1F("hEffMCPanelThr10", "hEffMCPanelThr10", 101, -0.005, 1.005);
    TH1F* hMCEffPanelThr20 = new TH1F("hEffMCPanelThr20", "hEffMCPanelThr20", 101, -0.005, 1.005);
    TH1F* hMCEffPanelThr30 = new TH1F("hEffMCPanelThr30", "hEffMCPanelThr30", 101, -0.005, 1.005);

    TH1F* hDataEffPanelThr0 = new TH1F("hEffDataPanelThr0", "hEffDataPanelThr0", 101, -0.005, 1.005);
    TH1F* hDataEffPanelThr10 = new TH1F("hEffDataPanelThr10", "hEffDataPanelThr10", 101, -0.005, 1.005);
    TH1F* hDataEffPanelThr20 = new TH1F("hEffDataPanelThr20", "hEffDataPanelThr20", 101, -0.005, 1.005);
    TH1F* hDataEffPanelThr30 = new TH1F("hEffDataPanelThr30", "hEffDataPanelThr30", 101, -0.005, 1.005);

    TH1F* hMCEffGapThr0 = new TH1F("hEffMCGapThr0", "hEffMCGapThr0", 101, -0.005, 1.005);
    TH1F* hMCEffGapThr10 = new TH1F("hEffMCGapThr10", "hEffMCGapThr10", 101, -0.005, 1.005);
    TH1F* hMCEffGapThr20 = new TH1F("hEffMCGapThr20", "hEffMCGapThr20", 101, -0.005, 1.005);
    TH1F* hMCEffGapThr30 = new TH1F("hEffMCGapThr30", "hEffMCGapThr30", 101, -0.005, 1.005);

    TH1F* hDataEffGapThr0 = new TH1F("hEffDataGapThr0", "hEffDataGapThr0", 101, -0.005, 1.005);
    TH1F* hDataEffGapThr10 = new TH1F("hEffDataGapThr10", "hEffDataGapThr10", 101, -0.005, 1.005);
    TH1F* hDataEffGapThr20 = new TH1F("hEffDataGapThr20", "hEffDataGapThr20", 101, -0.005, 1.005);
    TH1F* hDataEffGapThr30 = new TH1F("hEffDataGapThr30", "hEffDataGapThr30", 101, -0.005, 1.005);

    ifstream infileData;
    infileData.open("MergedHistograms.user.salaman.physics_Main.DESDM_MCP.RPCMon.251115.v2Total30Nov2015.Jan26_2016.txt");
    ifstream infileMC;
    infileMC.open("MonitorESD_GeantinoHits_test125k_condRun00281411_NewMonTag.effPerPanel.txt");

    unsigned long readPanelId;
    unsigned long effDataMapID[10000];
    int effDataMapNtrk[10000];
    float effDataMapNEffP[10000];
    float effDataMapNEffG[10000];
    float deffDataMapNEffP[10000];
    float deffDataMapNEffG[10000];
    unsigned long effMCMapID[10000];
    int effMCMapNtrk[10000];
    float effMCMapNEffP[10000];
    float effMCMapNEffG[10000];
    float deffMCMapNEffP[10000];
    float deffMCMapNEffG[10000];
    int readNtrkOnPanel, neffp, neffg;

    int nPanelDataTotal = 0;
    int nPanelDataWithTracks = 0;
    int nPanelDataWith0PanelEff = 0;
    int nPanelDataWith1PanelEff = 0;
    int nPanelDataWith0GapEff = 0;
    int nPanelDataWith1GapEff = 0;

    int nPanelMCTotal = 0;
    int nPanelMCWithTracks = 0;
    int nPanelMCWith0PanelEff = 0;
    int nPanelMCWith1PanelEff = 0;
    int nPanelMCWith0GapEff = 0;
    int nPanelMCWith1GapEff = 0;

    std::map<unsigned long, double>* effPhiPanelMap = new std::map<unsigned long, double>();
    std::map<unsigned long, double>* effGapPanelMap = new std::map<unsigned long, double>();
    std::map<unsigned long, double>* ntrkOnPanelMap = new std::map<unsigned long, double>();

    std::cout << "Start reading Data ......................" << std::endl;
    std::string STRING;
    stringstream ss;
    if (infileData.is_open())  // if the file is open
    {
        std::cout << "Eff. from Data - input file is open" << std::endl;
        int ind = 0;
        while (!infileData.eof())  // To get you all the lines.
        {
            ss.clear();
            getline(infileData, STRING);  // Saves the line in STRING.
            ss << STRING;
            ++nPanelDataTotal;

            ss >> readPanelId >> readNtrkOnPanel >> neffp >> neffg;

            effDataMapID[ind] = readPanelId;
            effDataMapNtrk[ind] = readNtrkOnPanel;

            std::map<unsigned long, double>::const_iterator intrk = ntrkOnPanelMap->find(readPanelId);
            if (intrk == (ntrkOnPanelMap->end())) {
                ntrkOnPanelMap->insert(std::pair<unsigned long, double>(readPanelId, (double)readNtrkOnPanel));
            } else
                std::cout << "Filling Eff. Maps for data:  panelID " << readPanelId << " was already found; skip record at row #" << ind + 1
                          << std::endl;

            if (effDataMapNtrk[ind] > 0) {
                ++nPanelDataWithTracks;
                effDataMapNEffP[ind] = float(neffp) / float(effDataMapNtrk[ind]);
                hDataEffPanelThr0->Fill(effDataMapNEffP[ind]);

                if (effPhiPanelMap->find(readPanelId) == effPhiPanelMap->end()) {
                    std::cout << " new entry in effPhiPanelMap - key = " << readPanelId << " efficiency = " << effDataMapNEffP[ind]
                              << std::endl;
                    effPhiPanelMap->insert(std::pair<unsigned long, double>(readPanelId, effDataMapNEffP[ind]));
                } else
                    std::cout << "Filling Eff. Maps for data (pan eff):  panelID " << readPanelId
                              << " was already found; skip record at row #" << ind + 1 << std::endl;
                if ((effGapPanelMap->find(readPanelId)) == (effGapPanelMap->end())) {
                    effGapPanelMap->insert(std::pair<unsigned long, double>(readPanelId, double(neffg) / double(readNtrkOnPanel)));
                } else
                    std::cout << "Filling Eff. Maps for data (gap eff):  panelID " << readPanelId
                              << " was already found; skip record at row #" << ind + 1 << std::endl;

                if (effDataMapNtrk[ind] > 20) {
                    hDataEffPanelThr10->Fill(effDataMapNEffP[ind]);
                    if (effDataMapNtrk[ind] > 40) {
                        hDataEffPanelThr20->Fill(effDataMapNEffP[ind]);
                        if (effDataMapNtrk[ind] > 50) { hDataEffPanelThr30->Fill(effDataMapNEffP[ind]); }
                    }
                }
                if (neffp > 0 && neffp < effDataMapNtrk[ind]) {
                    deffDataMapNEffP[ind] = sqrt(effDataMapNEffP[ind] * (1. - effDataMapNEffP[ind]) / float(effDataMapNtrk[ind]));
                } else if (neffp == 0) {
                    ++nPanelDataWith0PanelEff;
                    float maxeff = 1. / float(effDataMapNtrk[ind]);
                    deffDataMapNEffP[ind] = sqrt(maxeff * (1. - maxeff) / float(effDataMapNtrk[ind]));
                } else if (neffp == effDataMapNtrk[ind]) {
                    ++nPanelDataWith1PanelEff;
                    float mineff = float(effDataMapNtrk[ind] - 1) / float(effDataMapNtrk[ind]);
                    deffDataMapNEffP[ind] = sqrt(mineff * (1. - mineff) / float(effDataMapNtrk[ind]));
                }
            }
            ++ind;
        }
    }
    infileData.close();

    gStyle->SetOptStat(0);

    std::cout << " end of data file reached ... going to showEffMaps - myPanList = " << myPanList << std::endl;
    showEffMaps(1, myPanList, effPhiPanelMap, effGapPanelMap, ntrkOnPanelMap, false);
    showEffMaps(0, myPanList, effPhiPanelMap, effGapPanelMap, ntrkOnPanelMap, false);

    return;
    gStyle->SetOptFit(111111);
    gStyle->SetOptStat(111111);

    cDataEffp->cd(1);
    hDataEffPanelThr0->Draw();
    cDataEffp->cd(2);
    hDataEffPanelThr10->Draw();
    cDataEffp->cd(3);
    hDataEffPanelThr20->Draw();
    cDataEffp->cd(4);
    hDataEffPanelThr30->Draw();
    cDataEffp->SaveAs("panelEff_Data.png");

    TCanvas* cPanEffFit = new TCanvas("panelEff", "Panel Efficiency all");
    drawEffHist(hDataEffPanelThr0, 0.94, 1.);
    hDataEffPanelThr0->Draw();
    gPad->Update();
    TPaveStats* st = (TPaveStats*)hDataEffPanelThr0->FindObject("stats");
    st->SetX1NDC(0.15);  // new x start position
    st->SetX2NDC(0.6);   // new x start position
    st->SetY1NDC(0.5);   // new x start position
    st->SetY2NDC(0.9);   // new x start position
    gPad->Update();
    hDataEffPanelThr0->Draw();
    cPanEffFit->SaveAs("AllPanEffFit0_data.png");

    TCanvas* cPanEffFit50 = new TCanvas("panelEff50", "Panel Efficiency ntrk>50");
    drawEffHist(hDataEffPanelThr30, 0.94, 1.);
    hDataEffPanelThr30->Draw();
    gPad->Update();
    st = (TPaveStats*)hDataEffPanelThr30->FindObject("stats");
    st->SetX1NDC(0.15);  // new x start position
    st->SetX2NDC(0.6);   // new x start position
    st->SetY1NDC(0.5);   // new x start position
    st->SetY2NDC(0.9);   // new x start position
    gPad->Update();
    hDataEffPanelThr30->Draw();
    cPanEffFit50->SaveAs("AllPanEffFit50_data.png");

    Int_t netabins = 28;
    Int_t nphibins = 32;
    Double_t zMinOuter = -13000.;
    Double_t zMaxOuter = 13000.;
    Double_t zMinMiddle = -10000.;
    Double_t zMaxMiddle = 10000.;
    Double_t phiMin = -M_PI;
    Double_t phiMax = M_PI;

    TH2F* h_effmapEtaMC[6];
    h_effmapEtaMC[0] = new TH2F("effmapEtaMClay1", "effmapEtaMClay1", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapEtaMC[1] = new TH2F("effmapEtaMClay2", "effmapEtaMClay2", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapEtaMC[2] = new TH2F("effmapEtaMClay3", "effmapEtaMClay3", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapEtaMC[3] = new TH2F("effmapEtaMClay4", "effmapEtaMClay4", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapEtaMC[4] = new TH2F("effmapEtaMClay5", "effmapEtaMClay5", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    h_effmapEtaMC[5] = new TH2F("effmapEtaMClay6", "effmapEtaMClay6", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    TH2F* h_effmapPhiMC[6];
    h_effmapPhiMC[0] = new TH2F("effmapPhiMClay1", "effmapPhiMClay1", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhiMC[1] = new TH2F("effmapPhiMClay2", "effmapPhiMClay2", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhiMC[2] = new TH2F("effmapPhiMClay3", "effmapPhiMClay3", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhiMC[3] = new TH2F("effmapPhiMClay4", "effmapPhiMClay4", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhiMC[4] = new TH2F("effmapPhiMClay5", "effmapPhiMClay5", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    h_effmapPhiMC[5] = new TH2F("effmapPhiMClay6", "effmapPhiMClay6", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);

    TH2F* h_effmapEtaData[6];
    h_effmapEtaData[0] = new TH2F("effmapEtaDatalay1", "effmapEtaDatalay1", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapEtaData[1] = new TH2F("effmapEtaDatalay2", "effmapEtaDatalay2", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapEtaData[2] = new TH2F("effmapEtaDatalay3", "effmapEtaDatalay3", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapEtaData[3] = new TH2F("effmapEtaDatalay4", "effmapEtaDatalay4", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapEtaData[4] = new TH2F("effmapEtaDatalay5", "effmapEtaDatalay5", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    h_effmapEtaData[5] = new TH2F("effmapEtaDatalay6", "effmapEtaDatalay6", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    TH2F* h_effmapPhiData[6];
    h_effmapPhiData[0] = new TH2F("effmapPhiDatalay1", "effmapPhiDatalay1", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhiData[1] = new TH2F("effmapPhiDatalay2", "effmapPhiDatalay2", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhiData[2] = new TH2F("effmapPhiDatalay3", "effmapPhiDatalay3", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhiData[3] = new TH2F("effmapPhiDatalay4", "effmapPhiDatalay4", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhiData[4] = new TH2F("effmapPhiDatalay5", "effmapPhiDatalay5", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    h_effmapPhiData[5] = new TH2F("effmapPhiDatalay6", "effmapPhiDatalay6", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);

    TH2F* h_effmapEtaRes[6];
    h_effmapEtaRes[0] = new TH2F("effmapEtaReslay1", "effmapEtaReslay1", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapEtaRes[1] = new TH2F("effmapEtaReslay2", "effmapEtaReslay2", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapEtaRes[2] = new TH2F("effmapEtaReslay3", "effmapEtaReslay3", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapEtaRes[3] = new TH2F("effmapEtaReslay4", "effmapEtaReslay4", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapEtaRes[4] = new TH2F("effmapEtaReslay5", "effmapEtaReslay5", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    h_effmapEtaRes[5] = new TH2F("effmapEtaReslay6", "effmapEtaReslay6", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    TH2F* h_effmapPhiRes[6];
    h_effmapPhiRes[0] = new TH2F("effmapPhiReslay1", "effmapPhiReslay1", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhiRes[1] = new TH2F("effmapPhiReslay2", "effmapPhiReslay2", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhiRes[2] = new TH2F("effmapPhiReslay3", "effmapPhiReslay3", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhiRes[3] = new TH2F("effmapPhiReslay4", "effmapPhiReslay4", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhiRes[4] = new TH2F("effmapPhiReslay5", "effmapPhiReslay5", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    h_effmapPhiRes[5] = new TH2F("effmapPhiReslay6", "effmapPhiReslay6", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);

    TH2F* h_effmapEtaResNorm[6];
    h_effmapEtaResNorm[0] =
        new TH2F("effmapEtaResNormlay1", "effmapEtaResNormlay1", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapEtaResNorm[1] =
        new TH2F("effmapEtaResNormlay2", "effmapEtaResNormlay2", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapEtaResNorm[2] =
        new TH2F("effmapEtaResNormlay3", "effmapEtaResNormlay3", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapEtaResNorm[3] =
        new TH2F("effmapEtaResNormlay4", "effmapEtaResNormlay4", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapEtaResNorm[4] =
        new TH2F("effmapEtaResNormlay5", "effmapEtaResNormlay5", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    h_effmapEtaResNorm[5] =
        new TH2F("effmapEtaResNormlay6", "effmapEtaResormlay6", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    TH2F* h_effmapPhiResNorm[6];
    h_effmapPhiResNorm[0] =
        new TH2F("effmapPhiResNormlay1", "effmapPhiResNormlay1", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhiResNorm[1] =
        new TH2F("effmapPhiResNormlay2", "effmapPhiResNormlay2", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhiResNorm[2] =
        new TH2F("effmapPhiResNormlay3", "effmapPhiResNormlay3", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhiResNorm[3] =
        new TH2F("effmapPhiResNormlay4", "effmapPhiResNormlay4", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhiResNorm[4] =
        new TH2F("effmapPhiResNormlay5", "effmapPhiResNormlay5", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    h_effmapPhiResNorm[5] =
        new TH2F("effmapPhiResNormlay6", "effmapPhiResNormlay6", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);

    TH1F* hres = new TH1F("MCdataEffRes", "MCdataEffRes", 101, -1.01, 1.01);
    TH1F* hresNorm = new TH1F("MCdataEffResNorm", "MCdataEffResNorm", 201, -10.05, 10.05);

    TH1F* hres25 = new TH1F("MCdataEffRes25", "MCdataEffRes25", 101, -1.01, 1.01);
    TH1F* hresNorm25 = new TH1F("MCdataEffResNorm25", "MCdataEffResNorm25", 201, -10.05, 10.05);

    myPanList->setAcceptProximityID(false);
    std::cout << "Accept proximity ID flag is set to " << myPanList->getAcceptProximityID() << std::endl;

    std::cout << "Start reading MC ......................" << std::endl;
    if (infileMC.is_open())  // if the file is open
    {
        std::cout << "Eff. from MC - input file is open" << std::endl;
        int ind = 0;
        while (!infileMC.eof())  // To get you all the lines.
        {
            ss.clear();
            getline(infileMC, STRING);  // Saves the line in STRING.
            ss << STRING;
            ++nPanelMCTotal;
            ss >> effMCMapID[ind] >> effMCMapNtrk[ind] >> neffp >> neffg;
            if (effMCMapNtrk[ind] > 0) {
                ++nPanelMCWithTracks;
                effMCMapNEffP[ind] = float(neffp) / float(effMCMapNtrk[ind]);
                hMCEffPanelThr0->Fill(effMCMapNEffP[ind]);
                if (effMCMapNtrk[ind] > 20) {
                    hMCEffPanelThr10->Fill(effMCMapNEffP[ind]);
                    if (effMCMapNtrk[ind] > 40) {
                        hMCEffPanelThr20->Fill(effMCMapNEffP[ind]);
                        if (effMCMapNtrk[ind] > 50) { hMCEffPanelThr30->Fill(effMCMapNEffP[ind]); }
                    }
                }
                if (neffp > 0 && neffp < effMCMapNtrk[ind]) {
                    deffMCMapNEffP[ind] = sqrt(effMCMapNEffP[ind] * (1. - effMCMapNEffP[ind]) / float(effMCMapNtrk[ind]));
                } else if (neffp == 0) {
                    ++nPanelMCWith0PanelEff;
                    float maxeff = 1. / float(effMCMapNtrk[ind]);
                    deffMCMapNEffP[ind] = sqrt(maxeff * (1. - maxeff) / float(effMCMapNtrk[ind]));
                } else if (neffp == effMCMapNtrk[ind]) {
                    ++nPanelMCWith1PanelEff;
                    float mineff = float(effMCMapNtrk[ind] - 1) / float(effMCMapNtrk[ind]);
                    deffMCMapNEffP[ind] = sqrt(mineff * (1. - mineff) / float(effMCMapNtrk[ind]));
                }

                // start compare MC and data
                if (effMCMapNtrk[ind] > 25) {
                    unsigned long MCid = effMCMapID[ind];
                    int indDT = getIndWithID(MCid, effDataMapID, nPanelDataTotal);
                    if (indDT < 0) { continue; }
                    if (effMCMapID[ind] == effDataMapID[indDT]) {
                        float resEff = effMCMapNEffP[ind] - effDataMapNEffP[indDT];
                        float resEffNorm =
                            resEff / sqrt(deffMCMapNEffP[ind] * deffMCMapNEffP[ind] + deffDataMapNEffP[indDT] * deffDataMapNEffP[indDT]);
                        hres->Fill(resEff);
                        hresNorm->Fill(resEffNorm);
                        if (effDataMapNtrk[indDT] > 25) {
                            hres25->Fill(resEff);
                            hresNorm25->Fill(resEffNorm);
                            unsigned long ID = MCid;
                            unsigned short thisLayer = 0;
                            thisLayer = myPanList->getLayer(ID);
                            if (thisLayer == 0) {
                                std::cout << "Id " << ID << " not found in the map ... why ? " << std::endl;
                                continue;
                            }
                            unsigned short thisView = 2;
                            thisView = myPanList->getView(ID);
                            double phiP = 0.5 * (myPanList->getPhiMax(ID) + myPanList->getPhiMin(ID));
                            if (phiP > M_PI) phiP = phiP - 2. * M_PI;
                            if (phiP < -M_PI) phiP = phiP + 2. * M_PI;
                            double zP = 0.5 * (myPanList->getZMax(ID) + myPanList->getZMin(ID));
                            if (thisLayer < 1 || thisLayer > 6) {
                                std::cout << " Must skip THISLAYER = " << thisLayer << " out of range 1-6" << std::endl;
                                continue;
                            }
                            if (thisView == 1) {
                                // phi
                                h_effmapPhiMC[thisLayer - 1]->Fill(zP, phiP, effMCMapNEffP[ind]);
                                h_effmapPhiData[thisLayer - 1]->Fill(zP, phiP, effDataMapNEffP[ind]);
                                h_effmapPhiRes[thisLayer - 1]->Fill(zP, phiP, resEff);
                                h_effmapPhiResNorm[thisLayer - 1]->Fill(zP, phiP, resEffNorm);
                            } else {
                                // eta
                                h_effmapEtaMC[thisLayer - 1]->Fill(zP, phiP, effMCMapNEffP[ind]);
                                h_effmapEtaData[thisLayer - 1]->Fill(zP, phiP, effDataMapNEffP[ind]);
                                h_effmapEtaRes[thisLayer - 1]->Fill(zP, phiP, resEff);
                                h_effmapEtaResNorm[thisLayer - 1]->Fill(zP, phiP, resEffNorm);
                            }
                        }
                    } else {
                        std::cout << "not the same order in data and mc" << ind << " ID mc = " << effMCMapID[ind]
                                  << " ID data = " << effDataMapID[ind] << std::endl;
                    }

                }  // end compare MC and data
            }
            ++ind;
        }
    }
    infileMC.close();
    cMCEffp->cd(1);
    hMCEffPanelThr0->Draw();
    cMCEffp->cd(2);
    hMCEffPanelThr10->Draw();
    cMCEffp->cd(3);
    hMCEffPanelThr20->Draw();
    cMCEffp->cd(4);
    hMCEffPanelThr30->Draw();
    cMCEffp->SaveAs("panelEff_MC.png");

    TCanvas* cPanEffFitMC = new TCanvas("panelEffMC", "Panel Efficiency all MC");
    drawEffHist(hMCEffPanelThr0, 0.94, 1.);
    hMCEffPanelThr0->Draw();
    gPad->Update();
    st = (TPaveStats*)hMCEffPanelThr0->FindObject("stats");
    st->SetX1NDC(0.15);  // new x start position
    st->SetX2NDC(0.6);   // new x start position
    st->SetY1NDC(0.5);   // new x start position
    st->SetY2NDC(0.9);   // new x start position
    gPad->Update();
    hMCEffPanelThr0->Draw();
    cPanEffFitMC->SaveAs("AllPanEffFit0_mc.png");

    TCanvas* cPanEffFit50MC = new TCanvas("panelEff50MC", "Panel Efficiency ntrk>50 MC");
    drawEffHist(hMCEffPanelThr30, 0.94, 1.);
    hMCEffPanelThr30->Draw();
    gPad->Update();
    st = (TPaveStats*)hMCEffPanelThr30->FindObject("stats");
    st->SetX1NDC(0.15);  // new x start position
    st->SetX2NDC(0.6);   // new x start position
    st->SetY1NDC(0.5);   // new x start position
    st->SetY2NDC(0.9);   // new x start position
    gPad->Update();
    hMCEffPanelThr30->Draw();
    cPanEffFit50MC->SaveAs("AllPanEffFit50_mc.png");

    TCanvas* creseff = new TCanvas("creseff", "creseff", 800, 800);
    creseff->Divide(2, 2);
    creseff->cd(1);
    hres->Draw();
    creseff->cd(2);
    hresNorm->Draw();
    creseff->cd(3);
    hres25->Draw();
    creseff->cd(4);
    hresNorm25->Draw();
    creseff->SaveAs("reseff.png");

    for (short j = 0; j < 6; ++j) {
        showMapEffMCData(j + 1, true, h_effmapPhiMC[j], h_effmapPhiData[j], h_effmapPhiRes[j], h_effmapPhiResNorm[j]);
        showMapEffMCData(j + 1, false, h_effmapEtaMC[j], h_effmapEtaData[j], h_effmapEtaRes[j], h_effmapEtaResNorm[j]);
    }

    std::cout << "MC   ---  Panels in Total                     " << nPanelMCTotal << "\n"
              << "MC   ---  with tracks extrapolated on surface " << nPanelMCWithTracks << "\n"
              << "MC   ---  with 0 panel-eff                    " << nPanelMCWith0PanelEff << "\n"
              << "MC   ---  with 0 gap-eff                      " << nPanelMCWith0GapEff << "\n"
              << "MC   ---  with 1 panel-eff                    " << nPanelMCWith1PanelEff << "\n"
              << "MC   ---  with 1 gap-eff                      " << nPanelMCWith1GapEff << "\n";
    std::cout << "DATA ---  Panels in Total                     " << nPanelDataTotal << "\n"
              << "DATA ---  with tracks extrapolated on surface " << nPanelDataWithTracks << "\n"
              << "DATA ---  with 0 panel-eff                    " << nPanelDataWith0PanelEff << "\n"
              << "DATA ---  with 0 gap-eff                      " << nPanelDataWith0GapEff << "\n"
              << "DATA ---  with 1 panel-eff                    " << nPanelDataWith1PanelEff << "\n"
              << "DATA ---  with 1 gap-eff                      " << nPanelDataWith1GapEff << "\n";
}

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include "ChamberNames.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TF1.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TLegend.h"
#include "TPaveStats.h"
#include "TStyle.h"

int main() {
    ChamberNames chNames("chambers.txt");

    const int NFILES = 8;
    // const int NFILES=5;

    const string basedir = "/tmp/roberth/";
    const string dirs[NFILES] = {
        "group.det-muon.ZMUMU.20110609_EE-BIS8_iter0_V21",
        "group.det-muon.ZMUMU.20110609_EE-BIS8_iter1_V22",
        "group.det-muon.ZMUMU.20110609_EE-BIS8_iter2_V22",
        "group.det-muon.ZMUMU.20110609_EE-BIS8_iter3_V22",
        "group.det-muon.ZMUMU.20110609_EE-BIS8_iter4_V22",
        "group.det-muon.ZMUMU.20110609_EE-BIS8_iter5_V22",
        "group.det-muon.ZMUMU.20110609_EE-BIS8_iter6_V22",
        "group.det-muon.ZMUMU.20110609_EE-BIS8_iter7_V24"
        /*
        "group.det-muon.TMU15GEV.20110310_EE-BIS8_iter0_V16.110428220005",
        //"group.det-muon.TMU15GEV.20110310_EE-BIS8_iter1_V19.110524092107",
        //"group.det-muon.TMU15GEV.20110310_EE-BIS8_iter2_V19.110524114712",
        "group.det-muon.TMU15GEV.20110310_EE-BIS8_iter3_V19.110524234018",
        //"group.det-muon.TMU15GEV.20110310_EE-BIS8_iter4_V19.110525092140",
        //"group.det-muon.TMU15GEV.20110310_EE-BIS8_iter5_V19.110525150617",
        "group.det-muon.TMU15GEV.20110310_EE-BIS8_iter6_V19.110526103932",
        //"group.det-muon.TMU15GEV.20110310_EE-BIS8_iter7_V19.110526174338",
        //"group.det-muon.TMU15GEV.20110310_EE-BIS8_iter8_V19.110526211853",
        "group.det-muon.TMU15GEV.20110310_EE-BIS8_iter9_V19.110527014843",
        //"group.det-muon.TMU15GEV.20110310_EE-BIS8_iter10_V19.110527113253",
        "group.det-muon.TMU15GEV.20110310_EE-BIS8_iter12_V19.110527231239",
        "group.det-muon.TMU15GEV.20110310_EE-BIS8_iter15_V19.110529003002",
        "group.det-muon.TMU15GEV.20110310_EE-BIS8_iter18_V19.110529215537",
        "group.det-muon.TMU15GEV.20110310_EE-BIS8_iter21_V19.110606125252"
        */
    };

    /*
    const string dirs[NFILES]={
      "group.det-muon.TMU15GEV.20110310_EE-BIS8_iter17_V19.110529184925",
      "group.det-muon.TMU15GEV.20110310_EE-BIS8_iter18_V19.110529215537",
      "group.det-muon.TMU15GEV.20110310_EE-BIS8_iter19_V19.110529235409",
      "group.det-muon.TMU15GEV.20110310_EE-BIS8_iter20_V19.110530133814",
      "group.det-muon.TMU15GEV.20110310_EE-BIS8_iter21_V19.110606125252"};
    */

    const int iters[NFILES] = {0, 1, 2, 3, 4, 5, 6, 7};
    // const int iters[NFILES]={0,3,6,9,12,15,18,21};
    // const int iters[NFILES]={17,18,19,20,21};

    const std::string plottypes[6] = {"BEEA", "BEEC", "EELA", "EELC", "BIS8A", "BIS8C"};
    int** beecolor = new int*[2];
    int** eelcolor = new int*[2];
    int** bis8color = new int*[2];
    for (int ieta = 0; ieta < 2; ieta++) {
        beecolor[ieta] = new int[8];
        eelcolor[ieta] = new int[8];
        bis8color[ieta] = new int[8];
    }

    const int phicolor[16] = {kRed + 4, kRed + 2,  kRed,      kMagenta + 4, kMagenta + 2, kMagenta,   kBlue + 4, kBlue + 2,
                              kBlue,    kCyan + 4, kCyan + 2, kCyan,        kGreen + 4,   kGreen + 2, kGreen,    kYellow + 2};

    for (int iplot = 0; iplot < 6; iplot++) {
        const std::string plottype = plottypes[iplot];

        std::vector<double> traz0;
        std::vector<double> trat0;
        std::vector<double> rots0;
        std::vector<double> rotz0;
        std::vector<double> rott0;
        std::string dummy;
        char ALineMarker;
        int jff, jzz, job;
        std::string type;
        double tras, traz, trat, rots, rotz, rott;

        std::vector<std::string> names;
        std::vector<int> etas;
        std::vector<int> phis;
        std::vector<TGraph*> graphs_traz;
        std::vector<TGraph*> graphs_trat;
        std::vector<TGraph*> graphs_rots;
        std::vector<TGraph*> graphs_rotz;
        std::vector<TGraph*> graphs_rott;

        double minz(9.), mint(9.), minrs(9.), minrz(9.), minrt(9.);
        double maxz(-9.), maxt(-9.), maxrs(-9.), maxrz(-9.), maxrt(-9.);

        for (int ifile = 0; ifile < NFILES; ifile++) {
            int iter = iters[ifile];

            std::stringstream filename;
            filename << "outputAszt_iter" << iter << ".txt";
            std::ifstream file(filename.str().c_str());
            file >> dummy;
            int ich = 0;
            while (file >> ALineMarker >> type >> jff >> jzz >> job >> tras >> traz >> trat >> rots >> rotz >> rott) {
                if (plottype == "BEEA" && (type != "BEE" || jzz < 0)) continue;
                if (plottype == "BEEC" && (type != "BEE" || jzz > 0)) continue;
                if (plottype == "EELA" && (type != "EEL" || jzz < 0)) continue;
                if (plottype == "EELC" && (type != "EEL" || jzz > 0)) continue;
                if (plottype == "BIS8A" && (type != "BIS" || jzz != 8)) continue;
                if (plottype == "BIS8C" && (type != "BIS" || jzz != -8)) continue;

                // std::cout<<ich<<" "<<type<<" "<<jff<<" "<<jzz<<" "<<tras<<std::endl;

                if (traz < minz) minz = traz;
                if (traz > maxz) maxz = traz;
                if (trat < mint) mint = trat;
                if (trat > maxt) maxt = trat;
                if (rots < minrs) minrs = rots;
                if (rots > maxrs) maxrs = rots;
                if (rotz < minrz) minrz = rotz;
                if (rotz > maxrz) maxrz = rotz;
                if (rott < minrt) minrt = rott;
                if (rott > maxrt) maxrt = rott;

                if (ifile == 0) {
                    std::stringstream chname;
                    chname << type << std::abs(jzz);
                    if (jzz < 0)
                        chname << "C";
                    else
                        chname << "A";
                    int detphi = 2 * jff;
                    if (type.substr(2, 1) == "L")
                        detphi--;
                    else if (type.substr(2, 1) != "S" && type != "BEE")
                        detphi = jff;
                    chname << std::setfill('0') << std::setw(2) << detphi;

                    // std::cout<<"chname: "<<chname.str()<<std::endl;

                    names.push_back(chname.str());
                    etas.push_back(std::abs(jzz) - 1);
                    phis.push_back(jff - 1);
                    graphs_traz.push_back(new TGraph(NFILES));
                    graphs_trat.push_back(new TGraph(NFILES));
                    graphs_rots.push_back(new TGraph(NFILES));
                    graphs_rotz.push_back(new TGraph(NFILES));
                    graphs_rott.push_back(new TGraph(NFILES));
                }
                graphs_traz[ich]->SetPoint(ifile, iter, traz);
                graphs_trat[ich]->SetPoint(ifile, iter, trat);
                graphs_rots[ich]->SetPoint(ifile, iter, rots * 1000.);
                graphs_rotz[ich]->SetPoint(ifile, iter, rotz * 1000.);
                graphs_rott[ich]->SetPoint(ifile, iter, rott * 1000.);

                graphs_traz[ich]->SetTitle("z");
                graphs_trat[ich]->SetTitle("t");
                graphs_rots[ich]->SetTitle("rot s");
                graphs_rotz[ich]->SetTitle("rot z");
                graphs_rott[ich]->SetTitle("rot t");
                ich++;
            }
        }

        TCanvas* canv = new TCanvas("canv", "canv");
        TLegend* leg = new TLegend(.1, .1, .9, .9);
        canv->SetFillColor(10);
        canv->Divide(3, 2);
        for (int i = 0; i < 5; i++) {
            std::vector<TGraph*>& gr = graphs_traz;
            double max(0.), min(0.);
            switch (i) {
                case 0:
                    gr = graphs_traz;
                    max = maxz;
                    min = minz;
                    break;
                case 1:
                    gr = graphs_trat;
                    max = maxt;
                    min = mint;
                    break;
                case 2:
                    gr = graphs_rots;
                    max = maxrs;
                    min = minrs;
                    break;
                case 3:
                    gr = graphs_rotz;
                    max = maxrz;
                    min = minrz;
                    break;
                case 4:
                    gr = graphs_rott;
                    max = maxrt;
                    min = minrt;
                    break;
            }

            if (max < 0.) max = 0.;
            if (min > 0.) min = 0.;
            if (i >= 2) {
                max *= 1000.;
                min *= 1000.;
            }

            // std::cout<<"i/min/max: "<<i<<"/"<<min<<"/"<<max<<std::endl;

            canv->cd(i + 2);
            for (int ich = 0; ich < (int)names.size(); ich++) {
                gr[ich]->SetMaximum(1.2 * max);
                gr[ich]->SetMinimum(1.2 * min);
                gr[ich]->SetMarkerSize(.4);
                int color = (plottype.substr(0, 3) == "EEL") ? phicolor[ich * 2] : phicolor[ich];

                // set color for later use for BEE and EEL plots
                int ieta = etas[ich];
                int iphi = phis[ich];
                if (plottype.substr(0, 3) == "EEL")
                    eelcolor[ieta][iphi] = color;
                else if (plottype.substr(0, 3) == "BEE")
                    beecolor[ieta][iphi] = color;
                else
                    bis8color[0][iphi] = color;

                gr[ich]->SetMarkerColor(color);
                gr[ich]->SetMarkerStyle(8);
                gr[ich]->SetLineColor(color);
                // gr[ich]->SetLineWidth(1.6);
                gr[ich]->SetMarkerSize(.7);
                if (ich == 0)
                    gr[ich]->Draw("APL");
                else
                    gr[ich]->Draw("PLsame");
                gr[ich]->GetXaxis()->SetTitle("iteration");
                if (i < 2)
                    gr[ich]->GetYaxis()->SetTitle("mm");
                else
                    gr[ich]->GetYaxis()->SetTitle("mrad");
                if (i == 0) leg->AddEntry(gr[ich], names[ich].c_str(), "lp");
            }
        }
        canv->cd(1);
        leg->Draw();
        canv->Modified();

        if (plottype == "BEEA")
            canv->Print("BEEA_trend.eps");
        else if (plottype == "BEEC")
            canv->Print("BEEC_trend.eps");
        else if (plottype == "EELA")
            canv->Print("EELA_trend.eps");
        else if (plottype == "EELC")
            canv->Print("EELC_trend.eps");
        else if (plottype == "BIS8A")
            canv->Print("BIS8A_trend.eps");
        else if (plottype == "BIS8C")
            canv->Print("BIS8C_trend.eps");

        delete canv;
        delete leg;
    }

    // get residuals
    TH1F*** allBeeRes = new TH1F**[NFILES];
    TH1F*** allEelRes = new TH1F**[NFILES];
    TH1F***** beeRes = new TH1F****[NFILES];
    TH1F***** eelRes = new TH1F****[NFILES];
    TH1F***** bis8Res = new TH1F****[NFILES];
    string ***hbeenames(0), ***heelnames(0), ***hbis8names(0);

    TGraph* nTracksGr = new TGraph(NFILES);
    TGraph* nTracksBEEAGr = new TGraph(NFILES);
    TGraph* nTracksBEECGr = new TGraph(NFILES);
    TGraph* nTracksEELAGr = new TGraph(NFILES);
    TGraph* nTracksEELCGr = new TGraph(NFILES);
    TGraph* nTracksBIS8AGr = new TGraph(NFILES);
    TGraph* nTracksBIS8CGr = new TGraph(NFILES);
    TGraph* trackChi2Gr = new TGraph(NFILES);
    TGraph* trackChi2BEEAGr = new TGraph(NFILES);
    TGraph* trackChi2BEECGr = new TGraph(NFILES);
    TGraph* trackChi2EELAGr = new TGraph(NFILES);
    TGraph* trackChi2EELCGr = new TGraph(NFILES);
    TGraph* trackChi2BIS8AGr = new TGraph(NFILES);
    TGraph* trackChi2BIS8CGr = new TGraph(NFILES);

    nTracksGr->SetName("nTracks");
    nTracksBEEAGr->SetName("nTracksBEEA");
    nTracksBEECGr->SetName("nTracksBEEC");
    nTracksEELAGr->SetName("nTracksEELA");
    nTracksEELCGr->SetName("nTracksEELC");
    nTracksBIS8AGr->SetName("nTracksBIS8A");
    nTracksBIS8CGr->SetName("nTracksBIS8C");
    trackChi2Gr->SetName("trackChi2");
    trackChi2BEEAGr->SetName("trackChi2BEEA");
    trackChi2BEECGr->SetName("trackChi2BEEC");
    trackChi2EELAGr->SetName("trackChi2EELA");
    trackChi2EELCGr->SetName("trackChi2EELC");
    trackChi2BIS8AGr->SetName("trackChi2BIS8A");
    trackChi2BIS8CGr->SetName("trackChi2BIS8C");

    for (int ifile = 0; ifile < NFILES; ifile++) {
        int iter = iters[ifile];
        stringstream iterstr;
        iterstr << "iter" << iter;

        stringstream files;
        files << basedir << dirs[ifile] << "/iter" << iter << "/*";
        TChain* tree = new TChain("MuonTrackPreProcessor");
        std::cout << "adding " << files.str() << " to TChain" << std::endl;
        tree->Add(files.str().c_str());

        int nChambers, *chamberId(new int[50]), nresiduals, *resIndex(new int[50]);
        double* residuals(new double[100]);
        double trackChi2;
        tree->SetBranchAddress("nChambers", &nChambers);
        tree->SetBranchAddress("chamberId", chamberId);
        tree->SetBranchAddress("nresiduals", &nresiduals);
        tree->SetBranchAddress("resIndex", resIndex);
        tree->SetBranchAddress("residuals", residuals);
        tree->SetBranchAddress("trackChi2", &trackChi2);

        allBeeRes[ifile] = new TH1F*[2];
        allEelRes[ifile] = new TH1F*[2];
        beeRes[ifile] = new TH1F***[2];
        eelRes[ifile] = new TH1F***[2];
        bis8Res[ifile] = new TH1F***[2];
        if (ifile == 0) {
            hbeenames = new string**[2];
            heelnames = new string**[2];
            hbis8names = new string**[2];
        }
        // make histograms
        for (int isideAC = 0; isideAC < 2; isideAC++) {
            // all BEE/EEL
            stringstream allEEStr;
            if (isideAC == 0)
                allEEStr << "sideA_";
            else
                allEEStr << "sideC_";

            string allBeeName = allEEStr.str() + iterstr.str();
            allBeeRes[ifile][isideAC] = new TH1F(("allBEE_" + allBeeName).c_str(), ("allBEE_" + allBeeName).c_str(), 100, -6., 6.);
            allEelRes[ifile][isideAC] = new TH1F(("allEEL_" + allBeeName).c_str(), ("allEEL_" + allBeeName).c_str(), 100, -6., 6.);

            // individual BEE/EEL
            beeRes[ifile][isideAC] = new TH1F**[2];
            eelRes[ifile][isideAC] = new TH1F**[2];
            bis8Res[ifile][isideAC] = new TH1F**[2];
            if (ifile == 0) {
                hbeenames[isideAC] = new string*[2];
                heelnames[isideAC] = new string*[2];
                hbis8names[isideAC] = new string*[2];
            }
            for (int ieta = 0; ieta < 2; ieta++) {
                beeRes[ifile][isideAC][ieta] = new TH1F*[8];
                eelRes[ifile][isideAC][ieta] = new TH1F*[8];
                bis8Res[ifile][isideAC][ieta] = new TH1F*[8];
                if (ifile == 0) {
                    hbeenames[isideAC][ieta] = new string[8];
                    heelnames[isideAC][ieta] = new string[8];
                    hbis8names[isideAC][ieta] = new string[8];
                }
                for (int iphi = 0; iphi < 8; iphi++) {
                    stringstream beename;
                    beename << "BEE" << ieta + 1;
                    if (isideAC == 0)
                        beename << "A";
                    else
                        beename << "C";
                    beename << setfill('0') << setw(2) << (iphi + 1) * 2;
                    beeRes[ifile][isideAC][ieta][iphi] =
                        new TH1F((beename.str() + "_" + iterstr.str()).c_str(), beename.str().c_str(), 100, -6., 6.);
                    if (ifile == 0) hbeenames[isideAC][ieta][iphi] = beename.str();

                    stringstream eelname;
                    eelname << "EEL" << ieta + 1;
                    if (isideAC == 0)
                        eelname << "A";
                    else
                        eelname << "C";
                    eelname << setfill('0') << setw(2) << (iphi + 1) * 2 - 1;

                    eelRes[ifile][isideAC][ieta][iphi] =
                        new TH1F((eelname.str() + "_" + iterstr.str()).c_str(), eelname.str().c_str(), 100, -1., 1.);
                    if (ifile == 0) heelnames[isideAC][ieta][iphi] = eelname.str();

                    if (ieta == 0) {
                        stringstream bis8name;
                        bis8name << "BIS8";
                        if (isideAC == 0)
                            bis8name << "A";
                        else
                            bis8name << "C";
                        bis8name << setfill('0') << setw(2) << (iphi + 1) * 2;
                        bis8Res[ifile][isideAC][ieta][iphi] =
                            new TH1F((bis8name.str() + "_" + iterstr.str()).c_str(), bis8name.str().c_str(), 100, -1., 1.);
                        if (ifile == 0) hbis8names[isideAC][ieta][iphi] = bis8name.str();
                    }
                }
            }
        }

        int nTracks = 0;
        int nTracksBEEA = 0;
        int nTracksBEEC = 0;
        int nTracksEELA = 0;
        int nTracksEELC = 0;
        int nTracksBIS8A = 0;
        int nTracksBIS8C = 0;
        double totalTrackChi2 = 0.;
        double totalTrackChi2BEEA = 0.;
        double totalTrackChi2BEEC = 0.;
        double totalTrackChi2EELA = 0.;
        double totalTrackChi2EELC = 0.;
        double totalTrackChi2BIS8A = 0.;
        double totalTrackChi2BIS8C = 0.;

        // loop over entries and fill histograms
        std::cout << "tree has " << tree->GetEntries() << " entries" << std::endl;
        for (int ientry = 0; ientry < (int)tree->GetEntries(); ientry++) {
            // cout<<"ientry: "<<ientry<<endl;
            tree->GetEntry(ientry);

            string beeName = "";
            string eelName = "";
            string bis8Name = "";
            int firstBEERes(0), lastBEERes(0);
            int firstEELRes(0), lastEELRes(0);
            int firstBIS8Res(0), lastBIS8Res(0);
            for (int ich = 0; ich < nChambers; ich++) {
                int id = chamberId[ich];
                std::string name = chNames.chamberName(id);
                // std::cout<<"name: "<<name<<std::endl;

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
                if (name.substr(0, 4) == "BIS8") {
                    bis8Name = name;
                    firstBIS8Res = resIndex[ich];
                    lastBIS8Res = (ich + 1 < nChambers) ? resIndex[ich + 1] : nresiduals;
                }
            }

            // remove tracks not passing selection in MuonTrackPreProcessor
            if (beeName == "" && eelName == "" && bis8Name == "") { continue; }

            /*
            if (bis8Name!=""||eelName!="") {
              cout<<"                 ";
              for (int ich=0;ich<nChambers;ich++) {
                int id=chamberId[ich];
                std::string name=chNames.chamberName(id);
                if (ich==0) cout<<name;
                else        cout<<", "<<name;
              }
              cout<<endl;
            }
            */
            // cout<<"beeName: "<<beeName<<", eelName: "<<eelName<<endl;

            if (beeName != "") {
                int stPhi = chNames.stPhi(beeName);
                int stEta = chNames.stEta(beeName);
                int isideAC = (stEta > 0) ? 0 : 1;
                int ieta = std::abs(stEta) - 1;
                int iphi = stPhi - 1;
                for (int ires = firstBEERes; ires < lastBEERes; ires++) {
                    beeRes[ifile][isideAC][ieta][iphi]->Fill(residuals[ires]);
                    allBeeRes[ifile][isideAC]->Fill(residuals[ires]);
                    // cout<<"BEE res: "<<residuals[ires]<<endl;
                }

                if (isideAC == 0) {
                    totalTrackChi2BEEA += trackChi2;
                    nTracksBEEA++;
                } else {
                    totalTrackChi2BEEC += trackChi2;
                    nTracksBEEC++;
                }
            }
            if (eelName != "") {
                int stPhi = chNames.stPhi(eelName);
                int stEta = chNames.stEta(eelName);
                int isideAC = (stEta > 0) ? 0 : 1;
                int ieta = std::abs(stEta) - 1;
                int iphi = stPhi - 1;
                for (int ires = firstEELRes; ires < lastEELRes; ires++) {
                    eelRes[ifile][isideAC][ieta][iphi]->Fill(residuals[ires]);
                    allEelRes[ifile][isideAC]->Fill(residuals[ires]);
                    // cout<<"EEL res: "<<residuals[ires]<<endl;
                }

                if (isideAC == 0) {
                    totalTrackChi2EELA += trackChi2;
                    nTracksEELA++;
                } else {
                    totalTrackChi2EELC += trackChi2;
                    nTracksEELC++;
                }
            }
            if (bis8Name != "") {
                int stPhi = chNames.stPhi(bis8Name);
                int stEta = chNames.stEta(bis8Name);
                int isideAC = (stEta > 0) ? 0 : 1;
                int ieta = 0;
                int iphi = stPhi - 1;
                for (int ires = firstBIS8Res; ires < lastBIS8Res; ires++) { bis8Res[ifile][isideAC][ieta][iphi]->Fill(residuals[ires]); }
                if (isideAC == 0) {
                    totalTrackChi2BIS8A += trackChi2;
                    nTracksBIS8A++;
                } else {
                    totalTrackChi2BIS8C += trackChi2;
                    nTracksBIS8C++;
                }
            }

            totalTrackChi2 += trackChi2;
            nTracks++;

        }  // loop over tree entries

        // fill graphs
        nTracksGr->SetPoint(ifile, (double)iter, (double)nTracks);
        nTracksBEEAGr->SetPoint(ifile, (double)iter, (double)nTracksBEEA);
        nTracksBEECGr->SetPoint(ifile, (double)iter, (double)nTracksBEEC);
        nTracksEELAGr->SetPoint(ifile, (double)iter, (double)nTracksEELA);
        nTracksEELCGr->SetPoint(ifile, (double)iter, (double)nTracksEELC);
        nTracksBIS8AGr->SetPoint(ifile, (double)iter, (double)nTracksBIS8A);
        nTracksBIS8CGr->SetPoint(ifile, (double)iter, (double)nTracksBIS8C);
        trackChi2Gr->SetPoint(ifile, (double)iter, totalTrackChi2 / (double)nTracks);
        trackChi2BEEAGr->SetPoint(ifile, (double)iter, totalTrackChi2BEEA / (double)nTracksBEEA);
        trackChi2BEECGr->SetPoint(ifile, (double)iter, totalTrackChi2BEEC / (double)nTracksBEEC);
        trackChi2EELAGr->SetPoint(ifile, (double)iter, totalTrackChi2EELA / (double)nTracksEELA);
        trackChi2EELCGr->SetPoint(ifile, (double)iter, totalTrackChi2EELC / (double)nTracksEELC);
        trackChi2BIS8AGr->SetPoint(ifile, (double)iter, totalTrackChi2BIS8A / (double)nTracksBIS8A);
        trackChi2BIS8CGr->SetPoint(ifile, (double)iter, totalTrackChi2BIS8C / (double)nTracksBIS8C);

    }  // ifile

    TGraph* nTracksGrArr[7] = {nTracksGr, nTracksBEEAGr, nTracksBEECGr, nTracksEELAGr, nTracksEELCGr, nTracksBIS8AGr, nTracksBIS8CGr};
    string nTracksTitles[7] = {"# tracks",
                               "# BEE tracks - A side",
                               "# BEE tracks - C side",
                               "# EEL tracks - A side",
                               "# EEL tracks - C side",
                               "# BIS8 tracks - A side",
                               "# BIS8 tracks - C side"};

    TGraph* trackChi2GrArr[7] = {trackChi2Gr,     trackChi2BEEAGr,  trackChi2BEECGr, trackChi2EELAGr,
                                 trackChi2EELCGr, trackChi2BIS8AGr, trackChi2BIS8CGr};
    string trackChi2Titles[7] = {"average track #chi^{2}",
                                 "average track #chi^{2} -BEE A side",
                                 "average track #chi^{2} -BEE C side",
                                 "average track #chi^{2} -EEL A side",
                                 "average track #chi^{2} -EEL C side",
                                 "average track #chi^{2} -BIS8 A side",
                                 "average track #chi^{2} -BIS8 C side"};

    for (int igr = 0; igr < 7; igr++) {
        for (int itype = 0; itype < 2; itype++) {
            TCanvas* canvGr = new TCanvas("gr", "gr");
            canvGr->SetFillColor(10);

            TGraph* gr = (itype == 0) ? nTracksGrArr[igr] : trackChi2GrArr[igr];
            string title = (itype == 0) ? nTracksTitles[igr] : trackChi2Titles[igr];

            canvGr->Clear();
            gr->SetMarkerStyle(8);
            gr->SetMarkerSize(.7);
            gr->Draw("APL");
            gr->GetYaxis()->SetTitle(title.c_str());
            gr->GetXaxis()->SetTitle("# iteration");
            canvGr->Modified();

            string canvname = string(gr->GetName()) + ".eps";
            canvGr->Print(canvname.c_str());

            delete canvGr;
        }
    }

    // get residual means/widths
    gStyle->SetOptStat(000000011);
    gStyle->SetOptFit(0111);
    TGraph**** beeResPeaks0 = new TGraph***[2];
    TGraph**** beeResMeans0 = new TGraph***[2];
    TGraph**** beeResWidth0 = new TGraph***[2];
    TGraph**** eelResPeaks0 = new TGraph***[2];
    TGraph**** eelResMeans0 = new TGraph***[2];
    TGraph**** eelResWidth0 = new TGraph***[2];
    TGraph**** bis8ResPeaks0 = new TGraph***[2];
    TGraph**** bis8ResMeans0 = new TGraph***[2];
    TGraph**** bis8ResWidth0 = new TGraph***[2];

    TGraph** allBeeResPeaks0 = new TGraph*[2];
    TGraph** allBeeResMeans0 = new TGraph*[2];
    TGraph** allBeeResWidth0 = new TGraph*[2];
    TGraph** allEelResPeaks0 = new TGraph*[2];
    TGraph** allEelResMeans0 = new TGraph*[2];
    TGraph** allEelResWidth0 = new TGraph*[2];

    TGraph**** beeResPeaks1 = new TGraph***[2];
    TGraph**** beeResMeans1 = new TGraph***[2];
    TGraph**** beeResWidth1 = new TGraph***[2];
    TGraph**** eelResPeaks1 = new TGraph***[2];
    TGraph**** eelResMeans1 = new TGraph***[2];
    TGraph**** eelResWidth1 = new TGraph***[2];
    TGraph**** bis8ResPeaks1 = new TGraph***[2];
    TGraph**** bis8ResMeans1 = new TGraph***[2];
    TGraph**** bis8ResWidth1 = new TGraph***[2];

    TGraph** allBeeResPeaks1 = new TGraph*[2];
    TGraph** allBeeResMeans1 = new TGraph*[2];
    TGraph** allBeeResWidth1 = new TGraph*[2];
    TGraph** allEelResPeaks1 = new TGraph*[2];
    TGraph** allEelResMeans1 = new TGraph*[2];
    TGraph** allEelResWidth1 = new TGraph*[2];

    TF1***** beeFits = new TF1****[2];
    TF1***** eelFits = new TF1****[2];
    TF1***** bis8Fits = new TF1****[2];

    TF1*** allBeeFits = new TF1**[2];
    TF1*** allEelFits = new TF1**[2];

    TLegend** legbeeall = new TLegend*[2];

    for (int isideAC = 0; isideAC < 2; isideAC++) {
        legbeeall[isideAC] = new TLegend(.1, .1, .9, .9);

        allBeeFits[isideAC] = new TF1*[NFILES];
        allEelFits[isideAC] = new TF1*[NFILES];

        TCanvas* fitcanvbee = new TCanvas("fitcanvbee", "BEE gaussian fits");
        fitcanvbee->SetFillColor(10);
        fitcanvbee->Divide(3, 1);

        TCanvas* fitcanveel = new TCanvas("fitcanveel", "EEL gaussian fits");
        fitcanveel->SetFillColor(10);
        fitcanveel->Divide(3, 2);

        TCanvas* fitcanvbis8 = new TCanvas("fitcanvbis8", "EEL gaussian fits");
        fitcanvbis8->SetFillColor(10);
        fitcanvbis8->Divide(3, 1);

        TCanvas* alleelcanv = new TCanvas("alleelcanv", "alleelcanv");
        alleelcanv->SetFillColor(10);
        alleelcanv->Divide(3, 2);

        bool firstplotbee = true;
        bool firstploteel = true;
        bool firstplotbis8 = true;

        beeResPeaks0[isideAC] = new TGraph**[2];
        beeResMeans0[isideAC] = new TGraph**[2];
        beeResWidth0[isideAC] = new TGraph**[2];
        eelResPeaks0[isideAC] = new TGraph**[2];
        eelResMeans0[isideAC] = new TGraph**[2];
        eelResWidth0[isideAC] = new TGraph**[2];
        bis8ResPeaks0[isideAC] = new TGraph**[2];
        bis8ResMeans0[isideAC] = new TGraph**[2];
        bis8ResWidth0[isideAC] = new TGraph**[2];
        beeResPeaks1[isideAC] = new TGraph**[2];
        beeResMeans1[isideAC] = new TGraph**[2];
        beeResWidth1[isideAC] = new TGraph**[2];
        eelResPeaks1[isideAC] = new TGraph**[2];
        eelResMeans1[isideAC] = new TGraph**[2];
        eelResWidth1[isideAC] = new TGraph**[2];
        bis8ResPeaks1[isideAC] = new TGraph**[2];
        bis8ResMeans1[isideAC] = new TGraph**[2];
        bis8ResWidth1[isideAC] = new TGraph**[2];
        beeFits[isideAC] = new TF1***[2];
        eelFits[isideAC] = new TF1***[2];
        bis8Fits[isideAC] = new TF1***[2];

        for (int ieta = 0; ieta < 2; ieta++) {
            TCanvas* allbeecanv = new TCanvas("allbeecanv", "allbeecanv");
            allbeecanv->SetFillColor(10);
            allbeecanv->Divide(4, 2);

            TCanvas* allbis8canv = new TCanvas("allbis8canv", "allbis8canv");
            allbis8canv->SetFillColor(10);
            allbis8canv->Divide(4, 2);

            beeResPeaks0[isideAC][ieta] = new TGraph*[8];
            beeResMeans0[isideAC][ieta] = new TGraph*[8];
            beeResWidth0[isideAC][ieta] = new TGraph*[8];
            eelResPeaks0[isideAC][ieta] = new TGraph*[8];
            eelResMeans0[isideAC][ieta] = new TGraph*[8];
            eelResWidth0[isideAC][ieta] = new TGraph*[8];
            bis8ResPeaks0[isideAC][ieta] = new TGraph*[8];
            bis8ResMeans0[isideAC][ieta] = new TGraph*[8];
            bis8ResWidth0[isideAC][ieta] = new TGraph*[8];
            beeResPeaks1[isideAC][ieta] = new TGraph*[8];
            beeResMeans1[isideAC][ieta] = new TGraph*[8];
            beeResWidth1[isideAC][ieta] = new TGraph*[8];
            eelResPeaks1[isideAC][ieta] = new TGraph*[8];
            eelResMeans1[isideAC][ieta] = new TGraph*[8];
            eelResWidth1[isideAC][ieta] = new TGraph*[8];
            bis8ResPeaks1[isideAC][ieta] = new TGraph*[8];
            bis8ResMeans1[isideAC][ieta] = new TGraph*[8];
            bis8ResWidth1[isideAC][ieta] = new TGraph*[8];
            beeFits[isideAC][ieta] = new TF1**[8];
            eelFits[isideAC][ieta] = new TF1**[8];
            bis8Fits[isideAC][ieta] = new TF1**[8];
            for (int iphi = 0; iphi < 8; iphi++) {
                beeResPeaks0[isideAC][ieta][iphi] = new TGraph(NFILES);
                beeResMeans0[isideAC][ieta][iphi] = new TGraph(NFILES);
                beeResWidth0[isideAC][ieta][iphi] = new TGraph(NFILES);
                eelResPeaks0[isideAC][ieta][iphi] = new TGraph(NFILES);
                eelResMeans0[isideAC][ieta][iphi] = new TGraph(NFILES);
                eelResWidth0[isideAC][ieta][iphi] = new TGraph(NFILES);
                bis8ResPeaks0[isideAC][ieta][iphi] = new TGraph(NFILES);
                bis8ResMeans0[isideAC][ieta][iphi] = new TGraph(NFILES);
                bis8ResWidth0[isideAC][ieta][iphi] = new TGraph(NFILES);
                beeResPeaks1[isideAC][ieta][iphi] = new TGraph(NFILES);
                beeResMeans1[isideAC][ieta][iphi] = new TGraph(NFILES);
                beeResWidth1[isideAC][ieta][iphi] = new TGraph(NFILES);
                eelResPeaks1[isideAC][ieta][iphi] = new TGraph(NFILES);
                eelResMeans1[isideAC][ieta][iphi] = new TGraph(NFILES);
                eelResWidth1[isideAC][ieta][iphi] = new TGraph(NFILES);
                bis8ResPeaks1[isideAC][ieta][iphi] = new TGraph(NFILES);
                bis8ResMeans1[isideAC][ieta][iphi] = new TGraph(NFILES);
                bis8ResWidth1[isideAC][ieta][iphi] = new TGraph(NFILES);
                beeFits[isideAC][ieta][iphi] = new TF1*[NFILES];
                eelFits[isideAC][ieta][iphi] = new TF1*[NFILES];
                bis8Fits[isideAC][ieta][iphi] = new TF1*[NFILES];

                TCanvas* beecanv = new TCanvas("beecanv", "BEE residuals");
                beecanv->SetFillColor(10);
                TCanvas* eelcanv = new TCanvas("eelcanv", "EEL residuals");
                eelcanv->SetFillColor(10);
                TCanvas* bis8canv = new TCanvas("bis8canv", "BIS8 residuals");
                bis8canv->SetFillColor(10);

                TCanvas* dummy = new TCanvas("dummy", "dummy");
                // TLegend* legBee=new TLegend(.6,.8,.8,.95);
                // TLegend* legEel=new TLegend(.6,.8,.8,.95);
                TPaveStats** statsBee = new TPaveStats*[NFILES];
                TPaveStats** statsEel = new TPaveStats*[NFILES];
                // TPaveStats** statsBis8=new TPaveStats*[NFILES];

                double deltax(0.);
                double deltay(.2);
                bool firstiter = true;
                for (int ifile = NFILES - 1; ifile >= 0; ifile--) {
                    int iter = iters[ifile];
                    stringstream iterstr;
                    iterstr << "iter" << iter;
                    dummy->cd();
                    dummy->Clear();

                    beeFits[isideAC][ieta][iphi][ifile] =
                        new TF1((hbeenames[isideAC][ieta][iphi] + string("_fit_") + iterstr.str()).c_str(),
                                //"[0]*exp(-0.5*((x-[1])/[2])^2)+[3]*exp(-0.5*((x-[4])/[5])^2)", -1.,1.);
                                "[0]*exp(-0.5*((x-[1])/[2])^2)", -6., 6.);
                    TH1F* hBee = beeRes[ifile][isideAC][ieta][iphi];
                    TF1* fBee = beeFits[isideAC][ieta][iphi][ifile];
                    // fBee->SetParameters(hBee->GetMaximum(),hBee->GetMean(),.2,
                    //	      hBee->GetMaximum()/7.,hBee->GetMean(),.5);
                    fBee->SetParameters(hBee->GetMaximum(), hBee->GetMean(), .2);
                    // fBee->SetParLimits(0,hBee->GetMaximum()*.8,hBee->GetMaximum()*2.);
                    // fBee->SetParLimits(3,0.,hBee->GetMaximum()*.8);
                    // fBee->SetParLimits(5,0.,1.);
                    hBee->Fit(fBee, "QR");

                    beeResPeaks0[isideAC][ieta][iphi]->SetPoint(ifile, iter, fBee->GetParameter(0));
                    beeResMeans0[isideAC][ieta][iphi]->SetPoint(ifile, iter, fBee->GetParameter(1));
                    beeResWidth0[isideAC][ieta][iphi]->SetPoint(ifile, iter, std::fabs(fBee->GetParameter(2)));
                    beeResPeaks1[isideAC][ieta][iphi]->SetPoint(ifile, iter, fBee->GetParameter(3));
                    beeResMeans1[isideAC][ieta][iphi]->SetPoint(ifile, iter, fBee->GetParameter(4));
                    beeResWidth1[isideAC][ieta][iphi]->SetPoint(ifile, iter, std::fabs(fBee->GetParameter(5)));

                    dummy->Update();

                    TPaveStats* stats = (TPaveStats*)hBee->FindObject("stats");
                    // stringstream testname; testname<<"testBEE_iter"<<iter<<".eps";
                    // dummy->Print(testname.str().c_str());

                    if (!stats) {
                        cout << "no stats for beeRes[" << ifile << "][" << isideAC << "][" << ieta << "][" << iphi << "]" << std::endl;
                        continue;
                    }

                    statsBee[ifile] = stats;
                    if (firstiter) {
                        deltax = stats->GetX2NDC() - stats->GetX1NDC();
                        // deltay=stats->GetY2NDC()-stats->GetY1NDC();
                    }
                    int ipos = (ifile < 4) ? ifile : ifile - 4;

                    stats->SetY2NDC(stats->GetY2NDC() - deltay * (double)ipos);
                    stats->SetY1NDC(stats->GetY2NDC() - deltay);
                    if (ifile >= 4) {
                        stats->SetX1NDC(.05);
                        stats->SetX2NDC(.05 + deltax);
                    }
                    stats->SetTextColor(phicolor[ifile]);

                    beecanv->cd();
                    // hBee->SetLineWidth(.8);
                    hBee->SetLineColor(phicolor[ifile]);
                    hBee->GetXaxis()->SetTitle("residuals (mm)");
                    // rdh hBee->SetMaximum(1000.);
                    if (firstiter) {
                        hBee->Draw();
                    } else
                        hBee->Draw("same");
                    // fBee->SetLineWidth(1.5);
                    fBee->SetLineColor(phicolor[ifile]);
                    fBee->Draw("same");

                    allbeecanv->cd(iphi + 1);
                    if (firstiter)
                        hBee->Draw();
                    else
                        hBee->Draw("same");
                    fBee->SetLineColor(phicolor[ifile]);
                    fBee->Draw("same");

                    if ((isideAC == 0 && iphi == 5) || (isideAC == 1 && iphi == 7) || (iphi == 2 && ieta == 0) || iphi == 6) {
                        dummy->cd();
                        dummy->Clear();
                        eelFits[isideAC][ieta][iphi][ifile] =
                            new TF1((heelnames[isideAC][ieta][iphi] + string("_fit_") + iterstr.str()).c_str(),
                                    "[0]*exp(-0.5*((x-[1])/[2])^2)+[3]*exp(-0.5*((x-[4])/[5])^2)", -1., 1.);
                        //"[0]*exp(-0.5*((x-[1])/[2])^2)", -1.,1.);
                        TH1F* hEel = eelRes[ifile][isideAC][ieta][iphi];
                        TF1* fEel = eelFits[isideAC][ieta][iphi][ifile];
                        fEel->SetParameters(hEel->GetMaximum(), hEel->GetMean(), .2, hEel->GetMaximum() / 7., hEel->GetMean(), .5);
                        // fEel->SetParameters(hEel->GetMaximum(),hEel->GetMean(),.2);
                        //
                        fEel->SetParLimits(0, hEel->GetMaximum() * .8, hEel->GetMaximum() * 2.);
                        fEel->SetParLimits(3, 0., hEel->GetMaximum() * .8);
                        fEel->SetParLimits(5, 0., 1.);
                        hEel->Fit(fEel, "QR");

                        eelResPeaks0[isideAC][ieta][iphi]->SetPoint(ifile, iter, fEel->GetParameter(0));
                        eelResMeans0[isideAC][ieta][iphi]->SetPoint(ifile, iter, fEel->GetParameter(1));
                        eelResWidth0[isideAC][ieta][iphi]->SetPoint(ifile, iter, fEel->GetParameter(2));
                        eelResPeaks1[isideAC][ieta][iphi]->SetPoint(ifile, iter, fEel->GetParameter(3));
                        eelResMeans1[isideAC][ieta][iphi]->SetPoint(ifile, iter, fEel->GetParameter(4));
                        eelResWidth1[isideAC][ieta][iphi]->SetPoint(ifile, iter, fEel->GetParameter(5));

                        dummy->Update();
                        TPaveStats* stats1 = (TPaveStats*)hEel->FindObject("stats");
                        // dummy->Print("testEEL.eps");

                        statsEel[ifile] = stats1;
                        if (firstiter) { deltax = stats1->GetX2NDC() - stats1->GetX1NDC(); }
                        int ipos = (ifile < 4) ? ifile : ifile - 4;
                        stats1->SetY2NDC(stats1->GetY2NDC() - deltay * (double)ipos);
                        stats1->SetY1NDC(stats1->GetY2NDC() - deltay);
                        if (ifile >= 4) {
                            stats1->SetX1NDC(.05);
                            stats1->SetX2NDC(.05 + deltax);
                        }
                        stats1->SetTextColor(phicolor[ifile]);

                        eelcanv->cd();
                        // hEel->SetLineWidth(.8);
                        hEel->SetLineColor(phicolor[ifile]);
                        hEel->GetXaxis()->SetTitle("residuals (mm)");
                        // hEel->SetMaximum(3700.);
                        if (firstiter) {
                            hEel->Draw();
                        } else
                            hEel->Draw("same");
                        // fEel->SetLineWidth(1.5);
                        fEel->SetLineColor(phicolor[ifile]);
                        fEel->Draw("same");

                        int ipad = 1;
                        if (iphi >= 5 && iphi <= 7) {
                            if (iphi == 6) {
                                if (ieta == 0)
                                    ipad = 2;
                                else
                                    ipad = 5;
                            } else if (iphi == 5 || iphi == 7) {
                                if (ieta == 0)
                                    ipad = 3;
                                else
                                    ipad = 6;
                            }
                        }
                        alleelcanv->cd(ipad);
                        // hEel->SetLineWidth(.5);
                        // fEel->SetLineWidth(.6);
                        if (firstiter)
                            hEel->Draw();
                        else
                            hEel->Draw("same");
                        fEel->Draw("same");
                    }

                    /*
                    if (ieta==0) {
                      dummy->cd();
                      dummy->Clear();
                      bis8Fits[isideAC][ieta][iphi][ifile]=
                        //new TF1((hbis8names[isideAC][ieta][iphi]+string("_fit_")+iterstr.str()).c_str(),
                        //      "[0]*exp(-0.5*((x-[1])/[2])^2)+[3]*exp(-0.5*((x-[4])/[5])^2)", -1.,1.);
                        new TF1((hbis8names[isideAC][ieta][iphi]+string("_fit_")+iterstr.str()).c_str(),
                                "[0]*exp(-0.5*((x-[1])/[2])^2)", -1.,1.);
                      TH1F* hBis8=bis8Res [ifile][isideAC][ieta][iphi];
                      TF1*  fBis8=bis8Fits[isideAC][ieta][iphi][ifile];
                      fBis8->SetParameters(hBis8->GetMaximum(),hBis8->GetMean(),.2);
                      //hBis8->GetMaximum()/7.,hBis8->GetMean(),.5);
                      //fBis8->SetParLimits(0,hBis8->GetMaximum()*.8,hBis8->GetMaximum()*2.);
                      //fBis8->SetParLimits(3,0.,hBis8->GetMaximum()*.8);
                      //fBis8->SetParLimits(5,0.,1.);
                      hBis8->Fit(fBis8,"QRW");

                      bis8ResPeaks0[isideAC][ieta][iphi]->SetPoint(ifile,iter,fBis8->GetParameter(0));
                      bis8ResMeans0[isideAC][ieta][iphi]->SetPoint(ifile,iter,fBis8->GetParameter(1));
                      bis8ResWidth0[isideAC][ieta][iphi]->SetPoint(ifile,iter,fBis8->GetParameter(2));
                      //bis8ResPeaks1[isideAC][ieta][iphi]->SetPoint(iter,iter,fBis8->GetParameter(3));
                      //bis8ResMeans1[isideAC][ieta][iphi]->SetPoint(iter,iter,fBis8->GetParameter(4));
                      //bis8ResWidth1[isideAC][ieta][iphi]->SetPoint(iter,iter,fBis8->GetParameter(5));

                      dummy->Update();
                      TPaveStats* statsbis8=(TPaveStats*)hBis8->FindObject("stats");
                      //dummy->Print("testBIS8.eps");

                      statsBis8[ifile]=statsbis8;
                      if (firstiter) {
                        deltax=statsbis8->GetX2NDC()-statsbis8->GetX1NDC();
                      }
                      int iposbis8=(ifile<4)?ifile:ifile-4;
                      statsbis8->SetY2NDC(statsbis8->GetY2NDC()-deltay*(double)iposbis8);
                      statsbis8->SetY1NDC(statsbis8->GetY2NDC()-deltay);
                      if (ifile>=4) {
                        statsbis8->SetX1NDC(.05);
                        statsbis8->SetX2NDC(.05+deltax);
                      }
                      statsbis8->SetTextColor(phicolor[ifile]);

                      bis8canv->cd();
                      //hBis8->SetLineWidth(.8);
                      hBis8->SetLineColor(phicolor[ifile]);
                      hBis8->SetMaximum(20.);
                      if (firstiter) {
                        hBis8->Draw();
                      }
                      else hBis8->Draw("same");
                      //fBis8->SetLineWidth(1.5);
                      fBis8->SetLineColor(phicolor[ifile]);
                      fBis8->Draw("same");

                      allbis8canv->cd(iphi+1);
                      //hBis8->SetLineWidth(.5);
                      //fBis8->SetLineWidth(.6);
                      if (firstiter) hBis8->Draw();
                      else           hBis8->Draw();
                      fBis8->Draw("same");
                    }
                    */
                    firstiter = false;
                }
                // legBis8->Draw();
                if (ieta == 0) bis8canv->Print((hbis8names[isideAC][ieta][iphi] + "_res.eps").c_str());
                delete bis8canv;

                if ((isideAC == 0 && iphi == 5) || (isideAC == 1 && iphi == 7) || (iphi == 2 && ieta == 0) || iphi == 6) {
                    // legEel->Draw();
                    eelcanv->Print((heelnames[isideAC][ieta][iphi] + "_res.eps").c_str());
                }
                delete eelcanv;

                beecanv->Print((hbeenames[isideAC][ieta][iphi] + "_res.eps").c_str());
                delete beecanv;

                delete dummy;

                string grtitles[6] = {"inner peak", "inner mean (mm)", "inner width (mm)",
                                      "outer peak", "outer mean (mm)", "outer width (mm)"};
                // string grtitlesbee[6]={"inner peak","inner mean (mm)",
                //                       "inner width (mm)",
                //                       "outer peak","outer mean (mm)",
                //                       "outer width (mm)"};
                string grtitlesbee[3] = {"peak", "mean (mm)", "width (mm)"};

                int colorbee = beecolor[ieta][iphi];
                // double maxbee[6]={1000., .3,.3, 250., .6,1.};
                // double minbee[6]={   0.,-.3,0.,   0.,-.6,0.};
                // for (int ipad=1;ipad<=6;ipad++) {
                double maxbee[6] = {250., 4.5, 2.};
                double minbee[6] = {0., -2., 0.};
                for (int ipad = 1; ipad <= 3; ipad++) {
                    fitcanvbee->cd(ipad);
                    TGraph* gr;
                    switch (ipad) {
                        case 1: gr = beeResPeaks0[isideAC][ieta][iphi]; break;
                        case 2: gr = beeResMeans0[isideAC][ieta][iphi]; break;
                        case 3: gr = beeResWidth0[isideAC][ieta][iphi]; break;
                        case 4: gr = beeResPeaks1[isideAC][ieta][iphi]; break;
                        case 5: gr = beeResMeans1[isideAC][ieta][iphi]; break;
                        case 6: gr = beeResWidth1[isideAC][ieta][iphi]; break;
                    }

                    gr->SetMarkerColor(colorbee);
                    gr->SetMarkerStyle(8);
                    gr->SetMarkerSize(.7);
                    gr->SetLineColor(colorbee);
                    // gr->SetLineWidth(1.6);
                    if (firstplotbee) {
                        gr->SetTitle(grtitlesbee[ipad - 1].c_str());
                        gr->SetMinimum(minbee[ipad - 1]);
                        gr->SetMaximum(maxbee[ipad - 1]);
                        gr->Draw("APL");
                    } else
                        gr->Draw("PLsame");
                }
                firstplotbee = false;

                if ((isideAC == 0 && iphi == 5) || (isideAC == 1 && iphi == 7) || (iphi == 2 && ieta == 0) || iphi == 6) {
                    int coloreel = eelcolor[ieta][iphi];
                    double maxeel[6] = {3700., .1, .3, 1500., .5, .5};
                    double mineel[6] = {0., -.1, 0., 0., -.5, 0.};
                    for (int ipad = 1; ipad <= 6; ipad++) {
                        fitcanveel->cd(ipad);
                        TGraph* gr;
                        switch (ipad) {
                            case 1: gr = eelResPeaks0[isideAC][ieta][iphi]; break;
                            case 2: gr = eelResMeans0[isideAC][ieta][iphi]; break;
                            case 3: gr = eelResWidth0[isideAC][ieta][iphi]; break;
                            case 4: gr = eelResPeaks1[isideAC][ieta][iphi]; break;
                            case 5: gr = eelResMeans1[isideAC][ieta][iphi]; break;
                            case 6: gr = eelResWidth1[isideAC][ieta][iphi]; break;
                        }

                        gr->SetMarkerColor(coloreel);
                        gr->SetMarkerStyle(8);
                        gr->SetMarkerSize(.7);
                        gr->SetLineColor(coloreel);
                        // gr->SetLineWidth(1.6);
                        if (firstploteel) {
                            gr->SetTitle(grtitles[ipad - 1].c_str());
                            gr->SetMinimum(mineel[ipad - 1]);
                            gr->SetMaximum(maxeel[ipad - 1]);
                            gr->Draw("APL");
                        } else
                            gr->Draw("PLsame");
                    }
                    firstploteel = false;
                }

                if (ieta == 0) {
                    int colorbis8 = bis8color[ieta][iphi];
                    double maxbis8[6] = {20., .2, 1., 20., .5, .5};
                    double minbis8[6] = {0., -.2, 1., 0., -.5, 0.};
                    for (int ipad = 1; ipad <= 3; ipad++) {
                        fitcanvbis8->cd(ipad);
                        TGraph* gr;
                        switch (ipad) {
                            case 1: gr = bis8ResPeaks0[isideAC][ieta][iphi]; break;
                            case 2: gr = bis8ResMeans0[isideAC][ieta][iphi]; break;
                            case 3: gr = bis8ResWidth0[isideAC][ieta][iphi]; break;
                            case 4: gr = bis8ResPeaks1[isideAC][ieta][iphi]; break;
                            case 5: gr = bis8ResMeans1[isideAC][ieta][iphi]; break;
                            case 6: gr = bis8ResWidth1[isideAC][ieta][iphi]; break;
                        }

                        gr->SetMarkerColor(colorbis8);
                        gr->SetMarkerStyle(8);
                        gr->SetMarkerSize(.7);
                        gr->SetLineColor(colorbis8);
                        // gr->SetLineWidth(1.6);
                        if (firstplotbis8) {
                            gr->SetTitle(grtitles[ipad - 1].c_str());
                            gr->SetMinimum(minbis8[ipad - 1]);
                            gr->SetMaximum(maxbis8[ipad - 1]);
                            gr->Draw("APL");
                        } else
                            gr->Draw("PLsame");
                    }
                    firstplotbis8 = false;
                }

            }  // iphi

            std::stringstream beecanvname;
            beecanvname << "allBEE_side";
            if (isideAC == 0)
                beecanvname << "A";
            else
                beecanvname << "C";
            beecanvname << "_stEta" << ieta + 1;
            beecanvname << ".eps";
            allbeecanv->Print(beecanvname.str().c_str());
            delete allbeecanv;

            if (ieta == 0) {
                std::stringstream bis8canvname;
                bis8canvname << "allBIS8_side";
                if (isideAC == 0)
                    bis8canvname << "A";
                else
                    bis8canvname << "C";
                bis8canvname << ".eps";
                allbis8canv->Print(bis8canvname.str().c_str());
                delete allbis8canv;
            }

        }  // ieta

        TCanvas* allBeeResCanv = new TCanvas("allBeeResCanv", "allBeeResCanv");
        allBeeResCanv->SetFillColor(10);
        allBeeResCanv->Divide(3, 1);

        TCanvas* allEelResCanv = new TCanvas("allEelResCanv", "allEelResCanv");
        allEelResCanv->SetFillColor(10);
        allEelResCanv->Divide(3, 1);

        allBeeResPeaks0[isideAC] = new TGraph(NFILES);
        allBeeResMeans0[isideAC] = new TGraph(NFILES);
        allBeeResWidth0[isideAC] = new TGraph(NFILES);

        allEelResPeaks0[isideAC] = new TGraph(NFILES);
        allEelResMeans0[isideAC] = new TGraph(NFILES);
        allEelResWidth0[isideAC] = new TGraph(NFILES);
        allEelResPeaks1[isideAC] = new TGraph(NFILES);
        allEelResMeans1[isideAC] = new TGraph(NFILES);
        allEelResWidth1[isideAC] = new TGraph(NFILES);

        TCanvas* dummy2 = new TCanvas("dummy2", "dummy2");
        TPaveStats** statsAllBee = new TPaveStats*[NFILES];
        TPaveStats** statsAllEel = new TPaveStats*[NFILES];

        double deltax(0.);
        double deltay(.2);
        bool firstiter = true;
        for (int ifile = NFILES - 1; ifile >= 0; ifile--) {
            int iter = iters[ifile];
            stringstream iterstr;
            iterstr << "iter" << iter;

            // BEE
            dummy2->cd();
            dummy2->Clear();
            stringstream allBeeStr;
            allBeeStr << "allBEE_fit_" << iterstr.str();
            if (isideAC == 0)
                allBeeStr << "_sideA";
            else
                allBeeStr << "_sideC";
            allBeeFits[isideAC][ifile] = new TF1(allBeeStr.str().c_str(), "[0]*exp(-0.5*((x-[1])/[2])^2)", -6., 6.);

            TH1F* hAllBee = allBeeRes[ifile][isideAC];
            TF1* fAllBee = allBeeFits[isideAC][ifile];
            fAllBee->SetParameters(hAllBee->GetMaximum(), hAllBee->GetMean(), .2);
            hAllBee->Fit(fAllBee, "QR");

            allBeeResPeaks0[isideAC]->SetPoint(ifile, iter, fAllBee->GetParameter(0));
            allBeeResMeans0[isideAC]->SetPoint(ifile, iter, fAllBee->GetParameter(1));
            allBeeResWidth0[isideAC]->SetPoint(ifile, iter, fAllBee->GetParameter(2));

            dummy2->Update();

            TPaveStats* stats2 = (TPaveStats*)hAllBee->FindObject("stats");
            if (!stats2) {
                std::cout << "no stats box" << std::endl;
                continue;
            }
            statsAllBee[ifile] = stats2;
            if (firstiter) deltax = stats2->GetX2NDC() - stats2->GetX1NDC();

            int ipos = (ifile < 4) ? ifile : ifile - 4;
            stats2->SetY2NDC(stats2->GetY2NDC() - deltay * (double)ipos);
            stats2->SetY1NDC(stats2->GetY2NDC() - deltay);
            if (ifile >= 4) {
                stats2->SetX1NDC(.05);
                stats2->SetX2NDC(.05 + deltax);
            }
            stats2->SetTextColor(phicolor[ifile]);

            allBeeResCanv->cd();
            hAllBee->SetLineColor(phicolor[ifile]);
            hAllBee->GetXaxis()->SetTitle("residuals (mm)");

            if (firstiter)
                hAllBee->Draw();
            else
                hAllBee->Draw("same");

            fAllBee->SetLineColor(phicolor[ifile]);
            fAllBee->Draw("same");

            // EEL
            dummy2->cd();
            dummy2->Clear();
            stringstream allEelStr;
            allEelStr << "allEEL_fit_" << iterstr.str();
            if (isideAC == 0)
                allEelStr << "_sideA";
            else
                allEelStr << "_sideC";
            allEelFits[isideAC][ifile] =
                new TF1(allEelStr.str().c_str(), "[0]*exp(-0.5*((x-[1])/[2])^2)+[3]*exp(-0.5*((x-[4])/[5])^2)", -1., 1.);

            TH1F* hAllEel = allEelRes[ifile][isideAC];
            TF1* fAllEel = allEelFits[isideAC][ifile];
            fAllEel->SetParameters(hAllEel->GetMaximum(), hAllEel->GetMean(), .2);
            hAllEel->Fit(fAllEel, "QR");

            allEelResPeaks0[isideAC]->SetPoint(ifile, iter, fAllEel->GetParameter(0));
            allEelResMeans0[isideAC]->SetPoint(ifile, iter, fAllEel->GetParameter(1));
            allEelResWidth0[isideAC]->SetPoint(ifile, iter, fAllEel->GetParameter(2));

            dummy2->Update();

            TPaveStats* stats3 = (TPaveStats*)hAllEel->FindObject("stats");
            if (!stats3) {
                std::cout << "no stats box" << std::endl;
                continue;
            }
            statsAllEel[ifile] = stats3;
            if (firstiter) deltax = stats3->GetX2NDC() - stats3->GetX1NDC();

            ipos = (ifile < 4) ? ifile : ifile - 4;
            stats3->SetY2NDC(stats3->GetY2NDC() - deltay * (double)ipos);
            stats3->SetY1NDC(stats3->GetY2NDC() - deltay);
            if (ifile >= 4) {
                stats3->SetX1NDC(.05);
                stats3->SetX2NDC(.05 + deltax);
            }
            stats3->SetTextColor(phicolor[ifile]);

            allEelResCanv->cd();
            hAllEel->SetLineColor(phicolor[ifile]);
            hAllEel->GetXaxis()->SetTitle("residuals (mm)");

            if (firstiter)
                hAllEel->Draw();
            else
                hAllEel->Draw("same");

            fAllEel->SetLineColor(phicolor[ifile]);
            fAllEel->Draw("same");

            firstiter = false;
        }

        if (isideAC == 0)
            allBeeResCanv->Print("allBEERes_sideA.eps");
        else
            allBeeResCanv->Print("allBEERes_sideC.eps");

        if (isideAC == 0)
            allEelResCanv->Print("allEELRes_sideA.eps");
        else
            allEelResCanv->Print("allEELRes_sideC.eps");

        std::stringstream eelcanvname;
        eelcanvname << "allEEL_side";
        if (isideAC == 0)
            eelcanvname << "A";
        else
            eelcanvname << "C";
        eelcanvname << ".eps";
        alleelcanv->Print(eelcanvname.str().c_str());
        delete alleelcanv;

        // print fits
        if (isideAC == 0) {
            fitcanvbee->Print("BEEA_fits.eps");
            fitcanveel->Print("EELA_fits.eps");
            fitcanvbis8->Print("BIS8A_fits.eps");
        } else {
            fitcanvbee->Print("BEEC_fits.eps");
            fitcanveel->Print("EELC_fits.eps");
            fitcanvbis8->Print("BIS8C_fits.eps");
        }
        delete fitcanvbee;
        delete fitcanveel;
        delete fitcanvbis8;

    }  // isideAC
}

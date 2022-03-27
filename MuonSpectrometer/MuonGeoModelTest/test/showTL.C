/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TCanvas.h>
#include <TF1.h>
#include <TGaxis.h>
#include <TH1.h>
#include <TStyle.h>

#include <RooPlot>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "AtlasUtils.C"

#define maxvsize 3000
void showTL(std::string bLpar = "eg", std::string selection = "[7.4.1.3.0.1.", std::string cham = "BOLA") {
    std::cout << "running command:" << std::endl;
    std::cout << ".x showTL.C(\"" << bLpar << "\",\"" << selection << "\");" << std::endl;
    std::string fileNmdtNoBLines;
    std::string fileNmdt1BLinePar;
    std::string fileNmdtNoBLines;
    std::string fileNmdt1BLinePar;
    int lsel = selection.size();
    if (bLpar == "sp") {
        fileNmdt1BLinePar = "mdt_current_EP_R.03.13_sp10";
    } else if (bLpar == "sn") {
        fileNmdt1BLinePar = "mdt_current_EP_R.03.13_sn20";
    } else if (bLpar == "tw") {
        fileNmdt1BLinePar = "mdt_current_EP_R.03.13_tw30";
    } else if (bLpar == "pg") {
        fileNmdt1BLinePar = "mdt_current_EP_R.03.13_pg10";
    } else if (bLpar == "tr") {
        fileNmdt1BLinePar = "mdt_current_EP_R.03.13_tr20";
    } else if (bLpar == "eg") {
        fileNmdt1BLinePar = "mdt_current_EP_R.03.13_eg05";
    }
    std::string fileNmdtNoBLines = "mdt_current_EP_R.03.13_NO_blines";
    char fileNCmdtNoBLines[1000];
    char fileNCmdt1BLinePar[1000];
    sprintf(fileNCmdtNoBLines, "%s", fileNmdtNoBLines.c_str());
    sprintf(fileNCmdt1BLinePar, "%s", fileNmdt1BLinePar.c_str());

    fstream fmdtNoBLines;
    fstream fmdt1BLinePar;
    fmdtNoBLines.open(fileNCmdtNoBLines, fstream::in);
    fmdt1BLinePar.open(fileNCmdt1BLinePar, fstream::in);

    Double_t xro, yro, zro, x, y, z, xhv, yhv, zhv;
    Double_t vxro[maxvsize], vyro[maxvsize], vzro[maxvsize], vx[maxvsize], vy[maxvsize], vz[maxvsize], vxhv[maxvsize], vyhv[maxvsize],
        vzhv[maxvsize];
    Double_t nvxro[maxvsize], nvyro[maxvsize], nvzro[maxvsize], nvx[maxvsize], nvy[maxvsize], nvz[maxvsize], nvxhv[maxvsize],
        nvyhv[maxvsize], nvzhv[maxvsize];
    for (int i = 0; i < maxvsize; ++i) {
        vxro[i] = vyro[i] = vzro[i] = vx[i] = vy[i] = vz[i] = vxhv[i] = vyhv[i] = vzhv[i] = 0.;
        nvxro[i] = nvyro[i] = nvzro[i] = nvx[i] = nvy[i] = nvz[i] = nvxhv[i] = nvyhv[i] = nvzhv[i] = 0.;
    }
    std::string firstChan;
    std::string lastChan;

    std::string strId;
    std::string strIdPrevious;
    Int_t nw = 0;
    while (1) {
        fmdtNoBLines >> strId >> xro >> yro >> zro >> x >> y >> z >> xhv >> yhv >> zhv;
        // 	std::cout<<" string read is "
        // 		 <<strId<<" subset=<"
        // 		 <<strId.substr(0,lsel)
        // 		 <<"> to be compared with <"
        // 		 <<selection<<">"
        // 		 <<std::endl;

        if (!fmdtNoBLines.good()) break;
        if (nw == 0) firstChan = strId;
        if (strId.substr(0, lsel) != selection) continue;
        // 	 std::cout<<" string read is "
        // 		  <<strId<<" subset=<"
        // 		  <<strId.substr(0,lsel)
        // 		  <<"> to be compared with <"
        // 		  <<selection<<">"
        // 		  <<std::endl;
        nvxro[nw] = xro;
        nvyro[nw] = yro;
        nvzro[nw] = zro;
        nvx[nw] = x;
        nvy[nw] = y;
        nvz[nw] = z;
        nvxhv[nw] = xhv;
        nvyhv[nw] = yhv;
        nvzhv[nw] = zhv;
        std::cout << strId << " wire n. " << nw << " Nominal x,y,z " << nvx[nw] << " " << nvy[nw] << " " << nvz[nw] << std::endl;
        strIdPrevious = strId;
        ++nw;
    }
    lastChan = strIdPrevious;
    //    std::cout<<"  "<<nw<<" x,y,z "<<vx[nw]<<" "<<vy[nw]<<" "<<vz[nw]<<std::endl;

    // std::cout<<".x showTL.C(\""<<bLpar<<"\",\""<<selection<<"\");      DONE"<<std::endl;
    Int_t nwd = 0;
    while (1) {
        fmdt1BLinePar >> strId >> xro >> yro >> zro >> x >> y >> z >> xhv >> yhv >> zhv;
        // 	std::cout<<" string read is "
        // 		 <<strId<<" subset=<"
        // 		 <<strId.substr(0,lsel)
        // 		 <<"> to be compared with <"
        // 		 <<selection<<">"
        // 		 <<std::endl;

        if (!fmdt1BLinePar.good()) break;
        if (strId.substr(0, lsel) != selection) continue;
        // 	 std::cout<<" string read is "
        // 		  <<strId<<" subset=<"
        // 		  <<strId.substr(0,lsel)
        // 		  <<"> to be compared with <"
        // 		  <<selection<<">"
        // 		  <<std::endl;
        // std::cout<<" found at x,y,z "<<xro<<" "<<yro<<" "<<zro<<std::endl;

        vxro[nwd] = xro;
        vyro[nwd] = yro;
        vzro[nwd] = zro;
        vx[nwd] = x;
        vy[nwd] = y;
        vz[nwd] = z;
        vxhv[nwd] = xhv;
        vyhv[nwd] = yhv;
        vzhv[nwd] = zhv;
        std::cout << strId << " wire n. " << nwd << " Deformed x,y,z " << vx[nwd] << " " << vy[nwd] << " " << vz[nwd] << std::endl;
        ++nwd;
    }
    //    std::cout<<"  "<<nw<<" x,y,z "<<vx[nw]<<" "<<vy[nw]<<" "<<vz[nw]<<std::endl;

    Double_t xmin = 0.;
    Double_t xmax = 0.;

    // Double_t xmin =  2400.; //-3000.
    // Double_t xmax =  2500.; // 3000.
    Double_t xminBOL = -3000.;  //-3000.
    Double_t xmaxBOL = 3000.;   // 3000.
    Double_t xminEML = -1500.;  //-3000.
    Double_t xmaxEML = 1500.;   // 3000.
    Double_t yminBOL = 9240.;
    Double_t ymaxBOL = 9310.;
    Double_t zminBOLA = 0.;
    Double_t zmaxBOLA = 2500.;
    // Double_t zminBOLA = 510.;
    // Double_t zmaxBOLA = 600.;
    Double_t zminBOLC = -3500.;
    Double_t zmaxBOLC = -1000.;
    // Double_t zminBOLC =-3000.;
    // Double_t zmaxBOLC =-2920.;
    Double_t yminEML = 1500.;
    Double_t ymaxEML = 4000.;
    Double_t zminEMLA = 14110.;
    Double_t zmaxEMLA = 14170.;

    Double_t ymin = 0;
    Double_t ymax = 0;
    Double_t zmin = 0;
    Double_t zmax = 0;
    if (cham == "BOLA") {
        xmin = xminBOL;
        xmax = xmaxBOL;
        ymin = yminBOL;
        ymax = ymaxBOL;
        zmin = zminBOLA;
        zmax = zmaxBOLA;
    }
    if (cham == "EMLA") {
        xmin = xminEML;
        xmax = xmaxEML;
        ymin = yminEML;
        ymax = ymaxEML;
        zmin = zminEMLA;
        zmax = zmaxEMLA;
    }
    if (cham == "EMLC") {
        xmin = xminEML;
        xmax = xmaxEML;
        ymin = yminEML;
        ymax = ymaxEML;
        zmin = -zmaxEMLA;
        zmax = -zminEMLA;
    }
    if (cham == "BOLC") {
        xmin = xminBOL;
        xmax = xmaxBOL;
        ymin = yminBOL;
        ymax = ymaxBOL;
        zmin = zminBOLC;
        zmax = zmaxBOLC;
    }

    TPolyLine3D *plnRO = new TPolyLine3D(nw, nvzro, nvxro, nvyro);
    TPolyLine3D *plRO = new TPolyLine3D(nwd, vzro, vxro, vyro);
    TPolyLine3D *plnHV = new TPolyLine3D(nw, nvzhv, nvxhv, nvyhv);
    TPolyLine3D *plHV = new TPolyLine3D(nwd, vzhv, vxhv, vyhv);
    plnRO->SetLineColor(kBlack);
    plnRO->SetLineStyle(1);
    plnRO->SetLineWidth(3);
    plRO->SetLineColor(kRed);
    plRO->SetLineStyle(2);
    plRO->SetLineWidth(3);
    plnHV->SetLineColor(kBlack);
    plnHV->SetLineStyle(1);
    plnHV->SetLineWidth(3);
    plHV->SetLineColor(kRed);
    plHV->SetLineStyle(2);
    plHV->SetLineWidth(3);

    TCanvas *c1 = new TCanvas("c1", "3D view");
    TView *view = TView::CreateView(1);
    // view->SetRange(-3500.0,-3000.,9260.,-1000.0,3000.0,9280.0);
    view->SetRange(zmin, xmin, ymin, zmax, xmax, ymax);
    view->ShowAxis();
    plnRO->Draw();
    plRO->Draw();
    plnHV->Draw();
    plHV->Draw();

    TCanvas *c2 = new TCanvas("c2", "XY view");
    RooPlot *xFrameXY = new RooPlot(xmin, xmax, ymin, ymax);
    xFrameXY->SetTitle("XY view");
    xFrameXY->GetXaxis()->SetTitle("ATLAS global X axis");
    xFrameXY->GetXaxis()->SetTitleColor(kGreen);
    xFrameXY->GetYaxis()->SetTitle("ATLAS global y axis");
    xFrameXY->GetYaxis()->SetTitleColor(kBlue);

    xFrameXY->Draw();

    for (int i = 0; i < nw; ++i) {
        hline(nvxro[i], nvyro[i], nvxhv[i], nvyhv[i], 1);
        hline(vxro[i], vyro[i], vxhv[i], vyhv[i], 2);
    }

    TCanvas *c3 = new TCanvas("YZ", "YZ view");
    RooPlot *xFrameYZ = new RooPlot(zmin, zmax, ymin, ymax);
    xFrameYZ->SetTitle("YZ view");
    xFrameYZ->GetXaxis()->SetTitle("ATLAS global z axis");
    xFrameYZ->GetXaxis()->SetTitleColor(kRed);
    xFrameYZ->GetYaxis()->SetTitle("ATLAS global y axis");
    xFrameYZ->GetYaxis()->SetTitleColor(kBlue);

    xFrameYZ->Draw();

    for (int i = 0; i < nw; ++i) {
        hline(nvzro[i], nvyro[i], nvzhv[i], nvyhv[i], 1);
        hline(vzro[i], vyro[i], vzhv[i], vyhv[i], 2);
    }

    TCanvas *c4 = new TCanvas("XZ", "XZ view");
    RooPlot *xFrameXZ = new RooPlot(xmin, xmax, zmin, zmax);
    xFrameXZ->SetTitle("XZ view");
    xFrameXZ->GetXaxis()->SetTitle("ATLAS global x axis");
    xFrameXZ->GetXaxis()->SetTitleColor(kGreen);
    xFrameXZ->GetYaxis()->SetTitle("ATLAS global z axis");
    xFrameXZ->GetYaxis()->SetTitleColor(kRed);
    xFrameXZ->Draw();
    for (int i = 0; i < nw; ++i) {
        hline(nvxro[i], nvzro[i], nvxhv[i], nvzhv[i], 1);
        hline(vxro[i], vzro[i], vxhv[i], vzhv[i], 2);
    }

    // example of script showing how to divide a canvas
    // into adjacent subpads + axis labels on the top and right side
    // of the pads.
    // Author; Rene Brun

    TCanvas *c5 = new TCanvas("c5", "All views", 900, 700);
    gStyle->SetOptStat(0);
    c5->Divide(2, 2);

    c5->cd(1);
    // gPad->SetTickx(2);
    // h1->Draw();
    xFrameXY->Draw();
    for (int i = 0; i < nw; ++i) {
        hline(nvxro[i], nvyro[i], nvxhv[i], nvyhv[i], 1);
        hline(vxro[i], vyro[i], vxhv[i], vyhv[i], 2);
    }
    myText(0.5, 0.8, 1, firstChan.c_str());
    myText(0.2, 0.8, 1, lastChan.c_str());
    myText(0.5, 0.2, 1, bLpar.c_str());

    c5->cd(2);
    xFrameYZ->Draw();
    for (int i = 0; i < nw; ++i) {
        hline(nvzro[i], nvyro[i], nvzhv[i], nvyhv[i], 1);
        hline(vzro[i], vyro[i], vzhv[i], vyhv[i], 2);
    }

    c5->cd(3);
    // h3->Draw();
    xFrameXZ->Draw();
    for (int i = 0; i < nw; ++i) {
        hline(nvxro[i], nvzro[i], nvxhv[i], nvzhv[i], 1);
        hline(vxro[i], vzro[i], vxhv[i], vzhv[i], 2);
    }

    c5->cd(4);
    gPad->SetTitle("Pippo");
    // h4->Draw();
    TView *view2 = TView::CreateView(1);
    view2->SetRange(zmin, xmin, ymin, zmax, xmax, ymax);
    view2->ShowAxis();
    plnRO->Draw();
    plRO->Draw();
    plnHV->Draw();
    plHV->Draw();
}
void hline(Double_t x1, Double_t y1, Double_t x2, Double_t y2, Int_t lColor) {
    TLine *l = new TLine(x1, y1, x2, y2);
    // std::cout<<" x1,y1 "<<x1<<" "<<y1<<" x2,y2 "<<x2<<" "<<y2<<std::endl;

    l->Draw();
    l->SetLineWidth(1);
    l->SetLineColor(lColor);
}

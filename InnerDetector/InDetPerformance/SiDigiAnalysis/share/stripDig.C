//
//    Look at digitised hits (RDOs)
//

#define NB_SS_LAYERS 2
// ...3 for LoI, 2 for LTF

/*
#ifndef __CINT__
#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TVector.h"
#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
using std::cout;
using std::endl;
#endif
*/

//const double M_PI(acos(-1.));
const double OuterRadius(1050.0);
const double EndZ(3050.0);

void stripDig() {

unsigned int nPixelRDO;
std::vector<double> *pixelRDOGlobX = 0;
std::vector<double> *pixelRDOGlobY = 0;
std::vector<double> *pixelRDOGlobZ = 0;
std::vector<double> *pixelRDOXL = 0;
std::vector<double> *pixelRDOYL = 0;
std::vector<double> *pixelRDOZL = 0;

std::vector<double> *pixelRDO_BarrelEndcap = 0;
std::vector<double> *pixelRDO_Layer = 0;
std::vector<double> *pixelRDO_Phi = 0;
std::vector<double> *pixelRDO_Eta = 0;

std::vector<double> allPixelRDOListGlobR;
std::vector<double> allPixelRDOListGlobZ;
std::vector<double> barrelPixelRDOListGlobX;
std::vector<double> barrelPixelRDOListGlobY;
std::vector<double> barrelPixelRDOListGlobZ;

std::vector<double> endcapPixelRDOListGlobX;
std::vector<double> endcapPixelRDOListGlobY;
std::vector<double> endcapPixelRDOListGlobZ;

unsigned int nStripRDO;
std::vector<int> *stripRDO_BarrelEndcap = 0;
std::vector<int> *stripRDO_Layer = 0;
std::vector<int> *stripRDO_Phi = 0;
std::vector<int> *stripRDO_Eta = 0;
std::vector<int> *stripRDO_Side = 0;
std::vector<int> *stripRDO_Row = 0;
std::vector<int> *stripRDO_Strip = 0;

std::vector<int> *stripRDO_Size = 0;

std::vector<double> *stripRDOXL = 0;
std::vector<double> *stripRDOYL = 0;
std::vector<double> *stripRDOZL = 0;

std::vector<double> *stripRDOGlobX0 = 0;
std::vector<double> *stripRDOGlobY0 = 0;
std::vector<double> *stripRDOGlobZ0 = 0;

std::vector<double> *stripRDOGlobX1 = 0;
std::vector<double> *stripRDOGlobY1 = 0;
std::vector<double> *stripRDOGlobZ1 = 0;

std::vector<double> allStripRDOListGlobR0;
std::vector<double> allStripRDOListGlobZ0;
std::vector<double> barrelStripRDOListGlobX0;
std::vector<double> barrelStripRDOListGlobY0;
std::vector<double> barrelStripRDOListGlobZ0;

std::vector<double> evHitX;
std::vector<double> evHitY;
std::vector<double> evHitZ;
std::vector<double> evHitR;
std::vector<double> evHitPhi;
std::vector<double> stripLowPhi;
std::vector<double> stripHighPhi;

unsigned int nTruth;
std::vector<double> *trPx = 0;
std::vector<double> *trPy = 0;
std::vector<double> *trPz = 0;
std::vector<double> *trVx = 0;
std::vector<double> *trVy = 0;
std::vector<double> *trVz = 0;

double p, px, py, pz, startX, startY, startZ, endX, endY, endZ, dx, dy, dz, length;
int bec, lay, phi, eta, sid, row, str;

//
//=====================================================================================
//
    gROOT->SetStyle("Plain");
    gStyle->SetOptStat(110010); // 110010: overflow, underflow, entries

    TString inputFilename = "SiDigAnalOutput.root";
    cout << "Opening " << inputFilename << " for reading" << endl;
    TFile *f = TFile::Open(inputFilename);
//
//    Create Pixel histograms
//
    TH1F hPixelBarrelEndcapId("PixelBarrelEndcap-id", "Barrel-Endcap values from identifier", 10, -5., 5.0);
    TH1F hPixelLayerId("PixelLayer-id", "Layer-values from identifier", 10, 0., 10.0);
    TH1F hPixelPhiId("PixelPhi-id", "Phi-values from identifier", 100, 0., 100.0);
    TH1F hPixelEtaId("PixelEta-id", "Eta-values from identifier", 60, -30., 30.0);

    TH2F hLocalPixelYX("PixelYvsX", "Local hit coords Y vs X", 101, -50.5, 50.5, 101, -50.5, 50.5);
    TH2F hLocalPixelZX("PixelZvsX", "Local hit coords Z vs X", 101, -50.5, 50.5, 21, -1.05, 1.05);
    TH2F hLocalPixelYZ("PixelYvsZ", "Local hit coords Y vs Z", 21, -1.05, 1.05, 101, -50.5, 50.5);

//
//    Create Strip histograms
//
    TH1F hBarrelEndcapId("BarrelEndcap-id", "Barrel-Endcap values from identifier", 10, -5., 5.0);
    TH1F hLayerId("Layer-id", "Layer-values from identifier", 10, 0., 10.0);
    TH1F hPhiId("Phi-id", "Phi-values from identifier", 100, 0., 100.0);
    TH1F hEtaId("Eta-id", "Eta-values from identifier", 30, -15., 15.0);
    TH1F hSideId("Side-id", "Side-values from identifier", 5, 0., 5);
    TH1F hRowId("Row-id", "Row-values from identifier", 10, 0., 10.0);
    TH1F hStripId("Strip-id", "Strip-values from identifier", 3000, 0., 3000.0);

    TH1F hLayerIdB("Layer-idBarrel", "Barrel Layer-values from identifier", 10, 0., 10.0);
    TH1F hPhiIdB("Phi-idBarrel", "Barrel Phi-values from identifier", 100, 0., 100.0);
    TH1F hEtaIdB("Eta-idBarrel", "Barrel Eta-values from identifier", 30, -15., 15.0);
    TH1F hSideIdB("Side-idBarrel", "Barrel Side-values from identifier", 5, 0., 5);
    TH1F hRowIdB("Row-idBarrel", "Barrel Row-values from identifier", 10, 0., 10.0);
    TH1F hRowIdBSS("Row-idBarrelSS", "Barrel Row-values from identifier, SS sensors", 10, 0., 10.0);
    TH1F hRowIdBLS("Row-idBarrelLS", "Barrel Row-values from identifier, LS sensors", 10, 0., 10.0);
    TH1F hStripIdB("Strip-idBarrel", "Barrel Strip-values from identifier", 3000, 0., 3000.0);

    TH1F hLayerIdEC("Layer-idEC", "EC Layer-values from identifier", 10, 0., 10.0);
    TH1F hPhiIdEC("Phi-idEC", "EC Phi-values from identifier", 100, 0., 100.0);
    TH1F hEtaIdEC("Eta-idEC", "EC Eta-values from identifier", 30, -15., 15.0);
    TH1F hSideIdEC("Side-idEC", "EC Side-values from identifier", 5, 0., 5);
    TH1F hRowIdEC("Row-idEC", "EC Row-values from identifier", 10, 0., 10.0);
    TH1F hStripIdEC("Strip-idEC", "EC Strip-values from identifier", 3000, 0., 3000.0);

    TH1F hClusterWidth("Strip-cluster-width", "Strip Cluster Widths", 20, 0., 20.0);
    TH1F hClusterWidthB("Strip-cluster-width-B", "Barrel Strip Cluster Widths", 20, 0., 20.0);
    TH1F hClusterWidthEC("Strip-cluster-width-EC", "EC Cluster Widths", 20, 0., 20.0);

    TH2F hLocalSSYX("SSYvsX", "Local hit-strip coords Y vs X", 101, -50.5, 50.5, 101, -50.5, 50.5);
    TH2F hLocalSSZX("SSZvsX", "Local hit-strip coords Z vs X", 101, -50.5, 50.5, 21, -1.05, 1.05);
    TH2F hLocalSSYZ("SSYvsZ", "Local hit-strip coords Y vs Z", 21, -1.05, 1.05, 101, -50.5, 50.5);

    TH2F hLocalLSYX("LSYvsX", "Local hit-strip coords Y vs X", 101, -50.5, 50.5, 101, -50.5, 50.5);
    TH2F hLocalLSZX("LSZvsX", "Local hit-strip coords Z vs X", 101, -50.5, 50.5, 21, -1.05, 1.05);
    TH2F hLocalLSYZ("LSYvsZ", "Local hit-strip coords Y vs Z", 21, -1.05, 1.05, 101, -50.5, 50.5);

    TH2F hLocalECYX("ECYvsX", "Local hit-strip coords Y vs X", 650, 350., 1000., 151, -75.5, 75.5);
    TH2F hLocalECZX("ECZvsX", "Local hit-strip coords Z vs X", 650, 350., 1000., 21, -1.05, 1.05);
    TH2F hLocalECYZ("ECYvsZ", "Local hit-strip coords Y vs Z", 21, -1.05, 1.05, 151, -75.5, 75.5);
//
//    Where to store things from the Tree
//
    TTree *t = (TTree *) f->Get("digTree");
//
//    Pixel
//
    t->SetBranchAddress("NPixelRDO", &nPixelRDO);

    t->SetBranchAddress("PixelRDO_GlobalX", &pixelRDOGlobX);
    t->SetBranchAddress("PixelRDO_GlobalY", &pixelRDOGlobY);
    t->SetBranchAddress("PixelRDO_GlobalZ", &pixelRDOGlobZ);

    t->SetBranchAddress("PixelRDO_LocalX", &pixelRDOXL);
    t->SetBranchAddress("PixelRDO_LocalY", &pixelRDOYL);
    t->SetBranchAddress("PixelRDO_LocalZ", &pixelRDOZL);

    t->SetBranchAddress("PixelRDO_BarrelEndcap", &pixelRDO_BarrelEndcap);
    t->SetBranchAddress("PixelRDO_Layer", &pixelRDO_Layer);
    t->SetBranchAddress("PixelRDO_Phi", &pixelRDO_Phi);
    t->SetBranchAddress("PixelRDO_Eta", &pixelRDO_Eta);
//
//    Strips
//
    t->SetBranchAddress("NStripRDO", &nStripRDO);

    t->SetBranchAddress("StripRDO_BarrelEndcap", &stripRDO_BarrelEndcap);
    t->SetBranchAddress("StripRDO_Layer", &stripRDO_Layer);
    t->SetBranchAddress("StripRDO_Phi", &stripRDO_Phi);
    t->SetBranchAddress("StripRDO_Eta", &stripRDO_Eta);
    t->SetBranchAddress("StripRDO_Side", &stripRDO_Side);
    t->SetBranchAddress("StripRDO_Row", &stripRDO_Row);
    t->SetBranchAddress("StripRDO_Strip", &stripRDO_Strip);

    t->SetBranchAddress("StripRDO_size", &stripRDO_Size);

    t->SetBranchAddress("StripRDO_LocalX", &stripRDOXL);
    t->SetBranchAddress("StripRDO_LocalY", &stripRDOYL);
    t->SetBranchAddress("StripRDO_LocalZ", &stripRDOZL);

    t->SetBranchAddress("StripRDO_x0", &stripRDOGlobX0);
    t->SetBranchAddress("StripRDO_y0", &stripRDOGlobY0);
    t->SetBranchAddress("StripRDO_z0", &stripRDOGlobZ0);

    t->SetBranchAddress("StripRDO_x1", &stripRDOGlobX1);
    t->SetBranchAddress("StripRDO_y1", &stripRDOGlobY1);
    t->SetBranchAddress("StripRDO_z1", &stripRDOGlobZ1);

    t->SetBranchAddress("NTruth", &nTruth);
    t->SetBranchAddress("TruthPx", &trPx);
    t->SetBranchAddress("TruthPy", &trPy);
    t->SetBranchAddress("TruthPz", &trPz);
    t->SetBranchAddress("TruthVx", &trVx);
    t->SetBranchAddress("TruthVy", &trVy);
    t->SetBranchAddress("TruthVz", &trVz);
//
//    Event display canvas
//
    TCanvas evCanvas("evCanvas", "evCanvas", 1000, 800);
    evCanvas.Divide(2, 2);
    evCanvas.Paint();
    int nHits = 0;
    string input;
    bool doDisplay(true);
//
//    Event loop
//
    int nEntries = (int) t->GetEntries();
    for (int ientry = 0; ientry < nEntries; ientry++) {
        t->GetEntry(ientry);
//
//    Pixels
//
        for (int i = 0; i < nPixelRDO; i++) {

            double x = pixelRDOGlobX->at(i);
            double y = pixelRDOGlobY->at(i);
            double z = pixelRDOGlobZ->at(i);
            double r = sqrt(x * x + y * y);
            double phiAng = atan2(y, x);

            allPixelRDOListGlobR.push_back(r);
            allPixelRDOListGlobZ.push_back(z);

            if (pixelRDO_BarrelEndcap->at(i) == 0) {
                barrelPixelRDOListGlobX.push_back(x);
                barrelPixelRDOListGlobY.push_back(y);
                barrelPixelRDOListGlobZ.push_back(z);
            }
            else {
                endcapPixelRDOListGlobX.push_back(x);
                endcapPixelRDOListGlobY.push_back(y);
                endcapPixelRDOListGlobZ.push_back(z);
            }

            hPixelBarrelEndcapId.Fill(pixelRDO_BarrelEndcap->at(i));
            hPixelLayerId.Fill(pixelRDO_Layer->at(i));
            hPixelPhiId.Fill(pixelRDO_Phi->at(i));
            hPixelEtaId.Fill(pixelRDO_Eta->at(i));

            hLocalPixelYX.Fill(pixelRDOXL->at(i), pixelRDOYL->at(i));
            hLocalPixelZX.Fill(pixelRDOXL->at(i), pixelRDOZL->at(i));
            hLocalPixelYZ.Fill(pixelRDOZL->at(i), pixelRDOYL->at(i));

            evHitX.push_back(x);
            evHitY.push_back(y);
            evHitZ.push_back(z);
            evHitR.push_back(r);
            evHitPhi.push_back(phiAng);
            stripLowPhi.push_back(0.025 / r);  // Not relevant to pixels, but we need to pad the vectors out
            stripHighPhi.push_back(0.025 / r); // 50 micron pitch
            nHits += 1;
        }

//
//    Strips
//
        for (int i = 0; i < nStripRDO; ++i) { // Loop over RDOs in this event

            bec = stripRDO_BarrelEndcap->at(i);
            lay = stripRDO_Layer->at(i);
            phi = stripRDO_Phi->at(i);
            eta = stripRDO_Eta->at(i);
            sid = stripRDO_Side->at(i);
            row = stripRDO_Row->at(i);
            str = stripRDO_Strip->at(i);

            hBarrelEndcapId.Fill(bec);
            hLayerId.Fill(lay);
            hPhiId.Fill(phi);
            hEtaId.Fill(eta);
            hSideId.Fill(sid);
            hRowId.Fill(row);
            hStripId.Fill(str);

            if (bec == 0) {
                hLayerIdB.Fill(lay);
                hPhiIdB.Fill(phi);
                hEtaIdB.Fill(eta);
                hSideIdB.Fill(sid);
                hRowIdB.Fill(row);
if (stripRDO_Size->at(1) == 1) {
                if (lay < NB_SS_LAYERS) {
                    hRowIdBSS.Fill(row);
                }
                else { // LS
                    hRowIdBLS.Fill(row);
                }
                hStripIdB.Fill(str);
            }
}
            else {
                hLayerIdEC.Fill(lay);
                hPhiIdEC.Fill(phi);
                hEtaIdEC.Fill(eta);
                hSideIdEC.Fill(sid);
                hRowIdEC.Fill(row);
                hStripIdEC.Fill(str);
            }
//
//    Cluster widths
//
            hClusterWidth.Fill(stripRDO_Size->at(i));
            if (bec == 0) {
                hClusterWidthB.Fill(stripRDO_Size->at(i));
            }
            else {
                hClusterWidthEC.Fill(stripRDO_Size->at(i));
            }
//
//    Local strip-centre coords. In Trk frame: 0/X - phi; 1/Y - eta; 2/Z - depth
//    I make hists in GeoModel local frame. Names: YX means plot Y vs X. First par is X. But I swap X and Y for geoModel frame
//
            if (bec == 0) {
                if (lay < NB_SS_LAYERS) { // Barrel SS
                    hLocalSSYX.Fill(stripRDOYL->at(i), stripRDOXL->at(i));
                    hLocalSSZX.Fill(stripRDOYL->at(i), stripRDOZL->at(i));
                    hLocalSSYZ.Fill(stripRDOZL->at(i), stripRDOXL->at(i));
                }
                else { // Barrel LS
                    hLocalLSYX.Fill(stripRDOYL->at(i), stripRDOXL->at(i));
                    hLocalLSZX.Fill(stripRDOYL->at(i), stripRDOZL->at(i));
                    hLocalLSYZ.Fill(stripRDOZL->at(i), stripRDOXL->at(i));
                }
            }
            else { // EC
                hLocalECYX.Fill(stripRDOYL->at(i), stripRDOXL->at(i));
                hLocalECZX.Fill(stripRDOYL->at(i), stripRDOZL->at(i));
                hLocalECYZ.Fill(stripRDOZL->at(i), stripRDOXL->at(i));
            }
//
//    Global coords
//
            double x0 = stripRDOGlobX0->at(i);
            double y0 = stripRDOGlobY0->at(i);
            double z0 = stripRDOGlobZ0->at(i);
            double x1 = stripRDOGlobX1->at(i);
            double y1 = stripRDOGlobY1->at(i);
            double z1 = stripRDOGlobZ1->at(i);
            double x = (x0 + x1) /2.;
            double y = (y0 + y1) /2.;
            double z = (z0 + z1) /2.;
            double r = sqrt(x * x + y * y);
            double phiAng = atan2(y, x);

            allStripRDOListGlobR0.push_back(r);
            allStripRDOListGlobZ0.push_back(z);

            evHitX.push_back(x);
            evHitY.push_back(y);
            evHitZ.push_back(z);
            evHitR.push_back(r);
            evHitPhi.push_back(phiAng);
            stripLowPhi.push_back(fabs(atan2(y1, x1) - phiAng));
            stripHighPhi.push_back(fabs(atan2(y0, x0) - phiAng));
            nHits += 1;
/*
cout << stripRDO_BarrelEndcap->at(i) << " " <<  stripRDO_Layer->at(i) << " " <<  stripRDO_Phi->at(i) << " " <<  
        stripRDO_Eta->at(i) << " " <<  stripRDO_Side->at(i) << " " <<  stripRDO_Row->at(i) << " " <<  stripRDO_Strip->at(i) << 
        " z = " << z << endl;
*/
            if (stripRDO_BarrelEndcap->at(i) == 0) {
                barrelStripRDOListGlobX0.push_back(x);
                barrelStripRDOListGlobY0.push_back(y);
                barrelStripRDOListGlobZ0.push_back(z);
            }      
        }
//
//    Track from truth (first only)
//
        if (nTruth > 0) {
            px = trPx->at(0);
            py = trPy->at(0);
            pz = trPz->at(0);
            p = sqrt(px * px + py * py + pz * pz);
            dx = px / p;
            dy = py / p;
            dz = pz / p; // = cos theta
            startX = startY = startZ = 0.0;
            if (trVx->size() > 0) {
                startX = trVx->at(0);
                startY = trVy->at(0);
                startZ = trVz->at(0);
            }

            // Does it leave the side or the ends?
            // Track length to reach OuterRadius: r / sin theta
            length = OuterRadius / sqrt(1. - dz * dz);
            endZ = startZ + length * dz;
            if (fabs(endZ) > EndZ) { // leaves ends
                endX = startX + (EndZ - startZ) * dx / dz;
                endY = startY + (EndZ - startZ) * dy / dz;
                endZ = EndZ;
            }
            else { // Leaves cyl 
                endX = startX + length * dx;
                endY = startY + length * dy;
            }
//            cout << "Truth track: theta = " << atan2(r, z) << "; phi = " << phiAng << "End: (" << 
//            endX << ", " << endY << ", " << endZ << ")\n";
        }
//
//    Event display
//
        if (doDisplay && nHits > 0) {
            evCanvas.cd(1);
            TGraph evDisplayRZ(nHits, &(evHitZ[0]), &(evHitR[0]));
            evDisplayRZ.SetTitle("RZ event display");
            evDisplayRZ.GetXaxis()->SetTitle("z (mm)");
            evDisplayRZ.GetYaxis()->SetTitle("r (mm)");
            evDisplayRZ.GetXaxis()->SetLimits(-EndZ, EndZ);
            evDisplayRZ.GetHistogram()->SetMinimum(0.);
            evDisplayRZ.GetHistogram()->SetMaximum(OuterRadius);
            evDisplayRZ.Draw("a*");

            evCanvas.cd(2);
            TGraph evDisplayXY(nHits, &(evHitX[0]), &(evHitY[0]));
            evDisplayXY.SetTitle("XY event display");
            evDisplayXY.GetXaxis()->SetTitle("x (mm)");
            evDisplayXY.GetYaxis()->SetTitle("y (mm)");
            evDisplayXY.GetXaxis()->SetLimits(-OuterRadius, OuterRadius);
            evDisplayXY.GetHistogram()->SetMinimum(-OuterRadius);
            evDisplayXY.GetHistogram()->SetMaximum(OuterRadius);
            evDisplayXY.Draw("a*");

            evCanvas.cd(3);
            double trackPhi = atan2(py, px);
            for (int h = 0; h < nHits; ++h) {
                evHitPhi[h] -= trackPhi;
            }
            TGraphAsymmErrors evDisplayPhiZ(nHits, &evHitZ[0], &evHitPhi[0], 0, 0, &stripLowPhi[0], &stripHighPhi[0]); 
            evDisplayPhiZ.SetTitle("Phi-z event display");
            evDisplayPhiZ.GetXaxis()->SetTitle("z (mm)");
            evDisplayPhiZ.GetYaxis()->SetTitle("Deviation of RDO-phi from muon phi (rad)");
            evDisplayPhiZ.GetXaxis()->SetLimits(-EndZ, EndZ);
//            evDisplayPhiZ.GetHistogram()->SetMinimum(0.00);
//            evDisplayPhiZ.GetHistogram()->SetMaximum(0.03);
            evDisplayPhiZ.SetMarkerColor(4);
            evDisplayPhiZ.SetMarkerStyle(21);
            evDisplayPhiZ.SetMarkerSize(0.25);
            evDisplayPhiZ.Draw("ap");


            evCanvas.cd(4);
            TGraphAsymmErrors evDisplayPhiR(nHits, &evHitR[0], &evHitPhi[0], 0, 0, &stripLowPhi[0], &stripHighPhi[0]); 
            evDisplayPhiR.SetTitle("Phi-r event display");
            evDisplayPhiR.GetXaxis()->SetTitle("r (mm)");
            evDisplayPhiR.GetYaxis()->SetTitle("Deviation of RDO-phi from muon phi (rad)");
            evDisplayPhiR.GetXaxis()->SetLimits(0., OuterRadius);
//            evDisplayPhiR.GetHistogram()->SetMinimum(0.00);
//            evDisplayPhiR.GetHistogram()->SetMaximum(0.03);
            evDisplayPhiR.SetMarkerColor(4);
            evDisplayPhiR.SetMarkerStyle(21);
            evDisplayPhiR.SetMarkerSize(0.25);
            evDisplayPhiR.Draw("ap");

            evCanvas.Update();

            ostringstream evFilename;
            evFilename << "evDisp" << setw(3) << setfill('0') << ientry << ".pdf";
            evCanvas.SaveAs(evFilename.str().c_str());

            std::cout <<  "Number of hits was " << evHitZ.size() << 
                          ". Type n to stop event display [anything else to continue]: ";
            std::cin >> input;
            if (input == "n") {
                doDisplay = 0;
            }

            evCanvas.Clear("D");
            nHits = 0;
            evHitX.clear();
            evHitY.clear();
            evHitZ.clear();
            evHitR.clear();
            evHitPhi.clear();
            stripLowPhi.clear();
            stripHighPhi.clear();
        }
    } // event loop

    // Draw plots

    int nEndcapPixelRDOs = endcapPixelRDOListGlobX.size();
    if (nEndcapPixelRDOs > 0) {
        TCanvas p0("p0", "p0", 600, 600);
        TGraph gPixelRDOsGlobalXYendcap(nEndcapPixelRDOs, &endcapPixelRDOListGlobX[0], &endcapPixelRDOListGlobY[0]);
        gPixelRDOsGlobalXYendcap.SetTitle("Pixel Endcap RDO Locations");
        gPixelRDOsGlobalXYendcap.GetXaxis()->SetTitle("global X coordinate (mm)");
        gPixelRDOsGlobalXYendcap.GetYaxis()->SetTitle("global Y coordinate (mm)");
        gPixelRDOsGlobalXYendcap.Draw("ap");
        p0.SaveAs("PixelEndcapRDOsXY.pdf");
    }

    int nBarrelPixelRDOs = barrelPixelRDOListGlobX.size();
    if (nBarrelPixelRDOs > 0) {
        TCanvas p1("p1", "p1", 600, 600);
        TGraph gPixelRDOsGlobalXYbarrel(nBarrelPixelRDOs, &(barrelPixelRDOListGlobX[0]), &(barrelPixelRDOListGlobY[0]));
        gPixelRDOsGlobalXYbarrel.SetTitle("Pixel Barrel RDO Locations");
        gPixelRDOsGlobalXYbarrel.GetXaxis()->SetTitle("global X coordinate (mm)");
        gPixelRDOsGlobalXYbarrel.GetYaxis()->SetTitle("global Y coordinate (mm)");
        gPixelRDOsGlobalXYbarrel.Draw("ap");
        p1.SaveAs("PixelBarrelRDOsXY.pdf");
    }

    int nAllPixelRDOs = allPixelRDOListGlobR.size();
    if (nAllPixelRDOs > 0) {
        TCanvas p2("p2","p2");
        TGraph gPixelRDOsGlobalRZ(nAllPixelRDOs, &allPixelRDOListGlobZ[0], &allPixelRDOListGlobR[0]);
        gPixelRDOsGlobalRZ.SetTitle("Pixel RDO Locations");
        gPixelRDOsGlobalRZ.GetXaxis()->SetTitle("global Z coordinate (mm)");
        gPixelRDOsGlobalRZ.GetYaxis()->SetTitle("global R coordinate (mm)");
        gPixelRDOsGlobalRZ.Draw("ap");
        p2.SaveAs("PixelRDOsRZ.pdf");

        TCanvas p3("p3", "p3");
        p3.Divide(2, 2);
        p3.cd(1);
        hPixelBarrelEndcapId.Draw();
        p3.cd(2);
        hPixelLayerId.Draw();
        p3.cd(3);
        hPixelPhiId.Draw();
        p3.cd(4);
        hPixelEtaId.Draw();
        p3.SaveAs("PixelIDs.pdf");

        TCanvas p4("p4", "p4", 1000, 800);
        p4.Divide(2, 2);
        p4.cd(1);
        hLocalPixelYX.Draw("COLZ");
        p4.cd(2);
        hLocalPixelYZ.Draw("COLZ");
        p4.cd(3);
        hLocalPixelZX.Draw("COLZ");
        p4.SaveAs("PixelHitLocalCoords.pdf");
    }

    int nBarrelStripRDOs = barrelStripRDOListGlobX0.size();
    if (nBarrelStripRDOs > 0) {
        TCanvas c7("c7","c7",600,600);
        TGraph gStripRDOsGlobalXYbarrel(nBarrelStripRDOs, &barrelStripRDOListGlobX0[0], &barrelStripRDOListGlobY0[0]);
        gStripRDOsGlobalXYbarrel.SetTitle("Barrel Strip RDO y-x global positions");
        gStripRDOsGlobalXYbarrel.GetXaxis()->SetTitle("global X0 coordinate (mm)");
        gStripRDOsGlobalXYbarrel.GetYaxis()->SetTitle("global Y0 coordinate (mm)");
        gStripRDOsGlobalXYbarrel.Draw("ap");
        c7.SaveAs("StripBarrelRDOsXY.pdf");
    }
    else {
        cout << "No barrelStripRDOs!\n";
    }

    int nAllStripRDOs = allStripRDOListGlobR0.size();
    if (nAllStripRDOs > 0) {
        TCanvas c8("c8","c8");

        TGraph gStripRDOsGlobalRZ(nAllStripRDOs, &allStripRDOListGlobZ0[0], &allStripRDOListGlobR0[0]);

        gStripRDOsGlobalRZ.SetTitle("All Strip RDO r-z global positions");
        gStripRDOsGlobalRZ.GetXaxis()->SetTitle("global Z0 coordinate (mm)");
        gStripRDOsGlobalRZ.GetYaxis()->SetTitle("global R0 coordinate (mm)");
        gStripRDOsGlobalRZ.Draw("ap");
        c8.SaveAs("StripRDOsRZ.pdf");
    }

    TCanvas c9("c9", "c9", 1000, 800);
    c9.Divide(4, 2);
    c9.cd(1);
    hBarrelEndcapId.Draw();
    c9.cd(2);
    hLayerId.Draw();
    c9.cd(3);
    hPhiId.Draw();
    c9.cd(4);
    hEtaId.Draw();
    c9.cd(5);
    hSideId.Draw();
    c9.cd(6);
    hRowId.Draw();
    c9.cd(7);
    hStripId.Draw();
    c9.SaveAs("Id.pdf");

    TCanvas cl("cl", "cl", 1000, 800);
    cl.Divide(3, 1);
    cl.cd(1);
    hClusterWidth.Draw();
    cl.cd(2);
    hClusterWidthB.Draw();
    cl.cd(3);
    hClusterWidthEC.Draw();
    cl.SaveAs("ClusterWidth.pdf");

    TCanvas cA("cA", "cA", 1000, 800);
    cA.Divide(4, 2);
    cA.cd(1);
    hLayerIdB.Draw();
    cA.cd(2);
    hPhiIdB.Draw();
    cA.cd(3);
    hEtaIdB.Draw();
    cA.cd(4);
    hSideIdB.Draw();
    cA.cd(5);
    hRowIdB.Draw();
    cA.cd(6);
    hStripIdB.Draw();
    cA.cd(7);
    hRowIdBSS.Draw();
    cA.cd(8);
    hRowIdBLS.Draw();
    cA.SaveAs("BarrelId.pdf");

    TCanvas cB("cB", "cB", 1000, 800);
    cB.Divide(3, 2);
    cB.cd(1);
    hLayerIdEC.Draw();
    cB.cd(2);
    hPhiIdEC.Draw();
    cB.cd(3);
    hEtaIdEC.Draw();
    cB.cd(4);
    hSideIdEC.Draw();
    cB.cd(5);
    hRowIdEC.Draw();
    cB.cd(6);
    hStripIdEC.Draw();
    cB.SaveAs("EndcapId.pdf");

    TCanvas cC("cC", "cC", 1000, 800);
    cC.Divide(2, 2);
    cC.cd(1);
    hLocalSSYX.Draw("COLZ");
    cC.cd(2);
    hLocalSSYZ.Draw("COLZ");
    cC.cd(3);
    hLocalSSZX.Draw("COLZ");
    cC.SaveAs("SSHitStripCoords.pdf");

    cC.Clear("D");
    cC.cd(1);
    hLocalLSYX.Draw("COLZ");
    cC.cd(2);
    hLocalLSYZ.Draw("COLZ");
    cC.cd(3);
    hLocalLSZX.Draw("COLZ");
    cC.SaveAs("LSHitStripCoords.pdf");

    cC.Clear("D");
    cC.cd(1);
    hLocalECYX.Draw("COLZ");
    cC.cd(2);
    hLocalECYZ.Draw("COLZ");
    cC.cd(3);
    hLocalECZX.Draw("COLZ");
    cC.SaveAs("ECHitStripCoords.pdf");

}

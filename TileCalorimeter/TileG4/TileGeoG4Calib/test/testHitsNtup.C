/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <TChain.h>
#include <TH1F.h>
#include <TH2F.h>

Int_t testHitsNtup(Char_t* hits_file="hits.Ntup.root", Int_t det=1, Int_t Calc=0)
    // det=0 - Z_R-Tile, det=1 - Z_R-Barrel, det=2 - Z_R-Ext.Barrel
    // det=11...17 - zoom in Barrel regions 1-7, det=21...27 - zoom in Ext.Barrel regions 1-7
    // det=-21...-27 - zoom in Negative Ext.Barrel regions 1-7
        // Calc (IDcalc): 0-All IDCalculators, 1-CellIDCalculator, 2-ScintIDCalculator,
        //                3-PlateCellIDCalculator, 4-GirderCellIDCalculator,
        //                5-DefaultHitIDCalculator-Cell hit, 6-DefaultHitIDCalculator-DM hit
{

// Various scales of Z_R figure
// by default or wrong det - Z_R-Barrel (det=1):
Int_t Zbin=3100, Rbin=2100;
Float_t Zbeg=0., Zend=3100., Rbeg=2200., Rend=4300.;

if      (abs(det)== 1) { Zbin=3100; Zbeg=   0.; Zend=3100.; }  //Z_R-Barrel (by default)
else if (abs(det)== 2) { Zbin=3500; Zbeg=3100.; Zend=6600.; }  //Z_R-Ext.Barrel

else if (abs(det)==11) { Zbin=2000; Zbeg= 200.; Zend= 400.; }  //Z_R-Barrel-Zoom det1, reg1
else if (abs(det)==12) { Zbin=3000; Zbeg= 400.; Zend= 700.; }
else if (abs(det)==13) { Zbin=3500; Zbeg= 700.; Zend=1050.; }
else if (abs(det)==14) { Zbin=2250; Zbeg=1050.; Zend=1500.; }
else if (abs(det)==15) { Zbin=2500; Zbeg=1500.; Zend=2000.; }
else if (abs(det)==16) { Zbin=2500; Zbeg=2000.; Zend=2500.; }
else if (abs(det)==17) { Zbin=2500; Zbeg=2500.; Zend=3000.; }

else if (abs(det)==21) { Zbin=2000; Zbeg=3200.; Zend=3600.; }  //Z_R-Barrel-Zoom det2, reg1
else if (abs(det)==22) { Zbin=2500; Zbeg=3600.; Zend=4100.; }
else if (abs(det)==23) { Zbin=2500; Zbeg=4100.; Zend=4600.; }
else if (abs(det)==24) { Zbin=2500; Zbeg=4600.; Zend=5100.; }
else if (abs(det)==25) { Zbin=2500; Zbeg=5100.; Zend=5600.; }
else if (abs(det)==26) { Zbin=2500; Zbeg=5600.; Zend=6100.; }
else if (abs(det)==27) { Zbin=2500; Zbeg=6100.; Zend=6600.; }

else if (det== 0) { Zbin=3300; Zbeg=   0.; Zend=6600.; Rbin=1500; Rbeg=1300.; Rend=4300.; }  //Z_R-Tile

if(det<0 && abs(det)>20) { double zzz= -Zbeg; Zbeg= -Zend; Zend= zzz; }


// TH1F *Energy_hits = new TH1F("Energy_hits", "hit Energy",1050, -0.005, 0.1);

// Histograms definition of various TileCalVolumes
TH2F *z_R = new TH2F("z_R", "z_R",Zbin,Zbeg,Zend,Rbin,Rbeg,Rend);
TH2F *z_R_2 = new TH2F("z_R_2", "z_R_2",Zbin,Zbeg,Zend,Rbin,Rbeg,Rend);

TH2F *z_R_saddle_b = new TH2F("z_R_saddle_b", "z_R_saddle_b",Zbin,Zbeg,Zend,Rbin,Rbeg,Rend);
TH2F *z_R_saddle_eb = new TH2F("z_R_saddle_eb", "z_R_saddle_eb",Zbin,Zbeg,Zend,Rbin,Rbeg,Rend);
TH2F *z_R_saddle_2 = new TH2F("z_R_saddle_2", "z_R_saddle_2",Zbin,Zbeg,Zend,Rbin,Rbeg,Rend);
TH2F *z_R_ir_1 = new TH2F("z_R_ir_1", "z_R_ir_1",Zbin,Zbeg,Zend,Rbin,Rbeg,Rend);
TH2F *z_R_ir_2 = new TH2F("z_R_ir_2", "z_R_ir_2",Zbin,Zbeg,Zend,Rbin,Rbeg,Rend);
TH2F *z_R_finger_b = new TH2F("z_R_finger_b", "z_R_finger_b",Zbin,Zbeg,Zend,Rbin,Rbeg,Rend);
TH2F *z_R_finger_eb = new TH2F("z_R_finger_eb", "z_R_finger_eb",Zbin,Zbeg,Zend,Rbin,Rbeg,Rend);

TH2F *z_R_gilder_b[7];
char name_g[100], tittle_g[100];
for(Int_t et=0; et<7; et++) {
    sprintf(name_g,"z_R_gilder_b_%d",et);
    sprintf(tittle_g,"z_R_gilder_b_%d",et);
    z_R_gilder_b[et] = new TH2F(name_g,tittle_g,Zbin,Zbeg,Zend,Rbin,Rbeg,Rend);
}
TH2F *z_R_gilder_eb[4];
char name_g[100], tittle_g[100];
for(Int_t et=0; et<4; et++) {
    sprintf(name_g,"z_R_gilder_eb_%d",et);
    sprintf(tittle_g,"z_R_gilder_eb_%d",et);
    z_R_gilder_eb[et] = new TH2F(name_g,tittle_g,Zbin,Zbeg,Zend,Rbin,Rbeg,Rend);
}
TH2F *z_R_gilder_2 = new TH2F("z_R_gilder_2", "z_R_gilder_2" ,Zbin,Zbeg,Zend,Rbin,Rbeg,Rend);

TH2F *z_R_lost = new TH2F("z_R_lost", "z_R_lost",Zbin,Zbeg,Zend,Rbin,Rbeg,Rend);


    // Add input file to chain
    TChain * chain = new TChain("TileCalibHit");
    chain->Add(hits_file);
    Long64_t entries = chain->GetEntries();
    if (entries == 0) {
        cout << "Error! TChain has not entries!" << endl;
        return 1;
    } else {
        cout << "Number of entries in TChain is " << entries << endl;
    }

    Short_t subdet, nphi, neta, sample, region, IDcalc, event;
        // written for Cell: _detector, _module,  _tower,   _sample,  _cellNum
        // written for   DM: dm_subDet, dm_nphi, dm_neta, dm_sample, dm_region
    Float_t x, y, z, E, R;
    Int_t lost=0, good=0;

    // Set branch addresses
        chain->SetBranchAddress("detector", &subdet);
        chain->SetBranchAddress("module",   &nphi);
        chain->SetBranchAddress("tower",    &neta);
        chain->SetBranchAddress("sample",   &sample);
        chain->SetBranchAddress("region",   &region);
        chain->SetBranchAddress("IDcalc",   &IDcalc);
        chain->SetBranchAddress("event",    &event);
        chain->SetBranchAddress("xPos",     &x);
        chain->SetBranchAddress("yPos",     &y);
        chain->SetBranchAddress("zPos",     &z);
        chain->SetBranchAddress("energy",   &E);


    // Loop over all events
    for (Long64_t jentry=0; jentry<entries; jentry++) {
        chain->GetEntry(jentry);
        if(Calc != 0 && Calc != IDcalc) continue;  //choice of the necessary IDCalculator

        R = sqrt(x*x+y*y);
        if(Zbeg>=0 && abs(det)<21) z= fabs(z);
        if(z<Zbeg || z>Zend || R<Rbeg || R>Rend) continue;  //hit is out of figure scale

        // Energy_hits->Fill(E);

        // Filling of various TileCalVolumes by hits
        if(abs(subdet) == 5) { // Dead Material
            good++;
            if (sample==0) {
                if (neta%2==0) z_R_ir_1->Fill(z,R);
                else z_R_ir_2->Fill(z,R);
            } else if (sample==1) {
                if (region==0) {
                    z_R_saddle_b->Fill(z,R);
                } else if (region==2) {
                    z_R_saddle_eb->Fill(z,R);
                } else if (region==1) {
                    z_R_saddle_2->Fill(z,R);
                } else {
                    z_R_lost->Fill(z,R);
                    lost++;
                }
            } else if (sample==2) {
                if (region==0) {
                    if (neta == 7) {
                        z_R_finger_b->Fill(z,R);
                    } else {
                        z_R_gilder_b[neta]->Fill(z,R);
                    }
                } else if (region==1) {
                    if (neta == 4) {
                        z_R_finger_eb->Fill(z,R);
                    } else {
                        z_R_gilder_eb[neta]->Fill(z,R);
                    }
                } else if (region==2) {
                    z_R_gilder_2->Fill(z,R);
                } else {
                    z_R_lost->Fill(z,R);
                    lost++;
                }
            } else {
                z_R_lost->Fill(z,R);
                lost++;
            }
        } else if ((abs(subdet)==1) || (abs(subdet)==2)) {
            good++;
            if ((abs(subdet)==2)&&(sample==0)) {
                if (region%2==0) z_R_2->Fill(z,R);
                else z_R->Fill(z,R);
            } else {
                if (region%2==0) z_R->Fill(z,R);
                else z_R_2->Fill(z,R);
            }
        } else if (abs(subdet)==3) {
            good++;
            if (region%2==0) z_R->Fill(z,R);
            else z_R_2->Fill(z,R);
        } else {
            z_R_lost->Fill(z,R);
            lost++;
        }
    }  //end of event loop
    cout<< "good= " << good << "  lost= " << lost << endl;

//------------------------------------------------
// Histograms drawing
//------------------------------------------------

// TCanvas *c2 = new TCanvas("c2","Energy",964,748);
// c2->SetLogy();
// Energy_hits->GetYaxis()->SetTitle("hits");
// Energy_hits->GetXaxis()->SetTitle("Energy");

// Energy_hits->Draw();
// //c2->Print("E_h.eps");
// c2->SaveAs("E_h.gif");
// //c2->SaveAs("E_h.root");


// Set colors of various TileCalVolumes
z_R->SetMarkerColor(kMagenta);
z_R->SetStats(false);
z_R_2->SetMarkerColor(kBlue);
z_R_2->SetStats(false);

z_R_ir_1->SetMarkerColor(kMagenta);
z_R_ir_1->SetStats(false);
z_R_ir_2->SetMarkerColor(kBlue);
z_R_ir_2->SetStats(false);

z_R_saddle_b->SetMarkerColor(kGreen+1);
z_R_saddle_b->SetStats(false);
z_R_saddle_eb->SetMarkerColor(kGreen+1);
z_R_saddle_eb->SetStats(false);
z_R_saddle_2->SetMarkerColor(kGreen+2);
z_R_saddle_2->SetStats(false);

z_R_finger_b->SetMarkerColor(kYellow);
z_R_finger_b->SetStats(false);
z_R_finger_eb->SetMarkerColor(kYellow);
z_R_finger_eb->SetStats(false);

for(Int_t et=0; et<7; et++) {
    if (et%2==0) {
        z_R_gilder_b[et]->SetMarkerColor(kGreen);
    } else {
        z_R_gilder_b[et]->SetMarkerColor(kGreen+1);
    }
    z_R_gilder_b[et]->SetStats(false);
}
for(Int_t et=0; et<4; et++) {
    if (et%2==0) {
        z_R_gilder_eb[et]->SetMarkerColor(kGreen);
    } else {
        z_R_gilder_eb[et]->SetMarkerColor(kGreen+1);
    }
    z_R_gilder_eb[et]->SetStats(false);
}
z_R_gilder_2->SetMarkerColor(kGreen+2);
z_R_gilder_2->SetStats(false);

z_R_lost->SetMarkerColor(kRed);
z_R_lost->SetStats(false);


// Draw of various TileCalVolumes
TCanvas *c3 = new TCanvas("c3","Z_R",964,748);
if (det==0) z_R_saddle_b->SetTitle("Z - R, the whole Tile");
else if (det==1) z_R_saddle_b->SetTitle("Z - R, Barrel");
else if (det==2) z_R_saddle_b->SetTitle("Z - R, Ext.Barrel");
else if (abs(det)>10 && abs(det)<20) z_R_saddle_b->SetTitle("Z - R, Barrel - zoom");
else if (abs(det)>20) z_R_saddle_b->SetTitle("Z - R, Ext.Barrel - zoom");
else z_R_saddle_b->SetTitle("Z - R, Barrel");
z_R_saddle_b->GetYaxis()->SetTitleOffset(1.3);
z_R_saddle_b->GetYaxis()->SetTitle("R, mm");
z_R_saddle_b->GetXaxis()->SetTitle("Z, mm");

z_R_saddle_b->Draw();
z_R_saddle_eb->Draw("same");
z_R_saddle_2->Draw("same");
z_R_ir_1->Draw("same");
z_R_ir_2->Draw("same");
z_R_finger_b->Draw("same");
z_R_finger_eb->Draw("same");

for(Int_t et=0; et<7; et++) {
    z_R_gilder_b[et]->Draw("same");
}
for(Int_t et=0; et<4; et++) {
    z_R_gilder_eb[et]->Draw("same");
}
z_R_gilder_2->Draw("same");

z_R->Draw("same");
z_R_2->Draw("same");

z_R_lost->Draw("same");

//c3->Print("Z_R.eps");
c3->SaveAs("Z_R.gif");
//c3->SaveAs("Z_R.root");

return 0;
}


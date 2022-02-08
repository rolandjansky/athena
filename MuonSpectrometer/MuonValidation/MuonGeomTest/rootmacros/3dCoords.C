/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#include <iostream.h>
//#include <math.h>

void main() {
    TFile *f = new TFile("ntuple.root");
    TTree *t1 = (TTree *)f->Get("10");
    float x, y, z;

    // Histos defined in order z, x, y
    // For BOG chambers
    // TH3F *hcoords = new TH3F("hcoords", "3D coords of ML corners (BOG)", 300, -15000., 15000., 100, -5000., 5000., 160, -10600., -9000.);

    // For BIL
    // TH3F *hcoords = new TH3F("hcoords", "3D coords of ML corners (BIL)", 140, -7000., 7000., 120, -6000., 6000., 120, -6000., 6000.);

    // For BIS
    // TH3F *hcoords = new TH3F("hcoords", "3D coords of ML corners (BIS)", 160, -8000., 8000., 90, -4500., 4500., 100, -5000., 5000.);

    // For BML
    // TH3F *hcoords = new TH3F("hcoords", "3D coords of ML corners (BML)", 200, -10000., 10000., 160, -8000., 8000., 160, -8000., 8000.);

    // For BMS
    // TH3F *hcoords = new TH3F("hcoords", "3D coords of ML corners (BMS)", 200, -10000., 10000., 160, -8000., 8000., 160, -8000., 8000.);

    // For BOL
    // TH3F *hcoords = new TH3F("hcoords", "3D coords of ML corners (BOL)", 260, -13000., 13000., 200, -10000., 10000., 200, -10000.,
    // 10000.);

    // For BOS
    // TH3F *hcoords = new TH3F("hcoords", "3D coords of ML corners (BOS)", 280, -14000., 14000., 220, -11000., 11000., 220, -11000.,
    // 11000.);

    // For EIL
    // TH3F *hcoords = new TH3F("hcoords", "3D coords of ML corners (EIL)", 180, -9000., 9000., 140, -7000., 7000., 140, -7000., 7000.);

    // For EIS
    // TH3F *hcoords = new TH3F("hcoords", "3D coords of ML corners (EIS)", 160, -8000., 8000., 100, -5000., 5000., 100, -5000., 5000.);

    // For EML
    // TH3F *hcoords = new TH3F("hcoords", "3D coords of ML corners (EML)", 300, -15000., 15000., 240, -12000., 12000., 240, -12000.,
    // 12000.);

    // For EMS
    // TH3F *hcoords = new TH3F("hcoords", "3D coords of ML corners (EMS)", 300, -15000., 15000., 240, -12000., 12000., 240, -12000.,
    // 12000.);

    // For EOL
    // TH3F *hcoords = new TH3F("hcoords", "3D coords of ML corners (EOL)", 440, -22000., 22000., 260, -13000., 13000., 260, -13000.,
    // 13000.);

    // For EOS
    // TH3F *hcoords = new TH3F("hcoords", "3D coords of ML corners (EOS)", 460, -23000., 23000., 250, -12500., 12500., 250, -12500.,
    // 12500.);

    TLeaf *lx = t1->GetLeaf("x");
    TLeaf *ly = t1->GetLeaf("y");
    TLeaf *lz = t1->GetLeaf("z");

    for (int i = 0; i < t1->GetEntries(); i++) {
        t1->GetEntry(i);
        x = lx->GetValue();
        y = ly->GetValue();
        z = lz->GetValue();

        // Note order of filling histo
        hcoords->Fill(z, x, y);
    }
    hcoords->Draw();
}

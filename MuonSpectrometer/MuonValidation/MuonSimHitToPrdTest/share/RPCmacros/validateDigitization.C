#define validateDigitization_cxx
#include <TAxis.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TStyle.h>
#include <TVector3.h>
#include <math.h>

#include <iostream>
#include <string>

#include "validateDigitization.h"

void validateDigitization::Loop(Long64_t maxEvent = 99999999999) {
    //   In a ROOT session, you can do:
    //      root> .L validateDigitization.C
    //      root> validateDigitization t
    //      root> t.GetEntry(12); // Fill t data members with entry number 12
    //      root> t.Show();       // Show values of entry 12
    //      root> t.Show(16);     // Read and show values of entry 16
    //      root> t.Loop();       // Loop on all entries
    //

    //     This is the loop skeleton where:
    //    jentry is the global entry number in the chain
    //    ientry is the entry number in the current Tree
    //  Note that the argument to GetEntry must be:
    //    jentry for TChain::GetEntry
    //    ientry for TTree::GetEntry and TBranch::GetEntry
    //
    //       To read only selected branches, Insert statements like:
    // METHOD1:
    //    fChain->SetBranchStatus("*",0);  // disable all branches
    //    fChain->SetBranchStatus("branchname",1);  // activate branchname
    // METHOD2: replace line
    //    fChain->GetEntry(jentry);       //read all branches
    // by  b_branchname->GetEntry(ientry); //read only this branch
    if (fChain == 0) {
        std::cout << " Null pointer to the chain; stopping here ..." << std::endl;
        return;
    }

    bool m_verbose = false;
    bool m_debug = false;

    Double_t nTrackThr = 25.;

    TH1F* h_hitVsZ[6];
    h_hitVsZ[0] = new TH1F("hitVsZlay1", "hitVsZlay1", 1000, -13000., 13000.);
    h_hitVsZ[1] = new TH1F("hitVsZlay2", "hitVsZlay2", 1000, -13000., 13000.);
    h_hitVsZ[2] = new TH1F("hitVsZlay3", "hitVsZlay3", 1000, -13000., 13000.);
    h_hitVsZ[3] = new TH1F("hitVsZlay4", "hitVsZlay4", 1000, -13000., 13000.);
    h_hitVsZ[4] = new TH1F("hitVsZlay5", "hitVsZlay5", 1000, -13000., 13000.);
    h_hitVsZ[5] = new TH1F("hitVsZlay6", "hitVsZlay6", 1000, -13000., 13000.);
    TH1F* h_hitVsP[6];
    h_hitVsP[0] = new TH1F("hitVsPlay1", "hitVsPlay1", 1000, -3.15, 3.15);
    h_hitVsP[1] = new TH1F("hitVsPlay2", "hitVsPlay2", 1000, -3.15, 3.15);
    h_hitVsP[2] = new TH1F("hitVsPlay3", "hitVsPlay3", 1000, -3.15, 3.15);
    h_hitVsP[3] = new TH1F("hitVsPlay4", "hitVsPlay4", 1000, -3.15, 3.15);
    h_hitVsP[4] = new TH1F("hitVsPlay5", "hitVsPlay5", 1000, -3.15, 3.15);
    h_hitVsP[5] = new TH1F("hitVsPlay6", "hitVsPlay6", 1000, -3.15, 3.15);
    TH1F* h_digVsZ[6];
    h_digVsZ[0] = new TH1F("digitVsZlay1", "digitVsZlay1", 1000, -13000., 13000.);
    h_digVsZ[1] = new TH1F("digitVsZlay2", "digitVsZlay2", 1000, -13000., 13000.);
    h_digVsZ[2] = new TH1F("digitVsZlay3", "digitVsZlay3", 1000, -13000., 13000.);
    h_digVsZ[3] = new TH1F("digitVsZlay4", "digitVsZlay4", 1000, -13000., 13000.);
    h_digVsZ[4] = new TH1F("digitVsZlay5", "digitVsZlay5", 1000, -13000., 13000.);
    h_digVsZ[5] = new TH1F("digitVsZlay6", "digitVsZlay6", 1000, -13000., 13000.);
    TH1F* h_digVsP[6];
    h_digVsP[0] = new TH1F("digitVsPlay1", "digitVsPlay1", 1000, -3.15, 3.15);
    h_digVsP[1] = new TH1F("digitVsPlay2", "digitVsPlay2", 1000, -3.15, 3.15);
    h_digVsP[2] = new TH1F("digitVsPlay3", "digitVsPlay3", 1000, -3.15, 3.15);
    h_digVsP[3] = new TH1F("digitVsPlay4", "digitVsPlay4", 1000, -3.15, 3.15);
    h_digVsP[4] = new TH1F("digitVsPlay5", "digitVsPlay5", 1000, -3.15, 3.15);
    h_digVsP[5] = new TH1F("digitVsPlay6", "digitVsPlay6", 1000, -3.15, 3.15);
    TH1F* h_digeffVsZ[6];
    h_digeffVsZ[0] = new TH1F("digeffVsZlay1", "digeffVsZlay1", 1000, -13000., 13000.);
    h_digeffVsZ[1] = new TH1F("digeffVsZlay2", "digeffVsZlay2", 1000, -13000., 13000.);
    h_digeffVsZ[2] = new TH1F("digeffVsZlay3", "digeffVsZlay3", 1000, -13000., 13000.);
    h_digeffVsZ[3] = new TH1F("digeffVsZlay4", "digeffVsZlay4", 1000, -13000., 13000.);
    h_digeffVsZ[4] = new TH1F("digeffVsZlay5", "digeffVsZlay5", 1000, -13000., 13000.);
    h_digeffVsZ[5] = new TH1F("digeffVsZlay6", "digeffVsZlay6", 1000, -13000., 13000.);
    TH1F* h_digeffVsP[6];
    h_digeffVsP[0] = new TH1F("digeffVsPlay1", "digeffVsPlay1", 1000, -3.15, 3.15);
    h_digeffVsP[1] = new TH1F("digeffVsPlay2", "digeffVsPlay2", 1000, -3.15, 3.15);
    h_digeffVsP[2] = new TH1F("digeffVsPlay3", "digeffVsPlay3", 1000, -3.15, 3.15);
    h_digeffVsP[3] = new TH1F("digeffVsPlay4", "digeffVsPlay4", 1000, -3.15, 3.15);
    h_digeffVsP[4] = new TH1F("digeffVsPlay5", "digeffVsPlay5", 1000, -3.15, 3.15);
    h_digeffVsP[5] = new TH1F("digeffVsPlay6", "digeffVsPlay6", 1000, -3.15, 3.15);

    TH2F* h_digPhiResA = new TH2F("digitPhiResidualsA", "digitPhiResidualsA", 6, 0.5, 6.5, 101, -150.5, 150.5);
    TH2F* h_digPhiResC = new TH2F("digitPhiResidualsC", "digitPhiResidualsC", 6, 0.5, 6.5, 101, -150.5, 150.5);
    TH2F* h_dighitPhiResA = new TH2F("digithitPhiResidualsA", "digithitPhiResidualsA", 6, 0.5, 6.5, 101, -150.5, 150.5);
    TH2F* h_dighitPhiResC = new TH2F("digithitPhiResidualsC", "digithitPhiResidualsC", 6, 0.5, 6.5, 101, -150.5, 150.5);

    TH2F* h_digEtaZResA = new TH2F("digitEtaZResidualsA", "digitEtaZResidualsA", 6, 0.5, 6.5, 101, -150.5, 150.5);
    TH2F* h_digEtaZResC = new TH2F("digitEtaZResidualsC", "digitEtaZResidualsC", 6, 0.5, 6.5, 101, -150.5, 150.5);

    TH2F* h_digZResVsZ[6];
    h_digZResVsZ[0] = new TH2F("digitZResVsZlay1", "digitZResVsZlay1", 1000, -13000., 13000., 101, -150.5, 150.5);
    h_digZResVsZ[1] = new TH2F("digitZResVsZlay2", "digitZResVsZlay2", 1000, -13000., 13000., 101, -150.5, 150.5);
    h_digZResVsZ[2] = new TH2F("digitZResVsZlay3", "digitZResVsZlay3", 1000, -13000., 13000., 101, -150.5, 150.5);
    h_digZResVsZ[3] = new TH2F("digitZResVsZlay4", "digitZResVsZlay4", 1000, -13000., 13000., 101, -150.5, 150.5);
    h_digZResVsZ[4] = new TH2F("digitZResVsZlay5", "digitZResVsZlay5", 1000, -13000., 13000., 101, -150.5, 150.5);
    h_digZResVsZ[5] = new TH2F("digitZResVsZlay6", "digitZResVsZlay6", 1000, -13000., 13000., 101, -150.5, 150.5);
    TH2F* h_digRZResVsZ[6];
    h_digRZResVsZ[0] = new TH2F("digitRZResVsZlay1", "digitZResVsZlay1", 1000, -13000., 13000., 101, -150.5, 150.5);
    h_digRZResVsZ[1] = new TH2F("digitRZResVsZlay2", "digitZResVsZlay2", 1000, -13000., 13000., 101, -150.5, 150.5);
    h_digRZResVsZ[2] = new TH2F("digitRZResVsZlay3", "digitZResVsZlay3", 1000, -13000., 13000., 101, -150.5, 150.5);
    h_digRZResVsZ[3] = new TH2F("digitRZResVsZlay4", "digitZResVsZlay4", 1000, -13000., 13000., 101, -150.5, 150.5);
    h_digRZResVsZ[4] = new TH2F("digitRZResVsZlay5", "digitZResVsZlay5", 1000, -13000., 13000., 101, -150.5, 150.5);
    h_digRZResVsZ[5] = new TH2F("digitRZResVsZlay6", "digitZResVsZlay6", 1000, -13000., 13000., 101, -150.5, 150.5);

    TH2F* h_dighitRZResVsZ[6];
    h_dighitRZResVsZ[0] = new TH2F("digithitRZResVsZlay1", "digithitRZResVsZlay1", 1000, -13000., 13000., 101, -150.5, 150.5);
    h_dighitRZResVsZ[1] = new TH2F("digithitRZResVsZlay2", "digithitRZResVsZlay2", 1000, -13000., 13000., 101, -150.5, 150.5);
    h_dighitRZResVsZ[2] = new TH2F("digithitRZResVsZlay3", "digithitRZResVsZlay3", 1000, -13000., 13000., 101, -150.5, 150.5);
    h_dighitRZResVsZ[3] = new TH2F("digithitRZResVsZlay4", "digithitRZResVsZlay4", 1000, -13000., 13000., 101, -150.5, 150.5);
    h_dighitRZResVsZ[4] = new TH2F("digithitRZResVsZlay5", "digithitRZResVsZlay5", 1000, -13000., 13000., 101, -150.5, 150.5);
    h_dighitRZResVsZ[5] = new TH2F("digithitRZResVsZlay6", "digithitRZResVsZlay6", 1000, -13000., 13000., 101, -150.5, 150.5);
    TH2F* h_dighitZResVsZ[6];
    h_dighitZResVsZ[0] = new TH2F("digithitZResVsZlay1", "digithitZResVsZlay1", 1000, -13000., 13000., 101, -150.5, 150.5);
    h_dighitZResVsZ[1] = new TH2F("digithitZResVsZlay2", "digithitZResVsZlay2", 1000, -13000., 13000., 101, -150.5, 150.5);
    h_dighitZResVsZ[2] = new TH2F("digithitZResVsZlay3", "digithitZResVsZlay3", 1000, -13000., 13000., 101, -150.5, 150.5);
    h_dighitZResVsZ[3] = new TH2F("digithitZResVsZlay4", "digithitZResVsZlay4", 1000, -13000., 13000., 101, -150.5, 150.5);
    h_dighitZResVsZ[4] = new TH2F("digithitZResVsZlay5", "digithitZResVsZlay5", 1000, -13000., 13000., 101, -150.5, 150.5);
    h_dighitZResVsZ[5] = new TH2F("digithitZResVsZlay6", "digithitZResVsZlay6", 1000, -13000., 13000., 101, -150.5, 150.5);

    /*
    TH2F* h_dighitZResVsZsmall[6];
    h_dighitZResVsZsmall[0] = new TH2F("digithitZResVsZsmalllay1","digithitZResVsZsmalllay1",1000,-13000.,13000.,101,-150.5,150.5);
    h_dighitZResVsZsmall[1] = new TH2F("digithitZResVsZsmalllay2","digithitZResVsZsmalllay2",1000,-13000.,13000.,101,-150.5,150.5);
    h_dighitZResVsZsmall[2] = new TH2F("digithitZResVsZsmalllay3","digithitZResVsZsmalllay3",1000,-13000.,13000.,101,-150.5,150.5);
    h_dighitZResVsZsmall[3] = new TH2F("digithitZResVsZsmalllay4","digithitZResVsZsmalllay4",1000,-13000.,13000.,101,-150.5,150.5);
    h_dighitZResVsZsmall[4] = new TH2F("digithitZResVsZsmalllay5","digithitZResVsZsmalllay5",1000,-13000.,13000.,101,-150.5,150.5);
    h_dighitZResVsZsmall[5] = new TH2F("digithitZResVsZsmalllay6","digithitZResVsZsmalllay6",1000,-13000.,13000.,101,-150.5,150.5);
    TH2F* h_dighitZResVsZlarge[6];
    h_dighitZResVsZlarge[0] = new TH2F("digithitZResVsZlargelay1","digithitZResVsZlargelay1",1000,-13000.,13000.,101,-150.5,150.5);
    h_dighitZResVsZlarge[1] = new TH2F("digithitZResVsZlargelay2","digithitZResVsZlargelay2",1000,-13000.,13000.,101,-150.5,150.5);
    h_dighitZResVsZlarge[2] = new TH2F("digithitZResVsZlargelay3","digithitZResVsZlargelay3",1000,-13000.,13000.,101,-150.5,150.5);
    h_dighitZResVsZlarge[3] = new TH2F("digithitZResVsZlargelay4","digithitZResVsZlargelay4",1000,-13000.,13000.,101,-150.5,150.5);
    h_dighitZResVsZlarge[4] = new TH2F("digithitZResVsZlargelay5","digithitZResVsZlargelay5",1000,-13000.,13000.,101,-150.5,150.5);
    h_dighitZResVsZlarge[5] = new TH2F("digithitZResVsZlargelay6","digithitZResVsZlargelay6",1000,-13000.,13000.,101,-150.5,150.5);

    TH2F* h_dighitZResVsZdbZ1[6];
    h_dighitZResVsZdbZ1[0] = new TH2F("digithitZResVsZdbZ1lay1","digithitZResVsZdbZ1lay1",1000,-13000.,13000.,101,-150.5,150.5);
    h_dighitZResVsZdbZ1[1] = new TH2F("digithitZResVsZdbZ1lay2","digithitZResVsZdbZ1lay2",1000,-13000.,13000.,101,-150.5,150.5);
    h_dighitZResVsZdbZ1[2] = new TH2F("digithitZResVsZdbZ1lay3","digithitZResVsZdbZ1lay3",1000,-13000.,13000.,101,-150.5,150.5);
    h_dighitZResVsZdbZ1[3] = new TH2F("digithitZResVsZdbZ1lay4","digithitZResVsZdbZ1lay4",1000,-13000.,13000.,101,-150.5,150.5);
    h_dighitZResVsZdbZ1[4] = new TH2F("digithitZResVsZdbZ1lay5","digithitZResVsZdbZ1lay5",1000,-13000.,13000.,101,-150.5,150.5);
    h_dighitZResVsZdbZ1[5] = new TH2F("digithitZResVsZdbZ1lay6","digithitZResVsZdbZ1lay6",1000,-13000.,13000.,101,-150.5,150.5);
    TH2F* h_dighitZResVsZdbZ2[6];
    h_dighitZResVsZdbZ2[0] = new TH2F("digithitZResVsZdbZ2lay1","digithitZResVsZdbZ2lay1",1000,-13000.,13000.,101,-150.5,150.5);
    h_dighitZResVsZdbZ2[1] = new TH2F("digithitZResVsZdbZ2lay2","digithitZResVsZdbZ2lay2",1000,-13000.,13000.,101,-150.5,150.5);
    h_dighitZResVsZdbZ2[2] = new TH2F("digithitZResVsZdbZ2lay3","digithitZResVsZdbZ2lay3",1000,-13000.,13000.,101,-150.5,150.5);
    h_dighitZResVsZdbZ2[3] = new TH2F("digithitZResVsZdbZ2lay4","digithitZResVsZdbZ2lay4",1000,-13000.,13000.,101,-150.5,150.5);
    h_dighitZResVsZdbZ2[4] = new TH2F("digithitZResVsZdbZ2lay5","digithitZResVsZdbZ2lay5",1000,-13000.,13000.,101,-150.5,150.5);
    h_dighitZResVsZdbZ2[5] = new TH2F("digithitZResVsZdbZ2lay6","digithitZResVsZdbZ2lay6",1000,-13000.,13000.,101,-150.5,150.5);
    */

    Int_t netabins = 28;
    Int_t nphibins = 32;
    TH2F* h_effmapHits[6];
    Double_t zMinOuter = -13000.;
    Double_t zMaxOuter = 13000.;
    Double_t zMinMiddle = -10000.;
    Double_t zMaxMiddle = 10000.;
    Double_t phiMin = -M_PI;
    Double_t phiMax = M_PI;
    h_effmapHits[0] = new TH2F("effmapHitslay1", "effmapHitslay1", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapHits[1] = new TH2F("effmapHitslay2", "effmapHitslay2", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapHits[2] = new TH2F("effmapHitslay3", "effmapHitslay3", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapHits[3] = new TH2F("effmapHitslay4", "effmapHitslay4", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapHits[4] = new TH2F("effmapHitslay5", "effmapHitslay5", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    h_effmapHits[5] = new TH2F("effmapHitslay6", "effmapHitslay6", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    TH2F* h_effmapPhiAndEtaDig[6];
    h_effmapPhiAndEtaDig[0] =
        new TH2F("effmapPhiAndEtaDiglay1", "effmapPhiAndEtaDiglay1", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhiAndEtaDig[1] =
        new TH2F("effmapPhiAndEtaDiglay2", "effmapPhiAndEtaDiglay2", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhiAndEtaDig[2] =
        new TH2F("effmapPhiAndEtaDiglay3", "effmapPhiAndEtaDiglay3", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhiAndEtaDig[3] =
        new TH2F("effmapPhiAndEtaDiglay4", "effmapPhiAndEtaDiglay4", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhiAndEtaDig[4] =
        new TH2F("effmapPhiAndEtaDiglay5", "effmapPhiAndEtaDiglay5", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    h_effmapPhiAndEtaDig[5] =
        new TH2F("effmapPhiAndEtaDiglay6", "effmapPhiAndEtaDiglay6", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    TH2F* h_effmapPhiOnlyDig[6];
    h_effmapPhiOnlyDig[0] =
        new TH2F("effmapPhiOnlyDiglay1", "effmapPhiOnlyDiglay1", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhiOnlyDig[1] =
        new TH2F("effmapPhiOnlyDiglay2", "effmapPhiOnlyDiglay2", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhiOnlyDig[2] =
        new TH2F("effmapPhiOnlyDiglay3", "effmapPhiOnlyDiglay3", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhiOnlyDig[3] =
        new TH2F("effmapPhiOnlyDiglay4", "effmapPhiOnlyDiglay4", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhiOnlyDig[4] =
        new TH2F("effmapPhiOnlyDiglay5", "effmapPhiOnlyDiglay5", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    h_effmapPhiOnlyDig[5] =
        new TH2F("effmapPhiOnlyDiglay6", "effmapPhiOnlyDiglay6", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    TH2F* h_effmapEtaOnlyDig[6];
    h_effmapEtaOnlyDig[0] =
        new TH2F("effmapEtaOnlyDiglay1", "effmapEtaOnlyDiglay1", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapEtaOnlyDig[1] =
        new TH2F("effmapEtaOnlyDiglay2", "effmapEtaOnlyDiglay2", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapEtaOnlyDig[2] =
        new TH2F("effmapEtaOnlyDiglay3", "effmapEtaOnlyDiglay3", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapEtaOnlyDig[3] =
        new TH2F("effmapEtaOnlyDiglay4", "effmapEtaOnlyDiglay4", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapEtaOnlyDig[4] =
        new TH2F("effmapEtaOnlyDiglay5", "effmapEtaOnlyDiglay5", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    h_effmapEtaOnlyDig[5] =
        new TH2F("effmapEtaOnlyDiglay6", "effmapEtaOnlyDiglay6", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    TH2F* h_effmapEta[6];
    h_effmapEta[0] = new TH2F("effmapEtalay1", "effmapEtalay1", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapEta[1] = new TH2F("effmapEtalay2", "effmapEtalay2", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapEta[2] = new TH2F("effmapEtalay3", "effmapEtalay3", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapEta[3] = new TH2F("effmapEtalay4", "effmapEtalay4", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapEta[4] = new TH2F("effmapEtalay5", "effmapEtalay5", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    h_effmapEta[5] = new TH2F("effmapEtalay6", "effmapEtalay6", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    TH2F* h_effmapPhi[6];
    h_effmapPhi[0] = new TH2F("effmapPhilay1", "effmapPhilay1", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhi[1] = new TH2F("effmapPhilay2", "effmapPhilay2", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhi[2] = new TH2F("effmapPhilay3", "effmapPhilay3", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhi[3] = new TH2F("effmapPhilay4", "effmapPhilay4", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapPhi[4] = new TH2F("effmapPhilay5", "effmapPhilay5", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    h_effmapPhi[5] = new TH2F("effmapPhilay6", "effmapPhilay6", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    TH2F* h_effmapGap[6];
    h_effmapGap[0] = new TH2F("effmapGaplay1", "effmapGaplay1", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapGap[1] = new TH2F("effmapGaplay2", "effmapGaplay2", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapGap[2] = new TH2F("effmapGaplay3", "effmapGaplay3", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapGap[3] = new TH2F("effmapGaplay4", "effmapGaplay4", netabins, zMinMiddle, zMaxMiddle, nphibins, phiMin, phiMax);
    h_effmapGap[4] = new TH2F("effmapGaplay5", "effmapGaplay5", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);
    h_effmapGap[5] = new TH2F("effmapGaplay6", "effmapGaplay6", netabins, zMinOuter, zMaxOuter, nphibins, phiMin, phiMax);

    Long64_t nentries = fChain->GetEntries();
    Long64_t nentriesFast = fChain->GetEntriesFast();
    std::cout << " nentries, nentriesFast = " << nentries << " " << nentriesFast << std::endl;

    Int_t layerTypeH[100];
    Int_t layerNH[100];
    Int_t layerType[100];
    Int_t layerN[100];
    Int_t rpc_NsimhitAll = 0;
    Int_t nUnmatchedHits = 0;
    Int_t nMatchedHits = 0;
    Int_t nMatchedEtaHits = 0;
    Int_t nMatchedPhiHits = 0;
    Int_t nMatchedPhiOnlyHits = 0;
    Int_t nMatchedEtaOnlyHits = 0;
    Int_t nMatchedPhiAndEtaHits = 0;

    Int_t nEventsinRPCsistem = 0;
    Int_t nEventsinLowPtacceptance = 0;
    Int_t nEventsinHighPtacceptance = 0;
    Int_t nlowptTrig = 0;
    Int_t nhighptTrig = 0;

    Int_t eventNum = 0;
    Long64_t nbytes = 0, nb = 0;
    for (Long64_t jentry = 0; jentry < nentries; jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);
        nbytes += nb;

        // if (Cut(ientry) < 0) continue;
        ++eventNum;
        if (eventNum > maxEvent) break;
        if (Event == 35525 || Event == 39417 || Event == 64567 || Event == 78567 || Event == 86005 || Event == 98253 || Event == 99890)
            continue;
        std::cout << " ------------------------eventNum = " << eventNum << " Run/Event = " << Run << "/" << Event
                  << " ...... # particles = " << NpartStored << " nhit = " << rpc_Nsimhit << " nDig = " << rpc_Ndigit
                  << " nPRD = " << rpc_Nprd << std::endl;
        if (rpc_Nsimhit < 3) {
            std::cout << "less than 3 simHits" << std::endl;
            continue;
        }
        if (rpc_Nsimhit > 100) {
            std::cout << "more than 100 simHits" << std::endl;
            continue;
        }

        std::string side;
        if (partMomZ[0] > 0)
            side = "A";
        else
            side = "C";

        // for each event, clen the matched-hit array
        Int_t simhitMatched[10000];
        for (Int_t is = 0; is < 10000; ++is) simhitMatched[is] = 0;

        // here's my loop over events
        for (Int_t ih = 0; ih < 100; ++ih) {
            layerTypeH[ih] = 0;
            layerNH[ih] = 0;
        }
        for (Int_t ih = 0; ih < rpc_Nsimhit; ++ih) {
            if (rpc_simhit_station[ih] < 4 || rpc_simhit_station[ih] == 8) {
                // middle station
                if (rpc_simhit_doublr[ih] == 1) {
                    layerTypeH[ih] = 1;
                } else {
                    layerTypeH[ih] = 2;
                }
            } else
                layerTypeH[ih] = 3;
            layerNH[ih] = (layerTypeH[ih] - 1) * 2 + rpc_simhit_gasgap[ih];
            TVector3 simH(rpc_simhit_globalx[ih], rpc_simhit_globaly[ih], rpc_simhit_globalz[ih]);

            // hit z and phi profiles per layer
            if (m_debug)
                std::cout << " hit at index " << ih << " is eta view; layerNH " << layerNH[ih] << "  - name/eta/phi/dbR/gg "
                          << rpc_simhit_station[ih] << "/" << rpc_simhit_eta[ih] << "/" << rpc_simhit_phi[ih] << "/"
                          << rpc_simhit_doublr[ih] << "/" << rpc_simhit_gasgap[ih] << " measphi = " << rpc_simhit_measphi[ih] << std::endl;

            if (rpc_simhit_measphi[ih] == 0) {
                h_hitVsZ[layerNH[ih] - 1]->Fill(rpc_simhit_globalz[ih]);
                // if (m_debug) std::cout<<" hit at index "<<ih<<" is eta view; layerNH "<< layerNH[ih]<<"  - name/eta/phi/dbR/gg "
                //			  <<rpc_simhit_station[ih]<<"/"<<rpc_simhit_eta[ih]<<"/"<<rpc_simhit_phi[ih]<<"/"
                //			  <<rpc_simhit_doublr[ih]<<"/"
                //			  <<rpc_simhit_gasgap[ih]<<std::endl;
            } else if (rpc_simhit_measphi[ih] == 1) {
                simhitMatched[ih] = -1;
                h_hitVsP[layerNH[ih] - 1]->Fill(simH.Phi());
            }

            Double_t distXY = 0.;
            Double_t distRZ = 0.;
            Double_t distZ = 0.;
            if (NpartStored == 1) {
                computeDistFromPart(simH, distXY, distRZ, distZ);
                if (fabs(distXY) > 0.0005 || fabs(distZ) > 0.0005) {
                    std::cout << " LARGE dist of SIMHIT from particle track: xy " << distXY << " rz " << distRZ << " Z " << distZ
                              << std::endl;
                    std::cout << " ----- simhit at layerTypeH " << layerTypeH[ih] << " layerNH " << layerNH[ih] << " - name/dbR/gg "
                              << rpc_simhit_station[ih] << "/" << rpc_simhit_doublr[ih] << "/" << rpc_simhit_gasgap[ih] << std::endl;
                }
            }
        }
        for (Int_t ih = 0; ih < 100; ++ih) {
            layerType[ih] = 0;
            layerN[ih] = 0;
        }
        for (Int_t ih = 0; ih < rpc_Ndigit; ++ih) {
            // std::cout<<" start with a new  digit ih = "<<ih<< std::endl;
            if (rpc_digit_station[ih] < 4 || rpc_digit_station[ih] == 8) {
                // middle station
                if (rpc_digit_doublr[ih] == 1) {
                    layerType[ih] = 1;
                } else {
                    layerType[ih] = 2;
                }
            } else
                layerType[ih] = 3;
            layerN[ih] = (layerType[ih] - 1) * 2 + rpc_digit_gasgap[ih];
            if (m_verbose || m_debug)
                std::cout << "digit at layerType " << layerType[ih] << " layerN " << layerN[ih] << " - name/eta/phi/dbR/gg/measPhi "
                          << rpc_digit_station[ih] << "/" << rpc_digit_eta[ih] << "/" << rpc_digit_phi[ih] << "/" << rpc_digit_doublr[ih]
                          << "/" << rpc_digit_gasgap[ih] << "/" << rpc_digit_measphi[ih] << " strip = " << rpc_digit_strip[ih] << std::endl;
            TVector3 digH(rpc_digit_stripx[ih], rpc_digit_stripy[ih], rpc_digit_stripz[ih]);
            Double_t distXY = 0.;
            Double_t distRZ = 0.;
            Double_t distZ = 0.;
            Double_t disthXY = 0.;
            Double_t disthRZ = 0.;
            Double_t disthZ = 0.;
            bool LargeSect = false;
            if (rpc_digit_station[ih] == 2 || rpc_digit_station[ih] == 4) LargeSect = true;
            if (NpartStored == 1) {
                computeDistFromPart(digH, distXY, distRZ, distZ);
                if (rpc_digit_measphi[ih] == 1) {
                    if (m_verbose)
                        std::cout << " PHI DIGIT distance from particle track: xy " << distXY << " detector side" << side << std::endl;
                    if (side == "A")
                        h_digPhiResA->Fill((float)layerN[ih], distXY);
                    else
                        h_digPhiResC->Fill((float)layerN[ih], distXY);
                } else {
                    if (m_verbose)
                        std::cout << " ETA DIGIT distance from particle track: rz " << distRZ << " Z " << distZ << " detector side" << side
                                  << std::endl;
                    h_digZResVsZ[layerN[ih] - 1]->Fill(rpc_digit_stripz[ih], distZ);
                    h_digRZResVsZ[layerN[ih] - 1]->Fill(rpc_digit_stripz[ih], distRZ);

                    if (side == "A")
                        h_digEtaZResA->Fill((float)layerN[ih], distZ);
                    else
                        h_digEtaZResC->Fill((float)layerN[ih], distZ);
                }
            }

            // digit z and phi profiles per layer
            if (rpc_digit_measphi[ih] == 0) {
                h_digVsZ[layerN[ih] - 1]->Fill(rpc_digit_stripz[ih]);
                // if (m_debug) std::cout<<" filling dig profile ... "<<std::endl;
            } else if (rpc_digit_measphi[ih] == 1)
                h_digVsP[layerN[ih] - 1]->Fill(digH.Phi());

            bool hitFound = false;
            Int_t nmatchhit = 0;
            bool digeff = false;
            for (Int_t is = 0; is < rpc_Nsimhit; ++is) {
                if (rpc_simhit_station[is] != rpc_digit_station[ih]) continue;
                if (rpc_simhit_eta[is] != rpc_digit_eta[ih]) continue;
                if (rpc_simhit_phi[is] != rpc_digit_phi[ih]) continue;
                if (rpc_simhit_doublr[is] != rpc_digit_doublr[ih]) continue;
                if (rpc_simhit_doublz[is] != rpc_digit_doublz[ih]) continue;
                if (rpc_simhit_gasgap[is] != rpc_digit_gasgap[ih]) continue;
                // if (rpc_simhit_measphi[is]!=rpc_digit_measphi[ih] ) continue;
                if (rpc_simhit_measphi[is] != 0) { continue; }
                // if (m_debug) std::cout<<" is = "<<is<<" simhitMatched[is]="<<simhitMatched[is]<<" hitFound = "<<hitFound<<std::endl;
                TVector3 vh(rpc_simhit_globalx[is], rpc_simhit_globaly[is], rpc_simhit_globalz[is]);
                if (!hitFound) {
                    computeDistFromHit(digH, vh, disthXY, disthRZ, disthZ);
                    // if (m_debug) std::cout<<" dist to hit z/XY = "<<disthZ<<"/"<<disthXY<<std::endl;

                    if (rpc_digit_measphi[ih] == 0 && fabs(disthZ) < 30. && (simhitMatched[is] % 10) != 1) {
                        h_digeffVsZ[layerN[ih] - 1]->Fill(rpc_simhit_globalz[is]);
                        simhitMatched[is] = simhitMatched[is] + 1;
                        digeff = true;
                        hitFound = true;
                        // if (m_debug) std::cout<<" filling digeff eta profile ... simhitMatched["<<is<<"] reset to
                        // "<<simhitMatched[is]<<std::endl;
                    } else if (rpc_digit_measphi[ih] == 1 && fabs(disthXY) < 30. && int(simhitMatched[is] / 10) != 1) {
                        h_digeffVsP[layerN[ih] - 1]->Fill(vh.Phi());
                        simhitMatched[is] = simhitMatched[is] + 10;
                        digeff = true;
                        hitFound = true;
                        // if (m_debug) std::cout<<" filling digeff phi profile ... ... simhitMatched["<<is<<"] reset to
                        // "<<simhitMatched[is]<<std::endl;
                    }
                    // hitFound = true;
                } else {
                    nmatchhit++;
                    Double_t disthmoreXY = 0.;
                    Double_t disthmoreRZ = 0.;
                    Double_t disthmoreZ = 0.;
                    computeDistFromHit(digH, vh, disthmoreXY, disthmoreRZ, disthmoreZ);
                    std::cout << "More than 1 hit matching this digit - side" << side << std::endl;
                    std::cout << "... IS this closer in z to the digit ???? new : " << disthmoreZ << " <? previous " << disthZ << std::endl;
                }
            }
            if (!digeff && m_debug) std::cout << " DO NOT fill digeff profile ... " << std::endl;
            if (nmatchhit > 1) std::cout << nmatchhit << "More than 2 hit matching this digit - side" << side << std::endl;
            if (rpc_digit_measphi[ih] == 1) {
                if (m_verbose)
                    std::cout << " PHI DIGIT distance from particle track: xy " << disthXY << " detector side" << side << std::endl;
                if (side == "A")
                    h_dighitPhiResA->Fill((float)layerN[ih], disthXY);
                else
                    h_dighitPhiResC->Fill((float)layerN[ih], disthXY);
            } else {
                if (m_verbose)
                    std::cout << " ETA DIGIT distance from hit: rz " << disthRZ << " Z " << disthZ << " detector side" << side << std::endl;

                h_dighitRZResVsZ[layerN[ih] - 1]->Fill(rpc_digit_stripz[ih], disthRZ);

                h_dighitZResVsZ[layerN[ih] - 1]->Fill(rpc_digit_stripz[ih], disthZ);

                // if (LargeSect) h_dighitZResVsZlarge[layerN[ih]-1]->Fill(rpc_digit_stripz[ih],disthZ);
                // else           h_dighitZResVsZsmall[layerN[ih]-1]->Fill(rpc_digit_stripz[ih],disthZ);

                // if      (rpc_digit_doublz[ih]==1) h_dighitZResVsZdbZ1[layerN[ih]-1]->Fill(rpc_digit_stripz[ih],disthZ);
                // else if (rpc_digit_doublz[ih]==2) h_dighitZResVsZdbZ2[layerN[ih]-1]->Fill(rpc_digit_stripz[ih],disthZ);
            }
        }

        // end of event calculations
        Int_t nMatchedHitsEv = 0;
        Int_t nMatchedEtaHitsEv = 0;
        Int_t nMatchedPhiOnlyHitsEv = 0;
        Int_t nMatchedEtaOnlyHitsEv = 0;
        Int_t nMatchedPhiHitsEv = 0;
        Int_t nUnmatchedHitsEv = 0;
        Int_t nMatchedPhiAndEtaHitsEv = 0;
        Int_t nlowptphi = 0;
        Int_t nlowpteta = 0;
        Int_t nhighptphi = 0;
        Int_t nhighpteta = 0;
        Int_t nSimHitsInLowPtAcc = 0;
        Int_t nSimHitsInHighPtAcc = 0;
        ++nEventsinRPCsistem;
        for (Int_t is = 0; is < rpc_Nsimhit; ++is) {
            if (simhitMatched[is] < 0) continue;  // hit duplicate for phi view
            ++rpc_NsimhitAll;

            if (layerNH[is] < 5)
                ++nSimHitsInLowPtAcc;
            else
                ++nSimHitsInHighPtAcc;
            TVector3 simH(rpc_simhit_globalx[is], rpc_simhit_globaly[is], rpc_simhit_globalz[is]);
            Double_t phiH = simH.Phi();
            if (phiH > M_PI) phiH = phiH - 2. * M_PI;
            h_effmapHits[layerNH[is] - 1]->Fill(rpc_simhit_globalz[is], phiH);

            if (simhitMatched[is] < 1) {
                ++nUnmatchedHitsEv;
                ++nUnmatchedHits;
                std::cout << "hit at index " << is << " isUnMatched  " << simhitMatched[is] << ";  so far " << nMatchedHitsEv
                          << " matched and " << nUnmatchedHitsEv << " unmatched hits";
                std::cout << " layerNH " << layerNH[is] << " statName/eta/phi/dbR/dbZ/dbP/gg/measphi " << rpc_simhit_station[is] << "/"
                          << rpc_simhit_eta[is] << "/" << rpc_simhit_phi[is] << "/" << rpc_simhit_doublr[is] << "/" << rpc_simhit_doublz[is]
                          << "/" << rpc_simhit_doublphi[is] << "/" << rpc_simhit_gasgap[is] << "/" << rpc_simhit_measphi[is] << std::endl;
                continue;
            }
            ++nMatchedHits;
            ++nMatchedHitsEv;
            // eta
            if (m_debug) {
                std::cout << "hit at index " << is << " isMatched  " << simhitMatched[is] << ";  so far " << nMatchedHitsEv
                          << " matched and " << nUnmatchedHitsEv << " unmatched hits";
                std::cout << " layerNH " << layerNH[is] << " statName/eta/phi/dbR/dbZ/dbP/gg/measphi " << rpc_simhit_station[is] << "/"
                          << rpc_simhit_eta[is] << "/" << rpc_simhit_phi[is] << "/" << rpc_simhit_doublr[is] << "/" << rpc_simhit_doublz[is]
                          << "/" << rpc_simhit_doublphi[is] << "/" << rpc_simhit_gasgap[is] << "/" << rpc_simhit_measphi[is] << std::endl;
            }

            if (simhitMatched[is] == 11) {
                ++nMatchedPhiAndEtaHits;
                ++nMatchedPhiHits;
                ++nMatchedEtaHits;
                ++nMatchedPhiAndEtaHitsEv;
                ++nMatchedPhiHitsEv;
                ++nMatchedEtaHitsEv;
                h_effmapPhiAndEtaDig[layerNH[is] - 1]->Fill(rpc_simhit_globalz[is], phiH);
                if (layerNH[is] < 5) {
                    ++nlowptphi;
                    ++nlowpteta;
                } else {
                    ++nhighptphi;
                    ++nhighpteta;
                }
            } else if (simhitMatched[is] == 10) {
                ++nMatchedPhiHits;
                ++nMatchedPhiHitsEv;
                ++nMatchedPhiOnlyHits;
                ++nMatchedPhiOnlyHitsEv;
                h_effmapPhiOnlyDig[layerNH[is] - 1]->Fill(rpc_simhit_globalz[is], phiH);
                if (layerNH[is] < 5) {
                    ++nlowptphi;
                } else {
                    ++nhighptphi;
                }
            } else if (simhitMatched[is] == 1) {
                ++nMatchedEtaHits;
                ++nMatchedEtaHitsEv;
                ++nMatchedEtaOnlyHits;
                ++nMatchedEtaOnlyHitsEv;
                h_effmapEtaOnlyDig[layerNH[is] - 1]->Fill(rpc_simhit_globalz[is], phiH);
                if (layerNH[is] < 5) {
                    ++nlowpteta;
                } else {
                    ++nhighpteta;
                }
            } else {
                std::cout << " a problem here " << std::endl;
            }
        }
        std::cout << " nSimHits/nUnmatched/nMatched/nMatchedPhi/nMatchedEta/nMatchedPhiAndEta " << double(rpc_Nsimhit / 2.) << "/"
                  << nUnmatchedHitsEv << "/" << nMatchedHitsEv << "/" << nMatchedPhiHitsEv << "/" << nMatchedEtaHitsEv << "/"
                  << nMatchedPhiAndEtaHitsEv << std::endl;
        if (nSimHitsInLowPtAcc > 2) {
            ++nEventsinLowPtacceptance;
            if (nSimHitsInHighPtAcc > 0) ++nEventsinHighPtacceptance;
        }

        if (nlowpteta > 2 && nlowptphi > 2) {
            ++nlowptTrig;
            if (nhighpteta > 0 && nhighptphi > 0) ++nhighptTrig;
        }
    }
    Double_t lowPtEffxAcc = double(nlowptTrig) / double(nEventsinRPCsistem);
    Double_t highPtEffxAcc = double(nhighptTrig) / double(nEventsinRPCsistem);
    Double_t lowPtEffInAcc = double(nlowptTrig) / double(nEventsinLowPtacceptance);
    Double_t highPtEffInAcc = double(nhighptTrig) / double(nEventsinHighPtacceptance);
    Double_t lowPtAcc = double(nEventsinLowPtacceptance) / double(nEventsinRPCsistem);
    Double_t highPtAcc = double(nEventsinHighPtacceptance) / double(nEventsinRPCsistem);

    Double_t effOR = double(nMatchedHits) / double(rpc_NsimhitAll);
    Double_t effORErr = sqrt(effOR * (1. - effOR) / double(rpc_NsimhitAll));
    Double_t effPhi = double(nMatchedPhiHits) / double(rpc_NsimhitAll);
    Double_t effPhiErr = sqrt(effPhi * (1. - effPhi) / double(rpc_NsimhitAll));
    Double_t effEta = double(nMatchedEtaHits) / double(rpc_NsimhitAll);
    Double_t effEtaErr = sqrt(effEta * (1. - effEta) / double(rpc_NsimhitAll));
    Double_t effPhiAndEta = double(nMatchedPhiAndEtaHits) / double(rpc_NsimhitAll);
    Double_t effPhiAndEtaErr = sqrt(effPhiAndEta * (1. - effPhiAndEta) / double(rpc_NsimhitAll));
    Double_t effPhiOnly = double(nMatchedPhiOnlyHits) / double(rpc_NsimhitAll);
    Double_t effPhiOnlyErr = sqrt(effPhiOnly * (1. - effPhiOnly) / double(rpc_NsimhitAll));
    Double_t effEtaOnly = double(nMatchedEtaOnlyHits) / double(rpc_NsimhitAll);
    Double_t effEtaOnlyErr = sqrt(effEtaOnly * (1. - effEtaOnly) / double(rpc_NsimhitAll));
    Double_t effGap = double(nMatchedEtaOnlyHits + nMatchedPhiOnlyHits + nMatchedPhiAndEtaHits) / double(rpc_NsimhitAll);
    Double_t effGapErr = sqrt(effGap * (1. - effGap) / double(rpc_NsimhitAll));

    std::cout << "--------------------------------------------------------------------------------" << std::endl;
    std::cout << " Eff phi_or_eta (gap eff) = " << effOR << "+/-" << effORErr << " eta eff " << effEta << "+/-" << effEtaErr << " phi eff "
              << effPhi << "+/-" << effPhiErr << " \n"
              << " phi_and_eta eff " << effPhiAndEta << "+/-" << effPhiAndEtaErr << " only phi "
              << double(nMatchedPhiHits - nMatchedPhiAndEtaHits) / double(rpc_NsimhitAll) << " only eta "
              << double(nMatchedEtaHits - nMatchedPhiAndEtaHits) / double(rpc_NsimhitAll) << " \n"
              << " phi_and_eta eff " << effPhiAndEta << "+/-" << effPhiAndEtaErr << " only phi " << effPhiOnly << "+/-" << effPhiOnlyErr
              << " only eta " << effEtaOnly << "+/-" << effEtaOnlyErr << " \n"
              << " Gap Efficiency  " << effGap << "+/-" << effGapErr << std::endl;
    std::cout << " LVL1  low pt acceptance " << lowPtAcc << std::endl;
    std::cout << " LVL1 high pt acceptance " << highPtAcc << std::endl;
    std::cout << " LVL1  low pt acceptance x efficiency " << lowPtEffxAcc << std::endl;
    std::cout << " LVL1 high pt acceptance x efficiency " << highPtEffxAcc << std::endl;
    std::cout << " LVL1  low pt efficiency (within acceptance) " << lowPtEffInAcc << std::endl;
    std::cout << " LVL1 high pt efficiency (within acceptance) " << highPtEffInAcc << std::endl;
    std::cout << "--------------------------------------------------------------------------------" << std::endl;

    Int_t nPanelsAboveMeasThr = 0;

    TH1F* h_ntrackOnPanel = new TH1F("ntrackOnPanel", "ntrakOnPanel", 101, -5., 505.);
    TH1F* h_effPhiDistr = new TH1F("phiPanelEff", "phiPanelEff", 201, -0.0025, 1.0025);
    TH1F* h_effEtaDistr = new TH1F("etaPanelEff", "etaPanelEff", 201, -0.0025, 1.0025);
    TH1F* h_effGapDistr = new TH1F("gapEff", "gapEff", 201, -0.0025, 1.0025);
    for (int il = 0; il < 6; ++il) {
        for (int iz = 1; iz < netabins + 1; ++iz) {
            for (int ip = 1; ip < nphibins + 1; ++ip) {
                Double_t nHitsInBin = h_effmapHits[il]->GetBinContent(iz, ip);
                h_ntrackOnPanel->Fill(nHitsInBin);

                if (nHitsInBin < nTrackThr) continue;
                Double_t nPhiAndEtaDig = h_effmapPhiAndEtaDig[il]->GetBinContent(iz, ip);
                Double_t nPhiOnlyDig = h_effmapPhiOnlyDig[il]->GetBinContent(iz, ip);
                Double_t nEtaOnlyDig = h_effmapEtaOnlyDig[il]->GetBinContent(iz, ip);
                Double_t localPhiEff = (nPhiAndEtaDig + nPhiOnlyDig) / nHitsInBin;
                Double_t localPhiEffErr = sqrt(localPhiEff * (1. - localPhiEff) / nHitsInBin);
                Double_t localEtaEff = (nPhiAndEtaDig + nEtaOnlyDig) / nHitsInBin;
                Double_t localEtaEffErr = sqrt(localEtaEff * (1. - localEtaEff) / nHitsInBin);
                Double_t localGapEff = (nPhiAndEtaDig + nPhiOnlyDig + nEtaOnlyDig) / nHitsInBin;
                Double_t localGapEffErr = sqrt(localGapEff * (1. - localGapEff) / nHitsInBin);

                Double_t zthisBin = h_effmapPhi[il]->GetXaxis()->GetBinCenter(iz);
                Double_t phithisBin = h_effmapPhi[il]->GetYaxis()->GetBinCenter(ip);
                h_effmapPhi[il]->Fill(zthisBin, phithisBin, localPhiEff);
                h_effmapEta[il]->Fill(zthisBin, phithisBin, localEtaEff);
                h_effmapGap[il]->Fill(zthisBin, phithisBin, localGapEff);

                h_effPhiDistr->Fill(localPhiEff);
                h_effEtaDistr->Fill(localEtaEff);
                h_effGapDistr->Fill(localGapEff);
                ++nPanelsAboveMeasThr;
            }
        }
    }
    std::cout << "nPanelsAboveMeasThr = " << nPanelsAboveMeasThr << " out of " << netabins * nphibins * 6 << " threshold is " << nTrackThr
              << std::endl;

    TFile outFile("residuals.root", "RECREATE");
    h_ntrackOnPanel->Write();
    h_effPhiDistr->Write();
    h_effEtaDistr->Write();
    h_effGapDistr->Write();

    h_digEtaZResA->Write();
    h_digEtaZResC->Write();
    h_digPhiResA->Write();
    h_digPhiResC->Write();
    h_dighitPhiResA->Write();
    h_dighitPhiResC->Write();
    for (Int_t j = 0; j < 6; ++j) {
        h_hitVsZ[j]->Write();
        h_digVsZ[j]->Write();
        h_digeffVsZ[j]->Write();
        h_hitVsP[j]->Write();
        h_digVsP[j]->Write();
        h_digeffVsP[j]->Write();
        h_digRZResVsZ[j]->Write();
        h_digZResVsZ[j]->Write();
        h_dighitRZResVsZ[j]->Write();
        h_dighitZResVsZ[j]->Write();
        h_effmapHits[j]->Write();
        h_effmapPhiOnlyDig[j]->Write();
        h_effmapPhiAndEtaDig[j]->Write();
        h_effmapEtaOnlyDig[j]->Write();
        h_effmapPhi[j]->Write();
        h_effmapGap[j]->Write();
        h_effmapEta[j]->Write();
        /*
        h_dighitZResVsZsmall[j]->Write();
        h_dighitZResVsZlarge[j]->Write();
        h_dighitZResVsZdbZ1[j]->Write();
        h_dighitZResVsZdbZ2[j]->Write();
        */
    }
    outFile.Close();
}

void validateDigitization::computeDistFromPart(TVector3& myVect, Double_t& distXY, Double_t& distRZ, Double_t& distZ) {
    TVector3 vectPart(partMomX[0], partMomY[0], partMomZ[0]);
    Double_t mom = vectPart.Mag();
    TVector3 partCrossHitP = vectPart.Cross(myVect);
    partCrossHitP = (1. / mom) * partCrossHitP;
    distXY = partCrossHitP.z();
    distRZ = partCrossHitP.Perp();
    Double_t costTheta = partMomZ[0] / mom;
    Double_t zPart = myVect.Mag() * costTheta;
    distZ = myVect.z() - zPart;
    if (distZ < 0) distRZ = -1. * distRZ;
}
void validateDigitization::computeDistFromHit(TVector3& myVect, TVector3& myVectRef, Double_t& distXY, Double_t& distRZ, Double_t& distZ) {
    TVector3 distance = myVect - myVectRef;
    distXY = distance.Perp();
    Float_t sign = 1.;
    if (myVectRef.y() > 0) {
        if (myVect.x() < myVectRef.x()) sign = -1;
    } else {
        if (myVect.x() > myVectRef.x()) sign = -1;
    }
    distXY = sign * distXY;
    distZ = distance.z();
    distRZ = distance.Mag();
    if (distZ < 0) distRZ = -1. * distRZ;
}

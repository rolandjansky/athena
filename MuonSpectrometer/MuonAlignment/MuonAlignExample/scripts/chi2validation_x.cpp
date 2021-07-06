/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <TAxis.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TDecompBK.h>
#include <TEventList.h>
#include <TF1.h>
#include <TGraph.h>
#include <TLorentzVector.h>
#include <TMatrixDSym.h>
#include <TMatrixTSparse.h>
#include <TPaveText.h>
#include <TTree.h>
#include <TVectorD.h>

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "MuonFixedId.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TLine.h"
#include "TMatrixD.h"
#include "TStyle.h"
#include "TSystem.h"
#include "Tools.h"

using namespace MuonCalib;

MuonFixedId* mid;
const bool BATCH = false;
const bool doL1 = true;
const bool ROT = false;
const bool ROTZ = false;

const bool DOCORRELATIONS = true;

bool DEBUG = 0;

int m_ntrees = 0;

string dimstrL1[6] = {"X", "Y", "Z", "rotX", "rotY", "rotZ"};
string dimstrL3[6] = {"s", "z", "t", "rots", "rotz", "rott"};

double innerWidthsL1[6] = {30., 30., 10., 2., 2., 10.};
double outerWidthsL1[6] = {150., 150., 50., 10., 10., 50.};
double parRangesL1[6] = {300., 300., 32., 25., 25., 150.};
double errRangesL1[6] = {400., 400., 25., 6., 6., 30.};

// double innerWidthsL1norotz[6]    ={ 2., 2.,  7., .03, .03, 10.};
// double outerWidthsL1norotz[6]    ={ 7., 7.,  7., .5, .5, 50.};
// double innerWidthsL1norotz[6]    ={ .5, .5,  7., .03, .03, 10.};
// double outerWidthsL1norotz[6]    ={ 2., 2.,  7., .5, .5, 50.};
double innerWidthsL1norotz[6] = {15., 15., 12., 3.5, 3.5, 10.};
double outerWidthsL1norotz[6] = {30., 30., 30., 5., 5., 50.};
// double parRangesL1norotz[6]      ={60.,60., 60., 10., 10., 1.};
// double errRangesL1norotz[6]      ={45.,45., 45., 6., 6., 30.};
double parRangesL1norotz[6] = {50., 50., 50., 10., 10., 1.};
double errRangesL1norotz[6] = {45., 45., 45., 6., 6., 30.};

double innerWidthsL1nocorr[6] = {.5, .5, .8, .00001, .00001, .0001};
double outerWidthsL1nocorr[6] = {5., 5., 5., .02, .02, .002};
double parRangesL1nocorr[6] = {15., 15., 5., .0001, .0001, .01};
double errRangesL1nocorr[6] = {40., 40., 2.5, .6, .6, 3.};

double innerWidthsL3[6] = {.75, 2., 10., 2., 2., 2.};
double outerWidthsL3[6] = {8., 2., 10., 2., 2., 2.};
double parRangesL3[6] = {15., 20., 20., 10., 20., 20.};
double errRangesL3[6] = {40., 32000., 90000., 200., 320000., 200000.};

double parRangeErrCutsL1[6] = {20., 20., 30., .06, .06, 15.};
double errRangeErrCutsL1[6] = {.15, .15, .27, .01, .01, .4};
double parRangeErrCutsL3[6] = {20., 20., 30., .06, .06, 15.};
double errRangeErrCutsL3[6] = {.15, .15, .27, .01, .01, .4};

void help(const char* argv0) {
    cerr << "Usage: " << argv0 << " [options] " << endl
         << "Options:" << endl
         << " -o :\tOutput path" << endl
         << " -i :\tInput files" << endl
         << " -r :\tRootname" << endl
         << " -j :\tIteration" << endl
         << " -a :\tDraw absolute lines" << endl
         << " -l :\tL1 alignment" << endl
         << " -d :\tDebug" << endl
         << " -h :\tShow this message and quit" << endl
         << endl;
    exit(1);
}

bool contains(vector<int> vec, int test) {
    for (unsigned int i = 0; i < vec.size(); i++)
        if (vec[i] == test) return true;
    return false;
}

//_____________________________________________________________________________
int main(int argc, char** argv) {
    //----------------------------
    //-- Parse the command line --
    //----------------------------

    extern char* optarg;
    char c;
    TString inputdir("");
    TString outputdir("");
    TString rootname("");
    int iteration = 0;

    bool L1ALIGNMENT = false;
    bool ABSLINES = false;
    bool ERRCUT = false;

    while ((c = getopt(argc, argv, "-hdalo:i:r:j:")) != -1) {
        switch (c) {
            case 'o': outputdir = optarg; break;
            case 'i': inputdir = optarg; break;
            case 'r': rootname = optarg; break;
            case 'j': iteration = atoi(optarg); break;
            case 'd': DEBUG = true; break;
            case 'a': ABSLINES = true; break;
            case 'l': L1ALIGNMENT = true; break;
            default:
                cerr << "Help or unknown argument!" << endl;
                help(argv[0]);
                break;
        }
    }
    bool DOUBLEGAUS = L1ALIGNMENT;
    // bool DOUBLEGAUS=false;
    bool ANALDERIV = L1ALIGNMENT;
    bool DOROTZ = !L1ALIGNMENT;

    int minpar = (L1ALIGNMENT) ? 0 : 1;
    int maxpar = (DOROTZ) ? 5 : 4;
    // int maxpar=(DOROTZ)?5:0;

    if (rootname == "") {
        cerr << "no rootname passed, exiting..." << endl;
        exit(1);
    }
    if (inputdir == "") { inputdir = rootname; }
    if (outputdir == "") { outputdir = inputdir; }

    string* dimstr = new string[6];
    double* innerWidths = new double[6];
    double* outerWidths = new double[6];
    double* parRanges = new double[6];
    double* errRanges = new double[6];
    double* parRangeErrCuts = new double[6];
    double* errRangeErrCuts = new double[6];

    for (int i = 0; i < 6; i++) {
        dimstr[i] = (L1ALIGNMENT) ? dimstrL1[i] : dimstrL3[i];
        innerWidths[i] = (L1ALIGNMENT) ? innerWidthsL1[i] : innerWidthsL3[i];
        outerWidths[i] = (L1ALIGNMENT) ? outerWidthsL1[i] : outerWidthsL3[i];
        parRanges[i] = (L1ALIGNMENT) ? parRangesL1[i] : parRangesL3[i];
        errRanges[i] = (L1ALIGNMENT) ? errRangesL1[i] : errRangesL3[i];
        parRangeErrCuts[i] = (L1ALIGNMENT) ? parRangeErrCutsL1[i] : parRangeErrCutsL3[i];
        errRangeErrCuts[i] = (L1ALIGNMENT) ? errRangeErrCutsL1[i] : errRangeErrCutsL3[i];
        if (L1ALIGNMENT && !DOCORRELATIONS) {
            innerWidths[i] = innerWidthsL1nocorr[i];
            outerWidths[i] = outerWidthsL1nocorr[i];
            parRanges[i] = parRangesL1nocorr[i];
            errRanges[i] = errRangesL1nocorr[i];
        } else if (L1ALIGNMENT && DOCORRELATIONS && !DOROTZ) {
            innerWidths[i] = innerWidthsL1norotz[i];
            outerWidths[i] = outerWidthsL1norotz[i];
            parRanges[i] = parRangesL1norotz[i];
            errRanges[i] = errRangesL1norotz[i];
        }
    }

    gStyle->SetOptStat(110010);
    gStyle->SetOptFit(1111);
    gStyle->SetFrameFillStyle(0);
    gStyle->SetTitleFillColor(0);
    gStyle->SetTitleBorderSize(1);
    gStyle->SetPadLeftMargin(0.15);
    gStyle->SetPadRightMargin(0.04);
    gStyle->SetPalette(1);

    mid = new MuonFixedId(0);

    void* directory = gSystem->OpenDirectory(inputdir);
    if (!directory) {
        cerr << "input directory does not exist, exiting..." << endl;
        exit(1);
    }
    directory = gSystem->OpenDirectory(outputdir);
    if (!directory) {
        cerr << "create output directory: " << outputdir << endl;
        int status = gSystem->mkdir(outputdir, false);
        if (status != 0) {
            cerr << "could not create output directory, exiting..." << endl;
            exit(1);
        }
    }

    TString iterdir = inputdir + "/iter0";
    directory = gSystem->OpenDirectory(inputdir + "/iter0");
    bool multiplejobs = directory ? true : false;

    TString basefilename;
    if (multiplejobs) {
        TString iterdir = inputdir + Form("/iter%i/", iteration);
        directory = gSystem->OpenDirectory(iterdir);
        basefilename = iterdir;
        if (!directory) {
            cerr << "iteration: " << iteration << "does not exists, exiting..." << endl;
            exit(1);
        }
    } else {
        basefilename = inputdir + "/";
    }

    basefilename += "MuonAlign_" + rootname + Form("_iter%i", iteration);

    TString muonalign_filename = basefilename;
    TString alignpar_filename = basefilename;
    if (multiplejobs) {
        muonalign_filename += "_index*";
        alignpar_filename += "_solve";
    }
    muonalign_filename += ".root";
    alignpar_filename += ".root";
    std::cout << "filename: " << alignpar_filename << std::endl;

    //
    // Read in file and set branch addresses
    //

    TChain* tree = new TChain("globalChi2Deriv");

    tree->Add(muonalign_filename);
    m_ntrees = tree->GetNtrees();

    int chamberId, event;  //,nDoF,nMuonHits;
    double materialOnTrack, momentum, phi, eta;
    double perigee_x, perigee_y, perigee_z;
    int trackInfo;

    double* firstDeriv = new double[6];
    TMatrixD* secndDeriv = new TMatrixD(6, 6);

    tree->SetBranchAddress("event", &event);
    tree->SetBranchAddress("chamberId", &chamberId);
    tree->SetBranchAddress("secndDeriv", &secndDeriv);
    tree->SetBranchAddress("firstDeriv", firstDeriv);
    tree->SetBranchAddress("materialOnTrack", &materialOnTrack);
    tree->SetBranchAddress("momentum", &momentum);
    tree->SetBranchAddress("phi", &phi);
    tree->SetBranchAddress("eta", &eta);
    tree->SetBranchAddress("perigee_x", &perigee_x);
    tree->SetBranchAddress("perigee_y", &perigee_y);
    tree->SetBranchAddress("perigee_z", &perigee_z);
    tree->SetBranchAddress("trackInfo", &trackInfo);

    TChain* mualigntree = new TChain("MuonAlign");

    mualigntree->Add(muonalign_filename);
    int nTgcPhiHits, mualignRun, mualignEvt;
    int nCscPhiHits, nCscEtaHits, cscAvgPhiErr, cscAvgEtaErr;

    mualigntree->SetBranchStatus("*", 0);
    mualigntree->SetBranchStatus("nTgcPhiHits", 1);
    mualigntree->SetBranchStatus("run", 1);
    mualigntree->SetBranchStatus("evt", 1);
    mualigntree->SetBranchStatus("CscNumPhiHits", 1);
    mualigntree->SetBranchStatus("CscNumEtaHits", 1);
    mualigntree->SetBranchStatus("CscAvgPhiError", 1);
    mualigntree->SetBranchStatus("CscAvgEtaError", 1);

    mualigntree->SetBranchAddress("nTgcPhiHits", &nTgcPhiHits);
    mualigntree->SetBranchAddress("run", &mualignRun);
    mualigntree->SetBranchAddress("evt", &mualignEvt);

    mualigntree->SetBranchAddress("CscNumPhiHits", &nCscPhiHits);
    mualigntree->SetBranchAddress("CscNumEtaHits", &nCscEtaHits);
    mualigntree->SetBranchAddress("CscAvgPhiError", &cscAvgPhiErr);
    mualigntree->SetBranchAddress("CscAvgEtaError", &cscAvgEtaErr);

    //
    // Initialize variables
    //

    TH1F** hPar = new TH1F*[6];
    TH2F** h2ErrorVParam = new TH2F*[6];
    TH1F** hError = new TH1F*[6];
    TH1F** hPull = new TH1F*[6];
    TH1F** hFirstDeriv = new TH1F*[6];
    TH1F** hFirstDerivErr = new TH1F*[6];
    TH2F** h2FirstDerivErrVParam = new TH2F*[6];

    TH2F** hParVMaterialOnTrack = new TH2F*[6];
    TH2F** hParVMomentum = new TH2F*[6];
    TH2F** hParVPhi = new TH2F*[6];
    TH2F** hParVEta = new TH2F*[6];
    TH2F** hPullVEta = new TH2F*[6];

    TH2F** hErrVMaterialOnTrack = new TH2F*[6];
    TH2F** hErrVMomentum = new TH2F*[6];
    TH2F** hErrVPhi = new TH2F*[6];
    TH2F** hErrVEta = new TH2F*[6];

    TH2F** hErrVPerigeeXDist = new TH2F*[6];
    TH2F** hErrVTrackInfo = new TH2F*[6];

    TH2F** hErrVNTgcPhiHits = new TH2F*[6];
    TH2F** hParVNTgcPhiHits = new TH2F*[6];

    TH2F** hErrVCscPhiHits = new TH2F*[6];
    TH2F** hErrVCscEtaHits = new TH2F*[6];
    TH2F** hErrVCscAvgPhiErr = new TH2F*[6];
    TH2F** hErrVCscAvgEtaErr = new TH2F*[6];

    for (int i = 0; i < 6; i++) {
        stringstream hParName;
        hParName << "hPar_" << dimstr[i];
        stringstream h2ErrVParName;
        h2ErrVParName << "h2ErrorVParam_" << dimstr[i];
        stringstream hErrName;
        hErrName << "hError_" << dimstr[i];
        stringstream hPullName;
        hPullName << "hPull_" << dimstr[i];
        stringstream hParVMatlName;
        hParVMatlName << "hParVMatl_" << dimstr[i];
        stringstream hParVMomName;
        hParVMomName << "hParVMom_" << dimstr[i];
        stringstream hParVPhiName;
        hParVPhiName << "hParVPhi_" << dimstr[i];
        stringstream hParVEtaName;
        hParVPhiName << "hParVEta_" << dimstr[i];
        stringstream hPullVEtaName;
        hParVPhiName << "hPullVEta_" << dimstr[i];
        stringstream hErrVMatlName;
        hErrVMatlName << "hErrVMatl_" << dimstr[i];
        stringstream hErrVMomName;
        hErrVMomName << "hErrVMom_" << dimstr[i];
        stringstream hErrVPhiName;
        hErrVPhiName << "hErrVPhi_" << dimstr[i];
        stringstream hErrVEtaName;
        hErrVPhiName << "hErrVEta_" << dimstr[i];
        stringstream hErrVPerigeeXDistName;
        hErrVPerigeeXDistName << "hErrVPerigeeXDist_" << dimstr[i];
        stringstream hErrVTrackInfoName;
        hErrVTrackInfoName << "hErrVTrackInfo_" << dimstr[i];
        stringstream hErrVNTgcPhiHitsName;
        hErrVNTgcPhiHitsName << "hErrVNTgcPhiHits_" << dimstr[i];
        stringstream hParVNTgcPhiHitsName;
        hParVNTgcPhiHitsName << "hParVNTgcPhiHits_" << dimstr[i];
        stringstream hErrVCscPhiHitsName;
        hErrVCscPhiHitsName << "hErrVCscPhiHits_" << dimstr[i];
        stringstream hErrVCscEtaHitsName;
        hErrVCscEtaHitsName << "hErrVCscEtaHits_" << dimstr[i];
        stringstream hErrVCscAvgPhiErrName;
        hErrVCscAvgPhiErrName << "hErrVCscAvgPhiErr_" << dimstr[i];
        stringstream hErrVCscAvgEtaErrName;
        hErrVCscAvgEtaErrName << "hErrVCscAvgEtaErr_" << dimstr[i];

        double parRange = (ERRCUT) ? parRangeErrCuts[i] : parRanges[i];
        double errRange = (ERRCUT) ? errRangeErrCuts[i] : errRanges[i];

        hPar[i] = new TH1F(hParName.str().c_str(), dimstr[i].c_str(), 60, -parRange, parRange);
        h2ErrorVParam[i] = new TH2F(h2ErrVParName.str().c_str(), "param error v. param", 60, -parRange, parRange, 30, 0., errRange);
        hError[i] = new TH1F(hErrName.str().c_str(), "parameter error (from 2nd deriv)", 50, 0., errRange);
        hPull[i] = new TH1F(hPullName.str().c_str(), "pull", 50, -5., 5.);

        hParVMaterialOnTrack[i] =
            new TH2F(hParVMatlName.str().c_str(), "param v. material on track", 60, 100., 225., 60, -parRange, parRange);

        hParVMomentum[i] = new TH2F(hParVMomName.str().c_str(), "param v. momentum", 60, 0., 140., 60, -parRange, parRange);

        hParVPhi[i] = new TH2F(hParVPhiName.str().c_str(), "param v. phi", 60, -3.15, 3.15, 60, -parRange, parRange);
        double minEta = (L1ALIGNMENT) ? 1.0 : 1.25;
        double maxEta = (L1ALIGNMENT) ? 2.50 : 2.00;

        hParVEta[i] = new TH2F(hParVEtaName.str().c_str(), "param v. eta", 60, minEta, maxEta, 60, -parRange, parRange);
        hPullVEta[i] = new TH2F(hPullVEtaName.str().c_str(), "pull v. eta", 60, minEta, maxEta, 60, -5., 5.);

        double minMat = (L1ALIGNMENT) ? 100. : 0.;
        double maxMat = (L1ALIGNMENT) ? 225. : 20.;
        hErrVMaterialOnTrack[i] =
            new TH2F(hErrVMatlName.str().c_str(), "param error v. material on track", 60, minMat, maxMat, 60, 0., errRange);

        hErrVMomentum[i] = new TH2F(hErrVMomName.str().c_str(), "param error v. momentum", 60, 20., 140., 60, 0., errRange);

        double minPhi = (L1ALIGNMENT) ? -3.15 : 1.;
        double maxPhi = (L1ALIGNMENT) ? 3.15 : 2.;

        hErrVPhi[i] = new TH2F(hErrVPhiName.str().c_str(), "param error v. phi", 60, minPhi, maxPhi, 60, 0., errRange);

        hErrVEta[i] = new TH2F(hErrVEtaName.str().c_str(), "param error v. eta", 60, minEta, maxEta, 60, 0., errRange);

        hErrVPerigeeXDist[i] =
            new TH2F(hErrVPerigeeXDistName.str().c_str(), "param error vs. perigee transverse distance", 60, 0., .2, 60, 0., errRange);

        hErrVTrackInfo[i] = new TH2F(hErrVTrackInfoName.str().c_str(), "param error vs. track fitter", 10, 0., 10., 60, 0., errRange);

        hErrVNTgcPhiHits[i] = new TH2F(hErrVNTgcPhiHitsName.str().c_str(), "param error vs. # TGC phi hits", 10, 0., 20., 60, 0., errRange);

        hParVNTgcPhiHits[i] =
            new TH2F(hParVNTgcPhiHitsName.str().c_str(), "param vs. # TGC phi hits", 10, 0., 20., 60, -parRange, parRange);

        hErrVCscPhiHits[i] = new TH2F(hErrVCscPhiHitsName.str().c_str(), "param error vs. # CSC phi hits", 10, 0., 10., 60, 0., errRange);

        hErrVCscEtaHits[i] = new TH2F(hErrVCscEtaHitsName.str().c_str(), "param error vs. # CSC eta hits", 10, 0., 10., 60, 0., errRange);

        hErrVCscAvgPhiErr[i] =
            new TH2F(hErrVCscAvgPhiErrName.str().c_str(), "param error vs. # average CSC phi error", 50, 0., 15., 60, 0., errRange);

        hErrVCscAvgEtaErr[i] =
            new TH2F(hErrVCscAvgEtaErrName.str().c_str(), "param error vs. # average CSC eta error", 50, 0., 5., 60, 0., errRange);

        stringstream hFirstDerivName;
        hFirstDerivName << "hFirstDeriv_" << i;
        stringstream hFirstDerivErrName;
        hFirstDerivErrName << "hFirstDerivErr_" << i;
        stringstream hFirstDerivErrVParName;
        hFirstDerivErrVParName << "hFirstDerivChi2VParam_" << i;
        hFirstDeriv[i] = new TH1F(hFirstDerivName.str().c_str(), "first derivative", 50, -1., 1.);
        hFirstDerivErr[i] = new TH1F(hFirstDerivErrName.str().c_str(), "first deriv error", 30, 0., 1.);
        h2FirstDerivErrVParam[i] = new TH2F(hFirstDerivErrVParName.str().c_str(), "first deriv error v. param ", 30, -20., 20., 30, 0., 1.);
    }

    int nevtssel(0);

    //
    // Get list of moduleIDs
    //
    vector<int> moduleList;
    for (int hentry = 0; hentry < tree->GetEntries(); hentry++) {
        tree->GetEntry(hentry);
        if (!contains(moduleList, chamberId)) {
            moduleList.push_back(chamberId);
            std::cout << "pushing back " << chamberId << std::endl;
        }
    }  // end for hentry

    cout << "Found " << moduleList.size() << " modules: ";
    for (unsigned int tmp = 0; tmp < moduleList.size(); tmp++) cout << moduleList[tmp] << " ";
    cout << endl;

    TEventList* evlist = new TEventList("evlist");
    for (unsigned int selMod = 0; selMod < moduleList.size(); selMod++) {
        stringstream selcut;
        selcut << "chamberId==" << moduleList[selMod];
        int numEvent = tree->Draw(">>evlist", selcut.str().c_str());
        for (int selEvent = 0; selEvent < numEvent; selEvent++) {
            int ientry = evlist->GetEntry(selEvent);
            tree->GetEntry(ientry);
            mualigntree->GetEntry(ientry);
            if (event != mualignEvt) {
                std::cout << "bad run or event!" << std::endl;
                exit(3);
            }

            int npar = maxpar - minpar + 1;
            double* secderivarr = new double[npar * npar];
            double* firstderivarr = new double[npar];
            for (int i = 0; i < npar; i++) {
                int ipar = i + minpar;
                for (int j = i; j < npar; j++) {
                    int jpar = j + minpar;
                    double secderiv = (*secndDeriv)[ipar][jpar];
                    // if (!ANALDERIV) {
                    if (ipar > 2) secderiv *= .001;
                    if (jpar > 2) secderiv *= .001;
                    //}
                    secderivarr[i * npar + j] = secderiv;
                    secderivarr[j * npar + i] = secderiv;
                    if (jpar == ipar) {
                        firstderivarr[i] = firstDeriv[ipar];
                        // if (!ANALDERIV)
                        if (ipar > 2) firstderivarr[i] *= .001;
                    }
                }
            }

            TMatrixDSym a(npar, secderivarr);
            TVectorD b(npar, firstderivarr);
            // std::cout<<"a/b: "<<std::endl;
            // a.Print();
            // b.Print();

            TDecompBK c(a);
            // std::cout<<"c: "<<std::endl;
            // c.Print();

            Bool_t status;

            TMatrixDSym ainv(c.Invert(status));
            // std::cout<<"ainv: "<<std::endl;
            // ainv.Print();

            TMatrixD prod = ainv * a;
            // std::cout<<"prod: "<<std::endl;
            // prod.Print();

            TVectorD r(b.GetNrows());
            if (status) { r = c.Solve(b, status); }
            if (!status) {
                std::cout << "problem with matrix inversion" << std::endl;
                continue;
            }

            // std::cout<<"r: "<<std::endl;
            // r.Print();
            if (fabs(r[0]) < .01) {
                cout << "a: " << endl;
                a.Print();
                cout << "b: " << endl;
                b.Print();
                cout << "r: " << endl;
                r.Print();
            }
            double* errors = new double[6];
            for (int i = 0; i < npar; i++) {
                int ipar = (L1ALIGNMENT) ? i : i + 1;
                if (DOCORRELATIONS)
                    errors[ipar] = std::sqrt(2. * std::fabs(ainv(i, i)));
                else
                    errors[ipar] = 1000. * std::sqrt(2. / std::fabs((*secndDeriv)[ipar][ipar]));
                // std::cout<<"errors["<<ipar<<"]="<<errors[ipar]<<std::endl;
            }
            if (!L1ALIGNMENT) errors[0] = 0.;

            for (int ipar = minpar; ipar <= maxpar; ipar++) {
                // double param=-firstDeriv[ipar]/(*secndDeriv)[ipar][ipar];
                int iipar = (L1ALIGNMENT) ? ipar : ipar - 1;
                double param = (DOCORRELATIONS) ? -r[iipar] : firstDeriv[ipar] / (*secndDeriv)[ipar][ipar];
                double error = errors[ipar];

                if (ERRCUT && error > 5.) continue;

                // if (ipar>2) { param *=1000.; error *= 1000.; }

                if (ERRCUT && error > errRangeErrCuts[ipar]) continue;
                nevtssel++;

                hPar[ipar]->Fill(param);

                hFirstDeriv[ipar]->Fill(firstDeriv[ipar]);
                hPull[ipar]->Fill(param / error);
                h2ErrorVParam[ipar]->Fill(param, error);
                hError[ipar]->Fill(error);

                hParVMaterialOnTrack[ipar]->Fill(materialOnTrack, param);
                hParVMomentum[ipar]->Fill(momentum / 1000., param);
                hParVPhi[ipar]->Fill(phi, param);
                hParVEta[ipar]->Fill(eta, param);
                hPullVEta[ipar]->Fill(eta, param / error);

                hErrVMaterialOnTrack[ipar]->Fill(materialOnTrack, error);
                hErrVMomentum[ipar]->Fill(momentum / 1000., error);
                hErrVPhi[ipar]->Fill(phi, error);
                hErrVEta[ipar]->Fill(eta, error);

                hErrVNTgcPhiHits[ipar]->Fill(nTgcPhiHits, error);
                hParVNTgcPhiHits[ipar]->Fill(nTgcPhiHits, param);

                hErrVCscPhiHits[ipar]->Fill(nCscPhiHits, error);
                hErrVCscEtaHits[ipar]->Fill(nCscEtaHits, error);
                hErrVCscAvgPhiErr[ipar]->Fill(cscAvgPhiErr, error);
                hErrVCscAvgEtaErr[ipar]->Fill(cscAvgEtaErr, error);

                double perigeeDist = std::sqrt(perigee_x * perigee_x + perigee_y * perigee_y);
                hErrVPerigeeXDist[ipar]->Fill(perigeeDist, error);

                hErrVTrackInfo[ipar]->Fill(trackInfo, error);
            }
            delete[] errors;
        }

        // plot histograms
        const int NCANVAS = 21;
        TCanvas** canv = new TCanvas*[NCANVAS];
        for (int i = 0; i < NCANVAS; i++) {
            stringstream canvname;
            canvname << "canv_" << i;
            canv[i] = new TCanvas(canvname.str().c_str(), canvname.str().c_str());
            canv[i]->SetFillColor(10);
            canv[i]->Divide(3, 2);
        }

        TF1** f = new TF1*[6];
        for (int i = minpar; i <= maxpar; i++) {
            double parRange = (ERRCUT) ? parRangeErrCuts[i] : parRanges[i];
            double errRange = (ERRCUT) ? errRangeErrCuts[i] : errRanges[i];

            string units = (i > 2) ? "(mrad)" : "(mm)";

            canv[0]->cd(i + 1);
            hPar[i]->Draw();
            hPar[i]->GetXaxis()->SetTitle(("1-track alignment parameter " + units).c_str());
            hPar[i]->GetYaxis()->SetTitle("# tracks");

            TH1F* h = hPar[i];
            if (DOUBLEGAUS) {
                // f[i]=new TF1(dimstr[i].c_str(),"gaus(0)+gaus(3)",-parRange,parRange);
                f[i] = new TF1(dimstr[i].c_str(), "[0]*exp(-0.5*((x-[1])/[2])^2)+[3]*exp(-0.5*((x-[1])/[4])^2)", -parRange, parRange);
                // f[i]->SetParameters(h->GetMaximum(), h->GetMean(),
                //	    innerWidths[i], h->GetMaximum()/3.5, h->GetMean(),
                //	    outerWidths[i]);
                f[i]->SetParameters(h->GetMaximum(), h->GetMean(), innerWidths[i], h->GetMaximum() / 3.5, outerWidths[i]);
            } else {
                f[i] = new TF1(dimstr[i].c_str(), "[0]*exp(-0.5*((x-[1])/[2])^2)", -parRange, parRange);
                f[i]->SetParameters(h->GetMaximum(), h->GetMean(), innerWidths[i]);
            }

            h->Fit(f[i], "Q");

            canv[1]->cd(i + 1);
            hError[i]->Draw();
            hError[i]->GetXaxis()->SetTitle(("error (from second derivative) " + units).c_str());
            hError[i]->GetYaxis()->SetTitle("# tracks");

            canv[2]->cd(i + 1);
            h2ErrorVParam[i]->Draw("colz");
            h2ErrorVParam[i]->GetXaxis()->SetTitle(("1-track alignment parameter " + units).c_str());
            h2ErrorVParam[i]->GetYaxis()->SetTitle(("error (from 2nd derivative) " + units).c_str());

            canv[3]->cd(i + 1);
            hPull[i]->Draw();
            hPull[i]->GetXaxis()->SetTitle("1-track alignment parameter/error");
            hPull[i]->GetYaxis()->SetTitle("# tracks");
            hPull[i]->Fit("gaus");

            canv[4]->cd(i + 1);
            hParVMaterialOnTrack[i]->Draw("colz");
            hParVMaterialOnTrack[i]->GetXaxis()->SetTitle("material on track (in X0)");
            hParVMaterialOnTrack[i]->GetYaxis()->SetTitle(("1-track alignment parameters " + units).c_str());

            canv[5]->cd(i + 1);
            hParVMomentum[i]->Draw("colz");
            hParVMomentum[i]->GetXaxis()->SetTitle("momentum (GeV)");
            hParVMomentum[i]->GetYaxis()->SetTitle(("1-track alignment parameters " + units).c_str());

            canv[6]->cd(i + 1);
            hParVPhi[i]->Draw("colz");
            hParVPhi[i]->GetXaxis()->SetTitle("phi");
            hParVPhi[i]->GetYaxis()->SetTitle(("1-track alignment parameters " + units).c_str());

            canv[7]->cd(i + 1);
            hParVEta[i]->Draw("colz");
            hParVEta[i]->GetXaxis()->SetTitle("eta");
            hParVEta[i]->GetYaxis()->SetTitle(("1-track alignment parameters " + units).c_str());

            canv[8]->cd(i + 1);
            hErrVMaterialOnTrack[i]->Draw("colz");
            hErrVMaterialOnTrack[i]->GetXaxis()->SetTitle("material on track (in X0)");
            hErrVMaterialOnTrack[i]->GetYaxis()->SetTitle(("1-track alignment parameter errors " + units).c_str());

            canv[9]->cd(i + 1);
            hErrVMomentum[i]->Draw("colz");
            hErrVMomentum[i]->GetXaxis()->SetTitle("momentum (GeV)");
            hErrVMomentum[i]->GetYaxis()->SetTitle(("1-track alignment parameter errors " + units).c_str());

            canv[10]->cd(i + 1);
            hErrVPhi[i]->Draw("colz");
            hErrVPhi[i]->GetXaxis()->SetTitle("phi");
            hErrVPhi[i]->GetYaxis()->SetTitle(("1-track alignment parameter errors " + units).c_str());

            canv[11]->cd(i + 1);
            hErrVEta[i]->Draw("colz");
            hErrVEta[i]->GetXaxis()->SetTitle("eta");
            hErrVEta[i]->GetYaxis()->SetTitle(("1-track alignment parameter errors " + units).c_str());

            canv[12]->cd(i + 1);
            hErrVTrackInfo[i]->Draw("colz");
            hErrVTrackInfo[i]->GetXaxis()->SetTitle("track fitter");
            hErrVTrackInfo[i]->GetYaxis()->SetTitle(("1-track alignment parameter errors " + units).c_str());
            canv[13]->cd(i + 1);
            hErrVPerigeeXDist[i]->Draw("colz");
            hErrVPerigeeXDist[i]->GetXaxis()->SetTitle("perigee transverse distance");
            hErrVPerigeeXDist[i]->GetYaxis()->SetTitle(("1-track alignment parameter errors " + units).c_str());

            canv[14]->cd(i + 1);
            hPullVEta[i]->Draw("colz");
            hPullVEta[i]->GetXaxis()->SetTitle("eta");
            hPullVEta[i]->GetYaxis()->SetTitle("1-track alignment parameter/error");

            canv[15]->cd(i + 1);
            hErrVNTgcPhiHits[i]->Draw("colz");
            hErrVNTgcPhiHits[i]->GetXaxis()->SetTitle("# TGC #phi hits");
            hErrVNTgcPhiHits[i]->GetYaxis()->SetTitle(("1-track alignment errors " + units).c_str());

            canv[16]->cd(i + 1);
            hParVNTgcPhiHits[i]->Draw("colz");
            hParVNTgcPhiHits[i]->GetXaxis()->SetTitle("# TGC #phi hits");
            hParVNTgcPhiHits[i]->GetYaxis()->SetTitle(("1-track alignment parameters " + units).c_str());
            canv[17]->cd(i + 1);
            hErrVCscPhiHits[i]->Draw("colz");
            hErrVCscPhiHits[i]->GetXaxis()->SetTitle("# CSC #phi hits");
            hErrVCscPhiHits[i]->GetYaxis()->SetTitle(("1-track alignment parameter errors " + units).c_str());

            canv[18]->cd(i + 1);
            hErrVCscEtaHits[i]->Draw("colz");
            hErrVCscEtaHits[i]->GetXaxis()->SetTitle("# CSC #eta hits");
            hErrVCscEtaHits[i]->GetYaxis()->SetTitle(("1-track alignment parameter errors " + units).c_str());

            canv[19]->cd(i + 1);
            hErrVCscAvgPhiErr[i]->Draw("colz");
            hErrVCscAvgPhiErr[i]->GetXaxis()->SetTitle("# CSC average #phi error");
            hErrVCscAvgPhiErr[i]->GetYaxis()->SetTitle(("1-track alignment parameter errors " + units).c_str());

            canv[20]->cd(i + 1);
            hErrVCscAvgEtaErr[i]->Draw("colz");
            hErrVCscAvgEtaErr[i]->GetXaxis()->SetTitle("# CSC average #eta error");
            hErrVCscAvgEtaErr[i]->GetYaxis()->SetTitle(("1-track alignment parameter errors " + units).c_str());
        }
        delete[] f;

        stringstream chname;
        chname << "ch" << moduleList[selMod];

        string canvnames[NCANVAS] = {string(outputdir) + "/" + chname.str() + "_chi2val_alignParam.eps",
                                     string(outputdir) + "/" + chname.str() + "_chi2val_alignParamErr.eps",
                                     string(outputdir) + "/" + chname.str() + "_chi2val_alignParamVErr.eps",
                                     string(outputdir) + "/" + chname.str() + "_chi2val_pull.eps",
                                     string(outputdir) + "/" + chname.str() + "_chi2val_par_v_matl.eps",
                                     string(outputdir) + "/" + chname.str() + "_chi2val_par_v_momentum.eps",
                                     string(outputdir) + "/" + chname.str() + "_chi2val_par_v_phi.eps",
                                     string(outputdir) + "/" + chname.str() + "_chi2val_par_v_eta.eps",
                                     string(outputdir) + "/" + chname.str() + "_chi2val_err_v_matl.eps",
                                     string(outputdir) + "/" + chname.str() + "_chi2val_err_v_momentum.eps",
                                     string(outputdir) + "/" + chname.str() + "_chi2val_err_v_phi.eps",
                                     string(outputdir) + "/" + chname.str() + "_chi2val_err_v_eta.eps",
                                     string(outputdir) + "/" + chname.str() + "_chi2val_err_v_trackInfo.eps",
                                     string(outputdir) + "/" + chname.str() + "_chi2val_err_v_perigeeXDist.eps",
                                     string(outputdir) + "/" + chname.str() + "_chi2val_pull_v_eta.eps",
                                     string(outputdir) + "/" + chname.str() + "_chi2val_err_v_nTgcPhiHits.eps",
                                     string(outputdir) + "/" + chname.str() + "_chi2val_par_v_nTgcPhiHits.eps",
                                     string(outputdir) + "/" + chname.str() + "_chi2val_err_v_nCscPhiHits.eps",
                                     string(outputdir) + "/" + chname.str() + "_chi2val_err_v_nCscEtaHits.eps",
                                     string(outputdir) + "/" + chname.str() + "_chi2val_err_v_nCscAvgPhiErr.eps",
                                     string(outputdir) + "/" + chname.str() + "_chi2val_err_v_nCscAvgEtaErr.eps"};
        for (int i = 0; i < NCANVAS; i++) canv[i]->Print(canvnames[i].c_str());
    }
}

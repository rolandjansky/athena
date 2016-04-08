/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ==========================================================================
// Code for Event-Based Onia ntuple
// Modified : 2011 October 02
// Yi Yang <yi.yang@cern.ch>  Indiana University
//
// In ROOT: 
// root [0] .L BTrigger.C+
// root [1] StartAnalysis()
// ==========================================================================
#include <iostream>
#include "TROOT.h"
#include <TList.h>
#include <TString.h>
#include <TPRegexp.h>
#include <TSystemDirectory.h>
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "Riostream.h"
#include "TChain.h"
#include "TMath.h"
#include "TSystem.h"
#include "TLorentzVector.h"
#include <vector>
#include <sstream>
#include <string>

// GRLs : if no GRL package comments out 2 lines
#include "GoodRunsLists/TGoodRunsListReader.h"
#include "GoodRunsLists/TGoodRunsList.h"

#ifdef __CINT__
#pragma link C++ class std::vector<std::vector<string> >+;
#pragma link C++ class std::vector<std::vector<int> >+;
#endif

using namespace std;
// Looping varibles
int i, j, k;
double mu_mass = 105.658;
double GeV = 1000.;

// ==============================================================================================
// Loading the input files 
// ==============================================================================================
std::vector<TString> getFileNames()
{
    std::ifstream ifs("input.txt");
    std::vector<TString> theNames;
    Int_t nlines = 0;
    TString xstring;
    while (1) {
        ifs >> xstring ;
        theNames.push_back( xstring);
        if (!ifs.good()) break;
        nlines++;
    }

    return theNames;
}
// ================================================================================ 
// Analysis Functions
// ================================================================================ 
bool hasRegExp(std::string str, std::string subst) {
    TString tstr(str);
    TPRegexp rexp(subst);  
    return tstr.Contains(rexp);
}
// Delta R
double dR( double eta_1, double phi_1, double eta_2, double phi_2 ){
    double dEta = fabs( eta_1 - eta_2 );
    double dPhi = fabs( phi_1 - phi_2 );
    if ( dPhi > TMath::Pi() ){ dPhi = 2*TMath::Pi() - dPhi ;}
    return sqrt( dEta*dEta + dPhi*dPhi ) ;
}

// ===============================================================================================
// Main Analysis
// ===============================================================================================
void StartAnalysis( std::vector<std::string> denominators, std::vector<std::string> chains, std::vector<double> cuts, bool use_grl, const char *grl_file ) {
    int NumDeno  = denominators.size();
    int NumChain = chains.size();
    
    cout << "" << endl;
    cout << " ================= Selection Cuts ====================" << endl;
    cout << " Number of SCT             >= " << (int)cuts.at(0) << endl;
    cout << " Number of Pixel           >= " << (int)cuts.at(1) << endl;
    cout << " Number of TRT             >= " << (int)cuts.at(2) << endl;
    cout << " Number of Combined Muon    = " << (int)cuts.at(3) << endl;
    cout << " pT cut on the leading muon = " << cuts.at(4) << " GeV "<< endl;
    cout << " pT cut on the Second muon  = " << cuts.at(5) << " GeV "<< endl;
    cout << " eta cut on the muons       = " << cuts.at(6) << endl;
    cout << " =====================================================" << endl;
    cout << "" << endl;
    cout << " ================= Trigger Chains ====================" << endl;
    cout << " Number for Denominator Triggers : "<< NumDeno  << endl;
    for ( i = 0; i < NumDeno; i++ ){
        cout << " " << denominators.at(i) << endl;
    }
    cout << " ---------------------------------------" << endl;
    cout << " Number for Trigger Chains : "<< NumChain << endl;
    for ( i = 0; i < NumChain; i++ ){
        cout << " " << chains.at(i) << endl;
    }
    cout << " =====================================================" << endl;
    cout << "" << endl;

    // =============================================================================================
    // Selection Cuts 
    // =============================================================================================
    int NumSCT    = (int)cuts.at(0);
    int NumPixel  = (int)cuts.at(1);
    int NumTRT    = (int)cuts.at(2);
    int NumComb   = (int)cuts.at(3);
    float Trk1_pt = cuts.at(4);
    float Trk2_pt = cuts.at(5);
    float Trk_eta = cuts.at(6);

    float LoM_jpsi = 2.;
    float HiM_jpsi = 5.;
    float LoM_upsi = 5.;
    float HiM_upsi = 12.;
    float LoM_bmumu = 4.5;
    float HiM_bmumu = 7.5;
    // =============================================================================================
    // Book Histograms 
    // ============================================================================================= 
    TH1F *h_Jpsi_mass[NumDeno][NumChain];
    TH1F *h_Jpsi_mass_sg[NumDeno][NumChain];
    TH1F *h_Jpsi_mass_bk1[NumDeno][NumChain];
    TH1F *h_Jpsi_mass_bk2[NumDeno][NumChain];
    TH1F *h_Jpsi_mu1_pt_sg[NumDeno][NumChain];
    TH1F *h_Jpsi_mu1_pt_bk1[NumDeno][NumChain];
    TH1F *h_Jpsi_mu1_pt_bk2[NumDeno][NumChain];
    TH1F *h_Jpsi_mu1_eta_sg[NumDeno][NumChain];
    TH1F *h_Jpsi_mu1_eta_bk1[NumDeno][NumChain];
    TH1F *h_Jpsi_mu1_eta_bk2[NumDeno][NumChain];
    TH1F *h_Jpsi_mu2_pt_sg[NumDeno][NumChain];
    TH1F *h_Jpsi_mu2_pt_bk1[NumDeno][NumChain];
    TH1F *h_Jpsi_mu2_pt_bk2[NumDeno][NumChain];
    TH1F *h_Jpsi_mu2_eta_sg[NumDeno][NumChain];
    TH1F *h_Jpsi_mu2_eta_bk1[NumDeno][NumChain];
    TH1F *h_Jpsi_mu2_eta_bk2[NumDeno][NumChain];

    TH1F *h_Upsi_mass[NumDeno][NumChain];
    TH1F *h_Upsi_mass_sg[NumDeno][NumChain];
    TH1F *h_Upsi_mass_bk1[NumDeno][NumChain];
    TH1F *h_Upsi_mass_bk2[NumDeno][NumChain];
    TH1F *h_Upsi_mu1_pt_sg[NumDeno][NumChain];
    TH1F *h_Upsi_mu1_pt_bk1[NumDeno][NumChain];
    TH1F *h_Upsi_mu1_pt_bk2[NumDeno][NumChain];
    TH1F *h_Upsi_mu1_eta_sg[NumDeno][NumChain];
    TH1F *h_Upsi_mu1_eta_bk1[NumDeno][NumChain];
    TH1F *h_Upsi_mu1_eta_bk2[NumDeno][NumChain];
    TH1F *h_Upsi_mu2_pt_sg[NumDeno][NumChain];
    TH1F *h_Upsi_mu2_pt_bk1[NumDeno][NumChain];
    TH1F *h_Upsi_mu2_pt_bk2[NumDeno][NumChain];
    TH1F *h_Upsi_mu2_eta_sg[NumDeno][NumChain];
    TH1F *h_Upsi_mu2_eta_bk1[NumDeno][NumChain];
    TH1F *h_Upsi_mu2_eta_bk2[NumDeno][NumChain];

    TH1F *h_Bmumu_mass[NumDeno][NumChain];
    TH1F *h_Bmumu_mass_sg[NumDeno][NumChain];
    TH1F *h_Bmumu_mass_bk1[NumDeno][NumChain];
    TH1F *h_Bmumu_mass_bk2[NumDeno][NumChain];
    TH1F *h_Bmumu_mu1_pt_sg[NumDeno][NumChain];
    TH1F *h_Bmumu_mu1_pt_bk1[NumDeno][NumChain];
    TH1F *h_Bmumu_mu1_pt_bk2[NumDeno][NumChain];
    TH1F *h_Bmumu_mu1_eta_sg[NumDeno][NumChain];
    TH1F *h_Bmumu_mu1_eta_bk1[NumDeno][NumChain];
    TH1F *h_Bmumu_mu1_eta_bk2[NumDeno][NumChain];
    TH1F *h_Bmumu_mu2_pt_sg[NumDeno][NumChain];
    TH1F *h_Bmumu_mu2_pt_bk1[NumDeno][NumChain];
    TH1F *h_Bmumu_mu2_pt_bk2[NumDeno][NumChain];
    TH1F *h_Bmumu_mu2_eta_sg[NumDeno][NumChain];
    TH1F *h_Bmumu_mu2_eta_bk1[NumDeno][NumChain];
    TH1F *h_Bmumu_mu2_eta_bk2[NumDeno][NumChain];

    for ( j = 0; j < NumDeno; j++ ){
        std::stringstream rStr_demo;
        rStr_demo << denominators.at(j) ;
        for ( i = 0; i < NumChain; i++ ){
            std::stringstream rStr, sStr;
            rStr << chains.at(i) ; 
            sStr << "";
            // Mass 
            h_Jpsi_mass[j][i] = new TH1F( ("h_Jpsi_mass_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), ("Mu-pair Mass, "+rStr.str()+" "+sStr.str()+"").c_str(), (int)((HiM_jpsi-LoM_jpsi)*200), LoM_jpsi, HiM_jpsi);
            h_Jpsi_mass_sg[j][i] = new TH1F( ("h_Jpsi_mass_sg_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), ("Mu-pair Mass, "+rStr.str()+" "+sStr.str()+"").c_str(), (int)((HiM_jpsi-LoM_jpsi)*200), LoM_jpsi, HiM_jpsi);
            h_Jpsi_mass_bk1[j][i] = new TH1F( ("h_Jpsi_mass_bk1_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), ("Mu-pair Mass, "+rStr.str()+" "+sStr.str()+"").c_str(), (int)((HiM_jpsi-LoM_jpsi)*200), LoM_jpsi, HiM_jpsi);
            h_Jpsi_mass_bk2[j][i] = new TH1F( ("h_Jpsi_mass_bk2_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), ("Mu-pair Mass, "+rStr.str()+" "+sStr.str()+"").c_str(), (int)((HiM_jpsi-LoM_jpsi)*200), LoM_jpsi, HiM_jpsi);

            h_Jpsi_mu1_pt_sg[j][i]   = new TH1F( ("h_Jpsi_mu1_pt_sg_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu1 pt", 40, 0, 20 );
            h_Jpsi_mu1_pt_bk1[j][i]  = new TH1F( ("h_Jpsi_mu1_pt_bk1_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu1 pt", 40, 0, 20 );
            h_Jpsi_mu1_pt_bk2[j][i]  = new TH1F( ("h_Jpsi_mu1_pt_bk2_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu1 pt", 40, 0, 20 );
            h_Jpsi_mu1_eta_sg[j][i]  = new TH1F( ("h_Jpsi_mu1_eta_sg_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu1 eta", 50, -2.5, 2.5 );
            h_Jpsi_mu1_eta_bk1[j][i] = new TH1F( ("h_Jpsi_mu1_eta_bk1_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu1 eta", 50, -2.5, 2.5 );
            h_Jpsi_mu1_eta_bk2[j][i] = new TH1F( ("h_Jpsi_mu1_eta_bk2_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu1 eta", 50, -2.5, 2.5 );
            h_Jpsi_mu2_pt_sg[j][i]   = new TH1F( ("h_Jpsi_mu2_pt_sg_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu2 pt", 40, 0, 20 );
            h_Jpsi_mu2_pt_bk1[j][i]  = new TH1F( ("h_Jpsi_mu2_pt_bk1_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu2 pt", 40, 0, 20 );
            h_Jpsi_mu2_pt_bk2[j][i]  = new TH1F( ("h_Jpsi_mu2_pt_bk2_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu2 pt", 40, 0, 20 );
            h_Jpsi_mu2_eta_sg[j][i]  = new TH1F( ("h_Jpsi_mu2_eta_sg_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu2 eta", 50, -2.5, 2.5 );
            h_Jpsi_mu2_eta_bk1[j][i] = new TH1F( ("h_Jpsi_mu2_eta_bk1_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu2 eta", 50, -2.5, 2.5 );
            h_Jpsi_mu2_eta_bk2[j][i] = new TH1F( ("h_Jpsi_mu2_eta_bk2_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu2 eta", 50, -2.5, 2.5 );

            h_Jpsi_mass[j][i] -> Sumw2();
            h_Jpsi_mass_sg[j][i] -> Sumw2();
            h_Jpsi_mass_bk1[j][i] -> Sumw2();
            h_Jpsi_mass_bk2[j][i] -> Sumw2();
            h_Jpsi_mu1_pt_sg[j][i] -> Sumw2();
            h_Jpsi_mu1_pt_bk1[j][i] -> Sumw2();
            h_Jpsi_mu1_pt_bk2[j][i] -> Sumw2();
            h_Jpsi_mu1_eta_sg[j][i] -> Sumw2();
            h_Jpsi_mu1_eta_bk1[j][i] -> Sumw2();
            h_Jpsi_mu1_eta_bk2[j][i] -> Sumw2();
            h_Jpsi_mu2_pt_sg[j][i] -> Sumw2();
            h_Jpsi_mu2_pt_bk1[j][i] -> Sumw2();
            h_Jpsi_mu2_pt_bk2[j][i] -> Sumw2();
            h_Jpsi_mu2_eta_sg[j][i] -> Sumw2();
            h_Jpsi_mu2_eta_bk1[j][i] -> Sumw2();
            h_Jpsi_mu2_eta_bk2[j][i] -> Sumw2();

            h_Upsi_mass[j][i] = new TH1F( ("h_Upsi_mass_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), ("Mu-pair Mass, "+rStr.str()+" "+sStr.str()+"").c_str(), (int)((HiM_upsi-LoM_upsi)*200), LoM_upsi, HiM_upsi);
            h_Upsi_mass_sg[j][i] = new TH1F( ("h_Upsi_mass_sg_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), ("Mu-pair Mass, "+rStr.str()+" "+sStr.str()+"").c_str(), (int)((HiM_upsi-LoM_upsi)*200), LoM_upsi, HiM_upsi);
            h_Upsi_mass_bk1[j][i] = new TH1F( ("h_Upsi_mass_bk1_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), ("Mu-pair Mass, "+rStr.str()+" "+sStr.str()+"").c_str(), (int)((HiM_upsi-LoM_upsi)*200), LoM_upsi, HiM_upsi);
            h_Upsi_mass_bk2[j][i] = new TH1F( ("h_Upsi_mass_bk2_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), ("Mu-pair Mass, "+rStr.str()+" "+sStr.str()+"").c_str(), (int)((HiM_upsi-LoM_upsi)*200), LoM_upsi, HiM_upsi);

            h_Upsi_mu1_pt_sg[j][i]   = new TH1F( ("h_Upsi_mu1_pt_sg_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu1 pt", 40, 0, 20 );
            h_Upsi_mu1_pt_bk1[j][i]  = new TH1F( ("h_Upsi_mu1_pt_bk1_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu1 pt", 40, 0, 20 );
            h_Upsi_mu1_pt_bk2[j][i]  = new TH1F( ("h_Upsi_mu1_pt_bk2_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu1 pt", 40, 0, 20 );
            h_Upsi_mu1_eta_sg[j][i]  = new TH1F( ("h_Upsi_mu1_eta_sg_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu1 eta", 50, -2.5, 2.5 );
            h_Upsi_mu1_eta_bk1[j][i] = new TH1F( ("h_Upsi_mu1_eta_bk1_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu1 eta", 50, -2.5, 2.5 );
            h_Upsi_mu1_eta_bk2[j][i] = new TH1F( ("h_Upsi_mu1_eta_bk2_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu1 eta", 50, -2.5, 2.5 );
            h_Upsi_mu2_pt_sg[j][i]   = new TH1F( ("h_Upsi_mu2_pt_sg_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu2 pt", 40, 0, 20 );
            h_Upsi_mu2_pt_bk1[j][i]  = new TH1F( ("h_Upsi_mu2_pt_bk1_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu2 pt", 40, 0, 20 );
            h_Upsi_mu2_pt_bk2[j][i]  = new TH1F( ("h_Upsi_mu2_pt_bk2_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu2 pt", 40, 0, 20 );
            h_Upsi_mu2_eta_sg[j][i]  = new TH1F( ("h_Upsi_mu2_eta_sg_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu2 eta", 50, -2.5, 2.5 );
            h_Upsi_mu2_eta_bk1[j][i] = new TH1F( ("h_Upsi_mu2_eta_bk1_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu2 eta", 50, -2.5, 2.5 );
            h_Upsi_mu2_eta_bk2[j][i] = new TH1F( ("h_Upsi_mu2_eta_bk2_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu2 eta", 50, -2.5, 2.5 );

            h_Upsi_mass[j][i] -> Sumw2();
            h_Upsi_mass_sg[j][i] -> Sumw2();
            h_Upsi_mass_bk1[j][i] -> Sumw2();
            h_Upsi_mass_bk2[j][i] -> Sumw2();
            h_Upsi_mu1_pt_sg[j][i] -> Sumw2();
            h_Upsi_mu1_pt_bk1[j][i] -> Sumw2();
            h_Upsi_mu1_pt_bk2[j][i] -> Sumw2();
            h_Upsi_mu1_eta_sg[j][i] -> Sumw2();
            h_Upsi_mu1_eta_bk1[j][i] -> Sumw2();
            h_Upsi_mu1_eta_bk2[j][i] -> Sumw2();
            h_Upsi_mu2_pt_sg[j][i] -> Sumw2();
            h_Upsi_mu2_pt_bk1[j][i] -> Sumw2();
            h_Upsi_mu2_pt_bk2[j][i] -> Sumw2();
            h_Upsi_mu2_eta_sg[j][i] -> Sumw2();
            h_Upsi_mu2_eta_bk1[j][i] -> Sumw2();
            h_Upsi_mu2_eta_bk2[j][i] -> Sumw2();

            // Bmumu
            h_Bmumu_mass[j][i] = new TH1F( ("h_Bmumu_mass_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), ("Mu-pair Mass, "+rStr.str()+" "+sStr.str()+"").c_str(), (int)((HiM_bmumu-LoM_bmumu)*200), LoM_bmumu, HiM_bmumu);
            h_Bmumu_mass_sg[j][i] = new TH1F( ("h_Bmumu_mass_sg_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), ("Mu-pair Mass, "+rStr.str()+" "+sStr.str()+"").c_str(), (int)((HiM_bmumu-LoM_bmumu)*200), LoM_bmumu, HiM_bmumu);
            h_Bmumu_mass_bk1[j][i] = new TH1F( ("h_Bmumu_mass_bk1_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), ("Mu-pair Mass, "+rStr.str()+" "+sStr.str()+"").c_str(), (int)((HiM_bmumu-LoM_bmumu)*200), LoM_bmumu, HiM_bmumu);
            h_Bmumu_mass_bk2[j][i] = new TH1F( ("h_Bmumu_mass_bk2_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), ("Mu-pair Mass, "+rStr.str()+" "+sStr.str()+"").c_str(), (int)((HiM_bmumu-LoM_bmumu)*200), LoM_bmumu, HiM_bmumu);

            h_Bmumu_mu1_pt_sg[j][i]   = new TH1F( ("h_Bmumu_mu1_pt_sg_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu1 pt", 40, 0, 20 );
            h_Bmumu_mu1_pt_bk1[j][i]  = new TH1F( ("h_Bmumu_mu1_pt_bk1_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu1 pt", 40, 0, 20 );
            h_Bmumu_mu1_pt_bk2[j][i]  = new TH1F( ("h_Bmumu_mu1_pt_bk2_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu1 pt", 40, 0, 20 );
            h_Bmumu_mu1_eta_sg[j][i]  = new TH1F( ("h_Bmumu_mu1_eta_sg_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu1 eta", 50, -2.5, 2.5 );
            h_Bmumu_mu1_eta_bk1[j][i] = new TH1F( ("h_Bmumu_mu1_eta_bk1_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu1 eta", 50, -2.5, 2.5 );
            h_Bmumu_mu1_eta_bk2[j][i] = new TH1F( ("h_Bmumu_mu1_eta_bk2_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu1 eta", 50, -2.5, 2.5 );
            h_Bmumu_mu2_pt_sg[j][i]   = new TH1F( ("h_Bmumu_mu2_pt_sg_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu2 pt", 40, 0, 20 );
            h_Bmumu_mu2_pt_bk1[j][i]  = new TH1F( ("h_Bmumu_mu2_pt_bk1_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu2 pt", 40, 0, 20 );
            h_Bmumu_mu2_pt_bk2[j][i]  = new TH1F( ("h_Bmumu_mu2_pt_bk2_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu2 pt", 40, 0, 20 );
            h_Bmumu_mu2_eta_sg[j][i]  = new TH1F( ("h_Bmumu_mu2_eta_sg_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu2 eta", 50, -2.5, 2.5 );
            h_Bmumu_mu2_eta_bk1[j][i] = new TH1F( ("h_Bmumu_mu2_eta_bk1_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu2 eta", 50, -2.5, 2.5 );
            h_Bmumu_mu2_eta_bk2[j][i] = new TH1F( ("h_Bmumu_mu2_eta_bk2_"+rStr_demo.str()+"_"+rStr.str()+""+sStr.str()+"").c_str(), "mu2 eta", 50, -2.5, 2.5 );

            h_Bmumu_mass[j][i] -> Sumw2();
            h_Bmumu_mass_sg[j][i] -> Sumw2();
            h_Bmumu_mass_bk1[j][i] -> Sumw2();
            h_Bmumu_mass_bk2[j][i] -> Sumw2();
            h_Bmumu_mu1_pt_sg[j][i] -> Sumw2();
            h_Bmumu_mu1_pt_bk1[j][i] -> Sumw2();
            h_Bmumu_mu1_pt_bk2[j][i] -> Sumw2();
            h_Bmumu_mu1_eta_sg[j][i] -> Sumw2();
            h_Bmumu_mu1_eta_bk1[j][i] -> Sumw2();
            h_Bmumu_mu1_eta_bk2[j][i] -> Sumw2();
            h_Bmumu_mu2_pt_sg[j][i] -> Sumw2();
            h_Bmumu_mu2_pt_bk1[j][i] -> Sumw2();
            h_Bmumu_mu2_pt_bk2[j][i] -> Sumw2();
            h_Bmumu_mu2_eta_sg[j][i] -> Sumw2();
            h_Bmumu_mu2_eta_bk1[j][i] -> Sumw2();
            h_Bmumu_mu2_eta_bk2[j][i] -> Sumw2();
        }
    }
    // ===============================================================================================
    // Load GRL
    // ===============================================================================================
    // GRLs : if no GRL package comments out 8 lines        
    Root::TGoodRunsListReader* grlR_2011;
    Root::TGoodRunsList grl_2011;
    grlR_2011 = new Root::TGoodRunsListReader();
    string sname_2011 = grl_file;
    cout << "XML to load: " << sname_2011.c_str() << endl;
    grlR_2011->SetXMLFile(sname_2011.c_str());
    grlR_2011->Interpret();
    grl_2011 = grlR_2011->GetMergedGoodRunsList();
    // End of loading GRL


    // MAIN LOOP OVER INPUT FILES
    std::vector<TString> fileNames = getFileNames();
    int end_nfile = fileNames.size() - 1;

    int tot_file = 0;
    for ( int nfile = 0; nfile < end_nfile; nfile++  ) {
        tot_file ++;
        cout << " File #"<< tot_file << endl;
        // Open Input File
        TString fileName = fileNames.at( nfile ) ;
        TFile *f = TFile::Open(fileName);
        // Get Trees
        
        TTree* STACOMUON  = (TTree*)f->Get("STACOMUON");
        TTree* SEL_TRACKS = (TTree*)f->Get("SEL_TRACKS");
        TTree* PRIVX      = (TTree*)f->Get("PRIVX");
        TTree* JPSI       = (TTree*)f->Get("JPSI");
        TTree* TRIG       = (TTree*)f->Get("TRIG");
        TTree* TRIGOBJ    = (TTree*)f->Get("TRIGOBJ");
        TTree* ETMISS     = (TTree*)f->Get("ETMISS");
        
        // AddFriend All Trees
        JPSI->AddFriend(STACOMUON);
        JPSI->AddFriend(SEL_TRACKS);
        JPSI->AddFriend(PRIVX);
        JPSI->AddFriend(TRIG);
        JPSI->AddFriend(TRIGOBJ);
        JPSI->AddFriend(ETMISS);

        // ===================================================
        // Set Branch
        // ===================================================
        // Onia
        int onia_evtNum = 0; 
        int onia_runNum = 0; 
        int onia_lbNum = 0; 
        std::vector<double> *onia_mass(0);
        std::vector<double> *onia_massErr(0);
        std::vector<double> *onia_px(0);
        std::vector<double> *onia_py(0);
        std::vector<double> *onia_pz(0);
        std::vector<double> *onia_rap(0);
        std::vector<double> *onia_chi2(0);
        std::vector<int> *onia_ndf(0);
        std::vector<int> *onia_ch(0);
        std::vector< std::vector<int> > *onia_mu_ind(0);
        std::vector< std::vector<int> > *onia_trk_ind(0);
        JPSI->SetBranchAddress( "eventNumber",   &onia_evtNum);
        JPSI->SetBranchAddress( "runNumber",     &onia_runNum);
        JPSI->SetBranchAddress( "lumiBlock",     &onia_lbNum);
        JPSI->SetBranchAddress( "VTX_mass",      &onia_mass );
        JPSI->SetBranchAddress( "VTX_massError", &onia_massErr );
        JPSI->SetBranchAddress( "VTX_px",        &onia_px );
        JPSI->SetBranchAddress( "VTX_py",        &onia_py );
        JPSI->SetBranchAddress( "VTX_pz",        &onia_pz );
        JPSI->SetBranchAddress( "VTX_rapidity",  &onia_rap );
        JPSI->SetBranchAddress( "VTX_chi2",      &onia_chi2 );
        JPSI->SetBranchAddress( "VTX_NDF",       &onia_ndf );
        JPSI->SetBranchAddress( "charge",        &onia_ch );
        JPSI->SetBranchAddress( "MUONS_index",   &onia_mu_ind );
        JPSI->SetBranchAddress( "TRKS_index",    &onia_trk_ind );

        // Muon
        int mu_evtNum = 0; 
        std::vector<double> *mu_px(0);
        std::vector<double> *mu_py(0);
        std::vector<double> *mu_pz(0);
        std::vector<double> *mu_pt(0);
        std::vector<double> *mu_eta(0);
        std::vector<double> *mu_phi(0);
        std::vector<int>    *mu_ch(0);
        std::vector<bool>   *mu_comb(0);
        std::vector<int>    *mu_ind(0); 
        std::vector<int>    *mu_pix(0); 
        std::vector<int>    *mu_sct(0); 
        std::vector<int>    *mu_trt(0); 
        std::vector<double> *mu_etcone20(0); 
        std::vector<double> *mu_etcone40(0);
        std::vector<double> *mu_ptcone20(0); 
        std::vector<double> *mu_ptcone40(0);
        JPSI->SetBranchAddress("STACOMUON_eventNumber", &mu_evtNum);
        JPSI->SetBranchAddress("STACOMUON_MU_px", &mu_px);
        JPSI->SetBranchAddress("STACOMUON_MU_py", &mu_py);
        JPSI->SetBranchAddress("STACOMUON_MU_pz", &mu_pz);
        JPSI->SetBranchAddress("STACOMUON_MU_pt", &mu_pt);
        JPSI->SetBranchAddress("STACOMUON_MU_eta", &mu_eta);
        JPSI->SetBranchAddress("STACOMUON_MU_phi", &mu_phi);
        JPSI->SetBranchAddress("STACOMUON_MU_charge", &mu_ch);
        JPSI->SetBranchAddress("STACOMUON_MU_isCombinedMuon", &mu_comb);
        JPSI->SetBranchAddress("STACOMUON_MU_index", &mu_ind);
        JPSI->SetBranchAddress("STACOMUON_MU_numberOfPixelHits", &mu_pix);
        JPSI->SetBranchAddress("STACOMUON_MU_numberOfSCTHits", &mu_sct);
        JPSI->SetBranchAddress("STACOMUON_MU_numberOfTRTHits", &mu_trt);
        JPSI->SetBranchAddress("STACOMUON_MU_etcone20", &mu_etcone20);
        JPSI->SetBranchAddress("STACOMUON_MU_etcone40", &mu_etcone40);
        JPSI->SetBranchAddress("STACOMUON_MU_ptcone20", &mu_ptcone20);
        JPSI->SetBranchAddress("STACOMUON_MU_ptcone40", &mu_ptcone40);

        // Track
        std::vector<int>    *trk_ind(0);
        std::vector<double> *trk_pt(0);
        std::vector<double> *trk_eta(0);
        std::vector<double> *trk_phi(0);
        JPSI->SetBranchAddress("SEL_TRACKS_TRKS_index", &trk_ind);
        JPSI->SetBranchAddress("SEL_TRACKS_TRKS_pt", &trk_pt);
        JPSI->SetBranchAddress("SEL_TRACKS_TRKS_eta", &trk_eta);
        JPSI->SetBranchAddress("SEL_TRACKS_TRKS_phi0", &trk_phi);

        // Trigger
        std::vector<string>    *trig_L1_name(0); 
        std::vector<string>    *trig_L2_name(0); 
        std::vector<string>    *trig_EF_name(0); 
        JPSI->SetBranchAddress("TRIG_L1_trigger_name", &trig_L1_name);
        JPSI->SetBranchAddress("TRIG_L2_trigger_name", &trig_L2_name);
        JPSI->SetBranchAddress("TRIG_EF_trigger_name", &trig_EF_name);

        // Trigger Object
        std::vector< std::vector<string> >   *trigobj_EF_name(0); 
        std::vector<double> *trigobj_EF_pt(0);
        std::vector<double> *trigobj_EF_eta(0);
        std::vector<double> *trigobj_EF_phi(0);
        std::vector< std::vector<int> >   *trigobj_EF_combid(0); 
        JPSI->SetBranchAddress("EF_trigObj_names", &trigobj_EF_name);
        JPSI->SetBranchAddress("EF_trigObj_pt", &trigobj_EF_pt);
        JPSI->SetBranchAddress("EF_trigObj_eta", &trigobj_EF_eta);
        JPSI->SetBranchAddress("EF_trigObj_phi", &trigobj_EF_phi);
        JPSI->SetBranchAddress("EF_trigObj_combIds", &trigobj_EF_combid);

        // =================================================================================
        // Start Loop Event
        // ================================================================================= 
        int nJPSI  = (int)JPSI->GetEntries();
        for ( int njpsi = 0; njpsi < nJPSI; ++njpsi) {
            JPSI->GetEntry(njpsi);
            // count for total events

            // ============================
            // Good Run List
            // ============================
            if ( use_grl ){
                bool flag_grl = false;
                if ( grl_2011.HasRunLumiBlock( onia_runNum, onia_lbNum ) )  flag_grl  = true;
                if ( !flag_grl ) continue;   
            }

            // ============================
            // Trigger
            // ============================
            // initial the flags
            bool flag_trig_deno[NumDeno];
            for ( j = 0; j < NumDeno; j++ ){ flag_trig_deno[j] = false; }
            bool flag_trig[NumChain];
            for ( j = 0; j < NumChain; j++ ){ flag_trig[j] = false; }
            // Loop the decisions 
            // Level 1
            for ( i = 0; i < (*trig_L1_name).size(); i++ ){
                for ( j = 0; j < NumDeno; j++ ){
                    if (  (*trig_L1_name)[i] == denominators.at(j) ) { flag_trig_deno[j] = true; }
                }
                for ( j = 0; j < NumChain; j++ ){
                    if (  (*trig_L1_name)[i] == chains.at(j) ) { flag_trig[j] = true; }
                }
            }
            // Level 2
            for ( i = 0; i < (*trig_L2_name).size(); i++ ){
                for ( j = 0; j < NumDeno; j++ ){
                    if (  (*trig_L2_name)[i] == denominators.at(j) ) { flag_trig_deno[j] = true; }
                }
                for ( j = 0; j < NumChain; j++ ){
                    if (  (*trig_L2_name)[i] == chains.at(j) ) { flag_trig[j] = true; }
                }
            }
            // Event Filter
            for ( i = 0; i < (*trig_EF_name).size(); i++ ){
                 for ( j = 0; j < NumDeno; j++ ){
                    if (  (*trig_EF_name)[i] == denominators.at(j) ) { flag_trig_deno[j] = true; }
                }
                for ( j = 0; j < NumChain; j++ ){
                    if (  (*trig_EF_name)[i] == chains.at(j) ) { flag_trig[j] = true; }
                }
            }
            /*
            // ================================================
            // Loop Over Trigger Objects
            // ================================================
            // EF_2mu4_DiMu Trigger
            std::vector<double> TrigObj_EF_2mu4_DiMu_eta;
            std::vector<double> TrigObj_EF_2mu4_DiMu_phi;
            std::vector<int> TrigObj_EF_2mu4_DiMu_combid;
            TrigObj_EF_2mu4_DiMu_eta.clear();
            TrigObj_EF_2mu4_DiMu_phi.clear();
            TrigObj_EF_2mu4_DiMu_combid.clear();
            for ( j = 0; j < (*trigobj_EF_name).size(); j++ ){
                bool is_EF_2mu4_DiMu_TrigObj = false;
                int combid_EF_2mu4_DiMu = -999;
                for ( k = 0; k < (*trigobj_EF_name)[j].size(); k++){ 
                    // EF_2mu4_DiMu
                    if ( (*trigobj_EF_name)[j][k] == "EF_2mu4_DiMu" ){
                        is_EF_2mu4_DiMu_TrigObj = true;
                        combid_EF_2mu4_DiMu = (*trigobj_EF_combid)[j][k];
                    }
                }
                
                if ( is_EF_2mu4_DiMu_TrigObj ){
                    TrigObj_EF_2mu4_DiMu_eta.push_back( (*trigobj_EF_eta)[j] );
                    TrigObj_EF_2mu4_DiMu_phi.push_back( (*trigobj_EF_phi)[j] );
                    TrigObj_EF_2mu4_DiMu_combid.push_back( combid_EF_2mu4_DiMu );
                }
                
            }
            */ 
            // ================================================
            // Loop over Onia candidates 
            // ================================================
            int nOnia = (*onia_mass).size();
            for ( i = 0; i < nOnia; i++ ){ 
                // count for total Onia candidates
                // Find the associated Muons and Tracks 
                int Mu1OniaID  = (*onia_mu_ind)[i][0];
                int Mu2OniaID  = (*onia_mu_ind)[i][1];
                int Trk1OniaID = (*onia_trk_ind)[i][0];
                int Trk2OniaID = (*onia_trk_ind)[i][1];
                int mu1ind  = 0;
                int mu2ind  = 0;
                int trk1ind = 0;
                int trk2ind = 0;
                bool found_mu1  = false;
                bool found_mu2  = false;
                bool found_trk1 = false;
                bool found_trk2 = false;                
                for ( j = 0; j < (*mu_ind).size(); j++){
                    if ( Mu1OniaID == (*mu_ind)[j] ){ mu1ind = j; found_mu1 = true; } 
                    if ( Mu2OniaID == (*mu_ind)[j] ){ mu2ind = j; found_mu2 = true; } 
                }
                for ( j = 0; j < (*trk_ind).size(); j++){
                    if ( Trk1OniaID == (*trk_ind)[j] ){ trk1ind = j; found_trk1 = true; } 
                    if ( Trk2OniaID == (*trk_ind)[j] ){ trk2ind = j; found_trk2 = true; } 
                }
                
                double vtx_mass = (*onia_mass)[i]/GeV;
                bool jpsi_mass  = false;
                bool upsi_mass  = false;
                bool bmumu_mass  = false;
                if ( vtx_mass >= LoM_jpsi  && vtx_mass <= HiM_jpsi  ){ jpsi_mass = true; }
                if ( vtx_mass >= LoM_upsi  && vtx_mass <= HiM_upsi  ){ upsi_mass = true; }
                if ( vtx_mass >= LoM_bmumu && vtx_mass <= HiM_bmumu ){ bmumu_mass = true; }
                // Signale
                bool jpsi_mass_sg  = false;
                bool upsi_mass_sg  = false;
                bool bmumu_mass_sg = false;
                if ( vtx_mass >= 2.9  && vtx_mass <= 3.3  ){ jpsi_mass_sg  = true; }
                if ( vtx_mass >= 9.0  && vtx_mass <= 10.0 ){ upsi_mass_sg  = true; }
                if ( vtx_mass >= 5.0  && vtx_mass <= 6.0 ){ bmumu_mass_sg = true; }
                // Sideband 1
                bool jpsi_mass_bk1  = false;
                bool upsi_mass_bk1  = false;
                bool bmumu_mass_bk1 = false;
                if ( vtx_mass >= 2.6  && vtx_mass <= 2.9  ){ jpsi_mass_bk1  = true; }
                if ( vtx_mass >= 8.5  && vtx_mass <= 9.0  ){ upsi_mass_bk1  = true; }
                if ( vtx_mass >= 4.5  && vtx_mass <= 5.0  ){ bmumu_mass_bk1 = true; }
                // Sideband 2
                bool jpsi_mass_bk2  = false;
                bool upsi_mass_bk2  = false;
                bool bmumu_mass_bk2 = false;
                if ( vtx_mass >= 3.9  && vtx_mass <= 4.2  ){ jpsi_mass_bk2  = true; }
                if ( vtx_mass >= 10.0 && vtx_mass <= 10.5 ){ upsi_mass_bk2  = true; }
                if ( vtx_mass >= 6.0 && vtx_mass <= 7.5   ){ bmumu_mass_bk2 = true; }

                // =========================================
                // Require 2 muons and 2 ID tracks
                // =========================================
                if ( found_mu1 && found_mu2 && found_trk1 && found_trk2 ){
                    // Check Number of combined muons
                    if ( NumComb == 1 ){ if ( !(*mu_comb)[mu1ind] && !(*mu_comb)[mu2ind] ) continue; }
                    if ( NumComb == 2 ){ if ( !(*mu_comb)[mu2ind] || !(*mu_comb)[mu2ind] ) continue; }
                    // Basic Cut on SCT, Pixel
                    if ( (*mu_sct)[mu1ind] < NumSCT || (*mu_sct)[mu2ind] < NumSCT ) continue;
                    if ( (*mu_pix)[mu1ind] < NumPixel || (*mu_pix)[mu2ind] < NumPixel ) continue;
                    if ( (*mu_trt)[mu1ind] < NumTRT || (*mu_trt)[mu2ind] < NumTRT ) continue;
                    // Cut on pT of muon
                    if ( (*mu_pt)[mu1ind] < Trk1_pt*GeV && (*mu_pt)[mu2ind] < Trk1_pt*GeV ) continue;
                    if ( (*mu_pt)[mu1ind] < Trk2_pt*GeV || (*mu_pt)[mu2ind] < Trk2_pt*GeV ) continue;
                    if ( fabs( (*mu_eta)[mu1ind] ) > Trk_eta || fabs( (*mu_eta)[mu2ind] ) > Trk_eta ) continue;
                    // Charg of two muons
                    if ( (*mu_ch)[mu1ind] * (*mu_ch)[mu2ind] > 0 ) continue;
                    // End of the selection cut

                    // =========================================
                    //  Fill the Tree information
                    // =========================================
                    // Get the correct ordering
                    int Mu1Ind = mu1ind;
                    int Mu2Ind = mu2ind;
                    int Trk1Ind = trk1ind;
                    int Trk2Ind = trk2ind;
                    if ( (*mu_pt)[mu1ind] < (*mu_pt)[mu2ind] ){
                        Mu1Ind = mu2ind;
                        Mu2Ind = mu1ind;
                        Trk1Ind = trk2ind;
                        Trk2Ind = trk1ind;
                    }        
                    double mu1_pt  = (*mu_pt)[Mu1Ind]/GeV;
                    double mu2_pt  = (*mu_pt)[Mu2Ind]/GeV;
                    double mu1_eta = (*mu_eta)[Mu1Ind];
                    double mu2_eta = (*mu_eta)[Mu2Ind];
                    double mu1_phi = (*mu_phi)[Mu1Ind];
                    double mu2_phi = (*mu_phi)[Mu2Ind];

                    // Start loop the triggers
                    for ( k = 0; k < NumDeno; k++ ){
                        if ( flag_trig_deno[k] == true ){
                            for ( j = 0; j < NumChain; j++ ){ 
                                if ( flag_trig[j] == true ){
                                    // J/psi
                                    if ( jpsi_mass ){
                                        h_Jpsi_mass[k][j] -> Fill( vtx_mass );
                                    }
                                    if ( jpsi_mass_sg ) {
                                        h_Jpsi_mass_sg[k][j]    -> Fill( vtx_mass );
                                        h_Jpsi_mu1_pt_sg[k][j]  -> Fill( mu1_pt );
                                        h_Jpsi_mu1_eta_sg[k][j] -> Fill( mu1_eta );
                                        h_Jpsi_mu2_pt_sg[k][j]  -> Fill( mu2_pt );
                                        h_Jpsi_mu2_eta_sg[k][j] -> Fill( mu2_eta );
                                    }
                                    if ( jpsi_mass_bk1 ) {
                                        h_Jpsi_mass_bk1[k][j]    -> Fill( vtx_mass );
                                        h_Jpsi_mu1_pt_bk1[k][j]  -> Fill( mu1_pt );
                                        h_Jpsi_mu1_eta_bk1[k][j] -> Fill( mu1_eta );
                                        h_Jpsi_mu2_pt_bk1[k][j]  -> Fill( mu2_pt );
                                        h_Jpsi_mu2_eta_bk1[k][j] -> Fill( mu2_eta );
                                    }
                                    if ( jpsi_mass_bk2 ) {
                                        h_Jpsi_mass_bk2[k][j]    -> Fill( vtx_mass );
                                        h_Jpsi_mu1_pt_bk2[k][j]  -> Fill( mu1_pt );
                                        h_Jpsi_mu1_eta_bk2[k][j] -> Fill( mu1_eta );
                                        h_Jpsi_mu2_pt_bk2[k][j]  -> Fill( mu2_pt );
                                        h_Jpsi_mu2_eta_bk2[k][j] -> Fill( mu2_eta );
                                    }
                                    // Upsilon
                                    if ( upsi_mass ){
                                        h_Upsi_mass[k][j] -> Fill( vtx_mass );
                                    }
                                    if ( upsi_mass_sg ) {
                                        h_Upsi_mass_sg[k][j]    -> Fill( vtx_mass );
                                        h_Upsi_mu1_pt_sg[k][j]  -> Fill( mu1_pt );
                                        h_Upsi_mu1_eta_sg[k][j] -> Fill( mu1_eta );
                                        h_Upsi_mu2_pt_sg[k][j]  -> Fill( mu2_pt );
                                        h_Upsi_mu2_eta_sg[k][j] -> Fill( mu2_eta );
                                    }
                                    if ( upsi_mass_bk1 ) {
                                        h_Upsi_mass_bk1[k][j]    -> Fill( vtx_mass );
                                        h_Upsi_mu1_pt_bk1[k][j]  -> Fill( mu1_pt );
                                        h_Upsi_mu1_eta_bk1[k][j] -> Fill( mu1_eta );
                                        h_Upsi_mu2_pt_bk1[k][j]  -> Fill( mu2_pt );
                                        h_Upsi_mu2_eta_bk1[k][j] -> Fill( mu2_eta );
                                    }
                                    if ( upsi_mass_bk2 ) {
                                        h_Upsi_mass_bk2[k][j]    -> Fill( vtx_mass );
                                        h_Upsi_mu1_pt_bk2[k][j]  -> Fill( mu1_pt );
                                        h_Upsi_mu1_eta_bk2[k][j] -> Fill( mu1_eta );
                                        h_Upsi_mu2_pt_bk2[k][j]  -> Fill( mu2_pt );
                                        h_Upsi_mu2_eta_bk2[k][j] -> Fill( mu2_eta );
                                    }

                                    // Bmumu
                                    if ( bmumu_mass ){
                                        h_Bmumu_mass[k][j] -> Fill( vtx_mass );
                                    }
                                    if ( bmumu_mass_sg ) {
                                        h_Bmumu_mass_sg[k][j]    -> Fill( vtx_mass );
                                        h_Bmumu_mu1_pt_sg[k][j]  -> Fill( mu1_pt );
                                        h_Bmumu_mu1_eta_sg[k][j] -> Fill( mu1_eta );
                                        h_Bmumu_mu2_pt_sg[k][j]  -> Fill( mu2_pt );
                                        h_Bmumu_mu2_eta_sg[k][j] -> Fill( mu2_eta );
                                    }
                                    if ( bmumu_mass_bk1 ) {
                                        h_Bmumu_mass_bk1[k][j]    -> Fill( vtx_mass );
                                        h_Bmumu_mu1_pt_bk1[k][j]  -> Fill( mu1_pt );
                                        h_Bmumu_mu1_eta_bk1[k][j] -> Fill( mu1_eta );
                                        h_Bmumu_mu2_pt_bk1[k][j]  -> Fill( mu2_pt );
                                        h_Bmumu_mu2_eta_bk1[k][j] -> Fill( mu2_eta );
                                    }
                                    if ( bmumu_mass_bk2 ) {
                                        h_Bmumu_mass_bk2[k][j]    -> Fill( vtx_mass );
                                        h_Bmumu_mu1_pt_bk2[k][j]  -> Fill( mu1_pt );
                                        h_Bmumu_mu1_eta_bk2[k][j] -> Fill( mu1_eta );
                                        h_Bmumu_mu2_pt_bk2[k][j]  -> Fill( mu2_pt );
                                        h_Bmumu_mu2_eta_bk2[k][j] -> Fill( mu2_eta );
                                    }

                                }// End of Trigger for numerator
                            } // End of Number looping of numerators
                        } // End of Trigger for denominator  
                    } // End of looping of denominators

                    /*
                    // -------------------------
                    // For EF_2mu4_DiMu
                    // -------------------------
                    // Match dR < 0.01
                    std::vector<int> matched_mu1_combid_0p01, matched_mu2_combid_0p01, matched_mu1_ind_0p01, matched_mu2_ind_0p01;
                    matched_mu1_combid_0p01.clear();
                    matched_mu2_combid_0p01.clear();
                    matched_mu1_ind_0p01.clear();
                    matched_mu2_ind_0p01.clear();
                    if ( flag_trig_EF_2mu4_DiMu ){
                        for ( j = 0; j < TrigObj_EF_2mu4_DiMu_eta.size(); j++ ){
                            double dR_trk1_trigobj = dR( trk1_eta, trk1_phi, TrigObj_EF_2mu4_DiMu_eta[j], TrigObj_EF_2mu4_DiMu_phi[j] );
                            double dR_trk2_trigobj = dR( trk2_eta, trk2_phi, TrigObj_EF_2mu4_DiMu_eta[j], TrigObj_EF_2mu4_DiMu_phi[j] );
                            if ( dR_trk1_trigobj < 0.01 ){
                                matched_mu1_ind_0p01.push_back( j );
                                matched_mu1_combid_0p01.push_back( TrigObj_EF_2mu4_DiMu_combid[j] );
                            }
                            if ( dR_trk2_trigobj < 0.01 ){
                                matched_mu2_ind_0p01.push_back( j );
                                matched_mu2_combid_0p01.push_back( TrigObj_EF_2mu4_DiMu_combid[j] );
                            }
                        }
                    }
                    vtx_match_EF_2mu4_DiMu_0p01 = false;
                    for ( j = 0; j < matched_mu1_ind_0p01.size(); j++ ){
                        for ( k = 0; k < matched_mu2_ind_0p01.size(); k++){
                            if ( (matched_mu1_ind_0p01[j] != matched_mu2_ind_0p01[k] ) && ( matched_mu1_combid_0p01[j] == matched_mu2_combid_0p01[k] ) ){
                                vtx_match_EF_2mu4_DiMu_0p01 = true;
                            } 
                        }
                    }
                    */

                } // End of checking two muons
            } // End of looping over Dimuon candidates
        } // End of looping over events
        JPSI->Delete();
        STACOMUON->Delete();  
        SEL_TRACKS->Delete(); 
        PRIVX->Delete();      
        TRIG->Delete();       
        TRIGOBJ->Delete();    
        ETMISS->Delete();     
    } // END OF LOOP OVER INPUT FILES

    // =============================================================================
    // Book, Fill and Write Histograms and Save the file 
    // =============================================================================
    // Output File
    TFile *outputfile = new TFile("./Histograms_MC_.root","RECREATE","output");

    for ( k = 0; k < NumDeno; k++ ){
        for ( i = 0; i < NumChain; i ++ ){
            h_Jpsi_mass[k][i]        -> Write();
            h_Jpsi_mass_sg[k][i]     -> Write();
            h_Jpsi_mass_bk1[k][i]    -> Write();
            h_Jpsi_mass_bk2[k][i]    -> Write();
            h_Jpsi_mu1_pt_sg[k][i]   -> Write();
            h_Jpsi_mu1_pt_bk1[k][i]  -> Write();
            h_Jpsi_mu1_pt_bk2[k][i]  -> Write();
            h_Jpsi_mu1_eta_sg[k][i]  -> Write();
            h_Jpsi_mu1_eta_bk1[k][i] -> Write();
            h_Jpsi_mu1_eta_bk2[k][i] -> Write();
            h_Jpsi_mu2_pt_sg[k][i]   -> Write();
            h_Jpsi_mu2_pt_bk1[k][i]  -> Write();
            h_Jpsi_mu2_pt_bk2[k][i]  -> Write();
            h_Jpsi_mu2_eta_sg[k][i]  -> Write();
            h_Jpsi_mu2_eta_bk1[k][i] -> Write();
            h_Jpsi_mu2_eta_bk2[k][i] -> Write();

            h_Upsi_mass[k][i]        -> Write();
            h_Upsi_mass_sg[k][i]     -> Write();
            h_Upsi_mass_bk1[k][i]    -> Write();
            h_Upsi_mass_bk2[k][i]    -> Write();
            h_Upsi_mu1_pt_sg[k][i]   -> Write();
            h_Upsi_mu1_pt_bk1[k][i]  -> Write();
            h_Upsi_mu1_pt_bk2[k][i]  -> Write();
            h_Upsi_mu1_eta_sg[k][i]  -> Write();
            h_Upsi_mu1_eta_bk1[k][i] -> Write();
            h_Upsi_mu1_eta_bk2[k][i] -> Write();
            h_Upsi_mu2_pt_sg[k][i]   -> Write();
            h_Upsi_mu2_pt_bk1[k][i]  -> Write();
            h_Upsi_mu2_pt_bk2[k][i]  -> Write();
            h_Upsi_mu2_eta_sg[k][i]  -> Write();
            h_Upsi_mu2_eta_bk1[k][i] -> Write();
            h_Upsi_mu2_eta_bk2[k][i] -> Write();

            h_Bmumu_mass[k][i]        -> Write();
            h_Bmumu_mass_sg[k][i]     -> Write();
            h_Bmumu_mass_bk1[k][i]    -> Write();
            h_Bmumu_mass_bk2[k][i]    -> Write();
            h_Bmumu_mu1_pt_sg[k][i]   -> Write();
            h_Bmumu_mu1_pt_bk1[k][i]  -> Write();
            h_Bmumu_mu1_pt_bk2[k][i]  -> Write();
            h_Bmumu_mu1_eta_sg[k][i]  -> Write();
            h_Bmumu_mu1_eta_bk1[k][i] -> Write();
            h_Bmumu_mu1_eta_bk2[k][i] -> Write();
            h_Bmumu_mu2_pt_sg[k][i]   -> Write();
            h_Bmumu_mu2_pt_bk1[k][i]  -> Write();
            h_Bmumu_mu2_pt_bk2[k][i]  -> Write();
            h_Bmumu_mu2_eta_sg[k][i]  -> Write();
            h_Bmumu_mu2_eta_bk1[k][i] -> Write();
            h_Bmumu_mu2_eta_bk2[k][i] -> Write();
        }
    }

    outputfile->Close();
}



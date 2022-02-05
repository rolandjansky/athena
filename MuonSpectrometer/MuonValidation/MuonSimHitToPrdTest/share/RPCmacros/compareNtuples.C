#include <iomanip>
#include <sstream>

#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"

void compareNtuples() {
    TFile* _fileRef = TFile::Open("/afs/cern.ch/work/s/stefspa//atlas/sw/rpc/samples/fixDigiTimes/rel17_reference/RPCntuple_rel17.root");
    RPCvalidNT->cd();
    TTree* tref = (TTree*)gROOT->FindObject("1");
    TFile* _fileDev = TFile::Open("/afs/cern.ch/work/s/stefspa//atlas/sw/rpc/samples/fixDigiTimes/dev19/RPCntuple.root");
    RPCvalidNT->cd();
    TTree* tdev = (TTree*)gROOT->FindObject("1");

    TH1F* h_digit_time_R = new TH1F("h_digit_time_R", "h_digit_time Reference   17", 301, -150.5, 150.5);
    TH1F* h_digit_time_shR = new TH1F("h_digit_time_shR", "h_digit_time Reference   17", 101, -20.25, 30.25);
    h_digit_time_R->SetLineColor(kBlack);
    h_digit_time_R->SetLineStyle(2);
    h_digit_time_shR->SetLineColor(kBlack);
    h_digit_time_shR->SetLineStyle(2);
    TH1F* h_digit_time_D = new TH1F("h_digit_time_D", "h_digit_time Development 19", 301, -150.5, 150.5);
    TH1F* h_digit_time_shD = new TH1F("h_digit_time_shD", "h_digit_time Development 19", 101, -20.25, 30.25);
    h_digit_time_D->SetLineColor(kRed);
    h_digit_time_D->SetLineStyle(1);
    h_digit_time_shD->SetLineColor(kRed);
    h_digit_time_shD->SetLineStyle(1);

    std::string myCut = "rpc_digit_measphi==0 && rpc_digit_doublphi==2";
    std::string myCutPhi = myCut + "&&rpc_digit_measphi==1";
    std::string myCutEta = myCut + "&&rpc_digit_measphi==0";

    tref->Project("h_digit_time_R", "rpc_digit_time", myCut.c_str());
    tdev->Project("h_digit_time_D", "rpc_digit_time", myCut.c_str());

    tref->Project("h_digit_time_shR", "rpc_digit_time", myCut.c_str());
    tdev->Project("h_digit_time_shD", "rpc_digit_time", myCut.c_str());

    TCanvas* c = new TCanvas("c", "rel_17 vs rel_19", 750., 600.);
    c->Divide(1, 2);

    c->cd(1);
    h_digit_time_D->Draw();
    h_digit_time_R->Draw("SAME");
    c->cd(2);
    h_digit_time_shD->Draw();
    h_digit_time_shR->Draw("SAME");

    return;
    TCanvas* c0 = new TCanvas("c0", "rel_17 vs rel_19", 750., 600.);
    c0->Divide(2, 2);

    TH2F* hXYdR = new TH2F("XYviewDigitsR", "XYviewDigits", 200, -6200., -15500., 15500., -15500., 15500.);
    TH2F* hRZdR = new TH2F("RZviewDigitsR", "RZviewDigits", 1000, -14000., 14000., 100, 6000., 15500.);
    TH2F* hXYdD = new TH2F("XYviewDigitsD", "XYviewDigits", 200, -6200., -15500., 15500., -15500., 15500.);
    TH2F* hRZdD = new TH2F("RZviewDigitsD", "RZviewDigits", 1000, -14000., 14000., 100, 6000., 15500.);

    tref->Project("XYviewDigitsR", "rpc_digit_stripy:rpc_digit_stripx", myCutPhi.c_str());
    tref->Project("RZviewDigitsR", "sqrt(rpc_digit_stripy*rpc_digit_stripy+rpc_digit_stripx*rpc_digit_stripx):rpc_digit_stripz",
                  myCutEta.c_str());
    tdev->Project("XYviewDigitsD", "rpc_digit_stripy:rpc_digit_stripx", myCutPhi.c_str());
    tdev->Project("RZviewDigitsD", "sqrt(rpc_digit_stripy*rpc_digit_stripy+rpc_digit_stripx*rpc_digit_stripx):rpc_digit_stripz",
                  myCutEta.c_str());

    c0->cd(1);
    hXYdR->Draw("BOX");
    c0->cd(2);
    hRZdR->Draw("BOX");
    c0->cd(3);
    hXYdD->Draw("BOX");
    c0->cd(4);
    hRZdD->Draw("BOX");

    return;
}

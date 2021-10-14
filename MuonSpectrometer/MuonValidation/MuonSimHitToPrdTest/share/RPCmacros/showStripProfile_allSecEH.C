#include <iomanip>
#include <sstream>

#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
//#include "AtlasUtils.h"
//#include "AtlasStyle.C"
//#include "AtlasLabels.C"

void showStripProfile_allSecEH(int file = 999, int sector = 999, int eta = 999, int stName = 999, int dbZ = 999, int dbP = 999) {
    gStyle->SetOptStat(0);
    if (file == 999) {
        std::cout << "use with .x showStripProfile_allSecEH(file, sector, StationEta, StationName, dbZ, dbP)" << std::endl;
        return;
    }

    // gROOT->LoadMacro("AtlasUtils.C");
    // SetAtlasStyle();

    gStyle->SetOptStat(111);
    // gStyle->SetOptTitle(0);

    TFile* _file;
    //  if (file==0)      _file = TFile::Open("trackChanges/RPCntuple_ConfFileName_type7_12_16_17_v006_Corr006.root");
    //  if (file==0)      _file = TFile::Open("trackChanges/RPCntuple_ConfFileName_type7_12_16_17_8_14_11_15_v003_corr009.root");
    //  if (file==0)      _file = TFile::Open("trackChanges/RPCntuple_ConfFileName_type7_12_16_17_8_14_11_15_v003_corr009.root");
    //  if (file==0)      _file = TFile::Open("trackChanges/RPCntuple_ConfFileName_type7_12_16_17_8_14_11_15_v004_corr009.root");
    //  if (file==0)      _file = TFile::Open("trackChanges/RPCntuple_ConfFileName_type7_12_16_17_8_14_11_15_v004_corr009.root");
    //  if (file==0)      _file = TFile::Open("trackChanges/RPCntuple_ConfFileName_type7_12_16_17_8_14_11_15_v004_corr009_newGEO.root");
    //  if (file==0)      _file =
    //  TFile::Open("trackChanges/RPCntuple_ConfFileName_type7_12_16_17_8_14_11_15_v004_corr009_newGEOreSim_Will.root");
    // if (file==0)        _file =
    // TFile::Open("trackChanges/RPCntuple_ConfFileName_type7_12_16_17_8_14_11_15_v005GAB_corr009_newGEOreSim_Will.root");

    //  if (file==0)        _file =
    //  TFile::Open("trackChanges/RPCntuple_ConfFileName_type7_12_16_17_8_14_11_15_v007_corr009_newGEOreSim_Will.root"); if (file==0) _file
    //  = TFile::Open("trackChanges/RPCntuple_25kFeet_conf008corr010.root");
    if (file == 0)
        _file = TFile::Open("tmp/RPCntuple_v010.root");
    else if (file == 1)
        _file = TFile::Open("trackChanges/RPCntuple_25kFeet_conf007corr009.root");
    else if (file == 10)
        _file = TFile::Open("trackChanges/RPCntuple_100kBarrel_conf008corr010.root");
    else if (file == 101)
        _file = TFile::Open("trackChanges/RPCntuple_100kBarrel_conf009corr010.root");
    else if (file == 201)
        _file = TFile::Open("trackChanges/RPCntuple_100kBarrel_conf009corr010_sqlite.root");
    else if (file == 11)
        _file = TFile::Open("trackChanges/RPCntuple_100kBarrel_conf007corr009.root");
    else if (file == 1)
        _file = TFile::Open("trackChanges/RPCntuple_17tobereleased_006.root");
    else if (file == 2)
        _file = TFile::Open("trackChanges/RPCntuple_17tobereleased_000.root");
    // else if (file==10)  _file = TFile::Open("dev/RPCntuple_run1.root");
    // else if (file==11)  _file = TFile::Open("dev/RPCntuple_run1_reference.root");
    else if (file == 12)
        _file = TFile::Open("dev/RPCntuple_run1_devrel1_29Jul.root");
    else if (file == 100)
        _file = TFile::Open("dev/RPCntuple_run2_complete_devrel1_29Jul.root");
    else if (file == 1001)
        _file = TFile::Open("/afs/cern.ch/work/s/stefspa/RPCntuple_GeantinoHits_test125k_condRunDESDM_MCP25ns_sqliteWithTimingInCOOL.root");

    TDirectory* RPCvalidNT = (TDirectory*)gROOT->FindObject("RPCvalidNT");
    RPCvalidNT->cd();
    TTree* t002 = (TTree*)gROOT->FindObject("1");

    TH1F* hphiD2 = new TH1F("phiViewD2", "phiViewD2", 80, 0.5, 80.5);
    TH1F* hphiP2 = new TH1F("phiViewP2", "phiViewP2", 80, 0.5, 80.5);

    TH1F* hetaD2 = new TH1F("etaViewD2", "etaViewD2", 80, 0.5, 80.5);
    TH1F* hetaP2 = new TH1F("etaViewP2", "etaViewP2", 80, 0.5, 80.5);

    TH1F* hphiD1 = new TH1F("phiViewD1", "phiViewD1", 80, 0.5, 80.5);
    TH1F* hphiP1 = new TH1F("phiViewP1", "phiViewP1", 80, 0.5, 80.5);

    TH1F* hetaD1 = new TH1F("etaViewD1", "etaViewD1", 80, 0.5, 80.5);
    TH1F* hetaP1 = new TH1F("etaViewP1", "etaViewP1", 80, 0.5, 80.5);

    std::ostringstream myEta;
    std::ostringstream myStName;
    std::ostringstream mydbZ;
    std::ostringstream mydbP;
    myEta << eta;
    myStName << stName;
    mydbZ << dbZ;
    mydbP << dbP;

    std::cout << " myEta    = " << myEta.str() << std::endl;
    std::cout << " myStName = " << myStName.str() << std::endl;
    std::cout << " myDbZ    = " << mydbZ.str() << std::endl;
    std::cout << " myDbP    = " << mydbP.str() << std::endl;

    std::string basicDigitCut, basicPrdCut, basicSimhitCut;
    // sector 13
    if (sector == 13) {
        basicDigitCut = "(rpc_digit_phi==7  && (rpc_digit_station!=8  && rpc_digit_station!=9  && rpc_digit_station!=10 ))";
        basicPrdCut = "(rpc_prd_phi==7    && (rpc_prd_station!=8    && rpc_prd_station!=9    && rpc_prd_station!=10   ))";
        basicSimhitCut = "(rpc_simhit_phi==7 && (rpc_simhit_station!=8 && rpc_simhit_station!=9 && rpc_simhit_station!=10))";
    } else if (sector == 14) {
        basicDigitCut = "(rpc_digit_phi==7  && (rpc_digit_station==8  || rpc_digit_station==9  || rpc_digit_station==10 ))";
        basicPrdCut = "(rpc_prd_phi==7    && (rpc_prd_station==8    || rpc_prd_station==9    || rpc_prd_station==10   ))";
        basicSimhitCut = "(rpc_simhit_phi==7 && (rpc_simhit_station==8 || rpc_simhit_station==9 || rpc_simhit_station==10))";
    } else if (sector == 12) {
        basicDigitCut = "(rpc_digit_phi==6  && (rpc_digit_station==8  || rpc_digit_station==9  || rpc_digit_station==10 ))";
        basicPrdCut = "(rpc_prd_phi==6    && (rpc_prd_station==8    || rpc_prd_station==9    || rpc_prd_station==10   ))";
        basicSimhitCut = "(rpc_simhit_phi==6 && (rpc_simhit_station==8 || rpc_simhit_station==9 || rpc_simhit_station==10))";
    } else {
    }

    if (eta != 999) basicDigitCut = basicDigitCut + "&&rpc_digit_eta==" + myEta.str();
    if (stName != 999) basicDigitCut = basicDigitCut + "&&rpc_digit_station==" + myStName.str();
    if (dbZ != 999) basicDigitCut = basicDigitCut + "&&rpc_digit_doublz==" + mydbZ.str();
    if (dbP != 999) basicDigitCut = basicDigitCut + "&&rpc_digit_doublphi==" + mydbP.str();

    std::string cutDigitPhi = basicDigitCut + "&&rpc_digit_measphi==1";
    std::string cutDigitPhi1 = basicDigitCut + "&&rpc_digit_doublr==1&&rpc_digit_measphi==1";
    std::string cutDigitPhi2 = basicDigitCut + "&&rpc_digit_doublr==2&&rpc_digit_measphi==1";

    std::string cutDigitEta = basicDigitCut + "&&rpc_digit_measphi==0";
    std::string cutDigitEta1 = basicDigitCut + "&&rpc_digit_doublr==1&&rpc_digit_measphi==0";
    std::string cutDigitEta2 = basicDigitCut + "&&rpc_digit_doublr==2&&rpc_digit_measphi==0";

    std::cout << " Processing file = <" << _file->GetName() << ">" << std::endl;
    std::cout << " Basic CUT = <" << basicDigitCut << ">" << std::endl;
    //  std::cout<<" MY CUT = <"<<cutDigitPhi1<<">"<<std::endl;

    if (eta != 999) basicPrdCut = basicPrdCut + "&&rpc_prd_eta==" + myEta.str();
    if (stName != 999) basicPrdCut = basicPrdCut + "&&rpc_prd_station==" + myStName.str();
    if (dbZ != 999) basicPrdCut = basicPrdCut + "&&rpc_prd_doublz==" + mydbZ.str();
    if (dbP != 999) basicPrdCut = basicPrdCut + "&&rpc_prd_doublphi==" + mydbP.str();

    std::string cutPrdPhi = basicPrdCut + "&&rpc_prd_measphi==1";
    std::string cutPrdPhi1 = basicPrdCut + "&&rpc_prd_doublr==1&&rpc_prd_measphi==1";
    std::string cutPrdPhi2 = basicPrdCut + "&&rpc_prd_doublr==2&&rpc_prd_measphi==1";

    std::string cutPrdEta = basicPrdCut + "&&rpc_prd_measphi==0";
    std::string cutPrdEta1 = basicPrdCut + "&&rpc_prd_doublr==1&&rpc_prd_measphi==0";
    std::string cutPrdEta2 = basicPrdCut + "&&rpc_prd_doublr==2&&rpc_prd_measphi==0";

    //  std::string basicPrdCut = "(rpc_prd_phi==6||rpc_prd_phi==7)&&(rpc_prd_doublphi==1||rpc_prd_doublphi==2)";
    // std::string basicSimhitCut = "rpc_simhit_phi==3";
    //  std::string basicPrdCut = "(rpc_prd_phi==6||rpc_prd_phi==7)&&rpc_prd_station!=2&&rpc_prd_station!=4";
    //  std::string basicPrdCut = "(rpc_prd_phi==6&&rpc_prd_station!=2&&rpc_prd_station!=4)||rpc_prd_phi==7";
    if (eta != 999) basicSimhitCut = basicSimhitCut + "&&rpc_simhit_eta==" + myEta.str();
    if (stName != 999) basicSimhitCut = basicSimhitCut + "&&rpc_simhit_station==" + myStName.str();
    if (dbZ != 999) basicSimhitCut = basicSimhitCut + "&&rpc_simhit_doublz==" + mydbZ.str();
    if (dbP != 999) basicSimhitCut = basicSimhitCut + "&&rpc_simhit_doublphi==" + mydbP.str();

    std::string cutSimhitPhi = basicSimhitCut + "&&rpc_simhit_measphi==1";
    std::string cutSimhitPhi1 = basicSimhitCut + "&&rpc_simhit_doublr==1&&rpc_simhit_measphi==1";
    std::string cutSimhitPhi2 = basicSimhitCut + "&&rpc_simhit_doublr==2&&rpc_simhit_measphi==1";

    std::string cutSimhitEta = basicSimhitCut + "&&rpc_simhit_measphi==0";
    std::string cutSimhitEta1 = basicSimhitCut + "&&rpc_simhit_doublr==1&&rpc_simhit_measphi==0";
    std::string cutSimhitEta2 = basicSimhitCut + "&&rpc_simhit_doublr==2&&rpc_simhit_measphi==0";

    TCanvas* cBMF = new TCanvas("c1", basicPrdCut.c_str(), 800., 600.);
    //  gStyle->SetOptStat(0);

    //  TText* myTxt = new TText(10.,300.,basicDigitCut.c_str());
    //  myTxt->Draw();
    //  return;

    cBMF->Divide(2, 2);
    cBMF->cd(1);

    // t002->Print();
    // return;

    t002->Project("phiViewD1", "rpc_digit_strip", cutDigitPhi1.c_str());
    t002->Project("phiViewP1", "rpc_prd_strip", cutPrdPhi1.c_str());
    t002->Project("phiViewD2", "rpc_digit_strip", cutDigitPhi2.c_str());
    t002->Project("phiViewP2", "rpc_prd_strip", cutPrdPhi2.c_str());
    hphiD1->SetTitle("; dbR = 1 - phi View - black DIGIT - red PRD");
    hphiD2->SetTitle("; dbR = 2 - phi View - black DIGIT - red PRD");
    hphiD1->Draw();
    hphiP1->SetLineColor(2);
    hphiP1->SetLineStyle(2);
    hphiP1->Draw("SAME");
    cBMF->cd(2);
    hphiD2->Draw();
    hphiP2->SetLineColor(2);
    hphiP2->SetLineStyle(2);
    hphiP2->Draw("SAME");

    cBMF->cd(3);

    t002->Project("etaViewD1", "rpc_digit_strip", cutDigitEta1.c_str());
    t002->Project("etaViewP1", "rpc_prd_strip", cutPrdEta1.c_str());
    t002->Project("etaViewD2", "rpc_digit_strip", cutDigitEta2.c_str());
    t002->Project("etaViewP2", "rpc_prd_strip", cutPrdEta2.c_str());

    hetaD1->SetTitle("; dbR=1 - eta View - black DIGIT - red PRD");
    hetaD2->SetTitle("; dbR=2 - eta View - black DIGIT - red PRD");
    hetaD1->Draw();
    hetaP1->SetLineStyle(2);
    hetaP1->SetLineColor(2);
    hetaP1->Draw("SAME");
    cBMF->cd(4);
    hetaD2->Draw();
    hetaP2->SetLineStyle(2);
    hetaP2->SetLineColor(2);
    hetaP2->Draw("SAME");

    cBMF->SaveAs("/Users/spagnolo/Desktop/lastPict.gif");
    // return;

    TCanvas* cDigitLoc = new TCanvas("c2", basicPrdCut.c_str(), 800., 1000.);
    cDigitLoc->Divide(2, 3);

    TH2F* hXYh = new TH2F("XYviewHits", "XYviewHits", 200, -6200., 6200., 100, -15500., -4000.);
    TH2F* hXYd = new TH2F("XYviewDigits", "XYviewDigits", 200, -6200., 6200., 100, -15500., -4000.);
    TH2F* hXYp = new TH2F("XYviewPrd", "XYviewPrd", 200, -6200., 6200., 100, -15500., -4000.);
    TH2F* hRZh = new TH2F("RZviewHits", "RZviewHits", 1000, -14000., 14000., 100, 6000., 15500.);
    TH2F* hRZd = new TH2F("RZviewDigits", "RZviewDigits", 1000, -14000., 14000., 100, 6000., 15500.);
    TH2F* hRZp = new TH2F("RZviewPrd", "RZviewPrd", 1000, -14000., 14000., 100, 6000., 15500.);

    t002->Project("XYviewDigits", "rpc_digit_stripy:rpc_digit_stripx", cutDigitPhi.c_str());
    t002->Project("RZviewDigits", "sqrt(rpc_digit_stripy*rpc_digit_stripy+rpc_digit_stripx*rpc_digit_stripx):rpc_digit_stripz",
                  cutDigitEta.c_str());
    t002->Project("XYviewPrd", "rpc_prd_stripy:rpc_prd_stripx", cutPrdPhi.c_str());
    t002->Project("RZviewPrd", "sqrt(rpc_prd_stripy*rpc_prd_stripy+rpc_prd_stripx*rpc_prd_stripx):rpc_prd_stripz", cutPrdEta.c_str());
    t002->Project("XYviewHits", "rpc_simhit_globaly:rpc_simhit_globalx", cutSimhitPhi.c_str());
    t002->Project("RZviewHits", "sqrt(rpc_simhit_globaly*rpc_simhit_globaly+rpc_simhit_globalx*rpc_simhit_globalx):rpc_simhit_globalz",
                  cutSimhitEta.c_str());

    cDigitLoc->cd(1);
    hXYh->Draw("BOX");
    cDigitLoc->cd(2);
    hRZh->Draw("BOX");
    cDigitLoc->cd(3);
    hXYd->Draw("BOX");
    cDigitLoc->cd(4);
    hRZd->Draw("BOX");
    cDigitLoc->cd(5);
    hXYp->Draw("BOX");
    cDigitLoc->cd(6);
    hRZp->Draw("BOX");

    TCanvas* ccc = new TCanvas("ccc", "hits", 800., 1000.);

    hXYh->Draw();

    bool saveOut = true;
    if (saveOut) {
        std::string fn = "temporary.root";
        TFile* foutRootInp = new TFile(fn.c_str(), "RECREATE");
        hphiD2->Write("phiViewD2");
        hphiP2->Write("phiViewP2");
        hetaD2->Write("etaViewD2");
        hetaP2->Write("etaViewP2");
        hphiD1->Write("phiViewD1");
        hphiP1->Write("phiViewP1");
        hetaD1->Write("etaViewD1");
        hetaP1->Write("etaViewP1");
        foutRootInp->Close();
    }
}

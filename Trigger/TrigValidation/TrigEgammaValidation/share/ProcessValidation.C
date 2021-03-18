#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TNtuple.h"
#include "TH2F.h"
#include "TString.h"
#include "TDirectory.h"
#include "TKey.h"
#include "TPaveStats.h"
#include "TLine.h"
#include "TROOT.h"
#include "TStyle.h"

#include <fstream>

#include <vector>
#include <string>
#include <map>
#include <iostream>

using namespace std;

char * eff3hists[][4] = {
    { "Efficiencies/TruthL1Et", "Efficiencies/TruthL1Eta", "Efficiencies/TruthL1Phi", "L1TruthEfficiencies" },
    
    { "Efficiencies/TruthL2Et", "Efficiencies/TruthL2Eta", "Efficiencies/TruthL2Phi", "L2TruthEfficiencies" },

    { "Efficiencies/TruthL2IDScanEt", "Efficiencies/TruthL2IDScanEta", "Efficiencies/TruthL2IDScanPhi", "L2IDScanTruthEfficiencies" },
    { "Efficiencies/TruthL2IDSiEt", "Efficiencies/TruthL2IDSiEta", "Efficiencies/TruthL2IDSiPhi", "L2IDSiTruthEfficiencies" },

    { "Efficiencies/TruthEFScanCaloEt", "Efficiencies/TruthEFScanCaloEta", "Efficiencies/TruthEFScanCaloPhi", "EFScanCaloTruthEfficiencies" },
    { "Efficiencies/TruthEFSiCaloEt", "Efficiencies/TruthEFSiCaloEta", "Efficiencies/TruthEFSiCaloPhi", "EFSiCaloTruthEfficiencies" },

    { "Efficiencies/TruthEFScanTrackEt", "Efficiencies/TruthEFScanTrackEta", "Efficiencies/TruthEFScanTrackPhi", "EFScanTrackTruthEfficiencies" },
    { "Efficiencies/TruthEFSiTrackEt", "Efficiencies/TruthEFSiTrackEta", "Efficiencies/TruthEFSiTrackPhi", "EFSiTrackTruthEfficiencies" },
    
    { "OfflineEfficiencies/OffL1Et", "OfflineEfficiencies/OffL1Eta", "OfflineEfficiencies/OffL1Phi", "L1OffEfficiencies" },
    
    { "OfflineEfficiencies/OffL2Et", "OfflineEfficiencies/OffL2Eta", "OfflineEfficiencies/OffL2Phi", "L2OffEfficiencies" },

    { "OfflineEfficiencies/OffL2IDScanEt", "OfflineEfficiencies/OffL2IDScanEta", "OfflineEfficiencies/OffL2IDScanPhi", "L2IDScanOffEfficiencies" },
    { "OfflineEfficiencies/OffL2IDSiEt", "OfflineEfficiencies/OffL2IDSiEta", "OfflineEfficiencies/OffL2IDSiPhi", "L2IDSiOffEfficiencies" },

    { "OfflineEfficiencies/OffEFScanCaloEt", "OfflineEfficiencies/OffEFScanCaloEta", "OfflineEfficiencies/OffEFScanCaloPhi", "EFScanCaloOffEfficiencies" },
    { "OfflineEfficiencies/OffEFSiCaloEt", "OfflineEfficiencies/OffEFSiCaloEta", "OfflineEfficiencies/OffEFSiCaloPhi", "EFSiCaloOffEfficiencies" },

    { "OfflineEfficiencies/OffEFScanTrackEt", "OfflineEfficiencies/OffEFScanTrackEta", "OfflineEfficiencies/OffEFScanTrackPhi", "EFScanTrackOffEfficiencies" },
    { "OfflineEfficiencies/OffEFSiTrackEt", "OfflineEfficiencies/OffEFSiTrackEta", "OfflineEfficiencies/OffEFSiTrackPhi", "EFSiTrackOffEfficiencies" },
    
    // JetValidation
    { "Rates/L1Et", "Rates/L1Eta", "Rates/L1Phi", "L1Rates" },
    
    { "Rates/L2Et", "Rates/L2Eta", "Rates/L2Phi", "L2Rates" },

    { "Rates/L2IDScanEt", "Rates/L2IDScanEta", "Rates/L2IDScanPhi", "L2IDScanRates" },
    { "Rates/L2IDSiEt", "Rates/L2IDSiEta", "Rates/L2IDSiPhi", "L2IDSiRates" },

    { "Rates/EFScanCaloEt", "Rates/EFScanCaloEta", "Rates/EFScanCaloPhi", "EFScanCaloRates" },
    { "Rates/EFSiCaloEt", "Rates/EFSiCaloEta", "Rates/EFSiCaloPhi", "EFSiCaloRates" },

    { "Rates/EFScanTrackEt", "Rates/EFScanTrackEta", "Rates/EFScanTrackPhi", "EFScanTrackRates" },
    { "Rates/EFSiTrackEt", "Rates/EFSiTrackEta", "Rates/EFSiTrackPhi", "EFSiTrackRates" },
    
    { "", "", "", ""}
};

const TString RefBasePath = "/afs/cern.ch/user/d/dferreir/public/references_new_dataset";

TString RefPathDefault[] = {TString("TrigEgammaValidation_RTT_testOptions_e/reference.root"), TString("TrigEgammaValidation_RTT_testOptions_j/reference.root"), TString("TrigEgammaValidation_RTT_testOptions_se/reference.root"), TString("TrigEgammaValidation_RTT_testOptions_h/reference.root"), TString("TrigEgammaValidation_RTT_testOptions_off_e/reference.root"), TString("TrigEgammaValidation_RTT_testOptions_off_j/reference.root"), TString("TrigEgammaValidation_RTT_testOptions_off_se/reference.root"), TString("TrigEgammaValidation_RTT_testOptions_off_h/reference.root"), TString("TrigEgammaValidation_RTT_testOptions_jpsi/reference.root") };

TString FilePathDefault [] = { TString("Validation_e.root"), TString("Validation_j.root"), TString("Validation_se.root"), TString("Validation_h.root"), TString("Validation_off_e.root"), TString("Validation_off_j.root"), TString("Validation_off_se.root"), TString("Validation_off_h.root"), TString("Validation_jpsi.root") };

TString RootBasePathDefault [] = { TString("ZeeValidation"), TString("JetValidation"), TString("SingleElectronValidation"), TString("HggValidation"), TString("ZeeOfflineValidation"), TString("JetValidation"), TString("SingleElectronOfflineValidation"), TString("HggOfflineValidation"), TString("JpsiValidation") };

#define REF_ARRAY_SIZE (sizeof(FilePathDefault)/sizeof(TString))

// Draws and saves some histograms
void DrawCutHists(TH1F *tmp, TH1F *tmp_ref, TNtuple *tCuts, const TString &Prefix);
void DrawAllHists(TDirectory *dir, TFile *ref, TNtuple *tCuts, const TString &Prefix);
void DrawTH2(TH2F *tmp, const TString &Prefix);
void ProcessValidationNew(const TString &fil, const TString &ref);
void GeneratePlots(TFile *File, TFile *Ref);

// Entry point
void ProcessValidation(TString *FilePath = FilePathDefault, TString *RefPath = RefPathDefault)
{
  gStyle->SetOptStat(111111);

  TFile *File = 0;
  unsigned int FileK;
  for (FileK = 0; FileK < REF_ARRAY_SIZE; FileK++) {
    File = TFile::Open(FilePath[FileK]);
    if (File)
      if (!File->IsZombie())
        break;
  }

  TString TheRefPath = RefBasePath;
  TheRefPath += "/";
  TheRefPath += RefPath[FileK];

  TFile * Ref = TFile::Open(TheRefPath);

  if (!File) {
    cout << "It wasn't possible to open any of the histogram files!!!" << endl;
    return;
  }

  File->Close();
  File = (TFile *) 0;
  File = TFile::Open(FilePath[FileK], "UPDATE");
  if (!File) {
    cout << "It wasn't possible to reopen the file " << FilePath[FileK] << endl;
    return;
  }
  
  GeneratePlots(File, Ref);
  
  delete File;
  delete Ref;
  cout << "End of macro." << endl;
}

void ProcessValidationNew(const TString &fil, const TString &ref) {
  TFile *f = TFile::Open(fil, "UPDATE");
  TFile *r = TFile::Open(ref);

  GeneratePlots(f, r);

  delete f;
  delete r;
  cout << "End of macro." << endl;
}

void GeneratePlots(TFile *File, TFile *Ref) {

  TIter next(File->GetListOfKeys());
  TKey *key;

  while ((key = (TKey *) next())) {
    if (std::string(key->GetClassName()).find("TDirectory", 0) != std::string::npos) {
      TString BasePath = key->GetName();
      TString Prefix = BasePath;
      Prefix += "_";

      TString DumpPrefix = Prefix;
      DumpPrefix += "Dump_";
      TNtuple *tCuts = (TNtuple *) File->Get(BasePath+"/Dump/CutTable");
      if (File->cd(BasePath+"/Dump")) {
        DrawAllHists(gDirectory, Ref, tCuts, DumpPrefix);
      } else {
        cout << "Couldn't cd to " << BasePath << "/Dump. Dump histograms won't be generated." << endl;
      }

      TString ZPrefix = Prefix;
      ZPrefix += "ZReconstruction_";
      if (File->cd(BasePath+"/ZReconstruction")) {
        DrawAllHists(gDirectory, Ref, 0, ZPrefix);
      } else {
        cout << "Couldn't cd to " << BasePath << "/ZReconstruction. ZReconstruction histograms won't be generated." << endl;
      }
      
      TString ResPrefix = Prefix;
      ResPrefix += "Resolution_";
      if (File->cd(BasePath+"/Resolution")) {
        DrawAllHists(gDirectory, 0, 0, ResPrefix);
      } else {
        cout << "No " << BasePath << "/Resolution directory. Not drawing resolution plots." << endl;
      }
      
      // Draws "3 in 1" plots for efficiencies
      Double_t deltaY;
      Double_t deltaX;
      TH1F *EtHist, *EtaHist, *PhiHist;
      TPaveStats *pavs1, *pavs2, *pavs3;
      TCanvas *c1 = new TCanvas("c1", "Canvas 1", 800, 600);

      c1->Divide(1,2); // 2 rows canvas!
      // Divides TPad c1_1 in 2 pads.
      TPad *topPad = (TPad *) c1->cd(1);
      topPad->Divide(2,1);

      for (int k = 0; std::string(eff3hists[k][0]) != ""; k++) {
        TString EtP = TString(BasePath);
        EtP += "/";
        EtP += eff3hists[k][0];
        TString EtaP = TString(BasePath);
        EtaP += "/";
        EtaP += eff3hists[k][1];
        TString PhiP = TString(BasePath);
        PhiP += "/";
        PhiP += eff3hists[k][2];

        EtHist = (TH1F *) File->Get(EtP);
        EtaHist = (TH1F *) File->Get(EtaP);
        PhiHist = (TH1F *) File->Get(PhiP);

        if (EtHist && EtaHist && PhiHist) {
          topPad->cd(1);
          //EtaHist->SetMaximum(1.3);
          //EtaHist->SetMinimum(0);
          EtaHist->Draw("E1");
          c1->Update();
          pavs1 = (TPaveStats *) EtaHist->FindObject("stats");
          deltaY = fabs(pavs1->GetY2NDC() - pavs1->GetY1NDC());
          pavs1->SetY2NDC(1);
          pavs1->SetY1NDC(1-deltaY);
          deltaX = fabs(pavs1->GetX2NDC() - pavs1->GetX1NDC());
          pavs1->SetX2NDC(1);
          pavs1->SetX1NDC(1-deltaX);
          
          topPad->cd(2);
          //PhiHist->SetMaximum(1.3);
          //PhiHist->SetMinimum(0);
          PhiHist->Draw("E1");
          c1->Update();
          pavs2 = (TPaveStats *) PhiHist->FindObject("stats");
          deltaY = fabs(pavs2->GetY2NDC() - pavs2->GetY1NDC());
          pavs2->SetY2NDC(1);
          pavs2->SetY1NDC(1-deltaY);
          deltaX = fabs(pavs2->GetX2NDC() - pavs2->GetX1NDC());
          pavs2->SetX2NDC(1);
          pavs2->SetX1NDC(1-deltaX);
          
          c1->cd(2);
          //EtHist->SetMaximum(1.3);
          //EtHist->SetMinimum(0);
          EtHist->Draw("E1");
          c1->Update();
          pavs3 = (TPaveStats *) EtHist->FindObject("stats");
          deltaY = fabs(pavs3->GetY2NDC() - pavs3->GetY1NDC());
          pavs3->SetY2NDC(1);
          pavs3->SetY1NDC(1-deltaY);
          deltaX = fabs(pavs3->GetX2NDC() - pavs3->GetX1NDC());
          pavs3->SetX2NDC(1);
          pavs3->SetX1NDC(1-deltaX);
          
          c1->Update();
          
          TString FileNameGif = Prefix;
          FileNameGif += eff3hists[k][3];
          FileNameGif += TString(".gif");
      
          c1->SaveAs(FileNameGif);

          TString FileNameEps = Prefix;
          FileNameEps += eff3hists[k][3];
          FileNameEps += TString(".eps");
      
          c1->SaveAs(FileNameEps);
        }
      }
      delete c1;
      cout << "Canvas deleted." << endl;

      // Generate Tables

      TString tabledir = TString(BasePath);
      tabledir += "/Dump/EffTable";
      float sL,Total,L1,L2,L2IDScan,EFScanCalo,L2IDSi,EFSiCalo,EFScanTrack,EFSiTrack;
      TNtuple *ntable = (TNtuple *) File->Get(tabledir);
      if (ntable == 0) {
        cout << "Error extracting info for the table " << tabledir << " from the root file!" << endl;
      } else {
        ntable->SetBranchAddress("sL", &sL);
        ntable->SetBranchAddress("Total", &Total);
        ntable->SetBranchAddress("L1", &L1);
        ntable->SetBranchAddress("L2", &L2);
        ntable->SetBranchAddress("L2IDScan", &L2IDScan);
        ntable->SetBranchAddress("EFScanCalo" , &EFScanCalo);
        ntable->SetBranchAddress("L2IDSi", &L2IDSi);
        ntable->SetBranchAddress("EFSiCalo", &EFSiCalo);
        ntable->SetBranchAddress("EFScanTrack", &EFScanTrack);
        ntable->SetBranchAddress("EFSiTrack", &EFSiTrack);
        ntable->GetEntry(0);
      
        ofstream eff(std::string(BasePath + "Eff.txt").c_str(), std::ios_base::out);
	eff << "" << endl;
        eff << "sL          " << sL << endl;
        eff << "Total       " << Total << endl;
        eff << "L1          " << L1 << endl;
        eff << "L2          " << L2 << endl;
        eff << "L2IDScan    " << L2IDScan << endl;
        eff << "L2IDSi      " << L2IDSi << endl;
        eff << "EFScanCalo  " << EFScanCalo << endl;
        eff << "EFSiCalo    " << EFSiCalo << endl;
        eff << "EFScanTrack " << EFScanTrack << endl;
        eff << "EFSiTrack   " << EFSiTrack << endl;
        eff.close();
      }
    }
  }

}

// Draw all histograms
void DrawAllHists(TDirectory *File, TFile *ref, TNtuple *tCuts, const TString &Prefix) {
  TH1F *tmp;
  TH2F *tmp2;
  TH1F *tmp_ref;
  TIter next(File->GetListOfKeys());
  TKey *key;
  TDirectory *kDir;

  while ((key = (TKey *) next())) {
    kDir = key->GetMotherDir();
    if (kDir == 0) {
      cout << "ERROR: Unexpected failure. Can't get mother directory for key." << endl;
      continue;
    }

    TString ThisPath(kDir->GetPath());
    ThisPath += "/";
    ThisPath += key->GetName();

    if (ThisPath.Contains(":")) {
      Ssiz_t loc = ThisPath.Index(":", 0);
      ThisPath.Remove(0, loc+2);
    }

    if (std::string(key->GetClassName()).find("TDirectory", 0) != std::string::npos) {
      DrawAllHists((TDirectory *) key->ReadObj(), ref, tCuts, Prefix);
      continue;
    } else if (std::string(key->GetClassName()) == std::string("TH1F")) {
      tmp = (TH1F *) key->ReadObj();
      if (ref) {
        tmp_ref = (TH1F *) ref->Get(ThisPath);
      } else {
        tmp_ref = 0;
      }
      DrawCutHists(tmp, tmp_ref, tCuts, Prefix);
    } else if (std::string(key->GetClassName()) == std::string("TH2F")) {
      tmp2 = (TH2F *) key->ReadObj();
      DrawTH2(tmp2, Prefix);
    }
  }
}

char *namesCut[][2] =
  {
    {"L2CaloRcore", "L2Rcore"},
    {"L2CaloRstrip", "L2Rstrip"},
    {"L2CaloEt", "L2Et"},
    {"L2CaloHadEt", "L2Ehad"},
    {"L1EmClus", "L1EmClus"},
    {"L1EmIsol", "L1EmIsol"},
    {"L1HadCore", "L1HadCore"},
    {"L1HadIsol", "L1HadIsol"},
    {"L2IDScanPt", "L2IDPt"},
    {"L2IDScanDphi", "L2IDDphi"},
    {"L2IDScanDeta", "L2IDDeta"},
    {"L2IDScanEP", "L2IDEP"},
    {"L2IDSiPt", "L2IDPt"},
    {"L2IDSiDphi", "L2IDDphi"},
    {"L2IDSiDeta", "L2IDDeta"},
    {"L2IDSiEP", "L2IDEP"},
    {"EFScanCaloEt", "EFCaloEt"},
    {"EFScanCaloDphi", "EFCaloDphi"},
    {"EFScanCaloDeta", "EFCaloDeta"},
    {"EFSiCaloEt", "EFCaloEt"},
    {"EFSiCaloDphi", "EFCaloDphi"},
    {"EFSiCaloDeta", "EFCaloDeta"}
  };
// size: 22
#define NAMES_CUT_SIZE 22

void DrawCutHists(TH1F *tmp, TH1F *tmp_ref, TNtuple *tCuts, const TString &Prefix) {
  float cut;
  bool success = false;

  if (tmp == 0)
    return;

  if (tCuts != 0) {
    TString nHist = TString(tmp->GetName());
    int k;
    for (k = 0; k < NAMES_CUT_SIZE; k++) {
      if (strcmp((const char *) nHist, namesCut[k][0]) == 0)
        break;
    }
    
    if (k < NAMES_CUT_SIZE) {
      tCuts->SetBranchStatus("*", 0);
      tCuts->SetBranchStatus(namesCut[k][1], 1);
      tCuts->SetBranchAddress(namesCut[k][1], &cut);
      tCuts->GetEntry(0);
      tCuts->SetBranchStatus("*", 0);
      success = true;
    }
  }
  
  TString FileNameGif = Prefix;
  FileNameGif += TString(tmp->GetName());
  FileNameGif += TString(".gif");
  TString FileNameEps = Prefix;
  FileNameEps += TString(tmp->GetName());
  FileNameEps += TString(".eps");
  
  TCanvas *c1;
  TLine *li = 0;
  
  c1 = new TCanvas("c1", "GIF Canvas", 400, 300);

  if (tmp_ref != 0) {  
    tmp_ref->SetLineColor(kRed);
    tmp_ref->SetStats(0);
    tmp_ref->SetLineStyle(2);
    tmp_ref->Draw("HIST");
    c1->Update();
  }

  tmp->SetStats(1);
  tmp->SetLineColor(kBlue);
  tmp->SetLineWidth(2);
  tmp->Draw("HIST SAMES");

  c1->Update();

  if (success) {
    li = new TLine(cut, c1->GetUymin(), cut, c1->GetUymax());
    
    li->SetLineStyle(2);
    li->SetLineColor(kGreen);
    li->SetLineWidth(2);
    li->Draw();
  }
  
  c1->SetGrid();
  c1->Update();
  
  TPaveStats *pavs = (TPaveStats *) tmp->FindObject("stats");
  Double_t deltaX = fabs(pavs->GetX2NDC() - pavs->GetX1NDC())/2;
  pavs->SetX1NDC(0.7-deltaX);
  pavs->SetX2NDC(0.7+deltaX);
  Double_t deltaY = fabs(pavs->GetY2NDC() - pavs->GetY1NDC());
  pavs->SetY2NDC(1);
  pavs->SetY1NDC(1-deltaY);
  
  c1->Update();
  c1->SaveAs(FileNameGif);
  c1->SaveAs(FileNameEps);
  
  if (success)
    delete li;
  
  delete c1;
}

void DrawTH2(TH2F *tmp, const TString &Prefix) {
  if (tmp == 0)
    return;

  TString FileNameGif = Prefix;
  FileNameGif += TString(tmp->GetName());
  FileNameGif += TString(".gif");
  TString FileNameEps = Prefix;
  FileNameEps += TString(tmp->GetName());
  FileNameEps += TString(".eps");
  
  TCanvas *c1;
  
  c1 = new TCanvas("c1", "GIF Canvas", 400, 300);

  //tmp->SetStats(1);
  //tmp->SetLineColor(kBlue);
  //tmp->SetLineWidth(2);
  tmp->Draw("");

  c1->Update();
  c1->SaveAs(FileNameGif);
  c1->SaveAs(FileNameEps);
  
  delete c1;
}

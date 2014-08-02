
#include "TString.h"
#include "TFile.h"

#include "TH1F.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TNtuple.h"

#include <fstream>

#include <vector>
#include <string>
#include <map>

using std::string;
using std::vector;
using std::map;

void RecurseOnGifs(TDirectory *File, TFile *Ref) {
  if (!File)
    return;

  TH1F *tmp;
  TH1F *tmp_ref;
  TIter next(File->GetListOfKeys());
  TKey *key;
  TDirectory *kDir;
  while ((key = (TKey *) next())) {
    if (std::string(key->GetClassName()).find("TDirectory", 0) != std::string::npos) {
      RecurseOnGifs((TDirectory *) key->ReadObj(), Ref);
      continue;
    } else if (std::string(key->GetClassName()) != std::string("TH1F")) { // TODO: Isn't comparing other TH1's!
      continue;
    }

    tmp_ref = 0;
    tmp = (TH1F *) key->ReadObj();
    if ( tmp == 0 ) {
      cout << "ERROR: It wasn't possible to get histogram " << (const char *) key->GetName() << endl;
      continue;
    }

    kDir = key->GetMotherDir();
    if (kDir == 0) {
      std::cout << "WARNING: Unexpected failure. Can't get mother directory for key." << std::endl;
      continue;
    }
    TString ThisPath(kDir->GetPath());
    ThisPath += "/";
    ThisPath += key->GetName();

    if (ThisPath.Contains(":")) {
      Ssiz_t loc = ThisPath.Index(":", 0);
      ThisPath.Remove(0, loc+2);
    }

    if (Ref)
      tmp_ref = (TH1F *) Ref->Get(ThisPath);
    
    TString FileName = TString(ThisPath);
    FileName.ReplaceAll("/", "_");

    TString FileNameEps = TString(FileName);
    FileNameEps += TString(".eps");
    TString FileNameGif = TString(FileName);
    FileNameGif += TString(".gif");

    TCanvas *c1;

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

    delete c1;
  }
}

void MakeGifs(TString FilePath = "expert-monitoring.root", TString RefPath = "reference-expert-monitoring.root") {

  bool success = false;
  gStyle->SetOptStat(111111);

  TFile *file = TFile::Open(FilePath);
  if (!file)
    return;

  TFile *ref = TFile::Open(RefPath);
  if (!ref)
    std::cout << "Reference file " << RefPath << " is not available!" << std::endl;

  RecurseOnGifs(file, ref);
}


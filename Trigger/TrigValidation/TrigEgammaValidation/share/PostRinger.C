#include <iostream>
#include <string>
#include <fstream>

#include "TH1F.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TKey.h"

using namespace std;

void searchDirectories(TDirectory *File, TDirectory *Ref) {
  TIter next(File->GetListOfKeys());
  TKey *key;
  TDirectory *motherDir;
  TH1F *histo;

  while (key = (TKey *) next()) {
    TString keyName = key->GetName();
    if (string (key->GetClassName()).find("TDirectory",0) != string::npos) {
      searchDirectories((TDirectory *) key->ReadObj(), Ref);
      continue;
    } else if (string (key->GetClassName()) != string("TH1F"))
      continue;

    histo = (TH1F *) key->ReadObj();
    if (histo->GetEntries() != 0) {
      motherDir = key->GetMotherDir();
      if (!motherDir) {
        cout << "An unexpected error ocurred. Cannot get histogram's mother directory" << endl;
        continue;
      }

      TString filePath = motherDir->GetPath();
      if (filePath.Contains(":")) {
        Ssiz_t loc = filePath.Index(":", 0);
        filePath.Remove(0, loc + 2);
      }
      TString refPath = filePath + keyName;

      Ssiz_t loc = filePath.Index('/', 0);
      filePath.Remove(0, loc + 1);

      string path(filePath);
      string name;
      size_t pos1, pos2;
      int cont = 0;
      for (pos1 = 0, pos2 = path.find('/', 0); pos2!= string::npos; ) {
        name = name + path.substr(pos1, pos2) + '-';
        pos1 = pos2 + 1;
        pos2 = path.find('/', pos1+1);
        cont++;
      }
      name = name + path.substr(pos1) + '-';
      
      string keyName_str (keyName);
      name = name + keyName_str + ".gif"; 

      TCanvas *c1 = new TCanvas("c1", "GIF Canvas", 400, 300);
      TH1F *tmp_ref = (TH1F *) ref->Get(refPath.c_str());
      if (tmp_ref != 0) {  
        tmp_ref->SetLineColor(kRed);
        tmp_ref->SetStats(0);
        tmp_ref->SetLineStyle(2);
        tmp_ref->Draw("HIST");
        c1->Update();
      }
      histo->SetStats(1);
      histo->SetLineColor(kBlue);
      histo->SetLineWidth(2);
      histo->Draw("HIST SAMES");
      c1->Update();
      c1->SaveAs(name.c_str()); 

      for (int i = 0; i < cont; i++)
        gDirectory->cd("../");
    }
  }
}

void PostRinger() {
  std::ifstream infoFile = ifstream("file.txt");
  std::string file_root, file_reference;
  infoFile >> file_root;
  infoFile >> file_reference;
  infoFile.close();

  TFile *file = new TFile(file_root.c_str());
  TFile *ref = new TFile(file_reference.c_str());
  searchDirectories(file, ref);
  if (file->IsOpen())
    file->Close();
  if (ref->IsOpen())
    ref->Close();
}

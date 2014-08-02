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
#include <ss>

#include <iostream>

using std::string;
using std::vector;
using std::map;
using std::stringstream;
using std::ifstream;
using std::ofstream;

int getdir (string dir, vector<string> &files)
{
  void *dirp = gSystem->OpenDirectory(dir.c_str());
  if (dirp == NULL) {
    std::cout << "Error opening " << dir << std::endl;
    return -1;
  }
  const char *entry = gSystem->GetDirEntry(dirp);
  while (entry != 0) {
    files.push_back(string(entry));
    entry = gSystem->GetDirEntry(dirp);
  }
  return 0;
}


// Entry point
void PerfMonProcessing(TString FileOutPath = "out.perfmon.easy.txt")
{
  // Comment out! For debugging purposes.
  //gDebug = 1;

  gStyle->SetOptStat(111111);

  vector<string> files;
  if (getdir(string("."), files) != 0)
    return;

  TString FilePath;
  TString FileSummaryPath;
  for (unsigned i = 0; i < files.size(); ++i) {
    string s = files[i];
    if (s.find(".perfmon.root") != string::npos) {
      FilePath = s.c_str();
    }
    if (s.find(".perfmon.summary.txt") != string::npos) {
      FileSummaryPath = s.c_str();
    }
  }
  
  TFile *File = (TFile *) 0;
  cout << "Openning " << FilePath << endl;
  File = TFile::Open(FilePath, "UPDATE");
  if (!File) {
    cout << "It wasn't possible to open the file " << FilePath << endl;
    return;
  }

  // Processing
  PlotHists(File);

  delete File;

  cout << "File closed." << endl;

  cout << "Processing summary ... " << endl;
  ifstream summary(FileSummaryPath);
  ofstream easyData(FileOutPath, std::ios_base::trunc);

  stringstream line;
  string firstWord, angCoefUnit, linCoefUnit;
  double angCoef, linCoef;
  while (summary.good()) {
    summary.get(*line.rdbuf(), '\n');
    summary >> firstWord;
    if (firstWord == "[vmem-fit]") {
      summary >> linCoef >> linCoefUnit >> angCoef >> angCoefUnit;
      easyData << linCoef << endl << linCoefUnit << endl << angCoef << endl << angCoefUnit << endl; 
      break; // That's all we want
    }
  }
  summary.close();
  easyData.close();
  cout << "Done." << endl;

}

void PlotHists(TDirectory *File) {
  if (File == 0)
    return;

  TH1F *tmp;
  TIter next(File->GetListOfKeys());
  TKey *key;
  while ((key = (TKey *) next())) {
    if (std::string(key->GetClassName()).find("TDirectory", 0) != std::string::npos) {
      PlotHists((TDirectory *) key->ReadObj());
      continue;
    } else if (std::string(key->GetClassName()) != std::string("TH1F")) {
      continue;
    }

    tmp = (TH1F *) key->ReadObj();
    if ( tmp == 0 ) {
      cout << "ERROR: It wasn't possible to get histogram " << (char *) key->GetName() << endl;
      cout << "       Not plotting this histogram." << endl;
      continue;
    }
    
    TString FileName = TString(tmp->GetName());
    FileName += ".gif";
    TCanvas *c1;
    c1 = new TCanvas("c1", "GIF Canvas", 400, 300);
    tmp->SetStats(1);
    tmp->SetLineColor(kBlue);
    tmp->SetLineWidth(2);
    tmp->Draw("HIST");
    c1->Update();
    c1->SetGrid();
    c1->Update();
    c1->SaveAs(FileName);
    delete c1;
  }
}

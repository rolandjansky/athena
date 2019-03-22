/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetCalibTools/JetCalibrationToolBase.h"
#include "JetCalibTools/JetCalibUtils.h"

#include <cstdlib>
#include "TMath.h"
#include "TObjArray.h"
#include "TSystem.h"

namespace JetCalibUtils {

  StrV Vectorize(TString str, TString sep)
  {
    StrV result; 
    TObjArray* tokens = str.Tokenize(sep);
    TIter istr(tokens);
    while (TObjString* os=(TObjString*)istr())
      result.push_back(os->GetString());
    delete tokens;
    return result;
  }

  VecD VectorizeD(TString str, TString sep)
  {
    VecD result;
    TObjArray* tokens = str.Tokenize(sep);
    TIter istr(tokens);
    while (TObjString* os=(TObjString*)istr())
      result.push_back(atof(os->GetString()));
    delete tokens;
    return result;
  }

  TH1 *GetHisto(TFile *file, TString hname) {
    TH1 *h = (TH1*)file->Get(hname);
    if (h==NULL)
      printf("WARNING: Cannot access histogram \"%s\" in file %s",
	     hname.Data(),file->GetName());
    return h;
  }

  TH2 *GetHisto2(TFile *file, TString hname) {
    TH2 *h = (TH2*)file->Get(hname);
    if (h==NULL)
      printf("WARNING: Cannot access histogram \"%s\" in file %s",
	     hname.Data(),file->GetName());
    return h;
  }

  TH3 *GetHisto3(TFile *file, TString hname) {
    TH3* h = (TH3*)file->Get(hname);
    if (h==NULL)
      printf("WARNING: Cannot access histogram \"%s\" in file %s",
             hname.Data(),file->GetName());
    return h;
  }

  TTree *setTree(TTree *tree) { return tree; }
  
}

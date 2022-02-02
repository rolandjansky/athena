/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "JetCalibTools/JetCalibrationToolBase.h"
#include "JetCalibTools/JetCalibUtils.h"

#include <cstdlib>
#include "TMath.h"
#include "TObjArray.h"
#include "TSystem.h"

namespace JetCalibUtils {

  StrV Vectorize(const TString& str, TString sep)
  {
    StrV result; 
    TObjArray* tokens = str.Tokenize(sep);
    TIter istr(tokens);
    while (TObjString* os=(TObjString*)istr())
      result.push_back(os->GetString());
    delete tokens;
    return result;
  }

  VecD VectorizeD(const TString& str, TString sep)
  {
    VecD result;
    TObjArray* tokens = str.Tokenize(sep);
    TIter istr(tokens);
    while (TObjString* os=(TObjString*)istr())
      result.push_back(atof(os->GetString()));
    delete tokens;
    return result;
  }

  template<class H>
  std::unique_ptr<const H> GetHisto_impl(TFile& file, const TString& hname) {
    std::unique_ptr<H> h(static_cast<H*>(file.Get(hname)));
    if (h==nullptr) {
      std::cout << "WARNING: Cannot access histogram " << hname.Data()
                << " in file " << file.GetName() << std::endl;
    }
    else {
      h->SetDirectory(0);  // make histogram memory-resident
    }
    return h;
  }

  std::unique_ptr<const TH1> GetHisto(TFile& file, const TString& hname) {
    return GetHisto_impl<TH1>(file, hname);
  }

  std::unique_ptr<const TH2> GetHisto2(TFile& file, const TString& hname) {
    return GetHisto_impl<TH2>(file, hname);
  }

  std::unique_ptr<const TH3> GetHisto3(TFile& file, const TString& hname) {
    return GetHisto_impl<TH3>(file, hname);
  }

  TTree *setTree(TTree *tree) { return tree; }
  
}

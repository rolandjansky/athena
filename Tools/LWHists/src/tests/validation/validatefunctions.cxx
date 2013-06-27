/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "validatefunctions.h"
#include <cstdlib>

namespace HistValFunctions {

  static bool cfg_verbose=false;
  bool verbose() { return cfg_verbose; }
  void setVerbose(bool b) { cfg_verbose=b; }

  void testfailed(const std::string& testname)
  {
    std::cout<<"ERROR: HistValidator test failed: "<<testname<<std::endl;
    assert(false);//gives a stack-trace
    exit(1);
  }

  void testTypes() {
    test("sizeof(unsigned char)=1", unsigned(1), sizeof(unsigned char));
    test("sizeof(unsigned short)=2", unsigned(2), sizeof(unsigned short));
    test("sizeof(float)=4", unsigned(4), sizeof(float));
    test("sizeof(double)=8", unsigned(8), sizeof(double));
    test("sizeof(double)=sizeof(Double_t)", sizeof(double), sizeof(Double_t));
    test("sizeof(float)=sizeof(Float_t)", sizeof(float), sizeof(Float_t));
    test("sizeof(int)=sizeof(Int_t)", sizeof(int), sizeof(Int_t));
  }

  //Float comparisons need an epsilon:
  void test(const std::string& testname, double val1, double val2)
  {
    if (!compatible(val1,val2))
      testfailed(testname,val1,val2);
  }

  //string comparisons done right:
  void test(const std::string& testname, const char* val1, const char* val2)
  {
    if (std::string(val1)!=std::string(val2))
      testfailed(testname,val1,val2);
  }

  //____________________________________________________________________
  void compareFields(TH1 * t1, TH1 * t2)
  {
    test("[TH1-level] GetXaxis()->GetLabelSize()",t1->GetXaxis()->GetLabelSize(),t2->GetXaxis()->GetLabelSize());
    test("[TH1-level] GetXaxis()->GetTitle()",t1->GetXaxis()->GetTitle(),t2->GetXaxis()->GetTitle());
    test("[TH1-level] GetYaxis()->GetLabelSize()",t1->GetYaxis()->GetLabelSize(),t2->GetYaxis()->GetLabelSize());
    test("[TH1-level] GetYaxis()->GetTitle()",t1->GetYaxis()->GetTitle(),t2->GetYaxis()->GetTitle());
    test("[TH1-level] GetMarkerColor()",t1->GetMarkerColor(),t2->GetMarkerColor());
    test("[TH1-level] GetMarkerStyle()",t1->GetMarkerStyle(),t2->GetMarkerStyle());
    test("[TH1-level] GetMarkerSize()",t1->GetMarkerSize(),t2->GetMarkerSize());
    test("[TH1-level] GetMinimum()",t1->GetMinimum(),t2->GetMinimum());

    TH1_FieldsAccess * t1_fa(static_cast<TH1_FieldsAccess*>(t1));
    TH1_FieldsAccess * t2_fa(static_cast<TH1_FieldsAccess*>(t2));
    test("[TH1-level] fNcells",t1_fa->access_fNcells(), t2_fa->access_fNcells());
    test("[TH1-level] fDimension",t1_fa->access_fDimension(), t2_fa->access_fDimension());
    test("[TH1-level] fNormFactor",t1_fa->access_fNormFactor(), t2_fa->access_fNormFactor());
    test("[TH1-level] fMaximum",t1_fa->access_fMaximum(), t2_fa->access_fMaximum());
    test("[TH1-level] fMinimum",t1_fa->access_fMinimum(), t2_fa->access_fMinimum());
    test("[TH1-level] fEntries",t1_fa->access_fEntries(), t2_fa->access_fEntries());
    test("[TH1-level] fTsumw",t1_fa->access_fTsumw(), t2_fa->access_fTsumw());
    test("[TH1-level] fTsumw2",t1_fa->access_fTsumw2(), t2_fa->access_fTsumw2());
    test("[TH1-level] fTsumwx",t1_fa->access_fTsumwx(), t2_fa->access_fTsumwx());
    test("[TH1-level] fTsumwx2",t1_fa->access_fTsumwx2(), t2_fa->access_fTsumwx2());
    if (dynamic_cast<TH2*>(t1)) {
      TH2_FieldsAccess * t1_fa2(static_cast<TH2_FieldsAccess*>(t1));
      TH2_FieldsAccess * t2_fa2(static_cast<TH2_FieldsAccess*>(t2));
      test("[TH1-level] fTsumwy",t1_fa2->access_fTsumwy(), t2_fa2->access_fTsumwy());
      test("[TH1-level] fTsumwy2",t1_fa2->access_fTsumwy2(), t2_fa2->access_fTsumwy2());
      test("[TH1-level] fTsumwxy",t1_fa2->access_fTsumwxy(), t2_fa2->access_fTsumwxy());
      test("[TH1-level] fScalefactor",t1_fa2->access_fScalefactor(), t2_fa2->access_fScalefactor());
    }
  }


}

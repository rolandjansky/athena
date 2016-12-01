/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VALIDATEFUNCTIONS_H
#define VALIDATEFUNCTIONS_H

#include <string>
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TObjString.h"
#include "THashList.h"
#include "LWHists/LWHist1D.h"
#include "LWHists/LWHist2D.h"
#include "LWHists/TProfile_LW.h"
#include "LWHists/TProfile2D_LW.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <cassert>
#include <vector>
#include <utility>
#include <iomanip>

namespace HistValFunctions {

  bool verbose();
  void setVerbose(bool);

  template <class T> inline std::string toString(const T& t) { std::ostringstream s;s<<t;return s.str(); }
  template <class T1,class T2>
  inline std::string toString(const std::pair<T1,T2>& p) {
    std::ostringstream s;
    s<<"("<<toString(p.first)<<", "<<toString(p.second)<<")";
    return s.str();
  }
  template <class T>
  inline std::string toString(const std::vector<T>& v) {
    std::ostringstream s;
    s<<"[ ";
    for (unsigned i=0;i<v.size();++i)
      s<<(i==0?"":", ")<<toString(v.at(i));
    s<<" ]";
    return s.str();
  }

  inline bool floatCompat(const double& val1, const double& val2, const double& eps)
  {
    return (fabs(val1-val2)/(1.0+std::max(fabs(val1),fabs(val2)))<eps) || ((val1!=val1)&&(val2!=val2));
  }

  inline bool compatible(const double& val1, const double& val2)
  {
    return floatCompat(val1,val2,1.0e-5);
  }

  template<class T1, class T2>
  inline bool compatible(const std::pair<T1,T2>& val1, const std::pair<T1,T2>& val2)
  {
    return compatible(val1.first,val2.first) && compatible(val1.second,val2.second);
  }

  template<class T>
  inline bool compatible(const std::vector<T>& val1, const std::vector<T>& val2)
  {
    const unsigned n(val2.size());
    if (val1.size()!=n)
      return false;
    for (unsigned i=0;i<n;++i)
      if (!compatible(val1[i],val2[i]))
        return false;
    return true;
  }

  void testfailed(const std::string& testname);

  template <class T1, class T2>
  inline void testfailed(const std::string& testname, T1 val1, T2 val2)
  {
    std::cout<<"Value in h1 ("<<typeid(T1).name()<<"): ->"<<std::setprecision(14)<<val1<<std::setprecision(6)<<"<-"<<std::endl;
    std::cout<<"Value in h2 ("<<typeid(T2).name()<<"): ->"<<std::setprecision(14)<<val2<<std::setprecision(6)<<"<-"<<std::endl;
    testfailed(testname);
  }
  template <class T1, class T2>
  inline void test(const std::string& testname, T1 val1, T2 val2)
  {
    if (val1!=val2)
      testfailed(testname,val1,val2);
  }
  void test(const std::string& testname, double val1, double val2);
  void test(const std::string& testname, const char* val1, const char* val2);

  void testTypes();

  class TH1_FieldsAccess : public TH1 {
  public:
    Int_t& access_fNcells() { return fNcells; }
    Int_t& access_fDimension() { return fDimension; }
    Double_t& access_fEntries() { return fEntries; }
    Double_t& access_fTsumw() { return fTsumw; }
    Double_t& access_fTsumw2() { return fTsumw2; }
    Double_t& access_fTsumwx() { return fTsumwx; }
    Double_t& access_fTsumwx2() { return fTsumwx2; }
    Double_t& access_fMaximum() { return fMaximum; }
    Double_t& access_fMinimum() { return fMinimum; }
    Double_t& access_fNormFactor() { return fNormFactor; }
    static void set_fgAddDirectory(bool b) { fgAddDirectory = b; }
  };

  class TH2_FieldsAccess : public TH2 {
  public:
    Double_t& access_fTsumwy() { return fTsumwy; }
    Double_t& access_fTsumwy2() { return fTsumwy2; }
    Double_t& access_fTsumwxy() { return fTsumwxy; }
    Double_t& access_fScalefactor() { return fScalefactor; }
  };
  class TProfile_FieldsAccess : public TProfile {
  public:
    Double_t& access_fTsumwy() { return fTsumwy; }
    Double_t& access_fTsumwy2() { return fTsumwy2; }
  };
  class TProfile2D_FieldsAccess : public TProfile2D {
  public:
    Double_t& access_fTsumwz() { return fTsumwz; }
    Double_t& access_fTsumwz2() { return fTsumwz2; }
  };

  //____________________________________________________________________
  void compareFields(TH1 * t1, TH1 * t2);

  //____________________________________________________________________
  template <class TH, class THLW>
  static inline void compareMetaData(TH* hroot, THLW * hlw, bool ignorename = false)
  {
    assert (hroot&&hlw);
    if (!hroot||!hlw)
      return;
    //TH1 fields:
    if (!ignorename) {
      if (hroot->GetName()[0]!=':') {
        test("GetName()",hroot->GetName(), hlw->GetName());
      }
      //"if name starts with 't' it might begin with "::trigger::" => ignore
    }
    assert(hroot->GetTitle());
    assert(hlw->GetTitle());
    test("GetTitle()",hroot->GetTitle(), hlw->GetTitle());
    int nbinsx=int(hroot->GetNbinsX());
    test("GetNbinsX()",int(hroot->GetNbinsX()), int(hlw->GetNbinsX()));
    test("Integral()",hroot->Integral(), hlw->Integral());

    test("GetXaxis()->GetBinCenter(-2)",hroot->GetXaxis()->GetBinCenter(-2),hlw->GetXaxis()->GetBinCenter(-2));
    test("GetXaxis()->GetBinCenter(0)",hroot->GetXaxis()->GetBinCenter(0),hlw->GetXaxis()->GetBinCenter(0));
    test("GetXaxis()->GetBinCenter(1)",hroot->GetXaxis()->GetBinCenter(1),hlw->GetXaxis()->GetBinCenter(1));
    test("GetXaxis()->GetBinCenter(nbinsx/2)",hroot->GetXaxis()->GetBinCenter(nbinsx/2),hlw->GetXaxis()->GetBinCenter(nbinsx/2));
    test("GetXaxis()->GetBinCenter(nbinsx-2)",hroot->GetXaxis()->GetBinCenter(nbinsx-2),hlw->GetXaxis()->GetBinCenter(nbinsx-2));
    test("GetXaxis()->GetBinCenter(nbinsx-1)",hroot->GetXaxis()->GetBinCenter(nbinsx-1),hlw->GetXaxis()->GetBinCenter(nbinsx-1));
    test("GetXaxis()->GetBinCenter(nbinsx)",hroot->GetXaxis()->GetBinCenter(nbinsx),hlw->GetXaxis()->GetBinCenter(nbinsx));
    test("GetXaxis()->GetBinCenter(nbinsx+1)",hroot->GetXaxis()->GetBinCenter(nbinsx+1),hlw->GetXaxis()->GetBinCenter(nbinsx+1));
    test("GetXaxis()->GetBinCenter(nbinsx+2)",hroot->GetXaxis()->GetBinCenter(nbinsx+2),hlw->GetXaxis()->GetBinCenter(nbinsx+2));

    if (dynamic_cast<TH2*>(hroot)) {
      test("GetYaxis()->GetBinCenter(-200)",hroot->GetYaxis()->GetBinCenter(-200),hlw->GetYaxis()->GetBinCenter(-200));
      test("GetYaxis()->GetBinCenter(0)",hroot->GetYaxis()->GetBinCenter(0),hlw->GetYaxis()->GetBinCenter(0));
      test("GetYaxis()->GetBinCenter(1)",hroot->GetYaxis()->GetBinCenter(1),hlw->GetYaxis()->GetBinCenter(1));
      test("GetYaxis()->GetBinCenter(17)",hroot->GetYaxis()->GetBinCenter(17),hlw->GetYaxis()->GetBinCenter(17));
      test("GetYaxis()->GetBinCenter(100)",hroot->GetYaxis()->GetBinCenter(100),hlw->GetYaxis()->GetBinCenter(100));
      test("GetYaxis()->GetBinCenter(500)",hroot->GetYaxis()->GetBinCenter(500),hlw->GetYaxis()->GetBinCenter(500));
    }
    //Protected TH1 fields:
    TH1_FieldsAccess * hroot_1daccess(static_cast<TH1_FieldsAccess*>(static_cast<TH1*>(hroot)));
    test("Entries",hroot_1daccess->access_fEntries(), hlw->GetEntries());

    if (dynamic_cast<TH2*>(hroot)) {
      TH2* hroot2d = static_cast<TH2*>(static_cast<TH1*>(hroot));
      LWHist2D * hlw2d = dynamic_cast<LWHist2D *>(hlw);
      if (hlw2d) {
        test("GetNbinsY()",int(hroot2d->GetNbinsY()), int(hlw2d->GetNbinsY()));
        TH1_FieldsAccess * hroot_1daccess(static_cast<TH1_FieldsAccess*>(static_cast<TH1*>(hroot2d)));
        TH2_FieldsAccess * hroot_2daccess(static_cast<TH2_FieldsAccess*>(hroot2d));
        double sumW, sumW2, sumWX, sumWX2;
        double sumWY, sumWY2, sumWXY;
        hlw2d->getSums(sumW, sumW2, sumWX, sumWX2,sumWY, sumWY2, sumWXY);
        test("sumw",hroot_1daccess->access_fTsumw(), sumW);
        test("sumw2",hroot_1daccess->access_fTsumw2(), sumW2);
        test("sumwx",hroot_1daccess->access_fTsumwx(), sumWX);
        test("sumwx2",hroot_1daccess->access_fTsumwx2(), sumWX2);
        test("sumwy",hroot_2daccess->access_fTsumwy(), sumWY);
        test("sumwy2",hroot_2daccess->access_fTsumwy2(), sumWY2);
        test("sumwxy",hroot_2daccess->access_fTsumwxy(), sumWXY);
      } else {
        TProfile2D_LW * hlwprof2d = dynamic_cast<TProfile2D_LW *>(hlw);
        TProfile2D* hrootprof2d = static_cast<TProfile2D*>(static_cast<TH1*>(hroot));
        assert(hlwprof2d);
        assert(hrootprof2d);
        test("GetNbinsY()",int(hrootprof2d->GetNbinsY()), int(hlwprof2d->GetNbinsY()));
        TH1_FieldsAccess * hroot_1daccess(static_cast<TH1_FieldsAccess*>(static_cast<TH1*>(hroot2d)));
        TH2_FieldsAccess * hroot_2daccess(static_cast<TH2_FieldsAccess*>(hroot2d));
        TProfile2D_FieldsAccess * hroot_prof2daccess(static_cast<TProfile2D_FieldsAccess*>(hroot2d));
        double sumW, sumW2, sumWX, sumWX2;
        double sumWY, sumWY2, sumWXY;
        double sumWZ, sumWZ2;
        hlwprof2d->getSums(sumW, sumW2, sumWX, sumWX2,sumWY, sumWY2, sumWXY,sumWZ, sumWZ2);
        test("sumw",hroot_1daccess->access_fTsumw(), sumW);
        test("sumw2",hroot_1daccess->access_fTsumw2(), sumW2);
        test("sumwx",hroot_1daccess->access_fTsumwx(), sumWX);
        test("sumwx2",hroot_1daccess->access_fTsumwx2(), sumWX2);
        test("sumwy",hroot_2daccess->access_fTsumwy(), sumWY);
        test("sumwy2",hroot_2daccess->access_fTsumwy2(), sumWY2);
        test("sumwxy",hroot_2daccess->access_fTsumwxy(), sumWXY);
        test("sumwz",hroot_prof2daccess->access_fTsumwz(), sumWZ);
        test("sumwz2",hroot_prof2daccess->access_fTsumwz2(), sumWZ2);
      }
    } else {
      LWHist1D * hlw1d = dynamic_cast<LWHist1D *>(hlw);
      if (hlw1d) {
        double sumW, sumW2, sumWX, sumWX2;
        hlw1d->getSums(sumW, sumW2, sumWX, sumWX2);
        test("sumw",hroot_1daccess->access_fTsumw(), sumW);
        test("sumw2",hroot_1daccess->access_fTsumw2(), sumW2);
        test("sumwx",hroot_1daccess->access_fTsumwx(), sumWX);
        test("sumwx2",hroot_1daccess->access_fTsumwx2(), sumWX2);
      } else {
        TProfile_LW * hlwprof = dynamic_cast<TProfile_LW *>(hlw);
        assert(hlwprof);
        TProfile_FieldsAccess * hroot_profileaccess(static_cast<TProfile_FieldsAccess*>(static_cast<TH1*>(hroot)));
        double sumW, sumW2, sumWX, sumWX2,sumWY, sumWY2;
        hlwprof->getSums(sumW, sumW2, sumWX, sumWX2,sumWY, sumWY2);
        test("sumw",hroot_1daccess->access_fTsumw(), sumW);
        test("sumw2",hroot_1daccess->access_fTsumw2(), sumW2);
        test("sumwx",hroot_1daccess->access_fTsumwx(), sumWX);
        test("sumwx2",hroot_1daccess->access_fTsumwx2(), sumWX2);
        test("sumwy",hroot_profileaccess->access_fTsumwy(), sumWY);
        test("sumwy2",hroot_profileaccess->access_fTsumwy2(), sumWY2);
      }
    }
    //Fixme: add other stuff as necessary...

    HistValFunctions::test("[TH1-level] GetMarkerColor()",hroot->GetMarkerColor(), hlw->GetMarkerColor());
    HistValFunctions::test("[TH1-level] GetMarkerStyle()",hroot->GetMarkerStyle(), hlw->GetMarkerStyle());
    HistValFunctions::test("[TH1-level] GetMarkerSize()",hroot->GetMarkerSize(), hlw->GetMarkerSize());


    //Bin labels:

    TListIter next1(hroot->GetXaxis()->GetLabels());
    TObjString *obj1;
    while ((obj1=(TObjString*)next1())) {
      int bin1 = obj1->GetUniqueID();
      assert(bin1>=0);
      unsigned bin = static_cast<unsigned>(bin1);
      const std::string label(obj1->GetString().Data());
      const char * label_lw = hlw->GetXaxis()->GetBinLabel(bin);
      if (!label_lw)
        testfailed("GetXaxis()->GetBinLabel(..) (NULL)");
      test("GetXaxis()->GetBinLabel(..)",label.c_str(),label_lw);
   }
    TListIter next2(hroot->GetYaxis()->GetLabels());
    TObjString *obj2;
    while ((obj2=(TObjString*)next2())) {
      int bin1 = obj2->GetUniqueID();
      assert(bin1>=0);
      unsigned bin = static_cast<unsigned>(bin1);
      std::string label(obj2->GetString().Data());
      const char * label_lw = hlw->GetYaxis()->GetBinLabel(bin);
      if (!label_lw)
        testfailed("GetYaxis()->GetBinLabel(..) (NULL)");
      test("GetYaxis()->GetBinLabel(..)",label.c_str(),label_lw);
   }
  }

  //____________________________________________________________________
  template <class T1, class T2>
  static void compareBinContents_1D(T1* t1, T2 * t2, bool relaxedErrorComparison = false)
  {
    test("GetNbinsX()",int(t1->GetNbinsX()), int(t2->GetNbinsX()));
    int n = t1->GetNbinsX();
//     for (int i=0;i<=n+1;++i) {
//       std::cout<<"-----> Test: ibin="<<i<<", controot: "<<t1->GetBinContent(i)<<", contlw: "<<t2->GetBinContent(i)<<std::endl;
//       std::cout<<"-----> Test: ibin="<<i<<", errroot: "<<t1->GetBinError(i)<<", errlw: "<<t2->GetBinError(i)<<std::endl;
//     }
    for (int i=0;i<=n+1;++i) {
      if (!compatible(t1->GetBinContent(i),t2->GetBinContent(i))) {
        std::ostringstream s;
        s << "["<<typeid(T1).name()<<" vs. "<<typeid(T2).name()<<"] GetBinContent("<<i<<", nbins="<<n<<", ninternalbins="<<n+2<<")";
        test(s.str(),t1->GetBinContent(i),t2->GetBinContent(i));
      }
    }
    for (int i=0;i<=n+1;++i) {
      if (!(relaxedErrorComparison
            ?floatCompat(t1->GetBinError(i),t2->GetBinError(i),1.0e-3)
            :compatible(t1->GetBinError(i),t2->GetBinError(i)))) {
        std::ostringstream s;
        s << "["<<typeid(T1).name()<<" vs. "<<typeid(T2).name()<<"] GetBinError("<<i<<", nbins="<<n<<", ninternalbins="<<n+2<<")";
        test(s.str(),t1->GetBinError(i),t2->GetBinError(i));
      }
    }
  }
  //____________________________________________________________________
  template <class T1, class T2>
  static void compareBinContents_2D(T1* t1, T2 * t2, bool relaxedErrorComparison = false)
  {
    assert(t1);
    assert(t2);
    test("GetNbinsX()",int(t1->GetNbinsX()), int(t2->GetNbinsX()));
    test("GetNbinsY()",int(t1->GetNbinsY()), int(t2->GetNbinsY()));
    const int nx = t1->GetNbinsX();
    const int ny = t1->GetNbinsY();
    for (int ix=0;ix<=nx+1;++ix)
      for (int iy=0;iy<=ny+1;++iy) {
        if (!compatible(t1->GetBinContent(ix,iy),t2->GetBinContent(ix,iy))) {
          std::ostringstream s;
          s << "GetBinContent("<<ix<<", "<<iy<<")";
          test(s.str(),t1->GetBinContent(ix,iy),t2->GetBinContent(ix,iy));
        }
      }
    for (int ix=0;ix<=nx+1;++ix)
      for (int iy=0;iy<=ny+1;++iy) {
        if (!(relaxedErrorComparison
              ?floatCompat(t1->GetBinError(ix,iy),t2->GetBinError(ix,iy),1.0e-3)
              :compatible(t1->GetBinError(ix,iy),t2->GetBinError(ix,iy)))) {
          std::ostringstream s;
          s << "GetBinError("<<ix<<", "<<iy<<")";
          test(s.str(),t1->GetBinError(ix,iy),t2->GetBinError(ix,iy));
        }
      }
  }

}

#endif

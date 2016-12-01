/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "rootincludes.h"
#include "jobflags.h"
#include <cmath>

////////////////////////////////////////////
//               Interface:               //
////////////////////////////////////////////

template <class T>
bool checkParameters(T*h1,T*h2);


////////////////////////////////////////////
//                Helpers:                //
////////////////////////////////////////////

#define xstringify(s) str(s)
#define stringify(s) #s

#define REPORT(x) { std::cout << "    -> ERROR: "<<x<<std::endl; }
template <class T> bool varCompatible(const T var1,const T var2) {
  if (var1==var2)
    return true;
  const bool var1_nan = std::isnan(var1);
  const bool var2_nan = std::isnan(var2);
  if (var1_nan&&var2_nan) {
    if (cfg_report_NaN)
      std::cout<<"WARNING: Spotted NaN values in parameter (but same in both of the compared histograms)!"<<std::endl;
    return true;
  }
  if (var1_nan||var2_nan) {
    if (cfg_report_NaN)
      REPORT("Spotted NaN values in parameter (from only one of the compared histograms)!");
    return false;
  }
  REPORT("different values found: "<<var1<<" vs. "<<var2);
  return false;
}

inline bool floatingpointcompat(double var1, double var2, const double& epsilon)
{
  const bool var1_nan = (var1!=var1);
  const bool var2_nan = (var2!=var2);
  if (var1_nan&&var2_nan) {
    if (cfg_report_NaN)
      std::cout<<"WARNING: Spotted NaN values in floating point number (but same in both of the compared histograms)!"<<std::endl;
    return true;
  }
  if (var1_nan||var2_nan) {
    if (cfg_report_NaN)
      REPORT("Spotted NaN values in floating point number (from only one of the compared histograms)!");
    return false;
  }
  const double test(fabs(var1-var2)/(1.0+std::max<double>(fabs(var1),fabs(var2))));
  //   if (!(test<epsilon)) {
  //     std::cout<<"NB: var difference: "<<var1<<" vs. "<<var2<<" :"<<test<<std::endl;
  //   }
  return test<epsilon;
}
template <>        bool varCompatible(const double var1,const double var2) { return floatingpointcompat(var1,var2,1.0e-5); }
template <>        bool varCompatible(const float var1,const float var2) { return floatingpointcompat(var1,var2,1.0e-3); }
template <>        bool varCompatible(const char* var1,const char* var2) { return std::string(var1)==std::string(var2); }
template <>        bool varCompatible(TString var1,TString var2) { return var1 == var2; }


template <class TContainedType>
bool tlistsCompatible(const TList*h1,const TList*h2);//forward decl.

template <>        bool varCompatible(TFunction var1,  TFunction var2)
{
  if (!varCompatible(var1.GetMangledName(), var2.GetMangledName()))
    return false;
  if (!varCompatible(var1.GetSignature(), var2.GetSignature()))
    return false;
  if (!tlistsCompatible<TString>(var1.GetListOfMethodArgs(), var2.GetListOfMethodArgs()))
    return false;
  return true;
}

#define TESTSIMPLEVAR(varname) \
  { if (!varCompatible(h1->varname,h2->varname)) { \
      REPORT("Incompatible values of "<<stringify(varname)<<" ("<<h1->varname<<" vs. "<<h2->varname<<")"); \
      return false; } }

#define TESTSIMPLEEXTVAR(var1,var2,varname) \
  { if (!varCompatible(var1,var2)) { \
      REPORT("Incompatible values of "<<stringify(varname)<<" ("<<var1<<" vs. "<<var2<<")"); \
      return false; } }

#define TESTSIMPLEUNSUPPORTEDSTYLEVAR(varname) { if (!cfg_ignore_unsupported_style_vars)  { TESTSIMPLEVAR(varname); } }


template <class TArrayX>
bool arrayCompatible(const TArrayX* a1,const TArrayX* a2,const char*name) {
  if (!varCompatible(a1->fN,a2->fN)) {
    REPORT("Incompatible arrays "<<name<<" (number of entries)");
    return false;
  }
  if (a1->fArray==0&&a2->fArray==0)
    return true;
  if (a1->fArray==0||a2->fArray==0) {
    REPORT("Incompatible arrays "<<name<<" (one NULL, one not NULL)"); \
    return false;
  }
  for (int i = 0; i < a1->fN; ++i)
    if (!varCompatible(a1->fArray[i],a2->fArray[i])) {
      REPORT("Incompatible arrays "<<name<<" (at least one entry incompatible)"); \
      return false;
    }
  return true;
}

#define ACTUALTESTARRAY(a,b,name)	 \
  { if (!arrayCompatible(a,b,name)) { \
      return false; } }

#define TESTASARRAY() {ACTUALTESTARRAY(h1,h2,"")}
#define TESTARRAY(varname) {ACTUALTESTARRAY(&(h1->varname),&(h2->varname),stringify(varname))}
#define TESTARRAYPTR(varname) {ACTUALTESTARRAY((h1->varname),(h2->varname),stringify(varname))}

bool axisCompatible(const TAxis* h1,const TAxis* h2) {
  //TAxis:
  TESTSIMPLEVAR(GetNbins());
  TESTSIMPLEVAR(GetXmin());
  TESTSIMPLEVAR(GetXmax());
  TESTSIMPLEVAR(GetFirst());
  TESTSIMPLEVAR(GetLast());
  //TESTSIMPLEVAR(IsAlphanumeric());
  //TESTSIMPLEVAR(CanBeAlphanumeric());
  TESTSIMPLEVAR(CanExtend());
  TESTSIMPLEVAR(GetTimeDisplay());
  TESTSIMPLEVAR(GetTimeFormat());
  TESTARRAYPTR(GetXbins());
  if (!tlistsCompatible<TString>((TList*)(h1->GetLabels()),(TList*)(h2->GetLabels()))) {
    REPORT("Incompatible fLabels");
    return false;
  }
   //TAttAxis:
  TESTSIMPLEUNSUPPORTEDSTYLEVAR(GetNdivisions());
  TESTSIMPLEUNSUPPORTEDSTYLEVAR(GetAxisColor());
  TESTSIMPLEVAR(GetLabelColor());
  TESTSIMPLEVAR(GetLabelFont());
  TESTSIMPLEVAR(GetLabelOffset());
  TESTSIMPLEVAR(GetLabelSize());
  TESTSIMPLEUNSUPPORTEDSTYLEVAR(GetTickLength());
  TESTSIMPLEUNSUPPORTEDSTYLEVAR(GetTitleOffset());
  TESTSIMPLEVAR(GetTitleSize());
  TESTSIMPLEVAR(GetTitleColor());
  TESTSIMPLEVAR(GetTitleFont());
   //TNamed:
  TESTSIMPLEVAR(GetName());
  TESTSIMPLEVAR(GetTitle());
  return true;
}

template <class TContainedType>
bool tlistsCompatible(const TList*h1,const TList*h2) {
  if (!h1&&!h2)
    return true;
  if (!h1||!h2)
    return false;
  //TCollection:
  TESTSIMPLEVAR(GetName());
  TESTSIMPLEVAR(Capacity());
  //TSeqCollection:
  TESTSIMPLEVAR(IsSorted());
  //For the TList itself we loop, cast each element to TContainedType,
  //and compare using operator==.
  Int_t sz = h1->Capacity();
  for (int i = 0; i<sz; ++i) {
    TContainedType * t1 = dynamic_cast<TContainedType*>(h1->At(i));
    TContainedType * t2 = dynamic_cast<TContainedType*>(h1->At(i));
    if (!t1&&!t2)
      continue;
    if (!t1||!t2)
      return false;
    if (!varCompatible(*t1,*t2))
      return false;
  }
  return true;
}

////////////////////////////////////////////
//       Specific Implementations:        //
////////////////////////////////////////////

template <>
bool checkParameters(TH1*h1,TH1*h2)
{
  assert(h1!=h2);
  assert(h1&&h2);
  //TH1:
  //Ignore fDirectory and buffer related methods.
  TESTSIMPLEVAR(GetEntries());
  TESTSIMPLEVAR(GetMaximumStored());
  TESTSIMPLEVAR(GetMinimumStored());
  TESTSIMPLEVAR(GetNormFactor());
  Double_t stats1[20], stats2[20];
  h1->GetStats(stats1);
  h2->GetStats(stats2);
  TESTSIMPLEEXTVAR(stats1[0], stats2[0], fTsumw);
  TESTSIMPLEEXTVAR(stats1[1], stats2[1], fTsumw2);
  TESTSIMPLEEXTVAR(stats1[2], stats2[2], fTsumwx);
  TESTSIMPLEEXTVAR(stats1[3], stats2[3], fTsumwx2);
  TESTSIMPLEVAR(GetDimension());
  TESTSIMPLEVAR(GetNcells());
  TESTSIMPLEVAR(GetBarOffset());
  TESTSIMPLEVAR(GetBarWidth());

  Int_t nlevels1 = h1->GetContour(nullptr);
  Int_t nlevels2 = h2->GetContour(nullptr);
  TESTSIMPLEEXTVAR (nlevels1, nlevels2, fContour.fN);
  if (nlevels1 > 0) {
    TArrayD levels1 (nlevels1);
    TArrayD levels2 (nlevels1);
    h1->GetContour (levels1.GetArray());
    h2->GetContour (levels2.GetArray());
    ACTUALTESTARRAY(&levels1, &levels2, "fContour");
  }

  bool test_sumw2(true);
  if (cfg_ignore_errors_related_to_a_non_existant_sumw2_array) {
    if ((h1->GetSumw2()->fN==0||h2->GetSumw2()->fN==0) && ((h1->GetSumw2()->fN==0)!=(h2->GetSumw2()->fN==0)))
      test_sumw2 = false;
  }
  if (test_sumw2)
    TESTARRAYPTR(GetSumw2());
  TESTSIMPLEVAR(GetOption());
  if (!tlistsCompatible<TFunction>(h1->GetListOfFunctions(),h2->GetListOfFunctions())) {
    REPORT("Incompatible fXaxis");
    return false;
  }
  if (!axisCompatible(h1->GetXaxis(), h2->GetXaxis())) {
    REPORT("Incompatible fXaxis");
    return false;
  }
  if (!axisCompatible(h1->GetYaxis(), h2->GetYaxis())) {
    REPORT("Incompatible fYaxis");
    return false;
  }
  if (!axisCompatible(h1->GetZaxis(), h2->GetZaxis())) {
    REPORT("Incompatible fZaxis");
    return false;
  }

  //TNamed:
  //Ignore fName
  TESTSIMPLEVAR(GetTitle());
  //TAttLine:
  TESTSIMPLEVAR(GetLineColor());
  TESTSIMPLEVAR(GetLineStyle());
  TESTSIMPLEVAR(GetLineWidth());
  //TAttFill:
  TESTSIMPLEVAR(GetFillColor());
  TESTSIMPLEVAR(GetFillStyle());
  //TAttMarker:
  TESTSIMPLEVAR(GetMarkerColor());
  TESTSIMPLEVAR(GetMarkerStyle());
  TESTSIMPLEVAR(GetMarkerSize());
  //TObject:
  //Ignore fUniqueID
  for (int i=0; i < 32; i++)
    TESTSIMPLEVAR(TestBit(i));
  return true;
}

//Actual implementations:
template <> bool checkParameters(TH1F*h1,TH1F*h2)  { TESTASARRAY(); return true; }
template <> bool checkParameters(TH1D*h1,TH1D*h2)  { TESTASARRAY(); return true; }
template <> bool checkParameters(TH1I*h1,TH1I*h2)  { TESTASARRAY(); return true; }
template <> bool checkParameters(TH1C*h1,TH1C*h2)  { TESTASARRAY(); return true; }
template <> bool checkParameters(TH1S*h1,TH1S*h2)  { TESTASARRAY(); return true; }

template <>
bool checkParameters(TH2*h1,TH2*h2)
{
  //TESTSIMPLEVAR(fScalefactor);
  Double_t stats1[20], stats2[20];
  h1->GetStats(stats1);
  h2->GetStats(stats2);
  TESTSIMPLEEXTVAR(stats1[4], stats2[4], fTsumwy);
  TESTSIMPLEEXTVAR(stats1[5], stats2[5], fTsumwy2);
  TESTSIMPLEEXTVAR(stats1[6], stats2[6], fTsumwxy);
  return true;
}

template <> bool checkParameters(TH2F*h1,TH2F*h2)  { TESTASARRAY(); return true; }
template <> bool checkParameters(TH2D*h1,TH2D*h2)  { TESTASARRAY(); return true; }
template <> bool checkParameters(TH2I*h1,TH2I*h2)  { TESTASARRAY(); return true; }
template <> bool checkParameters(TH2C*h1,TH2C*h2)  { TESTASARRAY(); return true; }
template <> bool checkParameters(TH2S*h1,TH2S*h2)  { TESTASARRAY(); return true; }

template <> bool checkParameters(TProfile*h1,TProfile*h2)
{
  TESTSIMPLEVAR(GetNcells());
  Int_t sz = h1->GetNcells();
  for (Int_t i = 0; i < sz; i++)
    TESTSIMPLEVAR (GetBinEntries(i));
  TESTSIMPLEVAR(GetErrorOption());
  TESTSIMPLEVAR(GetYmin());
  TESTSIMPLEVAR(GetYmax());
  //TESTSIMPLEVAR(fScaling);
  Double_t stats1[20], stats2[20];
  h1->GetStats(stats1);
  h2->GetStats(stats2);
  TESTSIMPLEEXTVAR(stats1[4], stats2[4], fTsumwy);
  TESTSIMPLEEXTVAR(stats1[5], stats2[5], fTsumwy2);
  return true;
}

template <> bool checkParameters(TProfile2D*h1,TProfile2D*h2)
{
  TESTSIMPLEVAR(GetNcells());
  Int_t sz = h1->GetNcells();
  for (Int_t i = 0; i < sz; i++)
    TESTSIMPLEVAR (GetBinEntries(i));
  TESTSIMPLEVAR(GetErrorOption());
  TESTSIMPLEVAR(GetZmin());
  TESTSIMPLEVAR(GetZmax());
  //TESTSIMPLEVAR(fScaling);
  Double_t stats1[20], stats2[20];
  h1->GetStats(stats1);
  h2->GetStats(stats2);
  TESTSIMPLEEXTVAR(stats1[7], stats2[7], fTsumwz);
  TESTSIMPLEEXTVAR(stats1[8], stats2[8], fTsumwz2);
  return true;
}

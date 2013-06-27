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
  const bool var1_nan = (var1!=var1);
  const bool var2_nan = (var2!=var2);
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


template <class TContainedType>
bool tlistsCompatible(const TList*h1,const TList*h2);//forward decl.

template <>        bool varCompatible(const TFunction var1,const TFunction var2)
{
  if (!varCompatible(var1.fMangledName,var2.fMangledName))
    return false;
  if (!varCompatible(var1.fSignature,var2.fSignature))
    return false;
  if (!tlistsCompatible<TString>(var1.fMethodArgs,var2.fMethodArgs))
    return false;
  return true;
}

#define TESTSIMPLEVAR(varname) \
  { if (!varCompatible(h1->varname,h2->varname)) { \
      REPORT("Incompatible values of "<<stringify(varname)<<" ("<<h1->varname<<" vs. "<<h2->varname<<")"); \
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

bool axisCompatible(const TAxis* h1,const TAxis* h2) {
  //TAxis:
  TESTSIMPLEVAR(fNbins);
  TESTSIMPLEVAR(fXmin);
  TESTSIMPLEVAR(fXmax);
  TESTSIMPLEVAR(fFirst);
  TESTSIMPLEVAR(fLast);
  TESTSIMPLEVAR(fBits2);
  TESTSIMPLEVAR(fTimeDisplay);
  TESTSIMPLEVAR(fTimeFormat);
  TESTARRAY(fXbins);
  if (!tlistsCompatible<TString>((TList*)(h1->fLabels),(TList*)(h2->fLabels))) {
    REPORT("Incompatible fLabels");
    return false;
  }
   //TAttAxis:
  TESTSIMPLEUNSUPPORTEDSTYLEVAR(fNdivisions);
  TESTSIMPLEUNSUPPORTEDSTYLEVAR(fAxisColor);
  TESTSIMPLEVAR(fLabelColor);
  TESTSIMPLEVAR(fLabelFont);
  TESTSIMPLEVAR(fLabelOffset);
  TESTSIMPLEVAR(fLabelSize);
  TESTSIMPLEUNSUPPORTEDSTYLEVAR(fTickLength);
  TESTSIMPLEUNSUPPORTEDSTYLEVAR(fTitleOffset);
  TESTSIMPLEVAR(fTitleSize);
  TESTSIMPLEVAR(fTitleColor);
  TESTSIMPLEVAR(fTitleFont);
   //TNamed:
  TESTSIMPLEVAR(fName);
  TESTSIMPLEVAR(fTitle);
  return true;
}

template <class TContainedType>
bool tlistsCompatible(const TList*h1,const TList*h2) {
  if (!h1&&!h2)
    return true;
  if (!h1||!h2)
    return false;
  //TCollection:
  TESTSIMPLEVAR(fName);
  TESTSIMPLEVAR(fSize);
  //TSeqCollection:
  TESTSIMPLEVAR(fSorted);
  //For the TList itself we loop, cast each element to TContainedType,
  //and compare using operator==.
  for (int i = 0; i<h1->fSize; ++i) {
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
  TESTSIMPLEVAR(fEntries);
  TESTSIMPLEVAR(fMaximum);
  TESTSIMPLEVAR(fMinimum);
  TESTSIMPLEVAR(fNormFactor);
  TESTSIMPLEVAR(fTsumw);
  TESTSIMPLEVAR(fTsumw2);
  TESTSIMPLEVAR(fTsumwx);
  TESTSIMPLEVAR(fTsumwx2);
  TESTSIMPLEVAR(fDimension);
  TESTSIMPLEVAR(fNcells);
  TESTSIMPLEVAR(fBarOffset);
  TESTSIMPLEVAR(fBarWidth);
  TESTARRAY(fContour);
  bool test_sumw2(true);
  if (cfg_ignore_errors_related_to_a_non_existant_sumw2_array) {
    if ((h1->fSumw2.fN==0||h2->fSumw2.fN==0) && ((h1->fSumw2.fN==0)!=(h2->fSumw2.fN==0)))
      test_sumw2 = false;
  }
  if (test_sumw2)
    TESTARRAY(fSumw2);
  TESTSIMPLEVAR(fOption);
  if (!tlistsCompatible<TFunction>(h1->fFunctions,h2->fFunctions)) {
    REPORT("Incompatible fXaxis");
    return false;
  }
  if (!axisCompatible(&(h1->fXaxis),&(h2->fXaxis))) {
    REPORT("Incompatible fXaxis");
    return false;
  }
  if (!axisCompatible(&(h1->fYaxis),&(h2->fYaxis))) {
    REPORT("Incompatible fYaxis");
    return false;
  }
  if (!axisCompatible(&(h1->fZaxis),&(h2->fZaxis))) {
    REPORT("Incompatible fZaxis");
    return false;
  }

  //TNamed:
  //Ignore fName
  TESTSIMPLEVAR(fTitle);
  //TAttLine:
  TESTSIMPLEVAR(fLineColor);
  TESTSIMPLEVAR(fLineStyle);
  TESTSIMPLEVAR(fLineWidth);
  //TAttFill:
  TESTSIMPLEVAR(fFillColor);
  TESTSIMPLEVAR(fFillStyle);
  //TAttMarker:
  TESTSIMPLEVAR(fMarkerColor);
  TESTSIMPLEVAR(fMarkerStyle);
  TESTSIMPLEVAR(fMarkerSize);
  //TObject:
  //Ignore fUniqueID
  TESTSIMPLEVAR(fBits);
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
  TESTSIMPLEVAR(fScalefactor);
  TESTSIMPLEVAR(fTsumwy);
  TESTSIMPLEVAR(fTsumwy2);
  TESTSIMPLEVAR(fTsumwxy);
  return true;
}

template <> bool checkParameters(TH2F*h1,TH2F*h2)  { TESTASARRAY(); return true; }
template <> bool checkParameters(TH2D*h1,TH2D*h2)  { TESTASARRAY(); return true; }
template <> bool checkParameters(TH2I*h1,TH2I*h2)  { TESTASARRAY(); return true; }
template <> bool checkParameters(TH2C*h1,TH2C*h2)  { TESTASARRAY(); return true; }
template <> bool checkParameters(TH2S*h1,TH2S*h2)  { TESTASARRAY(); return true; }

template <> bool checkParameters(TProfile*h1,TProfile*h2)
{
  TESTARRAY(fBinEntries);
  TESTSIMPLEVAR(fErrorMode);
  TESTSIMPLEVAR(fYmin);
  TESTSIMPLEVAR(fYmax);
  TESTSIMPLEVAR(fScaling);
  TESTSIMPLEVAR(fTsumwy);
  TESTSIMPLEVAR(fTsumwy2);
  return true;
}

template <> bool checkParameters(TProfile2D*h1,TProfile2D*h2)
{
  TESTARRAY(fBinEntries);
  TESTSIMPLEVAR(fErrorMode);
  TESTSIMPLEVAR(fZmin);
  TESTSIMPLEVAR(fZmax);
  TESTSIMPLEVAR(fScaling);
  TESTSIMPLEVAR(fTsumwz);
  TESTSIMPLEVAR(fTsumwz2);
  return true;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class LWHistRootUtils                     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef LWHISTROOTUTILS_H
#define LWHISTROOTUTILS_H

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2D_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/LWHist1D.h"
#include "LWHists/TProfile_LW.h"
#include "LWHists/TProfile2D_LW.h"

#include "Flex1DHisto.h"
#include "Flex1DProfileHisto.h"
#include "Flex2DProfileHisto.h"

#if (__GNUC__ >= 6) && !defined(__clang__)
# define LWHISTS_NO_SANITIZE_UNDEFINED [[gnu::no_sanitize_undefined]]
#else
# define LWHISTS_NO_SANITIZE_UNDEFINED
#endif

class LWHistInt {
  //Trick to avoid exposing methods we don't really want to support in
  //the public interface:
  public:
  template<class TLW> static const float* getVarBins(TLW*h) { return h->getVarBins(); }
  template<class TLW> static const float* getVarBinsX(TLW*h) { return h->getVarBinsX(); }
  template<class TLW> static const float* getVarBinsY(TLW*h) { return h->getVarBinsY(); }
};

namespace LWHistRootUtils {
  //Tricks to get the weight-sum's (GetStats(..) can be unreliable).
  class TH1_FieldsAccess : public TH1 {
  public:
    double& getSumW() { return fTsumw; }
    double& getSumW2() { return fTsumw2; }
    double& getSumWX() { return fTsumwx; }
    double& getSumWX2() { return fTsumwx2; }
    TArrayD& getSumw2Array() { return fSumw2; }
  };
  class TH2_FieldsAccess : public TH2 {
  public:
    double& getSumWY() { return fTsumwy; }
    double& getSumWY2() { return fTsumwy2; }
    double& getSumWXY() { return fTsumwxy; }
  };
  class TProfile_FieldsAccess : public TProfile {
  public:
    double& getSumWY() { return fTsumwy; }
    double& getSumWY2() { return fTsumwy2; }
    TArrayD& getBinEntriesArray() { return fBinEntries; }
  };
  class TProfile2D_FieldsAccess : public TProfile2D {
  public:
    double& getSumWZ() { return fTsumwz; }
    double& getSumWZ2() { return fTsumwz2; }
    TArrayD& getBinEntriesArray() { return fBinEntries; }
  };

  inline double getSumW LWHISTS_NO_SANITIZE_UNDEFINED (TH1*h) { return static_cast<TH1_FieldsAccess*>(h)->getSumW(); }
  inline double getSumW2 LWHISTS_NO_SANITIZE_UNDEFINED (TH1*h) { return static_cast<TH1_FieldsAccess*>(h)->getSumW2(); }
  inline double getSumWX LWHISTS_NO_SANITIZE_UNDEFINED (TH1*h) { return static_cast<TH1_FieldsAccess*>(h)->getSumWX(); }
  inline double getSumWX2 LWHISTS_NO_SANITIZE_UNDEFINED (TH1*h) { return static_cast<TH1_FieldsAccess*>(h)->getSumWX2(); }
  inline double getSumWY LWHISTS_NO_SANITIZE_UNDEFINED (TH2*h) { return static_cast<TH2_FieldsAccess*>(h)->getSumWY(); }
  inline double getSumWY2 LWHISTS_NO_SANITIZE_UNDEFINED (TH2*h) { return static_cast<TH2_FieldsAccess*>(h)->getSumWY2(); }
  inline double getSumWXY LWHISTS_NO_SANITIZE_UNDEFINED (TH2*h) { return static_cast<TH2_FieldsAccess*>(h)->getSumWXY(); }
  inline TArrayD& getSumw2Array LWHISTS_NO_SANITIZE_UNDEFINED (TH1*h) { return static_cast<TH1_FieldsAccess*>(h)->getSumw2Array(); }

  inline void setSumW LWHISTS_NO_SANITIZE_UNDEFINED (TH1*h,const double&s) { static_cast<TH1_FieldsAccess*>(h)->getSumW() = s; }
  inline void setSumW2 LWHISTS_NO_SANITIZE_UNDEFINED (TH1*h,const double&s) { static_cast<TH1_FieldsAccess*>(h)->getSumW2() = s; }
  inline void setSumWX LWHISTS_NO_SANITIZE_UNDEFINED (TH1*h,const double&s) { static_cast<TH1_FieldsAccess*>(h)->getSumWX() = s; }
  inline void setSumWX2 LWHISTS_NO_SANITIZE_UNDEFINED (TH1*h,const double&s) { static_cast<TH1_FieldsAccess*>(h)->getSumWX2() = s; }
  inline void setSumWY LWHISTS_NO_SANITIZE_UNDEFINED (TH2*h,const double&s) { static_cast<TH2_FieldsAccess*>(h)->getSumWY() = s; }
  inline void setSumWY2 LWHISTS_NO_SANITIZE_UNDEFINED (TH2*h,const double&s) { static_cast<TH2_FieldsAccess*>(h)->getSumWY2() = s; }
  inline void setSumWXY LWHISTS_NO_SANITIZE_UNDEFINED (TH2*h,const double&s) { static_cast<TH2_FieldsAccess*>(h)->getSumWXY() = s; }

  inline double getSumWY LWHISTS_NO_SANITIZE_UNDEFINED (TProfile*h) { return static_cast<TProfile_FieldsAccess*>(h)->getSumWY(); }
  inline double getSumWY2 LWHISTS_NO_SANITIZE_UNDEFINED (TProfile*h) { return static_cast<TProfile_FieldsAccess*>(h)->getSumWY2(); }
  inline void setSumWY LWHISTS_NO_SANITIZE_UNDEFINED (TProfile*h,const double&s) { static_cast<TProfile_FieldsAccess*>(h)->getSumWY() = s; }
  inline void setSumWY2 LWHISTS_NO_SANITIZE_UNDEFINED (TProfile*h,const double&s) { static_cast<TProfile_FieldsAccess*>(h)->getSumWY2() = s; }
  inline TArrayD& getBinEntriesArray LWHISTS_NO_SANITIZE_UNDEFINED (TProfile*h) { return static_cast<TProfile_FieldsAccess*>(h)->getBinEntriesArray(); }

  inline double getSumWZ LWHISTS_NO_SANITIZE_UNDEFINED (TProfile2D*h) { return static_cast<TProfile2D_FieldsAccess*>(h)->getSumWZ(); }
  inline double getSumWZ2 LWHISTS_NO_SANITIZE_UNDEFINED (TProfile2D*h) { return static_cast<TProfile2D_FieldsAccess*>(h)->getSumWZ2(); }
  inline void setSumWZ LWHISTS_NO_SANITIZE_UNDEFINED (TProfile2D*h,const double&s) { static_cast<TProfile2D_FieldsAccess*>(h)->getSumWZ() = s; }
  inline void setSumWZ2 LWHISTS_NO_SANITIZE_UNDEFINED (TProfile2D*h,const double&s) { static_cast<TProfile2D_FieldsAccess*>(h)->getSumWZ2() = s; }
  inline TArrayD& getBinEntriesArray LWHISTS_NO_SANITIZE_UNDEFINED (TProfile2D*h) { return static_cast<TProfile2D_FieldsAccess*>(h)->getBinEntriesArray(); }

  template <class T, class TH1X_LW, class TH1X, class TFlexHist>
  TH1X * createRootHisto(TH1X_LW* lwhist, TFlexHist * flexHist, bool& tookSumW2FromPools);

  TProfile * createRootProfileHisto(TProfile_LW* lwhist, Flex1DProfileHisto * flexHist);
  TProfile2D * createRoot2DProfileHisto(TProfile2D_LW* lwhist, Flex2DProfileHisto * flexHist);

  template <class THX>
  inline void deleteRootHisto(THX*rootHist, bool& sumW2IsFromPools);
  template <class TProfileX>
  void deleteProfileHisto(TProfileX*rootHist);

  template <class THX>
  void scaleContentsAndErrors( THX*, const double& fact );//NB: Triggers sumw2!
}

#include "LWHistRootUtils.icc"

#endif

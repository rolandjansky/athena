/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class TH1I_LW                             //
//                                                            //
//  Description: LightWeight version of TH1I.                 //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef TH1I_LW_H
#define TH1I_LW_H

#include "LWHists/LWHist1D.h"
class TH1I;

class TH1I_LW : public LWHist1D {
public:
  typedef int bin_type_t;

  //To allocate from pool - remember to delete with LWHist::safeDelete(..):
  static TH1I_LW * create( const char* name, const char* title,
			   unsigned nbinsx, const double& xlow, const double& xup );
  static TH1I_LW * create( const char* name, const char* title, unsigned nbinsx, const double* xbins );
  static TH1I_LW * create( const char* name, const char* title, unsigned nbinsx, const float* xbins );

  void Fill(const double& x);
  void Fill(const double& x, const double& w);
  unsigned GetNbinsX() const;
  double GetBinContent(unsigned bin) const;
  double GetBinError(unsigned bin) const;
  void SetBinContent(unsigned bin, const double& );
  void SetBinError(unsigned bin, const double& );
  unsigned GetEntries() const;
  void SetEntries(unsigned);
  void SetBins(unsigned nbins,double xmin,double xmax);
  void Reset();

  double getXMin() const;
  double getXMax() const;
  void GetBinContentAndError(unsigned bin,double& content, double&error) const;
  void SetBinContentAndError(unsigned bin,const double& content, const double& error);

  void getSums( double& sumW, double& sumW2,
		double& sumWX,double& sumWX2 ) const;
  void setSums( const double& sumW,const double&sumW2,
		const double& sumWX,const double& sumWX2 );

  TH1I* getROOTHist();
  TH1* getROOTHistBase();

  double Integral() const;

  //For fast looping, skipping bins where (content,error)==(0,0):
  void resetActiveBinLoop();
  bool getNextActiveBin(unsigned& bin, double& content, double& error);

  void scaleContentsAndErrors( const double& fact );//C.f. comment in LWHist1D.h

private:
  friend class LWHistInt;
  friend class LWHistVal;
  void clear();
  unsigned actualFindBinX(const double&) const;
  double actualGetBinCenterX(int bin) const;
  TH1* getROOTHistBaseNoAlloc() const;
  void clearKeptROOTHist();//Does nothing if root-backend.
  const float * getVarBins() const;//null if fixed bin-widths

  TH1I_LW( const char* name, const char* title,
	   unsigned nbinsx, const double& xlow, const double& xup, bool rootbackend );
  TH1I_LW( const char* name, const char* title, unsigned nbinsx, const double* xbins, bool rootbackend );
  TH1I_LW( const char* name, const char* title, unsigned nbinsx, const float* xbins, bool rootbackend );
  virtual ~TH1I_LW();
  TH1I_LW( const TH1I_LW & );
  TH1I_LW & operator= ( const TH1I_LW & );
  void * m_flexHisto;
  TH1I * m_rootHisto;
  unsigned m_rootbackend_fastloopbin;
  bool m_ownsRootSumw2;
};

#endif

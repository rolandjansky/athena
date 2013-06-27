/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class TProfile2D_LW                       //
//                                                            //
//  Description: LightWeight version of TProfile2D.           //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2009                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef TPROFILE2D_LW_H
#define TPROFILE2D_LW_H

#include "LWHists/LWHist.h"
class TProfile2D;
class Flex2DProfileHisto;

class TProfile2D_LW : public LWHist {
public:

  //To allocate from pool - remember to delete with LWHist::safeDelete(..):
  static TProfile2D_LW * create( const char* name, const char* title,
				 unsigned nbinsx, const double& xlow, const double& xup,
				 unsigned nbinsy, const double& ylow, const double& yup,
				 const char* option=" " );
  static TProfile2D_LW * create( const char* name, const char* title,
				 unsigned nbinsx, const double& xlow, const double& xup,
				 unsigned nbinsy, const double& ylow, const double& yup,
				 const double& zlow, const double& zup, const char* option=" " );
  static TProfile2D_LW * create( const char* name, const char* title,
				 unsigned nbinsx, const double* xbins,
				 unsigned nbinsy, const double& ylow, const double& yup,
				 const char* option=" " );
  static TProfile2D_LW * create( const char* name, const char* title,
				 unsigned nbinsx, const double& xlow, const double& xup,
				 unsigned nbinsy, const double* ybins, const char* option=" " );
  static TProfile2D_LW * create( const char* name, const char* title,
				 unsigned nbinsx, const double* xbins,
				 unsigned nbinsy, const double* ybins, const char* option=" " );

  void Fill(const double& x, const double& y, const double& z );
  void Fill(const double& x, const double& y, const double& z, const double& w );
  unsigned GetNbinsX() const;
  unsigned GetNbinsY() const;
  //  double GetBinEntries(unsigned binx, unsigned biny) const; (not provided in ROOT)
  double GetBinContent(unsigned binx, unsigned biny) const;
  double GetBinError(unsigned binx, unsigned biny) const;
  //  void SetBinEntries(unsigned binx, unsigned biny, const double& ); (not provided in ROOT)
  void SetBinContent(unsigned binx, unsigned biny, const double& );
  void SetBinError(unsigned binx, unsigned biny, const double& );
  unsigned GetEntries() const;
  void SetEntries(unsigned);

  void Reset();

  double getXMin() const;
  double getXMax() const;
  double getYMin() const;
  double getYMax() const;
  void GetBinInfo( unsigned binx, unsigned biny,
		   double& entries, double& content, double&error ) const;
  void SetBinInfo( unsigned binx, unsigned biny,
		   const double& entries,const double& content, const double& error );

  void SetErrorOption(const char* option);
  const char* GetErrorOption() const;

  void getSums( double& sumW, double& sumW2,
		double& sumWX,double& sumWX2,
		double& sumWY, double& sumWY2,
		double& sumWXY,
		double& sumWZ, double&sumWZ2) const;
  void setSums( const double& sumW, const double&sumW2,
		const double& sumWX, const double& sumWX2,
		const double& sumWY, const double& sumWY2,
		const double& sumWXY,
		const double& sumWZ, const double&sumWZ2 );

  TProfile2D* getROOTHist();
  TH1* getROOTHistBase();

  double Integral() const;
private:
  friend class LWHistInt;
  friend class LWHistVal;
  void clear();
  TH1* getROOTHistBaseNoAlloc() const;
  void clearKeptROOTHist();//Does nothing if root-backend.
  const float * getVarBinsX() const;//null if fixed bin-widths
  const float * getVarBinsY() const;//null if fixed bin-widths
  static TProfile2D_LW * actualcreate( const char* name, const char* title,
				       unsigned nbinsx, const double& xlow, const double& xup, const double* xbins,
				       unsigned nbinsy, const double& ylow, const double& yup, const double* ybins,
				       const double& zlow, const double& zup, const char* option );
  TProfile2D_LW( const char* name, const char* title,
		 unsigned nbinsx, const double& xlow, const double& xup, const double* xbins,
		 unsigned nbinsy, const double& ylow, const double& yup, const double* ybins,
		 const double& zlow, const double& zup, const char* option, bool rootbackend );
  virtual ~TProfile2D_LW();
  TProfile2D_LW( const TProfile2D_LW & );
  TProfile2D_LW & operator= ( const TProfile2D_LW & );
  Flex2DProfileHisto * m_flexHisto;
  TProfile2D * m_rootHisto;
  virtual double actualGetBinCenterX(int bin) const;
  virtual double actualGetBinCenterY(int) const;
  virtual unsigned actualFindBinX(const double&) const;
  virtual unsigned actualFindBinY(const double&) const;
  virtual unsigned actualGetNBinsX() const { return GetNbinsX(); }
  virtual unsigned actualGetNBinsY() const { return GetNbinsY(); }
};

#endif

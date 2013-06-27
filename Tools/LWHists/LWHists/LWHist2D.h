/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class LWHist2D                            //
//                                                            //
//  Description: Common base for light-weight 2D histograms   //
//                                                            //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef LWHIST2D_H
#define LWHIST2D_H

#include "LWHists/LWHist.h"

class TH2;

class LWHist2D : public LWHist {
public:

  virtual void Fill(const double& x, const double& y) = 0;
  virtual void Fill(const double& x, const double& y, const double& w) = 0;
  virtual unsigned GetNbinsX() const = 0;
  virtual unsigned GetNbinsY() const = 0;

  //Using ROOT convention for bins in the calls below:
  //         bin = 0;       underflow bin
  //         bin = 1;       first bin with low-edge xlow INCLUDED
  //         bin = nbins;   last bin with upper-edge xup EXCLUDED
  //         bin = nbins+1; overflow bin

  virtual double GetBinContent(unsigned binx,unsigned biny) const = 0;
  virtual double GetBinError(unsigned binx, unsigned biny) const = 0;
  virtual void SetBinContent(unsigned binx, unsigned biny, const double& ) = 0;
  virtual void SetBinError(unsigned binx, unsigned biny, const double& ) = 0;
  virtual void SetBins(unsigned nbinsx,double xmin,double xmax,
		       unsigned nbinsy,double ymin,double ymax) = 0;

  //Extensions:
  virtual void GetBinContentAndError(unsigned binx, unsigned biny,double& content, double&error) const = 0;
  virtual void SetBinContentAndError(unsigned binx, unsigned biny,const double& content, const double& error) = 0;
  virtual void resetActiveBinLoop() = 0;
  virtual bool getNextActiveBin(unsigned& binx, unsigned& biny, double& content, double& error) = 0;

  virtual double getXMin() const = 0;
  virtual double getXMax() const = 0;
  virtual double getYMin() const = 0;
  virtual double getYMax() const = 0;

  virtual void getSums( double& sumW, double& sumW2,
		        double& sumWX,double& sumWX2,
		        double& sumWY, double& sumWY2,
		        double& sumWXY) const = 0;
  virtual void setSums( const double& sumW, const double& sumW2,
			const double& sumWX, const double& sumWX2,
			const double& sumWY, const double& sumWY2,
			const double& sumWXY ) = 0;

  virtual void scaleContentsAndErrors( const double& fact ) = 0;//C.f. comment in LWHist1D.h

protected:
  LWHist2D( const char* n, const char* t, bool rb ) : LWHist(n,t,rb) {}
  virtual ~LWHist2D() {}
  friend class LWHist::LWHistAxis;
  virtual unsigned actualGetNBinsX() const { return GetNbinsX(); }
  virtual unsigned actualGetNBinsY() const { return GetNbinsY(); }

  virtual bool apply(TH1*) const;
};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class LWHist1D                            //
//                                                            //
//  Description: Common base for light-weight 1D histograms   //
//                                                            //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef LWHIST1D_H
#define LWHIST1D_H

#include "LWHists/LWHist.h"

class LWHist1D : public LWHist {
public:

  virtual void Fill(const double& x) = 0;
  virtual void Fill(const double& x, const double& w) = 0;
  virtual unsigned GetNbinsX() const = 0;

  //Using ROOT convention for bins in the calls below:
  //         bin = 0;       underflow bin
  //         bin = 1;       first bin with low-edge xlow INCLUDED
  //         bin = nbins;   last bin with upper-edge xup EXCLUDED
  //         bin = nbins+1; overflow bin

  virtual double GetBinContent(unsigned bin) const = 0;
  virtual double GetBinError(unsigned bin) const = 0;
  virtual void SetBinContent(unsigned bin, const double& ) = 0;
  virtual void SetBinError(unsigned bin, const double& ) = 0;
  virtual void SetBins(unsigned nbins,double xmin,double xmax) = 0;

  //Extensions:
  virtual void GetBinContentAndError(unsigned bin,double& content, double&error) const = 0;
  virtual void SetBinContentAndError(unsigned bin,const double& content, const double& error) = 0;
  virtual void resetActiveBinLoop() = 0;
  virtual bool getNextActiveBin(unsigned& bin, double& content, double& error) = 0;

  virtual double getXMin() const = 0;
  virtual double getXMax() const = 0;
  virtual void getSums( double& sumW, double& sumW2,
		        double& sumWX,double& sumWX2 ) const = 0;
  virtual void setSums( const double& sumW,const double&sumW2,
 			const double& sumWX,const double& sumWX2 ) = 0;
  
  virtual void scaleContentsAndErrors( const double& fact ) = 0;//Scales all contents and errors by the
                                                                //factor (like changing unit on the weights).
                                                                //Triggers separate bookkeeping of errors if
                                                                //not already present.

protected:
  LWHist1D( const char* n, const char* t, bool rb ) : LWHist(n,t,rb) {}
  virtual ~LWHist1D() {}
  friend class LWHist::LWHistAxis;
  virtual bool apply(TH1*) const;
  virtual double actualGetBinCenterY(int) const { return 0; }
  virtual unsigned actualFindBinY(const double&) const { return 0; }
  virtual unsigned actualGetNBinsX() const { return GetNbinsX(); }
  virtual unsigned actualGetNBinsY() const { return 0; }
};

#endif

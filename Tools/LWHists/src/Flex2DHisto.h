/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class Flex2DHisto                         //
//                                                            //
//  Description: Templated 2D histo class, providing most     //
//               work for the 2D lightweight classes.         //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef FLEX2DHISTO_H
#define FLEX2DHISTO_H

#include "FlexErrArray.h"
#include "LWBinUtils.h"
#ifndef NDEBUG
#include <iostream>
#endif

#ifdef LW_DEBUG_HEAVY_USERS
#include "LWHistTraceUtils.h"
#include <map>
#include <iostream>
#endif

template <class T>
class Flex2DHisto {
public:

  static Flex2DHisto * create(unsigned nbinsx, const double& xmin, const double& xmax,
			      unsigned nbinsy, const double& ymin, const double& ymax);
  template <class TFloat>
  static Flex2DHisto * create(unsigned nbinsx, const TFloat*xbins,
			      unsigned nbinsy, const double& ymin, const double& ymax);
  template <class TFloat>
  static Flex2DHisto * create(unsigned nbinsx, const double& xmin, const double& xmax,
			      unsigned nbinsy, const TFloat* ybins);
  template <class TFloat>
  static Flex2DHisto * create(unsigned nbinsx, const TFloat* xbins,
			      unsigned nbinsy, const TFloat* ybins);

  static void destroy(Flex2DHisto *);

  double getXMin() const { return m_xmin; }
  double getXMax() const { return m_xmax; }
  double getYMin() const { return m_ymin; }
  double getYMax() const { return m_ymax; }

  void fill(const double& x, const double& y);
  void fill(const double& x, const double& y, const double& w);
  unsigned getNBinsX() const { return m_nbinsxPlus1-1; }
  unsigned getNBinsY() const { return m_nbinsyPlus1-1; }
  const float * getVarBinsX() const { return m_varBinsX; }
  const float * getVarBinsY() const { return m_varBinsY; }

  //Bin numbers in the next methods are in 0..m_nbins+1 (i.e. with
  //over/underflow bins):
  unsigned valueToXBin(const double& x) const;
  unsigned valueToYBin(const double& y) const;
  double getBinContent(unsigned binx, unsigned biny) const;
  double getBinError(unsigned binx, unsigned biny) const;
  void getBinContentAndError(unsigned binx, unsigned biny, double& cont, double& err ) const;
  void setBinContent(unsigned binx, unsigned biny, const double& );
  void setBinError(unsigned binx, unsigned biny, const double& );
  void setBinContentAndError(unsigned binx, unsigned biny, const double& cont, const double& err );

  void setEntries(unsigned);
  unsigned getEntries() const;//Count of fill(..) and setBinContent(..) calls.
  double getSumW() const;//Total Sum of weights from fill(..) calls.
  double getSumW2() const;//Total Sum of squares of weights from fill(..) calls.
  double getSumWX() const;//Total Sum of weight*X from fill(..) calls.
  double getSumWX2() const;//Total Sum of weight*X*X from fill(..) calls.
  double getSumWY() const;//Total Sum of weight*Y from fill(..) calls.
  double getSumWY2() const;//Total Sum of weight*Y*Y from fill(..) calls.
  double getSumWXY() const;//Total Sum of weight*X*Y from fill(..) calls.
  void setSums( const double& sumW, const double&sumW2,const double& sumWX, const double& sumWX2,
		const double& sumWY, const double& sumWY2,const double& sumWXY );

  bool holdsSeparateSumW2Info() const;
  void copyContents(T*cont, double*err=0) const;//Won't copy errors if err==0.
  double getBinCenterX(int bin) const;
  double getBinCenterY(int bin) const;

  double Integral() const;

  void resetActiveBinLoop();
  bool getNextActiveBin(unsigned& binx, unsigned& biny, double& content, double& error);

  void scaleContentsAndErrors( const double& fact );

#ifdef LW_DEBUG_HEAVY_USERS
  std::map<std::pair<void*,void*>,unsigned long> m_callmap;
  void countCall(const std::pair<void*,void*>&addresses);
  void produceReport(const char*histname);
#endif

private:
  //NB: Actual number of allocated bins is (nbins+2)**2 (over/underflow bins)
  //extraAllocSize indicates the size of the memory immediately
  //following the object which will be used for supergroups indices.
  static unsigned extraAllocSize(unsigned nbinsx,unsigned nbinsy) { return FlexErrArray<T>::extraAllocSize((nbinsx+2)*(nbinsy+2)); }
  Flex2DHisto( unsigned nbinsx, const double& xmin, const double& xmax,
	       unsigned nbinsy, const double& ymin, const double& ymax );
  template <class TFloat>
  Flex2DHisto( unsigned nbinsx, const TFloat* xbins,
	       unsigned nbinsy, const double& ymin, const double& ymax );
  template <class TFloat>
  Flex2DHisto( unsigned nbinsx, const double& xmin, const double& xmax,
	       unsigned nbinsy, const TFloat* ybins );
  template <class TFloat>
  Flex2DHisto( unsigned nbinsx, const TFloat* xbins,
	       unsigned nbinsy, const TFloat* ybins );
  ~Flex2DHisto();

  // Illegal to copy/assign a Flex2DHisto:
  Flex2DHisto( const Flex2DHisto & );
  Flex2DHisto & operator= ( const Flex2DHisto & );

  unsigned internal_bin(unsigned binx, unsigned biny) const
  {
    return LWBinUtils::internal_bin(binx,biny,m_nbinsxPlus2);
  }

  //Speedup:
  double m_invDeltaX;
  double m_invDeltaY;

  //Statistics:
  double m_sumW;
  double m_sumW2;
  double m_sumWX;
  double m_sumWX2;
  double m_sumWY;
  double m_sumWY2;
  double m_sumWXY;
  unsigned m_nEntries;

  //nbins:
  const unsigned m_nbinsxPlus1;
  const unsigned m_nbinsyPlus1;
  const unsigned m_nbinsxPlus2;

  //Ranges:
  double m_xmin;
  double m_xmax;
  double m_ymin;
  double m_ymax;
  float * m_varBinsX;//null if constant bin-width, otherwise m_nbinsPlus1 entries (first and last are xmin and xmax)
  float * m_varBinsY;//null if constant bin-width, otherwise m_nbinsPlus1 entries (first and last are ymin and ymax)

  //Array:
  FlexErrArray<T> m_flexArray;//Must be the last

};

#include "Flex2DHisto.icc"

#endif

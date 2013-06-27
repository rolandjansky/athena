/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class Flex1DHisto                         //
//                                                            //
//  Description: Templated 1D histo class, providing most     //
//               work for the 1D lightweight classes.         //
//               Basically just extends FlexErrArray with     //
//               a value-oriented rather than bin-oriented    //
//               interface (xmin,xmax & under/overflow).      //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef FLEX1DHISTO_H
#define FLEX1DHISTO_H

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
class Flex1DHisto {
public:

  static Flex1DHisto * create(unsigned nbins, const double& xmin, const double& xmax);
  template <class TFloat>
  static Flex1DHisto * create(unsigned nbins, const TFloat* xbins );
  static void destroy(Flex1DHisto *);

  double getXMin() const { return m_xmin; }
  double getXMax() const { return m_xmax; }

  void fill(const double& x);
  void fill(const double& x, const double& w);
  unsigned getNBins() const { return m_nbinsPlus1-1; }
  const float * getVarBins() const { return m_varBins; }

  //Bin numbers in the next methods are in 0..m_nbins+1 (i.e. with
  //over/underflow bins):
  unsigned valueToBin(const double& x) const;
  double getBinContent(unsigned bin) const;
  double getBinError(unsigned bin) const;
  void getBinContentAndError(unsigned bin, double& cont, double& err ) const;
  void setBinContent(unsigned bin, const double& );
  void setBinError(unsigned bin, const double& );
  void setBinContentAndError(unsigned bin, const double& cont, const double& err );

  void setEntries(unsigned);
  unsigned getEntries() const;//Count of fill(..) and setBinContent(..) calls.
  double getSumW() const;//Total Sum of weights from fill(..) calls.
  double getSumW2() const;//Total Sum of squares of weights from fill(..) calls.
  double getSumWX() const;//Total Sum of weight*X from fill(..) calls.
  double getSumWX2() const;//Total Sum of weight*X*X from fill(..) calls.
  void setSums( const double& sumW,const double&sumW2,const double& sumWX,const double& sumWX2 );

  bool holdsSeparateSumW2Info() const;
  void copyContents(T*cont, double*err=0) const;//Won't copy errors if err==0.
  double getBinCenter(int bin) const;

  double Integral() const;

  void resetActiveBinLoop();
  bool getNextActiveBin(unsigned& bin, double& content, double& error);

  void scaleContentsAndErrors( const double& fact );

#ifdef LW_DEBUG_HEAVY_USERS
  std::map<std::pair<void*,void*>,unsigned long> m_callmap;
  void countCall(const std::pair<void*,void*>&addresses);
  void produceReport(const char*histname);
#endif

private:

  //NB: Actual number of allocated bins is nbins+2 (over/underflow bins)
  //extraAllocSize indicates the size of the memory immediately
  //following the object which will be used for supergroups indices.
  static unsigned extraAllocSize(unsigned nbins) { return FlexErrArray<T>::extraAllocSize(nbins+2); }
  Flex1DHisto( unsigned nbins, const double& xmin, const double& xmax );
  template <class TFloat>
  Flex1DHisto( unsigned nbins, const TFloat* xbins );//xbins has nbins+1 entries
  ~Flex1DHisto();

  // Illegal to copy/assign a Flex1DHisto:
  Flex1DHisto( const Flex1DHisto & );
  Flex1DHisto & operator= ( const Flex1DHisto & );

  //Speedup:
  double m_invDelta;
  //Statistics:
  double m_sumW;
  double m_sumW2;
  double m_sumWX;
  double m_sumWX2;
  unsigned m_nEntries;

  //nbins:
  unsigned m_nbinsPlus1;

  //Range:
  double m_xmin;
  double m_xmax;
  float * m_varBins;//null if constant bin-width, otherwise m_nbinsPlus1 entries (first and last are xmin and xmax)
  //Array:
  FlexErrArray<T> m_flexArray;//Must be the last!

};

#include "Flex1DHisto.icc"

#endif

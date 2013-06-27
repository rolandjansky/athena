/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class Flex2DProfileHisto                  //
//                                                            //
//  Description: 2D profile histo class.                      //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2009                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef FLEX2DPROFILEHISTO_H
#define FLEX2DPROFILEHISTO_H

#include "FlexProfileArray.h"
#include "LWHistBitUtils.h"
#include "LWBinUtils.h"

#ifdef LW_DEBUG_HEAVY_USERS
#include "LWHistTraceUtils.h"
#include <map>
#include <iostream>
#endif

class Flex2DProfileHisto {
public:

  static Flex2DProfileHisto * create(unsigned nbinsx, const double& xmin, const double& xmax, const double* xbins,
				     unsigned nbinsy, const double& ymin, const double& ymax, const double* ybins,
				     const double& profparmin, const double& profparmax );
  static void destroy(Flex2DProfileHisto *);

  double getXMin() const { return m_xmin; }
  double getXMax() const { return m_xmax; }
  double getYMin() const { return m_ymin; }
  double getYMax() const { return m_ymax; }

  const float * getVarBinsX() const { return m_varBinsX; }
  const float * getVarBinsY() const { return m_varBinsY; }

  void fill(const double& x, const double& y, const double& profiledpar);
  void fill(const double& x, const double& y, const double& profiledpar, const double& weight);
  double getBinEntries(unsigned binx,unsigned biny) const { return m_flexArray.getBinEntries(internal_bin(binx,biny)); }
  double getBinContent(unsigned binx,unsigned biny) const { return m_flexArray.getBinContent(internal_bin(binx,biny)); }
  double getBinError(unsigned binx,unsigned biny) const { return m_flexArray.getBinError(internal_bin(binx,biny)); }
  void getBinInfo(unsigned binx,unsigned biny, double& entries, double& content, double& error ) const;
  void setBinEntries(unsigned binx,unsigned biny, const double& e ) { m_flexArray.setBinEntries(internal_bin(binx,biny),e); }
  void setBinContent(unsigned binx,unsigned biny, const double& c );
  void setBinError(unsigned binx,unsigned biny, const double& e );
  void setBinInfo(unsigned binx,unsigned biny, const double& entries, const double& content, const double& error );
  void reset();
  unsigned getNBinsX() const { return m_nbinsxPlus1-1; }
  unsigned getNBinsY() const { return m_nbinsyPlus1-1; }
  double getBinCenterX(int bin) const;
  double getBinCenterY(int bin) const;

  unsigned getEntries() const;
  void setEntries(unsigned n);

  void setErrorOptionFromString(const char *option) { m_flexArray.setErrorOptionFromString(option); }
  const char* getErrorOptionAsString() const { return m_flexArray.getErrorOptionAsString(); }

  double integral() const;

  double getSumW() const { return m_sumW; }
  double getSumW2() const { return m_sumW2; }
  double getSumWX() const { return m_sumWX; }
  double getSumWX2() const { return m_sumWX2; }
  double getSumWY() const { return m_sumWY; }
  double getSumWY2() const { return m_sumWY2; }
  double getSumWXY() const { return m_sumWXY; }
  double getSumWProfPar() const { return m_sumWProfiledPar; }
  double getSumWProfPar2() const { return m_sumWProfiledPar2; }
  double getProfParMin() const { return m_profParMin; }
  double getProfParMax() const { return m_profParMax; }
  void setSums( const double& sumW,const double&sumW2,
		const double& sumWX,const double& sumWX2,
		const double& sumWY, const double& sumWY2,const double& sumWXY,
		const double& sumWProfPar,const double& sumWProfPar2 );
  void copyContents(double* entries, double*contents, double*errors) const;

  unsigned valueToXBin(const double& x) const;
  unsigned valueToYBin(const double& y) const;

#ifdef LW_DEBUG_HEAVY_USERS
  std::map<std::pair<void*,void*>,unsigned long> m_callmap;
  void countCall(const std::pair<void*,void*>&addresses);
  void produceReport(const char*histname);
#endif

private:

  static unsigned allocSize(unsigned nbinsx,unsigned nbinsy);
  Flex2DProfileHisto( unsigned nbinsx, const double& xmin, const double& xmax, const double* xbins,
		      unsigned nbinsy, const double& ymin, const double& ymax, const double* ybins,
		      const double& profparmin, const double& profparmax);
  ~Flex2DProfileHisto();
  Flex2DProfileHisto( const Flex2DProfileHisto & );
  Flex2DProfileHisto & operator= ( const Flex2DProfileHisto & );


  bool profParOutsideRange(const double& profpar) const {
    return m_profParMin != m_profParMax
      && (profpar <m_profParMin || profpar> m_profParMax);
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
  double m_sumWProfiledPar;
  double m_sumWProfiledPar2;
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
  double m_profParMin;
  double m_profParMax;
  float * m_varBinsX;//null if constant bin-width, otherwise m_nbinsxPlus1 entries (first and last are xmin and xmax)
  float * m_varBinsY;//null if constant bin-width, otherwise m_nbinsyPlus1 entries (first and last are ymin and ymax)

  //Array:
  FlexProfileArray m_flexArray;//Must be the last!

  //Helpers:
  unsigned internal_bin(unsigned binx, unsigned biny) const
  {
    assert(binx<=m_nbinsxPlus1);
    assert(biny<=m_nbinsyPlus1);
    return LWBinUtils::internal_bin(binx,biny,m_nbinsxPlus2);
  }

};

#include "Flex2DProfileHisto.icc"

#endif

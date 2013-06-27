/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class Flex1DProfileHisto                  //
//                                                            //
//  Description: 1D profile histo class.                      //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2009                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef FLEX1DPROFILEHISTO_H
#define FLEX1DPROFILEHISTO_H

#include "FlexProfileArray.h"
#include "LWHistBitUtils.h"
#include "LWBinUtils.h"

#ifdef LW_DEBUG_HEAVY_USERS
#include "LWHistTraceUtils.h"
#include <map>
#include <iostream>
#endif

class Flex1DProfileHisto {
public:

  static Flex1DProfileHisto * create(unsigned nbins, const double& xmin, const double& xmax,
				     const double& profparmin, const double& profparmax );
  template <class TFloat>
  static Flex1DProfileHisto * create(unsigned nbins, const TFloat* xbins,
				     const double& profparmin, const double& profparmax );
  static void destroy(Flex1DProfileHisto *);

  double getXMin() const { return m_xmin; }
  double getXMax() const { return m_xmax; }
  const float * getVarBins() const { return m_varBins; }

  void fill(const double& x, const double& profiledpar);
  void fill(const double& x, const double& profiledpar, const double& weight);
  double getBinEntries(unsigned bin) const { return m_flexArray.getBinEntries(bin); }
  double getBinContent(unsigned bin) const;
  double getBinError(unsigned bin) const;
  void getBinInfo(unsigned bin, double& entries, double& content, double& error ) const;
  void setBinEntries(unsigned bin, const double& e );
  void setBinContent(unsigned bin, const double& c );
  void setBinError(unsigned bin, const double& e );
  void setBinInfo(unsigned bin, const double& entries, const double& content, const double& error );
  void reset();
  unsigned getNBins() const { return m_nbinsPlus1-1; }
  double getBinCenter(int bin) const;

  unsigned getEntries() const;
  void setEntries(unsigned n);

  void setErrorOptionFromString(const char *option) { m_flexArray.setErrorOptionFromString(option); }
  const char* getErrorOptionAsString() const { return m_flexArray.getErrorOptionAsString(); }

  double integral() const;

  double getSumW() const { return m_sumW; }
  double getSumW2() const { return m_sumW2; }
  double getSumWX() const { return m_sumWX; }
  double getSumWX2() const { return m_sumWX2; }
  double getSumWProfPar() const { return m_sumWProfiledPar; }
  double getSumWProfPar2() const { return m_sumWProfiledPar2; }
  double getProfParMin() const { return m_profParMin; }
  double getProfParMax() const { return m_profParMax; }

  void setSums( const double& sumW,const double&sumW2,
		const double& sumWX,const double& sumWX2,
		const double& sumWProfPar,const double& sumWProfPar2 );
  void copyContents(double* entries, double*contents, double*errors) const;

  unsigned valueToBin(const double& x) const;

#ifdef LW_DEBUG_HEAVY_USERS
  std::map<std::pair<void*,void*>,unsigned long> m_callmap;
  void countCall(const std::pair<void*,void*>&addresses);
  void produceReport(const char*histname);
#endif

private:

  static unsigned allocSize(unsigned nbins);
  Flex1DProfileHisto( unsigned nbins, const double& xmin, const double& xmax,
		      const double& profparmin, const double& profparmax );
  template <class TFloat>
  Flex1DProfileHisto( unsigned nbins, const TFloat* xbins,
		      const double& profparmin, const double& profparmax );

  ~Flex1DProfileHisto();
  Flex1DProfileHisto( const Flex1DProfileHisto & );
  Flex1DProfileHisto & operator= ( const Flex1DProfileHisto & );

  bool profParOutsideRange(const double& profpar) const {
    return m_profParMin != m_profParMax
      && (profpar <m_profParMin || profpar> m_profParMax);
  }

  //Speedup:
  double m_invDelta;

  //Statistics:
  double m_sumW;
  double m_sumW2;
  double m_sumWX;
  double m_sumWX2;
  double m_sumWProfiledPar;
  double m_sumWProfiledPar2;
  unsigned m_nEntries;

  //nbins:
  unsigned m_nbinsPlus1;

  //Range:
  double m_xmin;
  double m_xmax;
  float * m_varBins;
  double m_profParMin;
  double m_profParMax;
  //float * m_varBins;//null if constant bin-width, otherwise m_nbinsPlus1 entries (first and last are xmin and xmax)

  //Array:
  FlexProfileArray m_flexArray;//Must be the last!
};

#include "Flex1DProfileHisto.icc"

#endif

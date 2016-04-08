/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBUTILS_TB2DPROFILER_H
#define TBUTILS_TB2DPROFILER_H

#include "TBUtils/TBProfiler.h"

#include <vector>

template<typename T>
class TB2DProfiler
{
 public:

  typedef std::vector< std::vector<T> >   matrix_t;

  TB2DProfiler();
  TB2DProfiler(size_t xBins, const T& xLow, const T& xHigh,
	       size_t yBins, const T& yLow, const T& yHigh);
  TB2DProfiler(const TB2DProfiler&) = delete;
  TB2DProfiler& operator=(const TB2DProfiler&) = delete;

  virtual ~TB2DProfiler();

  void addData(const T& xPos, const T& yPos, const T& theData);
  void addData(size_t i, size_t j, const T& theData);
  
  bool getAverages(size_t i, size_t j, T& theData)               const;
  bool getAverages(const T& x, const T& y, T& theData)           const;
  bool getStandardDevs(size_t i, size_t j, T& theData)           const;
  bool getStandardDevs(const T&x, const T& y, T& theData)        const;
  bool getErrors(size_t i, size_t j, T& theData)                 const;
  bool getErrors(const T& x, const T& y, T& theData)             const;
  bool getBinEntries(size_t i, size_t j, size_t& theEntries)     const;
  bool getBinEntries(const T& x, const T& y, size_t& theEntries) const;

  //  size_t getEntries(size_t i, size_t j)    const;
  size_t getXIndex(const T& x)             const;
  size_t getYIndex(const T& y)             const;
  size_t getNumberOfChannels()             const;

  const TBProfiler<T>& getXBinning()           const;
  const TBProfiler<T>& getYBinning()           const; 

 protected:

  TBProfiler<T>* m_xBinning;
  TBProfiler<T>* m_yBinning;
  TBProfiler<T>* m_store;

  size_t getIndex(const T& x, const T& y) const;
  size_t getIndex(size_t i, size_t j)     const;

  bool m_skipFlag;
};

#include "TBUtils/TB2DProfiler.icc"

#endif

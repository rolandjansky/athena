/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBUTILS_TBPROFILER_H
#define TBUTILS_TBPROFILER_H

#include <vector>
#include <cmath>

template<typename T> 
class TBProfiler
{
  public:

    TBProfiler();
    TBProfiler(size_t storeSize);
    TBProfiler(size_t storeSize, const T& lowerLimit, const T& upperLimit);

    virtual ~TBProfiler();


    void addData(const T& theChannel, const T& theData);                
    void addData(const std::vector<T>&  theData);
    void addData(size_t index, const T& theData);

    bool getAverages(std::vector<T>& theAverages)            const;
    bool getAverages(size_t theIndex,T& theAverage)          const;
    bool getStandardDevs(std::vector<T>& theVariances)       const;
    bool getStandardDevs(size_t theIndex, T& theDev)         const;
    bool getErrors(std::vector<T>& theErrors)                const;
    bool getErrors(size_t theIndex, T& theError)             const;
    bool getBinEntries(std::vector<size_t>& theEntries)      const;
    bool getBinEntries(size_t theIndex, size_t& theEntries ) const;

    const std::vector<T>&      getSums()               const;
    const std::vector<T>&      getSumOfSquares()       const;
    const std::vector<size_t>& getEntries()            const;

    size_t getNumberOfChannels()                       const;
    T getChannelWidth()                                const;
    T getMinChannel()                                  const;
    T getMaxChannel()                                  const;
    size_t getIndex(const T& theChannel)               const;

 protected:

    std::vector<T>      m_sum;
    std::vector<T>      m_sumOfSquares;
    std::vector<size_t> m_entries;

    bool setupAction(size_t theSize);

 private:

    bool           m_channelFill;
    bool           m_skipFlag;
    std::vector<T> m_binLimits;
    T              m_deltaBin;
};

#include "TBUtils/TBProfiler.icc"

#endif

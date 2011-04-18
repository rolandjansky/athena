/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBUTILS_TBSIGNALHELPER_H
#define TBUTILS_TBSIGNALHELPER_H

#include <vector>

namespace TBSignalHelper 
{
  template<typename T>
  class DataProfiler
    {
    public:

      DataProfiler(unsigned int noChannels)
	{
	  m_sumStore.resize(noChannels,0);
	  m_rmsStore.resize(noChannels,0);
	  m_ctrStore.resize(noChannels,0);
	}

      virtual ~DataProfiler();

      virtual void collect(std::vector<T> theValues)
	{
	}

      virtual void collect(unsigned int theChannel, T theValue);

      virtual std::vector<T> getAverages()     const;
      virtual T              getAverages(
      virtual std::vector<T> getSum()          const;
      virtual std::vector<T> getSumOfSquares() const;

      virtual unsigned int 

    };
#endif

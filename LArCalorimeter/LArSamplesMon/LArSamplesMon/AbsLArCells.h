/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArSamples::AbsLArCells
   @brief A base class for accessing ntuple data
*/

#ifndef LArSamples_AbsLArCells_H
#define LArSamples_AbsLArCells_H

#include "LArCafJobs/Definitions.h"
#include <vector>

namespace LArSamples {
  
  class CellInfo;
  class History;
  class FilterParams;
  
  class AbsLArCells  {
  
    public:
      
      AbsLArCells();
      virtual ~AbsLArCells();
      
      virtual const History* newCellHistory(unsigned int i) const;
      virtual const History* cellHistory(unsigned int i) const;
      virtual const CellInfo* cellInfo(unsigned int i) const;
      virtual unsigned int nChannels() const { return Definitions::nChannels; }

      virtual void resetCache() const;

      const CellInfo* cellInfoCache(unsigned int i) const;
      const History* pass(unsigned int i, const FilterParams& f) const;

      virtual const History* getCellHistory(unsigned int i) const = 0;
      virtual const CellInfo* getCellInfo(unsigned int i) const;

      const History* cellCache() const { return m_cellCache; }
      unsigned int cachePos() const { return m_pos; }

      void resetCellInfoCache();

    private:

      mutable unsigned int m_pos;
      mutable const History* m_cellCache;
      mutable std::vector<CellInfo*> m_cellInfoCache;

  };
}
  
#endif

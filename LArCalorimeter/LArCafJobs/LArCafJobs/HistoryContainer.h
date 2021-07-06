/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArSamplesHistoryContainer
   @brief storage of a collection of digits for one cell
*/

#include "TString.h"
#include "TArrayI.h"
#include <vector>
#include "AthenaKernel/CLASS_DEF.h"

#ifndef LArSamples_HistoryContainer_H
#define LArSamples_HistoryContainer_H

class LArShapeDumper;

namespace LArSamples {
  
  class D3PDConverter;
  class DataContainer;
  class SimpleShape;
  class CellInfo;
  class OFC;
  
  class HistoryContainer  {
  
   public:
   
    /** @brief Constructor (takes ownership of LArCellInfo object) */
    HistoryContainer(CellInfo* cellInfo = 0);
    HistoryContainer(const HistoryContainer& other);

    /** @brief Destructor */
    virtual ~HistoryContainer();
  
    unsigned int nDataContainers() const { return m_data.size(); }
    const DataContainer* dataContainer(unsigned int i) const { return m_data[i]; }
  
    const CellInfo* cellInfo() const { return m_cellInfo.get(); }
    
    /** @brief append data (takes ownership) */
    void add(const DataContainer* data) { m_data.push_back(data); }
    
    bool isValid() const;
    
    double footprint() const;
   
    friend class ::LArShapeDumper;
    friend class D3PDConverter;

   private:
   
    CellInfo* cell_info() const { return m_cellInfo.get(); }

    std::vector<const DataContainer*> m_data;
    std::unique_ptr<CellInfo> m_cellInfo;

    HistoryContainer& operator= (const HistoryContainer&);
  };
}

CLASS_DEF(LArSamples::HistoryContainer,1147380099,0)

#endif

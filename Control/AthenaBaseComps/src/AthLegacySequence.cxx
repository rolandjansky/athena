/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
// AthenaBaseComps includes
#include "AthenaBaseComps/AthLegacySequence.h"
#include "AthAlgorithmDHUpdate.h"


namespace Athena {
  namespace details {
    AthLegacySequenceAdapter::AthLegacySequenceAdapter(const std::string& name, 
                                         ISvcLocator* pSvcLocator): 
      ::AthCommonDataStore<AthCommonMsg<Gaudi::Sequence>> ( name, pSvcLocator)
    {
      setProperty("Cardinality", 1);
      m_updateDataHandles =
        std::make_unique<AthenaBaseComps::AthAlgorithmDHUpdate>
        (m_extendedExtraObjects,
         std::move (m_updateDataHandles));        
    }
    

    const DataObjIDColl& 
    AthLegacySequenceAdapter::extraOutputDeps() const {
      if (!m_extendedExtraObjects.empty()) {
        return m_extendedExtraObjects;
      }
      return Gaudi::Sequence::extraOutputDeps();
    }
  }

}

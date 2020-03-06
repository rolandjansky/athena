/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/GaudiException.h"

#include "AthenaBaseComps/AthLegacySequence.h"
#include "AthAlgorithmDHUpdate.h"


namespace Athena {
  namespace details {
    AthLegacySequenceAdapter::AthLegacySequenceAdapter(const std::string& name, 
                                         ISvcLocator* pSvcLocator): 
      ::AthCommonDataStore<AthCommonMsg<Gaudi::Sequence>> ( name, pSvcLocator)
    {
      StatusCode sc = setProperty("Cardinality", 1);
      if ( sc.isFailure() ) {
        throw GaudiException("Failed to set property Cardinality",
                             "AthLegacySequenceAdapter", sc);
      }
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

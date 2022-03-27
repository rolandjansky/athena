/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef TRIGCONFIGSVC__BUNCHGROUPCONDALG
#define TRIGCONFIGSVC__BUNCHGROUPCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "CxxUtils/checker_macros.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "TrigConfData/L1BunchGroupSet.h"
#include <tbb/concurrent_unordered_map.h>


namespace TrigConf {

   class BunchGroupCondAlg : public AthReentrantAlgorithm {
   public:
      BunchGroupCondAlg (const std::string& name, ISvcLocator* pSvcLocator);
      virtual StatusCode initialize() override;
      virtual StatusCode execute(const EventContext& ctx) const override;

      // avoids running CondAlg multiple times for the same input (ATEAM-617)
      virtual bool isReEntrant() const override final { return false; }
    private:
      // helper function to load a L1BunchGroupSet set from a file
      std::shared_ptr<L1BunchGroupSet> createFromFile( const std::string & filename ) const;

      // helper function to load a L1BunchGroupSet 
      std::shared_ptr<L1BunchGroupSet> createFromDB( unsigned int ) const;

      // map the key to a L1BunchGroupSet
      mutable tbb::concurrent_unordered_map<unsigned int, std::shared_ptr<const L1BunchGroupSet>> m_BgsMap ATLAS_THREAD_SAFE;

      // input key to the BunchGroupKey folder
      SG::ReadCondHandleKey<AthenaAttributeList> m_bgkFolderInputKey{ this, "BGKFolder", "/TRIGGER/LVL1/BunchGroupKey", "SG Key of AthenaAttributeList bgk"};

      // output key to store the L1BunchGroup
      SG::WriteCondHandleKey<TrigConf::L1BunchGroupSet> m_l1BunchGroupSetOutputKey{ this, "L1BunchGroup", "L1BunchGroup", "L1 BunchGroups"};

      // properties
      Gaudi::Property< std::string > m_configSource { this, "Source", "FILE", "Configuration source, can be 'FILE', 'DB', or 'COOL'" };
      Gaudi::Property< std::string > m_dbConnection { this, "TriggerDB", "", "DB connection alias or 'JOSVC', used when property Source set to 'DB' or set to 'COOL'." };
      Gaudi::Property< unsigned int > m_bgk { this, "BGSK", 0, "L1 BunchGroup key, used when property 'Source' set to 'DB' or 'FILE'" };
      Gaudi::Property< std::string > m_filename { this, "Filename", "", "L1BunchGroupSet json file, used when property 'Source' set to 'FILE'" };


   };

}

#endif

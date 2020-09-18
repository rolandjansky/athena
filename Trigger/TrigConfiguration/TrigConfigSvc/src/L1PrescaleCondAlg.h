/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef TRIGCONFIGSVC__L1PRESCALECONDALG
#define TRIGCONFIGSVC__L1PRESCALECONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "TrigConfData/L1PrescalesSet.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include <tbb/concurrent_unordered_map.h>

#include <memory>

namespace TrigConf {

   /**
    * @brief Condition algorithm to provide the L1 trigger menu
    *
    * The algorithm will provide the menu configuration through a conditions handle using \c TrigConf::L1Menu to present the data
    *
    * It is able to load the configuration in the following 3 ways
    * * File-based menu description: for most test environments: for test jobs with no database involved
    * * DB-based menu description where the DB keys are provided as algorithm job-properties: for reprocessing grid jobs with new menus
    * * DB-based menu description where the DB keys are taken from COOL: for offline reconstruction jobs
    */
   class L1PrescaleCondAlg : public AthReentrantAlgorithm {
   public:
      L1PrescaleCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
      virtual ~L1PrescaleCondAlg() = default;

      virtual StatusCode initialize() override;
      virtual StatusCode execute(const EventContext& ctx) const override;

   private:
      
      // helper function to load a L1 prescales set from a file
      std::shared_ptr<L1PrescalesSet> createFromFile( const std::string & filename ) const;

      // helper function to load a L1 prescales set from a prescale key
      std::shared_ptr<L1PrescalesSet> createFromDB( unsigned int psk, bool isRun3 ) const;

      // map the prescale key to a L1PrescalesSet
      mutable tbb::concurrent_unordered_map<unsigned int, std::shared_ptr<L1PrescalesSet>> m_pssMap;

      // input key to the L1 Prescale Key folder
      SG::ReadCondHandleKey<AthenaAttributeList> m_pskFolderInputKey{ this, "PSKFolder", "/TRIGGER/LVL1/Lvl1ConfigKey", "SG Key of AthenaAttributeList containing l1 psk"};

      // output key to store the L1PrescalesSet
      SG::WriteCondHandleKey<TrigConf::L1PrescalesSet> m_l1PrescalesSetOutputKey{ this, "L1Prescales", "L1Prescales", "L1 prescales"};

      // properties
      Gaudi::Property< std::string > m_configSource { this, "Source", "FILE", "Configuration source, can be 'FILE', 'DB', or 'COOL'" };
      Gaudi::Property< std::string > m_dbConnection { this, "TriggerDB", "", "DB connection alias or 'JOSVC', used when property Source set to 'DB' or set to 'COOL'." };
      Gaudi::Property< unsigned int > m_psk { this, "L1Psk", 0, "L1 prescale key, used when property 'Source' set to 'DB'" };
      Gaudi::Property< std::string > m_filename { this, "Filename", "", "L1 prescale json file, used when property 'Source' set to 'FILE'" };

   };

}
#endif

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef TRIGCONFIGSVC__HLTPRESCALECONDALG
#define TRIGCONFIGSVC__HLTPRESCALECONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "CxxUtils/checker_macros.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "TrigConfData/HLTPrescalesSet.h"

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
   class HLTPrescaleCondAlg : public AthReentrantAlgorithm {
   public:
      HLTPrescaleCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
      virtual ~HLTPrescaleCondAlg() = default;

      virtual StatusCode initialize() override;
      virtual StatusCode execute(const EventContext& ctx) const override;

      // avoids running CondAlg multiple times for the same input (ATEAM-617)
      virtual bool isReEntrant() const override final { return false; }

   private:
      
      // helper function to load a HLT prescales set from a file
      std::shared_ptr<HLTPrescalesSet> createFromFile( const std::string & filename ) const;

      // helper function to load a HLT prescales set from a prescale key
      std::shared_ptr<HLTPrescalesSet> createFromDB( unsigned int psk, bool isRun3 ) const;

      // map the prescale key to a HLTPrescalesSet
      mutable tbb::concurrent_unordered_map<unsigned int, std::shared_ptr<const HLTPrescalesSet>> m_pssMap ATLAS_THREAD_SAFE;

      // input key to the HLT Prescale Key folder
      SG::ReadCondHandleKey<AthenaAttributeList> m_pskFolderInputKey{ this, "PSKFolder", "/TRIGGER/HLT/PrescaleKey", "SG Key of AthenaAttributeList containing hlt psk"};

      // output key to store the HLTPrescalesSet
      SG::WriteCondHandleKey<TrigConf::HLTPrescalesSet> m_hltPrescalesSetOutputKey{ this, "HLTPrescales", "HLTPrescales", "HLT prescales"};

      // properties
      Gaudi::Property< std::string > m_configSource { this, "Source", "FILE", "Configuration source, can be 'FILE', 'DB', or 'COOL'" };
      Gaudi::Property< std::string > m_dbConnection { this, "TriggerDB", "", "DB connection alias or 'JOSVC', used when property Source set to 'DB' or set to 'COOL'." };
      Gaudi::Property< unsigned int > m_psk { this, "HLTPsk", 0, "HLT prescale key, used when property 'Source' set to 'DB' or 'FILE'" };
      Gaudi::Property< std::string > m_filename { this, "Filename", "", "HLT prescale json file, used when property 'Source' set to 'FILE'" };

   };

}
#endif

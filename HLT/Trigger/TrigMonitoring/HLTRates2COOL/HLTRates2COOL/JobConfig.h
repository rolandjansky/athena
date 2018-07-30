/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef hltca_JobConfig
#define hltca_JobConfig

#include <iostream>

namespace hltca {
   class JobConfig;
   std::ostream& operator<<( std::ostream& os, const JobConfig& event );

   class JobConfig {
      friend std::ostream& operator<<( std::ostream& os, const JobConfig& event );
   public:
      JobConfig();
      ~JobConfig();

      std::string&     PartitionName()    { return m_partitionName; }
      std::string&     CoolDb()           { return m_coolDb; }
      std::string&     ISServer()         { return m_ISServer; }
      std::string&     Provider(const std::string& lvl) { return m_provider[lvl=="L2"?0:1]; }
      std::string&     Histogram(const std::string& lvl) { return m_histogram[lvl=="L2"?0:1]; }
      uint32_t&        CurrentRunNumber() { return m_currentRunNumber; }
      bool&            COOLconnStat()     { return m_COOLconnStat; }

   private:
      std::string        m_partitionName;
      std::string        m_coolDb;
      std::string        m_ISServer;
      std::string        m_provider[2];
      std::string        m_histogram[2];
      uint32_t           m_currentRunNumber;
      bool               m_COOLconnStat;
   };
}
#endif

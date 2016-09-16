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

      std::string&     PartitionName()    { return fPartitionName; }
      std::string&     CoolDb()           { return fCoolDb; }
      std::string&     ISServer()         { return fISServer; }
      std::string&     Provider(const std::string& lvl) { return fProvider[lvl=="L2"?0:1]; }
      std::string&     Histogram(const std::string& lvl) { return fHistogram[lvl=="L2"?0:1]; }
      uint32_t&        CurrentRunNumber() { return fCurrentRunNumber; }
      bool&            COOLconnStat()     { return fCOOLconnStat; }

   private:
      std::string        fPartitionName;
      std::string        fCoolDb;
      std::string        fISServer;
      std::string        fProvider[2];
      std::string        fHistogram[2];
      uint32_t           fCurrentRunNumber;
      bool               fCOOLconnStat;
   };
}
#endif

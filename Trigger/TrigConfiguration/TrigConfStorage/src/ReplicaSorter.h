/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ReplicaSorter - class implementing CORAL IReplicaSortingAlgorithm 
// for AtlCoolCopy, analogue of Athena DBReplicaSvc
// Richard Hawkings, 26/11/07

#include <string>
#include "RelationalAccess/IReplicaSortingAlgorithm.h"

namespace TrigConf {

   class ReplicaSorter : virtual public coral::IReplicaSortingAlgorithm {
   public:
      ReplicaSorter();
      void sort(std::vector<const coral::IDatabaseServiceDescription*>& 
                replicaSet);
   private:
      bool readConfig();
      FILE* findFile(const std::string filename, const std::string pathvar);
      std::string m_hostname;
      typedef std::pair<std::string,int> ServerPair;
      typedef std::vector< ServerPair > ServerMap;
      ServerMap m_servermap;
      bool m_frontiergen;
   };

}

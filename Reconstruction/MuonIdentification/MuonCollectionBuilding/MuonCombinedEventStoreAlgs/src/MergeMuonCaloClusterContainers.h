/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REC_MERGEMUONCALOCLUSTERCONTAINERS_H
#define REC_MERGEMUONCALOCLUSTERCONTAINERS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>
#include <vector>

class CaloClusterContainer;
class CaloCellLinkContainer;

namespace Rec {

  /** @class MergeMuonCaloClusterContainers
      @brief update MuonCaloCluster container during AODfix procedures
      @author Felix Rauscher
  */
  class MergeMuonCaloClusterContainers : public AthAlgorithm {
    
  public:
    //Constructor
    MergeMuonCaloClusterContainers(const std::string& name, ISvcLocator* pSvcLocator);
	
    //Algoritm interface functions
    StatusCode initialize();
    StatusCode finalize();
    StatusCode execute();
//=============================================================================
  private:
    std::string m_MuonCaloClusterContainerTarget;
    std::string  m_MuonCaloClusterContainerSource;
    std::vector<std::string>  m_NewMuonCollection;
 		
    inline bool retrieveCollections(CaloClusterContainer *& cluster_collection, CaloCellLinkContainer *& cell_link_collection, const std::string & contianer_name);
	
  };
}

#endif //REC_MERGEMUONCALOCLUSTERCONTAINERS_H

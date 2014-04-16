/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REC_MERGEMUONCALOENERGYCONTAINERS_H
#define REC_MERGEMUONCALOENERGYCONTAINERS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>

namespace Rec {
  /** @class MergeMuonCaloEnergyContainers
      @brief update MuonCaloEnergies container during AODfix procedures
      @author Felix Rauscher
  */

  class MergeMuonCaloEnergyContainers : public AthAlgorithm {
  public:
  //Constructor
    MergeMuonCaloEnergyContainers(const std::string& name, ISvcLocator* pSvcLocator);
	
  //Algoritm interface functions
    StatusCode initialize();
    StatusCode finalize();
    StatusCode execute();
//=============================================================================
  private:
	
  //MuonCaloEnergyContainer target and new container - job option
    std::string m_MuonCaloEnergyContainerTarget;
		
  //Newly created muon collection - job option
    std::string m_NewMuonCollection;
		
  };
}


#endif //REC_MERGEMUONCALOENERGYCONTAINERS_H

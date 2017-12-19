/*                                                                                                                                                                                                                                           
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration                                                                                                                                                                    
*/

#ifndef COMBINEPFO_H
#define COMBINEPFO_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODPFlow/PFOContainer.h"

class CombinePFO : public AthAlgorithm {

public:
 /** Default constructor */
  CombinePFO(const std::string& name, ISvcLocator* pSvcLocator);

  /** Default destructor */
  ~CombinePFO() {};
 
  /** Gaudi AthAlgorithm hooks */
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  /** ReadHandleKey for first PFOContainer to be combined */
  SG::ReadHandleKey<xAOD::PFOContainer> m_PFOContainerReadHandleKeyA{this,"PFOInputContainerNameA","JetETMissLCNeutralParticleFlowObjects","ReadHandleKey for xAOD::PFOContainer"};

  /** ReadHandleKey for second PFOContainer to be combined */
  SG::ReadHandleKey<xAOD::PFOContainer> m_PFOContainerReadHandleKeyB{this,"PFOInputContainerNameB","JetETMissLCNeutralParticleFlowObjectsNonModified","ReadHandleKey for xAOD::PFOContainer"};

  /** WriteHandleKey for PFOContainer, which is VIEW container combining containers A and B referenced above */
  SG::WriteHandleKey<xAOD::PFOContainer> m_PFOContainerWriteHandleKey{this,"PFOOutputContainerName","JetETMissLCCombinedNeutralParticleFlowObjects","WriteHandleKey for xAOD::PFOContainer"};
  
};
#endif

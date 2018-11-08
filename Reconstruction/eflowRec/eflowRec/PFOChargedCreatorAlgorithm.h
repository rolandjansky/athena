#ifndef PFOCHARGEDCREATORALGORITHM_H
#define PFOCHARGEDCREATORALGORITHM_H

#include "eflowRec/eflowCaloObject.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

#include "xAODPFlow/PFOContainer.h"

class PFOChargedCreatorAlgorithm : public AthAlgorithm {
  
public:
  
  PFOChargedCreatorAlgorithm(const std::string& name, ISvcLocator* pSvcLocator);

  ~PFOChargedCreatorAlgorithm() {}

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  /** Create the charged PFO */ 
  void createChargedPFO(const eflowCaloObject& energyFlowCaloObject, bool addClusters, SG::WriteHandle<xAOD::PFOContainer>& chargedPFOContainerWriteHandle);
  /** Function to add links to the vertex to which a charged PFO is matched (using the tracking CP loose vertex association tool) */
  void addVertexLinksToChargedPFO(const xAOD::VertexContainer* theVertexContainer, SG::WriteHandle<xAOD::PFOContainer>& chargedPFOContainerWriteHandle);

  /** Toggle EOverP algorithm mode, whereby no charged shower subtraction is performed */
  Gaudi::Property<bool> m_eOverPMode{this,"EOverPMode",false,"Toggle EOverP algorithm mode, whereby no charged shower subtraction is performed"};

  /** ReadHandleKey for eflowCaloObjectContainer */
  SG::ReadHandleKey<eflowCaloObjectContainer> m_eflowCaloObjectContainerReadHandleKey{this,"eflowCaloObjectContainerName","eflowCaloObjects","ReadHandleKey for eflowCaloObjectContainer"};
  
  /** WriteHandleKey for charged PFO */
  SG::WriteHandleKey<xAOD::PFOContainer> m_chargedPFOContainerWriteHandleKey{this,"PFOOutputName","JetETMissChargedParticleFlowObjects","WriteHandleKey for charged PFO"};
    
};
#endif

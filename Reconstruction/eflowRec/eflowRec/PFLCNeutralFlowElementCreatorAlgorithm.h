#ifndef PFLCNEUTRALFLOWELEMENTCREATORALGORITHM_H
#define PFLCNEUTRALFLOWELEMENTCREATORALGORITHM_H

#include "eflowRec/eflowCaloObject.h"

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODPFlow/FlowElement.h"
#include "xAODPFlow/FlowElementContainer.h"
#include "xAODPFlow/PFODefs.h"

class PFLCNeutralFlowElementCreatorAlgorithm : public AthReentrantAlgorithm {

public:
  using AthReentrantAlgorithm::AthReentrantAlgorithm;

  StatusCode initialize();
  StatusCode execute(const EventContext& ctx) const;

private:

  /** ReadHandleKey for eflowCaloObjectContainer */
  SG::ReadHandleKey<xAOD::FlowElementContainer> m_neutralFEContainerReadHandleKey{this,"FEInputContainerName","JetETMissNeutralFlowElements","ReadHandleKey for neutral FlowElementContainer"};
  
  /** WriteHandleKey for neutral FE */
  SG::WriteHandleKey<xAOD::FlowElementContainer> m_neutralFELCContainerWriteHandleKey{this,"FELCOutputName","JetETMissLCNeutralFlowElements","WriteHandleKey for LC neutral FlowElementContainer"};

};
#endif

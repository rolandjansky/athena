#ifndef PFNEUTRALFLOWELEMENTCREATORALGORITHM_H
#define PFNEUTRALFLOWELEMENTCREATORALGORITHM_H

#include "eflowRec/eflowCaloObject.h"

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODPFlow/FlowElement.h"
#include "xAODPFlow/FlowElementContainer.h"
#include "xAODPFlow/PFODefs.h"

class PFNeutralFlowElementCreatorAlgorithm : public AthReentrantAlgorithm {

public:
  
  using AthReentrantAlgorithm::AthReentrantAlgorithm;

  StatusCode initialize();
  StatusCode execute(const EventContext& ctx) const;

private:

  /** Create the chargedneutral FE */ 
  StatusCode createNeutralFlowElement(const eflowCaloObject& energyFlowCaloObject, xAOD::FlowElementContainer* neutralFEContainer) const;

  /** Function to add cluster moments onto FE */
  void addMoment(const xAOD::CaloCluster::MomentType& momentType, const std::string& feAttribute, const xAOD::CaloCluster& theCluster, xAOD::FlowElement& theFE) const;
 
  /** Function to add caluster sampling energies onto FE */
  void addSamplingEnergy(const xAOD::CaloCluster::CaloSample& sampling, const std::string& feAttribute, const xAOD::CaloCluster& theCluster, xAOD::FlowElement& theFE) const;

  /** Toggle usage of calibration hit truth - false by default */
  Gaudi::Property<bool> m_useCalibHitTruth{this,"UseCalibHitTruth",false,"Toggle usage of calibration hit truth - false by default"};

  /** ReadHandleKey for eflowCaloObjectContainer */
  SG::ReadHandleKey<eflowCaloObjectContainer> m_eflowCaloObjectContainerReadHandleKey{this,"eflowCaloObjectContainerName","eflowCaloObjects","ReadHandleKey for eflowCaloObjectContainer"};
  
  /** WriteHandleKey for neutral FE */
  SG::WriteHandleKey<xAOD::FlowElementContainer> m_neutralFEContainerWriteHandleKey{this,"FEOutputName","JetETMissNeutralFlowElements","WriteHandleKey for neutral FlowElements"};

};
#endif

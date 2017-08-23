#ifndef PFONEUTRALCREATORALGORITHM_H
#define PFONEUTRALCREATORALGORITHM_H

#include "eflowRec/eflowCaloObject.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/PFOContainer.h"

class PFONeutralCreatorAlgorithm :  public AthAlgorithm {
  
public:
  
  PFONeutralCreatorAlgorithm(const std::string& name,ISvcLocator* pSvcLocator);

  ~PFONeutralCreatorAlgorithm() {}

  static const InterfaceID& interfaceID();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  /** Create the chargedneutral PFO */ 
  void createNeutralPFO(const eflowCaloObject& energyFlowCaloObject);

  /** Function to add cluster moments onto PFO */
  void addMoment(const xAOD::CaloCluster::MomentType& momentType, const xAOD::PFODetails::PFOAttributes& pfoAttribute, const xAOD::CaloCluster& theCluster, xAOD::PFO& thePFO);
 
  /** Toggle e/p mode */
  bool m_eOverPMode;

  /** Bool to toggle which jetetmiss configuration we are in - EM cluster input or LC cluster input */
  bool m_LCMode;

  /** ReadHandle for eflowCaloObjectContainer */
  SG::ReadHandle<eflowCaloObjectContainer> m_eflowCaloObjectContainerReadHandle;
  
  /** WriteHandle for neutral PFO */
  SG::WriteHandle<xAOD::PFOContainer> m_neutralPFOContainerWriteHandle;

  /** WriteHandle for non-modified neutral PFO - only used in LC mode */
  SG::WriteHandle<xAOD::PFOContainer> m_neutralPFOContainerWriteHandle_nonModified;  
  
};
#endif

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
  void createNeutralPFO(const eflowCaloObject& energyFlowCaloObject,SG::WriteHandle<xAOD::PFOContainer>& neutralPFOContainerWriteHandle, SG::WriteHandle<xAOD::PFOContainer>& neutralPFOContainerWriteHandle_nonModified);

  /** Function to add cluster moments onto PFO */
  void addMoment(const xAOD::CaloCluster::MomentType& momentType, const xAOD::PFODetails::PFOAttributes& pfoAttribute, const xAOD::CaloCluster& theCluster, xAOD::PFO& thePFO);
 
  /** Toggle EOverP algorithm mode, whereby no charged shower subtraction is performed */
  Gaudi::Property<bool> m_eOverPMode{this,"EOverPMode",false,"Toggle EOverP algorithm mode, whereby no charged shower subtraction is performed"};

  /** Bool to toggle which jetetmiss configuration we are in - EM cluster input or LC cluster input */
  Gaudi::Property<bool> m_LCMode{this,"LCMode",false,"Bool to toggle which jetetmiss configuration we are in - EM cluster input or LC cluster input"};

  /** ReadHandleKey for eflowCaloObjectContainer */
  SG::ReadHandleKey<eflowCaloObjectContainer> m_eflowCaloObjectContainerReadHandleKey{this,"eflowCaloObjectContainerName","eflowCaloObjects","ReadHandleKey for eflowCaloObjectContainer"};
  
  /** WriteHandleKey for neutral PFO */
  SG::WriteHandleKey<xAOD::PFOContainer> m_neutralPFOContainerWriteHandleKey{this,"PFOOutputName","JetETMissNeutralParticleFlowObjects","WriteHandleKey for neutral PFO"};

  /** WriteHandleKey for non-modified neutral PFO - only used in LC mode */
  SG::WriteHandleKey<xAOD::PFOContainer> m_neutralPFOContainerWriteHandleKey_nonModified{this,"PFOOutputName_nonModified","JetETMissNeutralParticleFlowObjects_nonModified"," WriteHandleKey for non-modified neutral PFO - only used in LC mode"};
  
};
#endif

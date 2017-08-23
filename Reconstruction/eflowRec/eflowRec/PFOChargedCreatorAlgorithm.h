#ifndef PFOCHARGEDCREATORALGORITHM_H
#define PFOCHARGEDCREATORALGORITHM_H

#include "eflowRec/eflowCaloObject.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"

#include "xAODPFlow/PFOContainer.h"
#include "xAODTracking/VertexContainer.h"

class PFOChargedCreatorAlgorithm : public AthAlgorithm {
  
public:
  
  PFOChargedCreatorAlgorithm(const std::string& name, ISvcLocator* pSvcLocator);

  ~PFOChargedCreatorAlgorithm() {}

  StatusCode initialize();
  void execute(const eflowCaloObject& energyFlowCaloObject);
  StatusCode execute();
  StatusCode finalize();

private:
  /** Create the charged PFO */ 
  void createChargedPFO(const eflowCaloObject& energyFlowCaloObject, bool addClusters = false);
  /** Function to add links to the vertex to which a charged PFO is matched (using the tracking CP loose vertex association tool) */
  void addVertexLinksToChargedPFO(const xAOD::VertexContainer& theVertexContainer);

  /** Toggle e/p mode */
  bool m_eOverPMode;

  /** ToolHandle to tracking CP loose vertex selection tool */
  ToolHandle<CP::ITrackVertexAssociationTool> m_trackVertexAssociationTool;

  /** ReadHandle for vertex container */
  SG::ReadHandle<xAOD::VertexContainer> m_vertexContainerReadHandle;

  /** ReadHandle for eflowCaloObjectContainer */
  SG::ReadHandle<eflowCaloObjectContainer> m_eflowCaloObjectContainerReadHandle;
  
  /** WriteHandle for charged PFO */
  SG::WriteHandle<xAOD::PFOContainer> m_chargedPFOContainerWriteHandle;
  
  
};
#endif

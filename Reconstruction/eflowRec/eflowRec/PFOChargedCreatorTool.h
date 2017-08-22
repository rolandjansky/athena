#ifndef PFOCHARGEDCREATORTOOL_H
#define PFOCHARGEDCREATORTOOL_H

#include "eflowRec/IPFBaseTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"

#include "xAODPFlow/PFOContainer.h"
#include "xAODTracking/VertexContainer.h"

class eflowCaloObject;
class eflowCaloObjectContainer;
class eflowRecCluster;

static const InterfaceID IID_PFOChargedCreatorTool("PFOChargedCreatorTool", 1, 0);

class PFOChargedCreatorTool : virtual public IPFBaseTool, public AthAlgTool {
  
public:
  
  PFOChargedCreatorTool(const std::string& type,const std::string& name,const IInterface* parent);

  ~PFOChargedCreatorTool() {}

  static const InterfaceID& interfaceID();

  StatusCode initialize();
  StatusCode execute(eflowCaloObject* energyFlowCaloObject);
  void execute(eflowCaloObjectContainer* theEflowCaloObjectContainer, xAOD::CaloClusterContainer& theCaloClusterContainer);
  StatusCode finalize();

private:
  StatusCode setupPFOContainers();
  /** Create the charged PFO */ 
  void createChargedPFO(eflowCaloObject* energyFlowCaloObject, bool addClusters = false);
  /** Function to add links to the vertex to which a charged PFO is matched (using the tracking CP loose vertex association tool) */
  void addVertexLinksToChargedPFO(const xAOD::VertexContainer& theVertexContainer);

  /** Toggle e/p mode */
  bool m_eOverPMode;

  /** ToolHandle to tracking CP loose vertex selection tool */
  ToolHandle<CP::ITrackVertexAssociationTool> m_trackVertexAssociationTool;

  /** ReadHandle for vertex container */
  SG::ReadHandle<xAOD::VertexContainer> m_vertexContainerReadHandle;
  
  /** WriteHandle for charged PFO */
  SG::WriteHandle<xAOD::PFOContainer> m_chargedPFOContainerWriteHandle;
  
  
};

inline const InterfaceID& PFOChargedCreatorTool::interfaceID()
{ 
  return IID_PFOChargedCreatorTool;
}

#endif

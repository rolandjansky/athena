/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOPOCLUSTERVARIABLESTOOL_H
#define TOPOCLUSTERVARIABLESTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloClusterContainer.h"

static const InterfaceID IID_TopoClusterVariablesTool("TopoClusterVariablesTool", 1, 0);

class TopoClusterVariablesTool : public AthAlgTool {
public:
  TopoClusterVariablesTool(const std::string& type, const std::string& name,
                   const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_TopoClusterVariablesTool; };

  TopoClusterVariablesTool();
  ~TopoClusterVariablesTool();

  /** Overriding initialize and finalize */
  virtual StatusCode initialize();
  /** finalization */
  virtual StatusCode finalize();

  void setCaloClusterContainer(const CaloClusterContainer *cl) {m_clusterCont = cl;}
  const CaloClusterContainer *clusterContainer() {return m_clusterCont;}

  StatusCode retrieveContainers();

 private:
  //storegate key names
  std::string m_caloClusterContainerKey;

  //pointers to cluster containers
  const CaloClusterContainer *m_dummyClusterCont;
  const CaloClusterContainer *m_clusterCont;
};

#endif // TOPOCLUSTERVARIABLESTOOL_H

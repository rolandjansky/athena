/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWTAUPROCESSINGTOOLS_H
#define EFLOWTAUPROCESSINGTOOLS_H

/********************************************************************

NAME:     eflowTauProcessingTools.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  May 2013

********************************************************************/

#include "eflowRec/eflowBaseAlg.h"
#include "eflowRec/eflowTauBaseAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthLinks/ElementLinkVector.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODPFlow/PFO.h"

#include <string>

class eflowBaseAlgTool;
class StoreGateSvc;
class CaloCluster;

class eflowTauProcessingTools : public eflowBaseAlg
{

 public:

  /** The Constructor */
  eflowTauProcessingTools(const std::string& name, ISvcLocator* pSvcLocator);

  /** The Destructor */
  ~eflowTauProcessingTools();

  /* Gaudi algtool hooks */

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  typedef std::string tool_key;

  private:

  /** Function to check if we have added duplicate clusters to container */
  bool checkForDuplicates(ElementLinkVector<xAOD::CaloClusterContainer>* clusterContainer, xAOD::CaloCluster* theCluster);

  /** Name of PFO Container to write out */
  std::string m_PFOOutputName;

  /**Name of eflow CaloTopoCluster Container to write out */
  std::string m_eflowClustersOutputName;

  /**Name of CaloTopoCluster container to find EM scale cluster in */
  std::string  m_topoClusterCandsName;
  
  /** Name of tau candidate container to use as input */
  std::string m_tauRecCandsName;

  StoreGateSvc* m_storeGate;

  ToolHandleArray< eflowTauBaseAlgTool > m_tools;

};
#endif


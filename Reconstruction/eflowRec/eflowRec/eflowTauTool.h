/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWTAUTOOL_H
#define EFLOWTAUTOOL_H

/********************************************************************

NAME:     eflowTAUTool.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  MAY 2013

********************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "eflowRec/eflowTauBaseAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class eflowTauObject;
class eflowTrackToCaloTrackExtrapolatorTool;
class eflowBinnedParameters;
class eflowLayerIntegrator;
class eflowCellEOverPTool;

static const InterfaceID IID_eflowTauTool("eflowTauTool", 1, 0);

class eflowTauTool : virtual public eflowTauBaseAlgTool, public AthAlgTool {

  public:
  
  eflowTauTool(const std::string& type,const std::string& name,const IInterface* parent);

  ~eflowTauTool();

  static const InterfaceID& interfaceID();

  StatusCode initialize();
  StatusCode execute(const eflowTauObject& eflowTauObject);
  StatusCode finalize();

 private:

  double getHadronicEnergy(ElementLinkVector<xAOD::CaloClusterContainer>* theClusterContainer);
  double getHadronicEnergy(std::vector<xAOD::CaloCluster*> theClusterContainer);
  double calculateHadronicEnergy(const xAOD::CaloCluster* theCluster);
  void annihilateCluster(const xAOD::CaloCluster* theCluster);

  bool m_recoverIsolatedTracks;
  /** Choose whether to decide to kill left over energy deposits */
  bool m_doAnnihilation;

  double m_consistencySigmaCut;
  double m_subtractionSigmaCut;

  eflowBinnedParameters* m_binnedParameters;
  eflowLayerIntegrator* m_integrator;

  ToolHandle<eflowTrackToCaloTrackExtrapolatorTool> m_theTrackExtrapolatorTool;
  ToolHandle<eflowCellEOverPTool> m_theEOverPTool;

};

inline const InterfaceID& eflowTauTool::interfaceID()
{ 
  return IID_eflowTauTool; 
}


#endif

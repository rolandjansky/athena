/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWREC_EFLOWCELLLEVELSUBTRACTIONTOOL_H
#define EFLOWREC_EFLOWCELLLEVELSUBTRACTIONTOOL_H

/********************************************************************

NAME:     eflowFlowCellLevelSubtractionTool.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  25th January, 2005

********************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "eflowRec/eflowBaseAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "eflowRec/eflowCellList.h"
#include "xAODCaloEvent/CaloCluster.h"

#include <vector>

class eflowCaloObjectContainer;
class eflowTrackCaloPoints;
class eflowCellList;
class eflowRecCluster;
class eflowLayerIntegrator;

static const InterfaceID IID_eflowCellLevelSubtractionTool("eflowCellLevelSubtractionTool", 1, 0);

class eflowCellLevelSubtractionTool : virtual public eflowBaseAlgTool, public AthAlgTool {
public:

  eflowCellLevelSubtractionTool(const std::string& type,const std::string& name,const IInterface* parent);

  ~eflowCellLevelSubtractionTool() {};

  static const InterfaceID& interfaceID();

  StatusCode initialize();
  void execute(eflowCaloObjectContainer* theEflowCaloObjectContainer);
  StatusCode execute(eflowCaloObject* energyFlowCaloObject);
  StatusCode finalize();

 private:
  void subtractTracksFromCluster(eflowRecCluster* thisEfRecCluster);
  void annihilateCluster(xAOD::CaloCluster* cluster);
  void makeOrderedCellList(const eflowTrackCaloPoints& trackCalo, xAOD::CaloCluster* cluster);
  bool runInGoldenMode() { return ((m_goldenModeString == "golden1") || (m_goldenModeString == "golden2")); }

  double m_rCell;

  eflowCellList m_orderedCells;

  double m_subtractionSigmaCut;
  double m_consistencySigmaCut;

  //flag if we want to caluclate EoVerP from single particles we dont do eflow, so clusters are original unmodified ones.
  bool m_calcEOverP;

  // string flag to configure for running in golden e/p match mode
  std::string m_goldenModeString;
};

inline const InterfaceID& eflowCellLevelSubtractionTool::interfaceID()
{ 
  return IID_eflowCellLevelSubtractionTool; 
}

#endif // not EFLOWREC_EFLOWCELLLEVELSUBTRACTIONTOOL_H

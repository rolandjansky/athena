/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWTAULCCALIBTOOL_H
#define EFLOWTAULCCALIBTOOL_H

/********************************************************************

NAME:     eflowTauLCCalibTool.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  MAY 2013

********************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "eflowRec/eflowTauBaseAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "CaloRec/CaloClusterCollectionProcessor.h"

#include "eflowEvent/eflowTauObject.h"

static const InterfaceID IID_eflowTauLCCalibTool("eflowTauLCCalibTool", 1, 0);

class eflowTauLCCalibTool : virtual public eflowTauBaseAlgTool, public AthAlgTool {

  public:
  
  eflowTauLCCalibTool(const std::string& type,const std::string& name,const IInterface* parent);

  ~eflowTauLCCalibTool() {};

  static const InterfaceID& interfaceID();

  StatusCode initialize();
  StatusCode execute(const eflowTauObject& eflowTauObject);
  StatusCode finalize();

 private:
  
  /** Tool to calculate cluster moments */
  ToolHandle<CaloClusterCollectionProcessor> m_clusterMomentsMakerPointer;

  /** Tool for applying local hadronc calibration weights to cells */
  ToolHandle<CaloClusterCollectionProcessor> m_clusterLocalCalibTool;

  /** Tool to deal with out of cluster corrections */
  ToolHandle<CaloClusterCollectionProcessor> m_clusterLocalCalibOOCCTool;

  /** Tool to do Pi0 corrections */
  ToolHandle<CaloClusterCollectionProcessor> m_clusterLocalCalibOOCCPi0Tool;

  /** Tool for correcting clusters at cell level for dead materia */
  ToolHandle<CaloClusterCollectionProcessor> m_clusterLocalCalibDMTool;

};

inline const InterfaceID& eflowTauLCCalibTool::interfaceID()
{ 
  return IID_eflowTauLCCalibTool; 
}

#endif

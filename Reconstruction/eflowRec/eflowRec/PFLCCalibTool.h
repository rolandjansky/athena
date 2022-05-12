/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFLCCALIBTOOL_H
#define PFLCCALIBTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloRec/CaloClusterProcessor.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "eflowRec/IPFBaseTool.h"
#include "eflowRec/IPFClusterCollectionTool.h"

class eflowCaloObjectContainer;
class eflowRecCluster;

/**
This tool can either use a series of CaloClusterProcessor to calibrate the modified xAOD::CaloCluster (modified when we do the charged shower subtraction) using recalculated LC weights or its own internal method to use the stored LC weights in the eflowRecCluster objects. If using the LC weights which were stored in the eflowRecCluster then we use the container of eflowRecCluster, but if using recalculated LC weights for the modified xAOD::CaloCluster (modified when we remove calorimeter cells in the charged shower subtraction) then we need the container of xAOD::CaloCluster. The PFClusterCollectionTool provides both of these containers.
*/
class PFLCCalibTool : public extends<AthAlgTool, IPFBaseTool> {

  public:
  
  PFLCCalibTool(const std::string& type,const std::string& name,const IInterface* parent);

  ~PFLCCalibTool() {}

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const eflowCaloObjectContainer& theEflowCaloObjectContainer) override;
  virtual StatusCode finalize() override;

 private:

  StatusCode apply(ToolHandle<CaloClusterProcessor>& calibTool, xAOD::CaloCluster* cluster);  
  static void applyLocalWeight(eflowRecCluster* theEFRecCluster, const CaloDetDescrManager& calo_dd_man);

  /** Tool to put all clusters into a temporary container - then we use this to calculate moments, some of which depend on configuration of nearby clusters */
  ToolHandle<IPFClusterCollectionTool> m_clusterCollectionTool{this,"eflowRecClusterCollectionTool","eflowRecClusterCollectionTool","Tool to put all clusters into a temporary container - then we use this to calculate moments, some of which depend on configuration of nearby clusters"};

  /* Tool for applying local hadronc calibration weights to cells */
  ToolHandle<CaloClusterProcessor> m_clusterLocalCalibTool{this,"CaloClusterLocalCalib","CaloClusterLocalCalib","Tool for applying local hadronc calibration weights to cells"};

  /* Tool to deal with out of cluster corrections */
  ToolHandle<CaloClusterProcessor> m_clusterLocalCalibOOCCTool{this,"CaloClusterLocalCalibOOCC","CaloClusterLocalCalib","Tool to deal with out of cluster corrections"};

  /* Tool to do Pi0 corrections */
  ToolHandle<CaloClusterProcessor> m_clusterLocalCalibOOCCPi0Tool{this,"CaloClusterLocalCalibOOCCPi0","CaloClusterLocalCalib","Tool to do Pi0 corrections"};

  /* Tool for correcting clusters at cell level for dead material */
  ToolHandle<CaloClusterProcessor> m_clusterLocalCalibDMTool{this,"CaloClusterLocalCalibDM","CaloClusterLocalCalib","Tool for correcting clusters at cell level for dead material"};

  /** Toggle which LC weights scheme to use - default is to recalculate weights, rather than use saved weights */
  Gaudi::Property<bool> m_useLocalWeight{this,"UseLocalWeight",false,"Toggle which LC weights scheme to use - default is to recalculate weights, rather than use saved weights"};

  /** ReadCondHandleKey for CaloDetDescrManager */
  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
	, "CaloDetDescrManager"
	, "CaloDetDescrManager"
	, "SG Key for CaloDetDescrManager in the Condition Store" };

};

#endif

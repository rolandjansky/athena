/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class egammaLargeClusterMaker
   This tool takes the EM CaloCluster used by egamma objects
   and builds large 7x11 clusters around 

   @author J. Mitrevski
   @author C. Anastopoulos
*/

#ifndef EGAMMATOOLS_EGAMMALARGECLUSTERMAKER_H
#define EGAMMATOOLS_EGAMMALARGECLUSTERMAKER_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "xAODCaloEvent/CaloClusterFwd.h" 
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "egammaInterfaces/IegammaEnergyPositionAllSamples.h"
#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "GaudiKernel/ToolHandle.h"

#include "StoreGate/ReadHandleKey.h"

class CaloCellContainer;
class CaloClusterCellLink;

class egammaLargeClusterMaker : public AthAlgTool, virtual public CaloClusterCollectionProcessor {
public:
  
  /** @bried constructor */
  egammaLargeClusterMaker (const std::string& type,
			   const std::string& name,
			   const IInterface* parent);

  using CaloClusterCollectionProcessor::execute;

  /** @brief initialize method */
  virtual StatusCode initialize() override final;
  /** @brief execute on container */
  virtual StatusCode execute(const EventContext& ctx, xAOD::CaloClusterContainer* collection) const override final;
private:
  
  /** @brief tool to calculate sum of energy in all samples */
  ToolHandle<IegammaEnergyPositionAllSamples>  m_egammaEnergyPositionAllSamples {this,
      "egammaEnergyPositionAllSamplesTool", 
      "egammaEnergyPositionAllSamples/egammaEnergyPositionAllSamples"};
  
  /** @brief The name of the cluster container for electrons and photons */   
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_inputClusterCollection {this,
      "InputClusterCollection", "egammaClusters",
      "The name of the cluster container for electrons and photons"}; 

  /** @brief Cell container*/
  SG::ReadHandleKey<CaloCellContainer> m_cellsKey {this,
      "CellsName", "AllCalo", "Names of containers which contain cells"};
  
  Gaudi::Property<double> m_neta {this, "Neta", 7.0,
      "Number of eta cells in each sampling in which to look for hottest cell"};

  Gaudi::Property<double> m_nphi {this, "Nphi", 7.0,
      "Number of phi cell in each sampling in which to look for hottest cell"};


};

#endif // EGAMMATOOLS_EMCLUSTERTOOL_H

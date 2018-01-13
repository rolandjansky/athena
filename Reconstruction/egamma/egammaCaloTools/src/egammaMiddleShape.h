/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMACALOTOOLS_EGAMMAMIDDLESHAPE_H
#define EGAMMACALOTOOLS_EGAMMAMIDDLESHAPE_H

/// @class egammaMiddleShape
/// @brief   EM cluster shower shape calculations in 2nd ECAL sampling 
/// Calculate the width in the strip layer around the eta,phi of
/// the hottest cell in the **middle layer**.
///
/// @author Frederic Derue derue@lpnhe.in2p3.fr
/// @author Christos Anastopoulos
///
/// $Revision:$
/// $Date: 2014-02-11 17:40:48 +0100 (Tue, 11 Feb 2014) $
///

class CaloCellContainer;
class LArEM_ID;
class CaloDetDescrManager;

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "egammaInterfaces/Iegammaqweta2c.h"
#include "egammaInterfaces/IegammaMiddleShape.h"
#include "egammaInterfaces/IegammaEnergyPositionAllSamples.h"

class egammaMiddleShape : public AthAlgTool, virtual public IegammaMiddleShape {

 public:
  
  /** @brief Default constructor*/
  egammaMiddleShape(const std::string& type,
		    const std::string& name,
		    const IInterface* parent);
  /** @brief Destructor*/
  ~egammaMiddleShape();  

  /** @brief AlgTool initialize method.*/
  StatusCode initialize() override ;
  /** @brief AlgTool finalize method */
  StatusCode finalize() override;

  /** @brief AlgTool main method */
  virtual StatusCode execute(const xAOD::CaloCluster& cluster, 
			     const CaloCellContainer& cell_container, Info& info) const override final;
 private:


 /** @brief tool to calculate sum of energy in all samples */
  ToolHandle<IegammaEnergyPositionAllSamples>  m_egammaEnergyPositionAllSamples {this,
      "egammaEnergyPositionAllSamplesTool", 
      "egammaEnergyPositionAllSamples/egammaEnergyPositionAllSamples"};

  /** Tool to calculate correction for the eta width modulation in middle sampling */
  ToolHandle<Iegammaqweta2c>  m_egammaqweta2c {this,
      "egammaqweta2cTool", "egammaqweta2c/egammaqweta2c"};

  Gaudi::Property<double> m_neta {this, "Neta", 7.0,
      "Number of eta cells in each sampling in which to calculated shower shapes"};

  Gaudi::Property<double> m_nphi {this, "Nphi", 7.0,
      "Number of phi cell in each sampling in which to calculated shower shapes"};

    /** @brief boolean to calculate less important variables*/
  Gaudi::Property<bool> m_ExecOtherVariables {this,
      "ExecOtherVariables", true,
      "Calculate some less important variables"}
;
// Calo variables
  const CaloDetDescrManager* m_calo_dd;
};

endif

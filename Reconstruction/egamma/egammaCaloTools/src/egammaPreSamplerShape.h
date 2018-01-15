/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMACALOTOOLS_EGAMMAPRESAMPLERSHAPE_H
#define EGAMMACALOTOOLS_EGAMMAPRESAMPLERSHAPE_H

/// @class egammaPreSamplerShape
/// @brief   EM cluster shower shape calculations in presampler of ECAL.
///
/// @author Frederic Derue derue@lpnhe.in2p3.fr
/// @author Christos Anastopoulos
///
/// $Revision:$
/// $Date: 2014-02-11 17:40:48 +0100 (Tue, 11 Feb 2014) $
///

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "egammaInterfaces/IegammaPreSamplerShape.h"
#include "egammaInterfaces/IegammaEnergyPositionAllSamples.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
class CaloCellContainer;
class CaloDetDescrManager;


class egammaPreSamplerShape : public AthAlgTool, virtual public IegammaPreSamplerShape {

 public:
  
  /** @brief Default constructor*/
  egammaPreSamplerShape(const std::string& type,
			const std::string& name,
			const IInterface* parent);
  /** @brief Destructor*/
  ~egammaPreSamplerShape();  

  /** @brief AlgTool initialize method.*/
  StatusCode initialize() override ;
  /** @brief AlgTool finalize method */
  StatusCode finalize() override ;

  /** @brief AlgTool main method */
  virtual StatusCode execute(const xAOD::CaloCluster& cluster, 
			     const CaloCellContainer& cell_container, Info& info) const override final;
  private: 
 /** @brief tool to calculate sum of energy in all samples */
  ToolHandle<IegammaEnergyPositionAllSamples>  m_egammaEnergyPositionAllSamples {this,
      "egammaEnergyPositionAllSamplesTool", 
      "egammaEnergyPositionAllSamples/egammaEnergyPositionAllSamples"};

  const CaloDetDescrManager* m_calo_dd;
  /** @brief CaloSample */
};

#endif

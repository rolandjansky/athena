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
  StatusCode initialize();
  /** @brief AlgTool finalize method */
  StatusCode finalize();

  /** @brief AlgTool main method */
  virtual StatusCode execute(const xAOD::CaloCluster *cluster, 
			     const CaloCellContainer *cell_container);
  
  /** @brief return the energy in a 1x1 window in cells in eta X phi*/
  double e011() const { return m_e011 ;}
  /** @brief return the energy in a 3x3 window in cells in eta X phi*/
  double e033() const { return m_e033 ;}

 private: 
  /** @brief pointer to a CaloCluster*/
  const xAOD::CaloCluster* m_cluster; 
  /** @brief Cell container*/
  const CaloCellContainer* m_cellContainer;
  /** @brief tool to calculate sum of energy in all samples */
  ToolHandle<IegammaEnergyPositionAllSamples>  m_egammaEnergyPositionAllSamples {this,
      "egammaEnergyPositionAllSamplesTool", 
      "egammaEnergyPositionAllSamples/egammaEnergyPositionAllSamples"};

  /** @brief  energy in S0 in a 1x1 window in cells in eta X phi*/
  double m_e011;     

  /** @brief energy in S0 in a 3x3 window in cells in eta X phi*/
  double m_e033;     

  /** @brief (eta,phi) around which estimate the shower shapes */
  double m_eta;
  double m_phi;
  
  /** @brief (deta,dphi) granularity*/
  double m_deta;
  double m_dphi;
  
  // Calo variables
  const CaloDetDescrManager* m_calo_dd;
  /** @brief CaloSample */
  CaloSampling::CaloSample m_sam;
  CaloSampling::CaloSample m_sam2;
  CaloCell_ID::SUBCALO m_subcalo;
  bool m_barrel;
  int m_sampling_or_module;

  /** @brief From the original (eta,phi) position, find the location (sampling, barrel/end-cap, granularity) */
  bool FindPosition();
};

#endif

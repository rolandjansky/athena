/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMACALOTOOLS_EGAMMABACKSHAPE_H
#define EGAMMACALOTOOLS_EGAMMABACKSHAPE_H

/// @class egammaBackShape
/// @brief     EM cluster shower shape calculations in 3rd sampling. 
///  Calculate the width in the back layer around the eta,phi of
///  the hottest cell in the **middle layer**.
///
/// @author Frederic Derue derue@lpnhe.in2p3.fr
/// @author Christos Anastopoulos
///
/// $Revision:$
/// $Date: 2014-02-11 17:40:48 +0100 (Tue, 11 Feb 2014) $
///

class CaloCellContainer;
class CaloDetDescrManager;

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "egammaInterfaces/IegammaBackShape.h"
#include "egammaInterfaces/IegammaEnergyPositionAllSamples.h"

class egammaBackShape : public AthAlgTool, virtual public IegammaBackShape {

 public:
  
  /** @brief Default constructor*/
  egammaBackShape(const std::string& type,
		  const std::string& name,
		  const IInterface* parent);
  /** @brief Destructor*/
  ~egammaBackShape();  

  /** @brief AlgTool initialize method.*/
  StatusCode initialize();
  /** @brief AlgTool finalize method */
  StatusCode finalize();

  /** @brief AlgTool main method */
  virtual StatusCode execute(const xAOD::CaloCluster *cluster, const CaloCellContainer *cell_container);

  /** @brief return the uncor. energy in 3x3 cells*/
  double e333() const;
  /** @brief return the uncor. energy in 3x5 cells*/
  double e335() const;
  /** @brief return the uncor. energy in 5x5 cells*/
  double e355() const;
  /** @brief return the uncor. energy in 3x7 cells */
  double e337() const;
  /** @brief return the uncor. energy in 7x7 cells*/
  double e377() const;
  /** @brief return the ratio of energy reconstructed in the 3rd 
      sampling over all energy*/
  double f3()   const; 
  /** @brief return the ratio of energy deposited in the core of the third 
      longitudinal compartment of the em over all energy*/
  double f3core()   const; 

 private:
 
  /** @brief pointer to a CaloCluster*/
  const xAOD::CaloCluster* m_cluster; 
  /** @brief Cell container*/ 
  const CaloCellContainer* m_cellContainer;
  /** @brief tool to calculate sum of energy in all samples */
  ToolHandle<IegammaEnergyPositionAllSamples>  m_egammaEnergyPositionAllSamples {this,
      "egammaEnergyPositionAllSamplesTool", 
      "egammaEnergyPositionAllSamples/egammaEnergyPositionAllSamples"};

  /** @brief energy in all samplings*/
  double m_eallsamples;
  /** @brief uncorrected cluster energy in 3rd sampling*/
  double m_e3;

  /** @brief energy in a  3x3 window in cells in eta X phi*/
  double m_e333;             
  /** @brief energy in a  3x5 window in cells in eta X phi*/
  double m_e335;            
  /** @brief */
  double m_e355;       
  /** @brief energy in a  3x7 window in cells in eta X phi*/
  double m_e337;             
  /** @brief energy in a  7x7 window in cells in eta X phi*/
  double m_e377;     
  /** @brief // fraction of energy in back sampling*/
  double m_f3;               
  /** @brief energy in E3core/E i.e E333/E*/
  double m_f3core;

  /** @brief boolean to calculate less important variables*/
  Gaudi::Property<bool> m_ExecOtherVariables {this, 
      "ExecOtherVariables", true,
      "Calculate some less important variables"};

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

  /** @brief initialisation of all variables*/
  void InitVariables();
  /** @brief From the original (eta,phi) position, find the location
      (sampling, barrel/end-cap, granularity) */
  bool FindPosition();
};

//
// set values for the different variables in the egammaBackShape
//
inline double egammaBackShape::e333()   const { return m_e333 ;}
inline double egammaBackShape::e335()   const { return m_e335 ;}
inline double egammaBackShape::e355()   const { return m_e355 ;}
inline double egammaBackShape::e337()   const { return m_e337 ;}
inline double egammaBackShape::e377()   const { return m_e377 ;}
inline double egammaBackShape::f3()     const { return m_f3   ;}
inline double egammaBackShape::f3core() const { return m_f3core ;}

#endif

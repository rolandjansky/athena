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
  StatusCode initialize();
  /** @brief AlgTool finalize method */
  StatusCode finalize();

  /** @brief AlgTool main method */
  virtual StatusCode execute(const xAOD::CaloCluster *cluster, 
			     const CaloCellContainer *cell_container);

  /** @brief return the energy in a  3x3 window in cells in eta X phi*/
  double e233()   const; 
  /** @brief return the energy in a  3x5 window in cells in eta X phi*/
  double e235()   const; 
  /** @brief return the energy in a  3x7 window in cells in eta X phi*/
  double e237()   const; 
  /** @brief return the energy in a  5x5 window in cells in eta X phi*/
  double e255()   const; 
  /** @brief return the energy in a  7x7 window in cells in eta X phi*/
  double e277()   const; 
  /** @brief return the weighted eta in a 3X5 window in eta X phi */
  double etaw()   const; 
  /** @brief return the weighted phi in a 3X5 window in eta X phi*/
  double phiw()   const; 
  /** @brief return the uncorrected width in a 3X5 window in eta X phi*/
  double width()  const; 
  /** @brief return the rel. position in eta within cell*/
  double poscs2() const; 

 private:

  /** @brief pointer to a CaloCluster*/
  const xAOD::CaloCluster* m_cluster; 
  /** @brief Cell container*/
  const CaloCellContainer* m_cellContainer;
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

  /** @brief energy in all samplings*/
  double m_eallsamples;
  /** @brief energy in a 3X3 window in eta X phi*/
  double m_e233;            
  /** @brief energy in a 3X5 window in eta X phi*/
  double m_e235;            
  /** @brief energy in a 3X7 window in eta X phi*/
  double m_e237;            
  /** @brief energy in a 5X5 window in eta X phi*/
  double m_e255;            
  /** @brief  energy in a 7X7 window in eta X phi*/
  double m_e277;            
  /** @brief weighted phi in a 3X5 window in eta X phi*/
  double m_phiw;            
  /** @brief weighted eta in a 3X5 window in eta X phi*/
  double m_etaw;            
  /** @brief uncorrected width in a 3X5 window in eta X phi*/
  double m_width;           
  /** @brief rel. position in eta within cell*/
  double m_poscs2;          
  /** @briefgranularity in eta of middle sampling */
  //double m_etagranularity;  
  /** @brief granularity in phi of middle sampling*/
  //double m_phigranularity;  

  /** @brief boolean to calculate less important variables*/
  Gaudi::Property<bool> m_ExecOtherVariables {this,
      "ExecOtherVariables", true,
      "Calculate some less important variables"}
;
  /** @brief (eta,phi) around which estimate the shower shapes */
  double m_eta;
  double m_phi;
  /** @brief (deta,dphi) granularity*/
  double m_deta;
  double m_dphi;
  float m_etacell;
  // Calo variables
  const CaloDetDescrManager* m_calo_dd;
  //const LArEM_ID* m_emid;
  /** @brief CaloSample */
  CaloSampling::CaloSample m_sam;
  //CaloSampling::CaloSample m_sammax;
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
// set values for the different variables in the egammaMiddleShape
//
inline double egammaMiddleShape::e233()   const { return m_e233   ;}
inline double egammaMiddleShape::e235()   const { return m_e235   ;}
inline double egammaMiddleShape::e237()   const { return m_e237   ;}
inline double egammaMiddleShape::e255()   const { return m_e255   ;}
inline double egammaMiddleShape::e277()   const { return m_e277   ;}
inline double egammaMiddleShape::etaw()   const { return m_etaw   ;}
inline double egammaMiddleShape::phiw()   const { return m_phiw   ;}
inline double egammaMiddleShape::width()  const { return m_width  ;}
inline double egammaMiddleShape::poscs2() const { return m_poscs2 ;}

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMACALOTOOLS_EGAMMAISO_H
#define EGAMMACALOTOOLS_EGAMMAISO_H

/// @class egammaIso
/// @brief {Tool to estimate the hadronic energy behind the electromagnetic cluster, the 
/// calorimetric isolation of egamma objects and the energy above the noise 
///
/// - hadronic energy : real photons or electrons are purely electromagnetic objects, therefore they deposit 
///   their energy primarily in the electromagnetic compartment of the calorimeter. Fake electrons and 
///   photons induced from jets contain hadrons that would penetrate deeper into the calorimeter depositing 
///   sizable energy beyond the electromagnetic calorimeter. Two variables are defined ehad1 and ethad1 as the (transverse) energy 
///   in the first layer of the hadronic calorimeter in a window Delta eta × Delta phi = 0.24 × 0.24, in order to avoid boundary 
///   effects that could result from using readout cells. See definition of these variables to see which samplings are taken into account or not. 
///   A third variable ethad gives the transverse energy reconstructed into the hadronic calorimeter with exclusion of energy in CaloSampling::TileGap3.
///
///- calorimetric isolation : it is defined as the transverse energy reconstructed in a cone of R=0.45 (by default) in the calorimeters, 
///  with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3.
///  
/// - energy above noise : similarly a method calculates the transverse energy in a cone around the cluster, selecting only cells above noise.}
///
/// @author H. Ma
/// @Fred Derue
/// @author T. Cuhadar Donszelmann
/// @author Christos Anastopoulos
///
/// $Revision:$
/// $Date: 2014-02-11 17:40:48 +0100 (Tue, 11 Feb 2014) $
///

#include "AthenaBaseComps/AthAlgTool.h"


class CaloCellList; 
class ICalorimeterNoiseTool;
#include "xAODCaloEvent/CaloClusterFwd.h"
#include <vector>
#include "GaudiKernel/ToolHandle.h"
#include "egammaInterfaces/IegammaIso.h"

class egammaIso : public AthAlgTool, virtual public IegammaIso {

 public:

  /** @brief Default constructor*/
  egammaIso(const std::string& type,
	    const std::string& name,
	    const IInterface* parent);

  /** @brief Destructor*/
  ~egammaIso();  

  /** @brief initialize method*/
  StatusCode initialize();
  /** @brief finalize method*/
  StatusCode finalize();

  /** @brief  method:
      isolation et, 
      coneRadius = cone size, 
      neta and nphi excludes
      neta X nphi EM Cells for etcone*/
  virtual StatusCode execute(const xAOD::CaloCluster*, CaloCellList* had, 
			     CaloCellList* em, 
			     double R, int neta, int nphi);

  /** @brief  method:
      isolation et, 
      coneRadius = cone size, 
      neta and nphi excludes
      neta X nphi EM Cells for etcone
      ***NOTE:: First cone has to be the largest***
      */
  virtual StatusCode execute(const xAOD::CaloCluster*, CaloCellList* had, 
			     CaloCellList* em, 
			     const std::vector<double>& Rs, int neta, int nphi);

  /** @brief  Method to just calculate hadronic leakage*/
  virtual StatusCode execute(const xAOD::CaloCluster *cluster, CaloCellList* had);
  /** @brief ET leakage into hadronic calorimeter with exclusion of energy in CaloSampling::TileGap3*/
  double ethad()  const; 
  /** @brief E leakage into 1st sampling of had calo (CaloSampling::HEC0 + CaloSampling::TileBar0 + CaloSampling::TileExt0)*/
  double ehad1()  const; 
  /** @brief transverse energy in the first sampling of the hadronic
  calorimeters behind the cluster calculated from ehad1*/
  double ethad1() const; 
  /** @brief ET in a cone of calo, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3; i is the index if multiple are calculated*/
  double etcone(int i=0) const; 

  /** @brief Calculate transverse energy in a cone*/
  void CalcEtCone(double R,int neta, int nphi);
  /** @brief Calculate transverse energy in a ring above noise*/
  void CalcEtRingNoise(double R1, double R2, int flagCell, int flagNoise, int NsigNoise );
  /** @brief Calculate transverse energy in a cone around the cluster, selecting only cells above noise*/
  void CalcEtConeClusterNoise(double R2, int NsigNoise, int flagCell, int flagNoise, bool symmetric=false, int neta=5, int nphi=7);

 private:

  /** @brief pointer to a Calocluster*/
  const xAOD::CaloCluster* m_cluster; 
  /** @brief EM Calocell list*/
  CaloCellList* m_EMCellList;
  /** @brief HAD Calocell list*/
  CaloCellList* m_HADCellList;
  
  double m_ethad; 
  double m_ethad1; 
  double m_ehad1; 
  std::vector<double> m_etcone; 
  /** @brief Tool Handle for calorimeter noise*/
  ToolHandle<ICalorimeterNoiseTool> m_noiseTool;
  
};

// retrieve variables from egammaIso
inline double egammaIso::ethad()   const { return m_ethad   ;}
inline double egammaIso::ehad1()   const { return m_ehad1   ;}
inline double egammaIso::ethad1()  const { return m_ethad1  ;}
inline double egammaIso::etcone(int i)  const { return m_etcone.at(i);}

#endif

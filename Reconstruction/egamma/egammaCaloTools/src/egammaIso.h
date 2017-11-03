/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMACALOTOOLS_EGAMMAISO_H
#define EGAMMACALOTOOLS_EGAMMAISO_H

/// @class egammaIso
/// @brief {Tool to estimate the hadronic energy behind the electromagnetic cluster, the 

#include "AthenaBaseComps/AthAlgTool.h"


class CaloCellList; 
class ICalorimeterNoiseTool;
#include "xAODCaloEvent/CaloClusterFwd.h"
#include <vector>
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
  /** @brief  Method to just calculate hadronic leakage*/
  virtual StatusCode execute(const xAOD::CaloCluster *cluster, CaloCellList* had);
  /** @brief ET leakage into hadronic calorimeter with exclusion of energy in CaloSampling::TileGap3*/
  double ethad()  const; 
  /** @brief E leakage into 1st sampling of had calo (CaloSampling::HEC0 + CaloSampling::TileBar0 + CaloSampling::TileExt0)*/
  double ehad1()  const; 
  /** @brief transverse energy in the first sampling of the hadronic
  calorimeters behind the cluster calculated from ehad1*/
  double ethad1() const; 

 private:
  /** @brief pointer to a Calocluster*/
  const xAOD::CaloCluster* m_cluster; 
  /** @brief HAD Calocell list*/
  CaloCellList* m_HADCellList;
  
  double m_ethad; 
  double m_ethad1; 
  double m_ehad1; 
  
};

// retrieve variables from egammaIso
inline double egammaIso::ethad()   const { return m_ethad   ;}
inline double egammaIso::ehad1()   const { return m_ehad1   ;}
inline double egammaIso::ethad1()  const { return m_ethad1  ;}

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOTOPOEMLAYERS_H
#define CALOCLUSTERCORRECTION_CALOTOPOEMLAYERS_H
/********************************************************************

NAME:     CaloTopoEMLayers
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Dec. 15, 1999

PURPOSE:  Computer eta, phi, energy of each layers of a cluster
	  Base class: CaloClusterCorrection (tool)

Atrecon Orig: emreco/ecalclus.F   
       

Updated:  May 10, 2000    (SR, HM)
          Migrated to Athena Framework from PASO

Updated:  Jan 5, 2001    (HM)
          QA. 

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool

Updated:  January 21, 2005    (MB)
	  Move to Calorimeter/CaloClusterCorrection


********************************************************************/

// INCLUDE HEADER FILES:
#include "CaloClusterCorrection/CaloFillRectangularCluster.h"

class CaloTopoEMlayers : public CaloFillRectangularCluster
{

 public:

  // Constructor
  CaloTopoEMlayers(const std::string& type,
                   const std::string& name,
                   const IInterface* parent);

  // Algtool virtual method 
  virtual StatusCode initialize() override;

  virtual void get_seed (const xAOD::CaloCluster* cluster,
                         const CaloCell* max_et_cell,
                         double& eta, double& phi) const override;

 private:

  CaloTopoEMlayers() = delete;
};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWOVERLAPREMOVAL_H
#define EFLOWOVERLAPREMOVAL_H

/* 

Class to evaluate overlap between energy-flow objects and electrons

Author: Marianna Testa
Created: 10 October 2013

*/
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/EgammaContainer.h"
//Athena
#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ToolHandle.h"


class StoreGateSvc;
//C++
#include <string>

class eflowOverlapRemoval : public AthAlgorithm {
  
 public:

  eflowOverlapRemoval(const std::string& name, ISvcLocator* pSvcLocator);
  ~eflowOverlapRemoval();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  bool IsEleOverlap(const xAOD::CaloCluster* cluster, const xAOD::EgammaContainer* selElectrons);
  bool IsGammaOverlap(const xAOD::CaloCluster* cluster, const xAOD::EgammaContainer* photonCollection);
 private:

  std::string m_PFOName;

  StoreGateSvc* m_storeGate;
  
  std::string m_egammaContainerName;
  /** Name of electron container built by eflowRec/eflowPreparation */
  std::string m_eflowElectronContainerName;

   /*! \brief Property: reference cluster collection key */
  std::string m_refClusterContainer;

  /*! \brief Property: maximum distance for being "near" */
  double m_radialDist;
  /*! \brief Property: longitudinal extension scale */
  double m_longSig;
  /*! \brief Property: lateral extension scale */
  double m_radialSig;
  /*! \brief Property: overlap decision mechanism by energy fraction */
  double m_eFracThreshold;

  class Energy
  {
  public:
    double operator()(double& eSum,const CaloCell* pCell)
    {
      eSum += pCell->e();
      return eSum;
    }
  };
  
};
#endif

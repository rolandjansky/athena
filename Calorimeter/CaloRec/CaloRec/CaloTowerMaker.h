/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOTWERMAKER_H
#define CALOREC_CALOTWERMAKER_H

/********************************************************************

NAME:     CaloTowerMaker.h
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  H. Ma, 
CREATED:  Feb 5, 2001

PURPOSE:  This serves as the main algorithm hook that clients must invoke 
          to reconstruct CaloTower's.  This class expects to
	  instantiate sub-algorithms to fill the CaloTowerContainer. 
Updated:  Move to StoreGate, May 2001, (SR)

********************************************************************/

class StoreGateSvc;

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloEvent/CaloTowerSeg.h"
class CaloTower; 

class CaloTowerMaker : public AthAlgorithm
{

 public:

  CaloTowerMaker(const std::string& name, ISvcLocator* pSvcLocator);
  ~CaloTowerMaker();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();


 private:
  // define the properties:
  std::string              m_towerContainer; // TDS CaloTowerContainer name
  std::vector<std::string> m_towerBuilderNames;  // tower builder sub algs
  std::vector<Algorithm*> m_towerBuilderPointers; // pointers to sub algs.
  // segmentation 
  int m_netatower;  // number of tower in eta
  int m_nphitower;  // number of tower in phi
  double m_etamin;  // lower bound of eta
  double m_etamax;  // upper bound of eta

  
};
#endif

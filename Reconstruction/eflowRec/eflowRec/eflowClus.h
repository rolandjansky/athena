/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWREC_EFLOWCLUS_H
#define EFLOWREC_EFLOWCLUS_H
/********************************************************************

NAME:     eflowClus.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  D.R. Tovey
CREATED:  15th May, 2004

********************************************************************/

#include <string>
#include <utility>

#include "xAODCaloEvent/CaloCluster.h"

class eflowClus {

 public:

  eflowClus();
  ~eflowClus();

  void setCluster(const xAOD::CaloCluster* clus);

  void addObject(double energy, double var);

  bool checkEOverP(double sigmaCut);

 private:
  /** Cluster energy, at arb. scale, from cluster */
  double m_clusterEnergy;

  /** Total energy of charged deposits */
  double m_expectedEnergy;
  double m_varianceExpectedEnergy;
};

#endif

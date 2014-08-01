/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowClus.cxx
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  D.R. Tovey
CREATED:  15th May, 2004

*******************************************************************/

// INCLUDE HEADER FILES:

#include "eflowRec/eflowClus.h"

#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloCell.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloInterface/IHadronicCalibrationTool.h"
#include "eflowRec/phicorr.h"
#include "eflowRec/cycle.h"

#include "GaudiKernel/SystemOfUnits.h"

#include <math.h>
#include <iostream>

using namespace std;

eflowClus::eflowClus() :
  m_clusterEnergy(0),

  /** charged deposit variables */
  m_expectedEnergy(0),
  m_varianceExpectedEnergy(0) { }

eflowClus::~eflowClus() { }

void eflowClus::setCluster(const xAOD::CaloCluster* cluster) {
  if (!cluster) { return; }
  m_clusterEnergy = cluster->e();
}

void eflowClus::addObject(double expectedEnergy, double varianceExpectedEnergy) {
  m_expectedEnergy += expectedEnergy;
  m_varianceExpectedEnergy += varianceExpectedEnergy;
}

bool eflowClus::checkEOverP(double sigmaCut) {
  if ((m_expectedEnergy == 0) && (m_clusterEnergy > 0)) {
    return false;
  }

  return m_clusterEnergy < m_expectedEnergy - sigmaCut*sqrt(m_varianceExpectedEnergy);
}

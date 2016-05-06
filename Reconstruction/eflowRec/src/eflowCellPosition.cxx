/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowCellPosition.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson, R Duxfield (based on R.Duxfields Root package)
CREATED:  18th Aug, 2005

********************************************************************/

//Athena Headers
#include "eflowRec/eflowCellPosition.h"
#include "eflowRec/eflowAbstractCellList.h"
#include "eflowRec/eflowDatabase.h"

#include "eflowRec/cycle.h"

#include "CaloEvent/CaloCell.h"

//C++ Headers
#include <cmath>

eflowCellPosition::eflowCellPosition(const eflowAbstractCellList* deposit, eflowCaloENUM layer, double dR) :
   m_layer(layer),m_deposit(deposit) {
  m_eta = deposit->etaFF(layer) + dR;
  m_phi = deposit->phiFF(layer);
}

eflowCellPosition::eflowCellPosition(const eflowAbstractCellList* deposit, const CaloCell* cell) :
  m_eta(cell->eta()), m_phi(cell->phi()), m_deposit(deposit) {
  const CaloDetDescrElement* caloDDE = cell->caloDDE();

  if (caloDDE) {
    CaloCell_ID::CaloSample caloSampl = caloDDE->getSampling();
    m_layer = eflowCalo::translateSampl(caloSampl);
  } else {
    m_layer = eflowCalo::Unknown;
  }
}

bool eflowCellPosition::operator<(const eflowCellPosition& rhs) const {
  if (m_layer != rhs.m_layer) {
    return m_layer < rhs.m_layer;
  } else {
    double r1 = m_deposit->dR2(m_eta, m_phi, m_layer);
    double r2 = m_deposit->dR2(rhs.m_eta, rhs.m_phi, m_layer);

    if (fabs(r1 - r2) < 1.0e-12) {
      if (fabs(m_eta - rhs.m_eta) < 1.0e-6) {
        return (cycle(m_phi, rhs.m_phi) < rhs.m_phi);
      } else {
        return (m_eta < rhs.m_eta);
      }
    } else {
      return (r1 < r2);
    }
  }
}

double eflowCellPosition::dR() const {
  return m_deposit->dR(m_eta, m_phi, m_layer);
}

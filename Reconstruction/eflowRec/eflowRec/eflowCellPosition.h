/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWCELLPOSITION_H
#define EFLOWCELLPOSITION_H

/********************************************************************

NAME:     eflowCellPosition.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson, R Duxfield (based on R.Duxfields Root package)
CREATED:  18th Aug, 2005

********************************************************************/

#include "eflowRec/eflowCaloRegions.h"

class CaloCell;
class eflowAbstractCellList;

class eflowCellPosition {

 public:

  eflowCellPosition(const eflowAbstractCellList* deposit, eflowCaloENUM layer, double dR);
  eflowCellPosition(const eflowAbstractCellList* deposit, const CaloCell* cell);
  ~eflowCellPosition() {}
    
  bool operator<(const eflowCellPosition& rhs) const;
  
  double dR() const;

 private:

  eflowCaloENUM m_layer;
  double m_eta, m_phi;
  const eflowAbstractCellList* m_deposit;

};
#endif

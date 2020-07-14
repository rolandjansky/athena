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

/**
This class stores the eta,phi and layer of the calorimeter cell and defines a "<" operator which can be used to order the eflowCellPosition based on eta,phi and layer. The < operator makes use of a stored pointer to the eflowAbstractCellList that the eflowCellPosition is in, in order to use eflowCellList::DR2 inside the < operator.

So conceptually we can consider that a track has some list of associated calorimeter cells in the eflowCellList, where the information that we need for each cell is stored in the eflowCellPosition. A map is kept to link the eflowCellPosition back to the actual CaloCell.
*/
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

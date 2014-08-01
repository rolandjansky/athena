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

  eflowCellPosition()  :  m_deposit(0) {};
  eflowCellPosition(const eflowAbstractCellList* deposit, eflowCaloENUM layer, double eta, double phi);
  eflowCellPosition(const eflowAbstractCellList* deposit, eflowCaloENUM layer, double dR);
  eflowCellPosition(const eflowAbstractCellList* deposit, const CaloCell* cell);
  eflowCellPosition(const eflowAbstractCellList* deposit, std::pair<const CaloCell*,int> cell);
  ~eflowCellPosition() {}
    
  bool operator<(const eflowCellPosition& rhs) const;
  
  eflowCaloENUM Layer() const   {return m_layer;}
  double Eta() const  {return m_eta;}
  double Phi() const  {return m_phi;}
  double dR() const;

 private:

  eflowCaloENUM m_layer;
  double m_eta, m_phi;
  const eflowAbstractCellList* m_deposit;

};
#endif

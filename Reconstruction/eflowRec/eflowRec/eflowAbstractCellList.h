/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWREC_EFLOWABSTRACTCELLLIST_H
#define EFLOWREC_EFLOWABSTRACTCELLLIST_H

/********************************************************************

NAME:     eflowAbstractCellList.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  22nd Aug, 2005

********************************************************************/

//Athena Headers
#include "eflowRec/eflowCellPosition.h"

//C++ Headers
#include <map>

class CaloCell;
class eflowTrackCaloPoints;

typedef std::map<eflowCellPosition,std::vector<std::pair<CaloCell*,int> > >::iterator CellIt;

class eflowAbstractCellList {

 public:

  eflowAbstractCellList()  {};
  virtual ~eflowAbstractCellList()  {};

  virtual void setNewExtrapolatedTrack(const eflowTrackCaloPoints* trackCalo) = 0;

  virtual void addCell(std::pair<CaloCell*,int> cell) = 0;

  virtual void reorderWithoutLayers() = 0;

  virtual CellIt begin()  = 0;
  virtual CellIt end() = 0;
    
  virtual CellIt getLowerBound(eflowCaloENUM layer, double r) = 0;
  
  virtual void deleteFromList(CellIt& start, CellIt& end) = 0;
  virtual void deleteFromList(CellIt& it) = 0;

  virtual double etaFF(eflowCaloENUM layer) const  = 0;
  virtual double phiFF(eflowCaloENUM layer) const  = 0;
  
  virtual double dR2(double eta, double phi, eflowCaloENUM layer) const = 0;
  virtual double dR(double eta, double phi, eflowCaloENUM layer) const = 0;
  
};
#endif // not EFLOWREC_EFLOWABSTRACTCELLLIST_H

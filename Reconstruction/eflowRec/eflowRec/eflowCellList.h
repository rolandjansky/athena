/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWCELLLIST_H
#define EFLOWCELLLIST_H

/********************************************************************

NAME:     eflowCellList.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson, R Duxfield (based on R.Duxfields Root package)
CREATED:  18th Aug, 2005

********************************************************************/

//Athena Headers
#include "eflowRec/eflowCellPosition.h"
#include "eflowRec/eflowAbstractCellList.h"

//C++ Headers
#include <map>
#include <vector>

class CaloCell;
class eflowTrackCaloPoints;

typedef std::map<eflowCellPosition,std::vector<std::pair<CaloCell*,int> > >::iterator CellIt;

class eflowCellList : public eflowAbstractCellList{

 public:

  eflowCellList();
  virtual ~eflowCellList()  {}

  void setNewExtrapolatedTrack(const eflowTrackCaloPoints* trackCalo);

  void setNewExtrapolatedTrack(const eflowTrackCaloPoints& trackCalo);

  void addCell(std::pair<CaloCell*,int> cell);

  void reorderWithoutLayers();

  CellIt begin()  {return m_cellPositionToCellMap.begin();}
  CellIt end()  {return m_cellPositionToCellMap.end();}
    
  CellIt getLowerBound(eflowCaloENUM layer, double r)        { return m_cellPositionToCellMap.lower_bound( eflowCellPosition(this, layer, r) );}

  int mapSize ()                                   { return m_cellPositionToCellMap.size();}
  void deleteFromList(CellIt& start, CellIt& end)  { m_cellPositionToCellMap.erase(start, end);}
  void deleteFromList(CellIt& it)                  { m_cellPositionToCellMap.erase(it);}
  void eraseList()                                 { m_cellPositionToCellMap.clear();}

  double etaFF(eflowCaloENUM layer) const  {return m_etaFF[layer];}
  double phiFF(eflowCaloENUM layer) const  {return m_phiFF[layer];}
  
  double dR2(double eta, double phi, eflowCaloENUM layer) const;
  double dR(double eta, double phi, eflowCaloENUM layer) const;

 private:
  
  std::vector<double> m_etaFF;
  std::vector<double> m_phiFF;
  std::map<eflowCellPosition,std::vector<std::pair<CaloCell*,int> > > m_cellPositionToCellMap;

};
#endif

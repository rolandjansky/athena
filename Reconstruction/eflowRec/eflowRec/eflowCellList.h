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

// Athena Headers
#include "eflowRec/eflowAbstractCellList.h"
#include "eflowRec/eflowCellPosition.h"

// C++ Headers
#include <map>
#include <vector>

class CaloCell;
class eflowTrackCaloPoints;

typedef std::map<eflowCellPosition,
                 std::vector<std::pair<const CaloCell*, int>>>::iterator CellIt;

/**
Concrete class derived class from pure virtual eflowAbstractCellList. This
stores information about the calorimeter cells - the main data type is a map
between an eflowCellPosition and a pair linking the CaloCell pointer and the
index of its xAOD::CaloCluster in the calorimeter cluster container. A DR2
function is provided to calculate the distance between an eflowCellPosition and
the track impact point in a given calorimeter layer (eflowCellList also stores a
list of track eta,phi impact points for each calorimeter later).
*/
class eflowCellList : public eflowAbstractCellList
{

public:
  eflowCellList();
  virtual ~eflowCellList() {}

  void setNewExtrapolatedTrack(const eflowTrackCaloPoints* trackCalo);

  void setNewExtrapolatedTrack(const eflowTrackCaloPoints& trackCalo);

  void addCell(std::pair<const CaloCell*, int> cell);

  void reorderWithoutLayers();

  CellIt begin() { return m_cellPositionToCellMap.begin(); }
  CellIt end() { return m_cellPositionToCellMap.end(); }

  CellIt getLowerBound(eflowCaloENUM layer, double r)
  {
    return m_cellPositionToCellMap.lower_bound(
      eflowCellPosition(this, layer, r));
  }

  CellIt find(eflowCaloENUM layer, double r)
  {
    return m_cellPositionToCellMap.find(eflowCellPosition(this, layer, r));
  }

  int mapSize() { return m_cellPositionToCellMap.size(); }
  void deleteFromList(CellIt& start, CellIt& end)
  {
    m_cellPositionToCellMap.erase(start, end);
  }
  void deleteFromList(CellIt& it) { m_cellPositionToCellMap.erase(it); }
  void eraseList() { m_cellPositionToCellMap.clear(); }

  double etaFF(eflowCaloENUM layer) const { return m_etaFF[layer]; }
  double phiFF(eflowCaloENUM layer) const { return m_phiFF[layer]; }

  double dR2(double eta, double phi, eflowCaloENUM layer) const;
  double dR(double eta, double phi, eflowCaloENUM layer) const;

private:
  std::vector<double> m_etaFF;
  std::vector<double> m_phiFF;
  std::map<eflowCellPosition, std::vector<std::pair<const CaloCell*, int>>>
    m_cellPositionToCellMap;
};
#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowCellList.cxx
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson, R Duxfield (based on R.Duxfields Root package)
CREATED:  18th Aug, 2005

********************************************************************/

//Athena Headers
#include "eflowRec/eflowCaloRegions.h"
#include "eflowRec/eflowCellList.h"
#include "eflowRec/eflowCellPosition.h"
#include "eflowRec/eflowTrackCaloPoints.h"
#include "eflowRec/cycle.h"

#include "CaloEvent/CaloCell.h"

//C++ Headers

#include <cmath>

eflowCellList::eflowCellList() :
  m_etaFF(eflowCalo::nRegions),
  m_phiFF(eflowCalo::nRegions) {}

void eflowCellList::setNewExtrapolatedTrack(const eflowTrackCaloPoints* trackCalo) {
  setNewExtrapolatedTrack(*trackCalo);
}

void eflowCellList::setNewExtrapolatedTrack(const eflowTrackCaloPoints& trackCalo) {
  for (int i = 0; i < eflowCalo::nRegions; i++) {
    eflowCaloENUM layer = (eflowCaloENUM)i;
    m_etaFF[layer] = trackCalo.getEta(layer);
    m_phiFF[layer] = trackCalo.getPhi(layer);
  }

  m_cellPositionToCellMap.clear();
}

void eflowCellList::addCell(std::pair<CaloCell*,int> cell) {
  eflowCellPosition myPos(this,cell.first);

  CellIt inmap = m_cellPositionToCellMap.find( myPos );

  if (inmap != m_cellPositionToCellMap.end()) inmap->second.push_back(cell);
  else {
    std::vector<std::pair<CaloCell*,int> > myVec;
    myVec.push_back(cell);
    m_cellPositionToCellMap.insert( std::map<eflowCellPosition, std::vector<std::pair<CaloCell*,int> > >::value_type( myPos, myVec ) );
  }

}

void eflowCellList::reorderWithoutLayers() {
  /* Collect all cells in layer EMB2 and higher, i.e. start with dR = 0 to the track in EMB2 */
  CellIt it = m_cellPositionToCellMap.lower_bound( eflowCellPosition(this, eflowCalo::EMB2, 0.0) );
  CellIt end = m_cellPositionToCellMap.end();
  while (it != end) {
    /* Create a CellPosition in EMB1 that has the same dR to the track as in its original layer */
    eflowCellPosition tempPos(this, eflowCalo::EMB1, it->first.dR());

    /* (Try to) insert the new CellPosition into the map. The resulting iterator either points to the inserted or to the already existing element (but we don't care). */
    std::map<eflowCellPosition, std::vector<std::pair<CaloCell*,int> > >::iterator itInserted =
        m_cellPositionToCellMap.insert( std::make_pair(tempPos, std::vector<std::pair<CaloCell*,int> >(0) ) ).first;

    /* Append the cell/index pairs of the original CellPosition to the (either newly inserted or already existing) element */
    std::vector<std::pair<CaloCell*,int> >& pairVector(it->second);
    itInserted->second.insert(itInserted->second.end(), pairVector.begin(), pairVector.end());

    /* Remove the original CellPosition from the map */
    m_cellPositionToCellMap.erase(it++);
  }
}

double eflowCellList::dR2(double eta, double phi, eflowCaloENUM layer) const {
  if (eflowCalo::Unknown != layer) {
    double dEta = eta - m_etaFF[layer];
    double dPhi = cycle(phi, m_phiFF[layer]) - m_phiFF[layer];
    return dEta * dEta + dPhi * dPhi;
  } else {
    return -999.0;
  }
}

double eflowCellList::dR(double eta, double phi, eflowCaloENUM layer) const {
  return (eflowCalo::Unknown != layer) ? sqrt(dR2(eta, phi, layer)) : -999.0;
}

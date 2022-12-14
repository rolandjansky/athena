/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Given an input eta,phi, deta,dphi - this class will return you
// either an iterator over a list of cells (pointers) in this cone
// and the total cell energy of the cells in this cone.
// (11/10/2004) Method added: returns number of strips for an input
// eta,phi,ncell_eta,ncell_phi. The List of cells replaced by
// the Vector of cells.

#include "CaloUtils/CaloCellList.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloGeoHelpers/proxim.h"

#include <cmath>

namespace {
inline double
square(double x)
{
  return x * x;
}
}

CaloCellList::CaloCellList(const CaloDetDescrManager* mgr, const CaloCellContainer* cell_container)
  : m_cellcont(cell_container)
  , m_mgr(mgr)
  , m_energy(0)
  , m_et(0)
{
  m_caloNums.clear();
  // NSUBCALO indicate take them all
  m_caloNums.push_back(CaloCell_ID::NSUBCALO);
}

// specify one calo number
CaloCellList::CaloCellList(const CaloDetDescrManager* mgr, const CaloCellContainer* cell_container, const CaloCell_ID::SUBCALO caloNum)
  : m_cellcont(cell_container)
  , m_mgr(mgr)  
  , m_energy(0)
  , m_et(0)
{
  m_caloNums.clear();
  m_caloNums.push_back(caloNum);
}

// specify a vector of calonumber
// (if all calo it is more efficient to not specify anything)
CaloCellList::CaloCellList(const CaloDetDescrManager* mgr, const CaloCellContainer* cell_container,
                           const std::vector<CaloCell_ID::SUBCALO>& caloNums)
  : m_cellcont(cell_container)
  , m_mgr(mgr)  
  , m_energy(0)
  , m_et(0)

{
  m_caloNums.clear();
  std::copy(caloNums.begin(), caloNums.end(), back_inserter(m_caloNums));
}


void
CaloCellList::select(double eta, double phi, double deta, double dphi)
{
  doSelect(eta, phi, deta, dphi, -1);
}

void
CaloCellList::select(double eta, double phi, double deta, double dphi, int sam)
{
  doSelect(eta, phi, deta, dphi, -1, static_cast<CaloCell_ID::CaloSample>(sam));
}

void
CaloCellList::select(double eta, double phi, double dR)
{
  doSelect(eta, phi, dR, dR, dR);
}

void
CaloCellList::select(double eta, double phi, double dR, int sam)
{
  doSelect(eta, phi, dR, dR, dR, static_cast<CaloCell_ID::CaloSample>(sam));
}

void
CaloCellList::doSelect(double eta,
                       double phi,
                       double deta,
                       double dphi,
                       double dR,
                       CaloCell_ID::CaloSample sam /*= CaloCell_ID::Unknown*/)
{

  m_theCellVector.clear();
  m_energy = 0;
  m_et = 0;
  m_nBadT0 = 0;
  m_nBadT12 = 0;
  if (m_cellcont->empty()){
    return;
  }
  std::vector<IdentifierHash> calo_mgr_vect;
  std::vector<CaloCell_ID::SUBCALO>::const_iterator itrCaloNum = m_caloNums.begin();
  std::vector<CaloCell_ID::SUBCALO>::const_iterator itrEndCaloNum = m_caloNums.end();

  double dR2 = dR*dR;

  for (; itrCaloNum != itrEndCaloNum; ++itrCaloNum) {
    CaloCell_ID::SUBCALO caloNum = *itrCaloNum;
    if (sam != CaloCell_ID::Unknown) {
      m_mgr->cellsInZone(eta - deta, eta + deta, phi - dphi, phi + dphi, sam, calo_mgr_vect);
      itrCaloNum = itrEndCaloNum - 1;
    } else if (caloNum == CaloCell_ID::NSUBCALO) {
      m_mgr->cellsInZone(eta - deta, eta + deta, phi - dphi, phi + dphi, calo_mgr_vect);
    } else if (caloNum != CaloCell_ID::NOT_VALID) {
      m_mgr->cellsInZone(eta - deta, eta + deta, phi - dphi, phi + dphi, caloNum, calo_mgr_vect);
    } else {
      continue;
    }
    m_theCellVector.reserve(m_theCellVector.size() + calo_mgr_vect.size());

    for (unsigned int i = 0; i < calo_mgr_vect.size(); i++) {
      const CaloCell* cell = m_cellcont->findCell(calo_mgr_vect[i]);
      if (cell) {
	double pphi = proxim(cell->phi(), phi);
	if ( (dR > 0 && square(eta - cell->eta()) + square(pphi - phi) < dR2) ||
	     (dR < 0 && std::fabs(eta - cell->eta()) < deta && std::fabs(phi - pphi) < dphi) ) {
	  m_theCellVector.push_back(cell);
	  m_energy += cell->energy();
	  m_et += cell->et();
	  if (cell->badcell()) {
	    if (sam == CaloCell_ID::TileBar0 || sam == CaloCell_ID::TileExt0)
	      m_nBadT0++;
	    else if (sam == CaloCell_ID::TileBar1 || sam == CaloCell_ID::TileExt1 ||
		     sam == CaloCell_ID::TileBar2 || sam == CaloCell_ID::TileExt2 ||
		     sam == CaloCell_ID::TileGap1 || sam == CaloCell_ID::TileGap2)
	      m_nBadT12++;
	  }
	}
      }
    }
  } // end loop on calorimeters
}


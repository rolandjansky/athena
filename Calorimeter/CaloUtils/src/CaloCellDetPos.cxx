/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// for a given calorimeter layer, this class will convert detector eta,phi to Atlas eta,phi
//  and vice versa, taking into account calorimeter alignment
//  this assumes that the shift is the one of the cell to which the eta-phi position belongs
//  the methods return true if an element of the correct layer is found within the cell, false otherwise

#include "CaloUtils/CaloCellDetPos.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

const CaloPhiRange CaloCellDetPos::s_range;

CaloCellDetPos::CaloCellDetPos()
{}

CaloCellDetPos::~CaloCellDetPos()
{}
bool CaloCellDetPos::getDetPosition(const CaloDetDescrManager& mgr,
                                    CaloCell_ID::CaloSample sam,
                                    double etaAtlas, double phiAtlas,
                                    double& etaDet, double& phiDet) const {

  const CaloDetDescrElement* elt = mgr.get_element(sam,etaAtlas,phiAtlas);
  if (!elt) {
    etaDet = etaAtlas;
    phiDet = phiAtlas;
    return false;
  }
  etaDet = etaAtlas + elt->eta_raw()-elt->eta();
  phiDet = CaloPhiRange::fix(phiAtlas + elt->phi_raw()-elt->phi());
  return true;
}

bool CaloCellDetPos::getAtlasPosition(const CaloDetDescrManager& mgr,
                                      CaloCell_ID::CaloSample sam,
                                      double etaDet, double phiDet,
                                      double& etaAtlas,
                                      double& phiAtlas) const {

  const CaloDetDescrElement* elt = mgr.get_element_raw(sam,etaDet,phiDet);
  if (!elt) {
    etaAtlas = etaDet;
    phiAtlas = phiDet;
    return false;
  }
  etaAtlas = etaDet + elt->eta()-elt->eta_raw();
  phiAtlas = CaloPhiRange::fix(phiDet + elt->phi()-elt->phi_raw());
  return true;
}

bool CaloCellDetPos::getDetPosition(CaloCell_ID::CaloSample sam,
                                    double etaAtlas, double phiAtlas,
                                    double& etaDet, double& phiDet) const {
  const CaloDetDescrManager* calo_mgr;
  calo_mgr = CaloDetDescrManager::instance();
  return getDetPosition(*calo_mgr,sam,etaAtlas,phiAtlas,etaDet,phiDet);
}

bool CaloCellDetPos::getAtlasPosition(CaloCell_ID::CaloSample sam,
                                      double etaDet, double phiDet,
                                      double& etaAtlas,
                                      double& phiAtlas) const {

  const CaloDetDescrManager* calo_mgr;
  calo_mgr = CaloDetDescrManager::instance();
  return getAtlasPosition(*calo_mgr,sam,etaDet,phiDet,etaAtlas,phiAtlas);
}

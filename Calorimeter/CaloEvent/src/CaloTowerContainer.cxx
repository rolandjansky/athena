/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloTowerContainer.cxx
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  S. Rajagopalan
CREATED:  Oct 29, 2000

PURPOSE:

Updated:  Jan 3, 2001    (HM)
          QA.

Updated:  Feb 5, 2001    (HMa)
          Moved from LArClusterRec to CaloRec.


********************************************************************/

// INCLUDE HEADER FILES:

#include "CaloEvent/CaloTowerContainer.h"
#include "CaloEvent/CaloTower.h"
#include "CaloEvent/CaloTowerSeg.h"
#include "CaloIdentifier/CaloCell_ID.h"
#ifdef CALOTOWERCONTAINER_USES_DATAPOOL
#include "AthAllocators/DataPool.h"
#endif

#include <utility>
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdexcept>


/////////////////////////////
// Constructors/Destructor //
/////////////////////////////


CaloTowerContainer::CaloTowerContainer()
  : Base(SG::VIEW_ELEMENTS)
{ }

CaloTowerContainer::CaloTowerContainer(const CaloTowerSeg& theSegmentation,
                                       bool noTowers /*= false*/)
  : Base(SG::VIEW_ELEMENTS),
    m_towerSeg(theSegmentation)
{
  if (!noTowers)
    init();
}

#ifdef CALOTOWERCONTAINER_USES_DATAPOOL
CaloTowerContainer::CaloTowerContainer (const CaloTowerContainer& other)
  : DataVector<INavigable4Momentum>(other),
    Base(other),
    m_towerSeg (other.m_towerSeg),
    m_caloRegions (other.m_caloRegions)
{
}
#else
CaloTowerContainer::CaloTowerContainer (const CaloTowerContainer& other)
  : DataVector<INavigable4Momentum>(),
    Base(other.m_towers.size(), SG::VIEW_ELEMENTS),
    m_towerSeg (other.m_towerSeg),
    m_caloRegions (other.m_caloRegions),
    m_towers (other.m_towers)
{
  setTowers();
}
#endif

CaloTowerContainer&
CaloTowerContainer::operator= (const CaloTowerContainer& other)
{
  if (this != &other) {
    m_towerSeg = other.m_towerSeg;
    m_caloRegions = other.m_caloRegions;
#ifdef CALOTOWERCONTAINER_USES_DATAPOOL
    Base::operator= (other);
#else
    m_towers = other.m_towers;
    setTowers();
#endif
  }
  return *this;
}

void CaloTowerContainer::swap (CaloTowerContainer& other)
{
  if (this != &other) {
    Base::swap (other);
    std::swap (m_towerSeg, other.m_towerSeg);
    m_caloRegions.swap (other.m_caloRegions);
#ifndef CALOTOWERCONTAINER_USES_DATAPOOL
    m_towers.swap (other.m_towers);
#endif
  }
}

void CaloTowerContainer::init()
{
  // pre-size the container
  Base::clear (SG::VIEW_ELEMENTS);
  index_t etaBins = (index_t)m_towerSeg.neta();
  index_t phiBins = (index_t)m_towerSeg.nphi();
#ifdef CALOTOWERCONTAINER_USES_DATAPOOL
  Base::resize (etaBins * phiBins);
#else
  this->m_towers.resize (etaBins * phiBins);
  setTowers();
#endif


  // get the eta/phi segmentation
  double deltaEta = m_towerSeg.deta();
  double minEta   = m_towerSeg.etamin() + deltaEta / 2.;
  double deltaPhi = m_towerSeg.dphi();
  double minPhi   = CaloPhiRange::fix (m_towerSeg.phimin() + deltaPhi / 2.);

  // insert empty towers
  // NOTE: eta/phi indexing is 1-based.
  for (index_t etaIndex = 1; etaIndex <= etaBins; ++etaIndex) {
    double theEta = minEta + (etaIndex-1) * deltaEta;
    for (index_t phiIndex = 1; phiIndex <= phiBins; ++phiIndex) {
      double thePhi = CaloPhiRange::fix (minPhi + (phiIndex-1) * deltaPhi);
      index_t towerIndex   = this->getTowerIndex(etaIndex,phiIndex);
#ifdef CALOTOWERCONTAINER_USES_DATAPOOL
      DataPool<CaloTower> towersPool (etaBins * phiBins);
      CaloTower& tower = *towersPool.nextElementPtr();
      Base::operator[] (towerIndex) = &tower;
      tower.removeCells();
      tower.setE(0);
#else
      CaloTower& tower = m_towers[towerIndex];
#endif
      tower.setEta (theEta);
      tower.setPhi (thePhi);
    }
  }
}

#ifndef CALOTOWERCONTAINER_USES_DATAPOOL
void CaloTowerContainer::setTowers()
{
  assert (m_towers.size() == m_towerSeg.neta() * m_towerSeg.nphi() ||
          m_towers.empty());
  Base::resize (m_towers.size());
  for (size_t i = 0; i < m_towers.size(); i++)
    Base::operator[](i) = &m_towers[i];
}
#endif

CaloTowerContainer::~CaloTowerContainer()
{
}

//////////////////////////////////
// Included Calorimeter Modules //
//////////////////////////////////

void CaloTowerContainer::setCalo(const CaloCell_ID::SUBCALO& theCalo)
{
  unsigned int intCalo = theCalo ;
  if ( std::find(m_caloRegions.begin(),m_caloRegions.end(),intCalo)
       == m_caloRegions.end() )
    {
      m_caloRegions.push_back(intCalo);
    }
}

size_t
CaloTowerContainer::getCalos(std::vector<CaloCell_ID::SUBCALO>& theCalos) const
{
  theCalos.resize(m_caloRegions.size());
  std::vector<unsigned int>::size_type i;
  for(i=0;i<m_caloRegions.size();++i)
	theCalos[i] = (CaloCell_ID::SUBCALO) m_caloRegions[i];

//   std::copy(m_caloRegions.begin(),m_caloRegions.end(),theCalos.begin());
  return theCalos.size();
}

///////////////////
// Tower Binning //
///////////////////

// all inlined, see .h file

///////////////////////
// Kinematics Access //
///////////////////////

// energy by index
double CaloTowerContainer::energy(index_t etaIndex, index_t phiIndex) const
{
  const CaloTower* t = this->getTower(etaIndex,phiIndex);
  return t ? t->e() : 0;
}
// energy in (eta,phi)
double CaloTowerContainer::energy(double theEta, double thePhi) const
{
  const CaloTower* t = this->getTower(theEta, thePhi);
  return t ? t->e() : 0;
}

// et by index
double CaloTowerContainer::et(index_t etaIndex, index_t phiIndex) const
{
  const CaloTower* t = this->getTower(etaIndex, phiIndex);
  return t ? t->et() : 0;
}


// et in (eta,phi)
double CaloTowerContainer::et(double theEta, double thePhi) const
{
  const CaloTower* t = this->getTower(theEta, thePhi);
  return t ? t->et() : 0;
}


/////////////////////////////////////////
// Tower Access & Container Management //
/////////////////////////////////////////

// returns both indices at once
bool CaloTowerContainer::getTowerIndices(const CaloTower* aTower,
					 index_t& etaIndex,
					 index_t& phiIndex) const
{
  return aTower != nullptr
    ? this->getTowerIndices(aTower->eta(),aTower->phi(),etaIndex,phiIndex)
    : false;
}
bool CaloTowerContainer::getTowerIndices(double theEta, double thePhi,
					 index_t& etaIndex,
					 index_t& phiIndex) const
{
  etaIndex = this->getTowerEtaIndex(theEta);
  phiIndex = this->getTowerPhiIndex(thePhi);
  return ( etaIndex != m_outOfRange && phiIndex != m_outOfRange );
}

// returns CaloTower pointer by index
const CaloTower* CaloTowerContainer::getTower(index_t etaIndex,
					      index_t phiIndex) const

{
  index_t theIndex = this->getTowerIndex(etaIndex,phiIndex);
  return theIndex != m_outOfRange
#ifdef CALOTOWERCONTAINER_USES_DATAPOOL
    ? Base::operator[](theIndex)
#else
    ? &m_towers[theIndex]
#endif
    : (const CaloTower*)nullptr;
}
CaloTower* CaloTowerContainer::getTower(index_t etaIndex, index_t phiIndex)
{
  index_t theIndex = this->getTowerIndex(etaIndex,phiIndex);
  return theIndex != m_outOfRange
#ifdef CALOTOWERCONTAINER_USES_DATAPOOL
    ? Base::operator[](theIndex)
#else
    ? &m_towers[theIndex]
#endif
    : (CaloTower*)nullptr;
}
const CaloTower* CaloTowerContainer::getTower(double theEta,
					      double thePhi) const
{
  index_t etaIndex = m_outOfRange;
  index_t phiIndex = m_outOfRange;
  return this->getTowerIndices(theEta,thePhi,etaIndex,phiIndex)
    ? (this->operator[])(this->getTowerIndex(etaIndex,phiIndex))
    : (const CaloTower*)nullptr;
}
CaloTower* CaloTowerContainer::getTower(double theEta, double thePhi)
{
  index_t etaIndex = m_outOfRange;
  index_t phiIndex = m_outOfRange;
  return this->getTowerIndices(theEta,thePhi,etaIndex,phiIndex)
    ? (this->operator[])(this->getTowerIndex(etaIndex,phiIndex))
    : (CaloTower*)nullptr;
}


void CaloTowerContainer::clear (SG::OwnershipPolicy p)
{
  if (m_towerSeg.neta() > 0 || m_towerSeg.nphi() > 0)
    throw std::runtime_error ("Bad call to CaloTowerContainer::clear");
  Base::clear (p);
}

void CaloTowerContainer::insert (iterator position,
                                 iterator first,
                                 iterator last)
{
  if (m_towerSeg.neta() > 0 || m_towerSeg.nphi() > 0 ||
      ownPolicy() != SG::VIEW_ELEMENTS)
  {
    throw std::runtime_error ("Bad call to CaloTowerContainer::insert");
  }
  Base::insert (position.base(), first.base(), last.base());
}

void CaloTowerContainer::push_back(value_type pElem)
{
  if (m_towerSeg.neta() > 0 || m_towerSeg.nphi() > 0 ||
      ownPolicy() != SG::VIEW_ELEMENTS)
  {
    throw std::runtime_error ("Bad call to CaloTowerContainer::push_back");
  }
  Base::push_back (pElem);
}



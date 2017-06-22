/* Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration */
#include "CaloEvent/CaloCellTowerLink.h"

#include "Identifier/IdentifierHash.h"
#include "CaloEvent/CaloCell.h"

#include <cstdio>

CaloCellTowerLink::Element::Element() : base_t(index_t(),weight_t()) 
{ }
CaloCellTowerLink::Element::Element(index_t idx,weight_t wght) : base_t(idx,wght)
{ }

CaloCellTowerLink::index_t  CaloCellTowerLink::Element::index() const 
{ return this->get<0>(); }
CaloCellTowerLink::weight_t CaloCellTowerLink::Element::weight() const
{ return this->get<1>(); }

CaloCellTowerLink::ElementList CaloCellTowerLink::Map::m_emptyRef = CaloCellTowerLink::ElementList();

CaloCellTowerLink::Map::Map() : ElementLookup() 
{ printf("CaloCellToTowerLink::Map instantiated with size %i\n",(int)this->size()); }
CaloCellTowerLink::Map::Map(size_t size) : ElementLookup(size,CaloCellTowerLink::ElementList()) 
{ printf("CaloCellToTowerLink::Map instantiated with size %i\n",(int)this->size()); }

const CaloCellTowerLink::ElementList& CaloCellTowerLink::Map::elementList(const CaloCell* pCell) const
{
  // get the cell description
  const CaloDetDescrElement* dde = pCell->caloDDE();
  // return element list 
  return this->elementList(dde->calo_hash());
}

const CaloCellTowerLink::ElementList& CaloCellTowerLink::Map::elementList(IdentifierHash hashId) const
{ return hashId < this->size() ? this->at(hashId) : m_emptyRef; }

void CaloCellTowerLink::Map::addCell(const CaloCell* pCell,index_t towerIndex,weight_t weight)
{
  // get the cell description
  const CaloDetDescrElement* dde = pCell->caloDDE();
  // get the cell hash id
  addCell(dde->calo_hash(),towerIndex,weight);
}

void CaloCellTowerLink::Map::addCell(IdentifierHash hashId,index_t towerIndex,weight_t weight)
{
  // set/add element
  if ( static_cast<size_t>(hashId) >= this->size() ) { this->resize(hashId); }
  (*this)[hashId].push_back(Element(towerIndex,weight));
}

bool CaloCellTowerLink::Map::setCell(const CaloCell* pCell,const index_vec_t& towerIndices,const weight_vec_t& weights)
{
  // basic check
  if ( towerIndices.size() != weights.size() ) { return false; }
  // get the cell description
  const CaloDetDescrElement* dde = pCell->caloDDE();
  // set cell links using hash id
  return setCell(dde->calo_hash(),towerIndices,weights);
} 

bool CaloCellTowerLink::Map::setCell(IdentifierHash hashId,const index_vec_t& towerIndices,const weight_vec_t& weights)
{
  // basic check
  if ( towerIndices.size() != weights.size() ) { return false; }
  // set cell in map
  if ( static_cast<size_t>(hashId) >= this->size() ) { this->resize(hashId); }
  // printf("CaloCellTowerLink::setCell(...) cell [%06i/%06i] number of Elements (%i/%i)\n",(int)hashId,(int)this->size(),(int)towerIndices.size(),(int)weights.size());
  // fill links
  for ( size_t i(0); i<towerIndices.size(); ++i ) { (*this)[hashId].push_back(Element(towerIndices.at(i),weights.at(i))); } 
  return true;
} 

void CaloCellTowerLink::Map::reset()
{ for ( auto el : *this ) { el.clear(); } }

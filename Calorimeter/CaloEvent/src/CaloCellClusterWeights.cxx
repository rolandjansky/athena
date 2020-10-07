/* Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration */
#include "CaloEvent/CaloCellClusterWeights.h"

#include "CaloEvent/CaloCell.h"

#include <algorithm>

#ifndef CELLCLUSTERLOOKUP
#define CELLCLUSTERLOOKUP 200000
#endif

CaloCellClusterWeights::CaloCellClusterWeights(size_t size)
  : m_hashTable(size)
{
  this->clear(); 
}

CaloCellClusterWeights::CaloCellClusterWeights() 
  : CaloCellClusterWeights( CELLCLUSTERLOOKUP )
{ }

CaloCellClusterWeights::CaloCellClusterWeights(const CaloCellClusterWeights& cellClusterWeights)
  : m_hashTable(cellClusterWeights.m_hashTable)
{ }

CaloCellClusterWeights::~CaloCellClusterWeights()
{ }

const CaloCellClusterWeights::weight_t& CaloCellClusterWeights::operator[](size_t hash)   const 
{ return this->check(hash) ? m_hashTable.at(hash).get<1>() : m_defaultValue; }
const CaloCellClusterWeights::weight_t& CaloCellClusterWeights::at(size_t hash)           const 
{ return this->check(hash) ? m_hashTable.at(hash).get<1>() : m_defaultValue; }
const CaloCellClusterWeights::weight_t& CaloCellClusterWeights::at(const CaloCell* pCell) const 
{ return this->at(static_cast<size_t>(pCell->caloDDE()->calo_hash())); }

bool CaloCellClusterWeights::fastCheck(size_t hash)           const { return m_hashTable.at(hash).get<0>(); }
bool CaloCellClusterWeights::fastCheck(const CaloCell* pCell) const { return this->fastCheck(static_cast<size_t>(pCell->caloDDE()->calo_hash())); } 
bool CaloCellClusterWeights::check(size_t hash)               const { return hash < m_hashTable.size() && this->fastCheck(hash); }
bool CaloCellClusterWeights::check(const CaloCell* pCell)     const { return this->check(static_cast<size_t>(pCell->caloDDE()->calo_hash())); } 

void CaloCellClusterWeights::set(size_t hash,double value)           
{ 
  if ( hash < m_hashTable.size() ) {
    if ( this->fastCheck(hash) ) { 
      m_hashTable[hash].get<1>().push_back(value); 
    } else {
      m_hashTable[hash].get<0>() = true;
      m_hashTable[hash].get<1>().push_back(value); 
    } 
  }
}

void CaloCellClusterWeights::set(const CaloCell* pCell,double value) { this->set(static_cast<size_t>(pCell->caloDDE()->calo_hash()),value); } 

void CaloCellClusterWeights::clear() { std::fill(m_hashTable.begin(),m_hashTable.end(),value_t(false,m_defaultValue)); }
void CaloCellClusterWeights::clear(size_t hash) { 
  if ( hash < m_hashTable.size() ) { m_hashTable[hash].get<0>() = false; m_hashTable[hash].get<1>() = m_defaultValue; } 
}
void CaloCellClusterWeights::reset() { this->clear(); }

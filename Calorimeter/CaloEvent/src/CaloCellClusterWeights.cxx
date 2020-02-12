/* Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration */
#include "CaloEvent/CaloCellClusterWeights.h"

#include "CaloEvent/CaloCell.h"

#include <algorithm>

#ifndef CELLCLUSTERLOOKUP
#define CELLCLUSTERLOOKUP 200000
#endif

CaloCellClusterWeights::weight_t CaloCellClusterWeights::_defaultValue = weight_t();

CaloCellClusterWeights::CaloCellClusterWeights(size_t size)
  : _hashTable(size)
{
  this->clear(); 
}

CaloCellClusterWeights::CaloCellClusterWeights() 
  : CaloCellClusterWeights( CELLCLUSTERLOOKUP )
{ }

CaloCellClusterWeights::CaloCellClusterWeights(const CaloCellClusterWeights& cellClusterWeights)
  : _hashTable(cellClusterWeights._hashTable)
{ }

CaloCellClusterWeights::~CaloCellClusterWeights()
{ }

const CaloCellClusterWeights::weight_t& CaloCellClusterWeights::operator[](size_t hash)   const 
{ return this->check(hash) ? _hashTable.at(hash).get<1>() : _defaultValue; }
const CaloCellClusterWeights::weight_t& CaloCellClusterWeights::at(size_t hash)           const 
{ return this->check(hash) ? _hashTable.at(hash).get<1>() : _defaultValue; }
const CaloCellClusterWeights::weight_t& CaloCellClusterWeights::at(const CaloCell* pCell) const 
{ return this->at(static_cast<size_t>(pCell->caloDDE()->calo_hash())); }

bool CaloCellClusterWeights::fastCheck(size_t hash)           const { return _hashTable.at(hash).get<0>(); }
bool CaloCellClusterWeights::fastCheck(const CaloCell* pCell) const { return this->fastCheck(static_cast<size_t>(pCell->caloDDE()->calo_hash())); } 
bool CaloCellClusterWeights::check(size_t hash)               const { return hash < _hashTable.size() && this->fastCheck(hash); }
bool CaloCellClusterWeights::check(const CaloCell* pCell)     const { return this->check(static_cast<size_t>(pCell->caloDDE()->calo_hash())); } 

void CaloCellClusterWeights::set(size_t hash,double value)           
{ 
  if ( hash < _hashTable.size() ) {
    if ( this->fastCheck(hash) ) { 
      _hashTable[hash].get<1>().push_back(value); 
    } else {
      _hashTable[hash].get<0>() = true;
      _hashTable[hash].get<1>().push_back(value); 
    } 
  }
}

void CaloCellClusterWeights::set(const CaloCell* pCell,double value) { this->set(static_cast<size_t>(pCell->caloDDE()->calo_hash()),value); } 

void CaloCellClusterWeights::clear() { std::fill(_hashTable.begin(),_hashTable.end(),value_t(false,_defaultValue)); }
void CaloCellClusterWeights::clear(size_t hash) { 
  if ( hash < _hashTable.size() ) { _hashTable[hash].get<0>() = false; _hashTable[hash].get<1>() = _defaultValue; } 
}
void CaloCellClusterWeights::reset() { this->clear(); }

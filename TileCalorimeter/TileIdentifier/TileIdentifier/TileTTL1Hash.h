/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileTTL1Hash.h
// Author   : jose.maneira@cern.ch
// Created  : Nov 2005
//
// DESCRIPTION
//
// Provides hash (integer) conversion for TileTTL1 identifiers
//
// BUGS:
//
// History:
//
//
//****************************************************************************

#ifndef TileTTL1Hash_H
#define TileTTL1Hash_H

class CaloLVL1_ID;

#include <vector> 

/**
* @class TileTTL1Hash
* @brief Hash table for TileTTL1 identifiers  
* @author Jose Maneira
*
*
* Hash table Identifier builds a table of
*  tower identifiers
*  1920 in total for ATLAS
*/
class TileTTL1Hash  {

 public:

  typedef int ID; 
  TileTTL1Hash ();
  virtual ~TileTTL1Hash() { };

  void initialize(const CaloLVL1_ID * caloLVL1_ID); 

  /**Convert ID to int */
  int operator() (const ID& id) const ;

  /** reverse conversion */
  ID identifier(int i) const;   

  /**return maximum number of IDs */
  int max() const {return m_size;}
  
  
 private:
  /** total number of IDs */
  int m_size;  // total number of IDs

  /** lookup table */
  static const int m_table_size = 7000;
  int m_lookup[m_table_size] ;

  std::vector<ID> m_int2id;

};

#endif

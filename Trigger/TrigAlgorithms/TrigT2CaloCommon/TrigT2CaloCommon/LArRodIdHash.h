/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     LArRodIdHash.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCommon
 
 AUTHOR:   Denis Oliveira Damazio

 PURPOSE:  LArReadoutModuleID to integer (hash ID) mapping.
 *******************************************************************/

#ifndef TRIGT2CALOCOMMON_LARRODIDHASH_H
#define TRIGT2CALOCOMMON_LARRODIDHASH_H


#include <vector> 
#include <map>

/** class that provides LArReadoutModuleID to integer
    hash ID mapping.  */
class LArRodIdHash  {

 public:

  /** definition of ID type */
  typedef unsigned int ID; 

  /** Initialize. Here real map is built */
  void initialize(int offset ); 

  /**Convert ID to int */
  int operator() (const ID& id) const ;

  /**return maximum number of IDs */
  int max() const; 

  /** reverse conversion */
  ID identifier(int i) const;   

  /** return  offset */
  int offset() const ;

 private:
  /** total number of IDs */
  int m_size{0};  // total number of IDs

  /** test beam flag */
  std::map<unsigned int, int> m_lookup ;

  /** reverse look up */ 
  std::vector<ID> m_int2id;

  int m_offset{0};

};

#endif


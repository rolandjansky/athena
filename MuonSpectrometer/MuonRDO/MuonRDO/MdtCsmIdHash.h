/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDIGITCONATINER_MDTCSMIDHASH_H
#define MUONDIGITCONATINER_MDTCSMIDHASH_H


#include "MuonIdHelpers/MdtIdHelper.h" 
#include <map>

/** Hash function for Mdt module Identifier
    to be used in MdtDigitContainer (IdentifiableContainer) 
 * @author Hong Ma
 * @version 00-00-01 , Aug 28, 2002
 */
class MdtCsmIdHash  {

 public:

  typedef Identifier ID; 

  MdtCsmIdHash ();
  virtual ~MdtCsmIdHash() { };


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
  int m_size;  // total number of IDs

  /** lookup table 
  */
  std::map<Identifier,int>  m_lookup;

  /** reverse lookup 
  */ 
  std::vector<ID> m_int2id;

};

#endif


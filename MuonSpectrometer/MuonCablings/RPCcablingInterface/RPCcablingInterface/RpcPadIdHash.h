/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDIGITCONATINER_RPCPADIDHASH_H
#define MUONDIGITCONATINER_RPCPADIDHASH_H


#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "RPCcablingInterface/IRPCcablingSvc.h"


#include <map>

/** Hash function for Rpc module Identifier
    to be used in RpcDigitContainer (IdentifiableContainer) 
 * @author Hong Ma
 * @version 00-00-01 , Aug 28, 2002
 * Adapted by Ketevi A. Assamagan for RPC: September 25 2002
 * Adapted by Ketevi A. Assamagan for RPC PAD: December 7 2002
 */

class RpcPadIdHash  {

 public:

  typedef Identifier ID; 

  RpcPadIdHash ();
  virtual ~RpcPadIdHash() { };

  /**Convert ID to int */
  int operator() (const ID& id) const ;

  /**return maximum number of IDs */
  int max() const; 

  /** reverse conversion */
  ID identifier(int i) const;   

  /** return  offset */
  int offset() const ;
  
  std::vector<IdentifierHash> rod2hash(uint16_t subsystem, uint16_t rod) const;
  
  uint32_t hash2source(unsigned int h) const;

 private:
  /** total number of IDs */
  int m_size;  // total number of IDs

  /** lookup table 
  */
  std::map<Identifier,int>  m_lookup;

  /** reverse lookup 
  */ 
  std::vector<ID> m_int2id;
  std::vector<IdentifierHash> m_rod2hash[2][16];
  const IRPCcablingSvc* m_cabling;

};

#endif


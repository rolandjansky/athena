/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_TGCRDOIDHASH_H
#define MUONRDO_TGCRDOIDHASH_H

#include <map>
#include <vector>
#include <inttypes.h>

/*
  Hash function for TGC RDO Identifier
    @author Tadashi Maeno
       based on RpcPadIdHash by Ketevi A. Assamagan
*/
class TgcRdoIdHash
{
public:

  typedef uint16_t ID; 

  TgcRdoIdHash ();
  virtual ~TgcRdoIdHash() {}

  /**Convert ID to int. return INVALID_ID if invalid ID */
  int operator() (const ID& id) const ;
  
  /**return maximum number of IDs */
  int max() const {return m_size;} 

  /** reverse conversion */
  ID identifier(int i) const;   

  /** reverse conversion for SubDetectorID and ROD ID*/
  uint16_t subDetectorId(int i) const;
  uint16_t rodId(int i) const;

  // return value definition
  enum {INVALID_ID = 0xFFFF};

private:
  bool getIsAtlas() const;

  /** total number of IDs */
  int m_size;

  /** lookup table 
  */
  std::map<ID,int>  m_lookup;

  /** reverse lookup 
  */ 
  std::vector<ID>  m_int2id;

  /** reverse lookup for SubDetectorID and ROD ID
   */ 
  std::vector<uint16_t> m_int2subDetectorId;
  std::vector<uint16_t> m_int2rodId;
};

#endif


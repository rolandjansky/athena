/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECEVENT_TILERODIDHASH_H
#define TILERECEVENT_TILERODIDHASH_H


#include <vector> 
#include <map>

/**
* @class TileRodIdHash
* @brief Table to convert ROB ID to fragment hash
* @author Alexander Solodkov
*
*/

class TileRodIdHash  {

 public:

  typedef int ID; 
  TileRodIdHash ();
  virtual ~TileRodIdHash() { };

  /** initialize */
  void initialize(int offset, int runnum );

  /** new initialize for the HLT */
  void initialize (int offset, const std::vector<ID>& rmod );

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

  /** test beam flag */

  std::map<unsigned int, int> m_lookup ;

  /** reverse look up */ 
  std::vector<ID> m_int2id;

  int  m_offset; 

};

#endif


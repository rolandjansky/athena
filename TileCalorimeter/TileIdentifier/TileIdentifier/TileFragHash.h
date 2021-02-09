/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TileFragHash_H
#define TileFragHash_H

class TileHWID;

#include <vector> 

/**
* @class TileFragHash
* @brief Hash table for Tile fragments (==drawers  ==collections in StoreGate) 
* @author Alexander Solodkov
*
* Hash table Identifier builds a table of
* drawer identifiers
* 4 * 64 in total for ATLAS
* only 32 in the testbeam
*/
class TileFragHash  {

 public:

  typedef int ID; 
  TileFragHash ();
  virtual ~TileFragHash() { };

  /** initialize */
  enum TYPE {Beam=255, Default=0, Digitizer=0,
             OptFilterDsp=1, OptFilterOffline=2, OptFilterDspCompressed=3,
             ManyAmps=4, MF=5, FitFilter=6, FitFilterCool=7, FlatFilter=8,
             WienerFilterOffline=9 };

  void initialize(const TileHWID * tileHWID, TYPE type=Default ); 

  /**Convert ID to int */
  int operator() (const ID& id) const ;

  /** reverse conversion */
  ID identifier(int i) const;   

  /**return maximum number of IDs */
  int max() const {return m_size;}
  
  /**return offset */
  int offset() const {return m_offset;}   

  /**return hash type */
  TYPE type() const {return m_type;}
  
 private:
  /** total number of IDs */
  int m_size;  // total number of IDs
  int m_offset; // different offset for different types of collections
  TYPE m_type;

  /** lookup table */
  static const int m_table_size = 0x500;
  int m_lookup[m_table_size] ;

  std::vector<ID> m_int2id;

};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECELLCONT_H
#define TILECELLCONT_H

#include "TileIdentifier/TileRodIdHash.h"

#include <map>
#include <vector>

class TileCellCollection;
class TileHid2RESrcID;

  
/**
 * @class TileCellCont
 * @brief This class builds the Tile cells
 * @author Denis Oliveira Damazio
 * @author Alexander Solodkov
 */

class TileCellCont : public std::vector<TileCellCollection*>
{ 
  
 public:
  
  typedef TileCellCollection IDENTIFIABLE;
  
  /** constructor */
  TileCellCont( ) ;
  
  /** Finds a collection */
  const std::vector<TileCellCollection*>::const_iterator& find(const unsigned int& id) const ;
  unsigned int find_rod(const unsigned int& id) const ;
  
  /** destructor */
  virtual ~TileCellCont() { }

  /**
   * - section = 0 for barrel positive
   * - section = 1 for barrel negative
   * - section = 2 for extended barrel positive
   * - section = 3 for extended barrel negative
   */
  const std::vector<int>& Rw2CellMap ( int section ) {
    return m_Rw2Cell[section];
  }
  TileCellCollection* MBTS_collection( void ) const
        { return m_MBTS; }
  std::map<unsigned int, unsigned int>& MBTS_map ( void )
        { return m_mapMBTS; }
  const std::vector<unsigned int>* MBTS_RODs()
        { return &m_mbts_rods; }
  const std::vector<unsigned int>* MBTS_IDs()
        { return &m_mbts_IDs; }
  int MBTS_channel ( void ) { return m_MBTS_channel; }
  
  StatusCode initialize( void ) ;
  StatusCode finalize( void ) ;
  void setHashIdToROD ( const TileHid2RESrcID* p )
       { m_src = p; }

  void eventNumber ( const unsigned int eN) { m_event=eN; };
  bool cached( const unsigned int&rodid ){
   if ( m_eventNumber[rodid] != m_event ) { // Decoding a new event
   // Keep track of last decoded number
   m_eventNumber[rodid] = m_event;
   return false;
   } else return true;
  }
  const std::vector<int>& Rw2PmtMap ( int section ) {
    return m_Rw2Pmt[section];
  }
  const std::vector<int>& masked_drawers() {
	return m_masked;
  }
  
private:
  
        TileRodIdHash m_hash;
        // One needs to keep track of Rw2Cell
        std::vector<int> m_Rw2Cell[4];
        // One needs to keep track of Rw2Pmt
        std::vector<int> m_Rw2Pmt[4];
        // TileHid2RESrcID* m_conv;
	mutable std::vector<TileCellCollection*>::const_iterator* m_it;
	/** eventNumber of a given Collection */
	mutable std::vector<unsigned int> m_eventNumber;
	/** this event number */
	unsigned int m_event;
	// No Region Selector for MBTS
	std::vector<unsigned int> m_mbts_rods;
	std::vector<unsigned int> m_mbts_IDs;
	TileCellCollection* m_MBTS;
	std::map<unsigned int, unsigned int> m_mapMBTS;
	int m_MBTS_channel;
	std::vector<int> m_masked;
	unsigned int m_forceRun;
        /** map Hash ID to ROD */
        const TileHid2RESrcID* m_src;
  
};

#endif


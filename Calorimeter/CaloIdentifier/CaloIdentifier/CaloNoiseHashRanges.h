//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CALOCONDITIONS_CALONOISEHASHRANGES
#define CALOCONDITIONS_CALONOISEHASHRANGES

#include <map>
#include "Identifier/IdentifierHash.h"

class CaloCell_ID;


/**
 * @class CaloNoiseHashRanges
 * @brief Helper to calculate hash offsets in each COOL channel 
 */
class CaloNoiseHashRanges {

 public:
  //Delete default constructor
  CaloNoiseHashRanges() = delete;

  /**
   * @brief Constructor.
   * @param cellId @c CaloCell_ID pointer to a CaloCell_ID object
   *
   * Initializes the hash ranges
   */
  CaloNoiseHashRanges(const CaloCell_ID* cellId);
  

  ~CaloNoiseHashRanges() = default;

   //SYSTEM == COOL channel number
  enum SYSTEM{EMECZNEG = 0,
              EMBZNEG  = 1,
              EMBZPOS  = 2,
              EMECZPOS = 3,
              HEC      =16,
              FCAL     =32,
              TILE     =48};


  //Getters:

  /// Return a map of cool-channel and hash-offset
  const std::map<SYSTEM,IdentifierHash>& hashOffsets() const {return  m_hashOffsets;}
  /// Return the number of LAr cells
  std::size_t maxLArCells() const {return m_maxLArCells;}
  /// Return the number of Tile cells
  std::size_t maxTileCells() const {return m_maxTileCells;}


private:
  std::map<SYSTEM,IdentifierHash> m_hashOffsets;
  std::size_t m_maxLArCells;
  std::size_t m_maxTileCells;

};

#endif

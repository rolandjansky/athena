/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCONDITIONS_CALOCELLPEDSHIFT
#define CALOCONDITIONS_CALOCELLPEDSHIFT

#include "Identifier/IdentifierHash.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include <array>
#include <vector>

class CaloCell_ID;

class CaloCellPedShift {

 public:
  CaloCellPedShift(const CaloCell_ID* cellid);

  //Getter:
  float pedShift(const IdentifierHash hash, const int gain) const; 

  //Setter:
  void setPedShift(const IdentifierHash hash, const int gain, const float value);

  
 private:
  //Underlying data structure:
  //LAr has 3 gains:
  std::array<std::vector<float>,3> m_larValues;
  //Tile has 4 gains:
  std::array<std::vector<float>,4> m_tileValues;
  
  size_t m_tileMinHash;

};


inline 
float CaloCellPedShift::pedShift(const IdentifierHash hash, const int gain) const {
  if (hash<m_tileMinHash) {
    return m_larValues[gain][hash];
  }
  else {
    return m_tileValues[gain][hash-m_tileMinHash];
  }
}

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( CaloCellPedShift,  196920958, 1 )
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF(CaloCellPedShift,56606546 );
#endif


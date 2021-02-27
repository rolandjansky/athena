/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "CaloIdentifier/CaloNoiseHashRanges.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include <array>

CaloNoiseHashRanges::CaloNoiseHashRanges(const CaloCell_ID* caloCellID) {

  std::array<IdentifierHash,4> emHashRanges {IdentifierHash(),IdentifierHash(),IdentifierHash(),IdentifierHash()};
  IdentifierHash emHashBegin, emHashEnd;
  caloCellID->calo_cell_hash_range(CaloCell_ID::LAREM, emHashBegin, emHashEnd);
  for (IdentifierHash cellHash=emHashBegin;cellHash != emHashEnd; cellHash+=1) {
    const Identifier id =  caloCellID->cell_id(cellHash);
    int sys = 0;
    if ( caloCellID->is_em_barrel(id)) {
      sys = ( caloCellID->pos_neg(id) > 0) ? EMBZPOS : EMBZNEG;
    } else if ( caloCellID->is_em_endcap(id)) {
      sys = ( caloCellID->pos_neg(id) > 0) ? EMECZPOS : EMECZNEG;
    }
    if (!emHashRanges[sys].is_valid()) 
      emHashRanges[sys]=cellHash;
  }

  m_hashOffsets[EMECZNEG]=emHashRanges[EMECZNEG];
  m_hashOffsets[EMBZNEG]=emHashRanges[EMBZNEG];
  m_hashOffsets[EMBZPOS]=emHashRanges[EMBZPOS];
  m_hashOffsets[EMECZPOS]=emHashRanges[EMECZPOS];
  
  IdentifierHash from, to;
  caloCellID->calo_cell_hash_range(CaloCell_ID::LARHEC,from,to);
  m_hashOffsets[HEC]=from;
  
  caloCellID->calo_cell_hash_range(CaloCell_ID::LARFCAL,from,to);
  m_hashOffsets[FCAL]=from;
  m_maxLArCells=to;

  caloCellID->calo_cell_hash_range(CaloCell_ID::TILE,from,to);
  m_hashOffsets[TILE]=from;
  m_maxTileCells=to-from;

  return;
}

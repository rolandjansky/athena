/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileHitVecToCnt.h
// Author   : Alexandre Solodkov
// Created  : Dec 2003
//
// DESCRIPTION
//
//    TileHitVecToCnt copies all TileHits from TileHitVector (AthenaHitsVector)
//    to TileHitContainer (identifiable container) without any corrections
//    If pileup option is set, all hits are merged, if several hits have
//    the same ID, energy and time from all of them is stored in vectors
//    inside single TileHit
//
// Properties (JobOption Parameters):
//
//    TileHitVectors            string   Name of TileHitVector to read
//    TileHitContainer          string   Name of TileHitContainer to write
//    TileInfoName              string   Name of object in TDS with all parameters
//    PileUp                    bool     Do or not pileup
//    HitTimeFlag               int      Working with hit times
//
// BUGS:
//
// History:
//   10Jan04  Created from TileCellIDCToCell.
//   10Feb04  pileup flag added
///  20Feb04  renamed to TileHitVector
//
//****************************************************************************

#ifndef TILESIMALGS_TILEHITVECTOCNT_H
#define TILESIMALGS_TILEHITVECTOCNT_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class IPileUpTool;

/** Top algorithm class for TileHitVecToCnt */
class TileHitVecToCnt : public AthAlgorithm {

 public:

  /** Constructor with parameters */
  TileHitVecToCnt(const std::string &name,ISvcLocator *pSvcLocator);

  /** Basic algorithm methods */
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:
   ToolHandle<IPileUpTool> m_digTool;
};

#endif // TILESIMALGS_TILEHITVECTOCNT_H

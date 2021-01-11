/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REGSELENUMS_H
#define REGSELENUMS_H

//! An enum to define Muon chambers names.
enum TYPEID{
  BIL = 1,
  BIS = 2,
  BML = 3,
  BMS = 4,
  BOL = 5,
  BOS = 6,
  BMF = 9,
  BIR = 8,
  BOF = 10,
  BOG = 11 
};

//! An enum to define subdetector names.
enum DETID{
  PIXEL = 20,
  SCT,
  TRT,
  LAR,
  TTEM,
  TTHEC,
  TILE,
  MDT,
  RPC, 
  TGC,
  CSC,
  FCALEM,
  FCALHAD,
  MM,
  STGC
};


//! An enum to define identifier types.
enum IDENT{
  ROBID = 40,
  HASHID
};

//! An enum to define output types.
enum OUTTYPE{
  HASH = 50,
  UINT32,
  IDENTHASH
};
#endif

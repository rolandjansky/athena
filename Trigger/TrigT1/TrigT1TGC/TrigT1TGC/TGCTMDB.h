/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1TGC_TGCTMDB_H
#define TrigT1TGC_TGCTMDB_H

#include "TrigT1TGC/TGCTMDBOut.h"

namespace LVL1TGCTrigger {

class TGCTMDB
{
 public:
  TGCTMDB();
  virtual ~TGCTMDB();
 
  TGCTMDB(const TGCTMDB& right);
  TGCTMDB& operator=(const TGCTMDB& right);
  int operator==(const TGCTMDB& right) const { return (this==&right); }
  int operator!=(const TGCTMDB& right) const { return (this!=&right); }

  enum { NumberOfTileModule = 64 };

  const TGCTMDBOut* getOutput(int side, int moduleID) const;
  const TGCTMDBOut* getOutput(int side, int sectorID, int mod) const;
  void  setOutput(const int side, const int module, 
                  const TGCTMDBOut::TileModuleHit hit56, const TGCTMDBOut::TileModuleHit hit6);

  void  eraseOutput();
  int   NumberOfTileModules() const { return NumberOfTileModule; }
  int   getInnerTileBits(int side, int sectorID) const;

  void print() const;

 protected:
  TGCTMDBOut* m_buffer[2*NumberOfTileModule];

};


}   // end of namespace

#endif

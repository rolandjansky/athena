/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ====================================================================
/*
        TGCTMDB.h

*/
// ====================================================================
#ifndef TGC_TMDB_H
#define TGC_TMDB_H

namespace LVL1TGCTrigger {

class TGCTMDBOut;

// ====================================================================
//
// class definition
//
// ====================================================================
 
class TGCTMDB{
public:
  TGCTMDB();
  virtual ~TGCTMDB();
 
  TGCTMDB(const TGCTMDB& right);
  TGCTMDB& operator=(const TGCTMDB& right);
  int operator==(const TGCTMDB& right) const
  { return (this==&right); }
  int operator!=(const TGCTMDB& right) const
  { return (this!=&right); }

  enum { NumberOfTileModule = 64 };
  
  // methods  
  const TGCTMDBOut* getOutput(int side, int moduleID) const;
  const TGCTMDBOut* getOutput(int side, int sectorID, int mod) const;
  void  setOutput(int side, int module, int hit56, int hit6); 
  void  eraseOutput();
  int   NumberOfTileModules() const { return NumberOfTileModule; }   
  int   getInnerTileBits(int side, int sectorID) const;

  void Print() const;
  
protected:
  TGCTMDBOut* m_buffer[2*NumberOfTileModule];

};


} //end of namespace bracket

#endif

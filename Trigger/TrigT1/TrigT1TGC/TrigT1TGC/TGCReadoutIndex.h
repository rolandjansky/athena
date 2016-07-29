/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ====================================================================
/*
        TGCReadoutIndex.h
                                      QCJP, 1999
*/
// ====================================================================
#ifndef TGC_READOUT_INDEX_H
#define TGC_READOUT_INDEX_H

#include "TrigT1TGC/TGCIndex.h"

namespace LVL1TGCTrigger {

// ====================================================================
//
// class definition
//
// ====================================================================

class TGCReadoutIndex {
protected:
  TGCZDirection zDirection;
  int octantNumber;
  int moduleNumber;
  int rNumber;
  int layerNumber;

public:
  TGCReadoutIndex();

  TGCReadoutIndex(TGCZDirection iz, int ioct, int imd, int ir, int ilyr);

  TGCReadoutIndex(TGCIndex tgcindex, int ilyr);

  ~TGCReadoutIndex() { }
 
  TGCReadoutIndex(const TGCReadoutIndex& right)
  {
    *this= right;
  }
 
  TGCReadoutIndex& operator=(const TGCReadoutIndex& right)
  {
    if (this != &right) {
      zDirection= right.zDirection;
      octantNumber= right.octantNumber;
      moduleNumber= right.moduleNumber;
      rNumber= right.rNumber;
      layerNumber= right.layerNumber;
    }
    return *this;
  }
 
  int operator==(const TGCReadoutIndex& right) const
  {
    return (this==&right);
  }
 
  int operator!=(const TGCReadoutIndex& right) const
  {
    return (this!=&right);
  }

  // set function 
  // conversion from TGC-index and layer#
  void SetIndex(TGCIndex tgcindex, int ilyr);

  // get functions
  TGCZDirection GetZDirection() const { return zDirection; }
  int GetSideNumber() const { if(zDirection==kZ_FORWARD) return 0;
                              if(zDirection==kZ_BACKWARD) return 1;
                              return -1;}
  int GetOctantNumber() const { return octantNumber; }
  int GetModuleNumber() const { return moduleNumber; }
  int GetRNumber() const { return rNumber; }
  int GetLayerNumber() const { return layerNumber; }

  // operations
  void Print() const;

};

} //end of namespace

#endif

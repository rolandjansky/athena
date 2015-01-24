/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ====================================================================
/*
        TGCIndex.h
	ver.1.0
	ver.2.0     remove phiNumber
                                      QCJP, 1999
*/
// ====================================================================
#ifndef TGC_INDEX_H
#define TGC_INDEX_H

#include "TrigT1TGC/TGCNumbering.hh"

namespace LVL1TGCTrigger {

// ====================================================================
//
// class definition
//
// ====================================================================

class TGCIndex {
protected:
  TGCZDirection zDirection;
  TGCStationType stationType;
  int octantNumber;
  TGCRegionType regionType;
  int moduleNumber;
  int rNumber;

public:
  TGCIndex();
  TGCIndex(TGCZDirection iz, TGCStationType ist, int ioct, 
	   TGCRegionType irgn, int imd, int ir);

  ~TGCIndex() { }
 
  TGCIndex(const TGCIndex& right)
  {
    *this= right;
  }
 
  TGCIndex& operator=(const TGCIndex& right)
  {
    if (this != &right) {
      zDirection= right.zDirection;
      stationType= right.stationType;
      octantNumber= right.octantNumber;
      regionType= right.regionType;
      moduleNumber= right.moduleNumber;
      rNumber= right.rNumber;
    }
    return *this;
  }
 
  int operator==(const TGCIndex& right) const
  {
    return (this==&right);
  }
 
  int operator!=(const TGCIndex& right) const
  {
    return (this!=&right);
  }

  // set function
  void SetIndex(TGCZDirection iz, TGCStationType ist, int ioct, 
		TGCRegionType irgn, int imd, int ir);

  // get functions
  TGCZDirection GetZDirection() const { return zDirection; }
  TGCStationType GetStationType() const { return stationType; }
  int GetOctantNumber() const { return octantNumber; }
  TGCRegionType GetRegionType() const { return regionType; }
  int GetModuleNumber() const { return moduleNumber; }
  int GetRNumber() const { return rNumber; }

  // operations
  void Print() const;

};


} //end of namespace bracket

#endif

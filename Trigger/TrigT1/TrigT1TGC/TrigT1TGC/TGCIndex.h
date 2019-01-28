/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
  TGCZDirection m_zDirection;
  TGCStationType m_stationType;
  int m_octantNumber;
  TGCRegionType m_regionType;
  int m_moduleNumber;
  int m_rNumber;

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
      m_zDirection= right.m_zDirection;
      m_stationType= right.m_stationType;
      m_octantNumber= right.m_octantNumber;
      m_regionType= right.m_regionType;
      m_moduleNumber= right.m_moduleNumber;
      m_rNumber= right.m_rNumber;
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
  TGCZDirection GetZDirection() const { return m_zDirection; }
  TGCStationType GetStationType() const { return m_stationType; }
  int GetOctantNumber() const { return m_octantNumber; }
  TGCRegionType GetRegionType() const { return m_regionType; }
  int GetModuleNumber() const { return m_moduleNumber; }
  int GetRNumber() const { return m_rNumber; }

  // operations
  void Print() const;

};


} //end of namespace bracket

#endif

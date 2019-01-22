/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
  TGCZDirection m_zDirection;
  int m_octantNumber;
  int m_moduleNumber;
  int m_rNumber;
  int m_layerNumber;

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
      m_zDirection= right.m_zDirection;
      m_octantNumber= right.m_octantNumber;
      m_moduleNumber= right.m_moduleNumber;
      m_rNumber= right.m_rNumber;
      m_layerNumber= right.m_layerNumber;
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
  TGCZDirection GetZDirection() const { return m_zDirection; }
  int GetSideNumber() const { if(m_zDirection==kZ_FORWARD) return 0;
                              if(m_zDirection==kZ_BACKWARD) return 1;
                              return -1;}
  int GetOctantNumber() const { return m_octantNumber; }
  int GetModuleNumber() const { return m_moduleNumber; }
  int GetRNumber() const { return m_rNumber; }
  int GetLayerNumber() const { return m_layerNumber; }

  // operations
  void Print() const;

};

} //end of namespace

#endif

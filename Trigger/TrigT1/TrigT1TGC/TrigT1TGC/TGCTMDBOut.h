/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1TGC_TGCTMDBOut_H
#define TrigT1TGC_TGCTMDBOut_H

namespace LVL1TGCTrigger {

class TGCTMDBOut
{
 public:
  enum TileModuleHit { TM_NOHIT=0, TM_LOW=1, TM_HIGH=2, TM_NA=3};

  TGCTMDBOut();
  TGCTMDBOut(const int side, const int module,
             const TileModuleHit hit56=TM_NOHIT,
             const TileModuleHit hit6=TM_NOHIT);
  virtual ~TGCTMDBOut() = default;

  TGCTMDBOut(const TGCTMDBOut& right) = default;
  TGCTMDBOut& operator = (const TGCTMDBOut& right) = default;

  int operator==(const TGCTMDBOut& right) const
  {
    return (this==&right);
  }

  int operator!=(const TGCTMDBOut& right) const
  {
    return (this!=&right);
  }

  // set functons
  void setSide(int side) { m_sideID = side; }
  void setModuleID(int module) { m_moduleID = module; }
  void setHit56(const TileModuleHit hit) { m_hit_56 = hit; }
  void setHit6(const TileModuleHit hit) { m_hit_6 = hit; }
  void clear() { m_hit_56 = TM_NOHIT; m_hit_6 = TM_NOHIT; }

  // get functions
  int getSide() const { return m_sideID; }
  int getModuleID() const { return m_moduleID; }
  TileModuleHit getHit56() const { return m_hit_56; }
  TileModuleHit getHit6() const { return m_hit_6; }

  // methods  
  void print() const;

 protected:
  int            m_sideID;     // 0:A-side 1:C-side
  int            m_moduleID;   // 0..63
  TileModuleHit  m_hit_56;     // 0: No Hit, 1: Low, 2: High, 3: NA
  TileModuleHit  m_hit_6;      // 0: No Hit, 1: Low, 2: High, 3: NA

};


}   // end of namespace

#endif

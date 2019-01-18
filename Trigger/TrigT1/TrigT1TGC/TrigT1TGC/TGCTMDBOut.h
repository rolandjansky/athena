/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ====================================================================
/*
        TGCTMDBOut.h

*/
// ====================================================================
#ifndef TGC_TMDBOUT_H
#define TGC_TMDBOUT_H

namespace LVL1TGCTrigger {

// ====================================================================
//
// class definition
//
// ====================================================================
 
class TGCTMDBOut {
protected:
  int            m_sideID;     // 0:A-side 1:C-side
  int            m_moduleID;   // 0..63
  int            m_hit_56;     // 0: No Hit, 1: Low 2: High 3: NA
  int            m_hit_6;      // 0: No Hit, 1: Low 2: High 3: NA

public:
  TGCTMDBOut();
  TGCTMDBOut(int side, int module, int hit56=0, int hit6=0); 

  virtual ~TGCTMDBOut() { }
 
  TGCTMDBOut(const TGCTMDBOut& right);
 
  TGCTMDBOut& operator=(const TGCTMDBOut& right);
 
  int operator==(const TGCTMDBOut& right) const
  {
    return (this==&right);
  }
 
  int operator!=(const TGCTMDBOut& right) const
  {
    return (this!=&right);
  }

  enum { TM_NOHIT=0, TM_LOW=1, TM_HIGH=2 };

  // set functons
  void SetSide(int side){ m_sideID = side; }
  void SetModuleID(int module){ m_moduleID = module; }
  void SetHit56(int hit){ m_hit_56 = hit; }
  void SetHit6(int hit){ m_hit_6 = hit; }
  void Clear() {  m_hit_56 =0; m_hit_6 = 0; }

  // get functions
  int GetSide() const { return m_sideID; }
  int GetModuleID() const { return m_moduleID; }
  int GetHit56() const { return m_hit_56; }
  int GetHit6() const { return m_hit_6; }

  // methods  
  void Print() const;

};


} //end of namespace bracket

#endif

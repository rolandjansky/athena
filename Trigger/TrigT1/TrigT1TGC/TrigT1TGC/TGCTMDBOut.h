/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  int            sideID;     // 0:A-side 1:C-side
  int            moduleID;   // 0..63
  int            hit_56;     // 0: No Hit, 1: Low 2: High 3: NA
  int            hit_6;      // 0: No Hit, 1: Low 2: High 3: NA

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
  void SetSide(int side){ sideID = side; }
  void SetModuleID(int module){ moduleID = module; }
  void SetHit56(int hit){ hit_56 = hit; }
  void SetHit6(int hit){ hit_6 = hit; }
  void Clear() {  hit_56 =0; hit_6 = 0; }

  // get functions
  int GetSide() const { return sideID; }
  int GetModuleID() const { return moduleID; }
  int GetHit56() const { return hit_56; }
  int GetHit6() const { return hit_6; }

  // methods  
  void Print() const;

};


} //end of namespace bracket

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================
// HEC/EMEC TB FrontHit class
//
// information stored:
//  - energy deposition in scintillators
//  - hit position
//  - scintillator ID
//  - mwpc IDs
//  - track ID 
//  - particle code in F1/F2 
//============================================

#ifndef LArG4H6FrontHit_h
#define LArG4H6FrontHit_h 1

#include "CLHEP/Vector/ThreeVector.h"
#include <vector>
class LArG4H6FrontHit 
{
 public:
     LArG4H6FrontHit();
     LArG4H6FrontHit(int sc, double ed);
     LArG4H6FrontHit(int mw, int x1, int y1);
     LArG4H6FrontHit(const LArG4H6FrontHit &right);
//     ~LArG4H6FrontHit(){;};

     LArG4H6FrontHit& operator=(const LArG4H6FrontHit &right);
     int operator==(const LArG4H6FrontHit &right);
//     inline void *operator new(size_t, void* aHit = 0);
//     inline void operator delete(void *aHit);

     void Draw(){;};
     void Print(){;};

// Set- Get- methods
 // energy deposit
     inline void SetEdep(double ed) { m_edep = ed; }
     inline double GetEdep() { return m_edep; }

 // position
     inline void SetPos(const CLHEP::Hep3Vector& p) { m_pos = p; }
     inline CLHEP::Hep3Vector GetPos() { return m_pos; }

 // scnum
     inline void SetSC(int sc) { m_scnum = sc; }
     inline int GetSC() { return m_scnum; }

 // mwpcnum , x, y
     inline void SetMWPC(int mw) { m_mwpcnum = mw; }
     inline int GetMWPC()  { return m_mwpcnum; }
     inline void SetX(int x1) { m_x = x1; }
     inline int GetX() { return m_x; }
     inline void SetY(int y1) { m_y = y1; }
     inline int GetY() { return m_y; }

 // trackID
     inline void SetTrackID(int id)  { m_track_id = id; }
     inline int GetTrackID()  { return m_track_id; }

 // particle code
     inline void SetPcode(int code) { m_pcode = code; }
     inline int GetPcode() { return m_pcode; }     

 private:
     double m_edep;                // deposited energy in G4 internal units
     CLHEP::Hep3Vector m_pos;            // position of the hit
     int m_scnum, m_mwpcnum, m_x, m_y;   // sci. number, mwpc number, x- or y- wire number
     int m_track_id;               // track id
     int m_pcode;                  // particle code
};

typedef std::vector<LArG4H6FrontHit> LArG4H6FrontHitsCollection;

// extern G4Allocator<LArG4H6FrontHit> LArG4H6FrontHitAllocator;

/*
inline void* LArG4H6FrontHit::operator new(size_t, void* aHit)
{
// void *aHit;
 aHit = (void *) LArG4H6FrontHitAllocator.MallocSingle();
 return aHit;
}

inline void LArG4H6FrontHit::operator delete(void* aHit)
{
 LArG4H6FrontHitAllocator.FreeSingle( (LArG4H6FrontHit*) aHit);
}
*/
#endif

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

#ifndef LArTBFrontHit_h
#define LArTBFrontHit_h 1
#include <vector>
#include "CLHEP/Vector/ThreeVector.h"

class LArTBFrontHit 
{
 public:
     LArTBFrontHit();
     LArTBFrontHit(int sc, double ed);
     LArTBFrontHit(int mw, int x1, int y1);
     LArTBFrontHit(const LArTBFrontHit &right);
//     ~LArTBFrontHit(){;};

     const LArTBFrontHit& operator=(const LArTBFrontHit &right);
     int operator==(const LArTBFrontHit &right);
//     inline void *operator new(size_t, void* aHit = 0);
//     inline void operator delete(void *aHit);

     void Draw(){;};
     void Print(){;};

// Set- Get- methods
 // energy deposit
     inline void SetEdep(double ed) { edep = ed; }
     inline double GetEdep() { return edep; }

 // position
     inline void SetPos(CLHEP::Hep3Vector p) { pos = p; }
     inline CLHEP::Hep3Vector GetPos() { return pos; }

 // scnum
     inline void SetSC(int sc) { scnum = sc; }
     inline int GetSC() { return scnum; }

 // mwpcnum , x, y
     inline void SetMWPC(int mw) { mwpcnum = mw; }
     inline int GetMWPC()  { return mwpcnum; }
     inline void SetX(int x1) { x = x1; }
     inline int GetX() { return x; }
     inline void SetY(int y1) { y = y1; }
     inline int GetY() { return y; }

 // trackID
     inline void SetTrackID(int id)  { track_id = id; }
     inline int GetTrackID()  { return track_id; }

 // particle code
     inline void SetPcode(int code) { pcode = code; }
     inline int GetPcode() { return pcode; }     

 private:
     double edep;                // deposited energy in  internal units
     CLHEP::Hep3Vector pos;            // position of the hit
     int scnum, mwpcnum, x, y;   // sci. number, mwpc number, x- or y- wire number
     int track_id;               // track id
     int pcode;                  // particle code
};

typedef std::vector<LArTBFrontHit> LArTBFrontHitsCollection;

// extern G4Allocator<LArTBFrontHit> LArTBFrontHitAllocator;

/*
inline void* LArTBFrontHit::operator new(size_t, void* aHit)
{
// void *aHit;
 aHit = (void *) LArTBFrontHitAllocator.MallocSingle();
 return aHit;
}

inline void LArTBFrontHit::operator delete(void* aHit)
{
 LArTBFrontHitAllocator.FreeSingle( (LArTBFrontHit*) aHit);
}
*/
#endif

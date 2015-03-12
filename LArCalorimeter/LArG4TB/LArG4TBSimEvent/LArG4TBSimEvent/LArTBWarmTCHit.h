/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//=================================================
// LArTBWarmTCHit class
//
// information stored
//=================================================

#ifndef LArTBWarmTCHit_h
#define LArTBWarmTCHit_h 1

#include "CLHEP/Vector/ThreeVector.h"
#include <vector>

class LArTBWarmTCHit 
{
 public:
     LArTBWarmTCHit(){;};
     LArTBWarmTCHit(int ad, double en){addr = ad; energy = en; };
     LArTBWarmTCHit(const LArTBWarmTCHit &right);
//     ~LArTBWarmTCHit(){;};

     const LArTBWarmTCHit& operator=(const LArTBWarmTCHit &right);
     int operator==(const LArTBWarmTCHit &right);
//     inline void *operator new(size_t, void* aHit = 0);
//     inline void operator delete(void *aHit);

     void Draw(){;};
     void Print(){;};

//Set- Get- methods
 // energy deposit
     inline void SetEnergy(double ed) { energy = ed; }
     inline void AddEnergy(double ed) { energy += ed; }
     inline double GetEnergy() { return energy; }

 // address
     inline void SetAddr(int d) { addr = d; }
     inline int GetAddr() { return addr; }

 private:
     int     addr;   // Address: 
     double  energy;
};

typedef std::vector<LArTBWarmTCHit> LArTBWarmTCHitsCollection;

// extern G4Allocator<LArTBWarmTCHit> LArTBWarmTCHitAllocator;

/*
inline void* LArTBWarmTCHit::operator new(size_t, void* aHit)
{
// void *aHit;
 aHit = (void*) LArTBWarmTCHitAllocator.MallocSingle();
 return aHit;
}

inline void LArTBWarmTCHit::operator delete(void *aHit)
{
 LArTBWarmTCHitAllocator.FreeSingle( (LArTBWarmTCHit*) aHit);
}
*/
#endif

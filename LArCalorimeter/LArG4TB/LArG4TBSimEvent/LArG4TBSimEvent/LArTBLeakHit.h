/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//=================================================
// LArTBLeakHit class
//
// information stored
//  - energy deposition in the leakege detector
//  - hit position
//  - store the particle types, kinetic and total energies
//  - leakage direction
//=================================================

#ifndef LArTBLeakHit_h
#define LArTBLeakHit_h 1
#include "CLHEP/Vector/ThreeVector.h"
#include <vector>
class LArTBLeakHit 
{
 public:
     LArTBLeakHit(){;};
     LArTBLeakHit(int d){m_dir = d;};
     LArTBLeakHit(int dir, int pcode, float ekin, float etot);
     LArTBLeakHit(const LArTBLeakHit &right);
//     ~LArTBLeakHit(){;};

     const LArTBLeakHit& operator=(const LArTBLeakHit &right);
     int operator==(const LArTBLeakHit &right);
//     inline void *operator new(size_t, void* aHit = 0);
//     inline void operator delete(void *aHit);

     void Draw(){;};
     void Print(){;};

//Set- Get- methods
 // energy deposit
     inline void SetEkin(double ed) { m_ekin = ed; }
     inline void AddEkin(double ed) { m_ekin += ed; }
     inline double GetEkin() { return m_ekin; }
     inline void SetEtot(double ed) { m_etot = ed; }
     inline void AddEtot(double ed) { m_etot += ed; }
     inline double GetEtot() { return m_etot; }

 // particle code
     inline void SetCode(int cod) { m_pcode = cod; }
     inline int GetCode() { return m_pcode; }

 // position
     inline void SetPos(const CLHEP::Hep3Vector& p) { m_pos = p; }
     inline CLHEP::Hep3Vector GetPos() { return m_pos; }

 // leakage direction
     inline void SetDir(int d) { m_dir = d; }
     inline int GetDir() { return m_dir; }

 private:
     double m_ekin;   // kinetic energy of stopped particle
     double m_etot;   // total energy       -"-
     int    m_pcode;  // particle code, special codes for: deuterium 9001
                        //                                   tritium   9002
                        //                                   alpha     9003
                        //                                   He3       9004
     CLHEP::Hep3Vector m_pos; // absolute position of hit
     int    m_dir;     // leakage direction encoding
                        // 1,2     EMEC     x<0, x>0
                        // 3     EMEC down
                        // 4,5   EMEC back    -"-
                        // 6,7   HEC1         -"-
                        // 8     HEC1 down
                        // 9,10  HEC1 back    -"-
                        // 11,12  HEC2        -"-
                        // 13    HEC2 down
                        // 14    HEC2 back
};

typedef std::vector<LArTBLeakHit> LArTBLeakHitsCollection;

// extern G4Allocator<LArTBLeakHit> LArTBLeakHitAllocator;

/*
inline void* LArTBLeakHit::operator new(size_t, void* aHit)
{
// void *aHit;
 aHit = (void*) LArTBLeakHitAllocator.MallocSingle();
 return aHit;
}

inline void LArTBLeakHit::operator delete(void *aHit)
{
 LArTBLeakHitAllocator.FreeSingle( (LArTBLeakHit*) aHit);
}
*/
#endif

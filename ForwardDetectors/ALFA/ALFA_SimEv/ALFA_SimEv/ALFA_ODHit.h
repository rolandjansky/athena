/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_ODHit_h
#define ALFA_ODHit_h

// #include "GeneratorObjects/HepMcParticleLink.h"

class ALFA_ODHit
{
 public:
  ALFA_ODHit(int, int, int, float, float, float, float,
		     float, float, float, float, float,
		     int, int, int, int, int
		     );
  
  // needed by athenaRoot
  ALFA_ODHit();
  
  // Destructor: FIXME POOL 
  virtual ~ALFA_ODHit();
  
  bool operator < (const ALFA_ODHit& obj) const
    {return hitID < obj.hitID;}
  
  int GetHitID() const
    {return hitID;}
  
  int GetTrackID() const;

  // link to the particle generating the hit
//  const HepMcParticleLink& particleLink() const;
  

  int GetParticleEncoding() const
    {return particleEncoding;}
  
  float GetKineticEnergy() const
    {return kineticEnergy;}
  
  float GetEnergyDeposit() const
    {return energyDeposit;}
  
  float GetPreStepX() const
    {return preStepX;}
  
  float GetPreStepY() const
    {return preStepY;}
  
  float GetPreStepZ() const
    {return preStepZ;}
  
  float GetPostStepX() const
    {return postStepX;}
  
  float GetPostStepY() const
    {return postStepY;}
  
  float GetPostStepZ() const
    {return postStepZ;}
  
  float GetGlobalTime() const
    {return globalTime;}
  
  int GetSignFiber() const
    {return sign_fiber;}
    
  int GetODSide() const
    {return OD_side;}    
        
  int GetPlateNumber() const
    {return n_plate;}    

  int GetFiberNumber () const
    {return n_fiber;}

  int GetStationNumber () const
    {return n_station;}    
          
  
 private:
  int hitID; // To identify the hit
  int trackID;
//  HepMcParticleLink m_partLink; // link to the particle generating the hit
  int particleEncoding;         // PDG id 
  float kineticEnergy;          // kin energy of the particle
  float energyDeposit;          // energy deposit by the hit
  float preStepX;
  float preStepY;
  float preStepZ;
  float postStepX;
  float postStepY;
  float postStepZ;
  float globalTime;
  
  int sign_fiber;
  int OD_side;
  int n_plate;
  int n_fiber;
  int n_station;
};


// inline const HepMcParticleLink& ALFA_Hit::particleLink() const
// {
//   return m_partLink;
// }


inline float hitTime(const ALFA_ODHit& hit)
{
  return (float) hit.GetGlobalTime();
}
     
#endif
     

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
    {return m_hitID < obj.m_hitID;}
  
  int GetHitID() const
    {return m_hitID;}
  
  int GetTrackID() const;

  // link to the particle generating the hit
//  const HepMcParticleLink& particleLink() const;
  

  int GetParticleEncoding() const
    {return m_particleEncoding;}
  
  float GetKineticEnergy() const
    {return m_kineticEnergy;}
  
  float GetEnergyDeposit() const
    {return m_energyDeposit;}
  
  float GetPreStepX() const
    {return m_preStepX;}
  
  float GetPreStepY() const
    {return m_preStepY;}
  
  float GetPreStepZ() const
    {return m_preStepZ;}
  
  float GetPostStepX() const
    {return m_postStepX;}
  
  float GetPostStepY() const
    {return m_postStepY;}
  
  float GetPostStepZ() const
    {return m_postStepZ;}
  
  float GetGlobalTime() const
    {return m_globalTime;}
  
  int GetSignFiber() const
    {return m_sign_fiber;}
    
  int GetODSide() const
    {return m_OD_side;}    
        
  int GetPlateNumber() const
    {return m_n_plate;}    

  int GetFiberNumber () const
    {return m_n_fiber;}

  int GetStationNumber () const
    {return m_n_station;}    
          
  
 private:
  int m_hitID; // To identify the hit
  //int trackID;
//  HepMcParticleLink m_partLink; // link to the particle generating the hit
  int m_particleEncoding;         // PDG id 
  float m_kineticEnergy;          // kin energy of the particle
  float m_energyDeposit;          // energy deposit by the hit
  float m_preStepX;
  float m_preStepY;
  float m_preStepZ;
  float m_postStepX;
  float m_postStepY;
  float m_postStepZ;
  float m_globalTime;
  
  int m_sign_fiber;
  int m_OD_side;
  int m_n_plate;
  int m_n_fiber;
  int m_n_station;
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
     

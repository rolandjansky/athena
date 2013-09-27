/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTUncompressedHit_hh
#define TRTUncompressedHit_hh

#include "GeneratorObjects/HepMcParticleLink.h"

class TRTUncompressedHit
{
 public:
  TRTUncompressedHit(int, int, int, float, float, float, float,
		     float, float, float, float, float);
  
  // needed by athenaRoot
  TRTUncompressedHit();
  
  // Destructor: FIXME POOL
  virtual ~TRTUncompressedHit();
  
  bool operator < (const TRTUncompressedHit& obj) const
    {return hitID < obj.hitID;}
  
  int GetHitID() const
    {return hitID;}
  
  int GetTrackID() const;

  // link to the particle generating the hit
  const HepMcParticleLink& particleLink() const;
  

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
  
  void SetGlobalTime(float newGlobalTime) 
    {globalTime=newGlobalTime;}

 private:
  int hitID; // To identify the hit
  //  int trackID;
  HepMcParticleLink m_partLink; // link to the particle generating the hit
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
};


///////////////////////////////////////////////////////////////////
// open functions:
///////////////////////////////////////////////////////////////////

inline const HepMcParticleLink& TRTUncompressedHit::particleLink() const
{
  return m_partLink;
}


inline float hitTime(const TRTUncompressedHit& hit)
{
  return (float) hit.GetGlobalTime();
}
     
#endif
     

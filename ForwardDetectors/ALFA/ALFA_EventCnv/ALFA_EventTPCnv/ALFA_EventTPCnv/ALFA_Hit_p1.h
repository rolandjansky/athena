/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_Hit_p1_h
#define ALFA_Hit_p1_h

//#include "GeneratorObjectsTPCnv/HepMcParticleLink_p1.h"


class ALFA_Hit_p1
{
 public:
  // needed by athenaRoot 
  ALFA_Hit_p1(){};
  friend class ALFA_HitCnv_p1;
  
 private:
  int hitID; // To identify the hit
  int trackID;
//  HepMcParticleLink_p1 m_partLink; // link to the particle generating the hit
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
  int n_plate;
  int n_fiber;
  int n_station;
};

     
#endif
     

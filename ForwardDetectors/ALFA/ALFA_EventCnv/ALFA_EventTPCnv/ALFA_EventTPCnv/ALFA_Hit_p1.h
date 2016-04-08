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
  ALFA_Hit_p1()
  {
	hitID=-1;
	trackID=-1;
	particleEncoding=-1;
	kineticEnergy=-9999.0;
	energyDeposit=-9999.0;
	preStepX=-9999.0;
	preStepY=-9999.0;
	preStepZ=-9999.0;
	postStepX=-9999.0;
	postStepY=-9999.0;
	postStepZ=-9999.0;
	globalTime=-9999.0;
	sign_fiber=-1;
	n_plate=-1;
	n_fiber=-1;
	n_station=-1;
  };
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
     

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "ALFA_SimEv/ALFA_ODHit.h"


  // Called by ALFA_SensitiveDetector::ProcessHits

ALFA_ODHit::ALFA_ODHit(int hit, 
			int track, 
			int particle,
			float kinEnergy, 
			float eneDeposit, 
			float preX, float preY, float preZ, 
			float postX, float postY, float postZ, 
			float time,
			int sign, int side, int plate, int fiber, int station
			):
  hitID(hit),
  trackID(track), 
//  m_partLink(track), 
  particleEncoding(particle),
  kineticEnergy(kinEnergy), 
  energyDeposit(eneDeposit), 
  preStepX(preX), preStepY(preY), preStepZ(preZ), 
  postStepX(postX), postStepY(postY), postStepZ(postZ), 
  globalTime(time),
  sign_fiber(sign), OD_side(side),n_plate(plate), n_fiber(fiber), n_station(station) 
  {} 

// Default constructor needed by athenaroot/athenapool
//
ALFA_ODHit::ALFA_ODHit( ) :
  hitID(0xffff),
  trackID(0),  
  particleEncoding(0),
  kineticEnergy(0.), 
  energyDeposit(0.), 
  preStepX(0.), preStepY(0.), preStepZ(0.), 
  postStepX(0.), postStepY(0.), postStepZ(0.), 
  globalTime(0.),
  sign_fiber(0), OD_side(0), n_plate(0), n_fiber(0), n_station(0)  
{}

// destructor
ALFA_ODHit::~ALFA_ODHit() {}

// barcode
int ALFA_ODHit::GetTrackID() const 
{
  return 22;  
// return m_partLink.barcode();
}

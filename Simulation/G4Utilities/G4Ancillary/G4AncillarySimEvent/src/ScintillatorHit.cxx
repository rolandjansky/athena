/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ScintillatorHit.h
//   Implementation file for class ScintillatorHit
//
// Author Thomas Kittelmann <kittel@nbi.dk>
///////////////////////////////////////////////////////////////////
#include "G4AncillarySimEvent/ScintillatorHit.h"

// Default constructor needed by athenaroot
ScintillatorHit::ScintillatorHit() : 
  m_volNumber(-999),
  m_particleEncoding(0),
  m_kineticEnergy(0),
  m_energyDeposit(0),
  m_preStepX(0),
  m_preStepY(0),
  m_preStepZ(0),
  m_postStepX(0),
  m_postStepY(0),
  m_postStepZ(0),
  m_globalPreStepX(0),
  m_globalPreStepY(0),
  m_globalPreStepZ(0),
  m_globalPostStepX(0),
  m_globalPostStepY(0),
  m_globalPostStepZ(0),
  m_globalTime(0),
  m_partLink() {}

ScintillatorHit::~ScintillatorHit() {}

// Constructor
ScintillatorHit::ScintillatorHit( const int volNumber,
				  const int particleEncoding,
				  const float kineticEnergy,
				  const float energyDeposit,
				  const float preStepX,
				  const float preStepY,
				  const float preStepZ,
				  const float postStepX,
				  const float postStepY,
				  const float postStepZ,
				  const float globalPreStepX,
				  const float globalPreStepY,
				  const float globalPreStepZ,
				  const float globalPostStepX,
				  const float globalPostStepY,
				  const float globalPostStepZ,
				  const float globalTime,
				  const int track) : 
  m_volNumber(volNumber),
  m_particleEncoding(particleEncoding),
  m_kineticEnergy(kineticEnergy),
  m_energyDeposit(energyDeposit),
  m_preStepX(preStepX),
  m_preStepY(preStepY),
  m_preStepZ(preStepZ),
  m_postStepX(postStepX),
  m_postStepY(postStepY),
  m_postStepZ(postStepZ),
  m_globalPreStepX(globalPreStepX),
  m_globalPreStepY(globalPreStepY),
  m_globalPreStepZ(globalPreStepZ),
  m_globalPostStepX(globalPostStepX),
  m_globalPostStepY(globalPostStepY),
  m_globalPostStepZ(globalPostStepZ),
  m_globalTime(globalTime),
  m_partLink(track) {}
	
int ScintillatorHit::GetTrackID() const {
  return m_partLink.barcode();
}

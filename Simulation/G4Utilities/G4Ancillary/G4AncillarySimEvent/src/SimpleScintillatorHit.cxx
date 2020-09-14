/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SimpleScintillatorHit.h
//   Implementation file for class SimpleScintillatorHit
//
// Author Andrea Di Simone. Based on ScintillatorHit by homas Kittelmann <kittel@nbi.dk>
///////////////////////////////////////////////////////////////////
#include "G4AncillarySimEvent/SimpleScintillatorHit.h"

// Default constructor needed by athenaroot
SimpleScintillatorHit::SimpleScintillatorHit() :
  m_particleEncoding(0),
  m_kineticEnergy(0),
  m_energyDeposit(0),
  m_position(0),
  m_globalTime(0),
  m_copyNo(0),
  m_partLink() {}

SimpleScintillatorHit::~SimpleScintillatorHit() {}

// Constructor
SimpleScintillatorHit::SimpleScintillatorHit(const int particleEncoding,
				  const float kineticEnergy,
				  const float energyDeposit,
				  const CLHEP::Hep3Vector& position,
				  const float globalTime,
                                  const int copyNo,
				  const int track) :
  m_particleEncoding(particleEncoding),
  m_kineticEnergy(kineticEnergy),
  m_energyDeposit(energyDeposit),
  m_position(position),
  m_globalTime(globalTime),
  m_copyNo(copyNo),
  m_partLink(track) {}

int SimpleScintillatorHit::GetTrackID() const {
  return m_partLink.barcode();
}

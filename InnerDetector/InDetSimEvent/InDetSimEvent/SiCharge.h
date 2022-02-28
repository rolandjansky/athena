/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiCharge.h
//   Header file for class SiCharge
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class which contains the bare charge deposited by a single process
///////////////////////////////////////////////////////////////////
// Version 1.4 08/06/2001 David Calvet
///////////////////////////////////////////////////////////////////

#ifndef SITRACKEREVENT_SICHARGE_H
#define SITRACKEREVENT_SICHARGE_H

#include "InDetSimEvent/SiTrackDistance.h"

#include <iostream>

// Member classes
#include "GeneratorObjects/HepMcParticleLink.h"


class SiCharge {

public:
  enum Process {no,track,diodeX_Talk,cellX_Talk,noise,extraNoise,cut_track};

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Copy constructor:
  SiCharge(const SiCharge &charge);

  // Constructor with parameters:
  //   deposited charge
  //   time of deposition
  //   type of process which produced this charge
  //   Particle Link to the particle generating the Charge
  SiCharge(const double& charge,const double& time,
	   const Process& processType,const HepMcParticleLink& PL);

  SiCharge(const double& charge,const double& time,
	   const Process& processType);

  SiCharge(const double& charge,
           const double& time,
           const Process& processType,
           const HepMcParticleLink& PL,
           const SiTrackDistance& trackDistance);

  // Destructor:
  ~SiCharge();

  // Assignment operator:
  SiCharge &operator=(const SiCharge &charge);

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  // deposited charge:
  double charge() const;

  // time of deposition:
  double time() const;

  // type of process which produced this charge:
  Process processType() const;

  // Barcode of the particle generating the charge:
  int trackBarcode() const;

  // Particle Link of the particle generating the charge
  const HepMcParticleLink& particleLink() const;

  // Get the SiTrackDistance parameters
  const SiTrackDistance& trackDistance() const;

  ///////////////////////////////////////////////////////////////////
  // Non-const methods:
  ///////////////////////////////////////////////////////////////////

  // add another charge, if the process and track are the same:
  //   returns true if the charge was added
  bool add(const SiCharge &charge);

  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
private:

  SiCharge();

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:
  double m_charge; // deposited charge
  double m_time; // time of deposition
  Process m_processType; // type of process which produced this charge
  //  int m_trackNumber; // track number in case of track process
  HepMcParticleLink m_partLink; //Replace the track number with a PL
  SiTrackDistance m_trackDistance;
};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline SiCharge::~SiCharge() 
{}

inline double SiCharge::charge() const 
{
  return m_charge;
}

inline double SiCharge::time() const 
{
  return m_time;
}

inline SiCharge::Process SiCharge::processType() const 
{
  return m_processType;
}

inline int SiCharge::trackBarcode() const 
{
  return m_partLink.barcode();
}

inline const HepMcParticleLink& SiCharge::particleLink() const 
{
  return m_partLink;
}

inline const SiTrackDistance& SiCharge::trackDistance() const
{
  return m_trackDistance;
}

///////////////////////////////////////////////////////////////////
// Input/Output stream functions:
///////////////////////////////////////////////////////////////////
std::ostream &operator<<(std::ostream &out,const SiCharge &charge);

#endif // SITRACKEREVENT_SICHARGE_H

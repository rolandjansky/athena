/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiCharge.cxx
//   Implementation file for class SiCharge
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.4 08/06/2001 David Calvet
///////////////////////////////////////////////////////////////////

#include "InDetSimEvent/SiCharge.h"

// Copy constructor:
SiCharge::SiCharge(const SiCharge &charge) :
  m_charge(charge.m_charge),
  m_time(charge.m_time),
  m_processType(charge.m_processType),
  m_partLink(charge.m_partLink)
{}

// Constructor with parameters:
SiCharge::SiCharge(const double& charge,const double& time,
		   const Process& processType,const HepMcParticleLink& PL) :
  m_charge(charge),
  m_time(time),
  m_processType(processType),
  m_partLink(PL)
{}

SiCharge::SiCharge(const double& charge,const double& time,
		   const Process& processType) :
  m_charge(charge),
  m_time(time),
  m_processType(processType),
  m_partLink(HepMcParticleLink())
{}


// Assignment operator:
SiCharge &SiCharge::operator=(const SiCharge &charge)
{
  if (this!=&charge) {
    m_charge=charge.m_charge;
    m_time=charge.m_time;
    m_processType=charge.m_processType;
    m_partLink=charge.m_partLink;
  } else {}
  return *this;
}

// add another charge, if the process and track are the same:
bool SiCharge::add(const SiCharge &charge)
{
  // check if the two charges are compatible
  if (charge.m_processType!=m_processType || 
      charge.m_partLink!=m_partLink ||
      charge.m_time!=m_time) {
    return false;
  } else {
    m_charge+=charge.m_charge;
    return true;
  }
}

///////////////////////////////////////////////////////////////////
// Input/Output stream functions:
///////////////////////////////////////////////////////////////////
std::ostream &operator<<(std::ostream &out,const SiCharge &charge)
{
  out << "Charge=" << charge.charge() << " Time=" << charge.time()
      << " Process=";
  if (charge.processType()==SiCharge::no) out << "no";
  else if (charge.processType()==SiCharge::track) out << "track";
  else if (charge.processType()==SiCharge::diodeX_Talk) out << "diodeX_Talk";
  else if (charge.processType()==SiCharge::cellX_Talk) out << "cellX_Talk";
  else if (charge.processType()==SiCharge::noise) out << "noise";
  else if (charge.processType()==SiCharge::extraNoise) out << "extraNoise";
  else if (charge.processType()==SiCharge::cut_track) out << "cut_track";
  else out << "UNKNOWN !";
  return (out << " Barcode=" << charge.trackBarcode());
}


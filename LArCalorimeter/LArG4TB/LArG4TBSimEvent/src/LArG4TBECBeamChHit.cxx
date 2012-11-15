/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4TBECBeamChHit

#include "LArG4TBSimEvent/LArG4TBECBeamChHit.h"
#include <iostream>

// Default constructor.  Prevent undefined values.
LArG4TBECBeamChHit::LArG4TBECBeamChHit()
  : m_Chamber(0)
  , m_X(0.)
  , m_Y(0.)
{}

LArG4TBECBeamChHit::LArG4TBECBeamChHit(int chamber, double x, double y)
  : m_Chamber(chamber)
  , m_X(x)
  , m_Y(y)
{;}

LArG4TBECBeamChHit::~LArG4TBECBeamChHit() {;}

void LArG4TBECBeamChHit::Draw() {;}

void LArG4TBECBeamChHit::Print()
{
  std::cout << "LArG4TBECBeamChHit: ";
	std::cout << "chamber " << m_Chamber;
  std::cout << " at ("
	 << m_X << ","
	 << m_Y << ")" << std::endl;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SimulationHitCollection_p1_h
#define SimulationHitCollection_p1_h

#include "ForwardRegion_EventTPCnv/SimulationHit_p1.h"

#include <vector>
#include <string>

class SimulationHitCollection_p1
{
public:
	typedef std::vector<SimulationHit_p1> HitVector;
	typedef HitVector::const_iterator const_iterator;
	typedef HitVector::iterator       iterator;

	SimulationHitCollection_p1 () {}
	//friend class SimulationHitCollectionCnv_p1;

	// Accessors
	const std::string&  name() const { return m_name; }
	const HitVector&    getVector() const {return m_cont;}

private:
	std::string m_name;
	std::vector<SimulationHit_p1> m_cont;
};

#endif 

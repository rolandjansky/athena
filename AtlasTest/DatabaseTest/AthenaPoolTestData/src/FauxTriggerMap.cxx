/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file FauxTriggerMap.cxx
 * @brief Implementation of class FauxTriggerMap
 */
#include <string>
#include <set>
#include <sstream>
#include "AthenaPoolTestData/TrigPath.h"
#include "AthenaPoolTestData/FauxTriggerMap.h"

FauxTriggerMap::FauxTriggerMap() : m_bits(0) 
{
	int i=0;
	while (i<64) {
		m_l1results[i]=false;
		i++;
	}
}

FauxTriggerMap::~FauxTriggerMap()
{}

bool FauxTriggerMap::addPath(const TrigPath& tp) 
{	
	// Check is in range
	if ( tp.isValid() ) 
	{
		m_l1results[tp.l1bit] = true;
		m_bits = m_bits|(1<<tp.l1bit); // add bit to mask
		return true;
	} 
	else 
	{
		return false;
	}
}

bool FauxTriggerMap::hasL1(int l1bit) const
{
	return m_l1results[l1bit];
}

FauxTriggerMap::NUMBER_TYPE FauxTriggerMap::bits() const {return m_bits;}

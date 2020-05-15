/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDDigiCollection_p1_h
#define AFP_TDDigiCollection_p1_h


#include "AFP_EventTPCnv/AFP_TDDigi_p1.h"

#include <vector>
#include <string>

class AFP_TDDigiCollection_p1
{
public:
	typedef std::vector<AFP_TDDigi_p1> HitVector;
	typedef HitVector::const_iterator const_iterator;
	typedef HitVector::iterator       iterator;

	AFP_TDDigiCollection_p1 () {}

	// Accessors
	const std::string&  name() const { return m_name; }
	const HitVector&    getVector() const {return m_cont;}

	std::string m_name;
	std::vector<AFP_TDDigi_p1> m_cont;
};

#endif // AFP_TDDigiCollection_p1_h

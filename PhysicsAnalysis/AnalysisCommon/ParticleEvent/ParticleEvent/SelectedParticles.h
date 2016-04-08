// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLEEVENT_SELECTEDPARTICLES_H
#define PARTICLEEVENT_SELECTEDPARTICLES_H 1

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// SelectedParticles                                                         //
//                                                                      // 
// Store on/off bits for selected particles
// S. Protopopescu 9 Sep. 2008
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//////   Creation //////////////////////////////////////////////////////////////
//
//  SelectedBranches is a container mirroring a parent branch with particles
//  there should be only one instance in a branch, one can have multiple branches per parent branch
//
//  For each particle i a bit is stored indicating whether selected or rejected)
//  User can supply own vector of unsigned integers with bits set (1=selected, 0=rejected)
//  or create an empty instance with default constructor and  set bits with
//  SetBit(i, true or false)
//
//  If less than 32 particles then it is sufficient to call SetBit(i) for selected particles only
//
//  If more than 32 particles one should either call SetBit(i, true or false) for every particle 
//  or  call SetMaxBits(num of particles) and SetBit(i) for selected particles only
//
//  Note that once a bit is set to true there is no mechanism for resetting it to false.
//
//
///////   Useage ///////////////////////////////////////////////////
//
//  isGood(i) will return true if ith particle was selected
//
//  goodParticles(std::vector<unsigned>& goodparts ) will return list of selected particle numbers
//
//  displayBits() will print out the stored bits
//  Note that because SelectedParticles stores a vector of unsigned integers it does not know the
//  number of particles in parent branch
//  Selected Particles is useless if parent branch is discarded or trimmed
//
////////////////////////////////////////////////////////////////////
#include <vector>
#include<string>
#include "SGTools/CLASS_DEF.h"
#include "ParticleEvent/ParticleLinks.h"

class SelectedParticles {

 public:

    SelectedParticles():m_bits(){;}
	SelectedParticles(const std::vector<unsigned>& indices); 
	void Set(const std::vector<unsigned>& onoffBits){ m_bits=onoffBits;}
	// supply max number of bits
	void SetMaxBits(unsigned maxbits);
	// set specific bit
	void SetBit(unsigned i, bool good=true);
	~SelectedParticles(){;}
 
	// display the  bits
	std::string displayBits() const;
	// find if ith particle was selected
	bool isGood(unsigned i) const;
	// how many good particles
	unsigned numGood() const;
	// list of good particles
	void goodParticles(std::vector<unsigned>& goodparts ) const;
	// return all bits
	const std::vector<unsigned>& AllBits() const {return m_bits;}
 private:
  std::vector<unsigned> m_bits;

};
 CLASS_DEF(SelectedParticles, 162859305, 1 )


#endif // !PARTICLEEVENT_SELECTEDPARTICLES_H


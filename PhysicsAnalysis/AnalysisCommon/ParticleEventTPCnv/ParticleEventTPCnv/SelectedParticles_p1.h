////////////////////////////////////////////////////////////////// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SelectedParticles_p1.h 
// Header file for class SelectedParticles_p1
// persistent form of SelectedParticles (contents identical to SelectedParticles)
//
// Author: S. Protopopescu <serban@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef SELECTEDPARTICLES_P1_H
#define SELECTEDPARTICLES_P1_H 1
 
class SelectedParticles_p1 {
public:

    SelectedParticles_p1():m_bits() {}
   std::vector<unsigned> m_bits;
};

#endif

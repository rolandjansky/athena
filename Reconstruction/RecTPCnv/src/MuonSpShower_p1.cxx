///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// MuonSpShower_p1.cxx 
// Implementation file for class MuonSpShower_p1
// Author: Edward.Moyse@cern.ch
/////////////////////////////////////////////////////////////////// 


// RecTPCnv includes
#include "RecTPCnv/MuonSpShower_p1.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

MuonSpShower_p1::MuonSpShower_p1()
  : m_eta(0) 
  , m_phi(0) 
  , m_numberOfTriggerHits(0) 
  , m_numberOfInnerHits(0) 
  , m_numberOfInnerSegments(0) 
  , m_numberOfMiddleHits(0) 
  , m_numberOfMiddleSegments(0) 
  , m_numberOfOuterHits(0) 
  , m_numberOfOuterSegments(0) 
{}


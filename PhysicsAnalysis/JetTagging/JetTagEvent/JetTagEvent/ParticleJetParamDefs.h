/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGEVENT_PARTICLEJETPARAMDEFS_H
#define JETTAGEVENT_PARTICLEJETPARAMDEFS_H
/*****************************************************************************
Name    : ParticleJetParamDefs.h
Package : offline/PhysicsAnalysis/JetTagging/JetTagEvent
Author  : A. Wildauer
Created : March 2006

DESCRIPTION:

This file defines enums for ParticleJet parameters

*****************************************************************************/

namespace ParticleJetParams { 

  // do not change the order of these enums.  You can add one, but
  // changing the order is bad.
  enum Author {unknown=0, cone4=1, cone7=2, kt=3 };
}

#endif // JETTAGEVENT_PARTICLEJETPARAMDEFS_H

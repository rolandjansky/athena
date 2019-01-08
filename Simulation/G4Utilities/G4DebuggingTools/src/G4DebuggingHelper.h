/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4DEBUGGINGHELPER_H
#define G4DEBUGGINGTOOLS_G4DEBUGGINGHELPER_H

#include "G4String.hh"
#include "G4ParticleDefinition.hh"

#include <map>

namespace G4DebuggingHelpers {

  const G4String ClassifyParticle( const G4ParticleDefinition* def );

  const G4String ClassifyMaterial( const G4String &nom );

  const G4String ClassifyVolume( const G4String &nom );

}

#endif

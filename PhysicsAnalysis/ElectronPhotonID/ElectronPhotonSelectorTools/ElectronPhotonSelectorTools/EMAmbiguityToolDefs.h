/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMAMBIGUITYTOOLDEFS_H
#define EMAMBIGUITYTOOLDEFS_H

namespace EMAmbiguityType {
  enum AmbiguityResult {
    UNDEFINED = -999,
    ELECTRON = 0, // note that electron is False, possible photons are True
    LOOSE,  // loose heuristics for ambiguity
    TRT,    // has a TRT-only track, fails standard and loose ambiguity (might want to make single-track conv)
    LOOSE_TRT, // has a TRT-only track, fails standard but passes loose ambiguity
    PHOTON_ALT, // passes alternate criteria (like no or bad track particle)
    LOOSE_PHOTON_ALT, // passes alternate criteria (like no or bad track particle) and loose criteria
    PHOTON_AMBIGUITY, // passes standard conversion vx ambiguity criteria 
    MEDIUM // passes sligtly loosened ambibuity criteria for two-track conversions
  };
}

#endif // EMAMBIGUITYTOOLDEFS_H 

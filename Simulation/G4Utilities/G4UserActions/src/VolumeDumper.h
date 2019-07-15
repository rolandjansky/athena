/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef G4USERACTIONS_VOLUMEDUMPER_H
#define G4USERACTIONS_VOLUMEDUMPER_H

#include "G4UserEventAction.hh"
#include "G4Event.hh"

#include "AthenaBaseComps/AthMessaging.h"


namespace G4UA
{

class VolumeDumper final : public G4UserEventAction,
                           public AthMessaging
{

public:

VolumeDumper();
virtual void BeginOfEventAction( const G4Event* ) override;

}; // class VolumeDumper

} // namespace G4UA

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES__G4UA_IBEGINRUNACTION_H
#define G4ATLASINTERFACES__G4UA_IBEGINRUNACTION_H

// Forward declarations
class G4Run;

namespace G4UA
{

  /// Simple interface for an ATLAS begin-of-run action
  class IBeginRunAction
  {
    public:
      /// Virtual destructor
      virtual ~IBeginRunAction(){};
      /// Method called before simulating a G4 run
      virtual void beginOfRun(const G4Run*) = 0;
  };

} // namespace G4UA

#endif

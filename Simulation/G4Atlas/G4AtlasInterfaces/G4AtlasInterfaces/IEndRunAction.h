/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES__G4UA_IENDRUNACTION_H
#define G4ATLASINTERFACES__G4UA_IENDRUNACTION_H

// Forward declarations
class G4Run;

namespace G4UA
{

  /// Simple interface for an ATLAS end-of-run action
  class IEndRunAction
  {
    public:
      /// Virtual destructor
      virtual ~IEndRunAction(){};
      /// Method called after simulating a G4 run
      virtual void endOfRun(const G4Run*) = 0;
  };

} // namespace G4UA

#endif

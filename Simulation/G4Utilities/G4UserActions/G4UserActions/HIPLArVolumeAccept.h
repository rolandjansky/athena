/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_HIPLArVolumeAccept_H
#define G4UserActions_HIPLArVolumeAccept_H

#include "G4AtlasInterfaces/ISteppingAction.h"
#include "G4AtlasInterfaces/IBeginEventAction.h"
#include "G4AtlasInterfaces/IEndEventAction.h"
#include "G4AtlasInterfaces/IBeginRunAction.h"
#include "G4AtlasInterfaces/IEndRunAction.h"
#include "AthenaBaseComps/AthMessaging.h"

#include "GaudiKernel/ServiceHandle.h"

namespace G4UA
{

  /// @brief NEEDS DOCUMENTATION
  class HIPLArVolumeAccept : public AthMessaging, public ISteppingAction,
                             public IBeginEventAction, public IEndEventAction
  {
    public:
      HIPLArVolumeAccept();

      /// Report class for merging per-thread results.
      struct Report
      {
        int HIPevts=0;
        int HIPevts_failed=0;
        void merge(const Report& rep){
          HIPevts+=rep.HIPevts;
          HIPevts_failed+=rep.HIPevts_failed;
        }
      };

      const Report& getReport() const
      { return m_report; }

      virtual void processStep(const G4Step*) override;
      virtual void beginOfEvent(const G4Event*) override;
      virtual void endOfEvent(const G4Event*) override;

    private:
      Report m_report;
      bool m_HIPacc;

  }; // class HIPLArVolumeAccept

} // namespace G4UA

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_HIPLArVolumeAccept_H
#define G4UserActions_HIPLArVolumeAccept_H

#include "G4UserSteppingAction.hh"
#include "G4UserEventAction.hh"
#include "G4UserRunAction.hh"
#include "AthenaBaseComps/AthMessaging.h"

#include "GaudiKernel/ServiceHandle.h"

namespace G4UA
{

  /// @brief NEEDS DOCUMENTATION
  class HIPLArVolumeAccept : public AthMessaging,
                             public G4UserSteppingAction,
                             public G4UserEventAction
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

      virtual void UserSteppingAction(const G4Step*) override;
      virtual void BeginOfEventAction(const G4Event*) override;
      virtual void EndOfEventAction(const G4Event*) override;

    private:
      Report m_report;
      bool m_HIPacc;

  }; // class HIPLArVolumeAccept

} // namespace G4UA

#endif

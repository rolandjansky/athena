/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_HIPLArVolumeAccept_H
#define G4UserActions_HIPLArVolumeAccept_H

#include "G4AtlasTools/UserActionBase.h"


#include <string>

class HIPLArVolumeAccept final: public UserActionBase {

  public:
 HIPLArVolumeAccept(const std::string& type, const std::string& name, const IInterface* parent): UserActionBase(type,name,parent),HIPacc(0),HIPevts(0),HIPevts_failed(0) {}
   virtual void BeginOfEvent(const G4Event*) override;
   virtual void EndOfEvent(const G4Event*) override;
   virtual void BeginOfRun(const G4Run*) override;
   virtual void EndOfRun(const G4Run*) override;
   virtual void Step(const G4Step*) override;

   virtual StatusCode queryInterface(const InterfaceID&, void**) override;

  private:
   bool HIPacc;
   int HIPevts;
   int HIPevts_failed;

};


#include "G4AtlasInterfaces/ISteppingAction.h"
#include "G4AtlasInterfaces/IBeginEventAction.h"
#include "G4AtlasInterfaces/IEndEventAction.h"
#include "G4AtlasInterfaces/IBeginRunAction.h"
#include "G4AtlasInterfaces/IEndRunAction.h"
#include  "AthenaBaseComps/AthMessaging.h"

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

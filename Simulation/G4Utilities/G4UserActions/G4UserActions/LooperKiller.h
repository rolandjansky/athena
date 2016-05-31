/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_LooperKiller_H
#define G4UserActions_LooperKiller_H

#include "G4AtlasTools/UserActionBase.h"

#include <string>

class LooperKiller final: public UserActionBase {

  public:

  LooperKiller(const std::string& type, const std::string& name, const IInterface* parent);

  virtual void Step(const G4Step*) override;

  virtual StatusCode finalize() override;
  virtual StatusCode queryInterface(const InterfaceID&, void**) override;

 private:
  long m_max_steps;
  int m_print_steps;
  int m_count_steps;
  long m_killed_tracks;
  int m_verbose_level;
  bool m_abortEvent, m_setError;

};

#endif

//=============================================================================
// New design below for multithreading
//=============================================================================


#ifndef G4USERACTIONS_G4UA_LOOPERKILLER_H
#define G4USERACTIONS_G4UA_LOOPERKILLER_H

// Infrastructure includes
#include "AthenaKernel/MsgStreamMember.h"
#include "G4AtlasInterfaces/ISteppingAction.h"
#include "AthenaBaseComps/AthMessaging.h"

namespace G4UA
{

  /// @class LooperKiller
  /// @brief A user action to kill looping tracks.
  ///
  class LooperKiller final : public ISteppingAction, public AthMessaging
  {

    public:

      struct Config
      {
        int MaxSteps=1000000;
        bool PrintSteps=100;
        int VerboseLevel=1;
        bool AbortEvent=true;
        bool SetError=false;
      };

      struct Report
      {
        long killed_tracks=0;
        void merge(const Report& rep){
          killed_tracks+=rep.killed_tracks;
        }
      };

      LooperKiller(const Config& config);

      virtual void processStep(const G4Step*) override;
      /// Retrieve  results
      const Report& getReport() const
      { return m_report; }

    private:

      typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
      /// Pointer to StoreGate (event store by default)
      mutable StoreGateSvc_t m_evtStore;
      /// Pointer to StoreGate (detector store by default)
      mutable StoreGateSvc_t m_detStore;

      /// My configuration options
      Config m_config;
      Report m_report;
      int m_count_steps;

  }; // class LooperKiller

}

#endif

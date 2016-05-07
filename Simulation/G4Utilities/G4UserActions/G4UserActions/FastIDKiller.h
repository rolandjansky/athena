/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_FastIDKiller_H
#define G4UserActions_FastIDKiller_H

#include "G4AtlasTools/UserActionBase.h"

#include <string>

class FastIDKiller final: public UserActionBase {

 public:
  FastIDKiller(const std::string& type, const std::string& name, const IInterface* parent);

  virtual void BeginOfRun(const G4Run*) override;
  virtual void Step(const G4Step*) override;
  virtual StatusCode queryInterface(const InterfaceID&, void**) override;
  virtual StatusCode finalize() override;


 private:
  void doInit();                    //!< Initialization; zeroes variables and outputs a message
  void KillParticle(const G4Step* aStep);        //!< Routine for killing single particles
  double m_energyCut;                //!< Cut off for electron energy, below which they will be killed
  unsigned long m_killCount;            //!< Counts the number of particles killed
  bool m_init;                    //!< Initialization boolean (actions don't always see 'begin of run')
  double m_idR, m_idZ;                            //!< Edge of the inner detector envelope in R and Z

};


#include "G4AtlasInterfaces/IBeginRunAction.h"
#include "G4AtlasInterfaces/ISteppingAction.h"
#include "AthenaBaseComps/AthMessaging.h"

namespace G4UA
{

  /// @brief NEEDS DOCUMENTATION
  class FastIDKiller : public IBeginRunAction, public ISteppingAction,
                       public AthMessaging
  {

  public:

    /// Configuration parameters
    struct Config
    {
      float R=1150.; // ID outer radius by default.
      float Z=3490.; // ID maximum Z coordiate by default.

    };

    /// Constructor
    FastIDKiller(const Config& config);

    /// Per-thread results for merging at end of run.
    struct Report
    {
      int killCount=0;
      void merge(const Report& rep){
        killCount+=rep.killCount;
      }
    };

    const Report& getReport() const
    { return m_report; }

    virtual void beginOfRun(const G4Run*) override;
    virtual void processStep(const G4Step*) override;

  private:

    void KillParticle(const G4Step* aStep);

    Config m_config;
    Report m_report;
    bool m_init;
    float m_energyCut;

  }; // class FastIDKiller

} // namespace G4UA

#endif

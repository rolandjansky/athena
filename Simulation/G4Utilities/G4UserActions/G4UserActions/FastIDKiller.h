/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FastIDKiller_H
#define FastIDKiller_H

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

#endif

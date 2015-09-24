/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4SimTimer_H
#define G4SimTimer_H

#include "G4AtlasTools/UserActionBase.h"

#include <string>

#include "G4Timer.hh"

class G4SimTimer final: public UserActionBase {

 private:
  double runTime;
  double eventTime;
  double accumulatedEventTime;
  double accumulatedEventTimeSquared;
  unsigned int nrOfEntries;
  G4Timer* runTimer;
  G4Timer* eventTimer;
  double averageTimePerEvent();
  double Sigma();
  
 public:
  
 G4SimTimer(const std::string& type, const std::string& name, const IInterface* parent):UserActionBase(type,name,parent),runTime(0),eventTime(0),
    accumulatedEventTime(0),accumulatedEventTimeSquared(0),
    nrOfEntries(0)
      {
	runTimer=new G4Timer();
	eventTimer=new G4Timer();
      }
  
  ~G4SimTimer() {delete runTimer; delete eventTimer;}
  
  virtual StatusCode initialize() override;
  
  virtual void BeginOfEvent(const G4Event*) override;
  virtual void EndOfEvent(const G4Event*) override;
  virtual void BeginOfRun(const G4Run*) override;
  virtual void EndOfRun(const G4Run*) override;

  virtual StatusCode queryInterface(const InterfaceID&, void**) override;
  
};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4TrackCounter_H
#define G4TrackCounter_H

#include <string>

#include "G4AtlasTools/UserActionBase.h"



class G4TrackCounter final: public UserActionBase {

  private:
   unsigned int ntracks;
   unsigned int ntracks_tot;
   unsigned int ntracks_en;
   unsigned int ntracks_sec;
   unsigned int nevts;
   double avtracks;
   double avtracks_en;
   double avtracks_sec;

  public:
   G4TrackCounter(const std::string& type, const std::string& name, const IInterface* parent):UserActionBase(type,name,parent),
     ntracks(0),ntracks_tot(0),ntracks_en(0),
                                 ntracks_sec(0),nevts(0),avtracks(0),avtracks_en(0),avtracks_sec(0){};

   virtual StatusCode initialize() override;
   virtual StatusCode queryInterface(const InterfaceID&, void**) override;

   virtual void BeginOfEvent(const G4Event*) override;
   virtual void EndOfRun(const G4Run*) override;
   virtual void PreTracking(const G4Track* aTrack) override;

};

#endif

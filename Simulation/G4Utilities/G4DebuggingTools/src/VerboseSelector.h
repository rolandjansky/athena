/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_VerboseSelector_H
#define G4DEBUGGINGTOOLS_VerboseSelector_H

#include "G4AtlasTools/UserActionBase.h"

#include <string>

class VerboseSelector final: public UserActionBase {

  public:
  VerboseSelector(const std::string& type, const std::string& name, const IInterface* parent);

   virtual void EndOfEvent(const G4Event*) override;
   virtual void Step(const G4Step*) override;
   virtual void PreTracking(const G4Track* aTrack) override;
   virtual void PostTracking(const G4Track* aTrack) override;

   virtual StatusCode initialize() override;
   virtual StatusCode queryInterface(const InterfaceID&, void**) override;

  private:
   int m_targetEvent;
   int m_targetTrack;
   int m_targetBarcode;
   int m_verboseLevel;
   int m_evtCount;
   int m_verb;
   double m_Xmin, m_Xmax;
   double m_Ymin, m_Ymax;
   double m_Zmin, m_Zmax;
};

#endif //G4DEBUGGINGTOOLS_VerboseSelector_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4AtlantisDumper_H
#define G4DEBUGGINGTOOLS_G4AtlantisDumper_H

#include "G4AtlasTools/UserActionBase.h"

#include <fstream>
#include <string>

class G4AtlantisDumper final: public UserActionBase {

  public:
   G4AtlantisDumper(const std::string& type, const std::string& name, const IInterface* parent);

   virtual void BeginOfEvent(const G4Event*) override;
   virtual void EndOfEvent(const G4Event*) override;
   virtual void Step(const G4Step*) override;
   virtual StatusCode queryInterface(const InterfaceID&, void**) override;
   virtual StatusCode initialize() override;

  private:
   int m_nsteps;
   std::ofstream *m_file;
   double m_tedep_cut,m_length_cut,m_p_cut, m_ke_cut;
   std::string m_filename;

};

#endif //G4DEBUGGINGTOOLS_G4AtlantisDumper_H

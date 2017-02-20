/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DarkSector_DarkSectorLSP_H
#define DarkSector_DarkSectorLSP_H

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

class DarkSectorLSP: public G4ParticleDefinition
{
private:

  static DarkSectorLSP* theInstance;
  DarkSectorLSP(){}
  ~DarkSectorLSP(){}

public:

  static DarkSectorLSP* Definition(G4double mass=-1, G4double width=-1, G4double charge=-1, G4double PDG=-1, G4bool stable=true, G4double lifetime=-1, G4bool shortlived=false);


};
#endif //DarkSector_DarkSectorLSP_H


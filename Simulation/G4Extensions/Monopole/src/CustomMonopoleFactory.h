/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MONOPOLE_CustomMonopoleFactory_h
#define MONOPOLE_CustomMonopoleFactory_h 1

// package headers
#include "CustomMonopole.h"
// STL headers
#include <set>


// ######################################################################
// ###                          Monopole                              ###
// ######################################################################

class CustomMonopoleFactory
{
private:
  static bool loaded;
  //  static std::set<G4ParticleDefinition *> m_particles;
  static std::set<CustomMonopole *> m_particles;

public:
  static void loadCustomMonopoles();
  //   static bool isCustomMonopole(G4ParticleDefinition *particle);
  static bool isCustomMonopole(CustomMonopole *particle);


};


#endif // MONOPOLE_CustomMonopoleFactory_h

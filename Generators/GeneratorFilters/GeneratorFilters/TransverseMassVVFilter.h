/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERSTRANSVERSEMASS_H
#define GENERATORFILTERSTRANSVERSEMASS_H

#include "GeneratorModules/GenFilter.h"

/// Filter on dileptons within a ZZ/WZ transverse mass 
///
///
/// @authors Monica Trovatelli <monica.trovatelli@cern.ch>, Roberto Di Nardo <roberto.di.nardo@cern.ch>
class TransverseMassVVFilter : public GenFilter {
public:

  TransverseMassVVFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterEvent();

private:

  double m_minMass;
  double m_maxMass;

};

#endif

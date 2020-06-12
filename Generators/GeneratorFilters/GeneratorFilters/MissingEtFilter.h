/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_MISSINGETFILTER_H
#define GENERATORFILTERS_MISSINGETFILTER_H

#include "GeneratorModules/GenFilter.h"

/// Filters on total missing energy from nus and LSPs
/// @author Seth Zenz, December 2005
class MissingEtFilter:public GenFilter {
public:

  MissingEtFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterEvent();

  bool fromTau( HepMC::ConstGenParticlePtr tp ) const;
  bool fromWZ( HepMC::ConstGenParticlePtr tp ) const;

 private:

  double m_METmin;
  bool m_useHadronicNu;

};

#endif

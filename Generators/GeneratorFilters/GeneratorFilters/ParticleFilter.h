/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_PARTICLEFILTER_H
#define GENERATORFILTERS_PARTICLEFILTER_H

#include "GeneratorModules/GenFilter.h"

/// The filter will pass only if it finds a particle with the specified properties
/// @author I Hinchliffe, May 2004
/// @author B K Gjelsten, March 2006
/// @author R Bruneliere, Aug 2008
class ParticleFilter : public GenFilter {
public:

  ParticleFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterEvent();

private:

  double m_Ptmin;
  double m_EtaRange;
  double m_EnergyRange;
  int    m_PDGID;
  int    m_StatusReq;
  int    m_MinParts;
  bool   m_Exclusive;
};

#endif

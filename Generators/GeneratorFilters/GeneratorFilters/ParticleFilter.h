/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

  Gaudi::Property<double> m_Ptmin{this, "Ptcut", 10000.};
  Gaudi::Property<double> m_EtaRange{this, "Etacut", 10.0};
  Gaudi::Property<double> m_EnergyRange{this, "Energycut", 100000000.0};
  Gaudi::Property<int>    m_PDGID{this, "PDG", 11};
  Gaudi::Property<int>    m_StatusReq{this, "StatusReq", 1};
  Gaudi::Property<int>    m_MinParts{this, "MinParts", 1};
  Gaudi::Property<bool>   m_Exclusive{this, "Exclusive", false};
  Gaudi::Property<int>    m_simBarcodeOffset{this, "SimBarcodeOffset", 200000};
};

#endif

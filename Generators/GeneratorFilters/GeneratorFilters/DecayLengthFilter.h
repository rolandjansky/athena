/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_RPVDECAY_H
#define GENERATORFILTERS_RPVDECAY_H

#include "GeneratorModules/GenFilter.h"
#include <string>

/// Selects late decays that occur inside a specific region of the detector,
/// @author C. Horn, Oct 2008
class DecayLengthFilter : public GenFilter {
public:

  DecayLengthFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterEvent();

private:

  std::string m_McEventCollectionName;

  // Specify desired decay region in R and Z
  float m_Rmin;
  float m_Rmax;
  float m_Zmin;
  float m_Zmax;
  float m_particle_id;

  bool isAccepted(float distR, float distZ);

};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_LEADINGPHOTONFILTER_H
#define GENERATORFILTERS_LEADINGPHOTONFILTER_H

#include "GeneratorModules/GenFilter.h"


/// @brief Filter events based on the pT of the hardest photon within a given |eta|
class LeadingPhotonFilter : public GenFilter {
public:

  LeadingPhotonFilter(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode filterEvent();

private:

  double m_Ptmin;
  double m_Ptmax;
  double m_EtaRange;

};


#endif

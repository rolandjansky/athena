/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_TTBARPHOTONWHIZARDHWGFILTER_H
#define GENERATORFILTERS_TTBARPHOTONWHIZARDHWGFILTER_H

#include "GeneratorModules/GenFilter.h"

/// Require that the ME photon (WHIZARD-specific) passes a pT and eta cut
/// @author Oliver Rosenthal, December 2010
class TTbarPhotonWhizardHwgFilter : public GenFilter {
public:

  TTbarPhotonWhizardHwgFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterEvent();

private:

	double m_Ptmin;
	double m_Etamax;

};

#endif

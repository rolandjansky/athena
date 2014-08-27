/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_TOPCKMKINFILTER_H
#define GENERATORFILTERS_TOPCKMKINFILTER_H
/// Selects the process ttbar --> Wq1 Wq2 for q = d,s,b with q1 != q2.
/// It selects the decays W --> e nu, mu nu avoiding taus, with proper kinematic cuts
///
/// You need to specify the PDG id of the children in PDGChild (2-component vector)
/// and the children pt and eta cuts in PtMinChild and EtaRangeChild
///
/// @author Javier Llorente. March 2011

#include "GeneratorModules/GenFilter.h"

class TopCKMKinFilter : public GenFilter {
public:

  TopCKMKinFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterEvent();

private:

	std::vector<int> m_PDGChild;
	double m_PtMinChild;
	double m_EtaRangeChild;
};

#endif

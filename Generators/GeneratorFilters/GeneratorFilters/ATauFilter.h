/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_ATAUFILTER_H
#define GENERATORFILTERS_ATAUFILTER_H

#include "GeneratorModules/GenFilter.h"
/// Filters and looks for taus
/// @author Michael Heldmann, Jan 2003
class ATauFilter : public GenFilter {
public:

  ATauFilter(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode filterInitialize();
  StatusCode filterFinalize();
  StatusCode filterEvent();

private:

  //double m_Ptmin;
	double m_EtaRange;

	double m_maxdphi;

	double m_llPtmine;
	double m_llPtminmu;

	double m_lhPtmine;
	double m_lhPtminmu;
	double m_lhPtminh;

	double m_hhPtmin;

	double m_eventsaccepted;
	double m_eventsrefused;

	double m_eventsll;
	double m_eventslh;
	double m_eventshh;

	double m_eventsllacc;
	double m_eventslhacc;
	double m_eventshhacc;

};

#endif

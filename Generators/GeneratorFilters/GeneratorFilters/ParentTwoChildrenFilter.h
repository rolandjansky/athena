/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_PARENTTWOCHILDRENFILTER_H
#define GENERATORFILTERS_PARENTTWOCHILDRENFILTER_H

#include "GeneratorModules/GenFilter.h"

/// Allows the user to search for any given decay  Parent -> Child1 + anti(Child1) AND Parent -> Child2 + anti(Child2)...
/// @author Chaouki Boulahouache, Oct 2008
class ParentTwoChildrenFilter : public GenFilter {
public:

  ParentTwoChildrenFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterEvent();

private:

	std::vector<int> m_PDGParent;
	double m_PtMinParent;
	double m_EtaRangeParent;
	std::vector<int> m_PDGChild;
	double m_PtMinChild;
	double m_EtaRangeChild;

};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FILTER_USING_SPACEPOINTS_H
#define FILTER_USING_SPACEPOINTS_H

#include "AthenaBaseComps/AthAlgorithm.h"

class FilterUsingSpacePoints : public AthAlgorithm
{
	public:
		FilterUsingSpacePoints(const std::string& name, ISvcLocator* pSvcLocator);

		StatusCode initialize();
		StatusCode finalize();
		StatusCode execute();

	private:
		unsigned int m_SpacePoints_required; // Number of A side triggers required for pass
    std::string m_SpacePointContainerName;  //SpacePointContainer to use
};
#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FILTER_USING_MBTS_H
#define FILTER_USING_MBTS_H

#include "AthenaBaseComps/AthAlgorithm.h"

class FilterUsingMBTS : public AthAlgorithm
{
	public:
		FilterUsingMBTS(const std::string& name, ISvcLocator* pSvcLocator);

		StatusCode initialize();
		StatusCode finalize();
		StatusCode execute();

	private:
		unsigned int m_nA_required; // Number of A side triggers required for pass
		unsigned int m_nC_required; // Number of C side triggers required for pass
		unsigned int m_ctpOffset; // ctpID for L1_MBTSA0
		std::vector<unsigned int> m_ctpID;
		std::vector<std::string>  m_counterLabel;
};
#endif

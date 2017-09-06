/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EventDisplayFilters/FilterUsingMBTS.h"
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Result/CTP_Decoder.h"

#include <sstream>

/****
 * simple filter for splash events based on MBTS multiplicites
 * @author: Brian Thomas Martin
 ****/

	FilterUsingMBTS::FilterUsingMBTS(const std::string& name, ISvcLocator* pSvcLocator)  :  AthAlgorithm(name, pSvcLocator)
{
	declareProperty("NumberOfAsideHits", m_nA_required=4);
	declareProperty("NumberOfCsideHits", m_nC_required=4);
	declareProperty("FirstMbtsCtpId",    m_ctpOffset=30);
}

StatusCode FilterUsingMBTS::initialize()
{
        ATH_MSG_INFO( "Initializing " << name()  );
	
	// Set trigger mapping and build labels
	for(int c=0;c<32;c++)
	{
		m_ctpID.push_back(m_ctpOffset+c);
		std::ostringstream ss;
		ss << "L1_MBTS" << ((c<16) ? "A" : "C") << ((c<16) ? c : (c-16)) ;
		m_counterLabel.push_back(ss.str());
	}
	
	return StatusCode::SUCCESS;
}

StatusCode FilterUsingMBTS::execute()
{
	// Decode CTP RDO
	const DataHandle < CTP_RDO > theCTP_RDO = 0;
	ATH_CHECK( evtStore()->retrieve(theCTP_RDO, "CTP_RDO") );
	CTP_Decoder ctp;
	ctp.setRDO(theCTP_RDO);
	uint32_t numberBC = theCTP_RDO->getNumberOfBunches();

  unsigned int m_nA=0; // Number of A side triggers per event
  unsigned int m_nC=0; // Number of C side triggers per event

	if (numberBC > 0) 
	{
		short l1aBC = theCTP_RDO->getL1AcceptBunchPosition();
		const std::vector < CTP_BC > &BCs = ctp.getBunchCrossings();
		const CTP_BC & bunch = BCs[l1aBC];
		unsigned int l1aBCID = bunch.getBCID();
		ATH_MSG_DEBUG( "Number of Bunches in CTP window: " << numberBC  );
		ATH_MSG_DEBUG( "Level 1 Accept Bunch: " << l1aBC    );
		ATH_MSG_DEBUG( "Level 1 Accept BCID: "  << l1aBCID  );
		const std::bitset < 512 > TBP(bunch.getTBP());
		for(int c=0;c<32;c++)  // Loop over MBTS counters
		{
			// Check the L1 result for each MBTS counter
			if (TBP.test(m_ctpID[c]))
			{
                                ATH_MSG_INFO( "Trigger fired for : " << m_counterLabel[c]  );
				if(c<16) m_nA++;
				else     m_nC++;
			}
		} // end MBTS counter loop
	} // end if nBC>0

	// Check if filter is passed
	if( (m_nA >= m_nA_required) && (m_nC >= m_nC_required) )
	{
                ATH_MSG_INFO( "MBTS filter passed"  );
		this->setFilterPassed(true);
	}
	else
	{
                ATH_MSG_INFO( "MBTS filter failed"  );
		this->setFilterPassed(false);
	}	
	
	return StatusCode::SUCCESS;
}

StatusCode FilterUsingMBTS::finalize()
{
	return StatusCode::SUCCESS;
}

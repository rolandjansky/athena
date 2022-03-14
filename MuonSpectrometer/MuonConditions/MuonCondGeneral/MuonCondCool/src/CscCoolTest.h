/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDCOOL_CSCCOOLTEST_H
#define MUONCONDCOOL_CSCCOOLTEST_H

/**CscCoolTest - simple algorithm to test reading out parameters from the csc cool database using 
the CscCoolStrSvc getParameter method.  Can be used to test the auto-updating of the local cache withing
CscCoolStrSvc*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonCondInterface/CscICoolStrSvc.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

/**
	@class CscCoolTest

	@brief Class used to to test CscCoolStrSvc parameter retrieval

	@author lampen@physics.arizona.edu
	@section Description
	CscCoolTest is an algorithm used to demonstrate the use of the getParameter method of CscCoolStrSvc. 
	getParameter retrieves database info from a localy stored "mirror" of the cool database that is 
	quicker to access.  The local database should be updated automatically whenever it becomes out of date.
	Using an appropriate jobOptions file, this algoritm can be used to test this functionality.
*/

class CscCoolTest: public AthAlgorithm
{
	public: 
		CscCoolTest(const std::string& name, ISvcLocator* pSvcLocator);
		~CscCoolTest()=default;
		/**initialize CscCoolStrSvc*/
		StatusCode initialize(void);
		/**prints out all the parameters from a single strip as requested in the jobOptions file*/
		StatusCode execute(void);

	private:
		MuonCalib::CscICoolStrSvc* p_cscCoolStrSvc;
		int m_stripHash;
		bool m_doStatusTest;
		ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
};

//} // end namespace MuonCalib


#endif

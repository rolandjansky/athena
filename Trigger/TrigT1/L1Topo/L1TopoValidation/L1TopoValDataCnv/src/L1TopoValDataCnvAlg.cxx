/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigSteeringEvent/HLTExtraData.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "TrigT1Result/CTP_RDO.h"

// Local include(s):
#include "L1TopoValDataCnvAlg.h"
#include <vector>

namespace xAODMaker {

  L1TopoValDataCnvAlg::L1TopoValDataCnvAlg( const std::string& name,
                                           ISvcLocator* svcLoc )
    : AthAlgorithm( name, svcLoc ), 
      m_simulatedTopoResult(4,0),
      m_ctpTopoTIPWords(16,0),m_ctpTopoTBPWords(16,0),m_ctpTopoTAPWords(16,0),m_ctpTopoTAVWords(16,0)
  {

    declareProperty( "HLTResultKey" , m_hltResultKey = "HLTResult_HLT" );
    declareProperty( "xAODKey"      , m_xaodKey      = "HLT_xAOD__TrigCompositeContainer_L1TopoValData" );
    declareProperty( "WriteTAPWords", m_writeTAP     = true );
    declareProperty( "WriteTAVWords", m_writeTAV     = true );
  }

  StatusCode L1TopoValDataCnvAlg::initialize() {

    // Print parmeters
    ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
    ATH_MSG_INFO( "HLT Result Key                : " << m_hltResultKey );
    ATH_MSG_INFO( "xAOD output container Key     : " << m_xaodKey );
    ATH_MSG_INFO( "write out TAP words           : " << m_writeTAP );
    ATH_MSG_INFO( "write out TAV words           : " << m_writeTAV );

    return StatusCode::SUCCESS;
  }

  StatusCode L1TopoValDataCnvAlg::execute() {

    // Create the xAOD object and its auxiliary store:
    xAOD::TrigCompositeContainer*   xaod = new xAOD::TrigCompositeContainer();
    xAOD::TrigCompositeAuxContainer* aux = new xAOD::TrigCompositeAuxContainer();  
    xaod->setStore(aux);

    // Fill the xAOD object:
    xAOD::TrigComposite* tc = new xAOD::TrigComposite();
    xaod->push_back(tc);
    tc->setName("TopoValidationData");

    // Retrieve the HLT result object:
    const DataHandle<HLT::HLTResult> hltResult;
    CHECK( evtStore()->retrieve( hltResult, m_hltResultKey ) );

    // Store the simulated topo result
    m_simulatedTopoResult.clear();
    
    if ((hltResult) && (hltResult->getExtraData().anonymous.size() > 0)) {
      ATH_MSG_DEBUG( "HLTExtraData.anonymous size = " << hltResult->getExtraData().anonymous.size() );
      ATH_MSG_DEBUG( " hltResult->getExtraData().anonymous[0] =  : " << hltResult->getExtraData().anonymous[0] );
      m_simulatedTopoResult.assign( (hltResult->getExtraData()).anonymous.begin()+1,
				    (hltResult->getExtraData()).anonymous.begin()+(hltResult->getExtraData()).anonymous[0]+1 );
      ATH_MSG_DEBUG( " m_simulatedTopoResult =  " << m_simulatedTopoResult );
      tc->setDetail("L1TopoSimulation", m_simulatedTopoResult );       
    }

    // Get the TIP/TBP/TAP/TAV words from the CTP (DAQ) fragment for L1 accept bunch
    const DataHandle<CTP_RDO> ctpRDO;
    CHECK( evtStore()->retrieve( ctpRDO , "CTP_RDO" ) );

    if ((ctpRDO) && (ctpRDO->getNumberOfBunches() > 0)) {
      ATH_MSG_DEBUG( "Retrieve of CTP_RDO successful. \n" << ctpRDO->print(true) );

      // extract the information for L1A-1,L1A,L1A+1 bunches
      int bunch;
      std::string suffix("");

      int bunch_offset_start(0);
      int bunch_offset_end(1);
      if (ctpRDO->getNumberOfBunches() >= 3) {
	bunch_offset_start = -1;
	bunch_offset_end   = 2;
      }

      // Get the TIP/TBP/TAP/TAV words
      std::vector<uint32_t> tipWords = ctpRDO->getTIPWords();
      std::vector<uint32_t> tbpWords = ctpRDO->getTBPWords();
      std::vector<uint32_t> tapWords = ctpRDO->getTAPWords();
      std::vector<uint32_t> tavWords = ctpRDO->getTAVWords();

      for (int bunch_offset = bunch_offset_start; bunch_offset < bunch_offset_end; bunch_offset++) {
	m_ctpTopoTIPWords.clear();
	m_ctpTopoTBPWords.clear();
	m_ctpTopoTAPWords.clear();
	m_ctpTopoTAVWords.clear();

	// Calculate bunch position
	bunch = ctpRDO->getL1AcceptBunchPosition() + bunch_offset;
	if (bunch < 0) continue ;
	if (bunch > (int)ctpRDO->getNumberOfBunches()) continue ;

	if (bunch_offset == -1) {
	  suffix = "-1";
	} else if (bunch_offset == 1) {
	  suffix = "+1";
	} else {
	  suffix = "";
	}

	// Get Topo TIP words for the bunch
	if ((tipWords.size() > 0) && (tipWords.size() >= (bunch + 1)*(ctpRDO->getCTPVersion()).getTIPwords())) { 
	  m_ctpTopoTIPWords.assign( tipWords.begin() + bunch*(ctpRDO->getCTPVersion()).getTIPwords(),
				    tipWords.begin() + (bunch + 1)*(ctpRDO->getCTPVersion()).getTIPwords()
				    );
	}

	// Get Topo TBP words for the bunch
	if ((tbpWords.size() > 0) && (tbpWords.size() >= (bunch + 1)*(ctpRDO->getCTPVersion()).getTBPwords())) { 
	  m_ctpTopoTBPWords.assign( tbpWords.begin() + bunch*(ctpRDO->getCTPVersion()).getTBPwords(),
				    tbpWords.begin() + (bunch + 1)*(ctpRDO->getCTPVersion()).getTBPwords()
				    );
	}

	// Get Topo TAP words for the bunch
	if ((tapWords.size() > 0) && (tapWords.size() >= (bunch + 1)*(ctpRDO->getCTPVersion()).getTAPwords())) { 
	  m_ctpTopoTAPWords.assign( tapWords.begin() + bunch*(ctpRDO->getCTPVersion()).getTAPwords(),
				    tapWords.begin() + (bunch + 1)*(ctpRDO->getCTPVersion()).getTAPwords()
				    );
	}

	// Get Topo TAV words for the bunch
	if ((tavWords.size() > 0) && (tavWords.size() >= (bunch + 1)*(ctpRDO->getCTPVersion()).getTAVwords())) { 
	  m_ctpTopoTAVWords.assign( tavWords.begin() + bunch*(ctpRDO->getCTPVersion()).getTAVwords(),
				    tavWords.begin() + (bunch + 1)*(ctpRDO->getCTPVersion()).getTAVwords()
				    );
	}

	ATH_MSG_DEBUG( " NumberOfBunches = " << ctpRDO->getNumberOfBunches() << 
		       " L1AcceptBunchPosition = " << ctpRDO->getL1AcceptBunchPosition() << "\n" <<
		       " recorded bunch        = " << bunch << " suffix = " << suffix <<  "\n" <<
		       " m_ctpTopoTIPWords = " << m_ctpTopoTIPWords << "\n" <<
		       " m_ctpTopoTBPWords = " << m_ctpTopoTBPWords << "\n" <<
		       " m_ctpTopoTAPWords = " << m_ctpTopoTAPWords << "\n" <<
		       " m_ctpTopoTAVWords = " << m_ctpTopoTAVWords
		       );

	tc->setDetail("L1TopoTIPCTPL1A"+suffix, m_ctpTopoTIPWords );
	tc->setDetail("L1TopoTBPCTPL1A"+suffix, m_ctpTopoTBPWords );
	if(m_writeTAP) tc->setDetail("L1TopoTAPCTPL1A"+suffix, m_ctpTopoTAPWords );
	if(m_writeTAV) tc->setDetail("L1TopoTAVCTPL1A"+suffix, m_ctpTopoTAVWords );
      }
    }

    // Record the xAOD containers:
    CHECK( evtStore()->record( xaod, m_xaodKey ) );
    CHECK( evtStore()->record( aux, m_xaodKey + "Aux." ) );

    return StatusCode::SUCCESS;
  }
} // namespace xAODMaker

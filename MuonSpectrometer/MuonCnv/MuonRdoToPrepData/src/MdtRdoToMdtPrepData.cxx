/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// Author: Davide Costanzo
/// BNL, April 04 2005

/// algorithm to decode RDO into PrepRawData


#include "MuonRdoToPrepData/MdtRdoToMdtPrepData.h"
#include "Identifier/IdentifierHash.h"

MdtRdoToMdtPrepData::MdtRdoToMdtPrepData(const std::string& name,
                                         ISvcLocator* pSvcLocator) :
AthAlgorithm(name, pSvcLocator),
m_tool( "Muon::MdtRdoToPrepDataTool/MdtPrepDataProviderTool"), // 'this' as 2nd arg would make it private tool
m_print_inputRdo(false),
m_print_prepData(false),
m_seededDecoding(false),
m_roiCollectionKey("OutputRoIs"),
m_regionSelector("RegSelSvc",name)
{
    declareProperty("DecodingTool",       m_tool,       "mdt rdo to prep data conversion tool" );
    declareProperty("PrintInputRdo",      m_print_inputRdo, "If true, will dump information about the input RDOs");
    declareProperty("PrintPrepData",      m_print_prepData, "If true, will dump information about the resulting PRDs");
    declareProperty("DoSeededDecoding",   m_seededDecoding, "If true decode only in RoIs");
    declareProperty("RoIs",               m_roiCollectionKey, "RoIs to read in");
    declareProperty("RegionSelectionSvc", m_regionSelector, "Region Selector");
}

StatusCode MdtRdoToMdtPrepData::finalize()
{
    ATH_MSG_DEBUG( "in finalize()"  );
    return StatusCode::SUCCESS;
}

StatusCode MdtRdoToMdtPrepData::initialize()
{  
    ATH_MSG_DEBUG( " in initialize()"  );
    
    // verify that our tool handle is pointing to an accessible tool
    ATH_CHECK( m_tool.retrieve() );
    ATH_MSG_INFO( "Retrieved " << m_tool  );

    //Nullify key from scheduler if not needed  
    if(!m_seededDecoding) m_roiCollectionKey = "";
    if(m_seededDecoding){
      ATH_CHECK(m_roiCollectionKey.initialize());
      if (m_regionSelector.retrieve().isFailure()) {
	ATH_MSG_FATAL("Unable to retrieve RegionSelector Svc");
	return StatusCode::FAILURE;
      }
    }
    
    return StatusCode::SUCCESS;
}

StatusCode MdtRdoToMdtPrepData::execute()
{
    ATH_MSG_DEBUG( "**************** in MdtRdoToMdtPrepData::execute() ***********************************************"  );
    ATH_MSG_DEBUG( "in execute()"  );
    
    std::vector<IdentifierHash> myVector;
    std::vector<IdentifierHash> myVectorWithData;
    myVector.reserve(0); // empty vector 

    if(m_seededDecoding){//decoding from trigger roi
      SG::ReadHandle<TrigRoiDescriptorCollection> muonRoI(m_roiCollectionKey);
      if(!muonRoI.isValid()){
	ATH_MSG_WARNING("Cannot retrieve muonRoI "<<m_roiCollectionKey.key());
	return StatusCode::SUCCESS;
      }
      else{
	std::vector<uint32_t> mdtrobs;
	for(auto roi : *muonRoI){
	  m_regionSelector->DetROBIDListUint(MDT,*roi,mdtrobs);
	  if(mdtrobs.size()!=0){
	    ATH_CHECK(m_tool->decode(mdtrobs));
	    mdtrobs.clear();
	  }
	}
      }
    }
    else ATH_CHECK(  m_tool->decode(myVector, myVectorWithData) );

    if (m_print_inputRdo) m_tool->printInputRdo();
    if (m_print_prepData) m_tool->printPrepData();

    return StatusCode::SUCCESS;
}

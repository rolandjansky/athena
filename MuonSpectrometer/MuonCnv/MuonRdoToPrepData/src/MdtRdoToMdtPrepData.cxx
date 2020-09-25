/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// Author: Davide Costanzo
/// BNL, April 04 2005

/// algorithm to decode RDO into PrepRawData


#include "MuonRdoToPrepData/MdtRdoToMdtPrepData.h"
#include "Identifier/IdentifierHash.h"

MdtRdoToMdtPrepData::MdtRdoToMdtPrepData(const std::string& name,
                                         ISvcLocator* pSvcLocator) :
AthAlgorithm(name, pSvcLocator),
m_print_inputRdo(false),
m_print_prepData(false),
m_seededDecoding(false),
m_roiCollectionKey("OutputRoIs"),
m_mdtCollection("MDT_DriftCircles")
{
    declareProperty("PrintInputRdo",      m_print_inputRdo, "If true, will dump information about the input RDOs");
    declareProperty("PrintPrepData",      m_print_prepData, "If true, will dump information about the resulting PRDs");
    declareProperty("DoSeededDecoding",   m_seededDecoding, "If true decode only in RoIs");
    declareProperty("RoIs",               m_roiCollectionKey, "RoIs to read in");
    declareProperty("OutputCollection",   m_mdtCollection);
}

StatusCode MdtRdoToMdtPrepData::initialize()
{  
    ATH_MSG_DEBUG( " in initialize()"  );
    
    // verify that our tool handle is pointing to an accessible tool
    ATH_CHECK( m_tool.retrieve() );
    ATH_MSG_INFO( "Retrieved " << m_tool  );

    //Nullify key from scheduler if not needed  
    if(!m_seededDecoding){
      m_roiCollectionKey = "";
      m_mdtCollection="";
    }
    if(m_seededDecoding){
      ATH_CHECK(m_roiCollectionKey.initialize());
      ATH_CHECK(m_mdtCollection.initialize());
      if (m_regsel_mdt.retrieve().isFailure()) {
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
      bool decoded = false;
      SG::ReadHandle<TrigRoiDescriptorCollection> muonRoI(m_roiCollectionKey);
      if(!muonRoI.isValid()){
	ATH_MSG_WARNING("Cannot retrieve muonRoI "<<m_roiCollectionKey.key());
	return StatusCode::SUCCESS;
      }
      else{
	std::vector<uint32_t> mdtrobs;
	for(auto roi : *muonRoI){
	  m_regsel_mdt->ROBIDList(*roi,mdtrobs);
	  if(mdtrobs.size()!=0){
	    ATH_CHECK(m_tool->decode(mdtrobs));
	    mdtrobs.clear();
	    decoded=true;
	  }
	}
      }
      if(!decoded){
	//Need to store an empty prd container if we didn't decode anything
	//as the container is expected to exist downstream
	SG::WriteHandle<Muon::MdtPrepDataContainer> h_output (m_mdtCollection);
	ATH_CHECK(h_output.record(std::make_unique<Muon::MdtPrepDataContainer>(0)));
      }
    }
    else ATH_CHECK(  m_tool->decode(myVector, myVectorWithData) );


    if (m_print_inputRdo) m_tool->printInputRdo();
    if (m_print_prepData) m_tool->printPrepData();

    return StatusCode::SUCCESS;
}

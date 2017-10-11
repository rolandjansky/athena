/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// Author: Ketevi A. Assamagan
/// BNL, April 03 2005

/// algorithm to decode RDO into PrepRawData

/// Author: Ketevi A. Assamagan
/// BNL, April 03 2005

/// algorithm to decode RDO into PrepRawData

#include "StoreGate/DataHandle.h"
#include "Identifier/IdentifierHash.h"
#include "MuonRdoToPrepData/CscRdoToCscPrepData.h"

CscRdoToCscPrepData::CscRdoToCscPrepData(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator),
    m_muonRdoToPrepDataTool ("Muon::CscRdoToCscPrepDataTool/CscRdoToPrepDataTool"),
    m_print_inputRdo(false),
    m_print_prepData(false),
    m_seededDecoding(false),
    m_roiCollectionKey("OutputRoIs"),
    m_regionSelector("RegSelSvc",name),
    m_cscCollection("CSC_Measurements")
{
    declareProperty("CscRdoToCscPrepDataTool",     m_muonRdoToPrepDataTool );
    declareProperty("PrintInputRdo",      m_print_inputRdo, "If true, will dump information about the input RDOs");
    declareProperty("PrintPrepData",      m_print_prepData, "If true, will dump information about the resulting PRDs");
    declareProperty("DoSeededDecoding",   m_seededDecoding, "If true decode only in RoIs");
    declareProperty("RoIs",               m_roiCollectionKey, "RoIs to read in");
    declareProperty("RegionSelectionSvc", m_regionSelector, "Region Selector");
    declareProperty("OutputCollection", m_cscCollection);
}

StatusCode CscRdoToCscPrepData::finalize() {
  ATH_MSG_DEBUG("in finalize()");
  return StatusCode::SUCCESS;
}

StatusCode CscRdoToCscPrepData::initialize(){
    ATH_MSG_DEBUG(" in initialize()");

    if ( m_muonRdoToPrepDataTool.retrieve().isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve " << m_muonRdoToPrepDataTool);
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_INFO("Retrieved " << m_muonRdoToPrepDataTool);
    }

    //Nullify key from scheduler if not needed  
    if(!m_seededDecoding){
      m_roiCollectionKey = "";
      m_cscCollection="";
    }
    if(m_seededDecoding){
      ATH_CHECK(m_roiCollectionKey.initialize());
      ATH_CHECK(m_cscCollection.initialize());
      if (m_regionSelector.retrieve().isFailure()) {
	ATH_MSG_FATAL("Unable to retrieve RegionSelector Svc");
	return StatusCode::FAILURE;
      }
    }

    return StatusCode::SUCCESS;
}

StatusCode CscRdoToCscPrepData::execute() {
    ATH_MSG_DEBUG( " *************** in CscRdoToCscPrepData::execute()");

    /// process CSC RDO
    std::vector<IdentifierHash> givenIDs;
    std::vector<IdentifierHash> decodedIDs;
    StatusCode status = StatusCode::SUCCESS;

    if(m_seededDecoding){
      bool decoded=false;
      SG::ReadHandle<TrigRoiDescriptorCollection> muonRoI(m_roiCollectionKey);
      if(!muonRoI.isValid()){
	ATH_MSG_WARNING("Cannot retrieve muonRoI "<<m_roiCollectionKey.key());
	return StatusCode::SUCCESS;
      }
      else{
	for(auto roi : *muonRoI){
	  m_regionSelector->DetHashIDList(CSC,*roi,givenIDs);
	  if(givenIDs.size()!=0){
	    status=m_muonRdoToPrepDataTool->decode(givenIDs, decodedIDs);
	    givenIDs.clear();
	    decoded=true;
	  }
	}
      }
      if(!decoded){
	//Need to store an empty prd container if we didn't decode anything
	//as the container is expected to exist downstream
	SG::WriteHandle<Muon::CscStripPrepDataContainer> h_output (m_cscCollection);
	ATH_CHECK(h_output.record(std::make_unique<Muon::CscStripPrepDataContainer>(0)));
      }
    }
    else{
      // givenIDs size is zero so this invokes all the RDOs conversion to PrepData
      status =   m_muonRdoToPrepDataTool->decode(givenIDs, decodedIDs);
    }

    if (status.isFailure()) {
        ATH_MSG_ERROR("Unable to decode CSC RDO into CSC PrepRawData");
        return status;
    }

    if (m_print_inputRdo) m_muonRdoToPrepDataTool->printInputRdo();//printRpcPrepRawData();
    if (m_print_prepData) m_muonRdoToPrepDataTool->printPrepData();//printRpcPrepRawData();

    return status;
}




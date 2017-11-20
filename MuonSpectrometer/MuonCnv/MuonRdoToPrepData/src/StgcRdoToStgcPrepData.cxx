/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/DataHandle.h"
#include "Identifier/IdentifierHash.h"
#include "MuonRdoToPrepData/StgcRdoToStgcPrepData.h"

StgcRdoToStgcPrepData::StgcRdoToStgcPrepData(const std::string& name, ISvcLocator* pSvcLocator) :
AthAlgorithm(name, pSvcLocator),
m_decoderTool ("Muon::StgcRdoToStgcPrepDataTool/StgcRdoToStgcPrepDataTool"),
m_seededDecoding(false),
m_roiCollectionKey("OutputRoIs"),
m_regionSelector("RegSelSvc",name),
m_prdContainer("sTGC_Measurements")
{
  declareProperty("DoSeededDecoding",   m_seededDecoding, "If true decode only in RoIs");
  declareProperty("RoIs",               m_roiCollectionKey, "RoIs to read in");
  declareProperty("RegionSelectionSvc", m_regionSelector, "Region Selector");
  declareProperty("OutputCollection",   m_prdContainer);
}

StatusCode StgcRdoToStgcPrepData::finalize() {
  ATH_MSG_DEBUG("in finalize()");
  return StatusCode::SUCCESS;
}

StatusCode StgcRdoToStgcPrepData::initialize(){
  ATH_MSG_DEBUG(" in initialize()");

  //Nullify key from scheduler if not needed  
  if(!m_seededDecoding){
    m_roiCollectionKey = "";
    m_prdContainer="";
  }
    
  if(m_seededDecoding){
    ATH_CHECK(m_roiCollectionKey.initialize());
    ATH_CHECK(m_prdContainer.initialize());
    if (m_regionSelector.retrieve().isFailure()) {
      ATH_MSG_FATAL("Unable to retrieve RegionSelector Svc");
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode StgcRdoToStgcPrepData::execute() {
  ATH_MSG_DEBUG( " *************** in StgcRdoToStgcPrepData::execute()");

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
          status=m_decoderTool->decode(givenIDs, decodedIDs);
          givenIDs.clear();
          decoded=true;
        }
      }
    }
    if(!decoded){
      //Need to store an empty prd container if we didn't decode anything
      //as the container is expected to exist downstream
      SG::WriteHandle<Muon::sTgcPrepDataContainer> prds (m_prdContainer);
      ATH_CHECK(prds.record(std::make_unique<Muon::sTgcPrepDataContainer>(0)));
    }
  }
  else{
    // givenIDs size is zero so this invokes all the RDOs conversion to PrepData
    status =   m_decoderTool->decode(givenIDs, decodedIDs);
  }

  if (status.isFailure()) {
    ATH_MSG_ERROR("Unable to decode CSC RDO into CSC PrepRawData");
    return status;
  }

  return status;
}




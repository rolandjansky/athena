/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MdtRdoToPrepDataTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MmRdoToPrepDataTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"

#include "StoreGate/StoreGateSvc.h"

#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/MuonIdHelper.h"
#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"

#include "MuonPrepRawData/MMPrepDataContainer.h"

// BS access
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

using namespace MuonGM;
using namespace Trk;
using namespace Muon;

Muon::MmRdoToPrepDataTool::MmRdoToPrepDataTool(const std::string& t,
					       const std::string& n,
					       const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_muonMgr(0),
  m_mmIdHelper(0),
  m_muonIdHelper(0),
  m_mmPrepDataContainer(0)
{
  declareInterface<Muon::IMuonRdoToPrepDataTool>(this);

  //  template for property decalration
  declareProperty("OutputCollection",    m_mmPrepDataContainerKey = std::string("MM_Measurements"),
		  "Muon::MMPrepDataContainer to record");
  
}


Muon::MmRdoToPrepDataTool::~MmRdoToPrepDataTool()
{

}

StatusCode Muon::MmRdoToPrepDataTool::initialize()
{  
  ATH_MSG_DEBUG(" in initialize()");
  
  /// get the detector descriptor manager
  StoreGateSvc* detStore=0;
  StatusCode sc = serviceLocator()->service("DetectorStore", detStore);
  
  if (sc.isSuccess()) {
    sc = detStore->retrieve( m_muonMgr );
    if (sc.isFailure()) {
      ATH_MSG_FATAL(" Cannot retrieve MuonReadoutGeometry ");
      return sc;
    }
  } else {
    ATH_MSG_ERROR("DetectorStore not found ");
    return sc;
  }
  
  m_mmIdHelper = m_muonMgr->mmIdHelper();

  // check if the initialization of the data container is success
  ATH_CHECK(m_mmPrepDataContainerKey.initialize());


  ATH_MSG_INFO("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Muon::MmRdoToPrepDataTool::finalize()
{
  if (0 != m_mmPrepDataContainer) m_mmPrepDataContainer->release();
  return StatusCode::SUCCESS;

}

StatusCode Muon::MmRdoToPrepDataTool::processCollection(const MM_RawDataCollection *rdoColl, 
							std::vector<IdentifierHash>& idWithDataVect)
{
  ATH_MSG_DEBUG(" ***************** Start of processCsm");


  return StatusCode::SUCCESS;
}

// methods for ROB-based decoding
StatusCode Muon::MmRdoToPrepDataTool::decode( std::vector<IdentifierHash>& idVect, 
					       std::vector<IdentifierHash>& selectedIdVect )
{
  ATH_MSG_DEBUG("Size of Hashes" << idVect.size() );
  ATH_MSG_DEBUG("Size of selected Hashes" << selectedIdVect.size() );

  return StatusCode::SUCCESS;
} 

StatusCode Muon::MmRdoToPrepDataTool::decode( const std::vector<uint32_t>& robIds, 
					       const std::vector<IdentifierHash>& chamberHashInRobs )
{
  ATH_MSG_DEBUG("Size of the robIds" << robIds.size() );
  ATH_MSG_DEBUG("Size of the chamberHash" << chamberHashInRobs.size() );

  return StatusCode::SUCCESS;
}

StatusCode Muon::MmRdoToPrepDataTool::decode( const std::vector<uint32_t>& robIds )
{

  ATH_MSG_DEBUG("Size of the robIds" << robIds.size() );

  return StatusCode::SUCCESS;
}


// printout methods
void Muon::MmRdoToPrepDataTool::printInputRdo() {


  return;
}


void Muon::MmRdoToPrepDataTool::printPrepData() {


  return;
}

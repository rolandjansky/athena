/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// sTgcRdoToPrepDataTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "sTgcRdoToPrepDataTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"

#include "StoreGate/StoreGateSvc.h"

#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/MuonIdHelper.h"
#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"

// BS access
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

using namespace MuonGM;
using namespace Trk;
using namespace Muon;

Muon::sTgcRdoToPrepDataTool::sTgcRdoToPrepDataTool(const std::string& t,
						   const std::string& n,
						   const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_muonMgr(0),
  m_stgcIdHelper(0),
  m_muonIdHelper(0),
  m_fullEventDone(false),
  m_stgcPrepDataContainer(0)
{
  declareInterface<Muon::IMuonRdoToPrepDataTool>(this);
  
  //  template for property decalration
  declareProperty("OutputCollection",    m_stgcPrepDataContainerKey = std::string("sTGC_Measurements"),
		  "Muon::sTgcPrepDataContainer to record");
  
}


Muon::sTgcRdoToPrepDataTool::~sTgcRdoToPrepDataTool()
{

}

StatusCode Muon::sTgcRdoToPrepDataTool::initialize()
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
  
  m_stgcIdHelper = m_muonMgr->stgcIdHelper();

  // check if the initialization of the data container is success
  ATH_CHECK(m_stgcPrepDataContainerKey.initialize());


  ATH_MSG_INFO("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Muon::sTgcRdoToPrepDataTool::finalize()
{
  if (0 != m_stgcPrepDataContainer) m_stgcPrepDataContainer->release();
  return StatusCode::SUCCESS;

}

StatusCode Muon::sTgcRdoToPrepDataTool::processCollection(const STGC_RawDataCollection *rdoColl, 
							  std::vector<IdentifierHash>& idWithDataVect)
{
  ATH_MSG_DEBUG(" ***************** Start of process STGC Collection");

  const IdentifierHash hash = rdoColl->identifyHash();

  sTgcPrepDataCollection* prdColl = nullptr;
  
  // check if the collection already exists, otherwise add it
  if ( m_stgcPrepDataContainer->indexFind(hash) != m_stgcPrepDataContainer->end() ) {

    ATH_MSG_DEBUG("In processCollection: collection already contained in the MM PrepData container");
    return StatusCode::FAILURE;
  } 
  else {
    prdColl = new sTgcPrepDataCollection(hash);
    idWithDataVect.push_back(hash);

    if (StatusCode::SUCCESS != m_stgcPrepDataContainer->addCollection(prdColl, hash)) {
      ATH_MSG_DEBUG("In processCollection - Couldn't record in the Container MM Collection with hashID = "
		    << (int)hash );
      return StatusCode::FAILURE;
    }

  }

  // convert the RDO collection to a PRD collection
  STGC_RawDataCollection::const_iterator it = rdoColl->begin();
  for ( ; it != rdoColl->end() ; ++it ) {

    ATH_MSG_DEBUG("Adding a new sTgc PrepRawData");

    const STGC_RawData* rdo = *it;
    const Identifier rdoId = rdo->identify();
    const Identifier elementId = m_stgcIdHelper->elementID(rdoId);
    std::vector<Identifier> rdoList;
    rdoList.push_back(rdoId);
    
    // get the local and global positions
    const MuonGM::sTgcReadoutElement* detEl = m_muonMgr->getsTgcReadoutElement(elementId);
    Amg::Vector2D localPos;

    bool getLocalPos = detEl->stripPosition(rdoId,localPos);
    if ( !getLocalPos ) {
      ATH_MSG_ERROR("Could not get the local strip position for sTgc");
      return StatusCode::FAILURE;
    } 

    // get the resolution from strip width
    const int gasGap  = m_stgcIdHelper->gasGap(rdoId);
    const int channel = m_stgcIdHelper->channel(rdoId);

    // to be fixed: for now do not set the resolution, it will be added in the 
    // next update
    double width = 0.;

    int channelType = m_stgcIdHelper->channelType(rdoId);

    ATH_MSG_DEBUG("Adding a new STGC PRD, gasGap: " << gasGap << " channel: " << channel 
		  << " type: " << channelType );

    double resolution = width/sqrt(12.); 

    Amg::MatrixX* cov = new Amg::MatrixX(1,1);
    cov->setIdentity();
    (*cov)(0,0) = resolution*resolution;  

    sTgcPrepData* stgcPrd = new sTgcPrepData(rdoId,hash,localPos,
					     rdoList,cov,detEl);

    prdColl->push_back(stgcPrd);

  }



  return StatusCode::SUCCESS;
}


Muon::sTgcRdoToPrepDataTool::SetupSTGC_PrepDataContainerStatus Muon::sTgcRdoToPrepDataTool::setupSTGC_PrepDataContainer() 
{

  if(!evtStore()->contains<Muon::sTgcPrepDataContainer>(m_stgcPrepDataContainerKey.key())){    
    m_fullEventDone=false;
    
    SG::WriteHandle< Muon::sTgcPrepDataContainer > handle(m_stgcPrepDataContainerKey);
    StatusCode status = handle.record(std::make_unique<Muon::sTgcPrepDataContainer>(m_stgcIdHelper->module_hash_max()));
    
    if (status.isFailure() || !handle.isValid() )   {
      ATH_MSG_FATAL("Could not record container of STGC PrepData Container at " << m_stgcPrepDataContainerKey.key()); 
      return FAILED;
    }
    m_stgcPrepDataContainer = handle.ptr();
    return ADDED;
  }
  return ALREADYCONTAINED;
}


const STGC_RawDataContainer* Muon::sTgcRdoToPrepDataTool::getRdoContainer() {

  auto rdoContainerHandle  = SG::makeHandle(m_rdoContainerKey);
  if(rdoContainerHandle.isValid()) {
    ATH_MSG_DEBUG("STGC_getRdoContainer success");
    return rdoContainerHandle.cptr();  
  }
  ATH_MSG_WARNING("Retrieval of STGC_RawDataContainer failed !");

  return nullptr;
}


void Muon::sTgcRdoToPrepDataTool::processRDOContainer( std::vector<IdentifierHash>& idWithDataVect ) 
{

  ATH_MSG_DEBUG("In processRDOContainer");
  const STGC_RawDataContainer* rdoContainer = getRdoContainer();
  if (!rdoContainer) {
    return;
  }
  
  // run in unseeded mode
  for (STGC_RawDataContainer::const_iterator it = rdoContainer->begin(); it != rdoContainer->end(); ++it ) {
    
    auto rdoColl = *it;
    if (rdoColl->empty()) continue;
    ATH_MSG_DEBUG("New RDO collection with " << rdoColl->size() << "MM Hits");
    if(processCollection(rdoColl, idWithDataVect).isFailure()) {
      ATH_MSG_DEBUG("processCsm returns a bad StatusCode - keep going for new data collections in this event");
    }
  } 
  
  
  return;
}


// methods for ROB-based decoding
StatusCode Muon::sTgcRdoToPrepDataTool::decode( std::vector<IdentifierHash>& idVect, 
						std::vector<IdentifierHash>& idWithDataVect )
{

  ATH_MSG_DEBUG("Size of the input hash id vector: " << idVect.size());

  // clear the output vector of selected data
  idWithDataVect.clear();
  
  SetupSTGC_PrepDataContainerStatus containerRecordStatus = setupSTGC_PrepDataContainer();

  if ( containerRecordStatus == FAILED ) {
    return StatusCode::FAILURE;
  } 

  // check if the full event has already been decoded
  if ( m_fullEventDone ) {
    ATH_MSG_DEBUG ("Full event dcoded, nothing to do");
    return StatusCode::SUCCESS;
  } 

 
  return StatusCode::SUCCESS;
} 

StatusCode Muon::sTgcRdoToPrepDataTool::decode( const std::vector<uint32_t>& robIds )
{

  ATH_MSG_DEBUG("Size of the robIds" << robIds.size() );

  return StatusCode::SUCCESS;
}

// printout methods
void Muon::sTgcRdoToPrepDataTool::printInputRdo() {


  return;
}


void Muon::sTgcRdoToPrepDataTool::printPrepData() {


  return;
}

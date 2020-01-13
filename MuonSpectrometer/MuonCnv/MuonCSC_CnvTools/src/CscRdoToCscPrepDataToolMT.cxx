/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// Author: Ketevi A. Assamagan, Woochun Park
/// BNL, April 03 2005

/// algorithm to decode RDO into PrepRawData

#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonRDO/CscRawData.h"
#include "MuonRDO/CscRawDataCollection.h"
#include "MuonRDO/CscRawDataContainer.h"

#include "CscRdoToCscPrepDataToolMT.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include "EventPrimitives/EventPrimitives.h" 

#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "CscCalibTools/ICscCalibTool.h"
#include "MuonCSC_CnvTools/ICSC_RDO_Decoder.h"

#include "GaudiKernel/ThreadLocalContext.h"

using namespace MuonGM;
using namespace Trk;
using namespace Muon;


CscRdoToCscPrepDataToolMT::CscRdoToCscPrepDataToolMT
(const std::string& type, const std::string& name, const IInterface* parent)
  : CscRdoToCscPrepDataToolCore(type, name, parent) {
    declareProperty("CscRdoContainterCacheKey", m_prdContainerCacheKey, "Optional external cache for the CSC RDO container");
}  

CscRdoToCscPrepDataToolMT::~CscRdoToCscPrepDataToolMT(){}

StatusCode CscRdoToCscPrepDataToolMT::initialize(){
  ATH_MSG_VERBOSE("Starting init");
  ATH_CHECK( CscRdoToCscPrepDataToolCore::initialize() );
  ATH_CHECK( m_prdContainerCacheKey.initialize( !m_prdContainerCacheKey.key().empty() ) );
  ATH_MSG_DEBUG("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode CscRdoToCscPrepDataToolMT::finalize() {
  return CscRdoToCscPrepDataToolCore::finalize();
}


StatusCode CscRdoToCscPrepDataToolMT::decode(std::vector<IdentifierHash>& givenIdhs, std::vector<IdentifierHash>& decodedIdhs) {
  // WARNING : Trigger Part is not finished.
  unsigned int sizeVectorRequested = givenIdhs.size();
  ATH_MSG_DEBUG ( "decode for " << sizeVectorRequested << " offline collections called" );

  // clear output vector of selected data collections containing data
  decodedIdhs.clear();

  /// Recording the PRD container in StoreGate
  SG::WriteHandle< Muon::CscStripPrepDataContainer > outputHandle (m_outputCollectionKey);

  // Caching of PRD container
  const bool externalCachePRD = !m_prdContainerCacheKey.key().empty();
  if (!externalCachePRD) {
    // without the cache we just record the container
    StatusCode status = outputHandle.record(std::make_unique<Muon::CscStripPrepDataContainer>(m_muonMgr->cscIdHelper()->module_hash_max()));
    if (status.isFailure() || !outputHandle.isValid() )   {
      ATH_MSG_FATAL("Could not record container of CSC PrepData Container at " << m_outputCollectionKey.key());
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Created container " << m_outputCollectionKey.key());
  } 
  else {
    // use the cache to get the container
    SG::UpdateHandle<CscStripPrepDataCollection_Cache> update(m_prdContainerCacheKey);
    if (!update.isValid()){
      ATH_MSG_FATAL("Invalid UpdateHandle " << m_prdContainerCacheKey.key());
      return StatusCode::FAILURE;
    }
    StatusCode status = outputHandle.record(std::make_unique<Muon::CscStripPrepDataContainer>(update.ptr()));
    if (status.isFailure() || !outputHandle.isValid() )   {
      ATH_MSG_FATAL("Could not record container of CSC PrepData Container using cache " 
        << m_prdContainerCacheKey.key() << " - " <<m_outputCollectionKey.key()); 
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Created container using cache for " << m_prdContainerCacheKey.key());
  }
  // Pass the container from the handle
  m_outputCollection = outputHandle.ptr();


  if (sizeVectorRequested == 0) {
    m_fullEventDone=true;
    ATH_MSG_DEBUG ( "decoding the entire event " );
  } else {
    m_fullEventDone=false;
  }

  // retrieve the pointer to the RDO container
  // this will just get the pointer from memory if the container is already recorded in SG 
  // or 
  // will activate the TP converter for reading from pool root the RDO container and recording it in SG

  auto rdoContainerHandle = SG::makeHandle(m_rdoContainerKey);
  if (!rdoContainerHandle.isValid()) {
    ATH_MSG_WARNING ( "No CSC RDO container in StoreGate!" );
    return StatusCode::SUCCESS;
  }	
  const CscRawDataContainer* rdoContainer = rdoContainerHandle.cptr();

  ATH_MSG_DEBUG ( "Retrieved " << rdoContainer->size() << " CSC RDOs." );
  // here the RDO container is in SG and its pointer rdoContainer is initialised 
  // decoding 
  if (sizeVectorRequested)  {
    // seeded decoding
    for (unsigned int i=0; i<sizeVectorRequested; ++i) {
      if (decode(rdoContainer, givenIdhs[i],decodedIdhs).isFailure()) {
        ATH_MSG_ERROR ( "Unable to decode CSC RDO " << i << "th into CSC PrepRawData" );
        return StatusCode::FAILURE;
      }
    }
  } else {
    // unseeded decoding
    if (decode(rdoContainer, decodedIdhs).isFailure()) {
      ATH_MSG_ERROR ( "Unable to decode CSC RDO " );
      return StatusCode::FAILURE;
    }	
  }
  
  return StatusCode::SUCCESS;
}  

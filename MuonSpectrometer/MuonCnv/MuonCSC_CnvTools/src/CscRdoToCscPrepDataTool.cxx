/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// Author: Ketevi A. Assamagan, Woochun Park
/// BNL, April 03 2005

/// algorithm to decode RDO into PrepRawData

#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonRDO/CscRawData.h"
#include "MuonRDO/CscRawDataCollection.h"
#include "MuonRDO/CscRawDataContainer.h"

#include "CscRdoToCscPrepDataTool.h"
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


CscRdoToCscPrepDataTool::CscRdoToCscPrepDataTool
(const std::string& type, const std::string& name, const IInterface* parent)
  : CscRdoToCscPrepDataToolCore(type, name, parent) {
}  

StatusCode CscRdoToCscPrepDataTool::initialize(){
  ATH_MSG_VERBOSE("Starting init");
  ATH_CHECK( CscRdoToCscPrepDataToolCore::initialize() );
  ATH_MSG_DEBUG("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode CscRdoToCscPrepDataTool::decode(std::vector<IdentifierHash>& givenIdhs, std::vector<IdentifierHash>& decodedIdhs) {
  // WARNING : Trigger Part is not finished.
  unsigned int sizeVectorRequested = givenIdhs.size();
  ATH_MSG_DEBUG ( "decode for " << sizeVectorRequested << " offline collections called" );

  // clear output vector of selected data collections containing data
  decodedIdhs.clear();

  m_outputCollection=nullptr;
  if (!evtStore()->contains<Muon::CscStripPrepDataContainer>(m_outputCollectionKey.key())) {    
    /// record the container in storeGate
    SG::WriteHandle< Muon::CscStripPrepDataContainer > outputHandle (m_outputCollectionKey);
    ATH_CHECK(outputHandle.record(std::make_unique<Muon::CscStripPrepDataContainer>(m_idHelperSvc->cscIdHelper().module_hash_max())));

    if (!outputHandle.isValid()) {
      ATH_MSG_FATAL("Could not record container of CSC PrepData Container at " << m_outputCollectionKey.key());
      return StatusCode::FAILURE;
    } 
    m_outputCollection = outputHandle.ptr();

    if (sizeVectorRequested == 0) {
      m_fullEventDone=true;
      ATH_MSG_DEBUG ( "decoding the entire event " );
    } else {
      m_fullEventDone=false;
    }
  
    
  } else {

    ATH_MSG_DEBUG ("CSC PrepData Container is already in StoreGate ");
    const Muon::CscStripPrepDataContainer* outputCollection_c = nullptr;
    ATH_CHECK(evtStore()->retrieve (outputCollection_c, m_outputCollectionKey.key()).isFailure());

    m_outputCollection = const_cast<Muon::CscStripPrepDataContainer*> (outputCollection_c);
    if (m_fullEventDone) {
      ATH_MSG_DEBUG("Whole event has already been decoded; nothing to do");
      return StatusCode::SUCCESS;
    }

    if (sizeVectorRequested == 0) m_fullEventDone=true;
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

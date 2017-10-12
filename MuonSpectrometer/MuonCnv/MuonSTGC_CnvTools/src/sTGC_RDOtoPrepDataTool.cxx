/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TGC_CnvTool.cxx, (c) ATLAS Detector software
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "sTGC_RDOtoPrepDataTool.h"

#include "GaudiKernel/ISvcLocator.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"

#include "TrkSurfaces/Surface.h"

#include "MuonCnvToolInterfaces/IDC_Helper.h"

#include "EventPrimitives/EventPrimitives.h"
// BS access
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

#include <cfloat>
#include <algorithm>

//================ Constructor =================================================

Muon::sTGC_RDOtoPrepDataTool::sTGC_RDOtoPrepDataTool(const std::string& t, const std::string& n, const IInterface* p)
  : AthAlgTool(t, n, p), 
    m_muonMgr(0),
    m_sTGC_Helper(0),
    m_rawDataProviderTool("Muon::sTGC_RawDataProviderTool/sTGC_RawDataProviderTool"),
    m_RDO_Key("sTGCRDO"),
    m_PRD_Key("sTGC_Measurements")
{
  declareProperty("RawDataProviderTool",  m_rawDataProviderTool);
  declareProperty("RDOContainer",         m_RDO_Key);
  declareProperty("OutputCollection",     m_PRD_Key);
}  

//================ Destructor =================================================

Muon::sTGC_RDOtoPrepDataTool::~sTGC_RDOtoPrepDataTool()
{}

//___________________________________________________________________________
StatusCode Muon::sTGC_RDOtoPrepDataTool::queryInterface(const InterfaceID& riid, void** ppvIf)
{
  if(riid==IMuonRdoToPrepDataTool::interfaceID()) {
    *ppvIf = (IMuonRdoToPrepDataTool*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  
  return AthAlgTool::queryInterface(riid, ppvIf);
}


//================ Initialization =================================================

StatusCode Muon::sTGC_RDOtoPrepDataTool::initialize()
{
  StatusCode sc = AthAlgTool::initialize();
  if(sc.isFailure()) return sc;

  sc = detStore()->retrieve(m_muonMgr);
  if(sc.isFailure()) {
    ATH_MSG_FATAL("Cannot retrieve MuonDetectorManager");
    return sc;
  }

  /// get tgcIdHelper from muonMgr
  m_sTGC_Helper = m_muonMgr->tgcIdHelper();
    
  ATH_CHECK(m_RDO_Key.initialize());
  ATH_CHECK(m_PRD_Key.initialize());
  
  ATH_MSG_INFO("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

//================ Finalization =================================================

StatusCode Muon::sTGC_RDOtoPrepDataTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}

//================ Decoding =================================================
StatusCode Muon::sTGC_RDOtoPrepDataTool::decode(std::vector<IdentifierHash>& requestedIdHashVect, 
					      std::vector<IdentifierHash>& selectedIdHashVect)
{
  int sizeVectorRequested = requestedIdHashVect.size();
  ATH_MSG_DEBUG("decode for " << sizeVectorRequested << " offline collections called");

  // clear output vector of selected data collections containing data 
  selectedIdHashVect.clear(); 
  
  if(!evtStore()->contains<Muon::sTgcPrepDataContainer>( m_PRD_Key )) {
    // initialize with false  
    // std::fill(m_decodedOnlineId.begin(), m_decodedOnlineId.end(), false);
    SG::WriteHandle<sTgcPrepDataContainer>  handle(m_PRD_Key);
    
    // record the container in storeGate
    auto prds = std::make_unique<sTgcPrepDataContainer> (m_sTGC_Helper->module_hash_max());
    ATH_CHECK( handle.record (std::move (prds)) );
    
    // cache the pointer, storegate retains ownership
    sTgcPrepDataContainer* stgcPrepDataContainer = handle.ptr();
  } 

  // seeded or unseeded decoding
  if (sizeVectorRequested != 0) {
    processPRDHashes(idVect,idWithDataVect, *stgcPrepDataContainer);
  }  else { 
    processRDOContainer(idWithDataVect, *stgcPrepDataContainer);
  }

  return StatusCode::SUCCESS;
}

void Muon::MdtRdoToPrepDataTool::processPRDHashes( const std::vector<IdentifierHash>& chamberHashInRobs, std::vector<IdentifierHash>& idWithDataVect, sTgcPrepDataContainer& prds ){
  
  // get RDO container
  
  SG::ReadHandle<sTGC_RawDataContainer> rdos(m_RDO_Key);
  const sTGC_RawDataContainer* rdoContainer = *rdos;
    
  if(!rdoContainer) {
    return;
  }                 

  for( auto it = chamberHashInRobs.begin(); it != chamberHashInRobs.end(); ++it ){
    if(!handlePRDHash(*it,*rdoContainer,idWithDataVect, prds) ) {
      ATH_MSG_DEBUG("Failed to process hash " << *it );
    }
  }//ends loop over chamberhash  
}

bool Muon::MdtRdoToPrepDataTool::handlePRDHash( IdentifierHash hash, const sTGC_RawDataContainer& rdoContainer, std::vector<IdentifierHash>& idWithDataVect, sTgcPrepDataContainer& prds ) {
  
  // if in prep data the chamber already exists ... do nothing
  if( m_mdtPrepDataContainer->indexFind(hash) != m_mdtPrepDataContainer->end() ) return true;
  
  IdentifierHash rdoHash = hash; // before BMEs were installed, RDOs were indexed by offline hashes (same as PRD)
  // process CSM if data was found
  sTGC_RawDataContainer::const_iterator rdoColli = rdoContainer.indexFind(rdoHash);
  if( rdoColli != rdoContainer.end() ) {
    rdoColli->push_back(new sTGC_RawData() );
  } else {
    ATH_MSG_DEBUG("handlePRDHash: hash id " << (unsigned int)(hash) << " not found in RDO container");
  }
  return true;
}


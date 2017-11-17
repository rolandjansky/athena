/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// STGC_RawDataProviderTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "STGC_RawDataProviderTool.h"
#include "MuonRDO/STGC_RawDataContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"


//================ Constructor =================================================

Muon::STGC_RawDataProviderTool::STGC_RawDataProviderTool(
						       const std::string& t,
						       const std::string& n,
						       const IInterface*  p) :
  AthAlgTool(t, n, p),
  m_muonMgr(0)
{
  declareInterface<IMuonRawDataProviderTool>(this);
}

//================ Destructor =================================================

Muon::STGC_RawDataProviderTool::~STGC_RawDataProviderTool()
{}

//================ Initialisation =================================================

StatusCode Muon::STGC_RawDataProviderTool::initialize()
{
  StatusCode sc = AthAlgTool::initialize();

  if(detStore()->retrieve(m_muonMgr).isFailure()) {
    ATH_MSG_WARNING( "Cannot retrieve MuonDetectorManager" );
    return StatusCode::SUCCESS;
  }
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Muon::STGC_RawDataProviderTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}

//============================================================================================

StatusCode Muon::STGC_RawDataProviderTool::convert(const ROBFragmentList& ) 
{    
  ATH_MSG_ERROR("STGC_RawDataProviderTool::convert(const ROBFragmentList& vecRobs) Not yet implemented.");
  
  return StatusCode::FAILURE;
}

StatusCode  Muon::STGC_RawDataProviderTool::convert(const ROBFragmentList& vecRobs, 
						   const std::vector<IdentifierHash>&) 
{
  return convert(vecRobs);
}

StatusCode  Muon::STGC_RawDataProviderTool::convert()
{
  ATH_MSG_ERROR("STGC_RawDataProviderTool::convert() Not yet implemented.");
  return StatusCode::FAILURE;
}

StatusCode  Muon::STGC_RawDataProviderTool::convert(const std::vector<IdentifierHash>& )
{
  ATH_MSG_ERROR("STGC_RawDataProviderTool::convert(const std::vector<IdentifierHash>& rdoIdhVect) Not yet implemented.");
  return StatusCode::FAILURE;
}

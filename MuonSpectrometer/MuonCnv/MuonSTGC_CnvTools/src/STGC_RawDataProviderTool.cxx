/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
  base_class(t, n, p)
{
}

//================ Destructor =================================================

Muon::STGC_RawDataProviderTool::~STGC_RawDataProviderTool()
{}

//================ Initialisation =================================================

StatusCode Muon::STGC_RawDataProviderTool::initialize()
{
  ATH_CHECK( AthAlgTool::initialize() );

  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Muon::STGC_RawDataProviderTool::finalize()
{
  ATH_CHECK( AthAlgTool::finalize() );
  return StatusCode::SUCCESS;
}

//============================================================================================

StatusCode Muon::STGC_RawDataProviderTool::convert(const ROBFragmentList& ) const
{    
  ATH_MSG_ERROR("STGC_RawDataProviderTool::convert(const ROBFragmentList& vecRobs) Not yet implemented.");
  
  return StatusCode::FAILURE;
}

StatusCode  Muon::STGC_RawDataProviderTool::convert(const ROBFragmentList& vecRobs, 
						   const std::vector<IdentifierHash>&) const
{
  return convert(vecRobs);
}

StatusCode  Muon::STGC_RawDataProviderTool::convert() const
{
  ATH_MSG_ERROR("STGC_RawDataProviderTool::convert() Not yet implemented.");
  return StatusCode::FAILURE;
}

StatusCode  Muon::STGC_RawDataProviderTool::convert(const std::vector<IdentifierHash>& ) const
{
  ATH_MSG_ERROR("STGC_RawDataProviderTool::convert(const std::vector<IdentifierHash>& rdoIdhVect) Not yet implemented.");
  return StatusCode::FAILURE;
}

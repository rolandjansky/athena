/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonPatternSegmentAssociationTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MuonAssociationTools/MuonPatternSegmentAssociationTool.h"
#include "MuonSegment/MuonSegmentCombination.h" // Not sure these are actually needed. EJWM
#include "MuonPattern/MuonPatternCombination.h"

//================ Constructor =================================================

Muon::MuonPatternSegmentAssociationTool::MuonPatternSegmentAssociationTool(const std::string& t,
              const std::string& n,
              const IInterface*  p )
  :
  AthAlgTool(t,n,p)
{
  declareInterface<IMuonPatternSegmentAssociationTool>(this);

  //  template for property decalration
  //declareProperty("PropertyName", m_propertyName);
}

//================ Destructor =================================================

Muon::MuonPatternSegmentAssociationTool::~MuonPatternSegmentAssociationTool()
{}


//================ Initialisation =================================================

StatusCode Muon::MuonPatternSegmentAssociationTool::initialize()
{
  
  ATH_CHECK( AthAlgTool::initialize() );

  ATH_MSG_INFO("initialize() successful in " << name() );
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Muon::MuonPatternSegmentAssociationTool::finalize()
{
  ATH_CHECK( AthAlgTool::finalize() );
  return StatusCode::SUCCESS;
}

//============================================================================================


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
  AlgTool(t,n,p),
  m_log(msgSvc(),n)
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
  
  StatusCode sc = AlgTool::initialize();
  m_log.setLevel(outputLevel());
  if (sc.isFailure()) return sc;

  m_log << MSG::INFO << "initialize() successful in " << name() << endreq;
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Muon::MuonPatternSegmentAssociationTool::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
}

//============================================================================================


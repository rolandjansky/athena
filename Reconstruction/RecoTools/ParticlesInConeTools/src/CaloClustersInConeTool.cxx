/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// CaloClustersInConeTool
//
//  (c) ATLAS software
//////////////////////////////////////////////////////////////////////////////

#include "CaloClustersInConeTool.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

namespace xAOD {
 
  CaloClustersInConeTool::CaloClustersInConeTool (const std::string& type, const std::string& name, const IInterface* parent)
    :	AthAlgTool(type, name, parent)
  {
    declareInterface<ICaloClustersInConeTool>(this);
  }

  CaloClustersInConeTool::~CaloClustersInConeTool()
  {}

  StatusCode CaloClustersInConeTool::initialize() {
    ATH_CHECK(m_caloClusterLocation.initialize());
    return StatusCode::SUCCESS;
  }

  StatusCode CaloClustersInConeTool::finalize() {
    return StatusCode::SUCCESS;
  }

  const CaloClustersInConeTool::LookUpTable*
  CaloClustersInConeTool::getTable() const
  {
    const std::string tableName = name() + "LookUpTable";
    SG::ReadHandle<LookUpTable> rh (tableName);
    if (rh.isValid())
      return &*rh;

    
    SG::ReadHandle<CaloClusterContainer> caloClusters(m_caloClusterLocation);
    if (!caloClusters.isValid()) {
      ATH_MSG_ERROR("Was not able to access the calo cluster container " << m_caloClusterLocation.key());
      return nullptr;
    }
    auto lut = std::make_unique<LookUpTable>();
    lut->init(*caloClusters);
    SG::WriteHandle<LookUpTable> wh (tableName);
    return wh.put (std::move (lut), true);
  }

  bool CaloClustersInConeTool::particlesInCone( float eta, float phi, float dr, std::vector< const CaloCluster*>& output ) const {
    /// initialize if needed
    const LookUpTable* lut = getTable();

    return lut->iParticlesInCone( eta, phi, dr, output );
  }

  bool CaloClustersInConeTool::particlesInCone( float eta, float phi, float dr, std::vector< ElementLink<CaloClusterContainer> >& output ) const {
    /// initialize if needed
    const LookUpTable* lut = getTable();
    
    return lut->iParticlesInCone( eta, phi, dr, output );
  }
}	// end of namespace

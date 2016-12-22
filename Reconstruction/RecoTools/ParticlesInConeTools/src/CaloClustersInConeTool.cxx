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
    declareProperty("CaloClusterLocation",m_caloClusterLocation = "CaloCalTopoClusters");
  }

  CaloClustersInConeTool::~CaloClustersInConeTool()
  {}

  StatusCode CaloClustersInConeTool::initialize() {
    return StatusCode::SUCCESS;
  }

  StatusCode CaloClustersInConeTool::finalize() {
    return StatusCode::SUCCESS;
  }

  const CaloClusterContainer* CaloClustersInConeTool::retrieveCaloClusterContainer() const {
    // retrieve MuonSpectrometer tracks
    const CaloClusterContainer* caloClusters = 0;
    if(evtStore()->contains<CaloClusterContainer>(m_caloClusterLocation)) {
      if(evtStore()->retrieve(caloClusters,m_caloClusterLocation).isFailure()) {
        ATH_MSG_FATAL( "Unable to retrieve " << m_caloClusterLocation );
        return 0;
      }else{
        ATH_MSG_VERBOSE("retrieved CaloClusterContainer with name " << m_caloClusterLocation << " size " << caloClusters->size());
      
      }
    }else{
      ATH_MSG_WARNING("CaloClusterContainer with name " << m_caloClusterLocation << " not in StoreGate");
    }
    return caloClusters;
  }

  const CaloClustersInConeTool::LookUpTable*
  CaloClustersInConeTool::getTable() const
  {
    const std::string tableName = name() + "LookUpTable";
    SG::ReadHandle<LookUpTable> rh (tableName);
    if (rh.isValid())
      return &*rh;

    const CaloClusterContainer* caloClusters = retrieveCaloClusterContainer();
    if( !caloClusters ) return nullptr;
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

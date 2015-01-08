/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// CaloClustersInConeTool
//
//  (c) ATLAS software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>
#include "CaloClustersInConeTool.h"

namespace xAOD {
 
  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

  CaloClustersInConeTool::CaloClustersInConeTool (const std::string& type, const std::string& name, const IInterface* parent)
    :	AthAlgTool(type, name, parent),
        m_incidentSvc("IncidentSvc",name)
  {
    declareInterface<ICaloClustersInConeTool>(this);
    declareProperty("CaloClusterLocation",m_caloClusterLocation = "CaloCalTopoClusters");
  }

  CaloClustersInConeTool::~CaloClustersInConeTool()
  {}

  //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

  StatusCode CaloClustersInConeTool::initialize() {
     // call handle in case of EndEvent
    ATH_CHECK(m_incidentSvc.retrieve());
    m_incidentSvc->addListener( this, IncidentType::EndEvent );

    return StatusCode::SUCCESS;
  }

  StatusCode CaloClustersInConeTool::finalize() {
    return StatusCode::SUCCESS;
  }

  void CaloClustersInConeTool::handle(const Incident& inc) {
    // Only clear cache for EndEvent incident
    if (inc.type()  == IncidentType::EndEvent){
      ATH_MSG_DEBUG(" clearing cache at end of event " );
      m_lookUpTable.clear();
    }  
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

  bool CaloClustersInConeTool::initLookUp() {
    if( !m_lookUpTable.isInitialized() ){
      /// retrieve track particles
      const CaloClusterContainer* caloClusters = retrieveCaloClusterContainer();
      if( !caloClusters ) return false;
      m_lookUpTable.init(*caloClusters);
    }
    return true;
  }

  bool CaloClustersInConeTool::particlesInCone( float eta, float phi, float dr, std::vector< const CaloCluster*>& output ) {
    /// initialize if needed
    if(!initLookUp() ) return false;

    return m_lookUpTable.iParticlesInCone( eta, phi, dr, output );
  }

  bool CaloClustersInConeTool::particlesInCone( float eta, float phi, float dr, std::vector< ElementLink<CaloClusterContainer> >& output ) {
    /// initialize if needed
    if(!initLookUp() ) return false;
    
    return m_lookUpTable.iParticlesInCone( eta, phi, dr, output );
  }
}	// end of namespace

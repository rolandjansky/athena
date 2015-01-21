/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// PFlowObjectsInConeTool
//
//  (c) ATLAS software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>
#include "PFlowObjectsInConeTool.h"

#include "PFlowUtils/PFODefs.h"
#include "PFlowUtils/IRetrievePFOTool.h"

namespace xAOD {
 
  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

  PFlowObjectsInConeTool::PFlowObjectsInConeTool (const std::string& type, const std::string& name, const IInterface* parent)
    :	AthAlgTool(type, name, parent),
        m_pflowObjects(nullptr),
        m_incidentSvc("IncidentSvc",name)
  {
    declareInterface<IPFlowObjectsInConeTool>(this);
    declareProperty("RetrievePFOTool", m_retrievePFOTool, "Name of tool that builds the PFO collection.");
    declareProperty("PFlowType",       m_PFlowType    = "Neutral", "Type of PFlow objects (neutral, charged, both)");
    declareProperty("NeutralScale",    m_NeutralScale = "EM",      "Scale of neutral PFlow objects (EM or LC)");
  }

  PFlowObjectsInConeTool::~PFlowObjectsInConeTool()
  {}

  //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

  StatusCode PFlowObjectsInConeTool::initialize() {
     // call handle in case of EndEvent
    ATH_CHECK(m_incidentSvc.retrieve());
    m_incidentSvc->addListener( this, IncidentType::EndEvent );

    return StatusCode::SUCCESS;
  }

  StatusCode PFlowObjectsInConeTool::finalize() {
    return StatusCode::SUCCESS;
  }

  void PFlowObjectsInConeTool::handle(const Incident& inc) {
    // Only clear cache for EndEvent incident
    if (inc.type()  == IncidentType::EndEvent){
      ATH_MSG_DEBUG(" clearing cache at end of event " );
      m_lookUpTable.clear();
      delete m_pflowObjects;
      m_pflowObjects = 0;
    }  
  }

  const PFOContainer* PFlowObjectsInConeTool::retrievePFOContainer() const {
    CP::PFO_JetMETConfig_inputScale inscale  = m_NeutralScale == "EM" ? CP::EM : CP::LC;
    CP::PFO_JetMETConfig_charge     incharge = m_PFlowType == "All" ? CP::all : (m_PFlowType == "Neutral" ? CP::neutral : CP::charged);
    const PFOContainer* pflowObjects = m_retrievePFOTool->retrievePFO(inscale,incharge);
    if (pflowObjects == 0) {
      ATH_MSG_FATAL( "Unable to retrieve PFlow container");
      return 0;
    }
    if (pflowObjects->size() == 0) {
      ATH_MSG_INFO( "Empty PFlow container");
      return 0;
    }
    return pflowObjects;
  }

  bool PFlowObjectsInConeTool::particlesInCone( float eta, float phi, float dr, std::vector<const PFO*>& output ) {

    /// initialize if needed
    if( !m_lookUpTable.isInitialized() ){
      m_pflowObjects = retrievePFOContainer();
      if( !m_pflowObjects ) return false;
      m_lookUpTable.init(*m_pflowObjects);
    }
    return m_lookUpTable.iParticlesInCone( eta, phi, dr, output );
  }
}// end of namespace

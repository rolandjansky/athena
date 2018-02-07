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

namespace xAOD {
 
  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

  PFlowObjectsInConeTool::PFlowObjectsInConeTool (const std::string& type, const std::string& name, const IInterface* parent)
    :	AthAlgTool(type, name, parent)
  {
    declareInterface<IPFlowObjectsInConeTool>(this);
  }

  PFlowObjectsInConeTool::~PFlowObjectsInConeTool()
  {}

  //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

  StatusCode PFlowObjectsInConeTool::initialize() {
    ATH_CHECK(m_pfokey.initialize());
    return StatusCode::SUCCESS;
  }

  StatusCode PFlowObjectsInConeTool::finalize() {
    return StatusCode::SUCCESS;
  }

  const PFlowObjectsInConeTool::LookUpTable*
  PFlowObjectsInConeTool::getTable() const
  {
    const std::string tableName = name() + "LookUpTable";
    SG::ReadHandle<LookUpTable> rh (tableName);
    if (rh.isValid())
      return &*rh;

    
    SG::ReadHandle<PFOContainer> pfos(m_pfokey);
    if( !pfos.isValid() ) {
      ATH_MSG_WARNING("Unable to open pflow container with key " << m_pfokey.key());
      return nullptr;
    }
    auto lut = std::make_unique<LookUpTable>();
    lut->init(*pfos);
    SG::WriteHandle<LookUpTable> wh (tableName);
    return wh.put (std::move (lut), true);
  }

  bool PFlowObjectsInConeTool::particlesInCone( float eta, float phi, float dr, std::vector<const PFO*>& output ) const {

    /// initialize if needed
    const LookUpTable* lut = getTable();
    return lut->iParticlesInCone( eta, phi, dr, output );
  }
}// end of namespace

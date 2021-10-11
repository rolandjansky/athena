/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// FlowElementsInConeTool
//
//  (c) ATLAS software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>
#include "FlowElementsInConeTool.h"

namespace xAOD {
 
  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

  FlowElementsInConeTool::FlowElementsInConeTool (const std::string& type, const std::string& name, const IInterface* parent)
    :	AthAlgTool(type, name, parent)
  {
    declareInterface<IFlowElementsInConeTool>(this);
  }

  FlowElementsInConeTool::~FlowElementsInConeTool()
  {}

  //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

  StatusCode FlowElementsInConeTool::initialize() {
    ATH_CHECK(m_pfokey.initialize());
    return StatusCode::SUCCESS;
  }

  StatusCode FlowElementsInConeTool::finalize() {
    return StatusCode::SUCCESS;
  }

  const FlowElementsInConeTool::LookUpTable*
  FlowElementsInConeTool::getTable() const
  {
    const std::string tableName = name() + "LookUpTable";
    SG::ReadHandle<LookUpTable> rh (tableName);
    if (rh.isValid())
      return &*rh;

    
    SG::ReadHandle<FlowElementContainer> pfos(m_pfokey);
    if( !pfos.isValid() ) {
      ATH_MSG_WARNING("Unable to open pflow container with key " << m_pfokey.key());
      return nullptr;
    }
    auto lut = std::make_unique<LookUpTable>();
    lut->init(*pfos);
    SG::WriteHandle<LookUpTable> wh (tableName);
    return wh.put (std::move (lut), true);
  }

  bool FlowElementsInConeTool::particlesInCone( float eta, float phi, float dr, std::vector<const FlowElement*>& output ) const {

    /// initialize if needed
    const LookUpTable* lut = getTable();
    return lut->iParticlesInCone( eta, phi, dr, output );
  }
}// end of namespace

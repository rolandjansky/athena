/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLESINCONETOOLS_FLOWELEMENTSINCONETOOL_H
#define PARTICLESINCONETOOLS_FLOWELEMENTSINCONETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AsgTools/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"

#include "ParticlesInConeTools/IFlowElementsInConeTool.h"
#include "IParticlesLookUpTable.h"
#include "xAODPFlow/FlowElement.h"
#include "xAODPFlow/FlowElementContainer.h"
 
namespace xAOD {

  class FlowElementsInConeTool: public AthAlgTool, virtual public IFlowElementsInConeTool {
  public:
    /** constructor */
    FlowElementsInConeTool(const std::string& type, const std::string& name, const IInterface* parent);

    /** destructor */
    ~FlowElementsInConeTool(void); 
  
    /** initialize */
    StatusCode initialize() override;

    /** finalize */
    StatusCode finalize() override;

    /**IFlowElementsInConeTool interface */    
    bool particlesInCone( float eta, float phi, float dr, std::vector<const FlowElement*>& output ) const override;

    typedef IParticlesLookUpTable<FlowElement> LookUpTable;

  private:

    /** to retrieve pflow objects */

    SG::ReadHandleKey<FlowElementContainer> m_pfokey {this,
	"PFlowKey", "CHSNeutralParticleFlowObjects", 
	"StoreGate key for FlowElements"};
    
    // init look-up table
    const LookUpTable* getTable() const;
  };

}	// end of namespace

CLASS_DEF( xAOD::FlowElementsInConeTool::LookUpTable, 251336768, 1 )

#endif



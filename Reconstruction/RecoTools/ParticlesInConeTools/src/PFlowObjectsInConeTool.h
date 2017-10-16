/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLESINCONETOOLS_PFLOWOBJECTSINCONETOOL_H
#define PARTICLESINCONETOOLS_PFLOWOBJECTSINCONETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AsgTools/ToolHandle.h"

#include "ParticlesInConeTools/IPFlowObjectsInConeTool.h"
#include "IParticlesLookUpTable.h"
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/PFOContainer.h"
 
namespace xAOD {

  class PFlowObjectsInConeTool: public AthAlgTool, virtual public IPFlowObjectsInConeTool {
  public:
    /** constructor */
    PFlowObjectsInConeTool(const std::string& type, const std::string& name, const IInterface* parent);

    /** destructor */
    ~PFlowObjectsInConeTool(void); 
  
    /** initialize */
    StatusCode initialize() override;

    /** finalize */
    StatusCode finalize() override;

    /**IPFlowObjectsInConeTool interface */    
    bool particlesInCone( float eta, float phi, float dr, std::vector<const PFO*>& output ) const override;

    typedef IParticlesLookUpTable<PFO> LookUpTable;

  private:

    /** to retrieve pflow objects */

    std::string m_pfokey;
    const PFOContainer* retrievePFOContainer() const;
    
    // init look-up table
    const LookUpTable* getTable() const;
  };

}	// end of namespace

CLASS_DEF( xAOD::PFlowObjectsInConeTool::LookUpTable, 209511245, 1 )

#endif



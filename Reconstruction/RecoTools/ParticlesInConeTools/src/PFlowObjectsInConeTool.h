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
#include "GaudiKernel/IIncidentListener.h"
 
class IIncidentSvc;
namespace CP {
  class IRetrievePFOTool;
}
 
namespace xAOD {

  class PFlowObjectsInConeTool: public AthAlgTool, virtual public IPFlowObjectsInConeTool, virtual public IIncidentListener {
  public:
    /** constructor */
    PFlowObjectsInConeTool(const std::string& type, const std::string& name, const IInterface* parent);

    /** destructor */
    ~PFlowObjectsInConeTool(void); 
  
    /** initialize */
    StatusCode initialize();

    /** finalize */
    StatusCode finalize();

    /**IPFlowObjectsInConeTool interface */    
    bool particlesInCone( float eta, float phi, float dr, std::vector<const PFO*>& output );

    /** incident to clear cache at end of the event */
    void handle(const Incident& inc);

  private:

    /** to retrieve pflow objects */
    ToolHandle<CP::IRetrievePFOTool> m_retrievePFOTool; 
    std::string m_PFlowType;
    std::string m_NeutralScale;
    const PFOContainer* retrievePFOContainer() const;
    const PFOContainer* m_pflowObjects;
    
    /** look-up table */
    IParticlesLookUpTable<PFO> m_lookUpTable;

    /** incident service */
    ServiceHandle< IIncidentSvc >      m_incidentSvc;
  };

}	// end of namespace

#endif



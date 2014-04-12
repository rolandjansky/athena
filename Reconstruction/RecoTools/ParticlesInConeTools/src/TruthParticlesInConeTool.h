/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLESINCONETOOLS_TRUTHPARTICLESINCONETOOL_H
#define PARTICLESINCONETOOLS_TRUTHPARTICLESINCONETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "ParticlesInConeTools/ITruthParticlesInConeTool.h"
#include "IParticlesLookUpTable.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "GaudiKernel/IIncidentListener.h"
 
class IIncidentSvc;
 
namespace xAOD {

  class TruthParticlesInConeTool: public AthAlgTool, virtual public ITruthParticlesInConeTool, virtual public IIncidentListener {
  public:
    /** constructor */
    TruthParticlesInConeTool(const std::string& type, const std::string& name, const IInterface* parent);

    /** destructor */
    ~TruthParticlesInConeTool(void); 
  
    /** initialize */
    StatusCode initialize();

    /** finalize */
    StatusCode finalize();

    /**ITruthParticlesInConeTool interface */    
    bool particlesInCone( float eta, float phi, float dr, std::vector< const TruthParticle*>& output );

    /** incident to clear cache at end of the event */
    void handle(const Incident& inc);

  private:
    /** retrieve id track particles */
    const TruthParticleContainer* retrieveTruthParticleContainer() const; 

    /** ID track collection name */
    std::string m_truthParticleLocation;
    
    /** look-up table */
    IParticlesLookUpTable<TruthParticle> m_lookUpTable;

    /** incident service */
    ServiceHandle< IIncidentSvc >        m_incidentSvc;
  };

}	// end of namespace

#endif



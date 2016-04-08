/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLESINCONETOOLS_TRACKSINCONETOOL_H
#define PARTICLESINCONETOOLS_TRACKSINCONETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "ParticlesInConeTools/ITrackParticlesInConeTool.h"
#include "IParticlesLookUpTable.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "GaudiKernel/IIncidentListener.h"
 
class IIncidentSvc;
 
namespace xAOD {

  class TrackParticlesInConeTool: public AthAlgTool, virtual public ITrackParticlesInConeTool, virtual public IIncidentListener {
  public:
    /** constructor */
    TrackParticlesInConeTool(const std::string& type, const std::string& name, const IInterface* parent);

    /** destructor */
    ~TrackParticlesInConeTool(void); 
  
    /** initialize */
    StatusCode initialize();

    /** finalize */
    StatusCode finalize();

    /**ITrackParticlesInConeTool interface */    
    bool particlesInCone( float eta, float phi, float dr, std::vector< const TrackParticle*>& output );

    /** incident to clear cache at end of the event */
    void handle(const Incident& inc);

  private:
    /** retrieve id track particles */
    const TrackParticleContainer* retrieveTrackParticleContainer() const; 

    /** ID track collection name */
    std::string m_indetTrackParticleLocation;
    
    /** look-up table */
    IParticlesLookUpTable<TrackParticle> m_lookUpTable;

    /** incident service */
    ServiceHandle< IIncidentSvc >        m_incidentSvc;
  };

}	// end of namespace

#endif



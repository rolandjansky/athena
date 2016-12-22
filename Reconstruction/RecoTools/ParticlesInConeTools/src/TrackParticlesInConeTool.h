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
#include "SGTools/CLASS_DEF.h"
 
namespace xAOD {

  class TrackParticlesInConeTool: public AthAlgTool, virtual public ITrackParticlesInConeTool {
  public:
    /** constructor */
    TrackParticlesInConeTool(const std::string& type, const std::string& name, const IInterface* parent);

    /** destructor */
    virtual ~TrackParticlesInConeTool(void); 
  
    /** initialize */
    virtual StatusCode initialize() override;

    /** finalize */
    virtual StatusCode finalize() override;

    /**ITrackParticlesInConeTool interface */    
    virtual
    bool particlesInCone( float eta, float phi, float dr, std::vector< const TrackParticle*>& output ) const override;

    typedef IParticlesLookUpTable<TrackParticle> LookUpTable;

  private:
    // init look-up table
    const LookUpTable* getTable() const;

    /** retrieve id track particles */
    const TrackParticleContainer* retrieveTrackParticleContainer() const; 

    /** ID track collection name */
    std::string m_indetTrackParticleLocation;
  };

}	// end of namespace

CLASS_DEF( xAOD::TrackParticlesInConeTool::LookUpTable, 134433981, 0 )

#endif



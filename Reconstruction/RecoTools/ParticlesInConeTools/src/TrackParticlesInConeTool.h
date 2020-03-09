/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLESINCONETOOLS_TRACKSINCONETOOL_H
#define PARTICLESINCONETOOLS_TRACKSINCONETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/ReadHandleKey.h"

#include "ParticlesInConeTools/ITrackParticlesInConeTool.h"
#include "IParticlesLookUpTable.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "AthenaKernel/CLASS_DEF.h"
 
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

    /** ID track collection name */
    SG::ReadHandleKey<TrackParticleContainer> m_indetTrackParticleLocation {this,
	"TrackParticleLocation", "InDetTrackParticles"};
  };

}	// end of namespace

CLASS_DEF( xAOD::TrackParticlesInConeTool::LookUpTable, 134433981, 0 )

#endif



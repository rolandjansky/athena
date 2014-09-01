/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISOLATIONTOOL_TRACKISOLATIONTOOL_H
#define ISOLATIONTOOL_TRACKISOLATIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "IsolationTool/ITrackIsolationTool.h"
#include "ParticlesInConeTools/ITrackParticlesInConeTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODPrimitives/IsolationType.h"
#include <set>
#include <math.h>

namespace xAOD {

  class TrackIsolationTool: public AthAlgTool, virtual public ITrackIsolationTool {
  public:
    typedef std::vector< const TrackParticle* > TPVec;

    /// struct to cache inputs to simplify interfaces, for internal usage only
    struct TrackIsolationInput {
      TrackIsolationInput(const IParticle* particle_,SubtractionStrategy strategy_,const Vertex* vertex_,
                          const std::set<const xAOD::TrackParticle*>* exclusionSet_ ) :
        particle(particle_),strategy(strategy_),vertex(vertex_),exclusionSet(exclusionSet_) {}

      const IParticle* particle;     /// input IParticle
      std::vector<float>  cones;     /// cone sizes 
      float maxRadius;               /// maximum cone size
      SubtractionStrategy strategy;  /// subtraction strategy
      const Vertex* vertex;          /// vertex
      const std::set<const xAOD::TrackParticle*>* exclusionSet; /// tracks exclused in isolation
    };

  public:
    /** constructor */
    TrackIsolationTool(const std::string& type, const std::string& name, const IInterface* parent);

    /** destructor */
    ~TrackIsolationTool(void); 
  
    /** initialize */
    StatusCode initialize();

    /** finalize */
    StatusCode finalize();

    /**ITrackIsolationTool interface: */    
    bool trackIsolation( TrackIsolation& result, const IParticle& tp, const std::vector<Iso::IsolationType>& cones, 
                         SubtractionStrategy strategy = UsePointer, const Vertex* vertex = 0, 
                         const std::set<const xAOD::TrackParticle*>* exclusionSet = 0, const TrackParticleContainer* indetTrackParticles = 0 ); 

  private:
    /// define 2*Pi
    static constexpr float m_2PI = 2*M_PI;

    /// hepler function to ensure phi is within +-Pi
    float phiInRange(float phi) const { 
      while (phi >= M_PI) phi -= m_2PI;
      while (phi < -M_PI) phi += m_2PI;
      return phi;
    }

    /** retrieve track particles */
    const TrackParticleContainer* retrieveTrackParticleContainer() const; 

    /** simple isolation (loop) */
    bool simpleIsolation( TrackIsolationInput& input, TrackIsolation& result, const TrackParticleContainer* indetTrackParticles = 0 ) const;

    /** using TracksInConeTool */
    bool binnedIsolation( TrackIsolationInput& input, TrackIsolation& result );

    /** add track particle to isolation calculation */
    void add( TrackIsolationInput& input, const TrackParticle& tp2, TrackIsolation& result ) const;

    std::string m_indetTrackParticleLocation; /// track particle location

    bool m_simpleIsolation; /// flag to select calculation type
    
    float m_overlapCone2; /// overlap cone size squared

    ToolHandle<ITrackParticlesInConeTool> m_tracksInConeTool; /// tracks in cone tool
  };

  inline void TrackIsolationTool::add( TrackIsolationInput& input, const TrackParticle& tp2, TrackIsolation& result ) const {

    // check whether identical particle if using pointer comparison
    if( input.strategy == UsePointer && input.particle == &tp2 ) return;
    if( input.exclusionSet && input.exclusionSet->count(&tp2) )  return;

    // check eta
    float deta = input.particle->eta()-tp2.eta();
    if( deta > input.maxRadius ) return;

    // check phi
    float dphi = phiInRange(input.particle->phi()-tp2.phi());
    if( fabs(dphi) > input.maxRadius ) return;

    // check dr2
    float dr2 = deta*deta + dphi*dphi;
      
    // check cone if using cone based overlap removal
    if( input.strategy == UseCone && dr2 < m_overlapCone2 ) return;
      
    /// sum up particle to the different cones
    for( unsigned int k=0;k<input.cones.size();++k ){
      if( dr2 >= input.cones[k] ) return; 
      result.ptcones[k] += tp2.pt();
      ++result.nucones[k];
    }      
  }


}	// end of namespace

#endif



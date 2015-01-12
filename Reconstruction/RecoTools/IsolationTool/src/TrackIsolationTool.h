/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISOLATIONTOOL_TRACKISOLATIONTOOL_H
#define ISOLATIONTOOL_TRACKISOLATIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "RecoToolInterfaces/ITrackIsolationTool.h"
#include "RecoToolInterfaces/IChargedEFlowIsolationTool.h"
#include "RecoToolInterfaces/IsolationCommon.h"
#include "ParticlesInConeTools/ITrackParticlesInConeTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"

#include "xAODPrimitives/IsolationType.h"

#include <set>
#include <math.h>

namespace InDet {
  class IInDetTrackSelectionTool;
}

namespace xAOD {

  
  class TrackIsolationTool: public AthAlgTool,
    /* 
       virtual public ITrackIsolationTool,
       virtual public IChargedEFlowIsolationTool  { */
    virtual public ITrackIsolationTool {
  public:
      typedef std::vector< const TrackParticle* > TPVec;
      
      // struct to cache inputs to simplify interfaces, for internal usage only
      struct TrackIsolationInput {
	TrackIsolationInput(const IParticle* particle_,TrackCorrection corrections_,
			    const Vertex* vertex_,
			    const std::set<const TrackParticle*>* exclusionSet_ ) :
	  particle(particle_),
	  corrections(corrections_),
	  vertex(vertex_),
	  exclusionSet(exclusionSet_),
	  ptvarconeRadiusSquared( pow( 10000. / particle->pt() , 2 ) ) 
          {}
	
	const IParticle* particle;                     /// input IParticle
	std::vector<float>  coneSizesSquared;          /// cone sizes squared
	float maxRadius;                               /// maximum cone size
	TrackCorrection corrections;                   /// corrections
	const Vertex* vertex;                          /// vertex
	const std::set<const TrackParticle*>* exclusionSet; /// tracks exclused in isolation
	float ptvarconeRadiusSquared;                  /// Variable cone size squared
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
      bool trackIsolation( TrackIsolation& result, const IParticle& tp, 
			   const std::vector<Iso::IsolationType>& cones, 
			   TrackCorrection corrections, 
			   const Vertex* vertex = 0, 
			   const std::set<const TrackParticle*>* exclusionSet = 0, 
			   const TrackParticleContainer* indetTrackParticles = 0 ); 

      /**ITrackIsolationTool interface: */    
      bool decorateParticle( IParticle& tp, 
                             const std::vector<Iso::IsolationType>& cones, 
                             TrackCorrection corrections, 
                             const Vertex* vertex = 0, 
                             const std::set<const TrackParticle*>* exclusionSet = 0, 
                             const TrackParticleContainer* indetTrackParticles = 0 ); 

  private:
      /// define 2*Pi
    static constexpr float m_2PI = 2*M_PI;
    
    /// hepler function to ensure phi is within +-Pi
    inline float phiInRange(float phi) const { 
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

    /** get reference particle */
    const IParticle* getReferenceParticle(const IParticle& particle) const;

    /** init result struct */
    void initresult(TrackIsolation& result, TrackCorrection corrlist, unsigned int typesize);


    std::string m_indetTrackParticleLocation; /// track particle location

    bool m_simpleIsolation; /// flag to select calculation type
    
    float m_overlapCone2; /// overlap cone size squared

    ToolHandle<ITrackParticlesInConeTool> m_tracksInConeTool; /// tracks in cone tool
	
    ToolHandle<InDet::IInDetTrackSelectionTool> m_trkselTool; /// selection of tracks

    /** retrieve pvx if not given */
    const Vertex* retrieveIDBestPrimaryVertex() const;
    double m_z0cut;

  };

}	// end of namespace

#endif



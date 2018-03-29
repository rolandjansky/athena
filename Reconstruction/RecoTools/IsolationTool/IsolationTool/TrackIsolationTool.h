/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISOLATIONTOOL_TRACKISOLATIONTOOL_H
#define ISOLATIONTOOL_TRACKISOLATIONTOOL_H

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "RecoToolInterfaces/ITrackIsolationTool.h"
#include "RecoToolInterfaces/IsolationCommon.h"

#ifndef XAOD_ANALYSIS
// #include "GaudiKernel/ToolHandle.h"
#include "ParticlesInConeTools/ITrackParticlesInConeTool.h"
#endif // XAOD_STANDALONE

#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"

#include "xAODPrimitives/IsolationType.h"

#include <set>
#include <math.h>

// namespace InDet{
//   class IInDetTrackSelectionTool;
// }

namespace xAOD {
 
  class TrackIsolationTool: public asg::AsgTool,
    virtual public ITrackIsolationTool {
    /// Create a constructor for Athena
    ASG_TOOL_CLASS( TrackIsolationTool, ITrackIsolationTool )
  public:
      typedef std::vector< const TrackParticle* > TPVec;
      
      // struct to cache inputs to simplify interfaces, for internal usage only
      struct TrackIsolationInput {
	TrackIsolationInput(const IParticle* particle_,TrackCorrection corrections_,
			    const Vertex* vertex_,
			    const std::set<const TrackParticle*>* exclusionSet_, float maxRadius_=0.4) :
	  particle(particle_),
	  corrections(corrections_),
	  vertex(vertex_),
	  exclusionSet(exclusionSet_),
          maxRadius(maxRadius_), 
	    ptvarconeRadiusSquared(particle->pt() > 0 ? pow( 10000. / particle->pt() , 2 ) : maxRadius_*maxRadius_) // protection should not be needed... but at some point some 0 pt electrons have been seen...
        {}
	
	const IParticle* particle;                     /// input IParticle
	std::vector<float>  coneSizesSquared;          /// cone sizes squared
	TrackCorrection corrections;                   /// corrections
	const Vertex* vertex;                          /// vertex
	const std::set<const TrackParticle*>* exclusionSet; /// tracks exclused in isolation
	float maxRadius;                               /// maximum cone size
	float ptvarconeRadiusSquared;                  /// Variable cone size squared
      };
      
  public:
      /** constructor */
      TrackIsolationTool(const std::string& name);
      
      /** destructor */
      virtual ~TrackIsolationTool(void); 
      
      /** initialize */
      virtual StatusCode initialize() override;
      
      /**ITrackIsolationTool interface: */  
      virtual 
      bool trackIsolation( TrackIsolation& result, const IParticle& tp, 
			   const std::vector<Iso::IsolationType>& cones, 
			   TrackCorrection corrections, 
			   const Vertex* vertex = 0, 
			   const std::set<const TrackParticle*>* exclusionSet = 0, 
			   const TrackParticleContainer* indetTrackParticles = 0 ) const override; 

      /**ITrackIsolationTool interface: */    
      virtual
      bool decorateParticle( IParticle& tp, 
                             const std::vector<Iso::IsolationType>& cones, 
                             TrackCorrection corrections, 
                             const Vertex* vertex = 0, 
                             const std::set<const TrackParticle*>* exclusionSet = 0, 
                             const TrackParticleContainer* indetTrackParticles = 0 ) const override;

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
    bool binnedIsolation( TrackIsolationInput& input, TrackIsolation& result ) const;

    /** add track particle to isolation calculation */
    void add( TrackIsolationInput& input, const TrackParticle& tp2, TrackIsolation& result ) const;

    /** get reference particle */
    const IParticle* getReferenceParticle(const IParticle& particle) const;

    /** init result struct */
    void initresult(TrackIsolation& result, TrackCorrection corrlist, unsigned int typesize) const;

#ifdef XAOD_ANALYSIS // particlesInCone tool will not be avaible. Write our own...
    bool getparticlesInCone( float eta, float phi, float dr, std::vector< const TrackParticle*>& output ) const;
#endif // XAOD_STANDALONE

    std::string m_indetTrackParticleLocation; /// track particle location

    bool m_simpleIsolation; /// flag to select calculation type

    bool m_useTTVAtool; // flag to use dedicated trackVertexAssociationTool instead of trackSelectionTool
    
    float m_overlapCone2; /// overlap cone size squared
#ifndef XAOD_ANALYSIS
    ToolHandle<ITrackParticlesInConeTool> m_tracksInConeTool; /// tracks in cone tool
#endif // XAOD_STANDALONE
    ToolHandle<InDet::IInDetTrackSelectionTool> m_trkselTool; /// selection of tracks

    ToolHandle<CP::ITrackVertexAssociationTool> m_ttvaTool; /// vertex association of tracks

    /** retrieve pvx if not given */
    const Vertex* retrieveIDBestPrimaryVertex() const;

  };

}	// end of namespace

#endif

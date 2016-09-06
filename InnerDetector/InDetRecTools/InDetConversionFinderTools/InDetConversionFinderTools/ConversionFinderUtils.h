/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETCONVERSIONFINDERTOOLS_CONVERSIONFINDERUTILS_H
#define INDETCONVERSIONFINDERTOOLS_CONVERSIONFINDERUTILS_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "Particle/TrackParticle.h"
#include "xAODTracking/Vertex.h"

class MsgStream;

namespace Trk {
  class Track;
}

namespace InDet {
  
  /**
     @class ConversionFinderUtils
     Some helper tools like:
     * hits counter
     @author Tatjana Lenz , Thomas Koffas
  */
  
  class ConversionFinderUtils : public AthAlgTool {
    
  public:
    ConversionFinderUtils (const std::string& type,const std::string& name, const IInterface* parent);
    
    virtual ~ConversionFinderUtils();
    
    static const InterfaceID& interfaceID();
    
    virtual StatusCode initialize();
    
    virtual StatusCode finalize();
    
    /** helper functions */
    /** Hit counter. */
    void countHits(const DataVector<const Trk::MeasurementBase>* mb, int& ntrt, int& nclus);
    /** Estimate ratio of high-to-low level TRT hits. */
    double trRatio(const DataVector<const Trk::MeasurementBase>* mb);
    /** Momentum fraction of tracks in pair. */
    double momFraction(const Trk::TrackParameters* per1, const Trk::TrackParameters* per2);
    /** Approximate distance of minimum approach between tracks in pair. */
    double distBetweenTracks(const Trk::Track *trk_pos, const Trk::Track *trk_neg);
    /** Get measured track parameters at first hit. Trk::Track interface. */
    const Trk::TrackParameters* getTrkParameters(const Trk::Track* track);
    /** Add new perigee to track. */
    const Trk::Track* addNewPerigeeToTrack(const Trk::Track* track, const Trk::Perigee* mp);
    /** Correct VxCandidate with respect to a user defined vertex.  */
    xAOD::Vertex* correctVxCandidate(xAOD::Vertex*,Amg::Vector3D);
    /** Get measured track parameters at first hit. Trk::TrackParticleBase interface. */
    const Trk::TrackParameters* getTrkParticleParameters(const Trk::TrackParticleBase*);
  private:
		
  };
  
}
#endif // INDETCONVERSIONFINDERTOOLS_CONVERSIONFINDERUTILS_H 


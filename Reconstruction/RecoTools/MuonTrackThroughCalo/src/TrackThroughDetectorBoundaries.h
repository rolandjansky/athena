/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackThroughDetectorBoundaries.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef REC_TRACKTHROUGHDETECTORBOUNDARIES_H
#define REC_TRACKTHROUGHDETECTORBOUNDARIES_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"

#include "RecoToolInterfaces/ITrackThroughDetectorBoundaries.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/PropDirection.h"

namespace Trk {
  class IExtrapolator;
  //  class ITrackFitter;
  class TrackingVolume;
}

class IIncidentSvc;
class IExtrapolateToCaloTool;

namespace Rec {
  /** @class TrackThroughDetectorBoundaries
      @brief tool for producing a vector of parameters, each at a designated 
             detector boundary or layer surface, corresponding to the outwards
             movement of a particle. If calorimeter is OFF in test jobs, it makes
             only ID entrance/exit and MS entrance parameters.

      @author Wolfgang Liebig
  */

  class TrackThroughDetectorBoundaries : public AthAlgTool,
    virtual public ITrackThroughDetectorBoundaries,
    virtual public IIncidentListener {
      
  public:
      
      /** AlgTool like constructor */
      TrackThroughDetectorBoundaries(const std::string&,const std::string&,const IInterface*);
      
      /**Virtual destructor*/
      virtual ~TrackThroughDetectorBoundaries();
      
      /** AlgTool initialize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();

    /** extrapolation to fill common storegate object */
    virtual const Rec::ParticleExtrapolationVector* getParametersAcrossAtlas
      (const Trk::Track& trk,
       const Trk::ParticleHypothesis particle=Trk::muon,
       const Trk::PropDirection dir=Trk::alongMomentum) const;

    /** document this */
    virtual std::pair<const Trk::TrackParameters*, const Trk::TrackParameters*>
      getIndetEntryAndExitParameters(const Trk::Track&) const;

    /**  incident service handle e.g. for EndEvent */
    void handle(const Incident& inc);
      
  private:      

      // tools and services      
      ToolHandle<Trk::IExtrapolator>      m_extrapolator;      //!< atlas extrapolator
      ToolHandle<IExtrapolateToCaloTool>  m_trackToCalo;       //!< optional: tool to get pars at Calo layers
      ServiceHandle< IIncidentSvc >       m_incidentSvc;       //!< catch end of event when caches are reset
      mutable const Trk::TrackingVolume*  m_idEnclosingVolume; //!< cached pointer, conditions-aware
      mutable const Trk::TrackingVolume*  m_msEntranceVolume;  //!< cached pointer, conditions-aware
      bool                                m_haveMuonGeometry;  //!< configure tool for ID-Calo only job
      // ToolHandle<Trk::ITrackFitter>      m_fitter;   //!< tool for unslimming via track fit

    };
  
} // end of namespace

#endif //REC_TRACKTHROUGHDETECTORBOUNDARIES_H



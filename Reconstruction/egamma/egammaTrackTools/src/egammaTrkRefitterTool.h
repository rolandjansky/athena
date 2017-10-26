/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATRACKTOOLS_EGAMMATRKREFITTERTOOL_H
#define EGAMMATRACKTOOLS_EGAMMATRKREFITTERTOOL_H
/** @brief
  @class egammaTrackRefitterTool
          It recives a egamma object or Trk::Track
          Refits the track associated with an electron
          NOTE  a  new track has been created to ensure 
          that no memory leaks occur you must delete the Trk::Track pointed to by this function.
  @author A. Morley
*/

/** @brief******************************************************************

NAME:     egammaTrackRefitterTool.h
PACKAGE:  offline/Reconstruction/egamma/egammaTrackTools

CREATED:  Jul 2008
MODIFIED:
          Sep. 26 2008 (FD) moved to egammaTrackTools
          Dec. 09, 2008 (FD) move header to src + use of AthenaBaseComps
********************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrkFitterInterfaces/ITrackFitter.h"
#include "egammaInterfaces/IegammaTrkRefitterTool.h"
#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "xAODEgamma/ElectronFwd.h"
#include "xAODTracking/TrackParticleFwd.h"

class IBeamCondSvc;
class AtlasDetectorID ;

#include "VxVertex/RecVertex.h"
#include "TrkVertexFitterInterfaces/IVertexLinearizedTrackFactory.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "egammaInterfaces/ICaloCluster_OnTrackBuilder.h"

namespace Trk{
  class VxTrackAtVertex;
  class VertexOnTrack;
}

class egammaTrkRefitterTool : virtual public IegammaTrkRefitterTool, public AthAlgTool 
{

 public:

  /** @brief Constructor with AlgTool parameters */
  egammaTrkRefitterTool(const std::string&, const std::string&, const IInterface*);
    
  /** @brief Destructor */
  ~egammaTrkRefitterTool();
  
  /** @brief AlgTool initialise method */
  StatusCode initialize();

  /** @brief AlgTool finalise method */
  StatusCode finalize();
  
  /** @brief AlgTool finalise method */
  StatusCode execute();
    
  /** @brief Refit the track associated with an egamma object*/
  virtual StatusCode  refitElectronTrack(const xAOD::Electron*);
  
  /** @brief Refit a track assoicated to a TrackParticle*/  
  virtual StatusCode  refitTrackParticle(const xAOD::TrackParticle*, const xAOD::Electron* eg = 0);  
  
  /** @brief Refit a track*/
  StatusCode  refitTrack(const Trk::Track*, const xAOD::Electron* eg = 0);

  /** @brief Returns the refitted track.  -- NOTE  a  new track has been created to ensure 
      that no memory leaks occur the USER must delete the Trk::Track pointed to by this function*/
  virtual Trk::Track* refittedTrack();
  
  /** @brief Returns the perigee of the refitted track */
  virtual const Trk::Perigee* refittedTrackPerigee();
  
  /** @brief Returns the original track perigee*/
  virtual const Trk::Track* originalTrack();
  
  /** @brief Returns the original track perigee*/
  virtual const Trk::Perigee* originalTrackPerigee();
  
  /** @brief Returns the refitted track end point parameters*/
  virtual const Trk::TrackParameters* refittedEndParameters();  
  
  /** @brief Returns the original tracks final measurement parameters*/
  virtual const Trk::TrackParameters* originalEndParameters();  
  
  /** @brief Resets all of the pointers -- may cause memory leak if you have not take care of the newly created*/
  virtual void resetRefitter();

  /** @brief Returns the amount of material transversed by the refitted track (using X0)*/
  double getMaterial();
  
 private:
 
  std::vector<const Trk::MeasurementBase*> getIDHits(const Trk::Track* track) ;

  /** @brief Pointer to the refitted track*/  
  Trk::Track                  *m_refittedTrack; 
  
  /** @brief Pointer to the original track*/  
  const Trk::Track            *m_originalTrack; 
  
  /** @brief Pointer to the original E/gamma object*/   
  const xAOD::Electron        *m_egammaObject;
  
  /** @brief Pointer to the original Perigee*/    
  const Trk::Perigee          *m_oMeasPer;
  
  /** @brief Pointer to the refitted MeasuredPerigee*/   
  const Trk::Perigee          *m_rMeasPer;
  
  /** @brief Refit the track using RIO on Track. This option is not suggested and can not run on ESD or AOD*/
  Gaudi::Property<bool> m_fitRIO_OnTrack {this, 
      "Fit_RIO_OnTrack", false, 
      "Switch if refit should be made on PRD or ROT level"};
  
  /** @brief Run outlier removal when doing the track refit*/
  Gaudi::Property<Trk::RunOutlierRemoval> m_runOutlier {this,
      "runOutlier", false,
      "Switch to control outlier finding in track fit"};
  
  /** @brief Add outlier to track hits into vector of hits*/
  Gaudi::Property<bool> m_reintegrateOutliers {this,
      "ReintegrateOutliers", false,
      "Switch to control addition of  outliers back for track fit"};
  
  /** @brief type of material interaction in extrapolation*/
  Gaudi::Property<int> m_matEffects {this, "matEffects", 1,
      "Type of material interaction in extrapolation (Default Electron)"};         
  
  /** @brief Minimum number of silicon hits on track before it is allowed to be refitted*/
  Gaudi::Property<int> m_MinNoSiHits {this, "minNoSiHits", 3,
      "Minimum number of silicon hits on track before it is allowed to be refitted"};
  
  /** @brief Particle Hypothesis*/
  Trk::ParticleHypothesis     m_ParticleHypothesis; 

  /** @brief The track refitter */
  ToolHandle<Trk::ITrackFitter> m_ITrackFitter {this,  
      "FitterTool", "TrkKalmanFitter/AtlasKalmanFitter",
      "ToolHandle for track fitter implementation"};

  /** @brief Returns the final track parameters (ie track parameters furthest from the perigee)  */
  const Trk::TrackParameters* lastTrackParameters(const Trk::Track* track);

  /** @brief Returns the amount of material transversed by the track (using X0)*/
  double getMaterialTraversed(Trk::Track* track);

  /** @brief Adds a beam spot to the Measurements passed to the track refitter*/  
  std::vector<const Trk::MeasurementBase*> addPointsToTrack(const Trk::Track* track, const xAOD::Electron* eg = 0 ); 
  
  const Trk::VertexOnTrack*   provideVotFromBeamspot(const Trk::Track* track) const;
  const xAOD::TrackParticle*  getTrackParticle(Trk::VxTrackAtVertex*) const;

  /** @brief track extrapolator */
  ToolHandle<Trk::IExtrapolator> m_extrapolator {this, 
      "Extrapolator", "Trk::Extrapolator/AtlasExtrapolator",
      "Track extrapolator"};

  ToolHandle<Trk::IVertexLinearizedTrackFactory> m_linFactory {this,
      "LinearizedTrackFactory", "Trk::FullLinearizedTrackFactory"};

  ServiceHandle<IBeamCondSvc> m_beamCondSvc;     //!< condition service for beam-spot retrieval
  
  /** @brief Option to use very simplistic beam spot constraint*/ 
  Gaudi::Property<bool> m_useBeamSpot {this, "useBeamSpot", false, 
      "Switch to control use of Beam Spot Measurement"};
  
  Gaudi::Property<bool> m_useClusterPosition {this, 
      "useClusterPosition", false, 
      "Switch to control use of Cluster position measurement"};

  ToolHandle<ICaloCluster_OnTrackBuilder> m_CCOTBuilder {this,
      "CCOTBuilder", "CaloCluster_OnTrackBuilder"};
  
  /** @brief Option to remove TRT hits from track*/
  Gaudi::Property<bool> m_RemoveTRT {this, "RemoveTRTHits", false,
      "RemoveTRT Hits"};

  const AtlasDetectorID*  m_idHelper  ;

  std::vector<const Trk::MeasurementBase*>  m_trash;
};
  
#endif

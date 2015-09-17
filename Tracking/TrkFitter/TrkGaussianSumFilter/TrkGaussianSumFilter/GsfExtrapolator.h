/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
			GsfExtrapolator.h  -  description
			------------------------------------------
begin                : Tuesday 25th January 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Extrapolation of MultiComponentState class based on
		       Andi's single component state extrapolator. It is an
		       AlgTool inheriting from the IMultiStateExtrapolator class
*********************************************************************************/

#ifndef TrkGsfExtrapolator_H
#define TrkGsfExtrapolator_H

#include "TrkGaussianSumFilter/IMultiStateExtrapolator.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include "TrkGeometry/MagneticFieldProperties.h"

#include <map>
#include <vector>
#include <string>

namespace Trk{

class Layer;
class Surface;
class INavigator;
class TrackingVolume;
class TrackingGeometry;
class TrackStateOnSurface;
class MaterialProperties;
class IMultiComponentStateMerger;
class IMultiComponentStateCombiner;
class IMaterialMixtureConvolution;
class IMultipleScatteringUpdator;
class IEnergyLossUpdator;


/** @struct StateAtBoundarySurface
    - Structure to contain information about a state at the interface between tracking volumes
*/

struct StateAtBoundarySurface {

  /** Data members */
  mutable const MultiComponentState* stateAtBoundary;
  mutable const TrackParameters*     navigationParameters;
  mutable const TrackingVolume*      trackingVolume;
  
  /** Default constructor  */
  StateAtBoundarySurface()
    :
    stateAtBoundary(0),
    navigationParameters(0),
    trackingVolume(0)
    {}
  
  /** Update State at Boundary Surface Information */
  void updateBoundaryInformation( const MultiComponentState* boundaryState,
				  const TrackParameters*     navParameters,
				  const TrackingVolume*      nextVolume )
  {
    stateAtBoundary = boundaryState;
    navigationParameters = navParameters;
    trackingVolume = nextVolume;
  }

  /** Reset boundary information by clearing cashed parameters */
  void resetBoundaryInformation()
  {
    stateAtBoundary      = 0;
    navigationParameters = 0;
    trackingVolume       = 0;
  }

};

/** @class GsfExtrapolator */

class GsfExtrapolator : public AthAlgTool, virtual public IMultiStateExtrapolator {

 public:

  /** Constructor with AlgTool parameters */
  GsfExtrapolator(const std::string&, const std::string&, const IInterface*);
  
  /** Destructor */
  ~GsfExtrapolator();
  
  /** AlgTool initialise method */
  StatusCode initialize();

  /** AlgTool finalise method */
  StatusCode finalize();
	
  /** Extrapolation of a MutiComponentState to a destination surface (1) */
  virtual const MultiComponentState* extrapolate ( const IPropagator&,
						   const MultiComponentState&,
						   const Surface&,
						   PropDirection direction = anyDirection,
						   BoundaryCheck boundaryCheck = true,
						   ParticleHypothesis particleHypothesis = nonInteracting ) const;
  
  /** - Extrapolation of a MultiComponentState to destination surface without material effects (2) */
  virtual const MultiComponentState* extrapolateDirectly ( const IPropagator&,
							   const MultiComponentState&,
							   const Surface&,
							   PropDirection direction = anyDirection,
							   BoundaryCheck boundaryCheck = true,
							   ParticleHypothesis particleHypothesis = nonInteracting ) const;
  
  /** Configured AlgTool extrapolation method (1) */
  virtual const MultiComponentState* extrapolate ( const MultiComponentState&,
						   const Surface&,
						   PropDirection direction = anyDirection,
						   BoundaryCheck boundaryCheck = true,
						   ParticleHypothesis particleHypothesis = nonInteracting ) const;

  /** Configured AlgTool extrapolation without material effects method (2) */
  virtual const MultiComponentState* extrapolateDirectly ( const MultiComponentState&,
							   const Surface&,
							   PropDirection direction = anyDirection,
							   BoundaryCheck boundaryCheck = true,
							   ParticleHypothesis particleHypothesis = nonInteracting ) const;
 
 
 
  virtual const std::vector<const Trk::TrackStateOnSurface*>* extrapolateM(const MultiComponentState&,
                                                                  const Surface& sf,
                                                                  PropDirection dir,
                                                                  BoundaryCheck bcheck,
                                                                  ParticleHypothesis particle) const;
                                                                                    
                                                                                    
 private:
  
  /** Two primary private extrapolation methods 
      - extrapolateToVolumeBoundary - extrapolates to the exit of the destination tracking volume
                                    - Exit layer surface will be hit in this method.
      - extrapolateInsideVolume     - extrapolates to the destination surface in the final tracking volume
  */

  void extrapolateToVolumeBoundary ( const IPropagator&,
				     const MultiComponentState&,
				     const Layer*,
				     const TrackingVolume&,
				     PropDirection direction = anyDirection,
				     ParticleHypothesis particleHypothesis = nonInteracting ) const;
  
  const MultiComponentState* extrapolateInsideVolume ( const IPropagator&,
						       const MultiComponentState&,
						       const Surface&,
						       const Layer*,
						       const TrackingVolume&,
						       PropDirection direction = anyDirection,
						       BoundaryCheck boundaryCheck = true,
						       ParticleHypothesis particleHypothesis = nonInteracting ) const;
  
  /** Additional private extrapolation methods */
  
  /** Layer stepping, stopping at the last layer before destination */
  const MultiComponentState* extrapolateFromLayerToLayer( const IPropagator&,
							  const MultiComponentState&,
							  const TrackingVolume&,
							  const Layer* startLayer,
							  const Layer* destinationLayer = 0,
							  PropDirection direction = anyDirection,
							  ParticleHypothesis particleHypothesis = nonInteracting ) const;
  
  /** Single extrapolation step to an intermediate layer */
  const MultiComponentState* extrapolateToIntermediateLayer ( const IPropagator&,
							      const MultiComponentState&,
							      const Layer&,
							      const TrackingVolume&,
							      PropDirection direction = anyDirection,
							      ParticleHypothesis particleHypothesis = nonInteracting,
							      bool perpendicularCheck = true) const;
  
  /** Final extrapolation step to a destination layer */
  const MultiComponentState* extrapolateToDestinationLayer ( const IPropagator&,
							     const MultiComponentState&,
							     const Surface&,
							     const Layer&,
							     //const TrackingVolume&,
							     const Layer*,
							     PropDirection direction = anyDirection,
							     BoundaryCheck boundaryCheck = true,
							     ParticleHypothesis particleHypothesis = nonInteracting ) const;
  
  /** Extrapolation to consider material effects assuming all material on active sensor elements - CTB method */
  const MultiComponentState* extrapolateSurfaceBasedMaterialEffects ( const IPropagator&,
								      const MultiComponentState&,
								      const Surface&,
								      PropDirection direction = anyDirection,
								      BoundaryCheck boundaryCheck = true,
								      ParticleHypothesis particleHypothesis = nonInteracting ) const;
  
  /** GSF Method to propagate a number of components simultaneously */
  const MultiComponentState* multiStatePropagate ( const IPropagator&,
						   const MultiComponentState&,
						   const Surface&,
						   PropDirection direction = anyDirection,
						   BoundaryCheck boundaryCheck = true,
						   ParticleHypothesis particleHypothesis = nonInteracting ) const;
  
  /** Method to choose propagator type */
  unsigned int propagatorType ( const TrackingVolume& trackingVolume ) const;

  /** Method to initialise navigation parameters including starting state, layer and volume, and destination volume */
  void initialiseNavigation ( const IPropagator&         propagator, 
			      const MultiComponentState& initialState,
			      const Surface&             surface,
			      const Layer*&              associatedLayer,
			      const TrackingVolume*&     currentVolume,
			      const TrackingVolume*&     destinationVolume,
			      const TrackParameters*&    referenceParameters,
			      PropDirection              direction ) const;
  
  /** Method to print details of the state at a point in extrapolation */
  void printState( const std::string&, const TrackParameters& ) const;

  /** Method to set the recall information */
  void setRecallInformation( const Surface&, const Layer&, const TrackingVolume& ) const;
  
  /** Method to reset the recall information */
  void resetRecallInformation() const;
  
  /** Private method to throw a multi-component state into the garbage bin */
  void throwIntoGarbageBin( const MultiComponentState* ) const;

  /** Private method to throw a set of track parameters into the garbage bin */
  void throwIntoGarbageBin( const TrackParameters* ) const;

  /** Private method to empty garbage bins */
  void emptyGarbageBins() const;

  /** Private method to reset the garbage bins */
  void resetGarbageBins() const;
  
  /** Add material to vector*/
  void addMaterialtoVector(const Trk::Layer* nextLayer,
                           const Trk::TrackParameters* nextPar,
                           PropDirection direction  = anyDirection,
                           ParticleHypothesis particleHypothesis = nonInteracting) const;


  std::string layerRZoutput(const Trk::Layer& lay) const;

  std::string positionOutput(const Amg::Vector3D& pos) const;


  int  radialDirection(const Trk::MultiComponentState& pars, PropDirection dir) const;

  bool radialDirectionCheck(const IPropagator& prop,
                           const MultiComponentState& startParm,
                           const MultiComponentState& parsOnLayer,
                           const TrackingVolume& tvol,
                           PropDirection dir,
                           ParticleHypothesis particle) const;

 private:

  int                                m_outputlevel;                      //!< to cache current output level
  
  ToolHandleArray< IPropagator >     m_propagators;                      //!< Propagators to be retrieved from the ToolSvc
  bool                               m_propagatorStickyConfiguration;    //!< Switch between simple and full configured propagators
  unsigned int                       m_propagatorConfigurationLevel;     //!< Configuration level of the propagator
  unsigned int                       m_propagatorSearchLevel;            //!< Search level of the propagator

  ToolHandle<INavigator>                        m_navigator;                        //!< Navigator

  ToolHandle<IMaterialMixtureConvolution>       m_materialUpdator;                  //!< (Multi-component) material effects updator
  
  ToolHandle<IMultiComponentStateMerger>        m_merger;                           //!< Multi-component state reduction

  ToolHandle<IMultiComponentStateCombiner>      m_stateCombiner;                    //!< Multi-component State combiner
  
  ToolHandle< IMultipleScatteringUpdator >      m_msupdators;                 //!<  Array of MultipleScattering Updators
  ToolHandle< IEnergyLossUpdator >              m_elossupdators;                      //!<  Array of EnergyLoss Updators

  bool                               m_surfaceBasedMaterialEffects;      //!< Switch to turn on/off surface based material effects

  mutable bool                       m_recall;                           //!< Flag the recall solution
  mutable const Surface*             m_recallSurface;                    //!< Surface for recall
  mutable const Layer*               m_recallLayer;                      //!< Layer for recall
  mutable const TrackingVolume*      m_recallTrackingVolume;             //!< Tracking volume for recall

  mutable StateAtBoundarySurface     m_stateAtBoundarySurface;           //!< Instance of structure describing the state at a boundary of tracking volumes

  mutable int                        m_extrapolateCalls;                 //!< Statistics: Number of calls to the main extrapolate method
  mutable int                        m_extrapolateDirectlyCalls;         //!< Statistics: Number of calls to the extrapolate directly method
  mutable int                        m_extrapolateDirectlyFallbacks;     //!< Statistics: Number of calls to the extrapolate directly fallback
  mutable int                        m_navigationDistanceIncreaseBreaks; //!< Statistics: Number of times navigation stepping fails to go the right way
  mutable int                        m_oscillationBreaks;                //!< Statistics: Number of times a tracking volume oscillation is detected
  mutable int                        m_missedVolumeBoundary;             //!< Statistics: Number of times the volume boundary is missed

  ServiceHandle<IChronoStatSvc>      m_chronoSvc;                  //!< Timing: The Gaudi time auditing service

  mutable std::map< const MultiComponentState*, bool > m_mcsGarbageBin;  //!< Garbage bin for MultiComponentState objects
  mutable std::map< const TrackParameters*, bool >     m_tpGarbageBin;   //!< Garbage bin for TrackParameter objects

  mutable std::vector<const Trk::TrackStateOnSurface*>*  m_matstates;


  bool m_fastField;
  Trk::MagneticFieldProperties m_fieldProperties;
};

} // end namespace Trk

inline void Trk::GsfExtrapolator::setRecallInformation( const Trk::Surface& recallSurface, const Trk::Layer& recallLayer, const Trk::TrackingVolume& recallTrackingVolume ) const
{

  m_recall               = true;
  m_recallSurface        = &recallSurface;
  m_recallLayer          = &recallLayer;
  m_recallTrackingVolume = &recallTrackingVolume;

}

inline void Trk::GsfExtrapolator::resetRecallInformation() const
{

  m_recall               = false;
  m_recallSurface        = 0;
  m_recallLayer          = 0;
  m_recallTrackingVolume = 0;

}

inline void Trk::GsfExtrapolator::throwIntoGarbageBin( const Trk::MultiComponentState* garbage ) const
{ if (garbage) m_mcsGarbageBin[garbage] = true; }

inline void Trk::GsfExtrapolator::throwIntoGarbageBin( const Trk::TrackParameters* garbage ) const
{ if (garbage) m_tpGarbageBin[garbage] = true; }

inline void Trk::GsfExtrapolator::emptyGarbageBins() const
{

  // Reset the boundary information
  m_stateAtBoundarySurface.resetBoundaryInformation();

  // Loop over both garbage collections and delete
  std::map< const Trk::MultiComponentState*, bool >::iterator mcsGarbage = m_mcsGarbageBin.begin();
  
  for ( ; mcsGarbage != m_mcsGarbageBin.end() ; ++mcsGarbage )
    delete mcsGarbage->first;

  m_mcsGarbageBin.clear();

  std::map< const Trk::TrackParameters*, bool >::iterator tpGarbage = m_tpGarbageBin.begin();

  for ( ; tpGarbage != m_tpGarbageBin.end() ; ++tpGarbage )
    delete tpGarbage->first;

  m_tpGarbageBin.clear();

}
 
inline void Trk::GsfExtrapolator::resetGarbageBins() const
{

  m_mcsGarbageBin.clear();
  m_tpGarbageBin.clear();

}

#endif

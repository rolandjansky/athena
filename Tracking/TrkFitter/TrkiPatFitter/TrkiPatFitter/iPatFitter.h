/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// iPatFitter.h
//   Header file for class iPatFitter
//
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKIPATFITTER_IPATFITTER_H
#define TRKIPATFITTER_IPATFITTER_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <list>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkGeometry/MagneticFieldProperties.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class ITrackingVolumesSvc;
class MessageHelper;
class MsgStream;
namespace Trk
{
class FitMeasurement;
class FitParameters;
class FitProcedure;
class FitProcedureQuality;
class FitQuality;    
class IIntersector;
class IPropagator;
class IMaterialAllocator;
class MaterialEffectsBase;
class PerigeeSurface;
class Track;
class TrackingVolume;
class TrackStateOnSurface;
class Volume;    
    
/** Main Fitter tool providing the implementation for the different
 *  fitting, extension and refitting use cases.
 */
class iPatFitter: public AthAlgTool,
		  virtual public ITrackFitter
{
public:
    // standard AlgTool methods
    iPatFitter		(const std::string& type, 
			 const std::string& name,
			 const IInterface* parent);
    ~iPatFitter		(void); 	// destructor

    // standard Athena methods
    StatusCode initialize();
    StatusCode finalize();

    // 	using TrackFitter::fit;
	
    // iPat Fitter settings (FIXME: to provide??):
    //			RunOutlierRemoval    - use logic to remove bad hits

    // refit a track
    Track*	fit (const Track&,
		     const RunOutlierRemoval	runOutlier=false,
		     const ParticleHypothesis	particleHypothesis=Trk::nonInteracting) const;

    // refit a track adding a PrepRawDataSet
    Track*	fit (const Track&,
		     const PrepRawDataSet&,
		     const RunOutlierRemoval	runOutlier=false,
		     const ParticleHypothesis	particleHypothesis=Trk::nonInteracting) const;

    // fit a set of PrepRawData objects
    Track*	fit (const PrepRawDataSet&,
		     const TrackParameters&	perigeeStartValue,
		     const RunOutlierRemoval	runOutlier=false,
		     const ParticleHypothesis	particleHypothesis=Trk::nonInteracting) const;

    // refit a track adding a MeasurementSet
    Track*	fit (const Track&,
		     const MeasurementSet&,
		     const RunOutlierRemoval	runOutlier=false,
		     const ParticleHypothesis	particleHypothesis=Trk::nonInteracting) const;

    // fit a set of MeasurementBase objects with starting value for perigeeParameters
    Track*	fit (const MeasurementSet&,
		     const TrackParameters&	perigeeStartValue,
		     const RunOutlierRemoval	runOutlier=false,
		     const ParticleHypothesis	particleHypothesis=Trk::nonInteracting) const;

    // combined muon fit
    Track*	fit (const Track&,
		     const Track&,
		     const RunOutlierRemoval	runOutlier=false,
		     const ParticleHypothesis	particleHypothesis=Trk::nonInteracting) const;

protected:
    // class implementing the algorithmic code for fitting
    FitProcedure*				m_fitProcedure;

    // flag to keep member data after base fit completion
    bool					m_globalFit;

    // keep state information from the last fit for access through global interface
    mutable int					m_iterations;
    mutable std::list<FitMeasurement*>*		m_measurements;
    mutable FitParameters*			m_parameters;
    
private:
    // add MeasurementSet
    void	addMeasurements (std::list<FitMeasurement*>&			measurements,
				 const MeasurementSet&				measurementSet,
				 const FitParameters&				parameters) const;
    
    // add TrackStateOnSurfaces (true means material already allocated on trackTSOS)
    bool	addMeasurements (std::list<FitMeasurement*>&			measurements,
				 const FitParameters&				parameters,
				 ParticleHypothesis			   	particleHypothesis,
				 const DataVector<const TrackStateOnSurface>&	trackTSOS) const;

    // initialize measurement list (= re-use in case of global fit)
    std::list<FitMeasurement*>*&	measurementList (void) const;

    // perform fit
    Track*	performFit(std::list<FitMeasurement*>*			measurements,
			   FitParameters*				parameters,
			   const ParticleHypothesis			particleHypothesis,
			   const TrackInfo&				trackInfo,
			   const DataVector<const TrackStateOnSurface>*	leadingTSOS = 0,
			   const FitQuality*				perigeeQuality = 0) const;

    
    // print TSOS on a track (debugging aid)
    void	printTSOS (const Track&) const;

    void	refit(const Track&		track,
		      const RunOutlierRemoval	runOutlier,
		      const ParticleHypothesis	particleHypothesis) const;
    
    // configurables (tools and options)
    bool						m_aggregateMaterial;
    bool						m_asymmetricCaloEnergy;
    bool						m_eigenMatrixTreatment;
    bool						m_fullCombinedFit;
    bool						m_lineFit;
    double						m_lineMomentum;
    mutable ToolHandle<IMaterialAllocator>		m_materialAllocator;
    mutable ToolHandle<IIntersector>			m_rungeKuttaIntersector;
    ToolHandle<IIntersector>				m_solenoidalIntersector;
    ToolHandle<IPropagator>				m_stepPropagator;
    ToolHandle<IIntersector>				m_straightLineIntersector;
    ServiceHandle<ITrackingVolumesSvc>			m_trackingVolumesSvc;

    // configurable tolerances, warnings
    double						m_orderingTolerance;
    unsigned						m_maxWarnings;

    // configurables for validation purposes
    bool						m_constrainedAlignmentEffects;
    bool						m_extendedDebug;
    int 						m_forcedRefitsForValidation;
    int 						m_maxIterations;
    int 						m_useStepPropagator;
    
    // constants 
    Trk::Volume*					m_calorimeterVolume;
    Trk::Volume*					m_indetVolume;
    Trk::MagneticFieldProperties                        m_stepField; 

    // counters
    mutable unsigned					m_countFitAttempts;
    mutable unsigned					m_countGoodFits;
    mutable unsigned					m_countIterations;
    mutable unsigned					m_countRefitAttempts;
    mutable unsigned					m_countGoodRefits;
    mutable unsigned					m_countRefitIterations;

    // count warnings
    mutable MessageHelper*				m_messageHelper;

};

} // end of namespace

#endif // TRKIPATFITTER_IPATFITTER_H

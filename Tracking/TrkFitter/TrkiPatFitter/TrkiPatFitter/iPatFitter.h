/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include <vector>
#include <memory>
#include <mutex>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkiPatFitterUtils/FitMeasurement.h"
#include "TrkiPatFitterUtils/FitParameters.h"
#include "TrkiPatFitterUtils/IMaterialAllocator.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"
#include "TrkTrackSummary/MuonTrackSummary.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class ITrackingVolumesSvc;
class MessageHelper;
class MsgStream;
namespace Trk
{
class FitProcedure;
class FitProcedureQuality;
class FitQuality;    
class IIntersector;
class IPropagator;
class IMaterialAllocator;
class MaterialEffectsBase;
class PerigeeSurface;
class TrackingVolume;
class TrackStateOnSurface;
class Volume;    
class IExtendedTrackSummaryTool;
    
/** Main Fitter tool providing the implementation for the different
 *  fitting, extension and refitting use cases.
 */
class iPatFitter: public AthAlgTool,
		  virtual public ITrackFitter
{
public:
    typedef IMaterialAllocator::Garbage_t Garbage_t;

    // standard AlgTool methods
    iPatFitter		(const std::string& type, 
			 const std::string& name,
			 const IInterface* parent,
       bool isGlobalFit = false);
    virtual ~iPatFitter	(void) override; 	// destructor

    // standard Athena methods
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    // 	using TrackFitter::fit;
	
    // iPat Fitter settings (FIXME: to provide??):
    //			RunOutlierRemoval    - use logic to remove bad hits

    // refit a track
    virtual Track*	fit (const Track&,
		     const RunOutlierRemoval	runOutlier=false,
		     const ParticleHypothesis	particleHypothesis=Trk::nonInteracting) const override;

    // refit a track adding a PrepRawDataSet
    virtual Track*	fit (const Track&,
		     const PrepRawDataSet&,
		     const RunOutlierRemoval	runOutlier=false,
		     const ParticleHypothesis	particleHypothesis=Trk::nonInteracting) const override;

    // fit a set of PrepRawData objects
    virtual Track*	fit (const PrepRawDataSet&,
		     const TrackParameters&	perigeeStartValue,
		     const RunOutlierRemoval	runOutlier=false,
		     const ParticleHypothesis	particleHypothesis=Trk::nonInteracting) const override;

    // refit a track adding a MeasurementSet
    virtual Track*	fit (const Track&,
		     const MeasurementSet&,
		     const RunOutlierRemoval	runOutlier=false,
		     const ParticleHypothesis	particleHypothesis=Trk::nonInteracting) const override;

    // fit a set of MeasurementBase objects with starting value for perigeeParameters
    virtual Track*	fit (const MeasurementSet&,
		     const TrackParameters&	perigeeStartValue,
		     const RunOutlierRemoval	runOutlier=false,
		     const ParticleHypothesis	particleHypothesis=Trk::nonInteracting) const override;

    // combined muon fit
    virtual Track*	fit (const Track&,
		     const Track&,
		     const RunOutlierRemoval	runOutlier=false,
		     const ParticleHypothesis	particleHypothesis=Trk::nonInteracting) const override;

protected:
    class FitState {
      public:
        ~FitState() {
          deleteMeasurements();
        }

        void deleteMeasurements() {
          if (m_measurements) {
            for (auto m : *m_measurements) {
              delete m;
            }
          }
        }

        void newMeasurements() {
          deleteMeasurements();
          m_measurements = std::make_unique<std::vector<FitMeasurement*>>();
        }

        std::vector<FitMeasurement*>& getMeasurements() {
          if (m_measurements) {
            return *m_measurements;
          } else {
            throw std::runtime_error("no measurements exist");
          }
        }

        const std::vector<FitMeasurement*>& getMeasurements() const {
          if (m_measurements) {
            return *m_measurements;
          } else {
            throw std::runtime_error("no measurements exist");
          }
        }

        bool hasMeasurements() const {
          return m_measurements.get() != nullptr;
        }

        std::unique_ptr<FitParameters> parameters = nullptr;
        int iterations = 0;

      private:
        std::unique_ptr<std::vector<FitMeasurement*>> m_measurements = nullptr;
    };

    // fitWithState that creates and returns state
    std::pair<std::unique_ptr<Track>, std::unique_ptr<FitState>> fitWithState (
        const Track&,
        const RunOutlierRemoval runOutlier=false,
        const ParticleHypothesis particleHypothesis=Trk::nonInteracting) const;

    // class implementing the algorithmic code for fitting
    std::unique_ptr<FitProcedure> m_fitProcedure;
    mutable std::mutex m_fitProcedureMutex;

    // flag to indicate global fitter, only used for logging
    const bool m_globalFit = false;

private:
    // add MeasurementSet
    void	addMeasurements (std::vector<FitMeasurement*>&			measurements,
				 const MeasurementSet&				measurementSet,
				 const FitParameters&				parameters) const;
    
    // add TrackStateOnSurfaces (true means material already allocated on trackTSOS)
    bool	addMeasurements (std::vector<FitMeasurement*>&			measurements,
				 const FitParameters&				parameters,
				 ParticleHypothesis			   	particleHypothesis,
				 const DataVector<const TrackStateOnSurface>&	trackTSOS) const;

    // perform fit
    Track*	performFit(
        FitState& fitState,
        const ParticleHypothesis particleHypothesis,
        const TrackInfo& trackInfo,
        const DataVector<const TrackStateOnSurface>* leadingTSOS,
        const FitQuality*	perigeeQuality,
        Garbage_t& garbage) const;
    
    // print TSOS on a track (debugging aid)
    void	printTSOS (const Track&) const;

    void	refit(
        FitState& fitState,
        const Track&		track,
        const RunOutlierRemoval	runOutlier,
        const ParticleHypothesis	particleHypothesis) const;
    
    // configurables (tools and options)
    Gaudi::Property<bool> m_aggregateMaterial {this, "AggregateMaterial", true};
    Gaudi::Property<bool> m_asymmetricCaloEnergy {this, "AsymmetricCaloEnergy", true};
    Gaudi::Property<bool> m_fullCombinedFit {this, "FullCombinedFit", true};
    Gaudi::Property<bool> m_lineFit {this, "LineFit", false};
    Gaudi::Property<double> m_lineMomentum {this, "LineMomentum", 100. * Gaudi::Units::GeV};
    ToolHandle<IMaterialAllocator>			m_materialAllocator;
    ToolHandle<IIntersector>				m_rungeKuttaIntersector;
    ToolHandle<IIntersector>				m_solenoidalIntersector;
    ToolHandle<IPropagator>				m_stepPropagator;
    ToolHandle<IIntersector>				m_straightLineIntersector;
    ServiceHandle<ITrackingVolumesSvc>			m_trackingVolumesSvc;
    ToolHandle<Trk::IExtendedTrackSummaryTool>          m_trackSummaryTool;

    // configurable tolerances, warnings
    Gaudi::Property<double> m_orderingTolerance {this, "OrderingTolerance", 1. * Gaudi::Units::mm};
    Gaudi::Property<unsigned> m_maxWarnings {this, "MaxNumberOfWarnings", 10, 
      "Maximum number of permitted WARNING messages per message type."};

    // configurables for validation purposes
    Gaudi::Property<bool> m_constrainedAlignmentEffects {this, "ConstrainedAlignmentEffects", false};
    Gaudi::Property<bool> m_extendedDebug {this, "ExtendedDebug", false};
    Gaudi::Property<int> m_forcedRefitsForValidation {this, "ForcedRefitsForValidation", 0};
    Gaudi::Property<int> m_maxIterations {this, "MaxIterations", 25}; 

    // m_useStepPropagator 0 means not used (so Intersector used)
    // 1 Intersector not used and StepPropagator used with FullField
    // 2 StepPropagator with FastField propagation
    // 99 debug mode where both are ran with FullField
    Gaudi::Property<int> m_useStepPropagator {this, "UseStepPropagator", 1};
    
    // constants 
    std::unique_ptr<Trk::Volume> m_calorimeterVolume;
    std::unique_ptr<Trk::Volume> m_indetVolume;
    Trk::MagneticFieldProperties                        m_stepField; 

    // counters
    mutable std::atomic<unsigned> m_countFitAttempts = 0;
    mutable std::atomic<unsigned> m_countGoodFits = 0;
    mutable std::atomic<unsigned> m_countIterations = 0;
    mutable std::atomic<unsigned> m_countRefitAttempts = 0;
    mutable std::atomic<unsigned> m_countGoodRefits = 0;
    mutable std::atomic<unsigned> m_countRefitIterations = 0;

    // count warnings
    mutable std::unique_ptr<MessageHelper> m_messageHelper ATLAS_THREAD_SAFE; // MessageHelper is thread-safe

};

} // end of namespace

#endif // TRKIPATFITTER_IPATFITTER_H

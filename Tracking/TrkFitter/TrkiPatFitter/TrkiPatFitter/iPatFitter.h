/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKIPATFITTER_IPATFITTER_H
#define TRKIPATFITTER_IPATFITTER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/EventContext.h"
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
#include "TrkDetDescrInterfaces/ITrackingVolumesSvc.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkiPatFitterUtils/IMaterialAllocator.h"
#include "TrkiPatFitterUtils/MessageHelper.h"
#include "TrkiPatFitterUtils/FitProcedure.h"

#include <vector>
#include <memory>
#include <mutex>
namespace Trk
{
class FitQuality;
class TrackStateOnSurface;
/** Main Fitter tool providing the implementation for the different
 *  fitting, extension and refitting use cases.
 */
class iPatFitter: public AthAlgTool, virtual public ITrackFitter
{
public:
    typedef IMaterialAllocator::Garbage_t Garbage_t;

    // standard AlgTool methods
    iPatFitter		(const std::string& type, 
			 const std::string& name,
			 const IInterface* parent,
       bool isGlobalFit = false);
    virtual ~iPatFitter()=default;

    // standard Athena methods
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    // iPat Fitter settings (FIXME: to provide??):
    //	RunOutlierRemoval    - use logic to remove bad hits

    /*
     * Bring in default impl without
     * EventContext for now
     */
    using ITrackFitter::fit;
 
    // refit a track
    virtual std::unique_ptr<Track> fit(
      const EventContext& ctx,
      const Track&,
      const RunOutlierRemoval runOutlier = false,
      const ParticleHypothesis particleHypothesis = Trk::nonInteracting) const override;

    // refit a track adding a PrepRawDataSet
    virtual std::unique_ptr<Track> fit (
         const EventContext& ctx,
         const Track&,
		     const PrepRawDataSet&,
		     const RunOutlierRemoval	runOutlier=false,
		     const ParticleHypothesis	particleHypothesis=Trk::nonInteracting) const override;

    // fit a set of PrepRawData objects
    virtual std::unique_ptr<Track> fit(
      const EventContext& ctx,
      const PrepRawDataSet&,
      const TrackParameters& perigeeStartValue,
      const RunOutlierRemoval runOutlier = false,
      const ParticleHypothesis particleHypothesis = Trk::nonInteracting) const override;

    // refit a track adding a MeasurementSet
    virtual std::unique_ptr<Track> fit(
      const EventContext& ctx,
      const Track&,
      const MeasurementSet&,
      const RunOutlierRemoval runOutlier = false,
      const ParticleHypothesis particleHypothesis =
        Trk::nonInteracting) const override;

    // fit a set of MeasurementBase objects with starting value for perigeeParameters
    virtual std::unique_ptr<Track> fit(
      const EventContext& ctx,
      const MeasurementSet&,
      const TrackParameters& perigeeStartValue,
      const RunOutlierRemoval runOutlier = false,
      const ParticleHypothesis particleHypothesis = Trk::nonInteracting) const override;

    // combined muon fit
    virtual std::unique_ptr<Track> fit(
      const EventContext& ctx,
      const Track&,
      const Track&,
      const RunOutlierRemoval runOutlier = false,
      const ParticleHypothesis particleHypothesis = Trk::nonInteracting) const override;

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
        const EventContext& ctx,
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
  void addMeasurements(const EventContext& ctx,
                       std::vector<FitMeasurement*>& measurements,
                       const MeasurementSet& measurementSet,
                       const FitParameters& parameters) const;

  // add TrackStateOnSurfaces (true means material already allocated on
  // trackTSOS)
  bool addMeasurements(
    const EventContext& ctx,
    std::vector<FitMeasurement*>& measurements,
    const FitParameters& parameters,
    ParticleHypothesis particleHypothesis,
    const DataVector<const TrackStateOnSurface>& trackTSOS) const;

  // perform fit
  std::unique_ptr<Track> performFit(
    FitState& fitState,
    const ParticleHypothesis particleHypothesis,
    const TrackInfo& trackInfo,
    const DataVector<const TrackStateOnSurface>* leadingTSOS,
    const FitQuality* perigeeQuality,
    Garbage_t& garbage) const;

  // print TSOS on a track (debugging aid)
  void printTSOS(const Track&) const;

  void refit(const EventContext& ctx,
             FitState& fitState,
             const Track& track,
             const RunOutlierRemoval runOutlier,
             const ParticleHypothesis particleHypothesis) const;

  // configurables (tools and options)
  Gaudi::Property<bool> m_aggregateMaterial{ this, "AggregateMaterial", true };
  Gaudi::Property<bool> m_asymmetricCaloEnergy{ this,
                                                "AsymmetricCaloEnergy",
                                                true };
  Gaudi::Property<bool> m_fullCombinedFit{ this, "FullCombinedFit", true };
  Gaudi::Property<bool> m_lineFit{ this, "LineFit", false };
  Gaudi::Property<double> m_lineMomentum{ this,
                                          "LineMomentum",
                                          100. * Gaudi::Units::GeV };
  ToolHandle<IMaterialAllocator> m_materialAllocator;
  ToolHandle<IIntersector> m_rungeKuttaIntersector;
  ToolHandle<IIntersector> m_solenoidalIntersector;
  ToolHandle<IPropagator> m_stepPropagator;
  ToolHandle<IIntersector> m_straightLineIntersector;
  ServiceHandle<ITrackingVolumesSvc> m_trackingVolumesSvc;
  ToolHandle<Trk::IExtendedTrackSummaryTool> m_trackSummaryTool;

  // configurable tolerances, warnings
  Gaudi::Property<double> m_orderingTolerance{ this,
                                               "OrderingTolerance",
                                               1. * Gaudi::Units::mm };
  Gaudi::Property<unsigned> m_maxWarnings{
    this,
    "MaxNumberOfWarnings",
    10,
    "Maximum number of permitted WARNING messages per message type."
  };

  // configurables for validation purposes
  Gaudi::Property<bool> m_constrainedAlignmentEffects{
    this,
    "ConstrainedAlignmentEffects",
    false
  };
  Gaudi::Property<bool> m_extendedDebug{ this, "ExtendedDebug", false };
  Gaudi::Property<int> m_forcedRefitsForValidation{ this,
                                                    "ForcedRefitsForValidation",
                                                    0 };
  Gaudi::Property<int> m_maxIterations{ this, "MaxIterations", 25 };

  // constants
  std::unique_ptr<Trk::Volume> m_calorimeterVolume;
  std::unique_ptr<Trk::Volume> m_indetVolume;
  Trk::MagneticFieldProperties m_stepField;

  // counters
  mutable std::atomic<unsigned> m_countFitAttempts = 0;
  mutable std::atomic<unsigned> m_countGoodFits = 0;
  mutable std::atomic<unsigned> m_countIterations = 0;
  mutable std::atomic<unsigned> m_countRefitAttempts = 0;
  mutable std::atomic<unsigned> m_countGoodRefits = 0;
  mutable std::atomic<unsigned> m_countRefitIterations = 0;

  // count warnings
  mutable std::unique_ptr<MessageHelper> m_messageHelper
    ATLAS_THREAD_SAFE; // MessageHelper is thread-safe

};

} // end of namespace Trk

#endif // TRKIPATFITTER_IPATFITTER_H

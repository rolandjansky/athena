/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GLOBALCHI2FITTER_H
#define GLOBALCHI2FITTER_H
//#define GXFDEBUGCODE

#include "TrkDetDescrInterfaces/IMaterialEffectsOnTrackProvider.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/EventContext.h"

#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"

#include "TrkToolInterfaces/ITrkMaterialProviderTool.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkToolInterfaces/IUpdator.h"

#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkExInterfaces/INavigator.h"
#include "TrkExInterfaces/IMultipleScatteringUpdator.h"
#include "TrkExInterfaces/IEnergyLossUpdator.h"
#include "TrkExInterfaces/IMaterialEffectsUpdator.h"

#include "TrkFitterInterfaces/IGlobalTrackFitter.h"

#include "TrkGlobalChi2Fitter/GXFTrajectory.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkFitterUtils/FitterStatusCode.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"

#include <memory>
#include <mutex>

class AtlasDetectorID;


namespace Trk {
  class Track;
  class TransportJacobian;
  class TrackFitInputPreparator;
  class IMagneticFieldTool;
  class MeasuredPerigee;
  class PrepRawDataComparisonFunction;
  class MeasurementBaseComparisonFunction;
  class MaterialEffectsOnTrack;
  class Layer;
  class CylinderLayer;
  class DiscLayer;
  class MagneticFieldProperties;
  class TrackingGeometry;
  class TrackingVolume;
  class Volume;

  class GlobalChi2Fitter: public extends<AthAlgTool, IGlobalTrackFitter> {
    struct PropagationResult {
      std::unique_ptr<const TrackParameters> m_parameters;
      std::unique_ptr<TransportJacobian> m_jacobian;
      std::optional<std::vector<std::unique_ptr<const TrackParameters>>> m_preholes;
    };

    struct Cache {
      /*
       * Currently the information about what type of fit is being passed by the
       * presence of a TrackingVolume.
       */
      template <class T>
      static
      void objVectorDeleter(const std::vector<const T *> *ptr) {
        if (ptr) {
          for (const T *elm : *ptr) { delete elm; }
          delete ptr;
        }
      }

      const TrackingVolume *m_caloEntrance = nullptr;
      const TrackingVolume *m_msEntrance = nullptr;

      bool m_calomat, m_extmat;
      bool m_idmat = true;
      bool m_sirecal;
      bool m_getmaterialfromtrack;
      bool m_reintoutl;
      bool m_matfilled = false;
      bool m_acceleration;
      bool m_fiteloss;
      bool m_asymeloss;

      std::vector<double> m_phiweight;
      std::vector<int> m_firstmeasurement;
      std::vector<int> m_lastmeasurement;

      std::vector < const Trk::Layer * >m_negdiscs;
      std::vector < const Trk::Layer * >m_posdiscs;
      std::vector < const Trk::Layer * >m_barrelcylinders;

      bool m_fastmat = true;

      int m_lastiter;
      int m_miniter;

      #ifdef GXFDEBUGCODE
      int m_iterations = 0;
      #endif

      Amg::MatrixX m_derivmat;
      Amg::SymMatrixX m_fullcovmat;

      std::vector< std::unique_ptr< const std::vector < const TrackStateOnSurface *>,
                                    void (*)(const std::vector<const TrackStateOnSurface *> *) > >
        m_matTempStore;

      MagField::AtlasFieldCache m_field_cache;

      FitterStatusCode m_fittercode;

      Cache(const GlobalChi2Fitter *fitter):
        m_calomat(fitter->m_calomat),
        m_extmat(fitter->m_extmat),
        m_sirecal(fitter->m_sirecal),
        m_getmaterialfromtrack(fitter->m_getmaterialfromtrack),
        m_reintoutl(fitter->m_reintoutl),
        m_acceleration(fitter->m_acceleration),
        m_fiteloss(fitter->m_fiteloss),
        m_asymeloss(fitter->m_asymeloss),
        m_miniter(fitter->m_miniter)
      {}

      Cache & operator=(const Cache &) = delete;
    };

    enum FitterStatusType {
      S_FITS,
      S_SUCCESSFUL_FITS,
      S_MAT_INV_FAIL,
      S_NOT_ENOUGH_MEAS,
      S_PROPAGATION_FAIL,
      S_INVALID_ANGLES,
      S_NOT_CONVERGENT,
      S_HIGH_CHI2,
      S_LOW_MOMENTUM,
      __S_MAX_VALUE
    };

  public:
    GlobalChi2Fitter(
      const std::string &,
      const std::string &,
      const IInterface *
    );

    virtual ~ GlobalChi2Fitter();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    /*
     * Bring in default impl with
     * EventContext for now
     */
    using ITrackFitter::fit;

    virtual std::unique_ptr<Track> fit(
      const EventContext& ctx,
      const PrepRawDataSet&,
      const TrackParameters&,
      const RunOutlierRemoval runOutlier = false,
      const ParticleHypothesis matEffects = nonInteracting
      ) const override final;

    virtual std::unique_ptr<Track> fit(
      const EventContext& ctx,
      const Track &,
      const RunOutlierRemoval runOutlier = false,
      const ParticleHypothesis matEffects = nonInteracting
    ) const override final;

    virtual std::unique_ptr<Track> fit(
      const EventContext& ctx,
      const MeasurementSet &,
      const TrackParameters &,
      const RunOutlierRemoval runOutlier = false,
      const ParticleHypothesis matEffects = nonInteracting
    ) const override final;

    virtual std::unique_ptr<Track> fit(
      const EventContext& ctx,
      const Track &,
      const PrepRawDataSet &,
      const RunOutlierRemoval runOutlier = false,
      const ParticleHypothesis matEffects = nonInteracting
    ) const override final;

    virtual std::unique_ptr<Track> fit(
      const EventContext& ctx,
      const Track &,
      const Track &,
      const RunOutlierRemoval runOutlier = false,
      const ParticleHypothesis matEffects = nonInteracting
    ) const override final;

    virtual std::unique_ptr<Track> fit(
      const EventContext& ctx,
      const Track &,
      const MeasurementSet &,
      const RunOutlierRemoval runOutlier = false,
      const ParticleHypothesis matEffects = nonInteracting
    ) const override final;

    virtual Track* alignmentFit(
      AlignmentCache&,
      const Track&,
      const RunOutlierRemoval  runOutlier=false,
      const ParticleHypothesis matEffects=Trk::nonInteracting
    ) const override;

  private:
    void calculateJac(
      Eigen::Matrix<double, 5, 5> &,
      Eigen::Matrix<double, 5, 5> &,
      int, int
    ) const;

    Track * fitIm(
      const EventContext& ctx,
      Cache & cache,
      const Track & inputTrack,
      const RunOutlierRemoval runOutlier,
      const ParticleHypothesis matEffects
    ) const;

    Track *myfit(
      const EventContext& ctx,
      Cache &,
      GXFTrajectory &,
      const TrackParameters &,
      const RunOutlierRemoval runOutlier = false,
      const ParticleHypothesis matEffects = nonInteracting
    ) const;

    Track *myfit_helper(
      Cache &,
      GXFTrajectory &,
      const TrackParameters &,
      const RunOutlierRemoval runOutlier = false,
      const ParticleHypothesis matEffects = nonInteracting
    ) const;

    Track *mainCombinationStrategy(
      const EventContext& ctx,
      Cache &,
      const Track &,
      const Track &,
      GXFTrajectory &,
      std::vector<MaterialEffectsOnTrack> &
    ) const;

    Track *backupCombinationStrategy(
      const EventContext& ctx,
      Cache &,
      const Track &,
      const Track &,
      GXFTrajectory &,
      std::vector<MaterialEffectsOnTrack> &
    ) const;

    void makeProtoState(
      Cache &,
      GXFTrajectory &,
      const TrackStateOnSurface *,
      int index = -1,
      bool copytp = false
    ) const;

    void makeProtoStateFromMeasurement(
      Cache &,
      GXFTrajectory &,
      const MeasurementBase *,
      const TrackParameters * trackpar = nullptr,
      bool isoutlier = false,
      int index = -1
    ) const;

    bool processTrkVolume(
      Cache &,
      const Trk::TrackingVolume * tvol
    ) const;

    /**
     * @brief Find the intersection of a set of track parameters onto a disc
     * surface.
     *
     * Calculates the intersection from a point and momentum in space onto a
     * disc surface which represents a disc-shaped layer in the detector. The
     * position of the intersection can be used to find materials in that layer
     * at that position.
     *
     * @param[in] cache The standard GX2F cache.
     * @param[in] surface The surface to intersect with.
     * @param[in] param1 The main track parameters to calculate the
     * intersection from.
     * @param[in] param2 A secondary set of parameters used for electrons. The
     * purpose of this is not known to us at this time.
     * @param[in] mat A particle hypothesis describing the behaviour of the
     * particle.
     *
     * @returns Nothing if the intersection failed (i.e. there was no
     * intersection), otherwise both an intersection positition as well as the
     * angle of inflection.
     *
     * @note This method can probably be replaced entirely by the straight line
     * intersection method of the appropriate Surface subclass.
     */
    std::optional<std::pair<Amg::Vector3D, double>> addMaterialFindIntersectionDisc(
      Cache & cache,
      const DiscSurface & surface,
      const TrackParameters & param1,
      const TrackParameters & param2,
      const ParticleHypothesis mat
    ) const;

    /**
     * @brief Find the intersection of a set of track parameters onto a
     * cylindrical surface.
     *
     * See addMaterialFindIntersectionDisc for more information.
     *
     * @note This method can probably be replaced entirely by the straight line
     * intersection method of the appropriate Surface subclass.
     */
    std::optional<std::pair<Amg::Vector3D, double>> addMaterialFindIntersectionCyl(
      Cache & cache,
      const CylinderSurface & surface,
      const TrackParameters & param1,
      const TrackParameters & param2,
      const ParticleHypothesis mat
    ) const;

    /**
     * @brief Given layer information, probe those layers for scatterers and
     * add them to a track.
     *
     * This is the meat of the pudding, if you will. Given the information that
     * we have about layers, go through them all and find any possible material
     * hits that we need to add to the track.
     *
     * @param[in,out] cache General cache object.
     * @param[in,out] track The track object as it exists now in IR.
     * @param[in] offset The first state after any existing materials.
     * @param[in] layers The list of layers.
     * @param[in] ref1 The first set of reference parameters.
     * @param[in] ref2 The second set of reference parameters.
     * @param[in] mat The particle hypothesis describing the track behaviour.
     *
     * @note Attentive readers may wonder why we pass this function a vector
     * of layers, but not a vector of upstream layers. The reason for this is
     * that the vector of upstream layers is also a member of the cache object.
     */
    void addMaterialUpdateTrajectory(
      Cache & cache,
      GXFTrajectory & track,
      int offset,
      std::vector<std::pair<const Layer *, const Layer *>> & layers,
      const TrackParameters * ref1,
      const TrackParameters * ref2,
      ParticleHypothesis mat
    ) const;

    /**
     * @brief Collect all possible layers that a given track could have passed
     * through.
     *
     * If we are to use layer information to determine possible scatterer hits,
     * we must first gather those layers. That's what this method does. It
     * looks for disc and barrel cylinder layers that the given track might
     * have crossed and collects them into output vectors. One contains layers
     * between states on the track, and the upstream layers lie before the
     * first state of the track.
     *
     * @param[in,out] cache General cache object.
     * @param[out] layers Output vector for layers.
     * @param[out] uplayers Output vector for upstream layers, which lie before
     * the first hit in the track.
     * @param[in] states A list of track states on the track.
     * @param[in] first The first track state.
     * @param[in] last The last track state.
     * @param[in] refpar Reference parameters from which to extrapolate.
     * @param[in] hasmat Are there any existing materials on this track?
     */
    void addMaterialGetLayers(
      Cache & cache,
      std::vector<std::pair<const Layer *, const Layer *>> & layers,
      std::vector<std::pair<const Layer *, const Layer *>> & uplayers,
      std::vector<GXFTrackState *> & states,
      GXFTrackState & first,
      GXFTrackState & last,
      const TrackParameters * refpar,
      bool hasmat
    ) const;

    /**
     * @brief A faster strategy for adding scatter material to tracks, works
     * only for inner detector tracks.
     *
     * For every track, we need to add its scatterers. That is to say, we need
     * to determine which bits of non-active material the particle in question
     * may have passed through and add them to the track. This is generally an
     * expensive operation, but we can cut some corners if the track only
     * consists of inner detector hits. Specifically, we can exploit the layer
     * structure of the detector to find possible material hits more quickly
     * and efficiently than using the standard material adding algorithm, which
     * is addMaterial.
     *
     * @param[in,out] cache General cache object, as used everywhere.
     * @param[in,out] trajectory The current state of the track, respresented
     * in the fitter's internal track representation. States may be added to
     * this.
     * @param[in] parameters Starting parameters for the material addition
     * step.
     * @param[in] part Standard representation of particle type, used to
     * determine the behaviour of the particle as it traverses materials.
     */
    void addIDMaterialFast(
      const EventContext& ctx,
      Cache & cache,
      GXFTrajectory & track,
      const TrackParameters * parameters,
      ParticleHypothesis part
    ) const;

    void addMaterial(
      Cache &,
      GXFTrajectory &,
      const TrackParameters *,
      ParticleHypothesis
    ) const;

    const TrackParameters *makePerigee(
      Cache &,
      const TrackParameters &,
      const ParticleHypothesis
    ) const;

    void makeTrackFillDerivativeMatrix(
      Cache &,
      GXFTrajectory &
    ) const;

    std::unique_ptr<const TrackParameters> makeTrackFindPerigeeParameters(
      const EventContext &,
      Cache &,
      GXFTrajectory &,
      const ParticleHypothesis
    ) const;

    std::unique_ptr<const TrackStateOnSurface> makeTrackFindPerigee(
      const EventContext &,
      Cache &,
      GXFTrajectory &,
      const ParticleHypothesis
    ) const;

    std::unique_ptr<Track> makeTrack(
      const EventContext& ctx,
      Cache &,
      GXFTrajectory &,
      const ParticleHypothesis
    ) const;

    TrackStateOnSurface *makeTSOS(
      GXFTrackState *,
      const ParticleHypothesis
    ) const;

    void fillResiduals(
      Cache &,
      GXFTrajectory &,
      int,
      Amg::SymMatrixX &,
      Amg::VectorX &,
      Amg::SymMatrixX &,
      bool &
    ) const;

    void fillDerivatives(
      GXFTrajectory & traj,
      bool onlybrem = false
    ) const;

    FitterStatusCode runIteration(
      const EventContext& ctx,
      Cache &,
      GXFTrajectory &,
      int,
      Amg::SymMatrixX &,
      Amg::VectorX &,
      Amg::SymMatrixX &,
      bool &
    ) const;

    FitterStatusCode updateFitParameters(
      GXFTrajectory &,
      Amg::VectorX &,
      const Amg::SymMatrixX &
    ) const;

    GXFTrajectory *runTrackCleanerSilicon(
      const EventContext& ctx,
      Cache &,
      GXFTrajectory &,
      Amg::SymMatrixX &,
      Amg::SymMatrixX &,
      Amg::VectorX &,
      bool
    ) const;

    void runTrackCleanerTRT(
      Cache &,
      GXFTrajectory &,
      Amg::SymMatrixX &,
      Amg::VectorX &,
      Amg::SymMatrixX &,
      bool, bool, int
    ) const;

    /**
     * @brief Helper method that encapsulates calls to the propagator tool in
     * the calculateTrackParameters() method.
     *
     * This method encapsulates some of the logic relating to passing or not
     * passing a Jacobian matrix to make the calculateTrackParameters() a lot
     * more readable.
     *
     * For information about parameters see the
     * IPropagator::propagateParameters() method, which this method almost
     * directly wraps.
     */
    PropagationResult calculateTrackParametersPropagateHelper(
      const EventContext &,
      const TrackParameters &,
      const GXFTrackState &,
      PropDirection,
      MagneticFieldProperties,
      bool
    ) const;

    /**
     * @brief Propagate onto a track state, collecting new track parameters, and
     * optionally the Jacobian and possible holes.
     *
     * This is a helper function for the calculateTrackParameters() method. Its
     * purpose is to propagate from a set of track parameters onto a surface,
     * finding the new track parameters at that surface and optionally the
     * Jacobian and a list of possible holes.
     *
     * This method uses another helper function, aptly called
     * calculateTrackParametersPropagateHelper(), which wraps the actual
     * propagator calls. What calculateTrackParametersPropagate() is call this
     * method once and check the result. If the result is invalid, that is to
     * say we didn't manage to extract a correct set of track parameters, we
     * try again but with the propagation direction flipped.
     *
     * If the calcderiv argument is set, this method will attempt to calculate
     * the Jacobian as well as the new set of track parameters. This involves
     * non-trivial logic which is abstracted away in the underlying helper
     * function.
     *
     * @param[in] ctx An event context.
     * @param[in] prev The origin track parameters to start the propagation.
     * @param[in] ts The destination track state (in GX2F internal form).
     * @param[in] propdir The propagation direction.
     * @param[in] bf The magnetic field properties.
     * @param[in] calcderiv If set, calculate the derivative.
     *
     * @return An instance of PropagationResult, which is a struct with three
     * members. Firstly, it contains a unique pointer to a set of track
     * parameters, which are the track parameters at the destination track
     * state following propagation. If these parameters are a nullpointer, that
     * indicates a failure state. Secondly, if requested, the Jacobian is stored
     * in this struct. This may be a nullptr if it was not requested or if the
     * calculation of the Jacobian failed. Thirdly, it contains a vector of
     * possible holes found between the start and end of the propagation. Since
     * these hole states are not always necessary, they are wrapped in a
     * std::optional type.
     */
    PropagationResult calculateTrackParametersPropagate(
      const EventContext &,
      const TrackParameters &,
      const GXFTrackState &,
      PropDirection,
      MagneticFieldProperties,
      bool
    ) const;

    FitterStatusCode calculateTrackParameters(
      const EventContext& ctx,
      GXFTrajectory&,
      bool) const;

    void calculateDerivatives(GXFTrajectory &) const;

    void calculateTrackErrors(GXFTrajectory &, Amg::SymMatrixX &, bool) const;

    std::unique_ptr<TransportJacobian> numericalDerivatives(
      const EventContext& ctx,
      const TrackParameters *,
      const Surface *,
      PropDirection,
      const MagneticFieldProperties
    ) const;

    virtual int iterationsOfLastFit() const;

    virtual void setMinIterations(int);

    bool correctAngles(double &, double &) const;

    bool isMuonTrack(const Track &) const;

    void incrementFitStatus(enum FitterStatusType) const;

    /**
     * @brief Initialize a field cache inside a fit cache object.
     *
     * Following the shift from old-style magnetic field services to the new
     * cached implementation for thread safety, we need some additional logic
     * to create a magnetic field cache object and insert it into our fitting
     * cache object for access.
     *
     * @param[in] cache The GX2F cache objects in which to load the magnetic
     * field cache.
     */
    void initFieldCache(
      const EventContext& ctx,
      Cache & cache
    ) const;

    ToolHandle<IRIO_OnTrackCreator> m_ROTcreator {this, "RotCreatorTool", "", ""};
    ToolHandle<IRIO_OnTrackCreator> m_broadROTcreator {this, "BroadRotCreatorTool", "", ""};
    ToolHandle<IUpdator> m_updator {this, "MeasurementUpdateTool", "", ""};
    ToolHandle<IExtrapolator> m_extrapolator {this, "ExtrapolationTool", "Trk::Extrapolator/CosmicsExtrapolator", ""};
    ToolHandle<IMultipleScatteringUpdator> m_scattool {this, "MultipleScatteringTool", "Trk::MultipleScatteringUpdator/AtlasMultipleScatteringUpdator", ""};
    ToolHandle<IEnergyLossUpdator> m_elosstool {this, "EnergyLossTool", "Trk::EnergyLossUpdator/AtlasEnergyLossUpdator", ""};
    ToolHandle<IMaterialEffectsUpdator> m_matupdator {this, "MaterialUpdateTool", "", ""};
    ToolHandle<IPropagator> m_propagator {this, "PropagatorTool", "Trk::StraightLinePropagator/CosmicsPropagator", ""};
    ToolHandle<INavigator> m_navigator {this, "NavigatorTool", "Trk::Navigator/CosmicsNavigator", ""};
    ToolHandle<IResidualPullCalculator> m_residualPullCalculator {this, "ResidualPullCalculatorTool", "Trk::ResidualPullCalculator/ResidualPullCalculator", ""};
    ToolHandle<Trk::ITrkMaterialProviderTool> m_caloMaterialProvider {this, "CaloMaterialProvider", "Trk::TrkMaterialProviderTool/TrkMaterialProviderTool", ""};
    ToolHandle<IMaterialEffectsOnTrackProvider> m_calotool {this, "MuidTool", "Rec::MuidMaterialEffectsOnTrackProvider/MuidMaterialEffectsOnTrackProvider", ""};
    ToolHandle<IMaterialEffectsOnTrackProvider> m_calotoolparam {this, "MuidToolParam", "", ""};

    ServiceHandle<ITrackingGeometrySvc> m_trackingGeometrySvc;

    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_field_cache_key{
      this,
      "AtlasFieldCacheCondObj",
      "fieldCondObj",
      "Trk::GlobalChi2Fitter field conditions object key"
    };

    const AtlasDetectorID *m_DetID = nullptr;

    Gaudi::Property<bool> m_signedradius {this, "SignedDriftRadius", true};
    Gaudi::Property<bool> m_calomat {this, "MuidMat", false};
    Gaudi::Property<bool> m_extmat {this, "ExtrapolatorMaterial", true};
    Gaudi::Property<bool> m_fillderivmatrix {this, "FillDerivativeMatrix", false};
    Gaudi::Property<bool> m_printderivs {this, "PrintDerivatives", false};
    Gaudi::Property<bool> m_straightlineprop {this, "StraightLine", true};
    Gaudi::Property<bool> m_extensioncuts {this, "TRTExtensionCuts", true};
    Gaudi::Property<bool> m_sirecal {this, "RecalibrateSilicon", false};
    Gaudi::Property<bool> m_trtrecal {this, "RecalibrateTRT", false};
    Gaudi::Property<bool> m_kinkfinding {this, "KinkFinding", false};
    Gaudi::Property<bool> m_decomposesegments {this, "DecomposeSegments", true};
    Gaudi::Property<bool> m_getmaterialfromtrack {this, "GetMaterialFromTrack", true};
    Gaudi::Property<bool> m_domeastrackpar {this, "MeasuredTrackParameters", true};
    Gaudi::Property<bool> m_storemat {this, "StoreMaterialOnTrack", true};
    Gaudi::Property<bool> m_redoderivs {this, "RecalculateDerivatives", false};
    Gaudi::Property<bool> m_reintoutl {this, "ReintegrateOutliers", false};
    Gaudi::Property<bool> m_acceleration {this, "Acceleration", false};
    Gaudi::Property<bool> m_numderiv {this, "NumericalDerivs", false};
    Gaudi::Property<bool> m_fiteloss {this, "FitEnergyLoss", false};
    Gaudi::Property<bool> m_asymeloss {this, "AsymmetricEnergyLoss", true};
    Gaudi::Property<bool> m_useCaloTG {this, "UseCaloTG", false};
    Gaudi::Property<bool> m_rejectLargeNScat {this, "RejectLargeNScat", false};

    Gaudi::Property<double> m_outlcut {this, "OutlierCut", 5.0};
    Gaudi::Property<double> m_p {this, "Momentum", 0.0};
    Gaudi::Property<double> m_chi2cut {this, "TrackChi2PerNDFCut", 1.e15};
    Gaudi::Property<double> m_scalefactor {this, "TRTTubeHitCut", 2.5};

    Gaudi::Property<int> m_maxoutliers {this, "MaxOutliers", 10};
    Gaudi::Property<int> m_maxit {this, "MaxIterations", 30};
    Gaudi::Property<int> m_miniter {this, "MinimumIterations", 1};
    Gaudi::Property<int> m_fixbrem {this, "FixBrem", -1};

    ParticleMasses m_particleMasses;

    /*
     * The following members are mutable. They keep track of the number of
     * fits that have returned with a certain status. Since this must be
     * shared across threads, we protect the array with a mutex, and we mark
     * these members as thread_safe for the ATLAS G++ plugin.
     */
    mutable std::mutex m_fit_status_lock ATLAS_THREAD_SAFE;
    mutable std::array<unsigned int, __S_MAX_VALUE> m_fit_status ATLAS_THREAD_SAFE = {};
  };
}
#endif

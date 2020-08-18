/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// CombinedMuonTrackBuilder
//  AlgTool gathering  material effects along a combined muon track, in
//  particular the TSOS'es representing the calorimeter energy deposit and
//  Coulomb scattering.
//  The resulting track is fitted at the IP
//
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

#include "MuidTrackBuilder/CombinedMuonTrackBuilder.h"

#include <cmath>
#include <iomanip>
#include <memory>

#include "AthenaKernel/Units.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include "TrkDetDescrInterfaces/ITrackingVolumesSvc.h"
#include "TrkEventUtils/IdentifierExtractor.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/RotatedTrapezoidBounds.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkTrack/Track.h"
#include "TrkTrackSummary/MuonTrackSummary.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkiPatFitterUtils/MessageHelper.h"
#include "VxVertex/RecVertex.h"
#include "muonEvent/CaloEnergy.h"


namespace Units = Athena::Units;


namespace Rec {


CombinedMuonTrackBuilder::CombinedMuonTrackBuilder(const std::string& type, const std::string& name,
                                                   const IInterface* parent)
    : AthAlgTool(type, name, parent),
      m_trackingGeometrySvc("TrackingGeometrySvc/AtlasTrackingGeometrySvc", name),
      m_trackingVolumesSvc("TrackingVolumesSvc/TrackingVolumesSvc", name),
      m_magFieldProperties(Trk::FullField),
      m_allowCleanerVeto(true),
      m_cleanCombined(true),
      m_cleanStandalone(true),
      m_perigeeAtSpectrometerEntrance(false),
      m_reallocateMaterial(true),
      m_badFitChi2(2.5),
      m_largeImpact(100. * Gaudi::Units::mm),
      m_largeMomentumChange(0.05),
      m_largeMomentumError(0.15),
      m_largePhiError(0.020),
      m_lineMomentum(2. * Gaudi::Units::GeV),
      m_lowMomentum(10. * Gaudi::Units::GeV),
      m_maxWarnings(10),  // MessageHelper configuration
      m_minEnergy(0.3 * Gaudi::Units::GeV),
      m_numberSigmaFSR(2.5),
      m_redoRots(false),
      m_vertex2DSigmaRPhi(100. * Gaudi::Units::mm),
      m_vertex2DSigmaZ(100. * Gaudi::Units::meter),
      m_vertex3DSigmaRPhi(6. * Gaudi::Units::mm),
      m_vertex3DSigmaZ(60. * Gaudi::Units::mm),
      m_zECToroid(10. * Gaudi::Units::meter),
      m_IDMS_xySigma(1. * Gaudi::Units::mm),
      m_IDMS_rzSigma(1. * Gaudi::Units::mm),
      m_indetSlimming(false),
      m_inputSlimming(false),
      m_calorimeterVolume(nullptr),
      m_indetVolume(nullptr),
      m_spectrometerEntrance(nullptr),
      m_beamAxis(nullptr),
      m_perigeeSurface(nullptr),
      m_sigmaPhiSector(0),
      m_vertex(nullptr),
      m_countAcceptedStandaloneFit(0),
      m_countBeamAxis(0),
      m_countCombinedCleanerVeto(0),
      m_countDegradedStandaloneFit(0),
      m_countExtensionCleanerVeto(0),
      m_countStandaloneCleanerVeto(0),
      m_countVertexRegion(0),
      m_messageHelper(nullptr),
      m_updateWithCaloTG(false),
      m_useCaloTG(false),
      m_iterateCombinedTrackFit(false),
      m_refineELossCombinedTrackFit(true),
      m_refineELossStandAloneTrackFit(true),
      m_addElossID(true),
      m_addIDMSerrors(true),
      m_useRefitTrackError(true)
{
    m_messageHelper = new MessageHelper(*this);

    declareInterface<ICombinedMuonTrackBuilder>(this);
    declareProperty("TrackingVolumesSvc", m_trackingVolumesSvc);
    declareProperty("AllowCleanerVeto", m_allowCleanerVeto);
    declareProperty("CleanCombined", m_cleanCombined);
    declareProperty("CleanStandalone", m_cleanStandalone);
    declareProperty("PerigeeAtSpectrometerEntrance", m_perigeeAtSpectrometerEntrance);
    declareProperty("ReallocateMaterial", m_reallocateMaterial);
    declareProperty("BadFitChi2", m_badFitChi2);
    declareProperty("LargeImpact", m_largeImpact);
    declareProperty("LargeMomentumChange", m_largeMomentumChange);
    declareProperty("LargeMomentumError", m_largeMomentumError);
    declareProperty("LargePhiError", m_largePhiError);
    declareProperty("LineMomentum", m_lineMomentum);
    declareProperty("LowMomentum", m_lowMomentum);
    declareProperty("MaxNumberOfWarnings", m_maxWarnings,
                    "Maximum number of permitted WARNING messages per message type.");
    declareProperty("MinEnergy", m_minEnergy);
    declareProperty("NumberSigmaFSR", m_numberSigmaFSR);
    declareProperty("Vertex2DSigmaRPhi", m_vertex2DSigmaRPhi);
    declareProperty("Vertex2DSigmaZ", m_vertex2DSigmaZ);
    declareProperty("Vertex3DSigmaRPhi", m_vertex3DSigmaRPhi);
    declareProperty("Vertex3DSigmaZ", m_vertex3DSigmaZ);
    declareProperty("zECToroid", m_zECToroid);
    declareProperty("IDMS_xySigma", m_IDMS_xySigma);
    declareProperty("IDMS_rzSigma", m_IDMS_rzSigma);

    // deprecated
    declareProperty("IndetSlimming", m_indetSlimming);
    declareProperty("InputSlimming", m_inputSlimming);

    declareProperty("UpdateWithCaloTG", m_updateWithCaloTG);
    declareProperty("UseCaloTG", m_useCaloTG);
    declareProperty("IterateCombinedTrackFit", m_iterateCombinedTrackFit);
    declareProperty("RefineELossCombinedTrackFit", m_refineELossCombinedTrackFit);
    declareProperty("RefineELossStandAloneTrackFit", m_refineELossStandAloneTrackFit);
    declareProperty("AddElossID", m_addElossID);
    declareProperty("AddIDMSerrors", m_addIDMSerrors);
    declareProperty("UseRefitTrackError", m_useRefitTrackError);
}


StatusCode
CombinedMuonTrackBuilder::initialize()
{
    ATH_CHECK(AthAlgTool::initialize());

    ATH_MSG_DEBUG("Initializing CombinedMuonTrackBuilder"
                  << " - package version " << PACKAGE_VERSION);
    ATH_MSG_DEBUG(" with options: ");

    if (m_allowCleanerVeto) ATH_MSG_DEBUG(" AllowCleanerVeto");
    if (m_cleanCombined) ATH_MSG_DEBUG(" CleanCombined");
    if (m_cleanStandalone) ATH_MSG_DEBUG(" CleanStandalone");
    if (m_perigeeAtSpectrometerEntrance) ATH_MSG_DEBUG(" PerigeeAtSpectrometerEntrance");

    m_perigeeAtSpectrometerEntranceLocal = m_perigeeAtSpectrometerEntrance;

    if (m_reallocateMaterial) ATH_MSG_DEBUG(" ReallocateMaterial");
    if (!m_cscRotCreator.empty()) ATH_MSG_DEBUG(" RedoCscRots");
    if (!m_mdtRotCreator.empty()) ATH_MSG_DEBUG(" RedoMdtRots");
    if (!m_muonErrorOptimizer.empty()) ATH_MSG_DEBUG(" ErrorOptimisation");
    if (!m_muonHoleRecovery.empty()) ATH_MSG_DEBUG(" HoleRecovery");

    // fill WARNING messages
    m_messageHelper->setMaxNumberOfMessagesPrinted(m_maxWarnings);
    m_messageHelper->setNumberOfMessages(50);
    m_messageHelper->setMessage(0, "combinedFit:: missing MeasuredPerigee for indet track");
    m_messageHelper->setMessage(1, "combinedFit:: fail with MS removed by cleaner");
    m_messageHelper->setMessage(2, "combinedFit:: fail with perigee outside indet");
    m_messageHelper->setMessage(3, "combinedFit:: fail with missing caloEnergy");
    m_messageHelper->setMessage(4, "combinedFit:: final combined track lost, this should not happen");
    m_messageHelper->setMessage(5, "indetExtension:: reject with insufficient MS measurements");
    m_messageHelper->setMessage(6, "standaloneFit:: input vertex fails dynamic_cast");
    m_messageHelper->setMessage(7, "standaloneFit:: missing MeasuredPerigee for spectrometer track");
    m_messageHelper->setMessage(8, "standaloneFit:: missing TrackParameters on prefit");
    m_messageHelper->setMessage(9, "standaloneFit:: prefit fails parameter extrapolation to calo");
    m_messageHelper->setMessage(10, "standaloneFit:: extrapolated track missing TrackParameters at calo scatterer");
    m_messageHelper->setMessage(11, "standaloneFit:: final track lost, this should not happen");
    m_messageHelper->setMessage(12, "standaloneFit:: fail as calo incorrectly described");
    m_messageHelper->setMessage(13, "standaloneRefit:: fail track as no TSOS with type CaloDeposit");
    m_messageHelper->setMessage(14, "standaloneRefit:: no inner material");
    m_messageHelper->setMessage(15, "standaloneRefit:: no inner parameters");
    m_messageHelper->setMessage(16, "standaloneRefit:: innerScattering dynamic_cast failed");
    m_messageHelper->setMessage(17, "standaloneRefit:: no TSOS of type CaloDeposit found");
    m_messageHelper->setMessage(18, "standaloneRefit:: no inner scattering TSOS found");
    m_messageHelper->setMessage(19, "standaloneRefit:: no middle material");
    m_messageHelper->setMessage(20, "standaloneRefit:: no middle parameters");
    m_messageHelper->setMessage(21, "standaloneRefit:: no CaloDeposit TSOS found");
    m_messageHelper->setMessage(22, "standaloneRefit:: no outer material");
    m_messageHelper->setMessage(23, "standaloneRefit:: no outer parameters");
    m_messageHelper->setMessage(24, "standaloneRefit:: outerScattering dynamic_cast failed");
    m_messageHelper->setMessage(25, "standaloneRefit:: no outerScattering or CaloDeposit TSOS found");
    m_messageHelper->setMessage(26, "standaloneRefit:: failed propagation to innerTSOS");
    m_messageHelper->setMessage(27, "standaloneRefit:: failed propagation to middleTSOS");
    m_messageHelper->setMessage(28, "standaloneRefit:: fail as calo incorrectly described");
    m_messageHelper->setMessage(29, "fit:: particle hypothesis must be 0 or 2 (nonInteracting or muon). Requested: ");
    m_messageHelper->setMessage(30,
                                "fit:: about to add the TSOS's describing calorimeter association to a combined muon");
    m_messageHelper->setMessage(31, "fit:: particle hypothesis must be 0 or 2 (nonInteracting or muon). Requested: ");
    m_messageHelper->setMessage(32, "fit:: particle hypothesis must be 0 or 2 (nonInteracting or muon). Requested: ");
    m_messageHelper->setMessage(33,
                                "fit:: combined muon track is missing the TSOS's describing calorimeter association");
    m_messageHelper->setMessage(34, "appendSelectedTSOS:: skip duplicate measurement");
    m_messageHelper->setMessage(35, "caloEnergyParameters:: muonTrack without caloEnergy association");
    m_messageHelper->setMessage(36, "caloEnergyParameters:: combinedTrack without caloEnergy association");
    m_messageHelper->setMessage(37, "createMuonTrack:: should never happen: FSR caloEnergy delete");
    m_messageHelper->setMessage(38, "createSpectrometerTSOS:: missing MeasuredPerigee for spectrometer track");
    m_messageHelper->setMessage(39, "createSpectrometerTSOS:: skip unrecognized TSOS without TrackParameters. Type: ");
    m_messageHelper->setMessage(40, "createSpectrometerTSOS:: skip duplicate measurement on same Surface. Type: ");
    m_messageHelper->setMessage(
        41, "entrancePerigee:: missing TrackingGeometrySvc - no perigee will be added at MS entrance");
    m_messageHelper->setMessage(42, "extrapolatedParameters:: missing MeasuredPerigee for spectrometer track");
    m_messageHelper->setMessage(43, "extrapolatedParameters:: missing spectrometer parameters on spectrometer track");
    m_messageHelper->setMessage(44, "final track lost, this should not happen");
    m_messageHelper->setMessage(45, "momentumUpdate:: update failed, keeping original value");
    m_messageHelper->setMessage(46, "reallocateMaterial:: null perigeeStartValue");
    m_messageHelper->setMessage(47, "reallocateMaterial:: refit fails");
    m_messageHelper->setMessage(48, "standaloneFit:: insufficient measurements on input spectrometer track");
    m_messageHelper->setMessage(49, "standaloneFit:: inconsistent TSOS on input spectrometer track");

    ATH_CHECK(m_printer.retrieve());

    ATH_CHECK(m_caloEnergyParam.retrieve());
    ATH_MSG_DEBUG("Retrieved tool " << m_caloEnergyParam);
    ATH_CHECK(m_caloTSOS.retrieve());
    ATH_MSG_DEBUG("Retrieved tool " << m_caloTSOS);
    ATH_CHECK(m_cleaner.retrieve());
    ATH_MSG_DEBUG("Retrieved tool " << m_cleaner);

    if (!m_cscRotCreator.empty()) {
        m_redoRots = true;
        ATH_CHECK(m_cscRotCreator.retrieve());
        ATH_MSG_DEBUG("Retrieved tool " << m_cscRotCreator);
    }

    ATH_CHECK(m_extrapolator.retrieve());
    ATH_MSG_DEBUG("Retrieved tool " << m_extrapolator);
    ATH_CHECK(m_fitter.retrieve());
    ATH_MSG_DEBUG("Retrieved tool " << m_fitter);
    ATH_CHECK(m_fitterSL.retrieve());
    ATH_MSG_DEBUG("Retrieved tool " << m_fitterSL);
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_MSG_DEBUG("Retrieved tool " << m_idHelperSvc);
    ATH_CHECK(m_intersector.retrieve());
    ATH_MSG_DEBUG("Retrieved tool " << m_intersector);
    /// handle to the magnetic field cache
    ATH_CHECK(m_fieldCacheCondObjInputKey.initialize());
    ATH_MSG_DEBUG("Setup handle for key " << m_fieldCacheCondObjInputKey);

    if (!m_materialAllocator.empty()) {
        ATH_CHECK(m_materialAllocator.retrieve());
        ATH_MSG_DEBUG("Retrieved tool " << m_materialAllocator);
    }

    if (!m_mdtRotCreator.empty()) {
        m_redoRots = true;
        ATH_CHECK(m_mdtRotCreator.retrieve());
        ATH_MSG_DEBUG("Retrieved tool " << m_mdtRotCreator);
    }

    if (!m_muonErrorOptimizer.empty()) {
        ATH_CHECK(m_muonErrorOptimizer.retrieve());
        ATH_MSG_DEBUG("Retrieved tool " << m_muonErrorOptimizer);
    }

    if (!m_muonHoleRecovery.empty()) {
        ATH_CHECK(m_muonHoleRecovery.retrieve());
        ATH_MSG_DEBUG("Retrieved tool " << m_muonHoleRecovery);
    }

    ATH_CHECK(m_propagator.retrieve());
    ATH_MSG_DEBUG("Retrieved tool " << m_propagator);
    ATH_CHECK(m_propagatorSL.retrieve());
    ATH_MSG_DEBUG("Retrieved tool " << m_propagatorSL);
    ATH_CHECK(m_trackQuery.retrieve());
    ATH_MSG_DEBUG("Retrieved tool " << m_trackQuery);
    ATH_CHECK(m_trackSummary.retrieve());
    ATH_MSG_DEBUG("Retrieved tool " << m_trackSummary);
    ATH_CHECK(m_materialUpdator.retrieve());
    ATH_MSG_DEBUG("Retrieved tool " << m_materialUpdator);

    ATH_CHECK(m_trackingGeometrySvc.retrieve());
    ATH_MSG_DEBUG("Retrieved Svc " << m_trackingGeometrySvc);

    // need to know which TrackingVolume we are in: indet/calo/spectrometer
    ATH_CHECK(m_trackingVolumesSvc.retrieve());
    ATH_MSG_DEBUG("Retrieved Svc " << m_trackingVolumesSvc);

    m_calorimeterVolume =
        new Trk::Volume(m_trackingVolumesSvc->volume(Trk::ITrackingVolumesSvc::MuonSpectrometerEntryLayer));
    m_indetVolume = new Trk::Volume(m_trackingVolumesSvc->volume(Trk::ITrackingVolumesSvc::CalorimeterEntryLayer));

    // sigma of phi sector for pseudo-measurement constraint
    m_sigmaPhiSector = std::tan(0.125 * M_PI / std::sqrt(12.));

    // create beamAxis and vertexRegion for constrained (projective) track fits
    Amg::Vector3D origin(0., 0., 0.);
    m_perigeeSurface = new Trk::PerigeeSurface(origin);

    AmgSymMatrix(3) beamAxisCovariance;
    beamAxisCovariance.setZero();
    (beamAxisCovariance)(0, 0) = m_vertex2DSigmaRPhi * m_vertex2DSigmaRPhi;
    (beamAxisCovariance)(1, 1) = m_vertex2DSigmaRPhi * m_vertex2DSigmaRPhi;
    (beamAxisCovariance)(2, 2) = m_vertex2DSigmaZ * m_vertex2DSigmaZ;
    m_beamAxis                 = new Trk::RecVertex(origin, beamAxisCovariance);

    AmgSymMatrix(3) vertexRegionCovariance;
    vertexRegionCovariance.setZero();
    (vertexRegionCovariance)(0, 0) = m_vertex3DSigmaRPhi * m_vertex3DSigmaRPhi;
    (vertexRegionCovariance)(1, 1) = m_vertex3DSigmaRPhi * m_vertex3DSigmaRPhi;
    (vertexRegionCovariance)(2, 2) = m_vertex3DSigmaZ * m_vertex3DSigmaZ;
    m_vertex                       = new Trk::RecVertex(origin, vertexRegionCovariance);

#ifndef NDEBUG
    ATH_MSG_DEBUG(" vertex region: ");
    m_vertex->dump(msg(MSG::DEBUG));
#endif

    if (!m_trackingGeometrySvc) {
        m_perigeeAtSpectrometerEntranceLocal = false;
        // missing TrackingGeometrySvc - no perigee will be added at MS entrance
        m_messageHelper->printWarning(41);
    } else {
        const Trk::TrackingGeometry* trkGeo = m_trackingGeometrySvc->trackingGeometry();
        if (trkGeo) m_spectrometerEntrance = trkGeo->trackingVolume("MuonSpectrometerEntrance");
    }

    return StatusCode::SUCCESS;
}


StatusCode
CombinedMuonTrackBuilder::finalize()
{
    if (m_countAcceptedStandaloneFit) {
        double norm = 100. / static_cast<double>(m_countAcceptedStandaloneFit);
        ATH_MSG_INFO("Finalizing CombinedMuonTrackBuilder:"
                     << endmsg << "     " << std::setiosflags(std::ios::fixed) << std::setw(4) << std::setprecision(2)
                     << norm * static_cast<double>(m_countBeamAxis) << "% with beamAxis constraint" << endmsg << "     "
                     << std::setw(4) << std::setprecision(2) << norm * static_cast<double>(m_countVertexRegion)
                     << "% with vertexRegion constraint" << endmsg << "     " << m_countDegradedStandaloneFit
                     << " degraded standalone fit-chi2   " << endmsg << "     " << m_countStandaloneCleanerVeto
                     << " standalone fits with cleaner veto" << endmsg << "     " << m_countExtensionCleanerVeto
                     << " extension fits with cleaner veto" << endmsg << "     " << m_countCombinedCleanerVeto
                     << " combined fits with cleaner veto");
    }

    // summarize WARNINGs
    m_messageHelper->printSummary();

    delete m_beamAxis;
    delete m_calorimeterVolume;
    delete m_indetVolume;
    delete m_messageHelper;
    delete m_perigeeSurface;
    delete m_vertex;

    return AthAlgTool::finalize();
}


/** ICombinedMuonTrackBuilder interface: build and fit combined ID/Calo/MS track */
Trk::Track*
CombinedMuonTrackBuilder::combinedFit(const Trk::Track& indetTrack, const Trk::Track& extrapolatedTrack,
                                      const Trk::Track&) const
{
    ATH_MSG_VERBOSE("===== Start of combinedFit:: ");

    countAEOTs(&extrapolatedTrack, " extrapolatedTrack start combinedFit ");

    // require MeasuredPerigee for indetTrack
    const Trk::Perigee* indetPerigee = indetTrack.perigeeParameters();
    if (!indetPerigee) {
        // missing MeasuredPerigee for indet track
        m_messageHelper->printWarning(0);
        return nullptr;
    }

    // take inner calorimeter scattering surface from extrapolated track
    const Trk::Surface* surface = nullptr;
    if (m_trackQuery->isCaloAssociated(extrapolatedTrack)) {

        auto it    = extrapolatedTrack.trackStateOnSurfaces()->begin();
        auto itEnd = extrapolatedTrack.trackStateOnSurfaces()->end();

        for (; it != itEnd; ++it) {

            if (!(**it).materialEffectsOnTrack()) continue;

            Amg::Vector3D position = (**it).materialEffectsOnTrack()->associatedSurface().globalReferencePoint();

            if (m_indetVolume->inside(position)) continue;
            if (!m_calorimeterVolume->inside(position)) break;

            surface = &(**it).materialEffectsOnTrack()->associatedSurface();
            break;
        }
    }

    // match extrapolated indet track to inner calorimeter scattering surface
    // provided momentum defined (solenoid on)
    MagField::AtlasFieldCache fieldCache;
    // Get field cache object
    EventContext                               ctx = Gaudi::Hive::currentContext();
    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, ctx};
    const AtlasFieldCacheCondObj*              fieldCondObj{*readHandle};

    if (fieldCondObj == nullptr) {
        ATH_MSG_ERROR("SCTSiLorentzAngleCondAlg : Failed to retrieve AtlasFieldCacheCondObj with key "
                      << m_fieldCacheCondObjInputKey.key());
        return nullptr;
    }
    fieldCondObj->getInitializedCache(fieldCache);

    if (surface && fieldCache.solenoidOn() && !m_updateWithCaloTG) {
        const Trk::TrackStateOnSurface* innerTSOS = nullptr;

        if (m_useCaloTG) {
            ATH_MSG_VERBOSE(" Retriving Calorimeter TSOS from " << __func__ << " at line " << __LINE__);

            std::vector<const Trk::TrackStateOnSurface*>* caloTSOS =
                m_materialUpdator->getCaloTSOS(*indetTrack.perigeeParameters(), extrapolatedTrack);

            if (caloTSOS) {
                if (!caloTSOS->empty()) {
                    innerTSOS                                                          = caloTSOS->at(0);
                    std::vector<const Trk::TrackStateOnSurface*>::const_iterator it    = caloTSOS->begin() + 1;
                    std::vector<const Trk::TrackStateOnSurface*>::const_iterator itEnd = caloTSOS->end();

                    for (; it != itEnd; ++it) delete *it;
                }
                delete caloTSOS;
            }
        } else {
            innerTSOS = m_caloTSOS->innerTSOS(*indetTrack.perigeeParameters());
        }

        if (!innerTSOS) {
            ATH_MSG_DEBUG(" indet track fails to intersect the calorimeter ");
            return nullptr;
        }

        // will re-associate the calorimeter if they are not the same surface
        double surfaceOffset = (surface->globalReferencePoint()
                                - innerTSOS->materialEffectsOnTrack()->associatedSurface().globalReferencePoint())
                                   .mag();

        delete innerTSOS;

        if (surfaceOffset > 1. * Gaudi::Units::mm) {
            ATH_MSG_DEBUG(" different inner-calo-surface obtained from indet extrapolation, "
                          << "surface reference " << surface->globalReferencePoint() << " with offset " << surfaceOffset
                          << "mm. Re-evaluate the caloTSOS ");

            surface = nullptr;
        }
    }

    Trk::Track* muonTrack = nullptr;

    if (!fieldCache.toroidOn()) {
        ATH_MSG_VERBOSE(" SL MS track: Calling createMuonTrack from " << __func__ << " at line " << __LINE__);
        muonTrack = createMuonTrack(
            indetTrack, indetTrack.perigeeParameters(), nullptr, extrapolatedTrack.trackStateOnSurfaces()->begin(),
            extrapolatedTrack.trackStateOnSurfaces()->end(), extrapolatedTrack.trackStateOnSurfaces()->size());
    } else {
        // create a muon track without perigee in case of non-optimal precision -
        // such as need to replace calorimeter material or presence of pseudomeasurements
        if (!surface) {  // extrapolate outwards to associate calorimeter material effects
            ATH_MSG_VERBOSE("Calling createMuonTrack from " << __func__ << " at line " << __LINE__);
            muonTrack = createMuonTrack(extrapolatedTrack, indetTrack.perigeeParameters(), nullptr,
                                        extrapolatedTrack.trackStateOnSurfaces()->begin(),
                                        extrapolatedTrack.trackStateOnSurfaces()->end(),
                                        extrapolatedTrack.trackStateOnSurfaces()->size());
        } else if (m_trackQuery->numberPseudoMeasurements(extrapolatedTrack) > 1) {  // remove pseudo meas
            ATH_MSG_VERBOSE("Calling createMuonTrack from " << __func__ << " at line " << __LINE__);
            muonTrack = createMuonTrack(
                extrapolatedTrack, nullptr, nullptr, extrapolatedTrack.trackStateOnSurfaces()->begin(),
                extrapolatedTrack.trackStateOnSurfaces()->end(), extrapolatedTrack.trackStateOnSurfaces()->size());
        } else {  // otherwise can just copy the extrapolated track
            ATH_MSG_VERBOSE("Calling createMuonTrack from " << __func__ << " at line " << __LINE__);
            muonTrack = createMuonTrack(extrapolatedTrack, extrapolatedTrack.perigeeParameters(), nullptr,
                                        extrapolatedTrack.trackStateOnSurfaces()->begin(),
                                        extrapolatedTrack.trackStateOnSurfaces()->end(),
                                        extrapolatedTrack.trackStateOnSurfaces()->size());
        }
    }

    // no combined muon when failure to intersect calo
    if (!muonTrack) return nullptr;

    countAEOTs(muonTrack, " muonTrack track before fit ");

    // combined track fit
    Trk::Track* combinedTrack = fit(indetTrack, *muonTrack, m_cleanCombined, Trk::muon);

    // quit if fit failure or all MS measurements removed by fit or perigee outside indet
    bool haveMS         = false;
    bool perigeeOutside = false;

    if (combinedTrack) {
        countAEOTs(combinedTrack, " combinedTrack track after fit ");
        ATH_MSG_VERBOSE(" combined track " << m_printer->print(*combinedTrack) << std::endl
                                           << m_printer->printStations(*combinedTrack));

        auto rit    = combinedTrack->trackStateOnSurfaces()->rbegin();
        auto ritEnd = combinedTrack->trackStateOnSurfaces()->rend();
        for (; rit != ritEnd; ++rit) {
            if (!(**rit).measurementOnTrack() || !(**rit).trackParameters()) continue;
            if (m_calorimeterVolume->inside((**rit).trackParameters()->position())) break;
            if (!(**rit).type(Trk::TrackStateOnSurface::Outlier)) haveMS = true;
        }

        if (!haveMS) {
            // combinedTrack fails: MS removed by cleaner
            m_messageHelper->printWarning(1);
        }

        if (!combinedTrack->perigeeParameters()
            || !m_indetVolume->inside(combinedTrack->perigeeParameters()->position())) {

            if (!combinedTrack->perigeeParameters()) {
                ATH_MSG_DEBUG(" no perigee");
            } else {
                ATH_MSG_DEBUG("  position: r " << combinedTrack->perigeeParameters()->position().perp() << " z "
                                               << combinedTrack->perigeeParameters()->position().z());
            }
            // combinedTrack fails as perigee outside indet
            m_messageHelper->printWarning(2);
            perigeeOutside = true;
        }
    }

    if (!combinedTrack || !combinedTrack->fitQuality() || !haveMS || perigeeOutside) {

        bool hasFitQ = combinedTrack ? (combinedTrack->fitQuality() != nullptr) : false;
        ATH_MSG_DEBUG("combinedTrack fails with bad fit" << combinedTrack << " " << hasFitQ << " " << haveMS << " "
                                                         << perigeeOutside);

        delete combinedTrack;
        delete muonTrack;
        return nullptr;
    }

    // Get parameters at calo position
    const Trk::TrackParameters* combinedEnergyParameters = nullptr;
    const Trk::TrackParameters* muonEnergyParameters     = nullptr;
    const CaloEnergy*           caloEnergy =
        caloEnergyParameters(combinedTrack, muonTrack, combinedEnergyParameters, muonEnergyParameters);

    if (!caloEnergy) {
        // combinedTrack fails with missing caloEnergy
        m_messageHelper->printWarning(3);
        delete combinedTrack;
        delete muonTrack;
        return nullptr;
    }

    // if significant momentum change: re-evaluate calo energy and refit
    double pRatio = muonEnergyParameters->momentum().mag() / combinedEnergyParameters->momentum().mag();
    if (std::abs(pRatio - 1.) > m_largeMomentumChange || m_iterateCombinedTrackFit) {
        if (!m_iterateCombinedTrackFit) {
            ATH_MSG_DEBUG(
                " iterate combined fit to recollect calorimeter material as significant momentum change after fit "
                << pRatio << ", pT before " << muonEnergyParameters->momentum().perp() / Units::GeV << ", after "
                << combinedEnergyParameters->momentum().perp() / Units::GeV << " GeV");
        } else {
            ATH_MSG_DEBUG(" iterate combined fit to recollect calorimeter material");
        }

        const DataVector<const Trk::TrackStateOnSurface>* combinedTSOS = combinedTrack->trackStateOnSurfaces();

        Trk::Track* indetNewTrack = createIndetTrack(indetTrack.info(), combinedTSOS->begin(), combinedTSOS->end());

        Trk::Track* oldTrack = muonTrack;

        ATH_MSG_VERBOSE("Calling createMuonTrack from " << __func__ << " at line " << __LINE__);
        muonTrack = createMuonTrack(extrapolatedTrack, combinedEnergyParameters, nullptr, combinedTSOS->begin(),
                                    combinedTSOS->end(), combinedTSOS->size());

        if (indetNewTrack && muonTrack) {
            Trk::Track* refittedTrack = fit(*indetNewTrack, *muonTrack, m_cleanCombined, Trk::muon);
            caloEnergy = caloEnergyParameters(refittedTrack, muonTrack, combinedEnergyParameters, muonEnergyParameters);

            if (caloEnergy) {
                delete combinedTrack;
                delete oldTrack;
                combinedTrack = refittedTrack;
            } else {
                // why does the refit fail? This shouldn't really be necessary
                delete muonTrack;
                muonTrack = oldTrack;

                caloEnergy =
                    caloEnergyParameters(combinedTrack, muonTrack, combinedEnergyParameters, muonEnergyParameters);
            }
        }

        delete indetNewTrack;
    }

    // tracks with caloEnergy type 'tail' can arise from an incorrect categorization as isolated
    // in case of significant energy gain, switch to parametrization and reclassify as NotIsolated
    if (muonTrack && caloEnergy->energyLossType() == CaloEnergy::Tail && (!m_updateWithCaloTG || m_useCaloTG)) {
        double tolerance = 0;

        if (!indetPerigee->covariance()) {
            ATH_MSG_WARNING(" indetPerigee has no covariance tolerance left as zero. ");
        } else {
            tolerance = m_numberSigmaFSR * sqrt((*indetPerigee->covariance())(Trk::qOverP, Trk::qOverP));
        }

        double indetMaxE     = 1. / (std::abs(indetPerigee->parameters()[Trk::qOverP]) - tolerance);
        double energyBalance = combinedEnergyParameters->momentum().mag() + caloEnergy->deltaE() - indetMaxE;

        // get parametrised eloss if large energy imbalance and refit track
        CaloEnergy* paramEnergy = nullptr;
        if (indetMaxE > 0. && energyBalance > m_numberSigmaFSR * caloEnergy->sigmaMinusDeltaE()) {
            // parametrized energy deposition
            // run-2 schema, update default eloss with parametrised value
            if (m_useCaloTG) {
                paramEnergy = m_materialUpdator->getParamCaloELoss(muonTrack);
            } else {
                // run-1 schema, recalculate parametrised eloss
                paramEnergy = m_caloEnergyParam->energyLoss(combinedEnergyParameters->momentum().mag(),
                                                            combinedEnergyParameters->position().eta(),
                                                            combinedEnergyParameters->position().phi());
            }
            paramEnergy->set_energyLossType(CaloEnergy::NotIsolated);
        }

        // FIXME: add criterion on energy-balance significance param vs tail ?
        if (paramEnergy) {
            ATH_MSG_DEBUG(" FSR check: energyBalance "
                          << energyBalance / Units::GeV << " signif " << energyBalance / caloEnergy->sigmaMinusDeltaE()
                          << " indet max E " << indetMaxE / Units::GeV << std::endl
                          << " param CaloEnergy: " << paramEnergy->deltaE() / Units::GeV << " + "
                          << paramEnergy->sigmaPlusDeltaE() / Units::GeV << " for P "
                          << combinedEnergyParameters->momentum().mag() / Units::GeV << "  eta "
                          << combinedEnergyParameters->position().eta() << "  phi "
                          << combinedEnergyParameters->position().phi() << endmsg << " tail-param energy diff "
                          << (caloEnergy->deltaE() - paramEnergy->deltaE()) / Units::GeV);

            Trk::Track* oldTrack = muonTrack;
            ATH_MSG_VERBOSE("Calling createMuonTrack from " << __func__ << " at line " << __LINE__);
            muonTrack =
                createMuonTrack(extrapolatedTrack, nullptr, paramEnergy, oldTrack->trackStateOnSurfaces()->begin(),
                                oldTrack->trackStateOnSurfaces()->end(), oldTrack->trackStateOnSurfaces()->size());
            delete oldTrack;

            if (muonTrack) {
                Trk::Track* refittedTrack = fit(indetTrack, *muonTrack, m_cleanCombined, Trk::muon);
                if (refittedTrack) {
                    delete combinedTrack;
                    combinedTrack = refittedTrack;
                }
            }
        }
    }

    // in case of the unexpected ...
    delete muonTrack;
    if (!combinedTrack) {
        // final combined track lost, this should not happen
        m_messageHelper->printWarning(4);
        return nullptr;
    }

    if (m_refineELossCombinedTrackFit) {
        ATH_MSG_VERBOSE("Refining Calorimeter TSOS in Muon Combined Fit ...");
        m_materialUpdator->updateCaloTSOS(*combinedTrack);
    }


    //  adds uncertainties and removes AEOTs
    Trk::Track* newTrack = addIDMSerrors(combinedTrack);

    // recollect eloss for combined track and refit
    if (newTrack) {
        countAEOTs(newTrack, " combinedTrack after addIDMSerrors ");
        // Don't run the outliers anymore at this stage
        dumpCaloEloss(newTrack, "CB input TSOS after refine IDMS ");
        Trk::Track* refittedTrack = fit(*newTrack, false, Trk::muon);
        if (refittedTrack) countAEOTs(refittedTrack, " CB fit after refit ");
        if (refittedTrack) dumpCaloEloss(refittedTrack, "CB refit after refine IDMS ");
        if (newTrack != combinedTrack) delete newTrack;

        if (refittedTrack && refittedTrack->fitQuality() && checkTrack("combinedFit", refittedTrack, combinedTrack)) {
            delete combinedTrack;
            combinedTrack = refittedTrack;
        } else {
            if (refittedTrack) delete refittedTrack;
        }
    }

    // hole recovery, error optimization, attach TrackSummary
    finalTrackBuild(combinedTrack);

    return combinedTrack;
}


/** ICombinedMuonTrackBuilder interface:
    build and fit indet track extended to include MS Measurement set.
    Adds material effects as appropriate plus calo energy-loss treatment */
Trk::Track*
CombinedMuonTrackBuilder::indetExtension(const Trk::Track&           indetTrack,
                                         const Trk::MeasurementSet&  spectrometerMeasurements,
                                         const Trk::TrackParameters* innerParameters,
                                         const Trk::TrackParameters* middleParameters,
                                         const Trk::TrackParameters* outerParameters) const
{
    if (msgLvl(MSG::VERBOSE)) {
        msg(MSG::VERBOSE) << endmsg << "indetExtension fit:: " << std::setiosflags(std::ios::fixed);

        if (innerParameters || middleParameters || outerParameters) {

            msg(MSG::VERBOSE) << " parameters at R,Z ";

            if (innerParameters) {
                msg(MSG::VERBOSE) << "I:" << std::setw(5) << std::setprecision(0) << innerParameters->position().perp()
                                  << "," << std::setw(5) << std::setprecision(0) << innerParameters->position().z()
                                  << "  ";
            }

            if (middleParameters) {
                msg(MSG::VERBOSE) << "M:" << std::setw(5) << std::setprecision(0) << middleParameters->position().perp()
                                  << "," << std::setw(5) << std::setprecision(0) << middleParameters->position().z()
                                  << "  ";
            }

            if (outerParameters) {
                msg(MSG::VERBOSE) << "O:" << std::setw(6) << std::setprecision(0) << outerParameters->position().perp()
                                  << "," << std::setw(5) << std::setprecision(0) << outerParameters->position().z();
            }

            msg(MSG::VERBOSE) << " with P ";

            if (innerParameters) {
                msg(MSG::VERBOSE) << std::setw(9) << std::setprecision(3)
                                  << innerParameters->momentum().mag() / Units::GeV;
            }

            if (middleParameters) {
                msg(MSG::VERBOSE) << std::setw(9) << std::setprecision(3)
                                  << middleParameters->momentum().mag() / Units::GeV;
            }

            if (outerParameters) {
                msg(MSG::VERBOSE) << std::setw(9) << std::setprecision(3)
                                  << outerParameters->momentum().mag() / Units::GeV;
            }

            msg(MSG::VERBOSE) << " (GeV)" << endmsg;
        } else {
            msg(MSG::VERBOSE) << " without parameters" << endmsg;
        }
    }

    // propagate appropriate trackParameters to front, back and middle measurements
    // fail when solenoid off and toroid on (as extrapolation from ID is not the correct strategy)
    ToolHandle<Trk::IPropagator> propagator = m_propagatorSL;

    MagField::AtlasFieldCache fieldCache;
    // Get field cache object
    EventContext                               ctx = Gaudi::Hive::currentContext();
    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, ctx};
    const AtlasFieldCacheCondObj*              fieldCondObj{*readHandle};

    if (fieldCondObj == nullptr) {
        ATH_MSG_ERROR("SCTSiLorentzAngleCondAlg : Failed to retrieve AtlasFieldCacheCondObj with key "
                      << m_fieldCacheCondObjInputKey.key());
        return 0;
    }
    fieldCondObj->getInitializedCache(fieldCache);

    if (fieldCache.toroidOn()) {
        // fail when solenoid off and toroid on - as extrapolation from ID is not the correct strategy
        //   for material effects, fit starting value etc
        if (!fieldCache.solenoidOn()) {
            ATH_MSG_VERBOSE("indetExtension: method switched off when solenoid 'off' / toroid 'on'");
            return nullptr;
        }

        propagator = m_propagator;
    }

    const Trk::TrackParameters* frontParameters = nullptr;
    const Trk::TrackParameters* backParameters  = nullptr;

    if (innerParameters) {
        if (innerParameters->associatedSurface() == spectrometerMeasurements.front()->associatedSurface()) {
            frontParameters = innerParameters->clone();
        } else {
            frontParameters =
                m_propagator->propagate(*innerParameters, spectrometerMeasurements.front()->associatedSurface(),
                                        Trk::anyDirection, false, m_magFieldProperties, Trk::muon);
        }
    } else if (middleParameters) {
        if (middleParameters->associatedSurface() == spectrometerMeasurements.front()->associatedSurface()) {
            frontParameters = middleParameters->clone();
        } else {
            frontParameters =
                m_propagator->propagate(*middleParameters, spectrometerMeasurements.front()->associatedSurface(),
                                        Trk::anyDirection, false, m_magFieldProperties, Trk::muon);
        }
    }

    if (outerParameters) {
        if (outerParameters->associatedSurface() == spectrometerMeasurements.back()->associatedSurface()) {
            backParameters = outerParameters->clone();
        } else {

            backParameters =
                m_propagator->propagate(*outerParameters, spectrometerMeasurements.back()->associatedSurface(),
                                        Trk::anyDirection, false, m_magFieldProperties, Trk::muon);
        }
    } else if (middleParameters) {
        if (middleParameters->associatedSurface() == spectrometerMeasurements.back()->associatedSurface()) {
            backParameters = middleParameters->clone();
        } else {
            backParameters =
                m_propagator->propagate(*middleParameters, spectrometerMeasurements.back()->associatedSurface(),
                                        Trk::anyDirection, false, m_magFieldProperties, Trk::muon);
        }
    }

    // find middle measurement
    const Trk::TrackParameters* midParameters  = nullptr;
    const Trk::MeasurementBase* midMeasurement = nullptr;

    if (middleParameters && innerParameters && outerParameters) {
        Amg::Vector3D direction        = (outerParameters->position() - innerParameters->position()).unit();
        double        midDistance      = 0.5 * direction.dot(outerParameters->position() - innerParameters->position());
        double        previousDistance = 0.;

        Trk::MeasurementSet::const_iterator m = spectrometerMeasurements.begin();
        for (++m; m != spectrometerMeasurements.end(); ++m) {
            double distance = direction.dot((**m).globalPosition() - innerParameters->position());
            if (distance < midDistance) {
                previousDistance = distance;
            } else {
                if (midDistance - previousDistance < distance - midDistance) --m;

                midParameters = m_propagator->propagate(*middleParameters, (**m).associatedSurface(), Trk::anyDirection,
                                                        false, m_magFieldProperties, Trk::muon);

                if (midParameters) midMeasurement = *m;
                break;
            }
        }
    }

    // create muon track from spectrometer measurements
    const Trk::FitQualityOnSurface*                                         fitQoS          = nullptr;
    const Trk::MaterialEffectsOnTrack*                                      materialEffects = nullptr;
    const Trk::TrackParameters*                                             trackParameters = nullptr;
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typeM;
    typeM.set(Trk::TrackStateOnSurface::Measurement);
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typeP;
    typeP.set(Trk::TrackStateOnSurface::Measurement);
    typeP.set(Trk::TrackStateOnSurface::Parameter);

    DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>;

    trackStateOnSurfaces->reserve(spectrometerMeasurements.size());

    // append the spectrometer measurements
    auto m    = spectrometerMeasurements.begin();
    auto mEnd = spectrometerMeasurements.end();
    for (; m != mEnd; ++m) {
        if (frontParameters && *m == spectrometerMeasurements.front()) {
            trackStateOnSurfaces->push_back(
                new const Trk::TrackStateOnSurface((**m).clone(), frontParameters, fitQoS, materialEffects, typeP));

            frontParameters = nullptr;
        } else if (*m == midMeasurement) {
            trackStateOnSurfaces->push_back(
                new const Trk::TrackStateOnSurface((**m).clone(), midParameters, fitQoS, materialEffects, typeP));
        } else if (backParameters && *m == spectrometerMeasurements.back()) {
            trackStateOnSurfaces->push_back(
                new const Trk::TrackStateOnSurface((**m).clone(), backParameters, fitQoS, materialEffects, typeP));
        } else {
            trackStateOnSurfaces->push_back(
                new const Trk::TrackStateOnSurface((**m).clone(), trackParameters, fitQoS, materialEffects, typeM));
        }
    }

    Trk::TrackInfo trackInfo(Trk::TrackInfo::Unknown, Trk::muon);

    Trk::Track muonTrack(trackInfo, trackStateOnSurfaces, nullptr);
    countAEOTs(&muonTrack, " in detExtension muonTrack ");

    // perform combined fit
    ATH_MSG_VERBOSE("Calling combinedFit from " << __func__ << " at line " << __LINE__);
    Trk::Track* combinedTrack = combinedFit(indetTrack, muonTrack, muonTrack);

    // check some MS measurements really got added
    unsigned numberMS = 0;
    if (combinedTrack) {
        auto r    = combinedTrack->trackStateOnSurfaces()->rbegin();
        auto rEnd = combinedTrack->trackStateOnSurfaces()->rend();

        for (; r != rEnd; ++r) {
            if ((**r).measurementOnTrack()) ++numberMS;
            if ((**r).trackParameters() && m_calorimeterVolume->inside((**r).trackParameters()->position())) break;
        }

        ATH_MSG_VERBOSE("indetExtension:  " << numberMS << " fitted MS measurements ");
        if (numberMS < 5) {
            // reject with insufficient MS measurements
            ATH_MSG_DEBUG("indetExtension:: reject with insufficient MS measurements");
            delete combinedTrack;
            combinedTrack = nullptr;
        } else {
            countAEOTs(combinedTrack, " in detExtension final combinedTrack ");
        }
    }

    return combinedTrack;
}


/** ICombinedMuonTrackBuilder interface:
    propagate to perigee adding calo energy-loss and material to MS track */
Trk::Track*
CombinedMuonTrackBuilder::standaloneFit(const Trk::Track& inputSpectrometerTrack, const Trk::Vertex* inputVertex,
                                        float bs_x, float bs_y, float bs_z) const
{

    MagField::AtlasFieldCache fieldCache;
    // Get field cache object
    EventContext                               ctx = Gaudi::Hive::currentContext();
    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, ctx};
    const AtlasFieldCacheCondObj*              fieldCondObj{*readHandle};

    if (fieldCondObj == nullptr) {
        ATH_MSG_ERROR("SCTSiLorentzAngleCondAlg : Failed to retrieve AtlasFieldCacheCondObj with key "
                      << m_fieldCacheCondObjInputKey.key());
        return 0;
    }
    fieldCondObj->getInitializedCache(fieldCache);

    // no SA fit with vertex constraint for Toroid off data
    if (m_trackQuery->isLineFit(inputSpectrometerTrack) && !fieldCache.toroidOn()) {
        return nullptr;
    }

    if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << " standaloneFit beam position bs_x " << bs_x << " bs_y " << bs_y << " bs_z " << bs_z
                        << " inputVertex " << inputVertex << endmsg;
    }

    if (msgLvl(MSG::VERBOSE)) {
        msg(MSG::VERBOSE) << endmsg << "==== Start of standaloneFit:: " << std::setiosflags(std::ios::fixed);

        if (m_trackQuery->isExtrapolated(inputSpectrometerTrack)) {
            if (m_trackQuery->isLineFit(inputSpectrometerTrack)) {
                msg(MSG::VERBOSE) << "extrapolated has lineFit";
            } else {
                msg(MSG::VERBOSE) << "extrapolated momentum " << std::setprecision(1)
                                  << inputSpectrometerTrack.perigeeParameters()->momentum().mag() / Units::GeV
                                  << " (GeV)";
            }

            msg(MSG::VERBOSE) << " at eta " << std::setw(6) << std::setprecision(3)
                              << inputSpectrometerTrack.perigeeParameters()->momentum().eta() << "  phi "
                              << std::setw(6) << std::setprecision(3)
                              << inputSpectrometerTrack.perigeeParameters()->momentum().phi();

        } else if (!m_trackQuery->isProjective(inputSpectrometerTrack)) {
            msg(MSG::VERBOSE) << "spectrometer track does not project";
        } else if (inputSpectrometerTrack.perigeeParameters()) {
            if (m_trackQuery->isLineFit(inputSpectrometerTrack)) {
                msg(MSG::VERBOSE) << "spectrometer has lineFit";
            } else {
                msg(MSG::VERBOSE) << "spectrometer momentum " << std::setprecision(1)
                                  << inputSpectrometerTrack.perigeeParameters()->momentum().mag() / Units::GeV
                                  << " (GeV)";
            }

            msg(MSG::VERBOSE) << " at eta " << std::setw(6) << std::setprecision(3)
                              << inputSpectrometerTrack.perigeeParameters()->position().eta() << "  phi "
                              << std::setw(6) << std::setprecision(3)
                              << inputSpectrometerTrack.perigeeParameters()->position().phi();

            if (inputSpectrometerTrack.perigeeParameters()->covariance()) {
                msg(MSG::VERBOSE) << " hasCov";
            } else {
                msg(MSG::VERBOSE) << " noCov ";
            }
        } else {
            msg(MSG::VERBOSE) << " spectrometer track without PerigeeParameters";
        }

        if (inputSpectrometerTrack.fitQuality()) {
            msg(MSG::VERBOSE) << "  fit: chi2 /DoF " << std::setprecision(2) << normalizedChi2(inputSpectrometerTrack)
                              << " /" << std::setw(2) << inputSpectrometerTrack.fitQuality()->numberDoF();
        }

        if (m_trackQuery->numberPseudoMeasurements(inputSpectrometerTrack)) {
            msg(MSG::VERBOSE) << "   pseudo " << m_trackQuery->numberPseudoMeasurements(inputSpectrometerTrack);
        }

        msg(MSG::VERBOSE) << endmsg;
    }

    // check input vertex OK
    const Trk::RecVertex* vertex = dynamic_cast<const Trk::RecVertex*>(inputVertex);
    if (inputVertex && !vertex) {
        // input vertex fails dynamic_cast
        m_messageHelper->printWarning(6);
        return nullptr;
    }

    // fail input tracks with insufficient measurements or inconsistent structure
    const Trk::FitQuality*                            fitQuality = inputSpectrometerTrack.fitQuality();
    const DataVector<const Trk::TrackStateOnSurface>* tsos       = inputSpectrometerTrack.trackStateOnSurfaces();

    if (!fitQuality || !inputSpectrometerTrack.trackStateOnSurfaces()
        || static_cast<int>(inputSpectrometerTrack.trackStateOnSurfaces()->size()) < fitQuality->numberDoF())
    {

        // count measurements
        int                                                        measurements = 0;
        DataVector<const Trk::TrackStateOnSurface>::const_iterator s            = tsos->begin();
        for (; s != tsos->end(); ++s) {
            if ((**s).type(Trk::TrackStateOnSurface::Measurement) && !(**s).type(Trk::TrackStateOnSurface::Outlier)) {
                ++measurements;
            }
        }

        // ATH_MSG_INFO(" input spectrometer track has " << measurements << " measurements" );
        // insufficient measurements
        if (measurements < 4) {
            m_messageHelper->printWarning(48);
            ATH_MSG_VERBOSE(" SA::failed (1)");
            return nullptr;
        }

        // inconsistent TSOS on input track
        if (fitQuality && measurements < fitQuality->numberDoF() + 4) {
            m_messageHelper->printWarning(49);
            ATH_MSG_VERBOSE(" SA::failed (2)");
            return nullptr;
        }
    }

    // check the track is roughly projective in phi
    if (!m_trackQuery->isExtrapolated(inputSpectrometerTrack) && !m_trackQuery->isProjective(inputSpectrometerTrack)) {

        ATH_MSG_VERBOSE(" SA::failed (3)");
        return nullptr;
    }

    // possibly refit the spectrometer track with material reallocation
    bool              haveSpectrometerRefit = false;
    double            spectrometerFitChi2   = normalizedChi2(inputSpectrometerTrack);
    const Trk::Track* spectrometerFit       = &inputSpectrometerTrack;
    if (!vertex && (m_reallocateMaterial || m_trackQuery->isExtrapolated(inputSpectrometerTrack))) {

        spectrometerFit = reallocateMaterial(inputSpectrometerTrack);
        if (!spectrometerFit) {
            ATH_MSG_VERBOSE(" SA::failed (4)");
            return nullptr;
        }
        haveSpectrometerRefit = true;
    }

    const Trk::Track& spectrometerTrack = *spectrometerFit;

    // require a Perigee from the spectrometer track
    const Trk::Perigee* measuredPerigee = spectrometerTrack.perigeeParameters();

    if (!measuredPerigee || !measuredPerigee->covariance()) {
        // missing MeasuredPerigee for spectrometer track
        m_messageHelper->printWarning(7);

        if (haveSpectrometerRefit) delete spectrometerFit;

        ATH_MSG_VERBOSE(" SA::failed (5)");
        return nullptr;
    }

    // set measured momentum error and starting parameters
    bool badlyDeterminedCurvature = false;

    if ((*measuredPerigee->covariance())(Trk::qOverP, Trk::qOverP) < 0
        || (*measuredPerigee->covariance())(Trk::d0, Trk::d0) < 0
        || (*measuredPerigee->covariance())(Trk::z0, Trk::z0) < 0
        || (*measuredPerigee->covariance())(Trk::theta, Trk::theta) < 0
        || (*measuredPerigee->covariance())(Trk::phi, Trk::phi) < 0)
    {

        ATH_MSG_WARNING("standaloneFit: measuredPerigee has non-positive-definite covariance ");
        ATH_MSG_VERBOSE(" SA::failed (5.5)");
        return nullptr;
    }

    double errorP =
        sqrt(measuredPerigee->momentum().mag2() * (*measuredPerigee->covariance())(Trk::qOverP, Trk::qOverP));

    std::unique_ptr<Trk::RecVertex>      mvertex         = std::make_unique<Trk::RecVertex>(*m_vertex);
    std::unique_ptr<Trk::RecVertex>      mbeamAxis       = std::make_unique<Trk::RecVertex>(*m_beamAxis);
    std::unique_ptr<Trk::PerigeeSurface> mperigeeSurface = std::make_unique<Trk::PerigeeSurface>(*m_perigeeSurface);

    const Trk::TrackParameters* parameters = nullptr;

    if (vertex) {
        // vertex association only makes sense for magnet-on tracks with measured curvature
        if (!fieldCache.toroidOn() || m_trackQuery->isLineFit(spectrometerTrack) || errorP > m_largeMomentumError) {

            ATH_MSG_VERBOSE("standaloneFit: vertex fit not attempted as curvature badly measured");
            ATH_MSG_VERBOSE(" SA::failed (6)");
            return nullptr;
        }
        parameters = new Trk::Perigee(*spectrometerTrack.perigeeParameters());
    } else {
        //
        // update -if needed vertex and beam axis positions
        //
        if (std::abs(bs_x - mvertex->position().x()) > 0.001 || std::abs(bs_y - mvertex->position().y()) > 0.001
            || std::abs(bs_z - mvertex->position().z()) > 0.001)
        {

            // recreate beamAxis and vertexRegion for constrained (projective) track fits

            Amg::Vector3D origin(bs_x, bs_y, bs_z);
            mperigeeSurface = std::make_unique<Trk::PerigeeSurface>(origin);

            AmgSymMatrix(3) beamAxisCovariance;
            beamAxisCovariance.setZero();
            (beamAxisCovariance)(0, 0) = m_vertex2DSigmaRPhi * m_vertex2DSigmaRPhi;
            (beamAxisCovariance)(1, 1) = m_vertex2DSigmaRPhi * m_vertex2DSigmaRPhi;
            (beamAxisCovariance)(2, 2) = m_vertex2DSigmaZ * m_vertex2DSigmaZ;
            mbeamAxis                  = std::make_unique<Trk::RecVertex>(origin, beamAxisCovariance);

            AmgSymMatrix(3) vertexRegionCovariance;
            vertexRegionCovariance.setZero();
            (vertexRegionCovariance)(0, 0) = m_vertex3DSigmaRPhi * m_vertex3DSigmaRPhi;
            (vertexRegionCovariance)(1, 1) = m_vertex3DSigmaRPhi * m_vertex3DSigmaRPhi;
            (vertexRegionCovariance)(2, 2) = m_vertex3DSigmaZ * m_vertex3DSigmaZ;
            mvertex                        = std::make_unique<Trk::RecVertex>(origin, vertexRegionCovariance);
        }

        parameters =
            extrapolatedParameters(badlyDeterminedCurvature, spectrometerTrack, mvertex.get(), mperigeeSurface.get());
    }

    if (!parameters) {
        if (haveSpectrometerRefit) delete spectrometerFit;

        ATH_MSG_VERBOSE(" SA::failed (7)");
        return nullptr;
    }

    // create the spectrometer TSOS's for the extrapolated fit
    std::vector<const Trk::TrackStateOnSurface*>* spectrometerTSOS = createSpectrometerTSOS(spectrometerTrack);

    if (!spectrometerTSOS) {
        delete parameters;
        if (haveSpectrometerRefit) delete spectrometerFit;

        ATH_MSG_VERBOSE(" SA::failed (8)");
        return nullptr;
    }
    const Trk::TrackParameters* caloParameters         = nullptr;
    const Trk::TrackParameters* spectrometerParameters = parameters;

    Trk::ParticleHypothesis particleHypothesis = Trk::muon;

    bool haveFitWithVertex = false;
    bool performPrefit     = false;

    DataVector<const Trk::TrackStateOnSurface>::const_iterator s;

    if (m_redoRots) {
        auto s    = spectrometerTrack.trackStateOnSurfaces()->begin();
        auto sEnd = spectrometerTrack.trackStateOnSurfaces()->end();
        for (; s != sEnd; ++s) {
            if ((**s).measurementOnTrack() && !(**s).trackParameters()) {
                performPrefit = true;
            }
        }
    }

    // badly defined tracks use weak vertex constraint with prefit before calo association
    Trk::Track* prefit = nullptr;

    const Trk::RecVertex* vertexInFit = vertex;

    if (!vertexInFit) {
        double errorPhi = sqrt((*measuredPerigee->covariance())(Trk::phi0, Trk::phi0));

        bool inCSCregion = false;
        if (std::abs(measuredPerigee->momentum().eta()) > 2.0) {
            inCSCregion = true;
        }

        // FIXME: missing prefit case for excessive spectrometer eloss WARNING
        //       spot from line starting approx from vertex??
        if (inCSCregion || m_trackQuery->numberPseudoMeasurements(spectrometerTrack)
            || (fieldCache.toroidOn()
                && (badlyDeterminedCurvature || errorPhi > m_largePhiError
                    || measuredPerigee->momentum().mag() < m_lowMomentum)))
        {
            performPrefit = true;
            vertexInFit   = mbeamAxis.get();
            if (badlyDeterminedCurvature || inCSCregion) {
                vertexInFit = mvertex.get();
            }

            if (msgLvl(MSG::DEBUG)) {
                unsigned numberPseudo = m_trackQuery->numberPseudoMeasurements(spectrometerTrack);
                if (errorPhi > m_sigmaPhiSector) {
                    ++numberPseudo;
                }

                if (badlyDeterminedCurvature) {
                    ATH_MSG_DEBUG(" prefit with vertex: "
                                  << std::setiosflags(std::ios::fixed) << " momentum " << std::setprecision(1)
                                  << measuredPerigee->momentum().mag() / Units::GeV << " (GeV),  zFirst "
                                  << std::setprecision(1) << std::abs(parameters->position().z()) << ",  phiError "
                                  << std::setprecision(2) << errorPhi << ",  momentumError " << std::setprecision(2)
                                  << errorP << ",  numberPseudo " << numberPseudo);
                } else {
                    ATH_MSG_DEBUG(" prefit with beamAxis: "
                                  << std::setiosflags(std::ios::fixed) << " momentum " << std::setprecision(1)
                                  << measuredPerigee->momentum().mag() / Units::GeV << " (GeV),  zFirst "
                                  << std::setprecision(1) << std::abs(parameters->position().z()) << ",  phiError "
                                  << std::setprecision(2) << errorPhi << ",  momentumError " << std::setprecision(2)
                                  << errorP << ",  numberPseudo " << numberPseudo);
                }
            }
        }
    }

    const Trk::Perigee* prefitResult = nullptr;

    // prefit to stabilize calo look-up and/or provide trackParameters
    if (performPrefit) {
        if (!vertexInFit) {
            ATH_MSG_VERBOSE(" prefit without vertex");
        }

        if (vertexInFit) {
            haveFitWithVertex = true;
        }

        if (badlyDeterminedCurvature && parameters->momentum().mag() > m_lowMomentum) {
            particleHypothesis = Trk::nonInteracting;
        }

        ATH_MSG_VERBOSE("Calling createExtrapolatedTrack from " << __func__ << " at line " << __LINE__);
        prefit = createExtrapolatedTrack(spectrometerTrack, *parameters, particleHypothesis, false, *spectrometerTSOS,
                                         vertexInFit, mbeamAxis.get(), mperigeeSurface.get());

        // demand prefit success
        if (!prefit || !prefit->fitQuality() || !prefit->perigeeParameters()) {
            ATH_MSG_DEBUG(" prefit failure ");
            delete prefit;
            prefit = nullptr;
        }

        if (prefit) {
            dumpCaloEloss(prefit, " prefit ");
            bool hasCov = prefit->perigeeParameters() ? (prefit->perigeeParameters()->covariance() != nullptr) : false;
            ATH_MSG_VERBOSE(" got prefit " << m_printer->print(*prefit) << " hasCov " << hasCov);

            if (prefit->perigeeParameters()) {
                prefitResult = prefit->perigeeParameters()->clone();
            }

            for (s = prefit->trackStateOnSurfaces()->begin(); s != prefit->trackStateOnSurfaces()->end(); ++s) {
                // look for first measured TSOS in muon volume
                if (!(**s).trackParameters() || !(**s).trackParameters()->covariance()) {
                    continue;
                }
                if (m_calorimeterVolume->inside((**s).trackParameters()->position())) {
                    continue;
                }

                // check that it is a measurement
                if ((**s).type(Trk::TrackStateOnSurface::Measurement)) {
                    ATH_MSG_DEBUG("Found first parameters in MS " << (**s).trackParameters()->position().perp() << " z "
                                                                  << (**s).trackParameters()->position().z());
                    break;
                }
            }

            if (s != prefit->trackStateOnSurfaces()->end() && s != prefit->trackStateOnSurfaces()->begin()
                && (**s).trackParameters())
            {
                parameters     = (**s).trackParameters()->clone();
                caloParameters = parameters;
            } else {
                // this shouldn't happen, but best be safe...
                delete prefit;
                prefit = nullptr;
                // prefit: no parameter extrapolation to calo
                m_messageHelper->printWarning(9);
            }
        }

        // give up if prefit fails
        spectrometerTSOS->clear();

        if (!prefit) {
            delete spectrometerParameters;
            delete spectrometerTSOS;

            if (haveSpectrometerRefit) delete spectrometerFit;

            ATH_MSG_VERBOSE(" SA::failed (9)");
            delete prefitResult;
            return nullptr;
        }

        // create spectrometerTSOS corresponding to prefit
        //   skip start perigee, then preferentially take everything following MS perigee,
        //   otherwise (if no MS perigee) rely on VolumesSvc,
        //   but be aware that by design there are inconsistencies wrt tracking geometry
        s = prefit->trackStateOnSurfaces()->begin();
        ++s;

        while (m_calorimeterVolume->inside((**s).trackParameters()->position())
               && !(**s).type(Trk::TrackStateOnSurface::Perigee))
        {
            ++s;
        }  // end while

        if ((**s).type(Trk::TrackStateOnSurface::Perigee)) ++s;

        for (; s != prefit->trackStateOnSurfaces()->end(); ++s) {
            spectrometerTSOS->push_back((**s).clone());
        }
    }

    // update rot's (but not from trigger chambers) using TrackParameters
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> defaultType;
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type = defaultType;
    if (m_redoRots) {
        // recalibration: correct rots
        auto t    = spectrometerTSOS->begin();
        auto tEnd = spectrometerTSOS->end();
        for (; t != tEnd; ++t) {
            if (!(**t).measurementOnTrack() || !(**t).trackParameters()) {
                continue;
            }  // end of if

            const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>((**t).measurementOnTrack());

            if (!rot) continue;
            Identifier id = rot->identify();

            if (!m_idHelperSvc->isMuon(id)) continue;

            const Trk::RIO_OnTrack* updatedRot = nullptr;
            if (!m_cscRotCreator.empty() && m_idHelperSvc->isCsc(id)) {
                updatedRot = m_cscRotCreator->correct(*rot->prepRawData(), *(**t).trackParameters());
            } else if (!m_mdtRotCreator.empty() && m_idHelperSvc->isMdt(id)) {
                updatedRot = m_mdtRotCreator->correct(*rot->prepRawData(), *(**t).trackParameters());
            }

            if (updatedRot) {
                const Trk::TrackStateOnSurface* tsos = *t;
                type                                 = defaultType;
                type.set(Trk::TrackStateOnSurface::Measurement);

                if ((**t).type(Trk::TrackStateOnSurface::Outlier)) {
                    type.set(Trk::TrackStateOnSurface::Outlier);
                }

                *t = new const Trk::TrackStateOnSurface(updatedRot, (**t).trackParameters()->clone(), nullptr, nullptr,
                                                        type);
                delete tsos;
            }
        }
    }

    // extrapolate and fit track
    particleHypothesis      = Trk::muon;
    bool returnAfterCleaner = false;

    if (!fieldCache.toroidOn()) returnAfterCleaner = true;

    ATH_MSG_VERBOSE("Calling createExtrapolatedTrack from " << __func__ << " at line " << __LINE__);
    Trk::Track* extrapolated =
        createExtrapolatedTrack(spectrometerTrack, *parameters, particleHypothesis, m_cleanStandalone,
                                *spectrometerTSOS, vertexInFit, mbeamAxis.get(), mperigeeSurface.get(), prefitResult);

    if (extrapolated) dumpCaloEloss(extrapolated, " extrapolated  ");

    // fit problem: try fixup using vertex region or prefit
    if (!extrapolated || !extrapolated->fitQuality()) {
        if (extrapolated && !haveFitWithVertex && !vertexInFit) {
            ATH_MSG_DEBUG(" bad fitQuality: retry with vertex ");
            Trk::Track* badfit = extrapolated;

            DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces =
                new DataVector<const Trk::TrackStateOnSurface>;

            trackStateOnSurfaces->reserve(badfit->trackStateOnSurfaces()->size() + 1);

            s = badfit->trackStateOnSurfaces()->begin();

            type = defaultType;
            type.set(Trk::TrackStateOnSurface::Perigee);

            const Trk::PseudoMeasurementOnTrack* vertexInFit =
                vertexOnTrack(*extrapolated->perigeeParameters(), mvertex.get(), mbeamAxis.get());

            if (vertexInFit) type.set(Trk::TrackStateOnSurface::Measurement);

            trackStateOnSurfaces->push_back(new const Trk::TrackStateOnSurface(
                vertexInFit, extrapolated->perigeeParameters()->clone(), nullptr, nullptr, type));

            for (++s; s != badfit->trackStateOnSurfaces()->end(); ++s) {
                trackStateOnSurfaces->push_back((**s).clone());
            }

            Trk::Track* track = new Trk::Track(spectrometerTrack.info(), trackStateOnSurfaces, nullptr);
            extrapolated      = fit(*track, m_cleanStandalone, particleHypothesis);
            delete badfit;
            delete track;
        }

        // restart from prefit without cleaning
        if (!extrapolated || !extrapolated->fitQuality()) {
            if (prefit && prefit->fitQuality() && caloParameters) {
                ATH_MSG_DEBUG(" restarting from prefit as back extrapolation fit failed");
                spectrometerTSOS->clear();
                // create spectrometerTSOS corresponding to prefit
                s = prefit->trackStateOnSurfaces()->begin();

                while (m_calorimeterVolume->inside((**s).trackParameters()->position())
                       || (**s).type(Trk::TrackStateOnSurface::Perigee))
                {
                    ++s;
                }  // end of while


                for (; s != prefit->trackStateOnSurfaces()->end(); ++s) {
                    spectrometerTSOS->push_back((**s).clone());
                }

                delete extrapolated;

                ATH_MSG_VERBOSE("Calling createExtrapolatedTrack from " << __func__ << " at line " << __LINE__);
                extrapolated = createExtrapolatedTrack(spectrometerTrack, *caloParameters, particleHypothesis, false,
                                                       *spectrometerTSOS, vertexInFit, mbeamAxis.get(),
                                                       mperigeeSurface.get(), prefitResult);

                delete prefit;
                prefit             = nullptr;
                returnAfterCleaner = true;
            }

            if (!extrapolated || !extrapolated->fitQuality()) {
                bool hasFQ = extrapolated ? (extrapolated->fitQuality() != nullptr) : false;
                ATH_MSG_DEBUG("fail track as back extrapolation fit failed " << extrapolated << " hasFQ " << hasFQ);
                delete prefit;
                delete caloParameters;
                delete extrapolated;
                delete spectrometerParameters;
                spectrometerTSOS->clear();
                delete spectrometerTSOS;

                if (haveSpectrometerRefit) delete spectrometerFit;

                ATH_MSG_VERBOSE(" SA::failed (10)");
                delete prefitResult;
                return nullptr;
            }
        }
    }

    // delete prefitResult
    delete prefitResult;
    prefitResult = nullptr;

    // keep statistics for successful fits
    ++m_countAcceptedStandaloneFit;
    if (vertexInFit == mbeamAxis.get()) ++m_countBeamAxis;
    if (vertexInFit == mvertex.get()) ++m_countVertexRegion;

    // refit when there's been a significant momentum change (parameters at last calo scatterer)
    double momentum = parameters->momentum().mag();
    delete caloParameters;
    delete prefit;
    delete spectrometerParameters;
    bool   allowRefit = !badlyDeterminedCurvature;
    double pRatio     = 1.;

    s = extrapolated->trackStateOnSurfaces()->begin();

    if (returnAfterCleaner) {
        allowRefit = false;
    } else {
        // pRatio is the ratio of fitted to start momentum value at calo exit
        //  find parameters at calo exit
        parameters = nullptr;

        while (!(**s).trackParameters() || m_calorimeterVolume->inside((**s).trackParameters()->position())) {
            if ((**s).trackParameters() && !(**s).type(Trk::TrackStateOnSurface::Perigee))
                parameters = (**s).trackParameters();
            ++s;
        }  // end of while

        //  extrapolated fit with missing calo parameters - this should never happen!
        if (parameters) {
            pRatio = momentum / parameters->momentum().mag();
        } else {
            // extrapolated track missing TrackParameters at calo scatterer
            m_messageHelper->printWarning(10);
            allowRefit = false;
        }
    }

    // in case of a significant momentum change: iterate (re-associate calo and refit)
    Trk::Track* track = extrapolated;

    if (allowRefit && std::abs(pRatio - 1.) > m_largeMomentumChange) {
        if (msgLvl(MSG::VERBOSE)) {
            double sinTheta = parameters->momentum().perp() / parameters->momentum().mag();

            ATH_MSG_VERBOSE(" iterate as significant momentum change after fit "
                            << pRatio << ", pT before " << momentum * sinTheta / Units::GeV << ", after "
                            << parameters->momentum().perp() / Units::GeV << " GeV");
        }

        spectrometerTSOS->clear();
        spectrometerTSOS->push_back((**s).clone());
        for (++s; s != extrapolated->trackStateOnSurfaces()->end(); ++s) {
            if (!(**s).type(Trk::TrackStateOnSurface::Perigee)) spectrometerTSOS->push_back((**s).clone());
        }

        ATH_MSG_VERBOSE("Calling createExtrapolatedTrack from " << __func__ << " at line " << __LINE__);

        track = createExtrapolatedTrack(spectrometerTrack, *parameters, particleHypothesis, m_cleanStandalone,
                                        *spectrometerTSOS, vertexInFit, mbeamAxis.get(), mperigeeSurface.get(),
                                        extrapolated->perigeeParameters());

        if (track) {
            double extrapChi2 = normalizedChi2(*extrapolated);
            double fitChi2    = normalizedChi2(*track);
            if (fitChi2 < m_badFitChi2 || fitChi2 < extrapChi2 + 0.5) {
                delete extrapolated;
                extrapolated = nullptr;
            }
        }

        if (extrapolated) {
            delete track;
            track = extrapolated;
        }
    }

    if (!m_trackQuery->isCaloAssociated(*track)) {  // still want to perform this check probably though
        // fail as calo incorrectly described
        m_messageHelper->printWarning(12);
        delete track;
        spectrometerTSOS->clear();
        delete spectrometerTSOS;

        if (haveSpectrometerRefit) {
            delete spectrometerFit;
        }

        ATH_MSG_VERBOSE(" SA::failed (12)");
        return nullptr;
    }

    int improvementsFailed = 0;  // count the number of times the fit fails after improvements


    if (m_refineELossStandAloneTrackFit) {
        ATH_MSG_VERBOSE("Refining Calorimeter TSOS in StandAlone Fit ...");
        std::unique_ptr<Trk::Track> oldTrack = std::make_unique<Trk::Track>(Trk::Track(*track));

        m_materialUpdator->updateCaloTSOS(*track);

        Trk::Track* refinedTrack = fit(*track, false, Trk::muon);
        if (refinedTrack) {
            ATH_MSG_VERBOSE("successfully refit after refining calo TSOS");

            if (checkTrack("refineFit", refinedTrack, track)) {
                ATH_MSG_VERBOSE("refined track checks out");
            }

            delete track;
            track = refinedTrack;
        } else {
            ATH_MSG_VERBOSE("refined track fit failed");
            track = oldTrack.release();
            improvementsFailed++;
        }
    }


    //  adds uncertainties
    Trk::Track* newTrack = addIDMSerrors(track);

    if (newTrack) {
        countAEOTs(newTrack, " SA track after addIDMSerrors ");
        dumpCaloEloss(newTrack, "SA input TSOS after refine IDMS ");

        // Don't run the outliers anymore at this stage
        Trk::Track* refittedTrack = fit(*newTrack, false, Trk::muon);
        if (refittedTrack) {
            countAEOTs(refittedTrack, " SA track after refit ");
        }
        if (refittedTrack) {
            dumpCaloEloss(refittedTrack, " SA refit after refine IDMS ");
        }

        if (newTrack != track) delete newTrack;

        if (refittedTrack && refittedTrack->fitQuality() && checkTrack("standaloneFit", refittedTrack, track)) {
            delete track;
            track = refittedTrack;
        } else {
            if (refittedTrack) delete refittedTrack;
            improvementsFailed++;
        }
    } else {
        improvementsFailed++;
    }

    // hole recovery, error optimization, attach TrackSummary
    finalTrackBuild(track);

    if (track) {
        dumpCaloEloss(track, " finalTrackBuild ");
    }

    // report when extrapolated fit quality significantly worse than spectrometer quality
    if (track) {
        double fitChi2 = normalizedChi2(*track);

        if (fitChi2 > m_badFitChi2 && fitChi2 > spectrometerFitChi2 + 0.5) {
            ATH_MSG_DEBUG("standaloneFit: fit quality degraded wrt spectrometer alone. "
                          << " Chi2/DoF= " << fitChi2);

            ++m_countDegradedStandaloneFit;
            if (improvementsFailed == 2) {
                ATH_MSG_WARNING("reject track, quality degraded and improvements failed");
                return nullptr;
            }
        }
    }

    spectrometerTSOS->clear();
    delete spectrometerTSOS;
    if (haveSpectrometerRefit) {
        delete spectrometerFit;
    }
    ATH_MSG_VERBOSE(" SA::ok ");

    return track;
}

/** ICombinedMuonTrackBuilder interface:
    refit a track removing any indet measurements with possible addition of pseudoMeasurements */
Trk::Track*
CombinedMuonTrackBuilder::standaloneRefit(const Trk::Track& combinedTrack, float bs_x, float bs_y, float bs_z) const
{
    //
    // update -if needed vertex and beam axis positions
    //

    countAEOTs(&combinedTrack, " in standalone Refit input combinedTrack ");

    MagField::AtlasFieldCache fieldCache;
    // Get field cache object
    EventContext                               ctx = Gaudi::Hive::currentContext();
    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, ctx};
    const AtlasFieldCacheCondObj*              fieldCondObj{*readHandle};

    if (fieldCondObj == nullptr) {
        ATH_MSG_ERROR("SCTSiLorentzAngleCondAlg : Failed to retrieve AtlasFieldCacheCondObj with key "
                      << m_fieldCacheCondObjInputKey.key());
        return 0;
    }
    fieldCondObj->getInitializedCache(fieldCache);

    if (!fieldCache.toroidOn()) {
        // no standalone refit for Toroid off
        return nullptr;
    }

    if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << " StandaloneRefit beam position bs_x " << bs_x << " bs_y " << bs_y << " bs_z " << bs_z
                        << endmsg;
    }

    // vertex will change track by track
    Amg::Vector3D origin(bs_x, bs_y, bs_z);
    AmgSymMatrix(3) vertexRegionCovariance;
    vertexRegionCovariance.setZero();

    double              error2d0        = m_vertex3DSigmaRPhi * m_vertex3DSigmaRPhi;
    double              error2z0        = m_vertex3DSigmaZ * m_vertex3DSigmaZ;
    const Trk::Perigee* measuredPerigee = combinedTrack.perigeeParameters();

    if (measuredPerigee && measuredPerigee->covariance() && m_useRefitTrackError) {
        error2d0 = (*measuredPerigee->covariance())(Trk::d0, Trk::d0);
        error2z0 = (*measuredPerigee->covariance())(Trk::z0, Trk::z0);

        if (msgLvl(MSG::DEBUG)) {
            msg(MSG::DEBUG) << " StandaloneRefit  new vertex d0 error  " << sqrt(error2d0) << " new vertex z0 error  "
                            << sqrt(error2z0);
        }
    }

    (vertexRegionCovariance)(0, 0) = error2d0;
    (vertexRegionCovariance)(1, 1) = error2d0;
    (vertexRegionCovariance)(2, 2) = error2z0;

    Trk::RecVertex* vertex = new Trk::RecVertex(origin, vertexRegionCovariance);

    if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << " StandaloneRefit  new vertex position x " << vertex->position().x() << " y "
                        << vertex->position().y() << " z " << vertex->position().z();
    }

    bool addPhiPseudo = false;
    // release 21
    unsigned spectrometerPhiQuality = m_trackQuery->spectrometerPhiQuality(combinedTrack);
    if (spectrometerPhiQuality > 1) {
        addPhiPseudo = true;
    }

    if (msgLvl(MSG::VERBOSE)) {
        ATH_MSG_VERBOSE("standaloneRefit: using vertex region constraint with "
                        << "spectrometerPhiQuality " << spectrometerPhiQuality);
    }

    // create standalone track TSOS vector
    DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>;

    // size will allow for perigee + all TSOS outside indet
    unsigned size = combinedTrack.trackStateOnSurfaces()->size() + 2;
    // if (addVertexRegion) ++size;
    ++size;  // in r21, addVertexRegion is always true
    if (addPhiPseudo) {
        ++size;
    }

    trackStateOnSurfaces->reserve(size);

    // position TSOS iterator to be just after the indet
    bool haveCaloDeposit = false;

    DataVector<const Trk::TrackStateOnSurface>::const_iterator s = combinedTrack.trackStateOnSurfaces()->begin();

    do {
        ++s;

        if (s == combinedTrack.trackStateOnSurfaces()->end()) {
            // fail track as no TSOS with type CaloDeposit
            m_messageHelper->printWarning(13);
            delete trackStateOnSurfaces;
            delete vertex;
            return nullptr;
        }
        if ((**s).type(Trk::TrackStateOnSurface::CaloDeposit)) {
            haveCaloDeposit = true;
            --s;
        }
    } while (!haveCaloDeposit);

    // inner calo scatterer - keep scattering angles for vertex constraint
    // Amg::Vector3D direction;
    const Trk::ScatteringAngles*    innerScattering  = nullptr;
    Trk::TrackStateOnSurface*       innerTSOS        = nullptr;
    const Trk::MaterialEffectsBase* materialEffects  = (**s).materialEffectsOnTrack();
    const Trk::TrackParameters*     middleParameters = nullptr;
    const Trk::TrackParameters*     oldParameters    = nullptr;
    const Trk::ScatteringAngles*    outerScattering  = nullptr;
    const Trk::TrackParameters*     parameters       = (**s).trackParameters();

    if (materialEffects && parameters && m_calorimeterVolume->inside(parameters->position())) {
        // keep scattering angles when vertex constrained
        // in r21, addVertexRegion is always true
        // if (addVertexRegion)
        //{
        innerTSOS                               = (**s).clone();
        const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(materialEffects);

        if (!meot) {
            // innerScattering dynamic_cast failed
            m_messageHelper->printWarning(16);
            delete vertex;
            return nullptr;
        }
        innerScattering = meot->scatteringAngles();

        if (s != combinedTrack.trackStateOnSurfaces()->end() && !(**s).type(Trk::TrackStateOnSurface::CaloDeposit)) {

            ++s;
        }

        if (s == combinedTrack.trackStateOnSurfaces()->end()) {
            // no TSOS of type CaloDeposit found
            m_messageHelper->printWarning(17);
            materialEffects = nullptr;
            parameters      = nullptr;
        } else {
            materialEffects  = (**s).materialEffectsOnTrack();
            parameters       = (**s).trackParameters();
            middleParameters = parameters;
        }
    } else {
        // no inner material or parameters
        if (!materialEffects) m_messageHelper->printWarning(14);
        if (!parameters) m_messageHelper->printWarning(15);
    }

    if (!innerTSOS) {
        // no inner scattering TSOS found
        m_messageHelper->printWarning(18);
    }

    // middle calo scatterer (for energy deposit)
    double energyDeposit = 0.;

    Trk::TrackStateOnSurface* middleTSOS = nullptr;

    if (materialEffects && parameters && m_calorimeterVolume->inside(parameters->position())) {
        middleTSOS                              = (**s).clone();
        const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(materialEffects);

        if (meot && meot->energyLoss()) energyDeposit = meot->energyLoss()->deltaE();
        if (s != combinedTrack.trackStateOnSurfaces()->end()) ++s;

        if (s != combinedTrack.trackStateOnSurfaces()->end()) {
            materialEffects = (**s).materialEffectsOnTrack();
            parameters      = (**s).trackParameters();
        } else {
            materialEffects = nullptr;
            parameters      = nullptr;
        }
    } else {
        // no middle material or parameters
        if (!materialEffects) m_messageHelper->printWarning(19);
        if (!parameters) m_messageHelper->printWarning(20);
    }

    if (!middleTSOS) {
        // no CaloDeposit TSOS found
        m_messageHelper->printWarning(21);
    }

    // outer calo scatterer
    Trk::TrackStateOnSurface* outerTSOS = nullptr;

    double pInner = 0.;
    double pOuter = 0.;
    if (materialEffects && parameters && m_calorimeterVolume->inside(parameters->position())) {
        pOuter = parameters->momentum().mag();

        outerTSOS = (**s).clone();

        const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(materialEffects);

        if (!meot) {
            // outerScattering dynamic_cast failed
            m_messageHelper->printWarning(24);
            delete vertex;
            return nullptr;
        }
        outerScattering = meot->scatteringAngles();

        if (s != combinedTrack.trackStateOnSurfaces()->end()) ++s;

        // get parameters at middleSurface for energy correction,
        // start with parameters from middle surface when vertex in fit
        if (outerScattering && middleTSOS) {
            parameters = middleTSOS->trackParameters()->clone();
        } else {
            // otherwise extrapolate outer to middleSurface without scattering correction
            if (!middleTSOS) {
                parameters = nullptr;
                // no outerScattering or CaloDeposit TSOS found
                m_messageHelper->printWarning(25);
            } else {
                parameters =
                    m_propagator->propagate(*parameters, middleTSOS->trackParameters()->associatedSurface(),
                                            Trk::oppositeMomentum, false, m_magFieldProperties, Trk::nonInteracting);
            }
        }

        if (parameters) {
            // corrected parameters (include unfitted calo energy deposit),
            // inner momentum  = outer momentum plus energy deposit
            pInner = pOuter + energyDeposit;
            momentumUpdate(parameters, pInner);
        }
    } else {
        // no outer material or parameters
        if (!materialEffects) m_messageHelper->printWarning(22);
        if (!parameters) m_messageHelper->printWarning(23);
    }

    // fail track if missing any calo surface or extrapolation failure
    if (!innerTSOS || !middleTSOS || !outerTSOS || !parameters) {
        if (outerTSOS) delete parameters;
        delete trackStateOnSurfaces;
        delete innerTSOS;
        delete middleTSOS;
        delete outerTSOS;
        delete vertex;
        return nullptr;
    }

    // create perigee by back extrapolation from middleSurface via innerSurface
    oldParameters = parameters;

    parameters = m_propagator->propagate(*parameters, innerTSOS->trackParameters()->associatedSurface(),
                                         Trk::oppositeMomentum, false, m_magFieldProperties, Trk::nonInteracting);

    delete oldParameters;

    if (innerScattering) {
        momentumUpdate(parameters, pInner, true, -innerScattering->deltaPhi(), -innerScattering->deltaTheta());
    }

    const Trk::Perigee* perigee = nullptr;
    if (parameters) {
        perigee = dynamic_cast<const Trk::Perigee*>(m_propagator->propagate(
            *parameters, *m_perigeeSurface, Trk::oppositeMomentum, false, m_magFieldProperties, Trk::nonInteracting));
    }
    delete parameters;

    // in case of problem above: clone combined perigee
    if (!perigee) perigee = combinedTrack.perigeeParameters()->clone();

    // track back out to the 3 calo surfaces applying small correction for non-linearity
    parameters = m_propagator->propagate(*perigee, innerTSOS->trackParameters()->associatedSurface(),
                                         Trk::alongMomentum, false, m_magFieldProperties, Trk::nonInteracting);

    if (!parameters) {
        // failed propagation to innerTSOS
        m_messageHelper->printWarning(26);
    } else {
        if (innerScattering) {
            momentumUpdate(parameters, pInner, true, innerScattering->deltaPhi(), innerScattering->deltaTheta());
        }

        oldParameters = parameters;

        parameters = m_propagator->propagate(*parameters, middleParameters->associatedSurface(), Trk::alongMomentum,
                                             false, m_magFieldProperties, Trk::nonInteracting);

        delete oldParameters;

        if (!parameters) {
            // failed propagation to middleTSOS
            m_messageHelper->printWarning(27);
        } else {
            momentumUpdate(parameters, pOuter);
            oldParameters = parameters;

            parameters = m_propagator->propagate(*parameters, outerTSOS->trackParameters()->associatedSurface(),
                                                 Trk::alongMomentum, false, m_magFieldProperties, Trk::nonInteracting);

            delete oldParameters;
        }
    }

    if (!parameters) {
        delete trackStateOnSurfaces;
        delete innerTSOS;
        delete middleTSOS;
        delete outerTSOS;
        delete vertex;
        return nullptr;
    }

    if (outerScattering) {
        momentumUpdate(parameters, pOuter, true, outerScattering->deltaPhi(), outerScattering->deltaTheta());
    }

    // small correction term
    double deltaPhi = outerTSOS->trackParameters()->momentum().phi() - parameters->momentum().phi();

    if (deltaPhi > M_PI) deltaPhi -= 2. * M_PI;
    if (deltaPhi < -M_PI) deltaPhi += 2. * M_PI;

    double deltaTheta = outerTSOS->trackParameters()->momentum().theta() - parameters->momentum().theta();

    delete parameters;
    parameters = perigee;
    momentumUpdate(parameters, pInner, true, deltaPhi, deltaTheta);
    perigee = dynamic_cast<const Trk::Perigee*>(parameters);

    // create perigee TSOS
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type;
    type.set(Trk::TrackStateOnSurface::Perigee);

    trackStateOnSurfaces->push_back(new const Trk::TrackStateOnSurface(nullptr, perigee, nullptr, nullptr, type));

    // including vertex region pseudoMeas if requested: in r21, this is always requested
    std::unique_ptr<Trk::RecVertex>      mbeamAxis   = std::make_unique<Trk::RecVertex>(*m_beamAxis);
    const Trk::PseudoMeasurementOnTrack* vertexInFit = vertexOnTrack(*perigee, vertex, mbeamAxis.get());
    if (vertexInFit) {
        std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type;
        type.set(Trk::TrackStateOnSurface::Measurement);

        trackStateOnSurfaces->push_back(
            new const Trk::TrackStateOnSurface(vertexInFit, nullptr, nullptr, nullptr, type));
    }


    DataVector<const Trk::TrackStateOnSurface>::const_iterator t = combinedTrack.trackStateOnSurfaces()->begin();
    if (m_addElossID) {
        double Eloss               = 0.;
        double sigmaEloss          = 0.;
        double X0tot               = 0.;
        double sigmaDeltaPhitot2   = 0.;
        double sigmaDeltaThetatot2 = 0.;

        std::vector<int> tsosnr;
        tsosnr.reserve(combinedTrack.trackStateOnSurfaces()->size());
        int itsos = -1;

        for (; t != combinedTrack.trackStateOnSurfaces()->end(); ++t) {
            itsos++;
            if (!(**t).trackParameters()) continue;

            if ((**t).trackParameters()) {
                if (!m_indetVolume->inside((**t).trackParameters()->position())) break;
            }

            if ((**t).materialEffectsOnTrack()) {
                double X0 = (**t).materialEffectsOnTrack()->thicknessInX0();
                X0tot += X0;

                const Trk::MaterialEffectsOnTrack* meot =
                    dynamic_cast<const Trk::MaterialEffectsOnTrack*>((**t).materialEffectsOnTrack());

                if (meot) {
                    const Trk::EnergyLoss* energyLoss = meot->energyLoss();
                    if (energyLoss) {
                        Eloss += energyLoss->deltaE();
                        sigmaEloss += energyLoss->sigmaDeltaE();

                        ATH_MSG_DEBUG("CombinedMuonFit ID Eloss found r "
                                      << ((**t).trackParameters())->position().perp() << " z "
                                      << ((**t).trackParameters())->position().z() << " value " << energyLoss->deltaE()
                                      << " Eloss " << Eloss << " sigma Eloss " << energyLoss->sigmaDeltaE() << " X0 "
                                      << X0);

                        const Trk::ScatteringAngles* scat = meot->scatteringAngles();
                        if (scat) {
                            double sigmaDeltaPhi   = scat->sigmaDeltaPhi();
                            double sigmaDeltaTheta = scat->sigmaDeltaTheta();
                            sigmaDeltaPhitot2 += sigmaDeltaPhi * sigmaDeltaPhi;
                            sigmaDeltaThetatot2 += sigmaDeltaTheta * sigmaDeltaTheta;
                            tsosnr.push_back(itsos);
                        }
                    }
                }
            }
        }

        ATH_MSG_DEBUG("standaloneRefit Total ID Eloss " << Eloss << " sigma Eloss " << sigmaEloss << " X0 " << X0tot
                                                        << " sigma scat phi " << sqrt(sigmaDeltaPhitot2)
                                                        << " sigma scat theta " << sqrt(sigmaDeltaThetatot2));

        itsos = -1;
        if (tsosnr.size() > 0) {
            int itsosMiddle = tsosnr.size() / 2;
            itsosMiddle     = tsosnr[itsosMiddle];

            t = combinedTrack.trackStateOnSurfaces()->begin();

            ATH_MSG_DEBUG(" itsosMiddle " << itsosMiddle << " tsosnr size " << tsosnr.size());

            for (; t != combinedTrack.trackStateOnSurfaces()->end(); ++t) {
                itsos++;
                if (itsos == itsosMiddle) {
                    // Make TSOS for the ID
                    const Trk::EnergyLoss* energyLossNew =
                        new Trk::EnergyLoss(Eloss, sigmaEloss, sigmaEloss, sigmaEloss);

                    const Trk::ScatteringAngles* scatNew =
                        new Trk::ScatteringAngles(0., 0., sqrt(sigmaDeltaPhitot2), sqrt(sigmaDeltaThetatot2));

                    const Trk::Surface& surfNew = (**t).trackParameters()->associatedSurface();

                    std::bitset<Trk::MaterialEffectsBase::NumberOfMaterialEffectsTypes> meotPattern(0);
                    meotPattern.set(Trk::MaterialEffectsBase::EnergyLossEffects);
                    meotPattern.set(Trk::MaterialEffectsBase::ScatteringEffects);

                    const Trk::MaterialEffectsOnTrack* meotNew =
                        new Trk::MaterialEffectsOnTrack(X0tot, scatNew, energyLossNew, surfNew, meotPattern);

                    const Trk::TrackParameters* parsNew = ((**t).trackParameters())->clone();
                    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePatternScat(0);
                    typePatternScat.set(Trk::TrackStateOnSurface::Scatterer);

                    const Trk::TrackStateOnSurface* newTSOS =
                        new Trk::TrackStateOnSurface(nullptr, parsNew, nullptr, meotNew, typePatternScat);

                    trackStateOnSurfaces->push_back(newTSOS);
                    ATH_MSG_DEBUG(" add new TSOS for ID ");
                }
            }
        }

    }  // end m_addElossID


    // add the 3 surface calo model
    trackStateOnSurfaces->push_back(innerTSOS);
    trackStateOnSurfaces->push_back(middleTSOS);
    trackStateOnSurfaces->push_back(outerTSOS);

    // MS entrance perigee
    if (m_perigeeAtSpectrometerEntranceLocal) {
        const Trk::TrackStateOnSurface* entranceTSOS = entrancePerigee(outerTSOS->trackParameters());
        if (entranceTSOS) trackStateOnSurfaces->push_back(entranceTSOS);
    }

    // leading spectrometer material
    bool haveLeadingMaterial = false;

    t = s;
    for (; t != combinedTrack.trackStateOnSurfaces()->end(); ++t) {
        if ((**t).type(Trk::TrackStateOnSurface::Measurement)) break;
        haveLeadingMaterial = true;
    }

    // protection against overruning the end of the vector
    if (t == combinedTrack.trackStateOnSurfaces()->end()) {
        ATH_MSG_WARNING("At end of TSOS vector");
        delete vertex;
        return nullptr;
    }

    if (haveLeadingMaterial) appendSelectedTSOS(*trackStateOnSurfaces, s, ++t);

    // insert phi pseudo measurement if necessary
    if (addPhiPseudo) {
        const Trk::TrackStateOnSurface* tsos = createPhiPseudoMeasurement(combinedTrack);
        if (tsos) trackStateOnSurfaces->push_back(tsos);
    }

    // then append the remaining TSOS from the input track
    appendSelectedTSOS(*trackStateOnSurfaces, t, combinedTrack.trackStateOnSurfaces()->end());

    // create track for refit
    Trk::Track* standaloneTrack = new Trk::Track(combinedTrack.info(), trackStateOnSurfaces, nullptr);
    standaloneTrack->info().setPatternRecognitionInfo(Trk::TrackInfo::MuidStandaloneRefit);
    if (m_trackQuery->isCombined(*standaloneTrack)) {
        ATH_MSG_WARNING(" This should not happen standalone Track has ID hits ");
    }

    countAEOTs(standaloneTrack, " in standalone Refit standaloneTrack track before fit ");

    Trk::Track* refittedTrack = fit(*standaloneTrack, false, Trk::muon);
    delete standaloneTrack;

    if (!checkTrack("standaloneRefit", refittedTrack, standaloneTrack)) {
        delete refittedTrack;
        delete vertex;
        return nullptr;
    }

    // eventually this whole tool will use unique_ptrs
    // in the meantime, this allows the MuonErrorOptimisationTool and MuonRefitTool to use them
    std::unique_ptr<Trk::Track> refittedTrackUnique(refittedTrack);
    if (refittedTrackUnique) {
        if (!refittedTrackUnique->fitQuality()) {
            delete vertex;
            return nullptr;
        }

        if (!m_trackQuery->isCaloAssociated(*refittedTrackUnique)) {
            // fail as calo incorrectly described
            m_messageHelper->printWarning(28);
            delete vertex;
            return nullptr;
        }

        countAEOTs(refittedTrackUnique.get(), " standaloneRefit final refittedTrack ");

        // fit with optimized spectrometer errors
        // this should also be inside the "if(refittedTrack) statement
        if (!m_muonErrorOptimizer.empty() && !refittedTrackUnique->info().trackProperties(Trk::TrackInfo::StraightTrack)
            && countAEOTs(refittedTrackUnique.get(), " before optimize ") == 0)
        {

            ATH_MSG_VERBOSE(" perform spectrometer error optimization after cleaning ");
            std::unique_ptr<Trk::Track> optimizedTrack =
                m_muonErrorOptimizer->optimiseErrors(refittedTrackUnique.get());

            if (optimizedTrack) {
                if (checkTrack("standaloneRefitOpt", optimizedTrack.get(), refittedTrackUnique.get())) {
                    refittedTrackUnique.swap(optimizedTrack);
                    countAEOTs(refittedTrackUnique.get(), " standaloneRefit alignment errors Track ");
                }
            }
        }
    }

    delete vertex;
    // have to release it until the whole tool is migrated to unique_ptr
    return refittedTrackUnique.release();
}

/** refit a track */
Trk::Track*
CombinedMuonTrackBuilder::fit(Trk::Track& track, const Trk::RunOutlierRemoval runOutlier,
                              const Trk::ParticleHypothesis particleHypothesis) const
{

    ATH_MSG_VERBOSE(" fit() " << m_printer->print(track) << std::endl << m_printer->printStations(track));
    // check valid particleHypothesis
    if (particleHypothesis != Trk::muon && particleHypothesis != Trk::nonInteracting) {
        // invalid particle hypothesis
        std::stringstream ss;
        ss << particleHypothesis;
        m_messageHelper->printWarning(29, ss.str());
        return nullptr;
    }

    // check if combined or subsystem track
    bool isCombined = m_trackQuery->isCombined(track);
    // select straightLine fitter when magnets downstream of leading measurement are off
    ToolHandle<Trk::ITrackFitter> fitter = m_fitter;
    MagField::AtlasFieldCache     fieldCache;
    // Get field cache object
    EventContext                               ctx = Gaudi::Hive::currentContext();
    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, ctx};
    const AtlasFieldCacheCondObj*              fieldCondObj{*readHandle};

    if (fieldCondObj == nullptr) {
        ATH_MSG_ERROR("SCTSiLorentzAngleCondAlg : Failed to retrieve AtlasFieldCacheCondObj with key "
                      << m_fieldCacheCondObjInputKey.key());
        return 0;
    }
    fieldCondObj->getInitializedCache(fieldCache);

    if (!fieldCache.toroidOn() && !(isCombined && fieldCache.solenoidOn())) {

        fitter = m_fitterSL;
        ATH_MSG_VERBOSE(" fit (track refit method): select SL fitter ");
    }

    // redo ROTs:  ID, CROT and MDT specific treatments
    // if (m_redoRots) redoRots(track);

    // perform fit after ensuring calo is associated for combined tracks
    // calo association for combined tracks (WARN if missing from input)
    Trk::Track* fittedTrack = nullptr;
    if (isCombined && particleHypothesis == Trk::muon && !m_trackQuery->isCaloAssociated(track)) {

        // about to add the TSOS's describing calorimeter association to a combined muon;
        m_messageHelper->printWarning(30);

        DataVector<const Trk::TrackStateOnSurface>* combinedTSOS = new DataVector<const Trk::TrackStateOnSurface>;

        combinedTSOS->reserve(track.trackStateOnSurfaces()->size() + 3);
        bool caloAssociated = false;

        // run-2 schema, update default eloss with parametrised value
        if (m_useCaloTG) {
            ATH_MSG_VERBOSE("Updating Calorimeter TSOS in Muon Combined (re)Fit ...");
            m_materialUpdator->updateCaloTSOS(track);
            caloAssociated = true;
        }

        auto s    = track.trackStateOnSurfaces()->begin();
        auto sEnd = track.trackStateOnSurfaces()->end();
        for (; s != sEnd; ++s) {
            if (caloAssociated) {
                combinedTSOS->push_back((**s).clone());
            } else if (((**s).measurementOnTrack()
                        && m_indetVolume->inside((**s).measurementOnTrack()->globalPosition()))
                       || ((**s).trackParameters() && m_indetVolume->inside((**s).trackParameters()->position())))
            {
                combinedTSOS->push_back((**s).clone());
            } else {
                const Trk::TrackStateOnSurface* tsos = m_caloTSOS->innerTSOS(*track.perigeeParameters());
                if (tsos) {
                    combinedTSOS->push_back(tsos);
                    const Trk::TrackParameters* parameters = tsos->trackParameters();
                    if ((**s).type(Trk::TrackStateOnSurface::CaloDeposit)) {
                        combinedTSOS->push_back((**s).clone());
                        tsos = m_caloTSOS->outerTSOS(*parameters);
                        if (tsos) combinedTSOS->push_back(tsos);
                    } else {
                        tsos = m_caloTSOS->middleTSOS(*parameters);
                        if (tsos) combinedTSOS->push_back(tsos);
                        tsos = m_caloTSOS->outerTSOS(*parameters);
                        if (tsos) combinedTSOS->push_back(tsos);
                        combinedTSOS->push_back((**s).clone());
                    }
                }
                caloAssociated = true;
            }
        }

        Trk::Track* combinedTrack = new Trk::Track(track.info(), combinedTSOS, nullptr);

        if (combinedTrack) countAEOTs(combinedTrack, " combinedTrack track before fit ");

        caloAssociated = m_trackQuery->isCaloAssociated(*combinedTrack);
        caloAssociated = true;

        // Updates the calo TSOS with the ones from TG+corrections
        if (m_updateWithCaloTG && !m_useCaloTG && particleHypothesis == Trk::muon) {
            ATH_MSG_VERBOSE("Updating Calorimeter TSOS in Muon Combined (re)Fit ...");
            m_materialUpdator->updateCaloTSOS(*combinedTrack);
        }

        // FIT
        fittedTrack = fitter->fit(*combinedTrack, false, particleHypothesis);
        delete combinedTrack;
    } else {
        // Updates the calo TSOS with the ones from TG+corrections
        if (m_updateWithCaloTG && !m_useCaloTG && particleHypothesis == Trk::muon) {
            ATH_MSG_VERBOSE("Updating Calorimeter TSOS in Muon Standalone Fit ...");
            m_materialUpdator->updateCaloTSOS(track);
        }

        // FIT
        fittedTrack = fitter->fit(track, false, particleHypothesis);
    }

    // quit if fit has failed
    if (!fittedTrack) return nullptr;

    if (!checkTrack("fitInterface1", fittedTrack, fittedTrack)) {
        delete fittedTrack;
        return nullptr;
    }

    // eventually this whole tool will use unique_ptrs
    // in the meantime, this allows the MuonErrorOptimisationTool and MuonRefitTool to use them
    std::unique_ptr<Trk::Track> fittedTrackUnique(fittedTrack);
    // track cleaning
    if (runOutlier) {
        // fit with optimized spectrometer errors

        if (!m_muonErrorOptimizer.empty() && !fittedTrackUnique->info().trackProperties(Trk::TrackInfo::StraightTrack)
            && optimizeErrors(fittedTrackUnique.get()))
        {

            ATH_MSG_VERBOSE(" perform spectrometer error optimization after cleaning ");
            std::unique_ptr<Trk::Track> optimizedTrack = m_muonErrorOptimizer->optimiseErrors(fittedTrackUnique.get());

            if (optimizedTrack) {
                if (checkTrack("fitInterface1Opt", optimizedTrack.get(), fittedTrackUnique.get())) {
                    fittedTrackUnique.swap(optimizedTrack);
                    countAEOTs(fittedTrackUnique.get(), " re fit scaled errors Track ");
                }
            }
        }

        // chi2 before clean
        double chi2Before = normalizedChi2(*fittedTrackUnique);

        // muon cleaner
        ATH_MSG_VERBOSE(" perform track cleaning... " << m_printer->print(*fittedTrackUnique) << std::endl
                                                      << m_printer->printStations(*fittedTrackUnique));

        if (fittedTrackUnique) countAEOTs(fittedTrackUnique.get(), " refit: fitted track before cleaning ");

        std::unique_ptr<Trk::Track> cleanTrack = m_cleaner->clean(*fittedTrackUnique);

        if (cleanTrack) countAEOTs(cleanTrack.get(), " refit: after cleaning");

        if (cleanTrack && !checkTrack("fitInterface1Cleaner", cleanTrack.get(), fittedTrackUnique.get())) {
            cleanTrack.reset();
        }

        if (!cleanTrack) {
            if (m_allowCleanerVeto && chi2Before > m_badFitChi2) {
                ATH_MSG_DEBUG(" cleaner veto A ");
                ++m_countStandaloneCleanerVeto;
                fittedTrackUnique.reset();
            } else {
                ATH_MSG_DEBUG(" keep original standalone track despite cleaner veto ");
            }
        } else if (!(*cleanTrack->perigeeParameters() == *fittedTrackUnique->perigeeParameters())) {
            double chi2After = normalizedChi2(*cleanTrack);

            if (chi2After < m_badFitChi2 || chi2After < chi2Before) {
                ATH_MSG_VERBOSE(" found and removed spectrometer outlier(s) ");
                fittedTrackUnique.swap(cleanTrack);
            } else {
                ATH_MSG_VERBOSE(" keep original track despite cleaning ");
            }
        }

        // FIXME: provide indet cleaner
        if (fittedTrackUnique) {
            ATH_MSG_VERBOSE(" finished track cleaning... " << m_printer->print(*fittedTrackUnique) << std::endl
                                                           << m_printer->printStations(*fittedTrackUnique));
        }
    }

    // have to use release until the whole tool uses unique_ptr
    return fittedTrackUnique.release();
}

/**
    fit a set of MeasurementBase objects with starting value for perigeeParameters */
Trk::Track*
CombinedMuonTrackBuilder::fit(const Trk::MeasurementSet& measurementSet, const Trk::TrackParameters& perigeeStartValue,
                              const Trk::RunOutlierRemoval  runOutlier,
                              const Trk::ParticleHypothesis particleHypothesis) const
{
    // check valid particleHypothesis
    if (particleHypothesis != Trk::muon && particleHypothesis != Trk::nonInteracting) {
        // invalid particle hypothesis
        std::stringstream ss;
        ss << particleHypothesis;
        m_messageHelper->printWarning(31, ss.str());
        return nullptr;
    }

    // select straightLine fitter when magnets downstream of leading measurement are off
    MagField::AtlasFieldCache fieldCache;
    // Get field cache object
    EventContext                               ctx = Gaudi::Hive::currentContext();
    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, ctx};
    const AtlasFieldCacheCondObj*              fieldCondObj{*readHandle};

    if (fieldCondObj == nullptr) {
        ATH_MSG_ERROR("SCTSiLorentzAngleCondAlg : Failed to retrieve AtlasFieldCacheCondObj with key "
                      << m_fieldCacheCondObjInputKey.key());
        return 0;
    }
    fieldCondObj->getInitializedCache(fieldCache);

    ToolHandle<Trk::ITrackFitter> fitter = m_fitter;
    if (!fieldCache.toroidOn() || std::abs(perigeeStartValue.position().z()) > m_zECToroid) {

        fitter = m_fitterSL;
        ATH_MSG_VERBOSE(" fit (track refit method): select SL fitter ");
    }

    // redo ROTs:  ID, CROT and MDT specific treatments
    // if (m_redoRots) redoRots(track);

    // calo association (if relevant)

    // create Perigee if starting parameters given for a different surface type
    const Trk::TrackParameters* perigee        = &perigeeStartValue;
    Trk::PerigeeSurface*        perigeeSurface = nullptr;

    if (!dynamic_cast<const Trk::Perigee*>(perigee)) {
        Amg::Vector3D origin(perigeeStartValue.position());
        perigeeSurface = new Trk::PerigeeSurface(origin);

        perigee = new Trk::Perigee(perigeeStartValue.position(), perigeeStartValue.momentum(),
                                   perigeeStartValue.charge(), *perigeeSurface);
    }

    // FIT
    Trk::Track* fittedTrack = fitter->fit(measurementSet, *perigee, false, particleHypothesis);
    if (perigeeSurface) {
        delete perigeeSurface;
        delete perigee;
    }

    if (!fittedTrack) return nullptr;

    if (!checkTrack("fitInterface2", fittedTrack, fittedTrack)) {
        delete fittedTrack;
        return nullptr;
    }

    // eventually this whole tool will use unique_ptrs
    // in the meantime, this allows the MuonErrorOptimisationTool and MuonRefitTool to use them
    std::unique_ptr<Trk::Track> fittedTrackUnique(fittedTrack);
    // track cleaning
    if (runOutlier) {
        // fit with optimized spectrometer errors

        if (!m_muonErrorOptimizer.empty() && !fittedTrackUnique->info().trackProperties(Trk::TrackInfo::StraightTrack)
            && optimizeErrors(fittedTrackUnique.get()))
        {

            ATH_MSG_VERBOSE(" perform spectrometer error optimization after cleaning ");
            std::unique_ptr<Trk::Track> optimizedTrack = m_muonErrorOptimizer->optimiseErrors(fittedTrackUnique.get());
            if (optimizedTrack) {
                if (checkTrack("fitInterface2Opt", optimizedTrack.get(), fittedTrackUnique.get())) {
                    fittedTrackUnique.swap(optimizedTrack);
                    countAEOTs(fittedTrackUnique.get(), " fit mstSet scaled errors Track ");
                }
            }
        }

        // chi2 before clean
        double chi2Before = normalizedChi2(*fittedTrackUnique);

        // muon cleaner
        ATH_MSG_VERBOSE(" perform track cleaning... ");

        if (fittedTrackUnique) countAEOTs(fittedTrackUnique.get(), " fit mstSet before cleaning ");

        std::unique_ptr<Trk::Track> cleanTrack = m_cleaner->clean(*fittedTrackUnique);

        if (cleanTrack) countAEOTs(cleanTrack.get(), " fit mstSet clean Track ");

        if (cleanTrack && !checkTrack("fitInterface2Cleaner", cleanTrack.get(), fittedTrackUnique.get())) {
            cleanTrack.reset();
        }

        if (!cleanTrack) {
            if (m_allowCleanerVeto && chi2Before > m_badFitChi2) {
                ATH_MSG_DEBUG(" cleaner veto B");
                ++m_countExtensionCleanerVeto;
                fittedTrackUnique.reset();
            } else {
                ATH_MSG_DEBUG(" keep original extension track despite cleaner veto ");
            }
        } else if (!(*cleanTrack->perigeeParameters() == *fittedTrackUnique->perigeeParameters())) {
            double chi2After = normalizedChi2(*cleanTrack);
            if (chi2After < m_badFitChi2 || chi2After < chi2Before) {
                ATH_MSG_VERBOSE(" found and removed spectrometer outlier(s) ");
                fittedTrackUnique.swap(cleanTrack);
            } else {
                ATH_MSG_VERBOSE(" keep original track despite cleaning ");
            }
        }

        // FIXME: provide indet cleaner
        ATH_MSG_VERBOSE(" finished cleaning");
    }
    // have to use release until the whole code uses unique_ptr
    return fittedTrackUnique.release();
}


/**combined muon fit */
Trk::Track*
CombinedMuonTrackBuilder::fit(const Trk::Track& indetTrack, Trk::Track& extrapolatedTrack,
                              const Trk::RunOutlierRemoval  runOutlier,
                              const Trk::ParticleHypothesis particleHypothesis) const
{
    // check valid particleHypothesis
    if (particleHypothesis != Trk::muon && particleHypothesis != Trk::nonInteracting) {
        // invalid particle hypothesis
        std::stringstream ss;
        ss << particleHypothesis;
        m_messageHelper->printWarning(32, ss.str());
        return nullptr;
    }

    // select straightLine fitter when solenoid and toroid are off
    ToolHandle<Trk::ITrackFitter> fitter = m_fitter;
    MagField::AtlasFieldCache     fieldCache;
    // Get field cache object
    EventContext                               ctx = Gaudi::Hive::currentContext();
    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, ctx};
    const AtlasFieldCacheCondObj*              fieldCondObj{*readHandle};

    if (fieldCondObj == nullptr) {
        ATH_MSG_ERROR("SCTSiLorentzAngleCondAlg : Failed to retrieve AtlasFieldCacheCondObj with key "
                      << m_fieldCacheCondObjInputKey.key());
        return 0;
    }
    fieldCondObj->getInitializedCache(fieldCache);

    if (!fieldCache.toroidOn() && !fieldCache.solenoidOn()) {

        fitter = m_fitterSL;
        ATH_MSG_VERBOSE(" fit (combined muon fit method): select SL fitter ");
    }

    // redo ROTs:  ID, CROT and MDT specific treatments

    // calo association (for now just WARN if missing)
    if (particleHypothesis == Trk::muon && !m_trackQuery->isCaloAssociated(extrapolatedTrack)) {
        // combined muon track is missing the TSOS's describing calorimeter association
        m_messageHelper->printWarning(33);
    }

    // Updates the calo TSOS with the ones from TG+corrections
    if (m_updateWithCaloTG && !m_useCaloTG && particleHypothesis == Trk::muon) {
        ATH_MSG_VERBOSE("Updating Calorimeter TSOS in Muon Combined Fit ...");
        m_materialUpdator->updateCaloTSOS(indetTrack, extrapolatedTrack);
    }

    // FIT
    ATH_MSG_VERBOSE(" perform combined fit... " << std::endl
                                                << m_printer->print(indetTrack) << std::endl
                                                << m_printer->print(extrapolatedTrack));

    Trk::Track* fittedTrack = fitter->fit(indetTrack, extrapolatedTrack, false, particleHypothesis);

    if (!fittedTrack) return nullptr;

    // eventually this whole tool will use unique_ptrs
    // in the meantime, this allows the MuonErrorOptimisationTool and MuonRefitTool to use them
    std::unique_ptr<Trk::Track> fittedTrackUnique(fittedTrack);

    // track cleaning
    if (runOutlier) {
        // fit with optimized spectrometer errors

        if (!m_muonErrorOptimizer.empty() && !fittedTrack->info().trackProperties(Trk::TrackInfo::StraightTrack)
            && optimizeErrors(fittedTrackUnique.get()))
        {
            ATH_MSG_VERBOSE(" perform spectrometer error optimization after cleaning ");
            std::unique_ptr<Trk::Track> optimizedTrack = m_muonErrorOptimizer->optimiseErrors(fittedTrackUnique.get());

            if (optimizedTrack) {
                fittedTrackUnique.swap(optimizedTrack);
                countAEOTs(fittedTrackUnique.get(), " cbfit scaled errors Track ");
            }
        }

        // chi2 before clean
        double chi2Before = normalizedChi2(*fittedTrackUnique.get());

        // muon cleaner
        ATH_MSG_VERBOSE(" perform track cleaning... " << m_printer->print(*fittedTrackUnique) << std::endl
                                                      << m_printer->printStations(*fittedTrackUnique));

        if (fittedTrackUnique) {
            countAEOTs(fittedTrackUnique.get(), " cb before clean Track ");
        }
        std::unique_ptr<Trk::Track> cleanTrack = m_cleaner->clean(*fittedTrackUnique);
        if (cleanTrack) {
            countAEOTs(cleanTrack.get(), " cb after clean Track ");
        }

        if (!cleanTrack) {
            if (m_allowCleanerVeto && chi2Before > m_badFitChi2) {
                ATH_MSG_DEBUG(" cleaner veto C");
                ++m_countCombinedCleanerVeto;
                fittedTrackUnique.reset();
            } else {
                ATH_MSG_DEBUG(" keep original combined track despite cleaner veto ");
            }
        } else if (!(*cleanTrack->perigeeParameters() == *fittedTrackUnique->perigeeParameters())) {
            double chi2After = normalizedChi2(*cleanTrack);
            if (chi2After < m_badFitChi2 || chi2After < chi2Before) {
                ATH_MSG_VERBOSE(" found and removed spectrometer outlier(s) ");
                fittedTrackUnique.swap(cleanTrack);
            } else {
                ATH_MSG_VERBOSE(" keep original track despite cleaning ");
            }
        }

        // FIXME: provide indet cleaner
        ATH_MSG_VERBOSE(" finished cleaning");
    }
    // have to use release until the whole code uses unique_ptr
    return fittedTrackUnique.release();
}

/*   private methods follow */

bool
CombinedMuonTrackBuilder::optimizeErrors(Trk::Track* track) const
{
    const Trk::MuonTrackSummary* muonSummary = nullptr;
    const Trk::TrackSummary*     summary     = track->trackSummary();

    if (summary) {
        muonSummary = summary->muonTrackSummary();
    } else {
        m_trackSummary->updateTrack(*track);
        summary     = track->trackSummary();
        muonSummary = summary->muonTrackSummary();
    }

    if (!muonSummary) return false;

    typedef Trk::MuonTrackSummary::ChamberHitSummary ChamberHitSummary;
    const std::vector<ChamberHitSummary>&            chamberHitSummary = muonSummary->chamberHitSummary();

    std::vector<ChamberHitSummary>::const_iterator chit     = chamberHitSummary.begin();
    std::vector<ChamberHitSummary>::const_iterator chit_end = chamberHitSummary.end();

    int optimize = 0;
    int nBarrel  = 0;
    int nEndcap  = 0;
    int nSmall   = 0;
    int nLarge   = 0;

    for (; chit != chit_end; ++chit) {
        const Identifier& id    = chit->chamberId();
        bool              isMdt = m_idHelperSvc->isMdt(id);
        if (!isMdt) continue;

        Muon::MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex(id);

        if (stIndex == Muon::MuonStationIndex::BE) {
            optimize = 1;
        }

        if (stIndex == Muon::MuonStationIndex::BI && m_idHelperSvc->chamberIndex(id) == Muon::MuonStationIndex::BIS
            && abs(m_idHelperSvc->stationEta(id)) > 6)
        {

            optimize = 2;
        }

        if (stIndex == Muon::MuonStationIndex::BI || stIndex == Muon::MuonStationIndex::BM
            || stIndex == Muon::MuonStationIndex::BO || stIndex == Muon::MuonStationIndex::BE)
        {

            nBarrel++;
        }

        if (stIndex == Muon::MuonStationIndex::EI || stIndex == Muon::MuonStationIndex::EM
            || stIndex == Muon::MuonStationIndex::EO || stIndex == Muon::MuonStationIndex::EE)
        {

            nEndcap++;
        }

        if (m_idHelperSvc->isSmallChamber(id)) {
            nSmall++;
        } else {
            nLarge++;
        }
    }

    if (nBarrel > 0 && nEndcap > 0) {
        optimize += 10;
    }

    if (nSmall > 0 && nLarge > 0) {
        optimize += 100;
    }

    if (optimize > 0) {
        ATH_MSG_DEBUG(" OptimizeErrors with value " << optimize);
    }

    if (optimize > 0) {
        return true;
    }
    return false;
}

Trk::Track*
CombinedMuonTrackBuilder::addIDMSerrors(Trk::Track* track) const
{
    //
    // take track and correct the two scattering planes in the Calorimeter
    // to take into account m_IDMS_rzSigma and m_IDMS_xySigma
    //
    // returns a new Track
    //
    if (!m_addIDMSerrors) {
        return track;
    }

    ATH_MSG_DEBUG(" CombinedMuonTrackBuilder addIDMSerrors to track ");
    Amg::Vector3D positionMS(0, 0, 0);
    Amg::Vector3D positionCaloFirst(0, 0, 0);
    Amg::Vector3D positionCaloLast(0, 0, 0);

    int    itsos          = 0;
    int    itsosCaloFirst = -1;
    int    itsosCaloLast  = -1;
    double p              = -1.;

    DataVector<const Trk::TrackStateOnSurface>::const_iterator t = track->trackStateOnSurfaces()->begin();
    for (; t != track->trackStateOnSurfaces()->end(); ++t) {
        itsos++;

        if ((**t).trackParameters()) {
            if (p == -1.) {
                p = (**t).trackParameters()->momentum().mag() / Units::GeV;
            }

            if (m_indetVolume->inside((**t).trackParameters()->position())) {
                continue;
            }

            if ((**t).trackParameters()->position().mag() < 1000) {
                continue;
            }

            if (m_calorimeterVolume->inside((**t).trackParameters()->position())) {
                // first scattering plane in Calorimeter
                if ((**t).type(Trk::TrackStateOnSurface::Scatterer) && (**t).materialEffectsOnTrack()) {
                    double X0 = (**t).materialEffectsOnTrack()->thicknessInX0();
                    if (X0 < 10) {
                        continue;
                    }

                    if (itsosCaloFirst != -1) {
                        itsosCaloLast    = itsos;
                        positionCaloLast = (**t).trackParameters()->position();
                    }

                    if (itsosCaloFirst == -1) {
                        itsosCaloFirst    = itsos;
                        positionCaloFirst = (**t).trackParameters()->position();
                    }
                }
            }

            if (!m_calorimeterVolume->inside((**t).trackParameters()->position())) {
                if ((**t).measurementOnTrack()) {
                    // inside muon system
                    positionMS = (**t).trackParameters()->position();
                    break;
                }
            }
        }
    }

    // it can happen that no Calorimeter Scatterers are found.
    ATH_MSG_DEBUG(" CombinedMuonTrackBuilder addIDMSerrors p muon GeV "
                  << p << " First Calorimeter Scatterer radius " << positionCaloFirst.perp() << " z "
                  << positionCaloFirst.z() << " Second Scatterer r " << positionCaloLast.perp() << " z "
                  << positionCaloLast.z() << " First Muon hit r " << positionMS.perp() << " z " << positionMS.z());

    if (itsosCaloFirst < 0 || itsosCaloLast < 0) {
        ATH_MSG_DEBUG(" addIDMSerrors keep original track ");
        return track;
    }

    // If no Calorimeter no IDMS uncertainties have to be propagated
    positionCaloFirst = positionCaloFirst - positionMS;
    positionCaloLast  = positionCaloLast - positionMS;

    double sigmaDeltaPhiIDMS2   = m_IDMS_xySigma / (positionCaloFirst.perp() + positionCaloLast.perp());
    double sigmaDeltaThetaIDMS2 = m_IDMS_rzSigma / (positionCaloFirst.mag() + positionCaloLast.mag());

    sigmaDeltaPhiIDMS2 *= sigmaDeltaPhiIDMS2;
    sigmaDeltaThetaIDMS2 *= sigmaDeltaThetaIDMS2;

    DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>;
    trackStateOnSurfaces->reserve(track->trackStateOnSurfaces()->size());

    t     = track->trackStateOnSurfaces()->begin();
    itsos = 0;
    for (; t != track->trackStateOnSurfaces()->end(); ++t) {
        itsos++;

        if (itsos == itsosCaloFirst || itsos == itsosCaloLast) {
            if ((**t).materialEffectsOnTrack()) {
                double X0 = (**t).materialEffectsOnTrack()->thicknessInX0();

                const Trk::MaterialEffectsOnTrack* meot =
                    dynamic_cast<const Trk::MaterialEffectsOnTrack*>((**t).materialEffectsOnTrack());

                if (meot) {
                    const Trk::ScatteringAngles* scat = meot->scatteringAngles();

                    if (scat) {
                        double sigmaDeltaPhi =
                            sqrt((scat->sigmaDeltaPhi()) * (scat->sigmaDeltaPhi()) + sigmaDeltaPhiIDMS2);

                        double sigmaDeltaTheta =
                            sqrt((scat->sigmaDeltaTheta()) * (scat->sigmaDeltaTheta()) + sigmaDeltaThetaIDMS2);

                        const Trk::EnergyLoss* energyLossNew = new Trk::EnergyLoss(0., 0., 0., 0.);

                        const Trk::ScatteringAngles* scatNew =
                            new Trk::ScatteringAngles(0., 0., sigmaDeltaPhi, sigmaDeltaTheta);

                        const Trk::Surface& surfNew = (**t).trackParameters()->associatedSurface();

                        std::bitset<Trk::MaterialEffectsBase::NumberOfMaterialEffectsTypes> meotPattern(0);
                        meotPattern.set(Trk::MaterialEffectsBase::EnergyLossEffects);
                        meotPattern.set(Trk::MaterialEffectsBase::ScatteringEffects);

                        const Trk::MaterialEffectsOnTrack* meotNew =
                            new Trk::MaterialEffectsOnTrack(X0, scatNew, energyLossNew, surfNew, meotPattern);

                        const Trk::TrackParameters* parsNew = ((**t).trackParameters())->clone();

                        std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePatternScat(0);
                        typePatternScat.set(Trk::TrackStateOnSurface::Scatterer);

                        const Trk::TrackStateOnSurface* newTSOS =
                            new Trk::TrackStateOnSurface(nullptr, parsNew, nullptr, meotNew, typePatternScat);

                        trackStateOnSurfaces->push_back(newTSOS);

                        ATH_MSG_DEBUG(" old Calo scatterer had sigmaDeltaPhi mrad      "
                                      << scat->sigmaDeltaPhi() * 1000 << " sigmaDeltaTheta mrad "
                                      << scat->sigmaDeltaTheta() * 1000 << " X0 " << X0);

                        ATH_MSG_DEBUG(" new Calo scatterer made with sigmaDeltaPhi mrad "
                                      << sigmaDeltaPhi * 1000 << " sigmaDeltaTheta mrad " << sigmaDeltaTheta * 1000);

                    } else {
                        ATH_MSG_WARNING(" This should not happen: no Scattering Angles for scatterer ");
                    }
                } else {
                    ATH_MSG_WARNING(" This should not happen: no MaterialEffectsOnTrack for scatterer ");
                }
            }
        } else {
            // skip AEOTs
            if ((**t).alignmentEffectsOnTrack()) {
                ATH_MSG_DEBUG(" addIDMSerrors alignmentEffectsOnTrack()  found on track ");
            }
            const Trk::TrackStateOnSurface* TSOS = (**t).clone();
            trackStateOnSurfaces->push_back(TSOS);
        }
    }

    ATH_MSG_DEBUG(" trackStateOnSurfaces on input track " << track->trackStateOnSurfaces()->size()
                                                          << " trackStateOnSurfaces found "
                                                          << trackStateOnSurfaces->size());

    Trk::Track* newTrack = new Trk::Track(track->info(), trackStateOnSurfaces, nullptr);
    if (newTrack) countAEOTs(newTrack, " add IDMS errors ");
    return newTrack;
}


void
CombinedMuonTrackBuilder::appendSelectedTSOS(DataVector<const Trk::TrackStateOnSurface>& trackStateOnSurfaces,
                                             DataVector<const Trk::TrackStateOnSurface>::const_iterator begin,
                                             DataVector<const Trk::TrackStateOnSurface>::const_iterator end) const
{
    // spectrometer measurement selection
    std::vector<const Trk::Surface*> measurementSurfaces;
    measurementSurfaces.reserve(trackStateOnSurfaces.size());
    const Trk::Surface* previousSurface = nullptr;

    DataVector<const Trk::TrackStateOnSurface>::const_iterator s = begin;
    for (; s != end; ++s) {
        // AEOTs are skipped here
        if ((**s).alignmentEffectsOnTrack()) {
            ATH_MSG_VERBOSE("appendSelectedTSOS:: alignmentEffectsOnTrack ");
            // continue;
        }

        // skip non-understood features in iPatFitter
        if (!(**s).measurementOnTrack() && !(**s).materialEffectsOnTrack()) {
            if ((**s).type(Trk::TrackStateOnSurface::Perigee)) {
                continue;
            } else if (!(**s).type(Trk::TrackStateOnSurface::Hole) || !(**s).trackParameters()) {

                ATH_MSG_VERBOSE("appendSelectedTSOS:: skip unrecognized TSOS " << (**s).dumpType());
                continue;
            }
        }

        if ((**s).measurementOnTrack()) {
            // skip any pseudo measurements
            if (dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**s).measurementOnTrack())) {
                continue;
            }

            // skip duplicate measurements on same surface
            const Trk::Surface* surface = &(**s).measurementOnTrack()->associatedSurface();
            if (previousSurface
                && std::find(measurementSurfaces.begin(), measurementSurfaces.end(), surface)
                       != measurementSurfaces.end()
                && !m_idHelperSvc->isMM(surface->associatedDetectorElementIdentifier()))
            {

                std::string type = "";
                if (dynamic_cast<const Trk::CompetingRIOsOnTrack*>((**s).measurementOnTrack())) {
                    type = " of type competingROT";
                } else if (dynamic_cast<const Trk::PlaneSurface*>(surface)) {
                    type = " on a plane surface";
                } else {
                    type = " of type drift circle";
                }

                // skip duplicate measurement
                m_messageHelper->printWarning(34, type);
                continue;
            }

            measurementSurfaces.push_back(surface);
            previousSurface = surface;
        }

        const Trk::TrackStateOnSurface* TSOS = (**s).clone();
        trackStateOnSurfaces.push_back(TSOS);
    }
}


const CaloEnergy*
CombinedMuonTrackBuilder::caloEnergyParameters(const Trk::Track* combinedTrack, const Trk::Track* muonTrack,
                                               const Trk::TrackParameters*& combinedEnergyParameters,
                                               const Trk::TrackParameters*& muonEnergyParameters) const
{
    // will also set the caloEnergyParameters (from both combinedTrack and muonTrack)
    combinedEnergyParameters = nullptr;
    muonEnergyParameters     = nullptr;

    // quit if missing track
    if (!combinedTrack || !muonTrack) return nullptr;

    // muonTrack: get parameters at CaloDeposit
    DataVector<const Trk::TrackStateOnSurface>::const_iterator s = muonTrack->trackStateOnSurfaces()->begin();

    while (!(**s).type(Trk::TrackStateOnSurface::CaloDeposit)) {
        if (++s == muonTrack->trackStateOnSurfaces()->end()) {
            // muonTrack without caloEnergy association
            m_messageHelper->printWarning(35);
            return nullptr;
        }
    }
    muonEnergyParameters = (**s).trackParameters();

    // find corresponding parameters from combinedTrack
    s = combinedTrack->trackStateOnSurfaces()->begin();
    while (!(**s).type(Trk::TrackStateOnSurface::CaloDeposit)) {
        if (++s == combinedTrack->trackStateOnSurfaces()->end()) {
            // combinedTrack without caloEnergy association
            m_messageHelper->printWarning(36);
            return nullptr;
        }
    }

    combinedEnergyParameters = (**s).trackParameters();
    if (muonEnergyParameters && combinedEnergyParameters) {
        ATH_MSG_DEBUG("muon and combined EnergyParameters: " << muonEnergyParameters->momentum().mag() << " "
                                                             << combinedEnergyParameters->momentum().mag());
    }
    // success!
    return m_trackQuery->caloEnergy(*combinedTrack);
}


Trk::Track*
CombinedMuonTrackBuilder::createExtrapolatedTrack(const Trk::Track&           spectrometerTrack,
                                                  const Trk::TrackParameters& parameters,
                                                  Trk::ParticleHypothesis     particleHypothesis,
                                                  Trk::RunOutlierRemoval      runOutlier,
                                                  const std::vector<const Trk::TrackStateOnSurface*>& spectrometerTSOS,
                                                  const Trk::RecVertex* vertex, const Trk::RecVertex* mbeamAxis,
                                                  const Trk::PerigeeSurface* mperigeeSurface,
                                                  const Trk::Perigee*        seedParameters) const
{

    ATH_MSG_VERBOSE(" createExtrapolatedTrack: pt "
                    << parameters.momentum().perp() << " r " << parameters.position().perp() << " z "
                    << parameters.position().z() << " cov " << parameters.covariance() << " muonfit "
                    << (particleHypothesis == Trk::muon));

    std::vector<const Trk::TrackStateOnSurface*>* caloTSOS    = nullptr;
    std::vector<const Trk::TrackStateOnSurface*>* leadingTSOS = nullptr;

    std::vector<const Trk::TrackStateOnSurface*>::const_iterator s;

    const Trk::TrackParameters* trackParameters = nullptr;
    const Trk::Perigee*         perigee         = nullptr;

    if (vertex && m_indetVolume->inside(parameters.position())) {
        perigee = dynamic_cast<const Trk::Perigee*>(&parameters);
    }

    if (perigee) {
        ATH_MSG_DEBUG("got a perigee");
        trackParameters = perigee->clone();
    } else {
        ATH_MSG_DEBUG("no perigee");
        // extrapolate backwards to associate leading material in spectrometer
        // (provided material has already been allocated between measurements)
        const Trk::TrackParameters* leadingParameters = &parameters;

        if (particleHypothesis == Trk::muon) {
            bool haveMaterial        = false;
            bool haveLeadingMaterial = false;
            bool firstMSHit          = false;

            for (s = spectrometerTSOS.begin(); s != spectrometerTSOS.end(); ++s) {

                if ((**s).materialEffectsOnTrack()) {
                    haveMaterial = true;

                    if (!firstMSHit) haveLeadingMaterial = true;
                }

                if ((**s).measurementOnTrack() && !firstMSHit) {
                    firstMSHit = true;
                }

                if (haveMaterial && firstMSHit) {
                    break;
                }
            }

            // only add leading material if there is no material in fron of first muon measurement

            if (!m_materialAllocator.empty() && haveMaterial && !haveLeadingMaterial) {
                // protect the momentum to avoid excessive Eloss
                Amg::VectorX parameterVector = parameters.parameters();

                double Emax = 50000.;

                if (parameterVector[Trk::qOverP] == 0.) {
                    parameterVector[Trk::qOverP] = 1. / Emax;
                } else {
                    if (std::abs(parameterVector[Trk::qOverP]) * Emax < 1) {
                        parameterVector[Trk::qOverP] = parameters.charge() / Emax;
                    }
                }
                const Trk::TrackParameters* correctedParameters = parameters.associatedSurface().createTrackParameters(
                    parameterVector[Trk::loc1], parameterVector[Trk::loc2], parameterVector[Trk::phi],
                    parameterVector[Trk::theta], parameterVector[Trk::qOverP], nullptr);

                Trk::IMaterialAllocator::Garbage_t garbage;
                leadingTSOS = m_materialAllocator->leadingSpectrometerTSOS(*correctedParameters, garbage);

                if (leadingTSOS && leadingTSOS->size() && leadingTSOS->front()->trackParameters()) {
                    leadingParameters = leadingTSOS->front()->trackParameters();
                }

                delete correctedParameters;
            }

        }  // if (particleHypothesis == Trk::muon) {

        // extrapolate backwards to associate calorimeter material effects
        bool caloAssociated = false;

        if (particleHypothesis == Trk::muon) {
            ATH_MSG_VERBOSE(" Retriving Calorimeter TSOS from " << __func__ << " at line " << __LINE__);
            if (m_useCaloTG) {
                caloTSOS = m_materialUpdator->getCaloTSOS(*leadingParameters, spectrometerTrack);

                // Dump CaloTSOS
                //
                for (auto m : *caloTSOS) {
                    if (m->materialEffectsOnTrack()) {
                        const Trk::MaterialEffectsOnTrack* meot =
                            dynamic_cast<const Trk::MaterialEffectsOnTrack*>(m->materialEffectsOnTrack());

                        double pcalo  = 0.;
                        double deltaP = 0.;

                        if (meot) {
                            if (meot->thicknessInX0() > 20) {
                                const Trk::ScatteringAngles* scatAngles = meot->scatteringAngles();

                                ATH_MSG_DEBUG(" Calorimeter X0  " << meot->thicknessInX0() << "  pointer scat "
                                                                  << scatAngles);

                                if (scatAngles) {
                                    pcalo = m->trackParameters()->momentum().mag();

                                    double pullPhi   = scatAngles->deltaPhi() / scatAngles->sigmaDeltaPhi();
                                    double pullTheta = scatAngles->deltaTheta() / scatAngles->sigmaDeltaTheta();

                                    ATH_MSG_DEBUG(" Calorimeter scatterer deltaPhi "
                                                  << scatAngles->deltaPhi() << " pull " << pullPhi << " deltaTheta "
                                                  << scatAngles->deltaTheta() << " pull " << pullTheta);
                                }
                            }

                            const Trk::EnergyLoss* energyLoss = meot->energyLoss();
                            if (energyLoss) {
                                if (m->trackParameters()) {
                                    ATH_MSG_DEBUG("Eloss found r " << (m->trackParameters())->position().perp() << " z "
                                                                   << (m->trackParameters())->position().z()
                                                                   << " deltaE " << energyLoss->deltaE());
                                }

                                if (m->type(Trk::TrackStateOnSurface::CaloDeposit)) {
                                    double caloEloss = std::abs(energyLoss->deltaE());
                                    if (m->trackParameters()) {
                                        deltaP = m->trackParameters()->momentum().mag() - pcalo;
                                    }

                                    ATH_MSG_DEBUG(" Calorimeter Deposit " << caloEloss << " pcalo Entrance " << pcalo
                                                                          << " deltaP " << deltaP);
                                }
                            }  // if (energyLoss) {

                        }  // if (meot) {
                    }      // if (m->materialEffectsOnTrack()) {
                }          // for (auto m : *caloTSOS) {

            } else {
                caloTSOS = m_caloTSOS->caloTSOS(*leadingParameters);
            }

            if (caloTSOS && caloTSOS->size() > 2) {
                caloAssociated = true;
            } else {
                ATH_MSG_VERBOSE("Failed to associated calorimeter");
            }
        } else {
            // TDDO Run2 Calo TG
            const Trk::TrackStateOnSurface* tsos = m_caloTSOS->innerTSOS(parameters);
            if (tsos) {
                caloTSOS = new std::vector<const Trk::TrackStateOnSurface*>;
                caloTSOS->reserve(2);
                caloTSOS->push_back(tsos);

                tsos = m_caloTSOS->outerTSOS(*tsos->trackParameters());
                if (tsos) {
                    caloAssociated = true;
                    caloTSOS->push_back(tsos);
                }
            }
            ATH_MSG_VERBOSE("Special non-muon case for calo: " << caloAssociated);
        }

        // if association OK, create perigee surface and back-track to it
        if (caloAssociated) {
            MagField::AtlasFieldCache fieldCache;
            // Get field cache object
            EventContext                               ctx = Gaudi::Hive::currentContext();
            SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, ctx};
            const AtlasFieldCacheCondObj*              fieldCondObj{*readHandle};

            if (fieldCondObj == nullptr) {
                ATH_MSG_ERROR("SCTSiLorentzAngleCondAlg : Failed to retrieve AtlasFieldCacheCondObj with key "
                              << m_fieldCacheCondObjInputKey.key());
                return 0;
            }
            fieldCondObj->getInitializedCache(fieldCache);

            if (fieldCache.toroidOn()) {
                const Trk::TrackParameters* oldParameters = caloTSOS->front()->trackParameters();

                if (oldParameters && !oldParameters->covariance()) {
                    ATH_MSG_VERBOSE(" createExtrapolatedTrack: no cov (0)");
                }

                trackParameters = m_propagator->propagate(*oldParameters, *mperigeeSurface, Trk::oppositeMomentum,
                                                          false, m_magFieldProperties, Trk::nonInteracting);
            } else {
                trackParameters = m_propagatorSL->propagate(parameters, *mperigeeSurface, Trk::oppositeMomentum, false,
                                                            m_magFieldProperties, Trk::nonInteracting);
            }

            // only accept when perigee in indet tracking volume
            if (trackParameters && !m_indetVolume->inside(trackParameters->position())) {
                ATH_MSG_DEBUG("  back extrapolation problem: probably outside indet volume ");
                delete trackParameters;
                trackParameters = nullptr;
                caloAssociated  = false;
            }

            if (trackParameters && !trackParameters->covariance()) {
                ATH_MSG_VERBOSE(" createExtrapolatedTrack: no cov (1)");
            }

            if (trackParameters) {
                ATH_MSG_VERBOSE(" Seed parameter: r " << trackParameters->position().perp() << " z "
                                                      << trackParameters->position().z() << " pt "
                                                      << trackParameters->momentum().perp());
            }

        }  // if (caloAssociated) {

        // start from vertex in case of calo association problem
        if (vertex && !caloAssociated) {
            ATH_MSG_DEBUG("  back extrapolation problem: retry with tracking out from vertex ");
            // delete any existing calo objects
            if (caloTSOS) {
                for (s = caloTSOS->begin(); s != caloTSOS->end(); ++s) {
                    delete *s;
                }

                delete caloTSOS;
                caloTSOS = nullptr;
            }

            // track out from vertex
            Amg::Vector3D momentum = parameters.position() * Gaudi::Units::TeV / parameters.position().mag();

            trackParameters = new Trk::Perigee(vertex->position(), momentum, 1., *mperigeeSurface);

            particleHypothesis = Trk::nonInteracting;
            runOutlier         = false;

            ATH_MSG_VERBOSE(" Retriving Calorimeter TSOS from " << __func__ << " at line " << __LINE__);

            if (m_useCaloTG) {
                caloTSOS = m_materialUpdator->getCaloTSOS(*trackParameters, spectrometerTrack);
            } else {
                const Trk::TrackStateOnSurface* tsos = m_caloTSOS->innerTSOS(*trackParameters);
                if (tsos) {
                    caloTSOS = new std::vector<const Trk::TrackStateOnSurface*>;
                    caloTSOS->reserve(2);
                    caloTSOS->push_back(tsos);

                    tsos = m_caloTSOS->outerTSOS(*trackParameters);
                    if (tsos) {
                        caloTSOS->push_back(tsos);
                    } else {
                        delete trackParameters;
                        trackParameters = nullptr;
                    }
                }
            }
        }

        // failure in calo association and/or extrapolation to indet
        if (!trackParameters || !caloTSOS) {
            if (trackParameters) delete trackParameters;

            // delete spectrometer TSOS
            for (s = spectrometerTSOS.begin(); s != spectrometerTSOS.end(); ++s) {
                delete *s;
            }

            // delete leading material TSOS
            if (leadingTSOS) {
                for (s = leadingTSOS->begin(); s != leadingTSOS->end(); ++s) {
                    delete *s;
                }
                delete leadingTSOS;
            }

            // delete calo objects
            if (caloTSOS) {
                for (s = caloTSOS->begin(); s != caloTSOS->end(); ++s) {
                    delete *s;
                }
                delete caloTSOS;
            } else {
                ATH_MSG_DEBUG("  calo association fails ");
            }

            ATH_MSG_DEBUG("  perigee back-extrapolation fails ");
            return nullptr;
        }
    }  // if (perigee) {

    // set seed if provided
    if (seedParameters) {
        delete trackParameters;
        trackParameters = seedParameters->clone();
    }

    // append TSOS objects into DataVector
    // reserve allows for perigee + vertex + calo + entrancePerigee + spectrometer TSOS
    unsigned int size = spectrometerTSOS.size() + 3;
    if (caloTSOS) {
        size += caloTSOS->size();
    }
    if (leadingTSOS) {
        size += leadingTSOS->size();
    }

    DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>;

    trackStateOnSurfaces->reserve(size);

    // start with perigee TSOS (this just carries the perigee parameters)
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> perigeeType;
    perigeeType.set(Trk::TrackStateOnSurface::Perigee);

    if (trackParameters && !trackParameters->covariance()) {
        ATH_MSG_VERBOSE(" createExtrapolatedTrack: no cov (2)");
    }

    if (trackParameters) {
        trackStateOnSurfaces->push_back(new const Trk::TrackStateOnSurface(
            nullptr, dynamic_cast<const Trk::Perigee*>(trackParameters), nullptr, nullptr, perigeeType));
    }

    // optionally append a pseudoMeasurement describing the vertex
    if (vertex && trackParameters) {
        const Trk::PseudoMeasurementOnTrack* vertexInFit = vertexOnTrack(*trackParameters, vertex, mbeamAxis);

        if (vertexInFit) {
            ATH_MSG_VERBOSE("Adding vertex constraint ");

            std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type;
            type.set(Trk::TrackStateOnSurface::Measurement);

            trackStateOnSurfaces->push_back(
                new const Trk::TrackStateOnSurface(vertexInFit, nullptr, nullptr, nullptr, type));
        }
    }

    // append calo TSOS
    if (caloTSOS) {
        for (s = caloTSOS->begin(); s != caloTSOS->end(); ++s) {
            trackStateOnSurfaces->push_back(*s);
        }
        delete caloTSOS;
    }

    // MS entrance perigee
    if (m_perigeeAtSpectrometerEntranceLocal) {
        ATH_MSG_DEBUG("adding perigee at spectrometer entrance");
        const Trk::TrackParameters* mstrackParameters = trackStateOnSurfaces->back()->trackParameters();

        if (!mstrackParameters) {
            mstrackParameters = spectrometerTSOS.front()->trackParameters();
        }

        if (mstrackParameters) {
            const Trk::TrackStateOnSurface* entranceTSOS = entrancePerigee(mstrackParameters);
            if (entranceTSOS) {
                trackStateOnSurfaces->push_back(entranceTSOS);
            }
        }
    }

    // append leading MS material TSOS
    if (leadingTSOS) {
        for (s = leadingTSOS->begin(); s != leadingTSOS->end(); ++s) {
            if ((**s).materialEffectsOnTrack()) {
                trackStateOnSurfaces->push_back(*s);
            } else {
                delete *s;
            }
        }
        delete leadingTSOS;
    }

    // append the remaining spectrometer TSOS
    for (s = spectrometerTSOS.begin(); s != spectrometerTSOS.end(); ++s) {
        if (!(**s).type(Trk::TrackStateOnSurface::Perigee)) {
            trackStateOnSurfaces->push_back(*s);
        }

        if ((**s).measurementOnTrack()
            && dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**s).measurementOnTrack())) {

            ATH_MSG_VERBOSE(" MS Pseudo");
        }
    }

    // create track
    Trk::Track* track = new Trk::Track(spectrometerTrack.info(), trackStateOnSurfaces, nullptr);

    dumpCaloEloss(track, " createExtrapolatedTrack ");
    countAEOTs(track, " createExtrapolatedTrack before fit ");

    // remove material when curvature badly determined (to remove energy loss uncertainty)
    if (particleHypothesis == Trk::nonInteracting) {
        ATH_MSG_VERBOSE("  remove spectrometer material ");
        removeSpectrometerMaterial(track);
    }

    // fit the track
    if (msgLvl(MSG::VERBOSE)) {
        msg(MSG::VERBOSE) << "  fit SA track with " << track->trackStateOnSurfaces()->size() << " TSOS";
        if (particleHypothesis == Trk::nonInteracting) {
            msg() << " using nonInteracting hypothesis";
        }
        msg() << endmsg;
    }

    Trk::Track* fittedTrack = fit(*track, runOutlier, particleHypothesis);

    if (fittedTrack) {
        delete track;
        countAEOTs(fittedTrack, " createExtrapolatedTrack after fit");

        // only accept when perigee in indet tracking volume
        if (fittedTrack->perigeeParameters() && !m_indetVolume->inside(fittedTrack->perigeeParameters()->position())) {
            ATH_MSG_DEBUG("  back extrapolation problem: fitted perigee outside indet volume ");
            delete fittedTrack;
            return nullptr;
        }

        // limit momentum for future energy loss allocation
        if (particleHypothesis != Trk::muon) {
            ATH_MSG_VERBOSE("  set momentum limit ");
            removeSpectrometerMaterial(fittedTrack);
        }

        ATH_MSG_VERBOSE(" found track " << m_printer->print(*fittedTrack));
        return fittedTrack;
    }

    // if(trackParameters) delete trackParameters; //if these are still hanging around (coverity thinks yes), delete
    // them

    // return the unfitted track in case of problem
    return track;
}


Trk::Track*
CombinedMuonTrackBuilder::createIndetTrack(const Trk::TrackInfo&                                      info,
                                           DataVector<const Trk::TrackStateOnSurface>::const_iterator begin,
                                           DataVector<const Trk::TrackStateOnSurface>::const_iterator end) const
{
    // create indet track TSOS vector
    DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>;

    // set end iterator to be the first TSOS after the indet
    unsigned size = 1;

    DataVector<const Trk::TrackStateOnSurface>::const_iterator s = begin;

    const Trk::TrackStateOnSurface* perigeeTSOS = (**s).clone();

    ++s;  // keep start perigee where-ever!
    for (; s != end; ++s) {
        ++size;
        if (!m_indetVolume->inside((**s).trackParameters()->position())) {
            break;
        }
    }
    end = s;

    trackStateOnSurfaces->reserve(size);
    trackStateOnSurfaces->push_back(perigeeTSOS);

    // then append selected TSOS
    appendSelectedTSOS(*trackStateOnSurfaces, begin, end);

    return new Trk::Track(info, trackStateOnSurfaces, nullptr);
}

Trk::Track*
CombinedMuonTrackBuilder::createMuonTrack(const Trk::Track& muonTrack, const Trk::TrackParameters* parameters,
                                          const CaloEnergy*                                          caloEnergy,
                                          DataVector<const Trk::TrackStateOnSurface>::const_iterator begin,
                                          DataVector<const Trk::TrackStateOnSurface>::const_iterator end,
                                          unsigned                                                   size) const
{

    countAEOTs(&muonTrack, " createMuonTrack ");

    // set iterator to current TSOS on input track to be after the indet
    const Trk::TrackParameters*                                lastIDtp = nullptr;
    DataVector<const Trk::TrackStateOnSurface>::const_iterator s        = begin;
    while ((**s).trackParameters()
           && (m_indetVolume->inside((**s).trackParameters()->position())
               || (**s).type(Trk::TrackStateOnSurface::Perigee)))
    {
        if (m_indetVolume->inside((**s).trackParameters()->position())) {
            lastIDtp = (**s).trackParameters();
        }

        ++s;
        --size;
    }

    // create muon track TSOS vector
    DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>;

    // redo calo association from inside if requested
    bool redoCaloAssoc = false;
    if (parameters) {
        redoCaloAssoc = true;

        // move current TSOS iterator to be outside the calorimeter
        while ((**s).trackParameters()
               && (m_calorimeterVolume->inside((**s).trackParameters()->position())
                   || (**s).type(Trk::TrackStateOnSurface::Perigee)))
        {
            ++s;
            --size;
        }

        // associate calo by extrapolation from last ID parameters
        std::vector<const Trk::TrackStateOnSurface*>* caloTSOS;
        if (m_useCaloTG) {
            if (!lastIDtp) {
                lastIDtp = parameters;
            }
            ATH_MSG_VERBOSE(" Retriving Calorimeter TSOS from " << __func__ << " at line " << __LINE__);
            caloTSOS = m_materialUpdator->getCaloTSOS(*lastIDtp, muonTrack);
        } else {
            caloTSOS = m_caloTSOS->caloTSOS(*parameters);
        }

        if (!caloTSOS || caloTSOS->size() < 3) {
            ATH_MSG_DEBUG(" muonTrack: parameters fail to fully intersect the calorimeter");
            if (caloTSOS) {
                std::vector<const Trk::TrackStateOnSurface*>::const_iterator t = caloTSOS->begin();

                for (; t != caloTSOS->end(); ++t) {
                    delete *t;
                }

                delete caloTSOS;
            }
            delete trackStateOnSurfaces;
            return nullptr;
        }

        size += caloTSOS->size();
        trackStateOnSurfaces->reserve(size + 1);

        // start with the calo TSOS
        std::vector<const Trk::TrackStateOnSurface*>::const_iterator t = caloTSOS->begin();
        for (; t != caloTSOS->end(); ++t) {
            trackStateOnSurfaces->push_back(*t);
        }

        delete caloTSOS;
    } else {
        trackStateOnSurfaces->reserve(size + 1);
    }

    // if requested, replace caloEnergy on appropriate TSOS
    if (caloEnergy && (**s).trackParameters() && m_calorimeterVolume->inside((**s).trackParameters()->position())) {
        const Trk::TrackStateOnSurface* TSOS = (**s).clone();
        trackStateOnSurfaces->push_back(TSOS);
        ++s;

        // create MEOT owning CaloEnergy
        if ((**s).trackParameters() && m_calorimeterVolume->inside((**s).trackParameters()->position())) {
            std::bitset<Trk::MaterialEffectsBase::NumberOfMaterialEffectsTypes> typePattern;
            typePattern.set(Trk::MaterialEffectsBase::EnergyLossEffects);

            const Trk::MaterialEffectsOnTrack* materialEffects =
                new const Trk::MaterialEffectsOnTrack(0., dynamic_cast<const Trk::EnergyLoss*>(caloEnergy),
                                                      (**s).trackParameters()->associatedSurface(), typePattern);

            // create TSOS
            const Trk::FitQualityOnSurface* fitQoS          = nullptr;
            const Trk::MeasurementBase*     measurementBase = nullptr;

            std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type;
            type.set(Trk::TrackStateOnSurface::CaloDeposit);

            TSOS = new const Trk::TrackStateOnSurface(measurementBase, (**s).trackParameters()->clone(), fitQoS,
                                                      materialEffects, type);

            trackStateOnSurfaces->push_back(TSOS);
            ++s;
        } else {
            // should never happen: FSR caloEnergy delete
            m_messageHelper->printWarning(37);
            delete caloEnergy;
        }
    }

    // MS entrance perigee

    bool hasAlreadyPerigee = false;
    if (m_perigeeAtSpectrometerEntranceLocal) {
        // copy calorimeter TSOS
        while ((**s).trackParameters() && m_calorimeterVolume->inside((**s).trackParameters()->position())) {

            if (!(**s).type(Trk::TrackStateOnSurface::Perigee)) {
                const Trk::TrackStateOnSurface* TSOS = (**s).clone();
                trackStateOnSurfaces->push_back(TSOS);
            }
            ++s;
        }

        // add entrance TSOS if not already present
        const Trk::TrackStateOnSurface* entranceTSOS = nullptr;

        if ((**s).type(Trk::TrackStateOnSurface::Perigee)) {
            hasAlreadyPerigee = true;
        }

        if (!hasAlreadyPerigee && (**s).trackParameters()) {
            if ((**s).trackParameters()) {
                entranceTSOS = entrancePerigee((**s).trackParameters());
            } else {
                entranceTSOS = entrancePerigee(trackStateOnSurfaces->back()->trackParameters());
            }

            if (entranceTSOS) {
                double distance =
                    (entranceTSOS->trackParameters()->position() - (**s).trackParameters()->position()).mag();

                if (distance > 2000) {
                    ATH_MSG_DEBUG(" Added Muon Entrance "
                                  << " r " << entranceTSOS->trackParameters()->position().perp() << " z "
                                  << entranceTSOS->trackParameters()->position().z() << " track pars r "
                                  << (**s).trackParameters()->position().perp() << " z "
                                  << (**s).trackParameters()->position().z());
                }

                trackStateOnSurfaces->push_back(entranceTSOS);
            }
        }
    }

    // then append selected TSOS from the extrapolated or spectrometer track
    appendSelectedTSOS(*trackStateOnSurfaces, s, end);

    Trk::Track* newMuonTrack = new Trk::Track(muonTrack.info(), trackStateOnSurfaces, nullptr);

    // Updates the calo TSOS with the ones from TG+corrections (if needed)
    if (m_updateWithCaloTG && !m_useCaloTG && redoCaloAssoc) {
        ATH_MSG_VERBOSE("Updating Calorimeter TSOS in CreateMuonTrack ...");
        m_materialUpdator->updateCaloTSOS(*newMuonTrack, parameters);
    }

    return newMuonTrack;
}


const Trk::TrackStateOnSurface*
CombinedMuonTrackBuilder::createPhiPseudoMeasurement(const Trk::Track& track) const
{
    const Trk::TrackParameters* parameters = m_trackQuery->spectrometerParameters(track);
    Amg::MatrixX                covarianceMatrix(1, 1);
    covarianceMatrix.setZero();
    covarianceMatrix(0, 0) = m_sigmaPhiSector * m_sigmaPhiSector * parameters->position().perp2();

    const Trk::PseudoMeasurementOnTrack* pseudo = new Trk::PseudoMeasurementOnTrack(
        Trk::LocalParameters(Trk::DefinedParameter(0., Trk::locY)), covarianceMatrix, parameters->associatedSurface());

    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type;
    type.set(Trk::TrackStateOnSurface::Measurement);

    const Trk::TrackStateOnSurface* tsos = new Trk::TrackStateOnSurface(pseudo, parameters, nullptr, nullptr, type);

    return tsos;
}


std::vector<const Trk::TrackStateOnSurface*>*
CombinedMuonTrackBuilder::createSpectrometerTSOS(const Trk::Track& spectrometerTrack) const
{
    const Trk::Perigee* measuredPerigee = spectrometerTrack.perigeeParameters();

    if (!measuredPerigee || !measuredPerigee->covariance()) {
        // missing MeasuredPerigee for spectrometer track
        m_messageHelper->printWarning(38);
        return nullptr;
    }

    double errorPhi = sqrt((*measuredPerigee->covariance())(Trk::phi0, Trk::phi0));

    // create the spectrometer TSOS's for the extrapolated fit
    std::vector<const Trk::TrackStateOnSurface*>* spectrometerTSOS = new std::vector<const Trk::TrackStateOnSurface*>;

    spectrometerTSOS->reserve(spectrometerTrack.trackStateOnSurfaces()->size());

    // start with a 'phi sector constraint' pseudomeasurement when necessary
    unsigned numberPseudo = m_trackQuery->numberPseudoMeasurements(spectrometerTrack);
    if (errorPhi > m_sigmaPhiSector) {
        ++numberPseudo;
    }

    if (numberPseudo > 1 && !m_trackQuery->isSectorOverlap(spectrometerTrack)) {
        ATH_MSG_VERBOSE("standaloneFit: add pseudo to constrain phi sector");

        const Trk::TrackStateOnSurface* tsos = createPhiPseudoMeasurement(spectrometerTrack);
        if (tsos) {
            spectrometerTSOS->push_back(tsos);
        }
    }

    // make a measurement selection to fixup non-standard TSOS's
    double deltaZ          = 0.;
    bool   haveMeasurement = false;

    std::vector<const Trk::Surface*> measurementSurfaces;
    measurementSurfaces.reserve(spectrometerTrack.trackStateOnSurfaces()->size());

    unsigned numberMaterial   = 0;
    unsigned numberParameters = 0;

    const Trk::Surface*             previousSurface = nullptr;
    const Trk::TrackStateOnSurface* previousTSOS    = nullptr;

    DataVector<const Trk::TrackStateOnSurface>::const_iterator s    = spectrometerTrack.trackStateOnSurfaces()->begin();
    auto                                                       sEnd = spectrometerTrack.trackStateOnSurfaces()->end();

    for (; s != sEnd; ++s) {
        // skip any leading material
        if (!haveMeasurement) {
            if ((**s).measurementOnTrack()) {
                haveMeasurement = true;
            } else if ((**s).materialEffectsOnTrack()) {
                continue;
            }
        }

        // input statistics for VERBOSE
        const Trk::TrackParameters* trackParameters = (**s).trackParameters();
        if (msgLvl(MSG::VERBOSE)) {
            if ((**s).materialEffectsOnTrack()) ++numberMaterial;
            if (trackParameters) ++numberParameters;
        }

        // skip unwanted TSOS and non-understood features in iPatFitter
        if (!(**s).measurementOnTrack() && !(**s).materialEffectsOnTrack()) {
            // remove holes as they will be reallocated
            if ((**s).type(Trk::TrackStateOnSurface::Hole)) continue;

            // same for MS perigee
            if ((**s).type(Trk::TrackStateOnSurface::Perigee)) continue;

            if ((**s).trackParameters()) {
                ATH_MSG_DEBUG("createSpectrometerTSOS:: skip unrecognized TSOS "
                              << (**s).dumpType() << " r " << (**s).trackParameters()->position().perp() << " z "
                              << (**s).trackParameters()->position().z());
            } else {
                // skip unrecognized TSOS without TrackParameters
                m_messageHelper->printWarning(39, (**s).dumpType());
            }
            continue;
        }

        // several checks applied to  measurements:
        bool trapezoid   = false;
        bool rotatedTrap = false;
        if ((**s).measurementOnTrack()) {
            // skip pseudo
            if (dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**s).measurementOnTrack())) {
                continue;
            } else {
                // careful with trapezoid ordering (put trapezoid before rotatedTrapezoid)
                const Trk::Surface* surface = &(**s).measurementOnTrack()->associatedSurface();

                if (previousSurface) {
                    deltaZ = std::abs(previousSurface->center().z() - surface->center().z());
                }

                if (dynamic_cast<const Trk::PlaneSurface*>(surface)) {
                    if (dynamic_cast<const Trk::TrapezoidBounds*>(&surface->bounds())) {
                        trapezoid = true;
                    } else if (dynamic_cast<const Trk::RotatedTrapezoidBounds*>(&surface->bounds())) {
                        rotatedTrap = true;
                    }
                }

                // skip duplicate measurements on same surface
                if (previousSurface
                    && std::find(measurementSurfaces.begin(), measurementSurfaces.end(), surface)
                           != measurementSurfaces.end()
                    && !m_idHelperSvc->isMM(surface->associatedDetectorElementIdentifier()))
                {

                    std::string type = "";
                    if (dynamic_cast<const Trk::CompetingRIOsOnTrack*>((**s).measurementOnTrack())) {
                        type = " of type competingROT";
                    } else if (dynamic_cast<const Trk::PlaneSurface*>(surface)) {
                        type = " on a plane surface";
                    } else {
                        type = " of type drift circle";
                    }
                    // skip duplicate measurement
                    m_messageHelper->printWarning(40, type);
                    continue;
                }
                measurementSurfaces.push_back(surface);
                previousSurface = surface;
            }
        } else if (previousTSOS) {
            spectrometerTSOS->push_back(previousTSOS);
            previousTSOS = nullptr;
        }

        // trapezoid precedes rotatedTrapezoid
        const Trk::TrackStateOnSurface* TSOS = (**s).clone();
        if (previousTSOS) {
            if (trapezoid && deltaZ < 1. * Gaudi::Units::mm) {
                spectrometerTSOS->push_back(TSOS);
                TSOS = previousTSOS;
            } else {
                spectrometerTSOS->push_back(previousTSOS);
            }
            previousTSOS = nullptr;
        }

        if (rotatedTrap) {
            previousTSOS = TSOS;
            continue;
        }

        spectrometerTSOS->push_back(TSOS);
    }

    if (previousTSOS) spectrometerTSOS->push_back(previousTSOS);

    ATH_MSG_VERBOSE(" input spectrometer track with "
                    << spectrometerTrack.trackStateOnSurfaces()->size() << " TSOS, of which " << numberMaterial
                    << " have MaterialEffects and " << numberParameters << " have TrackParameters");

    return spectrometerTSOS;
}


const Trk::TrackStateOnSurface*
CombinedMuonTrackBuilder::entrancePerigee(const Trk::TrackParameters* parameters) const
{
    // make sure the spectrometer entrance volume is available
    if (!parameters) return nullptr;

    if (!m_spectrometerEntrance) return nullptr;

    const Trk::TrackParameters* entranceParameters = m_extrapolator->extrapolateToVolume(
        *parameters, *m_spectrometerEntrance, Trk::anyDirection, Trk::nonInteracting);

    if (!entranceParameters) return nullptr;

    Trk::PerigeeSurface         surface(entranceParameters->position());
    const Trk::TrackParameters* trackParameters = m_extrapolator->extrapolateDirectly(*entranceParameters, surface);
    delete entranceParameters;

    if (!trackParameters) return nullptr;

    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> perigeeType;
    perigeeType.set(Trk::TrackStateOnSurface::Perigee);

    const Trk::Perigee* perigee =
        new Trk::Perigee(trackParameters->position(), trackParameters->momentum(), trackParameters->charge(), surface);

    delete trackParameters;
    return new const Trk::TrackStateOnSurface(nullptr, perigee, nullptr, nullptr, perigeeType);
}


const Trk::TrackParameters*
CombinedMuonTrackBuilder::extrapolatedParameters(bool& badlyDeterminedCurvature, const Trk::Track& spectrometerTrack,
                                                 const Trk::RecVertex*      mvertex,
                                                 const Trk::PerigeeSurface* mperigeeSurface) const
{
    badlyDeterminedCurvature            = false;
    const Trk::Perigee* measuredPerigee = spectrometerTrack.perigeeParameters();

    if (!measuredPerigee || !measuredPerigee->covariance()) {
        // missing MeasuredPerigee for spectrometer track
        m_messageHelper->printWarning(42);
        return nullptr;
    }

    // set starting parameters and measured momentum error
    const Trk::TrackParameters* parameters = m_trackQuery->spectrometerParameters(spectrometerTrack);
    if (!parameters || !parameters->covariance()) {
        // missing spectrometer parameters on spectrometer track
        m_messageHelper->printWarning(43);
        return nullptr;
    }

    double errorP =
        sqrt(measuredPerigee->momentum().mag2() * (*measuredPerigee->covariance())(Trk::qOverP, Trk::qOverP));

    // corrected parameters ensure the track fitting starts with a projective approximation
    const Trk::TrackParameters* correctedParameters = nullptr;
    Amg::VectorX                parameterVector     = parameters->parameters();
    double                      trackEnergy         = 1. / std::abs(parameterVector[Trk::qOverP]);

    // careful: need to reset parameters to have a sensible energy if starting from a lineFit
    if (m_trackQuery->isLineFit(spectrometerTrack)) {
        trackEnergy                  = m_lineMomentum;
        parameterVector[Trk::qOverP] = parameters->charge() / trackEnergy;

        correctedParameters = parameters->associatedSurface().createTrackParameters(
            parameterVector[Trk::loc1], parameterVector[Trk::loc2], parameterVector[Trk::phi],
            parameterVector[Trk::theta], parameterVector[Trk::qOverP], new AmgSymMatrix(5)(*parameters->covariance()));

        delete parameters;
        parameters          = correctedParameters;
        correctedParameters = nullptr;
    }

    // check if the track curvature is well determined (with sufficient energy to penetrate material)
    //  (i.e. field off or small momentum error, starting parameters upstream of endcap toroid)
    bool curvatureOK = false;

    ToolHandle<Trk::IPropagator> propagator = m_propagator;
    MagField::AtlasFieldCache    fieldCache;
    // Get field cache object
    EventContext                               ctx = Gaudi::Hive::currentContext();
    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, ctx};
    const AtlasFieldCacheCondObj*              fieldCondObj{*readHandle};

    if (fieldCondObj == nullptr) {
        ATH_MSG_ERROR("SCTSiLorentzAngleCondAlg : Failed to retrieve AtlasFieldCacheCondObj with key "
                      << m_fieldCacheCondObjInputKey.key());
        return 0;
    }
    fieldCondObj->getInitializedCache(fieldCache);
    if (!fieldCache.toroidOn()) {
        curvatureOK = true;
        propagator  = m_propagatorSL;
    } else if (std::abs(parameters->position().z()) < m_zECToroid
               && (!m_trackQuery->isLineFit(spectrometerTrack) && errorP < m_largeMomentumError))
    {
        curvatureOK = true;
    }

    if (curvatureOK) {
        // TDDO Run2 Calo TG
        // energy loss correction
        CaloEnergy* caloEnergy =
            m_caloEnergyParam->energyLoss(trackEnergy, parameters->position().eta(), parameters->position().phi());

        if (trackEnergy + caloEnergy->deltaE() < m_minEnergy) {
            ATH_MSG_DEBUG("standaloneFit: trapped in calorimeter");
            delete caloEnergy;
            delete parameters;

            return nullptr;
        }

        parameterVector[Trk::qOverP] = parameters->charge() / (trackEnergy + caloEnergy->deltaE());
        correctedParameters          = parameters->associatedSurface().createTrackParameters(
            parameterVector[Trk::loc1], parameterVector[Trk::loc2], parameterVector[Trk::phi],
            parameterVector[Trk::theta], parameterVector[Trk::qOverP], new AmgSymMatrix(5)(*parameters->covariance()));

        // protect against spectrometer track with unrealistic energy loss
        // check material in spectrometer is not vastly greater than in the calo
        // (there are some very dense spectrometer regions)
        double spectrometerEnergyLoss = 0.;

        DataVector<const Trk::TrackStateOnSurface>::const_iterator s =
            spectrometerTrack.trackStateOnSurfaces()->begin();

        auto sEnd = spectrometerTrack.trackStateOnSurfaces()->end();
        for (; s != sEnd; ++s) {
            if (!(**s).materialEffectsOnTrack()) {
                continue;
            }

            const Trk::MaterialEffectsOnTrack* meot =
                dynamic_cast<const Trk::MaterialEffectsOnTrack*>((**s).materialEffectsOnTrack());

            if (meot && meot->energyLoss()) {
                spectrometerEnergyLoss += meot->energyLoss()->deltaE();
            }
        }

        if (std::abs(spectrometerEnergyLoss) > 1.5 * std::abs(caloEnergy->deltaE())) {
            curvatureOK = false;

            if (correctedParameters == parameters) {

                ATH_MSG_WARNING(
                    "deleting parameters pointer that could be used further down in execution, setting it to zero!");

                parameters = nullptr;
            }

            delete correctedParameters;
            correctedParameters = nullptr;

            ATH_MSG_DEBUG("standaloneFit: excessive energy loss in spectrometer "
                          << std::abs(spectrometerEnergyLoss / Units::GeV) << " GeV"
                          << "  in calo " << std::abs(caloEnergy->deltaE() / Units::GeV) << " GeV");
        }
        delete caloEnergy;
        caloEnergy = nullptr;
    }

    // check perigee in indet volume when the curvature is well determined
    // otherwise will assume projectivity for starting parameters
    if (curvatureOK) {
        const Trk::TrackParameters* perigee =
            propagator->propagate(*correctedParameters, *mperigeeSurface, Trk::oppositeMomentum, false,
                                  m_magFieldProperties, Trk::nonInteracting);

        if (!perigee) {
            ATH_MSG_DEBUG("standaloneFit: failed back extrapolation to perigee");
            delete correctedParameters;
            delete parameters;
            return nullptr;
        }

        // large impact: set phi to be projective (note iteration)
        if (std::abs(perigee->parameters()[Trk::d0]) < m_largeImpact || !fieldCache.toroidOn()) {
            if (correctedParameters == parameters) {
                ATH_MSG_WARNING(
                    "deleting parameters pointer that could be used further down in execution, setting it to zero!");
                parameters = nullptr;
            }
            delete correctedParameters;
            correctedParameters = nullptr;
        } else {
            Amg::Vector3D position = correctedParameters->position();

            double deltaPhi = 0.;
            double deltaR   = (position - perigee->position()).perp();

            if (std::abs(deltaR * M_PI) > std::abs(perigee->parameters()[Trk::d0])) {
                deltaPhi = perigee->parameters()[Trk::d0] / deltaR;
            }

            ATH_MSG_DEBUG("standaloneFit:    large perigee impact "
                          << perigee->parameters()[Trk::d0] << "  deltaR, deltaPhi " << deltaR << ", " << deltaPhi);

            parameterVector[Trk::phi0] += deltaPhi;

            if (parameterVector[Trk::phi0] > M_PI) {
                parameterVector[Trk::phi0] -= 2. * M_PI;
            } else if (parameterVector[Trk::phi0] < -M_PI) {
                parameterVector[Trk::phi0] += 2. * M_PI;
            }

            delete correctedParameters;
            delete perigee;

            correctedParameters = parameters->associatedSurface().createTrackParameters(
                parameterVector[Trk::loc1], parameterVector[Trk::loc2], parameterVector[Trk::phi],
                parameterVector[Trk::theta], parameterVector[Trk::qOverP],
                new AmgSymMatrix(5)(*parameters->covariance()));

            perigee = propagator->propagate(*correctedParameters, *mperigeeSurface, Trk::oppositeMomentum, false,
                                            m_magFieldProperties, Trk::nonInteracting);

            if (perigee) {
                deltaPhi = 0.;
                deltaR   = (position - perigee->position()).perp();

                if (std::abs(deltaR * M_PI) > std::abs(perigee->parameters()[Trk::d0])) {
                    deltaPhi = perigee->parameters()[Trk::d0] / deltaR;
                }

                ATH_MSG_VERBOSE("standaloneFit: corrected perigee impact " << perigee->parameters()[Trk::d0]
                                                                           << "  deltaR, deltaPhi " << deltaR << ", "
                                                                           << deltaPhi);

                parameterVector[Trk::phi0] += deltaPhi;
                if (parameterVector[Trk::phi0] > M_PI) {
                    parameterVector[Trk::phi0] -= 2. * M_PI;
                } else if (parameterVector[Trk::phi0] < -M_PI) {
                    parameterVector[Trk::phi0] += 2. * M_PI;
                }

                delete correctedParameters;
                delete perigee;
                correctedParameters = parameters->associatedSurface().createTrackParameters(
                    parameterVector[Trk::loc1], parameterVector[Trk::loc2], parameterVector[Trk::phi],
                    parameterVector[Trk::theta], parameterVector[Trk::qOverP],
                    new AmgSymMatrix(5)(*parameters->covariance()));

                perigee = propagator->propagate(*correctedParameters, *mperigeeSurface, Trk::oppositeMomentum, false,
                                                m_magFieldProperties, Trk::nonInteracting);
            }

            if (perigee) {
                ATH_MSG_VERBOSE("standaloneFit:    restart with impact " << perigee->parameters()[Trk::d0] << "   phi0 "
                                                                         << perigee->parameters()[Trk::phi0]);
            }

            if (correctedParameters == parameters) {
                ATH_MSG_WARNING(
                    "deleting parameters pointer that could be used further down in execution, setting it to zero!");
                parameters = nullptr;
            }

            delete correctedParameters;
            parameterVector[Trk::qOverP] = parameters->charge() / trackEnergy;
            correctedParameters          = parameters->associatedSurface().createTrackParameters(
                parameterVector[Trk::loc1], parameterVector[Trk::loc2], parameterVector[Trk::phi],
                parameterVector[Trk::theta], parameterVector[Trk::qOverP],
                new AmgSymMatrix(5)(*parameters->covariance()));

            delete parameters;
            parameters = correctedParameters;
        }

        // cut if perigee outside indet (but keep endcap halo)
        if (!perigee || !m_indetVolume->inside(perigee->position())) {
            if (perigee && perigee->position().z() * perigee->momentum().z() < 0. && perigee->momentum().eta() > 2.0) {
                ATH_MSG_DEBUG("standaloneFit: halo candidate, perigee at R  " << perigee->position().perp() << "  Z "
                                                                              << perigee->position().z());
            } else {
                ATH_MSG_DEBUG("standaloneFit: perigee outside indet volume");
                delete parameters;
                delete perigee;
                ///  if (haveSpectrometerRefit) delete spectrometerFit;
                return nullptr;
            }
        }
        delete perigee;
    } else {
        // otherwise track out from origin (fix bug #54820)
        badlyDeterminedCurvature = true;
        Amg::Vector3D momentum   = parameters->position() * Gaudi::Units::TeV / parameters->position().mag();

        const Trk::TrackParameters* trigParameters = m_trackQuery->triggerStationParameters(spectrometerTrack);

        if (trigParameters) {
            momentum = trigParameters->position() * Gaudi::Units::TeV / trigParameters->position().mag();
        }

        if (msgLvl(MSG::VERBOSE)) {
            if (trigParameters) {
                ATH_MSG_VERBOSE("standaloneFit: imprecise curvature measurement -"
                                << " start with line from origin to 1st trigger station ");
            } else {
                ATH_MSG_VERBOSE("standaloneFit: imprecise curvature measurement -"
                                << " start with line from origin to 1st measurement ");
            }
        }

        Trk::TrackParameters* perigee = new Trk::Perigee(mvertex->position(), momentum, 1., *mperigeeSurface);

        correctedParameters = m_propagator->propagate(*perigee, perigee->associatedSurface(), Trk::alongMomentum, false,
                                                      m_magFieldProperties, Trk::nonInteracting);

        delete parameters;
        delete perigee;
        delete trigParameters;
        parameters = correctedParameters;

        if (!parameters) {
            ATH_MSG_DEBUG("standaloneFit: failed back extrapolation to perigee");
            return nullptr;
        }
    }

    return parameters;
}


void
CombinedMuonTrackBuilder::finalTrackBuild(Trk::Track*& track) const
{
    // as a final step:
    //   refit the track if any holes can be recovered,
    //   refit with optimization of the spectrometer hit errors,
    //  add the corresponding TrackSummary to the track
    countAEOTs(track, " finalTrackBuilt input ");

    if (!m_muonHoleRecovery.empty()) {
        // chi2 before recovery
        double chi2Before = normalizedChi2(*track);

        ATH_MSG_VERBOSE(" perform spectrometer hole recovery procedure... ");

        Trk::Track* recoveredTrack = m_muonHoleRecovery->recover(*track);

        if (!recoveredTrack || !recoveredTrack->fitQuality() || !checkTrack("finalTrackBuild1", recoveredTrack, track))
        {
            // final track lost, this should not happen
            m_messageHelper->printWarning(44);
            delete recoveredTrack;
        } else {
            double chi2After = normalizedChi2(*recoveredTrack);
            if (chi2After < m_badFitChi2 || chi2After < chi2Before + 0.1) {
                delete track;
                track = recoveredTrack;
            } else {
                ATH_MSG_VERBOSE(" track rejected by recovery as chi2 " << chi2After << "  compared to " << chi2Before);

                delete recoveredTrack;
                if (chi2Before > m_badFitChi2) {
                    delete track;
                    track = nullptr;
                    return;
                }
            }
        }
        ATH_MSG_VERBOSE(" finished hole recovery procedure ");
    }

    // eventually this whole tool will use unique_ptrs
    // in the meantime, this allows the MuonErrorOptimisationTool and MuonRefitTool to use them
    std::unique_ptr<Trk::Track> trackUnique(track);
    // final fit with optimized spectrometer errors
    if (!m_muonErrorOptimizer.empty() && !trackUnique->info().trackProperties(Trk::TrackInfo::StraightTrack)
        && countAEOTs(trackUnique.get(), " before optimize ") == 0)
    {
        ATH_MSG_VERBOSE(" perform spectrometer error optimization... ");
        std::unique_ptr<Trk::Track> optimizedTrack = m_muonErrorOptimizer->optimiseErrors(trackUnique.get());
        if (optimizedTrack && checkTrack("finalTrackBuild2", optimizedTrack.get(), trackUnique.get())) {
            trackUnique.swap(optimizedTrack);
            countAEOTs(track, " finalTrackBuilt alignment errors Track ");
        }
    }

    // add the track summary
    m_trackSummary->updateTrack(*trackUnique);
    // have to use release until the whole code uses unique_ptr
    track = trackUnique.release();
}

Trk::Track*
CombinedMuonTrackBuilder::interfaceNotImplemented() const
{
    ATH_MSG_WARNING(" this ITrackFitter interface has not been implemented."
                    << " It is not relevant for combined muon tracks");
    return nullptr;
}

void
CombinedMuonTrackBuilder::momentumUpdate(const Trk::TrackParameters*& parameters, double updatedP, bool directionUpdate,
                                         double deltaPhi, double deltaTheta) const
{
    if (!parameters) return;

    const Trk::TrackParameters* originalParameters = parameters;
    const Trk::TrackParameters* updatedParameters  = nullptr;

    // update for angle change
    Amg::Vector3D direction = parameters->momentum().unit();

    if (directionUpdate) {
        double cosDeltaPhi = 0.;
        double sinDeltaPhi = sin(deltaPhi);

        if (std::abs(sinDeltaPhi) < 1.) {
            cosDeltaPhi = sqrt(1. - sinDeltaPhi * sinDeltaPhi);
        }

        double cosDeltaTheta = 0.;
        double sinDeltaTheta = sin(deltaTheta);

        if (std::abs(sinDeltaTheta) < 1.) {
            cosDeltaTheta = sqrt(1. - sinDeltaTheta * sinDeltaTheta);
        }

        double cosTheta = direction.z() * cosDeltaTheta - direction.perp() * sinDeltaTheta;
        if (std::abs(cosTheta) < 1.) {

            direction = Amg::Vector3D(direction.x() * cosDeltaPhi - direction.y() * sinDeltaPhi,
                                      direction.y() * cosDeltaPhi + direction.x() * sinDeltaPhi,
                                      direction.perp() * cosTheta / sqrt(1. - cosTheta * cosTheta));

        } else {
            direction = Amg::Vector3D(0., 0., cosTheta);
        }
        direction = direction.unit();
    }

    // update for momentum (magnitude) change
    Amg::Vector3D momentum = updatedP * direction;

    // create updated parameters
    double        charge        = parameters->charge();
    Amg::Vector3D position      = parameters->position();
    AmgSymMatrix(5)* covariance = parameters->covariance() ? new AmgSymMatrix(5)(*(parameters->covariance())) : nullptr;
    const Trk::Surface* surface = &(parameters->associatedSurface());
    updatedParameters           = surface->createTrackParameters(position, momentum, charge, covariance);

    if (updatedParameters) {
        parameters = updatedParameters;
        delete originalParameters;
    } else {
        // update failed, keeping original value
        m_messageHelper->printWarning(45);
    }
}


double
CombinedMuonTrackBuilder::normalizedChi2(const Trk::Track& track) const
{
    double chi2 = 999999.;
    if (track.fitQuality()) {
        if (track.fitQuality()->numberDoF()) {
            chi2 = track.fitQuality()->chiSquared() / track.fitQuality()->doubleNumberDoF();
        } else {
            chi2 = m_badFitChi2;
        }
    }

    return chi2;
}


const Trk::Track*
CombinedMuonTrackBuilder::reallocateMaterial(const Trk::Track& spectrometerTrack) const
{
    // build MeasurementSet for the spectrometer
    const Trk::TrackParameters* perigeeStartValue = nullptr;
    double                      perigeeDistance   = 0.;

    Trk::MeasurementSet spectrometerMeasurements;

    DataVector<const Trk::TrackStateOnSurface>::const_iterator s    = spectrometerTrack.trackStateOnSurfaces()->begin();
    auto                                                       sEnd = spectrometerTrack.trackStateOnSurfaces()->end();
    for (; s != sEnd; ++s) {
        if ((**s).measurementOnTrack() && !(**s).type(Trk::TrackStateOnSurface::Outlier)) {
            // // skip leading pseudo measurement(s)
            // if (! perigeeStartValue
            //  && dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**s).measurementOnTrack()))
            //  continue;

            // skip pseudo measurement(s)
            // FIXME - need phi pseudo in some cases
            if (dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**s).measurementOnTrack())) {
                continue;
            }

            spectrometerMeasurements.push_back((**s).measurementOnTrack()->clone());
            if (!(**s).trackParameters()
                || (perigeeStartValue && (**s).trackParameters()->position().mag() > perigeeDistance)) {

                continue;
            }

            perigeeDistance   = (**s).trackParameters()->position().mag();
            perigeeStartValue = (**s).trackParameters();
        }
    }

    // check perigeeStartValue defined
    if (!perigeeStartValue) {
        // FIXME: use spectrometerTrack.perigeeParameters()
        // null perigeeStartValue
        m_messageHelper->printWarning(46);
        return nullptr;
    }

    // fit with various recovery strategies
    Trk::Track* spectrometerFit = fit(spectrometerMeasurements, *perigeeStartValue, true, Trk::muon);
    if (!spectrometerFit) {

        spectrometerFit = fit(spectrometerMeasurements, *perigeeStartValue, false, Trk::muon);

        if (!spectrometerFit) {

            spectrometerFit = fit(spectrometerMeasurements, *perigeeStartValue, false, Trk::nonInteracting);

            if (!spectrometerFit) {
                // spectrometer refit fails
                m_messageHelper->printWarning(47);
            }
        }
    }

    if (spectrometerFit) {
        spectrometerFit->info().addPatternReco(spectrometerTrack.info());
    }

    Trk::MeasurementSet::iterator m    = spectrometerMeasurements.begin();
    auto                          mEnd = spectrometerMeasurements.end();
    for (; m != mEnd; ++m) {
        delete *m;
    }

    return spectrometerFit;
}

void
CombinedMuonTrackBuilder::removeSpectrometerMaterial(Trk::Track*& track) const
{
    // limit momentum to avoid refit allocating excessive energy loss
    bool   limitMomentum = false;
    double momentum      = track->perigeeParameters()->momentum().mag();
    double qOverP        = 0.;

    if (momentum > m_lowMomentum) {
        const Trk::Perigee* measuredPerigee = track->perigeeParameters();

        if (measuredPerigee) {
            DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator r =
                track->trackStateOnSurfaces()->rbegin();

            while (!(**r).trackParameters()) {
                --r;
            }

            limitMomentum = true;

            if (!measuredPerigee->covariance()) {
                ATH_MSG_DEBUG("measuredPerigee has no covariance, qOverP not set");
                qOverP = (**r).trackParameters()->parameters()[Trk::qOverP];
            } else {
                qOverP = (**r).trackParameters()->parameters()[Trk::qOverP]
                         + measuredPerigee->charge() * sqrt((*measuredPerigee->covariance())(Trk::qOverP, Trk::qOverP));

                ATH_MSG_DEBUG(" limit momentum to " << 1. / std::abs(qOverP * Gaudi::Units::GeV)
                                                    << "  from original value " << momentum / Units::GeV);
            }
        }
    }

    // remove spectrometer material from track
    const Trk::FitQualityOnSurface* fitQoS          = nullptr;
    const Trk::MaterialEffectsBase* materialEffects = nullptr;
    const Trk::MeasurementBase*     measurementBase = nullptr;
    const Trk::TrackParameters*     trackParameters = nullptr;

    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> defaultType;
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type = defaultType;

    DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>;

    trackStateOnSurfaces->reserve(track->trackStateOnSurfaces()->size());

    DataVector<const Trk::TrackStateOnSurface>::const_iterator s    = track->trackStateOnSurfaces()->begin();
    auto                                                       sEnd = track->trackStateOnSurfaces()->end();
    for (; s != sEnd; ++s) {
        // limit perigee
        if ((**s).trackParameters()) {

            if (limitMomentum && s == track->trackStateOnSurfaces()->begin() && (**s).trackParameters()->covariance()
                && dynamic_cast<const Trk::Perigee*>(*s))
            {

                Amg::VectorX parameterVector = (**s).trackParameters()->parameters();
                parameterVector[Trk::qOverP] = qOverP;

                /// aaaaaahhhhh
                const Trk::Perigee* parameters = dynamic_cast<const Trk::Perigee*>(
                    (**s).trackParameters()->associatedSurface().createTrackParameters(
                        parameterVector[Trk::loc1], parameterVector[Trk::loc2], parameterVector[Trk::phi],
                        parameterVector[Trk::theta], parameterVector[Trk::qOverP],
                        new AmgSymMatrix(5)(*(**s).trackParameters()->covariance())));

                type = defaultType;
                type.set(Trk::TrackStateOnSurface::Perigee);

                if ((**s).measurementOnTrack()) {
                    measurementBase = (**s).measurementOnTrack()->clone();
                    type.set(Trk::TrackStateOnSurface::Measurement);
                }

                trackStateOnSurfaces->push_back(
                    new const Trk::TrackStateOnSurface(measurementBase, parameters, fitQoS, materialEffects, type));
            } else {
                trackStateOnSurfaces->push_back((**s).clone());
            }
            continue;
        }

        // material in spectrometer
        if ((**s).materialEffectsOnTrack()
            && !m_calorimeterVolume->inside((**s).materialEffectsOnTrack()->associatedSurface().globalReferencePoint()))
        {

            if ((**s).measurementOnTrack()) {

                materialEffects              = nullptr;
                Amg::VectorX parameterVector = (**s).trackParameters()->parameters();
                if (limitMomentum) {
                    parameterVector[Trk::qOverP] = qOverP;
                }
                trackParameters = (**s).trackParameters()->associatedSurface().createTrackParameters(
                    parameterVector[Trk::loc1], parameterVector[Trk::loc2], parameterVector[Trk::phi],
                    parameterVector[Trk::theta], parameterVector[Trk::qOverP],
                    (**s).trackParameters()->covariance() ? new AmgSymMatrix(5)(*(**s).trackParameters()->covariance())
                                                          : nullptr);

                type = defaultType;
                type.set(Trk::TrackStateOnSurface::Measurement);

                if ((**s).type(Trk::TrackStateOnSurface::Outlier)) {
                    type.set(Trk::TrackStateOnSurface::Outlier);
                }

                trackStateOnSurfaces->push_back(new const Trk::TrackStateOnSurface(
                    (**s).measurementOnTrack()->clone(), trackParameters, fitQoS, materialEffects, type));
            }
            continue;
        } else if (!(**s).measurementOnTrack() && (**s).trackParameters()
                   && !m_calorimeterVolume->inside((**s).trackParameters()->position()))
        {
            continue;
        }

        if (limitMomentum && (**s).trackParameters()) {
            materialEffects              = nullptr;
            measurementBase              = nullptr;
            Amg::VectorX parameterVector = (**s).trackParameters()->parameters();
            parameterVector[Trk::qOverP] = qOverP;

            trackParameters = (**s).trackParameters()->associatedSurface().createTrackParameters(
                parameterVector[Trk::loc1], parameterVector[Trk::loc2], parameterVector[Trk::phi],
                parameterVector[Trk::theta], parameterVector[Trk::qOverP],
                (**s).trackParameters()->covariance() ? new AmgSymMatrix(5)(*(**s).trackParameters()->covariance())
                                                      : nullptr);

            type = defaultType;

            if ((**s).measurementOnTrack()) {
                type.set(Trk::TrackStateOnSurface::Measurement);

                if ((**s).type(Trk::TrackStateOnSurface::Outlier)) {
                    type.set(Trk::TrackStateOnSurface::Outlier);
                }

                measurementBase = (**s).measurementOnTrack()->clone();
            }

            if ((**s).materialEffectsOnTrack()) {
                if ((**s).type(Trk::TrackStateOnSurface::CaloDeposit)) {
                    type.set(Trk::TrackStateOnSurface::CaloDeposit);
                }

                if ((**s).type(Trk::TrackStateOnSurface::Scatterer)) {
                    type.set(Trk::TrackStateOnSurface::Scatterer);
                }

                materialEffects = (**s).materialEffectsOnTrack()->clone();
            }
            trackStateOnSurfaces->push_back(
                new const Trk::TrackStateOnSurface(measurementBase, trackParameters, fitQoS, materialEffects, type));
        } else {
            trackStateOnSurfaces->push_back((**s).clone());
        }
    }

    // replace track
    Trk::TrackInfo   trackInfo  = track->info();
    Trk::FitQuality* fitQuality = nullptr;
    if (track->fitQuality()) {
        fitQuality = new Trk::FitQuality(*track->fitQuality());
    }
    delete track;

    track = new Trk::Track(trackInfo, trackStateOnSurfaces, fitQuality);
}

Trk::PseudoMeasurementOnTrack*
CombinedMuonTrackBuilder::vertexOnTrack(const Trk::TrackParameters& parameters, const Trk::RecVertex* vertex,
                                        const Trk::RecVertex* mbeamAxis) const
{
    // create the corresponding PerigeeSurface, localParameters and covarianceMatrix
    const Trk::PerigeeSurface surface(vertex->position());
    Trk::LocalParameters      localParameters;
    Amg::MatrixX              covarianceMatrix;
    covarianceMatrix.setZero();

    // transform Cartesian (x,y,z) to beam axis or perigee
    Amg::Vector2D localPosition(0, 0);
    double        ptInv = 1. / parameters.momentum().perp();

    if (vertex == mbeamAxis) {
        Trk::DefinedParameter d0(localPosition[Trk::locX], Trk::locX);
        localParameters = Trk::LocalParameters(d0);

        Amg::MatrixX jacobian(1, 3);
        jacobian.setZero();
        jacobian(0, 0) = -ptInv * parameters.momentum().y();
        jacobian(0, 1) = ptInv * parameters.momentum().x();

        const Amg::MatrixX& cov = vertex->covariancePosition();
        covarianceMatrix        = cov.similarity(jacobian);
    } else {
        localParameters = Trk::LocalParameters(localPosition);

        Amg::MatrixX jacobian(2, 3);
        jacobian.setZero();
        jacobian(0, 0) = -ptInv * parameters.momentum().y();
        jacobian(0, 1) = ptInv * parameters.momentum().x();
        jacobian(1, 2) = 1.0;

        const Amg::MatrixX& cov = vertex->covariancePosition();
        covarianceMatrix        = cov.similarity(jacobian);
    }

    return new Trk::PseudoMeasurementOnTrack(localParameters, covarianceMatrix, surface);
}


int
CombinedMuonTrackBuilder::countAEOTs(const Trk::Track* track, std::string txt) const
{
    const DataVector<const Trk::TrackStateOnSurface>* trackTSOS = track->trackStateOnSurfaces();
    int                                               naeots    = 0;

    for (auto m : *trackTSOS) {
        if (m->alignmentEffectsOnTrack()) naeots++;
    }

    ATH_MSG_DEBUG(" count AEOTs " << txt << " " << naeots);

    // add VEBOSE for checking TSOS order

    DataVector<const Trk::TrackStateOnSurface>::const_iterator it     = track->trackStateOnSurfaces()->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator it_end = track->trackStateOnSurfaces()->end();

    int tsos     = 0;
    int nperigee = 0;

    for (; it != it_end; ++it) {
        tsos++;

        if ((*it)->type(Trk::TrackStateOnSurface::Perigee)) {
            ATH_MSG_DEBUG("perigee");
            nperigee++;
        }

        if ((*it)->trackParameters()) {
            ATH_MSG_VERBOSE(" check tsos " << tsos << " TSOS tp "
                                           << " r " << (*it)->trackParameters()->position().perp() << " z "
                                           << (*it)->trackParameters()->position().z() << " momentum "
                                           << (*it)->trackParameters()->momentum().mag());
        } else if ((*it)->measurementOnTrack()) {
            ATH_MSG_VERBOSE(" check tsos " << tsos << " TSOS mst "
                                           << " r " << (*it)->measurementOnTrack()->associatedSurface().center().perp()
                                           << " z " << (*it)->measurementOnTrack()->associatedSurface().center().z());
        } else if ((*it)->materialEffectsOnTrack()) {
            ATH_MSG_VERBOSE(" check tsos "
                            << tsos << " TSOS mat "
                            << " r "
                            << (*it)->materialEffectsOnTrack()->associatedSurface().globalReferencePoint().perp()
                            << " z "
                            << (*it)->materialEffectsOnTrack()->associatedSurface().globalReferencePoint().z());
        } else {
            ATH_MSG_VERBOSE(" check tsos other than above " << tsos);
        }
    }

    ATH_MSG_VERBOSE(" track with number of TSOS perigees " << nperigee);

    return naeots;
}


void
CombinedMuonTrackBuilder::dumpCaloEloss(const Trk::Track* track, std::string txt) const
{
    // will refit if extrapolated track was definitely bad
    if (!track) return;
    if (!m_trackQuery->isCaloAssociated(*track)) {
        ATH_MSG_DEBUG(txt << " no TSOS in Calorimeter ");
        return;
    }

    const Trk::Track& originalTrack = *track;
    const CaloEnergy* caloEnergy    = m_trackQuery->caloEnergy(originalTrack);
    if (caloEnergy) {
        ATH_MSG_DEBUG(txt << " Calorimeter Eloss " << caloEnergy->deltaE());
    } else {
        ATH_MSG_DEBUG(txt << " No Calorimeter Eloss");
    }

    const DataVector<const Trk::TrackStateOnSurface>* trackTSOS = track->trackStateOnSurfaces();

    double Eloss      = 0.;
    double idEloss    = 0.;
    double caloEloss  = 0.;
    double msEloss    = 0.;
    double deltaP     = 0.;
    double pcalo      = 0.;
    double pstart     = 0.;
    double eta        = 0.;
    double pMuonEntry = 0.;

    for (auto m : *trackTSOS) {
        const Trk::MeasurementBase* mot = m->measurementOnTrack();

        if (m->trackParameters()) {
            pMuonEntry = m->trackParameters()->momentum().mag();
        }

        if (mot) {
            Identifier id = Trk::IdentifierExtractor::extract(mot);
            if (id.is_valid()) {
                // skip after first Muon hit
                if (m_idHelperSvc->isMuon(id)) {
                    break;
                }
            }
        }

        if (pstart == 0 && m->trackParameters()) {
            pstart = m->trackParameters()->momentum().mag();
            eta    = m->trackParameters()->momentum().eta();

            ATH_MSG_DEBUG("Start pars found eta " << eta << " r " << (m->trackParameters())->position().perp() << " z "
                                                  << (m->trackParameters())->position().z() << " pstart " << pstart);
        }

        if (m->materialEffectsOnTrack()) {
            const Trk::MaterialEffectsOnTrack* meot =
                dynamic_cast<const Trk::MaterialEffectsOnTrack*>(m->materialEffectsOnTrack());

            if (meot) {
                if (meot->thicknessInX0() > 20) {
                    const Trk::ScatteringAngles* scatAngles = meot->scatteringAngles();

                    ATH_MSG_DEBUG(" Calorimeter X0  " << meot->thicknessInX0() << "  pointer scat " << scatAngles);

                    if (scatAngles) {
                        pcalo            = m->trackParameters()->momentum().mag();
                        double pullPhi   = scatAngles->deltaPhi() / scatAngles->sigmaDeltaPhi();
                        double pullTheta = scatAngles->deltaTheta() / scatAngles->sigmaDeltaTheta();

                        ATH_MSG_DEBUG(" Calorimeter scatterer deltaPhi (mrad) "
                                      << 1000 * scatAngles->deltaPhi() << " sigma "
                                      << 1000 * scatAngles->sigmaDeltaPhi() << " pull " << pullPhi
                                      << " deltaTheta (mrad) " << 1000 * scatAngles->deltaTheta() << " sigma "
                                      << 1000 * scatAngles->sigmaDeltaTheta() << " pull " << pullTheta);
                    }
                }

                const Trk::EnergyLoss* energyLoss = meot->energyLoss();
                if (energyLoss) {
                    if (m->trackParameters()) {
                        ATH_MSG_DEBUG("Eloss found r " << (m->trackParameters())->position().perp() << " z "
                                                       << (m->trackParameters())->position().z() << " value "
                                                       << energyLoss->deltaE() << " Eloss " << Eloss);
                    }

                    if (m->type(Trk::TrackStateOnSurface::CaloDeposit)) {
                        idEloss   = Eloss;
                        caloEloss = std::abs(energyLoss->deltaE());
                        Eloss     = 0.;

                        if (m->trackParameters()) {
                            deltaP = m->trackParameters()->momentum().mag() - pcalo;
                        }

                        const Trk::Surface& surface = m->surface();

                        ATH_MSG_DEBUG(" Calorimeter surface " << surface);
                        ATH_MSG_DEBUG(txt << " Calorimeter delta p " << deltaP << " deltaE " << caloEloss
                                          << " delta pID = pcaloEntry-pstart " << pcalo - pstart);

                    } else {
                        Eloss += std::abs(energyLoss->deltaE());
                    }
                }
            }
        }
    }

    msEloss = Eloss;
    Eloss   = idEloss + caloEloss + msEloss;

    ATH_MSG_DEBUG(txt << " eta " << eta << " pstart " << pstart / Units::GeV << " Eloss on TSOS idEloss " << idEloss
                      << " caloEloss " << caloEloss << " msEloss " << msEloss << " Total " << Eloss
                      << " pstart - pMuonEntry " << pstart - pMuonEntry);

    return;
}


bool
CombinedMuonTrackBuilder::checkTrack(std::string txt, Trk::Track* newTrack, Trk::Track* track) const
{
    if (!newTrack) return false;

    const DataVector<const Trk::TrackParameters>* pars = newTrack->trackParameters();
    if (!pars || pars->empty()) {
        return false;
    }

    bool newTrackOK = true;

    DataVector<const Trk::TrackParameters>::const_iterator it = pars->end();
    it                                                        = it - 1;

    if ((*it)->position().dot((*it)->momentum().unit()) < 0) {
        newTrackOK = false;
        ATH_MSG_DEBUG(txt << " checkTrack newTrack ALARM position " << (*it)->position() << " direction "
                          << (*it)->momentum().unit());
    } else {
        ATH_MSG_DEBUG(txt << " checkTrack newTrack OK position " << (*it)->position() << " direction "
                          << (*it)->momentum().unit());
    }

    if (!track) {
        return newTrackOK;
    }

    for (const auto& par : *pars) {
        if (!par->covariance()) {
            continue;
        }

        if ((*par->covariance())(0, 0) < 0 || (*par->covariance())(1, 1) < 0 || (*par->covariance())(2, 2) < 0
            || (*par->covariance())(3, 3) < 0 || (*par->covariance())(4, 4) < 0)
        {

            ATH_MSG_DEBUG(Amg::toString(*par->covariance()));
            ATH_MSG_DEBUG("covariance matrix has negative diagonal element, killing track");
            return false;
        }
    }

    return newTrackOK;
}


}  // namespace Rec

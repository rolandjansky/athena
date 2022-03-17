/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// CombinedMuonTrackBuilder
//  AlgTool gathering  material effects along a combined muon track, in
//  particular the TSOS'es representing the calorimeter energy deposit and
//  Coulomb scattering.
//  The resulting track is fitted at the IP
//
//////////////////////////////////////////////////////////////////////////////

#include "CombinedMuonTrackBuilder.h"

#include <cmath>
#include <iomanip>
#include <memory>
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "AthenaKernel/Units.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "FourMomUtils/xAODP4Helpers.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
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
#include "MuonTrackMakerUtils/MuonTSOSHelper.h"

namespace {
    /// sigma of phi sector for pseudo-measurement constraint
    const double s_sigmaPhiSector = std::tan(0.125 * M_PI / std::sqrt(12.));
    
}
namespace Rec {
    CombinedMuonTrackBuilder::~CombinedMuonTrackBuilder() {}
    CombinedMuonTrackBuilder::CombinedMuonTrackBuilder(const std::string& type, const std::string& name, const IInterface* parent) :
        AthAlgTool(type, name, parent),
        m_alignUncertTool_theta("Muon::MuonAlignmentUncertTool/MuonAlignmentUncertToolTheta"),
        m_alignUncertTool_phi("Muon::MuonAlignmentUncertTool/MuonAlignmentUncertToolPhi") {
        m_messageHelper = std::make_unique<MessageHelper>(*this, 50);

        declareInterface<ICombinedMuonTrackBuilder>(this);

        declareProperty("AlignmentUncertToolTheta", m_alignUncertTool_theta);
        declareProperty("AlignmentUncertToolPhi", m_alignUncertTool_phi);
    }

    StatusCode CombinedMuonTrackBuilder::initialize() {
        ATH_MSG_DEBUG("Initializing CombinedMuonTrackBuilder.");
        ATH_MSG_DEBUG(" with options: ");

        if (m_allowCleanerVeto) ATH_MSG_DEBUG(" AllowCleanerVeto");
        if (m_cleanCombined) ATH_MSG_DEBUG(" CleanCombined");
        if (m_cleanStandalone) ATH_MSG_DEBUG(" CleanStandalone");
        if (m_perigeeAtSpectrometerEntrance) ATH_MSG_DEBUG(" PerigeeAtSpectrometerEntrance");
        if (m_reallocateMaterial) ATH_MSG_DEBUG(" ReallocateMaterial");
        if (!m_cscRotCreator.empty()) ATH_MSG_DEBUG("Redo CscRots");
        if (!m_mdtRotCreator.empty()) ATH_MSG_DEBUG("Redo MdtRots");
        if (!m_mmRotCreator.empty()) ATH_MSG_DEBUG("Redo MicroMegaRots");
        if (!m_muonErrorOptimizer.empty()) ATH_MSG_DEBUG(" ErrorOptimisation");
        if (!m_muonHoleRecovery.empty()) ATH_MSG_DEBUG(" HoleRecovery");

        // fill WARNING messages
        m_messageHelper->setMaxNumberOfMessagesPrinted(m_maxWarnings);
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
        m_messageHelper->setMessage(30, "fit:: about to add the TSOS's describing calorimeter association to a combined muon");
        m_messageHelper->setMessage(31, "fit:: particle hypothesis must be 0 or 2 (nonInteracting or muon). Requested: ");
        m_messageHelper->setMessage(32, "fit:: particle hypothesis must be 0 or 2 (nonInteracting or muon). Requested: ");
        m_messageHelper->setMessage(33, "fit:: combined muon track is missing the TSOS's describing calorimeter association");
        m_messageHelper->setMessage(34, "appendSelectedTSOS:: skip duplicate measurement");
        m_messageHelper->setMessage(35, "caloEnergyParameters:: muonTrack without caloEnergy association");
        m_messageHelper->setMessage(36, "caloEnergyParameters:: combinedTrack without caloEnergy association");
        m_messageHelper->setMessage(37, "createMuonTrack:: should never happen: FSR caloEnergy delete");
        m_messageHelper->setMessage(38, "createSpectrometerTSOS:: missing MeasuredPerigee for spectrometer track");
        m_messageHelper->setMessage(39, "createSpectrometerTSOS:: skip unrecognized TSOS without TrackParameters. Type: ");
        m_messageHelper->setMessage(40, "createSpectrometerTSOS:: skip duplicate measurement on same Surface. Type: ");
        m_messageHelper->setMessage(41, "entrancePerigee:: missing TrackingGeometrySvc - no perigee will be added at MS entrance");
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
        if (!m_mmRotCreator.empty()) {
            m_redoRots = true;
            ATH_CHECK(m_mmRotCreator.retrieve());
            ATH_MSG_DEBUG("Retrieved tool " << m_mmRotCreator);
        }

        ATH_CHECK(m_extrapolator.retrieve());
        ATH_MSG_DEBUG("Retrieved tool " << m_extrapolator);
        ATH_CHECK(m_fitter.retrieve());
        ATH_MSG_DEBUG("Retrieved tool " << m_fitter);
        ATH_CHECK(m_fitterSL.retrieve());
        ATH_MSG_DEBUG("Retrieved tool " << m_fitterSL);
        ATH_CHECK(m_idHelperSvc.retrieve());
        ATH_MSG_DEBUG("Retrieved tool " << m_idHelperSvc);
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

        if (!m_trackingGeometryReadKey.empty()) {
            ATH_CHECK(m_trackingGeometryReadKey.initialize());
        } else {
            ATH_MSG_ERROR("Could not retrieve a valid tracking geometry");
        }
        // need to know which TrackingVolume we are in: indet/calo/spectrometer
        ATH_CHECK(m_trackingVolumesSvc.retrieve());
        ATH_MSG_DEBUG("Retrieved Svc " << m_trackingVolumesSvc);

        m_calorimeterVolume =
            std::make_unique<Trk::Volume>(m_trackingVolumesSvc->volume(Trk::ITrackingVolumesSvc::MuonSpectrometerEntryLayer));
        m_indetVolume = std::make_unique<Trk::Volume>(m_trackingVolumesSvc->volume(Trk::ITrackingVolumesSvc::CalorimeterEntryLayer));

        // create beamAxis and vertexRegion for constrained (projective) track fits
        Amg::Vector3D origin(0., 0., 0.);
        m_perigeeSurface = std::make_unique<Trk::PerigeeSurface>(origin);

        AmgSymMatrix(3) beamAxisCovariance;
        beamAxisCovariance.setZero();
        (beamAxisCovariance)(0, 0) = m_vertex2DSigmaRPhi * m_vertex2DSigmaRPhi;
        (beamAxisCovariance)(1, 1) = m_vertex2DSigmaRPhi * m_vertex2DSigmaRPhi;
        (beamAxisCovariance)(2, 2) = m_vertex2DSigmaZ * m_vertex2DSigmaZ;
        m_beamAxis = std::make_unique<Trk::RecVertex>(origin, beamAxisCovariance);

        AmgSymMatrix(3) vertexRegionCovariance;
        vertexRegionCovariance.setZero();
        (vertexRegionCovariance)(0, 0) = m_vertex3DSigmaRPhi * m_vertex3DSigmaRPhi;
        (vertexRegionCovariance)(1, 1) = m_vertex3DSigmaRPhi * m_vertex3DSigmaRPhi;
        (vertexRegionCovariance)(2, 2) = m_vertex3DSigmaZ * m_vertex3DSigmaZ;
        m_vertex = std::make_unique<Trk::RecVertex>(origin, vertexRegionCovariance);
        if (m_addIDMSerrors) {
            ATH_CHECK(m_alignUncertTool_theta.retrieve());
            ATH_CHECK(m_alignUncertTool_phi.retrieve());
        }
#ifndef NDEBUG
        ATH_MSG_DEBUG(" vertex region: ");
        m_vertex->dump(msg(MSG::DEBUG));
#endif

        return StatusCode::SUCCESS;
    }

    StatusCode CombinedMuonTrackBuilder::finalize() {
        if (m_countAcceptedStandaloneFit) {
            double norm = 100. / static_cast<double>(m_countAcceptedStandaloneFit);
            ATH_MSG_INFO("Finalizing CombinedMuonTrackBuilder:"
                         << endmsg << "     " << std::setiosflags(std::ios::fixed) << std::setw(4) << std::setprecision(2)
                         << norm * static_cast<double>(m_countBeamAxis) << "% with beamAxis constraint" << endmsg << "     " << std::setw(4)
                         << std::setprecision(2) << norm * static_cast<double>(m_countVertexRegion) << "% with vertexRegion constraint"
                         << endmsg << "     " << m_countDegradedStandaloneFit << " degraded standalone fit-chi2   " << endmsg << "     "
                         << m_countStandaloneCleanerVeto << " standalone fits with cleaner veto" << endmsg << "     "
                         << m_countExtensionCleanerVeto << " extension fits with cleaner veto" << endmsg << "     "
                         << m_countCombinedCleanerVeto << " combined fits with cleaner veto");
        }
        // summarize WARNINGs
        m_messageHelper->printSummary();
        return StatusCode::SUCCESS;
    }
    std::unique_ptr<Trk::Track> CombinedMuonTrackBuilder::combinedFit(const EventContext& ctx, const Trk::Track& indetTrack, const Trk::Track& extrapolatedTrack,
                                                                      const Trk::Track&) const {
        ATH_MSG_VERBOSE("===== Start of combinedFit:: ");

        if (msgLevel(MSG::DEBUG)) countAEOTs(&extrapolatedTrack, " extrapolatedTrack start combinedFit ");

        // require MeasuredPerigee for indetTrack
        const Trk::Perigee* indetPerigee = indetTrack.perigeeParameters();
        if (!indetPerigee) {
            // missing MeasuredPerigee for indet track
            m_messageHelper->printWarning(0);
            return nullptr;
        }

        // take inner calorimeter scattering surface from extrapolated track
        const Trk::Surface* surface = nullptr;
        if (m_trackQuery->isCaloAssociated(extrapolatedTrack, ctx)) {
            for (const Trk::TrackStateOnSurface* it : *extrapolatedTrack.trackStateOnSurfaces()) {
                if (!it->materialEffectsOnTrack()) continue;

                const Amg::Vector3D& position = it->materialEffectsOnTrack()->associatedSurface().globalReferencePoint();

                if (m_indetVolume->inside(position)) continue;
                if (!m_calorimeterVolume->inside(position)) break;

                surface = &it->materialEffectsOnTrack()->associatedSurface();
                break;
            }
        }

        // match extrapolated indet track to inner calorimeter scattering surface
        // provided momentum defined (solenoid on)
        MagField::AtlasFieldCache fieldCache;
        // Get field cache object
        if (!loadMagneticField(ctx, fieldCache)) return nullptr;

        if (surface && fieldCache.solenoidOn() && !m_updateWithCaloTG) {
            std::unique_ptr<const Trk::TrackStateOnSurface> innerTSOS;
            if (m_useCaloTG) {
                ATH_MSG_VERBOSE(" Retrieving Calorimeter TSOS from " << __func__ << " at line " << __LINE__);
                std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> caloTSOS =
                    getCaloTSOSfromMatProvider(*indetTrack.perigeeParameters(), extrapolatedTrack);
                if (!caloTSOS.empty()) { innerTSOS.swap(caloTSOS.front()); }
            } else {
                innerTSOS = m_caloTSOS->innerTSOS(ctx, *indetTrack.perigeeParameters());
            }

            if (!innerTSOS) {
                ATH_MSG_DEBUG(" indet track fails to intersect the calorimeter ");
                return nullptr;
            }

            // will re-associate the calorimeter if they are not the same surface
            double surfaceOffset =
                (surface->globalReferencePoint() - innerTSOS->materialEffectsOnTrack()->associatedSurface().globalReferencePoint()).mag();

            if (surfaceOffset > 1. * Gaudi::Units::mm) {
                ATH_MSG_DEBUG(" different inner-calo-surface obtained from indet extrapolation, "
                              << "surface reference " << surface->globalReferencePoint() << " with offset " << surfaceOffset
                              << "mm. Re-evaluate the caloTSOS ");

                surface = nullptr;
            }
        }

        std::unique_ptr<Trk::Track> muonTrack;
        if (!fieldCache.toroidOn()) {
            ATH_MSG_VERBOSE(" SL MS track: Calling createMuonTrack from " << __func__ << " at line " << __LINE__);
            muonTrack =
                createMuonTrack(ctx, indetTrack, indetTrack.perigeeParameters(), nullptr, extrapolatedTrack.trackStateOnSurfaces());
        } else {
            // create a muon track without perigee in case of non-optimal precision -
            // such as need to replace calorimeter material or presence of pseudomeasurements
            if (!surface) {  // extrapolate outwards to associate calorimeter material effects
                ATH_MSG_VERBOSE("Calling createMuonTrack from " << __func__ << " at line " << __LINE__);
                muonTrack = createMuonTrack(
                    ctx, extrapolatedTrack, indetTrack.perigeeParameters(), nullptr, extrapolatedTrack.trackStateOnSurfaces());
            } else if (m_trackQuery->numberPseudoMeasurements(extrapolatedTrack) > 1) {  // remove pseudo meas
                ATH_MSG_VERBOSE("Calling createMuonTrack from " << __func__ << " at line " << __LINE__);
                muonTrack = createMuonTrack(ctx, extrapolatedTrack, 
                                            nullptr, nullptr, extrapolatedTrack.trackStateOnSurfaces());
            } else {  // otherwise can just copy the extrapolated track
                ATH_MSG_VERBOSE("Calling createMuonTrack from " << __func__ << " at line " << __LINE__);
                muonTrack =
                    createMuonTrack(ctx, extrapolatedTrack, extrapolatedTrack.perigeeParameters(), nullptr,
                                    extrapolatedTrack.trackStateOnSurfaces());
            }
        }

        // no combined muon when failure to intersect calo
        if (!muonTrack) return nullptr;              
        if (msgLevel(MSG::DEBUG)) countAEOTs(muonTrack.get(), " muonTrack track before fit ");

        // combined track fit
        std::unique_ptr<Trk::Track> combinedTrack{fit(ctx, indetTrack, *muonTrack, m_cleanCombined, Trk::muon)};

        // quit if fit failure or all MS measurements removed by fit or perigee outside indet
        bool haveMS = false;
        bool perigeeOutside = false;

        if (combinedTrack) {
            if (msgLevel(MSG::DEBUG)) countAEOTs(combinedTrack.get(), " combinedTrack track after fit ");
            ATH_MSG_VERBOSE(" combined track " << m_printer->print(*combinedTrack) << std::endl
                                               << m_printer->printStations(*combinedTrack));

            auto rit = combinedTrack->trackStateOnSurfaces()->rbegin();
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

            if (!combinedTrack->perigeeParameters() || !m_indetVolume->inside(combinedTrack->perigeeParameters()->position())) {
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
            ATH_MSG_DEBUG("combinedTrack fails with bad fit" << combinedTrack.get() << " " << hasFitQ << " " << haveMS << " "
                                                             << perigeeOutside);

            return nullptr;
        }

        // Get parameters at calo position
        const Trk::TrackParameters* combinedEnergyParameters = nullptr;
        const Trk::TrackParameters* muonEnergyParameters = nullptr;
        const CaloEnergy* caloEnergy {caloEnergyParameters(combinedTrack.get(), muonTrack.get(), combinedEnergyParameters, muonEnergyParameters)};

        if (!caloEnergy) {
            // combinedTrack fails with missing caloEnergy
            m_messageHelper->printWarning(3);
            return nullptr;
        }

        // if significant momentum change: re-evaluate calo energy and refit
        double pRatio = muonEnergyParameters->momentum().mag() / combinedEnergyParameters->momentum().mag();
        if (std::abs(pRatio - 1.) > m_largeMomentumChange || m_iterateCombinedTrackFit) {
            if (!m_iterateCombinedTrackFit) {
                ATH_MSG_DEBUG(" iterate combined fit to recollect calorimeter material as significant momentum change after fit "
                              << pRatio << ", pT before " << muonEnergyParameters->momentum().perp() / Gaudi::Units::GeV << ", after "
                              << combinedEnergyParameters->momentum().perp() / Gaudi::Units::GeV << " GeV");
            } else {
                ATH_MSG_DEBUG(" iterate combined fit to recollect calorimeter material");
            }

            const Trk::TrackStates* combinedTSOS = combinedTrack->trackStateOnSurfaces();

            std::unique_ptr<Trk::Track> indetNewTrack{createIndetTrack(indetTrack.info(), combinedTSOS)};

            std::unique_ptr<Trk::Track> oldTrack(std::move(muonTrack));

            ATH_MSG_VERBOSE("Calling createMuonTrack from " << __func__ << " at line " << __LINE__);
            muonTrack = createMuonTrack(ctx, extrapolatedTrack, combinedEnergyParameters, nullptr, combinedTSOS);

            if (indetNewTrack && muonTrack) {
                std::unique_ptr<Trk::Track> refittedTrack{fit(ctx, *indetNewTrack, *muonTrack, m_cleanCombined, Trk::muon)};
                caloEnergy = caloEnergyParameters(refittedTrack.get(), muonTrack.get(), combinedEnergyParameters, muonEnergyParameters);

                if (caloEnergy) {
                    combinedTrack.swap(refittedTrack);
                } else {
                    // why does the refit fail? This shouldn't really be necessary
                    muonTrack.swap(oldTrack);
                    caloEnergy = caloEnergyParameters(combinedTrack.get(), muonTrack.get(), combinedEnergyParameters, muonEnergyParameters);
                }
            }
        }

        // tracks with caloEnergy type 'tail' can arise from an incorrect categorization as isolated
        // in case of significant energy gain, switch to parametrization and reclassify as NotIsolated
        if (muonTrack && caloEnergy->energyLossType() == CaloEnergy::Tail && (!m_updateWithCaloTG || m_useCaloTG)) {
            double tolerance = 0;

            if (!indetPerigee->covariance()) {
                ATH_MSG_WARNING(" indetPerigee has no covariance tolerance left as zero. ");
            } else {
                tolerance = m_numberSigmaFSR * std::sqrt((*indetPerigee->covariance())(Trk::qOverP, Trk::qOverP));
            }

            double indetMaxE = 1. / (std::abs(indetPerigee->parameters()[Trk::qOverP]) - tolerance);
            double energyBalance = combinedEnergyParameters->momentum().mag() + caloEnergy->deltaE() - indetMaxE;

            // get parametrised eloss if large energy imbalance and refit track
            std::unique_ptr<CaloEnergy> paramEnergy;
            if (indetMaxE > 0. && energyBalance > m_numberSigmaFSR * caloEnergy->sigmaMinusDeltaE()) {
                // parametrized energy deposition
                // run-2 schema, update default eloss with parametrised value
                if (m_useCaloTG) {
                    paramEnergy.reset(m_materialUpdator->getParamCaloELoss(muonTrack.get()));
                } else {
                    // run-1 schema, recalculate parametrised eloss
                    paramEnergy = m_caloEnergyParam->energyLoss(ctx, combinedEnergyParameters->momentum().mag(),
                                                                combinedEnergyParameters->position().eta(),
                                                                combinedEnergyParameters->position().phi());
                }
                paramEnergy->set_energyLossType(CaloEnergy::NotIsolated);
            }

            // FIXME: add criterion on energy-balance significance param vs tail ?
            if (paramEnergy) {
                ATH_MSG_DEBUG(" FSR check: energyBalance "
                              << energyBalance / Gaudi::Units::GeV << " signif " << energyBalance / caloEnergy->sigmaMinusDeltaE()
                              << " indet max E " << indetMaxE / Gaudi::Units::GeV << std::endl
                              << " param CaloEnergy: " << paramEnergy->deltaE() / Gaudi::Units::GeV << " + "
                              << paramEnergy->sigmaPlusDeltaE() / Gaudi::Units::GeV << " for P "
                              << combinedEnergyParameters->momentum().mag() / Gaudi::Units::GeV << "  eta "
                              << combinedEnergyParameters->position().eta() << "  phi " << combinedEnergyParameters->position().phi()
                              << endmsg << " tail-param energy diff "
                              << (caloEnergy->deltaE() - paramEnergy->deltaE()) / Gaudi::Units::GeV);

                ATH_MSG_VERBOSE("Calling createMuonTrack from " << __func__ << " at line " << __LINE__);
                muonTrack =
                    createMuonTrack(ctx, extrapolatedTrack, nullptr, std::move(paramEnergy), muonTrack->trackStateOnSurfaces());

                if (muonTrack) {
                    std::unique_ptr<Trk::Track> refittedTrack{fit(ctx, indetTrack, *muonTrack, m_cleanCombined, Trk::muon)};
                    if (refittedTrack) { combinedTrack.swap(refittedTrack); }
                }
            }
        }

        // in case of the unexpected ...
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
        //  We will either have nullptr or a new Track.
        //  What we pass stays untouched.
        std::unique_ptr<Trk::Track> newTrack = addIDMSerrors(combinedTrack.get());
        // recollect eloss for combined track and refit
        // newTrack will not be used after this block, either
        // we updated the combined or kept the combined as it was
        if (newTrack) {
            if (msgLevel(MSG::DEBUG)) countAEOTs(newTrack.get(), " combinedTrack after addIDMSerrors ");
            // Don't run the outliers anymore at this stage
            dumpCaloEloss(newTrack.get(), "CB input TSOS after refine IDMS ");
            std::unique_ptr<Trk::Track> refittedTrack{fit(ctx, *newTrack, false, Trk::muon)};
            if (msgLevel(MSG::DEBUG)) countAEOTs(refittedTrack.get(), " CB fit after refit ");
            if (refittedTrack) dumpCaloEloss(refittedTrack.get(), "CB refit after refine IDMS ");
            /// This should only happen if adding the ID/MS errors fails or the property is disabled
            if (refittedTrack && refittedTrack->fitQuality() && checkTrack("combinedFit", refittedTrack.get(), combinedTrack.get())) {
                // Make the combined point to the refitted
                combinedTrack.swap(refittedTrack);
            }
        }

        /// Final check to avoid FPEs later on
        if (!checkTrack("addIDMS failed", combinedTrack.get(), combinedTrack.get())) {
            ATH_MSG_DEBUG("addIDMS errors failed and original track does not pass checkTrack");
            return nullptr;
        }
        // hole recovery, error optimization, attach TrackSummary
        finalTrackBuild(ctx, combinedTrack);

        return combinedTrack;
    }
    std::unique_ptr<Trk::Track> CombinedMuonTrackBuilder::indetExtension(const EventContext& ctx,
                                                                         const Trk::Track& indetTrack,
                                                                         const Trk::MeasurementSet& spectrometerMeasurements,
                                                                         const Trk::TrackParameters* innerParameters,
                                                                         const Trk::TrackParameters* middleParameters,
                                                                         const Trk::TrackParameters* outerParameters) const {
        if (msgLvl(MSG::VERBOSE)) {
            msg(MSG::VERBOSE) << endmsg << "indetExtension fit:: " << std::setiosflags(std::ios::fixed);

            if (innerParameters || middleParameters || outerParameters) {
                msg(MSG::VERBOSE) << " parameters at R,Z ";

                if (innerParameters) {
                    msg(MSG::VERBOSE) << "I:" << std::setw(5) << std::setprecision(0) << innerParameters->position().perp() << ","
                                      << std::setw(5) << std::setprecision(0) << innerParameters->position().z() << "  ";
                }

                if (middleParameters) {
                    msg(MSG::VERBOSE) << "M:" << std::setw(5) << std::setprecision(0) << middleParameters->position().perp() << ","
                                      << std::setw(5) << std::setprecision(0) << middleParameters->position().z() << "  ";
                }

                if (outerParameters) {
                    msg(MSG::VERBOSE) << "O:" << std::setw(6) << std::setprecision(0) << outerParameters->position().perp() << ","
                                      << std::setw(5) << std::setprecision(0) << outerParameters->position().z();
                }

                msg(MSG::VERBOSE) << " with P ";

                if (innerParameters) {
                    msg(MSG::VERBOSE) << std::setw(9) << std::setprecision(3) << innerParameters->momentum().mag() / Gaudi::Units::GeV;
                }

                if (middleParameters) {
                    msg(MSG::VERBOSE) << std::setw(9) << std::setprecision(3) << middleParameters->momentum().mag() / Gaudi::Units::GeV;
                }

                if (outerParameters) {
                    msg(MSG::VERBOSE) << std::setw(9) << std::setprecision(3) << outerParameters->momentum().mag() / Gaudi::Units::GeV;
                }

                msg(MSG::VERBOSE) << " (GeV)" << endmsg;
            } else {
                msg(MSG::VERBOSE) << " without parameters" << endmsg;
            }
        }

        // propagate appropriate trackParameters to front, back and middle measurements
        // fail when solenoid off and toroid on (as extrapolation from ID is not the correct strategy)
        const Trk::IPropagator* propagator = m_propagatorSL.get();

        MagField::AtlasFieldCache fieldCache;
        // Get field cache object
        if (!loadMagneticField(ctx, fieldCache)) return nullptr;
        if (fieldCache.toroidOn()) {
            // fail when solenoid off and toroid on - as extrapolation from ID is not the correct strategy
            //   for material effects, fit starting value etc
            if (!fieldCache.solenoidOn()) {
                ATH_MSG_VERBOSE("indetExtension: method switched off when solenoid 'off' / toroid 'on'");
                return nullptr;
            }

            /// Need to check with Will whether this propagator is actually
            /// needed or not
            propagator = m_propagator.get();
        }

        std::unique_ptr<Trk::TrackParameters> frontParameters, backParameters;

        if (innerParameters) {
            if (innerParameters->associatedSurface() == spectrometerMeasurements.front()->associatedSurface()) {
                frontParameters = innerParameters->uniqueClone();
            } else {
                // TSoS will own this
                frontParameters = propagator->propagate(ctx, *innerParameters, spectrometerMeasurements.front()->associatedSurface(),
                                                        Trk::anyDirection, false, m_magFieldProperties, Trk::muon);
            }
        } else if (middleParameters) {
            if (middleParameters->associatedSurface() == spectrometerMeasurements.front()->associatedSurface()) {
                frontParameters = middleParameters->uniqueClone();
            } else {
                // TSoS will own this
                frontParameters = propagator->propagate(ctx, *middleParameters, spectrometerMeasurements.front()->associatedSurface(),
                                                        Trk::anyDirection, false, m_magFieldProperties, Trk::muon);
            }
        }

        if (outerParameters) {
            if (outerParameters->associatedSurface() == spectrometerMeasurements.back()->associatedSurface()) {
                backParameters = outerParameters->uniqueClone();
            } else {
                // TSoS will own this
                backParameters = propagator->propagate(ctx, *outerParameters, spectrometerMeasurements.back()->associatedSurface(),
                                                       Trk::anyDirection, false, m_magFieldProperties, Trk::muon);
            }
        } else if (middleParameters) {
            if (middleParameters->associatedSurface() == spectrometerMeasurements.back()->associatedSurface()) {
                backParameters = middleParameters->uniqueClone();
            } else {
                // TSoS will own this
                backParameters = propagator->propagate(ctx, *middleParameters, spectrometerMeasurements.back()->associatedSurface(),
                                                       Trk::anyDirection, false, m_magFieldProperties, Trk::muon);
            }
        }

        // find middle measurement
        std::unique_ptr<Trk::TrackParameters> midParameters;
        const Trk::MeasurementBase* midMeasurement = nullptr;

        if (middleParameters && innerParameters && outerParameters) {
            Amg::Vector3D direction = (outerParameters->position() - innerParameters->position()).unit();
            double midDistance = 0.5 * direction.dot(outerParameters->position() - innerParameters->position());
            double previousDistance = 0.;

            Trk::MeasurementSet::const_iterator m = spectrometerMeasurements.begin();
            for (++m; m != spectrometerMeasurements.end(); ++m) {
                double distance = direction.dot((**m).globalPosition() - innerParameters->position());
                if (distance < midDistance) {
                    previousDistance = distance;
                } else {
                    if (midDistance - previousDistance < distance - midDistance) --m;
                    // TSoS will own this
                    midParameters = m_propagator->propagate(ctx, *middleParameters, (**m).associatedSurface(), Trk::anyDirection, false,
                                                            m_magFieldProperties, Trk::muon);

                    if (midParameters) midMeasurement = *m;
                    break;
                }
            }
        }

        // create muon track from spectrometer measurements
        std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typeM;
        typeM.set(Trk::TrackStateOnSurface::Measurement);
        std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typeP;
        typeP.set(Trk::TrackStateOnSurface::Measurement);
        typeP.set(Trk::TrackStateOnSurface::Parameter);

        Trk::TrackStates trackStateOnSurfaces{};

        trackStateOnSurfaces.reserve(spectrometerMeasurements.size());

        // append the spectrometer measurements
        for (const Trk::MeasurementBase* const in_meas : spectrometerMeasurements) {
            if (frontParameters) {
                trackStateOnSurfaces.push_back(
                    new Trk::TrackStateOnSurface(in_meas->uniqueClone(), std::move(frontParameters), nullptr, nullptr, typeP));
            } else if (in_meas == midMeasurement) {
                trackStateOnSurfaces.push_back(
                    new Trk::TrackStateOnSurface(in_meas->uniqueClone(), std::move(midParameters), nullptr, nullptr, typeP));
            } else if (backParameters && in_meas == spectrometerMeasurements.back()) {
                trackStateOnSurfaces.push_back(
                    new Trk::TrackStateOnSurface(in_meas->uniqueClone(), std::move(backParameters), nullptr, nullptr, typeP));
            } else {
                trackStateOnSurfaces.push_back(
                    new Trk::TrackStateOnSurface(in_meas->uniqueClone(), nullptr, nullptr, nullptr, typeM));
            }
        }

        Trk::TrackInfo trackInfo(Trk::TrackInfo::Unknown, Trk::muon);

        Trk::Track muonTrack(trackInfo, std::move(trackStateOnSurfaces), nullptr);
        if (msgLevel(MSG::DEBUG)) countAEOTs(&muonTrack, " in detExtension muonTrack ");

        // perform combined fit
        ATH_MSG_VERBOSE("Calling combinedFit from " << __func__ << " at line " << __LINE__);
        std::unique_ptr<Trk::Track> combinedTrack{combinedFit(ctx, indetTrack, muonTrack, muonTrack)};

        // check some MS measurements really got added
        unsigned numberMS = 0;
        if (combinedTrack) {
            auto r = combinedTrack->trackStateOnSurfaces()->rbegin();
            auto rEnd = combinedTrack->trackStateOnSurfaces()->rend();

            for (; r != rEnd; ++r) {
                if ((**r).measurementOnTrack()) ++numberMS;
                if ((**r).trackParameters() && m_calorimeterVolume->inside((**r).trackParameters()->position())) break;
            }

            ATH_MSG_VERBOSE("indetExtension:  " << numberMS << " fitted MS measurements ");
            if (numberMS < 5) {
                // reject with insufficient MS measurements
                ATH_MSG_DEBUG("indetExtension:: reject with insufficient MS measurements");
                return nullptr;
            } else if (msgLevel(MSG::DEBUG)) {
                countAEOTs(combinedTrack.get(), " in detExtension final combinedTrack ");
            }
        }

        return combinedTrack;
    }
    std::unique_ptr<Trk::Track> CombinedMuonTrackBuilder::standaloneFit(const EventContext& ctx, const Trk::Track& inputSpectrometerTrack,
                                                                        const Amg::Vector3D& origin, const Trk::Vertex* inputVertex) const {
        MagField::AtlasFieldCache fieldCache;
        // Get field cache object

        if (!loadMagneticField(ctx, fieldCache)) return nullptr;

        // no SA fit with vertex constraint for Toroid off data
        if (m_trackQuery->isLineFit(inputSpectrometerTrack) && !fieldCache.toroidOn()) { return nullptr; }

        ATH_MSG_DEBUG(" standaloneFit beam position bs_x " << origin << " inputVertex "
                                                           << inputVertex);

        if (msgLvl(MSG::VERBOSE)) {
            msg(MSG::VERBOSE) << endmsg << "==== Start of standaloneFit:: " << std::setiosflags(std::ios::fixed);

            if (m_trackQuery->isExtrapolated(inputSpectrometerTrack, ctx)) {
                if (m_trackQuery->isLineFit(inputSpectrometerTrack)) {
                    msg(MSG::VERBOSE) << "extrapolated has lineFit";
                } else {
                    msg(MSG::VERBOSE) << "extrapolated momentum " << std::setprecision(1)
                                      << inputSpectrometerTrack.perigeeParameters()->momentum().mag() / Gaudi::Units::GeV << " (GeV)";
                }

                msg(MSG::VERBOSE) << " at eta " << std::setw(6) << std::setprecision(3)
                                  << inputSpectrometerTrack.perigeeParameters()->momentum().eta() << "  phi " << std::setw(6)
                                  << std::setprecision(3) << inputSpectrometerTrack.perigeeParameters()->momentum().phi();

            } else if (!m_trackQuery->isProjective(inputSpectrometerTrack)) {
                msg(MSG::VERBOSE) << "spectrometer track does not project";
            } else if (inputSpectrometerTrack.perigeeParameters()) {
                if (m_trackQuery->isLineFit(inputSpectrometerTrack)) {
                    msg(MSG::VERBOSE) << "spectrometer has lineFit";
                } else {
                    msg(MSG::VERBOSE) << "spectrometer momentum " << std::setprecision(1)
                                      << inputSpectrometerTrack.perigeeParameters()->momentum().mag() / Gaudi::Units::GeV << " (GeV)";
                }

                msg(MSG::VERBOSE) << " at eta " << std::setw(6) << std::setprecision(3)
                                  << inputSpectrometerTrack.perigeeParameters()->position().eta() << "  phi " << std::setw(6)
                                  << std::setprecision(3) << inputSpectrometerTrack.perigeeParameters()->position().phi();

                if (inputSpectrometerTrack.perigeeParameters()->covariance()) {
                    msg(MSG::VERBOSE) << " hasCov";
                } else {
                    msg(MSG::VERBOSE) << " noCov ";
                }
            } else {
                msg(MSG::VERBOSE) << " spectrometer track without PerigeeParameters";
            }

            if (inputSpectrometerTrack.fitQuality()) {
                msg(MSG::VERBOSE) << "  fit: chi2 /DoF " << std::setprecision(2) << normalizedChi2(inputSpectrometerTrack) << " /"
                                  << std::setw(2) << inputSpectrometerTrack.fitQuality()->numberDoF();
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
        const Trk::FitQuality* fitQuality = inputSpectrometerTrack.fitQuality();
        const Trk::TrackStates* tsos = inputSpectrometerTrack.trackStateOnSurfaces();

        if (!fitQuality || !inputSpectrometerTrack.trackStateOnSurfaces() ||
            static_cast<int>(inputSpectrometerTrack.trackStateOnSurfaces()->size()) < fitQuality->numberDoF()) {
            // count measurements
            int measurements = 0;
            for (const Trk::TrackStateOnSurface* s : *tsos) {
                measurements += (s->type(Trk::TrackStateOnSurface::Measurement) && !s->type(Trk::TrackStateOnSurface::Outlier));
            }
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
        const bool is_extrapolated = m_trackQuery->isExtrapolated(inputSpectrometerTrack, ctx);
        if (!is_extrapolated && !m_trackQuery->isProjective(inputSpectrometerTrack)) {
            ATH_MSG_VERBOSE(" SA::failed (3)");
            return nullptr;
        }

        // possibly refit the spectrometer track with material reallocation
        double spectrometerFitChi2 = normalizedChi2(inputSpectrometerTrack);
        std::unique_ptr<Trk::Track> spectrometerFit = std::make_unique<Trk::Track>(inputSpectrometerTrack);
        if (!vertex && (m_reallocateMaterial || is_extrapolated)) {
            spectrometerFit = reallocateMaterial(ctx, inputSpectrometerTrack);
            if (!spectrometerFit) {
                ATH_MSG_VERBOSE(" SA::failed (4)");
                return nullptr;
            }
        }

        const Trk::Track& spectrometerTrack = *spectrometerFit;

        // require a Perigee from the spectrometer track
        const Trk::Perigee* measuredPerigee = spectrometerTrack.perigeeParameters();

        if (!measuredPerigee || !measuredPerigee->covariance()) {
            // missing MeasuredPerigee for spectrometer track
            m_messageHelper->printWarning(7);

            ATH_MSG_VERBOSE(" SA::failed (5)");
            return nullptr;
        }

        // set measured momentum error and starting parameters
        bool badlyDeterminedCurvature = false;

        if (!Amg::saneCovarianceDiagonal(*measuredPerigee->covariance())) {
            ATH_MSG_WARNING("standaloneFit: measuredPerigee has non-positive-definite covariance ");
            ATH_MSG_VERBOSE(" SA::failed (5.5)");
            /// Delete manually until we switch to unique_ptrs
            return nullptr;
        }

        double errorP = std::sqrt(measuredPerigee->momentum().mag2() * (*measuredPerigee->covariance())(Trk::qOverP, Trk::qOverP));

        std::unique_ptr<Trk::RecVertex> mvertex = std::make_unique<Trk::RecVertex>(*m_vertex);
        std::unique_ptr<Trk::RecVertex> mbeamAxis = std::make_unique<Trk::RecVertex>(*m_beamAxis);
        std::unique_ptr<Trk::PerigeeSurface> mperigeeSurface = std::make_unique<Trk::PerigeeSurface>(*m_perigeeSurface);

        std::unique_ptr<const Trk::TrackParameters> parameters;

        if (vertex) {
            // vertex association only makes sense for magnet-on tracks with measured curvature
            if (!fieldCache.toroidOn() || m_trackQuery->isLineFit(spectrometerTrack) || errorP > m_largeMomentumError) {
                ATH_MSG_VERBOSE("standaloneFit: vertex fit not attempted as curvature badly measured");
                ATH_MSG_VERBOSE(" SA::failed (6)");
                return nullptr;
            }
            parameters = std::make_unique<Trk::Perigee>(*spectrometerTrack.perigeeParameters());
        } else {
            //
            // update -if needed vertex and beam axis positions
            //
            if ((origin - mvertex->position()).mag() > 0.001) {
                // recreate beamAxis and vertexRegion for constrained (projective) track fits

                mperigeeSurface = std::make_unique<Trk::PerigeeSurface>(origin);

                AmgSymMatrix(3) beamAxisCovariance;
                beamAxisCovariance.setZero();
                (beamAxisCovariance)(0, 0) = m_vertex2DSigmaRPhi * m_vertex2DSigmaRPhi;
                (beamAxisCovariance)(1, 1) = m_vertex2DSigmaRPhi * m_vertex2DSigmaRPhi;
                (beamAxisCovariance)(2, 2) = m_vertex2DSigmaZ * m_vertex2DSigmaZ;
                mbeamAxis = std::make_unique<Trk::RecVertex>(origin, beamAxisCovariance);

                AmgSymMatrix(3) vertexRegionCovariance;
                vertexRegionCovariance.setZero();
                (vertexRegionCovariance)(0, 0) = m_vertex3DSigmaRPhi * m_vertex3DSigmaRPhi;
                (vertexRegionCovariance)(1, 1) = m_vertex3DSigmaRPhi * m_vertex3DSigmaRPhi;
                (vertexRegionCovariance)(2, 2) = m_vertex3DSigmaZ * m_vertex3DSigmaZ;
                mvertex = std::make_unique<Trk::RecVertex>(origin, vertexRegionCovariance);
            }

            parameters = extrapolatedParameters(ctx, badlyDeterminedCurvature, spectrometerTrack, mvertex.get(), mperigeeSurface.get());
        }

        if (!parameters) {
            ATH_MSG_VERBOSE(" SA::failed (7)");
            return nullptr;
        }

        // create the spectrometer TSOS's for the extrapolated fit
        std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> spectrometerTSOS = createSpectrometerTSOS(ctx, spectrometerTrack);

        if (spectrometerTSOS.empty()) {
            ATH_MSG_VERBOSE(" SA::failed (8)");
            return nullptr;
        }
        const Trk::TrackParameters* caloParameters = nullptr;

        Trk::ParticleHypothesis particleHypothesis = Trk::muon;

        bool haveFitWithVertex = false;
        bool performPrefit = false;

        if (m_redoRots) {
            for (const Trk::TrackStateOnSurface* s : *spectrometerTrack.trackStateOnSurfaces()) {
                if (s->measurementOnTrack() && !s->trackParameters()) {
                    performPrefit = true;
                    break;
                }
            }
        }

        // badly defined tracks use weak vertex constraint with prefit before calo association
        std::unique_ptr<Trk::Track> prefit;

        const Trk::RecVertex* vertexInFit = vertex;

        if (!vertexInFit) {
            double errorPhi = std::sqrt((*measuredPerigee->covariance())(Trk::phi0, Trk::phi0));

            bool inCSCregion = std::abs(measuredPerigee->momentum().eta()) > 2.0;

            // FIXME: missing prefit case for excessive spectrometer eloss WARNING
            //       spot from line starting approx from vertex??
            if (inCSCregion || m_trackQuery->numberPseudoMeasurements(spectrometerTrack) ||
                (fieldCache.toroidOn() &&
                 (badlyDeterminedCurvature || errorPhi > m_largePhiError || measuredPerigee->momentum().mag() < m_lowMomentum))) {
                performPrefit = true;
                vertexInFit = (badlyDeterminedCurvature || inCSCregion) ? mvertex.get() : mbeamAxis.get();

                if (msgLvl(MSG::DEBUG)) {
                    unsigned numberPseudo = m_trackQuery->numberPseudoMeasurements(spectrometerTrack);
                    if (errorPhi > s_sigmaPhiSector) { ++numberPseudo; }

                    if (badlyDeterminedCurvature) {
                        ATH_MSG_DEBUG(" prefit with vertex: " << std::setiosflags(std::ios::fixed) << " momentum " << std::setprecision(1)
                                                              << measuredPerigee->momentum().mag() / Gaudi::Units::GeV << " (GeV),  zFirst "
                                                              << std::setprecision(1) << std::abs(parameters->position().z())
                                                              << ",  phiError " << std::setprecision(2) << errorPhi << ",  momentumError "
                                                              << std::setprecision(2) << errorP << ",  numberPseudo " << numberPseudo);
                    } else {
                        ATH_MSG_DEBUG(" prefit with beamAxis: "
                                      << std::setiosflags(std::ios::fixed) << " momentum " << std::setprecision(1)
                                      << measuredPerigee->momentum().mag() / Gaudi::Units::GeV << " (GeV),  zFirst " << std::setprecision(1)
                                      << std::abs(parameters->position().z()) << ",  phiError " << std::setprecision(2) << errorPhi
                                      << ",  momentumError " << std::setprecision(2) << errorP << ",  numberPseudo " << numberPseudo);
                    }
                }
            }
        }

        std::unique_ptr<const Trk::Perigee> prefitResult;

        // prefit to stabilize calo look-up and/or provide trackParameters
        if (performPrefit) {
            if (!vertexInFit) { ATH_MSG_VERBOSE(" prefit without vertex"); }

            if (vertexInFit) { haveFitWithVertex = true; }

            if (badlyDeterminedCurvature && parameters->momentum().mag() > m_lowMomentum) { particleHypothesis = Trk::nonInteracting; }

            ATH_MSG_VERBOSE("Calling createExtrapolatedTrack from " << __func__ << " at line " << __LINE__);
            prefit = createExtrapolatedTrack(ctx, spectrometerTrack, *parameters, particleHypothesis, false, spectrometerTSOS, vertexInFit,
                                             mbeamAxis.get(), mperigeeSurface.get());

            // demand prefit success
            if (!prefit || !prefit->fitQuality() || !prefit->perigeeParameters()) {
                ATH_MSG_DEBUG(" prefit failure ");
                prefit.reset();
            }

            if (prefit) {
                dumpCaloEloss(prefit.get(), " prefit ");
                bool hasCov = prefit->perigeeParameters() ? (prefit->perigeeParameters()->covariance() != nullptr) : false;
                ATH_MSG_VERBOSE(" got prefit " << m_printer->print(*prefit) << " hasCov " << hasCov);

                if (prefit->perigeeParameters()) { prefitResult.reset(prefit->perigeeParameters()->clone()); }
                const Trk::TrackStateOnSurface* ms_entrance = nullptr;
                for (const Trk::TrackStateOnSurface* s : *prefit->trackStateOnSurfaces()) {
                    // look for first measured TSOS in muon volume
                    if (!s->trackParameters() || !s->trackParameters()->covariance()) { continue; }
                    if (m_calorimeterVolume->inside(s->trackParameters()->position())) { continue; }

                    // check that it is a measurement
                    if (s->type(Trk::TrackStateOnSurface::Measurement)) {
                        ATH_MSG_DEBUG("Found first parameters in MS " << s->trackParameters()->position().perp() << " z "
                                                                      << s->trackParameters()->position().z());
                        ms_entrance = s;
                        break;
                    }
                }

                if (ms_entrance && ms_entrance != prefit->trackStateOnSurfaces()->front() && ms_entrance->trackParameters()) {
                    parameters.reset(ms_entrance->trackParameters()->clone());
                    caloParameters = parameters.get();
                } else {
                    // prefit: no parameter extrapolation to calo
                    m_messageHelper->printWarning(9);
                }
            }

            // give up if prefit fails
            spectrometerTSOS.clear();

            if (!prefit) {
                ATH_MSG_VERBOSE(" SA::failed (9)");
                return nullptr;
            }
            const Trk::TrackStates* prefit_tsos = prefit->trackStateOnSurfaces();
            // create spectrometerTSOS corresponding to prefit
            //   skip start perigee, then preferentially take everything following MS perigee,
            //   otherwise (if no MS perigee) rely on VolumesSvc,
            //   but be aware that by design there are inconsistencies wrt tracking geometry
            Trk::TrackStates::const_iterator s =
                std::find_if(prefit_tsos->begin() + 1, prefit_tsos->end(), [this](const Trk::TrackStateOnSurface* tsos) -> bool {
                    return (tsos->trackParameters() && !m_calorimeterVolume->inside(tsos->trackParameters()->position())) ||
                           tsos->type(Trk::TrackStateOnSurface::Perigee);
                });

            if (s != prefit_tsos->end() && (*s)->type(Trk::TrackStateOnSurface::Perigee)) ++s;

            for (; s != prefit_tsos->end(); ++s) { spectrometerTSOS.emplace_back((*s)->clone()); }
        }

        // update rot's (but not from trigger chambers) using TrackParameters
        std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> defaultType;
        std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type = defaultType;
        if (m_redoRots) {
            // recalibration: correct rots
            for (std::unique_ptr<const Trk::TrackStateOnSurface>& t : spectrometerTSOS) {
                if (!t->measurementOnTrack() || !t->trackParameters()) { continue; }  // end of if

                const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(t->measurementOnTrack());

                if (!rot) continue;
                Identifier id = rot->identify();

                if (!m_idHelperSvc->isMuon(id)) continue;

                std::unique_ptr<const Trk::RIO_OnTrack> updatedRot;
                if (!m_cscRotCreator.empty() && m_idHelperSvc->isCsc(id)) {
                    updatedRot.reset(m_cscRotCreator->correct(*rot->prepRawData(), *(*t).trackParameters()));
                } else if (!m_mdtRotCreator.empty() && m_idHelperSvc->isMdt(id)) {
                    updatedRot.reset(m_mdtRotCreator->correct(*rot->prepRawData(), *(*t).trackParameters()));
                } else if (!m_mmRotCreator.empty() && m_idHelperSvc->isMM(id)) {
                    updatedRot.reset(m_mmRotCreator->correct(*rot->prepRawData(), *(*t).trackParameters()));
                }

                if (updatedRot) {
                    type = defaultType;
                    type.set(Trk::TrackStateOnSurface::Measurement);
                    if (t->type(Trk::TrackStateOnSurface::Outlier)) { type.set(Trk::TrackStateOnSurface::Outlier); }
                    t = std::make_unique<Trk::TrackStateOnSurface>(std::move(updatedRot), t->trackParameters()->uniqueClone(), nullptr,
                                                                   nullptr, type);
                }
            }
        }

        // extrapolate and fit track
        particleHypothesis = Trk::muon;
        bool returnAfterCleaner = !fieldCache.toroidOn();

        ATH_MSG_VERBOSE("Calling createExtrapolatedTrack from " << __func__ << " at line " << __LINE__);
        std::unique_ptr<Trk::Track> extrapolated(createExtrapolatedTrack(ctx, spectrometerTrack, *parameters, particleHypothesis,
                                                                         m_cleanStandalone, spectrometerTSOS, vertexInFit, mbeamAxis.get(),
                                                                         mperigeeSurface.get(), prefitResult.get()));

        if (extrapolated) dumpCaloEloss(extrapolated.get(), " extrapolated  ");

        // fit problem: try fixup using vertex region or prefit
        if (!extrapolated || !extrapolated->fitQuality()) {
            if (extrapolated && !haveFitWithVertex && !vertexInFit) {
                ATH_MSG_DEBUG(" bad fitQuality: retry with vertex ");
                std::unique_ptr<Trk::Track> badfit(std::move(extrapolated));

                Trk::TrackStates trackStateOnSurfaces{};
                trackStateOnSurfaces.reserve(badfit->trackStateOnSurfaces()->size() + 1);

                type = defaultType;
                type.set(Trk::TrackStateOnSurface::Perigee);

                std::unique_ptr<Trk::PseudoMeasurementOnTrack> vertexInFit =
                    vertexOnTrack(*badfit->perigeeParameters(), mvertex.get(), mbeamAxis.get());

                if (vertexInFit) type.set(Trk::TrackStateOnSurface::Measurement);

                trackStateOnSurfaces.push_back(new Trk::TrackStateOnSurface(
                    std::move(vertexInFit), badfit->perigeeParameters()->uniqueClone(), nullptr, nullptr, type));

                for (Trk::TrackStates::const_iterator s = badfit->trackStateOnSurfaces()->begin() + 1;
                     s != badfit->trackStateOnSurfaces()->end(); ++s) {
                    trackStateOnSurfaces.push_back((**s).clone());
                }

                std::unique_ptr<Trk::Track> track =
                    std::make_unique<Trk::Track>(spectrometerTrack.info(), std::move(trackStateOnSurfaces), nullptr);
                extrapolated = fit(ctx, *track, m_cleanStandalone, particleHypothesis);
            }

            // restart from prefit without cleaning
            if (!extrapolated || !extrapolated->fitQuality()) {
                if (prefit && prefit->fitQuality() && caloParameters) {
                    ATH_MSG_DEBUG(" restarting from prefit as back extrapolation fit failed");
                    spectrometerTSOS.clear();
                    // create spectrometerTSOS corresponding to prefit
                    Trk::TrackStates::const_iterator s = prefit->trackStateOnSurfaces()->begin();

                    while (m_calorimeterVolume->inside((**s).trackParameters()->position()) ||
                           (**s).type(Trk::TrackStateOnSurface::Perigee)) {
                        ++s;
                    }  // end of while

                    for (; s != prefit->trackStateOnSurfaces()->end(); ++s) { spectrometerTSOS.emplace_back((**s).clone()); }

                    ATH_MSG_VERBOSE("Calling createExtrapolatedTrack from " << __func__ << " at line " << __LINE__);
                    extrapolated =
                        createExtrapolatedTrack(ctx, spectrometerTrack, *caloParameters, particleHypothesis, false, spectrometerTSOS,
                                                vertexInFit, mbeamAxis.get(), mperigeeSurface.get(), prefitResult.get());
                    returnAfterCleaner = true;
                }

                if (!extrapolated || !extrapolated->fitQuality()) {
                    bool hasFQ = extrapolated ? (extrapolated->fitQuality() != nullptr) : false;
                    ATH_MSG_DEBUG("fail track as back extrapolation fit failed " << extrapolated.get() << " hasFQ " << hasFQ);

                    ATH_MSG_VERBOSE(" SA::failed (10)");
                    return nullptr;
                }
            }
        }

        // keep statistics for successful fits
        ++m_countAcceptedStandaloneFit;
        if (vertexInFit == mbeamAxis.get()) ++m_countBeamAxis;
        if (vertexInFit == mvertex.get()) ++m_countVertexRegion;

        // refit when there's been a significant momentum change (parameters at last calo scatterer)
        double momentum = parameters->momentum().mag();

        bool allowRefit = !badlyDeterminedCurvature;
        double pRatio = 1.;

        const Trk::TrackParameters* params_pRat = parameters.get();
        if (returnAfterCleaner) {
            allowRefit = false;
        } else {
            // pRatio is the ratio of fitted to start momentum value at calo exit
            //  find parameters at calo exit
            const Trk::TrackParameters* params_pRat = nullptr;
            auto s = extrapolated->trackStateOnSurfaces()->begin();
            while (!(**s).trackParameters() || m_calorimeterVolume->inside((**s).trackParameters()->position())) {
                if ((**s).trackParameters() && !(**s).type(Trk::TrackStateOnSurface::Perigee)) params_pRat = (**s).trackParameters();
                ++s;
            }

            //  extrapolated fit with missing calo parameters - this should never happen!
            if (params_pRat) {
                pRatio = momentum / parameters->momentum().mag();
            } else {
                // extrapolated track missing TrackParameters at calo scatterer
                m_messageHelper->printWarning(10);
                allowRefit = false;
            }
        }

        // in case of a significant momentum change: iterate (re-associate calo and refit)
        std::unique_ptr<Trk::Track> track;

        if (allowRefit && std::abs(pRatio - 1.) > m_largeMomentumChange) {
            if (msgLvl(MSG::VERBOSE)) {
                double sinTheta = params_pRat->momentum().perp() / params_pRat->momentum().mag();

                ATH_MSG_VERBOSE(" iterate as significant momentum change after fit "
                                << pRatio << ", pT before " << momentum * sinTheta / Gaudi::Units::GeV << ", after "
                                << params_pRat->momentum().perp() / Gaudi::Units::GeV << " GeV");
            }

            spectrometerTSOS.clear();
            for (const Trk::TrackStateOnSurface* s : *extrapolated->trackStateOnSurfaces()) {
                if (!s->type(Trk::TrackStateOnSurface::Perigee)) spectrometerTSOS.emplace_back(s->clone());
            }

            ATH_MSG_VERBOSE("Calling createExtrapolatedTrack from " << __func__ << " at line " << __LINE__);

            track = createExtrapolatedTrack(ctx, spectrometerTrack, *parameters, particleHypothesis, m_cleanStandalone, spectrometerTSOS,
                                            vertexInFit, mbeamAxis.get(), mperigeeSurface.get(), extrapolated->perigeeParameters());

            if (track) {
                double extrapChi2 = normalizedChi2(*extrapolated);
                double fitChi2 = normalizedChi2(*track);
                if (fitChi2 < m_badFitChi2 || fitChi2 < extrapChi2 + 0.5) { extrapolated.reset(); }
            }
        }
        if (extrapolated) { track.swap(extrapolated); }

        if (!m_trackQuery->isCaloAssociated(*track, ctx)) {  // still want to perform this check probably though
            // fail as calo incorrectly described
            m_messageHelper->printWarning(12);
            ATH_MSG_VERBOSE(" SA::failed (12)");
            return nullptr;
        }

        int improvementsFailed = 0;  // count the number of times the fit fails after improvements

        if (m_refineELossStandAloneTrackFit) {
            ATH_MSG_VERBOSE("Refining Calorimeter TSOS in StandAlone Fit ...");

            m_materialUpdator->updateCaloTSOS(*track);

            std::unique_ptr<Trk::Track> refinedTrack(fit(ctx, *track, false, Trk::muon));
            if (refinedTrack) {
                ATH_MSG_VERBOSE("successfully refit after refining calo TSOS");

                if (checkTrack("refineFit", refinedTrack.get(), track.get())) {
                    ATH_MSG_VERBOSE("refined track checks out");
                    track.swap(refinedTrack);
                }
            } else {
                ATH_MSG_VERBOSE("refined track fit failed");
                ++improvementsFailed;
            }
        }

        //  adds uncertainties
        //  We will either have nullptr or a new Track.
        //  What we pass stays untouched.
        std::unique_ptr<Trk::Track> newTrack = addIDMSerrors(track.get());
        // newTrack will not be used after this block, either
        // we updated the track  or kept the track as it was
        if (newTrack) {
            if (msgLevel(MSG::DEBUG)) countAEOTs(newTrack.get(), " SA track after addIDMSerrors ");
            dumpCaloEloss(newTrack.get(), "SA input TSOS after refine IDMS ");

            // Don't run the outliers anymore at this stage
            std::unique_ptr<Trk::Track> refittedTrack(fit(ctx, *newTrack, false, Trk::muon));
            if (msgLevel(MSG::DEBUG)) { countAEOTs(refittedTrack.get(), " SA track after refit "); }
            dumpCaloEloss(refittedTrack.get(), " SA refit after refine IDMS ");
            if (refittedTrack && refittedTrack->fitQuality() && checkTrack("standaloneFit", refittedTrack.get(), track.get())) {
                // Here we swap
                track.swap(refittedTrack);
            } else {
                ++improvementsFailed;
            }
        } else {
            ++improvementsFailed;
        }

        // hole recovery, error optimization, attach TrackSummary
        finalTrackBuild(ctx, track);

        if (track) {
            dumpCaloEloss(track.get(), " finalTrackBuild ");

            // report when extrapolated fit quality significantly worse than spectrometer quality
            double fitChi2 = normalizedChi2(*track);
            if (fitChi2 > m_badFitChi2 && fitChi2 > spectrometerFitChi2 + 0.5) {
                ATH_MSG_DEBUG("standaloneFit: fit quality degraded wrt spectrometer alone. "
                              << " Chi2/DoF= " << fitChi2);

                ++m_countDegradedStandaloneFit;
                if (improvementsFailed >= 2) {
                    ATH_MSG_DEBUG("reject track, quality degraded and improvements failed");
                    return nullptr;
                }
            }
        }
        ATH_MSG_VERBOSE(" SA::ok ");        
        return track;
    }
    std::unique_ptr<Trk::Track> CombinedMuonTrackBuilder::standaloneRefit(const EventContext& ctx, const Trk::Track& combinedTrack, 
                                                                          const Amg::Vector3D& origin) const {
        //
        // update -if needed vertex and beam axis positions
        //

        if (msgLevel(MSG::DEBUG)) countAEOTs(&combinedTrack, " in standalone Refit input combinedTrack ");

        MagField::AtlasFieldCache fieldCache;
        // Get field cache object

        if (!loadMagneticField(ctx, fieldCache)) return nullptr;

        if (!fieldCache.toroidOn()) {
            // no standalone refit for Toroid off
            return nullptr;
        }

        ATH_MSG_DEBUG(" StandaloneRefit beam position bs_x " << origin);

        // vertex will change track by track
        AmgSymMatrix(3) vertexRegionCovariance;
        vertexRegionCovariance.setZero();

        double error2d0 = m_vertex3DSigmaRPhi * m_vertex3DSigmaRPhi;
        double error2z0 = m_vertex3DSigmaZ * m_vertex3DSigmaZ;
        const Trk::Perigee* measuredPerigee = combinedTrack.perigeeParameters();

        if (measuredPerigee && measuredPerigee->covariance() && m_useRefitTrackError) {
            error2d0 = (*measuredPerigee->covariance())(Trk::d0, Trk::d0);
            error2z0 = (*measuredPerigee->covariance())(Trk::z0, Trk::z0);
            ATH_MSG_DEBUG(" StandaloneRefit  new vertex d0 error  " << std::sqrt(error2d0) << " new vertex z0 error  "
                                                                    << std::sqrt(error2z0));
        }

        (vertexRegionCovariance)(0, 0) = error2d0;
        (vertexRegionCovariance)(1, 1) = error2d0;
        (vertexRegionCovariance)(2, 2) = error2z0;

        std::unique_ptr<Trk::RecVertex> vertex = std::make_unique<Trk::RecVertex>(origin, vertexRegionCovariance);

        ATH_MSG_DEBUG(" StandaloneRefit  new vertex position x " << vertex->position().x() << " y " << vertex->position().y() << " z "
                                                                 << vertex->position().z());

        bool addPhiPseudo = false;
        // release 21
        unsigned spectrometerPhiQuality = m_trackQuery->spectrometerPhiQuality(combinedTrack, ctx);
        if (spectrometerPhiQuality > 1) { addPhiPseudo = true; }

        ATH_MSG_VERBOSE("standaloneRefit: using vertex region constraint with "
                        << "spectrometerPhiQuality " << spectrometerPhiQuality);

        // create standalone track TSOS vector
        Trk::TrackStates trackStateOnSurfaces{};

        // size will allow for perigee + all TSOS outside indet
        unsigned size = combinedTrack.trackStateOnSurfaces()->size() + 3 + addPhiPseudo;

        trackStateOnSurfaces.reserve(size);

        // position TSOS iterator to be just after the indet
        bool haveCaloDeposit = false;

        Trk::TrackStates::const_iterator s = combinedTrack.trackStateOnSurfaces()->begin();
        const Trk::TrackStates::const_iterator cmb_end_itr = combinedTrack.trackStateOnSurfaces()->end();
        do {
            ++s;
            if (s == cmb_end_itr) {
                // fail track as no TSOS with type CaloDeposit
                m_messageHelper->printWarning(13);
                return nullptr;
            }
            if ((*s)->type(Trk::TrackStateOnSurface::CaloDeposit)) {
                haveCaloDeposit = true;
                --s;
            }
        } while (!haveCaloDeposit);
        /// At this stage we have the first calo scatterer or end up with nothing
        const Trk::TrackStateOnSurface* const cmb_inner_tsos = (*s);
        // inner calo scatterer - keep scattering angles for vertex constraint
        // Amg::Vector3D direction;
        const Trk::ScatteringAngles* innerScattering = nullptr;
        std::unique_ptr<Trk::TrackStateOnSurface> innerTSOS;
        const Trk::MaterialEffectsBase* materialEffects = cmb_inner_tsos->materialEffectsOnTrack();
        const Trk::TrackParameters* middleParameters = nullptr;
        const Trk::ScatteringAngles* outerScattering = nullptr;
        const Trk::TrackParameters* parameters = cmb_inner_tsos->trackParameters();
        std::unique_ptr<const Trk::TrackParameters> param_owner;
        if (materialEffects && parameters && m_calorimeterVolume->inside(parameters->position())) {
            // keep scattering angles when vertex constrained
            // in r21, addVertexRegion is always true

            innerTSOS.reset(cmb_inner_tsos->clone());
            const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(materialEffects);

            if (!meot) {
                // innerScattering dynamic_cast failed
                m_messageHelper->printWarning(16);
                return nullptr;
            }
            innerScattering = meot->scatteringAngles();
            /// The while loop above goes one iterator back after it found the calorimeter
            /// deposit. So we go back to the calo deposit
            if (s != cmb_end_itr && !(*s)->type(Trk::TrackStateOnSurface::CaloDeposit)) { ++s; }

            /// Make another check that the pointer is valid
            if (s != cmb_end_itr) {
                const Trk::TrackStateOnSurface* const cmb_middle_tsos = (*s);
                materialEffects = cmb_middle_tsos->materialEffectsOnTrack();
                parameters = cmb_middle_tsos->trackParameters();
                middleParameters = parameters;
            } else {
                // no TSOS of type CaloDeposit found
                m_messageHelper->printWarning(17);
                materialEffects = nullptr;
                parameters = nullptr;
            }

        } else {
            // no inner material or parameters
            if (!materialEffects) m_messageHelper->printWarning(14);
            if (!parameters) m_messageHelper->printWarning(15);
        }

        if (!innerTSOS) {
            // no inner scattering TSOS found
            m_messageHelper->printWarning(18);
            return nullptr;
        }

        // middle calo scatterer (for energy deposit)
        double energyDeposit{0.};

        std::unique_ptr<Trk::TrackStateOnSurface> middleTSOS;

        if (materialEffects && parameters && m_calorimeterVolume->inside(parameters->position())) {
            const Trk::TrackStateOnSurface* const cmb_middle_tsos = (*s);
            middleTSOS.reset(cmb_middle_tsos->clone());
            const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(materialEffects);

            if (meot && meot->energyLoss()) energyDeposit = meot->energyLoss()->deltaE();
            /// Prepare for the outer TSOS

            ++s;
            if (s != cmb_end_itr) {
                const Trk::TrackStateOnSurface* const cmb_outer_tsos = (*s);
                materialEffects = cmb_outer_tsos->materialEffectsOnTrack();
                parameters = cmb_outer_tsos->trackParameters();
            } else {
                materialEffects = nullptr;
                parameters = nullptr;
            }
        } else {
            // no middle material or parameters
            if (!materialEffects) m_messageHelper->printWarning(19);
            if (!parameters) m_messageHelper->printWarning(20);
        }

        if (!middleTSOS) {
            // no CaloDeposit TSOS found
            m_messageHelper->printWarning(21);
            return nullptr;
        }

        // outer calo scatterer
        std::unique_ptr<Trk::TrackStateOnSurface> outerTSOS;

        double pInner{0.}, pOuter{0.};
        if (materialEffects && parameters && m_calorimeterVolume->inside(parameters->position())) {
            const Trk::TrackStateOnSurface* const cmb_outer_tsos = (*s);
            pOuter = parameters->momentum().mag();

            outerTSOS.reset(cmb_outer_tsos->clone());

            const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(materialEffects);

            if (!meot) {
                // outerScattering dynamic_cast failed
                m_messageHelper->printWarning(24);
                return nullptr;
            }
            outerScattering = meot->scatteringAngles();

            // Go to the next surface
            ++s;

            // get parameters at middleSurface for energy correction,
            // start with parameters from middle surface when vertex in fit
            if (outerScattering && middleTSOS) {
                parameters = middleTSOS->trackParameters();
            } else {
                // otherwise extrapolate outer to middleSurface without scattering correction
                param_owner = m_propagator->propagate(ctx, *parameters, middleTSOS->trackParameters()->associatedSurface(),
                                                      Trk::oppositeMomentum, false, m_magFieldProperties, Trk::nonInteracting);
                parameters = param_owner.get();
            }
            /// The extrapolation from above could actually fail
            if (parameters) {
                /// Now we need to check whether we actually own the parameters
                if (!param_owner) param_owner = parameters->uniqueClone();
                // corrected parameters (include unfitted calo energy deposit),
                // inner momentum  = outer momentum plus energy deposit
                pInner = pOuter + energyDeposit;
                momentumUpdate(param_owner, pInner);
                /// Ensure that the parameters are also updated accordingly
                parameters = param_owner.get();
            }
        } else {
            // no outer material or parameters
            if (!materialEffects) m_messageHelper->printWarning(22);
            if (!parameters) m_messageHelper->printWarning(23);
        }

        // fail track if missing any calo surface or extrapolation failure
        if (!innerTSOS || !middleTSOS || !outerTSOS || !parameters) { return nullptr; }
        /// We do not need the parameters pointer anymore
        parameters = nullptr;

        /// create perigee by back extrapolation from middleSurface via innerSurface
        param_owner = m_propagator->propagate(ctx, *param_owner, innerTSOS->trackParameters()->associatedSurface(), Trk::oppositeMomentum,
                                              false, m_magFieldProperties, Trk::nonInteracting);

        /// From this point we can be sure that the parameters down the chain orignate from some propagation
        if (innerScattering) { momentumUpdate(param_owner, pInner, true, -innerScattering->deltaPhi(), -innerScattering->deltaTheta()); }

        std::unique_ptr<const Trk::TrackParameters> perigee_owner;
        if (param_owner) {
            perigee_owner = m_propagator->propagate(ctx, *param_owner, *m_perigeeSurface, Trk::oppositeMomentum, false,
                                                    m_magFieldProperties, Trk::nonInteracting);
            /// If the perigee parameters are not of Type Trk::Peirgee forget what has been tried
            if (perigee_owner && perigee_owner->surfaceType() != Trk::SurfaceType::Perigee) { perigee_owner.reset(); }
        }

        // in case of problem above: clone combined perigee
        if (!perigee_owner) { perigee_owner = combinedTrack.perigeeParameters()->uniqueClone(); }
        // track back out to the 3 calo surfaces applying small correction for non-linearity
        param_owner = m_propagator->propagate(ctx, *perigee_owner, innerTSOS->trackParameters()->associatedSurface(), Trk::alongMomentum,
                                              false, m_magFieldProperties, Trk::nonInteracting);
        if (!param_owner) {
            // failed propagation to innerTSOS
            m_messageHelper->printWarning(26);
        } else {
            if (innerScattering) { momentumUpdate(param_owner, pInner, true, innerScattering->deltaPhi(), innerScattering->deltaTheta()); }

            param_owner = m_propagator->propagate(ctx, *param_owner, middleParameters->associatedSurface(), Trk::alongMomentum, false,
                                                  m_magFieldProperties, Trk::nonInteracting);

            if (!param_owner) {
                // failed propagation to middleTSOS
                m_messageHelper->printWarning(27);
            } else {
                momentumUpdate(param_owner, pOuter);
                param_owner = m_propagator->propagate(ctx, *param_owner, outerTSOS->trackParameters()->associatedSurface(),
                                                      Trk::alongMomentum, false, m_magFieldProperties, Trk::nonInteracting);
            }
        }

        if (!param_owner) { return nullptr; }

        if (outerScattering) { momentumUpdate(param_owner, pOuter, true, outerScattering->deltaPhi(), outerScattering->deltaTheta()); }

        // small correction term
        const double deltaPhi = xAOD::P4Helpers::deltaPhi(outerTSOS->trackParameters()->momentum().phi(), param_owner->momentum().phi());
        const double deltaTheta = outerTSOS->trackParameters()->momentum().theta() - param_owner->momentum().theta();

        momentumUpdate(perigee_owner, pInner, true, deltaPhi, deltaTheta);

        std::unique_ptr<Trk::RecVertex> mbeamAxis = std::make_unique<Trk::RecVertex>(*m_beamAxis);
        /// Create the vertex element before the perigee_owner looses ownership
        std::unique_ptr<const Trk::PseudoMeasurementOnTrack> vertexInFit{vertexOnTrack(*perigee_owner, vertex.get(), mbeamAxis.get())};

        // create perigee TSOS
        trackStateOnSurfaces.push_back(Muon::MuonTSOSHelper::createPerigeeTSOS(std::move(perigee_owner)));

        // including vertex region pseudoMeas if requested: in r21, this is always requested
        if (vertexInFit) {
            trackStateOnSurfaces.push_back(Muon::MuonTSOSHelper::createMeasTSOS(std::move(vertexInFit), nullptr, Trk::TrackStateOnSurface::Measurement));
        }

        if (m_addElossID) {
            double Eloss{0.}, sigmaEloss{0.}, X0tot{0.}, sigmaDeltaPhitot2{0.}, sigmaDeltaThetatot2{0.};

            std::vector<const Trk::TrackStateOnSurface*> scatter_tsos;
            scatter_tsos.reserve(combinedTrack.trackStateOnSurfaces()->size());

            for (const Trk::TrackStateOnSurface* comb_tsos : *combinedTrack.trackStateOnSurfaces()) {
                if (!comb_tsos->trackParameters()) continue;
                if (!m_indetVolume->inside(comb_tsos->trackParameters()->position())) break;
                if (!comb_tsos->materialEffectsOnTrack()) { continue; }
                const double X0 = comb_tsos->materialEffectsOnTrack()->thicknessInX0();
                X0tot += X0;
                const Trk::MaterialEffectsOnTrack* meot =
                    dynamic_cast<const Trk::MaterialEffectsOnTrack*>(comb_tsos->materialEffectsOnTrack());

                if (!meot) { continue; }
                const Trk::EnergyLoss* energyLoss = meot->energyLoss();
                if (!energyLoss) { continue; }
                Eloss += energyLoss->deltaE();
                sigmaEloss += energyLoss->sigmaDeltaE();

                ATH_MSG_DEBUG("CombinedMuonFit ID Eloss found r " << (comb_tsos->trackParameters())->position().perp() << " z "
                                                                  << (comb_tsos->trackParameters())->position().z() << " value "
                                                                  << energyLoss->deltaE() << " Eloss " << Eloss << " sigma Eloss "
                                                                  << energyLoss->sigmaDeltaE() << " X0 " << X0);

                const Trk::ScatteringAngles* scat = meot->scatteringAngles();
                if (scat) {
                    double sigmaDeltaPhi = scat->sigmaDeltaPhi();
                    double sigmaDeltaTheta = scat->sigmaDeltaTheta();
                    sigmaDeltaPhitot2 += sigmaDeltaPhi * sigmaDeltaPhi;
                    sigmaDeltaThetatot2 += sigmaDeltaTheta * sigmaDeltaTheta;
                    scatter_tsos.push_back(comb_tsos);
                }
            }

            ATH_MSG_DEBUG("standaloneRefit Total ID Eloss " << Eloss << " sigma Eloss " << sigmaEloss << " X0 " << X0tot
                                                            << " sigma scat phi " << std::sqrt(sigmaDeltaPhitot2) << " sigma scat theta "
                                                            << std::sqrt(sigmaDeltaThetatot2));
            if (!scatter_tsos.empty()) {
                const int itsosMiddle = scatter_tsos.size() / 2;
                const Trk::TrackStateOnSurface* mid_scatter = scatter_tsos[itsosMiddle];

                const Trk::EnergyLoss* energyLossNew = new Trk::EnergyLoss(Eloss, sigmaEloss, sigmaEloss, sigmaEloss);

                const Trk::Surface& surfNew = mid_scatter->trackParameters()->associatedSurface();
                Trk::ScatteringAngles scatNew{0., 0., std::sqrt(sigmaDeltaPhitot2), std::sqrt(sigmaDeltaThetatot2)};

                std::bitset<Trk::MaterialEffectsBase::NumberOfMaterialEffectsTypes> meotPattern(0);
                meotPattern.set(Trk::MaterialEffectsBase::EnergyLossEffects);
                meotPattern.set(Trk::MaterialEffectsBase::ScatteringEffects);

                ATH_MSG_DEBUG(" itsosMiddle " << itsosMiddle << " tsosnr size " << scatter_tsos.size());

                std::unique_ptr<Trk::MaterialEffectsOnTrack> meotNew = std::make_unique<Trk::MaterialEffectsOnTrack>(X0tot, std::move(scatNew), energyLossNew, surfNew, meotPattern);

                std::unique_ptr<Trk::TrackParameters> parsNew = mid_scatter->trackParameters()->uniqueClone();
                std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePatternScat(0);
                typePatternScat.set(Trk::TrackStateOnSurface::Scatterer);

                std::unique_ptr<Trk::TrackStateOnSurface> newTSOS =
                    std::make_unique<Trk::TrackStateOnSurface>(nullptr, std::move(parsNew), nullptr, std::move(meotNew), typePatternScat);

                trackStateOnSurfaces.push_back(std::move(newTSOS));
                ATH_MSG_DEBUG(" add new TSOS for ID ");
            }

        }  // end m_addElossID

        // add the 3 surface calo model
        trackStateOnSurfaces.push_back(std::move(innerTSOS));
        trackStateOnSurfaces.push_back(std::move(middleTSOS));
        trackStateOnSurfaces.push_back(std::move(outerTSOS));
        const Trk::TrackParameters* outerTSOSParam = trackStateOnSurfaces.back()->trackParameters();
        // MS entrance perigee
        if (m_perigeeAtSpectrometerEntrance) {
            std::unique_ptr<Trk::TrackStateOnSurface> entranceTSOS = entrancePerigee(ctx, outerTSOSParam);
            if (entranceTSOS) trackStateOnSurfaces.push_back(std::move(entranceTSOS));           
        }

        // leading spectrometer material
        bool haveLeadingMaterial = false;

        Trk::TrackStates::const_iterator mat_it = s;
        for (; mat_it != cmb_end_itr; ++mat_it) {
            if ((*mat_it)->type(Trk::TrackStateOnSurface::Measurement)) break;
            haveLeadingMaterial = true;
        }

        // protection against overruning the end of the vector
        if (mat_it == cmb_end_itr) {
            ATH_MSG_WARNING("At end of TSOS vector");
            return nullptr;
        }

        if (haveLeadingMaterial) appendSelectedTSOS(trackStateOnSurfaces, s, ++mat_it);

        // insert phi pseudo measurement if necessary
        if (addPhiPseudo) {
            std::unique_ptr<Trk::TrackStateOnSurface> tsos = createPhiPseudoMeasurement(ctx, combinedTrack);
            if (tsos) trackStateOnSurfaces.push_back(std::move(tsos));
        }

        // then append the remaining TSOS from the input track
        appendSelectedTSOS(trackStateOnSurfaces, mat_it, cmb_end_itr);

        // create track for refit
        std::unique_ptr<Trk::Track> standaloneTrack =
            std::make_unique<Trk::Track>(combinedTrack.info(), std::move(trackStateOnSurfaces), nullptr);
        standaloneTrack->info().setPatternRecognitionInfo(Trk::TrackInfo::MuidStandaloneRefit);
        if (m_trackQuery->isCombined(*standaloneTrack, ctx)) { ATH_MSG_WARNING(" This should not happen standalone Track has ID hits "); }

        if (msgLevel(MSG::DEBUG)) countAEOTs(standaloneTrack.get(), " in standalone Refit standaloneTrack track before fit ");

        std::unique_ptr<Trk::Track> refittedTrack{fit(ctx, *standaloneTrack, false, Trk::muon)};
        if (!checkTrack("standaloneRefit", refittedTrack.get(), standaloneTrack.get())) { return nullptr; }

        // eventually this whole tool will use unique_ptrs
        // in the meantime, this allows the MuonErrorOptimisationTool and MuonRefitTool to use them
        if (refittedTrack) {
            if (!refittedTrack->fitQuality()) { return nullptr; }

            if (!m_trackQuery->isCaloAssociated(*refittedTrack, ctx)) {
                // fail as calo incorrectly described
                m_messageHelper->printWarning(28);
                return nullptr;
            }

            if (msgLevel(MSG::DEBUG)) countAEOTs(refittedTrack.get(), " standaloneRefit final refittedTrack ");

            // fit with optimized spectrometer errors
            // this should also be inside the "if(refittedTrack) statement
            if (!m_muonErrorOptimizer.empty() && !refittedTrack->info().trackProperties(Trk::TrackInfo::StraightTrack) &&
                countAEOTs(refittedTrack.get(), " before optimize ") == 0) {
                ATH_MSG_VERBOSE(" perform spectrometer error optimization after cleaning ");
                std::unique_ptr<Trk::Track> optimizedTrack = m_muonErrorOptimizer->optimiseErrors(*refittedTrack, ctx);

                if (optimizedTrack && checkTrack("standaloneRefitOpt", optimizedTrack.get(), refittedTrack.get())) {
                    refittedTrack.swap(optimizedTrack);
                    if (msgLevel(MSG::DEBUG)) countAEOTs(refittedTrack.get(), " standaloneRefit alignment errors Track ");
                }
            }
        }

        // have to release it until the whole tool is migrated to unique_ptr
        return refittedTrack;
    }
    std::unique_ptr<Trk::Track> CombinedMuonTrackBuilder::fit(const EventContext& ctx, Trk::Track& track, 
                                                              const Trk::RunOutlierRemoval runOutlier,
                                                              const Trk::ParticleHypothesis particleHypothesis) const {
        ATH_MSG_VERBOSE(" fit() " << m_printer->print(track) << std::endl
                                  << m_printer->printMeasurements(track) << std::endl
                                  << m_printer->printStations(track));
        // check valid particleHypothesis
        if (particleHypothesis != Trk::muon && particleHypothesis != Trk::nonInteracting) {
            // invalid particle hypothesis
            std::stringstream ss;
            ss << particleHypothesis;
            m_messageHelper->printWarning(29, ss.str());
            return nullptr;
        }

        // check if combined or subsystem track
        bool isCombined = m_trackQuery->isCombined(track, ctx);
        // select straightLine fitter when magnets downstream of leading measurement are off
        const Trk::ITrackFitter* fitter = m_fitter.get();
        MagField::AtlasFieldCache fieldCache;
        // Get field cache object

        if (!loadMagneticField(ctx, fieldCache)) return nullptr;

        if (!fieldCache.toroidOn() && !(isCombined && fieldCache.solenoidOn())) {
            fitter = m_fitterSL.get();
            ATH_MSG_VERBOSE(" fit (track refit method): select SL fitter ");
        }

        // redo ROTs:  ID, CROT and MDT specific treatments
        // if (m_redoRots) redoRots(track);

        // perform fit after ensuring calo is associated for combined tracks
        // calo association for combined tracks (WARN if missing from input)
        std::unique_ptr<Trk::Track> fittedTrack;
        if (isCombined && particleHypothesis == Trk::muon && !m_trackQuery->isCaloAssociated(track, ctx)) {
            // about to add the TSOS's describing calorimeter association to a combined muon;
            m_messageHelper->printWarning(30);

            Trk::TrackStates combinedTSOS{};

            combinedTSOS.reserve(track.trackStateOnSurfaces()->size() + 3);
            bool caloAssociated = false;

            // run-2 schema, update default eloss with parametrised value
            if (m_useCaloTG) {
                ATH_MSG_VERBOSE("Updating Calorimeter TSOS in Muon Combined (re)Fit ...");
                m_materialUpdator->updateCaloTSOS(track);
                caloAssociated = true;
            }

            for (const Trk::TrackStateOnSurface* in_tsos : *track.trackStateOnSurfaces()) {
                if (caloAssociated) {
                    combinedTSOS.push_back(in_tsos->clone());
                } else if ((in_tsos->measurementOnTrack() && m_indetVolume->inside(in_tsos->measurementOnTrack()->globalPosition())) ||
                           (in_tsos->trackParameters() && m_indetVolume->inside(in_tsos->trackParameters()->position()))) {
                    combinedTSOS.push_back(in_tsos->clone());
                } else {
                    std::unique_ptr<const Trk::TrackStateOnSurface> tsos = m_caloTSOS->innerTSOS(ctx, *track.perigeeParameters());
                    if (tsos) {
                        combinedTSOS.push_back(std::move(tsos));
                        const Trk::TrackParameters* parameters = combinedTSOS.back()->trackParameters();
                        if (in_tsos->type(Trk::TrackStateOnSurface::CaloDeposit)) {
                            combinedTSOS.push_back(in_tsos->clone());
                            tsos = m_caloTSOS->outerTSOS(ctx, *parameters);
                            if (tsos) combinedTSOS.push_back(std::move(tsos));
                        } else {
                            tsos = m_caloTSOS->middleTSOS(ctx, *parameters);
                            if (tsos) combinedTSOS.push_back(std::move(tsos));
                            tsos = m_caloTSOS->outerTSOS(ctx, *parameters);
                            if (tsos) combinedTSOS.push_back(std::move(tsos));
                            combinedTSOS.push_back(in_tsos->clone());
                        }
                    }
                    caloAssociated = true;
                }
            }

            std::unique_ptr<Trk::Track> combinedTrack = std::make_unique<Trk::Track>(track.info(), std::move(combinedTSOS), nullptr);

            if (msgLevel(MSG::DEBUG)) countAEOTs(combinedTrack.get(), " combinedTrack track before fit ");

            caloAssociated = m_trackQuery->isCaloAssociated(*combinedTrack, ctx);

            // Updates the calo TSOS with the ones from TG+corrections
            if (m_updateWithCaloTG && !m_useCaloTG && particleHypothesis == Trk::muon) {
                ATH_MSG_VERBOSE("Updating Calorimeter TSOS in Muon Combined (re)Fit ...");
                m_materialUpdator->updateCaloTSOS(*combinedTrack);
            }
            // FIT
            fittedTrack = fitter->fit(ctx, *combinedTrack, false, particleHypothesis);
        } else {
            // Updates the calo TSOS with the ones from TG+corrections
            if (m_updateWithCaloTG && !m_useCaloTG && particleHypothesis == Trk::muon) {
                ATH_MSG_VERBOSE("Updating Calorimeter TSOS in Muon Standalone Fit ...");
                m_materialUpdator->updateCaloTSOS(track);
            }

            // FIT
            fittedTrack = fitter->fit(ctx, track, false, particleHypothesis);
        }

        // quit if fit has failed
        if (!fittedTrack) return nullptr;

        if (!checkTrack("fitInterface1", fittedTrack.get(), fittedTrack.get())) { return nullptr; }

        // eventually this whole tool will use unique_ptrs
        // in the meantime, this allows the MuonErrorOptimisationTool and MuonRefitTool to use them
        // track cleaning
        if (runOutlier) {
            // fit with optimized spectrometer errors

            if (!m_muonErrorOptimizer.empty() && !fittedTrack->info().trackProperties(Trk::TrackInfo::StraightTrack) &&
                optimizeErrors(ctx, fittedTrack.get())) {
                ATH_MSG_VERBOSE(" perform spectrometer error optimization after cleaning ");
                std::unique_ptr<Trk::Track> optimizedTrack = m_muonErrorOptimizer->optimiseErrors(*fittedTrack, ctx);

                if (optimizedTrack) {
                    if (checkTrack("fitInterface1Opt", optimizedTrack.get(), fittedTrack.get())) {
                        fittedTrack.swap(optimizedTrack);
                        if (msgLevel(MSG::DEBUG)) countAEOTs(fittedTrack.get(), " re fit scaled errors Track ");
                    }
                }
            }

            // chi2 before clean
            double chi2Before = normalizedChi2(*fittedTrack);

            // muon cleaner
            ATH_MSG_VERBOSE(" perform track cleaning... " << m_printer->print(*fittedTrack) << std::endl
                                                          << m_printer->printStations(*fittedTrack));

            if (msgLevel(MSG::DEBUG)) countAEOTs(fittedTrack.get(), " refit: fitted track before cleaning ");

            std::unique_ptr<Trk::Track> cleanTrack = m_cleaner->clean(*fittedTrack, ctx);

            if (msgLevel(MSG::DEBUG)) countAEOTs(cleanTrack.get(), " refit: after cleaning");

            if (cleanTrack && !checkTrack("fitInterface1Cleaner", cleanTrack.get(), fittedTrack.get())) { cleanTrack.reset(); }

            if (!cleanTrack) {
                if (m_allowCleanerVeto && chi2Before > m_badFitChi2) {
                    ATH_MSG_DEBUG(" cleaner veto A ");
                    ++m_countStandaloneCleanerVeto;
                    fittedTrack.reset();
                } else {
                    ATH_MSG_DEBUG(" keep original standalone track despite cleaner veto ");
                }
            } else if (!(*cleanTrack->perigeeParameters() == *fittedTrack->perigeeParameters())) {
                double chi2After = normalizedChi2(*cleanTrack);

                if (chi2After < m_badFitChi2 || chi2After < chi2Before) {
                    ATH_MSG_VERBOSE(" found and removed spectrometer outlier(s) ");
                    fittedTrack.swap(cleanTrack);
                } else {
                    ATH_MSG_VERBOSE(" keep original track despite cleaning ");
                }
            }

            // FIXME: provide indet cleaner
            if (fittedTrack) {
                ATH_MSG_VERBOSE(" finished track cleaning... " << m_printer->print(*fittedTrack) << std::endl
                                                               << m_printer->printStations(*fittedTrack));
            }
        }

        // have to use release until the whole tool uses unique_ptr
        return fittedTrack;
    }

    std::unique_ptr<Trk::Track> CombinedMuonTrackBuilder::fit(const EventContext& ctx, const Trk::MeasurementSet& measurementSet,
                                                              const Trk::TrackParameters& perigeeStartValue,
                                                              const Trk::RunOutlierRemoval runOutlier,
                                                              const Trk::ParticleHypothesis particleHypothesis) const {
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
        if (!loadMagneticField(ctx, fieldCache)) return nullptr;

        const Trk::ITrackFitter* fitter = m_fitter.get();
        if (!fieldCache.toroidOn() || std::abs(perigeeStartValue.position().z()) > m_zECToroid) {
            fitter = m_fitterSL.get();
            ATH_MSG_VERBOSE(" fit (track refit method): select SL fitter ");
        }

        // redo ROTs:  ID, CROT and MDT specific treatments
        // if (m_redoRots) redoRots(track);

        // calo association (if relevant)

        // create Perigee if starting parameters given for a different surface type
        std::unique_ptr<Trk::TrackParameters> perigee = perigeeStartValue.uniqueClone();
        std::unique_ptr<Trk::PerigeeSurface> perigeeSurface;

        if (perigee->surfaceType() != Trk::SurfaceType::Perigee) {
            Amg::Vector3D origin(perigeeStartValue.position());
            perigeeSurface = std::make_unique<Trk::PerigeeSurface>(origin);

            perigee = std::make_unique<Trk::Perigee>(perigeeStartValue.position(), perigeeStartValue.momentum(), perigeeStartValue.charge(),
                                                     *perigeeSurface);
        }

        // FIT
        std::unique_ptr<Trk::Track> fittedTrack(fitter->fit(ctx, measurementSet, *perigee, false, particleHypothesis));

        if (!fittedTrack) return nullptr;

        if (!checkTrack("fitInterface2", fittedTrack.get(), fittedTrack.get())) { return nullptr; }

        // eventually this whole tool will use unique_ptrs
        // in the meantime, this allows the MuonErrorOptimisationTool and MuonRefitTool to use them

        // track cleaning
        if (runOutlier) {
            // fit with optimized spectrometer errors

            if (!m_muonErrorOptimizer.empty() && !fittedTrack->info().trackProperties(Trk::TrackInfo::StraightTrack) &&
                optimizeErrors(ctx, fittedTrack.get())) {
                ATH_MSG_VERBOSE(" perform spectrometer error optimization after cleaning ");
                std::unique_ptr<Trk::Track> optimizedTrack = m_muonErrorOptimizer->optimiseErrors(*fittedTrack, ctx);
                if (optimizedTrack) {
                    if (checkTrack("fitInterface2Opt", optimizedTrack.get(), fittedTrack.get())) {
                        fittedTrack.swap(optimizedTrack);
                        if (msgLevel(MSG::DEBUG)) countAEOTs(fittedTrack.get(), " fit mstSet scaled errors Track ");
                    }
                }
            }

            // chi2 before clean
            double chi2Before = normalizedChi2(*fittedTrack);

            // muon cleaner
            ATH_MSG_VERBOSE(" perform track cleaning... ");

            if (msgLevel(MSG::DEBUG)) countAEOTs(fittedTrack.get(), " fit mstSet before cleaning ");

            std::unique_ptr<Trk::Track> cleanTrack = m_cleaner->clean(*fittedTrack, ctx);

            if (msgLevel(MSG::DEBUG)) countAEOTs(cleanTrack.get(), " fit mstSet clean Track ");

            if (cleanTrack && !checkTrack("fitInterface2Cleaner", cleanTrack.get(), fittedTrack.get())) { cleanTrack.reset(); }

            if (!cleanTrack) {
                if (m_allowCleanerVeto && chi2Before > m_badFitChi2) {
                    ATH_MSG_DEBUG(" cleaner veto B");
                    ++m_countExtensionCleanerVeto;
                    fittedTrack.reset();
                } else {
                    ATH_MSG_DEBUG(" keep original extension track despite cleaner veto ");
                }
            } else if (!(*cleanTrack->perigeeParameters() == *fittedTrack->perigeeParameters())) {
                double chi2After = normalizedChi2(*cleanTrack);
                if (chi2After < m_badFitChi2 || chi2After < chi2Before) {
                    ATH_MSG_VERBOSE(" found and removed spectrometer outlier(s) ");
                    fittedTrack.swap(cleanTrack);
                } else {
                    ATH_MSG_VERBOSE(" keep original track despite cleaning ");
                }
            }

            // FIXME: provide indet cleaner
            ATH_MSG_VERBOSE(" Finished cleaning");
        }
        // have to use release until the whole code uses unique_ptr
        return fittedTrack;
    }

    /**combined muon fit */
    std::unique_ptr<Trk::Track> CombinedMuonTrackBuilder::fit(const EventContext& ctx, const Trk::Track& indetTrack,
                                                              Trk::Track& extrapolatedTrack, const Trk::RunOutlierRemoval runOutlier,
                                                              const Trk::ParticleHypothesis particleHypothesis) const {
        // check valid particleHypothesis
        if (particleHypothesis != Trk::muon && particleHypothesis != Trk::nonInteracting) {
            // invalid particle hypothesis
            std::stringstream ss;
            ss << particleHypothesis;
            m_messageHelper->printWarning(32, ss.str());
            return nullptr;
        }

        // select straightLine fitter when solenoid and toroid are off
        const Trk::ITrackFitter* fitter = m_fitter.get();
        MagField::AtlasFieldCache fieldCache;
        // Get field cache object
        if (!loadMagneticField(ctx, fieldCache)) return nullptr;

        if (!fieldCache.toroidOn() && !fieldCache.solenoidOn()) {
            fitter = m_fitterSL.get();
            ATH_MSG_VERBOSE(" fit (combined muon fit method): select SL fitter ");
        }

        // redo ROTs:  ID, CROT and MDT specific treatments

        // calo association (for now just WARN if missing)
        if (particleHypothesis == Trk::muon && !m_trackQuery->isCaloAssociated(extrapolatedTrack, ctx)) {
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
        
        std::unique_ptr<Trk::Track> fittedTrack(fitter->fit(ctx, indetTrack, extrapolatedTrack, false, particleHypothesis));

        if (!fittedTrack) return nullptr;

        // eventually this whole tool will use unique_ptrs
        // in the meantime, this allows the MuonErrorOptimisationTool and MuonRefitTool to use them

        // track cleaning
        if (runOutlier) {
            // fit with optimized spectrometer errors

            if (!m_muonErrorOptimizer.empty() && !fittedTrack->info().trackProperties(Trk::TrackInfo::StraightTrack) &&
                optimizeErrors(ctx, fittedTrack.get())) {
                ATH_MSG_VERBOSE(" perform spectrometer error optimization after cleaning ");
                std::unique_ptr<Trk::Track> optimizedTrack = m_muonErrorOptimizer->optimiseErrors(*fittedTrack, ctx);
                if (optimizedTrack) {
                    fittedTrack.swap(optimizedTrack);
                    if (msgLevel(MSG::DEBUG)) countAEOTs(fittedTrack.get(), " cbfit scaled errors Track ");
                }
            }

            // chi2 before clean
            double chi2Before = normalizedChi2(*fittedTrack.get());

            // muon cleaner
            ATH_MSG_VERBOSE(" perform track cleaning... " << m_printer->print(*fittedTrack) << std::endl
                                                          << m_printer->printStations(*fittedTrack));

            if (msgLevel(MSG::DEBUG)) { countAEOTs(fittedTrack.get(), " cb before clean Track "); }
            std::unique_ptr<Trk::Track> cleanTrack = m_cleaner->clean(*fittedTrack, ctx);
            if (msgLevel(MSG::DEBUG)) { countAEOTs(cleanTrack.get(), " cb after clean Track "); }

            if (!cleanTrack) {
                if (m_allowCleanerVeto && chi2Before > m_badFitChi2) {
                    ATH_MSG_DEBUG(" cleaner veto C");
                    ++m_countCombinedCleanerVeto;
                    fittedTrack.reset();
                } else {
                    ATH_MSG_DEBUG(" keep original combined track despite cleaner veto ");
                }
            } else if (!(*cleanTrack->perigeeParameters() == *fittedTrack->perigeeParameters())) {
                double chi2After = normalizedChi2(*cleanTrack);
                if (chi2After < m_badFitChi2 || chi2After < chi2Before) {
                    ATH_MSG_VERBOSE(" found and removed spectrometer outlier(s) ");
                    fittedTrack.swap(cleanTrack);
                } else {
                    ATH_MSG_VERBOSE(" keep original track despite cleaning ");
                }
            }

            // FIXME: provide indet cleaner
            ATH_MSG_VERBOSE(" finished cleaning");
        }
        // have to use release until the whole code uses unique_ptr
        return fittedTrack;
    }

    /*   private methods follow */

    bool CombinedMuonTrackBuilder::optimizeErrors(const EventContext& ctx, Trk::Track* track) const {
        const Trk::MuonTrackSummary* muonSummary = nullptr;
        const Trk::TrackSummary* summary = track->trackSummary();

        if (summary) {
            muonSummary = summary->muonTrackSummary();
        } else {
            m_trackSummary->updateTrack(ctx, *track);
            summary = track->trackSummary();
            muonSummary = summary->muonTrackSummary();
        }

        if (!muonSummary) return false;

        typedef Trk::MuonTrackSummary::ChamberHitSummary ChamberHitSummary;
        const std::vector<ChamberHitSummary>& chamberHitSummary = muonSummary->chamberHitSummary();

        std::vector<ChamberHitSummary>::const_iterator chit = chamberHitSummary.begin();
        std::vector<ChamberHitSummary>::const_iterator chit_end = chamberHitSummary.end();

        int optimize = 0;
        int nBarrel = 0;
        int nEndcap = 0;
        int nSmall = 0;
        int nLarge = 0;

        for (; chit != chit_end; ++chit) {
            const Identifier& id = chit->chamberId();
            bool isMdt = m_idHelperSvc->isMdt(id);
            if (!isMdt) continue;

            Muon::MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex(id);

            if (stIndex == Muon::MuonStationIndex::BE) { optimize = 1; }

            if (stIndex == Muon::MuonStationIndex::BI && m_idHelperSvc->chamberIndex(id) == Muon::MuonStationIndex::BIS &&
                abs(m_idHelperSvc->stationEta(id)) > 6) {
                optimize = 2;
            }

            if (stIndex == Muon::MuonStationIndex::BI || stIndex == Muon::MuonStationIndex::BM || stIndex == Muon::MuonStationIndex::BO ||
                stIndex == Muon::MuonStationIndex::BE) {
                nBarrel++;
            }

            if (stIndex == Muon::MuonStationIndex::EI || stIndex == Muon::MuonStationIndex::EM || stIndex == Muon::MuonStationIndex::EO ||
                stIndex == Muon::MuonStationIndex::EE) {
                nEndcap++;
            }

            if (m_idHelperSvc->isSmallChamber(id)) {
                nSmall++;
            } else {
                nLarge++;
            }
        }

        if (nBarrel > 0 && nEndcap > 0) { optimize += 10; }

        if (nSmall > 0 && nLarge > 0) { optimize += 100; }

        if (optimize > 0) { ATH_MSG_DEBUG(" OptimizeErrors with value " << optimize); }

        return optimize > 0;
    }

    std::unique_ptr<Trk::Track> CombinedMuonTrackBuilder::addIDMSerrors(const Trk::Track* track) const {
        //
        // take track and correct the two scattering planes in the Calorimeter
        // to take into account m_IDMS_rzSigma and m_IDMS_xySigma
        //
        // returns a new Track or nullptr does not modify the input in any way
        //
        if (!m_addIDMSerrors) { return nullptr; }

        ATH_MSG_DEBUG(" CombinedMuonTrackBuilder addIDMSerrors to track ");

        /// Use pointer in the data vector to refer on the track
        const Trk::TrackStateOnSurface* id_exit = nullptr;
        const Trk::TrackStateOnSurface* calo_entrance = nullptr;
        const Trk::TrackStateOnSurface* calo_exit = nullptr;
        const Trk::TrackStateOnSurface* ms_entrance = nullptr;

        m_alignUncertTool_theta->get_track_state_measures(track, id_exit, calo_entrance, calo_exit, ms_entrance);
        /// it can happen that no Calorimeter Scatterers are found.
        if (!calo_entrance || !calo_exit || !ms_entrance) {
            ATH_MSG_DEBUG(" addIDMSerrors keep original track ");
            return nullptr;
        }

        Trk::TrackStates trackStateOnSurfaces{};
        trackStateOnSurfaces.reserve(track->trackStateOnSurfaces()->size());

        for (const Trk::TrackStateOnSurface* trk_srf : *track->trackStateOnSurfaces()) {
            if (calo_entrance == trk_srf || calo_entrance == trk_srf) {
                if (!trk_srf->materialEffectsOnTrack()) {
                    ATH_MSG_DEBUG("No material effect on track");
                    continue;
                }
                const Trk::MaterialEffectsOnTrack* meot =
                    dynamic_cast<const Trk::MaterialEffectsOnTrack*>(trk_srf->materialEffectsOnTrack());
                if (!meot) {
                    ATH_MSG_WARNING(" This should not happen: no MaterialEffectsOnTrack for scatterer ");
                    continue;
                }
                const Trk::ScatteringAngles* scat = meot->scatteringAngles();
                if (!scat) {
                    ATH_MSG_WARNING(" This should not happen: no Scattering Angles for scatterer ");
                    continue;
                }

                float sigmaDeltaPhi = std::hypot(scat->sigmaDeltaPhi(), m_alignUncertTool_phi->get_uncertainty(track));
                float sigmaDeltaTheta = std::hypot(scat->sigmaDeltaTheta(), m_alignUncertTool_theta->get_uncertainty(track));
                float X0 = trk_srf->materialEffectsOnTrack()->thicknessInX0();
                //
                const Trk::EnergyLoss* energyLossNew = new Trk::EnergyLoss(0., 0., 0., 0.);
                auto scatNew = Trk::ScatteringAngles(0., 0., sigmaDeltaPhi, sigmaDeltaTheta);

                const Trk::Surface& surfNew = trk_srf->trackParameters()->associatedSurface();

                std::bitset<Trk::MaterialEffectsBase::NumberOfMaterialEffectsTypes> meotPattern(0);
                meotPattern.set(Trk::MaterialEffectsBase::EnergyLossEffects);
                meotPattern.set(Trk::MaterialEffectsBase::ScatteringEffects);

                auto meotNew = std::make_unique<Trk::MaterialEffectsOnTrack>(X0, std::move(scatNew), energyLossNew, surfNew, meotPattern);
                auto parsNew = trk_srf->trackParameters()->uniqueClone();

                std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePatternScat(0);
                typePatternScat.set(Trk::TrackStateOnSurface::Scatterer);

                const Trk::TrackStateOnSurface* newTSOS =
                    new Trk::TrackStateOnSurface(nullptr, std::move(parsNew), nullptr, std::move(meotNew), typePatternScat);
                trackStateOnSurfaces.push_back(newTSOS);

                ATH_MSG_DEBUG(" old Calo scatterer had sigmaDeltaPhi mrad      " << scat->sigmaDeltaPhi() * 1000 << " sigmaDeltaTheta mrad "
                                                                                 << scat->sigmaDeltaTheta() * 1000 << " X0 " << X0);

                ATH_MSG_DEBUG(" new Calo scatterer made with sigmaDeltaPhi mrad " << sigmaDeltaPhi * 1000 << " sigmaDeltaTheta mrad "
                                                                                  << sigmaDeltaTheta * 1000);

            } else {
                // skip AEOTs
                if (trk_srf->alignmentEffectsOnTrack()) {
                    ATH_MSG_DEBUG(" addIDMSerrors alignmentEffectsOnTrack()  found on track ");
                    continue;
                }
                trackStateOnSurfaces.push_back(trk_srf->clone());
            }
        }
        ATH_MSG_DEBUG(" trackStateOnSurfaces on input track " << track->trackStateOnSurfaces()->size() << " trackStateOnSurfaces found "
                                                              << trackStateOnSurfaces.size());

        std::unique_ptr<Trk::Track> newTrack = std::make_unique<Trk::Track>(track->info(), std::move(trackStateOnSurfaces), nullptr);
        return newTrack;
    }

    void CombinedMuonTrackBuilder::appendSelectedTSOS(Trk::TrackStates& trackStateOnSurfaces, Trk::TrackStates::const_iterator begin,
                                                      Trk::TrackStates::const_iterator end) const {
        // spectrometer measurement selection
        std::vector<const Trk::Surface*> measurementSurfaces;
        measurementSurfaces.reserve(trackStateOnSurfaces.size());
        const Trk::Surface* previousSurface = nullptr;

        Trk::TrackStates::const_iterator s = begin;
        for (; s != end; ++s) {
            const Trk::TrackStateOnSurface& tsos = **s;
            if (tsos.alignmentEffectsOnTrack()) {
                ATH_MSG_VERBOSE("appendSelectedTSOS:: alignmentEffectsOnTrack ");
            }
            // skip non-understood features in iPatFitter
            if (!tsos.measurementOnTrack() && !tsos.materialEffectsOnTrack()) {
                if (tsos.type(Trk::TrackStateOnSurface::Perigee)) {
                    ATH_MSG_VERBOSE("appendSelectedTSOS:: skip a perigee without material and measuremet "<<tsos);
                    continue;
                } else if (!tsos.type(Trk::TrackStateOnSurface::Hole) || !tsos.trackParameters()) {
                    ATH_MSG_VERBOSE("appendSelectedTSOS:: skip unrecognized TSOS " << tsos.dumpType());
                    continue;
                }
            }

            if (tsos.measurementOnTrack()) {
                // skip any pseudo measurements
                if (dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(tsos.measurementOnTrack())) { continue; }

                // skip duplicate measurements on same surface
                const Trk::Surface* surface = &tsos.measurementOnTrack()->associatedSurface();
                if (previousSurface &&
                    std::find(measurementSurfaces.begin(), measurementSurfaces.end(), surface) != measurementSurfaces.end() &&
                    !m_idHelperSvc->isMM(surface->associatedDetectorElementIdentifier())) {
                    std::string type = "";
                    if (dynamic_cast<const Trk::CompetingRIOsOnTrack*>(tsos.measurementOnTrack())) {
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
            trackStateOnSurfaces.push_back(tsos.clone());
        }
    }

    const CaloEnergy* CombinedMuonTrackBuilder::caloEnergyParameters(const Trk::Track* combinedTrack, const Trk::Track* muonTrack,
                                                                     const Trk::TrackParameters*& combinedEnergyParameters,
                                                                     const Trk::TrackParameters*& muonEnergyParameters) const {
        // will also set the caloEnergyParameters (from both combinedTrack and muonTrack)
        combinedEnergyParameters = nullptr;
        muonEnergyParameters = nullptr;

        // quit if missing track
        if (!combinedTrack || !muonTrack) return nullptr;

        // muonTrack: get parameters at CaloDeposit
        Trk::TrackStates::const_iterator s = muonTrack->trackStateOnSurfaces()->begin();

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

    std::unique_ptr<Trk::Track> CombinedMuonTrackBuilder::createExtrapolatedTrack(
        const EventContext& ctx, const Trk::Track& spectrometerTrack, const Trk::TrackParameters& parameters,
        Trk::ParticleHypothesis particleHypothesis, Trk::RunOutlierRemoval runOutlier,
        const std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>>& spectrometerTSOS, const Trk::RecVertex* vertex,
        const Trk::RecVertex* mbeamAxis, const Trk::PerigeeSurface* mperigeeSurface, const Trk::Perigee* seedParameters) const {
        ATH_MSG_DEBUG(" createExtrapolatedTrack() - " << __LINE__ << ": pt " << parameters.momentum().perp() << " r "
                                                      << parameters.position().perp() << " z " << parameters.position().z() << " cov "
                                                      << parameters.covariance() << " muonfit " << (particleHypothesis == Trk::muon));

        std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> caloTSOS, leadingTSOS;

        std::unique_ptr<const Trk::TrackParameters> track_param_owner;
        const Trk::TrackParameters* trackParameters{nullptr};
        const Trk::Perigee* perigee{nullptr};

        if (vertex && m_indetVolume->inside(parameters.position())) { perigee = dynamic_cast<const Trk::Perigee*>(&parameters); }
        if (perigee) {
            ATH_MSG_DEBUG("createExtrapolatedTrack(): Got a perigee ");
            trackParameters = perigee;
        } else {
            ATH_MSG_DEBUG("createExtrapolatedTrack(): no perigee");
            // extrapolate backwards to associate leading material in spectrometer
            // (provided material has already been allocated between measurements)
            const Trk::TrackParameters* leadingParameters = &parameters;
            if (particleHypothesis == Trk::muon) {
                bool haveMaterial{false}, haveLeadingMaterial{false}, firstMSHit{false};

                for (const std::unique_ptr<const Trk::TrackStateOnSurface>& s : spectrometerTSOS) {
                    if (s->materialEffectsOnTrack()) {
                        haveMaterial = true;
                        if (!firstMSHit) haveLeadingMaterial = true;
                    }

                    if (s->measurementOnTrack() && !firstMSHit) { firstMSHit = true; }

                    if (haveMaterial && firstMSHit) { break; }
                }

                // only add leading material if there is no material in fron of first muon measurement

                if (!m_materialAllocator.empty() && haveMaterial && !haveLeadingMaterial) {
                    // protect the momentum to avoid excessive Eloss
                    Amg::VectorX parameterVector = parameters.parameters();

                    constexpr double Emax = 50000.;

                    if (parameterVector[Trk::qOverP] == 0.) {
                        parameterVector[Trk::qOverP] = 1. / Emax;
                    } else {
                        if (std::abs(parameterVector[Trk::qOverP]) * Emax < 1) {
                            parameterVector[Trk::qOverP] = parameters.charge() / Emax;
                        }
                    }
                    std::unique_ptr<Trk::TrackParameters> correctedParameters{parameters.associatedSurface().createUniqueTrackParameters(
                        parameterVector[Trk::loc1], parameterVector[Trk::loc2], parameterVector[Trk::phi], parameterVector[Trk::theta],
                        parameterVector[Trk::qOverP], std::nullopt)};

                    Trk::IMaterialAllocator::Garbage_t garbage;
                    std::unique_ptr<std::vector<const Trk::TrackStateOnSurface*>> lead_tsos_from_alloc{
                        m_materialAllocator->leadingSpectrometerTSOS(*correctedParameters, garbage)};
                    if (lead_tsos_from_alloc) {
                        for (const Trk::TrackStateOnSurface* l_tsos : *lead_tsos_from_alloc) leadingTSOS.emplace_back(l_tsos);
                    }
                    if (!leadingTSOS.empty() && leadingTSOS.front()->trackParameters()) {
                        leadingParameters = leadingTSOS.front()->trackParameters();
                    }
                }
            }

            // extrapolate backwards to associate calorimeter material effects
            bool caloAssociated = false;

            if (particleHypothesis == Trk::muon) {
                ATH_MSG_VERBOSE(" Retrieving Calorimeter TSOS from " << __func__ << " at line " << __LINE__);
                if (m_useCaloTG) {
                    caloTSOS = getCaloTSOSfromMatProvider(*leadingParameters, spectrometerTrack);
                    // Dump CaloTSOS
                    //
                    if (msgLevel(MSG::DEBUG)) {
                        for (std::unique_ptr<const Trk::TrackStateOnSurface>& m : caloTSOS) {
                            if (!m->materialEffectsOnTrack()) continue;
                            const Trk::MaterialEffectsOnTrack* meot =
                                dynamic_cast<const Trk::MaterialEffectsOnTrack*>(m->materialEffectsOnTrack());
                            double pcalo{0.}, deltaP{0.};
                            if (!meot) continue;
                            if (meot->thicknessInX0() <= 20) { continue; }
                            const Trk::ScatteringAngles* scatAngles = meot->scatteringAngles();

                            ATH_MSG_DEBUG(" Calorimeter X0  " << meot->thicknessInX0() << "  pointer scat " << scatAngles);

                            if (!scatAngles) { continue; }
                            pcalo = m->trackParameters()->momentum().mag();

                            const double pullPhi = scatAngles->deltaPhi() / scatAngles->sigmaDeltaPhi();
                            const double pullTheta = scatAngles->deltaTheta() / scatAngles->sigmaDeltaTheta();

                            ATH_MSG_DEBUG(" Calorimeter scatterer deltaPhi " << scatAngles->deltaPhi() << " pull " << pullPhi
                                                                             << " deltaTheta " << scatAngles->deltaTheta() << " pull "
                                                                             << pullTheta);

                            const Trk::EnergyLoss* energyLoss = meot->energyLoss();
                            if (!energyLoss) continue;

                            if (m->trackParameters()) {
                                ATH_MSG_DEBUG("Eloss found r " << (m->trackParameters())->position().perp() << " z "
                                                               << (m->trackParameters())->position().z() << " deltaE "
                                                               << energyLoss->deltaE());
                            }

                            if (m->type(Trk::TrackStateOnSurface::CaloDeposit)) {
                                double caloEloss = std::abs(energyLoss->deltaE());
                                if (m->trackParameters()) { deltaP = m->trackParameters()->momentum().mag() - pcalo; }

                                ATH_MSG_DEBUG(" Calorimeter Deposit " << caloEloss << " pcalo Entrance " << pcalo << " deltaP " << deltaP);
                            }
                        }
                    }
                } else {
                    caloTSOS = m_caloTSOS->caloTSOS(ctx, *leadingParameters);
                }

                if (caloTSOS.size() > 2) {
                    caloAssociated = true;
                } else {
                    ATH_MSG_VERBOSE("Failed to associated calorimeter");
                }
            } else {
                // TDDO Run2 Calo TG
                std::unique_ptr<const Trk::TrackStateOnSurface> tsos = m_caloTSOS->innerTSOS(ctx, parameters);
                if (tsos) {
                    caloTSOS.push_back(std::move(tsos));
                    tsos = m_caloTSOS->outerTSOS(ctx, *caloTSOS.back()->trackParameters());
                    if (tsos) {
                        caloAssociated = true;
                        caloTSOS.push_back(std::move(tsos));
                    }
                }
                ATH_MSG_VERBOSE("Special non-muon case for calo: " << caloAssociated);
            }

            // if association OK, create perigee surface and back-track to it
            if (caloAssociated) {
                MagField::AtlasFieldCache fieldCache;
                // Get field cache object
                if (!loadMagneticField(ctx, fieldCache)) return nullptr;

                if (fieldCache.toroidOn()) {
                    const Trk::TrackParameters* oldParameters = caloTSOS.front()->trackParameters();

                    if (oldParameters && !oldParameters->covariance()) { ATH_MSG_VERBOSE(" createExtrapolatedTrack: no cov (0)"); }
                    // chickened out of sorting out ownership
                    track_param_owner = m_propagator->propagate(ctx, *oldParameters, *mperigeeSurface, Trk::oppositeMomentum, false,
                                                                m_magFieldProperties, Trk::nonInteracting);
                } else {
                    track_param_owner = m_propagatorSL->propagate(ctx, parameters, *mperigeeSurface, Trk::oppositeMomentum, false,
                                                                  m_magFieldProperties, Trk::nonInteracting);
                }
                trackParameters = track_param_owner.get();

                // only accept when perigee in indet tracking volume
                if (trackParameters && !m_indetVolume->inside(trackParameters->position())) {
                    ATH_MSG_DEBUG("  back extrapolation problem: probably outside indet volume ");
                    caloAssociated = false;
                }

                if (trackParameters && !trackParameters->covariance()) { ATH_MSG_VERBOSE(" createExtrapolatedTrack: no cov (1)"); }

                if (trackParameters) {
                    ATH_MSG_VERBOSE(" Seed parameter: r " << trackParameters->position().perp() << " z " << trackParameters->position().z()
                                                          << " pt " << trackParameters->momentum().perp());
                }

            }  // if (caloAssociated) {
            // start from vertex in case of calo association problem
            else if (vertex) {
                ATH_MSG_DEBUG("  back extrapolation problem: retry with tracking out from vertex ");
                // delete any existing calo objects
                caloTSOS.clear();

                // track out from vertex
                const Amg::Vector3D momentum = parameters.position().unit() * Gaudi::Units::TeV;

                track_param_owner = std::make_unique<Trk::Perigee>(vertex->position(), momentum, 1., *mperigeeSurface);
                trackParameters = track_param_owner.get();
                particleHypothesis = Trk::nonInteracting;
                runOutlier = false;

                ATH_MSG_VERBOSE(" Retriving Calorimeter TSOS from " << __func__ << " at line " << __LINE__);

                if (m_useCaloTG) {
                    caloTSOS = getCaloTSOSfromMatProvider(*trackParameters, spectrometerTrack);
                } else {
                    std::unique_ptr<const Trk::TrackStateOnSurface> tsos = m_caloTSOS->innerTSOS(ctx, *trackParameters);
                    if (tsos) {
                        caloTSOS.push_back(std::move(tsos));
                        tsos = m_caloTSOS->outerTSOS(ctx, *trackParameters);
                        if (tsos) {
                            caloTSOS.push_back(std::move(tsos));
                        } else {
                            track_param_owner.reset();
                        }
                    }
                }
                trackParameters = track_param_owner.get();
            }

            // failure in calo association and/or extrapolation to indet
            if (!trackParameters || caloTSOS.empty()) {
                ATH_MSG_DEBUG("  perigee back-extrapolation fails ");
                return nullptr;
            }
        }  // if (perigee) {

        // set seed if provided
        if (seedParameters) { trackParameters = seedParameters; }

        // append TSOS objects into DataVector
        // reserve allows for perigee + vertex + calo + entrancePerigee + spectrometer TSOS
        const unsigned int size = spectrometerTSOS.size() + 3 + caloTSOS.size() + leadingTSOS.size();

        Trk::TrackStates trackStateOnSurfaces{};
        trackStateOnSurfaces.reserve(size);

        // start with perigee TSOS (this just carries the perigee parameters)
        
        if (trackParameters && !trackParameters->covariance()) { ATH_MSG_VERBOSE(" createExtrapolatedTrack: no cov (2)"); }

        if (trackParameters) {
            if (trackParameters->surfaceType() != Trk::SurfaceType::Perigee) {
                ATH_MSG_DEBUG("createExtrapolatedTrack() - Track parameters are not perigee " << (*trackParameters));
            }
            trackStateOnSurfaces.push_back(Muon::MuonTSOSHelper::createPerigeeTSOS(trackParameters->uniqueClone()));
        }

        // optionally append a pseudoMeasurement describing the vertex
        if (vertex && trackParameters) {
            std::unique_ptr<Trk::PseudoMeasurementOnTrack> vertexInFit = vertexOnTrack(*trackParameters, vertex, mbeamAxis);
            if (vertexInFit) {
                ATH_MSG_VERBOSE("Adding vertex constraint ");          
                trackStateOnSurfaces.push_back(Muon::MuonTSOSHelper::createMeasTSOS(std::move(vertexInFit), nullptr, Trk::TrackStateOnSurface::Measurement));
            }
        }

        // append calo TSOS
        for (std::unique_ptr<const Trk::TrackStateOnSurface>& c_tsos : caloTSOS) { trackStateOnSurfaces.push_back(std::move(c_tsos)); }
        caloTSOS.clear();
        // MS entrance perigee
        if (m_perigeeAtSpectrometerEntrance) {
            ATH_MSG_DEBUG("adding perigee at spectrometer entrance");
            const Trk::TrackParameters* mstrackParameters = trackStateOnSurfaces.back()->trackParameters();

            if (!mstrackParameters) { mstrackParameters = spectrometerTSOS.front()->trackParameters(); }

            if (mstrackParameters) {
                std::unique_ptr<Trk::TrackStateOnSurface> entranceTSOS = entrancePerigee(ctx, mstrackParameters);
                if (entranceTSOS) { trackStateOnSurfaces.push_back(std::move(entranceTSOS)); }
            }
        }

        // append leading MS material TSOS
        for (std::unique_ptr<const Trk::TrackStateOnSurface>& c_tsos : leadingTSOS) {
            if (c_tsos->materialEffectsOnTrack()) { trackStateOnSurfaces.push_back(std::move(c_tsos)); }
        }
        leadingTSOS.clear();

        // append the remaining spectrometer TSOS
        for (const auto& s : spectrometerTSOS) {
            if (!s->type(Trk::TrackStateOnSurface::Perigee)) {
                /// Actually I am not certain whether the paramters need a clone or we can move them
                trackStateOnSurfaces.push_back(s->clone());
            }

            if (s->measurementOnTrack() && dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(s->measurementOnTrack())) {
                ATH_MSG_VERBOSE(" MS Pseudo");
            }
        }

        // create track
        std::unique_ptr<Trk::Track> track =
            std::make_unique<Trk::Track>(spectrometerTrack.info(), std::move(trackStateOnSurfaces), nullptr);

        if (!track->perigeeParameters()) {
            ATH_MSG_DEBUG("Reject track without perigee.");
            return nullptr;
        }
        dumpCaloEloss(track.get(), " createExtrapolatedTrack ");
        if (msgLevel(MSG::DEBUG)) countAEOTs(track.get(), " createExtrapolatedTrack before fit ");

        // remove material when curvature badly determined (to remove energy loss uncertainty)
        if (particleHypothesis == Trk::nonInteracting) {
            ATH_MSG_VERBOSE("  remove spectrometer material ");
            removeSpectrometerMaterial(track);
        }

        // fit the track
        ATH_MSG_VERBOSE("  fit SA track with " << track->trackStateOnSurfaces()->size() << " TSOS"
                                               << (particleHypothesis == Trk::nonInteracting ? " using nonInteracting hypothesis"
                                                                                             : "usig interacting hypothesis"));

        std::unique_ptr<Trk::Track> fittedTrack{fit(ctx, *track, runOutlier, particleHypothesis)};
        if (fittedTrack) {
            if (msgLevel(MSG::DEBUG)) countAEOTs(fittedTrack.get(), " createExtrapolatedTrack after fit");

            // only accept when perigee in indet tracking volume
            if (fittedTrack->perigeeParameters() && !m_indetVolume->inside(fittedTrack->perigeeParameters()->position())) {
                ATH_MSG_DEBUG("  back extrapolation problem: fitted perigee outside indet volume ");
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
        // return the unfitted track in case of problem
        return track;
    }

    std::unique_ptr<Trk::Track> CombinedMuonTrackBuilder::createIndetTrack(const Trk::TrackInfo& info,
                                                                           const Trk::TrackStates* tsos) const {
        // create indet track TSOS vector
        Trk::TrackStates trackStateOnSurfaces{};
        Trk::TrackStates::const_iterator begin  = tsos->begin();
        Trk::TrackStates::const_iterator end  = tsos->end();
        

        // set end iterator to be the first TSOS after the indet
        unsigned size = 1;

        Trk::TrackStates::const_iterator s = begin;

        std::unique_ptr<Trk::TrackStateOnSurface> perigeeTSOS {(**s).clone()};

        ++s;  // keep start perigee where-ever!
        for (; s != end; ++s) {
            ++size;
            if (!m_indetVolume->inside((**s).trackParameters()->position())) { break; }
        }
        end = s;

        trackStateOnSurfaces.reserve(size);
        trackStateOnSurfaces.push_back(std::move(perigeeTSOS));

        // then append selected TSOS
        appendSelectedTSOS(trackStateOnSurfaces, begin, end);

        return std::make_unique<Trk::Track>(info, std::move(trackStateOnSurfaces), nullptr);
    }

    std::unique_ptr<Trk::Track> CombinedMuonTrackBuilder::createMuonTrack(const EventContext& ctx, const Trk::Track& muonTrack,
                                                                      const Trk::TrackParameters* parameters, std::unique_ptr<CaloEnergy> caloEnergy,
                                                                      const Trk::TrackStates* tsos) const{
        Trk::TrackStates::const_iterator begin = tsos->begin();
        Trk::TrackStates::const_iterator end = tsos->end();
        size_t size = tsos->size();

        if (msgLevel(MSG::DEBUG)) countAEOTs(&muonTrack, " createMuonTrack ");

        // set iterator to current TSOS on input track to be after the indet
        const Trk::TrackParameters* lastIDtp = nullptr;
        Trk::TrackStates::const_iterator s = begin;
        while ((**s).trackParameters() &&
               (m_indetVolume->inside((**s).trackParameters()->position()) || (**s).type(Trk::TrackStateOnSurface::Perigee))) {
            if (m_indetVolume->inside((**s).trackParameters()->position())) { lastIDtp = (**s).trackParameters(); }
            ++s;
            --size;
        }

        // create muon track TSOS vector
        Trk::TrackStates trackStateOnSurfaces{};

        // redo calo association from inside if requested
        bool redoCaloAssoc = false;
        if (parameters) {
            redoCaloAssoc = true;

            // move current TSOS iterator to be outside the calorimeter
            while ((**s).trackParameters() &&
                   (m_calorimeterVolume->inside((**s).trackParameters()->position()) || (**s).type(Trk::TrackStateOnSurface::Perigee))) {
                ++s;
                --size;
            }

            // associate calo by extrapolation from last ID parameters
            std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> caloTSOS;
            if (m_useCaloTG) {
                if (!lastIDtp) { lastIDtp = parameters; }
                ATH_MSG_VERBOSE(" Retriving Calorimeter TSOS from " << __func__ << " at line " << __LINE__);
                caloTSOS = getCaloTSOSfromMatProvider(*lastIDtp, muonTrack);
            } else {
                caloTSOS = m_caloTSOS->caloTSOS(ctx, *parameters);
            }

            if (caloTSOS.size() < 3) {
                ATH_MSG_DEBUG(" muonTrack: parameters fail to fully intersect the calorimeter");
                return nullptr;
            }

            size += caloTSOS.size();
            trackStateOnSurfaces.reserve(size + 1);

            // start with the calo TSOS
            for (std::unique_ptr<const Trk::TrackStateOnSurface>& c_tsos : caloTSOS) { trackStateOnSurfaces.push_back(std::move(c_tsos)); }

        } else {
            trackStateOnSurfaces.reserve(size + 1);
        }

        // if requested, replace caloEnergy on appropriate TSOS
        //// Note that this can never happen if parameters are given
        if (caloEnergy && (**s).trackParameters() && m_calorimeterVolume->inside((**s).trackParameters()->position())) {
            const Trk::TrackStateOnSurface* TSOS = (**s).clone();
            trackStateOnSurfaces.push_back(TSOS);
            ++s;

            // create MEOT owning CaloEnergy
            if ((**s).trackParameters() && m_calorimeterVolume->inside((**s).trackParameters()->position())) {
                std::bitset<Trk::MaterialEffectsBase::NumberOfMaterialEffectsTypes> typePattern;
                typePattern.set(Trk::MaterialEffectsBase::EnergyLossEffects);

                std::unique_ptr<Trk::MaterialEffectsOnTrack> materialEffects =
                    std::make_unique<Trk::MaterialEffectsOnTrack>(0., std::move(caloEnergy), (**s).trackParameters()->associatedSurface(), typePattern);
                 
                // create TSOS

                std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type;
                type.set(Trk::TrackStateOnSurface::CaloDeposit);

                TSOS = new Trk::TrackStateOnSurface(nullptr, (**s).trackParameters()->uniqueClone(), nullptr, std::move(materialEffects), type);
                trackStateOnSurfaces.push_back(TSOS);
                ++s;
            } else {
                // should never happen: FSR caloEnergy delete
                m_messageHelper->printWarning(37);               
            }
        }

        // MS entrance perigee
        bool hasAlreadyPerigee = false;
        if (m_perigeeAtSpectrometerEntrance) {
            // copy calorimeter TSOS
            while ((**s).trackParameters() && m_calorimeterVolume->inside((**s).trackParameters()->position())) {
                if (!(**s).type(Trk::TrackStateOnSurface::Perigee)) {
                    const Trk::TrackStateOnSurface* TSOS = (**s).clone();
                    trackStateOnSurfaces.push_back(TSOS);
                }
                ++s;
            }

            // add entrance TSOS if not already present
            std::unique_ptr<Trk::TrackStateOnSurface> entranceTSOS;

            if ((**s).type(Trk::TrackStateOnSurface::Perigee)) { hasAlreadyPerigee = true; }

            if (!hasAlreadyPerigee) {
                if ((**s).trackParameters()) {
                    entranceTSOS = entrancePerigee(ctx, (**s).trackParameters());
                } else {
                    entranceTSOS = entrancePerigee(ctx, trackStateOnSurfaces.back()->trackParameters());
                }
                if (entranceTSOS) {
                    double distance = (entranceTSOS->trackParameters()->position() - (**s).trackParameters()->position()).mag();

                    if (distance > 2000) {
                        ATH_MSG_DEBUG(" Added Muon Entrance "
                                      << " r " << entranceTSOS->trackParameters()->position().perp() << " z "
                                      << entranceTSOS->trackParameters()->position().z() << " track pars r "
                                      << (**s).trackParameters()->position().perp() << " z " << (**s).trackParameters()->position().z());
                    }
                    trackStateOnSurfaces.push_back(std::move(entranceTSOS));
                    hasAlreadyPerigee = true;
                }
            }
        }


        // then append selected TSOS from the extrapolated or spectrometer track
        appendSelectedTSOS(trackStateOnSurfaces, s, end);
        /// Check that the perigee parameters exist
        if (!hasAlreadyPerigee && std::find_if(trackStateOnSurfaces.begin(), trackStateOnSurfaces.end(), 
                [] (const Trk::TrackStateOnSurface* tsos){
                     return tsos->type(Trk::TrackStateOnSurface::Perigee);
        }) == trackStateOnSurfaces.end() && muonTrack.perigeeParameters() ){
            trackStateOnSurfaces.push_back( Muon::MuonTSOSHelper::createPerigeeTSOS(muonTrack.perigeeParameters()->uniqueClone()));
            /// Move the perigee to the front
            std::stable_sort(trackStateOnSurfaces.begin(),trackStateOnSurfaces.end(), 
                    [](const Trk::TrackStateOnSurface* a , const Trk::TrackStateOnSurface* b){
                        return a->type(Trk::TrackStateOnSurface::Perigee) > b->type(Trk::TrackStateOnSurface::Perigee);
                    });
            ATH_MSG_DEBUG(__FILE__<<":"<<__LINE__<<" No track perigee parameters were added. Copy the existing ones from the muon track");            
        }
        std::unique_ptr<Trk::Track> newMuonTrack = std::make_unique<Trk::Track>(muonTrack.info(), std::move(trackStateOnSurfaces), nullptr);
        
        // Updates the calo TSOS with the ones from TG+corrections (if needed)
        if (m_updateWithCaloTG && !m_useCaloTG && redoCaloAssoc) {
            ATH_MSG_VERBOSE("Updating Calorimeter TSOS in CreateMuonTrack ...");
            m_materialUpdator->updateCaloTSOS(*newMuonTrack, parameters);
        }

        return newMuonTrack;
    }

    std::unique_ptr<Trk::TrackStateOnSurface> CombinedMuonTrackBuilder::createPhiPseudoMeasurement(const EventContext& ctx,
                                                                                                   const Trk::Track& track) const {
        auto parameters = m_trackQuery->spectrometerParameters(track, ctx);
        Amg::MatrixX covarianceMatrix(1, 1);
        covarianceMatrix.setZero();
        covarianceMatrix(0, 0) = s_sigmaPhiSector * s_sigmaPhiSector * parameters->position().perp2();

        std::unique_ptr<Trk::PseudoMeasurementOnTrack> pseudo = std::make_unique<Trk::PseudoMeasurementOnTrack>(Trk::LocalParameters(Trk::DefinedParameter(0., Trk::locY)),
                                                                            covarianceMatrix, parameters->associatedSurface());
        
        return Muon::MuonTSOSHelper::createMeasTSOS(std::move(pseudo), std::move(parameters), Trk::TrackStateOnSurface::Measurement);
    }

    std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> CombinedMuonTrackBuilder::createSpectrometerTSOS(const EventContext& ctx, 
        const Trk::Track& spectrometerTrack) const {
        const Trk::Perigee* measuredPerigee = spectrometerTrack.perigeeParameters();
        std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> spectrometerTSOS;

        if (!measuredPerigee || !measuredPerigee->covariance() || !Amg::saneCovarianceDiagonal(*measuredPerigee->covariance())) {
            // missing MeasuredPerigee for spectrometer track
            if (!measuredPerigee)
                m_messageHelper->printWarning(38);
            else if (!measuredPerigee->covariance())
                m_messageHelper->printWarning(38);
            else
                ATH_MSG_DEBUG("createSpectrometerTSOS::perigee covariance not valid");
            return spectrometerTSOS;
        }

        double errorPhi = std::sqrt((*measuredPerigee->covariance())(Trk::phi0, Trk::phi0));

        // create the spectrometer TSOS's for the extrapolated fit
        spectrometerTSOS.reserve(spectrometerTrack.trackStateOnSurfaces()->size());

        // start with a 'phi sector constraint' pseudomeasurement when necessary
        unsigned numberPseudo = m_trackQuery->numberPseudoMeasurements(spectrometerTrack);
        if (errorPhi > s_sigmaPhiSector) { ++numberPseudo; }

        if (numberPseudo > 1 && !m_trackQuery->isSectorOverlap(spectrometerTrack)) {
            ATH_MSG_VERBOSE("standaloneFit: add pseudo to constrain phi sector");

            std::unique_ptr<Trk::TrackStateOnSurface> tsos = {createPhiPseudoMeasurement(ctx, spectrometerTrack)};
            if (tsos) { spectrometerTSOS.emplace_back(std::move(tsos)); }
        }

        // make a measurement selection to fixup non-standard TSOS's
        double deltaZ = 0.;
        bool haveMeasurement = false;

        std::vector<const Trk::Surface*> measurementSurfaces;
        measurementSurfaces.reserve(spectrometerTrack.trackStateOnSurfaces()->size());

        unsigned numberMaterial{0}, numberParameters{0};

        const Trk::Surface* previousSurface = nullptr;
        std::unique_ptr<const Trk::TrackStateOnSurface> previousTSOS;

        for (const Trk::TrackStateOnSurface* s : *spectrometerTrack.trackStateOnSurfaces()) {
            // skip any leading material
            if (!haveMeasurement) {
                if (s->measurementOnTrack()) {
                    haveMeasurement = true;
                } else if (s->materialEffectsOnTrack()) {
                    continue;
                }
            }

            // input statistics for VERBOSE
            const Trk::TrackParameters* trackParameters = s->trackParameters();
            if (msgLvl(MSG::VERBOSE)) {
                if (s->materialEffectsOnTrack()) ++numberMaterial;
                if (trackParameters) ++numberParameters;
            }

            // skip unwanted TSOS and non-understood features in iPatFitter
            if (!s->measurementOnTrack() && !s->materialEffectsOnTrack()) {
                // remove holes as they will be reallocated
                if (s->type(Trk::TrackStateOnSurface::Hole)) continue;

                // same for MS perigee
                if (s->type(Trk::TrackStateOnSurface::Perigee)) continue;

                if (s->trackParameters()) {
                    ATH_MSG_DEBUG("createSpectrometerTSOS:: skip unrecognized TSOS " << s->dumpType() << " r "
                                                                                     << s->trackParameters()->position().perp() << " z "
                                                                                     << s->trackParameters()->position().z());
                } else {
                    // skip unrecognized TSOS without TrackParameters
                    m_messageHelper->printWarning(39, s->dumpType());
                }
                continue;
            }

            // several checks applied to  measurements:
            bool trapezoid = false;
            bool rotatedTrap = false;
            if (s->measurementOnTrack()) {
                // skip pseudo
                if (dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(s->measurementOnTrack())) {
                    continue;
                } else {
                    // careful with trapezoid ordering (put trapezoid before
                    // rotatedTrapezoid)
                    const Trk::Surface* surface = &s->measurementOnTrack()->associatedSurface();

                    if (previousSurface) { deltaZ = std::abs(previousSurface->center().z() - surface->center().z()); }

                    if (dynamic_cast<const Trk::PlaneSurface*>(surface)) {
                        if (dynamic_cast<const Trk::TrapezoidBounds*>(&surface->bounds())) {
                            trapezoid = true;
                        } else if (dynamic_cast<const Trk::RotatedTrapezoidBounds*>(&surface->bounds())) {
                            rotatedTrap = true;
                        }
                    }

                    // skip duplicate measurements on same surface
                    if (previousSurface &&
                        std::find(measurementSurfaces.begin(), measurementSurfaces.end(), surface) != measurementSurfaces.end() &&
                        !m_idHelperSvc->isMM(surface->associatedDetectorElementIdentifier())) {
                        std::string type = "";
                        if (dynamic_cast<const Trk::CompetingRIOsOnTrack*>(s->measurementOnTrack())) {
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
                spectrometerTSOS.emplace_back(std::move(previousTSOS));
                previousTSOS.reset();
            }

            // trapezoid precedes rotatedTrapezoid
            std::unique_ptr<const Trk::TrackStateOnSurface> TSOS(s->clone());
            if (previousTSOS) {
                if (trapezoid && deltaZ < 1. * Gaudi::Units::mm) {
                    spectrometerTSOS.emplace_back(std::move(TSOS));
                    TSOS = std::move(previousTSOS);
                } else {
                    spectrometerTSOS.emplace_back(std::move(previousTSOS));
                }
            }

            if (rotatedTrap) {
                previousTSOS.swap(TSOS);
                continue;
            }

            spectrometerTSOS.emplace_back(std::move(TSOS));
        }

        if (previousTSOS) spectrometerTSOS.emplace_back(std::move(previousTSOS));

        ATH_MSG_VERBOSE(" input spectrometer track with " << spectrometerTrack.trackStateOnSurfaces()->size() << " TSOS, of which "
                                                          << numberMaterial << " have MaterialEffects and " << numberParameters
                                                          << " have TrackParameters");

        return spectrometerTSOS;
    }

     std::unique_ptr<Trk::TrackStateOnSurface> CombinedMuonTrackBuilder::entrancePerigee(const EventContext& ctx, const Trk::TrackParameters* parameters) const {
        // make sure the spectrometer entrance volume is available
        if (!parameters) return nullptr;
        const Trk::TrackingVolume* spectrometerEntrance = getVolume(ctx, "MuonSpectrometerEntrance");
        if (!spectrometerEntrance) return nullptr;

        std::unique_ptr<Trk::TrackParameters> entranceParameters{
            m_extrapolator->extrapolateToVolume(ctx, *parameters, *spectrometerEntrance, Trk::anyDirection, Trk::nonInteracting)};

        if (!entranceParameters) return nullptr;

        Trk::PerigeeSurface surface(entranceParameters->position());
        std::unique_ptr<Trk::TrackParameters> trackParameters{m_extrapolator->extrapolateDirectly(ctx, *entranceParameters, surface)};

        if (!trackParameters) return nullptr;

        std::unique_ptr<Trk::Perigee> perigee =
            std::make_unique<Trk::Perigee>(trackParameters->position(), trackParameters->momentum(), trackParameters->charge(), std::move(surface));
        return std::unique_ptr<Trk::TrackStateOnSurface>(Muon::MuonTSOSHelper::createPerigeeTSOS(std::move(perigee)));
    }

    std::unique_ptr<Trk::TrackParameters> CombinedMuonTrackBuilder::extrapolatedParameters(
        const EventContext& ctx, bool& badlyDeterminedCurvature, const Trk::Track& spectrometerTrack, const Trk::RecVertex* mvertex,
        const Trk::PerigeeSurface* mperigeeSurface) const {
        badlyDeterminedCurvature = false;
        const Trk::Perigee* measuredPerigee = spectrometerTrack.perigeeParameters();

        if (!measuredPerigee || !measuredPerigee->covariance()) {
            // missing MeasuredPerigee for spectrometer track
            m_messageHelper->printWarning(42);
            return nullptr;
        }

        // set starting parameters and measured momentum error
        auto parameters = m_trackQuery->spectrometerParameters(spectrometerTrack, ctx);
        if (!parameters || !parameters->covariance()) {
            // missing spectrometer parameters on spectrometer track
            m_messageHelper->printWarning(43);
            return nullptr;
        }

        double errorP = std::sqrt(measuredPerigee->momentum().mag2() * (*measuredPerigee->covariance())(Trk::qOverP, Trk::qOverP));

        // corrected parameters ensure the track fitting starts with a projective approximation
        std::unique_ptr<Trk::TrackParameters> correctedParameters{};
        Amg::VectorX parameterVector = parameters->parameters();
        double trackEnergy = 1. / std::abs(parameterVector[Trk::qOverP]);

        // careful: need to reset parameters to have a sensible energy if starting from a lineFit
        if (m_trackQuery->isLineFit(spectrometerTrack)) {
            trackEnergy = m_lineMomentum;
            parameterVector[Trk::qOverP] = parameters->charge() / trackEnergy;

            parameters = parameters->associatedSurface().createUniqueTrackParameters(
                parameterVector[Trk::loc1], parameterVector[Trk::loc2], parameterVector[Trk::phi], parameterVector[Trk::theta],
                parameterVector[Trk::qOverP], AmgSymMatrix(5)(*parameters->covariance()));
        }

        // check if the track curvature is well determined (with sufficient energy to penetrate material)
        //  (i.e. field off or small momentum error, starting parameters upstream of endcap toroid)
        bool curvatureOK = false;

        const Trk::IPropagator* propagator = m_propagator.get();
        MagField::AtlasFieldCache fieldCache;
        // Get field cache object
        if (!loadMagneticField(ctx, fieldCache)) return nullptr;
        if (!fieldCache.toroidOn()) {
            curvatureOK = true;
            propagator = m_propagatorSL.get();
        } else if (std::abs(parameters->position().z()) < m_zECToroid &&
                   (!m_trackQuery->isLineFit(spectrometerTrack) && errorP < m_largeMomentumError)) {
            curvatureOK = true;
        }

        if (curvatureOK) {
            // TDDO Run2 Calo TG
            // energy loss correction
            std::unique_ptr<CaloEnergy> caloEnergy{
                m_caloEnergyParam->energyLoss(ctx, trackEnergy, parameters->position().eta(), parameters->position().phi())};

            if (trackEnergy + caloEnergy->deltaE() < m_minEnergy) {
                ATH_MSG_DEBUG("standaloneFit: trapped in calorimeter");
                return nullptr;
            }
  
            parameterVector[Trk::qOverP] = parameters->charge() / (trackEnergy + caloEnergy->deltaE());
            correctedParameters = parameters->associatedSurface().createUniqueTrackParameters(
                parameterVector[Trk::loc1], parameterVector[Trk::loc2], parameterVector[Trk::phi], parameterVector[Trk::theta],
                parameterVector[Trk::qOverP], AmgSymMatrix(5)(*parameters->covariance()));

            // protect against spectrometer track with unrealistic energy loss
            // check material in spectrometer is not vastly greater than in the calo
            // (there are some very dense spectrometer regions)
            double spectrometerEnergyLoss = 0.;

            Trk::TrackStates::const_iterator s = spectrometerTrack.trackStateOnSurfaces()->begin();
            Trk::TrackStates::const_iterator sEnd = spectrometerTrack.trackStateOnSurfaces()->end();
            for (; s != sEnd; ++s) {
                if (!(**s).materialEffectsOnTrack()) { continue; }

                const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>((**s).materialEffectsOnTrack());

                if (meot && meot->energyLoss()) { spectrometerEnergyLoss += meot->energyLoss()->deltaE(); }
            }

            if (std::abs(spectrometerEnergyLoss) > 1.5 * std::abs(caloEnergy->deltaE())) {
                curvatureOK = false;
                ATH_MSG_DEBUG("standaloneFit: excessive energy loss in spectrometer "
                              << std::abs(spectrometerEnergyLoss / Gaudi::Units::GeV) << " GeV"
                              << "  in calo " << std::abs(caloEnergy->deltaE() / Gaudi::Units::GeV) << " GeV");
            }
        }

        // check perigee in indet volume when the curvature is well determined
        // otherwise will assume projectivity for starting parameters
        if (curvatureOK) {
            std::unique_ptr<Trk::TrackParameters> perigee{propagator->propagate(
                ctx, *correctedParameters, *mperigeeSurface, Trk::oppositeMomentum, false, m_magFieldProperties, Trk::nonInteracting)};

            if (!perigee) {
                ATH_MSG_DEBUG("standaloneFit: failed back extrapolation to perigee");
                return nullptr;
            }

            // large impact: set phi to be projective (note iteration)
            if (std::abs(perigee->parameters()[Trk::d0]) < m_largeImpact || !fieldCache.toroidOn()) {
                ATH_MSG_DEBUG("Track d0 perigee: " << std::abs(perigee->parameters()[Trk::d0]) << " which is smaller than "
                                                   << m_largeImpact);
            } else {
                Amg::Vector3D position = correctedParameters->position();

                double deltaPhi = 0.;
                double deltaR = (position - perigee->position()).perp();

                if (std::abs(deltaR * M_PI) > std::abs(perigee->parameters()[Trk::d0])) {
                    deltaPhi = perigee->parameters()[Trk::d0] / deltaR;
                }

                ATH_MSG_DEBUG("standaloneFit:    large perigee impact " << perigee->parameters()[Trk::d0] << "  deltaR, deltaPhi " << deltaR
                                                                        << ", " << deltaPhi);

                parameterVector[Trk::phi0] += deltaPhi;

                if (parameterVector[Trk::phi0] > M_PI) {
                    parameterVector[Trk::phi0] -= 2. * M_PI;
                } else if (parameterVector[Trk::phi0] < -M_PI) {
                    parameterVector[Trk::phi0] += 2. * M_PI;
                }

                correctedParameters = parameters->associatedSurface().createUniqueTrackParameters(
                    parameterVector[Trk::loc1], parameterVector[Trk::loc2], parameterVector[Trk::phi], parameterVector[Trk::theta],
                    parameterVector[Trk::qOverP], AmgSymMatrix(5)(*parameters->covariance()));

                perigee = propagator->propagate(ctx, *correctedParameters, *mperigeeSurface, Trk::oppositeMomentum, false,
                                                m_magFieldProperties, Trk::nonInteracting);

                if (perigee) {
                    deltaPhi = 0.;
                    deltaR = (position - perigee->position()).perp();

                    if (std::abs(deltaR * M_PI) > std::abs(perigee->parameters()[Trk::d0])) {
                        deltaPhi = perigee->parameters()[Trk::d0] / deltaR;
                    }

                    ATH_MSG_VERBOSE("standaloneFit: corrected perigee impact " << perigee->parameters()[Trk::d0] << "  deltaR, deltaPhi "
                                                                               << deltaR << ", " << deltaPhi);

                    parameterVector[Trk::phi0] += deltaPhi;
                    if (parameterVector[Trk::phi0] > M_PI) {
                        parameterVector[Trk::phi0] -= 2. * M_PI;
                    } else if (parameterVector[Trk::phi0] < -M_PI) {
                        parameterVector[Trk::phi0] += 2. * M_PI;
                    }
                    correctedParameters = parameters->associatedSurface().createUniqueTrackParameters(
                        parameterVector[Trk::loc1], parameterVector[Trk::loc2], parameterVector[Trk::phi], parameterVector[Trk::theta],
                        parameterVector[Trk::qOverP], AmgSymMatrix(5)(*parameters->covariance()));

                    perigee = propagator->propagate(ctx, *correctedParameters, *mperigeeSurface, Trk::oppositeMomentum, false,
                                                    m_magFieldProperties, Trk::nonInteracting);
                }

                if (perigee) {
                    ATH_MSG_VERBOSE("standaloneFit:    restart with impact " << perigee->parameters()[Trk::d0] << "   phi0 "
                                                                             << perigee->parameters()[Trk::phi0]);
                }
                parameterVector[Trk::qOverP] = parameters->charge() / trackEnergy;
                correctedParameters = parameters->associatedSurface().createUniqueTrackParameters(
                    parameterVector[Trk::loc1], parameterVector[Trk::loc2], parameterVector[Trk::phi], parameterVector[Trk::theta],
                    parameterVector[Trk::qOverP], AmgSymMatrix(5)(*parameters->covariance()));

                parameters = std::move(correctedParameters);
            }

            // cut if perigee outside indet (but keep endcap halo)
            if (!perigee || !m_indetVolume->inside(perigee->position())) {
                if (perigee && perigee->position().z() * perigee->momentum().z() < 0. && perigee->momentum().eta() > 2.0) {
                    ATH_MSG_DEBUG("standaloneFit: halo candidate, perigee at R  " << perigee->position().perp() << "  Z "
                                                                                  << perigee->position().z());
                } else {
                    ATH_MSG_DEBUG("standaloneFit: perigee outside indet volume");
                    ///  if (haveSpectrometerRefit) delete spectrometerFit;
                    return nullptr;
                }
            }           
        } else {
            // otherwise track out from origin (fix bug #54820)
            badlyDeterminedCurvature = true;
            Amg::Vector3D momentum = parameters->position().unit() * Gaudi::Units::TeV;

            std::unique_ptr<const Trk::TrackParameters> trigParameters{m_trackQuery->triggerStationParameters(spectrometerTrack, ctx)};

            if (trigParameters) { momentum = trigParameters->position().unit() * Gaudi::Units::TeV; }

            if (msgLvl(MSG::VERBOSE)) {
                if (trigParameters) {
                    ATH_MSG_VERBOSE("standaloneFit: imprecise curvature measurement -"
                                    << " start with line from origin to 1st trigger station ");
                } else {
                    ATH_MSG_VERBOSE("standaloneFit: imprecise curvature measurement -"
                                    << " start with line from origin to 1st measurement ");
                }
            }

            std::unique_ptr<Trk::TrackParameters> perigee =
                std::make_unique<Trk::Perigee>(mvertex->position(), momentum, 1., *mperigeeSurface);

            parameters = m_propagator->propagate(ctx, *perigee, perigee->associatedSurface(), Trk::alongMomentum, false,
                                                 m_magFieldProperties, Trk::nonInteracting);

            if (!parameters) {
                ATH_MSG_DEBUG("standaloneFit: failed back extrapolation to perigee");
                return nullptr;
            }
        }

        return parameters;
    }

    void CombinedMuonTrackBuilder::finalTrackBuild(const EventContext& ctx, std::unique_ptr<Trk::Track>& track) const {
        // as a final step:
        //   refit the track if any holes can be recovered,
        //   refit with optimization of the spectrometer hit errors,
        //  add the corresponding TrackSummary to the track
        if (msgLevel(MSG::DEBUG)) countAEOTs(track.get(), " finalTrackBuilt input ");

        if (!m_muonHoleRecovery.empty()) {
            // chi2 before recovery
            double chi2Before = normalizedChi2(*track);

            ATH_MSG_VERBOSE(" perform spectrometer hole recovery procedure... ");

            std::unique_ptr<Trk::Track> recoveredTrack{m_muonHoleRecovery->recover(*track, ctx)};

            if (!recoveredTrack || !recoveredTrack->fitQuality() || !checkTrack("finalTrackBuild1", recoveredTrack.get(), track.get())) {
                // final track lost, this should not happen
                m_messageHelper->printWarning(44);

            } else {
                double chi2After = normalizedChi2(*recoveredTrack);
                if (chi2After < m_badFitChi2 || chi2After < chi2Before + 0.1) {
                    track.swap(recoveredTrack);
                } else {
                    ATH_MSG_VERBOSE(" track rejected by recovery as chi2 " << chi2After << "  compared to " << chi2Before);

                    if (chi2Before > m_badFitChi2) {
                        track.reset();
                        return;
                    }
                }
            }
            ATH_MSG_VERBOSE(" finished hole recovery procedure ");
        }
        // final fit with optimized spectrometer errors
        if (!m_muonErrorOptimizer.empty() && !track->info().trackProperties(Trk::TrackInfo::StraightTrack) &&
            countAEOTs(track.get(), " before optimize ") == 0) {
            ATH_MSG_VERBOSE(" perform spectrometer error optimization... ");
            std::unique_ptr<Trk::Track> optimizedTrack = m_muonErrorOptimizer->optimiseErrors(*track, ctx);
            if (optimizedTrack && checkTrack("finalTrackBuild2", optimizedTrack.get(), track.get())) {
                track.swap(optimizedTrack);
                if (msgLevel(MSG::DEBUG)) countAEOTs(track.get(), " finalTrackBuilt alignment errors Track ");
            }
        }

        // add the track summary
        m_trackSummary->updateTrack(ctx, *track);
    }
    void CombinedMuonTrackBuilder::momentumUpdate(std::unique_ptr<const Trk::TrackParameters>& parameters, double updatedP,
                                                  bool directionUpdate, double deltaPhi, double deltaTheta) const {
        if (!parameters) return;

        std::unique_ptr<const Trk::TrackParameters> updatedParameters;

        // update for angle change
        Amg::Vector3D direction = parameters->momentum().unit();

        if (directionUpdate) {
            double cosDeltaPhi = 0.;
            double sinDeltaPhi = std::sin(deltaPhi);

            if (std::abs(sinDeltaPhi) < 1.) { cosDeltaPhi = std::sqrt(1. - sinDeltaPhi * sinDeltaPhi); }

            double cosDeltaTheta = 0.;
            double sinDeltaTheta = std::sin(deltaTheta);

            if (std::abs(sinDeltaTheta) < 1.) { cosDeltaTheta = std::sqrt(1. - sinDeltaTheta * sinDeltaTheta); }

            double cosTheta = direction.z() * cosDeltaTheta - direction.perp() * sinDeltaTheta;
            if (std::abs(cosTheta) < 1.) {
                direction = Amg::Vector3D(direction.x() * cosDeltaPhi - direction.y() * sinDeltaPhi,
                                          direction.y() * cosDeltaPhi + direction.x() * sinDeltaPhi,
                                          direction.perp() * cosTheta / std::sqrt(1. - cosTheta * cosTheta));

            } else {
                direction = Amg::Vector3D(0., 0., cosTheta);
            }
            direction = direction.unit();
        }

        // update for momentum (magnitude) change
        Amg::Vector3D momentum = updatedP * direction;

        // create updated parameters
        double charge = parameters->charge();
        Amg::Vector3D position = parameters->position();
        std::optional<AmgSymMatrix(5)> covariance =
            parameters->covariance() ? std::optional<AmgSymMatrix(5)>(*(parameters->covariance())) : std::nullopt;
        const Trk::Surface* surface = &(parameters->associatedSurface());
        updatedParameters = surface->createUniqueTrackParameters(position, momentum, charge, covariance);

        if (updatedParameters) {
            parameters = std::move(updatedParameters);
        } else {
            // update failed, keeping original value
            m_messageHelper->printWarning(45);
        }
    }

    double CombinedMuonTrackBuilder::normalizedChi2(const Trk::Track& track) const {
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

    std::unique_ptr<Trk::Track> CombinedMuonTrackBuilder::reallocateMaterial(const EventContext& ctx,
                                                                             const Trk::Track& spectrometerTrack) const {
        // build MeasurementSet for the spectrometer
        const Trk::TrackParameters* perigeeStartValue = nullptr;
        double perigeeDistance = 0.;

        Trk::MeasurementSet spectrometerMeasurements;

        Trk::TrackStates::const_iterator s = spectrometerTrack.trackStateOnSurfaces()->begin();
        auto sEnd = spectrometerTrack.trackStateOnSurfaces()->end();
        for (; s != sEnd; ++s) {
            if ((**s).measurementOnTrack() && !(**s).type(Trk::TrackStateOnSurface::Outlier)) {
                // skip pseudo measurement(s)
                // FIXME - need phi pseudo in some cases
                if (dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**s).measurementOnTrack())) { continue; }

                spectrometerMeasurements.push_back((**s).measurementOnTrack()->clone());
                if (!(**s).trackParameters() || (perigeeStartValue && (**s).trackParameters()->position().mag() > perigeeDistance)) {
                    continue;
                }

                perigeeDistance = (**s).trackParameters()->position().mag();
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
        std::unique_ptr<Trk::Track> spectrometerFit = fit(ctx, spectrometerMeasurements, *perigeeStartValue, true, Trk::muon);
        if (!spectrometerFit) {
            spectrometerFit = fit(ctx, spectrometerMeasurements, *perigeeStartValue, false, Trk::muon);

            if (!spectrometerFit) {
                spectrometerFit = fit(ctx, spectrometerMeasurements, *perigeeStartValue, false, Trk::nonInteracting);

                if (!spectrometerFit) {
                    // spectrometer refit fails
                    m_messageHelper->printWarning(47);
                }
            }
        }

        if (spectrometerFit) { spectrometerFit->info().addPatternReco(spectrometerTrack.info()); }

        Trk::MeasurementSet::iterator m = spectrometerMeasurements.begin();
        auto mEnd = spectrometerMeasurements.end();
        for (; m != mEnd; ++m) { delete *m; }

        return spectrometerFit;
    }

    void CombinedMuonTrackBuilder::removeSpectrometerMaterial(std::unique_ptr<Trk::Track>& track) const {
        // limit momentum to avoid refit allocating excessive energy loss
        bool limitMomentum = false;
        double momentum = track->perigeeParameters()->momentum().mag();
        double qOverP = 0.;

        if (momentum > m_lowMomentum) {
            const Trk::Perigee* measuredPerigee = track->perigeeParameters();

            if (measuredPerigee) {
                Trk::TrackStates::const_reverse_iterator r = track->trackStateOnSurfaces()->rbegin();

                while (!(**r).trackParameters()) { --r; }

                limitMomentum = true;

                if (!measuredPerigee->covariance()) {
                    ATH_MSG_DEBUG("measuredPerigee has no covariance, qOverP not set");
                    qOverP = (**r).trackParameters()->parameters()[Trk::qOverP];
                } else {
                    qOverP = (**r).trackParameters()->parameters()[Trk::qOverP] +
                             measuredPerigee->charge() * std::sqrt((*measuredPerigee->covariance())(Trk::qOverP, Trk::qOverP));

                    ATH_MSG_DEBUG(" limit momentum to " << 1. / std::abs(qOverP * Gaudi::Units::GeV) << "  from original value "
                                                        << momentum / Gaudi::Units::GeV);
                }
            }
        }

        // remove spectrometer material from track
        std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> defaultType;
        std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type = defaultType;
        Trk::TrackStates trackStateOnSurfaces{};

        trackStateOnSurfaces.reserve(track->trackStateOnSurfaces()->size());
        bool is_first{true};
        for (const Trk::TrackStateOnSurface* tsos : *track->trackStateOnSurfaces()) {
            // limit perigee
            if (tsos->trackParameters()) {
                if (limitMomentum && is_first && tsos->trackParameters()->covariance() &&
                    tsos->trackParameters()->surfaceType() == Trk::SurfaceType::Perigee) {
                    Amg::VectorX parameterVector = tsos->trackParameters()->parameters();
                    parameterVector[Trk::qOverP] = qOverP;

                    /// aaaaaahhhhh
                    std::unique_ptr<Trk::TrackParameters> parameters =
                        tsos->trackParameters()->associatedSurface().createUniqueTrackParameters(
                            parameterVector[Trk::loc1], parameterVector[Trk::loc2], parameterVector[Trk::phi], parameterVector[Trk::theta],
                            parameterVector[Trk::qOverP], *tsos->trackParameters()->covariance());

                    type = defaultType;
                    type.set(Trk::TrackStateOnSurface::Perigee);

                    std::unique_ptr<Trk::MeasurementBase> measurementBase;
                    if (tsos->measurementOnTrack()) {
                        measurementBase = tsos->measurementOnTrack()->uniqueClone();
                        type.set(Trk::TrackStateOnSurface::Measurement);
                    }
                    trackStateOnSurfaces.push_back(
                        new Trk::TrackStateOnSurface(std::move(measurementBase), std::move(parameters), nullptr, nullptr, type));
                } else {
                    trackStateOnSurfaces.push_back(tsos->clone());
                }
                is_first = false;
                continue;
            }
            is_first = false;

            // material in spectrometer
            if (tsos->materialEffectsOnTrack() &&
                !m_calorimeterVolume->inside(tsos->materialEffectsOnTrack()->associatedSurface().globalReferencePoint())) {
                if (tsos->measurementOnTrack()) {
                    Amg::VectorX parameterVector = tsos->trackParameters()->parameters();
                    if (limitMomentum) { parameterVector[Trk::qOverP] = qOverP; }
                    std::unique_ptr<Trk::TrackParameters> trackParameters =
                        tsos->trackParameters()->associatedSurface().createUniqueTrackParameters(
                            parameterVector[Trk::loc1], parameterVector[Trk::loc2], parameterVector[Trk::phi], parameterVector[Trk::theta],
                            parameterVector[Trk::qOverP],
                            tsos->trackParameters()->covariance() ? std::optional<AmgSymMatrix(5)>(*tsos->trackParameters()->covariance())
                                                                  : std::nullopt);

                    type = defaultType;
                    type.set(Trk::TrackStateOnSurface::Measurement);

                    if (tsos->type(Trk::TrackStateOnSurface::Outlier)) { type.set(Trk::TrackStateOnSurface::Outlier); }
                    std::unique_ptr<Trk::MeasurementBase> measurementBase;
                    measurementBase = tsos->measurementOnTrack()->uniqueClone();
                    trackStateOnSurfaces.push_back(
                        new Trk::TrackStateOnSurface(std::move(measurementBase), std::move(trackParameters), nullptr, nullptr, type));
                }
                continue;
            } else if (!tsos->measurementOnTrack() && tsos->trackParameters() &&
                       !m_calorimeterVolume->inside(tsos->trackParameters()->position())) {
                continue;
            }

            if (limitMomentum && tsos->trackParameters()) {
                Amg::VectorX parameterVector = tsos->trackParameters()->parameters();
                parameterVector[Trk::qOverP] = qOverP;
                std::unique_ptr<Trk::TrackParameters> trackParameters =
                    tsos->trackParameters()->associatedSurface().createUniqueTrackParameters(
                        parameterVector[Trk::loc1], parameterVector[Trk::loc2], parameterVector[Trk::phi], parameterVector[Trk::theta],
                        parameterVector[Trk::qOverP],
                        tsos->trackParameters()->covariance() ? std::optional<AmgSymMatrix(5)>(*tsos->trackParameters()->covariance())
                                                              : std::nullopt);

                type = defaultType;

                std::unique_ptr<Trk::MeasurementBase> measurementBase;
                if (tsos->measurementOnTrack()) {
                    type.set(Trk::TrackStateOnSurface::Measurement);

                    if (tsos->type(Trk::TrackStateOnSurface::Outlier)) { type.set(Trk::TrackStateOnSurface::Outlier); }

                    measurementBase = tsos->measurementOnTrack()->uniqueClone();
                }

                std::unique_ptr<Trk::MaterialEffectsBase> materialEffects;
                if (tsos->materialEffectsOnTrack()) {
                    if (tsos->type(Trk::TrackStateOnSurface::CaloDeposit)) { type.set(Trk::TrackStateOnSurface::CaloDeposit); }
                    if (tsos->type(Trk::TrackStateOnSurface::Scatterer)) { type.set(Trk::TrackStateOnSurface::Scatterer); }

                    materialEffects = tsos->materialEffectsOnTrack()->uniqueClone();
                }
                trackStateOnSurfaces.push_back(new Trk::TrackStateOnSurface(std::move(measurementBase), std::move(trackParameters),
                                                                                  nullptr, std::move(materialEffects), type));
            } else {
                trackStateOnSurfaces.push_back(tsos->clone());
            }
        }

        // replace track
        Trk::TrackInfo trackInfo = track->info();
        Trk::FitQuality* fitQuality = nullptr;
        if (track->fitQuality()) { fitQuality = new Trk::FitQuality(*track->fitQuality()); }

        track = std::make_unique<Trk::Track>(trackInfo, std::move(trackStateOnSurfaces), fitQuality);
    }

    std::unique_ptr<Trk::PseudoMeasurementOnTrack> CombinedMuonTrackBuilder::vertexOnTrack(const Trk::TrackParameters& parameters,
                                                                                           const Trk::RecVertex* vertex,
                                                                                           const Trk::RecVertex* mbeamAxis) {
        // create the corresponding PerigeeSurface, localParameters and covarianceMatrix
        const Trk::PerigeeSurface surface(vertex->position());
        Trk::LocalParameters localParameters;
        Amg::MatrixX covarianceMatrix;
        covarianceMatrix.setZero();

        // transform Cartesian (x,y,z) to beam axis or perigee
        Amg::Vector2D localPosition(0, 0);
        double ptInv = 1. / parameters.momentum().perp();

        if (vertex == mbeamAxis) {
            Trk::DefinedParameter d0(localPosition[Trk::locX], Trk::locX);
            localParameters = Trk::LocalParameters(d0);

            Amg::MatrixX jacobian(1, 3);
            jacobian.setZero();
            jacobian(0, 0) = -ptInv * parameters.momentum().y();
            jacobian(0, 1) = ptInv * parameters.momentum().x();

            const Amg::MatrixX& cov = vertex->covariancePosition();
            covarianceMatrix = cov.similarity(jacobian);
        } else {
            localParameters = Trk::LocalParameters(localPosition);

            Amg::MatrixX jacobian(2, 3);
            jacobian.setZero();
            jacobian(0, 0) = -ptInv * parameters.momentum().y();
            jacobian(0, 1) = ptInv * parameters.momentum().x();
            jacobian(1, 2) = 1.0;

            const Amg::MatrixX& cov = vertex->covariancePosition();
            covarianceMatrix = cov.similarity(jacobian);
        }

        return std::make_unique<Trk::PseudoMeasurementOnTrack>(localParameters, covarianceMatrix, surface);
    }

    int CombinedMuonTrackBuilder::countAEOTs(const Trk::Track* track, const std::string& txt) const {
        if (!track) return 0;
        const Trk::TrackStates* trackTSOS = track->trackStateOnSurfaces();
        int naeots = 0;

        for (const auto* m : *trackTSOS) {
            if (m->alignmentEffectsOnTrack()) naeots++;
        }

        ATH_MSG_DEBUG(" count AEOTs " << txt << " " << naeots);

        // add VEBOSE for checking TSOS order

        Trk::TrackStates::const_iterator it = track->trackStateOnSurfaces()->begin();
        Trk::TrackStates::const_iterator it_end = track->trackStateOnSurfaces()->end();

        int tsos = 0;
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
                                               << " r " << (*it)->measurementOnTrack()->associatedSurface().center().perp() << " z "
                                               << (*it)->measurementOnTrack()->associatedSurface().center().z());
            } else if ((*it)->materialEffectsOnTrack()) {
                ATH_MSG_VERBOSE(" check tsos " << tsos << " TSOS mat "
                                               << " r "
                                               << (*it)->materialEffectsOnTrack()->associatedSurface().globalReferencePoint().perp()
                                               << " z " << (*it)->materialEffectsOnTrack()->associatedSurface().globalReferencePoint().z());
            } else {
                ATH_MSG_VERBOSE(" check tsos other than above " << tsos);
            }
        }

        ATH_MSG_VERBOSE(" track with number of TSOS perigees " << nperigee);

        return naeots;
    }

    void CombinedMuonTrackBuilder::dumpCaloEloss(const Trk::Track* track, const std::string& txt) const {
        // will refit if extrapolated track was definitely bad
        if (!track || !msgLevel(MSG::DEBUG)) return;
        if (!m_trackQuery->isCaloAssociated(*track, Gaudi::Hive::currentContext())) {
            ATH_MSG_DEBUG(txt << " no TSOS in Calorimeter ");
            return;
        }

        const Trk::Track& originalTrack = *track;
        const CaloEnergy* caloEnergy = m_trackQuery->caloEnergy(originalTrack);
        if (caloEnergy) {
            ATH_MSG_DEBUG(txt << " Calorimeter Eloss " << caloEnergy->deltaE());
        } else {
            ATH_MSG_DEBUG(txt << " No Calorimeter Eloss");
        }

        const Trk::TrackStates* trackTSOS = track->trackStateOnSurfaces();

        double Eloss = 0.;
        double idEloss = 0.;
        double caloEloss = 0.;
        double msEloss = 0.;
        double deltaP = 0.;
        double pcalo = 0.;
        double pstart = 0.;
        double eta = 0.;
        double pMuonEntry = 0.;

        for (const auto* m : *trackTSOS) {
            const Trk::MeasurementBase* mot = m->measurementOnTrack();

            if (m->trackParameters()) { pMuonEntry = m->trackParameters()->momentum().mag(); }

            if (mot) {
                Identifier id = Trk::IdentifierExtractor::extract(mot);
                if (id.is_valid()) {
                    // skip after first Muon hit
                    if (m_idHelperSvc->isMuon(id)) { break; }
                }
            }

            if (pstart == 0 && m->trackParameters()) {
                pstart = m->trackParameters()->momentum().mag();
                eta = m->trackParameters()->momentum().eta();

                ATH_MSG_DEBUG("Start pars found eta " << eta << " r " << (m->trackParameters())->position().perp() << " z "
                                                      << (m->trackParameters())->position().z() << " pstart " << pstart);
            }

            if (m->materialEffectsOnTrack()) {
                const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(m->materialEffectsOnTrack());

                if (meot) {
                    if (meot->thicknessInX0() > 20) {
                        const Trk::ScatteringAngles* scatAngles = meot->scatteringAngles();

                        ATH_MSG_DEBUG(" Calorimeter X0  " << meot->thicknessInX0() << "  pointer scat " << scatAngles);

                        if (scatAngles) {
                            pcalo = m->trackParameters()->momentum().mag();
                            double pullPhi = scatAngles->deltaPhi() / scatAngles->sigmaDeltaPhi();
                            double pullTheta = scatAngles->deltaTheta() / scatAngles->sigmaDeltaTheta();

                            ATH_MSG_DEBUG(" Calorimeter scatterer deltaPhi (mrad) "
                                          << 1000 * scatAngles->deltaPhi() << " sigma " << 1000 * scatAngles->sigmaDeltaPhi() << " pull "
                                          << pullPhi << " deltaTheta (mrad) " << 1000 * scatAngles->deltaTheta() << " sigma "
                                          << 1000 * scatAngles->sigmaDeltaTheta() << " pull " << pullTheta);
                        }
                    }

                    const Trk::EnergyLoss* energyLoss = meot->energyLoss();
                    if (energyLoss) {
                        if (m->trackParameters()) {
                            ATH_MSG_DEBUG("Eloss found r " << (m->trackParameters())->position().perp() << " z "
                                                           << (m->trackParameters())->position().z() << " value " << energyLoss->deltaE()
                                                           << " Eloss " << Eloss);
                        }

                        if (m->type(Trk::TrackStateOnSurface::CaloDeposit)) {
                            idEloss = Eloss;
                            caloEloss = std::abs(energyLoss->deltaE());
                            Eloss = 0.;

                            if (m->trackParameters()) { deltaP = m->trackParameters()->momentum().mag() - pcalo; }

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
        Eloss = idEloss + caloEloss + msEloss;

        ATH_MSG_DEBUG(txt << " eta " << eta << " pstart " << pstart / Gaudi::Units::GeV << " Eloss on TSOS idEloss " << idEloss
                          << " caloEloss " << caloEloss << " msEloss " << msEloss << " Total " << Eloss << " pstart - pMuonEntry "
                          << pstart - pMuonEntry);

        return;
    }

    bool CombinedMuonTrackBuilder::checkTrack(const std::string& txt, const Trk::Track* newTrack, const Trk::Track* track) const {
        if (!newTrack) return false;

        const DataVector<const Trk::TrackParameters>* pars = newTrack->trackParameters();
        if (!pars || pars->empty()) { return false; }

        bool newTrackOK = true;

        DataVector<const Trk::TrackParameters>::const_iterator it = pars->end();
        it = it - 1;

        if ((*it)->position().dot((*it)->momentum().unit()) < 0) {
            newTrackOK = false;
            ATH_MSG_DEBUG(txt << " checkTrack newTrack ALARM position " << (*it)->position() << " direction " << (*it)->momentum().unit());
        } else {
            ATH_MSG_DEBUG(txt << " checkTrack newTrack OK position " << (*it)->position() << " direction " << (*it)->momentum().unit());
        }

        if (!track) { return newTrackOK; }

        for (const Trk::TrackParameters* par : *pars) {
            if (!par->covariance()) { continue; }
            if (!Amg::saneCovarianceDiagonal(*par->covariance())) {
                ATH_MSG_DEBUG(Amg::toString(*par->covariance()));
                ATH_MSG_DEBUG(
                    "covariance matrix has negative diagonal element, "
                    "killing track");
                return false;
            }
        }

        return newTrackOK;
    }

    const Trk::TrackingVolume* CombinedMuonTrackBuilder::getVolume(const EventContext& ctx, const std::string&& vol_name) const {
        /// Tracking geometry is provided by the TrackingGeometryAlg
        SG::ReadCondHandle<Trk::TrackingGeometry> handle(m_trackingGeometryReadKey, ctx);
        if (!handle.isValid()) {
            ATH_MSG_WARNING("Could not retrieve a valid tracking geometry");
            return nullptr;
        }
        return handle.cptr()->trackingVolume(vol_name);
    }
    const AtlasFieldCacheCondObj* CombinedMuonTrackBuilder::getFieldCacheObj(const EventContext& ctx) const {
        SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, ctx};
        if (!readHandle.isValid()) {
            ATH_MSG_ERROR("Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCacheCondObjInputKey.key());
            return nullptr;
        }
        return readHandle.cptr();
    }
    bool CombinedMuonTrackBuilder::loadMagneticField(const EventContext& ctx, MagField::AtlasFieldCache& fieldCache) const {
        const AtlasFieldCacheCondObj* fieldCondObj = getFieldCacheObj(ctx);
        if (!fieldCondObj) return false;
        fieldCondObj->getInitializedCache(fieldCache);
        return true;
    }
    std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> CombinedMuonTrackBuilder::getCaloTSOSfromMatProvider(
        const Trk::TrackParameters& track_params, const Trk::Track& me_track) const {
        std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> to_ret;
        std::unique_ptr<std::vector<const Trk::TrackStateOnSurface*>> tsos_vec{m_materialUpdator->getCaloTSOS(track_params, me_track)};
        if (tsos_vec) {
            to_ret.reserve(tsos_vec->size());
            for (const Trk::TrackStateOnSurface* tsos : *tsos_vec) to_ret.emplace_back(tsos);
        }
        return to_ret;
    }

}  // namespace Rec

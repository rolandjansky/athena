/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// OutwardsCombinedMuonTrackBuilder
//  AlgTool gathering  material effects along a combined muon track, in
//  particular the TSOS'es representing the calorimeter energy deposit and
//  Coulomb scattering.
//  The resulting track is fitted at the IP using the ITrackFitter interface.
//
//////////////////////////////////////////////////////////////////////////////
#include "OutwardsCombinedMuonTrackBuilder.h"

#include <cmath>
#include <iomanip>

#include "AthenaKernel/Units.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "VxVertex/RecVertex.h"

namespace Rec {

    OutwardsCombinedMuonTrackBuilder::OutwardsCombinedMuonTrackBuilder(const std::string& type, const std::string& name,
                                                                       const IInterface* parent) :
        AthAlgTool(type, name, parent),
        m_allowCleanerVeto(true),
        m_cleanCombined(true),
        m_recoverCombined(false),
        m_IDMS_xySigma(1. * Gaudi::Units::mm),
        m_IDMS_rzSigma(1. * Gaudi::Units::mm),
        m_addIDMSerrors(true) {
        declareInterface<ICombinedMuonTrackBuilder>(this);
        declareProperty("AllowCleanerVeto", m_allowCleanerVeto);
        declareProperty("CleanCombined", m_cleanCombined);
        declareProperty("RecoverCombined", m_recoverCombined);
        declareProperty("IDMS_xySigma", m_IDMS_xySigma);
        declareProperty("IDMS_rzSigma", m_IDMS_rzSigma);
        declareProperty("AddIDMSerrors", m_addIDMSerrors);
    }

    StatusCode OutwardsCombinedMuonTrackBuilder::initialize() {
        ATH_MSG_INFO("Initializing OutwardsCombinedMuonTrackBuilder.");

        msg(MSG::INFO) << " with options: ";
        if (m_allowCleanerVeto) msg(MSG::INFO) << " AllowCleanerVeto";
        if (m_cleanCombined) msg(MSG::INFO) << " CleanCombined";
        msg(MSG::INFO) << endmsg;

        if (!m_cleaner.empty()) {
            ATH_CHECK(m_cleaner.retrieve());
            ATH_MSG_INFO("Retrieved tool " << m_cleaner);
        }

        if (!m_muonHoleRecovery.empty()) {
            ATH_CHECK(m_muonHoleRecovery.retrieve());
            ATH_MSG_INFO("Retrieved tool " << m_muonHoleRecovery);
        }

        if (!m_muonErrorOptimizer.empty()) {
            ATH_CHECK(m_muonErrorOptimizer.retrieve());
            ATH_MSG_INFO("Retrieved tool " << m_muonErrorOptimizer);
        }

        if (!m_fitter.empty()) {
            ATH_CHECK(m_fitter.retrieve());
            ATH_MSG_INFO("Retrieved tool " << m_fitter);
        }

        if (!m_trackSummary.empty()) {
            ATH_CHECK(m_trackSummary.retrieve());
            ATH_MSG_INFO("Retrieved tool " << m_trackSummary);
        }

        ATH_CHECK(m_trackingVolumesSvc.retrieve());
        ATH_MSG_DEBUG("Retrieved Svc " << m_trackingVolumesSvc);
        m_calorimeterVolume =
            std::make_unique<const Trk::Volume>(m_trackingVolumesSvc->volume(Trk::ITrackingVolumesSvc::MuonSpectrometerEntryLayer));
        m_indetVolume = std::make_unique<const Trk::Volume>(m_trackingVolumesSvc->volume(Trk::ITrackingVolumesSvc::CalorimeterEntryLayer));
        return StatusCode::SUCCESS;
    }

    /** ICombinedMuonTrackBuilder interface: build and fit combined ID/Calo/MS track */

    std::unique_ptr<Trk::Track> OutwardsCombinedMuonTrackBuilder::combinedFit(const Trk::Track& indetTrack,
                                                                              const Trk::Track& /*extrapolatedTrack*/,
                                                                              const Trk::Track& spectrometerTrack,
                                                                              const EventContext& ctx) const {
        ATH_MSG_VERBOSE("combinedFit:: ");

        std::unique_ptr<Trk::Track> combinedTrack = fit(indetTrack, spectrometerTrack, ctx, m_cleanCombined, Trk::muon);

        // add the track summary

        if (combinedTrack) m_trackSummary->updateTrack(*combinedTrack);

        return combinedTrack;
    }

    std::unique_ptr<Trk::Track> OutwardsCombinedMuonTrackBuilder::indetExtension(const Trk::Track& indetTrack,
                                                                                 const Trk::MeasurementSet& spectrometerMeas,
                                                                                 const EventContext& ctx,
                                                                                 const Trk::TrackParameters* /*innerParameters*/,
                                                                                 const Trk::TrackParameters* /*middleParameters*/,
                                                                                 const Trk::TrackParameters* /*outerParameters*/) const {
        ATH_MSG_VERBOSE("indetExtension fit::");

        auto trajectory = DataVector<const Trk::TrackStateOnSurface>();

        for (int i = 0; i < (int)spectrometerMeas.size(); i++) {
            std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typeM;
            typeM.set(Trk::TrackStateOnSurface::Measurement);

            Trk::TrackStateOnSurface* tsos = new Trk::TrackStateOnSurface(spectrometerMeas[i]->uniqueClone(), nullptr, nullptr, nullptr, typeM);
            trajectory.push_back(tsos);
        }

        Trk::TrackInfo trackInfo(Trk::TrackInfo::Unknown, Trk::muon);
        Trk::Track inputtrack2(trackInfo, std::move(trajectory), nullptr);
        std::unique_ptr<Trk::Track> track{fit(indetTrack, inputtrack2, ctx, m_cleanCombined, Trk::muon)};

        return track;
    }

    std::unique_ptr<Trk::Track> OutwardsCombinedMuonTrackBuilder::standaloneFit(const Trk::Track& /*spectrometerTrack*/,
                                                                                const EventContext&, const Trk::Vertex* /*vertex*/,
                                                                                float /*bs_x*/, float /*bs_y*/, float /*bs_z*/) const {
        return nullptr;
    }

    std::unique_ptr<Trk::Track> OutwardsCombinedMuonTrackBuilder::standaloneRefit(const Trk::Track& combinedTrack, const EventContext& ctx,
                                                                                  float bs_x, float bs_y, float bs_z) const {
        ATH_MSG_DEBUG(" start OutwardsCombinedMuonTrackBuilder standaloneRefit");

        ATH_MSG_DEBUG(" beam position bs_x " << bs_x << " bs_y " << bs_y << " bs_z " << bs_z);

        double vertex3DSigmaRPhi = 6.0;
        double vertex3DSigmaZ = 60.0;

        auto trackStateOnSurfaces = DataVector<const Trk::TrackStateOnSurface>();

        bool addVertexRegion = true;
        Amg::Vector3D origin(bs_x, bs_y, bs_z);

        AmgSymMatrix(3) vertexRegionCovariance;
        vertexRegionCovariance.setZero();
        vertexRegionCovariance(0, 0) = vertex3DSigmaRPhi * vertex3DSigmaRPhi;
        vertexRegionCovariance(1, 1) = vertex3DSigmaRPhi * vertex3DSigmaRPhi;
        vertexRegionCovariance(2, 2) = vertex3DSigmaZ * vertex3DSigmaZ;

        Trk::RecVertex vertex(origin, vertexRegionCovariance);

        int itsos = 0;
        DataVector<const Trk::TrackStateOnSurface>::const_iterator t = combinedTrack.trackStateOnSurfaces()->begin();

        // create perigee TSOS
        for (; t != combinedTrack.trackStateOnSurfaces()->end(); ++t) {
            itsos++;

            if ((**t).alignmentEffectsOnTrack()) continue;

            if ((**t).type(Trk::TrackStateOnSurface::Perigee)) {
                const Trk::TrackParameters* pars = (**t).trackParameters();

                if (pars) {
                    const Trk::TrackStateOnSurface* TSOS = (**t).clone();
                    trackStateOnSurfaces.push_back(TSOS);

                    // including vertex region pseudoMeas
                    if (addVertexRegion) {
                        auto vertexInFit = vertexOnTrack(pars, vertex);

                        if (vertexInFit) {
                            std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type;
                            type.set(Trk::TrackStateOnSurface::Measurement);
                            trackStateOnSurfaces.push_back(
                                new const Trk::TrackStateOnSurface(std::move(vertexInFit), nullptr, nullptr, nullptr, type));
                            ATH_MSG_DEBUG(" found Perigee and added vertex " << itsos);
                        }
                    }
                    break;
                }
            }
        }

        //
        // add ID Eloss
        //
        double Eloss = 0.;
        for (; t != combinedTrack.trackStateOnSurfaces()->end(); ++t) {
            itsos++;
            if ((**t).alignmentEffectsOnTrack()) continue;
            if (!(**t).trackParameters()) continue;

            if ((**t).materialEffectsOnTrack()) {
                if (!m_indetVolume->inside((**t).trackParameters()->position())) break;

                double X0 = (**t).materialEffectsOnTrack()->thicknessInX0();

                const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>((**t).materialEffectsOnTrack());

                if (meot) {
                    const Trk::EnergyLoss* energyLoss = meot->energyLoss();
                    if (energyLoss) {
                        Eloss += std::abs(energyLoss->deltaE());

                        ATH_MSG_DEBUG("OutwardsCombinedMuonFit ID Eloss found r " << ((**t).trackParameters())->position().perp() << " z "
                                                                                  << ((**t).trackParameters())->position().z() << " value "
                                                                                  << energyLoss->deltaE() << " Eloss " << Eloss);

                        const Trk::ScatteringAngles* scat = meot->scatteringAngles();

                        if (scat) {
                            double sigmaDeltaPhi = scat->sigmaDeltaPhi();
                            double sigmaDeltaTheta = scat->sigmaDeltaTheta();

                            const Trk::EnergyLoss* energyLossNew = new Trk::EnergyLoss(
                                energyLoss->deltaE(), energyLoss->sigmaDeltaE(), energyLoss->sigmaDeltaE(), energyLoss->sigmaDeltaE());

                            auto scatNew = Trk::ScatteringAngles(0., 0., sigmaDeltaPhi, sigmaDeltaTheta);

                            const Trk::Surface& surfNew = (**t).trackParameters()->associatedSurface();

                            std::bitset<Trk::MaterialEffectsBase::NumberOfMaterialEffectsTypes> meotPattern(0);
                            meotPattern.set(Trk::MaterialEffectsBase::EnergyLossEffects);
                            meotPattern.set(Trk::MaterialEffectsBase::ScatteringEffects);

                            auto meotNew =
                              std::make_unique<Trk::MaterialEffectsOnTrack>(X0,
                                                              std::move(scatNew),
                                                              energyLossNew,
                                                              surfNew,
                                                              meotPattern);


                            auto parsNew = ((**t).trackParameters())->uniqueClone();

                            std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePatternScat(0);
                            typePatternScat.set(Trk::TrackStateOnSurface::Scatterer);

                            const Trk::TrackStateOnSurface* newTSOS =
                                new Trk::TrackStateOnSurface(nullptr, std::move(parsNew), nullptr, std::move(meotNew), typePatternScat);

                            trackStateOnSurfaces.push_back(newTSOS);
                        }
                    }
                }
            }
        }

        ATH_MSG_DEBUG("OutwardsCombinedMuonFit Total ID Eloss " << Eloss << " nr of states " << itsos);

        // add Calo and MS TSOSs

        for (; t != combinedTrack.trackStateOnSurfaces()->end(); ++t) {
            itsos++;

            if ((**t).alignmentEffectsOnTrack()) continue;

            const Trk::TrackStateOnSurface* TSOS = (**t).clone();
            trackStateOnSurfaces.push_back(TSOS);
        }

        ATH_MSG_DEBUG(" trackStateOnSurfaces found " << trackStateOnSurfaces.size() << " from total " << itsos);

        std::unique_ptr<Trk::Track> standaloneTrack =
            std::make_unique<Trk::Track>(combinedTrack.info(), std::move(trackStateOnSurfaces), nullptr);
        standaloneTrack->info().setPatternRecognitionInfo(Trk::TrackInfo::MuidStandaloneRefit);

        std::unique_ptr<Trk::Track> refittedTrack = fit(*standaloneTrack, ctx, false, Trk::muon);

        if (!refittedTrack) {
            ATH_MSG_DEBUG(" OutwardsCombinedMuonTrackBuilder standaloneRefit FAILED ");
            return nullptr;
        }

        ATH_MSG_DEBUG(" OutwardsCombinedMuonTrackBuilder standaloneRefit OK ");

        m_trackSummary->updateTrack(*refittedTrack);
        return refittedTrack;
    }

    /** refit a track */
    std::unique_ptr<Trk::Track> OutwardsCombinedMuonTrackBuilder::fit(Trk::Track& track, const EventContext& ctx,
                                                                      const Trk::RunOutlierRemoval runOutlier,
                                                                      const Trk::ParticleHypothesis particleHypothesis) const {
        // check valid particleHypothesis
        if (particleHypothesis != Trk::muon && particleHypothesis != Trk::nonInteracting) {
            ATH_MSG_WARNING(" invalid particle hypothesis " << particleHypothesis
                                                            << " requested. Must be 0 or 2 (nonInteracting or muon) ");
            return nullptr;
        }

        // fit
        std::unique_ptr<Trk::Track> fittedTrack{m_fitter->fit(ctx, track, false, particleHypothesis)};
        if (!fittedTrack) return nullptr;

        // track cleaning
        if (runOutlier) {
            // fit with optimized spectrometer errors
            if (!m_muonErrorOptimizer.empty() && !fittedTrack->info().trackProperties(Trk::TrackInfo::StraightTrack)) {
                ATH_MSG_VERBOSE(" perform spectrometer error optimization before cleaning ");

                std::unique_ptr<Trk::Track> optimizedTrack = m_muonErrorOptimizer->optimiseErrors(*fittedTrack, ctx);
                if (optimizedTrack) { fittedTrack.swap(optimizedTrack); }
            }

            // muon cleaner
            ATH_MSG_VERBOSE(" perform track cleaning... ");

            std::unique_ptr<Trk::Track> cleanTrack = m_cleaner->clean(*fittedTrack, ctx);
            if (!cleanTrack) {
                ATH_MSG_DEBUG(" cleaner veto ");

                if (m_allowCleanerVeto) { fittedTrack.reset(); }
            } else if (!(*cleanTrack->perigeeParameters() == *fittedTrack->perigeeParameters())) {
                ATH_MSG_VERBOSE(" found and removed spectrometer outlier(s) ");
                // this will probably never be fixed as the outwards combined
                // builder is deprecated
                fittedTrack.swap(cleanTrack);
            }

            // FIXME: provide indet cleaner
            ATH_MSG_VERBOSE(" finished cleaning");
        }

        return fittedTrack;
    }

    std::unique_ptr<Trk::Track> OutwardsCombinedMuonTrackBuilder::fit(const Trk::Track& indetTrack, const Trk::Track& extrapolatedTrack,
                                                                      const EventContext& ctx, const Trk::RunOutlierRemoval runOutlier,
                                                                      const Trk::ParticleHypothesis particleHypothesis) const {
        // check valid particleHypothesis
        if (particleHypothesis != Trk::muon && particleHypothesis != Trk::nonInteracting) {
            ATH_MSG_WARNING(" invalid particle hypothesis " << particleHypothesis
                                                            << " requested. Must be 0 or 2 (nonInteracting or muon) ");
            return nullptr;
        }

        // fit
        std::unique_ptr<Trk::Track> fittedTrack{m_fitter->fit(ctx, indetTrack, extrapolatedTrack, false, particleHypothesis)};

        if (!fittedTrack) { return nullptr; }

        if (!fittedTrack->perigeeParameters()) {
            ATH_MSG_WARNING(" Fitter returned a track without perigee, failing fit");
            return nullptr;
        }

        // track cleaning
        if (runOutlier) {
            // fit with optimized spectrometer errors
            if (!m_muonErrorOptimizer.empty() && !fittedTrack->info().trackProperties(Trk::TrackInfo::StraightTrack)) {
                ATH_MSG_VERBOSE(" perform spectrometer error optimization before cleaning ");
                std::unique_ptr<Trk::Track> optimizedTrack{m_muonErrorOptimizer->optimiseErrors(*fittedTrack, ctx)};
                if (optimizedTrack) {
                    // until code is updated to use unique_ptr or removed
                    fittedTrack.swap(optimizedTrack);
                }
            }
            // muon cleaner
            ATH_MSG_VERBOSE(" perform track cleaning... ");
            std::unique_ptr<Trk::Track> cleanTrack = m_cleaner->clean(*fittedTrack, ctx);
            if (!cleanTrack) {
                ATH_MSG_DEBUG(" cleaner veto ");
                if (m_allowCleanerVeto) { fittedTrack.reset(); }
            } else if (!(*cleanTrack->perigeeParameters() == *fittedTrack->perigeeParameters())) {
                ATH_MSG_VERBOSE("  found and removed spectrometer outlier(s) ");
                // this will probably never be fixed as the outwards builder is
                // deprecated
                fittedTrack.swap(cleanTrack);
            }

            ATH_MSG_VERBOSE(" finished cleaning");
        }

        if (fittedTrack) {
            std::unique_ptr<Trk::Track> newTrack{addIDMSerrors(fittedTrack.get())};
            if (newTrack) {
                std::unique_ptr<Trk::Track> refittedTrack{fit(*newTrack, ctx, false, Trk::muon)};
                /// Both hold the same object. Release the newTrack from its duty.
                if (newTrack == fittedTrack) { newTrack.release(); }

                if (refittedTrack) {
                    if (refittedTrack->fitQuality()) { fittedTrack.swap(refittedTrack); }
                }
            }
        }

        if (m_recoverCombined && fittedTrack) {
            std::unique_ptr<Trk::Track> recoveredTrack{m_muonHoleRecovery->recover(*fittedTrack, ctx)};
            double oldfitqual = fittedTrack->fitQuality()->chiSquared() / fittedTrack->fitQuality()->numberDoF();

            if (recoveredTrack && recoveredTrack != fittedTrack) {
                double newfitqual = recoveredTrack->fitQuality()->chiSquared() / recoveredTrack->fitQuality()->numberDoF();
                if (newfitqual < oldfitqual || newfitqual < 1.5 || (newfitqual < 2 && newfitqual < oldfitqual + .5)) {
                    fittedTrack.swap(recoveredTrack);
                }
            }
        }

        if (fittedTrack && !fittedTrack->perigeeParameters()) {
            ATH_MSG_WARNING(" Fitter returned a track without perigee, failing fit");
            return nullptr;
        }

        if (runOutlier && fittedTrack) {
            double fitqual = fittedTrack->fitQuality()->chiSquared() / fittedTrack->fitQuality()->numberDoF();

            if (fitqual > 5 || (fittedTrack->perigeeParameters()->pT() < 20000 && fitqual > 2.5)) {
                fittedTrack.reset();
            } else {
                DataVector<const Trk::TrackStateOnSurface>::const_iterator itStates = fittedTrack->trackStateOnSurfaces()->begin();

                DataVector<const Trk::TrackStateOnSurface>::const_iterator endStates = fittedTrack->trackStateOnSurfaces()->end();

                for (; itStates != endStates; itStates++) {
                    if ((*itStates)->materialEffectsOnTrack()) {
                        const Trk::MaterialEffectsOnTrack* meot =
                            dynamic_cast<const Trk::MaterialEffectsOnTrack*>((*itStates)->materialEffectsOnTrack());

                        if (!meot) continue;

                        if (meot->scatteringAngles() && !meot->energyLoss()) {
                            double pullphi = std::abs(meot->scatteringAngles()->deltaPhi() / meot->scatteringAngles()->sigmaDeltaPhi());

                            double pulltheta =
                                std::abs(meot->scatteringAngles()->deltaTheta() / meot->scatteringAngles()->sigmaDeltaTheta());

                            if (pullphi > 7 || pulltheta > 7) {
                                fittedTrack.reset();
                                break;
                            }
                        }
                    }
                }
            }
        }

        // final fit with optimized spectrometer errors
        if (!m_muonErrorOptimizer.empty() && fittedTrack && !fittedTrack->info().trackProperties(Trk::TrackInfo::StraightTrack)) {
            ATH_MSG_VERBOSE(" perform spectrometer error optimization... ");

            std::unique_ptr<Trk::Track> optimizedTrack{m_muonErrorOptimizer->optimiseErrors(*fittedTrack, ctx)};
            if (optimizedTrack) {
                // until the code uses unique ptrs (or is removed since it's deprecated)
                fittedTrack.swap(optimizedTrack);
            }
        }
        return fittedTrack;
    }

    Trk::Track* OutwardsCombinedMuonTrackBuilder::addIDMSerrors(Trk::Track* track) const {
        //
        // take track and correct the two scattering planes in the Calorimeter
        // to take into account m_IDMS_rzSigma and m_IDMS_xySigma
        //
        // returns a new Track
        //
        if (!m_addIDMSerrors) return track;

        ATH_MSG_VERBOSE(" OutwardsCombinedMuonTrackBuilder addIDMSerrors to track ");

        Amg::Vector3D positionMS(0, 0, 0);
        Amg::Vector3D positionCaloFirst(0, 0, 0);
        Amg::Vector3D positionCaloLast(0, 0, 0);

        int itsos = 0;
        int itsosCaloFirst = -1;
        int itsosCaloLast = -1;
        double p = -1.;

        DataVector<const Trk::TrackStateOnSurface>::const_iterator t = track->trackStateOnSurfaces()->begin();
        for (; t != track->trackStateOnSurfaces()->end(); ++t) {
            itsos++;

            if ((**t).trackParameters()) {
                if (p == -1.) { p = (**t).trackParameters()->momentum().mag() / Gaudi::Units::GeV; }
                if (m_indetVolume->inside((**t).trackParameters()->position())) { continue; }

                if ((**t).trackParameters()->position().mag() < 1000) { continue; }

                if (m_calorimeterVolume->inside((**t).trackParameters()->position())) {
                    // first scattering plane in Calorimeter
                    if ((**t).type(Trk::TrackStateOnSurface::Scatterer) && (**t).materialEffectsOnTrack()) {
                        double X0 = (**t).materialEffectsOnTrack()->thicknessInX0();

                        if (X0 < 10) continue;

                        if (itsosCaloFirst != -1) {
                            itsosCaloLast = itsos;
                            positionCaloLast = (**t).trackParameters()->position();
                        }

                        if (itsosCaloFirst == -1) {
                            itsosCaloFirst = itsos;
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

        ATH_MSG_DEBUG(" OutwardsCombinedMuonTrackBuilder addIDMSerrors p muon GeV "
                      << p << " First Calorimeter Scatterer radius " << positionCaloFirst.perp() << " z " << positionCaloFirst.z()
                      << " Second Scatterer r " << positionCaloLast.perp() << " z " << positionCaloLast.z() << " First Muon hit r "
                      << positionMS.perp() << " z " << positionMS.z());

        if (itsosCaloFirst < 0 || itsosCaloLast < 0) {
            ATH_MSG_DEBUG(" addIDMSerrors keep original track ");
            return track;
        }

        // If no Calorimeter no IDMS uncertainties have to be propagated
        positionCaloFirst = positionCaloFirst - positionMS;
        positionCaloLast = positionCaloLast - positionMS;

        double sigmaDeltaPhiIDMS2 = m_IDMS_xySigma / (positionCaloFirst.perp() + positionCaloLast.perp());
        double sigmaDeltaThetaIDMS2 = m_IDMS_rzSigma / (positionCaloFirst.mag() + positionCaloLast.mag());

        sigmaDeltaPhiIDMS2 *= sigmaDeltaPhiIDMS2;
        sigmaDeltaThetaIDMS2 *= sigmaDeltaThetaIDMS2;

        auto trackStateOnSurfaces = DataVector<const Trk::TrackStateOnSurface>();
        trackStateOnSurfaces.reserve(track->trackStateOnSurfaces()->size());

        t = track->trackStateOnSurfaces()->begin();
        itsos = 0;

        for (; t != track->trackStateOnSurfaces()->end(); ++t) {
            itsos++;

            if ((**t).alignmentEffectsOnTrack()) { continue; }

            if (itsos == itsosCaloFirst || itsos == itsosCaloLast) {
                if ((**t).materialEffectsOnTrack()) {
                    double X0 = (**t).materialEffectsOnTrack()->thicknessInX0();

                    const Trk::MaterialEffectsOnTrack* meot =
                        dynamic_cast<const Trk::MaterialEffectsOnTrack*>((**t).materialEffectsOnTrack());

                    if (meot) {
                        const Trk::ScatteringAngles* scat = meot->scatteringAngles();

                        if (scat) {
                            double sigmaDeltaPhi = std::sqrt((scat->sigmaDeltaPhi()) * (scat->sigmaDeltaPhi()) + sigmaDeltaPhiIDMS2);

                            double sigmaDeltaTheta =
                                std::sqrt((scat->sigmaDeltaTheta()) * (scat->sigmaDeltaTheta()) + sigmaDeltaThetaIDMS2);

                            const Trk::EnergyLoss* energyLossNew = new Trk::EnergyLoss(0., 0., 0., 0.);

                            auto scatNew = Trk::ScatteringAngles(0., 0., sigmaDeltaPhi, sigmaDeltaTheta);

                            const Trk::Surface& surfNew = (**t).trackParameters()->associatedSurface();

                            std::bitset<Trk::MaterialEffectsBase::NumberOfMaterialEffectsTypes> meotPattern(0);
                            meotPattern.set(Trk::MaterialEffectsBase::EnergyLossEffects);
                            meotPattern.set(Trk::MaterialEffectsBase::ScatteringEffects);

                            auto meotNew =
                              std::make_unique<Trk::MaterialEffectsOnTrack>(X0,
                                                              std::move(scatNew),
                                                              energyLossNew,
                                                              surfNew,
                                                              meotPattern);

                            auto parsNew = ((**t).trackParameters())->uniqueClone();

                            std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePatternScat(0);
                            typePatternScat.set(Trk::TrackStateOnSurface::Scatterer);

                            const Trk::TrackStateOnSurface* newTSOS =
                                new Trk::TrackStateOnSurface(nullptr, std::move(parsNew), nullptr, std::move(meotNew), typePatternScat);

                            trackStateOnSurfaces.push_back(newTSOS);

                            ATH_MSG_DEBUG(" old Calo scatterer had sigmaDeltaPhi mrad      "
                                          << scat->sigmaDeltaPhi() * 1000 << " sigmaDeltaTheta mrad " << scat->sigmaDeltaTheta() * 1000
                                          << " X0 " << X0);

                            ATH_MSG_DEBUG(" new Calo scatterer made with sigmaDeltaPhi mrad "
                                          << sigmaDeltaPhi * 1000 << " sigmaDeltaTheta mrad " << sigmaDeltaTheta * 1000);
                        } else {
                            ATH_MSG_WARNING(
                                " This should not happen: no Scattering Angles for "
                                "scatterer ");
                        }
                    } else {
                        ATH_MSG_WARNING(
                            " This should not happen: no MaterialEffectsOnTrack "
                            "for "
                            "scatterer ");
                    }
                }
            } else {
                const Trk::TrackStateOnSurface* TSOS = (**t).clone();
                trackStateOnSurfaces.push_back(TSOS);
            }
        }
        ATH_MSG_DEBUG(" trackStateOnSurfaces on input track " << track->trackStateOnSurfaces()->size() << " trackStateOnSurfaces found "
                                                              << trackStateOnSurfaces.size());

        Trk::Track* newTrack = new Trk::Track(track->info(), std::move(trackStateOnSurfaces), nullptr);
        return newTrack;
    }

    std::unique_ptr<Trk::PseudoMeasurementOnTrack>
    OutwardsCombinedMuonTrackBuilder::vertexOnTrack(const Trk::TrackParameters* parameters,
                                                    const Trk::RecVertex& vertex) {
        // create the corresponding PerigeeSurface, localParameters and
        // covarianceMatrix
        const Trk::PerigeeSurface surface(vertex.position());

        Trk::LocalParameters localParameters;

        Amg::MatrixX covarianceMatrix;
        covarianceMatrix.setZero();

        // transform Cartesian (x,y,z) to beam axis or perigee
        Amg::Vector2D localPosition(0, 0);

        double ptInv = 1. / parameters->momentum().perp();

        localParameters = Trk::LocalParameters(localPosition);

        Amg::MatrixX jacobian(2, 3);
        jacobian.setZero();
        jacobian(0, 0) = -ptInv * parameters->momentum().y();
        jacobian(0, 1) = ptInv * parameters->momentum().x();
        jacobian(1, 2) = 1.0;

        const Amg::MatrixX& cov = vertex.covariancePosition();
        covarianceMatrix = cov.similarity(jacobian);

        return std::make_unique<Trk::PseudoMeasurementOnTrack>(localParameters, covarianceMatrix, surface);
    }
}  // namespace Rec

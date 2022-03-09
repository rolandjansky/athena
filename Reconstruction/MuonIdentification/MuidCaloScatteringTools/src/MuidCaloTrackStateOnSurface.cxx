/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuidCaloTrackStateOnSurface
//  AlgTool to allocate 3 scattering centres as TrackStateOnSurface objects
//  to represent the Coulomb scattering and energy deposit between the InDet
//  and MuonSpectrometer entrance.
//
//  This tool retrieves the appropriate parametrized scattering centres and
//  hybrid (measured + parametrized) energy deposit evaluation
//  using the IMuidCaloMaterialParam and IMuidCaloEnergy interfaces
//
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuidCaloTrackStateOnSurface.h"

#include <cmath>
#include <iomanip>

#include "FourMomUtils/xAODP4Helpers.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkTrack/TrackStateOnSurface.h"
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

namespace Rec {

    MuidCaloTrackStateOnSurface::MuidCaloTrackStateOnSurface(const std::string& type, const std::string& name, const IInterface* parent) :
        AthAlgTool(type, name, parent),
        m_minCaloRadius(0.4 * Gaudi::Units::meter),
        m_minRemainingEnergy(0.5 * Gaudi::Units::GeV),
        m_paramPtCut(15.0 * Gaudi::Units::GeV) {
        declareInterface<IMuidCaloTrackStateOnSurface>(this);
        declareProperty("MinCaloRadius", m_minCaloRadius);
        declareProperty("MinRemainingEnergy", m_minRemainingEnergy);
        declareProperty("ParamPtCut", m_paramPtCut);
    }

    //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

    StatusCode MuidCaloTrackStateOnSurface::initialize() {
        ATH_MSG_DEBUG("Initializing CombinedMuonTrackStateOnSurface");

        // get the Tools
        ATH_CHECK(m_caloEnergyDeposit.retrieve());
        ATH_MSG_DEBUG("Retrieved tool " << m_caloEnergyDeposit);

        ATH_CHECK(m_caloEnergyParam.retrieve());
        ATH_MSG_DEBUG("Retrieved tool " << m_caloEnergyParam);

        ATH_CHECK(m_caloMaterialParam.retrieve());
        ATH_MSG_DEBUG("Retrieved tool " << m_caloMaterialParam);

        /// handle to the magnetic field cache
        ATH_CHECK(m_fieldCacheCondObjInputKey.initialize());

        ATH_CHECK(m_propagator.retrieve());
        ATH_MSG_DEBUG("Retrieved tool " << m_propagator);

        return StatusCode::SUCCESS;
    }

    StatusCode MuidCaloTrackStateOnSurface::finalize() {
        ATH_MSG_INFO("finalized with " << m_count << " muons asking for calo association, out of which: " << endmsg << "     "
                                       << m_countInnerFailure << " failed to find the inner calo scattering plane," << endmsg << "     "
                                       << m_countOuterFailure << " failed to find the outer calo scattering plane," << endmsg
                                       << "     while another " << m_countCompleteFailure
                                       << " completely failed to intersect the calorimeter." << endmsg << "     "
                                       << m_countArbitrarySolution << " with oscillation resolved by taking an arbitrary solution");

        return StatusCode::SUCCESS;
    }

    std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> MuidCaloTrackStateOnSurface::caloTSOS(
        const EventContext& ctx, const Trk::TrackParameters& parameters) const {
        std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> caloTSOS;
        caloTSOS.reserve(3);

        std::unique_ptr<Trk::TrackStateOnSurface> innerTS, middleTS, outerTS;
        std::unique_ptr<const Trk::TrackParameters> inParams, midParams;
        const Trk::TrackParameters* innerParams = nullptr;
        const Trk::TrackParameters* middleParams = nullptr;
        const Trk::TrackParameters* outerParams = nullptr;

        MagField::AtlasFieldCache fieldCache;
        // Get field cache object
        SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, ctx};
        const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};

        if (!fieldCondObj) {
            ATH_MSG_ERROR("Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCacheCondObjInputKey.key());
            return caloTSOS;
        }
        fieldCondObj->getInitializedCache(fieldCache);

        // track to calo surfaces - first decide in or outwards
        bool trackOutwards = true;
        if (dynamic_cast<const Trk::PerigeeSurface*>(&parameters.associatedSurface())) {
            if (parameters.associatedSurface().center().perp() > 0.5 * Gaudi::Units::meter) trackOutwards = false;
        } else if (std::abs(parameters.position().z()) > 4.0 * Gaudi::Units::meter ||
                   parameters.position().perp() > 1.2 * Gaudi::Units::meter) {
            trackOutwards = false;
        }

        if (trackOutwards)  // forward tracking
        {
            innerTS = innerTSOS(ctx, parameters);
            if (innerTS) {
                innerParams = innerTS->trackParameters();
                if (innerParams->momentum().dot(innerParams->position()) > 0.) {
                    midParams = getExtrapolatedParameters(ctx, *innerParams, SurfaceLayer::Middle);
                    middleParams = midParams.get();
                } else {
                    ATH_MSG_VERBOSE(" quit as looper " << innerParams);
                }
                if (middleParams) {
                    // get calo energy deposit
                    middleTS = m_caloEnergyParam->trackStateOnSurface(ctx, *midParams, innerParams, outerParams);
                    if (!middleTS) { middleParams = nullptr; }
                }

                if (middleParams) {
                    // apply energy deposit to give outgoing middleParams,
                    // note the iteration required to get optimum energy correction
                    std::unique_ptr<const Trk::TrackParameters> params;
                    for (int iterate = 0; iterate != 2; ++iterate) {
                        if (!middleTS) break;

                        const Trk::MaterialEffectsOnTrack* meot =
                            dynamic_cast<const Trk::MaterialEffectsOnTrack*>(middleTS->materialEffectsOnTrack());
                        const double energyDeposit = meot ? meot->energyLoss()->deltaE() : 0.;
                        middleParams = middleTS->trackParameters();
                        Amg::Vector3D momentum = middleParams->momentum();
                        double correctedEnergy = innerParams->momentum().mag() - energyDeposit;

                        // fail potential loopers
                        if (fieldCache.toroidOn() && correctedEnergy < m_minRemainingEnergy) {
                            middleTS.reset();
                            ATH_MSG_VERBOSE(" fail tracking outwards: momentum " << momentum.mag() / Gaudi::Units::GeV
                                                                                 << "   energyDeposit " << energyDeposit / Gaudi::Units::GeV
                                                                                 << "   correctedEnergy "
                                                                                 << correctedEnergy / Gaudi::Units::GeV << " (GeV units)");
                            break;
                        }
                        // toroid off: set floor for remaining energy
                        if (correctedEnergy < m_minRemainingEnergy) {
                            momentum *= m_minRemainingEnergy / momentum.mag();
                        } else {
                            momentum *= correctedEnergy / momentum.mag();
                        }

                        const Trk::CylinderSurface* cylinder =
                            dynamic_cast<const Trk::CylinderSurface*>(&middleParams->associatedSurface());
                        if (cylinder) {
                            params =
                                std::make_unique<Trk::AtaCylinder>(middleParams->position(), momentum, middleParams->charge(), *cylinder);
                        } else {
                            const Trk::DiscSurface* disc = dynamic_cast<const Trk::DiscSurface*>(&middleParams->associatedSurface());
                            if (disc) {
                                params = std::make_unique<Trk::AtaDisc>(middleParams->position(), momentum, middleParams->charge(), *disc);
                            } else {
                                ATH_MSG_WARNING(" caloTSOS: unexpected TrackParameters type ");
                            }
                        }

                        // delete middle and outer,
                        // then recompute with parameters as for backwards tracking
                        if (!params) {
                            middleTS.reset();
                        } else if (params->momentum().perp() > m_paramPtCut) {
                            middleTS = m_caloEnergyDeposit->trackStateOnSurface(ctx, *params, innerParams, outerParams);
                        } else {
                            middleTS = m_caloEnergyParam->trackStateOnSurface(ctx, *params, innerParams, outerParams);
                        }
                        ////
                        if (middleTS) { outerTS = outerTSOS(ctx, *params); }
                    }
                    if (!middleTS) { ATH_MSG_VERBOSE(" fail tracking outwards: no intersect at middle surface"); }
                }
            } else {
                ATH_MSG_VERBOSE(" fail tracking ouwards: no intersect at inner surface");
            }
        } else  // backward tracking
        {
            outerTS = outerTSOS(ctx, parameters);
            if (outerTS) {
                outerParams = outerTS->trackParameters();
                midParams = getExtrapolatedParameters(ctx, *outerParams, SurfaceLayer::Middle);
                middleParams = midParams.get();
                if (middleParams) {
                    inParams = getExtrapolatedParameters(ctx, *midParams, SurfaceLayer::Inner);
                    innerParams = inParams.get();
                    if (inParams) {
                        // get calo energy deposit
                        if (middleParams->momentum().perp() > m_paramPtCut) {
                            middleTS = m_caloEnergyDeposit->trackStateOnSurface(ctx, *middleParams, inParams.get(), outerParams);
                        } else {
                            middleTS = m_caloEnergyParam->trackStateOnSurface(ctx, *middleParams, inParams.get(), outerParams);
                        }
                        // apply energy deposit
                        const Trk::MaterialEffectsOnTrack* meot =
                            middleTS ? dynamic_cast<const Trk::MaterialEffectsOnTrack*>(middleTS->materialEffectsOnTrack()) : nullptr;
                        const double energyDeposit = meot ? meot->energyLoss()->deltaE() : 0.;
                        Amg::Vector3D momentum = middleParams->momentum();
                        double correctedEnergy = momentum.mag() + energyDeposit;

                        // fail potential loopers
                        if (correctedEnergy < 0.5 * Gaudi::Units::GeV) {
                            middleTS.reset();
                            ATH_MSG_VERBOSE(" fail tracking inwards: momentum " << momentum.mag() / Gaudi::Units::GeV << "   energyDeposit "
                                                                                << energyDeposit / Gaudi::Units::GeV
                                                                                << "   correctedEnergy "
                                                                                << correctedEnergy / Gaudi::Units::GeV << " (GeV units)");
                        } else {
                            momentum *= 1. + energyDeposit / momentum.mag();
                            std::unique_ptr<const Trk::TrackParameters> params;
                            const Trk::CylinderSurface* cylinder =
                                dynamic_cast<const Trk::CylinderSurface*>(&middleParams->associatedSurface());
                            std::optional<AmgSymMatrix(5)> cov =
                                middleParams->covariance() ? std::optional<AmgSymMatrix(5)>(*middleParams->covariance()) : std::nullopt;
                            if (cylinder) {
                                params = std::make_unique<Trk::AtaCylinder>(middleParams->position(), momentum, middleParams->charge(),
                                                                            *cylinder, cov);
                            } else {
                                const Trk::DiscSurface* disc = dynamic_cast<const Trk::DiscSurface*>(&middleParams->associatedSurface());
                                if (disc) {
                                    params = std::make_unique<Trk::AtaDisc>(middleParams->position(), momentum, middleParams->charge(),
                                                                            *disc, cov);
                                } else {
                                    ATH_MSG_WARNING(" caloTSOS: unexpected TrackParameters type ");
                                }
                            }
                            if (params && middleTS) { innerTS = innerTSOS(ctx, *params); }
                        }
                    } else {
                        ATH_MSG_VERBOSE(" fail tracking inwards: no intersect at inner surface");
                    }
                } else {
                    ATH_MSG_VERBOSE(" fail tracking inwards: no intersect at middle surface");
                }
            }
        }

        if (innerTS)
            caloTSOS.push_back(std::move(innerTS));
        else
            ++m_countInnerFailure;
        if (middleTS) caloTSOS.push_back(std::move(middleTS));
        if (outerTS)
            caloTSOS.push_back(std::move(outerTS));
        else
            ++m_countOuterFailure;

        // keep some statistics
        ++m_count;

        if (caloTSOS.empty()) { ++m_countCompleteFailure; }
        return caloTSOS;
    }
    std::unique_ptr<Trk::TrackStateOnSurface> MuidCaloTrackStateOnSurface::innerTSOS(const EventContext& ctx,
                                                                                           const Trk::TrackParameters& parameters) const {
        std::unique_ptr<const Trk::TrackParameters> extrapolation = getExtrapolatedParameters(ctx, parameters, SurfaceLayer::Inner);
        if (!extrapolation || extrapolation->position().perp() < m_minCaloRadius) {
            ATH_MSG_DEBUG(" innerTSOS:  extrapolation fails ");
            return nullptr;
        }
        return m_caloMaterialParam->trackStateOnSurface(*extrapolation);
    }
    std::unique_ptr<Trk::TrackStateOnSurface> MuidCaloTrackStateOnSurface::outerTSOS(const EventContext& ctx,
                                                                                           const Trk::TrackParameters& parameters) const {
        std::unique_ptr<const Trk::TrackParameters> extrapolation = getExtrapolatedParameters(ctx, parameters, SurfaceLayer::Outer);
        if (!extrapolation || extrapolation->position().perp() < m_minCaloRadius) {
            ATH_MSG_DEBUG(" outerTSOS:  extrapolation fails ");
            return nullptr;
        }
        return m_caloMaterialParam->trackStateOnSurface(*extrapolation);
    }
    std::unique_ptr<Trk::TrackStateOnSurface> MuidCaloTrackStateOnSurface::middleTSOS(const EventContext& ctx,
                                                                                            const Trk::TrackParameters& middleParams,
                                                                                            const Trk::TrackParameters* innerParams,
                                                                                            const Trk::TrackParameters* outerParams) const {
        std::unique_ptr<const Trk::TrackParameters> extrapolation = getExtrapolatedParameters(ctx, middleParams, SurfaceLayer::Middle);

        if (!extrapolation || extrapolation->position().perp() < m_minCaloRadius) {
            ATH_MSG_DEBUG(" middleTSOS:  extrapolation fails ");
            return nullptr;
        }
        std::unique_ptr<Trk::TrackStateOnSurface> TSOS;
        if (extrapolation->momentum().perp() > m_paramPtCut) {
            TSOS = m_caloEnergyDeposit->trackStateOnSurface(ctx, *extrapolation, innerParams, outerParams);
        } else {
            TSOS = m_caloEnergyParam->trackStateOnSurface(ctx, *extrapolation, innerParams, outerParams);
        }
        return TSOS;
    }

    //<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

    const Trk::Surface* MuidCaloTrackStateOnSurface::getCaloSurface(const double eta, const short layer) const {
        if (layer == SurfaceLayer::Inner) return m_caloMaterialParam->innerSurface(eta);
        if (layer == SurfaceLayer::Middle) return m_caloMaterialParam->middleSurface(eta);
        if (layer == SurfaceLayer::Outer) return m_caloMaterialParam->outerSurface(eta);
        ATH_MSG_WARNING("Invalid layer id given " << layer);
        return nullptr;
    }
    std::unique_ptr<const Trk::TrackParameters> MuidCaloTrackStateOnSurface::getExtrapolatedParameters(
        const EventContext& ctx, const Trk::TrackParameters& parameters, const short layer) const {
        // will need to know whether forward or back tracking
        Trk::PropDirection momentumDirection = Trk::alongMomentum;
        Trk::PropDirection oppositeDirection = Trk::oppositeMomentum;
        Trk::PropDirection propDirection = Trk::anyDirection;
        const std::string surf_layer_str = layer == SurfaceLayer::Inner ? "Inner" : (layer == SurfaceLayer::Middle ? "Middle" : "Outer");
        // initial surface at track eta -
        //   using position or direction according to distance from origin
        double startingPhi = 0.;
        const Trk::Surface* surface = nullptr;
        if (useEtaPhiFromDirection(parameters)) {
            startingPhi = parameters.momentum().phi();
            surface = getCaloSurface(parameters.momentum().eta(), layer);
        } else {
            startingPhi = parameters.position().phi();
            surface = getCaloSurface(parameters.position().eta(), layer);
            if (parameters.momentum().dot(parameters.position()) < 0.) {
                momentumDirection = Trk::oppositeMomentum;
                oppositeDirection = Trk::alongMomentum;
            }
        }
        if (!surface) return nullptr;

        // extrapolate to calo surface (take care to get correct cylinder intersect)
        unsigned extrapolations = 0;
        if (surface->type() == Trk::SurfaceType::Cylinder) {
            if (parameters.position().perp() < surface->globalReferencePoint().perp()) {
                propDirection = momentumDirection;
            } else {
                propDirection = oppositeDirection;
            }
        }
        // tidy up ownership later
        std::unique_ptr<const Trk::TrackParameters> extrapolation{
            m_propagator->propagate(ctx, parameters, *surface, propDirection, false, m_magFieldProperties, Trk::nonInteracting)};
        if (!extrapolation) return nullptr;

        // phi flip means track has crossed beam-axis (so quit)
        double deltaPhi = xAOD::P4Helpers::deltaPhi(extrapolation->position().phi(), startingPhi);
        if (std::abs(deltaPhi) > M_PI_2) { return nullptr; }

        // also quit wrong rz-direction in endcap
        if (surface->type() != Trk::SurfaceType::Cylinder) {
            double signRZ = (extrapolation->position().perp() - parameters.position().perp()) *
                            (extrapolation->position().z() - parameters.position().z());
            if (signRZ * extrapolation->position().z() < 0.) {
                ATH_MSG_VERBOSE(" wrong way in endcap ");
                return nullptr;
            }
        }

        // iterate if extrapolated surface is different
        bool restart = false;
        const Trk::Surface* oldSurface = surface;
        const Trk::Surface* extrapolatedSurface = getCaloSurface(extrapolation->position().eta(), layer);
        /// The TrackParameters own their dedicated instance of the track surface... In cases,
        /// where the loop below resets the extrapolation we need to clone the surface as well
        /// this pointer is just the surface garbage collection
        std::unique_ptr<const Trk::Surface> reset_surface;
        while (++extrapolations < 5 && extrapolatedSurface != oldSurface) {
            // take care to get correct solution for cylinder when starting from inside
            if (surface->type() == Trk::SurfaceType::Cylinder) {
                if (extrapolation->position().perp() < surface->globalReferencePoint().perp()) {
                    propDirection = momentumDirection;
                } else {
                    propDirection = oppositeDirection;
                }
            } else {
                propDirection = Trk::anyDirection;
            }
            std::unique_ptr<const Trk::TrackParameters> oldParameters = std::move(extrapolation);
            extrapolation = m_propagator->propagate(ctx, *oldParameters, *extrapolatedSurface, propDirection, false, m_magFieldProperties,
                                                    Trk::nonInteracting);
            if (!extrapolation) {
                // restart from input parameters (if not already done)
                //   trap no solution after restart
                if ((*oldParameters) == parameters) {
                    ATH_MSG_VERBOSE(" innerParameters:  extrap fails ");
                    return nullptr;
                }
                if (restart) {
                    ATH_MSG_DEBUG(surf_layer_str << " Parameters:  oscillating => arbitrary solution chosen");
                    ++m_countArbitrarySolution;
                    extrapolation = std::move(oldParameters);
                    reset_surface.reset(extrapolation->associatedSurface().clone());
                    extrapolatedSurface = reset_surface.get();
                    surface = extrapolatedSurface;
                    restart = false;
                } else {
                    ATH_MSG_VERBOSE(surf_layer_str << " Parameters:  restart extrap after " << extrapolations << " extrapolations");
                    restart = true;
                    extrapolations -= 2;
                    extrapolation = parameters.uniqueClone();
                    surface = oldSurface;
                }
            } else {
                // update surface
                ATH_MSG_DEBUG(surf_layer_str << " Parameters: Extrapolation succeeded go to next iteration");
                oldSurface = surface;
                surface = extrapolatedSurface;
                extrapolatedSurface = getCaloSurface(extrapolation->position().eta(), layer);
            }
        }

        // final check for phi flip
        deltaPhi = xAOD::P4Helpers::deltaPhi(extrapolation->position().phi(), startingPhi);
        if (std::abs(deltaPhi) > M_PI_2) { return nullptr; }

        ATH_MSG_VERBOSE(surf_layer_str << " Parameters:  success after " << extrapolations << " extrapolation step(s). "
                                       << std::setiosflags(std::ios::fixed) << "  Intersection at: r,phi,z " << std::setw(7)
                                       << std::setprecision(1) << extrapolation->position().perp() << std::setw(7) << std::setprecision(3)
                                       << extrapolation->position().phi() << std::setw(8) << std::setprecision(1)
                                       << extrapolation->position().z() << "  Direction: eta,phi " << std::setw(7) << std::setprecision(3)
                                       << extrapolation->momentum().eta() << std::setw(7) << std::setprecision(3)
                                       << extrapolation->momentum().phi());

        return extrapolation;
    }

    bool MuidCaloTrackStateOnSurface::useEtaPhiFromDirection(const Trk::TrackParameters& parameters) const {
        // estimate eta,phi for the track intersect with the calo from the track direction when the
        // parameters are expressed near the beam-line, further out it's better to use the track position
        double parameterDistance = parameters.position().mag();
        if (dynamic_cast<const Trk::PerigeeSurface*>(&parameters.associatedSurface()))
            parameterDistance = parameters.associatedSurface().center().perp();

        return parameterDistance < 0.5 * Gaudi::Units::meter;
    }

}  // namespace Rec

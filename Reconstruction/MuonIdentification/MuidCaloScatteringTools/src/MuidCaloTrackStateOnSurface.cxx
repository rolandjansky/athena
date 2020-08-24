/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuidCaloScatteringTools/MuidCaloTrackStateOnSurface.h"

#include <cmath>
#include <iomanip>

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

MuidCaloTrackStateOnSurface::MuidCaloTrackStateOnSurface(const std::string& type, const std::string& name,
                                                         const IInterface* parent)
    : AthAlgTool(type, name, parent),
      m_magFieldProperties(0),
      m_minCaloRadius(0.4 * Gaudi::Units::meter),
      m_minRemainingEnergy(0.5 * Gaudi::Units::GeV),
      m_paramPtCut(15.0 * Gaudi::Units::GeV),
      m_count(0),
      m_countArbitrarySolution(0),
      m_countCompleteFailure(0),
      m_countInnerFailure(0),
      m_countOuterFailure(0)
{
    declareInterface<IMuidCaloTrackStateOnSurface>(this);
    declareProperty("MinCaloRadius", m_minCaloRadius);
    declareProperty("MinRemainingEnergy", m_minRemainingEnergy);
    declareProperty("ParamPtCut", m_paramPtCut);
}

MuidCaloTrackStateOnSurface::~MuidCaloTrackStateOnSurface(void) {}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
MuidCaloTrackStateOnSurface::initialize()
{
    ATH_MSG_DEBUG("Initializing CombinedMuonTrackStateOnSurface"
                  << " - package version " << PACKAGE_VERSION);

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

    m_magFieldProperties = new Trk::MagneticFieldProperties(Trk::FullField);

    return StatusCode::SUCCESS;
}

StatusCode
MuidCaloTrackStateOnSurface::finalize()
{
    ATH_MSG_INFO(
        "finalized with " << m_count << " muons asking for calo association, out of which: " << endmsg << "     "
                          << m_countInnerFailure << " failed to find the inner calo scattering plane," << endmsg
                          << "     " << m_countOuterFailure << " failed to find the outer calo scattering plane,"
                          << endmsg << "     while another " << m_countCompleteFailure
                          << " completely failed to intersect the calorimeter." << endmsg << "     "
                          << m_countArbitrarySolution << " with oscillation resolved by taking an arbitrary solution");

    delete m_magFieldProperties;

    return StatusCode::SUCCESS;
}

std::vector<const Trk::TrackStateOnSurface*>*
MuidCaloTrackStateOnSurface::caloTSOS(const Trk::TrackParameters& parameters) const
{
    const Trk::TrackStateOnSurface* innerTS      = 0;
    const Trk::TrackStateOnSurface* middleTS     = 0;
    const Trk::TrackStateOnSurface* outerTS      = 0;
    const Trk::TrackParameters*     innerParams  = 0;
    const Trk::TrackParameters*     middleParams = 0;
    const Trk::TrackParameters*     outerParams  = 0;

    MagField::AtlasFieldCache fieldCache;
    // Get field cache object
    EventContext                               ctx = Gaudi::Hive::currentContext();
    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, ctx};
    const AtlasFieldCacheCondObj*              fieldCondObj{*readHandle};

    if (fieldCondObj == nullptr) {
        ATH_MSG_ERROR("Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCacheCondObjInputKey.key());
        return nullptr;
    }
    fieldCondObj->getInitializedCache(fieldCache);

    // track to calo surfaces - first decide in or outwards
    bool trackOutwards = true;
    if (dynamic_cast<const Trk::PerigeeSurface*>(&parameters.associatedSurface())) {
        if (parameters.associatedSurface().center().perp() > 0.5 * Gaudi::Units::meter) trackOutwards = false;
    } else if (std::abs(parameters.position().z()) > 4.0 * Gaudi::Units::meter
               || parameters.position().perp() > 1.2 * Gaudi::Units::meter)
    {
        trackOutwards = false;
    }

    if (trackOutwards)  // forward tracking
    {
        innerTS = innerTSOS(parameters);
        if (innerTS) {
            innerParams = innerTS->trackParameters();
            if (innerParams->momentum().dot(innerParams->position()) > 0.) {
                middleParams = middleParameters(*innerParams);
            } else {
                ATH_MSG_VERBOSE(" quit as looper " << innerParams);
            }
            if (middleParams) {
                // get calo energy deposit
                middleTS = m_caloEnergyParam->trackStateOnSurface(*middleParams, innerParams, outerParams);
                if (!middleTS) {
                    delete middleParams;
                    middleParams = 0;
                }
            }
            if (middleParams) {
                // apply energy deposit to give outgoing middleParams,
                // note the iteration required to get optimum energy correction
                const Trk::TrackParameters* params = 0;
                for (int iterate = 0; iterate != 2; ++iterate) {
                    double energyDeposit = 0.;
                    if (!middleTS) break;
                    const Trk::MaterialEffectsOnTrack* meot =
                        dynamic_cast<const Trk::MaterialEffectsOnTrack*>(middleTS->materialEffectsOnTrack());
                    if (meot) energyDeposit = meot->energyLoss()->deltaE();
                    middleParams                  = middleTS->trackParameters();
                    Amg::Vector3D momentum        = middleParams->momentum();
                    double        correctedEnergy = innerParams->momentum().mag() - energyDeposit;

                    // fail potential loopers
                    if (fieldCache.toroidOn() && correctedEnergy < m_minRemainingEnergy) {
                        delete middleTS;
                        middleTS = 0;
                        ATH_MSG_VERBOSE(" fail tracking outwards: momentum "
                                        << momentum.mag() / Gaudi::Units::GeV << "   energyDeposit "
                                        << energyDeposit / Gaudi::Units::GeV << "   correctedEnergy "
                                        << correctedEnergy / Gaudi::Units::GeV << " (GeV units)");
                        break;
                    } else {
                        // toroid off: set floor for remaining energy
                        if (correctedEnergy < m_minRemainingEnergy) {
                            momentum *= m_minRemainingEnergy / momentum.mag();
                        } else {
                            momentum *= correctedEnergy / momentum.mag();
                        }

                        const Trk::CylinderSurface* cylinder =
                            dynamic_cast<const Trk::CylinderSurface*>(&middleParams->associatedSurface());
                        if (cylinder) {
                            params = new const Trk::AtaCylinder(middleParams->position(), momentum,
                                                                middleParams->charge(), *cylinder);
                        } else {
                            const Trk::DiscSurface* disc =
                                dynamic_cast<const Trk::DiscSurface*>(&middleParams->associatedSurface());
                            if (disc) {
                                params = new const Trk::AtaDisc(middleParams->position(), momentum,
                                                                middleParams->charge(), *disc);
                            } else {
                                ATH_MSG_WARNING(" caloTSOS: unexpected TrackParameters type ");
                            }
                        }

                        // delete middle and outer,
                        // then recompute with parameters as for backwards tracking
                        delete middleTS;
                        if (!params) {
                            middleTS = 0;
                        } else if (params->momentum().perp() > m_paramPtCut) {
                            middleTS = m_caloEnergyDeposit->trackStateOnSurface(*params, innerParams, outerParams);
                        } else {
                            middleTS = m_caloEnergyParam->trackStateOnSurface(*params, innerParams, outerParams);
                        }
                    }
                    delete outerTS;
                    if (middleTS) {
                        outerTS = outerTSOS(*params);
                    } else {
                        delete params;
                    }
                }
            }
            if (!middleTS) {
                ATH_MSG_VERBOSE(" fail tracking outwards: no intersect at middle surface");
            }
        } else {
            ATH_MSG_VERBOSE(" fail tracking ouwards: no intersect at inner surface");
        }
    } else  // backward tracking
    {
        outerTS = outerTSOS(parameters);
        if (outerTS) {
            outerParams  = outerTS->trackParameters();
            middleParams = middleParameters(*outerParams);
            if (middleParams) {
                innerParams = innerParameters(*middleParams);
                if (innerParams) {
                    // get calo energy deposit
                    if (middleParams->momentum().perp() > m_paramPtCut) {
                        middleTS = m_caloEnergyDeposit->trackStateOnSurface(*middleParams, innerParams, outerParams);
                    } else {
                        middleTS = m_caloEnergyParam->trackStateOnSurface(*middleParams, innerParams, outerParams);
                    }
                    delete innerParams;  // these will be recomputed below

                    // apply energy deposit
                    const Trk::MaterialEffectsOnTrack* meot = 0;
                    if (middleTS)
                        meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(middleTS->materialEffectsOnTrack());
                    double energyDeposit = 0.;
                    if (meot) energyDeposit = meot->energyLoss()->deltaE();
                    Amg::Vector3D momentum        = middleParams->momentum();
                    double        correctedEnergy = momentum.mag() + energyDeposit;

                    // fail potential loopers
                    if (correctedEnergy < 0.5 * Gaudi::Units::GeV) {
                        delete middleTS;
                        middleTS = 0;
                        ATH_MSG_VERBOSE(" fail tracking inwards: momentum "
                                        << momentum.mag() / Gaudi::Units::GeV << "   energyDeposit "
                                        << energyDeposit / Gaudi::Units::GeV << "   correctedEnergy "
                                        << correctedEnergy / Gaudi::Units::GeV << " (GeV units)");
                    } else {
                        momentum *= 1. + energyDeposit / momentum.mag();
                        const Trk::TrackParameters* params = 0;
                        const Trk::CylinderSurface* cylinder =
                            dynamic_cast<const Trk::CylinderSurface*>(&middleParams->associatedSurface());
                        AmgSymMatrix(5)* cov =
                            middleParams->covariance() ? new AmgSymMatrix(5)(*middleParams->covariance()) : 0;
                        bool deleteCov = true;
                        if (cylinder) {
                            params    = new const Trk::AtaCylinder(middleParams->position(), momentum,
                                                                middleParams->charge(), *cylinder, cov);
                            deleteCov = false;
                        } else {
                            const Trk::DiscSurface* disc =
                                dynamic_cast<const Trk::DiscSurface*>(&middleParams->associatedSurface());
                            if (disc) {
                                params    = new const Trk::AtaDisc(middleParams->position(), momentum,
                                                                middleParams->charge(), *disc, cov);
                                deleteCov = false;
                            } else {
                                ATH_MSG_WARNING(" caloTSOS: unexpected TrackParameters type ");
                            }
                        }
                        if (params && middleTS) innerTS = innerTSOS(*params);
                        if (deleteCov) delete cov;
                        delete params;
                    }
                } else {
                    ATH_MSG_VERBOSE(" fail tracking inwards: no intersect at inner surface");
                    delete middleParams;
                }
            } else {
                ATH_MSG_VERBOSE(" fail tracking inwards: no intersect at middle surface");
            }
        }
    }

    std::vector<const Trk::TrackStateOnSurface*>* caloTSOS = new std::vector<const Trk::TrackStateOnSurface*>;
    caloTSOS->reserve(3);
    if (innerTS) caloTSOS->push_back(innerTS);
    if (middleTS) caloTSOS->push_back(middleTS);
    if (outerTS) caloTSOS->push_back(outerTS);

    // keep some statistics
    ++m_count;
    if (caloTSOS->size() < 3) {
        if (caloTSOS->size() == 0) {
            ++m_countCompleteFailure;
        } else {
            if (!innerTS) ++m_countInnerFailure;
            if (!outerTS) ++m_countOuterFailure;
        }

        ATH_MSG_DEBUG(" caloTSOS:  only " << caloTSOS->size() << " calo surfaces associated");
    }

    return caloTSOS;
}

const Trk::TrackStateOnSurface*
MuidCaloTrackStateOnSurface::innerTSOS(const Trk::TrackParameters& parameters) const
{
    const Trk::TrackParameters* extrapolation = innerParameters(parameters);
    if (!extrapolation || extrapolation->position().perp() < m_minCaloRadius) {
        ATH_MSG_DEBUG(" innerTSOS:  extrapolation fails ");
        delete extrapolation;
        return nullptr;
    }

    return m_caloMaterialParam->trackStateOnSurface(extrapolation);
}

const Trk::TrackStateOnSurface*
MuidCaloTrackStateOnSurface::outerTSOS(const Trk::TrackParameters& parameters) const
{
    const Trk::TrackParameters* extrapolation = outerParameters(parameters);
    if (!extrapolation || extrapolation->position().perp() < m_minCaloRadius) {
        ATH_MSG_DEBUG(" outerTSOS:  extrapolation fails ");
        delete extrapolation;
        return nullptr;
    }

    return m_caloMaterialParam->trackStateOnSurface(extrapolation);
}

const Trk::TrackStateOnSurface*
MuidCaloTrackStateOnSurface::middleTSOS(const Trk::TrackParameters& middleParams,
                                        const Trk::TrackParameters* innerParams,
                                        const Trk::TrackParameters* outerParams) const
{
    const Trk::TrackParameters* extrapolation = middleParameters(middleParams);
    if (!extrapolation || extrapolation->position().perp() < m_minCaloRadius) {
        ATH_MSG_DEBUG(" middleTSOS:  extrapolation fails ");
        delete extrapolation;
        return nullptr;
    }

    const Trk::TrackStateOnSurface* TSOS = 0;
    if (extrapolation->momentum().perp() > m_paramPtCut) {
        TSOS = m_caloEnergyDeposit->trackStateOnSurface(*extrapolation, innerParams, outerParams);
    } else {
        TSOS = m_caloEnergyParam->trackStateOnSurface(*extrapolation, innerParams, outerParams);
    }

    return TSOS;
}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

const Trk::TrackParameters*
MuidCaloTrackStateOnSurface::innerParameters(const Trk::TrackParameters& parameters) const
{
    // will need to know whether forward or back tracking
    Trk::PropDirection momentumDirection = Trk::alongMomentum;
    Trk::PropDirection oppositeDirection = Trk::oppositeMomentum;
    Trk::PropDirection propDirection     = Trk::anyDirection;

    // initial surface at track eta -
    //   using position or direction according to distance from origin
    double              startingPhi = 0.;
    const Trk::Surface* surface     = 0;
    if (useEtaPhiFromDirection(parameters)) {
        startingPhi = parameters.momentum().phi();
        surface     = m_caloMaterialParam->innerSurface(parameters.momentum().eta());
    } else {
        startingPhi = parameters.position().phi();
        surface     = m_caloMaterialParam->innerSurface(parameters.position().eta());
        if (parameters.momentum().dot(parameters.position()) < 0.) {
            momentumDirection = Trk::oppositeMomentum;
            oppositeDirection = Trk::alongMomentum;
        }
    }
    if (!surface) return nullptr;

    // extrapolate to calo surface (take care to get correct cylinder intersect)
    unsigned                    extrapolations = 0;
    const Trk::CylinderSurface* cylinder       = dynamic_cast<const Trk::CylinderSurface*>(surface);
    if (cylinder) {
        if (parameters.position().perp() < cylinder->globalReferencePoint().perp()) {
            propDirection = momentumDirection;
        } else {
            propDirection = oppositeDirection;
        }
    }
    const Trk::TrackParameters* extrapolation =
        m_propagator->propagate(parameters, *surface, propDirection, false, *m_magFieldProperties, Trk::nonInteracting);
    if (!extrapolation) return nullptr;

    // phi flip means track has crossed beam-axis (so quit)
    double deltaPhi = std::abs(extrapolation->position().phi() - startingPhi);
    if (deltaPhi > 0.5 * M_PI && deltaPhi < 1.5 * M_PI) {
        delete extrapolation;
        return nullptr;
    }

    // also quit wrong rz-direction in endcap
    if (!cylinder) {
        double signRZ = (extrapolation->position().perp() - parameters.position().perp())
                        * (extrapolation->position().z() - parameters.position().z());
        if (signRZ * extrapolation->position().z() < 0.) {
            ATH_MSG_VERBOSE(" wrong way in endcap ");
            delete extrapolation;
            return nullptr;
        }
    }

    // iterate if extrapolated surface is different
    bool                restart             = false;
    const Trk::Surface* oldSurface          = surface;
    const Trk::Surface* extrapolatedSurface = m_caloMaterialParam->innerSurface(extrapolation->position().eta());
    while (++extrapolations < 5 && extrapolatedSurface != oldSurface && extrapolatedSurface != surface) {
        // take care to get correct solution for cylinder when starting from inside
        const Trk::CylinderSurface* cylinder = dynamic_cast<const Trk::CylinderSurface*>(extrapolatedSurface);
        if (cylinder) {
            if (extrapolation->position().perp() < cylinder->globalReferencePoint().perp()) {
                propDirection = momentumDirection;
            } else {
                propDirection = oppositeDirection;
            }
        } else {
            propDirection = Trk::anyDirection;
        }

        const Trk::TrackParameters* oldParameters = extrapolation;
        extrapolation = m_propagator->propagate(*oldParameters, *extrapolatedSurface, propDirection, false,
                                                *m_magFieldProperties, Trk::nonInteracting);
        if (!extrapolation) {
            // restart from input parameters (if not already done)
            //   trap no solution after restart
            if (oldParameters == &parameters) {
                ATH_MSG_VERBOSE(" innerParameters:  extrap fails ");
                return nullptr;
            }
            if (restart) {
                ATH_MSG_DEBUG(" innerParameters:  oscillating => arbitrary solution chosen");
                ++m_countArbitrarySolution;
                extrapolation       = oldParameters;
                extrapolatedSurface = &extrapolation->associatedSurface();
                surface             = extrapolatedSurface;
            } else {
                ATH_MSG_VERBOSE(" innerParameters:  restart extrap after " << extrapolations << " extrapolations");
                delete oldParameters;
                restart = true;
                extrapolations -= 2;
                extrapolation = &parameters;
                surface       = oldSurface;
            }
        } else {
            // update surface
            if (oldParameters != &parameters) delete oldParameters;
            oldSurface          = surface;
            surface             = extrapolatedSurface;
            extrapolatedSurface = m_caloMaterialParam->innerSurface(extrapolation->position().eta());
        }
    }

    // final check for phi flip
    deltaPhi = std::abs(extrapolation->position().phi() - startingPhi);
    if (deltaPhi > 0.5 * M_PI && deltaPhi < 1.5 * M_PI) {
        delete extrapolation;
        return nullptr;
    }

    ATH_MSG_VERBOSE(" innerParameters:  success after "
                    << extrapolations << " extrapolation step(s). " << std::setiosflags(std::ios::fixed)
                    << "  Intersection at: r,phi,z " << std::setw(7) << std::setprecision(1)
                    << extrapolation->position().perp() << std::setw(7) << std::setprecision(3)
                    << extrapolation->position().phi() << std::setw(8) << std::setprecision(1)
                    << extrapolation->position().z() << "  Direction: eta,phi " << std::setw(7) << std::setprecision(3)
                    << extrapolation->momentum().eta() << std::setw(7) << std::setprecision(3)
                    << extrapolation->momentum().phi());

    return extrapolation;
}

const Trk::TrackParameters*
MuidCaloTrackStateOnSurface::middleParameters(const Trk::TrackParameters& parameters) const
{
    // will need to know whether forward or back tracking
    Trk::PropDirection momentumDirection = Trk::alongMomentum;
    Trk::PropDirection oppositeDirection = Trk::oppositeMomentum;
    Trk::PropDirection propDirection     = Trk::anyDirection;

    // initial surface at track eta -
    //   using position or direction according to distance from origin
    double              startingPhi = 0.;
    const Trk::Surface* surface     = 0;
    if (useEtaPhiFromDirection(parameters)) {
        startingPhi = parameters.momentum().phi();
        surface     = m_caloMaterialParam->middleSurface(parameters.momentum().eta());
    } else {
        startingPhi = parameters.position().phi();
        surface     = m_caloMaterialParam->middleSurface(parameters.position().eta());
        if (parameters.momentum().dot(parameters.position()) < 0.) {
            momentumDirection = Trk::oppositeMomentum;
            oppositeDirection = Trk::alongMomentum;
        }
    }
    if (!surface) return nullptr;

    // extrapolate to calo surface (take care to get correct cylinder intersect)
    unsigned                    extrapolations = 0;
    const Trk::CylinderSurface* cylinder       = dynamic_cast<const Trk::CylinderSurface*>(surface);
    if (cylinder) {
        if (parameters.position().perp() < cylinder->globalReferencePoint().perp()) {
            propDirection = momentumDirection;
        } else {
            propDirection = oppositeDirection;
        }
    }
    const Trk::TrackParameters* extrapolation =
        m_propagator->propagate(parameters, *surface, propDirection, false, *m_magFieldProperties, Trk::nonInteracting);
    if (!extrapolation) return nullptr;

    // phi flip means track has crossed beam-axis (so quit)
    double deltaPhi = std::abs(extrapolation->position().phi() - startingPhi);
    if (deltaPhi > 0.5 * M_PI && deltaPhi < 1.5 * M_PI) {
        delete extrapolation;
        return nullptr;
    }

    // also quit wrong rz-direction in endcap
    if (!cylinder) {
        double signRZ = (extrapolation->position().perp() - parameters.position().perp())
                        * (extrapolation->position().z() - parameters.position().z());
        if (signRZ * extrapolation->position().z() < 0.) {
            ATH_MSG_VERBOSE(" wrong way in endcap ");
            delete extrapolation;
            return nullptr;
        }
    }

    // iterate if extrapolated surface is different
    bool                restart             = false;
    const Trk::Surface* oldSurface          = surface;
    const Trk::Surface* extrapolatedSurface = m_caloMaterialParam->middleSurface(extrapolation->position().eta());
    while (++extrapolations < 5 && extrapolatedSurface != oldSurface && extrapolatedSurface != surface) {
        // take care to get correct solution for cylinder when starting from inside
        const Trk::CylinderSurface* cylinder = dynamic_cast<const Trk::CylinderSurface*>(extrapolatedSurface);
        if (cylinder) {
            if (extrapolation->position().perp() < cylinder->globalReferencePoint().perp()) {
                propDirection = momentumDirection;
            } else {
                propDirection = oppositeDirection;
            }
        } else {
            propDirection = Trk::anyDirection;
        }

        const Trk::TrackParameters* oldParameters = extrapolation;
        extrapolation = m_propagator->propagate(*oldParameters, *extrapolatedSurface, propDirection, false,
                                                *m_magFieldProperties, Trk::nonInteracting);
        if (!extrapolation) {
            // restart from input parameters (if not already done)
            //   trap no solution after restart
            if (oldParameters == &parameters) {
                ATH_MSG_VERBOSE(" middleParameters:  extrap fails ");
                return nullptr;
            }
            //   arbitrary choice for oscillating solutions (i.e. following restart)
            if (restart) {
                ATH_MSG_DEBUG(" middleParameters:  oscillating => arbitrary solution chosen");
                ++m_countArbitrarySolution;
                extrapolation       = oldParameters;
                extrapolatedSurface = &extrapolation->associatedSurface();
                surface             = extrapolatedSurface;
            } else {
                ATH_MSG_VERBOSE(" middleParameters:  restart extrap after " << extrapolations << " extrapolations");
                delete oldParameters;
                restart = true;
                extrapolations -= 2;
                extrapolation = &parameters;
                surface       = oldSurface;
            }
        } else {
            // update surface
            if (oldParameters != &parameters) delete oldParameters;
            oldSurface          = surface;
            surface             = extrapolatedSurface;
            extrapolatedSurface = m_caloMaterialParam->middleSurface(extrapolation->position().eta());
        }
    }

    // final check for phi flip
    deltaPhi = std::abs(extrapolation->position().phi() - startingPhi);
    if (deltaPhi > 0.5 * M_PI && deltaPhi < 1.5 * M_PI) {
        delete extrapolation;
        return nullptr;
    }

    ATH_MSG_VERBOSE(" middleParameters: success after "
                    << extrapolations << " extrapolation step(s). " << std::setiosflags(std::ios::fixed)
                    << "  Intersection at: r,phi,z " << std::setw(7) << std::setprecision(1)
                    << extrapolation->position().perp() << std::setw(7) << std::setprecision(3)
                    << extrapolation->position().phi() << std::setw(8) << std::setprecision(1)
                    << extrapolation->position().z() << "  Direction: eta,phi " << std::setw(7) << std::setprecision(3)
                    << extrapolation->momentum().eta() << std::setw(7) << std::setprecision(3)
                    << extrapolation->momentum().phi());

    return extrapolation;
}

const Trk::TrackParameters*
MuidCaloTrackStateOnSurface::outerParameters(const Trk::TrackParameters& parameters) const
{
    // will need to know whether forward or back tracking
    Trk::PropDirection momentumDirection = Trk::alongMomentum;
    Trk::PropDirection oppositeDirection = Trk::oppositeMomentum;
    Trk::PropDirection propDirection     = Trk::anyDirection;

    // initial surface at track eta -
    //   using position or direction according to distance from origin
    double              startingPhi = 0.;
    const Trk::Surface* surface     = 0;
    if (useEtaPhiFromDirection(parameters)) {
        startingPhi = parameters.momentum().phi();
        surface     = m_caloMaterialParam->outerSurface(parameters.momentum().eta());
    } else {
        startingPhi = parameters.position().phi();
        surface     = m_caloMaterialParam->outerSurface(parameters.position().eta());
        if (parameters.momentum().dot(parameters.position()) < 0.) {
            momentumDirection = Trk::oppositeMomentum;
            oppositeDirection = Trk::alongMomentum;
        }
    }
    if (!surface) return nullptr;

    // extrapolate to calo surface (take care to get correct cylinder intersect)
    unsigned                    extrapolations = 0;
    const Trk::CylinderSurface* cylinder       = dynamic_cast<const Trk::CylinderSurface*>(surface);
    if (cylinder) {
        if (parameters.position().perp() < cylinder->globalReferencePoint().perp()) {
            propDirection = momentumDirection;
        } else {
            propDirection = oppositeDirection;
        }
    }
    const Trk::TrackParameters* extrapolation =
        m_propagator->propagate(parameters, *surface, propDirection, false, *m_magFieldProperties, Trk::nonInteracting);
    if (!extrapolation) return nullptr;

    // phi flip means track has crossed beam-axis (so quit)
    double deltaPhi = std::abs(extrapolation->position().phi() - startingPhi);
    if (deltaPhi > 0.5 * M_PI && deltaPhi < 1.5 * M_PI) {
        delete extrapolation;
        return nullptr;
    }

    // also quit wrong rz-direction in endcap
    if (!cylinder) {
        double signRZ = (extrapolation->position().perp() - parameters.position().perp())
                        * (extrapolation->position().z() - parameters.position().z());
        if (signRZ * extrapolation->position().z() < 0.) {
            ATH_MSG_VERBOSE(" wrong way in endcap ");
            delete extrapolation;
            return nullptr;
        }
    }

    // iterate if extrapolated surface is different
    bool                restart             = false;
    const Trk::Surface* oldSurface          = surface;
    const Trk::Surface* extrapolatedSurface = m_caloMaterialParam->outerSurface(extrapolation->position().eta());
    while (++extrapolations < 5 && extrapolatedSurface != oldSurface && extrapolatedSurface != surface) {
        // take care to get correct solution for cylinder when starting from inside
        const Trk::CylinderSurface* cylinder = dynamic_cast<const Trk::CylinderSurface*>(extrapolatedSurface);
        if (cylinder) {
            if (extrapolation->position().perp() < cylinder->globalReferencePoint().perp()) {
                propDirection = momentumDirection;
            } else {
                propDirection = oppositeDirection;
            }
        } else {
            propDirection = Trk::anyDirection;
        }

        const Trk::TrackParameters* oldParameters = extrapolation;
        extrapolation = m_propagator->propagate(*oldParameters, *extrapolatedSurface, propDirection, false,
                                                *m_magFieldProperties, Trk::nonInteracting);
        if (!extrapolation) {
            // restart from input parameters (if not already done)
            //   trap no solution after restart
            if (oldParameters == &parameters) {
                ATH_MSG_VERBOSE(" outerParameters:  extrap fails ");
                return nullptr;
            }
            //   arbitrary choice for oscillating solutions (i.e. following restart)
            if (restart) {
                ATH_MSG_DEBUG(" outerParameters:  oscillating => arbitrary solution chosen");
                ++m_countArbitrarySolution;
                extrapolation       = oldParameters;
                extrapolatedSurface = &extrapolation->associatedSurface();
                surface             = extrapolatedSurface;
            } else {
                ATH_MSG_VERBOSE(" outerParameters:  restart extrap after " << extrapolations << " extrapolations");
                delete oldParameters;
                restart = true;
                extrapolations -= 2;
                extrapolation = &parameters;
                surface       = oldSurface;
            }
        } else {
            // update surface
            if (oldParameters != &parameters) delete oldParameters;
            oldSurface          = surface;
            surface             = extrapolatedSurface;
            extrapolatedSurface = m_caloMaterialParam->outerSurface(extrapolation->position().eta());
        }
    }

    // final check for phi flip
    deltaPhi = std::abs(extrapolation->position().phi() - startingPhi);
    if (deltaPhi > 0.5 * M_PI && deltaPhi < 1.5 * M_PI) {
        delete extrapolation;
        return nullptr;
    }

    ATH_MSG_VERBOSE(" outerParameters:  success after "
                    << extrapolations << " extrapolation step(s). " << std::setiosflags(std::ios::fixed)
                    << "  Intersection at: r,phi,z " << std::setw(7) << std::setprecision(1)
                    << extrapolation->position().perp() << std::setw(7) << std::setprecision(3)
                    << extrapolation->position().phi() << std::setw(8) << std::setprecision(1)
                    << extrapolation->position().z() << "  Direction: eta,phi " << std::setw(7) << std::setprecision(3)
                    << extrapolation->momentum().eta() << std::setw(7) << std::setprecision(3)
                    << extrapolation->momentum().phi());

    return extrapolation;
}

bool
MuidCaloTrackStateOnSurface::useEtaPhiFromDirection(const Trk::TrackParameters& parameters) const
{
    // estimate eta,phi for the track intersect with the calo from the track direction when the
    // parameters are expressed near the beam-line, further out it's better to use the track position
    double parameterDistance = parameters.position().mag();
    if (dynamic_cast<const Trk::PerigeeSurface*>(&parameters.associatedSurface()))
        parameterDistance = parameters.associatedSurface().center().perp();

    if (parameterDistance < 0.5 * Gaudi::Units::meter) return true;
    return false;
}

}  // namespace Rec

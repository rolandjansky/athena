/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuidMaterialEffectsOnTrackProvider.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MuidCaloScatteringTools/MuidMaterialEffectsOnTrackProvider.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/DiscLayer.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/CylinderBounds.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "muonEvent/CaloEnergy.h"

namespace Rec {
    // constructor
    MuidMaterialEffectsOnTrackProvider::MuidMaterialEffectsOnTrackProvider(const std::string& t, const std::string& n,
                                                                           const IInterface* p) :
        AthAlgTool(t, n, p), m_cosmics{false} {
        declareProperty("Cosmics", m_cosmics);
        declareInterface<IMaterialEffectsOnTrackProvider>(this);
    }

    // Athena standard methods
    // initialize
    StatusCode MuidMaterialEffectsOnTrackProvider::initialize() {
        if (!m_cosmics) {
            ATH_CHECK(m_calotsos.retrieve());
            ATH_MSG_INFO("Retrieved tool " << m_calotsos.name());

            if (!m_calotsosparam.empty()) {
                ATH_CHECK(m_calotsosparam.retrieve());
                ATH_MSG_INFO("Retrieved tool " << m_calotsosparam.name());
            }

            ATH_CHECK(m_scattool.retrieve());
        }
        return StatusCode::SUCCESS;
    }

    std::vector<Trk::MaterialEffectsOnTrack> MuidMaterialEffectsOnTrackProvider::extrapolationSurfacesAndEffects(
        const Trk::TrackingVolume& /*vol*/, const Trk::IPropagator& /*prop*/, const Trk::TrackParameters& parm,
        const Trk::Surface& /*surf*/, Trk::PropDirection /*dir*/, Trk::ParticleHypothesis /*mateffects*/) const {
        std::vector<Trk::MaterialEffectsOnTrack> meots;
        if (m_cosmics) {
            Amg::Vector3D position(0, -3700, 0);

            double X0outer = 60;
            double X0inner = 60;

            static const Trk::CylinderSurface outersurf(3700, 5000);
            static const Trk::CylinderSurface innersurf(2000, 5000);
            static const Trk::CylinderSurface middlesurf(2850, 5000);

            std::pair<double, double> energy{-3000., 500.};
            ATH_MSG_DEBUG("first x0: " << X0inner << " second x0: " << X0outer << " eloss: " << energy.first
                                       << " sigma: " << energy.second);

            Trk::MaterialProperties matprop(X0outer, 1., 0., 0., 0., 0.);
            double sigmascat = std::sqrt(m_scattool->sigmaSquare(matprop, std::abs(1. / parm.parameters()[Trk::qOverP]), 1., Trk::muon));

            Trk::ScatteringAngles* newsa = new Trk::ScatteringAngles(0, 0, sigmascat / std::sin(parm.parameters()[Trk::theta]), sigmascat);
            Trk::ScatteringAngles* newsa2 = new Trk::ScatteringAngles(0, 0, sigmascat / std::sin(parm.parameters()[Trk::theta]), sigmascat);

            meots.emplace_back(X0inner, newsa, innersurf);
            meots.emplace_back(0, new Trk::EnergyLoss(energy.first, energy.second), middlesurf,
                               Trk::MaterialEffectsBase::EnergyLossEffects);
            meots.emplace_back(X0outer, newsa2, outersurf);

        } else {
            const EventContext& ctx = Gaudi::Hive::currentContext();
            std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> tsosvec = m_calotsos->caloTSOS(ctx, parm);
            if (tsosvec.size() != 3 && !m_calotsosparam.empty()) { tsosvec = m_calotsosparam->caloTSOS(ctx, parm); }
            for (std::unique_ptr<const Trk::TrackStateOnSurface>& tsos : tsosvec) {
                const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(tsos->materialEffectsOnTrack());
                if (!meot) continue;
                double sintheta = std::sin(tsos->trackParameters()->parameters()[Trk::theta]);
                double qoverp = tsos->trackParameters()->parameters()[Trk::qOverP];
                const CaloEnergy* eloss = nullptr;
                if (meot) eloss = dynamic_cast<const CaloEnergy*>(meot->energyLoss());

                Trk::EnergyLoss* neweloss = nullptr;
                Trk::ScatteringAngles* newsa = nullptr;
                if (eloss)
                    neweloss = new CaloEnergy(*eloss);
                else {
                    Trk::MaterialProperties matprop(meot->thicknessInX0(), 1., 0., 0., 0., 0.);
                    double sigmascat = std::sqrt(m_scattool->sigmaSquare(matprop, std::abs(1. / qoverp), 1., Trk::muon));
                    newsa = new Trk::ScatteringAngles(0, 0, sigmascat / sintheta, sigmascat);
                }
                meots.emplace_back(meot->thicknessInX0(), newsa, neweloss, tsos->trackParameters()->associatedSurface());
            }
        }
        if (meots.size() < 3) meots.clear();
        return meots;
    }
}  // namespace Rec
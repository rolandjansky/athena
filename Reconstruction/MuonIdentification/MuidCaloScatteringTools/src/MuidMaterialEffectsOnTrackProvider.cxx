/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuidMaterialEffectsOnTrackProvider.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MuidCaloScatteringTools/MuidMaterialEffectsOnTrackProvider.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/DiscLayer.h"
// #include "TrkGeometry/HomogenousLayerMaterial.h"
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

// constructor
Rec::MuidMaterialEffectsOnTrackProvider::MuidMaterialEffectsOnTrackProvider(const std::string& t, const std::string& n,
                                                                            const IInterface* p)
    : AthAlgTool(t, n, p)
{
    m_cosmics = false;
    declareProperty("Cosmics", m_cosmics);
    declareInterface<IMaterialEffectsOnTrackProvider>(this);
}

// destructor
Rec::MuidMaterialEffectsOnTrackProvider::~MuidMaterialEffectsOnTrackProvider() {}

// Athena standard methods
// initialize
StatusCode
Rec::MuidMaterialEffectsOnTrackProvider::initialize()
{
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

// finalize
StatusCode
Rec::MuidMaterialEffectsOnTrackProvider::finalize()
{
    ATH_MSG_DEBUG(name() << " finalize() successful");
    return StatusCode::SUCCESS;
}

std::vector<Trk::MaterialEffectsOnTrack>
Rec::MuidMaterialEffectsOnTrackProvider::extrapolationSurfacesAndEffects(
    const Trk::TrackingVolume& /*vol*/, const Trk::IPropagator& /*prop*/, const Trk::TrackParameters& parm,
    const Trk::Surface& /*surf*/, Trk::PropDirection /*dir*/, Trk::ParticleHypothesis /*mateffects*/) const
{

    std::vector<Trk::MaterialEffectsOnTrack> meots;
    if (m_cosmics) {
        Amg::Vector3D position(0, -3700, 0);


        double X0outer = 60;
        double X0inner = 60;

        Trk::CylinderSurface outersurf(3700, 5000);
        Trk::CylinderSurface innersurf(2000, 5000);
        Trk::CylinderSurface middlesurf(2850, 5000);

        std::pair<double, double> energy;

        energy.first  = -3000;
        energy.second = 500;  // ???

        // log << MSG::DEBUG << " returning " << matLayers->size() << " materialLayers " << endmsg;
        ATH_MSG_DEBUG("first x0: " << X0inner << " second x0: " << X0outer << " eloss: " << energy.first
                                   << " sigma: " << energy.second);

        Trk::MaterialProperties matprop(X0outer, 1., 0., 0., 0., 0.);
        double                  sigmascat =
            std::sqrt(m_scattool->sigmaSquare(matprop, std::abs(1. / parm.parameters()[Trk::qOverP]), 1., Trk::muon));

        Trk::ScatteringAngles* newsa =
            new Trk::ScatteringAngles(0, 0, sigmascat / std::sin(parm.parameters()[Trk::theta]), sigmascat);
        Trk::ScatteringAngles* newsa2 =
            new Trk::ScatteringAngles(0, 0, sigmascat / std::sin(parm.parameters()[Trk::theta]), sigmascat);


        meots.push_back(Trk::MaterialEffectsOnTrack(X0inner, newsa, innersurf));
        meots.push_back(Trk::MaterialEffectsOnTrack(0, new Trk::EnergyLoss(energy.first, energy.second), middlesurf,
                                                    Trk::MaterialEffectsBase::EnergyLossEffects));
        meots.push_back(Trk::MaterialEffectsOnTrack(X0outer, newsa2, outersurf));

        for (int i = 0; i < 3; i++) {
            // std::cout << "meot: " << meots[i] << std::endl;
        }
    } else {
        std::vector<const Trk::TrackStateOnSurface*>* tsosvec = m_calotsos->caloTSOS(parm);
        if ((!tsosvec || tsosvec->size() != 3) && !m_calotsosparam.empty()) {
            if (tsosvec) {
                for (int i = 0; i < (int)tsosvec->size(); i++) delete (*tsosvec)[i];
            }
            delete tsosvec;
            tsosvec = m_calotsosparam->caloTSOS(parm);
        }
        for (int i = 0; i < (tsosvec ? (int)tsosvec->size() : 0); i++) {
            const Trk::MaterialEffectsOnTrack* meot =
                dynamic_cast<const Trk::MaterialEffectsOnTrack*>((*tsosvec)[i]->materialEffectsOnTrack());
            if (!meot) continue;
            double            sintheta = std::sin((*tsosvec)[i]->trackParameters()->parameters()[Trk::theta]);
            double            qoverp   = (*tsosvec)[i]->trackParameters()->parameters()[Trk::qOverP];
            const CaloEnergy* eloss    = 0;
            if (meot) eloss = dynamic_cast<const CaloEnergy*>(meot->energyLoss());

            Trk::EnergyLoss*       neweloss = 0;
            Trk::ScatteringAngles* newsa    = 0;
            if (eloss)
                neweloss = new CaloEnergy(*eloss);
            else {
                Trk::MaterialProperties matprop(meot->thicknessInX0(), 1., 0., 0., 0., 0.);
                double sigmascat = std::sqrt(m_scattool->sigmaSquare(matprop, std::abs(1. / qoverp), 1., Trk::muon));
                newsa            = new Trk::ScatteringAngles(0, 0, sigmascat / sintheta, sigmascat);
            }
            Trk::MaterialEffectsOnTrack newmeot(meot->thicknessInX0(), newsa, neweloss,
                                                (*tsosvec)[i]->trackParameters()->associatedSurface());
            meots.push_back(newmeot);
            delete (*tsosvec)[i];
        }
        delete tsosvec;
    }
    if (meots.size() < 3) meots.clear();
    return meots;
}

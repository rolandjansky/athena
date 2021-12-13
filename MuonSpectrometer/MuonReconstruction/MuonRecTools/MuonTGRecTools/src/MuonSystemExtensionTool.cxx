/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSystemExtensionTool.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "MuonCombinedEvent/InDetCandidate.h"
#include "MuonDetDescrUtils/MuonChamberLayerDescription.h"
#include "MuonIdHelpers/MuonStationIndexHelpers.h"
#include "MuonLayerEvent/MuonSystemExtension.h"
#include "MuonLayerEvent/MuonSystemExtensionCollection.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PlaneSurface.h"

namespace Muon {

    MuonSystemExtensionTool::MuonSystemExtensionTool(const std::string& type, const std::string& name, const IInterface* parent) :
        AthAlgTool(type, name, parent) {
        declareInterface<IMuonSystemExtensionTool>(this);
    }

    StatusCode MuonSystemExtensionTool::initialize() {
        ATH_CHECK(m_caloExtensionTool.retrieve());
        ATH_CHECK(m_extrapolator.retrieve());

        if (!initializeGeometry()) return StatusCode::FAILURE;

        return StatusCode::SUCCESS;
    }

    bool MuonSystemExtensionTool::initializeGeometry() {
        // initialize reference surfaces hash vectors per region and sector
        m_referenceSurfaces.resize(MuonStationIndex::DetectorRegionIndexMax);
        for (auto& vec : m_referenceSurfaces) vec.resize(MuonStationIndex::numberOfSectors());

        // first build the barrel, we need different reference planes for all sectors
        ATH_MSG_DEBUG("Initializing barrel ");
        for (unsigned int sector = 1; sector <= 16; ++sector) {
            // get rotation into sector frame
            double sectorPhi = m_sectorMapping.sectorPhi(sector);
            Amg::AngleAxis3D sectorRotation(sectorPhi, Amg::Vector3D(0., 0., 1.));
            if (!initializeGeometryBarrel(sector, sectorRotation)) return false;

            if (!initializeGeometryEndcap(sector, MuonStationIndex::EndcapA, sectorRotation)) return false;
            if (!initializeGeometryEndcap(sector, MuonStationIndex::EndcapC, sectorRotation)) return false;
        }

        return true;
    }

    bool MuonSystemExtensionTool::initializeGeometryEndcap(int sector, MuonStationIndex::DetectorRegionIndex regionIndex,
                                                           const Amg::AngleAxis3D& sectorRotation) {
        ATH_MSG_DEBUG("Initializing endcap: sector " << sector << " " << MuonStationIndex::regionName(regionIndex));

        SurfaceVec& surfaces = m_referenceSurfaces[regionIndex][sector - 1];
        MuonChamberLayerDescription chamberLayerDescription;

        std::vector<MuonStationIndex::StIndex> layers = {MuonStationIndex::EI, MuonStationIndex::EE, MuonStationIndex::EM,
                                                         MuonStationIndex::EO};

        for (std::vector<MuonStationIndex::StIndex>::const_iterator stLayer = layers.begin(); stLayer != layers.end(); ++stLayer) {
            // calculate reference position from radial position of the layer
            MuonStationIndex::LayerIndex layer = MuonStationIndex::toLayerIndex(*stLayer);
            MuonChamberLayerDescriptor layerDescriptor = chamberLayerDescription.getDescriptor(sector, regionIndex, layer);
            Amg::Vector3D globalPosition(0., 0., layerDescriptor.referencePosition);

            // reference transform + surface
            Amg::Transform3D trans(sectorRotation);  //*Amg::AngleAxis3D(xToZRotation,Amg::Vector3D(0.,1.,0.)
            trans.pretranslate(globalPosition);

            Trk::PlaneSurface* surface = new Trk::PlaneSurface(trans);
            MuonLayerSurface data(MuonLayerSurface::SurfacePtr(surface), sector, regionIndex, layer);
            surfaces.push_back(std::move(data));

            // sanity checks
            if (msgLvl(MSG::VERBOSE)) {
                for (unsigned int test = 0; test < 2; ++test) {
                    Amg::Vector3D lpos3d = surface->transform().inverse() * globalPosition;
                    Amg::Vector2D lpos;
                    surface->globalToLocal(globalPosition, globalPosition, lpos);

                    ATH_MSG_VERBOSE(" sector " << sector << " layer " << MuonStationIndex::layerName(layer) << " hit x "
                                               << globalPosition.x() << " hit z " << globalPosition.z() << " lpos3d " << lpos3d.x() << " "
                                               << lpos3d.y() << " " << lpos3d.z() << " lpos " << lpos[Trk::loc1] << " " << lpos[Trk::loc2]
                                               << " center " << surface->center().z() << " normal: phi " << surface->normal().phi()
                                               << " theta " << surface->normal().theta() << " normal: x " << surface->normal().x() << " y "
                                               << surface->normal().y() << " z " << surface->normal().z());
                    globalPosition[0] += 100;
                }
            }
        }
        ATH_MSG_VERBOSE(" Total number of surfaces " << surfaces.size());
        return true;
    }

    bool MuonSystemExtensionTool::initializeGeometryBarrel(int sector, const Amg::AngleAxis3D& sectorRotation) {
        MuonChamberLayerDescription chamberLayerDescription;

        SurfaceVec& surfaces = m_referenceSurfaces[MuonStationIndex::Barrel][sector - 1];
        double xToZRotation = -0.5 * M_PI;

        for (unsigned int stationLayer = MuonStationIndex::BI; stationLayer <= MuonStationIndex::BE; ++stationLayer) {
            // skip BEE if in small sectors, not installed
            if (stationLayer == MuonStationIndex::BE && MuonStationIndexHelpers::isSmall(sector)) continue;

            // calculate reference position from radial position of the laeyr
            MuonStationIndex::LayerIndex layer = MuonStationIndex::toLayerIndex((MuonStationIndex::StIndex)(stationLayer));
            MuonChamberLayerDescriptor layerDescriptor = chamberLayerDescription.getDescriptor(sector, MuonStationIndex::Barrel, layer);
            Amg::Vector3D positionInSector(layerDescriptor.referencePosition, 0., 0.);
            Amg::Vector3D globalPosition = sectorRotation * positionInSector;

            // reference transform + surface
            Amg::Transform3D trans(sectorRotation * Amg::AngleAxis3D(xToZRotation, Amg::Vector3D(0., 1., 0.)));
            trans.pretranslate(globalPosition);
            Trk::PlaneSurface* surface = new Trk::PlaneSurface(trans);

            MuonLayerSurface data(MuonLayerSurface::SurfacePtr(surface), sector, MuonStationIndex::Barrel, layer);
            surfaces.push_back(std::move(data));

            // sanity checks
            if (msgLvl(MSG::VERBOSE)) {
                Amg::Vector3D lpos3d = surface->transform().inverse() * globalPosition;
                // Amg::Vector3D lpos3d2 = surface->transform().inverse()*globalPosition2;
                Amg::Vector2D lpos;
                surface->globalToLocal(globalPosition, globalPosition, lpos);
                double sectorPhi = m_sectorMapping.sectorPhi(sector);

                ATH_MSG_VERBOSE(" sector " << sector << " layer " << MuonStationIndex::layerName(layer) << " phi " << sectorPhi
                                           << " ref theta " << globalPosition.theta() << " phi " << globalPosition.phi() << " r "
                                           << globalPosition.perp() << " pos " << globalPosition.x() << " " << globalPosition.y() << " "
                                           << globalPosition.z() << " lpos3d " << lpos3d.x() << " " << lpos3d.y() << " " << lpos3d.z()
                                           << " normal: x " << surface->normal().x() << " y " << surface->normal().y() << " z "
                                           << surface->normal().z());
            }
        }
        return true;
    }

    bool MuonSystemExtensionTool::muonSystemExtension(const EventContext& ctx, SystemExtensionCache& cache) const {
        /// Get the calo extension
        if (!cache.candidate->getCaloExtension()) {
            if (!cache.extensionContainer) {
                std::unique_ptr<Trk::CaloExtension> caloExtension = 
                        m_caloExtensionTool->caloExtension(ctx, cache.candidate->indetTrackParticle());
                if (!caloExtension || !caloExtension->muonEntryLayerIntersection()) {
                    ATH_MSG_VERBOSE("Failed to create the calo extension for "<<cache.candidate->toString());
                    return false;
                }
                cache.candidate->setExtension(caloExtension);
            } else {
                const Trk::CaloExtension* caloExtension = m_caloExtensionTool->caloExtension(cache.candidate->indetTrackParticle(),
                                                                    *cache.extensionContainer);
                if (!caloExtension || !caloExtension->muonEntryLayerIntersection()) {
                    ATH_MSG_VERBOSE("Failed to create the calo extension for "<<cache.candidate->toString());
                    return false;
                }
                cache.candidate->setExtension(caloExtension);
            }            
        }
       
        if (!cache.createSystemExtension) {
            ATH_MSG_VERBOSE("No system extension is required for "<<cache.candidate->toString());           
            return true;
        }
        // get entry parameters, use it as current parameter for the extrapolation
        const Trk::TrackParameters* currentPars = cache.candidate->getCaloExtension()->muonEntryLayerIntersection();

        // get reference surfaces
        ATH_MSG_VERBOSE(" getSurfacesForIntersection " << currentPars);
        SurfaceVec surfaces = getSurfacesForIntersection(*currentPars, cache);

        // store intersections
        std::vector<MuonSystemExtension::Intersection> intersections;

        // garbage collection
        std::vector<std::shared_ptr<const Trk::TrackParameters> > trackParametersVec;

        // loop over reference surfaces
        for (const Muon::MuonLayerSurface& it : surfaces) {
            // extrapolate to next layer
            const Trk::Surface& surface = *it.surfacePtr;
            if (msgLvl(MSG::VERBOSE)) {
                ATH_MSG_VERBOSE(" startPars: phi pos "
                                << currentPars->position().phi() << " direction phi " << currentPars->momentum().phi() << " theta pos "
                                << currentPars->position().theta() << " theta " << currentPars->momentum().theta() << " r "
                                << currentPars->position().perp() << " z " << currentPars->position().z() << " momentum "
                                << currentPars->momentum().mag() << " local " << currentPars->parameters()[Trk::locX] << " "
                                << currentPars->parameters()[Trk::locY]);
                if (currentPars->covariance())
                    ATH_MSG_VERBOSE(" err " << Amg::error(*currentPars->covariance(), Trk::locX) << " "
                                            << Amg::error(*currentPars->covariance(), Trk::locY));
                ATH_MSG_VERBOSE(" destination: sector " << it.sector << "  " << MuonStationIndex::regionName(it.regionIndex) << "  "
                                                        << MuonStationIndex::layerName(it.layerIndex) << " phi  " << surface.center().phi()
                                                        << " r " << surface.center().perp() << " z " << surface.center().z());
            }

            std::unique_ptr<const Trk::TrackParameters> exPars{
                m_extrapolator->extrapolate(ctx, *currentPars, surface, Trk::alongMomentum, false, Trk::muon)};
            if (!exPars) {
                ATH_MSG_VERBOSE("extrapolation failed, trying next layer ");
                continue;
            }

            //    reject intersections with very big uncertainties (parallel to surface)
            if (!Amg::saneCovarianceDiagonal(*exPars->covariance()) || Amg::error(*exPars->covariance(), Trk::locX) > 10000. ||
                Amg::error(*exPars->covariance(), Trk::locY) > 10000.)
                continue;

            // create shared pointer and add to garbage collection
            std::shared_ptr<const Trk::TrackParameters> sharedPtr{std::move(exPars)};
            trackParametersVec.emplace_back(sharedPtr);

            MuonSystemExtension::Intersection intersection = makeInterSection(sharedPtr, it);
            if (intersection.trackParameters) intersections.push_back(std::move(intersection));

            if (Amg::error(*sharedPtr->covariance(), Trk::locX) < 10. * Amg::error(*currentPars->covariance(), Trk::locX) &&
                Amg::error(*sharedPtr->covariance(), Trk::locY) < 10. * Amg::error(*currentPars->covariance(), Trk::locY)) {
                // only update the parameters if errors don't blow up
                currentPars = sharedPtr.get();
            }
        }
        ATH_MSG_DEBUG("Completed extrapolation: destinations " << surfaces.size() << " intersections " << intersections.size());
        if (intersections.empty()){
            ATH_MSG_DEBUG("No system extensions are made for "<<cache.candidate->toString()
                         <<" will accept the candidate: "<< (!cache.requireSystemExtension ? "si": "no"));
            return !cache.requireSystemExtension;
        }
        cache.candidate->setExtension(
            std::make_unique<MuonSystemExtension>(cache.candidate->getCaloExtension()->muonEntryLayerIntersection(), std::move(intersections)));
        return true;
    }
    MuonSystemExtension::Intersection MuonSystemExtensionTool::makeInterSection(std::shared_ptr<const Trk::TrackParameters> sharedPtr, const MuonLayerSurface& it) const{
        if (sharedPtr && it.regionIndex != MuonStationIndex::Barrel) {
            // in the endcaps we need to update the sector and check that we are in the correct sector
            double phi = sharedPtr->position().phi();
            std::vector<int> sectors;
            m_sectorMapping.getSectors(phi, sectors);

            // loop over sectors, if the surface sector and the sector in the loop are either both large or small, pick
            // the sector
            int sector = -1;
            for (const int& sec : sectors) {
                if (it.sector % 2 == sec % 2) {
                    sector = sec;
                    break;
                }
            }
            ATH_MSG_DEBUG(" sector " << it.sector << " selected " << sector << " nsectors " << sectors);
            // only select if we found a matching sector in the set
            if (sector != -1) {
                MuonSystemExtension::Intersection intersection{sharedPtr, it};
                intersection.layerSurface.sector = sector;
                return intersection;              
            } 
            return MuonSystemExtension::Intersection{nullptr, it};
        }
        return  MuonSystemExtension::Intersection{sharedPtr, it}; 
    }

    MuonSystemExtensionTool::SurfaceVec MuonSystemExtensionTool::getSurfacesForIntersection(const Trk::TrackParameters& muonEntryPars,
                                                                                            const SystemExtensionCache& cache) const {
        // if in endcaps pick endcap surfaces
        double eta = muonEntryPars.position().eta();
        MuonStationIndex::DetectorRegionIndex regionIndex = MuonStationIndex::Barrel;
        if (eta < -1.05) regionIndex = MuonStationIndex::EndcapC;
        if (eta > 1.05) regionIndex = MuonStationIndex::EndcapA;

        // in barrel pick primary sector
        const double phi = muonEntryPars.position().phi();
        std::vector<int> sectors;
        m_sectorMapping.getSectors(phi, sectors);
        SurfaceVec surfaces;
        /// Look whether one of the sectors has actually a recorded hit
        if (cache.useHitSectors) {
            const auto map_itr = cache.sectorsWithHits->find(regionIndex);
            if (map_itr == cache.sectorsWithHits->end()) {
                ATH_MSG_DEBUG("No hits in detector region " << Muon::MuonStationIndex::regionName(regionIndex));
                return surfaces;
            }
            std::vector<int>::const_iterator sec_itr = std::find_if(
                sectors.begin(), sectors.end(), [&map_itr](const int& sector) -> bool { return map_itr->second.count(sector); });
            if (sec_itr == sectors.end()) {
                ATH_MSG_DEBUG("No hits found for sector " << m_sectorMapping.getSector(phi) << " in MuonStation "
                                                          << Muon::MuonStationIndex::regionName(regionIndex));
                return surfaces;
            }
        }

        for (auto sector : sectors) {
            surfaces.insert(surfaces.end(), m_referenceSurfaces[regionIndex][sector - 1].begin(),
                            m_referenceSurfaces[regionIndex][sector - 1].end());
        }
        auto sortFunction = (regionIndex == MuonStationIndex::Barrel)
                                ? [](const MuonLayerSurface& s1,
                                     const MuonLayerSurface& s2) { return s1.surfacePtr->center().perp() < s2.surfacePtr->center().perp(); }
                                : [](const MuonLayerSurface& s1, const MuonLayerSurface& s2) {
                                      return std::abs(s1.surfacePtr->center().z()) < std::abs(s2.surfacePtr->center().z());
                                  };

        std::stable_sort(surfaces.begin(), surfaces.end(), sortFunction);
        return surfaces;
    }

    MuonSystemExtension::Intersection MuonSystemExtensionTool::getInterSection(const EventContext& ctx, const Trk::TrackParameters& muon_pars) const {
        // get reference surfaces
        ATH_MSG_VERBOSE(" getSurfacesForIntersection " << muon_pars);
        SystemExtensionCache cache{};
        cache.useHitSectors = false;

        /// Find the surface which is closes to the muon_pars
        SurfaceVec surfaces = getSurfacesForIntersection(muon_pars, cache);
        if (surfaces.empty()) {
            return makeInterSection(nullptr, MuonLayerSurface());
        }   
        const Amg::Vector3D pos = muon_pars.position();
        /// Pick the surface which is the closest to the track parameters
        SurfaceVec::const_iterator itr = std::min_element(surfaces.begin(),surfaces.end(), [&pos](const MuonLayerSurface& a, const MuonLayerSurface& b )  {
            return (a.surfacePtr->center() - pos).mag2() < (b.surfacePtr->center() - pos).mag2();
        });
        /// Define the direction of the extrapolation
        const Trk::Surface& surf = *(itr->surfacePtr);
        const bool isBarrel = std::abs(pos.eta()) < 1.05;
        const Trk::PropDirection dir = (isBarrel && pos.perp2() < surf.center().perp()) || 
                                 (!isBarrel && std::abs(pos.z()) < std::abs(surf.center().z()) ) ? Trk::alongMomentum : Trk::oppositeMomentum;
       
        std::unique_ptr<const Trk::TrackParameters> exPars{m_extrapolator->extrapolate(ctx, muon_pars, surf, dir, false, Trk::muon)};
        return makeInterSection(std::move(exPars), *itr);
    } 

}  // namespace Muon

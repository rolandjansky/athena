/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTrkMuIdTools/TrackDepositInCaloTool.h"

// --- Tracking ---
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"

// --- Calorimeter ---
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloUtils/CaloCellList.h"
#include "FourMomUtils/xAODP4Helpers.h"
#include "GaudiKernel/ITHistSvc.h"
#include "ParticleCaloExtension/ParticleCaloAssociation.h"
#include "ParticleCaloExtension/ParticleCellAssociationCollection.h"
#include "ParticleCaloExtension/ParticleCellIntersection.h"
#include "TileDetDescr/TileCellDim.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TrkCaloExtension/CaloExtension.h"
#include "TrkCaloExtension/CaloExtensionCollection.h"
#include "TrkCaloExtension/CaloExtensionHelpers.h"
#include "AthenaKernel/SlotSpecificObj.h"
// --- ROOT ---
#include <cmath>

#include "TH1F.h"
#include "TH2F.h"

namespace {
    /// Radius of the solenoid magnet
    constexpr float solenoidRadius = 1280.;
}  // namespace
///////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////
TrackDepositInCaloTool::TrackDepositInCaloTool(const std::string& type, const std::string& name, const IInterface* pInterface) :
    AthAlgTool(type, name, pInterface) {
    declareInterface<ITrackDepositInCaloTool>(this);
    declareProperty("doExtrapolation", m_doExtr = true);
    declareProperty("doEDeposHist", m_doHist = false);
}

///////////////////////////////////////////////////////////////////////////////
// Initialize
///////////////////////////////////////////////////////////////////////////////
StatusCode TrackDepositInCaloTool::initialize() {
    ATH_CHECK(detStore()->retrieve(m_tileDDM));
    if (!m_tileDDM) { return StatusCode::FAILURE; }
    if (m_doHist) { ATH_CHECK(bookHistos()); }

    ATH_CHECK(m_extrapolator.retrieve());
    ATH_CHECK(m_caloExtensionTool.retrieve());
    ATH_CHECK(m_caloCellAssociationTool.retrieve());
    ATH_CHECK(m_caloDetDescrMgrKey.initialize());
    ATH_MSG_INFO("initialize() successful in " << name());
    return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// getDeposits
///////////////////////////////////////////////////////////////////////////////
std::vector<DepositInCalo> TrackDepositInCaloTool::getDeposits(const Trk::TrackParameters* par,
                                                               const CaloCellContainer* caloCellCont) const {
    const EventContext& ctx = Gaudi::Hive::currentContext();
    ATH_MSG_DEBUG("In TrackDepositInCaloTool::getDeposits()");
    std::vector<DepositInCalo> result;

    // --- from now on, par can be assumed to exist ---
    if (!par) { return result; }

    if (!caloCellCont) return result;

    SG::ReadCondHandle<CaloDetDescrManager> caloDetDescrMgrHandle{ m_caloDetDescrMgrKey,ctx };
    if (!caloDetDescrMgrHandle.isValid()) {
      return result;
    }

    const CaloDetDescrManager* caloDDM = *caloDetDescrMgrHandle;

    const Trk::ParticleHypothesis muonHypo = Trk::muon;

    // --- Preselection of track's crossing with detector elements ---
    std::vector<Amg::Vector3D> extrapolations;
    std::map<double, const CaloDetDescriptor*> caloInfo;
    std::unique_ptr<const Trk::TrackParameters> currentPar = extrapolateToSolenoid(ctx, par);
    if (!currentPar) {
        ATH_MSG_WARNING("Extrapolation to solenoid did not succeed!");
        return result;
    }
    if (getTraversedLayers(caloDDM,currentPar.get(), caloInfo, extrapolations).isFailure()) {
        ATH_MSG_WARNING("Failure in getTraversedLayers(). ");
        return result;
    }

    // --- Iterate over all the detector regions ---
    for (const std::pair<const double, const CaloDetDescriptor*>& detDescrIt : caloInfo) {
        const CaloDetDescriptor* descr = detDescrIt.second;
        // ---- extrapolate to entrance of layer ---
        std::unique_ptr<const Trk::TrackParameters> parEntrance = extrapolateToEntranceOfLayer(ctx, currentPar.get(), descr);
        if (!parEntrance) {
            ATH_MSG_DEBUG("Could not extrapolate to entrance.");
            continue;
        }
        // ---- extrapolate to exit of layer ---
        std::unique_ptr<const Trk::TrackParameters> parExit = extrapolateToExitOfLayer(ctx, parEntrance.get(), descr);
        if (!parExit) {
            ATH_MSG_DEBUG("Could not extrapolate to exit.");
            // delete parEntrance;
            currentPar.swap(parEntrance);
            continue;
        }
        // --- Calculate energy loss ---
        double energyLoss = calcEnergy(parEntrance.get(), muonHypo) - calcEnergy(parExit.get(), muonHypo);
        double distance = (parEntrance->position() - parExit->position()).mag();
        // --- Retrieve crossed cells ---
        std::vector<const CaloCell*> cells =
          getCaloCellsForLayer(caloDDM, descr, parEntrance.get(), parExit.get(), caloCellCont);

        // --- Add contributions ---
        double sumEnergy = 0;
        double sumEt = 0;
        for (const CaloCell* cell : cells) {
            if (cell) {
                sumEnergy += cell->energy();
                sumEt += cell->et();
            } else {
                ATH_MSG_VERBOSE("Cell NULL pointer found!");
            }
        }
        // --- Write DepositInCalo ---
        ATH_MSG_DEBUG("Energy = " << sumEnergy << " for sample " << descr->getSampling() << " in " << cells.size() << " cells.");
        if (distance) { result.emplace_back(descr->getSampling(), sumEnergy, energyLoss, sumEt); }

        // --- Free memory and prepare for next round ---
        currentPar.swap(parEntrance);
    }
    return result;
}

///////////////////////////////////////////////////////////////////////////////
// - New getDeposits
///////////////////////////////////////////////////////////////////////////////
std::vector<DepositInCalo> TrackDepositInCaloTool::getDeposits(const xAOD::TrackParticle* tp, const CaloCellContainer* caloCellCont,
                                                               const CaloExtensionCollection* extensionCache) const {
    ATH_MSG_DEBUG("In TrackDepositsInCaloTool::getDeposits() - new");
    std::vector<DepositInCalo> result;
    const unsigned int nSamples = CaloSampling::getNumberOfSamplings();

    // - associate calocells to trackparticle, cone size 0.2, use cache

    std::unique_ptr<const Rec::ParticleCellAssociation> association =
        m_caloCellAssociationTool->particleCellAssociation(*tp, 0.2, caloCellCont, extensionCache);

    if (!association) return result;
    ATH_MSG_VERBOSE(" particleCellAssociation done  " << association.get());

    // - pick up the cell intersections

    std::vector<std::pair<const CaloCell*, Rec::ParticleCellIntersection*>> cellIntersections = association->cellIntersections();

    const Trk::CaloExtension& caloExtension = association->caloExtension();

    if (!caloExtension.caloEntryLayerIntersection()) {
        ATH_MSG_WARNING(" No caloEntryLayerIntersection found ");
        return result;
    }

    ATH_MSG_DEBUG(" nr of cell intersections " << cellIntersections.size());
    if (cellIntersections.size() < 3) ATH_MSG_WARNING(" Strange nr of calorimeter cell intersections " << cellIntersections.size());

    CaloExtensionHelpers::EntryExitLayerMap entryExitLayerMap;
    CaloExtensionHelpers::entryExitLayerMap(caloExtension, entryExitLayerMap);
    ATH_MSG_VERBOSE("EntryExitLayerMap " << entryExitLayerMap.size());

    CaloExtensionHelpers::ScalarLayerMap eLossLayerMap;
    CaloExtensionHelpers::eLossLayerMap(caloExtension, eLossLayerMap);
    ATH_MSG_VERBOSE("eLossLayerMap " << eLossLayerMap.size());

    std::vector<float> exp_E(nSamples, 0.0);
    std::vector<float> meas_E(nSamples, 0.0);
    std::vector<float> meas_Et(nSamples, 0.0);
    std::vector<int> LayerHit(nSamples, 0);
    CaloSampling::CaloSample sample;
    Amg::Vector3D lEntry, lExit;

    // loop over cellIntersections, there can be more than one cell hit in a layer

    for (auto& it : cellIntersections) {
        const CaloCell* curr_cell = it.first;
        if (curr_cell->badcell()) continue;
        int cellSampling = curr_cell->caloDDE()->getSampling();
        CaloSampling::CaloSample sample = static_cast<CaloSampling::CaloSample>(cellSampling);
        // bool badCell     = curr_cell->badcell();
        auto pos = entryExitLayerMap.find(sample);
        if (pos == entryExitLayerMap.end()) continue;
        lEntry = pos->second.first;
        lExit = pos->second.second;
        if (TrackDepositInCaloTool::inCell(curr_cell, lEntry) || TrackDepositInCaloTool::inCell(curr_cell, lExit)) {
            meas_E[cellSampling] += Float_t(curr_cell->energy());
            meas_Et[cellSampling] += Float_t(curr_cell->energy() * curr_cell->sinTh());
            exp_E[cellSampling] = Float_t((it.second)->expectedEnergyLoss());
            LayerHit[cellSampling]++;
            ATH_MSG_VERBOSE(" Layer : " << cellSampling << "   Energy = " << curr_cell->energy()
                                        << " Exp : " << (it.second)->expectedEnergyLoss());
        }
    }

    // sum cells per layer and fill samples per layer

    for (int i = CaloSampling::PreSamplerB; i < CaloSampling::CaloSample::Unknown; i++) {
        if (LayerHit[i] > 0) {
            sample = static_cast<CaloSampling::CaloSample>(i);
            result.emplace_back(sample, meas_E[i], exp_E[i], meas_Et[i]);
            ATH_MSG_DEBUG(" Layer : " << sample << "   Energy = " << meas_E[i] << " nCells : " << LayerHit[i] << " Exp: " << exp_E[i]);
        }
    }

    return result;
}

// - end getDeposits

///////////////////////////////////////////////////////////////////////////////
// getCaloCellsForLayer
///////////////////////////////////////////////////////////////////////////////
std::vector<const CaloCell*> TrackDepositInCaloTool::getCaloCellsForLayer(const CaloDetDescrManager* caloDDM,
                                                                          const CaloDetDescriptor* descr,
                                                                          const Trk::TrackParameters* parEntrance,
                                                                          const Trk::TrackParameters* parExit,
                                                                          const CaloCellContainer* caloCellCont) const {
  if (descr->is_tile()) {
    // --- Tile implemention is lengthy and therefore put in seperate function
    // ---
    return getCaloCellsForTile(caloDDM, descr, parEntrance, parExit, caloCellCont);
  } else {
    // --- LAr implementation is short, quick and simple ---
    const CaloCell* cellEntrance =
      getClosestCellLAr(caloDDM, parEntrance, descr, caloCellCont);
    const CaloCell* cellExit =
      getClosestCellLAr(caloDDM, parExit, descr, caloCellCont);
    std::vector<const CaloCell*> result;
    result.push_back(cellEntrance);
    if (cellEntrance != cellExit) {
      result.push_back(cellExit);
    }
    return result;
  }
}

///////////////////////////////////////////////////////////////////////////////
// getCaloCellsForTile
///////////////////////////////////////////////////////////////////////////////
std::vector<const CaloCell*> TrackDepositInCaloTool::getCaloCellsForTile(const CaloDetDescrManager* caloDDM,
                                                                         const CaloDetDescriptor* descr,
                                                                         const Trk::TrackParameters* parEntrance,
                                                                         const Trk::TrackParameters* parExit,
                                                                         const CaloCellContainer* caloCellCont) const {
    /*
      ...to be written...
    */
    ATH_MSG_VERBOSE("in getCaloCellsForTileLayer()...");

    std::vector<const CaloCell*> result{};
    // --- Get all the relevant kinematical variables ---
    double phiPar = parEntrance->position().phi();
    double phiParEntrance = parEntrance->position().phi();
    double zParEntrance = parEntrance->position().z();
    double phiParExit = parExit->position().phi();
    double zParExit = parExit->position().z();
    double diffZPar = zParExit - zParEntrance;
    double etaPar = parEntrance->position().eta();
    // --- Determine granularity ---
    double etaWidth = 2 * (descr->calo_eta_max() - descr->calo_eta_min()) / descr->n_eta();
    double etaMin = etaPar - etaWidth;
    double etaMax = etaPar + etaWidth;
    double phiWidth = (descr->calo_phi_max() - descr->calo_phi_min()) / descr->n_phi();
    // TODO: HOW TO DEAL WITH PHI ~ PI?
    double phiMin = phiPar - phiWidth;
    double phiMax = phiPar + phiWidth;
    // --- Fill vecHash ---
    CaloCell_ID::CaloSample sample = descr->getSampling();

    std::vector<IdentifierHash> vecHash;
    caloDDM->cellsInZone(etaMin, etaMax, phiMin, phiMax, sample, vecHash);

    // --- Iterate and find closest to track (around 12-15 elements in loop) ---
    std::map<double, const CaloCell*> neighbourMap0;
    std::map<double, const CaloCell*> neighbourMap1;

    for (const IdentifierHash& id : vecHash) {
        const CaloCell* cell = caloCellCont->findCell(id);
        if (!cell) continue;
        const CaloDetDescrElement* dde = cell->caloDDE();
        if (!dde) continue;

        double dPhiCell = dde->dphi();
        double phiCellMin = dde->phi() - dPhiCell / 2;
        double phiCellMax = phiCellMin + dPhiCell;
        if (!(phiParEntrance > phiCellMin && phiParExit < phiCellMax) && !(phiParExit > phiCellMin && phiParExit < phiCellMax)) {
            continue;
        }

        // --- There are two z dimensions for the BC cells ---
        double zCellMin0{0}, zCellMax0{0}, zCellMin1{0}, zCellMax1{0};
        TileCellDim* cellDim = m_tileDDM->get_cell_dim(cell->ID());
        if (!cellDim) {
            ATH_MSG_WARNING("TileCellDim object not found for cell " << cell->ID());
            continue;
        }
        // --- This is valid for the BC cells ---
        if (cellDim->getNRows() == 6) {
            zCellMin0 = cellDim->getZMin(0);
            zCellMax0 = cellDim->getZMax(0);
            zCellMin1 = cellDim->getZMin(3);
            zCellMax1 = cellDim->getZMax(3);
        } else {
            zCellMin0 = cellDim->getZMin(0);
            zCellMax0 = cellDim->getZMax(0);
            zCellMin1 = zCellMin0;
            zCellMax1 = zCellMax0;
        }

        // --- Check wether entrance parameters are within cell acceptance at entrance ---
        // --- The equal signs (>=, <=) are very important for the boundary cells ---
        if (zParEntrance >= zCellMin0 && zParEntrance <= zCellMax0 && phiParEntrance > phiCellMin && phiParEntrance < phiCellMax) {
            result.push_back(cell);
            continue;
        }
        // --- Check wether exit parameters are within cell acceptance at exit ---
        if (zParExit >= zCellMin1 && zParExit <= zCellMax1 && phiParExit > phiCellMin && phiParExit < phiCellMax) {
            result.push_back(cell);
            continue;
        }
        // --- Check wether it crosses a cell ---
        if (diffZPar > 0) {
            if (zParEntrance < zCellMin0 && zParExit > zCellMax1) {
                result.push_back(cell);
                continue;
            }
        } else {
            if (zParEntrance > zCellMax0 && zParExit < zCellMin1) {
                result.push_back(cell);
                continue;
            }
        }
    }
    return result;
}

///////////////////////////////////////////////////////////////////////////////
// extrapolateToEntraceOfLayer
///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<const Trk::TrackParameters> TrackDepositInCaloTool::extrapolateToEntranceOfLayer(const EventContext& ctx,
                                                                                                 const Trk::TrackParameters* par,
                                                                                                 const CaloDetDescriptor* descr) const {
    // --- Initialization ---
    std::unique_ptr<const Trk::TrackParameters> paramEntrance;
    bool checkBoundary = true;
    Trk::ParticleHypothesis muonHypo = Trk::muon;

    std::unique_ptr<Trk::Surface> surfEntrance{createSurface(descr, Entrance)};
    if (!surfEntrance) {
        ATH_MSG_DEBUG("Error in extrapolateToBeginOfLayer()");
        return nullptr;
    }
    // --- Try to extrapolate to entrance ---
    paramEntrance = m_extrapolator->extrapolate(ctx, *par, *surfEntrance, Trk::alongMomentum, !checkBoundary, muonHypo);
    if (!paramEntrance) {
        ATH_MSG_DEBUG("Extrapolation to entrance failed without boundary check.");
        return nullptr;
    }
    const Amg::Vector3D& posVecEntrance = paramEntrance->position();
    // --- If the parameters are not on surface, extrapolate to side of calorimeter ---
    if (surfEntrance->isOnSurface(posVecEntrance, checkBoundary)) {
        ATH_MSG_VERBOSE("Successfully extrapolated to entrance of calo for sample " << descr->getSampling());
        return paramEntrance;
    }
    std::unique_ptr<Trk::Surface> surfInside{createSurface(descr, Inside)};
    if (surfInside) {
        std::unique_ptr<const Trk::TrackParameters> paramOnInside{
            m_extrapolator->extrapolate(ctx, *paramEntrance, *surfInside, Trk::alongMomentum, checkBoundary, muonHypo)};
        if (paramOnInside) {
            ATH_MSG_VERBOSE("Successfully extrapolated to inner side of calo for sample " << descr->getSampling());
            paramEntrance.swap(paramOnInside);
        } else {
            ATH_MSG_DEBUG("Extrapolation to entrance failed!");
            return nullptr;
        }
    }
    return paramEntrance;
}

///////////////////////////////////////////////////////////////////////////////
// extrapolateToExitOfLayer
///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<const Trk::TrackParameters> TrackDepositInCaloTool::extrapolateToExitOfLayer(const EventContext& ctx,
                                                                                             const Trk::TrackParameters* par,
                                                                                             const CaloDetDescriptor* descr) const {
    std::unique_ptr<const Trk::TrackParameters> paramExit;
    bool checkBoundary = true;
    Trk::ParticleHypothesis muonHypo = Trk::muon;

    // --- Create surface parallel to layer ---
    std::unique_ptr<Trk::Surface> surfExit{createSurface(descr, Exit)};
    if (!surfExit) {
        ATH_MSG_WARNING("Could not create exit surface.");
        return nullptr;
    }
    // --- Try to extrapolate to exit of layer ---
    paramExit = m_extrapolator->extrapolate(ctx, *par, *surfExit, Trk::alongMomentum, checkBoundary, muonHypo);
    if (paramExit) {
        ATH_MSG_VERBOSE("Extrapolated to exit. ");
        return paramExit;
    }
    // --- Try to extrapolate to side ---
    std::unique_ptr<Trk::Surface> surfOutside{createSurface(descr, Outside)};
    if (!surfOutside) { return nullptr; }
    paramExit = m_extrapolator->extrapolate(ctx, *par, *surfOutside, Trk::alongMomentum, checkBoundary, muonHypo);
    if (paramExit) {
        ATH_MSG_VERBOSE("Succesfully extrapolated to outer side of calo for sample " << descr->getSampling());
    } else {
        ATH_MSG_VERBOSE("Could not extrapolate to exit of calo.");
    }
    return paramExit;
}

///////////////////////////////////////////////////////////////////////////////
// extrapolateToSolenoid
///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<const Trk::TrackParameters> TrackDepositInCaloTool::extrapolateToSolenoid(const EventContext& ctx,
                                                                                          const Trk::TrackParameters* par,
                                                                                          bool oppositeMomentum) const {
    Trk::PropDirection direction = Trk::alongMomentum;
    if (oppositeMomentum) { direction = Trk::oppositeMomentum; }

    // --- First extrapolate to solenoid radius or EndCap disk ---
    // --- Ownership of the HepTransform3Ds is passed to the Surfaces ---
    constexpr double halfLengthOfCylinder = 3700;
    bool checkBoundary = true;
    Trk::CylinderSurface solenoid(solenoidRadius, halfLengthOfCylinder);
    Trk::ParticleHypothesis muonHypo = Trk::muon;

    std::unique_ptr<const Trk::TrackParameters> parAtSolenoid{
        m_extrapolator->extrapolate(ctx, *par, solenoid, direction, checkBoundary, muonHypo)};
    if (!parAtSolenoid) {
        // --- Guess EndCap side by direction ---
        double zTrans = par->eta() > 0 ? halfLengthOfCylinder : -halfLengthOfCylinder;
        Trk::DiscSurface disc(Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0., 0., zTrans))), 0, solenoidRadius);

        parAtSolenoid = m_extrapolator->extrapolate(ctx, *par, disc, direction, checkBoundary, muonHypo);

        if (!parAtSolenoid) {
            ATH_MSG_VERBOSE("extrapolateToSolenoid(): Extrapolation to cap of solenoid failed. Trying opposite side.");
            Trk::DiscSurface discOpp(Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0., 0., -zTrans))), 0, solenoidRadius);
            parAtSolenoid = m_extrapolator->extrapolate(ctx, *par, discOpp, direction, checkBoundary, muonHypo);
        }

        if (parAtSolenoid) { ATH_MSG_VERBOSE("extrapolateToSolenoid(): Extrapolation succeeded for disc-type surface."); }

    } else {
        ATH_MSG_VERBOSE("extrapolateToSolenoid(): Extrapolation succeeded for cylinder-type surface.");
    }

    return parAtSolenoid;
}

///////////////////////////////////////////////////////////////////////////////
// deposits
///////////////////////////////////////////////////////////////////////////////
std::vector<DepositInCalo> TrackDepositInCaloTool::deposits(const Trk::TrackParameters* par, const CaloCellContainer* cellContainer) const {
    /*
       This method retrieves a vector of DepositInCalo (sample, energy, energyloss, et) containing one entry for each
       calorimeter sample traversed. The method is written for muons which are MIPs, so they don't shower very much. This is the
       reason why only the one sample is returned that is closest to the muon track.
       The algorithm for finding the deposits works as follows. First a preselection is done by getTraversedLayers(). This returns
       a vector of CaloDetDescriptors*. From these CaloDetDescriptors surfaces are built to which the extrapolator extrapolates
       the track parameters . The extrapolator first tries to hit the entrance surface, but if it fails it will try to hit the
       sides. From there on, the track is extrapolated to the middle of the Calorimeter and the CaloCell that is closest to this point
       is retrieved using the function getClosestCell. Eventually the parameters are extrapolated to the exit, or outer side of the
       sample.
     */
    ATH_MSG_DEBUG("In TrackDepositInCaloTool::deposits()");
    std::vector<DepositInCalo> result;
    const EventContext& ctx = Gaudi::Hive::currentContext();

    // --- Possible crash prevention ---
    if (!par) { return result; }

    SG::ReadCondHandle<CaloDetDescrManager> caloDetDescrMgrHandle{ m_caloDetDescrMgrKey, ctx };
    if (!caloDetDescrMgrHandle.isValid()) {
      return result;
    }

    const CaloDetDescrManager* caloDDM = *caloDetDescrMgrHandle;

    const Trk::ParticleHypothesis muonHypo = Trk::muon;
    bool checkBoundary = true;

    // --- Preselection of track's crossing with detector elements ---
    std::vector<Amg::Vector3D> extrapolations;
    std::map<double, const CaloDetDescriptor*> caloInfo;
    std::unique_ptr<const Trk::TrackParameters> parAtSolenoid = extrapolateToSolenoid(ctx, par);
    if (parAtSolenoid) {
        if (getTraversedLayers(caloDDM, parAtSolenoid.get(), caloInfo, extrapolations).isFailure()) {
            ATH_MSG_WARNING("Failure in finding getTraversedLayers. ");
            return result;
        }
    } else {
        ATH_MSG_WARNING("Failure in extrapolating to solenoid surface.");
        return result;
    }
    // --- Loop over the cells from preselection ---
    std::vector<Amg::Vector3D>::iterator itP = extrapolations.begin();
    for (const std::pair<const double, const CaloDetDescriptor*>& it : caloInfo) {
        // --- Initialization of variables to be determined below ---
        double energyEntrance{0}, energyExit{0}, energyDeposit{0}, ETDeposit{0}, energyLoss{0};
        bool eLossFound = false;
        const CaloDetDescriptor* descr = it.second;
        CaloCell_ID::CaloSample sample = descr->getSampling();
        ATH_MSG_VERBOSE("Analysing crossing of calo sample " << sample << " ");

        // --- Extrapolate to entrance of layer ---
        std::unique_ptr<const Trk::Surface> surfEntrance{createSurface(descr, Entrance)};
        if (!surfEntrance) {
            continue;
            ATH_MSG_VERBOSE("Could not create entrance surface.");
        }
        std::unique_ptr<const Trk::TrackParameters> paramEntrance{
            m_extrapolator->extrapolate(ctx, *par, *surfEntrance, Trk::alongMomentum, !checkBoundary, muonHypo)};
        if (!paramEntrance) {
            ATH_MSG_VERBOSE("Could not extrapolate to entrance of calo.");
            continue;
        }
        const Amg::Vector3D& posVecEntrance = paramEntrance->position();
        if (!surfEntrance->isOnSurface(posVecEntrance, checkBoundary)) {
            std::unique_ptr<Trk::Surface> surfInside{createSurface(descr, Inside)};
            if (surfInside) {
                std::unique_ptr<const Trk::TrackParameters> paramOnInside{
                    m_extrapolator->extrapolate(ctx, *paramEntrance, *surfInside, Trk::alongMomentum, checkBoundary, muonHypo)};
                if (paramOnInside) {
                    ATH_MSG_VERBOSE("Succesfully extrapolated to inner side of calo for sample " << sample);
                    paramEntrance.swap(paramOnInside);
                } else {
                    ATH_MSG_WARNING("Extrapolation failed to inner side of calo " << sample);
                }
            } else {
                ATH_MSG_WARNING("Could not create surface for inside of calo for sample " << sample);
            }
        }
        energyEntrance = calcEnergy(paramEntrance.get(), muonHypo);
        // --- Extrapolate to middle of layer ---
        std::unique_ptr<Trk::Surface> surfMiddle{createSurface(descr, Middle)};
        if (surfMiddle) {
            std::unique_ptr<const Trk::TrackParameters> paramMiddle{
                m_extrapolator->extrapolate(ctx, *paramEntrance, *surfMiddle, Trk::alongMomentum, checkBoundary, muonHypo)};
            if (paramMiddle) {
                // Get energy:
                const CaloCell* cell = getClosestCell(caloDDM, paramMiddle.get(), descr, cellContainer);
                if (cell) {
                    energyDeposit = cell->energy();
                    ETDeposit = cell->et();
                }
                // --- Extrapolate to exit of layer ---
                std::unique_ptr<Trk::Surface> surfExit{createSurface(descr, Exit)};
                std::unique_ptr<const Trk::TrackParameters> paramExit;
                if (surfExit) {
                    paramExit = m_extrapolator->extrapolate(ctx, *paramMiddle, *surfExit, Trk::alongMomentum, checkBoundary, muonHypo);
                    if (paramExit) {
                        ATH_MSG_VERBOSE("Extrapolated to exit. ");
                        energyExit = calcEnergy(paramExit.get(), muonHypo);
                        eLossFound = true;
                    } else {
                        // Try to extrapolate to outside
                        std::unique_ptr<Trk::Surface> surfOutside{createSurface(descr, Outside)};
                        if (surfOutside) {
                            paramExit = 
                                m_extrapolator->extrapolate(ctx, *paramMiddle, *surfOutside, Trk::alongMomentum, checkBoundary, muonHypo);
                            if (paramExit) {
                                ATH_MSG_VERBOSE("Succesfully extrapolated to outer side of calo for sample " << sample);
                                energyExit = calcEnergy(paramExit.get(), muonHypo);
                                eLossFound = true;
                            } else {
                                ATH_MSG_VERBOSE("Could not extrapolate to exit of calo.");
                            }
                        } else {
                            ATH_MSG_WARNING("Could not create surface for outside of calo for sample " << sample);
                        }
                    }
                } else {
                    ATH_MSG_WARNING("Could not create exit surface.");
                }

            } else {
                ATH_MSG_VERBOSE("Could not extrapolate to middle of calo.");
            }
        } else {
            ATH_MSG_VERBOSE("Could not create middle surface.");
        }

        energyLoss = eLossFound ? -(energyExit - energyEntrance) : 0;
        result.emplace_back(sample, energyDeposit, energyLoss, ETDeposit);
        ATH_MSG_DEBUG("Sample: " << sample << "\tEnergyDeposit: " << energyDeposit << "\tEnergyLoss: " << energyLoss);

        if (m_doHist) {
            Hists& h = getHists();
            h.m_hParELossEta->Fill(energyLoss, itP->eta());
            h.m_hParELossSample->Fill(energyLoss, sample);
        }

        // itP++;
        // it++;
    }

    ATH_MSG_VERBOSE("---TRACKDEPOSITINCALOTOOL---TRACKDEPOSITINCALOTOOL---TRACKDEPOSITINCALOTOOL");
    return result;
}

///////////////////////////////////////////////////////////////////////////////
// calcEnergy
///////////////////////////////////////////////////////////////////////////////
double TrackDepositInCaloTool::calcEnergy(const Trk::TrackParameters* par, const Trk::ParticleHypothesis& particleHypo) const {
    double mass = m_particlemasses.mass[particleHypo];
    if (!par) return 0.;
    double pX = par->momentum().x();
    double pY = par->momentum().y();
    double pZ = par->momentum().z();
    return std::sqrt(mass * mass + pX * pX + pY * pY + pZ * pZ);
}

///////////////////////////////////////////////////////////////////////////////
// initializeDetectorInfo()
///////////////////////////////////////////////////////////////////////////////
TrackDepositInCaloTool::LayerMaps
TrackDepositInCaloTool::initializeDetectorInfo(const CaloDetDescrManager* caloDDM) const {
    LayerMaps maps;

    ATH_MSG_DEBUG("In CaloTrkMuIdDetStore::initialize()");
    // Initialize LAr
    for (const CaloDetDescriptor* descr : caloDDM->calo_descriptors_range()) {
        if (descr) {
            CaloCell_ID::CaloSample sample = descr->getSampling();
            ATH_MSG_VERBOSE("Detector Description element for sample " << sample);
            if (descr->is_lar_em_barrel()) {
                ATH_MSG_VERBOSE("  this is a cylindrical detector element.");
                double thickness = descr->calo_r_max() - descr->calo_r_min();
                double r = descr->calo_r_min() + thickness / 2.0;
                ATH_MSG_VERBOSE("  R = " << r);
                ATH_MSG_VERBOSE("  sign = " << descr->calo_sign());
                ATH_MSG_VERBOSE("  range_low  = " << descr->calo_z_min());
                ATH_MSG_VERBOSE("  range_high = " << descr->calo_z_max());
                if (sample != CaloCell_ID::PreSamplerB) maps.m_barrelLayerMap[r].push_back(descr);
            } else {
                ATH_MSG_VERBOSE("  this is a disk-like detector element.");
                double thickness = descr->calo_z_max() - descr->calo_z_min();
                double z = descr->calo_z_min() + thickness / 2.0;
                ATH_MSG_VERBOSE("  Z = " << z);
                ATH_MSG_VERBOSE("  sign = " << descr->calo_sign());
                ATH_MSG_VERBOSE("  range_low  = " << descr->calo_r_min());
                ATH_MSG_VERBOSE("  range_high = " << descr->calo_r_max());
                if (sample != CaloCell_ID::PreSamplerE) { maps.m_endCapLayerMap[z].push_back(descr); }
            }
        } else
            ATH_MSG_VERBOSE("CaloDetDescriptor was not available!");
    }

    ATH_MSG_VERBOSE("Processing tiles... ");
    for (const CaloDetDescriptor* descr : caloDDM->tile_descriptors_range()) {
        if (descr) {
            ATH_MSG_VERBOSE("Detector Description element for sample " << descr->getSampling());
            if (!descr->is_tile()) { ATH_MSG_VERBOSE("This is not a isTile()==true element."); }
            CaloCell_ID::CaloSample sample = descr->getSampling();
            if (sample >= 15 && sample <= 17) {
                // --- Skip the TileGap detector elements ---
                continue;
            }
            ATH_MSG_VERBOSE("  this is a cylindrical detector element.");
            double thickness = descr->calo_r_max() - descr->calo_r_min();
            double r = descr->calo_r_min() + thickness / 2.0;
            ATH_MSG_VERBOSE("  r = " << r);
            ATH_MSG_VERBOSE("  sign = " << descr->calo_sign());
            ATH_MSG_VERBOSE("  range_low  = " << descr->calo_z_min());
            ATH_MSG_VERBOSE("  range_high = " << descr->calo_z_max());
            maps.m_barrelLayerMap[r].push_back(descr);
        } else
            ATH_MSG_VERBOSE("CaloDetDescriptor was not available!");
    }
    return maps;
}

///////////////////////////////////////////////////////////////////////////////
// extrapolateR
///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<Amg::Vector3D> TrackDepositInCaloTool::extrapolateR(const Amg::Vector3D& initialPosition, double phi0, double theta0,
                                                                    double r) {
    double x0 = initialPosition.x();
    double y0 = initialPosition.y();
    double z0 = initialPosition.z();

    double b = 2 * x0 * std::cos(phi0) + 2 * y0 * std::sin(phi0);
    double c = x0 * x0 + y0 * y0 - r * r;
    double det = b * b - 4 * c;
    double lsin = 0;
    if (det < 0) {
        return nullptr;
    } else
        lsin = (-b + std::sqrt(det)) / 2;

    double xe = x0 + lsin * std::cos(phi0);
    double ye = y0 + lsin * std::sin(phi0);
    double ze = z0 + lsin * std::cos(theta0) / std::sin(theta0);

    if (std::abs(xe * xe + ye * ye - r * r) > 10 && det > 0) {
        // ATH_MSG_WARNING("ExtrapoateR(): extrapolation did not succeed!");
        return nullptr;
    }
    // ATH_MSG_INFO("ExtrapolateR(): Extrapolation succeeded.");
    return std::make_unique<Amg::Vector3D>(xe, ye, ze);
}

///////////////////////////////////////////////////////////////////////////////
// extrapolateZ
///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<Amg::Vector3D> TrackDepositInCaloTool::extrapolateZ(const Amg::Vector3D& initialPosition, double phi0, double theta0,
                                                                    double z) {
    double x0 = initialPosition.x();
    double y0 = initialPosition.y();
    double z0 = initialPosition.z();
    double cosTheta0 = std::cos(theta0);
    if (cosTheta0) {
        double dist = z - z0;
        double lambda = dist / cosTheta0;
        if (lambda < 0) {
            // ATH_MSG_WARNING("ExtrapolateZ(): extrapolation did not succeed!");
            return nullptr;
        }
        double xe = x0 + lambda * std::sin(theta0) * std::cos(phi0);
        double ye = y0 + lambda * std::sin(theta0) * std::sin(phi0);
        double ze = z;
        // ATH_MSG_INFO("ExtrapolateZ(): Extrapolation succeeded.");
        return std::make_unique<Amg::Vector3D>(xe, ye, ze);
    } else {
        // ATH_MSG_WARNING("ExtrapolateZ(): extrapolation did not succeed!");
        return nullptr;
    }
}

///////////////////////////////////////////////////////////////////////////////
// getTraversedLayers
///////////////////////////////////////////////////////////////////////////////
StatusCode TrackDepositInCaloTool::getTraversedLayers(const CaloDetDescrManager* caloDDM,
                                                      const Trk::TrackParameters* par, 
                                                      std::map<double, const CaloDetDescriptor*>& caloInfo,
                                                      std::vector<Amg::Vector3D>& extrapolations) const {
    // Cannot do this in initialize: see ATLASRECTS-5012
    if (!m_layerMaps.isValid()) {
      m_layerMaps.set (initializeDetectorInfo (caloDDM));
    }
    const LayerMaps& layerMaps = *m_layerMaps.ptr();

    const Trk::TrackParameters* parAtSolenoid = nullptr;
    // --- To be replaced by a check, possibly extrapolating to solenoid surface if needed ---
    bool parIsAtSolenoid = true;
    if (parIsAtSolenoid) parAtSolenoid = par;

    // --- This performs a straight line extrapolation and determines wether it is in calorimeter layer acceptance. ---
    if (parAtSolenoid) {
        // ATH_MSG_INFO("Parameters at solenoid are well-defined.");

        double deltaR_solLast = std::abs(parAtSolenoid->position().perp() - par->position().perp());
        double deltaEta_solLast = std::abs(parAtSolenoid->position().eta() - par->position().eta());
        if (m_doHist) {
            Hists& h = getHists();
            h.m_hDeltaEtaLastPar->Fill(deltaEta_solLast);
            h.m_hDeltaRadiusLastPar->Fill(deltaR_solLast);
        }

        const Amg::Vector3D positionAtSolenoid = parAtSolenoid->position();
        double theta0 = parAtSolenoid->momentum().theta();
        double phi0 = parAtSolenoid->momentum().phi();

        // --- This Code fragment determines the Barrel crossings ---
        for (const std::pair<const double, std::vector<const CaloDetDescriptor*>>& mapIt : layerMaps.m_barrelLayerMap) {
            const double& radius = mapIt.first;
            std::unique_ptr<Amg::Vector3D> extrapolation = extrapolateR(positionAtSolenoid, phi0, theta0, radius);
            if (!extrapolation) { continue; }
            for (const CaloDetDescriptor* descr : mapIt.second) {
                double zSigned = extrapolation->z() * descr->calo_sign();
                if (zSigned >= descr->calo_z_min() && zSigned <= descr->calo_z_max()) {
                    double distance = (*extrapolation - positionAtSolenoid).mag();
                    caloInfo[distance] = descr;
                    extrapolations.emplace_back(*extrapolation);
                }
            }
        }

        // This code fragment determines the EndCap crossings
        for (const std::pair<const double, std::vector<const CaloDetDescriptor*>>& mapIt : layerMaps.m_endCapLayerMap) {
            const double& zCenter = mapIt.first;
            for (const CaloDetDescriptor* descr : mapIt.second) {
                double z = zCenter * descr->calo_sign();
                std::unique_ptr<Amg::Vector3D> extrapolation = extrapolateZ(positionAtSolenoid, phi0, theta0, z);
                if (!extrapolation) continue;

                double radius = extrapolation->perp();
                if (radius >= descr->calo_r_min() && radius <= descr->calo_r_max()) {
                    double distance = (*extrapolation - positionAtSolenoid).mag();
                    caloInfo[distance] = descr;
                    extrapolations.emplace_back(*extrapolation);
                }
            }
        }

    } else {
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// createSurface()
///////////////////////////////////////////////////////////////////////////////
Trk::Surface* TrackDepositInCaloTool::createSurface(const CaloDetDescriptor* descr, CaloSurfaceType type) const {
    /*
    Creates a Calorimeter surface for parameter descr. For barrel layers CaloSurfaceType Entrance, Middle and Exit yield
    a Trk::CylinderSurface. For CaloSurfaceTypes Inside and Outside the result will be a Trk::DiscSurface. Inside in
    this case is the smallest abs(z) and Outside the largest abs(z) for the two possible sides. For EndCap layers,
    Inside is the cylinder surface with smallest radius, and Outside the one with the largest radius. It all makes sense
    when you realize that CaloTrkMuId extrapolates a track outwards from the Inner Detector. The ownership of the
    transforms is passed to the Trk::Surface result.
    */
    Trk::Surface* res = nullptr;
    if (descr->is_tile() || descr->is_lar_em_barrel()) {
        // --- Cylindrical calo elements ---
        if (type >= Entrance && type <= Exit) {
            double thickness = descr->calo_r_max() - descr->calo_r_min();
            double halfLength = (descr->calo_z_max() - descr->calo_z_min()) / 2;
            double middle = descr->calo_z_min() + halfLength;
            double radius = type / 2.0 * thickness + descr->calo_r_min();
            // ATH_MSG_INFO("r = " << radius << " for type " << type << " and sample " << descr->getSampling());
            //    HepGeom::Transform3D* trans = new HepGeom::Translate3D(0,0,descr->calo_sign()*middle);
            res = new Trk::CylinderSurface(Amg::Transform3D(Amg::Translation3D(0., 0., descr->calo_sign() * middle)), radius, halfLength);
            return res;
        } else if (type == Inside || type == Outside) {
            double offset;
            if (type == Inside) {
                offset = descr->calo_z_min() * descr->calo_sign();
            } else {
                offset = descr->calo_z_max() * descr->calo_sign();
            }
            res = new Trk::DiscSurface(Amg::Transform3D(Amg::Translation3D(0., 0., offset)), descr->calo_r_min(), descr->calo_r_max());
            return res;
        } else {
            ATH_MSG_WARNING("Type error in CaloTrkMuIdDetStore::createSurface().");
            return nullptr;
        }
    } else {
        // --- Disc-like calo elements (EndCap) ---
        if (type >= Entrance && type <= Exit) {
            double thickness = descr->calo_z_max() - descr->calo_z_min();
            double offset = descr->calo_sign() * (thickness * type / 2.0 + descr->calo_z_min());
            res = new Trk::DiscSurface(Amg::Transform3D(Amg::Translation3D(0., 0., offset)), descr->calo_r_min(), descr->calo_r_max());
            return res;
        } else if (type == Inside || type == Outside) {
            double radius;
            if (type == Inside) {
                radius = descr->calo_r_min();
            } else {
                radius = descr->calo_r_max();
            }
            double halfLength = (descr->calo_z_max() - descr->calo_z_min()) / 2.0;
            double offset = descr->calo_sign() * (descr->calo_z_min() + halfLength);
            res = new Trk::CylinderSurface(Amg::Transform3D(Amg::Translation3D(0., 0., offset)), radius, halfLength);
            return res;
        } else {
            ATH_MSG_WARNING("Type error in CaloTrkMuIdDetStore::createSurface().");
            return nullptr;
        }
    }
    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
// getClosestCell()
///////////////////////////////////////////////////////////////////////////////
const CaloCell*
TrackDepositInCaloTool::getClosestCell(const CaloDetDescrManager* caloDDM,
                                       const Trk::TrackParameters* par,
                                       const CaloDetDescriptor* descr,
                                       const CaloCellContainer* caloCellCont) const
{
  /*
  Get closest cell near the TrackParameters par. For LAr this can be done using the get_element function of the
  CaloDetDescrManager. This should be really fast since it is nothing more than a sequence of lookups.
  For the non-projective tile cells one has to select cells in a certain (eta,phi) region and then select the one
  that is closest to the track.
  */

  const CaloCell* cell = nullptr;
  if (descr->is_tile()) {
    cell = getClosestCellTile(caloDDM, par, descr, caloCellCont);
  } else {
    cell = getClosestCellLAr(caloDDM, par, descr, caloCellCont);
  }
  return cell;
}

///////////////////////////////////////////////////////////////////////////////
// getClosestCellLAr()
///////////////////////////////////////////////////////////////////////////////
const CaloCell*
TrackDepositInCaloTool::getClosestCellLAr(const CaloDetDescrManager* caloDDM,
                                          const Trk::TrackParameters* par,
                                          const CaloDetDescriptor* descr,
                                          const CaloCellContainer* caloCellCont)
{
  CaloCell_ID::CaloSample sample = descr->getSampling();
  // ATH_MSG_INFO("Sampling = " << sample);
  const CaloDetDescrElement* cellDescr = caloDDM->get_element(sample, par->position().eta(), par->position().phi());
  if (cellDescr) {
    IdentifierHash hash = cellDescr->calo_hash();
    const CaloCell* cell = caloCellCont->findCell(hash);
    return cell;
  }
  return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
// getClosestCellTile()
///////////////////////////////////////////////////////////////////////////////
const CaloCell*
TrackDepositInCaloTool::getClosestCellTile(
  const CaloDetDescrManager* caloDDM,
  const Trk::TrackParameters* par,
  const CaloDetDescriptor* descr,
  const CaloCellContainer* caloCellCont) const
{
  std::map<double, const CaloCell*> neighbourMap;
  const CaloCell* result = nullptr;
  // --- Determine granularity ---
  double etaPar = par->position().eta();
  double phiPar = par->position().phi();
  double etaWidth = 2 * (descr->calo_eta_max() - descr->calo_eta_min()) / descr->n_eta();
  double etaMin = etaPar - etaWidth;
  double etaMax = etaPar + etaWidth;
  double phiWidth = (descr->calo_phi_max() - descr->calo_phi_min()) / descr->n_phi();
  // TODO: HOW TO DEAL WITH PHI ~ PI?
  double phiMin = phiPar - phiWidth;
  double phiMax = phiPar + phiWidth;
  // --- Fill vecHash ---
  CaloCell_ID::CaloSample sample = descr->getSampling();

  // Cannot do this in initialize: see ATLASRECTS-5012

  std::vector<IdentifierHash> vecHash;
  caloDDM->cellsInZone(etaMin, etaMax, phiMin, phiMax, sample, vecHash);

  // --- Iterate and find closest to track (around 12-15 elements in loop) ---
  double dR2Min{ 999 };
  for (const IdentifierHash& id : vecHash) {
    const CaloCell* cell = caloCellCont->findCell(id);
    if (!cell)
      continue;

    const CaloDetDescrElement* dde = cell->caloDDE();
    if (!dde)
      continue;
    const double etaCell = dde->eta();
    const double phiCell = dde->phi();
    const double dEta = etaPar - etaCell;
    const double dPhi = xAOD::P4Helpers::deltaPhi(phiPar, phiCell);
    const double dR2 = dEta * dEta + dPhi * dPhi;
    neighbourMap[sqrt(dR2)] = cell;
    if (dR2 < dR2Min) {
      dR2Min = dR2;
      result = cell;
    }
  }

  // --- Show deposits near this track (only if debugMode is on) ---
  if (msgLevel(MSG::VERBOSE)) {
    ATH_MSG_INFO("SAMPLE = " << sample);
    for (const std::pair<const double, const CaloCell*>& mapIt : neighbourMap) {
      const CaloCell* cell = mapIt.second;
      double distance = mapIt.first;
      if (cell) {
        ATH_MSG_VERBOSE("dR2 = " << distance << ", energy = " << cell->energy());
      } else {
        ATH_MSG_VERBOSE("dR2 = " << distance << ", NULL pointer!");
      }
    }
  }
  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Book Histograms
///////////////////////////////////////////////////////////////////////////////
StatusCode TrackDepositInCaloTool::bookHistos() {
    ATH_MSG_DEBUG("Booking the ROOT Histos");
    if (SG::getNSlots() > 1) {
      ATH_MSG_FATAL("Filling histograms not supported in MT jobs.");
      return StatusCode::FAILURE;
    }
    ATH_CHECK(service("THistSvc", m_histSvc));
    if (!m_histSvc) return StatusCode::FAILURE;
    m_h = std::make_unique<Hists>();
    ATH_CHECK( m_h->book (*m_histSvc) );
    return StatusCode::SUCCESS;
}

StatusCode TrackDepositInCaloTool::Hists::book (ITHistSvc& histSvc)
{
    m_hDepositLayer12 = new TH1F("hDepositLayer12", "hDepositLayer12", 40, 0, 4000);
    m_hDepositLayer13 = new TH1F("hDepositLayer13", "hDepositLayer13", 40, 0, 4000);
    m_hDepositLayer14 = new TH1F("hDepositLayer14", "hDepositLayer14", 40, 0, 4000);
    m_hParELossEta = new TH2F("hParELossEta", "Parametrized eLoss vs eta", 40, 0, 4000, 30, -3, 3);
    m_hParELossSample = new TH2F("hParELossSample", "Parametrized eLoss vs sample", 40, 0, 4000, 21, 0, 21);
    m_hDeltaEtaLastPar = new TH1F("hDeltaEtaLastPar", "hDeltaEtaLastPar", 50, -2, 2);
    m_hDeltaRadiusLastPar = new TH1F("hDeltaRadiusLastPar", "hDeltaRadiusLastPar", 50, 0, 5000);
    m_hDepositsInCore = new TH1F("hDepositsInCore", "hDepositsInCore", 50, 0, 5000);
    m_hDepositsInCone = new TH1F("hDepositsInCone", "hDepositsInCone", 50, 0, 5000);
    m_hDistDepositsTile = new TH2F("hDistDepositsTile", "hDistDepositsTile", 30, 0.0, 0.3, 30, 0, 4000);
    m_hDistDepositsHEC = new TH2F("hDistDepositsHEC", "hDistDepositsHEC", 30, 0.0, 0.3, 30, 0, 4000);

    m_hEMB1vsdPhi = new TH2F("hEMB1vsdPhi", "hEMB1vsdPhi", 50, -M_PI, M_PI, 50, 0, 500);
    m_hEMB2vsdPhi = new TH2F("hEMB2vsdPhi", "hEMB2vsdPhi", 50, -M_PI, M_PI, 50, 0, 500);
    m_hEMB3vsdPhi = new TH2F("hEMB3vsdPhi", "hEMB3vsdPhi", 50, -M_PI, M_PI, 50, 0, 500);
    m_hEMB1vsdEta = new TH2F("hEMB1vsdEta", "hEMB1vsdEta", 50, -M_PI, M_PI, 50, 0, 500);
    m_hEMB2vsdEta = new TH2F("hEMB2vsdEta", "hEMB2vsdEta", 50, -M_PI, M_PI, 50, 0, 500);
    m_hEMB3vsdEta = new TH2F("hEMB3vsdEta", "hEMB3vsdEta", 50, -M_PI, M_PI, 50, 0, 500);

#define H_CHECK(X) if((X).isFailure()) return StatusCode::FAILURE
    H_CHECK( histSvc.regHist("/AANT/CaloTrkMuId/hDepositLayer12", m_hDepositLayer12) );
    H_CHECK( histSvc.regHist("/AANT/CaloTrkMuId/hDepositLayer13", m_hDepositLayer13) );
    H_CHECK( histSvc.regHist("/AANT/CaloTrkMuId/hDepositLayer14", m_hDepositLayer14) );
    H_CHECK( histSvc.regHist("/AANT/CaloTrkMuId/hParELossSample", m_hParELossSample) );
    H_CHECK( histSvc.regHist("/AANT/CaloTrkMuId/hParELossEta", m_hParELossEta) );
    H_CHECK( histSvc.regHist("/AANT/DetStore/hDeltaEtaLastPar", m_hDeltaEtaLastPar) );
    H_CHECK( histSvc.regHist("/AANT/DetStore/hDeltaRadiusLastPar", m_hDeltaRadiusLastPar) );
    H_CHECK( histSvc.regHist("/AANT/DetStore/hDepositsInCore", m_hDepositsInCore) );
    H_CHECK( histSvc.regHist("/AANT/DetStore/hDepositsInCone", m_hDepositsInCone) );
    H_CHECK( histSvc.regHist("/AANT/DetStore/hDistDepositsTile", m_hDistDepositsTile) );
    H_CHECK( histSvc.regHist("/AANT/DetStore/hDistDepositsHEC", m_hDistDepositsHEC) );
    
    H_CHECK( histSvc.regHist("/AANT/DetStore/hEMB1vsdPhi", m_hEMB1vsdPhi) );
    H_CHECK( histSvc.regHist("/AANT/DetStore/hEMB2vsdPhi", m_hEMB2vsdPhi) );
    H_CHECK( histSvc.regHist("/AANT/DetStore/hEMB3vsdPhi", m_hEMB3vsdPhi) );
    H_CHECK( histSvc.regHist("/AANT/DetStore/hEMB1vsdEta", m_hEMB1vsdEta) );
    H_CHECK( histSvc.regHist("/AANT/DetStore/hEMB2vsdEta", m_hEMB2vsdEta) );
    H_CHECK( histSvc.regHist("/AANT/DetStore/hEMB3vsdEta", m_hEMB3vsdEta) );
#undef H_CHECK

    return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// Get reference to histograms.
///////////////////////////////////////////////////////////////////////////////

TrackDepositInCaloTool::Hists&
TrackDepositInCaloTool::getHists() const
{
  // We earlier checked that no more than one thread is being used.
  Hists* h ATLAS_THREAD_SAFE = m_h.get();
  return *h;
}


///////////////////////////////////////////////////////////////////////////////
// Functions below are under development, these are not used yet.
///////////////////////////////////////////////////////////////////////////////
bool TrackDepositInCaloTool::isInsideDomain(double position, double domainCenter, double domainWidth, bool phiVariable) {
    double halfWidth = domainWidth / 2;
    if (phiVariable) {
        if (std::abs(std::abs(domainCenter) - M_PI) < domainWidth) {
            position += M_PI;
            domainCenter += M_PI;
            if (position > M_PI) { position -= 2 * M_PI; }
            if (domainCenter > M_PI) { domainCenter -= 2 * M_PI; }
        }
    }
    double boundLow = domainCenter - halfWidth;
    double boundHigh = domainCenter + halfWidth;
    if (position < boundLow) return false;
    if (position > boundHigh) return false;
    return true;
}

bool TrackDepositInCaloTool::isInsideCell(const Amg::Vector3D& position, const CaloCell* cell) {
    const CaloDetDescrElement* dde = cell->caloDDE();
    if (!dde) return false;
    if (dde->is_tile()) {
        if (!isInsideDomain(position.z(), dde->z(), dde->dz())) return false;
    } else {
        if (!isInsideDomain(position.eta(), dde->eta(), dde->eta())) return false;
    }
    if (!isInsideDomain(position.phi(), dde->phi(), dde->dphi(), true)) { return false; }
    // if (!isInsideDomain(position.r(), dde->r(), dde->dr()))
    //   return false;
    return true;
}

// inCell
// an alternative version of crossedPhi from CaloCellHelpers, which has a bug

bool TrackDepositInCaloTool::inCell(const CaloCell* cell, const Amg::Vector3D& pos) {
    bool result = std::abs(CaloPhiRange::diff(pos.phi(), cell->phi())) < cell->caloDDE()->dphi() / 2;
    if (cell->caloDDE()->getSubCalo() != CaloCell_ID::TILE)
        result &= std::abs(pos.eta() - cell->eta()) < cell->caloDDE()->deta() / 2;
    else if (cell->caloDDE()->getSampling() != CaloCell_ID::TileBar1)  // still need to deal with tilebar1
        result &= std::abs(pos.z() - cell->z()) < cell->caloDDE()->dz() / 2;
    return result;
}

double TrackDepositInCaloTool::distance(const Amg::Vector3D& p1, const Amg::Vector3D& p2) {
    double diff_x = p1.x() - p2.x();
    double diff_y = p1.y() - p2.y();
    double diff_z = p1.z() - p2.z();
    return std::hypot(diff_x, diff_y, diff_z);
}

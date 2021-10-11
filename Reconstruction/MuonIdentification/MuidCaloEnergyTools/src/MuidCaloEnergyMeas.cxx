/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuidCaloEnergyMeas
//  AlgTool to access the energy deposited by a muon in the
//  calorimeters. The energy deposit is estimated according to
//  the active measurements in the traversed calorimeter cells.
//
///////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuidCaloEnergyTools/MuidCaloEnergyMeas.h"

#include "AthenaKernel/Units.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CaloEvent/CaloCell.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/TileID.h"
#include "CaloUtils/CaloCellList.h"
#include "FourMomUtils/xAODP4Helpers.h"
#include "MuidEvent/CaloMeas.h"
#include "MuidInterfaces/IMuidCaloEnergyParam.h"
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

namespace Units = Athena::Units;

namespace Rec {

    MuidCaloEnergyMeas::MuidCaloEnergyMeas(const std::string& type, const std::string& name, const IInterface* parent) :
        AthAlgTool(type, name, parent),
        m_tileID(nullptr),
        m_emID(nullptr),
        m_hecID(nullptr),
        m_sigmasAboveNoise(4.),
        m_sigmasAboveNoiseCore(1.5) {
        declareInterface<IMuidCaloEnergyMeas>(this);
        declareProperty("NoiseThresInSigmas", m_sigmasAboveNoise);
        declareProperty("NoiseThresInSigmasCore", m_sigmasAboveNoiseCore);

        m_measurementConeTile = 0.15;
        m_measurementConeLArHEC = 0.15;
        m_measurementConeLArEM = 0.075;
        m_isolationConeTile = 0.3;
        m_isolationConeLArHEC = 0.3;
        m_isolationConeLArEM = 0.15;
    }

    MuidCaloEnergyMeas::~MuidCaloEnergyMeas() = default;

    //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

    StatusCode MuidCaloEnergyMeas::initialize() {
        ATH_MSG_INFO("Initializing MuidCaloEnergyMeas AlgTool");

        // retrieve TileID helper and TileIfno from det store
        ATH_CHECK(detStore()->retrieve(m_tileID));
        ATH_MSG_VERBOSE("Accessed TileID helper");

        // retrieve LArEM helper
        ATH_CHECK(detStore()->retrieve(m_emID, "LArEM_ID"));
        ATH_MSG_VERBOSE("Accessed LArEM helper");

        // retrieve LArHEC helper
        ATH_CHECK(detStore()->retrieve(m_hecID, "LArHEC_ID"));
        ATH_MSG_VERBOSE("Accessed LArHEC helper");

        ATH_CHECK(m_caloParamTool.retrieve());
        ATH_CHECK(m_noiseCDOKey.initialize());
        ATH_CHECK(m_cellContainerLocation.initialize());
        ATH_CHECK(m_caloMgrKey.initialize());
        return StatusCode::SUCCESS;
    }

    StatusCode MuidCaloEnergyMeas::finalize() {
        ATH_MSG_INFO("Finalizing MuidCaloEnergyMeas Tool");
        ATH_MSG_INFO(" EM:   selected " << m_totalSelectedEM << " from " << m_totalCoreCellsEM << " cells in core");
        ATH_MSG_INFO(" Tile: selected " << m_totalSelectedTile << " from " << m_totalCoreCellsTile << " cells in core");
        ATH_MSG_INFO(" HEC:  selected " << m_totalSelectedHEC << " from " << m_totalCoreCellsHEC << " cells in core");
        return StatusCode::SUCCESS;
    }
    std::unique_ptr<CaloMeas> MuidCaloEnergyMeas::energyMeasurement(const EventContext& ctx, double etaEM, double phiEM, double etaHad,
                                                                    double phiHad) const {
        SG::ReadHandle<CaloCellContainer> cellContainer(m_cellContainerLocation, ctx);
        if (!cellContainer.isPresent()) {
            ATH_MSG_DEBUG("No calo cell container " << m_cellContainerLocation.key() << ", energy measurement is 0");
            return nullptr;
        }
        if (!cellContainer.isValid()) {
            ATH_MSG_WARNING("Calo cell container " << m_cellContainerLocation.key() << " not valid!");
            return nullptr;
        }

        SG::ReadCondHandle<CaloNoise> noiseHdl{m_noiseCDOKey, ctx};
        if (!noiseHdl.isValid()) {
            ATH_MSG_WARNING("Calo cell container " << m_noiseCDOKey.key() << " not valid!");
            return nullptr;
        }

        const CaloNoise* noiseCDO = *noiseHdl;
        SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey,ctx};
        const CaloDetDescrManager* caloDDMgr = *caloMgrHandle;

        // set measured tile energy, measured sampling fraction and isolation energy into CaloMeas
        std::unique_ptr<CaloMeas> caloMeas = std::make_unique<CaloMeas>();
        energyInCalo(*caloMeas, cellContainer.cptr(), caloDDMgr, noiseCDO, etaHad, phiHad, SubCaloId::TILE);
        isolationEnergy(*caloMeas, cellContainer.cptr(), caloDDMgr, noiseCDO, etaHad, phiHad, SubCaloId::TILE);

        // similar for LArHEC
        energyInCalo(*caloMeas, cellContainer.cptr(), caloDDMgr, noiseCDO, etaHad, phiHad, SubCaloId::LARHEC);
        isolationEnergy(*caloMeas, cellContainer.cptr(), caloDDMgr,noiseCDO, etaHad, phiHad, SubCaloId::LARHEC);

        // and for the em calo
        energyInCalo(*caloMeas, cellContainer.cptr(), caloDDMgr, noiseCDO, etaEM, phiEM, SubCaloId::LAREM);
        isolationEnergy(*caloMeas, cellContainer.cptr(), caloDDMgr, noiseCDO, etaEM, phiEM, SubCaloId::LAREM);

        ATH_MSG_DEBUG(std::setiosflags(std::ios::fixed)
                      << " Tile energy (GeV)  :" << std::setw(8) << std::setprecision(3) << caloMeas->Tile_EnergyMeasured() / Units::GeV
                      << " Material percent:" << std::setw(4) << std::setprecision(0) << 100. * caloMeas->Tile_SamplingFraction()
                      << " ISO :" << std::setw(8) << std::setprecision(3) << caloMeas->Tile_Isolation() / Units::GeV << endmsg
                      << " LArHEC energy (GeV):" << std::setw(8) << std::setprecision(3) << caloMeas->LArHEC_EnergyMeasured() / Units::GeV
                      << " Material percent:" << std::setw(4) << std::setprecision(0) << 100. * caloMeas->LArHEC_SamplingFraction()
                      << " ISO :" << std::setw(8) << std::setprecision(3) << caloMeas->LArHEC_Isolation() / Units::GeV << endmsg
                      << " EM energy          :" << std::setw(8) << std::setprecision(3) << caloMeas->LArEM_EnergyMeasured() / Units::GeV
                      << " first compartment:" << std::setw(8) << std::setprecision(3)
                      << caloMeas->LArEM_FirstCompartmentEnergy() / Units::GeV << " Material percent:" << std::setw(4)
                      << std::setprecision(0) << 100. * caloMeas->LArEM_SamplingFraction() << " ISO :" << std::setw(8)
                      << std::setprecision(3) << caloMeas->LArEM_Isolation() / Units::GeV);

        return caloMeas;
    }

    //<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

    int MuidCaloEnergyMeas::cellCounting(const CaloCellContainer* cellContainer, const CaloDetDescrManager* detMgr, 
                                         const CaloNoise* noiseCDO, double mu_eta, double mu_phi) const {
        // int isubcalo = 2;
        constexpr double lowest_threshold = 4 * 50.;

        CaloCell_ID::SUBCALO iCalo = CaloCell_ID::LAREM;
        CaloCellList myList(detMgr,cellContainer, iCalo);  // Construct the list
        myList.select(mu_eta, mu_phi, 0.2, 0.2);

        int count = 0;
        for (const CaloCell* cell : myList) {
            const double cellEnergy = cell->energy();
            const double noise_rms = noiseCDO->getNoise(cell->ID(), cell->gain());

            if (cellEnergy > lowest_threshold && cellEnergy > noise_rms * m_sigmasAboveNoise) { count += 1; }
        }

        ATH_MSG_DEBUG(" counted " << count << " cells over threshold out of a total of " << myList.ncells() << " cells");

        return count;
    }

    int MuidCaloEnergyMeas::samplingID(const CaloCell* cell, int isubcalo) const {
        if (isubcalo == SubCaloId::TILE) {
            return m_tileID->sample(cell->ID());
        } else if (isubcalo == SubCaloId::LARHEC) {
            return m_hecID->sampling(cell->ID());
        } else if (isubcalo == SubCaloId::LAREM) {
            return m_emID->sampling(cell->ID());
        }

        return -1;
    }

    void MuidCaloEnergyMeas::energyInCalo(CaloMeas& caloMeas, const CaloCellContainer* cellContainer, const CaloDetDescrManager* detMgr, 
                                          const CaloNoise* noiseCDO, double muEta, double muPhi, int isubcalo) const {
        /* -------------------------------------------
           Tile Cal
           sample_number
           0 --> Sampling 1
           1 --> Sampling 2
           2 --> Sampling 3
           3 --> ITC
           ===========================================
           LarHEC calorimeter
           sample_number
           0 --> Sampling 1
           1 --> Sampling 2
           2 --> Sampling 3
           3 --> Sampling 4
           ===========================================
           LarEM calorimeter
           sample_number
           0 --> Presampler
           1 --> Sampling 1
           2 --> Sampling 2
           3 --> Sampling 3
           leadingEnergy is contribution from presampler and first compartment
           -------------------------------------------*/

        double totalEnergy = 0.;
        double leadingEnergy = 0.;
        std::unique_ptr<CaloCellList> myList = nullptr;

        if (isubcalo == SubCaloId::TILE) {
            CaloCell_ID::SUBCALO iCalo = CaloCell_ID::TILE;
            myList = std::make_unique<CaloCellList>(detMgr,cellContainer, iCalo);
            myList->select(muEta, muPhi, m_measurementConeTile, m_measurementConeTile);
        } else if (isubcalo == SubCaloId::LARHEC) {
            CaloCell_ID::SUBCALO iCalo = CaloCell_ID::LARHEC;
            myList = std::make_unique<CaloCellList>(detMgr,cellContainer, iCalo);
            myList->select(muEta, muPhi, m_measurementConeLArHEC, m_measurementConeLArHEC);
        } else if (isubcalo == SubCaloId::LAREM) {
            CaloCell_ID::SUBCALO iCalo = CaloCell_ID::LAREM;
            myList = std::make_unique<CaloCellList>(detMgr,cellContainer, iCalo);
            myList->select(muEta, muPhi, m_measurementConeLArEM, m_measurementConeLArEM);
        }

        int countCoreCells = 0;
        int countSelected = 0;
        bool compartment[4] = {false, false, false, false};
        double measuredSamplings = 0.;

        if (myList && myList->ncells()) {
            // get last cell (as CaloCellList->back() method doesn't work)
            const CaloCell* lastCell = *(myList->end() - 1);

            // flag core in each sampling
            for (int coreSampling = 0; coreSampling != 4; ++coreSampling) {
                const CaloCell* coreCell = nullptr;
                double coreRadius = 0.;
                for (const CaloCell* cell : *myList) {
                    int sampling = samplingID(cell, isubcalo);

                    if (cell != lastCell && sampling != coreSampling) continue;

                    const double deltaEta = cell->eta() - muEta;
                    const double deltaPhi = xAOD::P4Helpers::deltaPhi(cell->phi(), muPhi);
                    const double radius = deltaEta * deltaEta + deltaPhi * deltaPhi;

                    if (sampling == coreSampling) {
                        if (!coreCell || radius < coreRadius) {
                            coreCell = cell;
                            coreRadius = radius;
                        }
                    }

                    if (cell != lastCell || !coreCell) continue;

                    for (const CaloCell* cell2 : *myList) {
                        sampling = samplingID(cell2, isubcalo);
                        if (sampling != coreSampling) continue;

                        double cellEnergy = cell2->energy();
                        double noiseRms = noiseCDO->getNoise(cell2->ID(), cell2->gain());

                        // looser selection for core cell where at least mip is expected
                        bool cellSelected = cellEnergy > m_sigmasAboveNoise * noiseRms;
                        if (cell2 == coreCell && cellEnergy > m_sigmasAboveNoiseCore * noiseRms) cellSelected = true;

                        if (cellSelected) {
                            ++countSelected;
                            totalEnergy += cellEnergy;
                            compartment[coreSampling] = true;
                            if (coreSampling < 2) leadingEnergy += cellEnergy;
                        }
                        if (cell2 == coreCell) {
                            ++countCoreCells;
                            if (isubcalo == SubCaloId::TILE) {
                                ++m_totalCoreCellsTile;
                                if (cellSelected) ++m_totalSelectedTile;
                            } else if (isubcalo == SubCaloId::LARHEC) {
                                ++m_totalCoreCellsHEC;
                                if (cellSelected) ++m_totalSelectedHEC;
                            } else if (isubcalo == SubCaloId::LAREM) {
                                ++m_totalCoreCellsEM;
                                if (cellSelected) ++m_totalSelectedEM;
                            }
                        }
                        if (msgLvl(MSG::DEBUG)) {
                            std::string info = "";
                            std::string type = "  Tile  ";
                            if (isubcalo == SubCaloId::LARHEC) {
                                type = "  LArHEC";
                            } else if (isubcalo == SubCaloId::LAREM) {
                                type = "  EM    ";
                            }
                            if (cellSelected && cell2 == coreCell) {
                                info = "  selected core cell# ";
                            } else if (cellSelected) {
                                info = "  selected cell# ";
                            } else if (cell2 == coreCell) {
                                info = "  cell in core NOT selected";
                            }

                            if (info.empty()) {
                                ATH_MSG_VERBOSE(std::setiosflags(std::ios::fixed)
                                                << type << " Sampling: " << std::setw(1) << coreSampling << " Radius :" << std::setw(6)
                                                << std::setprecision(0) << cell2->caloDDE()->r() << " Eta : " << std::setw(6)
                                                << std::setprecision(2) << cell2->eta() << " Phi : " << std::setw(6) << std::setprecision(2)
                                                << cell2->phi() << " Noise level : " << std::setw(6) << std::setprecision(0) << noiseRms
                                                << " Energy : " << std::setw(7) << std::setprecision(0) << cell2->energy() << info);
                            } else if (cellSelected) {
                                ATH_MSG_DEBUG(std::setiosflags(std::ios::fixed)
                                              << type << " Sampling: " << std::setw(1) << coreSampling << " Radius :" << std::setw(6)
                                              << std::setprecision(0) << cell2->caloDDE()->r() << " Eta : " << std::setw(6)
                                              << std::setprecision(2) << cell2->eta() << " Phi : " << std::setw(6) << std::setprecision(2)
                                              << cell2->phi() << " Noise level : " << std::setw(6) << std::setprecision(0) << noiseRms
                                              << " Energy : " << std::setw(7) << std::setprecision(0) << cell2->energy() << info
                                              << countSelected);
                            } else {
                                ATH_MSG_DEBUG(std::setiosflags(std::ios::fixed)
                                              << type << " Sampling: " << std::setw(1) << coreSampling << " Radius :" << std::setw(6)
                                              << std::setprecision(0) << cell2->caloDDE()->r() << " Eta : " << std::setw(6)
                                              << std::setprecision(2) << cell2->eta() << " Phi : " << std::setw(6) << std::setprecision(2)
                                              << cell2->phi() << " Noise level : " << std::setw(6) << std::setprecision(0) << noiseRms
                                              << " Energy : " << std::setw(7) << std::setprecision(0) << cell2->energy() << info);
                            }
                        }
                    }
                }
            }

            if (msgLvl(MSG::DEBUG)) {
                std::string info = "";
                std::string type = "  Tile  ";
                if (isubcalo == SubCaloId::LARHEC) {
                    type = "  LArHEC";
                } else if (isubcalo == SubCaloId::LAREM) {
                    type = "  EM    ";
                }

                ATH_MSG_DEBUG(type << " at eta = " << muEta << " : selected " << countSelected << " from measured cone with "
                                   << countCoreCells << " cells forming core");
            }

            for (int i = 0; i < 4; ++i)
                if (compartment[i]) measuredSamplings += m_caloParamTool->caloCompartmentDepth(isubcalo, i);

            // store result in caloMeas
            if (isubcalo == SubCaloId::TILE) {
                caloMeas.Tile_EnergyMeasured(totalEnergy);
                caloMeas.Tile_SamplingFraction(measuredSamplings);
            } else if (isubcalo == SubCaloId::LARHEC) {
                caloMeas.LArHEC_EnergyMeasured(totalEnergy);
                caloMeas.LArHEC_SamplingFraction(measuredSamplings);
            } else if (isubcalo == SubCaloId::LAREM) {
                caloMeas.LArEM_EnergyMeasured(totalEnergy);
                caloMeas.LArEM_FirstCompartmentEnergy(leadingEnergy);
                caloMeas.LArEM_SamplingFraction(measuredSamplings);
            }
        }
    }

    void MuidCaloEnergyMeas::isolationEnergy(CaloMeas& caloMeas, const CaloCellContainer* cellContainer, const CaloDetDescrManager* detMgr, 
                                             const CaloNoise* noiseCDO, double muEta, double muPhi, int isubcalo) const {
        double totalEnergy = 0.;

        // SG::ReadCondHandle<CaloNoise> noiseHdl{m_noiseCDOKey, ctx};
        // const CaloNoise* noiseCDO = *noiseHdl;

        std::unique_ptr<CaloCellList> myList;

        if (isubcalo == SubCaloId::TILE) {
            CaloCell_ID::SUBCALO iCalo = CaloCell_ID::TILE;
            myList = std::make_unique<CaloCellList>(detMgr,cellContainer, iCalo);
            myList->select(muEta, muPhi, m_isolationConeTile, m_isolationConeTile);
        } else if (isubcalo == SubCaloId::LARHEC) {
            CaloCell_ID::SUBCALO iCalo = CaloCell_ID::LARHEC;
            myList = std::make_unique<CaloCellList>(detMgr,cellContainer, iCalo);
            myList->select(muEta, muPhi, m_isolationConeLArHEC, m_isolationConeLArHEC);
        } else if (isubcalo == SubCaloId::LAREM) {
            CaloCell_ID::SUBCALO iCalo = CaloCell_ID::LAREM;
            myList = std::make_unique<CaloCellList>(detMgr,cellContainer, iCalo);
            myList->select(muEta, muPhi, m_isolationConeLArEM, m_isolationConeLArEM);
        }

        if (myList && myList->ncells()) {
            // get last cell (as CaloCellList->back() method doesn't work)
            const CaloCell* lastCell = *(myList->end() - 1);

            // flag core in each sampling
            for (int coreSampling = 0; coreSampling != 4; ++coreSampling) {
                const CaloCell* coreCell = nullptr;
                double coreRadius = 0.;
                for (const CaloCell* cell : *myList) {
                    int sampling = samplingID(cell, isubcalo);

                    if (cell != lastCell && sampling != coreSampling) continue;

                    const double deltaEta = cell->eta() - muEta;
                    const double deltaPhi = xAOD::P4Helpers::deltaPhi(cell->phi(), muPhi);
                    const double radius = deltaEta * deltaEta + deltaPhi * deltaPhi;

                    if (sampling == coreSampling) {
                        if (!coreCell || radius < coreRadius) {
                            coreCell = cell;
                            coreRadius = radius;
                        }
                    }

                    if (cell != lastCell || !coreCell) continue;

                    for (const CaloCell* cell2 : *myList) {
                        sampling = samplingID(cell2, isubcalo);
                        if (sampling != coreSampling) continue;

                        double cellEnergy = cell2->energy();
                        double noiseRms = noiseCDO->getNoise(cell2->ID(), cell2->gain());
                        // looser selection for core cell where at least mip is expected
                        bool cellSelected = cellEnergy > m_sigmasAboveNoise * noiseRms;
                        if (cell2 == coreCell && cellEnergy > m_sigmasAboveNoiseCore * noiseRms) cellSelected = true;

                        if (cellSelected) totalEnergy += cellEnergy;
                    }
                }
            }

            // store result in caloMeas
            if (isubcalo == SubCaloId::TILE) {
                caloMeas.Tile_Isolation(totalEnergy);
            } else if (isubcalo == SubCaloId::LARHEC) {
                caloMeas.LArHEC_Isolation(totalEnergy);
            } else if (isubcalo == SubCaloId::LAREM) {
                caloMeas.LArEM_Isolation(totalEnergy);
            }
        }
    }

    double MuidCaloEnergyMeas::energyInTile(const CaloCellContainer* cellContainer, const CaloDetDescrManager* detMgr, 
                                            const CaloNoise* noiseCDO, double mu_eta, double mu_phi, int sample, int cone) const {
        //      Tile Cal
        //      sample_number
        // 	     0 --> Sampling 1
        // 	     1 --> Sampling 2
        // 	     2 --> Sampling 3
        // 	     3 --> ITC

        // int i,j,k;

        double tileTotalEnergy = 0.;
        double tileTestEnergy = 0.;

        CaloCell_ID::SUBCALO iCalo = CaloCell_ID::TILE;
        CaloCellList myList(detMgr,cellContainer, iCalo);  // Construct the list
        if (cone == SubCaloId::TILE) {
            myList.select(mu_eta, mu_phi, 0.15, 0.15);
        } else if (cone == SubCaloId::LARHEC) {
            myList.select(mu_eta, mu_phi, 0.3, 0.3);
        } else {
            myList.select(mu_eta, mu_phi, 0., 0.);
        }

        int count = 0;
        double lowest_threshold = 0.;
        for (const CaloCell* cell : myList) {
            double cellEnergy = cell->energy();
            double noise_rms = noiseCDO->getNoise(cell->ID(), cell->gain());

            if (cellEnergy > lowest_threshold && cellEnergy > noise_rms * m_sigmasAboveNoise && m_tileID->sample(cell->ID()) == sample) {
                count += 1;

                ATH_MSG_DEBUG("Energy : " << cell->energy() << "  Sampling: " << m_tileID->sample(cell->ID())
                                          << "  Radius :" << cell->caloDDE()->r() << "  Eta : " << cell->eta() << "  Phi : " << cell->phi()
                                          << " Noise Level : " << noise_rms);

                tileTotalEnergy += cellEnergy;
            } else {
                tileTestEnergy += cellEnergy;
            }
        }

        ATH_MSG_DEBUG(" counted " << count << " test energy " << tileTestEnergy);

        return tileTotalEnergy;
    }

    double MuidCaloEnergyMeas::energyInLArHEC(const CaloCellContainer* cellContainer, const CaloDetDescrManager* detMgr, 
                                              const CaloNoise* noiseCDO, double mu_eta, double mu_phi, int sample, int cone) const {
        // Look in the LarHEC calorimeter
        // i.e. loop over its cells
        /* sample_number
           0 --> Sampling 1
           1 --> Sampling 2
           2 --> Sampling 3
           3 --> Sampling 4
        */

        // int i,j,k;

        // SG::ReadCondHandle<CaloNoise> noiseHdl{m_noiseCDOKey};
        // const CaloNoise* noiseCDO = *noiseHdl;
        double larhecTotal = 0.;

        CaloCell_ID::SUBCALO iCalo = CaloCell_ID::LARHEC;
        CaloCellList myList(detMgr, cellContainer, iCalo);  // Construct the list
        if (cone == SubCaloId::LARHEC) {
            myList.select(mu_eta, mu_phi, 0.15, 0.15);
        } else if (cone == SubCaloId::LAREM) {
            myList.select(mu_eta, mu_phi, 0.3, 0.3);
        } else {
            myList.select(mu_eta, mu_phi, 0., 0.);
        }

        int count = 0;
        constexpr double lowest_threshold = 4. * 150.;

        for (const CaloCell* cell : myList) {
            double cellEnergy = cell->energy();
            const double noise_rms = noiseCDO->getNoise(cell->ID(), cell->gain());
            if (cellEnergy > lowest_threshold && cellEnergy > noise_rms * m_sigmasAboveNoise && m_hecID->sampling(cell->ID()) == sample) {
                count += 1;
                // new_cell_list.push_backcell;

                ATH_MSG_DEBUG("Energy : " << cell->energy() << "  Sampling: " << m_hecID->sampling(cell->ID())
                                          << "  z  :" << cell->caloDDE()->z() << "  Eta : " << cell->eta() << "  Phi : " << cell->phi()
                                          << " Noise Level : " << noise_rms);

                larhecTotal += cell->energy();
            }
        }

        ATH_MSG_DEBUG("larhec  counted " << count);

        return larhecTotal;
    }

    double MuidCaloEnergyMeas::energyInLArEM(const CaloCellContainer* cellContainer, const CaloDetDescrManager* detMgr, 
                                             const CaloNoise* noiseCDO, double mu_eta, double mu_phi, int sample, int cone) const {
        // Look in the LarEM calorimeter
        // i.e. loop over its cells
        /*
           sample_number
           0 --> Presampler
           1 --> Sampling 1
           2 --> Sampling 2
           3 --> Sampling 3
        */

        // int i,j,k;
        double emTotalEnergy = 0.;

        CaloCell_ID::SUBCALO iCalo = CaloCell_ID::LAREM;
        CaloCellList myList(detMgr, cellContainer, iCalo);  // Construct the list
        if (cone == SubCaloId::LARHEC) {
            myList.select(mu_eta, mu_phi, 0.075, 0.075);  // 0.1 0.1
        } else if (cone == SubCaloId::LAREM) {
            myList.select(mu_eta, mu_phi, 0.15, 0.15);
        } else {
            myList.select(mu_eta, mu_phi, 0., 0.);
        }

        // std::vector<const CaloCell*> new_cell_list;
        int count = 0;

        double lowest_threshold = 4. * 50.;

        for (const CaloCell* cell : myList) {
            double cellEnergy = cell->energy();
            const double noise_rms = noiseCDO->getNoise(cell->ID(), cell->gain());

            if (cellEnergy > lowest_threshold && cellEnergy > noise_rms * m_sigmasAboveNoise && m_emID->sampling(cell->ID()) == sample) {
                count += 1;
                // new_cell_list.push_backcell;

                ATH_MSG_DEBUG("Energy : " << cell->energy() << "  Sampling: " << m_emID->sampling(cell->ID())
                                          << "  Radius :" << cell->caloDDE()->r() << "  z :" << cell->caloDDE()->z()
                                          << "  Eta : " << cell->eta() << "  Phi : " << cell->phi() << " Noise Level : " << noise_rms);

                emTotalEnergy += cell->energy();
            }
        }

        ATH_MSG_DEBUG("larem counted " << count);

        return emTotalEnergy;
    }

}  // namespace Rec

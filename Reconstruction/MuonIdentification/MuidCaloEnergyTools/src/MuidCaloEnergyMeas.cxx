/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuidCaloEnergyMeas
//  AlgTool to access the energy deposited by a muon in the
//  calorimeters. The energy deposit is estimated according to
//  the active measurements in the traversed calorimeter cells.
//
//  (c) ATLAS Combined Muon software
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
#include "MuidEvent/CaloMeas.h"
#include "MuidInterfaces/IMuidCaloEnergyParam.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

namespace Units = Athena::Units;

namespace Rec {

MuidCaloEnergyMeas::MuidCaloEnergyMeas(const std::string& type, const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent),
      m_tileID(0),
      m_emID(0),
      m_hecID(0),
      m_sigmasAboveNoise(4.),
      m_sigmasAboveNoiseCore(1.5),
      m_totalCoreCellsEM(0),
      m_totalCoreCellsHEC(0),
      m_totalCoreCellsTile(0),
      m_totalSelectedEM(0),
      m_totalSelectedHEC(0),
      m_totalSelectedTile(0)
{
    declareInterface<IMuidCaloEnergyMeas>(this);
    declareProperty("NoiseThresInSigmas", m_sigmasAboveNoise);
    declareProperty("NoiseThresInSigmasCore", m_sigmasAboveNoiseCore);

    m_measurementConeTile   = 0.15;
    m_measurementConeLArHEC = 0.15;
    m_measurementConeLArEM  = 0.075;
    m_isolationConeTile     = 0.3;
    m_isolationConeLArHEC   = 0.3;
    m_isolationConeLArEM    = 0.15;
}

MuidCaloEnergyMeas::~MuidCaloEnergyMeas(void) {}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>


StatusCode
MuidCaloEnergyMeas::initialize()
{
    ATH_MSG_INFO("Initializing MuidCaloEnergyMeas AlgTool"
                 << " - package version " << PACKAGE_VERSION);

    // retrieve TileID helper and TileIfno from det store
    if (StatusCode::SUCCESS != detStore()->retrieve(m_tileID)) {
        ATH_MSG_FATAL("Unable to retrieve TileID helper from DetectorStore");
        return StatusCode::FAILURE;
    }
    ATH_MSG_VERBOSE("Accessed TileID helper");

    // retrieve LArEM helper
    if (StatusCode::SUCCESS != detStore()->retrieve(m_emID, "LArEM_ID")) {
        ATH_MSG_FATAL("Unable to retrieve LArEM_ID helper from DetectorStore");
        return StatusCode::FAILURE;
    }
    ATH_MSG_VERBOSE("Accessed LArEM helper");

    // retrieve LArHEC helper
    if (StatusCode::SUCCESS != detStore()->retrieve(m_hecID, "LArHEC_ID")) {
        ATH_MSG_FATAL("Unable to retrieve LArHEC_ID helper from DetectorStore");
        return StatusCode::FAILURE;
    }
    ATH_MSG_VERBOSE("Accessed LArHEC helper");

    ATH_CHECK(m_caloParamTool.retrieve());
    ATH_CHECK(m_noiseCDOKey.initialize());
    ATH_CHECK(m_cellContainerLocation.initialize());

    return StatusCode::SUCCESS;
}

StatusCode
MuidCaloEnergyMeas::finalize()
{
    ATH_MSG_INFO("Finalizing MuidCaloEnergyMeas Tool");
    ATH_MSG_INFO(" EM:   selected " << m_totalSelectedEM << " from " << m_totalCoreCellsEM << " cells in core");

    ATH_MSG_INFO(" Tile: selected " << m_totalSelectedTile << " from " << m_totalCoreCellsTile << " cells in core");

    ATH_MSG_INFO(" HEC:  selected " << m_totalSelectedHEC << " from " << m_totalCoreCellsHEC << " cells in core");
    return StatusCode::SUCCESS;
}

CaloMeas*
MuidCaloEnergyMeas::energyMeasurement(double etaEM, double phiEM, double etaHad, double phiHad) const
{
    SG::ReadHandle<CaloCellContainer> cellContainer(m_cellContainerLocation);
    if (!cellContainer.isPresent()) {
        ATH_MSG_DEBUG("No calo cell container " << m_cellContainerLocation.key() << ", energy measurement is 0");
        return 0;
    }
    if (!cellContainer.isValid()) {
        ATH_MSG_WARNING("Calo cell container " << m_cellContainerLocation.key() << " not valid!");
        return 0;
    }

    // set measured tile energy, measured sampling fraction and isolation energy into CaloMeas
    CaloMeas* caloMeas = new CaloMeas();
    energyInCalo(*caloMeas, cellContainer.cptr(), etaHad, phiHad, 0);
    isolationEnergy(*caloMeas, cellContainer.cptr(), etaHad, phiHad, 0);

    // similar for LArHEC
    energyInCalo(*caloMeas, cellContainer.cptr(), etaHad, phiHad, 1);
    isolationEnergy(*caloMeas, cellContainer.cptr(), etaHad, phiHad, 1);

    // and for the em calo
    energyInCalo(*caloMeas, cellContainer.cptr(), etaEM, phiEM, 2);
    isolationEnergy(*caloMeas, cellContainer.cptr(), etaEM, phiEM, 2);

    if (msgLvl(MSG::DEBUG)) {
        // int cellsOverThreshold	= 0;
        // cellsOverThreshold	= cellCounting(cellContainer,etaEM,phiEM);
        ATH_MSG_DEBUG(std::setiosflags(std::ios::fixed)
                      << " Tile energy (GeV)  :" << std::setw(8) << std::setprecision(3)
                      << caloMeas->Tile_EnergyMeasured() / Units::GeV << " Material percent:" << std::setw(4)
                      << std::setprecision(0) << 100. * caloMeas->Tile_SamplingFraction() << " ISO :" << std::setw(8)
                      << std::setprecision(3) << caloMeas->Tile_Isolation() / Units::GeV << endmsg
                      << " LArHEC energy (GeV):" << std::setw(8) << std::setprecision(3)
                      << caloMeas->LArHEC_EnergyMeasured() / Units::GeV << " Material percent:" << std::setw(4)
                      << std::setprecision(0) << 100. * caloMeas->LArHEC_SamplingFraction() << " ISO :" << std::setw(8)
                      << std::setprecision(3) << caloMeas->LArHEC_Isolation() / Units::GeV << endmsg
                      << " EM energy          :" << std::setw(8) << std::setprecision(3)
                      << caloMeas->LArEM_EnergyMeasured() / Units::GeV << " first compartment:" << std::setw(8)
                      << std::setprecision(3) << caloMeas->LArEM_FirstCompartmentEnergy() / Units::GeV
                      << " Material percent:" << std::setw(4) << std::setprecision(0)
                      << 100. * caloMeas->LArEM_SamplingFraction() << " ISO :" << std::setw(8) << std::setprecision(3)
                      << caloMeas->LArEM_Isolation() / Units::GeV);
    }

    return caloMeas;
}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

int
MuidCaloEnergyMeas::cellCounting(const CaloCellContainer* cellContainer, double mu_eta, double mu_phi) const
{
    // int isubcalo = 2;
    constexpr double lowest_threshold = 4 * 50.;

    SG::ReadCondHandle<CaloNoise> noiseHdl{m_noiseCDOKey};
    const CaloNoise*              noiseCDO = *noiseHdl;

    CaloCell_ID::SUBCALO iCalo = CaloCell_ID::LAREM;
    CaloCellList         myList(cellContainer, iCalo);  // Construct the list
    myList.select(mu_eta, mu_phi, 0.2, 0.2);

    int                         count      = 0;
    CaloCellList::list_iterator ilistfirst = myList.begin();
    CaloCellList::list_iterator ilistlast  = myList.end();
    for (; ilistfirst != ilistlast; ilistfirst++) {
        const double cellEnergy = (*ilistfirst)->energy();
        const double noise_rms  = noiseCDO->getNoise((*ilistfirst)->ID(), (*ilistfirst)->gain());

        if (cellEnergy > lowest_threshold && cellEnergy > noise_rms * m_sigmasAboveNoise) {
            count += 1;
        }
    }

    ATH_MSG_DEBUG(" counted " << count << " cells over threshold out of a total of " << myList.ncells() << " cells");

    return count;
}

void
MuidCaloEnergyMeas::energyInCalo(CaloMeas& caloMeas, const CaloCellContainer* cellContainer, double muEta, double muPhi,
                                 int isubcalo) const
{
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

    SG::ReadCondHandle<CaloNoise> noiseHdl{m_noiseCDOKey};
    const CaloNoise*              noiseCDO = *noiseHdl;

    double        totalEnergy   = 0.;
    double        leadingEnergy = 0.;
    CaloCellList* myList        = 0;

    if (isubcalo == 0) {
        CaloCell_ID::SUBCALO iCalo = CaloCell_ID::TILE;
        myList                     = new CaloCellList(cellContainer, iCalo);
        myList->select(muEta, muPhi, m_measurementConeTile, m_measurementConeTile);
    } else if (isubcalo == 1) {
        CaloCell_ID::SUBCALO iCalo = CaloCell_ID::LARHEC;
        myList                     = new CaloCellList(cellContainer, iCalo);
        myList->select(muEta, muPhi, m_measurementConeLArHEC, m_measurementConeLArHEC);
    } else if (isubcalo == 2) {
        CaloCell_ID::SUBCALO iCalo = CaloCell_ID::LAREM;
        myList                     = new CaloCellList(cellContainer, iCalo);
        myList->select(muEta, muPhi, m_measurementConeLArEM, m_measurementConeLArEM);
    }

    int    countCoreCells    = 0;
    int    countSelected     = 0;
    bool   compartment[4]    = {false, false, false, false};
    double measuredSamplings = 0.;

    if (myList) {
        // get last cell (as CaloCellList->back() method doesn't work)
        const CaloCell*             lastCell = 0;
        CaloCellList::list_iterator cell     = myList->begin();
        for (; cell != myList->end(); ++cell) lastCell = *cell;

        // flag core in each sampling
        for (int coreSampling = 0; coreSampling != 4; ++coreSampling) {
            const CaloCell* coreCell   = 0;
            double          coreRadius = 0.;
            for (cell = myList->begin(); cell != myList->end(); ++cell) {
                int sampling = -1;
                if (isubcalo == 0) {
                    sampling = m_tileID->sample((**cell).ID());
                } else if (isubcalo == 1) {
                    sampling = m_hecID->sampling((**cell).ID());
                } else if (isubcalo == 2) {
                    sampling = m_emID->sampling((**cell).ID());
                }
                if (*cell != lastCell && sampling != coreSampling) continue;

                double deltaEta = (**cell).eta() - muEta;
                double deltaPhi = (**cell).phi() - muPhi;
                double radius   = deltaEta * deltaEta + deltaPhi * deltaPhi;

                if (sampling == coreSampling) {
                    if (!coreCell || radius < coreRadius) {
                        coreCell   = *cell;
                        coreRadius = radius;
                    }
                }

                if (*cell != lastCell || !coreCell) continue;

                for (CaloCellList::list_iterator cell2 = myList->begin(); cell2 != myList->end(); ++cell2) {
                    if (isubcalo == 0) {
                        sampling = m_tileID->sample((**cell2).ID());
                    } else if (isubcalo == 1) {
                        sampling = m_hecID->sampling((**cell2).ID());
                    } else if (isubcalo == 2) {
                        sampling = m_emID->sampling((**cell2).ID());
                    }
                    if (sampling != coreSampling) continue;

                    double cellEnergy = (**cell2).energy();
                    double noiseRms   = noiseCDO->getNoise((**cell2).ID(), (**cell2).gain());

                    // looser selection for core cell where at least mip is expected
                    bool cellSelected = cellEnergy > m_sigmasAboveNoise * noiseRms;
                    if (*cell2 == coreCell && cellEnergy > m_sigmasAboveNoiseCore * noiseRms) cellSelected = true;

                    if (cellSelected) {
                        ++countSelected;
                        totalEnergy += cellEnergy;
                        compartment[coreSampling] = true;
                        if (coreSampling < 2) leadingEnergy += cellEnergy;
                    }
                    if (*cell2 == coreCell) {
                        ++countCoreCells;
                        if (isubcalo == 0) {
                            ++m_totalCoreCellsTile;
                            if (cellSelected) ++m_totalSelectedTile;
                        } else if (isubcalo == 1) {
                            ++m_totalCoreCellsHEC;
                            if (cellSelected) ++m_totalSelectedHEC;
                        } else if (isubcalo == 2) {
                            ++m_totalCoreCellsEM;
                            if (cellSelected) ++m_totalSelectedEM;
                        }
                    }
                    if (msgLvl(MSG::DEBUG)) {
                        std::string info = "";
                        std::string type = "  Tile  ";
                        if (isubcalo == 1) {
                            type = "  LArHEC";
                        } else if (isubcalo == 2) {
                            type = "  EM    ";
                        }
                        if (cellSelected && *cell2 == coreCell) {
                            info = "  selected core cell# ";
                        } else if (cellSelected) {
                            info = "  selected cell# ";
                        } else if (*cell2 == coreCell) {
                            info = "  cell in core NOT selected";
                        }

                        if (info == "") {
                            ATH_MSG_VERBOSE(std::setiosflags(std::ios::fixed)
                                            << type << " Sampling: " << std::setw(1) << coreSampling << " Radius :"
                                            << std::setw(6) << std::setprecision(0) << (**cell2).caloDDE()->r()
                                            << " Eta : " << std::setw(6) << std::setprecision(2) << (**cell2).eta()
                                            << " Phi : " << std::setw(6) << std::setprecision(2) << (**cell2).phi()
                                            << " Noise level : " << std::setw(6) << std::setprecision(0) << noiseRms
                                            << " Energy : " << std::setw(7) << std::setprecision(0)
                                            << (**cell2).energy() << info);
                        } else if (cellSelected) {
                            ATH_MSG_DEBUG(std::setiosflags(std::ios::fixed)
                                          << type << " Sampling: " << std::setw(1) << coreSampling << " Radius :"
                                          << std::setw(6) << std::setprecision(0) << (**cell2).caloDDE()->r()
                                          << " Eta : " << std::setw(6) << std::setprecision(2) << (**cell2).eta()
                                          << " Phi : " << std::setw(6) << std::setprecision(2) << (**cell2).phi()
                                          << " Noise level : " << std::setw(6) << std::setprecision(0) << noiseRms
                                          << " Energy : " << std::setw(7) << std::setprecision(0) << (**cell2).energy()
                                          << info << countSelected);
                        } else {
                            ATH_MSG_DEBUG(std::setiosflags(std::ios::fixed)
                                          << type << " Sampling: " << std::setw(1) << coreSampling << " Radius :"
                                          << std::setw(6) << std::setprecision(0) << (**cell2).caloDDE()->r()
                                          << " Eta : " << std::setw(6) << std::setprecision(2) << (**cell2).eta()
                                          << " Phi : " << std::setw(6) << std::setprecision(2) << (**cell2).phi()
                                          << " Noise level : " << std::setw(6) << std::setprecision(0) << noiseRms
                                          << " Energy : " << std::setw(7) << std::setprecision(0) << (**cell2).energy()
                                          << info);
                        }
                    }
                }
            }
        }

        if (msgLvl(MSG::DEBUG)) {
            std::string info = "";
            std::string type = "  Tile  ";
            if (isubcalo == 1) {
                type = "  LArHEC";
            } else if (isubcalo == 2) {
                type = "  EM    ";
            }

            ATH_MSG_DEBUG(type << " at eta = " << muEta << " : selected " << countSelected
                               << " from measured cone with " << countCoreCells << " cells forming core");
        }

        delete myList;

        for (int i = 0; i < 4; ++i)
            if (compartment[i]) measuredSamplings += m_caloParamTool->caloCompartmentDepth(isubcalo, i);

        // store result in caloMeas
        if (isubcalo == 0) {
            caloMeas.Tile_EnergyMeasured(totalEnergy);
            caloMeas.Tile_SamplingFraction(measuredSamplings);
        } else if (isubcalo == 1) {
            caloMeas.LArHEC_EnergyMeasured(totalEnergy);
            caloMeas.LArHEC_SamplingFraction(measuredSamplings);
        } else if (isubcalo == 2) {
            caloMeas.LArEM_EnergyMeasured(totalEnergy);
            caloMeas.LArEM_FirstCompartmentEnergy(leadingEnergy);
            caloMeas.LArEM_SamplingFraction(measuredSamplings);
        }
    }
}

void
MuidCaloEnergyMeas::isolationEnergy(CaloMeas& caloMeas, const CaloCellContainer* cellContainer, double muEta,
                                    double muPhi, int isubcalo) const
{
    double        totalEnergy = 0.;
    CaloCellList* myList      = 0;

    SG::ReadCondHandle<CaloNoise> noiseHdl{m_noiseCDOKey};
    const CaloNoise*              noiseCDO = *noiseHdl;

    if (isubcalo == 0) {
        CaloCell_ID::SUBCALO iCalo = CaloCell_ID::TILE;
        myList                     = new CaloCellList(cellContainer, iCalo);
        myList->select(muEta, muPhi, m_isolationConeTile, m_isolationConeTile);
    } else if (isubcalo == 1) {
        CaloCell_ID::SUBCALO iCalo = CaloCell_ID::LARHEC;
        myList                     = new CaloCellList(cellContainer, iCalo);
        myList->select(muEta, muPhi, m_isolationConeLArHEC, m_isolationConeLArHEC);
    } else if (isubcalo == 2) {
        CaloCell_ID::SUBCALO iCalo = CaloCell_ID::LAREM;
        myList                     = new CaloCellList(cellContainer, iCalo);
        myList->select(muEta, muPhi, m_isolationConeLArEM, m_isolationConeLArEM);
    }

    if (myList) {
        // get last cell (as CaloCellList->back() method doesn't work)
        const CaloCell*             lastCell = 0;
        CaloCellList::list_iterator cell     = myList->begin();
        for (; cell != myList->end(); ++cell) lastCell = *cell;

        // flag core in each sampling
        for (int coreSampling = 0; coreSampling != 4; ++coreSampling) {
            const CaloCell* coreCell   = 0;
            double          coreRadius = 0.;
            for (cell = myList->begin(); cell != myList->end(); ++cell) {
                int sampling = -1;
                if (isubcalo == 0) {
                    sampling = m_tileID->sample((**cell).ID());
                } else if (isubcalo == 1) {
                    sampling = m_hecID->sampling((**cell).ID());
                } else if (isubcalo == 2) {
                    sampling = m_emID->sampling((**cell).ID());
                }
                if (*cell != lastCell && sampling != coreSampling) continue;

                double deltaEta = (**cell).eta() - muEta;
                double deltaPhi = (**cell).phi() - muPhi;
                double radius   = deltaEta * deltaEta + deltaPhi * deltaPhi;

                if (sampling == coreSampling) {
                    if (!coreCell || radius < coreRadius) {
                        coreCell   = *cell;
                        coreRadius = radius;
                    }
                }

                if (*cell != lastCell || !coreCell) continue;

                for (CaloCellList::list_iterator cell2 = myList->begin(); cell2 != myList->end(); ++cell2) {
                    if (isubcalo == 0) {
                        sampling = m_tileID->sample((**cell2).ID());
                    } else if (isubcalo == 1) {
                        sampling = m_hecID->sampling((**cell2).ID());
                    } else if (isubcalo == 2) {
                        sampling = m_emID->sampling((**cell2).ID());
                    }
                    if (sampling != coreSampling) continue;

                    double cellEnergy = (**cell2).energy();
                    double noiseRms   = noiseCDO->getNoise((**cell2).ID(), (**cell2).gain());
                    // looser selection for core cell where at least mip is expected
                    bool cellSelected = cellEnergy > m_sigmasAboveNoise * noiseRms;
                    if (*cell2 == coreCell && cellEnergy > m_sigmasAboveNoiseCore * noiseRms) cellSelected = true;

                    if (cellSelected) totalEnergy += cellEnergy;
                }
            }
        }

        // store result in caloMeas
        if (isubcalo == 0) {
            caloMeas.Tile_Isolation(totalEnergy);
        } else if (isubcalo == 1) {
            caloMeas.LArHEC_Isolation(totalEnergy);
        } else if (isubcalo == 2) {
            caloMeas.LArEM_Isolation(totalEnergy);
        }
    }

    delete myList;
}

double
MuidCaloEnergyMeas::energyInTile(const CaloCellContainer* cellContainer, double mu_eta, double mu_phi, int sample,
                                 int cone) const
{
    //      Tile Cal
    //      sample_number
    // 	     0 --> Sampling 1
    // 	     1 --> Sampling 2
    // 	     2 --> Sampling 3
    // 	     3 --> ITC

    // int i,j,k;

    SG::ReadCondHandle<CaloNoise> noiseHdl{m_noiseCDOKey};
    const CaloNoise*              noiseCDO = *noiseHdl;

    double tileTotalEnergy = 0.;
    double tileTestEnergy  = 0.;

    CaloCell_ID::SUBCALO iCalo = CaloCell_ID::TILE;
    CaloCellList         myList(cellContainer, iCalo);  // Construct the list
    if (cone == 1) {
        myList.select(mu_eta, mu_phi, 0.15, 0.15);
    } else if (cone == 2) {
        myList.select(mu_eta, mu_phi, 0.3, 0.3);
    } else {
        myList.select(mu_eta, mu_phi, 0., 0.);
    }

    CaloCellList::list_iterator ilistfirst = myList.begin();
    CaloCellList::list_iterator ilistlast  = myList.end();

    int    count            = 0;
    double lowest_threshold = 0.;
    for (; ilistfirst != ilistlast; ilistfirst++) {
        double cellEnergy = (*ilistfirst)->energy();
        double noise_rms  = noiseCDO->getNoise((*ilistfirst)->ID(), (*ilistfirst)->gain());

        if (cellEnergy > lowest_threshold && cellEnergy > noise_rms * m_sigmasAboveNoise
            && m_tileID->sample((*ilistfirst)->ID()) == sample)
        {
            count += 1;

            ATH_MSG_DEBUG("Energy : " << (*ilistfirst)->energy()
                                      << "  Sampling: " << m_tileID->sample((*ilistfirst)->ID()) << "  Radius :"
                                      << (*ilistfirst)->caloDDE()->r() << "  Eta : " << (*ilistfirst)->eta()
                                      << "  Phi : " << (*ilistfirst)->phi() << " Noise Level : " << noise_rms);

            tileTotalEnergy += cellEnergy;
        } else {
            tileTestEnergy += cellEnergy;
        }
    }

    ATH_MSG_DEBUG(" counted " << count << " test energy " << tileTestEnergy);

    return tileTotalEnergy;
}

double
MuidCaloEnergyMeas::energyInLArHEC(const CaloCellContainer* cellContainer, double mu_eta, double mu_phi, int sample,
                                   int cone) const
{
    // Look in the LarHEC calorimeter
    // i.e. loop over its cells
    /* sample_number
       0 --> Sampling 1
       1 --> Sampling 2
       2 --> Sampling 3
       3 --> Sampling 4
    */

    // int i,j,k;

    SG::ReadCondHandle<CaloNoise> noiseHdl{m_noiseCDOKey};
    const CaloNoise*              noiseCDO    = *noiseHdl;
    double                        larhecTotal = 0.;

    CaloCell_ID::SUBCALO iCalo = CaloCell_ID::LARHEC;
    CaloCellList         myList(cellContainer, iCalo);  // Construct the list
    if (cone == 1) {
        myList.select(mu_eta, mu_phi, 0.15, 0.15);
    } else if (cone == 2) {
        myList.select(mu_eta, mu_phi, 0.3, 0.3);
    } else {
        myList.select(mu_eta, mu_phi, 0., 0.);
    }

    CaloCellList::list_iterator ilistfirst = myList.begin();
    CaloCellList::list_iterator ilistlast  = myList.end();

    // std::vector<const CaloCell*> new_cell_list;
    int              count            = 0;
    constexpr double lowest_threshold = 4. * 150.;

    for (; ilistfirst != ilistlast; ilistfirst++) {
        double       cellEnergy = (*ilistfirst)->energy();
        const double noise_rms  = noiseCDO->getNoise((*ilistfirst)->ID(), (*ilistfirst)->gain());
        if (cellEnergy > lowest_threshold && cellEnergy > noise_rms * m_sigmasAboveNoise
            && m_hecID->sampling((*ilistfirst)->ID()) == sample)
        {
            count += 1;
            // new_cell_list.push_back(*ilistfirst);

            ATH_MSG_DEBUG("Energy : " << (*ilistfirst)->energy()
                                      << "  Sampling: " << m_hecID->sampling((*ilistfirst)->ID())
                                      << "  z  :" << (*ilistfirst)->caloDDE()->z() << "  Eta : " << (*ilistfirst)->eta()
                                      << "  Phi : " << (*ilistfirst)->phi() << " Noise Level : " << noise_rms);

            larhecTotal += (*ilistfirst)->energy();
        }
    }

    ATH_MSG_DEBUG("larhec  counted " << count);

    return larhecTotal;
}

double
MuidCaloEnergyMeas::energyInLArEM(const CaloCellContainer* cellContainer, double mu_eta, double mu_phi, int sample,
                                  int cone) const
{
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

    SG::ReadCondHandle<CaloNoise> noiseHdl{m_noiseCDOKey};
    const CaloNoise*              noiseCDO = *noiseHdl;

    double emTotalEnergy = 0.;

    CaloCell_ID::SUBCALO iCalo = CaloCell_ID::LAREM;
    CaloCellList         myList(cellContainer, iCalo);  // Construct the list
    if (cone == 1) {
        myList.select(mu_eta, mu_phi, 0.075, 0.075);  // 0.1 0.1
    } else if (cone == 2) {
        myList.select(mu_eta, mu_phi, 0.15, 0.15);
    } else {
        myList.select(mu_eta, mu_phi, 0., 0.);
    }
    CaloCellList::list_iterator ilistfirst = myList.begin();
    CaloCellList::list_iterator ilistlast  = myList.end();

    // std::vector<const CaloCell*> new_cell_list;
    int count = 0;

    double lowest_threshold = 4. * 50.;

    for (; ilistfirst != ilistlast; ilistfirst++) {
        double       cellEnergy = (*ilistfirst)->energy();
        const double noise_rms  = noiseCDO->getNoise((*ilistfirst)->ID(), (*ilistfirst)->gain());

        if (cellEnergy > lowest_threshold && cellEnergy > noise_rms * m_sigmasAboveNoise
            && m_emID->sampling((*ilistfirst)->ID()) == sample)
        {
            count += 1;
            // new_cell_list.push_back(*ilistfirst);

            ATH_MSG_DEBUG("Energy : " << (*ilistfirst)->energy()
                                      << "  Sampling: " << m_emID->sampling((*ilistfirst)->ID())
                                      << "  Radius :" << (*ilistfirst)->caloDDE()->r()
                                      << "  z :" << (*ilistfirst)->caloDDE()->z() << "  Eta : " << (*ilistfirst)->eta()
                                      << "  Phi : " << (*ilistfirst)->phi() << " Noise Level : " << noise_rms);

            emTotalEnergy += (*ilistfirst)->energy();
        }
    }

    ATH_MSG_DEBUG("larem counted " << count);

    return emTotalEnergy;
}

}  // namespace Rec

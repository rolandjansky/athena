/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////
// author: Shikma Bressler sshikma@techunix.technion.ac.il
//
//Following the method presented in ATL-COM-TILECAL-2009-028 by
//  S. Hellman, M. Johansen, R. Mackeprang, P. Mermod, D. Milstead, C. Ohm, A. Solodkov
//
///////////////////////////////////
#include "MuGirlStau/StauTileCal.h"
//#include "Particle/TrackParticle.h"
#include "RecoToolInterfaces/IParticleCaloCellAssociationTool.h"
#include "ParticleCaloExtension/ParticleCellAssociation.h"
#include "CaloEvent/CaloCellContainer.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"

//static const int nLayers = 15;    // Number of layers in the calorimeter (for beta tile)
static const int nEbins  = 44;    // Number of energy bins in calorimeter calibration (for beta tile)
static const int nEbinsDeca = 20; // Number of energy bins per decade in calorimeter calibration (for beta tile)
static const double Emin = 100.0; // Lowest cell energy (in MeV) used in calibration (for beta tile)

//================ Constructor =================================================

MuGirlNS::StauTileCal::StauTileCal(StauTool* pStauTool, MsgStream& log,
                                   CLHEP::HepRandomEngine& randEngine,
        const xAOD::TrackParticle* trkParticle) :
                m_pStau(pStauTool),
                m_log(log),
                m_pTrkParticle(trkParticle),
                m_beta(StauBetaDefault),
                m_avgBeta(StauBetaDefault),
                m_rmsBeta(-1.),
                m_pCaloCells(NULL),
                m_hasCells(false),
                m_pCalibration(NULL),
                m_randEngine (randEngine)
{
    LOG_DEBUG << "trkParticle=" << trkParticle << endreq;
    //std::vector<const CaloCell*> cellsOnTrack = m_pToCalo->getCellsOnTrack(m_pTrkParticle,tile1,tile3,true);
    /*
     std::vector<const CaloCell*> cellsOnTrack = m_pToCalo->getCellsOnTrack(m_pTrkParticle,em1,hec3,true);
     initCaloCells(cellsOnTrack);
     */
    m_pCaloCells = new StauTileCalCells();
    if (m_pTrkParticle != NULL)
    {
        const Rec::ParticleCellAssociation* association = NULL;
        if (m_pStau->caloCellAssociationTool()->particleCellAssociation(*m_pTrkParticle, association, 0.1))
        {
            int intersected = 0, passed_enery_cut = 0;
            double energy_cut = m_pStau->tileEnergyCut() * 1000;
            for (auto caloCell : association->data())
            {
                if (association->cellIntersection(*caloCell) != NULL)
                {
                    intersected++;
                    if (caloCell->e() >= energy_cut)
                        passed_enery_cut++;
                }
            }
            LOG_DEBUG << "total cells: " << association->data().size()
                      << ", intersected cells: " << intersected
                      << ", passed energy cut " << energy_cut << ": " << passed_enery_cut
                      << endreq;
            initCaloCells(association);
        }
        else
            LOG_DEBUG << "failed to get Calo cells" << endreq;
    }
    LOG_DEBUG << "done" << endreq;
}

MuGirlNS::StauTileCal::StauTileCal(StauTool* pStauTool, MsgStream& log,
                                   CLHEP::HepRandomEngine& randEngine,
                                   const Trk::Track* /*trk*/) :
                m_pStau(pStauTool),
                m_log(log),
                m_pTrkParticle(NULL),
                m_beta(StauBetaDefault),
                m_avgBeta(StauBetaDefault),
                m_rmsBeta(-1.),
                m_pCaloCells(NULL),
                m_hasCells(false),
                m_pCalibration(nullptr),
                m_randEngine(randEngine)
{
    m_pCaloCells = new StauTileCalCells();
}

static int BinFromE(double E)
{
    if (E < Emin ) return 0;
    else if (E > Emin * pow(10.0, double(nEbins - 2) / double(nEbinsDeca))) return nEbins - 1;
    else return int(log10(E / Emin) * nEbinsDeca + 1);
}

static int LayerFromSampling(int sampling)
{
    // Define array with layer values for the samplings
    // Samplings for composite layers: Layer 12: {12,18}. Layer 13: {13,19}. Layer 14: {14,15,16,20}.
    static int layer[21] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, -1, 12, 13, 14};
    return layer[sampling];
}

void MuGirlNS::StauTileCal::initCaloCells(const Rec::ParticleCellAssociation* association)
{
    if (m_pStau->doCalibration()) m_pCalibration = m_pStau->calibration().getTileCalibration();
    double energy_cut = m_pStau->tileEnergyCut() * 1000;
    for (auto caloCell : association->data())
    {
        if (association->cellIntersection(*caloCell) == NULL) continue;
        if (caloCell->e() < energy_cut) continue;

        double shift = 0;
        double error = TILECALRESOLUTION;
        if (m_pStau->doCalibration())
        {
            int id = caloCell->ID().get_identifier32().get_compact() & 0xFFFF0000;
            auto dde = caloCell->caloDDE();
            if (dde == NULL) // No cell location
            {
                LOG_DEBUG << "could not find CaloDet Description Element for track cell " << hex << id << endreq;
                continue;
            }
            int layer = LayerFromSampling(dde->getSampling());
            int ebin = BinFromE(caloCell->e());

            auto itCalib = m_pCalibration->find(std::make_pair(layer, ebin));
            if (itCalib == m_pCalibration->end()) // No cell calibration
            {
                LOG_DEBUG << "(layer,bin)=(" << layer << "," << ebin << ") not in calibration map"
                          << ", cell energy: " << caloCell->e()
                          << endreq;
                continue;
            }
            error = (itCalib->second.error > 0) ? itCalib->second.error : TILECALRESOLUTION;
            if (m_pStau->isData())
                shift = itCalib->second.timeShift; //shift
            else
            { //smear
              // double error = itCalib->second.error;
                shift = m_pStau->tileSmearFactor() * CLHEP::RandGaussZiggurat::shoot(&m_randEngine, 0, error); //Sofia: Low 0.5 MID 0.9 HIGH 1.4
            }
        }

        auto tileCell = new StauTileCalCell();
        tileCell->x = caloCell->x();
        tileCell->y = caloCell->y();
        tileCell->z = caloCell->z();
        tileCell->error = error;
        tileCell->distance = sqrt(
                caloCell->x() * caloCell->x() + caloCell->y() * caloCell->y()
                        + caloCell->z() * caloCell->z()) / 1000.;
        tileCell->recoTime = caloCell->time() - shift;
        tileCell->energy = caloCell->e();
        tileCell->id = caloCell->ID();
        m_pCaloCells->push_back(tileCell);

        double muonToF = tileCell->distance / SPEEDOFLIGHT;
        StauHit hit(CALO_STAU_HIT, caloCell->time() + muonToF - shift, caloCell->x(), caloCell->y(),
                caloCell->z(), caloCell->ID(), caloCell->e(), error, shift);
        m_hits.push_back(hit);
    }
    if (!m_pCaloCells->empty())
    {
        m_hasCells = true;
        averageBeta();
    }
}

//================ Destructor =================================================

MuGirlNS::StauTileCal::~StauTileCal()
{
    this->clear();
}

void MuGirlNS::StauTileCal::initStepData(TileCalStepData* tileCalStepData, double beta,
        double tTrack)
{
    tileCalStepData->beta = beta;
    tileCalStepData->chi2 = 0.;
    tileCalStepData->dof = 0.;
    tileCalStepData->numOfCells = 0.;
    tileCalStepData->tTrack = tTrack;
}

void MuGirlNS::StauTileCal::processTileCalWithBeta(double currentBeta,
        TileCalStepData* tileCalStepData)
{
    LOG_VERBOSE << "currentBeta=" << currentBeta << endreq;
    double chi2 = 0.;
    for (auto cell : *m_pCaloCells)
    {
        double stauToF = cell->distance / (currentBeta * SPEEDOFLIGHT);
        double muonToF = cell->distance / SPEEDOFLIGHT;
        double calcTime = stauToF - muonToF;
        double error = cell->error;
        chi2 += (cell->recoTime - calcTime) * (cell->recoTime - calcTime) / (error * error);
    }
    tileCalStepData->chi2 = chi2;
    tileCalStepData->numOfCells = (double) m_pCaloCells->size();
    tileCalStepData->dof = (double) m_pCaloCells->size();
}

void MuGirlNS::StauTileCal::clearStepData(TileCalStepData* tileCalStepData)
{
    if (tileCalStepData != NULL) delete tileCalStepData;
}

void MuGirlNS::StauTileCal::printStepData(TileCalStepData* tileCalStepData)
{
    if (tileCalStepData != NULL)
        LOG_VERBOSE << "tileCal data: beta=" << tileCalStepData->beta << " chi2="
                    << tileCalStepData->chi2 << " #cells=" << tileCalStepData->numOfCells << endreq;
}

void MuGirlNS::StauTileCal::clear()
{
    if (m_pCaloCells != NULL)
    {
        for (auto cell : *m_pCaloCells)
            delete cell;
        m_pCaloCells->clear();
        delete m_pCaloCells;
        m_pCaloCells=nullptr;
    }
    m_beta = StauBetaDefault;
    m_avgBeta = StauBetaDefault;
    m_rmsBeta = -1.;
    m_hits.clear();
}
void MuGirlNS::StauTileCal::averageBeta()
{
    //Average
    double up = 0;
    double dwn = 0;

    for (auto cell : *m_pCaloCells)
    {
        double muonToF = cell->distance / SPEEDOFLIGHT;
        double measureToF = cell->recoTime + muonToF;
        double distance = cell->distance;
        double invBeta = (measureToF * SPEEDOFLIGHT) / distance;
        double tofError = cell->error;
        double invBetaError = tofError * SPEEDOFLIGHT / distance;
        up += invBeta / (invBetaError * invBetaError);
        dwn += (1 / (invBetaError * invBetaError));
    }
    double avgInvBeta = (dwn != 0) ? up / dwn : 1. / StauBetaDefault;
    m_avgBeta = 1. / avgInvBeta; // 1/(up/dwn)

    //RMS
    up = 0;
    dwn = 0;

    for (auto cell : *m_pCaloCells)
    {
        double muonToF = cell->distance / SPEEDOFLIGHT;
        double measureToF = cell->recoTime + muonToF;
        double distance = cell->distance;
        double invBeta = SPEEDOFLIGHT * measureToF / distance;
        double deltaInvBeta = invBeta - avgInvBeta;
        double tofError = cell->error;
        double invBetaError = tofError * SPEEDOFLIGHT / distance;
        //double measuredBetaError = distance * m_inverseSpeedOfLight * (1. / (measureToF * measureToF)) * tofError;
        up += deltaInvBeta * deltaInvBeta / (invBetaError * invBetaError);
        dwn += (1. / (invBetaError * invBetaError));
    }
    double rmsInvBeta = (dwn != 0) ? sqrt(up / dwn) : -1;
    m_rmsBeta = (dwn != 0) ? m_avgBeta * m_avgBeta * rmsInvBeta : -1;
}

void MuGirlNS::StauTileCal::processTileCalWithTTrack(double tTrack,
        TileCalStepData* tileCalStepData, double beta)
{
    LOG_VERBOSE << "tTrack=" << tTrack << endreq;
    double currentBeta = beta;
    double stauToF;
    double muonToF;
    double calcTime;
    double error;
    double chi2 = 0.;
    for (auto cell : *m_pCaloCells)
    {
        stauToF = cell->distance / (currentBeta * SPEEDOFLIGHT) + tTrack;
        muonToF = cell->distance / SPEEDOFLIGHT;
        calcTime = stauToF - muonToF;
        error = cell->error;
        chi2 += (cell->recoTime - calcTime) * (cell->recoTime - calcTime) / (error * error);
    }
    tileCalStepData->chi2 = chi2;
    tileCalStepData->numOfCells = (double) m_pCaloCells->size();
    tileCalStepData->dof = (double) m_pCaloCells->size();
}

unsigned int MuGirlNS::StauTileCal::tileMinIsAt()
{
    // maybe this can be pre-calculated while function is being evaluated
    FcnStepsData firstSteps = *(m_pStau->fcnStepsData());
    unsigned int iMinLoc = 0;
    double minChi2 = 100000.;

    for (unsigned int i = 0; i < firstSteps.size(); i++)
    {
        auto pStepData = firstSteps[i];
        auto pTileCalStepData = pStepData->tileCalData;
        if (pTileCalStepData == NULL) return 0;
        double chi2 = pTileCalStepData->chi2;

        if (chi2 < minChi2)
        {
            minChi2 = chi2;
            iMinLoc = i;
        }
    }

    return iMinLoc;
}

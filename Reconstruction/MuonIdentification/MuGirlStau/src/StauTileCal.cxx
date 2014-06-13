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
#include "TRandom3.h"

//================ Constructor =================================================

MuGirlNS::StauTileCal::StauTileCal(StauTool* pStauTool, MsgStream& log, const xAOD::TrackParticle* trkParticle):
    m_pStau(pStauTool),
    m_log(log),
    m_pTrkParticle(trkParticle),
    m_beta(StauBetaDefault),
    m_avgBeta(StauBetaDefault),
    m_rmsBeta(-1.),
    m_pToCalo(pStauTool->toCaloTool()),
    m_pCaloCells(NULL),
    m_hasCells(false)
{
    //std::vector<const CaloCell*> cellsOnTrack = m_pToCalo->getCellsOnTrack(m_pTrkParticle,tile1,tile3,true);
/*
    std::vector<const CaloCell*> cellsOnTrack = m_pToCalo->getCellsOnTrack(m_pTrkParticle,em1,hec3,true);
    initCaloCells(cellsOnTrack);
*/
}    

MuGirlNS::StauTileCal::StauTileCal(StauTool* pStauTool, MsgStream& log, const Trk::Track* /*trk*/):
    m_pStau(pStauTool),
    m_log(log),
    m_pTrkParticle(NULL),
    m_pToCalo(pStauTool->toCaloTool()),
    m_pCaloCells(NULL),
    m_hasCells(false)
{

  //const Trk::VxCandidate* pVxCandidate = 0 ;
   
/*
    Rec::TrackParticle* tp = m_pStau->particleCreator()->createParticle(trk,pVxCandidate,Trk::NoVtx);
    //std::vector<const CaloCell*> cellsOnTrack = m_pToCalo->getCellsOnTrack(tp,tile1,tile3,true);
    std::vector<const CaloCell*> cellsOnTrack = m_pToCalo->getCellsOnTrack(tp,em1,hec3,true);
    initCaloCells(cellsOnTrack);
*/
}    

void MuGirlNS::StauTileCal::initCaloCells(std::vector<const CaloCell*> cellsOnTrack)
{ 
    m_pCaloCells = new StauTileCalCells();
    if(m_pStau->doCalibration()) m_pCalibration = m_pStau->calibration().getTileCalibration();
    for(std::vector<const CaloCell*>::const_iterator it = cellsOnTrack.begin(); it!=cellsOnTrack.end(); it++)
    {
        const CaloCell* caloCell = *it;

        if(caloCell->e() < m_pStau->tileEnergyCut()*1000) continue;

        double shift = 0;
        double error = TILECALRESOLUTION; 
        if(m_pStau->doCalibration())
        {
             int id = caloCell->ID().get_identifier32().get_compact() & 0xFFFF0000;

             std::map<int, StauCalibrationParameters >::iterator itCalib = m_pCalibration->find(id);
             if(itCalib == m_pCalibration->end())
             {
                 continue; //don't use this cell if it doesn't have calibration
             }else
             {
                 error = (itCalib->second.error > 0) ? itCalib->second.error : TILECALRESOLUTION;
                 if(m_pStau->isData()) shift = itCalib->second.timeShift + m_pStau->phaseShift();//shift
                 else
                 {//smear
                     // double error = itCalib->second.error;
                      TRandom3 rand(0);
                      shift = m_pStau->tileSmearFactor() * rand.Gaus(0,error); //Sofia: Low 0.5 MID 0.9 HIGH 1.4
                 }
             }
        }

	StauTileCalCell* tileCell = new StauTileCalCell();
	tileCell->x = caloCell->x();
	tileCell->y = caloCell->x();
	tileCell->z = caloCell->x();
	tileCell->error = error;
	tileCell->distance = sqrt(caloCell->x()*caloCell->x()+caloCell->y()*caloCell->y()+caloCell->z()*caloCell->z())/1000.;
	tileCell->recoTime = caloCell->time() - shift;
        tileCell->energy = caloCell->e();
	tileCell->id = caloCell->ID();
	m_pCaloCells->push_back(tileCell);
       
        double muonToF = tileCell->distance/SPEEDOFLIGHT;
        StauHit hit(TILECAL_TECH,caloCell->time()+muonToF-shift,caloCell->x(),caloCell->y(),caloCell->z(),caloCell->ID(),caloCell->e(),error, shift);
        m_hits.push_back(hit); 
    }
    if(m_pCaloCells->size()>0)
    {
        m_hasCells = true;
        averageBeta();
    }
}

//================ Destructor =================================================

MuGirlNS::StauTileCal::~StauTileCal()
{}

void MuGirlNS::StauTileCal::initStepData(TileCalStepData* tileCalStepData, double beta, double tTrack)
{  
     tileCalStepData->beta = beta;
     tileCalStepData->chi2 = 0.;
     tileCalStepData->dof = 0.;
     tileCalStepData->numOfCells = 0.;
     tileCalStepData->tTrack  = tTrack;
}

void MuGirlNS::StauTileCal::processTileCalWithBeta(double currentBeta, TileCalStepData* tileCalStepData)
{
     m_log << MSG::VERBOSE << "processTileCalWithBeta( " << currentBeta << " )" << endreq;
     double chi2 = 0.;
     for(StauTileCalCells::iterator it=m_pCaloCells->begin(); it!=m_pCaloCells->end(); it++)
     {
         StauTileCalCell* cell = *it;
	 double stauToF = cell->distance/(currentBeta*SPEEDOFLIGHT);
	 double muonToF = cell->distance/SPEEDOFLIGHT;
	 double calcTime = stauToF - muonToF;
         double error = cell->error;
	 chi2 += (cell->recoTime - calcTime)*(cell->recoTime - calcTime)/(error*error);
     }
     tileCalStepData->chi2 = chi2;
     tileCalStepData->numOfCells = (double)m_pCaloCells->size();
     tileCalStepData->dof = (double)m_pCaloCells->size();
}

void MuGirlNS::StauTileCal::clearStepData(TileCalStepData* tileCalStepData)
{
    if(NULL==tileCalStepData) return;
    delete tileCalStepData;
    tileCalStepData = NULL;
}

void MuGirlNS::StauTileCal::printStepData(TileCalStepData* tileCalStepData)
{
    if(NULL==tileCalStepData) return;
    m_log<<MSG::VERBOSE << "tileCal data: beta=" << tileCalStepData->beta
              << " chi2=" << tileCalStepData->chi2
              << " #cells=" << tileCalStepData->numOfCells << endreq;
}

void MuGirlNS::StauTileCal::clear()
{
    if(NULL!=m_pCaloCells)
    {
         for(StauTileCalCells::iterator it = m_pCaloCells->begin(); it!=m_pCaloCells->end(); it++)
         {
            delete *it;
	 }
	 m_pCaloCells->clear();
	 delete m_pCaloCells;
    }
    m_beta = StauBetaDefault;
    m_avgBeta = StauBetaDefault;
    m_rmsBeta = -1.;
    m_hits.clear(); 
    return;
}

void MuGirlNS::StauTileCal::averageBeta()
{
    //Average
    double up = 0;
    double dwn = 0;
    for(StauTileCalCells::iterator it=m_pCaloCells->begin(); it!=m_pCaloCells->end(); it++)
    {
        StauTileCalCell* cell = *it;
        double muonToF = cell->distance/SPEEDOFLIGHT;
        double measureToF = cell->recoTime + muonToF;
        double distance = cell->distance;
        double invBeta = (measureToF * SPEEDOFLIGHT) / distance;
        double tofError = cell->error;
	double invBetaError = tofError * SPEEDOFLIGHT / distance; 
        up += invBeta / ( invBetaError * invBetaError );
        dwn += ( 1 / (invBetaError * invBetaError) );
    }
    double avgInvBeta = ( 0 != dwn) ?  up/dwn : 1./StauBetaDefault;
    m_avgBeta = 1./ avgInvBeta; // 1/(up/dwn)

    //RMS
    up = 0;
    dwn = 0;
    for(StauTileCalCells::iterator it=m_pCaloCells->begin(); it!=m_pCaloCells->end(); it++)
    {
        StauTileCalCell* cell = *it;
        double muonToF = cell->distance/SPEEDOFLIGHT;
        double measureToF = cell->recoTime + muonToF;
        double distance = cell->distance;
        double invBeta =  SPEEDOFLIGHT * measureToF / distance;
        double deltaInvBeta = invBeta - avgInvBeta;
        double tofError = cell->error;
        double invBetaError = tofError * SPEEDOFLIGHT / distance;
        //double measuredBetaError = distance * m_inverseSpeedOfLight * (1. / (measureToF * measureToF)) * tofError;
        up +=   deltaInvBeta *  deltaInvBeta / (invBetaError * invBetaError);
        dwn += ( 1. / (invBetaError * invBetaError) );
    }
    double rmsInvBeta = (0 != dwn) ? sqrt(up/dwn) : -1;
    m_rmsBeta = (0 != dwn) ?  m_avgBeta * m_avgBeta * rmsInvBeta : -1;
}


void MuGirlNS::StauTileCal::processTileCalWithTTrack(double tTrack, TileCalStepData* tileCalStepData,double beta)
{
	 m_log << MSG::VERBOSE << "processTileCalWithTTrack( " << tTrack << " )" << endreq;
	 double currentBeta = beta;
	 double stauToF;
	 double muonToF;
	 double calcTime;
	 double error;
	 StauTileCalCell* cell;
	 double chi2 = 0.;
	 for(StauTileCalCells::iterator it=m_pCaloCells->begin(); it!=m_pCaloCells->end(); it++)
	 {
	         cell = *it;
	         stauToF = cell->distance/(currentBeta*SPEEDOFLIGHT) + tTrack;
	         muonToF = cell->distance/SPEEDOFLIGHT;
	         calcTime = stauToF - muonToF;
                 error = cell->error;
	         chi2 += (cell->recoTime - calcTime)*(cell->recoTime - calcTime)/(error*error);
	 }
	 tileCalStepData->chi2 = chi2;
	 tileCalStepData->numOfCells = (double)m_pCaloCells->size();
         tileCalStepData->dof = (double)m_pCaloCells->size();
}

unsigned int MuGirlNS::StauTileCal::tileMinIsAt()
{
        // maybe this can be pre-calculated while function is being evaluated
    FcnStepsData firstSteps = *(m_pStau->fcnStepsData());
    unsigned int iMinLoc = 0;
    double minChi2 = 100000.;

    for(unsigned int i=0; i<firstSteps.size(); i++)
    {
        FcnStepData* pStepData = firstSteps[i];
        TileCalStepData* pTileCalStepData = pStepData->tileCalData;
        if(NULL==pTileCalStepData) return 0;
        double chi2 = pTileCalStepData->chi2;

        if(chi2<minChi2)
        {
            minChi2 = chi2;
            iMinLoc = i;
        }
    }

    return iMinLoc;
}

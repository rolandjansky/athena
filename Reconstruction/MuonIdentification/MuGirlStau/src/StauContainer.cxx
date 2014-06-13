/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlStau/StauContainer.h"
#include "MuGirlInterfaces/RpcSegmentInfo.h"


//================ Constructor =================================================

MuGirlNS::StauContainer::StauContainer():
    m_rp(StauContainer::RP_ID),
    m_hasRpcHits(false),
    m_hasTileCells(false),
    m_beta(-100.),
    m_pIdTrack(NULL),
    m_ann(-100.),   
    m_mfP(0,0,0), 
    m_muonP(0,0,0), 
    m_stauP(0,0,0), 
    m_rpcHitsByStation(0)
{
}

MuGirlNS::StauContainer::~StauContainer()
{
    if(NULL!=m_rpcHitsByStation)
    {
        for(StauRpcHitsLists::iterator it = m_rpcHitsByStation->begin(); it != m_rpcHitsByStation->end(); it++)
        {
            StauRpcHitsList* pHitsList = *it;
            for(StauRpcHitsList::iterator it2 = pHitsList->begin(); it2!=pHitsList->end(); it2++)
            {
                StauRpcHit* pHit = *it2;
                delete pHit;
            } 
            pHitsList->clear();
            delete pHitsList;
        }
        m_rpcHitsByStation->clear();
        delete m_rpcHitsByStation;
    }
}


void MuGirlNS::StauContainer::setRpcHitsByStation( StauRpcHitsLists* rpcHitsByStation )
{
    m_rpcHitsByStation = new StauRpcHitsLists();
    for(StauRpcHitsLists::iterator it = rpcHitsByStation->begin(); it != rpcHitsByStation->end(); it++)
    {
        StauRpcHitsList* pHitsList = *it; 
        StauRpcHitsList* pCopyList = new StauRpcHitsList();
        m_rpcHitsByStation->push_back(pCopyList);
        for(StauRpcHitsList::iterator it2 = pHitsList->begin(); it2!=pHitsList->end(); it2++)
        {
            StauRpcHit* pHit = *it2;
            StauRpcHit* pCopyHit = new StauRpcHit();
            pCopyList->push_back(pCopyHit);
         
            pCopyHit->id = pHit->id;
            pCopyHit->distance = pHit->distance;
            pCopyHit->propagationTime = pHit->propagationTime;
            pCopyHit->measuredTime = pHit->measuredTime;
            pCopyHit->muonToF = pHit->muonToF;
            pCopyHit->error = pHit->error;
            pCopyHit->x = pHit->x;
            pCopyHit->y = pHit->y;
            pCopyHit->z = pHit->z;
            pCopyHit->isEta = pHit->isEta;
        }
    }
}

void MuGirlNS::StauContainer::setTileCells(StauTileCalCells* pCaloCells )
{
	if (pCaloCells == NULL) return;
	StauTileCalCells::iterator i_cell = pCaloCells->begin();
	for (; i_cell != pCaloCells->end(); ++i_cell)
	{
		m_pCaloCells.push_back(*(*i_cell));
	} 
	 
}


void MuGirlNS::StauContainer::addHits(StauHits hits)
{
    for(unsigned int i = 0; i<hits.size(); i++) m_hits.push_back(hits[i]);
}

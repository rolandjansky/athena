/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#undef private
#undef protected
#include "Identifier/Identifier.h"

#include "Identifier/IdentifierHash.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloDM_ID.h"
#include "CaloIdentifier/CaloIdManager.h"

#include "EventCommonTPCnv/Compressor.h"

// CaloCalibrationHitContainerCnv_p2, used for T/P separation
// author Ilija Vukotic

#include "CaloTPCnv/CaloCalibrationHitContainerCnv_p2.h"
#include "map"

void CaloCalibrationHitContainerCnv_p2::transToPers(const CaloCalibrationHitContainer* transCont, CaloCalibrationHitContainer_p2* persCont, MsgStream &log) 
{ 
	
//    static int ev=0;
    size_t size = transCont->size();  
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " ***  Writing CaloCalibrationHitContainer_p2 of size:"<<size<<endreq;
	
    persCont->m_channelHash.reserve(size);	
    std::vector<float> tempE;	tempE.reserve(size*4);
	
//	const CaloCell_ID* cellIdHelper = (CaloIdManager::instance())->getCaloCell_ID();
//	const CaloDM_ID* cellIdHelper = (CaloIdManager::instance())->getDM_ID();
	

    CaloCalibrationHitContainer::const_iterator it  = transCont->begin();
	
    std::multimap <unsigned int, unsigned int> map_hashPositions;// first hash ; second its position in container

    for (unsigned int w=0;w<size;++w){
        //IdentifierHash hashId = cellIdHelper->calo_cell_hash((*it)->m_ID);
		
        unsigned int id = (*it)->m_ID.get_identifier32().get_compact();
        /* when hash things are clear it could be usefull to change from storing ID to storing hash 
           IdentifierHash hashId = -1;
           if (cellIdHelper->is_lar_dm(id)) 
           hashId =cellIdHelper->lar_zone_hash(id);
           else if (cellIdHelper->is_tile_dm(id))
           hashId =cellIdHelper->tile_zone_hash(id)+cellIdHelper->lar_zone_hash_max(); 
        */
        map_hashPositions.insert(std::pair<unsigned int, int>(id, w)); 
/*		if (!ev){
                if ( cellIdHelper->is_lar_dm(id) ) std::cout<<"LAR ";
                if ( cellIdHelper->is_tile_dm(id)) std::cout<<"TILE ";
                std::cout<<hashId;
                std::cout<<"\t"<<id<<std::endl; 
                }*/
        ++it;
    }

    std::multimap<unsigned int, unsigned int>::const_iterator iter;
    unsigned int old=0;
    for (iter=map_hashPositions.begin(); iter != map_hashPositions.end(); ++iter) {
        //cout <<"container position:"<< iter->second << " \t hash:" << iter->first << endl;
        unsigned int pHash=(iter->first)-old; // to store as a difference 
        old=iter->first;
        unsigned int pos=iter->second;
        persCont->m_channelHash.push_back(pHash);
        tempE.push_back( (float) (transCont->At(pos))->m_energy0 ); 
        tempE.push_back( (float) (transCont->At(pos))->m_energy1 ); 
        tempE.push_back( (float) (transCont->At(pos))->m_energy2 ); 
        tempE.push_back( (float) (transCont->At(pos))->m_energy3 ); 
//		if (!ev) std::cout<<"Writing Hash: "<<iter->first<<"\t E: "<< (float) (transCont->At(pos))->m_energy0<<std::endl;
    }			
	
    Compressor A; A.setNrBits(18);	
    A.reduce(tempE,persCont->m_energy); // packs energy
    persCont->m_name = transCont->Name(); //stores name
	
//	ev++;
}



void CaloCalibrationHitContainerCnv_p2::persToTrans(const CaloCalibrationHitContainer_p2* persCont, CaloCalibrationHitContainer* transCont, MsgStream &log) 
{
//	static int dog=0;
    size_t cells=persCont->m_channelHash.size();
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " ***  Reading CaloCalibrationHitContainer of size: "<<cells<<endreq;
    transCont->clear();
    transCont->reserve(cells);
    transCont->setName(persCont->name() );

		
    //const CaloCell_ID* cellIdHelper = (CaloIdManager::instance())->getCaloCell_ID(); 
    //const CaloDM_ID* cellIdHelper = (CaloIdManager::instance())->getDM_ID();
	
    Compressor A;
    std::vector<float> tempE;	tempE.reserve(cells*4);
    A.expandToFloat(persCont->m_energy,tempE);
	
    unsigned int sum=0;
    for (unsigned int i=0;i<cells;++i){
        CaloCalibrationHit* trans=new CaloCalibrationHit();
        sum+= persCont->m_channelHash[i];
		
	/*	//trans->m_ID=cellIdHelper->cell_id(sum);
		//trans->m_ID=cellIdHelper->lar_zone_id(sum);  when story with hashes gets clarified 
                */
        trans->m_ID=sum;
		
        trans->m_energy0 =(double)tempE[i*4];
        trans->m_energy1 =(double)tempE[i*4+1];
        trans->m_energy2 =(double)tempE[i*4+2];
        trans->m_energy3 =(double)tempE[i*4+3];
//		if(!dog) std::cout<<"Reading hash: "<<sum <<"\t E: "<< (double)tempE[i] <<std::endl;
        transCont->push_back(trans);
    }
//	dog++;
		
}
 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "Identifier/Identifier.h"

#include "Identifier/IdentifierHash.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloDM_ID.h"
#include "CaloIdentifier/CaloIdManager.h"

#include "AthenaPoolCnvSvc/Compressor.h"

// CaloCalibrationHitContainerCnv_p3, used for T/P separation
// author Ilija Vukotic

#include "CaloSimEventTPCnv/CaloCalibrationHitContainerCnv_p3.h"
#include "map"

void CaloCalibrationHitContainerCnv_p3::transToPers(const CaloCalibrationHitContainer* transCont, CaloCalibrationHitContainer_p3* persCont, MsgStream &log) 
{ 
	
//    static int ev=0;
    size_t size = transCont->size();  
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " ***  Writing CaloCalibrationHitContainer_p3 of size:"<<size<<endmsg;
	
    persCont->m_channelHash.reserve(size);	
    std::vector<float> tempE;	tempE.reserve(size*4);
    std::vector<unsigned int> tempPID; tempPID.reserve(size);

//	const CaloCell_ID* cellIdHelper = (CaloIdManager::instance())->getCaloCell_ID();
//	const CaloDM_ID* cellIdHelper = (CaloIdManager::instance())->getDM_ID();
	

    CaloCalibrationHitContainer::const_iterator it  = transCont->begin();
	
    std::multimap <unsigned int, unsigned int> map_hashPositions;// first hash ; second its position in container

    for (unsigned int w=0;w<size;++w){
        //IdentifierHash hashId = cellIdHelper->calo_cell_hash((*it)->m_ID);
		
        unsigned int id = (*it)->cellID().get_identifier32().get_compact();
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
        const CaloCalibrationHit& hit = *transCont->At(pos);
        tempE.push_back( (float) hit.energyEM() ); 
        tempE.push_back( (float) hit.energyNonEM() ); 
        tempE.push_back( (float) hit.energyInvisible() ); 
        tempE.push_back( (float) hit.energyEscaped() ); 
        tempPID.push_back( (unsigned int) hit.particleID() );

//		if (!ev) std::cout<<"Writing Hash: "<<iter->first<<"\t E: "<< (float) (transCont->At(pos))->m_energy0<<std::endl;
    }			
	
    Compressor A; A.setNrBits(18);	
    A.reduce(tempE,persCont->m_energy); // packs energy
    persCont->m_name = transCont->Name(); //stores name
    persCont->m_particleID = tempPID;

//	ev++;
}



void CaloCalibrationHitContainerCnv_p3::persToTrans(const CaloCalibrationHitContainer_p3* persCont, CaloCalibrationHitContainer* transCont, MsgStream &log) 
{
//	static int dog=0;
    size_t cells=persCont->m_channelHash.size();
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " ***  Reading CaloCalibrationHitContainer of size: "<<cells<<endmsg;
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
        sum+= persCont->m_channelHash[i];
		
	/*	//trans->m_ID=cellIdHelper->cell_id(sum);
		//trans->m_ID=cellIdHelper->lar_zone_id(sum);  when story with hashes gets clarified 
                */

        transCont->push_back
          (new CaloCalibrationHit(static_cast<Identifier>(sum),
                                  tempE[i*4],
                                  tempE[i*4+1],
                                  tempE[i*4+2],
                                  tempE[i*4+3],
                                  persCont->m_particleID[i]));

//		if(!dog) std::cout<<"Reading hash: "<<sum <<"\t E: "<< (double)tempE[i] <<std::endl;
    }
//	dog++;
		
}
 

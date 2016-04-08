/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetTruthEvent/TrigInDetTrackTruth.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthCnv_p1.h"



void TrigInDetTrackTruthCnv_p1::persToTrans(const TrigInDetTrackTruth_p1* persObj, 
				          TrigInDetTrackTruth* transObj, 
				          MsgStream &log)
{
   log << MSG::DEBUG << "TrigInDetTrackTruthCnv_p1::persToTrans called " << endreq;

   transObj->m_best_match_hits     = persObj->best_match_hits    ;
   transObj->m_best_Si_match_hits  = persObj->best_Si_match_hits ;
   transObj->m_best_TRT_match_hits = persObj->best_TRT_match_hits;

   //convert m_true_part_vec

   int n_true_part = persObj->m_true_part_vec.size();
   transObj->m_true_part_vec.clear();
   transObj->m_true_part_vec.resize( n_true_part );
   int index=0;
   for(std::vector<TPObjRef>::const_iterator it = persObj->m_true_part_vec.begin();
       it!=persObj->m_true_part_vec.end();++it)
     {
       fillTransFromPStore( &m_hepMcParticleLinkCnv_p1, (*it), &transObj->m_true_part_vec[index], log  );
       index++;
     }
   //convert m_nr_common_hits 
   int n_hits = (persObj->m_nr_common_hits).size();
   transObj->m_nr_common_hits.clear();
   transObj->m_nr_common_hits.resize( n_hits );
   index=0;
   for(std::vector<TPObjRef>::const_iterator it = persObj->m_nr_common_hits.begin();
       it!=persObj->m_nr_common_hits.end();++it)
     {
       fillTransFromPStore( &m_trigIDHitStatsCnv_p1, (*it), &(transObj->m_nr_common_hits[index]), log  );    
       index++;
     }

   //convert m_family_tree

   transObj->m_family_tree = persObj->m_family_tree;
   
}


void TrigInDetTrackTruthCnv_p1::transToPers(const TrigInDetTrackTruth* transObj, 
				          TrigInDetTrackTruth_p1* persObj, 
				          MsgStream &log)
{
   log << MSG::DEBUG << "TrigInDetTrackTruthCnv_p1::transToPers called " << endreq;


   persObj->best_match_hits     = transObj->m_best_match_hits    ;
   persObj->best_Si_match_hits  = transObj->m_best_Si_match_hits ;
   persObj->best_TRT_match_hits = transObj->m_best_TRT_match_hits;

   //convert m_true_part_vec

   int n_true_part = transObj->m_true_part_vec.size();
   persObj->m_true_part_vec.clear();
   persObj->m_true_part_vec.reserve( n_true_part );

   for(std::vector<HepMcParticleLink>::const_iterator it = transObj->m_true_part_vec.begin();
       it!=transObj->m_true_part_vec.end();++it)
     {
       persObj->m_true_part_vec.push_back(toPersistent( &m_hepMcParticleLinkCnv_p1, &(*it), log  ));  
     }

   //convert m_nr_common_hits 
   int n_hits = (transObj->m_nr_common_hits).size();
   persObj->m_nr_common_hits.clear();
   persObj->m_nr_common_hits.reserve( n_hits );
 
   for(std::vector<TrigIDHitStats>::const_iterator it = transObj->m_nr_common_hits.begin();
       it!=transObj->m_nr_common_hits.end();++it)
     {
       persObj->m_nr_common_hits.push_back(toPersistent( &m_trigIDHitStatsCnv_p1, &(*it), log  )); 
     }

   //convert m_family_tree
   persObj->m_family_tree = transObj->m_family_tree;


}

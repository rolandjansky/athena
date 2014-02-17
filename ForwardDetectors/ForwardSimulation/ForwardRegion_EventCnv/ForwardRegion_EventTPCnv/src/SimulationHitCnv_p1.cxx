/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#define private public
//#define protected public
#include "ForwardRegion_SimEv/SimulationHit.h"
//#undef private
//#undef protected

#include "Identifier/Identifier.h"
#include "ForwardRegion_EventTPCnv/SimulationHitCnv_p1.h"

void SimulationHitCnv_p1::persToTrans(const SimulationHit_p1* persObj, SimulationHit* transObj, MsgStream &log)
{
	log << MSG::DEBUG << "SimulationHitCnv_p1::persToTrans called " << endreq;

    SimulationHit::StepPoint pre( persObj->m_pre_time,CLHEP::Hep3Vector( persObj->m_pre_position_x, persObj->m_pre_position_y, persObj->m_pre_position_z) ,
                              persObj->m_pre_kinetic_energy, CLHEP::Hep3Vector(persObj->m_pre_direction_x, persObj->m_pre_direction_y, persObj->m_pre_direction_z),
                              CLHEP::Hep3Vector(persObj->m_pre_momentum_x, persObj->m_pre_momentum_y, persObj->m_pre_momentum_z));
    SimulationHit::StepPoint post( persObj->m_post_time,CLHEP::Hep3Vector( persObj->m_post_position_x, persObj->m_post_position_y, persObj->m_post_position_z) ,
                              persObj->m_post_kinetic_energy, CLHEP::Hep3Vector(persObj->m_post_direction_x, persObj->m_post_direction_y, persObj->m_post_direction_z),
                              CLHEP::Hep3Vector(persObj->m_post_momentum_x, persObj->m_post_momentum_y, persObj->m_post_momentum_z));
    transObj->m_pre = pre;
    transObj->m_post = post;
	transObj->m_deposit_energy=persObj->m_deposit_energy;
	transObj->m_pdgcode=persObj->m_pdgcode;
	transObj->m_track_id=persObj->m_track_id;
	transObj->m_parent_id=persObj->m_parent_id;
}

void SimulationHitCnv_p1::transToPers(const SimulationHit* transObj, SimulationHit_p1* persObj, MsgStream &log)
{
	log << MSG::DEBUG << "SimulationHitCnv_p1::transToPers called " << endreq;
    
    persObj->m_pre_time = transObj->m_pre.time;
    persObj->m_pre_kinetic_energy = transObj->m_pre.kinetic_energy;
    persObj->m_pre_position_x = transObj->m_pre.position.x();
    persObj->m_pre_position_y = transObj->m_pre.position.y();
    persObj->m_pre_position_z = transObj->m_pre.position.z();
    persObj->m_pre_direction_x = transObj->m_pre.direction.x();
    persObj->m_pre_direction_y = transObj->m_pre.direction.y();
    persObj->m_pre_direction_z = transObj->m_pre.direction.z();
    persObj->m_pre_momentum_x = transObj->m_pre.momentum.x();
    persObj->m_pre_momentum_y = transObj->m_pre.momentum.y();
    persObj->m_pre_momentum_z = transObj->m_pre.momentum.z();
    persObj->m_post_time = transObj->m_post.time;
    persObj->m_post_kinetic_energy = transObj->m_post.kinetic_energy;
    persObj->m_post_position_x = transObj->m_post.position.x();
    persObj->m_post_position_y = transObj->m_post.position.y();
    persObj->m_post_position_z = transObj->m_post.position.z();
    persObj->m_post_direction_x = transObj->m_post.direction.x();
    persObj->m_post_direction_y = transObj->m_post.direction.y();
    persObj->m_post_direction_z = transObj->m_post.direction.z();
    persObj->m_post_momentum_x = transObj->m_post.momentum.x();
    persObj->m_post_momentum_y = transObj->m_post.momentum.y();
    persObj->m_post_momentum_z = transObj->m_post.momentum.z();
	persObj->m_deposit_energy=transObj->m_deposit_energy;
	persObj->m_pdgcode=transObj->m_pdgcode;
	persObj->m_track_id=transObj->m_track_id;
	persObj->m_parent_id=transObj->m_parent_id;

 }

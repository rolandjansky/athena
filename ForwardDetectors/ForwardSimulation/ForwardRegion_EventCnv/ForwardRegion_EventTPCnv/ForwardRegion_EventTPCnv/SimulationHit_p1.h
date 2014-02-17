/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SimulationHit_p1_h
#define SimulationHit_p1_h

#include "CLHEP/Vector/ThreeVector.h"
#include "ForwardRegion_SimEv/SimulationHit.h"
#include "AthenaPoolUtilities/TPObjRef.h"
//#include "GeneratorObjectsTPCnv/HepMcParticleLink_p1.h"

class SimulationHit_p1
{
 public:
	// needed by athenaRoot 
	SimulationHit_p1(){};
	friend class SimulationHitCnv_p1;

private:
    double m_pre_time;
    double m_pre_position_x;
    double m_pre_position_y;
    double m_pre_position_z;
    double m_pre_kinetic_energy;
    double m_pre_direction_x;        
    double m_pre_direction_y;        
    double m_pre_direction_z;        
    double m_pre_momentum_x;
    double m_pre_momentum_y;
    double m_pre_momentum_z;
    double m_post_time;
    double m_post_position_x;
    double m_post_position_y;
    double m_post_position_z;
    double m_post_kinetic_energy;
    double m_post_direction_x;        
    double m_post_direction_y;        
    double m_post_direction_z;        
    double m_post_momentum_x;
    double m_post_momentum_y;
    double m_post_momentum_z;
	double m_deposit_energy;
    int m_pdgcode;
    int m_track_id;
    int m_parent_id;
};


#endif 


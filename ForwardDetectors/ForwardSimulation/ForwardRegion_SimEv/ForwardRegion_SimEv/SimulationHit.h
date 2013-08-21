/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SimulationHit_h
#define SimulationHit_h

#include "CLHEP/Vector/ThreeVector.h"

class SimulationHit
{
public:
	SimulationHit();
    SimulationHit(double pre_time, const CLHEP::Hep3Vector& pre_position, const double pre_kinetic_energy, const CLHEP::Hep3Vector& pre_direction, const CLHEP::Hep3Vector& pre_momentum,
                  double post_time, const CLHEP::Hep3Vector& post_position, const double post_kinetic_energy, const CLHEP::Hep3Vector& post_direction, const CLHEP::Hep3Vector& post_momentum,
                   const double deposit_energy, const int pdgcode, const int track_id, const int parent_id);
    virtual ~SimulationHit();
    
    class StepPoint {
        public:
        StepPoint();
        StepPoint(double time,  const CLHEP::Hep3Vector& position, const double kinetic_energy,  const CLHEP::Hep3Vector& direction,  const CLHEP::Hep3Vector& momentum);
        double time;
        CLHEP::Hep3Vector position;
        double kinetic_energy;
        CLHEP::Hep3Vector direction;        
        CLHEP::Hep3Vector momentum;
    };        

    const StepPoint& pre() const { return m_pre; }
    const StepPoint& post() const { return m_post; }
    double depositEnergy() const { return m_deposit_energy; }
	int particleEncoding() const { return m_pdgcode; }
	int trackID() const { return m_track_id; }
	int parentID() const { return m_parent_id; }
    

public:
    StepPoint m_pre;
    StepPoint m_post;
	double m_deposit_energy;
    int m_pdgcode;
    int m_track_id;
    int m_parent_id;

};


#endif 


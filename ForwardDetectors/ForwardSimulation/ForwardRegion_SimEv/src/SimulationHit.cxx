/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardRegion_SimEv/SimulationHit.h"

SimulationHit::StepPoint::StepPoint() : time(0), kinetic_energy(0) { }
SimulationHit::StepPoint::StepPoint(double time,  const CLHEP::Hep3Vector& position, const double kinetic_energy,  const CLHEP::Hep3Vector& direction,  const CLHEP::Hep3Vector& momentum) : time(time), position(position), kinetic_energy(kinetic_energy), direction(direction), momentum(momentum) { }

SimulationHit::SimulationHit( ) :  m_deposit_energy(0), m_pdgcode(0), m_track_id(0), m_parent_id(0) { }
SimulationHit::SimulationHit(double pre_time, const CLHEP::Hep3Vector& pre_position, const double pre_kinetic_energy, const CLHEP::Hep3Vector& pre_direction, const CLHEP::Hep3Vector& pre_momentum,
                  double post_time, const CLHEP::Hep3Vector& post_position, const double post_kinetic_energy, const CLHEP::Hep3Vector& post_direction, const CLHEP::Hep3Vector& post_momentum,
                   const double deposit_energy, const int pdgcode, const int track_id, const int parent_id) 
    : m_pre(pre_time,pre_position,pre_kinetic_energy,pre_direction, pre_momentum),  m_post(post_time,post_position,post_kinetic_energy,post_direction, post_momentum),
      m_deposit_energy(deposit_energy), m_pdgcode(pdgcode), m_track_id(track_id), m_parent_id(parent_id) { }

SimulationHit::~SimulationHit() { }


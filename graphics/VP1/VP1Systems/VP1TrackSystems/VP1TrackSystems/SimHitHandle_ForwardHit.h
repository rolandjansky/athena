/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class SimHitHandle_ForwardHit             //
//                                                            //
//  Description: Handle for SiHit's                           //
//                                                            //
//  Author: Thomas Keck (thomas.keck@cern.ch)  //
//  Initial version: July 2013                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef SIMHITHANDLE_FORWARDHIT_H
#define SIMHITHANDLE_FORWARDHIT_H

#include "VP1TrackSystems/SimHitHandleBase.h"

class SimulationHit;
class SimHitHandle_ForwardHit : public SimHitHandleBase {
public:

  SimHitHandle_ForwardHit( const SimulationHit * );
  virtual ~SimHitHandle_ForwardHit();

  QString type() const { return "SimulationHit"; };

  Amg::Vector3D momentumDirection() const;
  double actualMomentum() const;
  Amg::Vector3D posStart() const;
  Amg::Vector3D posEnd() const;
  double hitTime() const;
  int actualPDGCodeFromSimHit() const;
  const HepMcParticleLink& particleLink() const;

  Trk::TrackParameters * createTrackParameters() const;

private:

  class Imp;
  Imp * m_d;
  HepMcParticleLink m_link;
};

#endif

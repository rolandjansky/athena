/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class SimHitHandle_SiHit                  //
//                                                            //
//  Description: Handle for SiHit's                           //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef SIMHITHANDLE_SIHIT_H
#define SIMHITHANDLE_SIHIT_H

#include "VP1TrackSystems/SimHitHandleBase.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkParameters/TrackParameters.h"

class SiHit;
class SimHitHandle_SiHit : public SimHitHandleBase {
public:

  SimHitHandle_SiHit( const SiHit * );
  virtual ~SimHitHandle_SiHit();

  QString type() const { return "SiHit"; };

  Amg::Vector3D momentumDirection() const;
  Amg::Vector3D posStart() const;
  Amg::Vector3D posEnd() const;
  double hitTime() const;

  const HepMcParticleLink& particleLink() const;

  Trk::TrackParameters * createTrackParameters() const;

private:

  class Imp;
  Imp * m_d;

};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class SimHitHandle_TRTHit                 //
//                                                            //
//  Description: Handle for TRTUncompressedHit's              //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef SIMHITHANDLE_TRTHIT_H
#define SIMHITHANDLE_TRTHIT_H

#include "VP1TrackSystems/SimHitHandleBase.h"

class TRTUncompressedHit;
class SimHitHandle_TRTHit : public SimHitHandleBase {
public:

  SimHitHandle_TRTHit( const TRTUncompressedHit* );
  virtual ~SimHitHandle_TRTHit();

  QString type() const { return "TRTUncompressedHit"; };

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

};

#endif

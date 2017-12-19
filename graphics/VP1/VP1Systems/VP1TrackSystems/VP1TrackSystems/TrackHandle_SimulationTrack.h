/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class TrackHandle_SimulationTrack              //
//                                                            //
//  Description: Handle for truth tracks based on HepMC       //
//               records and sim. hits.                       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef TRACKHANDLE_SIMULATIONTRACK_H
#define TRACKHANDLE_SIMULATIONTRACK_H

#include "VP1TrackSystems/TrackHandleBase.h"
#include "VP1TrackSystems/SimHitHandleBase.h"

#include "GeoPrimitives/GeoPrimitives.h"

class TrackHandle_SimulationTrack : public TrackHandleBase {
public:

  TrackHandle_SimulationTrack( TrackCollHandleBase*, const SimBarCode& simBarCode, const SimHitList& );

  virtual ~TrackHandle_SimulationTrack();

  virtual QStringList clicked() const;

  virtual Amg::Vector3D momentum() const;
  virtual int pdgCode() const;
  bool hasBarCodeZero() const;

  void setAscObjsVisible(bool);
  
  virtual QString type() const { return QString("Simulation"); } //!< return very short word with type (maybe link with collection type?)
  

protected:
  virtual double calculateCharge() const;
  const Trk::Track * provide_pathInfoTrkTrack() const;
  void visibleStateChanged();

private:

  class Imp;
  Imp * m_d;

};

#endif

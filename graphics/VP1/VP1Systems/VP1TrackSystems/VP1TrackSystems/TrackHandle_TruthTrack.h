/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class TrackHandle_TruthTrack              //
//                                                            //
//  Description: Handle for truth tracks based on HepMC       //
//               records and sim. hits.                       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef TRACKHANDLE_TRUTHTRACK_H
#define TRACKHANDLE_TRUTHTRACK_H

#include "VP1TrackSystems/TrackHandleBase.h"
#include "VP1TrackSystems/SimHitHandleBase.h"

#include "GeoPrimitives/GeoPrimitives.h"

namespace HepMC { class GenParticle; }

class TrackHandle_TruthTrack : public TrackHandleBase {
public:

  TrackHandle_TruthTrack( TrackCollHandleBase*,
			  const SimBarCode&,
			  const SimHitList&,
			  const HepMC::GenParticle* genPart = 0 );

  virtual ~TrackHandle_TruthTrack();

  virtual QStringList clicked() const;

  virtual Amg::Vector3D momentum() const;
  virtual int pdgCode() const;
  bool hasBarCodeZero() const;


  bool hasVertexAtIR(const double& rmaxsq, const double& zmax) const;
  //Fixme: Something like this?:  bool isPrimaryTrack() const;
  void setAscObjsVisible(bool);
  
  virtual QString type() const { return QString("Truth"); } //!< return very short word with type (maybe link with collection type?)
  

protected:
  virtual double calculateCharge() const;
  const Trk::Track * provide_pathInfoTrkTrack() const;
  void ensureTouchedMuonChambersInitialised() const;
  void visibleStateChanged();

private:

  class Imp;
  Imp * m_d;

};

#endif

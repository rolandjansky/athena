/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetBeamSpotService_IBEAMCOND_H
#define InDetBeamSpotService_IBEAMCOND_H
// IBeamCondSvc.h - abstract interface to service giving beamspot data
// Richard Hawkings, started 15/6/05

#include "GaudiKernel/IInterface.h"
#include "GeoPrimitives/GeoPrimitives.h"
#ifndef SIMULATIONBASE
#include "VxVertex/RecVertex.h"
#endif

class IBeamCondSvc : virtual public IInterface {

 public:
  static const InterfaceID& interfaceID();

  // return the position of the beamspot centroid
  virtual const Amg::Vector3D& beamPos() const =0;

  // return the beamspot RMS widths (sigmas) in x (i=0), y (i=1) and z (i=2)
  virtual float beamSigma(int i) const =0;

  // return the beamspot XY correlation sigmaXY
  virtual float beamSigmaXY() const =0;

  // return the beam tilts (rad) wrt the xz (i=0) and yz (i=0) planes
  // the tilts give the angle the beam deviates from the z axis in the 2 planes
  virtual float beamTilt(int i) const =0;

  // return the beamspot status word
  virtual int beamStatus() const =0;

  // return the beamspot centroid as a RecVertex, ready for use in fitting
  // this takes into account the position, RMS width and tilts

#ifndef SIMULATIONBASE
  virtual const Trk::RecVertex& beamVtx() const =0;
#endif

  // fill the beamspot record in the TDS for (not to be used)
  virtual bool fillRec() const =0;
};

inline const InterfaceID& IBeamCondSvc::interfaceID() {
  static const InterfaceID IID_IBeamCondSvc("IBeamCondSvc",1,0);
  return IID_IBeamCondSvc;
}

#endif // InDetBeamSpotService_IBEAMCOND_H

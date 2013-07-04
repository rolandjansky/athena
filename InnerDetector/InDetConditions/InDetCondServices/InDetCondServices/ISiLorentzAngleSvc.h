/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISiLorentzAngleSvc.h
 * @author Grant.Gorfine@cern.ch
**/
#ifndef ISiLorentzAngleSvc_h
#define ISiLorentzAngleSvc_h

//Gaudi Includes
#include "GaudiKernel/IInterface.h"
#include "AthenaKernel/IOVSvcDefs.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"

//forward declarations
class Identifier;
class IdentifierHash;

/**
 * @class ISiLorentzAngleSvc
 * Interface class for service providing Lorentz angle (and the corresponding shift of the measurement) 
 * for each detector element.
**/
class ISiLorentzAngleSvc: virtual public IInterface
{
public:
  virtual ~ISiLorentzAngleSvc(){}

  static const InterfaceID & interfaceID(); //!< reimplemented from IInterface

  /** Get the Lorentz angle shift in the local x (phiDist) direction
      Assumes the center of the detector and is generally cached. */
  virtual double getLorentzShift(const IdentifierHash & elementHash) = 0;

  /** As above, but provide the local position. 
      More accurate but slower */
  virtual double getLorentzShift(const IdentifierHash & elementHash, const Amg::Vector2D & locPos) = 0;

  /** Get the Lorentz angle shift in the local y (etaDist) direction
      Assumes the center of the detector and is generally cached.*/
  virtual double getLorentzShiftEta(const IdentifierHash & elementHash) = 0;

  /** As above, but provide the local position. 
      More accurate but slower. */
  virtual double getLorentzShiftEta(const IdentifierHash & elementHash, const Amg::Vector2D & locPos) = 0;

  /** Get tan af the Lorentz angle in the local x (phiDist) direction
      Assumes the center of the detector and is generally cached. */
  virtual double getTanLorentzAngle(const IdentifierHash & elementHash) = 0;

  /** As above, but provide the local position. 
      More accurate but slower. */
  virtual double getTanLorentzAngle(const IdentifierHash & elementHash, const Amg::Vector2D & locPos) = 0;

  /** Get tan af the Lorentz angle in the local y (etaDist) direction
      Assumes the center of the detector and is generally cached */
  virtual double getTanLorentzAngleEta(const IdentifierHash & elementHash) = 0;

  /** As above, but provide the local position. 
      More accurate but slower. */
  virtual double getTanLorentzAngleEta(const IdentifierHash & elementHash, const Amg::Vector2D & locPos) = 0;

  /// IOV CallBack
  virtual StatusCode callBack(IOVSVC_CALLBACK_ARGS) = 0;

};

inline const InterfaceID & ISiLorentzAngleSvc::interfaceID(){
  static const InterfaceID IID_ISiLorentzAngleSvc("ISiLorentzAngleSvc",1,0);
  return IID_ISiLorentzAngleSvc;
}
#endif

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISiLorentzAngleTool.h
 * @author Susumu.Oda@cern.ch
**/
#ifndef ISiLorentzAngleTool_h
#define ISiLorentzAngleTool_h

//Gaudi Includes
#include "GaudiKernel/IAlgTool.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"

//forward declarations
class IdentifierHash;

/**
 * @class ISiLorentzAngleTool
 * Interface class for tool providing Lorentz angle (and the corresponding shift of the measurement) 
 * for each detector element.
**/
class ISiLorentzAngleTool: virtual public IAlgTool
{
public:
  virtual ~ISiLorentzAngleTool() = default;

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ISiLorentzAngleTool, 1, 0);

  /** Get the Lorentz angle shift in the local x (phiDist) direction
      Assumes the center of the detector and is generally cached. */
  virtual double getLorentzShift(const IdentifierHash& elementHash) const = 0;

  /** As above, but provide the local position. 
      More accurate but slower */
  virtual double getLorentzShift(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) const = 0;

  /** Get the Lorentz angle shift in the local y (etaDist) direction
      Assumes the center of the detector and is generally cached.*/
  virtual double getLorentzShiftEta(const IdentifierHash& elementHash) const = 0;

  /** As above, but provide the local position. 
      More accurate but slower. */
  virtual double getLorentzShiftEta(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) const = 0;

  /** Get tan af the Lorentz angle in the local x (phiDist) direction
      Assumes the center of the detector and is generally cached. */
  virtual double getTanLorentzAngle(const IdentifierHash& elementHash) const = 0;

  /** As above, but provide the local position. 
      More accurate but slower. */
  virtual double getTanLorentzAngle(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) const = 0;

  /** Get tan af the Lorentz angle in the local y (etaDist) direction
      Assumes the center of the detector and is generally cached */
  virtual double getTanLorentzAngleEta(const IdentifierHash& elementHash) const = 0;

  /** As above, but provide the local position. 
      More accurate but slower. */
  virtual double getTanLorentzAngleEta(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) const = 0;

  /** Get bias voltage */
  virtual double getBiasVoltage(const IdentifierHash& elementHash) const = 0;

  /** Get temperature */
  virtual double getTemperature(const IdentifierHash& elementHash) const = 0;

  /** Get depletion voltage */
  virtual double getDepletionVoltage(const IdentifierHash& elementHash) const = 0;
};

#endif // ISiLorentzAngleTool_h

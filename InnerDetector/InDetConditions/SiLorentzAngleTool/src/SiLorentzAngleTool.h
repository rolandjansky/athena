/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SiLorentzAngleTool.h
 * @author Susumu.Oda@cern.ch
**/
#ifndef SiLorentzAngleTool_h
#define SiLorentzAngleTool_h

#include "InDetCondTools/ISiLorentzAngleTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "SiLorentzAngleTool/SiLorentzAngleCondData.h"
#include "StoreGate/ReadCondHandleKey.h"

//Gaudi Includes
#include "GaudiKernel/ServiceHandle.h"

class IdentifierHash;
namespace InDetDD {
  class SiDetectorElement;
}

/**
 * @class SiLorentzAngleTool
 * Concrete class for tool providing Lorentz angle (and the corresponding correction for the shift of the measurement) 
 * for each detector element.
**/

class SiLorentzAngleTool: public extends<AthAlgTool, ISiLorentzAngleTool>
{
public:

  SiLorentzAngleTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SiLorentzAngleTool() = default;

  virtual StatusCode initialize() override;          //!< Service init
  virtual StatusCode finalize() override;            //!< Service finalize

  /** Get the Lorentz shift correction in the local x (phiDist) direction
      Assumes the center of the detector and is generally cached. */
  virtual double getLorentzShift(const IdentifierHash& elementHash) const override;

  /** As above, but provide the local position. 
      More accurate but slower. */
  virtual double getLorentzShift(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) const override;

  /**Get the Lorentz shift correction in the local y (etaDist) direction
     Assumes the center of the detector and is generally cached. */
  virtual double getLorentzShiftEta(const IdentifierHash& elementHash) const override;

  /** As above, but provide the local position. 
      More accurate but slower. */
  virtual double getLorentzShiftEta(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) const override;

  /** Get tan af the Lorentz angle in the local x (phiDist) direction
      Assumes the center of the detector and is generally cached. */
  virtual double getTanLorentzAngle(const IdentifierHash& elementHash) const override;

  /** As above, but provide the local position. 
      More accurate but slower. */
  virtual double getTanLorentzAngle(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) const override;

  /** Get tan af the Lorentz angle in the local y (etaDist) direction
      Assumes the center of the detector and is generally cached. */
  virtual double getTanLorentzAngleEta(const IdentifierHash& elementHash) const override;

  /** As above, but provide the local position. 
      More accurate but slower. */
  virtual double getTanLorentzAngleEta(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) const override;

  /** Get bias voltage */
  virtual double getBiasVoltage(const IdentifierHash& elementHash) const override;

  /** Get temperature */
  virtual double getTemperature(const IdentifierHash& elementHash) const override;

  /** Get depletion voltage */
  virtual double getDepletionVoltage(const IdentifierHash& elementHash) const override;

private:
  enum Variable {LorentzShift, LorentzShiftEta, TanLorentzAngle, TanLorentzAngleEta};

  double getValue(const IdentifierHash& elementHash, const Amg::Vector2D& locPos, Variable variable) const;
  double getCorrectionFactor() const;
  Amg::Vector3D getMagneticField(const Amg::Vector3D& pointvec) const;
  const SiLorentzAngleCondData* getCondData() const;
  const InDetDD::SiDetectorElement* getDetectorElement(const IdentifierHash& waferHash) const;

  // Properties
  std::string              m_detectorName;
  bool                     m_isPixel;  
  double                   m_nominalField;
  bool                     m_useMagFieldSvc;
  bool                     m_ignoreLocalPos;   // Makes methods using localPosition behave as method without passing localPosition.
  SG::ReadCondHandleKey<SiLorentzAngleCondData> m_condData;
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_detEleCollKey{this, "DetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT or Pixel"};

  // needed services
  ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc;

  static const double s_invalidValue;
};

#endif // SiLorentzAngleTool_h

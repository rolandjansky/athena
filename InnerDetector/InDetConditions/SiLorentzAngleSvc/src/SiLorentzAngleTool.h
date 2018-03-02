/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SiLorentzAngleTool.h
 * @author Susumu.Oda@cern.ch
**/
#ifndef SiLorentzAngleTool_h
#define SiLorentzAngleTool_h

#include "InDetCondServices/ISiLorentzAngleTool.h"

//Gaudi Includes
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "Identifier/IdentifierHash.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "SiPropertiesSvc/SiliconProperties.h"
#include "SiLorentzAngleSvc/SiLorentzAngleCondData.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"

namespace InDetDD {
  class SiDetectorManager;
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
  virtual double getLorentzShift(const IdentifierHash& elementHash) override;

  /** As above, but provide the local position. 
      More accurate but slower. */
  virtual double getLorentzShift(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) override;

  /**Get the Lorentz shift correction in the local y (etaDist) direction
     Assumes the center of the detector and is generally cached. */
  virtual double getLorentzShiftEta(const IdentifierHash& elementHash) override;

  /** As above, but provide the local position. 
      More accurate but slower. */
  virtual double getLorentzShiftEta(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) override;

  /** Get tan af the Lorentz angle in the local x (phiDist) direction
      Assumes the center of the detector and is generally cached. */
  virtual double getTanLorentzAngle(const IdentifierHash& elementHash) override;

  /** As above, but provide the local position. 
      More accurate but slower. */
  virtual double getTanLorentzAngle(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) override;

  /** Get tan af the Lorentz angle in the local y (etaDist) direction
      Assumes the center of the detector and is generally cached. */
  virtual double getTanLorentzAngleEta(const IdentifierHash& elementHash) override;

  /** As above, but provide the local position. 
      More accurate but slower. */
  virtual double getTanLorentzAngleEta(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) override;

  /** Get bias voltage */
  virtual double getBiasVoltage(const IdentifierHash& elementHash) override;

  /** Get temperature */
  virtual double getTemperature(const IdentifierHash& elementHash) override;

  /** Get depletion voltage */
  virtual double getDepletionVoltage(const IdentifierHash& elementHash) override;

private:
  enum Variable {LorentzShift, LorentzShiftEta, TanLorentzAngle, TanLorentzAngleEta};

  double getValue(const IdentifierHash& elementHash, const Amg::Vector2D& locPos, Variable variable);
  double getCorrectionFactor() const;
  Amg::Vector3D getMagneticField(const IdentifierHash& elementHash, const Amg::Vector2D& locPos);
  const SiLorentzAngleCondData* getCondData() const;
 
  // Properties
  std::string              m_detectorName;
  bool                     m_isPixel;  
  double                   m_nominalField;
  bool                     m_useMagFieldSvc;
  bool                     m_ignoreLocalPos;   // Makes methods using localPosition behave as method without passing localPosition.
  SG::ReadCondHandleKey<SiLorentzAngleCondData> m_condData;

  // needed services
  ServiceHandle<MagField::IMagFieldSvc>  m_magFieldSvc;

  InDet::SiliconProperties m_siProperties;
  const InDetDD::SiDetectorManager* m_detManager;

  static const double s_invalidValue;
};

#endif // SiLorentzAngleTool_h

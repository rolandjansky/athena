/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SiLorentzAngleCHSvc.h
 * @author Susumu.Oda@cern.ch
**/
#ifndef SiLorentzAngleCHSvc_h
#define SiLorentzAngleCHSvc_h

#include "InDetCondServices/ISiLorentzAngleSvc.h"

//Gaudi Includes
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "AthenaBaseComps/AthService.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "SiPropertiesSvc/SiliconProperties.h"
#include "SiLorentzAngleSvc/SiLorentzAngleCondData.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"

#include <string>

//forward declarations
class IdentifierHash;
class StoreGateSvc;

namespace InDetDD {
  class SiDetectorManager;
}  

/**
 * @class SiLorentzAngleCHSvc
 * Concrete class for service providing Lorentz angle (and the corresponding correction for the shift of the measurement) 
 * for each detector element.
**/

class SiLorentzAngleCHSvc:  public AthService, virtual public ISiLorentzAngleSvc 
{
public:

  SiLorentzAngleCHSvc(const std::string& name, ISvcLocator* sl);
  virtual ~SiLorentzAngleCHSvc() = default;

  virtual StatusCode initialize() override;          //!< Service init
  virtual StatusCode finalize() override;            //!< Service finalize
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;

  static const InterfaceID& interfaceID();

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

  /** IOV CallBack */
  virtual StatusCode callBack(IOVSVC_CALLBACK_ARGS) override;

  /** IOV Callback for correction folder */
  virtual StatusCode corrFolderCallBack(IOVSVC_CALLBACK_ARGS);

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
  ServiceHandle<StoreGateSvc>            m_detStore;

  InDet::SiliconProperties m_siProperties;
  const InDetDD::SiDetectorManager* m_detManager;

  static const double s_invalidValue;
};

inline const InterfaceID& SiLorentzAngleCHSvc::interfaceID(){
  return ISiLorentzAngleSvc::interfaceID();
}

#endif // SiLorentzAngleCHSvc_h

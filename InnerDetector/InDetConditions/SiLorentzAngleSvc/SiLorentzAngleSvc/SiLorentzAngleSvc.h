/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SiLorentzAngleSvc.h
 * @author Grant.Gorfine@cern.ch
**/
#ifndef SiLorentzAngleSvc_h
#define SiLorentzAngleSvc_h

#include "InDetCondServices/ISiLorentzAngleSvc.h"

//Gaudi Includes
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "StoreGate/DataHandle.h"
#include "SiPropertiesSvc/SiliconProperties.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"

#include <string>
#include <vector>


//forward declarations
class IdentifierHash;
class ISiliconConditionsSvc;
class IGeoModelSvc;
class StoreGateSvc;

namespace InDetDD {
  class SiDetectorManager;
}  

class AthenaAttributeList;

/**
 * @class SiLorentzAngleSvc
 * Concrete class for service providing Lorentz angle (and the corresponding correction for the shift of the measurement) 
 * for each detector element.
**/

class SiLorentzAngleSvc:  public AthService, virtual public ISiLorentzAngleSvc 
{
public:

  SiLorentzAngleSvc(const std::string& name, ISvcLocator* sl);
  virtual ~SiLorentzAngleSvc();

  virtual StatusCode initialize();          //!< Service init
  virtual StatusCode finalize();            //!< Service finalize
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

  static const InterfaceID& interfaceID();

  /** Get the Lorentz shift correction in the local x (phiDist) direction
      Assumes the center of the detector and is generally cached. */
  virtual double getLorentzShift(const IdentifierHash& elementHash) ;

  /** As above, but provide the local position. 
      More accurate but slower. */
  virtual double getLorentzShift(const IdentifierHash& elementHash, const Amg::Vector2D& locPos);

  /**Get the Lorentz shift correction in the local y (etaDist) direction
     Assumes the center of the detector and is generally cached. */
  virtual double getLorentzShiftEta(const IdentifierHash& elementHash);

  /** As above, but provide the local position. 
      More accurate but slower. */
  virtual double getLorentzShiftEta(const IdentifierHash& elementHash, const Amg::Vector2D& locPos);

  /** Get tan af the Lorentz angle in the local x (phiDist) direction
      Assumes the center of the detector and is generally cached. */
  virtual double getTanLorentzAngle(const IdentifierHash& elementHash);

  /** As above, but provide the local position. 
      More accurate but slower. */
  virtual double getTanLorentzAngle(const IdentifierHash& elementHash, const Amg::Vector2D& locPos);

  /** Get tan af the Lorentz angle in the local y (etaDist) direction
      Assumes the center of the detector and is generally cached. */
  virtual double getTanLorentzAngleEta(const IdentifierHash& elementHash);

  /** As above, but provide the local position. 
      More accurate but slower. */
  virtual double getTanLorentzAngleEta(const IdentifierHash& elementHash, const Amg::Vector2D& locPos);

  /** Get bias voltage */
  virtual double getBiasVoltage(const IdentifierHash & elementHash);

  /** Get temperature */
  virtual double getTemperature(const IdentifierHash & elementHash);

  /** Get depletion voltage */
  virtual double getDepletionVoltage(const IdentifierHash & elementHash);

  /** IOV CallBack */
  virtual StatusCode callBack(IOVSVC_CALLBACK_ARGS);


  /** GeoInit callback */
  StatusCode geoInitCallback(IOVSVC_CALLBACK_ARGS);

  /** IOV Callback for correction folder */
  virtual StatusCode corrFolderCallBack(IOVSVC_CALLBACK_ARGS);

private:
  StatusCode geoInitialize();
  void updateCache(const IdentifierHash& elementHash);
  void updateCache(const IdentifierHash& elementHash, const Amg::Vector2D& locPos, bool useLocPos);
  const Amg::Vector3D& getMagneticField(const IdentifierHash& elementHash, const Amg::Vector2D& locPos, bool useLocPos);
  void invalidateCache();
  bool invalidateMagFieldCache();

private:
 
  // Properties
  std::string              m_detectorName;
  double                   m_temperature;
  double                   m_temperaturePix;
  double                   m_deplVoltage;
  double                   m_biasVoltage;
  double                   m_biasVoltageIBLPl;
  double                   m_biasVoltageIBL3D;
  double                   m_correctionFactor;
  double                   m_nominalField;
  bool                     m_useMagFieldSvc;
  bool                     m_ignoreLocalPos;   // Makes methods using localPosition behave as method without passing localPosition.
  bool                     m_calcEta;          // Control whether eta component is calculated. Default is false as its a negligible effect.
  std::vector<std::string> m_bfieldFolders;
  std::string              m_corrDBFolder;
  bool                     m_pixelDefaults;
  bool                     m_sctDefaults;

  // needed services
  ServiceHandle<ISiliconConditionsSvc>   m_siConditionsSvc;
  ServiceHandle<MagField::IMagFieldSvc>  m_magFieldSvc;
  ServiceHandle<StoreGateSvc>            m_detStore;
  ServiceHandle<IGeoModelSvc>            m_geoModelSvc;

  const DataHandle<AthenaAttributeList> m_dbData;

  bool m_isPixel;  

  InDet::SiliconProperties m_siProperties;
  const InDetDD::SiDetectorManager * m_detManager;

  std::vector<double> m_lorentzShift;
  std::vector<double> m_lorentzShiftEta;
  std::vector<double> m_tanLorentzAngle;
  std::vector<double> m_tanLorentzAngleEta;
  std::vector<double> m_monitorBiasVoltage;
  std::vector<double> m_monitorTemperature;
  std::vector<double> m_monitorDepletionVoltage;

  std::vector<Amg::Vector3D> m_magFieldCache;

  std::vector<bool> m_cacheValid;
  std::vector<bool> m_magFieldCacheValid;

  //std::vector<double> m_lorentzShiftInput;

  std::vector<bool> m_outOfRangeWarning;

};

inline const InterfaceID& SiLorentzAngleSvc::interfaceID(){
  return ISiLorentzAngleSvc::interfaceID();
}


#endif

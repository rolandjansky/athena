/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ChargeTrappingSvc.h
 * @author Peter Vankov (peter.vankov@cern.ch)
 * @author Marco Filipuzzi (marco.filipuzzi@cern.ch)
 **/

#ifndef SCT_ChargeTrappingSvc_h
#define SCT_ChargeTrappingSvc_h

#include "SCT_ConditionsServices/ISCT_ChargeTrappingSvc.h"
//Gaudi Includes
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "SiPropertiesSvc/SiliconProperties.h"
#include "CLHEP/Geometry/Vector3D.h"
#include <string>
#include <vector>

#include "SCT_ElectricFieldTool.h"

//forward declarations
class IdentifierHash;
class ISiliconConditionsSvc;
class IGeoModelSvc;
class StoreGateSvc;

namespace InDetDD {
  class SiDetectorManager;
}

/**
 * @class SCT_ChargeTrappingSvc
 * Concrete class for service providing quantities related to radiation damage effects
 * for each detector element:
 * 1) Mean Free Path assuming exponential loss of charge - dependent on Fluence
 * 2) Electric Field dependent on Bias voltage and Fluence
 **/

class SCT_ChargeTrappingSvc:  public AthService, virtual public  ISCT_ChargeTrappingSvc
{
public:
  SCT_ChargeTrappingSvc(const std::string& name, ISvcLocator* sl);
  virtual ~SCT_ChargeTrappingSvc();
  virtual StatusCode initialize();          //!< Service init
  virtual StatusCode finalize();            //!< Service finalize
  virtual StatusCode queryInterface(const InterfaceID& riid,  void** ppvInterface);
  static const InterfaceID & interfaceID();
 
  // -- Get the Electric Field for the charge position under consideration
  virtual double getElectricField(const IdentifierHash &  elementHash, const double & zpos);
  // -- Get the Mean Free Path for the charge position under consideration
  virtual double getMeanFreePathElectrons(const IdentifierHash &  elementHash, const double & pos);
  virtual double getMeanFreePathHoles(const IdentifierHash &  elementHash, const double & pos);
  virtual double getTrappingElectrons(const IdentifierHash &  elementHash, const double & pos);
  virtual double getTrappingHoles(const IdentifierHash &  elementHash, const double & pos);
  virtual double getTrappingProbability(const IdentifierHash &  elementHash, const double & pos);
  virtual double getTrappingTime(const IdentifierHash &  elementHash, const double & pos);
  virtual double getTimeToElectrode(const IdentifierHash &  elementHash, const double & pos);
  virtual double getTrappingPositionZ(const IdentifierHash &  elementHash, const double & pos);
  virtual double getHoleDriftMobility(const IdentifierHash &  elementHash, const double & pos);
  bool getdoCTrap(const IdentifierHash & elementHash, const  double & pos);
  virtual void getHoleTransport(double & x0, double & y0, double & xfin, double & yfin, double &  Q_m2, double & Q_m1, double & Q_00, double & Q_p1, double & Q_p2 )const;
  virtual void getInitPotentialValue();
  /// IOV CallBack
  virtual StatusCode callBack(IOVSVC_CALLBACK_ARGS);
  
private:
  void updateCache(const IdentifierHash & elementHash, const  double & pos);

  bool valid(const IdentifierHash & elementHash);
  void invalidateCache();

  void initPotentialValue();
  double induced(int istrip, double x, double y)const;
  double GetPotentialValue(int& ix, int& iy);  
  void holeTransport(double & x0, double & y0, double & xfin, double & yfin, double &  Q_m2, double & Q_m1, double & Q_00, double & Q_p1, double & Q_p2 )const;

  
  
private:
  // Properties
  //std::string m_textFile;
  std::string m_detectorName;
  double m_temperature;
  double m_temperatureMin;
  double m_temperatureMax;
  double m_deplVoltage;
  double m_biasVoltage;
  //bool m_ignoreLocalPos; // Makes methods using localPosition  behave as method without passing localPosition.
  ServiceHandle<ISiliconConditionsSvc> m_siConditionsSvc;
  ServiceHandle<StoreGateSvc> m_detStore;
  bool m_conditionsSvcValid;
  bool m_conditionsSvcWarning;
  bool m_isSCT;
  InDet::SiliconProperties m_siProperties;
  const InDetDD::SiDetectorManager * m_detManager;
 
  // -- Radiation damage specific
  bool m_calcHoles;
  double m_fluence;
  double m_betaElectrons;
  double m_betaHoles;
  std::vector<double> m_cacheValid;
  std::vector<double> m_electricField;
  std::vector<double> m_meanFreePathElectrons; 
  std::vector<double> m_meanFreePathHoles;
  std::vector<double> m_trappingElectrons; 
  std::vector<double> m_trappingHoles;
  std::vector<double> m_trappingProbability;
  std::vector<double> m_trappingTime;
  std::vector<double> m_electrodeTime;
  std::vector<double> m_trappingPosition;
  std::vector<double> m_holeDriftMobility;
  mutable bool m_getdoCTrap;

  double m_PotentialValue[81][115];

  // -- Tool Handle for Electric field
  ToolHandle<ISCT_ElectricFieldTool> m_electricFieldTool{this, "SCT_ElectricFieldTool", "SCT_ElectricFieldTool", "SCT electric field tool"};

};
inline const InterfaceID & SCT_ChargeTrappingSvc::interfaceID(){
  static const InterfaceID IID_SCT_ChargeTrappingSvc("SCT_ChargeTrappingSvc", 1, 0);
  return IID_SCT_ChargeTrappingSvc;
 
  //  return ISCT_ChargeTrappingSvc::interfaceID();
}

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_ChargeTrappingSvc.h
 * @note primary author for Pixels: Carolina Deluca (carolina.deluca.silberberg@cern.ch)
 * @author for SCT: Peter Vankov (peter.vankov@cern.ch) 
 *                  Marco Filipuzzi (marco.filipuzzi@cern.ch)
 **/

#ifndef ISCT_ChargeTrappingSvc_h
#define ISCT_ChargeTrappingSvc_h

//Gaudi Includes
#include "GaudiKernel/IInterface.h"
#include "AthenaKernel/IOVSvcDefs.h"

//forward declarations
class IdentifierHash;

/**
 * @class ISCT_ChargeTrappingSvc
 * Interface class for service providing the charge trapping probability
 * for each charge position
 **/
class ISCT_ChargeTrappingSvc: virtual public IInterface
{
 public:
  virtual ~ISCT_ChargeTrappingSvc() {}
  
  static const InterfaceID& interfaceID(); //!< reimplemented from IInterface
  
  virtual double getTrappingProbability(const IdentifierHash& elementHash, const double& pos)=0;
  virtual double getTrappingElectrons(const IdentifierHash& elementHash, const double& pos)=0;
  virtual double getElectricField(const IdentifierHash& elementHash, const double& pos)=0;
  virtual double getTrappingTime(const IdentifierHash& elementHash, const double& pos)=0;
  virtual double getTimeToElectrode(const IdentifierHash& elementHash, const double& pos)=0;
  virtual double getTrappingPositionZ(const IdentifierHash& elementHash, const double& pos)=0;
  virtual double getTrappingHoles(const IdentifierHash& elementHash, const double& pos)=0;  
  virtual double getHoleDriftMobility(const IdentifierHash& elementHash, const double& pos)=0;
  virtual bool getdoCTrap(const IdentifierHash& elementHash,  const double& pos)=0;
  virtual void getHoleTransport(double& x0, double& y0, double& xfin, double& yfin, double& Q_m2, double& Q_m1, double& Q_00, double& Q_p1, double& Q_p2) const =0;
  virtual void getInitPotentialValue()=0;

  /// IOV CallBack
  virtual StatusCode callBack(IOVSVC_CALLBACK_ARGS)=0;
  
};

inline const InterfaceID& ISCT_ChargeTrappingSvc::interfaceID() {
  static const InterfaceID IID_ISCT_ChargeTrappingSvc{"ISCT_ChargeTrappingSvc", 1, 0};
  return IID_ISCT_ChargeTrappingSvc; 
}

#endif // ISCT_ChargeTrappingSvc_h

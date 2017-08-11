/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_RadDamageSummarySvc.h
 * @note primary author for Pixels: Carolina Deluca (carolina.deluca.silberberg@cern.ch)
 * @author for SCT: Peter Vankov (peter.vankov@cern.ch)
 *                  Marco Filipuzzi (marco.filipuzzi@cern.ch)
 **/

#ifndef ISCT_RadDamageSummarySvc_h
#define ISCT_RadDamageSummarySvc_h

//Gaudi Includes
#include "GaudiKernel/IInterface.h"
// #include "AthenaKernel/IOVSvcDefs.h"
// #include "CLHEP/Vector/ThreeVector.h"

//forward declarations
class IdentifierHash;

/**
 * @class ISCT_RadDamageSummarySvc
 * Interface class for service providing Trapping Probability 
 * for each charge in the digitization package.
 **/
class ISCT_RadDamageSummarySvc: virtual public IInterface
{
 public:
  virtual ~ISCT_RadDamageSummarySvc() {}
  
  static const InterfaceID& interfaceID(); //!< reimplemented from IInterface

  virtual double ChargeTrappingProbability(const IdentifierHash& elementHash, const double& zpos) =0;
  virtual double TrappingConstant(const IdentifierHash& elementHash, const double& zpos) =0;
  virtual double ElectricField(const IdentifierHash& elementHash, const double& zpos) =0;
  virtual double TrappingTime(const IdentifierHash& elementHash, const double& zpos) =0;
  virtual double TimeToElectrode(const IdentifierHash& elementHash, const double& zpos) =0;
  virtual double TrappingPositionZ(const IdentifierHash& elementHash, const double& zpos) =0;
  virtual double HoleDriftMobility(const IdentifierHash& elementHash, const double& zpos) =0;
  virtual bool doCTrap(const IdentifierHash& elementHash, const double& zpos) =0; 
  virtual void HoleTransport(double& x0, double& y0, double& xfin, double& yfin, double& Q_m2, double& Q_m1, double& Q_00, double& Q_p1, double& Q_p2) const =0;
  virtual void InitPotentialValue()=0;
};

inline const InterfaceID& ISCT_RadDamageSummarySvc::interfaceID() {
  static const InterfaceID IID_ISCT_RadDamageSummarySvc{"ISCT_RadDamageSummarySvc", 1, 0};
  return IID_ISCT_RadDamageSummarySvc;  
}

#endif // ISCT_RadDamageSummarySvc_h

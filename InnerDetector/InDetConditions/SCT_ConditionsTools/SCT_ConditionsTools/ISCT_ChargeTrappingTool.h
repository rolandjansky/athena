/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_ChargeTrappingTool.h
 * @note primary author for Pixels: Carolina Deluca (carolina.deluca.silberberg@cern.ch)
 * @author for SCT: Peter Vankov (peter.vankov@cern.ch) 
 *                  Marco Filipuzzi (marco.filipuzzi@cern.ch)
 **/

#ifndef ISCT_ChargeTrappingTool_h
#define ISCT_ChargeTrappingTool_h

//Gaudi Includes
#include "GaudiKernel/IAlgTool.h"

//Athena include
#include "SCT_ConditionsData/SCT_ChargeTrappingCondData.h"

//forward declarations
class IdentifierHash;

/**
 * @class ISCT_ChargeTrappingTool
 * Interface class for tool providing the charge trapping probability
 * for each charge position
 **/
class ISCT_ChargeTrappingTool: virtual public IAlgTool
{
 public:
  virtual ~ISCT_ChargeTrappingTool() = default;

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ISCT_ChargeTrappingTool, 1, 0);

  virtual SCT_ChargeTrappingCondData getCondData(const IdentifierHash& elementHash, const double& pos) const =0;
  virtual void getHoleTransport(double& x0, double& y0, double& xfin, double& yfin, double& Q_m2, double& Q_m1, double& Q_00, double& Q_p1, double& Q_p2) const =0;
  virtual void getInitPotentialValue()=0;

};

#endif // ISCT_ChargeTrappingTool_h

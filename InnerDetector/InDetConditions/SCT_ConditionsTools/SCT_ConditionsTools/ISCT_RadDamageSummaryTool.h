/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_RadDamageSummaryTool.h
 * @note primary author for Pixels: Carolina Deluca (carolina.deluca.silberberg@cern.ch)
 * @author for SCT: Peter Vankov (peter.vankov@cern.ch)
 *                  Marco Filipuzzi (marco.filipuzzi@cern.ch)
 **/

#ifndef ISCT_RadDamageSummaryTool_h
#define ISCT_RadDamageSummaryTool_h

//Athena include
#include "SCT_ConditionsData/SCT_ChargeTrappingCondData.h"

//Gaudi Includes
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IAlgTool.h"

//forward declarations
class IdentifierHash;

/**
 * @class ISCT_RadDamageSummaryTool
 * Interface class for tool providing Trapping Probability 
 * for each charge in the digitization package.
 **/
class ISCT_RadDamageSummaryTool: virtual public IAlgTool
{
 public:
  virtual ~ISCT_RadDamageSummaryTool() = default;

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ISCT_RadDamageSummaryTool, 1, 0);

  virtual void holeTransport(double& x0, double& y0, double& xfin, double& yfin, double& Q_m2, double& Q_m1, double& Q_00, double& Q_p1, double& Q_p2, const EventContext& ctx) const =0;
  virtual void holeTransport(double& x0, double& y0, double& xfin, double& yfin, double& Q_m2, double& Q_m1, double& Q_00, double& Q_p1, double& Q_p2) const =0;
  virtual SCT_ChargeTrappingCondData getCondData(const IdentifierHash& elementHash, const double& zpos, const EventContext& ctx) const =0;
  virtual SCT_ChargeTrappingCondData getCondData(const IdentifierHash& elementHash, const double& zpos) const =0;
};

#endif // ISCT_RadDamageSummaryTool_h

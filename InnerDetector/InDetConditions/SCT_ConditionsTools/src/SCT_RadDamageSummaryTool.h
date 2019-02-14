/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RADDAMAGESUMMARYTOOL_H
#define SCT_RADDAMAGESUMMARYTOOL_H

#include "SCT_ConditionsTools/ISCT_RadDamageSummaryTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_ConditionsTools/ISCT_ChargeTrappingTool.h"

#include <string>

class IdentifierHash;

// ----------------------------------------------------------------------------- //
// -- Summary tool for radiation damage related tools
//    Provides access to SCT_ChargeTrappingTool                                        
//                                                                               
//    primary author for Pixels: Carolina Deluca <carolina.deluca.silberberg@cern.ch> 
//    author for SCT: Peter Vankov <peter.vankov@cern.ch>,
//                    Marco Filipuzzi <marco.filipuzzi@cern.ch>                 
//    date: 2012-05-18                                                              
// ----------------------------------------------------------------------------- //

class SCT_RadDamageSummaryTool: public extends<AthAlgTool, ISCT_RadDamageSummaryTool> {

 public:
  SCT_RadDamageSummaryTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SCT_RadDamageSummaryTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
 
  virtual void holeTransport(double& x0, double& y0, double& xfin, double& yfin, double& Q_m2, double& Q_m1, double& Q_00, double& Q_p1, double& Q_p2, const EventContext& ctx) const override;
  virtual void holeTransport(double& x0, double& y0, double& xfin, double& yfin, double& Q_m2, double& Q_m1, double& Q_00, double& Q_p1, double& Q_p2) const override;
  virtual SCT_ChargeTrappingCondData getCondData(const IdentifierHash& elementHash, const double& zpos, const EventContext& ctx) const override;
  virtual SCT_ChargeTrappingCondData getCondData(const IdentifierHash& elementHash, const double& zpos) const override;

 private:
  ToolHandle<ISCT_ChargeTrappingTool> m_chargeTrappingTool{this, "ChargeTrappingTool", "SCT_ChargeTrappingTool", "Tool to retrieve charge trapping properties"};
};

#endif // SCT_RADDAMAGESUMMARYTOOL_H

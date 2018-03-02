/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RADDAMAGESUMMARYTOOL_H
#define SCT_RADDAMAGESUMMARYTOOL_H

#include "SCT_ConditionsTools/ISCT_RadDamageSummaryTool.h"

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"

#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsTools/ISCT_ChargeTrappingTool.h"
#include "CLHEP/Vector/ThreeVector.h"

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
 
  // virtual double RamoPotential(const IdentifierHash & elementHash, const Hep3Vector & pos);
  virtual double ChargeTrappingProbability(const IdentifierHash & elementHash, const double & zpos) override;
  virtual double TrappingConstant(const IdentifierHash & elementHash, const double & zpos) override;
  virtual double ElectricField(const IdentifierHash & elementHash, const double & zpos) override;
  virtual double TrappingTime(const IdentifierHash & elementHash, const double & zpos) override;
  virtual double TimeToElectrode(const IdentifierHash & elementHash, const double & zpos) override;
  virtual double TrappingPositionZ(const IdentifierHash & elementHash, const double & zpos) override;
  virtual double HoleDriftMobility(const IdentifierHash & elementHash, const double & zpos) override; 
  virtual bool doCTrap(const IdentifierHash & elementHash,  const double & zpos) override;
  virtual void HoleTransport(double & x0, double & y0, double & xfin, double & yfin, double & Q_m2, double & Q_m1, double & Q_00, double & Q_p1, double & Q_p2 )const override;
  virtual void InitPotentialValue() override;

 private:
  ToolHandle<ISCT_ChargeTrappingTool> m_chargeTrappingTool{this, "ChargeTrappingTool", "SCT_ChargeTrappingTool", "Tool to retrieve charge trapping properties"};
  
  bool m_useTrapping;
  mutable bool m_doCTrap;

};

#endif // SCT_RADDAMAGESUMMARYTOOL_H

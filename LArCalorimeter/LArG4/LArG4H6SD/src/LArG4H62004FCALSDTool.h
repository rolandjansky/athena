/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4H62004FCALSDTOOL_H
#define LARG4H62004FCALSDTOOL_H

#include "LArG4Code/LArG4SDTool.h"
#include <string>
#include <vector>

#include "StoreGate/WriteHandle.h"
#include "LArSimEvent/LArHitContainer.h"
#include "LArG4Code/ILArCalculatorSvc.h"

class LArG4SimpleSD;

/// DEPRECATED AND WILL BE REMOVED.
/// Please see LArG4::H62004FCALSDTool instead.
///
class LArG4H62004FCALSDTool : public LArG4SDTool
{
 public:
  // Constructor
  LArG4H62004FCALSDTool(const std::string& type, const std::string& name, const IInterface *parent);

  // Destructor
  virtual ~LArG4H62004FCALSDTool() {}

  // Method in which all the SDs are created and assigned to the relevant volumes
  StatusCode initializeSD() override final;

  // Calls down to all the SDs to get them to pack their hits into a central collection
  StatusCode Gather() override final;

 private:
  StatusCode initializeCalculators() override final;

  // The actual hit container - here because the base class is for both calib and standard SD tools
  SG::WriteHandle<LArHitContainer> m_HitColl;

  ServiceHandle<ILArCalculatorSvc> m_fcal1calc;
  ServiceHandle<ILArCalculatorSvc> m_fcal2calc;
  ServiceHandle<ILArCalculatorSvc> m_fcalcoldcalc;
  // Sensitive detectors and their corresponding volumes
  LArG4SimpleSD* m_fcal1SD;
  LArG4SimpleSD* m_fcal2SD;
  LArG4SimpleSD* m_fcalColdSD;
  std::vector<std::string> m_fcal1Volumes;
  std::vector<std::string> m_fcal2Volumes;
  std::vector<std::string> m_fcalColdVolumes;
};

#endif
